/* ONU image download via TK private OAM
   Broadcom extensions enables ONUs to download new firmware upgardes from OLT using a simple file transfer protocol.
   This protocl is a simplified form of TFTP. It has been modified to operate over IEEE 802.1ah OAM channel instead of IP.*/
#ifdef HAVE_TK_OAM

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include "ca_version.h"
#include "osal_cmn.h"
#include "osal_timer.h"
#include "app_misc.h"

#include "oam_cmn.h"
#include "oam_core.h"
#include "oam_main.h"
#include "oam_adapt.h"

#ifdef HAVE_KT_OAM
#include "oam_tk_pdu.h"
#include "oam_tk.h"
#include "oam_tk_upg.h"
#include "oam_tk_adapt.h"

typedef enum{
    OAM_TK_UPG_DOWNLOAD_INIT = 0,
    OAM_TK_UPG_DOWNLOAD_STARTED,
    OAM_TK_UPG_DOWNLOAD_COMPLETED,
    OAM_TK_UPG_COMMIT,
    OAM_TK_UPG_DONE
}oam_tk_upg_state_t;

typedef enum{
    OAM_TK_UPG_SUCCESS = 0,
    OAM_TK_UPG_ERROR,
}oam_tk_upg_status_t;

typedef struct{
    oam_uint32 thread_id;
    oam_uint32 stop_timer;
    oam_uint32 timeout_timer;

    oam_int32 file;
    
    oam_tk_upg_state_t upg_state;
    oam_tk_upg_status_t upg_status;

    oam_uint32 expect_block_number;
    oam_uint32 rx_block_number;
    oam_uint32 rx_file_read;
    oam_uint32 rx_file_write;
    oam_uint32 rx_file_data;
    oam_uint32 rx_file_ack;
    oam_uint32 tx_file_ack;

}oam_tk_upg_mgmt_t;

static oam_tk_upg_mgmt_t g_oam_tk_upg_mgmt;
static oam_uint8 oam_tk_upg_rsp[64];

void oam_tk_upg_download_reinit();

/*
1. system("kt.upgrade") 
2. get active part: img_info.active_flag
suppose img0 is active.
3. verify img1: img_info.valid_flag

4. commit: commit + execute
5. reset
*/

void oam_tk_upg_dump()
{  
    oam_printf("\r\nupg_state  %d\r\n", g_oam_tk_upg_mgmt.upg_state);
    oam_printf("upg_status  %d\r\n", g_oam_tk_upg_mgmt.upg_status);
    oam_printf("expect_block    %d\r\n", g_oam_tk_upg_mgmt.expect_block_number);
    oam_printf("rx_block_number %d\r\n", g_oam_tk_upg_mgmt.rx_block_number);
    
    oam_printf("stop_timer      %d\r\n", g_oam_tk_upg_mgmt.stop_timer);
    oam_printf("thread_id       %d\r\n", g_oam_tk_upg_mgmt.thread_id);
    oam_printf("file            %d\r\n", g_oam_tk_upg_mgmt.file);
    oam_printf("rx_file_read    %d\r\n", g_oam_tk_upg_mgmt.rx_file_read);
    oam_printf("rx_file_write   %d\r\n", g_oam_tk_upg_mgmt.rx_file_write);
    oam_printf("rx_file_data    %d\r\n", g_oam_tk_upg_mgmt.rx_file_data);
    oam_printf("rx_file_ack     %d\r\n", g_oam_tk_upg_mgmt.rx_file_ack);
    oam_printf("tx_file_ack     %d\r\n", g_oam_tk_upg_mgmt.tx_file_ack);
    
}

oam_uint32 oam_tk_upg_ack_build(
        oam_uint8 *pOut,
        oam_uint16 block_number,
        oam_uint8 code)
{
    oam_uint32 len=0;
    oam_tk_upg_file_ack_t *p_rsp = (oam_tk_upg_file_ack_t *)pOut;

    g_oam_tk_upg_mgmt.tx_file_ack++;
    if(block_number == 0 || code != OAM_TK_UPG_ACK_OK)
    {
        OAM_ORG_LOG_MAJ("send file ack with code %d, expected block number %d\r\n", code, block_number);
    }else
    {
        OAM_ORG_LOG("send file ack with code %d, expected block number %d\r\n", code, block_number);
    }
    p_rsp->block_number = htons(block_number);
    p_rsp->resp_code = code;

    len += sizeof(oam_tk_upg_file_ack_t);
    return len;
}

void oam_tk_upg_status_resp(oam_uint8 code)
{
    oam_uint8 *pOut;
    oam_uint32 len;
    oam_if_t * intf = oam_intf_find_valid();

    if(intf == NULL){
        return;
    }

    pOut = &(oam_tk_upg_rsp[0]);
    len = sizeof(tk_oam_pdu_t) - 1;
    
    len += oam_tk_upg_ack_build(pOut+len, 0, code); 
    //OAM_ORG_LOG_MAJ("send file ack with code %d\r\n", code);
    
    oam_tk_send(OAM_PON_PORT_ID, intf->llid, pOut, len);
    //oam_buf_print(pOut, len);
    
}

oam_uint8 oam_tk_upg_status_get()
{
    int file_fd = 0;
    char upg_status = 0;

    file_fd = open("/tmp/upgrade_status",O_RDONLY);
    read(file_fd, &upg_status, 1);
    close(file_fd);
    
    return upg_status;
}

void oam_tk_upg_status_check()
{
    oam_uint8 upg_status = 0;

    upg_status = oam_tk_upg_status_get();

    if(upg_status == 0x30){
        ca_timer_add(1000, oam_tk_upg_status_check, NULL); 
    }
    return;
}

oam_status oam_tk_upg_done_evt_handle()
{
    int file_fd = 0;
    oam_uint8 upg_status;
    oam_uint8 resp_code = OAM_TK_UPG_ACK_OK;

    upg_status = oam_tk_upg_status_get();

    OAM_ORG_LOG_MAJ( "upgrade_status 0x%x\r\n", upg_status);
    
    switch(upg_status){
    case 0x32:
        resp_code = OAM_TK_UPG_ACK_ILLEGAL_OP;  
        OAM_ERROR( "image CRC error\r\n");
        break;
    case 0x34:
        resp_code = OAM_TK_UPG_ACK_FULL;
        OAM_ERROR( "image write fail\r\n");
        break;
    case 0x39:
        resp_code = OAM_TK_UPG_ACK_OK;
        break;
    default:
        resp_code = OAM_TK_UPG_ACK_UNDEF; 
        break;
    }

    if(resp_code == OAM_TK_UPG_ACK_OK){
        OAM_ORG_LOG_MAJ( "start image commit. \r\n");
        oam_img_commit_adapt();
    }
    g_oam_tk_upg_mgmt.upg_state = OAM_TK_UPG_DONE;    
    g_oam_tk_upg_mgmt.upg_status = resp_code;
    
    oam_tk_upg_status_resp(resp_code);
    
    OAM_ORG_LOG_MAJ("image upgrade %s (%d)!\r\n", (resp_code == 0)?"successfully":"failed", resp_code);

    oam_tk_upg_download_reinit();
    //kt_oam_action_reset_adapt(1);

    return OAM_E_OK;
}

void oam_tk_upg_entry()
{
    oam_img_upgrade_adapt();
    
    oam_tk_upg_done_evt_send();

    return;
}

void oam_tk_upg_download_done(void* data)
{
    g_oam_tk_upg_mgmt.upg_state = OAM_TK_UPG_DOWNLOAD_INIT;
    if(g_oam_tk_upg_mgmt.file != -1){
        close(g_oam_tk_upg_mgmt.file);
        g_oam_tk_upg_mgmt.file = -1;
    }
}

void oam_tk_upg_download_abort()
{
    if(!oam_tk_upg_upgrading()){
        return;
    }
    OAM_ORG_LOG_MAJ("software-download aborted.\r\n");
    g_oam_tk_upg_mgmt.upg_state = OAM_TK_UPG_DOWNLOAD_INIT;
    if(g_oam_tk_upg_mgmt.stop_timer != 0){
        ca_timer_del(g_oam_tk_upg_mgmt.stop_timer);
        g_oam_tk_upg_mgmt.stop_timer  = 0;
    }

    if(g_oam_tk_upg_mgmt.file != -1){
        close(g_oam_tk_upg_mgmt.file);
        g_oam_tk_upg_mgmt.file = -1;
    }
}

void oam_tk_upg_download_reinit()
{    
    OAM_ORG_LOG_MAJ("reinit, ready for new upgrade session\r\n");
    g_oam_tk_upg_mgmt.upg_state = OAM_TK_UPG_DOWNLOAD_INIT;
    if(g_oam_tk_upg_mgmt.stop_timer != 0){
        ca_timer_del(g_oam_tk_upg_mgmt.stop_timer);
        g_oam_tk_upg_mgmt.stop_timer  = 0;
    }

    if(g_oam_tk_upg_mgmt.file != -1){
        close(g_oam_tk_upg_mgmt.file);
        g_oam_tk_upg_mgmt.file = -1;
    }
    //oam_img_remove_adapt(); /* don't remove, remain it for debug */
}

oam_int32 oam_tk_upg_download_open()
{
    /* creat() is equivalent to open() with flags equal to O_CREAT|O_WRONLY|O_TRUNC. 
       O_TRUNC: If the file already exists and is a regular file and the open mode  allows  writing  (i.e.,  is  
       O_RDWR or O_WRONLY)  it  will  be  truncated to length 0. */           
    g_oam_tk_upg_mgmt.file = open(OAM_UPG_IMG_NAME,
            O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR|S_IXUSR);

    if(g_oam_tk_upg_mgmt.file < 0){
        OAM_ERROR("\r\nopen file %s failed \r\n",OAM_UPG_IMG_NAME);
        return OAM_E_ERROR;
    }

    /* upgrade done within 5min*/
    g_oam_tk_upg_mgmt.stop_timer = ca_timer_add(300000, oam_tk_upg_download_reinit, NULL); 

    return OAM_E_OK;
}

oam_int32 oam_tk_upg_download_write(
        oam_uint8 * buf,
        oam_uint32 len)
{
    write(g_oam_tk_upg_mgmt.file,buf,len);
    return OAM_E_OK;
}

oam_int32 oam_tk_upg_download_close()
{
    //OAM_ORG_LOG_MAJ("software download closed.\r\n");
    close(g_oam_tk_upg_mgmt.file);
    g_oam_tk_upg_mgmt.file = -1;

    if(g_oam_tk_upg_mgmt.stop_timer != 0){
        ca_timer_del(g_oam_tk_upg_mgmt.stop_timer);
        g_oam_tk_upg_mgmt.stop_timer  = 0;
    }
    
    return OAM_E_OK;
}

oam_uint32 oam_tk_upg_rx_file_read_req(oam_uint8 *pIn, oam_uint8 *pOut)
{
    g_oam_tk_upg_mgmt.rx_file_write++;
    OAM_ORG_LOG_MAJ("received file read request OAM. state %d\r\n", g_oam_tk_upg_mgmt.upg_state);

    OAM_ERROR("software-download don't support FILE_READ_request\r\n");
    return oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_ILLEGAL_OP);
}

oam_uint32 oam_tk_upg_rx_file_write_req(oam_uint8 *pIn, oam_uint8 *pOut)
{
    oam_int32 ret = 0;
    oam_tk_upg_file_req_t *file_req;

    g_oam_tk_upg_mgmt.rx_file_write++;
    oam_printf("\r\n");
    OAM_ORG_LOG_MAJ("received file write request OAM. state %d\r\n", g_oam_tk_upg_mgmt.upg_state);

    file_req = (oam_tk_upg_file_req_t *)(pIn);

    
    /* handle file write request at any state except commit state, 
       and treat it as a new upgrade session start */
    if(g_oam_tk_upg_mgmt.upg_state == OAM_TK_UPG_COMMIT)
    {
        OAM_ERROR("image upgrade and commit now.\r\n");
        return oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_ILLEGAL_OP);  
        
    }else if(g_oam_tk_upg_mgmt.upg_state != OAM_TK_UPG_DOWNLOAD_INIT)
    {
        oam_tk_upg_download_reinit();
    }

    if(file_req->file_type != OAM_TK_UPG_FILE_TYPE_APP)
    {
        OAM_ERROR("software-download only support APPLICATION file type. file_type %d\r\n", file_req->file_type);
        return oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_ILLEGAL_OP);
        
    }
    
    OAM_ORG_LOG_MAJ("software-download started\r\n");
    ret = oam_tk_upg_download_open();
    if(ret != OAM_E_OK)
    {
        OAM_ERROR("upgrade image file open failed.\r\n");
        ret = oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_NOT_ACCESS);
        oam_tk_upg_download_reinit();
        return ret;
    }

    g_oam_tk_upg_mgmt.upg_state = OAM_TK_UPG_DOWNLOAD_STARTED;
    g_oam_tk_upg_mgmt.expect_block_number = 0;

    return oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_OK);

}


oam_uint32 oam_tk_upg_rx_file_transfer(oam_uint8 *pIn, oam_uint8 *pOut)
{
    oam_uint32 ret = 0;
    oam_tk_upg_file_data_t *file_data;
    oam_uint16 block_number;
    oam_uint16 block_size;
    
    g_oam_tk_upg_mgmt.rx_file_data++;

    file_data = (oam_tk_upg_file_data_t *)(pIn);
            
    block_number = ntohs(file_data->block_number);
    g_oam_tk_upg_mgmt.rx_block_number = block_number;

    if(g_oam_tk_upg_mgmt.upg_state != OAM_TK_UPG_DOWNLOAD_STARTED){ 
        OAM_ORG_LOG_MAJ("received file transfer OAM, block_number %d\r\n", block_number);
        OAM_ERROR("software-download state is not under start state. state %d\r\n", g_oam_tk_upg_mgmt.upg_state);
        return oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_ILLEGAL_OP);
    }

    /* the block number is not equal to the expected block number */
    if(block_number != g_oam_tk_upg_mgmt.expect_block_number){ 
        OAM_ERROR("received wrong block number %d. expect block number is %d\r\n", 
            block_number, g_oam_tk_upg_mgmt.expect_block_number);
        return oam_tk_upg_ack_build(pOut, g_oam_tk_upg_mgmt.expect_block_number, OAM_TK_UPG_ACK_BAD_BLOCK);
    }

    block_size = ntohs(file_data->block_size);
    if((block_size > OAM_TK_UPG_BLOCK_SIZE_MAX)){
        
    }

    if((block_number == 0)&&(ca_img_valid_check(0, file_data->data, block_size))){ 
        OAM_ERROR("received wrong file!\r\n");
        return oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_ILLEGAL_OP);
    }

    /*write onu image*/
    if(g_oam_tk_upg_mgmt.expect_block_number%16 == 0){
        if(g_oam_tk_upg_mgmt.expect_block_number%(16*32) == 0)
            oam_printf("\r\n");
        oam_printf(".");
    }

    //OAM_ORG_LOG( "\r\noam software download...\r\n");
    oam_tk_upg_download_write(file_data->data, block_size);

    g_oam_tk_upg_mgmt.upg_state = OAM_TK_UPG_DOWNLOAD_STARTED;
    g_oam_tk_upg_mgmt.expect_block_number = block_number+1;       
    return oam_tk_upg_ack_build(pOut, g_oam_tk_upg_mgmt.expect_block_number, OAM_TK_UPG_ACK_OK);              

}

oam_uint32 oam_tk_upg_rx_file_ack_ok(oam_uint8 *pIn, oam_uint8 *pOut)
{
    oam_uint32 ret = 0;
    OAM_ORG_LOG_MAJ( "start image upgrade\r\n");

    /* wait upgrade status, and response ACK in oam_tk_img_upgrade_entry */
    ret = OS_THREAD_CREATE(&(g_oam_tk_upg_mgmt.thread_id), "tk_oam_upg", oam_tk_upg_entry, NULL, 32*1024, 3, 0);
    if(ret != 0){
        OAM_ORG_LOG_ERROR("tk oam upgrade thread create failed\r\n");
        ret = oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_BAD_BLOCK);
        oam_tk_upg_download_reinit();      
        return ret;
    }
    g_oam_tk_upg_mgmt.upg_state = OAM_TK_UPG_COMMIT;

    return 0;
}

oam_uint32 oam_tk_upg_rx_file_ack_nok(oam_uint8 *pIn, oam_uint8 *pOut)
{
    oam_uint32 ret = 0;
    
    ret = oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_BAD_BLOCK);
    oam_tk_upg_download_reinit();      
    return ret;
}

oam_uint32 oam_tk_upg_rx_file_ack(oam_uint8 *pIn, oam_uint8 *pOut)
{
    oam_uint32 ret = 0;
    oam_tk_upg_file_ack_t *file_ack;
    oam_uint16 block_number;
    
    g_oam_tk_upg_mgmt.rx_file_ack++;
    
    file_ack = (oam_tk_upg_file_ack_t *)(pIn);

    oam_printf("\r\n");
    OAM_ORG_LOG_MAJ("received file ack with code %d. Totally %d blocks received\r\n", 
        file_ack->resp_code, g_oam_tk_upg_mgmt.expect_block_number);

    if(g_oam_tk_upg_mgmt.upg_state != OAM_TK_UPG_DOWNLOAD_STARTED){
        OAM_ERROR("software-download state is not under start state. state %d\r\n", g_oam_tk_upg_mgmt.upg_state);
        return oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_ILLEGAL_OP);
    }
               
    block_number = ntohs(file_ack->block_number);      
    if(block_number != 0){
        OAM_ERROR("received wrong block number %d. expect block number is 0\r\n", block_number);
        return oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_BAD_BLOCK);
    }

    /* when using wrong file to upgrade image, OLT will send ACK with ack_code 0, block_number 0 */
    if(file_ack->resp_code == OAM_TK_UPG_ACK_OK && g_oam_tk_upg_mgmt.expect_block_number == 0)
    {
        OAM_ERROR("software-download didn't receive any right data blocks.\r\n");
        ret = oam_tk_upg_ack_build(pOut, 0, OAM_TK_UPG_ACK_ILLEGAL_OP);
        oam_tk_upg_download_reinit();      
        return ret;
    }
            
    OAM_ORG_LOG_MAJ( "software-download completed.\r\n");
    oam_tk_upg_download_close();
    g_oam_tk_upg_mgmt.upg_state = OAM_TK_UPG_DOWNLOAD_COMPLETED;
    g_oam_tk_upg_mgmt.expect_block_number = 0;

    if(file_ack->resp_code == OAM_TK_UPG_ACK_OK){
        return oam_tk_upg_rx_file_ack_ok(pIn, pOut);
    }else{
        return oam_tk_upg_rx_file_ack_nok(pIn, pOut);
    } 
    
}

void oam_tk_upg_build(
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 length)
{
    oam_status ret=  OAM_E_OK;
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    tk_oam_pdu_t *pRecvPkt = (tk_oam_pdu_t *)frame;
    tk_oam_pdu_t *pOutPkt = (tk_oam_pdu_t *)pdu;
    oam_uint32 outPktLen = 0, resp_len = 0;
    oam_uint8 *pIn = pRecvPkt->data;
    oam_uint8 *pOut = pOutPkt->data;

    OAM_ASSERT(frame != NULL && intf != NULL);

    OAM_ORG_LOG( "\r\nreceived software download OAM with opcode %d\r\n", pRecvPkt->opcode);
    memset(&pdu[0],0, OAM_MAX_OAMPDU_SIZE);
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
        
    outPktLen =  sizeof(tk_oam_pdu_t) - 1;
    
    switch(pRecvPkt->opcode)
    {
        case TK_OPCODE_FILE_READ_REQ:
            resp_len = oam_tk_upg_rx_file_read_req(pIn, pOut);
            break;
            
        case TK_OPCODE_FILE_WRITE_REQ:
            resp_len = oam_tk_upg_rx_file_write_req(pIn, pOut);
            break;
            
        case TK_OPCODE_FILE_DATA: /* start */
            resp_len =  oam_tk_upg_rx_file_transfer(pIn, pOut);      
            break;
            
        case TK_OPCODE_FILE_ACK:
            resp_len =  oam_tk_upg_rx_file_ack(pIn, pOut);       
            break;
        default:
            break;
    }

    outPktLen += resp_len;
    if(resp_len != 0)
    {
        pOutPkt->opcode = TK_OPCODE_FILE_ACK;
        oam_tk_send(OAM_PON_PORT_ID, llid, pOutPkt, outPktLen);
    }else
    {
        memset(oam_tk_upg_rsp, 0, sizeof(oam_tk_upg_rsp));
        pOutPkt->opcode = TK_OPCODE_FILE_ACK;
        memcpy((oam_uint8*)oam_tk_upg_rsp, pOutPkt, outPktLen);
    }
    intf->org_specific_tx++;

    return;
}

oam_boolean oam_tk_upg_upgrading()
{
    return g_oam_tk_upg_mgmt.upg_state != OAM_TK_UPG_DOWNLOAD_INIT;
}

void oam_tk_upg_init()
{
    memset(&g_oam_tk_upg_mgmt, 0, sizeof(oam_tk_upg_mgmt_t));
    g_oam_tk_upg_mgmt.upg_state = OAM_TK_UPG_DOWNLOAD_INIT;
    g_oam_tk_upg_mgmt.file = -1;

}
#endif
#endif