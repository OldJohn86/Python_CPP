#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_DPOE_OAM

#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_dpoe.h"
#include "oam_dpoe_pdu.h"
#include "oam_dpoe_client.h"
#include "oam_dpoe_adapt.h"
#include "oam_mux_par.h"
#include "oam_client.h"
#include "oam_timer.h"
#include "osal_timer.h"
#include "oam_dpoe_upg.h"
#include "os_core.h"
#include "ca_version.h"

dpoe_oam_upg_mgr_ctx_t upg_mgr_ctx_glb;

dpoe_oam_upg_mgr_ctx_t *dpoe_oam_upg_mgr_ctx_glb_get(void)
{
    return &upg_mgr_ctx_glb;
}

void dpoe_oam_upg_trans_llid_set(oam_llid_t llid)
{
    dpoe_oam_upg_mgr_ctx_t *upg_mgr = dpoe_oam_upg_mgr_ctx_glb_get();

    upg_mgr->dpoe_oam_upg_trans_ctx.llid = llid;
}

oam_llid_t dpoe_oam_upg_trans_llid_get(void)
{
    dpoe_oam_upg_mgr_ctx_t *upg_mgr = dpoe_oam_upg_mgr_ctx_glb_get();

    return upg_mgr->dpoe_oam_upg_trans_ctx.llid;
}

oam_int32 dpoe_oam_upg_img_ctrl_retry_count_set(oam_uint8 retry_limit)
{
    dpoe_oam_upg_mgr_ctx_t  *upg_mgr      = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_img_ctrl_ctx_t *img_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);

    img_ctrl_ctx->retry_count = retry_limit;

    return 0;
}

oam_int32 dpoe_oam_upg_file_trans_opcode_dipatch(
    oam_uint8  ftOpcode, 
    oam_uint8  *data,
    oam_uint8  *outData,
    oam_uint32 *outLen)
{
    dpoe_oam_upg_mgr_ctx_t        *upg_mgr          = dpoe_oam_upg_mgr_ctx_glb_get();
    oam_pdu_dpoe_file_trans_ack_t *peoam_pdu_ft_ack = NULL;            
    
    OAM_ASSERT_RET(data    != NULL, 0);
    OAM_ASSERT_RET(outData != NULL, 0);
    OAM_ASSERT_RET(outLen  != NULL, 0);
    
    switch(ftOpcode)
    {
        case DPOE_OAM_UPG_FILE_TRANS_OPECODE_WR:
        {
            upg_mgr->img_upg_fsm_handler[upg_mgr->dpoe_oam_upg_fsm_ctx.upg_state][DPOE_OAM_UPG_EOAMI_WR_EVENT](data, outData, outLen);
            break;
        }

        case DPOE_OAM_UPG_FILE_TRANS_OPECODE_FTD:
        {
            upg_mgr->img_upg_fsm_handler[upg_mgr->dpoe_oam_upg_fsm_ctx.upg_state][DPOE_OAM_UPG_EOAMI_FTD_EVENT](data, outData, outLen);
            break;
        }

        case DPOE_OAM_UPG_FILE_TRANS_OPECODE_FTA:
        {            			
            peoam_pdu_ft_ack = (oam_pdu_dpoe_file_trans_ack_t *)data;

            if(DPOE_OAM_FILE_TRANS_RESP_TIMEOUT == peoam_pdu_ft_ack->respCode)
            {            
                upg_mgr->img_upg_fsm_handler[upg_mgr->dpoe_oam_upg_fsm_ctx.upg_state][DPOE_OAM_UPG_EOAMI_FTA_TIMEOUT_EVENT](data, outData, outLen);
                break;
            }

            if((DPOE_OAM_FILE_TRANS_RESP_OK != peoam_pdu_ft_ack->respCode) && 
                (DPOE_OAM_FILE_TRANS_RESP_TIMEOUT != peoam_pdu_ft_ack->respCode))
            {
                upg_mgr->img_upg_fsm_handler[upg_mgr->dpoe_oam_upg_fsm_ctx.upg_state][DPOE_OAM_UPG_EOAMI_FTA_ERROR_EVENT](data, outData, outLen);
                break;
            }

            if((DPOE_OAM_FILE_TRANS_RESP_OK == peoam_pdu_ft_ack->respCode) &&
                (0x0000 == (ntohs(peoam_pdu_ft_ack->blockNum))))
            {
                upg_mgr->img_upg_fsm_handler[upg_mgr->dpoe_oam_upg_fsm_ctx.upg_state][DPOE_OAM_UPG_EOAMI_FTA_END_EVENT](data, outData, outLen);
                break;
            }
            
            break;
        }
        default:
            break;
    }
}

oam_uint32 dpoe_oam_build_file_trans_pdu_header(
    oam_uint8 *oui,
    oam_llid_t  llid,
    oam_uint8 *buf,
    oam_uint8  opcode,
    oam_uint8  ftOpcode)
{
    oam_pdu_dpoe_file_trans_t *pdu = (oam_pdu_dpoe_file_trans_t *)buf;
    oam_if_t *intf                 = oam_intf_find(llid);

    OAM_ASSERT_RET(buf != NULL, 0);

    if (intf == NULL) {
        OAM_ORG_LOG("Unknown llid 0x%x\n", llid);
        return 0;
    }

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)buf, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pdu->oui, oui, OAM_OUI_LEN);
    pdu->opcode          = opcode;
    pdu->fileTransOpcode = ftOpcode;

    return sizeof(oam_pdu_dpoe_file_trans_t) - 1;
}

oam_int32 dpoe_oam_handle_file_trans(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_uint8                 pdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dpoe_file_trans_t *pOutPkt   = (oam_pdu_dpoe_file_trans_t *)pdu;
    oam_pdu_dpoe_file_trans_t *pRecvPkt  = NULL;
    oam_uint32                outPktLen  = 0;
    oam_uint32                len        = 0;

    memset(pdu, 0x0, OAM_MAX_OAMPDU_SIZE);
    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length >=
                   sizeof(oam_pdu_dpoe_file_trans_t) - 1, 0);
    
    pRecvPkt    = (oam_pdu_dpoe_file_trans_t *)frame;
    outPktLen   = dpoe_oam_build_file_trans_pdu_header(pRecvPkt->oui, llid, pdu,
                                                    OAM_PDU_DPOE_FILE_TRANS,
                                                    DPOE_OAM_UPG_FILE_TRANS_OPECODE_FTA);
    
    dpoe_oam_upg_trans_llid_set(llid);
    
    dpoe_oam_upg_file_trans_opcode_dipatch(pRecvPkt->fileTransOpcode, frame, (oam_uint8 *)pOutPkt, &len);

    if(!len)
    {
        OAM_LOG_DEBUG("resp len is NULL\r\n");
        return 0;
    }
    
    outPktLen += len;
    dpoe_oam_send(llid, pdu, outPktLen);
            
    return 0;
}


oam_int32 dpoe_oam_upg_fsm_ctx_init(void)
{
    dpoe_oam_upg_mgr_ctx_t *upg_mgr     = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_upg_fsm_ctx_t *upg_fsm_ctx = &(upg_mgr->dpoe_oam_upg_fsm_ctx);

    upg_fsm_ctx->upg_state     = DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE;
    upg_fsm_ctx->pre_upg_state = DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE;
    upg_fsm_ctx->event         = DPOE_OAM_UPG_MAX_EVENT;

    return 0;
}

void dpoe_oam_img_ctrl_ctx_init(void)
{
    dpoe_oam_upg_mgr_ctx_t  *upg_mgr      = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_img_ctrl_ctx_t *img_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);

    memset(img_ctrl_ctx, 0x0, sizeof(dpoe_oam_img_ctrl_ctx_t));
    
    return;
}

void dpoe_oam_upg_cfg_ctx_init(void)
{
    dpoe_oam_upg_mgr_ctx_t *upg_mgr     = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_upg_cfg_ctx_t *upg_cfg_ctx = &(upg_mgr->dpoe_oam_upg_cfg_ctx);

    upg_cfg_ctx->recv_timeout = DPOE_OAM_UPG_DFLT_RECV_TIMEOUT;
    upg_cfg_ctx->retry_limit  = DPOE_OAM_UPG_DFLT_RETRY_LIMIT;

    return;
}

oam_int32 dpoe_oam_upg_verify_storage(void)
{
    dpoe_oam_upg_img_remove();
    
    return 0;
}

void dpoe_oam_upg_fsm_ctx_update(oam_uint32 state, oam_uint32 event)
{
    dpoe_oam_upg_mgr_ctx_t *upg_mgr = dpoe_oam_upg_mgr_ctx_glb_get();

    upg_mgr->dpoe_oam_upg_fsm_ctx.pre_upg_state = upg_mgr->dpoe_oam_upg_fsm_ctx.upg_state;
    upg_mgr->dpoe_oam_upg_fsm_ctx.upg_state     = state;
    upg_mgr->dpoe_oam_upg_fsm_ctx.event         = event;
}

oam_int32 dpoe_oam_upg_fta_resp_construct(oam_uint8 *data, oam_int32 *len, oam_uint16 block_num, oam_uint8 rCode)
{
    OAM_ASSERT_RET(data != NULL, 0);
    OAM_ASSERT_RET(len  != NULL, 0);
    oam_uint16 blockNum = htons(block_num);
    
    memcpy(data, &blockNum, sizeof(oam_uint16));
    memcpy((data + sizeof(oam_uint16)), &rCode, sizeof(oam_uint8));

    *len = sizeof(oam_uint16) + sizeof(oam_uint8);
    
    return 0;
}

oam_int32 dpoe_oam_upg_img_ctrl_construct(
    oam_uint8  *file_name,
    oam_uint8  file_len,
    oam_uint16 next_block,
    oam_uint8  *oui)
{
    dpoe_oam_upg_mgr_ctx_t  *upg_mgr       = dpoe_oam_upg_mgr_ctx_glb_get(); 
    dpoe_oam_img_ctrl_ctx_t *pImg_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);
    dpoe_oam_upg_cfg_ctx_t  *pUpg_cfg_ctx  = &(upg_mgr->dpoe_oam_upg_cfg_ctx);
    
    OAM_ASSERT_RET(file_name != NULL, 0);

    memset(pImg_ctrl_ctx, 0x0, sizeof(dpoe_oam_img_ctrl_ctx_t));

    if(file_len <= DPOE_OAM_UPG_FILE_NAME_MAX_LEN)
    {
        memcpy(pImg_ctrl_ctx->file_name, file_name, file_len);
    }

    pImg_ctrl_ctx->next_block_num = next_block;
    pImg_ctrl_ctx->retry_count    = pUpg_cfg_ctx->retry_limit;    
    pImg_ctrl_ctx->fd             = dpoe_oam_upg_img_open();
    memcpy(pImg_ctrl_ctx->oui, oui, OAM_OUI_LEN);
    
    return 0;
}

void dpoe_oam_upg_retry_timeout_handler(void *data)
{
    oam_uint8                 pdu[OAM_MAX_OAMPDU_SIZE];
    dpoe_oam_img_ctrl_ctx_t   *pImg_ctrl_ctx = (dpoe_oam_img_ctrl_ctx_t *)data;
    oam_pdu_dpoe_file_trans_t *pOutPkt       = (oam_pdu_dpoe_file_trans_t *)pdu;
    oam_int32                 outPktLen      = 0;
    oam_int32                 len            = 0;
    oam_llid_t                llid           = dpoe_oam_upg_trans_llid_get();

    DPOE_OAM_UPG_ENTER();

    memset(pdu, 0x0, OAM_MAX_OAMPDU_SIZE);
    pImg_ctrl_ctx->retry_count--;
    
    outPktLen   = dpoe_oam_build_file_trans_pdu_header(pImg_ctrl_ctx->oui,
                                                       llid, 
                                                       pdu,
                                                       OAM_PDU_DPOE_FILE_TRANS,
                                                       DPOE_OAM_UPG_FILE_TRANS_OPECODE_FTA);

    if(pImg_ctrl_ctx->retry_count <= 0)
    {
        DPOE_OAM_UPG_IMG_CLOSE(pImg_ctrl_ctx);
        dpoe_oam_upg_img_remove();
        dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE, DPOE_OAM_UPG_RETRY_TIMER_DONE_EVENT);
        dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), &len, pImg_ctrl_ctx->next_block_num, DPOE_OAM_FILE_TRANS_RESP_TIMEOUT);
        dpoe_oam_send(llid, pdu, (outPktLen+len));
        DPOE_OAM_UPG_LEAVE();
        return;
    }
    
    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_DATA_TRANSFER_STATE, DPOE_OAM_UPG_RETRY_TIMER_DONE_EVENT);
    dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), &len, pImg_ctrl_ctx->next_block_num, DPOE_OAM_FILE_TRANS_RESP_TIMEOUT);    
    dpoe_oam_send(llid, pdu, (outPktLen+len));
    dpoe_oam_upg_retry_timer_start();
    
    DPOE_OAM_UPG_LEAVE();

    return;
}

oam_int32 dpoe_oam_upg_retry_timer_start(void)
{
    dpoe_oam_upg_mgr_ctx_t  *upg_mgr       = dpoe_oam_upg_mgr_ctx_glb_get(); 
    dpoe_oam_img_ctrl_ctx_t *pImg_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);
    dpoe_oam_upg_cfg_ctx_t  *pUpg_cfg_ctx  = &(upg_mgr->dpoe_oam_upg_cfg_ctx);

    if(CA_INVALID_TIMER != pImg_ctrl_ctx->timerId){
        ca_timer_del(pImg_ctrl_ctx->timerId);
        pImg_ctrl_ctx->timerId = CA_INVALID_TIMER;
    }
    
    if(CA_INVALID_TIMER == pImg_ctrl_ctx->timerId)
    {
        pImg_ctrl_ctx->timerId = ca_timer_add(pUpg_cfg_ctx->recv_timeout,
                                              dpoe_oam_upg_retry_timeout_handler,
                                              pImg_ctrl_ctx);
        if(0 == pImg_ctrl_ctx->timerId)
            ca_timer_show();
        
        return 0;
    }

    ca_timer_restart(pImg_ctrl_ctx->timerId, pUpg_cfg_ctx->recv_timeout);

    return 0;
}

oam_int32 dpoe_oam_upg_retry_timer_stop(void)
{
    dpoe_oam_upg_mgr_ctx_t  *upg_mgr       = dpoe_oam_upg_mgr_ctx_glb_get(); 
    dpoe_oam_img_ctrl_ctx_t *pImg_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);

    if(pImg_ctrl_ctx->timerId != CA_INVALID_TIMER)
    {
        ca_timer_del(pImg_ctrl_ctx->timerId);
        pImg_ctrl_ctx->timerId = CA_INVALID_TIMER;
        return 0;
    }

    return 0;
}

oam_int32 dpoe_oam_upg_s_wait_wr_e_wr(void *data, void *outData, void *len)
{
    oam_pdu_dpoe_file_trans_t *pRecvPkt      = NULL;
    oam_pdu_varies_t          *pVar          = NULL;
    oam_pdu_dpoe_file_trans_t *pOutPkt       = NULL;
    oam_uint8 fw_name[ENV_INFO_LEN+1]={0};
    oam_uint8 i=0;
    DPOE_OAM_UPG_ENTER();

    OAM_ASSERT_RET(data    != NULL, -1);
    OAM_ASSERT_RET(outData != NULL, -1);
    OAM_ASSERT_RET(len     != NULL, -1);
    
    dpoe_oam_upg_verify_storage();
    
    pRecvPkt = (oam_pdu_dpoe_file_trans_t *)data;
    pVar     = (oam_pdu_varies_t *)(pRecvPkt->data);
    pOutPkt  = (oam_pdu_dpoe_file_trans_t *)outData;
    
    OAM_LOG_DEBUG("varies brance %d leaf %d width %d\r\n", pVar->hdr.branch, pVar->hdr.leaf, pVar->hdr.width);

    memcpy(fw_name,pRecvPkt->data,ENV_INFO_LEN);
    OAM_LOG_DEBUG("Firmware Filename get frm OLT is %s\r\n",fw_name);

    for(i=0;i<ENV_INFO_LEN;i++){
        if(NULL == fw_name[i]){
            memset(&fw_name[i],0x2A,(ENV_INFO_LEN-i));        
            break;
        }
    }  
    OAM_LOG_DEBUG("Firmware Filename is set %s\r\n",fw_name);
    ca_img_info_set(0,0,fw_name);
    dpoe_oam_upg_img_ctrl_construct("testxxx", 32, 0x0, pRecvPkt->oui);
    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_DATA_TRANSFER_STATE, DPOE_OAM_UPG_EOAMI_WR_EVENT);
    dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, 0x00, DPOE_OAM_FILE_TRANS_RESP_OK);
    DPOE_OAM_UPG_LEAVE();
    
    return 0;
}

oam_int32 dpoe_oam_upg_s_wait_wr_e_common(void *data, void *outData, void *len)
{
    dpoe_oam_upg_mgr_ctx_t *upg_mgr = dpoe_oam_upg_mgr_ctx_glb_get(); 
    oam_int32              ret      = 0;

    DPOE_OAM_UPG_ENTER();
    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_ACK_ILLEGAL_STATE, DPOE_OAM_UPG_UTC_EVENT);
    
    ret = upg_mgr->img_upg_fsm_handler[upg_mgr->dpoe_oam_upg_fsm_ctx.upg_state][DPOE_OAM_UPG_UTC_EVENT](data, outData, len);

    DPOE_OAM_UPG_LEAVE();
    
    return ret;
}

oam_int32 dpoe_oam_upg_s_ack_illegal_e_utc(void *data, void *outData, void *len)
{
    dpoe_oam_upg_mgr_ctx_t    *upg_mgr  = dpoe_oam_upg_mgr_ctx_glb_get(); 
    oam_pdu_dpoe_file_trans_t *pOutPkt  = NULL;
    
    DPOE_OAM_UPG_ENTER();

    OAM_ASSERT_RET(data    != NULL, -1);
    OAM_ASSERT_RET(outData != NULL, -1);
    OAM_ASSERT_RET(len     != NULL, -1);

    pOutPkt  = (oam_pdu_dpoe_file_trans_t *)outData;

    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE, DPOE_OAM_UPG_UTC_EVENT);
    
    dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, 0x00, DPOE_OAM_FILE_TRANS_RESP_ILLEGAL_OPERATION);
    
    DPOE_OAM_UPG_LEAVE();

    return 0;
}

oam_int32 dpoe_oam_upg_s_wr_e_store_done(void *data, void *outData, void *len)
{
    return 0;
}

oam_int32 dpoe_oam_upg_s_ar_e_rc_ok(void *data, void *outData, void *len)
{

    return 0;
}

oam_int32 dpoe_oam_upg_s_wait_data_trans_e_ftd(void *data, void *outData, void *len)
{
    oam_pdu_dpoe_file_trans_t      *pOutPkt      = NULL;    
    oam_pdu_dpoe_file_trans_data_t *pRecvPkt     = NULL;
    dpoe_oam_upg_mgr_ctx_t         *upg_mgr      = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_img_ctrl_ctx_t        *img_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);
    dpoe_oam_upg_cfg_ctx_t         *pUpg_cfg     = &(upg_mgr->dpoe_oam_upg_cfg_ctx);
    oam_int32                      ret           = 0;
    oam_uint16                     blockWidth    = 0;
    oam_uint16                     blockNum      = 0;
    
    DPOE_OAM_UPG_ENTER();
    
    dpoe_oam_upg_retry_timer_stop();

    OAM_ASSERT_RET(data    != NULL, -1);
    OAM_ASSERT_RET(outData != NULL, -1);
    OAM_ASSERT_RET(len     != NULL, -1);
        
    pOutPkt  = (oam_pdu_dpoe_file_trans_t *)outData;
    pRecvPkt = (oam_pdu_dpoe_file_trans_data_t *)data;
    
    dpoe_oam_upg_img_ctrl_retry_count_set(pUpg_cfg->retry_limit);
    blockWidth = ntohs(pRecvPkt->blockWidth);
    blockNum   = ntohs(pRecvPkt->blockNum);
    
    if((img_ctrl_ctx->next_block_num == blockNum) &&
        (blockWidth > 0))
    {    
        img_ctrl_ctx->next_block_num++;
        img_ctrl_ctx->commit_len += blockWidth;
        
        ret = dpoe_oam_upg_img_write(img_ctrl_ctx->fd, pRecvPkt->data, blockWidth);
        if(ret < 0)
        {       
            DPOE_OAM_UPG_IMG_CLOSE(img_ctrl_ctx);
            dpoe_oam_upg_img_remove();
            dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE, DPOE_OAM_UPG_EOAMI_FTD_EVENT);
            dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, img_ctrl_ctx->next_block_num, DPOE_OAM_FILE_TRANS_RESP_NO_ACCESS);
            return 0;
        }
    }

    OAM_LOG_DEBUG("recv block data commit len %d\r\n", img_ctrl_ctx->commit_len);
    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_DATA_TRANSFER_STATE, DPOE_OAM_UPG_EOAMI_FTD_EVENT);
    dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, img_ctrl_ctx->next_block_num, DPOE_OAM_FILE_TRANS_RESP_OK);
    dpoe_oam_upg_retry_timer_start();
    
    DPOE_OAM_UPG_LEAVE();
    
    return 0;
}

static void dpoe_oam_upg_img_upgrade_entry(void *data)
{
    dpoe_oam_upg_img_upgrade();
    oam_upg_done_evt_msg_send(dpoe_oam_upg_trans_llid_get());
    
    return;
}

oam_int32 dpoe_oam_upg_done_evt_handle(oam_llid_t llid)
{
    dpoe_oam_upg_mgr_ctx_t    *upg_mgr      = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_upg_fsm_ctx_t    *upg_fsm_ctx  = &(upg_mgr->dpoe_oam_upg_fsm_ctx);    
    dpoe_oam_img_ctrl_ctx_t   *img_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);
    oam_uint8                 pdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dpoe_file_trans_t *pOutPkt      = (oam_pdu_dpoe_file_trans_t *)pdu;    
    oam_uint32                outPktLen     = 0;
    oam_uint32                len           = 0;
    
    DPOE_OAM_UPG_ENTER();
    
    outPktLen = dpoe_oam_build_file_trans_pdu_header(img_ctrl_ctx->oui, llid, pdu,
                                                     OAM_PDU_DPOE_FILE_TRANS,
                                                     DPOE_OAM_UPG_FILE_TRANS_OPECODE_FTA);
    
    upg_mgr->img_upg_fsm_handler[upg_fsm_ctx->upg_state][DPOE_OAM_UPG_COMMMIT_DONE_EVENT](NULL, (void *)pOutPkt, &len);
    
    if(!len)
    {
        OAM_LOG_DEBUG("resp len is NULL\r\n");        
        DPOE_OAM_UPG_LEAVE();
        return 0;
    }
    
    outPktLen += len;
    dpoe_oam_send(llid, pdu, outPktLen);
    
    DPOE_OAM_UPG_LEAVE();

    return 0;
}
    
oam_int32 dpoe_oma_upg_img_commit_start(void)
{
    dpoe_oam_upg_mgr_ctx_t  *upg_mgr      = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_img_ctrl_ctx_t *img_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);
    
    return OS_THREAD_CREATE(&(img_ctrl_ctx->threadId), "dpoe_oam_upg", dpoe_oam_upg_img_upgrade_entry, img_ctrl_ctx, 32*1024, 3, 0);
}

oam_int32 dpoe_oam_upg_s_wait_data_trans_e_fta_end(void *data, void *outData, void *len)
{
    oam_pdu_dpoe_file_trans_t      *pOutPkt      = NULL;    
    oam_pdu_dpoe_file_trans_data_t *pRecvPkt     = NULL;
    dpoe_oam_upg_mgr_ctx_t         *upg_mgr      = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_img_ctrl_ctx_t        *img_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);
    dpoe_oam_upg_cfg_ctx_t         *pUpg_cfg     = &(upg_mgr->dpoe_oam_upg_cfg_ctx);
    oam_int32                      ret           = 0;
    
    DPOE_OAM_UPG_ENTER();
    OAM_ASSERT_RET(data    != NULL, -1);
    OAM_ASSERT_RET(outData != NULL, -1);
    OAM_ASSERT_RET(len     != NULL, -1);

    pOutPkt  = (oam_pdu_dpoe_file_trans_t *)outData;
    pRecvPkt = (oam_pdu_dpoe_file_trans_data_t *)data;
    
    dpoe_oam_upg_retry_timer_stop();
    DPOE_OAM_UPG_IMG_CLOSE(img_ctrl_ctx);
    ret = dpoe_oma_upg_img_commit_start();
    if(ret < 0)
    {
        dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE, DPOE_OAM_UPG_EOAMI_FTA_END_EVENT);
        dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, 0x00, DPOE_OAM_FILE_TRANS_RESP_UNDEFINED);        
        DPOE_OAM_UPG_LEAVE();
        return 0;
    }

    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_COMMIT_IMAGE_STATE, DPOE_OAM_UPG_EOAMI_FTA_END_EVENT);
    DPOE_OAM_UPG_LEAVE();

    return 0;
}

oam_int32 dpoe_oam_upg_s_wait_data_trans_e_wr(void *data, void *outData, void *len)
{
    oam_pdu_dpoe_file_trans_t *pOutPkt = NULL;    
    dpoe_oam_upg_mgr_ctx_t    *upg_mgr      = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_img_ctrl_ctx_t   *img_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);
    DPOE_OAM_UPG_ENTER();
    
    dpoe_oam_upg_retry_timer_stop();
    DPOE_OAM_UPG_IMG_CLOSE(img_ctrl_ctx);
    dpoe_oam_upg_img_remove();

    OAM_ASSERT_RET(data    != NULL, -1);
    OAM_ASSERT_RET(outData != NULL, -1);
    OAM_ASSERT_RET(len     != NULL, -1);
        
    pOutPkt  = (oam_pdu_dpoe_file_trans_t *)outData;

    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE, DPOE_OAM_UPG_EOAMI_WR_EVENT);
    dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, 0x00, DPOE_OAM_FILE_TRANS_RESP_ILLEGAL_OPERATION);
    DPOE_OAM_UPG_LEAVE();
    
    return 0;
}

oam_int32 dpoe_oam_upg_s_wait_data_trans_e_fta_err(void *data, void *outData, void *len)
{
    oam_pdu_dpoe_file_trans_t *pOutPkt      = NULL;    
    dpoe_oam_upg_mgr_ctx_t    *upg_mgr      = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_img_ctrl_ctx_t   *img_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);
    
    DPOE_OAM_UPG_ENTER();
    
    dpoe_oam_upg_retry_timer_stop();
    DPOE_OAM_UPG_IMG_CLOSE(img_ctrl_ctx);
    dpoe_oam_upg_img_remove();
    
    OAM_ASSERT_RET(data    != NULL, -1);
    OAM_ASSERT_RET(outData != NULL, -1);
    OAM_ASSERT_RET(len     != NULL, -1);
        
    pOutPkt  = (oam_pdu_dpoe_file_trans_t *)outData;

    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE, DPOE_OAM_UPG_EOAMI_FTA_ERROR_EVENT);
    dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, 0x00, DPOE_OAM_FILE_TRANS_RESP_OK);
    DPOE_OAM_UPG_LEAVE();
    
    return 0;
}

oam_int32 dpoe_oam_upg_s_wait_data_trans_e_fta_timeout(void *data, void *outData, void *len)
{
    oam_pdu_dpoe_file_trans_t *pOutPkt = NULL;
    dpoe_oam_upg_mgr_ctx_t    *upg_mgr      = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_img_ctrl_ctx_t   *img_ctrl_ctx = &(upg_mgr->dpoe_oam_img_ctrl_ctx);
    
    DPOE_OAM_UPG_ENTER();
    
    dpoe_oam_upg_retry_timer_stop();
    DPOE_OAM_UPG_IMG_CLOSE(img_ctrl_ctx);
    dpoe_oam_upg_img_remove();

    OAM_ASSERT_RET(data    != NULL, -1);
    OAM_ASSERT_RET(outData != NULL, -1);
    OAM_ASSERT_RET(len     != NULL, -1);
        
    pOutPkt  = (oam_pdu_dpoe_file_trans_t *)outData;

    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE, DPOE_OAM_UPG_EOAMI_FTA_TIMEOUT_EVENT);
    dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, 0x00, DPOE_OAM_FILE_TRANS_RESP_OK);
    DPOE_OAM_UPG_LEAVE();
    
    return 0;
}

oam_int32 dpoe_oam_upg_s_wait_data_trans_e_timer_done(void *data, void *outData, void *len)
{
    return 0;
}

oam_int32 dpoe_oam_upg_s_verify_img_e_rc_ok(void *data, void *outData, void *len)
{
    return 0;
}

oam_int32 dpoe_oam_upg_s_commit_img_e_common(void *data, void *outData, void *len, oam_uint32 event)
{
    oam_pdu_dpoe_file_trans_t *pOutPkt = NULL;
    
    OAM_ASSERT_RET(data    != NULL, -1);
    OAM_ASSERT_RET(outData != NULL, -1);
    OAM_ASSERT_RET(len     != NULL, -1);
    
    DPOE_OAM_UPG_ENTER();
    pOutPkt  = (oam_pdu_dpoe_file_trans_t *)outData;
    
    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_ACK_BUSY_STATE, event);
    dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, 0x00, DPOE_OAM_FILE_TRANS_RESP_BUSY);
    DPOE_OAM_UPG_LEAVE();

    return 0;
}

oam_int32 dpoe_oam_upg_s_commit_img_e_ftd(void *data, void *outData, void *len)
{
    return dpoe_oam_upg_s_commit_img_e_common(data, outData, len, DPOE_OAM_UPG_EOAMI_FTD_EVENT);
}

oam_int32 dpoe_oam_upg_s_commit_img_e_fta_end(void *data, void *outData, void *len)
{
    return dpoe_oam_upg_s_commit_img_e_common(data, outData, len, DPOE_OAM_UPG_EOAMI_FTA_END_EVENT);
}

oam_int32 dpoe_oam_upg_s_commit_img_e_fta_err(void *data, void *outData, void *len)
{
    return dpoe_oam_upg_s_commit_img_e_common(data, outData, len, DPOE_OAM_UPG_EOAMI_FTA_ERROR_EVENT);
}

oam_int32 dpoe_oam_upg_s_commit_img_e_fta_timeout(void *data, void *outData, void *len)
{
    return dpoe_oam_upg_s_commit_img_e_common(data, outData, len, DPOE_OAM_UPG_EOAMI_FTA_TIMEOUT_EVENT);
}

oam_int32 dpoe_oam_upg_s_commit_img_e_wr(void *data, void *outData, void *len)
{
    return dpoe_oam_upg_s_commit_img_e_common(data, outData, len, DPOE_OAM_UPG_EOAMI_WR_EVENT);
}

oam_int32 dpoe_oam_upg_s_ack_busy_e_any(void *data, void *outData, void *len, oam_uint32 event)
{
    oam_pdu_dpoe_file_trans_t *pOutPkt = NULL;
    
    OAM_ASSERT_RET(data    != NULL, -1);
    OAM_ASSERT_RET(outData != NULL, -1);
    OAM_ASSERT_RET(len     != NULL, -1);
    
    DPOE_OAM_UPG_ENTER();
    pOutPkt  = (oam_pdu_dpoe_file_trans_t *)outData;
    
    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_ACK_BUSY_STATE, event);
    dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, 0x00, DPOE_OAM_FILE_TRANS_RESP_BUSY);
    
    DPOE_OAM_UPG_LEAVE();

    return 0;
}

oam_int32 dpoe_oam_upg_img_active_part_get(void)
{
    ca_img_info_t img_info0;

    dpoe_oam_upg_img_info_get(0, 0, &img_info0);
    
    if(img_info0.active_flag)
        return 0;

    return 1;
}

oam_int32 dpoe_oam_upg_img_verify(void)
{
    ca_img_info_t img_info0;
    ca_img_info_t img_info1;
    oam_int32     ret;
    
    dpoe_oam_upg_img_info_get(0, 0, &img_info0);
    dpoe_oam_upg_img_info_get(0, 1, &img_info1);
    
    OAM_LOG_MAJ("img Img0 commit flag %d valid flag %d active flag %d\r\n", img_info0.commit_flag, img_info0.valid_flag, img_info0.active_flag);
    OAM_LOG_MAJ("img Img1 commit flag %d valid flag %d active flag %d\r\n", img_info1.commit_flag, img_info1.valid_flag, img_info1.active_flag);
    
    ret = dpoe_oam_upg_img_active_part_get();
    if(ret)
        return img_info0.valid_flag;

    return img_info1.valid_flag;
}

oam_int32 dpoe_oam_upg_s_commit_img_e_commit_done(void *data, void *outData, void *len)
{
    dpoe_oam_upg_mgr_ctx_t *upg_mgr     = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_upg_fsm_ctx_t *upg_fsm_ctx = &(upg_mgr->dpoe_oam_upg_fsm_ctx);
    
    DPOE_OAM_UPG_ENTER();
    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_CONFIRM_IMAGE_STATE, DPOE_OAM_UPG_COMMMIT_DONE_EVENT);
    upg_mgr->img_upg_fsm_handler[upg_fsm_ctx->upg_state][DPOE_OAM_UPG_UTC_EVENT](data, outData, len);
    DPOE_OAM_UPG_LEAVE();

    return 0;
}

oam_int32 dpoe_oam_upg_s_ack_busy_e_commit_done(void *data, void *outData, void *len)
{
    dpoe_oam_upg_mgr_ctx_t *upg_mgr     = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_upg_fsm_ctx_t *upg_fsm_ctx = &(upg_mgr->dpoe_oam_upg_fsm_ctx);
    
    DPOE_OAM_UPG_ENTER();
    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_CONFIRM_IMAGE_STATE, DPOE_OAM_UPG_COMMMIT_DONE_EVENT);
    upg_mgr->img_upg_fsm_handler[upg_fsm_ctx->upg_state][DPOE_OAM_UPG_UTC_EVENT](data, outData, len);
    DPOE_OAM_UPG_LEAVE();

    return 0;
}

oam_int32 dpoe_oam_upg_s_ack_busy_e_ftd(void *data, void *outData, void *len)
{
    return dpoe_oam_upg_s_ack_busy_e_any(data, outData, len, DPOE_OAM_UPG_EOAMI_FTD_EVENT);
}

oam_int32 dpoe_oam_upg_s_ack_busy_e_fta_end(void *data, void *outData, void *len)
{
    return dpoe_oam_upg_s_ack_busy_e_any(data, outData, len, DPOE_OAM_UPG_EOAMI_FTA_END_EVENT);
}

oam_int32 dpoe_oam_upg_s_ack_busy_e_fta_err(void *data, void *outData, void *len)
{
    return dpoe_oam_upg_s_ack_busy_e_any(data, outData, len, DPOE_OAM_UPG_EOAMI_FTA_ERROR_EVENT);
}

oam_int32 dpoe_oam_upg_s_ack_busy_e_fta_timeout(void *data, void *outData, void *len)
{
    return dpoe_oam_upg_s_ack_busy_e_any(data, outData, len, DPOE_OAM_UPG_EOAMI_FTA_TIMEOUT_EVENT);
}

oam_int32 dpoe_oam_upg_s_ack_busy_e_wr(void *data, void *outData, void *len)
{
    return dpoe_oam_upg_s_ack_busy_e_any(data, outData, len, DPOE_OAM_UPG_EOAMI_WR_EVENT);
}

oam_int32 dpoe_oam_upg_img_commit(void)
{
    oam_int32 ret = 0;

    ret = dpoe_oam_upg_img_active_part_get();

    if(ret)
    {
        dpoe_oam_upg_img_active_set(0,0,1);
        dpoe_oam_upg_img_active_set(0,1,0);
        dpoe_oam_upg_img_commit_set(0, 0, 1);
        dpoe_oam_upg_img_commit_set(0, 1, 0);
        dpoe_oam_upg_img_executed_set(0, 1, 1);
        dpoe_oam_upg_img_executed_set(0, 0, 0);
        return 0;
    }

    dpoe_oam_upg_img_active_set(0,0,0);
    dpoe_oam_upg_img_active_set(0,1,1);
    dpoe_oam_upg_img_commit_set(0, 0, 0);
    dpoe_oam_upg_img_commit_set(0, 1, 1);
    dpoe_oam_upg_img_executed_set(0, 0, 1);
    dpoe_oam_upg_img_executed_set(0, 1, 0);

    return 0;
}
    
oam_int32 dpoe_oam_upg_s_confirm_img_e_utc(void *data, void *outData, void *len)
{
    oam_int32                 ret      = 0;
    oam_pdu_dpoe_file_trans_t *pOutPkt = NULL;
    
    OAM_ASSERT_RET(outData != NULL, -1);
    OAM_ASSERT_RET(len     != NULL, -1);
    DPOE_OAM_UPG_ENTER();

    pOutPkt  = (oam_pdu_dpoe_file_trans_t *)outData;

    ret = dpoe_oam_upg_img_verify();
    if(ret <= 0)
    {
        dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE, DPOE_OAM_UPG_UTC_EVENT);
        dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, 0x00, DPOE_OAM_FILE_TRANS_RESP_UNDEFINED);
        DPOE_OAM_UPG_LEAVE();
        return ret;
    }

    dpoe_oam_upg_img_commit();
    dpoe_oam_upg_fsm_ctx_update(DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE, DPOE_OAM_UPG_UTC_EVENT);
    dpoe_oam_upg_fta_resp_construct((oam_int8 *)(pOutPkt->data), (oam_int32 *)len, 0x00, DPOE_OAM_FILE_TRANS_RESP_OK);
    DPOE_OAM_UPG_LEAVE();
    
    return 0;
}

oam_int32 dpoe_oam_img_upg_fsm_err_handler(void *data, void *outData, void *len)
{
    dpoe_oam_upg_mgr_ctx_t *upg_mgr     = dpoe_oam_upg_mgr_ctx_glb_get();
    dpoe_oam_upg_fsm_ctx_t *upg_fsm_ctx = &(upg_mgr->dpoe_oam_upg_fsm_ctx);

    OAM_LOG_MAJ("unsupport state %d pre_state %d event %d\r\n", upg_fsm_ctx->upg_state,
                                                                upg_fsm_ctx->pre_upg_state,
                                                                upg_fsm_ctx->event);

    return 0;
}

oam_int32 dpoe_oam_img_upg_fsm_handler_init(void)
{
    dpoe_oam_upg_mgr_ctx_t *upg_mgr     = dpoe_oam_upg_mgr_ctx_glb_get();
    oam_int32 stateIdx = 0;
    oam_int32 eventIdx = 0;

    for(stateIdx = 0; stateIdx < DPOE_OAM_UPG_MAX_STATE; stateIdx++)
    {
        for(eventIdx = 0; eventIdx < DPOE_OAM_UPG_MAX_EVENT; eventIdx++)
        {
            upg_mgr->img_upg_fsm_handler[stateIdx][eventIdx] = dpoe_oam_img_upg_fsm_err_handler;
        }
    }
    
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE][DPOE_OAM_UPG_EOAMI_WR_EVENT]          = dpoe_oam_upg_s_wait_wr_e_wr;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE][DPOE_OAM_UPG_EOAMI_FTD_EVENT]         = dpoe_oam_upg_s_wait_wr_e_common;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE][DPOE_OAM_UPG_EOAMI_FTA_END_EVENT]     = dpoe_oam_upg_s_wait_wr_e_common;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE][DPOE_OAM_UPG_EOAMI_FTA_ERROR_EVENT]   = dpoe_oam_upg_s_wait_wr_e_common;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE][DPOE_OAM_UPG_EOAMI_FTA_TIMEOUT_EVENT] = dpoe_oam_upg_s_wait_wr_e_common;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_ACK_ILLEGAL_STATE][DPOE_OAM_UPG_UTC_EVENT]                      = dpoe_oam_upg_s_ack_illegal_e_utc;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WRITE_REQUEST_STATE][DPOE_OAM_UPG_STORAGEDONE_EVENT]            = dpoe_oam_upg_s_wr_e_store_done;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_ACK_REQUEST_STATE][DPOE_OAM_UPG_RESULTCODE_OK_EVENT]            = dpoe_oam_upg_s_ar_e_rc_ok;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_DATA_TRANSFER_STATE][DPOE_OAM_UPG_EOAMI_FTD_EVENT]         = dpoe_oam_upg_s_wait_data_trans_e_ftd;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_DATA_TRANSFER_STATE][DPOE_OAM_UPG_EOAMI_FTA_END_EVENT]     = dpoe_oam_upg_s_wait_data_trans_e_fta_end;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_DATA_TRANSFER_STATE][DPOE_OAM_UPG_EOAMI_WR_EVENT]          = dpoe_oam_upg_s_wait_data_trans_e_wr;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_DATA_TRANSFER_STATE][DPOE_OAM_UPG_EOAMI_FTA_ERROR_EVENT]   = dpoe_oam_upg_s_wait_data_trans_e_fta_err;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_DATA_TRANSFER_STATE][DPOE_OAM_UPG_EOAMI_FTA_TIMEOUT_EVENT] = dpoe_oam_upg_s_wait_data_trans_e_fta_timeout;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_WAIT_DATA_TRANSFER_STATE][DPOE_OAM_UPG_RETRY_TIMER_DONE_EVENT]  = dpoe_oam_upg_s_wait_data_trans_e_timer_done;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_VERIFY_IMAGE_STATE][DPOE_OAM_UPG_RESULTCODE_OK_EVENT]           = dpoe_oam_upg_s_verify_img_e_rc_ok;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_COMMIT_IMAGE_STATE][DPOE_OAM_UPG_EOAMI_FTD_EVENT]               = dpoe_oam_upg_s_commit_img_e_ftd;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_COMMIT_IMAGE_STATE][DPOE_OAM_UPG_EOAMI_FTA_END_EVENT]           = dpoe_oam_upg_s_commit_img_e_fta_end;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_COMMIT_IMAGE_STATE][DPOE_OAM_UPG_EOAMI_FTA_ERROR_EVENT]         = dpoe_oam_upg_s_commit_img_e_fta_err;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_COMMIT_IMAGE_STATE][DPOE_OAM_UPG_EOAMI_FTA_TIMEOUT_EVENT]       = dpoe_oam_upg_s_commit_img_e_fta_timeout;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_COMMIT_IMAGE_STATE][DPOE_OAM_UPG_EOAMI_WR_EVENT]                = dpoe_oam_upg_s_commit_img_e_wr;    
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_COMMIT_IMAGE_STATE][DPOE_OAM_UPG_COMMMIT_DONE_EVENT]            = dpoe_oam_upg_s_commit_img_e_commit_done;    
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_ACK_BUSY_STATE][DPOE_OAM_UPG_COMMMIT_DONE_EVENT]                = dpoe_oam_upg_s_ack_busy_e_commit_done;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_ACK_BUSY_STATE][DPOE_OAM_UPG_EOAMI_FTD_EVENT]                   = dpoe_oam_upg_s_ack_busy_e_ftd;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_ACK_BUSY_STATE][DPOE_OAM_UPG_EOAMI_FTA_END_EVENT]               = dpoe_oam_upg_s_ack_busy_e_fta_end;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_ACK_BUSY_STATE][DPOE_OAM_UPG_EOAMI_FTA_ERROR_EVENT]             = dpoe_oam_upg_s_ack_busy_e_fta_err;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_ACK_BUSY_STATE][DPOE_OAM_UPG_EOAMI_FTA_TIMEOUT_EVENT]           = dpoe_oam_upg_s_ack_busy_e_fta_timeout;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_ACK_BUSY_STATE][DPOE_OAM_UPG_EOAMI_WR_EVENT]                    = dpoe_oam_upg_s_ack_busy_e_wr;
    upg_mgr->img_upg_fsm_handler[DPOE_OAM_UPG_CONFIRM_IMAGE_STATE][DPOE_OAM_UPG_UTC_EVENT]                    = dpoe_oam_upg_s_confirm_img_e_utc;
    
    
    return 0;
}

oam_int32 dpoe_oam_upg_init(void)
{
    dpoe_oam_upg_fsm_ctx_init();
    dpoe_oam_img_ctrl_ctx_init();
    dpoe_oam_upg_cfg_ctx_init();
    dpoe_oam_img_upg_fsm_handler_init();

    return 0;
}

#endif/*HAVE_DPOE_OAM*/
