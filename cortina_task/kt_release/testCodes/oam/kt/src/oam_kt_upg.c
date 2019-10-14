#ifdef HAVE_KT_OAM
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_TFTPD
#include "tftp.h"
#endif
#include "oam_main.h"
#include "oam_cmn.h"
#include "oam_core.h"
#include "oam_supp.h"
#include "oam_event.h"
#include "oam_control.h"
#include "oam_timer.h"
#include "oam_adapt.h"
#include "oam_std_pdu.h"
#include "oam_client.h"
#include "oam_mux_par.h"

//#include "oam_kt.h"
#include "oam_kt_client.h"
#include "oam_kt_pdu.h"
#include "oam_kt_adapt.h"
#include "oam_kt_upg.h"

#include "ca_version.h"
#include "rate.h"
#include "scfg.h"
#include "ca_types.h"
#include "ca_gpio.h"
#include "classifier.h"

oam_kt_upg_mgr_ctx_t oam_kt_upg_mgr_ctx_gl;
ca_shaper_t  g_shaper1;
ca_shaper_t  g_shaper2;

void oam_kt_config_shaper(void){

    ca_shaper_t  cpu_shaper;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    ca_port_id_t pon_port = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
    ca_uint32_t index;
    ca_status_t ret = CA_E_OK;

    memset(&g_shaper1,0,sizeof(ca_shaper_t));
    memset(&g_shaper2,0,sizeof(ca_shaper_t));
    memset(&cpu_shaper,0,sizeof(ca_shaper_t));
    memset(&key, 0, sizeof(key));
    memset(&key_mask, 0, sizeof(key_mask));
    memset(&action, 0, sizeof(action));

    //ca_port_shaper_get(0,CA_PORT_ID(CA_PORT_TYPE_CPU,0x10),&g_shaper1);
#if 0
    cpu_shaper.enable = TRUE;
    cpu_shaper.pps = TRUE;
    cpu_shaper.rate = 100;
    cpu_shaper.burst_size = 100;
    ca_port_shaper_set(0,CA_PORT_ID(CA_PORT_TYPE_CPU,0x10),&cpu_shaper);

    key.src_port = pon_port;
    key_mask.src_port = 1;
    action.forward = CA_CLASSIFIER_FORWARD_FE;
    action.options.priority = 7;
    action.options.masks.priority = 1;
    key_mask.key_handle = 1;

    ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&index);
    if(CA_E_OK != ret){
        OAM_ORG_LOG("ca_classifier_rule_add() pon port failed in oam_kt_config_shaper() \n\r");
    }
#endif
    memset(&key, 0, sizeof(key));
    memset(&key_mask, 0, sizeof(key_mask));
    memset(&action, 0, sizeof(action));

    key.src_port = CA_PORT_ID(CA_PORT_TYPE_CPU,0x10);
    key_mask.src_port = 1;
    action.forward = CA_CLASSIFIER_FORWARD_FE;
    action.options.priority = 6;
    action.options.masks.priority = 1;

    ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&index);
    if(CA_E_OK != ret){
        cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_ERROR,0, "ca_classifier_rule_add() cpu0 failed in oam_kt_config_shaper() \n\r");
    }

}

oam_kt_upg_mgr_ctx_t *oam_kt_upg_mgr_ctx_gl_get(void)
{
    return &oam_kt_upg_mgr_ctx_gl;
}

oam_int32 oam_kt_upg_tftpd_thread_start_msg_send(void *data, void *arg)
{
    return oam_img_upg_msg_send(OAM_KT_IMG_UPG_EVT, OAM_KT_UPG_TFTP_THREAD_START_S);
}

oam_int32 oam_kt_upg_tftpd_thread_stop_msg_send(void *data, void *arg)
{
    return oam_img_upg_msg_send(OAM_KT_IMG_UPG_EVT, OAM_KT_UPG_TFTP_THREAD_STOP_S);
}

oam_int32 oam_kt_upg_tftpd_recv_done_msg_send(void *data, void *arg)
{
    //ca_port_shaper_set(0,CA_PORT_ID(CA_PORT_TYPE_CPU,0x10),&g_shaper1);
    return oam_img_upg_msg_send(OAM_KT_IMG_UPG_EVT, OAM_KT_UPG_TFTP_RECV_DONE_S);
}

oam_int32 oam_kt_upg_tftpd_recv_err_msg_send(void *data, void *arg)
{
    return oam_img_upg_msg_send(OAM_KT_IMG_UPG_EVT, OAM_KT_UPG_TFTP_RECV_ERR_S);
}

void tk_oam_onu_img_upg_handle(void *arg)
{
    oam_img_upgrade_adapt();
    oam_img_upg_msg_send(OAM_KT_IMG_UPG_EVT, OAM_KT_UPG_TFTP_IMG_UPG_DONE_S);
}

oam_int32 tk_oam_onu_upg_img_start(void)
{
    OAM_KT_UPG_THREAD_CREATE(tk_oam_onu_img_upg_handle, NULL);

    return 0;
}

void tk_oam_img_upg_done_handle(void * data)
{
    oam_int32            ret      = 0;
    int file_fd = 0;
    char upgrade_status = 0;
    oam_kt_upg_mgr_ctx_t *upg_mgr = oam_kt_upg_mgr_ctx_gl_get();

    file_fd = open("/tmp/upgrade_status",O_RDONLY);
    read(file_fd, &upgrade_status, 1);
    close(file_fd);
    if(upgrade_status == 0x30)
    {
        ca_timer_add(3000, tk_oam_img_upg_done_handle, NULL);
        return;
    }
    if(upgrade_status < 0x39)
    {
        upg_mgr->dl_state = OAM_KT_UPG_CRC_ERR_S;
        return;
    }
    ret = oam_img_verify_adapt();
    if(ret <= 0)
    {
        upg_mgr->dl_state = OAM_KT_UPG_WR_ERR_S;
        return;
    }

    oam_img_commit_adapt();
    upg_mgr->dl_state = OAM_KT_UPG_FINSHED_S;
#if 0
{
    ca_device_config_tlv_t  scfg_tlv;
    ca_uint32               board_info;
    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_BOARD_INFO_BIT;
    scfg_tlv.len  = sizeof(ca_uint32_t);
    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if(CA_E_OK == ret)
    {
        memcpy(&board_info, scfg_tlv.value, sizeof(ca_uint32_t));
        if (board_info & BOARD_INFO_BIT_3)
        {
            CA_GPIO_SET_VALUE(CA_GPIO_FW_UPG_IND,0);
        }
    }
}
#endif
    if(upg_mgr->auto_reboot == OAM_KT_UPG_AUTO_REBOOT)
    {
        kt_oam_action_reset_adapt(1);
    }

    return;
}

void oam_kt_upg_tftp_transaction_clean(void)
{
    oam_kt_upg_mgr_ctx_t *upg_mgr = oam_kt_upg_mgr_ctx_gl_get();

    tftpd_thread_exit_set();
#ifdef TFTP_VLAN
    oam_kt_adapt_vlan_port_del(upg_mgr->tftp_addr.vlan);
    oam_kt_adapt_dev_vlan_del(upg_mgr->dev_ctx.vlanDev);
#endif
}

void oam_kt_upg_evt_handler(void *msg)
{
    oam_img_upg_msg_t    *upg_msg = (oam_img_upg_msg_t *)msg;
    oam_kt_upg_mgr_ctx_t *upg_mgr = oam_kt_upg_mgr_ctx_gl_get();

    switch(upg_msg->data){
        case OAM_KT_UPG_TFTP_RECV_ERR_S:
            upg_mgr->dl_state = OAM_KT_UPG_DL_ERR_S;
            oam_kt_upg_tftp_transaction_clean();
            break;

        case OAM_KT_UPG_TFTP_RECV_DONE_S:
            upg_mgr->dl_state = OAM_KT_UPG_DL_WRITING_S;
            tk_oam_onu_upg_img_start();
            oam_kt_upg_tftp_transaction_clean();
            break;

        case OAM_KT_UPG_TFTP_IMG_UPG_DONE_S:
            ca_timer_add(3000, tk_oam_img_upg_done_handle, NULL);
            break;

        case OAM_KT_UPG_TFTP_THREAD_START_S:
            upg_mgr->tftpd_thread_state = OAM_KT_UPG_THREAD_STATE_RUNNING;
            break;

        case OAM_KT_UPG_TFTP_THREAD_STOP_S:
            upg_mgr->tftpd_thread_state = OAM_KT_UPG_THREAD_STATE_STOP;
            break;

        default:
            break;
    }
}

oam_int32 tk_oam_onu_upg_tftpd_start(void)
{
    oam_kt_upg_mgr_ctx_t *upg_mgr = oam_kt_upg_mgr_ctx_gl_get();

    if(OAM_KT_UPG_THREAD_STATE_RUNNING == upg_mgr->tftpd_thread_state)
    {
        OAM_ORG_LOG_MAJ("tftpd thread already run\r\n");
        return 0;
    }

    OAM_KT_UPG_THREAD_CREATE(TFTPD_START, NULL);

    return 0;
}

#ifdef TFTP_VLAN
void oam_kt_upg_vlan_dev_construct(void)
{
    oam_kt_upg_mgr_ctx_t *upg_mgr   = oam_kt_upg_mgr_ctx_gl_get();

    memset(upg_mgr->dev_ctx.vlanDev, 0x0, KT_MAX_IFNAME_LEN);
    snprintf(upg_mgr->dev_ctx.vlanDev, KT_MAX_IFNAME_LEN, "%s.%d", upg_mgr->dev_ctx.dfltDev, upg_mgr->tftp_addr.vlan);

    OAM_ORG_LOG("vlan dev name %s\r\n", upg_mgr->dev_ctx.vlanDev);
}
#endif

void oam_kt_upg_tftpd_addr_ctx_construct(oam_kt_upg_tftp_addr_ctx_t *addr_ctx)
{
    oam_kt_upg_mgr_ctx_t *upg_mgr = oam_kt_upg_mgr_ctx_gl_get();

    upg_mgr->tftp_addr.gw       = ntohl(addr_ctx->gw);
    upg_mgr->tftp_addr.ipAddr   = ntohl(addr_ctx->ipAddr);
    upg_mgr->tftp_addr.ipMask   = ntohl(addr_ctx->ipMask);
    upg_mgr->tftp_addr.vlan     = ntohs(addr_ctx->vlan);
    upg_mgr->tftp_addr.priority = addr_ctx->priority;
    OAM_ORG_LOG("gw %u ipAddr %u ipMask %u priority %d vlan %d\r\n", upg_mgr->tftp_addr.gw, upg_mgr->tftp_addr.ipAddr, upg_mgr->tftp_addr.ipMask,
                                                                     upg_mgr->tftp_addr.priority, upg_mgr->tftp_addr.vlan);
}

void oam_kt_upg_dl_state_set(oam_uint8 state)
{
    oam_kt_upg_mgr_ctx_t *upg_mgr = oam_kt_upg_mgr_ctx_gl_get();

    upg_mgr->dl_state = state;
}
/**********************************************************************/
/********* OAM configuration for image upgrade via TFTP ***************/
/**********************************************************************/
oam_uint32 oam_onu_upg_tftp_addr_set(
    oam_kt_upg_tftp_addr_ctx_t *addr_ctx)
{
    oam_status                 retCode    = OAM_E_OK;
    oam_kt_upg_mgr_ctx_t       *upg_mgr   = oam_kt_upg_mgr_ctx_gl_get();

    oam_kt_upg_tftpd_addr_ctx_construct(addr_ctx);
#ifdef TFTP_VLAN
    oam_kt_adapt_dev_vlan_del(upg_mgr->dev_ctx.vlanDev);
    oam_kt_upg_vlan_dev_construct();
#endif
    oam_kt_upg_dl_state_set(OAM_KT_UPG_DL_DLING_S);
    oam_img_remove_adapt();
#ifdef TFTP_VLAN
    oam_kt_adapt_vlan_port_del(upg_mgr->tftp_addr.vlan);
    oam_kt_adapt_vlan_port_set(upg_mgr->tftp_addr.vlan);

    retCode = oam_kt_adapt_dev_vlan_add(upg_mgr->dev_ctx.dfltDev, upg_mgr->tftp_addr.vlan);
    if(OAM_E_OK != retCode)
    {
        OAM_ORG_LOG_MAJ("dev vlan set error\r\n");
        goto out;
    }

    retCode = oam_kt_adapt_set_ipAddr(upg_mgr->dev_ctx.vlanDev, upg_mgr->tftp_addr.ipAddr);
    if(OAM_E_OK != retCode)
    {
        OAM_ORG_LOG_MAJ("dev ip addr set error\r\n");
        oam_kt_adapt_dev_vlan_del(upg_mgr->dev_ctx.vlanDev);
        goto out;
    }

    retCode = oam_kt_adapt_set_ipMask(upg_mgr->dev_ctx.vlanDev, upg_mgr->tftp_addr.ipMask);
    if(OAM_E_OK != retCode)
    {
        OAM_ORG_LOG_MAJ("dev ip mask set error\r\n");
        oam_kt_adapt_dev_vlan_del(upg_mgr->dev_ctx.vlanDev);
        goto out;
    }

    retCode = oam_kt_adapt_set_dev_up(upg_mgr->dev_ctx.vlanDev);
    if(OAM_E_OK != retCode)
    {
        OAM_ORG_LOG_MAJ("set dev up error\r\n");
        oam_kt_adapt_dev_vlan_del(upg_mgr->dev_ctx.vlanDev);
        goto out;
    }
#else
    retCode = oam_kt_adapt_set_ipAddr(upg_mgr->dev_ctx.dfltDev, upg_mgr->tftp_addr.ipAddr);
    if(OAM_E_OK != retCode)
    {
        OAM_ORG_LOG_MAJ("dev ip addr set error\r\n");
        goto out;
    }

    retCode = oam_kt_adapt_set_ipMask(upg_mgr->dev_ctx.dfltDev, upg_mgr->tftp_addr.ipMask);
    if(OAM_E_OK != retCode)
    {
        OAM_ORG_LOG_MAJ("dev ip mask set error\r\n");
        goto out;
    }
#endif
    oam_kt_config_shaper();
#if 0
    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_BOARD_INFO_BIT;
    scfg_tlv.len  = sizeof(ca_uint32_t);
    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if(CA_E_OK == ret)
    {
        memcpy(&board_info, scfg_tlv.value, sizeof(ca_uint32_t));
        if (board_info & BOARD_INFO_BIT_3)
        {
            CA_GPIO_SET_VALUE(CA_GPIO_FW_UPG_IND,1);
        }
    }
#endif
    retCode = tk_oam_onu_upg_tftpd_start();

out:
    return retCode;
    
}
oam_uint32 oam_onu_upg_tftp_auto_reboot_set(
        oam_kt_upg_auto_reboot_e auto_reboot)
{
    oam_kt_upg_mgr_ctx_t       *upg_mgr   = oam_kt_upg_mgr_ctx_gl_get();
    
    OAM_ORG_LOG("enter auto reboot set\r\n");

    upg_mgr->auto_reboot = auto_reboot;

    OAM_ORG_LOG("auto reboot %d\r\n", upg_mgr->auto_reboot);

    return OAM_E_OK;
}

oam_uint32 oam_onu_upg_tftp_dl_state_get(
	oam_uint8  *dl_state)
{
    oam_kt_upg_mgr_ctx_t *upg_mgr   = oam_kt_upg_mgr_ctx_gl_get();

    OAM_ORG_LOG("#####enter dl state get\r\n");

    *dl_state = upg_mgr->dl_state;

    return OAM_E_OK;
}
/**********************************************************************/
/********* END OAM configuration for image upgrade via TFTP ***********/
/**********************************************************************/

oam_uint8 *oam_kt_upg_dflt_dev_name_get(void)
{
    return OAM_KT_UPG_DFLT_DEV_NAME;
}

void tk_oam_onu_upg_init(void)
{
    oam_kt_upg_mgr_ctx_t *upg_mgr   = oam_kt_upg_mgr_ctx_gl_get();

    upg_mgr->dl_state = OAM_KT_UPG_DL_DLING_S;

    memcpy(upg_mgr->dev_ctx.dfltDev, oam_kt_upg_dflt_dev_name_get(), KT_MAX_IFNAME_LEN);
    TFTPD_THREAD_START_CB_REGISTER(oam_kt_upg_tftpd_thread_start_msg_send);
    TFTPD_THREAD_STOP_CB_REGISTER(oam_kt_upg_tftpd_thread_stop_msg_send);
    TFTPD_RECV_MSG_ERR_CB_REGISTER(oam_kt_upg_tftpd_recv_err_msg_send);
    TFTPD_RECV_MSG_DONE_CB_REGISTER(oam_kt_upg_tftpd_recv_done_msg_send);
}

#endif/*HAVE_KT_OAM*/
