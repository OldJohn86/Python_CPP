#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "ca_event.h"
//#include "event_type.h"
//#include "epon.h"
//#include "gpon.h"
#include "pkt_tx.h"
#include "rx.h"

#include "rx.h"
#include "scfg.h"
#include "classifier.h"
#include "vlan.h"
#include "port.h"
#include "epon.h"
#include "queue.h"

#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
#include "classifier.h"
#include "eth_port.h"
#include "l2.h"
#include "mcast.h"
#include "mirror.h"
#include "nat.h"
#include "port.h"
#include "pkt_tx.h"

#include "qos.h"
#include "queue.h"
#include "rate.h"
#include "route.h"
#include "special_packet.h"
#include "sys.h"
#include "tunnel.h"
#include "vlan.h"
#endif

#include "oam_types.h"
#include "oam_adapt.h"
#include "oam_cmn.h"
#include "oam_main.h"

#ifdef HAVE_CTC_OAM
#include "oam_ctc_client.h"
#endif

#ifdef HAVE_LOOP_DETECT
#include "loop_detect.h"
#endif

oam_uint8 oam_olt_type[CFG_ID_PON_OLT_TYPE_LEN];
oam_ext_mode_t      oam_ext_mode       = OAM_EXT_MODE_DPOE;
static char*        g_oam_ext_mode_str[] = { "CTC", "DPOE", "KT", "NOT SUPPORTED" };

oam_onu_port_info_t oam_onu_port_info;

/* load olt type from startup config file */
oam_status oam_olt_type_init_adapt()
{
    ca_status_t ret = CA_E_OK;
    ca_device_config_tlv_t scfg_tlv;
    
    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_PON_OLT_TYPE;
    scfg_tlv.len = CFG_ID_PON_OLT_TYPE_LEN;

    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if ( CA_E_OK != ret )
    {
        OAM_LOG_DEBUG("read CFG_ID_PON_OLT_TYPE_LEN fail. ret = %d \r\n", ret);
        return OAM_E_ERROR;
    }
    memcpy(oam_olt_type,scfg_tlv.value,CFG_ID_PON_OLT_TYPE_LEN);
    return OAM_E_OK;
}

oam_status oam_olt_type_get_adapt(oam_uint8 *type, oam_uint32 *len)
{
    memcpy(type, oam_olt_type,CFG_ID_PON_OLT_TYPE_LEN);
    *len = CFG_ID_PON_OLT_TYPE_LEN;
    return OAM_E_OK;
}

/* load ONU port info (user port id, ge, xge bitmap) from startup config file */
oam_status oam_onu_port_info_init_adapt()
{
    ca_status_t ret = CA_E_OK;
    oam_uint32  i = 0;
    oam_uint32 uni_num = 0;
    ca_device_config_tlv_t scfg_tlv;
    oam_uint8 user_port_id[CFG_ID_USER_PORT_ID_LEN];
    oam_boolean is_ubq = FALSE;

    memset(&oam_onu_port_info, 0, sizeof(oam_onu_port_info_t));

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    memset(user_port_id,0,CFG_ID_USER_PORT_ID_LEN);

    scfg_tlv.type = CA_CFG_ID_USER_PORT_ID;
    scfg_tlv.len = CFG_ID_USER_PORT_ID_LEN;

    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if ( CA_E_OK != ret )
    {
        OAM_LOG_DEBUG("read CA_CFG_ID_USER_PORT_ID fail. ret = %d \r\n", ret);
        return OAM_E_ERROR;
    }
    memcpy(user_port_id,scfg_tlv.value,CFG_ID_USER_PORT_ID_LEN);

    oam_ext_mode = oam_ext_mode_get_adapt();
    if ( oam_ext_mode >= OAM_EXT_MODE_MAX )
    {
        OAM_LOG_DEBUG (
            "Error!!! OAM ext mode %u is NOT SUPPORTED\r\n" ,
            oam_ext_mode );
        return OAM_E_ERROR;
    }

    ca_printf (
        "OAM ext mode is %s\r\n" ,
        g_oam_ext_mode_str[oam_ext_mode] );
    oam_ca_ge_bmp_get_adapt ( &oam_onu_port_info.ca_ge_bmp );
    oam_ca_xge_bmp_get_adapt ( &oam_onu_port_info.ca_xge_bmp );
    for ( i = 0; i < CFG_ID_USER_PORT_ID_LEN; i ++ )
    {
        if ( 0xFF != user_port_id[i] )
        {
            if ( oam_ext_mode == OAM_EXT_MODE_DPOE ) /* for dpoe oam, user uni port id is start from 0 */
            {
                if(user_port_id[i] >= OAM_UNI_PORT_NUM_MAX)
                    OAM_ERROR("It is DPOE OAM MDOE, user_uni_port_id range is 0~3\r\n");

            }
            else if ( oam_ext_mode == OAM_EXT_MODE_KT )
            {
                /* for kt  oam, user uni port id is the same as oam_port_id_t */
                if ( user_port_id[i] < OAM_UNI_PORT_ID1 ||
                     user_port_id[i] >= (OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM_MAX) )
                {
                    OAM_ERROR (
                        "It is %s OAM MDOE, user_uni_port_id range is 1~4\r\n" ,
                        g_oam_ext_mode_str[oam_ext_mode] );
                }
            }
            else if ( oam_ext_mode == OAM_EXT_MODE_CTC )
            {
                //
                // ctc oam, shift up user_port_id with 1, because port 0 is pon port.
                //
                user_port_id [ i ] += 1;
            }

            oam_onu_port_info.user_port_id[uni_num] = user_port_id[i];
            oam_onu_port_info.ca_port_id[uni_num] = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, i);
            oam_onu_port_info.oam_uni_bmp |= ( 1 << user_port_id[i]);
            uni_num++;
        }
    }

    for ( i = uni_num; i < OAM_UNI_PORT_NUM_MAX; i ++ )
    {
        oam_onu_port_info.ca_port_id[i] = CA_PORT_ID(CA_PORT_TYPE_INVALID, 0);
        oam_onu_port_info.user_port_id[i] = 0xFF;
    }

    is_ubq = oam_dpoe_ubq_check();

    if(is_ubq)
    {
        ca_printf("The flag for ubq is TRUE\r\n");
        oam_onu_port_info.ca_port_id[2] = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x12);
        oam_onu_port_info.ca_port_id[3] = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x13);
        oam_onu_port_info.user_port_id[2] = 0x2;
        oam_onu_port_info.user_port_id[3] = 0x3;
        uni_num = uni_num+2;
    }

    oam_onu_port_info.uni_num = uni_num;

    for ( i = 0; i < oam_onu_port_info.uni_num; i ++ )
    {
        ca_printf (
            "OAM UNI %d -> user_port_id: 0x%08x, ca_port_id: 0x%08x\r\n" ,
            i ,
            oam_onu_port_info.user_port_id[i] ,
            oam_onu_port_info.ca_port_id[i] );
    }
    return OAM_E_OK;
}

oam_status oam_onu_info_init_adapt()
{
    ca_status_t ret = CA_E_OK;
    oam_uint32  i = 0;
    oam_boolean is_ubq = FALSE;
    ca_qos_config_t qos_config;
    ca_vlan_port_control_t vlan_port_ctl;

    oam_olt_type_init_adapt();

    oam_onu_port_info_init_adapt();
    
    /* please move following init to xxx_init() of each OAM mode */
    memset(&qos_config,0,sizeof(ca_qos_config_t));
    memset(&vlan_port_ctl,0,sizeof(ca_vlan_port_control_t));
    
    is_ubq = oam_dpoe_ubq_check();
    if ( is_ubq )
    {
        for ( i = 0; i < oam_onu_port_info.uni_num; i ++ )
        {
            ca_qos_config_get ( 0 , oam_onu_port_info.ca_port_id[i] , &qos_config );
            qos_config.qos_map_mode = CA_QOS_MAP_CLASSIFIER;
            ca_qos_config_set ( 0 , oam_onu_port_info.ca_port_id[i] , &qos_config );
        }
    }
    //fix dup arp/ip issue ,bug#50810 bug#50870
    if ( oam_ext_mode == OAM_EXT_MODE_DPOE )
    {
        ca_l2_vlan_port_control_get ( 0 , CA_PORT_ID ( CA_PORT_TYPE_EPON , 0x07 ) , &vlan_port_ctl );
        vlan_port_ctl.ingress_membership_check_enable = 0;
        vlan_port_ctl.egress_membership_check_enable = 0;
        ca_l2_vlan_port_control_set(0,CA_PORT_ID(CA_PORT_TYPE_EPON,0x07),&vlan_port_ctl);

        if ( is_ubq )
        {
            memset ( &vlan_port_ctl , 0 , sizeof(ca_vlan_port_control_t) );
            ca_l2_vlan_port_control_get ( 0 , CA_PORT_ID ( CA_PORT_TYPE_CPU , 0x12 ) , &vlan_port_ctl );
            vlan_port_ctl.ingress_membership_check_enable = 0;
            vlan_port_ctl.egress_membership_check_enable = 0;
            ca_l2_vlan_port_control_set(0,CA_PORT_ID(CA_PORT_TYPE_CPU,0x12),&vlan_port_ctl);

            memset(&vlan_port_ctl,0,sizeof(ca_vlan_port_control_t));
            ca_l2_vlan_port_control_get(0,CA_PORT_ID(CA_PORT_TYPE_CPU,0x13),&vlan_port_ctl);
            vlan_port_ctl.ingress_membership_check_enable = 0;
            vlan_port_ctl.egress_membership_check_enable = 0;
            ca_l2_vlan_port_control_set(0,CA_PORT_ID(CA_PORT_TYPE_CPU,0x13),&vlan_port_ctl);
        }
    }
    else if (oam_ext_mode == OAM_EXT_MODE_CTC)
    {
        ca_l2_vlan_port_control_get(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x03),&vlan_port_ctl);
        vlan_port_ctl.ingress_membership_check_enable = 1 ;
        vlan_port_ctl.egress_membership_check_enable  = 1;
        vlan_port_ctl.ingress_vlan_action_enable      = 1; 
        ca_l2_vlan_port_control_set(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x03),&vlan_port_ctl);

        ca_l2_vlan_port_control_get(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x04),&vlan_port_ctl);
        vlan_port_ctl.ingress_membership_check_enable = 1 ;
        vlan_port_ctl.egress_membership_check_enable  = 1;
        vlan_port_ctl.ingress_vlan_action_enable      = 1; 
        ca_l2_vlan_port_control_set(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x04),&vlan_port_ctl);

    }

    return OAM_E_OK;
}

void oam_onu_port_info_dump (void)
{
    oam_uint32 i = 0;

    if ( oam_ext_mode == OAM_EXT_MODE_DPOE )
    {
        ca_printf ( "OAM ext mode is DPOE\r\n" );
    }
    else if ( oam_ext_mode == OAM_EXT_MODE_CTC )
    {
        ca_printf ( "OAM ext mode is CTC\r\n" );
    }
    else if ( oam_ext_mode == OAM_EXT_MODE_KT )
    {
        ca_printf ( "OAM ext mode is KT\r\n" );
    }
    else
    {
        ca_printf ( "OAM ext mode is not supported\r\n" );
    }

    ca_printf ( "oam uni port num is %d\r\n" , oam_onu_port_info.uni_num );
    for ( i = 0; i < OAM_UNI_PORT_NUM_MAX; i ++ )
    {
        ca_printf ( "OAM UNI %d -> user_port_id: 0x%08x, ca_port_id: 0x%08x\r\n" , i , oam_onu_port_info.user_port_id[i] , oam_onu_port_info.ca_port_id[i] );
    }
}

/* oam port id is defined in oam_types.h
   ca_port_id_t is defined in ca_types.h */
ca_port_id_t oam_port_to_ca_port_adapt(oam_port_id_t port)
{
    oam_uint32 i = 0;
    oam_uint16 user_port = 0;
    ca_port_id_t ca_port = CA_PORT_ID(CA_PORT_TYPE_INVALID, 0);

    if (port == OAM_PON_PORT_ID)
    {
        ca_port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7);

    }
    else if (port >= OAM_UNI_PORT_ID1 && port <= OAM_UNI_PORT_ID4)
    {
        if (oam_ext_mode == OAM_EXT_MODE_DPOE)
        {
            user_port = port-1; /* for dpoe oam, user uni port id is start from 0 */
        }
        else if (oam_ext_mode == OAM_EXT_MODE_KT ||
                 oam_ext_mode == OAM_EXT_MODE_CTC)
        {
            user_port = port;   /* for kt oam, user uni port id is the same as oam_port_id_t */
        }

        for (i = 0; i < OAM_UNI_PORT_NUM_MAX; i++)
        {
            if (user_port == oam_onu_port_info.user_port_id[i])
            {
                ca_port = oam_onu_port_info.ca_port_id[i];
                break;
            }
        }
    }
    else if (port == OAM_CPU0_PORT_ID)
    {
        ca_port = CA_PORT_ID(CA_PORT_TYPE_CPU, CA_PORT_ID_CPU0);

    }
    else if (port == OAM_CPU1_PORT_ID)
    {
        ca_port = CA_PORT_ID(CA_PORT_TYPE_CPU, CA_PORT_ID_CPU1);

    }

    return ca_port;
}

oam_port_id_t oam_ca_port_to_oam_port_adapt(ca_port_id_t port)
{
    oam_uint32 i = 0;
    oam_port_id_t oam_port = OAM_PON_PORT_ID;
    ca_uint16 port_id = PORT_ID(port);

    if (port_id == CA_PORT_ID_NI7)
    {
        oam_port = OAM_PON_PORT_ID;

    }
    else if (port_id >= CA_PORT_ID_NI3 && port_id <= CA_PORT_ID_NI6)
    {
        for (i = 0; i < OAM_UNI_PORT_NUM_MAX; i++)
        {
            if (port == oam_onu_port_info.ca_port_id[i])
            {
                if (oam_ext_mode == OAM_EXT_MODE_DPOE)
                {
                    oam_port = oam_onu_port_info.user_port_id[i]+1;
                }
                else if (oam_ext_mode == OAM_EXT_MODE_KT ||
                         oam_ext_mode == OAM_EXT_MODE_CTC)
                {
                    oam_port = oam_onu_port_info.user_port_id[i];
                }
                break;
            }
        }
    }
    return oam_port;
}

oam_uint16 oam_uni_port_num_get_adapt()
{
    return oam_onu_port_info.uni_num;
}

oam_status
oam_uni_bmp_get_adapt (
    oam_uint32* oam_uni_bmp)
{
    oam_status ret = OAM_E_ERROR;

    if ( !oam_uni_bmp )
    {
        ret = OAM_E_PARAM;
        goto ERROR_OAM_UNI_BMP_GET_ADAPT;
    }

    *oam_uni_bmp = oam_onu_port_info.oam_uni_bmp;

ERROR_OAM_UNI_BMP_GET_ADAPT:
    return ret;
}

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
    ca_status_t ret = 0;
    ca_uint32_t i = 0;
    oam_uint8 onu_mac[OAM_MACADDR_LEN] = {0x00, 0x13, 0x25, 0x00, 0x00, 0x01};
    ca_device_config_tlv_t dev_cfg_tlv;
    OAM_ASSERT_RET(mac != NULL, OAM_E_PARAM);

    memset(&dev_cfg_tlv, 0, sizeof(ca_device_config_tlv_t));
    dev_cfg_tlv.type = CA_CFG_ID_MAC_ADDRESS;
    dev_cfg_tlv.len = OAM_MACADDR_LEN;
    ret = ca_device_config_tlv_get(0, &dev_cfg_tlv);
    if(ret == CA_E_OK && dev_cfg_tlv.len == OAM_MACADDR_LEN){
        for(i = 0; i < dev_cfg_tlv.len; i++) {
            onu_mac[i] = dev_cfg_tlv.value[i];
        }
        //printf("oam_llid_mac_get_adapt SCFG: MAC is 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n\r",*onu_mac,*(onu_mac+1),*(onu_mac+2),*(onu_mac+3),*(onu_mac+4),*(onu_mac+5));
    }
    onu_mac[5] += llid;
    memcpy(mac, onu_mac, OAM_MACADDR_LEN);

    //printf("oam_llid_mac_get_adapt: MAC is 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n\r",*mac,*(mac+1),*(mac+2),*(mac+3),*(mac+4),*(mac+5));
    
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
    static oam_boolean g_oam_loopback = FALSE;
    ca_port_id_t pon_port = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
    //ca_port_loopback_mode_t loopback;
    //cs_sal_port_loopback_t loopback_mode;
    ca_status_t ret = CA_E_OK;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    static ca_uint32_t index = 0;
    ca_uint8_t upstream_use_dq = 0;
    ca_port_id_t pon_port_pon = 0;
    ca_device_config_tlv_t scfg_tlv;

    memset(&key, 0, sizeof(ca_classifier_key_t));
    memset(&key_mask, 0, sizeof(ca_classifier_key_mask_t));
    memset(&action, 0, sizeof(ca_classifier_action_t));

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CFG_ID_PON_UPSTREAM_USE_DEEP_QUEUE;
    scfg_tlv.len  = sizeof(ca_uint8_t);
    ret = ca_device_config_tlv_get(0, &scfg_tlv);

    if (CA_E_OK == ret){
        memcpy(&upstream_use_dq, scfg_tlv.value, sizeof(ca_uint8_t));
    }

    if (upstream_use_dq == 2)
        pon_port_pon = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
    else
        pon_port_pon = CA_PORT_ID(CA_PORT_TYPE_EPON, 0x20);

    if ((par_action == OAM_PAR_ACTION_LB) &&
        (mux_action == OAM_MUX_ACTION_DISCARD)) {
        //loopback = CA_PORT_LOOPBACK_MAC_REMOTE;
        if (g_oam_loopback == TRUE) {
            return OAM_E_OK;
        }
        else {
            key.src_port = pon_port;
            key.key_handle.llid_cos_index = 0;
            key_mask.src_port = 1;
            key_mask.key_handle = 1;
            action.forward = CA_CLASSIFIER_FORWARD_PORT;
            action.dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
            action.options.masks.action_handle = 1;
            action.options.action_handle.flow_id = 0;
            ret = ca_classifier_rule_add(0,7, &key, &key_mask, &action, &index);
            if (ret != CA_E_OK)
            {
                cap_log_msg(0, CA_MOD_ALARM_APP, CAP_LOG_ERROR,0, "CLS to add loop back rule failed, ret = %d\n", ret);
            }
            g_oam_loopback = TRUE;
            ret = ca_port_stp_state_set(0, pon_port_pon, CA_PORT_STP_FWD_NO_LEARN);
        }
    }

    if ((par_action == OAM_PAR_ACTION_FWD) &&
        (mux_action == OAM_MUX_ACTION_FWD)) {
        //loopback = CA_PORT_LOOPBACK_NONE;

        if (g_oam_loopback == FALSE) {
            return OAM_E_OK;
        }
        else {
            g_oam_loopback = FALSE;
            ca_classifier_rule_delete(0, index);
            ret = ca_port_stp_state_set(0, pon_port_pon, CA_PORT_STP_FORWARDING);
        }
    }

    if (ret != CA_E_OK) {
        return OAM_E_ERROR;
    }

#if 0
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
oam_status oam_pon_stats_get_adapt(oam_boolean read_clear, oam_pon_stats_t *stats)
{
    oam_status ret = OAM_E_OK;
    ca_port_id_t pon_port;
    ca_epon_port_stats_t port_stats;
    ca_epon_mpcp_stats_t mpcp_stats;
    ca_epon_llid_stats_t llid_stats; /* currently only support 1 llid */
    ca_epon_port_fec_stats_t fec_stats;
    ca_policer_stats_t policer_stats;

    pon_port = OAM_PON_PORT_ID_TO_CA_PORT();
    memset(&port_stats, 0, sizeof(ca_epon_port_stats_t));
    ret = ca_epon_port_stats_get(0, pon_port, read_clear, &port_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_port_stat_get return failed, ret = %d \n",ret);
        //return ret;
    }

    memset(&mpcp_stats, 0, sizeof(ca_epon_mpcp_stats_t));
    ret = ca_epon_mpcp_stats_get(0, pon_port, 0, read_clear, &mpcp_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_mpcp_stat_get return failed, ret = %d \n",ret);
        //return ret;
    }

    memset(&llid_stats, 0, sizeof(ca_epon_llid_stats_t));
    ret = ca_epon_llid_stats_get(0, pon_port, 0, read_clear, &llid_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_llid_stats_get return failed, ret = %d \n",ret);
        //return ret;
    }

    /* in fact, fec stats not support read_keep */
    memset(&fec_stats, 0, sizeof(ca_epon_port_fec_stats_t));
    ret = ca_epon_port_fec_stats_get(0, pon_port, read_clear, &fec_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_port_fec_stats_get return failed, ret = %d \n",ret);
        //return ret;
    }

    memset(&policer_stats, 0, sizeof(ca_policer_stats_t));
    ca_port_policer_stats_get(0, pon_port, read_clear, &policer_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_port_fec_stats_get return failed, ret = %d \n",ret);
        //return ret;
    }

    /* EPON PORT/LINK RX */
    stats->pon_byte_cnt            =  port_stats.rx_good_octets;
    stats->pon_frame_cnt           =  port_stats.rx_uc_frames + port_stats.rx_mc_frames + port_stats.rx_bc_frames;
    stats->pon_ucframe_cnt         =  port_stats.rx_uc_frames;
    stats->pon_mcframe_cnt         =  port_stats.rx_mc_frames;
    stats->pon_bcframe_cnt         =  port_stats.rx_bc_frames;
    stats->pon_rmon64_cnt          =  port_stats.rx_64_frames;
    stats->pon_rmon65_127_cnt      =  port_stats.rx_65_127_frames;
    stats->pon_rmon128_255_cnt     =  port_stats.rx_128_255_frames;
    stats->pon_rmon256_511_cnt     =  port_stats.rx_256_511_frames;
    stats->pon_rmon512_1023_cnt    =  port_stats.rx_512_1023_frames;
    stats->pon_rmon1024_1518_cnt   =  port_stats.rx_1024_1518_frames;
    stats->pon_rx1519_maxcnt       =  port_stats.rx_1519_maxframes;
    stats->pon_undersizecnt        =  port_stats.rx_runt_frames;   
    stats->pon_crcerr_cnt          =  port_stats.rx_fcs_error_frames;
    //stats->pon_rx_fcs_err          =  port_stats.rx_fcs_error_frames;       
    stats->pon_rx_fcs_err          =  fec_stats.err_pkt;    /* just for KT BMT test */
    stats->pon_rx_green            = policer_stats.green_packets;

    /* EPON PORT/LINK TX */
    stats->pon_tx_byte_cnt         =  port_stats.tx_good_octets;
    stats->pon_txframe_cnt         =  port_stats.tx_uc_frames + port_stats.tx_mc_frames + port_stats.tx_bc_frames;
    stats->pon_txucframe_cnt       =  port_stats.tx_uc_frames;
    stats->pon_txmcframe_cnt       =  port_stats.tx_mc_frames;
    stats->pon_txbcframe_cnt       =  port_stats.tx_bc_frames;
    stats->pon_txrmon64_cnt        =  port_stats.tx_64_frames;
    stats->pon_txrmon65_127_cnt    =  port_stats.tx_65_127_frames;
    stats->pon_txrmon128_255_cnt   =  port_stats.tx_128_255_frames;
    stats->pon_txrmon256_511_cnt   =  port_stats.tx_256_511_frames;
    stats->pon_txrmon512_1023_cnt  =  port_stats.tx_512_1023_frames;
    stats->pon_txrmon1024_1518_cnt =  port_stats.tx_1024_1518_frames;
    stats->pon_tx1519_maxcnt       =  port_stats.tx_1519_maxframes;
    stats->pon_tx_green            =  policer_stats.green_packets;
    
    /* EPON LINK RX */
    stats->pon_mpcp_uc_gat_rev     =  mpcp_stats.rx_mpcp_gate;
    stats->pon_mpcp_bc_reg_reg_rev =  port_stats.mpcp_rx_bc_reg;
    
    /* EPON LINK RX */
    stats->pon_mpcp_tx_req_reg     =  mpcp_stats.tx_mpcp_reg_request;
    stats->pon_mpcp_tx_ack_ack     =  mpcp_stats.tx_mpcp_reg_ack;
    stats->pon_mpcp_tx_rpt         =  mpcp_stats.tx_mpcp_report;

    /* */
    stats->pon_ctrlframe_cnt       =  port_stats.mpcp_rx_mac_ctrl_frames;
    stats->pon_txctrlframe_cnt     =  port_stats.mpcp_tx_mac_ctrl_frames;
    stats->pon_pauseframe_cnt      =  port_stats.rx_pause_frames;
    stats->pon_unknownop_cnt       =  port_stats.rx_unknown_oframes;
    stats->pon_runt_cnt            =  port_stats.rx_runt_frames;

    stats->pon_mpcp_uc_rev_cnt     =  port_stats.rx_uc_frames;
    stats->pon_mpcp_uc_reg_ack     =  0;
    stats->pon_mpcp_uc_reg_nack    =  0;
    stats->pon_mpcp_uc_reg_dereg   =  0;
    stats->pon_mpcp_uc_reg_rereg   =  0;
    stats->pon_mpcp_uc_reg_reg_rev =  mpcp_stats.rx_mpcp_register;
    stats->pon_mpcp_uc_gat_norm    =  0;
    stats->pon_mpcp_uc_gat_frpt    =  0;
    stats->pon_mpcp_uc_gat_udis    =  0;
    stats->pon_mpcp_uc_gat_bdis    =  0;
    stats->pon_mpcp_mc_rev_cnt     =  port_stats.rx_mc_frames;
    stats->pon_mpcp_mc_reg_ack     =  0;
    stats->pon_mpcp_mc_reg_nack    =  0;
    stats->pon_mpcp_mc_reg_dereg   =  0;
    stats->pon_mpcp_mc_reg_rereg   =  0;
    stats->pon_mpcp_mc_reg_reg_rev =  0;
    stats->pon_mpcp_mc_gat_rev     =  0;
    stats->pon_mpcp_mc_gat_norm    =  0;
    stats->pon_mpcp_mc_gat_frpt    =  0;
    stats->pon_mpcp_mc_gat_udis    =  0;
    stats->pon_mpcp_mc_gat_bdis    =  0;
    stats->pon_mpcp_bc_rev_cnt     =  port_stats.rx_bc_frames;
    stats->pon_mpcp_bc_reg_ack     =  0;
    stats->pon_mpcp_bc_reg_nack    =  0;
    stats->pon_mpcp_bc_reg_dereg   =  0;
    stats->pon_mpcp_bc_reg_rereg   =  0;  
    stats->pon_mpcp_bc_gat_rev     =  port_stats.mpcp_rx_bc_gate;
    stats->pon_mpcp_bc_gat_norm    =  0;
    stats->pon_mpcp_bc_gat_frpt    =  0;
    stats->pon_mpcp_bc_gat_udis    =  0;
    stats->pon_mpcp_bc_gat_bdis    =  0;
    stats->pon_mpcp_rx_oam         =  llid_stats.rx_oam_frames;
    stats->pon_mpcp_tx_req_dereg   =  0;
    stats->pon_mpcp_tx_ack_nack    =  0;   
    stats->pon_mpcp_tx_oam         =  llid_stats.tx_oam_frames;
    stats->pon_mpcp_da_mc_nslf     =  0;
    stats->pon_mpcp_da_mc_slf      =  0;
    stats->pon_mpcp_da_uc          =  0;
    stats->pon_txoversizecnt       =  0;
    stats->pon_txpausecnt          =  port_stats.tx_pause_frames;
    stats->pon_oversize_cnt        =  port_stats.rx_oversize_frames;
    stats->pon_jabbercnt           =  port_stats.rx_1519_maxframes;
    stats->pon_extensioncnt        =  port_stats.rx_ext_frames;
    stats->pon_txextensioncnt      =  port_stats.tx_ext_frames;
    stats->pon_err_symbol          =  0;
    stats->pon_outofsync           =  0;
    stats->pon_sld_err             =  port_stats.rx_crc8_sld_err;
    stats->pon_crc8_err            =  port_stats.rx_crc8_sld_err;
    stats->pon_mac_drop            =  0;
    stats->pon_ipg_drop            =  0;
    stats->pon_drain               =  0;
    stats->pon_tx_crc_err          =  0;

    return ret;
}

oam_status oam_pon_stats_summary_get_adapt(oam_boolean read_clear, oam_uint64 *frames,
        oam_uint64 *errors, oam_uint64 *err_symb)
{
    OAM_ASSERT_RET((frames != NULL
                    && errors != NULL
                    && err_symb != NULL), OAM_E_PARAM);

    oam_status ret = OAM_E_OK;
    ca_port_id_t pon_port;
    ca_epon_port_stats_t port_stats;

    pon_port = OAM_PON_PORT_ID_TO_CA_PORT();
    memset(&port_stats, 0, sizeof(ca_epon_port_stats_t));
    ret = ca_epon_port_stats_get(0, pon_port, read_clear, &port_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_port_stat_get return failed, ret = %d \n",ret);
        return ret;
    }

    *frames = port_stats.rx_uc_frames + port_stats.rx_mc_frames + port_stats.rx_bc_frames;
    *errors = port_stats.rx_crc8_sld_err + port_stats.rx_fcs_error_frames;
    *err_symb = 0;

    return ret;
}

oam_status oam_uni_stats_get_adapt(oam_port_id_t port, oam_boolean read_clear, oam_uni_stats64_t *stats)
{
    oam_status ret = OAM_E_OK;
    ca_port_id_t ca_port; 
    ca_eth_port_stats_t uni_stats;
    ca_policer_stats_t policer_stats;

    /* TODO need to add support for OAM_DOWNLINK_PORT */
    if(port == OAM_DOWNLINK_PORT)
    {
        OAM_ERROR("OAM_ORG_LOG:port is downlink port. not supported yet\r\n");
        return OAM_E_OK;
    }

    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);

    memset(stats, 0, sizeof(oam_uni_stats64_t));
    /* 1. some stats (CRC error, ...) will be dropped by realtek switch, so need to get from switch port 
       2. some stats (rxstatsfrm64_cnt, rxstatsfrm65_127_cnt...), realtek switch not support per direction rx&tx.
       3. for both saturn port and switch port, crc error pkt will not be countered in rxucfrm_cnt..., but countered in 
       rxstatsfrm64_cnt... and rxbyte_cnt.
    */ 
    
    memset(&uni_stats, 0, sizeof(ca_eth_port_stats_t));
    ret = ca_eth_port_stats_get(0, ca_port, read_clear, &uni_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR(
            "%s: ca_eth_port_stats_get return failed, oam_port %u, ca_port = 0x%x ret = %d \n",
            __FUNCTION__,
            port,
            ca_port,
            ret);
        //return ret;
    }
    memset(&policer_stats, 0, sizeof(ca_policer_stats_t));
    ca_port_policer_stats_get(0, ca_port, read_clear, &policer_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR(
            "%s: ca_port_policer_stats_get return failed, oam_port %u, ca_port = 0x%x ret = %d \n",
            __FUNCTION__,
            port,
            ca_port,
            ret);
        //return ret;
    }
    /* OAM_ONU_UNI_RX */
    stats->rxbyte_cnt                = uni_stats.rx_good_octets;
    stats->rxfrm_cnt                 = uni_stats.rx_uc_frames + uni_stats.rx_mc_frames + uni_stats.rx_bc_frames;
    stats->rxucfrm_cnt               = uni_stats.rx_uc_frames;
    stats->rxmcfrm_cnt               = uni_stats.rx_mc_frames;
    stats->rxbcfrm_cnt               = uni_stats.rx_bc_frames;
    stats->rxstatsfrm64_cnt          = uni_stats.rx_64_frames;
    stats->rxstatsfrm65_127_cnt      = uni_stats.rx_65_127_frames;
    stats->rxstatsfrm128_255_cnt     = uni_stats.rx_128_255_frames;
    stats->rxstatsfrm256_511_cnt     = uni_stats.rx_256_511_frames;
    stats->rxstatsfrm512_1023_cnt    = uni_stats.rx_512_1023_frames;
    stats->rxstatsfrm1024_1518_cnt   = uni_stats.rx_1024_1518_frames;
    stats->rxstatsfrm1519_Max_cnt    = uni_stats.rx_1519_max_frames;
    stats->rxundersizefrm_cnt        = uni_stats.rx_undersize_frames;
    stats->rxruntfrm_cnt             = uni_stats.rx_runt_frames;
    stats->rxoversizefrm_cnt         = uni_stats.rx_oversize_frames;
    stats->rxjabberfrm_cnt           = uni_stats.rx_1519_max_frames;
    stats->rxpausefrm_cnt            = uni_stats.rx_pause_frames; 
    stats->alignment_errors          = uni_stats.phy_stats.alignmentErrors;
    stats->rxcrcerrfrm_cnt           = uni_stats.rx_fcs_error_frames; 
    stats->rxfrmgreen_cnt            = policer_stats.green_packets;
    stats->rxerror_cnt               = uni_stats.rx_error_frames;
    
    /* OAM_ONU_UNI_TX */
    stats->txbyte_cnt                = uni_stats.tx_good_octets;
    stats->txfrm_cnt                 = uni_stats.tx_uc_frames + uni_stats.tx_mc_frames + uni_stats.tx_bc_frames;
    stats->txucfrm_cnt               = uni_stats.tx_uc_frames;
    stats->txmcfrm_cnt               = uni_stats.tx_mc_frames;
    stats->txbcfrm_cnt               = uni_stats.tx_bc_frames;
    stats->txstatsfrm64_cnt          = uni_stats.tx_64_frames;
    stats->txstatsfrm65_127_cnt      = uni_stats.tx_65_127_frames;
    stats->txstatsfrm128_255_cnt     = uni_stats.tx_128_255_frames;
    stats->txstatsfrm256_511_cnt     = uni_stats.tx_256_511_frames;
    stats->txstatsfrm512_1023_cnt    = uni_stats.tx_512_1023_frames;
    stats->txstatsfrm1024_1518_cnt   = uni_stats.tx_1024_1518_frames;
    stats->txstatsfrm1519_Max_cnt    = uni_stats.tx_1519_max_frames;
    stats->txpausefrm_cnt            = uni_stats.tx_pause_frames;
    stats->txfrmgreen_cnt            = policer_stats.green_packets;
    stats->txerror_cnt               = uni_stats.tx_error_frames;
    
    stats->single_collision          = uni_stats.phy_stats.single_collision_frames;
    stats->multiple_collision        = uni_stats.phy_stats.multiple_collision_frames;
    stats->late_collisions           = uni_stats.phy_stats.late_collisions;
    stats->excessive_collisions      = uni_stats.phy_stats.frames_with_excessive_deferral;
    
    stats->txcrcerrfrm_cnt           = 0;
    stats->txoversizefrm_cnt         = 0;
    stats->deferred_transmissions    = uni_stats.phy_stats.frames_with_deferredXmissions;
    stats->foam_errors               = 0;
    //stats->internal_mac_rx_errors  = uni_stats.phy_stats.;
    //stats->internal_mac_tx_errors  = uni_stats.phy_stats.;
    stats->symbol_errors             = uni_stats.phy_stats.symbol_error_during_carrier;

    return ret;
}

oam_status oam_pon_stats_clear_adapt()
{
    oam_status ret = OAM_E_OK;
    ca_port_id_t pon_port;
    ca_epon_port_stats_t port_stats;
    ca_epon_mpcp_stats_t mpcp_stats;
    ca_epon_llid_stats_t llid_stats; /* currently only support 1 llid */
    ca_epon_port_fec_stats_t fec_stats;

    pon_port = OAM_PON_PORT_ID_TO_CA_PORT();
    memset(&port_stats, 0, sizeof(ca_epon_port_stats_t));
    ret = ca_epon_port_stats_get(0, pon_port, TRUE, &port_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_port_stat_get return failed, ret = %d \n",ret);
        return ret;
    }

    memset(&port_stats, 0, sizeof(ca_epon_port_stats_t));
    ret = ca_epon_mpcp_stats_get(0, pon_port, 0, TRUE, &mpcp_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_mpcp_stat_get return failed, ret = %d \n",ret);
        return ret;
    }

    memset(&llid_stats, 0, sizeof(ca_epon_llid_stats_t));
    ret = ca_epon_llid_stats_get(0, pon_port, 0, TRUE, &llid_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_llid_stats_get return failed, ret = %d \n",ret);
        return ret;
    }

    memset(&fec_stats, 0, sizeof(ca_epon_port_fec_stats_t));
    ret = ca_epon_port_fec_stats_get(0, pon_port, TRUE, &llid_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_port_fec_stats_get return failed, ret = %d \n",ret);
        return ret;
    }
    return ret;
}

oam_status oam_uni_stats_clear_adapt(oam_port_id_t port)
{
    oam_status ret = OAM_E_OK;
    ca_port_id_t ca_port;
    ca_eth_port_stats_t uni_stats;

    /* TODO need to add support for OAM_DOWNLINK_PORT */
    if(port == OAM_DOWNLINK_PORT)
    {
        OAM_ORG_LOG("port is downlink port. not supported yet\r\n");
        return OAM_E_OK;
    }

    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);

    memset(&uni_stats, 0, sizeof(ca_eth_port_stats_t));
    ret = ca_eth_port_stats_get(0, ca_port, TRUE, &uni_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_eth_port_stats_get return failed, oam_port %u, ca_port = 0x%x ret = %d \n", port, ca_port,ret);

        return ret;
    }   

    return ret;
}

oam_status oam_queue_stats_get_adapt(
    oam_port_id_t port, 
    oam_uint8 queue, 
    oam_boolean read_clear, 
    oam_queue_stats_t *stats)
{
    oam_status ret = OAM_E_OK;
    ca_port_id_t ca_port;
    ca_queue_stats_t queue_stats_l2;
    ca_queue_stats_t queue_stats_l3;
    ca_eth_port_speed_t speed = CA_PORT_SPEED_10G;
    ca_uint16_t port_id = 0;
     
    /* For KT project:
       LAN1, 2, 3 used DeepQ;
       LAN4: speed != 10G, used DeepQ, speed = 10G, not use DeepQ */
       
    memset(&queue_stats_l2, 0, sizeof(ca_queue_stats_t));
    ca_port = OAM_PORT_ID_TO_CA_PORT(port);
    port_id = PORT_ID(ca_port);
    ret = ca_queue_stats_get(0, ca_port, queue, !read_clear, &queue_stats_l2);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_queue_stats_get return failed, ca_port = 0x%x, queue = %d ret = %d \n",ca_port,queue,ret);
        return ret;
    }

    memset(&queue_stats_l3, 0, sizeof(ca_queue_stats_t));
    if(port >= OAM_UNI_PORT_ID1 && port <= OAM_UNI_PORT_ID4)
    {
        ca_port = CA_PORT_ID(CA_PORT_TYPE_L3, port_id);
        ret = ca_queue_stats_get(0, ca_port, queue, !read_clear, &queue_stats_l3);
        if(ret != OAM_E_OK){
            OAM_ERROR("ca_queue_stats_get return failed, ca_port = 0x%x, queue = %d ret = %d \n",ca_port,queue,ret);
            return ret;
        } 
    }

    stats->fwd_frames   = queue_stats_l2.forwarded + queue_stats_l3.forwarded;
    stats->drop_frames  = queue_stats_l2.tail_dropped + queue_stats_l2.marked_dropped + queue_stats_l2.unmarked_dropped +
                        queue_stats_l3.tail_dropped + queue_stats_l3.marked_dropped + queue_stats_l3.unmarked_dropped;
    stats->fwd_bytes    = queue_stats_l2.forwarded_bytes + queue_stats_l3.forwarded_bytes;
    stats->drop_bytes   = queue_stats_l2.tail_dropped_bytes + queue_stats_l2.marked_dropped_bytes + 
                        queue_stats_l2.unmarked_dropped_bytes + 
                        queue_stats_l3.tail_dropped_bytes + queue_stats_l3.marked_dropped_bytes + 
                        queue_stats_l3.unmarked_dropped_bytes;

    OAM_LOG_DEBUG("oam_queue_stats_get_adapt: port 0x%x, queue %d, fwd_frames: %d, drop_frames: %d, fwd_bytes: %d, drop_bytes: %d\r\n\r\n", 
                ca_port, queue, stats->fwd_frames, stats->drop_frames, stats->fwd_bytes, stats->drop_bytes);
    return OAM_E_OK;
}

oam_status oam_queue_stats_clear_adapt(
    oam_port_id_t port, 
    oam_uint8 queue)
{
    oam_status ret = OAM_E_OK;
    ca_port_id_t ca_port;
    ca_queue_stats_t queue_stats;
    ca_eth_port_speed_t speed;

    ca_port = OAM_PORT_ID_TO_CA_PORT(port);
#ifdef HAVE_KT_OAM 
    /* LAN1, 2, 3 used L3QM(DeepQ);
       LAN4: speed != 10G, used L3QM(DeepQ), speed = 10G, used L2QM;  */
    if(port == OAM_UNI_PORT_ID1 || port == OAM_UNI_PORT_ID2 || port == OAM_UNI_PORT_ID3)
    {
        ca_port = CA_PORT_ID(CA_PORT_TYPE_L3, port);
    }
    else if(port == OAM_UNI_PORT_ID4)
    {
        ca_eth_port_phy_speed_get(0, ca_port, &speed);
        if(speed != CA_PORT_SPEED_10G)
        {
            ca_port = CA_PORT_ID(CA_PORT_TYPE_L3, port);
        }
    } 
#endif

    memset(&queue_stats, 0, sizeof(ca_queue_stats_t));

    ret = ca_queue_stats_get(0, ca_port, queue, FALSE, &queue_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_queue_stats_get return failed, ca_port = 0x%x, queue = %d ret = %d \n",ca_port,queue,ret);
        return ret;
    }

    return OAM_E_OK;
}

ca_uint32_t oam_frame_handler(ca_device_id_t device_id, ca_pkt_t *pkt, void *cookie)
{
    oam_llid_t  llid;
    oam_uint8 llid_idx;

    OAM_ASSERT_RET(pkt != NULL && pkt->pkt_data != NULL, OAM_E_PARAM);

    llid_idx = PORT_ID(pkt->src_sub_port_id);
    if ((pkt->pkt_type != CA_PKT_TYPE_OAM) ||
        (0 == pkt->pkt_data->len))
    {
        OAM_LOG_MAJ("receive error packet type%d len %d", pkt->pkt_type, pkt->pkt_data->len);
        return OAM_E_ERROR;
    }

    pkt->src_port_id = OAM_PON_PORT_ID;
    oam_pkt_msg_send(pkt->src_port_id, llid_idx, pkt->pkt_data->data,
                     pkt->pkt_len);

    if(pkt->pkt_data->done_callback_function){
        pkt->pkt_data->done_callback_function(pkt->pkt_data->cookie);
    }

    return OAM_E_OK;
}
ca_uint32_t oam_reg_status_event_handler
(
    ca_device_id_t         device_id,
    ca_uint32_t         event_id,
    ca_void_t         *event,
    ca_uint32_t         len,
    ca_void_t        *cookie
)
{
    oam_onu_reg_evt_t reg_evt;
    ca_event_reg_stat_t *pEvent;

    //ca_printf("<%s, %d> event_id:%d\r\n", __FUNCTION__, __LINE__, event_id);

    OAM_ASSERT(event != NULL);
    pEvent = (ca_event_reg_stat_t *)event;

    OAM_LOG_DEBUG("OAM EVENT received: index %d, llid 0x%x, status %d\r\n", pEvent->llid_idx, pEvent->llid_value, pEvent->status);

    memset(&reg_evt, 0, sizeof(reg_evt));

    reg_evt.index = pEvent->llid_idx;
    reg_evt.llid = pEvent->llid_value;
    reg_evt.status = pEvent->status ? OAM_TRUE : OAM_FALSE;

    if(pEvent->status == OAM_FALSE){
        switch (pEvent->dereg_cause)
        {
            case CA_EPON_MPCP_DEREG_CAUSE_NO_DEREGISTER:
                reg_evt.dereg_cause = OAM_MPCP_DEREG_CAUSE_NO_DEREGISTER;
                break;
            case CA_EPON_MPCP_DEREG_CAUSE_TIMESTAMPDRIPT:
                reg_evt.dereg_cause = OAM_MPCP_DEREG_CAUSE_TIMESTAMPDRIPT;
                break;
            case CA_EPON_MPCP_DEREG_CAUSE_OLT_DEREGISTER:
                reg_evt.dereg_cause = OAM_MPCP_DEREG_CAUSE_OLT_DEREGISTER;
                break;
            case CA_EPON_MPCP_DEREG_CAUSE_ONU_DEREGISTER:
                reg_evt.dereg_cause = OAM_MPCP_DEREG_CAUSE_ONU_DEREGISTER;
                break;
            case CA_EPON_MPCP_DEREG_CAUSE_RX_REREGISTER:
                reg_evt.dereg_cause = OAM_MPCP_DEREG_CAUSE_RX_REREGISTER;
                break;
            case CA_EPON_MPCP_DEREG_CAUSE_MPCP_TIMEOUT:
                reg_evt.dereg_cause = OAM_MPCP_DEREG_CAUSE_MPCP_TIMEOUT;
                break;
            case CA_EPON_MPCP_DEREG_CAUSE_GATE_TIMEOUT:
                reg_evt.dereg_cause = OAM_MPCP_DEREG_CAUSE_GATE_TIMEOUT;
                break;
            case CA_EPON_MPCP_DEREG_CAUSE_SW_DEREGISTER:
                reg_evt.dereg_cause = OAM_MPCP_DEREG_CAUSE_SW_DEREGISTER;
                break;
            default:
                reg_evt.dereg_cause = OAM_MPCP_DEREG_CAUSE_NO_DEREGISTER;
                break;
        }
    }

    oam_llid_reg_evt_msg_send(&reg_evt);

    return 0;
}

ca_uint32_t oam_port_link_event_handler
(
    ca_device_id_t         device_id,
    ca_uint32_t         event_id,
    ca_void_t         *event,
    ca_uint32_t         len,
    ca_void_t        *cookie
)
{
    oam_onu_port_link_evt_t link_evt;
    ca_event_port_link_t *pEvent;

    OAM_ASSERT(event != NULL);
    pEvent = (ca_event_port_link_t *)event;

    OAM_LOG_DEBUG("OAM EVENT received: port_id 0x%x, status %d\r\n", pEvent->port_id, pEvent->status);

    memset(&link_evt, 0, sizeof(link_evt));

    link_evt.port = OAM_CA_PORT_TO_OAM_PORT(pEvent->port_id);
    if(pEvent->status == CA_PORT_LINK_UP){
        link_evt.status = OAM_TRUE;

    }else{
        link_evt.status = OAM_FALSE;
    }

    oam_port_link_evt_msg_send(&link_evt);

    return 0;
}

oam_status oam_evt_reg_adapt()
{
    ca_status_t       status;

    status = ca_event_register(0, CA_EVENT_EPON_REG_STAT_CHNG, oam_reg_status_event_handler, NULL);

    if (CA_E_OK != status) {
        OAM_LOG_INFO("Error1: Reg state Event reg failed. ret=0x%x\n", status);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status oam_pkt_trap_adapt()
{
    ca_status_t ret = CA_E_OK;
    ca_special_packet_option_t   options = {0};

#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ret = ca_special_packet_set(0, CA_PORT_ID(CA_PORT_TYPE_ETHERNET, CA_PORT_ID_NI4),
                           CA_PKT_TYPE_OAM, TRUE,
                           CA_PORT_ID(CA_PORT_TYPE_CPU, CA_PORT_ID_CPU0),
                           0, &options);
    if (CA_E_OK != ret) {
        OAM_LOG_INFO("ca_special_packet_set failed. ret=0x%x\n", ret);
        return OAM_E_ERROR;
    }

    ca_rx_register(OAM_DEV_ID, CA_PKT_TYPE_OAM, oam_frame_handler, 0, NULL, 0);

#endif

    return OAM_E_OK;
}

#define OAM_SATURN_PKT_HEADER_LEN   16
#define OAM_SATURN_LNK_TYPE_LEN     4
#ifdef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
#define OAM_SATURN_PON_PORT_ID      4
#else
#define OAM_SATURN_PON_PORT_ID      7
#endif

void oam_octet_dump( char *tp, unsigned char *dp, int len )
{
  unsigned char *cp = dp;
  int  ii;
  int  jj;

  printf("%s\r\n", tp);
  for ( ii=len; ii>16; ii-=16 )
  {
    for ( jj=0; jj<16; jj++, cp++ )
      printf("%02x ", *cp);
    printf("\r\n");
  }
  for ( jj=0; jj<ii; jj++, cp++ )
  printf("%02x ", *cp);
  printf("\r\n\r\n");
}

oam_status oam_pkt_send_adapt(
    oam_port_id_t port,
    oam_llid_t llid,
    oam_uint8 *buf,
    oam_uint32 len)
{
    oam_if_t  *intf;
    ca_pkt_t  pkt;
    ca_pkt_block_t  *pkt_data;
    ca_status_t   ret = CA_E_OK;
    //ca_mac_addr_t puc_da = {0x00, 0x13, 0x25, 0x00, 0x00, 0x00};
    //ca_mac_addr_t puc_sa = {0x00, 0x13, 0x25, 0x00, 0x00, 0x01};
    //ca_uint8 puc_lnk_type[OAM_SATURN_LNK_TYPE_LEN] = {0xff, 0xf1, 0x00, 0x00};

    OAM_ASSERT_RET(buf != NULL, OAM_E_PARAM);

    memset(&pkt, 0, sizeof(pkt));
    memset(&pkt_data, 0, sizeof(pkt_data));

    if (len < OAM_MIN_OAMPDU_SIZE)
      len = OAM_MIN_OAMPDU_SIZE;

    intf = oam_intf_find(llid);
    if (intf == NULL) {
        OAM_LOG_MAJ("intf not found \n");
        return OAM_E_ERROR;
    }

    pkt_data = (ca_pkt_block_t *)OAM_MALLOC(sizeof(ca_pkt_block_t));
    pkt_data->len = len;
    //pkt_data->len = len + OAM_SATURN_PKT_HEADER_LEN;
    pkt_data->data = (ca_uint8 *)OAM_MALLOC(pkt_data->len);

    //puc_da[5] = (puc_da[5] & 0x3f) | intf->index; /* add llid info to puc_da[5:0] */
    //memcpy(pkt_data->data, puc_da, CA_ETH_ADDR_LEN);
    //memcpy(pkt_data->data + CA_ETH_ADDR_LEN, puc_sa, CA_ETH_ADDR_LEN);
    //memcpy(pkt_data->data + CA_ETH_ADDR_LEN + CA_ETH_ADDR_LEN, puc_lnk_type, OAM_SATURN_LNK_TYPE_LEN);
    //memcpy(pkt_data->data + OAM_SATURN_PKT_HEADER_LEN, buf, len);
    memcpy(pkt_data->data, buf, len);

    pkt.device_id = 0;
    pkt.flags = CA_TX_BYPASS_FWD_ENGINE;
    pkt.pkt_data = pkt_data;
    pkt.block_count = 1;
    pkt.src_port_id = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x10); // CPU0
    pkt.dst_port_id = CA_PORT_ID(CA_PORT_TYPE_CPU, OAM_SATURN_PON_PORT_ID);
    pkt.dst_sub_port_id = intf->index;
    pkt.pkt_len = pkt_data->len;
    pkt.cos = 8;
    pkt.pkt_type = CA_PKT_TYPE_OAM;

    //ca_printf("ca_tx to port %d, llid index %d\r\n", pkt.dst_port_id, pkt.dst_sub_port_id);

    //ca_app_octet_dump("ca_tx", pkt_data->data, pkt.pkt_len);

    ret = ca_tx(OAM_DEV_ID, &pkt);
    if(CA_E_OK != ret){
        OAM_LOG_INFO("oam packet tx failed:ret:%d\n", ret);
    }

    OAM_FREE(pkt_data->data);
    OAM_FREE(pkt_data);

    return ret;
}

oam_ext_mode_t oam_ext_mode_get_adapt()
{
    ca_uint8_t oam_mode;
    ca_device_config_tlv_t tlv;

    tlv.type = CA_CFG_ID_OAM_MODE;
    ca_device_config_tlv_get(0, &tlv);

    oam_mode = tlv.value[0];

    /*
    OAM_EXT_MODE_CTC = 0,
    OAM_EXT_MODE_DPOE,
    OAM_EXT_MODE_KT,
    */
    return oam_mode;
}

oam_status
oam_ca_ge_bmp_get_adapt (
    oam_uint32* bmp)
{
    oam_status             ret      = OAM_E_ERROR;
    ca_uint8_t             oam_mode;
    ca_device_config_tlv_t tlv;

    if ( !bmp )
    {
        goto ERROR_OAM_GE_BMP_GET_ADAPT;
    }

    memset ( &tlv , 0x00 , sizeof(ca_device_config_tlv_t) );

    tlv.type = CA_CFG_ID_GE_PORT_BIT_MAP;
    ret = ca_device_config_tlv_get ( 0 , &tlv );
    if ( ret != OAM_E_OK )
    {
        ca_printf (
            "%s: Failed to get scfg for CA_CFG_ID_GE_PORT_BIT_MAP\n" ,
            __FUNCTION__ );
    }
    else
    {
        *bmp = *((unsigned*)tlv.value );
    }

ERROR_OAM_GE_BMP_GET_ADAPT:
    return ret;
}

oam_status
oam_ca_xge_bmp_get_adapt (
    oam_uint32* bmp)
{
    oam_status             ret      = OAM_E_ERROR;
    ca_uint8_t             oam_mode;
    ca_device_config_tlv_t tlv;

    if ( !bmp )
    {
        goto ERROR_OAM_XGE_BMP_GET_ADAPT;
    }

    memset ( &tlv , 0x00 , sizeof(ca_device_config_tlv_t) );

    tlv.type = CA_CFG_ID_XGE_PORT_BIT_MAP;
    ret = ca_device_config_tlv_get ( 0 , &tlv );
    if ( ret != OAM_E_OK )
    {
        ca_printf (
            "%s: Failed to get scfg for CA_CFG_ID_GE_PORT_BIT_MAP\n" ,
            __FUNCTION__ );
    }
    else
    {
        *bmp = *((unsigned*)tlv.value );
    }

ERROR_OAM_XGE_BMP_GET_ADAPT:
    return ret;
}

oam_status
oam_epon_mac_mode_get_adapt(  
    ca_uint8_t* epon_mac_mod)
{
    oam_status ret = OAM_E_ERROR;
    ca_status_t ca_ret = CA_E_ERROR;
    ca_device_config_tlv_t scfg;

    memset ( &scfg, 0x00, sizeof ( scfg ) );

    scfg.type = CA_CFG_ID_PON_MAC_MODE;
    scfg.len  = 1;

    ca_ret = ca_device_config_tlv_get ( 0, &scfg );
    if(ca_ret != CA_E_OK) {
        ca_printf ( "%s: Failed to get scfg for CA_CFG_ID_PON_MAC_MODE\n", __FUNCTION__ );
    }
    
    *epon_mac_mod = scfg.value[ 0 ];
    ret = OAM_E_OK;

ERROR_OAM_EPON_MAC_MODE_GET_ADAPT:
    return ret;
}

oam_status oam_pon_led_set_adapt(oam_uint8 action)
{
#ifdef SATURN_PORT_REMOVE
    led_send_oam_message(status);
#endif
    return OAM_E_OK;
}

/* traffic enable is for temp use */
static ca_uint32_t g_oam_traffic_rule_index[OAM_UNI_PORT_NUM_MAX];
oam_status oam_traffic_enable_adapt(oam_uint8 enable)
{
    ca_uint32_t i = 0;
    ca_port_id_t pon_port;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t mask;
    ca_classifier_action_t action;

    ca_vlan_port_control_t control;

    pon_port = OAM_PON_PORT_ID_TO_CA_PORT();
    memset(&key, 0, sizeof(ca_classifier_key_t));
    memset(&mask, 0, sizeof(ca_classifier_key_mask_t));
    memset(&action, 0, sizeof(ca_classifier_action_t));

    for(i = OAM_UNI_PORT_ID1; i <= OAM_UNI_PORT_NUM; i++){
        if(enable == 1){
            mask.src_port = 1;
            key.src_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, i+2);
            action.dest.port = pon_port;
            action.forward = CA_CLASSIFIER_FORWARD_PORT;
            action.options.masks.action_handle = 1;
            action.options.action_handle.flow_id = 0;
            //ca_classifier_rule_add(0, 0, &key, &mask, &action, &g_oam_traffic_rule_index[i-1]);
        }else{
            //ca_classifier_rule_delete(0, g_oam_traffic_rule_index[i-1]);
        }
    }

    memset(&control, 0, sizeof(ca_vlan_port_control_t));
    ca_l2_vlan_port_control_get(0, pon_port, &control);
    control.drop_unknown_vlan = !enable;
    ca_l2_vlan_port_control_set(0, pon_port, &control);

    return OAM_E_OK;
}

oam_status oam_port_control_set_adapt(oam_uint8 enable){

    ca_port_id_t pon_port;
    ca_vlan_port_control_t control;

    pon_port = OAM_PON_PORT_ID_TO_CA_PORT();

    memset(&control, 0, sizeof(ca_vlan_port_control_t));
    ca_l2_vlan_port_control_get(0, pon_port, &control);
    control.drop_unknown_vlan = enable;
    ca_l2_vlan_port_control_set(0, pon_port, &control);

    return OAM_E_OK;
}

static ca_boolean_t g_oam_traffic_status = 0;
oam_status oam_traffic_set_adapt(oam_uint8 link_status)
{
    ca_device_config_tlv_t *dev_cfg_tlv = NULL;
    oam_uint8 enable;
    dev_cfg_tlv = ca_malloc(sizeof(ca_device_config_tlv_t));
    dev_cfg_tlv->type = CA_CFG_ID_EN_TRAFFIC_AFTER_OAM_DISCOVERY;
    ca_device_config_tlv_get(0, dev_cfg_tlv);
    enable = dev_cfg_tlv->value[0];
    if(enable == TRUE){
        if(link_status != g_oam_traffic_status){
            ca_printf("traffic %s after oam link change\r\n", (link_status!=0)?"enable":"disable");
            oam_traffic_enable_adapt(link_status);
            g_oam_traffic_status = link_status;
        }
    }else {
        //clear all default cls for bug#50715
        oam_port_control_set_adapt(enable);
    }
    return OAM_E_OK;
}

void __onu_block_data_path(ca_boolean_t enable)
{
    ca_port_tx_traffic_block_set(0,0x30007,enable);
    ca_port_tx_traffic_block_set(0,0x3000F,enable);
    ca_port_egr_qm_block_set(0,0x30007,enable);
    ca_port_egr_qm_block_set(0,0x3000F,enable);
    return;
}

void oam_pon_mac_reset_adapt()
{
    ca_uint8_t queue_index = 0;

    __onu_block_data_path(TRUE);
    for(queue_index = 0; queue_index < 8; queue_index++){
        ca_queue_flush(0,0x3000C,queue_index);
        ca_queue_flush(0,0x30007,queue_index);
        ca_queue_flush(0,0xC0007,queue_index);
    }
    //ca_epon_reset_pon_mac(0);
    //ca_printf("reset_pon_mac!\r\n");
}

oam_status oam_llid_admin_set_adapt(oam_uint16 llid_idx, oam_boolean enable)
{
    ca_port_id_t pon_port;
    pon_port = OAM_PON_PORT_ID_TO_CA_PORT();

    ca_printf("ca_epon_mpcp_registration_set llid_idx %d enable\r\n", llid_idx);
    ca_epon_mpcp_registration_set(0, pon_port, llid_idx, enable);

    return OAM_E_OK;
}

oam_status oam_llid_enable_init_adapt()
{
    ca_status_t ret = CA_E_OK;
    ca_port_id_t  port = 0;
    ca_uint32_t i = 0;
    ca_uint8_t reg_mode = 1;
    ca_device_config_tlv_t reg_scfg;
    ca_device_config_tlv_t llid_mode_scfg;
    ca_device_config_tlv_t llid_num_scfg;
    ca_uint8_t llid_mode = CA_EPON_MPCP_S_LLID;
    ca_uint8_t llid_num = 1;

    /* reg_mode:
    0: No LLID registration after ONU bootup
    1: Register all LLIDs as specified in CFG_ID_PON_LLID_NUM if M-LLID mode is enabled (CFG_ID_PON_LLID_MODE = 1), or
    register the base LLID if M-LLID mode is disabled (CFG_ID_PON_LLID_MODE = 0)
    2: Only register the base LLID after ONU bootup. If M-LLID mode is enabled, ONU will wait for OLT instruction in
    OAM (0x07/0x7FF1) to only register the LLIDs specified in the OAM
    */

    memset(&reg_scfg, 0, sizeof(reg_scfg));
    reg_scfg.type = CA_CFG_ID_EN_REGISTER_AFTER_BOOT;
    if (CA_E_OK == ca_device_config_tlv_get(0, &reg_scfg)){
        reg_mode = reg_scfg.value[0];
        ca_printf("CA_CFG_ID_EN_REGISTER_AFTER_BOOT = %d\r\n", reg_mode);
    }

    if(reg_mode != 0){

        if(reg_mode == 1){

            memset(&llid_mode_scfg, 0, sizeof(llid_mode_scfg));
            llid_mode_scfg.type = CA_CFG_ID_PON_LLID_MODE;
            if (CA_E_OK == ca_device_config_tlv_get(0, &llid_mode_scfg)){
                llid_mode = llid_mode_scfg.value[0];
                ca_printf("CA_CFG_ID_PON_LLID_MODE = %d\r\n", llid_mode);
            }
            if(llid_mode == CA_EPON_MPCP_M_LLID){ /* multi llid mode */

                memset(&llid_num_scfg, 0, sizeof(llid_num_scfg));
                llid_num_scfg.type = CA_CFG_ID_PON_LLID_NUM;
                if (CA_E_OK == ca_device_config_tlv_get(0, &llid_num_scfg)){
                    llid_num = llid_num_scfg.value[0];
                    ca_printf("CA_CFG_ID_PON_LLID_NUM = %d\r\n", llid_num);
                }
            }
        }else{
            llid_num = 1;
        }

        for(i = 0; i < llid_num; i++){
            ca_printf("enable mpcp registration for llid_idx %d\r\n", i);
            ret = ca_epon_mpcp_registration_set(0, 0x20007, i, 1);
        }
    }

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
#ifdef HAVE_KT_OAM
    ca_uint8_t tk_value_samsung[] = {
        0x52,0x01,0,0x11,
        0xfe,0x0e,0,0x0d,
        0xb6,0x41,0x1
    };
#endif

    if (intf == NULL)
        return OAM_E_ERROR;

    intf->local_dying_gasp = TRUE;
#ifndef HAVE_KT_OAM
    intf->local_critical_event = TRUE;
#endif
    memset(pdu, 0x00, OAM_MIN_OAMPDU_SIZE);

    len = oam_build_pdu_info(intf, NULL, (oam_pdu_info_t *)pdu);
#ifdef HAVE_KT_OAM
        memset(pdu + 16, 0, (len - 16));
        memcpy(pdu + 16, tk_value_samsung, sizeof(tk_value_samsung));
#endif
    ca_epon_dying_gasp_set(0, 0, 1, pdu);

    OAM_LOG_DEBUG("Build Dying Gasp packet.\n");
    /*oam_buf_print(pdu, 64);*/

    intf->local_dying_gasp = FALSE;
#ifndef HAVE_KT_OAM
    intf->local_critical_event = FALSE;
#endif
    return OAM_E_OK;
}

#ifdef HAVE_LOOP_DETECT
oam_status oam_enable_loop_detect_adapt(oam_boolean enable)
{
    if (enable)
        loop_detect_enable(LOOP_DETECT_ALL_PORT_ID);
    else
        loop_detect_disable(LOOP_DETECT_ALL_PORT_ID);

    return OAM_E_OK;
}
#endif

void oam_system_reboot(void)
{
    ca_reset(0, 0);
    return;
}

void oam_eth_enable(oam_uint8 eth ,oam_boolean enable){

    if(eth&DPOE_ETH3_BIT){
        if(enable){
            system("ifconfig eth3 up");
        }else{
            system("ifconfig eth3 down");
        }
    }

    if(eth&DPOE_ETH2_BIT){
        if(enable){
            system("ifconfig eth2 up");
        }else{
            system("ifconfig eth2 down");
        }
    }
}

oam_boolean oam_dpoe_ubq_check(void){
    ca_uint32_t customer_bit   = 0;
    ca_device_config_tlv_t scfg_tlv;
    ca_status_t ret = CA_E_OK;

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_CUSTOMER_BIT;
    scfg_tlv.len = 4;
    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if (CA_E_OK != ret) {
        OAM_LOG_DEBUG("read CA_CFG_ID_CUSTOMER_BIT fail. ret = %d \r\n", ret);
        return OAM_E_ERROR;
    }
    memcpy(&customer_bit,scfg_tlv.value,4);

    if((customer_bit & CUSTOMER_BIT_0) &&
        (customer_bit & CUSTOMER_BIT_16)){
        return TRUE;
    }else{
        return FALSE;
    }
}

/****************************************************************************************************/
/***************** IMAGE upgrade ********************************************************************/
/****************************************************************************************************/

void oam_img_remove_adapt(void)
{
    oam_int32 ret = 0;

    if(access(OAM_UPG_IMG_NAME, F_OK) != 0)
    {
        cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_ERROR,0, "image file %s is not exist", OAM_UPG_IMG_NAME);
        return;
    }

    // need to call functions for clean the image in flash
    ret = remove(OAM_UPG_IMG_NAME);
    if(ret < 0)
    {
        cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_ERROR,0, "remove file err");
        perror("remove");
    }

    return;
}

void oam_img_upgrade_adapt(void)
{
    char upgrade_cmd[64]={0};
    system("echo 0 > /tmp/upgrade_status");
    snprintf(upgrade_cmd, 63, "kt-upgrade -f %s ", OAM_UPG_IMG_NAME);
    system(upgrade_cmd);

    return 0;
}

oam_int32 __img_info_get(
    ca_device_id_t device_id,
    ca_uint8_t     img_idx,
    ca_img_info_t  *img_info)
{
    ca_img_env_load(device_id);

    return ca_img_info_get(device_id, img_idx, img_info);
}

oam_int32 __img_active_part_get(void)
{
    ca_img_info_t img_info0;

    __img_info_get(0, 0, &img_info0);

    if(img_info0.active_flag)
        return 0;

    return 1;
}

oam_int32 oam_img_verify_adapt(void)
{
    ca_img_info_t img_info0;
    ca_img_info_t img_info1;
    oam_uint32 active_part;

    memset(&img_info0, 0x0, sizeof(ca_img_info_t));
    memset(&img_info1, 0x0, sizeof(ca_img_info_t));

    __img_info_get(0, 0, &img_info0);
    __img_info_get(0, 1, &img_info1);
    
    OAM_ORG_LOG("img Img0 commit flag %d valid flag %d active flag %d\r\n", img_info0.commit_flag, img_info0.valid_flag, img_info0.active_flag);
    OAM_ORG_LOG("img Img1 commit flag %d valid flag %d active flag %d\r\n", img_info1.commit_flag, img_info1.valid_flag, img_info1.active_flag);

    active_part = __img_active_part_get();
    if(active_part)
        return img_info0.valid_flag;

    return img_info1.valid_flag;
}

oam_int32 oam_img_commit_adapt(void)
{
    oam_uint32 active_part;

    active_part = __img_active_part_get();

    if(active_part)
    {
        ca_img_commit_set(0, 0, 1); // image#0, active
        ca_img_commit_set(0, 1, 0); // image#1, de-active
        ca_img_executed_set(0, 1, 1);
        ca_img_executed_set(0, 0, 0);
        return 0;
    }

    ca_img_commit_set(0, 0, 0); // image#0, de-active
    ca_img_commit_set(0, 1, 1); // image#1, active
    ca_img_executed_set(0, 0, 1);
    ca_img_executed_set(0, 1, 0);

    return 0;
}


