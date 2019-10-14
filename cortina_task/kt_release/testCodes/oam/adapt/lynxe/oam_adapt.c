#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oam_types.h"
#include "oam_adapt.h"
#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_ctc_client.h"
#include "event_type.h"
#include "event.h"
#include "cpu_port.h"
#include "special_packet.h"
#if 0
#include "sdl_ext_port.h"
#include "sdl_ext_ctrl.h"
#include "sal_epon.h"
#endif
#include "stats_api.h"
#include "scfg.h"
//#include "led.h"

oam_boolean oam_d2_compatible_get_adapt()
{
    return FALSE;
}

oam_uint8  oam_dying_gasp_num_get_adapt()
{
    /*default dying gasp num is 1*/
    return OAM_DYING_GASP_NUM;
}

oam_status oam_dying_gasp_send_adapt(oam_pdu_info_t *pdu, oam_uint32 len)
{
    return OAM_E_OK;
}

oam_uint16 oam_switch_port_num_get_adapt()
{
    return OAM_DEFAULT_PORT_NUM;
}

oam_boolean oam_is_image_updating_adapt()
{
    return 0;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_llid_mac_get_adapt                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_llid_mac_get_adapt(
    oam_llid_t llid,
    oam_uint8 *mac)
/*                                                                           */
/* INPUTS  : o llid - onu llid                                               */
/*           o mac - llid mac address                                        */
/* OUTPUTS : SDL status                                                      */
/* DESCRIPTION:                                                              */
/* get the mac address for the llid                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 onu_mac[OAM_MACADDR_LEN] = {0x00, 0x13, 0x25, 0x00, 0x00, 0x01};
    
    OAM_ASSERT_RET(mac != NULL, OAM_E_PARAM);
    
    scfg_read(0, CFG_ID_MAC_ADDRESS, OAM_MACADDR_LEN, onu_mac);
    memcpy(mac, onu_mac, OAM_MACADDR_LEN);

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_port_lpbk_set_adapt                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_port_lpbk_set_adapt(
    oam_llid_t llid,
    oam_uint8 mux_action,
    oam_uint8 par_action)
/*                                                                           */
/* INPUTS  : o llid - onu llid                                               */
/*           o mux_action - mux action                                       */
/*           o par_action - par action                                       */
/* OUTPUTS : SDL status                                                      */
/* DESCRIPTION:                                                              */
/* set the loopback to the sdl                                               */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
#if 0
    static cs_boolean g_oam_loopback = FALSE;
    cs_sal_port_loopback_t loopback_mode;
    cs_status_t ret = CS_E_OK;

    if ((par_action == OAM_PAR_ACTION_LB) &&
        (mux_action == OAM_MUX_ACTION_DISCARD)) {
        loopback_mode = SAL_PORT_LOOPBACK_TX2RX;

        if (g_oam_loopback == TRUE) {
            return OAM_E_OK;
        }
        else {
            g_oam_loopback = TRUE;
        }
    }

    if ((par_action == OAM_PAR_ACTION_FWD) &&
        (mux_action == OAM_MUX_ACTION_FWD)) {
        loopback_mode = SAL_PORT_LOOPBACK_NONE;

        if (g_oam_loopback == FALSE) {
            return OAM_E_OK;
        }
        else {
            g_oam_loopback = FALSE;
        }
    }

    ret = cs_sal_epon_onu_pon_loopback_set(0, -1, g_oam_loopback);

    if (ret != CS_SUCCESS) {
        return OAM_E_ERROR;
    }

    if(loopback_mode != SAL_PORT_LOOPBACK_NONE) {
        /* pon led -> ora on */
        led_send_message(0, LED_MSG_UPDATE, LED_EVENT_DIAG_EN);
    }
    else {
        led_send_message(0, LED_MSG_UPDATE, LED_EVENT_DIAG_DIS);
    }

#endif
    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_buf_int_status_get_adapt                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_buf_int_status_get_adapt(oam_boolean *status)
/*                                                                           */
/* INPUTS  : o status - onu buf int status                                   */
/* OUTPUTS : status                                                          */
/* DESCRIPTION:                                                              */
/* get the bm status                                                         */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    //TODO:Geesun remove the MACRO after the SDL complete

    return OAM_E_OK;
}

oam_status oam_pon_stats_get_adapt(oam_pon_stats_t *stats)
{
    stats_pon_info_t pon_stats;

    OAM_ASSERT_RET(stats != NULL, OAM_E_PARAM);

    memset(&pon_stats, 0, sizeof(pon_stats));

    cs_stats_pon_get(&pon_stats);

    memcpy(stats, &pon_stats, sizeof(pon_stats));

    return OAM_E_OK;
}

oam_status oam_pon_stats_summary_get_adapt(oam_uint64 *frames,
        oam_uint64 *errors, oam_uint64 *err_symb)
{
    OAM_ASSERT_RET((frames != NULL
                    && errors != NULL
                    && err_symb != NULL), OAM_E_PARAM);

    cs_stats_pon_sum_get(frames, errors, err_symb);

    return OAM_E_OK;
}

void oam_evt_handler(const sdl_event_info_s *pinfo)
{
    oam_onu_reg_evt_t reg_evt;

    OAM_ASSERT(pinfo != NULL);

    switch (pinfo->event_type) {
        case CS_EVENT_EPON_REG_STAT_CHNG:
            memset(&reg_evt, 0, sizeof(reg_evt));

            reg_evt.llid = pinfo->event_data.llid_data.assigned_llid;
            reg_evt.status = pinfo->event_data.llid_data.status ? OAM_TRUE : OAM_FALSE;

            OAM_LOG_INFO("llid: 0x%x\n", pinfo->event_data.llid_data.assigned_llid);
            oam_llid_reg_evt_msg_send(reg_evt.llid, &reg_evt);
            break;

        default:
            OAM_LOG_INFO("unknow tpye %d \n", pinfo->event_type);
            break;
    }
}

cs_uint32_t oam_frame_handler(cs_dev_id_t device_id, cs_pkt_t *pkt, void *cookie)
{
    OAM_ASSERT_RET(pkt != NULL, OAM_E_PARAM);

    oam_pkt_msg_send(pkt->src_port_id, pkt->src_sub_port_id, pkt->pkt_data->data,
                     pkt->pkt_len);

    if(pkt->pkt_data->done_callback_function){
        pkt->pkt_data->done_callback_function(pkt->pkt_data->cookie);
    }

    return OAM_E_OK;
}

oam_status oam_evt_reg_adapt()
{
    cs_event_cfg_s   evt_cfg;
    cs_status_t       status;

    memset(&evt_cfg, 0, sizeof(cs_event_cfg_s));
    evt_cfg.callback   = oam_evt_handler;
    evt_cfg.user_data  = 0;
    evt_cfg.context_id = 1;
    evt_cfg.priority   = 1;
    evt_cfg.context    = SDL_EVENT_CONTEXT_USER;
    evt_cfg.port_type_bitmap = CS_PORT_TYPE_EPON;
    evt_cfg.port_num =  (CS_PORT_TYPE_EPON << 8);
    status = cs_event_register(0, CS_EVENT_EPON_REG_STAT_CHNG, &evt_cfg);

    if (CS_E_OK != status) {
        OAM_LOG_INFO("Error1: Reg state Event reg failed. ret=0x%x\n", status);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status oam_pkt_trap_adapt()
{
    cs_status_t ret = CS_E_OK;
    
    ret = cs_special_packet_set(0, CS_PON_PORT_ID, CS_RX_PKT_TYPE_OAM,TRUE, (CS_PORT_TYPE_CPU << 8), 7, 0);
    if (CS_E_OK != ret) {
        OAM_LOG_INFO("cs_special_packet_set failed. ret=0x%x\n", ret);
        return OAM_E_ERROR;
    }
    
    cs_rx_register(OAM_DEV_ID, oam_frame_handler, 0, NULL, CS_RX_PKT_TYPE_OAM);

    return OAM_E_OK;
}

oam_status oam_pkt_send_adapt(
    oam_port_id_t port,
    oam_llid_t llid,
    oam_uint8 *buf,
    oam_uint32 len)
{
    cs_pkt_t  pkt;
    cs_pkt_blk_t  pkt_data;
    cs_status_t   ret = CS_E_OK;
    
    OAM_ASSERT_RET(buf != NULL, OAM_E_PARAM);

    memset(&pkt, 0, sizeof(pkt));
    memset(&pkt_data, 0, sizeof(pkt_data));

  if (len < OAM_MIN_OAMPDU_SIZE)
      len = OAM_MIN_OAMPDU_SIZE;

    pkt_data.len = len;
    pkt_data.data = buf;
    pkt.pkt_data = &pkt_data;
    pkt.blk_count = 1;
    pkt.src_port_id = (CS_PORT_TYPE_CPU << 8);
    pkt.src_sub_port_id = llid;
    pkt.dst_port_id = (CS_PORT_TYPE_EPON << 8);
    pkt.pkt_len = len;
    pkt.cos = 7;
    
    ret = cs_tx(OAM_DEV_ID, &pkt);
    if(CS_E_OK != ret){
        OAM_LOG_INFO("oam packet tx failed.\n");
    }
    
    return OAM_E_OK;
}

oam_ext_mode_t oam_ext_mode_get_adapt()
{
    return OAM_EXT_MODE_CTC;
}

oam_status oam_pon_led_set_adapt(oam_uint8 action)
{
    oam_boolean status = FALSE;

    status = (action == 0) ? FALSE : TRUE;

    led_send_oam_message(status);

    return OAM_E_OK;
}

oam_status oam_llid_admin_set_adapt(oam_uint16 llid, oam_boolean enable)
{
#if 0
    cs_status_t       ret;

    ret = cs_sal_epon_onu_mpcp_offline_set(0, llid, !enable);

    if (!ret)
        return OAM_E_ERROR;
    else
        return OAM_E_OK;
#endif
    return OAM_E_OK;
}

oam_status oam_pon_admin_set_adapt(
    oam_port_id_t port,
    oam_boolean enable)
{
    return OAM_E_OK;
}


oam_status oam_dying_gasp_init_adapt(oam_if_t *intf)
{
    oam_uint8    pdu[OAM_MIN_OAMPDU_SIZE];
    oam_uint32 len = 0;

    if (intf == NULL)
        return;

    intf->local_dying_gasp = TRUE;
    intf->local_critical_event = TRUE;

    memset(pdu, 0x00, OAM_MIN_OAMPDU_SIZE);

    len = oam_build_pdu_info(intf, NULL, (oam_pdu_info_t *)pdu);
    cs_epon_dying_gasp_set(0, 0, pdu);

    OAM_LOG_DEBUG("Build Dying Gasp packet.\n");
    /*oam_buf_print(pdu, 64);*/

    intf->local_dying_gasp = FALSE;
    intf->local_critical_event = FALSE;
}


