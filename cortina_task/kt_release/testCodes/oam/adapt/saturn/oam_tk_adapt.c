#ifdef HAVE_TK_OAM

#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <fcntl.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <errno.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/route.h>
#include <assert.h>

#include "scfg.h"
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
#include "oam_port_stats.h"

#include "oam_adapt.h"
#include "oam_tk_adapt.h"
#include "oam_tk_event.h"
#include "oam_tk.h"

#include "osal_cmn.h"

#include "l2.h"
#include "port.h"
#include "eth_port.h"
#include "rate.h"
#include "vlan.h"

#include "pon.h"
#include "epon.h"
#include "qos.h"
#ifdef HAVE_LOOP_DETECT
#include "loop_detect.h"
#endif

#include "ca_gpio.h"

#include "mc_api.h"


#ifdef HAVE_ONU_RSTP
#include "stp_mgmt.h"
#endif
#ifdef     CONFIG_ARCH_CORTINA_SATURN_SFU
#ifndef CONFIG_ARCH_CORTINA_SATURN_SFP_PLUS
#include "app_rtl83xx_util.h"
#endif
#endif

#include "dhcp.h"


#ifdef HAVE_DPOE_OAM
/******************************************************************************/
/*               TK MSG FOR DPOE project                                      */
/******************************************************************************/

typedef struct{
    oam_uint8 valid;
    tk_oam_queue_cfg_t cfg;
}tk_oam_queue_info_t;

typedef struct{
    oam_uint32 report_mode;
    tk_oam_queue_info_t link[TK_OAM_LINK_NUM_MAX];
    tk_oam_queue_info_t port[TK_OAM_PORT_NUM_MAX];
}tk_oam_queue_tbl_t;

static tk_oam_queue_tbl_t g_tk_oam_queue_tbl;

oam_status tk_oam_queue_report_mode_get_adapt(oam_uint32 *report_mode)
{
    *report_mode = g_tk_oam_queue_tbl.report_mode;
}

oam_status tk_oam_queue_report_mode_set_adapt(oam_uint32 report_mode)
{
    g_tk_oam_queue_tbl.report_mode = report_mode;
    //memset(&g_tk_oam_queue_tbl, 0, sizeof(g_tk_oam_queue_tbl));
}

oam_status tk_oam_link_queue_get_adapt(oam_uint16 llid_idx, tk_oam_queue_cfg_t *cfg)
{
    if(llid_idx >= TK_OAM_LINK_NUM_MAX){
        return OAM_E_ERROR;
    }
    if(g_tk_oam_queue_tbl.link[llid_idx].valid){
        memcpy(cfg, &g_tk_oam_queue_tbl.link[llid_idx].cfg, sizeof(tk_oam_queue_cfg_t));
        
    }else{
        return OAM_E_ERROR;
    }
    return OAM_E_OK;
}

oam_status tk_oam_link_queue_set_adapt(oam_uint16 llid_idx, tk_oam_queue_cfg_t cfg)
{
    if(llid_idx >= TK_OAM_LINK_NUM_MAX){
        return OAM_E_ERROR;
    }
    g_tk_oam_queue_tbl.link[llid_idx].valid = 1;
    memcpy(&g_tk_oam_queue_tbl.link[llid_idx].cfg, &cfg, sizeof(tk_oam_queue_cfg_t));
    return OAM_E_OK;
}

oam_status tk_oam_port_queue_get_adapt(oam_uint16 port, tk_oam_queue_cfg_t *cfg)
{
    if(port > TK_OAM_PORT_NUM_MAX){
        return OAM_E_ERROR;
    }
    if(g_tk_oam_queue_tbl.port[port-1].valid){
        memcpy(cfg, &g_tk_oam_queue_tbl.port[port-1].cfg, sizeof(tk_oam_queue_cfg_t));
        
    }else{
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status tk_oam_port_queue_set_adapt(oam_uint16 port, tk_oam_queue_cfg_t cfg)
{
    if(port > TK_OAM_PORT_NUM_MAX){
        return OAM_E_ERROR;
    }
    g_tk_oam_queue_tbl.port[port-1].valid = 1;
    memcpy(&g_tk_oam_queue_tbl.port[port-1], &cfg, sizeof(tk_oam_queue_cfg_t));
    return OAM_E_OK;
}

oam_status tk_oam_port_mtu_get_adapt(oam_uint16 port, oam_uint32 *mtu)
{
    ca_port_id_t ca_port = 0;

    OAM_PORT_ID_CHECK(port);
    ca_port = OAM_PORT_ID_TO_CA_PORT(port);
    
    ca_port_max_frame_size_get(0, ca_port, mtu);
    
    return OAM_E_OK;
}

oam_status tk_oam_port_mtu_set_adapt(oam_uint16 port, oam_uint32 mtu)
{
    ca_port_id_t ca_port = 0;

    OAM_PORT_ID_CHECK(port);
    ca_port = OAM_PORT_ID_TO_CA_PORT(port);

    ca_printf("ca_port_max_frame_size_set ca_port 0x%x, mtu %d\r\n", ca_port, mtu);
    ca_port_max_frame_size_set(0, ca_port, mtu);
    
    return OAM_E_OK;
}

/* the delay between llid_x and llid_y registration has been done in SDK */
void tk_oam_mllid_reg_enable_adapt(oam_uint8 llid_num)
{
    ca_uint32_t i = 0;
    ca_port_id_t pon_port = OAM_PON_PORT_ID_TO_CA_PORT();
    
    for(i = 1; i < llid_num; i++){
        ca_epon_mpcp_registration_set(0, pon_port, i, 1);
    }
}

#endif/*END_HAVE_DPOE_OAM*/

#ifdef HAVE_KT_OAM
/******************************************************************************/
/*               TK MSG FOR KT project                                        */
/******************************************************************************/
oam_uint8 g_tk_rule_getindex = 0;

oam_uint16 g_ageing_time = 0x7fbc;   /* init value from LynxC */
oam_uint8  g_learning_mode[4+1] = {1, 1, 1, 1, 1};      /* init value from LynxC */

kt_oam_manage_t     tk_manage;
oam_uint8 g_flow_control[4] = {0, 0, 0, 0};

extern void init_onu_mac_filter_ctrl();
extern void init_per_port_storm_ctrl();
extern void uni_link_event_status_init();

/****************************************************************/

oam_status tk_oam_fw_version_get_adapt(
        oam_kt_fw_version_t * ver)
{
    oam_int8 buffer[128];
    oam_int8 tmpBuf[64];
    oam_int8 VerBuf[64];
    oam_int32 i = 0;
    oam_int32 build;

    //extern int rtk_env_get(const char *name, char *buf, unsigned int buflen);

    //rtk_env_get("sw_active", buffer, sizeof(buffer));
    sscanf(buffer, "sw_active=%d", &i);
    sprintf(tmpBuf, "sw_version%d", i);


    //rtk_env_get(tmpBuf, buffer, sizeof(buffer));
    strcat(tmpBuf, "=%s");
    /* tmpBuf == "sw_version0=%s" */
    sscanf(buffer, tmpBuf, VerBuf);
    sscanf(VerBuf,"%d.%d.%d.%d.%d",&ver->maj,&ver->min,&ver->rev,&ver->build,&build);
    return OAM_E_OK;
}

void tk_oam_fw_version_get_adapt_2(
        oam_kt_fw_version_t * ver)
{
    ca_img_info_t img_info;
    memset(&img_info, 0, sizeof(img_info));
    ca_img_info_get(0, 0, &img_info);
    if(!img_info.active_flag)
    {
        ca_img_info_get(0, 1, &img_info);
    }

    ver->maj = img_info.version[1] - 0x30;
    ver->min = img_info.version[4] - 0x30;
    ver->rev = img_info.version[7] - 0x30;
    ver->build = img_info.version[10] - 0x30;
}

oam_status tk_oam_fw_backup_version_get_adapt(
        oam_kt_fw_version_t * ver)
{
    oam_int8 buffer[128];
    oam_int8 tmpBuf[64];
    oam_int8 VerBuf[64];
    oam_int32 i = 0;
    oam_int32 build;

    //extern int rtk_env_get(const char *name, char *buf, unsigned int buflen);

    //rtk_env_get("sw_active", buffer, sizeof(buffer));
    sscanf(buffer, "sw_active=%d", &i);
    i = i == 0? 1:0;
    sprintf(tmpBuf, "sw_version%d", i);


    //rtk_env_get(tmpBuf, buffer, sizeof(buffer));
    strcat(tmpBuf, "=%s");
    /* tmpBuf == "sw_version0=%s" */
    sscanf(buffer, tmpBuf, VerBuf);
    sscanf(VerBuf,"%d.%d.%d.%d.%d",&ver->maj,&ver->min,&ver->rev,&ver->build,&build);

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_onu_fw_version_get_adapt                             */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_onu_fw_version_get_adapt(
        tk_oam_onu_fw_version_get_rsp_t  *p_version
        )
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_kt_fw_version_t ver;
    tk_oam_fw_version_get_adapt_2(&ver);

    oam_int8 data[16];

    if (!p_version)
        return OAM_E_ERROR;

    snprintf(data,15,"%d%d%d%d",ver.maj,ver.min,ver.rev,ver.build);

    p_version->fw_version = atoi(data);
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_onu_model_name_get_adapt                             */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_onu_model_name_get_adapt(
    tk_oam_onu_model_name_get_rsp_t  *p_model
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_uint16 version = 0x8271;
    oam_uint8 model_name[16] = {0};

    if (!p_model)
        return OAM_E_ERROR;

    memset(p_model->model_name, 0, sizeof(p_model->model_name));
/*
    extern oam_status onu_get_chip_version(oam_uint16 *version);
    onu_get_chip_version(&version);
*/
    //mib_get(MIB_HW_ONU_MOD_ID, model_name);

    memcpy(p_model->model_name, model_name, 16);

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_onu_loop_detect_set_adapt                            */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_onu_loop_detect_set_adapt(
    oam_uint8       enable
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status result = OAM_E_OK;
#ifdef HAVE_LOOP_DETECT
    if (enable) {
        loop_detect_interval_set(2000);
        loop_detect_block_time_set(600000);
        result = loop_detect_enable(OAM_ALL_UNI_PORT_ID);
    } else
        result = loop_detect_disable(OAM_ALL_UNI_PORT_ID);
#endif

    return result;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_onu_loop_detect_get_adapt                            */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_onu_loop_detect_get_adapt(
    oam_uint8      *enable
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    if (!enable) {
        return OAM_E_PARAM;
    }

    return loop_detect_oper_get(OAM_ALL_UNI_PORT_ID, enable);
    //return 0;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_onu_stp_set_adapt                                    */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_onu_stp_set_adapt(
    oam_uint8  enable
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status ret = OAM_E_OK;
#ifdef HAVE_ONU_RSTP
    epon_onu_stp_config_t bridge_cfg;

    memset(&bridge_cfg, 0, sizeof(epon_onu_stp_config_t));
    ret = app_stp_bridge_cfg_get(&bridge_cfg);

    bridge_cfg.mask = EPON_MASK_STP_BRIDGE_ENABLE | EPON_MASK_STP_BRIDGE_MAX_AGE |
                      EPON_MASK_STP_BRIDGE_HELLO_TIME | EPON_MASK_STP_BRIDGE_FORWARD_DELAY;
    bridge_cfg.enable = enable;

    ret = app_stp_bridge_cfg_set(&bridge_cfg);
#endif
    return ret;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_onu_stp_get_adapt                                    */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_onu_stp_get_adapt(
    oam_uint8  *enable
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status ret = OAM_E_OK;
#ifdef HAVE_ONU_RSTP
    epon_onu_stp_config_t bridge_cfg;

    if (!enable)
        return OAM_E_ERROR;
    ret = app_stp_bridge_cfg_get(&bridge_cfg);

    if (bridge_cfg.enable == 1)
        *enable = 1;
    else
        *enable = 0;
#endif
    return ret;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_onu_stp_enable_set_adapt                             */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_onu_stp_enable_set_adapt(
    oam_uint8  enable
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    //ca_callback_context_t      context;
    //return epon_request_onu_stp_enable(context, ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, enable);
    return epon_request_onu_stp_enable(0, 0, enable);
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_onu_bpdu2cpu_get_adapt                               */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_onu_bpdu2cpu_get_adapt(
    oam_uint8  *action
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
#if 0
    ca_callback_context_t      context;
    return epon_request_onu_bpdu2cpu_get(context, ONU_DEVICEID_FOR_API,
                                         ONU_LLIDPORT_FOR_API, (oam_boolean *)action);
#endif
    stp_bpdu_capture_get(action);
    return 0;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_block_data_path_set_adapt                            */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_block_data_path_set_adapt(
    oam_uint16  enable
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = OAM_E_OK;
    ca_status_t ret;
    oam_port_id_t port;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    static ca_uint32_t index[1+4] = {0,0,0,0,0};
    static ca_uint32_t configured[1+4] = {0,0,0,0,0};
    ca_port_id_t ca_port;

    memset(&key, 0, sizeof(key));
    memset(&key_mask, 0, sizeof(key_mask));
    memset(&action, 0, sizeof(action));
    if (enable) /* disable traffic */
    {
        for(port = OAM_PON_PORT_ID; port <= OAM_UNI_PORT_NUM; port++) {
            if(configured[port])
            {
                continue;
            }
            ca_port = OAM_PORT_ID_TO_CA_PORT(port);
            key.src_port = ca_port;
            key_mask.src_port = 1;
            action.forward = CA_CLASSIFIER_FORWARD_DENY;
            if(port == OAM_PON_PORT_ID)
            {
        key.key_handle.llid_cos_index = 0;
        key_mask.key_handle = 1;
            }
            ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action, &index[port]);
        if (ret == CA_E_OK)
        {
                configured[port] = TRUE;
                OAM_ORG_LOG("port %d disable traffic OK, cls index %d\n", port, index[port]);
        }
        else
        {
                configured[port] = FALSE;
                OAM_ORG_LOG_ERROR("port %d disable traffic fail, cls ret %d\n", port, ret);
        }

    }
        }
    else    /* enable traffic */
        {
        for(port = OAM_PON_PORT_ID; port <= OAM_UNI_PORT_NUM; port++)
        {
            if(configured[port])
        {
                ret = ca_classifier_rule_delete(0, index[port]);
                configured[port] = FALSE;
                OAM_ORG_LOG("del CLS index %d ret %d\n", index[port], ret);
        }
        }
    }
#if 0
    rc = ca_port_enable_set(0, pon_port, enable);
    if(rc != OAM_E_OK)
        return rc;

    for(port = OAM_UNI_PORT_ID1; port <= OAM_UNI_PORT_ID4; port++){
        uni_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ca_port_enable_set(0, uni_port, enable);
    }
#endif
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_tx_power_set_adapt                                   */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_gpio_set_adapt(
    oam_uint32  port_map,
    oam_uint32  value_map
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
#if 0
    oam_boolean en;
    ca_callback_context_t   context;
    static oam_boolean off_received = 0;

    if (port_map == 2) {
        en = value_map;
    } else if (port_map == 5) {
        en = !value_map;
    } else {
        return OAM_E_OK;
    }

    if (g_laser_tx_power_timer != 0) { /*stop timer incase called before previous timer expired */
        ca_timer_del(g_laser_tx_power_timer);
        g_laser_tx_power_timer = 0;
    }

    if (!en && !off_received) { /* setup timer only when power off */
        g_laser_tx_power_timer = ca_timer_add(15 * 1000, __laser_tx_power_on, NULL);
        off_received = 1;
    }

    if(!en) {
        onu_auto_reboot_enable(0);
    }

    epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, en);
    epon_request_onu_pon_rx_enable_set(context, 0, 0, en);
#endif
    return OAM_E_OK;
}



oam_status tk_oam_add_new_rule_adapt (
                            oam_uint8            port,
                            epon_tk_rule_t *oam_config
)
{

    tk_record_t         *     record=NULL;
    oam_type_t               oam_type= TK_OAM;
    oam_uint8                    rule_action=0;
    oam_uint8                    field=0;
    tk_cls_index_t   *      cls_index=NULL;
    oam_status                   ret=CA_E_OK;

    if ( oam_config ==NULL) {
        ret =CA_E_ERROR;
        /*diag_printf("\r\n in %s %d \r\n ", __FUNCTION__, __LINE__);*/
        goto END;
    }

    /*check if the set is exist*/
   ret= tk_record_find ( port, oam_type ,  oam_config, &record);
   if (ret == CA_E_OK) {
        /*diag_printf("\r\n in %s %d \r\n ", __FUNCTION__, __LINE__);*/
        goto END;
   }


    rule_action= oam_config->rule_action;

    field=oam_config->cond[0].field;

    ret=tk_vlan_rule_is ( field,  rule_action);
   if (ret ==CA_E_OK  ) {

        /*port vlan process*/
        ret = tk_vlan_add_adapt(port, oam_config);
        /*diag_printf("\r\n in %s %d \r\n ", __FUNCTION__, __LINE__);*/


   } else {
        /*Filter or classification process*/
       ret= tk_cls_add_adapt ( oam_config, &cls_index);
       /* diag_printf("\r\n in %s %d \r\n ", __FUNCTION__, __LINE__);*/
   }

    if (ret ==CA_E_OK) {
        /*save oam set*/
        if(cls_index != NULL){
            cls_index->valid=1;
        }
        /*diag_printf("\r\n in %s %d \r\n ", __FUNCTION__, __LINE__);*/
        tk_record_add ( port, oam_type , cls_index,  oam_config);

    }

END:


     return ret ;


    return 0;
}

oam_status tk_oam_del_new_rule_adapt (
                            oam_uint8            port,
                            epon_tk_rule_t *p_rule
)
{

    //ca_callback_context_t context;
    oam_int32                   device=0;
    oam_int32                   llidport=0;
    oam_uint8                   index=0;
    tk_record_t         *     record=NULL;
    oam_type_t               oam_type=TK_OAM;
    oam_uint8                    rule_action=0;
    oam_uint8                    field=0;
    tk_cls_index_t           * cls_index=NULL;
    oam_status                   ret=CA_E_OK;


    //memset (&context, 0, sizeof (ca_callback_context_t));

    if ( p_rule ==NULL) {
        ret =CA_E_ERROR;
        goto END;
    }

    /*check if the set is exist*/
   ret= tk_record_find ( port, oam_type  , p_rule, &record);
   if (ret == CA_E_ERROR) {
        return CA_E_OK;
   }

    rule_action= p_rule->rule_action;

    field=p_rule->cond[0].field;

    ret=tk_vlan_rule_is ( field,  rule_action);
    if ( ret ==CA_E_OK ) {
         /*port vlan process*/
        ret = tk_vlan_del_adapt(port, p_rule);
    } else {
        cls_index=record->cls_index;
        index=cls_index->rule_index;
#ifdef RM_KT_LINUX
        ret=epon_request_onu_cls_del ( context, device,llidport, index);
#endif
    }

    if (ret ==CA_E_OK ) {
        tk_record_del ( record);
    }

    ret= CA_E_OK ;

END:

     return ret;

    return 0;
}

oam_status tk_oam_del_all_rule_adapt (
                            oam_uint8            port
)
{
    oam_int32                 i=0;
    tk_record_t          *    record=NULL;
    epon_tk_rule_t      * tk_rule=NULL;
    oam_status               ret=CA_E_OK;


    for ( i=0; i < KT_OAM_MAX_RULE_RECORD ; i++) {
        record=&tk_manage.record[i];

        if ( (record->valid==1)
              && (record->port ==port )) {
            tk_rule=& record->rule.tk;
            tk_oam_del_new_rule_adapt ( port, tk_rule);
        }

    }

     return ret;
}

void tk_rule_getindex_clr_adapt()
{
    g_tk_rule_getindex = 0;
}

oam_status tk_oam_onu_rules_get_adapt(oam_uint16 port_id, oam_uint8 rsp[], oam_int32 *p_len)
{
    oam_int32 length = 0;

    if (g_tk_rule_getindex >= KT_OAM_MAX_RULE_RECORD) {
        g_tk_rule_getindex = 0;
        return CA_E_ERROR;
    }

    while (!tk_manage.record[g_tk_rule_getindex].valid || tk_manage.record[g_tk_rule_getindex].oam_type != TK_OAM ||
           (tk_manage.record[g_tk_rule_getindex].valid &&
           (tk_manage.record[g_tk_rule_getindex].port != port_id))) {
        g_tk_rule_getindex ++;
        if (g_tk_rule_getindex >= KT_OAM_MAX_RULE_RECORD) {
            g_tk_rule_getindex = 0;
            return CA_E_ERROR;
        }
    }

    length = sizeof(epon_tk_rule_t) - ONU_MAX_CONDITION_RULE * sizeof(epon_tk_rule_condition_t);
    length += tk_manage.record[g_tk_rule_getindex].rule.tk.num_rule * sizeof(epon_tk_rule_condition_t);
    memcpy(rsp, &tk_manage.record[g_tk_rule_getindex].rule, length);
    *p_len = length;
    g_tk_rule_getindex++;

    return CA_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_loopback_enable_adapt                                */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_loopback_enable_adapt(
    oam_port_id_t  port,
    oam_boolean  enable
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = OAM_E_OK;
    ca_port_id_t ca_port;

    OAM_UNI_PORT_ID_CHECK(port);
    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);

    rc = ca_port_loopback_set(0, ca_port, enable);
    if(rc != OAM_E_OK)
            return rc;

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_mac_limit_set_adapt                                  */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_mac_limit_set_adapt(
    oam_uint8   port,
    oam_uint16  mac_limit
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_port_id_t ca_port;

    if((port > OAM_UNI_PORT_NUM)  && (port != 0x10)){
        return OAM_E_ERROR;
    }

    if(mac_limit == 64 || mac_limit == 128)
    {
        mac_limit = 128;
    }

    if((port == 0) || (port == 0x10)){
        return ca_l2_mac_limit_set(0, CA_L2_MAC_LIMIT_SYSTEM, 0, mac_limit);
    }
    else{
        ca_port = OAM_PORT_ID_TO_CA_PORT(port);
        return ca_l2_mac_limit_set(0, CA_L2_MAC_LIMIT_PORT, ca_port, mac_limit);
    }
    return 0;
}

oam_status tk_oam_mac_limit_get_adapt(
    oam_uint8   port,
    oam_uint16  *mac_limit
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
#if 0
    ca_callback_context_t context;
    oam_uint32 mac_nums = 0;

    if (NULL == mac_limit)
        return OAM_E_ERROR;

    if((port > OAM_UNI_PORT_ID4)  && (port != 0X10)){
        return OAM_E_ERROR;
    }

    memset(&context, 0, sizeof(context));
    if((port == 0) || (port == 0X10)){
        if (epon_request_onu_fdb_mac_limit_get(context, 0, 0, OAM_UNI_PORT_ID1, &mac_nums) != OAM_E_OK)
            return OAM_E_ERROR;
    }
    else{
        if (epon_request_port_fdb_mac_limit_get(context, 0, 0, port, &mac_nums) != OAM_E_OK)
            return OAM_E_ERROR;
    }

    *mac_limit = mac_nums;
#endif
    ca_port_id_t ca_port;
    ca_uint32_t mac_nums = 0;

    if (NULL == mac_limit)
        return OAM_E_ERROR;

    if((port > OAM_UNI_PORT_NUM)  && (port != 0x10)){
        return OAM_E_ERROR;
    }

    if((port == 0) || (port == 0x10)){
        ca_port = OAM_PORT_ID_TO_CA_PORT(OAM_UNI_PORT_ID1);
        if (ca_l2_mac_limit_get(0, 0, ca_port, &mac_nums) != OAM_E_OK)
            return OAM_E_ERROR;
    }
    else{
        ca_port = OAM_PORT_ID_TO_CA_PORT(port);
        if (ca_l2_mac_limit_get(0, 0, ca_port, &mac_nums) != OAM_E_OK)
            return OAM_E_ERROR;
    }

    *mac_limit = mac_nums;

    return OAM_E_OK;
}

oam_uint8 oam_port_limit_00d1[4 + 1];
oam_status tk_oam_mac_limit_00d1_set_adapt(
    oam_uint8   port,
    oam_uint16  mac_limit
)
{
    oam_uint8 port_id = 0;
    if((port == 0) || (port == 0x10))
        port_id = 0;
    else
        port_id = port;
    if(port_id < OAM_UNI_PORT_NUM_MAX + 1)
        oam_port_limit_00d1[port_id] = mac_limit;
    return OAM_E_OK;
}
oam_status tk_oam_mac_limit_00d1_get_adapt(
    oam_uint8   port,
    oam_uint16  *mac_limit
)
{
    oam_uint8 port_id = 0;
    if((port == 0) || (port == 0x10))
        port_id = 0;
    else
        port_id = port;
    if(port_id < OAM_UNI_PORT_NUM_MAX + 1)
        *mac_limit = oam_port_limit_00d1[port_id];
    return OAM_E_OK;
}
/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_fdb_age_time_set_adapt                               */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_fdb_age_time_set_adapt(
    oam_uint16  age_time
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    g_ageing_time = age_time;
    //ca_l2_aging_time_set(0, age_time);
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_fdb_age_time_get_adapt                               */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_fdb_age_time_get_adapt(
    oam_uint16  *age_time
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    if (NULL == age_time)
        return OAM_E_ERROR;

    *age_time = g_ageing_time;
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_lrn_mode_set_adapt                                   */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_lrn_mode_set_adapt(
    oam_uint8  port,
    oam_uint8  lrn_mode
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    if(port > OAM_UNI_PORT_NUM_MAX)
        return OAM_E_ERROR;
    g_learning_mode[port] = lrn_mode;
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_lrn_mode_get_adapt                                   */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_lrn_mode_get_adapt(
    oam_uint8  port,
    oam_uint8  *lrn_mode
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_port_id_t 	ca_port;
    ca_uint32_t mac_nums;
    if (NULL == lrn_mode || port > OAM_UNI_PORT_NUM_MAX || port < 1)
        return OAM_E_ERROR;

    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);
    if (ca_l2_mac_limit_get(0, 0, ca_port, &mac_nums) != OAM_E_OK)
        return OAM_E_ERROR;
    *lrn_mode = 1;
    if (mac_nums >= KT_TK_MAX_MAC_PER_PORT)
        *lrn_mode = 0;
    //*lrn_mode = g_learning_mode[port];
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_uni_phy_admin_adapt                                  */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_uni_phy_admin_adapt(
    oam_uint8  port,
    oam_uint8  enable
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status 		rc = OAM_E_OK;
    ca_port_id_t 	ca_port;

    OAM_UNI_PORT_ID_CHECK(port);
    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ca_port_enable_set(0, ca_port, enable);

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_flow_ctrl_enable_adapt                          */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_flow_ctrl_enable_adapt(
    oam_uint8  port,
    oam_uint8  enable
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = OAM_E_OK;
    ca_port_id_t ca_port;
    ca_eth_port_ability_t ability;

    OAM_UNI_PORT_ID_CHECK(port);
    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);

    if(port == 4)   /* PORT 4 is external PHY, need to set PHY ability */
    {
        OAM_ORG_LOG("ca_eth_port_phy_advert_set port %x, enable %d\r\n", ca_port, enable);
        memset(&ability, 0, sizeof(ca_eth_port_ability_t));
        rc = ca_eth_port_phy_advert_get(0, ca_port, &ability);
        ability.flow_ctrl = enable;
        ability.asym_flow_ctrl = enable;
        ca_eth_port_phy_advert_set(0, ca_port, ability);
    }

    OAM_ORG_LOG( "ca_eth_port_pause_set: port 0x%x, enable %d\r\n", ca_port, enable);

    g_flow_control[port-1] = enable;

    if (port == 1 || port == 4)
        ca_eth_port_pause_set(0, ca_port, FALSE, enable, enable);
    else
        ca_eth_port_pause_set(0, ca_port, FALSE, TRUE, enable);

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_auto_admin_set_adapt                            */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_auto_admin_set_adapt(
    oam_uint8  port,
    oam_uint8  enable
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = OAM_E_OK;
    ca_port_id_t ca_port;
    ca_eth_port_ability_t ability;
    ca_eth_port_speed_t speed;
    ca_eth_port_duplex_t duplex;

    OAM_UNI_PORT_ID_CHECK(port);
    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);

    /* when set auto-neg on, set phy abilities to max */
    if(enable){
        rc = ca_eth_port_phy_advert_get(0, ca_port, &ability);
        ability.half_10 = 1;
        ability.full_10 = 1;
        ability.half_100 = 1;
        ability.full_100 = 1;
        ability.full_1g = 1;
        if(CA_PORT_IS_GE(ca_port)){
#if 0
            /* workaround start */
            speed = CA_PORT_SPEED_1G;
            duplex = CA_PORT_DUPLEX_FULL;
            OAM_ORG_LOG( "\r\nca_eth_port_phy_speed_set: port 0x%x, speed %d\r\n", ca_port, speed);
            rc = ca_eth_port_phy_speed_set (0, ca_port, speed);
            OAM_ORG_LOG( "\r\ca_eth_port_phy_duplex_set: port 0x%x, duplex %d\r\n", ca_port, duplex);
            rc = ca_eth_port_phy_duplex_set (0, ca_port, duplex);
            /* workround end */
#endif
        }else if(CA_PORT_IS_XGE(ca_port)){
            ability.full_10g = 1;
            ability.full_2d5g = 1;
            ability.full_5g = 1;
        }
        //ability.flow_ctrl = 1;
        //ability.asym_flow_ctrl = 1;

        OAM_ORG_LOG( "\r\nca_eth_port_phy_advert_set: port 0x%x, abiltity to max\r\n", ca_port);
        rc = ca_eth_port_phy_advert_set(0, ca_port, ability);
    }

    OAM_ORG_LOG( "\r\nca_eth_port_phy_autoneg_set: port 0x%x, enable %d\r\n", ca_port, enable);
    rc = ca_eth_port_autoneg_set (0, ca_port, enable);

    rc = ca_eth_port_phy_autoneg_set (0, ca_port, enable);

    return OAM_E_OK;
}

#if (defined(CONFIG_ARCH_CORTINA_SATURN_SFU) && !defined(CONFIG_ARCH_CORTINA_SATURN_SFP_PLUS))
oam_status    __rtl_port_phy_ability_set (ca_uint32 ca_port, oam_uint16   param )
{
    oam_status              rc = 0;
    ca_eth_port_speed_t     speed = CA_PORT_SPEED_INVALID;
    ca_eth_port_duplex_t    duplex = CA_PORT_DUPLEX_AUTO;
    ca_boolean_t            rtl_port_flag = FALSE;
    rtl_port_phy_ability_t  rtl_phy_ability;
    rtl_port_mac_ability_t  mac_ability;

    app_rtl_port_phy_ability_get( 0, ca_port, &rtl_phy_ability);

    switch(param){
        case TK_OAM_AUTO_ENG_ADV_TECH_10M_HALF:
            rtl_phy_ability.Full_1000 = 0;
            rtl_phy_ability.Full_100 = 0;
            rtl_phy_ability.Half_100 = 0;
            rtl_phy_ability.Full_10 = 0;
            rtl_phy_ability.Half_10 = 1;

            speed = CA_PORT_SPEED_10M;
            duplex = CA_PORT_DUPLEX_HALF;
            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_10M_FULL:
            rtl_phy_ability.Full_1000 = 0;
            rtl_phy_ability.Full_100 = 0;
            rtl_phy_ability.Half_100 = 0;
            rtl_phy_ability.Full_10 = 1;
            rtl_phy_ability.Half_10 = 0;

            speed = CA_PORT_SPEED_10M;
            duplex = CA_PORT_DUPLEX_FULL;

            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_100M_HALF:
            rtl_phy_ability.Full_1000 = 0;
            rtl_phy_ability.Full_100 = 0;
            rtl_phy_ability.Half_100 = 1;
            rtl_phy_ability.Full_10 = 0;
            rtl_phy_ability.Half_10 = 0;

            speed = CA_PORT_SPEED_100M;
            duplex = CA_PORT_DUPLEX_HALF;

            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_100M_FULL:
            rtl_phy_ability.Full_1000 = 0;
            rtl_phy_ability.Full_100 = 1;
            rtl_phy_ability.Half_100 = 0;
            rtl_phy_ability.Full_10 = 0;
            rtl_phy_ability.Half_10 = 0;

            speed = CA_PORT_SPEED_100M;
            duplex = CA_PORT_DUPLEX_FULL;

            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_1G:
        case TK_OAM_AUTO_ENG_ADV_TECH_1G_FULL:
            rtl_phy_ability.Full_1000 = 1;
            rtl_phy_ability.Full_100 = 0;
            rtl_phy_ability.Half_100 = 0;
            rtl_phy_ability.Full_10 = 0;
            rtl_phy_ability.Half_10 = 0;

            speed = CA_PORT_SPEED_1G;
            duplex = CA_PORT_DUPLEX_FULL;

            break;

         default:
            ca_printf ("\r\nnot support type 0x%x \r\n", param);
            break;
    }

    app_rtl_port_phy_ability_set( 0, ca_port,&rtl_phy_ability);



    return OAM_E_OK;

}
#endif

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_auto_neg_tech_adv_set_adapt                     */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_auto_neg_tech_adv_set_adapt(
    oam_uint8  port,
    oam_uint16  param
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = 0;
    ca_boolean_t enable = TRUE ;
    ca_port_id_t ca_port;
    ca_eth_port_ability_t ability;
    ca_eth_port_speed_t speed;
    ca_eth_port_duplex_t duplex;

    OAM_UNI_PORT_ID_CHECK(port);
    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);

    if (param == TK_OAM_PORT_MAC_CTRL_ENABLE) {
        rc = tk_oam_port_flow_ctrl_enable_adapt(port, TRUE);
        return rc;
    }
    else {
        tk_oam_port_flow_ctrl_enable_adapt(port, FALSE);

    }

    rc = ca_eth_port_phy_advert_get(0, ca_port, &ability);
    if(rc == CA_E_OK){
        switch(param){

        case TK_OAM_AUTO_ENG_ADV_TECH_10M_HALF:
            ability.half_10   = 1;
            ability.full_10   = 0;
            ability.half_100  = 0;
            ability.full_100  = 0;
            ability.full_1g   = 0;
            ability.full_2d5g = 0;
            ability.full_5g   = 0;
            ability.full_10g  = 0;
            speed = CA_PORT_SPEED_10M;
            duplex = CA_PORT_DUPLEX_HALF;
            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_10M_FULL:
            ability.half_10   = 0;
            ability.full_10   = 1;
            ability.half_100  = 0;
            ability.full_100  = 0;
            ability.full_1g   = 0;
            ability.full_2d5g = 0;
            ability.full_5g   = 0;
            ability.full_10g  = 0;
            speed = CA_PORT_SPEED_10M;
            duplex = CA_PORT_DUPLEX_FULL;
            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_100M_HALF:
            ability.half_10   = 0;
            ability.full_10   = 0;
            ability.half_100  = 1;
            ability.full_100  = 0;
            ability.full_1g   = 0;
            ability.full_2d5g = 0;
            ability.full_5g   = 0;
            ability.full_10g  = 0;
            speed = CA_PORT_SPEED_100M;
            duplex = CA_PORT_DUPLEX_HALF;
            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_100M_FULL:
            ability.half_10   = 0;
            ability.full_10   = 0;
            ability.half_100  = 0;
            ability.full_100  = 1;
            ability.full_1g   = 0;
            ability.full_2d5g = 0;
            ability.full_5g   = 0;
            ability.full_10g  = 0;
            speed = CA_PORT_SPEED_100M;
            duplex = CA_PORT_DUPLEX_FULL;
            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_1G:
        case TK_OAM_AUTO_ENG_ADV_TECH_1G_FULL:
            ability.half_10   = 0;
            ability.full_10   = 0;
            ability.half_100  = 0;
            ability.full_100  = 0;
            ability.full_1g   = 1;
            ability.full_2d5g = 0;
            ability.full_5g   = 0;
            ability.full_10g  = 0;
            speed = CA_PORT_SPEED_1G;
            duplex = CA_PORT_DUPLEX_FULL;
            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_2_5G:
            ability.half_10   = 0;
            ability.full_10   = 0;
            ability.half_100  = 0;
            ability.full_100  = 0;
            ability.full_1g   = 0;
            ability.full_2d5g = 1;
            ability.full_5g   = 0;
            ability.full_10g  = 0;
            speed = CA_PORT_SPEED_2_5G;
            duplex = CA_PORT_DUPLEX_FULL;
            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_5G:
            ability.half_10   = 0;
            ability.full_10   = 0;
            ability.half_100  = 0;
            ability.full_100  = 0;
            ability.full_1g   = 0;
            ability.full_2d5g = 0;
            ability.full_5g   = 1;
            ability.full_10g  = 0;
            speed = CA_PORT_SPEED_5G;
            duplex = CA_PORT_DUPLEX_FULL;
            break;

        case TK_OAM_AUTO_ENG_ADV_TECH_10G:
            ability.half_10   = 0;
            ability.full_10   = 0;
            ability.half_100  = 0;
            ability.full_100  = 0;
            ability.full_1g   = 0;
            ability.full_2d5g = 0;
            ability.full_5g   = 0;
            ability.full_10g  = 1;
            speed = CA_PORT_SPEED_10G;
            duplex = CA_PORT_DUPLEX_FULL;
            break;
        }

        OAM_ORG_LOG( "\r\nca_eth_port_phy_advert_set: port 0x%x\r\n", ca_port);
        rc = ca_eth_port_phy_advert_set(0, ca_port, ability);
        if(rc != CA_E_OK){
            ca_printf("ca_eth_port_phy_advert_set port %#x fail, return %d\r\n", ca_port, rc);
            //return OAM_E_ERROR;
        }
    }

    ca_eth_port_autoneg_get(0, ca_port, &enable);
    if(!enable){
        OAM_ORG_LOG( "\r\nca_eth_port_phy_speed_set: port 0x%x, speed %d (1: 10M, 2: 200M, 3: 1G, 4: 2.5G, 5: 10G)\r\n", ca_port, speed);
        ca_eth_port_speed_set(0, ca_port, speed);
        ca_eth_port_phy_speed_set(0, ca_port, speed);
        OAM_ORG_LOG( "\r\nca_eth_port_phy_duplex_set: port 0x%x, duplex %d (0: half, 1: full)\r\n", ca_port, duplex);
        ca_eth_port_duplex_set(0, ca_port, duplex);
        ca_eth_port_phy_duplex_set(0, ca_port, duplex);
    }
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_auto_neg_tech_adv_get_adapt                     */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_auto_neg_tech_adv_get_adapt(
    oam_uint8  port,
    oam_uint16  *rsp_value
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = 0;
    ca_port_id_t ca_port;
    ca_eth_port_speed_t speed;
    ca_eth_port_ability_t ability;

    OAM_UNI_PORT_ID_CHECK(port);
    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);

    rc = ca_eth_port_phy_speed_get(0, ca_port, &speed);
    if(rc != CA_E_OK){
        return OAM_E_ERROR;
    }
    if(speed == CA_PORT_SPEED_10G){
        *rsp_value = TK_OAM_AUTO_ENG_ADV_TECH_10G;

    }else if(speed == CA_PORT_SPEED_5G){
        *rsp_value = TK_OAM_AUTO_ENG_ADV_TECH_5G;

    }else if(speed == CA_PORT_SPEED_2_5G){
        *rsp_value = TK_OAM_AUTO_ENG_ADV_TECH_2_5G;

    }else if(speed == CA_PORT_SPEED_1G){
        *rsp_value = TK_OAM_AUTO_ENG_ADV_TECH_1G;

    }else{

        rc = ca_eth_port_phy_advert_get(0, ca_port, &ability);
        if(rc != CA_E_OK){
            return OAM_E_ERROR;
        }
        if (ability.half_100) {
            *rsp_value = TK_OAM_AUTO_ENG_ADV_TECH_100M_HALF;

        } else if (ability.full_100) {
            *rsp_value = TK_OAM_AUTO_ENG_ADV_TECH_100M_FULL;

        } else if (ability.half_10) {
            *rsp_value = TK_OAM_AUTO_ENG_ADV_TECH_10M_HALF;

        } else if (ability.full_10) {
            *rsp_value = TK_OAM_AUTO_ENG_ADV_TECH_10M_FULL;

        } else {
            *rsp_value = TK_OAM_AUTO_ENG_ADV_TECH_100M_FULL;
        }
    }
    return rc;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_status_get_adapt                                */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_status_get_adapt(
    oam_uint8  port,
    tk_oam_port_status_t  *p_status
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = 0;
    ca_port_id_t ca_port;
    ca_uint16_t mac_ctrl;
    ca_eth_port_duplex_t duplex = CA_PORT_DUPLEX_FULL;
    ca_boolean_t auto_neg = TRUE;
    ca_boolean_t mac_admin = FALSE;
    ca_boolean_t pfc_enable = FALSE;
    ca_boolean_t pause_rx = FALSE;
    ca_boolean_t pause_tx = FALSE;
    ca_eth_port_link_status_t link;
    ca_eth_port_speed_t speed;

    OAM_UNI_PORT_ID_CHECK(port);
    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);


    /* aoto-neg */
    rc = ca_eth_port_autoneg_get(0, ca_port, &auto_neg);    
    if (rc) {
        goto end;
    }
    /* link status */
    rc = ca_eth_port_link_status_get(0, ca_port, &link);
    if (rc) {
        goto end;
    }
    /* speed and duplex */
    /* No matter auto-neg on or off, always reply PHY speed and duplex when link up */
    //if(auto_neg){
        if(link == CA_PORT_LINK_UP){

            rc = ca_eth_port_phy_speed_get(0, ca_port, &speed);
            if (rc) {
                goto end;
            }

            rc = ca_eth_port_phy_duplex_get(0, ca_port, &duplex);            
            if (rc) {
                goto end;
            }
        }
        else if(link == CA_PORT_LINK_DOWN)
        { /* reply max ability of PHY */
            if (port == 4)
            {
                speed = CA_PORT_SPEED_10G;
            }
            else
            {
                speed = CA_PORT_SPEED_1G;
            }
            duplex = CA_PORT_DUPLEX_FULL;
        }

    /* mac admin state */
    rc = ca_port_enable_get(0, ca_port, &mac_admin);
    if (rc) {
        goto end;
    }

    rc = ca_eth_port_pause_get(0, ca_port, &pfc_enable, &pause_rx, &pause_tx);
    if (rc) {
        goto end;
    }

    /* cortina API macro convert to OAM deifned value */
    OAM_ORG_LOG("\r\nCortina API return for port %d: auto_neg %d, link %d, speed %d, duplex %d, mac_admin %d, pause_rx %d, pause_tx %d\r\n",
        port, auto_neg, link, speed, duplex, mac_admin, pause_rx, pause_tx);
    OAM_ORG_LOG("SPEED - 0:invalid; 1:10M; 2:100M; 3:1G; 5:10G;  DUPLEX - 0:half;1:full\r\n");

    if (auto_neg) {
        p_status->auto_neg = TK_OAM_PORT_AUTO_NEG_ON;
    } else {
        p_status->auto_neg = TK_OAM_PORT_AUTO_NEG_OFF;
    }
    if (link == CA_PORT_LINK_UP) {
        p_status->link = TK_OAM_PORT_LINK_UP;
    } else {
        p_status->link = TK_OAM_PORT_LINK_DOWN;
    }
    if (speed == CA_PORT_SPEED_10G) {
        p_status->phy_type = TK_OAM_STD_PHY_TYPE_10G_BASE_X;

    }else if (speed == CA_PORT_SPEED_5G) {
        p_status->phy_type = TK_OAM_STD_PHY_TYPE_5G;

    }else if (speed == CA_PORT_SPEED_2_5G) {
        p_status->phy_type = TK_OAM_STD_PHY_TYPE_2_5G;

    }else if (speed == CA_PORT_SPEED_1G) {
        p_status->phy_type = TK_OAM_STD_PHY_TYPE_1G;

    } else if (speed == CA_PORT_SPEED_100M) {
        p_status->phy_type = TK_OAM_STD_PHY_TYPE_100M;

    } else if (speed == CA_PORT_SPEED_10M) {
        p_status->phy_type = TK_OAM_STD_PHY_TYPE_10M;;
    }else{
        ca_printf("unkonw phy speed %d\r\n", speed);
        p_status->phy_type = TK_OAM_STD_PHY_TYPE_1G;
    }
    if (duplex == CA_PORT_DUPLEX_HALF) {
        p_status->duplex = TK_OAM_PORT_DUPLEX_MODE_HALF;
    } else {
        p_status->duplex = TK_OAM_PORT_DUPLEX_MODE_FULL;
    }

    p_status->mac_admin = mac_admin;

    if (g_flow_control[port - 1]) {
        p_status->mac_ctrl = TK_OAM_PORT_MAC_CTRL_ENABLE;
    } else {
        p_status->mac_ctrl = TK_OAM_PORT_MAC_CTRL_DISABLE;
    }

end:

    return rc;
}

/* TK VLAN configuration */

/* When OLT add a VLAN to a port, All VLAN member of this port will be added one by one in each branch/leaf(07 00bd)
   So need to wait this OAMPDU has been parsered done, then we can known all VLAN member of this port.

   After this OAMPDU has been parsered done, check the difference of VLAN member with the last time,
   then config the difference VLAN info to Hardware.
   For the VLAN member which has no update, do nothing to avoid packets drop.

   CPU port also need to add to VLAN as there's VLAN membership check before packets trapped to CPU

   CPU0 egress_tag mode is the same as PON port;
   CPU1 egress_tag mode is always untag;
        For ARP, ICMP, Linux protocol stack can only handle untag packets.
        For DHCP, Application get VLAN tag from FDB entry.
        For IGMP, packets are mirrored to CPU, VLAN Engine is bypassed, original tag is always kept.

   When Add/Remove PON port from a VLAN, Add/Remove CPU0 together.

   When Add PON/LAN port to a VLAN, add CPU1 to this VLAN
   When Remove PON/LAN port from a VLAN, check if there're ports in this VLAN,
        if not, delete it, because VLAN resource is limited */

#define OAM_TK_PORT_VLAN_MODE_ACCESS    0
#define OAM_TK_PORT_VLAN_MODE_TRUNK     1

typedef struct {
    oam_uint8 valid;
    oam_uint8 no_update; /* vlan info is the same as last time */
    epon_tk_port_vlan_member_config_t cfg;  /* VLAN member configured by OAM */
} oam_tk_vlan_mbr_t;

typedef struct{
    oam_uint32 mode;    /* access or trunk mode */
    oam_uint16 pvid;
    oam_uint8 cfg_flag; /* need to config vlan member to hardware */
    oam_uint8 debug;
    oam_tk_vlan_mbr_t vlan_mbr[OAM_TK_PORT_VLAN_NUM_MAX];
    oam_tk_vlan_mbr_t vlan_mbr_last[OAM_TK_PORT_VLAN_NUM_MAX];
}oam_tk_vlan_ctrl_t;

oam_tk_vlan_ctrl_t __tk_vlan_ctrl[1+OAM_UNI_PORT_NUM_MAX]; /* index0 for PON, index1~4 for LAN */

static void __sw_vlan_member_get(oam_uint16 port, ca_uint16 *vlan_counter, epon_tk_port_vlan_member_config_t *vlan_cfg)
{
    oam_uint32 i;
    oam_uint32 counter = 0;

    oam_tk_vlan_mbr_t *vlan_mbr;

    vlan_mbr = (oam_tk_vlan_mbr_t *)__tk_vlan_ctrl[port].vlan_mbr_last;

    for (i = 0; i < OAM_TK_PORT_VLAN_NUM_MAX; i++)
    {
        if (vlan_mbr[i].valid)
        {
            memcpy(&vlan_cfg[counter], &(vlan_mbr[i].cfg), sizeof(epon_tk_port_vlan_member_config_t));
            counter++;
        }
    }
    *vlan_counter = counter;
}

static void __sw_vlan_member_add(oam_uint16 port, epon_tk_port_vlan_member_config_t *cfg)
{
    oam_uint32 i;
    oam_tk_vlan_mbr_t *vlan_mbr;

    vlan_mbr = (oam_tk_vlan_mbr_t *)__tk_vlan_ctrl[port].vlan_mbr;

    for (i = 0; i < OAM_TK_PORT_VLAN_NUM_MAX; i++)
    {
        if(vlan_mbr[i].valid &&
            (memcmp(&(vlan_mbr[i].cfg), cfg, sizeof(epon_tk_port_vlan_member_config_t)) == 0))
        {
            return;
        }
    }

    if(i == OAM_TK_PORT_VLAN_NUM_MAX)
    {
        vlan_mbr = (oam_tk_vlan_mbr_t *)__tk_vlan_ctrl[port].vlan_mbr;
        for (i = 0; i < OAM_TK_PORT_VLAN_NUM_MAX; i++)
        {
            if(!vlan_mbr[i].valid)
            {
                vlan_mbr[i].valid = 1;
                memcpy(&(vlan_mbr[i].cfg), cfg, sizeof(epon_tk_port_vlan_member_config_t));
                OAM_ORG_LOG("add vlan member. ingress_vlan %d, egress_vlan %d, ingress_tag %d, egress_tag %d\r\n\r\n",
                    cfg->ingress_vlan, cfg->egress_vlan, cfg->ingress_tag, cfg->egress_tag);
                return;
            }
        }
        if(i == OAM_TK_PORT_VLAN_NUM_MAX)
        {
            OAM_ORG_LOG("Error: Exceed the max VLAN number 16, VLAN %d add failed\r\n", cfg->ingress_vlan);
            return;
        }
    }
}

static void __sw_vlan_member_remove(oam_uint16 port, epon_tk_port_vlan_member_config_t *cfg)
{
    oam_uint32 i;
    oam_tk_vlan_mbr_t *vlan_mbr;

    vlan_mbr = (oam_tk_vlan_mbr_t *)__tk_vlan_ctrl[port].vlan_mbr;

    for (i = 0; i < OAM_TK_PORT_VLAN_NUM_MAX; i++)
    {
        if(vlan_mbr[i].valid && (memcmp(&(vlan_mbr[i].cfg), cfg, sizeof(epon_tk_port_vlan_member_config_t)) == 0))
        {
            memset(&vlan_mbr[i], 0, sizeof(oam_tk_vlan_mbr_t));
            return;
        }
    }
    if(i == OAM_TK_PORT_VLAN_NUM_MAX){
        OAM_ORG_LOG("Error: VLAN %d is not found\r\n", cfg->ingress_vlan);
    }
    return;
}

static void __sw_vlan_member_clear(oam_uint16 port)
{
    oam_tk_vlan_mbr_t *vlan_mbr;

    vlan_mbr = (oam_tk_vlan_mbr_t *)__tk_vlan_ctrl[port].vlan_mbr;
    memset(vlan_mbr, 0, sizeof(oam_tk_vlan_mbr_t) * OAM_TK_PORT_VLAN_NUM_MAX);
}

static oam_status __hw_vlan_port_add(
    oam_uint16      vid,
    ca_port_id_t    ca_port,
    oam_uint8       untag)
{
    oam_status ret = 0;
    oam_uint32 i;
    ca_port_id_t cpu0;
    ca_port_id_t member_ports[OAM_TK_PORT_VLAN_NUM_MAX];
    oam_uint8 member_count = 0;
    oam_uint8 untagged_count = 0;
    ca_port_id_t untagged_ports[OAM_TK_PORT_VLAN_NUM_MAX];

    memset(member_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
    memset(untagged_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);

    ca_l2_vlan_port_get(0, vid, &member_count, member_ports, &untagged_count, untagged_ports);
    OAM_ORG_LOG("ca_l2_vlan_port_get vlan %d, member_count %d, untagged_count %d\r\n", vid, member_count, untagged_count);

    OAM_ORG_LOG("HW add port 0x%x to vlan %d membership \r\n", ca_port, vid);
    member_ports[member_count] = ca_port;
    member_count ++;
    if(untag){
        untagged_ports[untagged_count] = ca_port;
        untagged_count++;
    }
    if(ca_port == CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7))
    {
        OAM_ORG_LOG("HW add CPU0 to vlan %d membership \r\n", vid);
        cpu0 = CA_PORT_ID(CA_PORT_TYPE_CPU, CA_PORT_ID_CPU0);
        member_ports[member_count] = cpu0;
        member_count ++;
        if(untag){
            untagged_ports[untagged_count] = cpu0;
            untagged_count++;
        }
    }

    OAM_ORG_LOG("ca_l2_vlan_port_set vlan %d, member_count %d, untagged_count %d\r\n\r\n", vid, member_count, untagged_count);

    ret = ca_l2_vlan_port_set(0,vid, member_count, member_ports, untagged_count, untagged_ports);
}

/* remove port from VLAN */
static oam_status __hw_vlan_port_remove(
    oam_uint16      vid,
    ca_port_id_t    ca_port)
{
    oam_status ret = 0;
    oam_uint32 i, j;
    ca_port_id_t cpu0;
    ca_port_id_t member_ports[OAM_TK_PORT_VLAN_NUM_MAX];
    oam_uint8 member_count = 0;
    oam_uint8 untagged_count = 0;
    ca_port_id_t untagged_ports[OAM_TK_PORT_VLAN_NUM_MAX];

    ca_port_id_t new_member_ports[OAM_TK_PORT_VLAN_NUM_MAX];
    oam_uint8 new_member_count = 0;
    oam_uint8 new_untagged_count = 0;
    ca_port_id_t new_untagged_ports[OAM_TK_PORT_VLAN_NUM_MAX];

    memset(member_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
    memset(new_member_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
    memset(untagged_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
    memset(new_untagged_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);

    ca_l2_vlan_port_get(0, vid, &member_count, member_ports, &untagged_count, untagged_ports);
    OAM_ORG_LOG("ca_l2_vlan_port_get vlan %d, member_count %d, untagged_count %d\r\n", vid, member_count, untagged_count);

    cpu0 = CA_PORT_ID(CA_PORT_TYPE_CPU, CA_PORT_ID_CPU0);
    for(i = 0; i < member_count; i++)
    {
        if(member_ports[i] == ca_port)
        {
            OAM_ORG_LOG("HW remove port 0x%x from vlan %d membership \r\n", ca_port, vid);
            continue;
        }
        else
        {
            if(ca_port == CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7) &&
                member_ports[i] == cpu0 )
            {
                OAM_ORG_LOG("HW remove CPU0 from vlan %d membership \r\n", vid);
                continue;
            }
            new_member_ports[new_member_count] = member_ports[i];
            new_member_count ++;
        }
    }

    for(j = 0; j < untagged_count; j++)
    {
        if(untagged_ports[j] == ca_port)
        {
            OAM_ORG_LOG("HW remove port 0x%x from vlan %d untag membership \r\n", ca_port, vid);
            continue;
        }
        else
        {
            if(ca_port == CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7) &&
                untagged_ports[j] == cpu0 )
            {
                OAM_ORG_LOG("HW remove CPU0 from vlan %d untag membership \r\n", vid);
                continue;
            }
            new_untagged_ports[new_untagged_count] = untagged_ports[j];
            new_untagged_count ++;
        }
    }
    OAM_ORG_LOG("ca_l2_vlan_port_set vlan %d, member_count %d, untagged_count %d\r\n\r\n", vid, new_member_count,
        new_untagged_count);
    ret = ca_l2_vlan_port_set(0,vid, new_member_count, new_member_ports, new_untagged_count, new_untagged_ports);
    return ret;
}

static oam_status __vlan_port_add(
    oam_uint16 vid,
    oam_uint16 port,
    oam_uint8 untag)
{
    oam_status ret = 0;
    oam_uint32 i;
    ca_port_id_t ca_port;
    ca_port_id_t cpu1;
    oam_uint8 cpu1_flag = 0;

    ca_port_id_t member_ports[OAM_TK_PORT_VLAN_NUM_MAX];
    oam_uint8 member_count = 0;
    oam_uint8 untagged_count = 0;
    ca_port_id_t untagged_ports[OAM_TK_PORT_VLAN_NUM_MAX];

    OAM_PORT_ID_CHECK(port);

    ret = ca_l2_vlan_create(0,vid);

    ca_port = OAM_PORT_ID_TO_CA_PORT(port);
    __hw_vlan_port_add(vid, ca_port, untag);

    /* ADD CPU1 port to VID */
    memset(member_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
    memset(untagged_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);

    ca_l2_vlan_port_get(0, vid, &member_count, member_ports, &untagged_count, untagged_ports);
    //OAM_ORG_LOG("ca_l2_vlan_port_get vlan %d member count %d untagged_count %d\r\n", vid, member_count, untagged_count);

    cpu1 = CA_PORT_ID(CA_PORT_TYPE_CPU, CA_PORT_ID_CPU1);
    if(ret == CA_E_EXISTS)  /* check if cpu1 port is existed */
    {
        for(i = 0; i < member_count; i++)
        {
            if(member_ports[i] == cpu1)
            {
                cpu1_flag = 1;
                break;
            }
        }
    }else if(ret == CA_E_OK)/* the first time create VLAN */
    {
        cpu1_flag = 0;
    }else
    {
        return ret;
    }

    if(!cpu1_flag)
    {
        OAM_ORG_LOG("HW add cpu1 to vlan %d membership \r\n");
        member_ports[member_count] = cpu1;
        member_count ++;

        untagged_ports[untagged_count] = cpu1;
        untagged_count++;

        OAM_ORG_LOG("ca_l2_vlan_port_set vlan %d, member_count %d, untagged_count %d, untag %d\r\n\r\n", vid, member_count,
            untagged_count, untag);
        ret = ca_l2_vlan_port_set(0,vid, member_count, member_ports, untagged_count, untagged_ports);
    }

    return ret;

}

/* remove port from VLAN */
static oam_status __vlan_port_remove(
    oam_uint16 vid,
    oam_uint16 port)
{
    oam_status ret = 0;
    ca_port_id_t ca_port, cpu0, cpu1;
    oam_uint32 i = 0;
    oam_uint32 port_num = 0;
    ca_port_id_t member_ports[OAM_TK_PORT_VLAN_NUM_MAX];
    oam_uint8 member_count = 0;
    oam_uint8 untagged_count = 0;
    ca_port_id_t untagged_ports[OAM_TK_PORT_VLAN_NUM_MAX];

    OAM_PORT_ID_CHECK(port);
    ca_port = OAM_PORT_ID_TO_CA_PORT(port);

    __hw_vlan_port_remove(vid, ca_port);

    /* if only CPU0 or CPU1 in VLAN, delete this VLAN */
    memset(member_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
    memset(untagged_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);

    ca_l2_vlan_port_get(0, vid, &member_count, member_ports, &untagged_count, untagged_ports);
    //OAM_ORG_LOG("\r\nca_l2_vlan_port_get vlan %d member count %d untagged_count %d\r\n", vid, member_count, untagged_count);

    cpu0 = CA_PORT_ID(CA_PORT_TYPE_CPU, CA_PORT_ID_CPU0);
    cpu1 = CA_PORT_ID(CA_PORT_TYPE_CPU, CA_PORT_ID_CPU1);

    for(i = 0; i < member_count; i++)
    {

        if(member_ports[i] != cpu0 && member_ports[i] != cpu1)
        {
            port_num++;
        }
    }
    if(port_num == 0)
    {
        OAM_ORG_LOG("No ports in vlan %d, delete this vlan\r\n\r\n", vid);
        ca_l2_vlan_delete(0, vid);
        return ret;
    }
    return ret;
}

void tk_oam_vlan_debug_set(oam_uint8 enable)
{
    oam_uint32 i = 0;

    for(i = 0; i < (OAM_UNI_PORT_NUM+1); i++)
    {
        __tk_vlan_ctrl[i].debug = enable;
    }
    return;
}

void tk_oam_sw_vlan_member_printf(oam_uint16 port)
{
    oam_uint32 i;
    oam_tk_vlan_mbr_t *vlan_mbr;

    vlan_mbr = (oam_tk_vlan_mbr_t *)__tk_vlan_ctrl[port].vlan_mbr;

    ca_printf("\r\nport %d, vlan member info: \r\n", port);
    for(i = 0; i < OAM_TK_PORT_VLAN_NUM_MAX; i++)
    {
        if(vlan_mbr[i].valid)
            ca_printf(" %d, ingress_vlan %d, egress_vlan %d, ingress_tag %d, egress_tag %d \r\n",
                i,
                vlan_mbr[i].cfg.ingress_vlan, vlan_mbr[i].cfg.egress_vlan, vlan_mbr[i].cfg.ingress_tag, vlan_mbr[i].cfg.egress_tag);
    }

    ca_printf("\r\nport %d, vlan member info last: \r\n", port);
    vlan_mbr = (oam_tk_vlan_mbr_t *)__tk_vlan_ctrl[port].vlan_mbr_last;
    for(i = 0; i < OAM_TK_PORT_VLAN_NUM_MAX; i++)
    {
        if(vlan_mbr[i].valid)
            ca_printf(" %d, ingress_vlan %d, egress_vlan %d, ingress_tag %d, egress_tag %d \r\n",
                i,
                vlan_mbr[i].cfg.ingress_vlan, vlan_mbr[i].cfg.egress_vlan, vlan_mbr[i].cfg.ingress_tag, vlan_mbr[i].cfg.egress_tag);
    }
    ca_printf("\r\n");
}

/* SET PVID and trunk/access mode */
/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_vlan_policy_set_adapt                           */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_vlan_policy_set_adapt(
    oam_uint16  port,
    epon_tk_port_vlan_policy_config_t * vlan_policy
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = 0;
    ca_port_id_t ca_port;
    ca_vlan_port_control_t config;

    OAM_PORT_ID_CHECK(port);
    ca_port = OAM_PORT_ID_TO_CA_PORT(port);

    if(__tk_vlan_ctrl[port].pvid == vlan_policy->vlan){
        return OAM_E_OK;
    }

    if(vlan_policy->egress_policy == 3)
    {
        __tk_vlan_ctrl[port].mode = OAM_TK_PORT_VLAN_MODE_TRUNK;
    }else
    {
        __tk_vlan_ctrl[port].mode = OAM_TK_PORT_VLAN_MODE_ACCESS;
    }

    OAM_ORG_LOG("ca_port_pvid_vlan_set port 0x%x vid %d\r\n", ca_port, vlan_policy->vlan);

    rc = ca_port_pvid_vlan_set(0, ca_port, vlan_policy->vlan);
    if(rc != OAM_E_OK)
        return rc;

    __tk_vlan_ctrl[port].pvid = vlan_policy->vlan;

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_vlan_policy_get_adapt                           */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_vlan_policy_get_adapt(
    oam_uint16  port,
    epon_tk_port_vlan_policy_config_t * rsp_value
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = 0;
    ca_port_id_t ca_port;
    ca_uint32_t vid = 0;

    OAM_PORT_ID_CHECK(port);
    ca_port = OAM_PORT_ID_TO_CA_PORT(port);

    rc = ca_port_pvid_vlan_get(0, ca_port, &vid);
    if(rc != OAM_E_OK)
            return rc;

    if(__tk_vlan_ctrl[port].mode == OAM_TK_PORT_VLAN_MODE_TRUNK)
    {
    rsp_value->ingress_policy = 3;
    rsp_value->egress_policy = 3;
    }else
    {
        rsp_value->ingress_policy = 0;
        rsp_value->egress_policy = 0;
    }
    rsp_value->vlan = vid;

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_vlan_member_set_adapt                           */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_vlan_member_set_adapt(
    oam_uint16  port,
    epon_tk_port_vlan_member_config_t * vlan_mbr
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = 0;

    OAM_PORT_ID_CHECK(port);

    /* VLAN member is added one by one in each branch/leaf.
       So first add this VLAN member to software table, after this OAMPDU has been parsered done,
       config VLAN info to hardware.*/
    __tk_vlan_ctrl[port].cfg_flag = 1;
    __sw_vlan_member_add(port, vlan_mbr);

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_vlan_member_get_adapt                           */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_vlan_member_get_adapt(
    oam_uint16  port,
    oam_uint16* vlan_count,
    epon_tk_port_vlan_member_config_t * rsp_value
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    OAM_PORT_ID_CHECK(port);
    __sw_vlan_member_get(port, vlan_count, rsp_value);

    return OAM_E_OK;
}

void tk_oam_vlan_member_cfg_adapt(oam_uint16 port)
    {
    oam_uint32 i = 0, j =0;
    oam_uint8 untag = 0;
    oam_tk_vlan_mbr_t *vlan_mbr;
    oam_tk_vlan_mbr_t *vlan_mbr_last;

    if(port >= (1 + OAM_UNI_PORT_NUM_MAX))
    {
        return;
    }

    if(__tk_vlan_ctrl[port].cfg_flag == 0)
    { /* no need to do vlan config */
        //ca_printf("\r\nno need to do vlan config\r\n");
        return;
    }
    OAM_ORG_LOG("start to config vlan info to driver\r\n");
    if(__tk_vlan_ctrl[port].debug)
    {
        tk_oam_sw_vlan_member_printf(port);
    }

    /* check the difference */
    for(i = 0; i < OAM_TK_PORT_VLAN_NUM_MAX; i++)
    {
        vlan_mbr = (oam_tk_vlan_mbr_t *)__tk_vlan_ctrl[port].vlan_mbr;
        vlan_mbr_last = (oam_tk_vlan_mbr_t *)__tk_vlan_ctrl[port].vlan_mbr_last;

        if(vlan_mbr_last[i].valid)
        {
            for(j = 0; j < OAM_TK_PORT_VLAN_NUM_MAX; j++)
            {
                if(vlan_mbr[j].valid)
            {
                    if(memcmp(&(vlan_mbr_last[i].cfg), &(vlan_mbr[j].cfg), sizeof(epon_tk_port_vlan_member_config_t)) == 0)
                {
                        vlan_mbr[j].no_update = 1; /* this vlan is the same as last */
                        //OAM_ORG_LOG("vlan %d member has no update\r\n", vlan_mbr_last[i].cfg.egress_vlan);
                        break;
                }
                }
            }
            if(j == OAM_TK_PORT_VLAN_NUM_MAX)
            {
                /* remove __tk_vlan_member_last[port][i] */
                OAM_ORG_LOG("REMOVE port %d from vlan %d \r\n", port, vlan_mbr_last[i].cfg.egress_vlan);
                __vlan_port_remove(vlan_mbr_last[i].cfg.egress_vlan, port);
            }
            }
            }

    for(i = 0; i < OAM_TK_PORT_VLAN_NUM_MAX; i++)
                {
        vlan_mbr = (oam_tk_vlan_mbr_t *)__tk_vlan_ctrl[port].vlan_mbr;

        if(vlan_mbr[i].valid)
                    {
            if(vlan_mbr[i].no_update != 1)
                    {
                /* add __tk_vlan_member[port][i] */
                if(vlan_mbr[i].cfg.egress_tag == TK_VLAN_EG_TAG_POP)
                {
                    untag = 1;
                }
                OAM_ORG_LOG("ADD port %d to vlan %d untag %d\r\n", port, vlan_mbr[i].cfg.egress_vlan, untag);
                __vlan_port_add(vlan_mbr[i].cfg.egress_vlan, port, untag);

            }
        }
        }

    memcpy(__tk_vlan_ctrl[port].vlan_mbr_last, __tk_vlan_ctrl[port].vlan_mbr, sizeof(__tk_vlan_ctrl[port].vlan_mbr));
    __sw_vlan_member_clear(port);
    __tk_vlan_ctrl[port].cfg_flag = 0;

    }

/* NOT USED NOW */
/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_vlan_member_get_adapt                           */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_vlan_member_clear_adapt(
    oam_uint16  port
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = 0;
    ca_port_id_t ca_port;
    epon_tk_port_vlan_member_config_t vlan_mbr[OAM_TK_PORT_VLAN_NUM_MAX];
    ca_uint32_t i, j, k, m, counter = 0;
    ca_uint8_t   member_count = 0;
    ca_port_id_t  member_ports[OAM_TK_PORT_VLAN_NUM_MAX];
    ca_uint8_t   new_member_count = 0;
    ca_port_id_t  new_member_ports[OAM_TK_PORT_VLAN_NUM_MAX];
    ca_uint8_t   untagged_count = 0;
    ca_port_id_t  untagged_ports[OAM_TK_PORT_VLAN_NUM_MAX];
    ca_uint8_t   new_untagged_count = 0;
    ca_port_id_t  new_untagged_ports[OAM_TK_PORT_VLAN_NUM_MAX];
    ca_status_t ret;

    OAM_PORT_ID_CHECK(port);
    ca_port = OAM_PORT_ID_TO_CA_PORT(port);

    memset(member_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
    memset(new_member_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
    memset(untagged_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
    memset(new_untagged_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);

    __sw_vlan_member_get(port, &counter, vlan_mbr);
    //printf_vlan_member(port, counter, vlan_mbr);

    /* remove port from all the vlan */
    for(i = 0; i < counter; i++){
        memset(member_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
        memset(new_member_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
        memset(untagged_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
        memset(new_untagged_ports, 0, sizeof(ca_port_id_t) * OAM_TK_PORT_VLAN_NUM_MAX);
        k = 0; m = 0;
        new_member_count = 0;
        new_untagged_count = 0;
        ca_l2_vlan_port_get(0, vlan_mbr[i].ingress_vlan, &member_count, member_ports, &untagged_count, untagged_ports);
        for(j = 0; j < member_count; j++){
            if(member_ports[j] == ca_port){
                continue;
            }
            else {
                new_member_ports[k] = member_ports[j];
                k++;
                new_member_count ++;
            }
        }
        for(j = 0; j < untagged_count; j++){
            if(untagged_ports[j] == ca_port){
                continue;
            }
            else {
                new_untagged_ports[m] = untagged_ports[j];
                m++;
                new_untagged_count ++;
            }
        }

        ret = ca_l2_vlan_port_set(0,vlan_mbr[i].ingress_vlan, new_member_count, new_member_ports, new_untagged_count, new_untagged_ports);
    }
    __sw_vlan_member_clear(port);
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  ttk_oam_dync_mac_get_adapt                                  */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_dync_mac_get_adapt(
    oam_uint8  port,
    tk_oam_mac_rules_t  *rules,
    oam_uint8 *mac_num
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_l2_addr_entry_t *entry = NULL, l2_entry;
    ca_iterator_t return_entry;
    ca_status_t status;
    oam_if_t * intf = oam_intf_find_valid();
    int i = 0;
    ca_port_id_t ca_port;

    if (!rules || !mac_num || !intf)
        return 1;

    OAM_PORT_ID_CHECK(port);

    memset(&return_entry, 0, sizeof(ca_iterator_t));
    memset(&l2_entry, 0, sizeof(ca_l2_addr_entry_t));
    *mac_num = 0;
    return_entry.entry_count = 1;
    return_entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * return_entry.entry_count;
    return_entry.entry_data = &l2_entry;

    status = ca_l2_addr_iterate(0, &return_entry);

    while (CA_E_OK == status && *mac_num < KT_TK_MAX_MAC_PER_PORT){
        if (return_entry.entry_count == 1){
            entry = (ca_l2_addr_entry_t *)return_entry.entry_data;
            OAM_ORG_DEBUG("add %2x:%2x:%2x:%2x:%2x:%2x  port=%d\r\n", l2_entry.mac_addr[0], l2_entry.mac_addr[1],l2_entry.mac_addr[2],
                    l2_entry.mac_addr[3],l2_entry.mac_addr[4],l2_entry.mac_addr[5], l2_entry.port_id);
            ca_port = OAM_PORT_ID_TO_CA_PORT(l2_entry.port_id);
            if(memcmp(l2_entry.mac_addr,intf->macaddr,6) != 0
               && memcmp(l2_entry.mac_addr,intf->macaddr_peer,6) != 0
               && port == ca_port){
                rules[i].dir     = 1;
                rules[i].prio    = 2;
                rules[i].count   = 1;
                memset(rules[i].dest,0x00,sizeof(rules[i].dest));
                rules[i].action = 0;
                rules[i].rules.fselect = 1;
                rules[i].rules.oper    = 1;
                memcpy(rules[i].rules.mac, l2_entry.mac_addr, 6);
                i++;
            }
        }
        status = ca_l2_addr_iterate(0, &return_entry);
    }
    *mac_num = i;

    return 0;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_port_statistics_get_adapt                            */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_port_statistics_get_adapt(
    oam_uint8  port,
    tk_oam_onu_port_statis_data_t  *statis
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_pon_stats64_t pon_stats;
    oam_uni_stats64_t uni_stats;
    epon_if_stats_t if_stats;

    if (!statis || (port > (OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM - 1)))
        return OAM_E_ERROR;

    memset(&pon_stats, 0, sizeof(oam_pon_stats64_t));
    memset(&uni_stats, 0, sizeof(oam_uni_stats64_t));
    memset(&if_stats, 0, sizeof(epon_if_stats_t));
    memset(statis, 0, sizeof(tk_oam_onu_port_statis_data_t));
    
    if (port == OAM_PON_PORT_ID) {
        oam_onu_pon_stats_get(&pon_stats);
        //oam_onu_std_pon_stats_get(&if_stats);
        //statis->in_octets = if_stats.in_octets;
        //statis->in_unicast = if_stats.in_unicast;
        //statis->in_multicast = if_stats.in_multicast;
        //statis->in_broadcast = if_stats.in_broadcast;

        /* ONU PORT RX */
        statis->in_octets = pon_stats.pon_byte_cnt;
        statis->in_unicast = pon_stats.pon_ucframe_cnt;
        statis->in_multicast = pon_stats.pon_mcframe_cnt;
        statis->in_broadcast = pon_stats.pon_bcframe_cnt;
        statis->in_64_octets = pon_stats.pon_rmon64_cnt;
        statis->in_65to127_octets = pon_stats.pon_rmon65_127_cnt;
        statis->in_128to255_octets = pon_stats.pon_rmon128_255_cnt;
        statis->in_256to511_octets = pon_stats.pon_rmon256_511_cnt;
        statis->in_512to1023_octets = pon_stats.pon_rmon512_1023_cnt;
        statis->in_1024t01518_octets = pon_stats.pon_rmon1024_1518_cnt;
        statis->in_1519_plus_octets = pon_stats.pon_rx1519_maxcnt;
        statis->in_too_short_octets = pon_stats.pon_undersizecnt;
        statis->fcs_errors = pon_stats.pon_rx_fcs_err;

        /* ONU PORT TX */
        statis->out_octets = pon_stats.pon_tx_byte_cnt;
        statis->out_unicast = pon_stats.pon_txucframe_cnt;
        statis->out_multicast = pon_stats.pon_txmcframe_cnt;
        statis->out_broadcast = pon_stats.pon_txbcframe_cnt;
        statis->out_64_octets = pon_stats.pon_txrmon64_cnt;
        statis->out_65to127_octets = pon_stats.pon_txrmon65_127_cnt;
        statis->out_128to255_octets = pon_stats.pon_txrmon128_255_cnt;
        statis->out_256to511_octets = pon_stats.pon_txrmon256_511_cnt;
        statis->out_512to1023_octets = pon_stats.pon_txrmon512_1023_cnt;
        statis->out_1024t01518_octets = pon_stats.pon_txrmon1024_1518_cnt;
        statis->out_1519_plus_octets = pon_stats.pon_tx1519_maxcnt;

        /* ONU LINK RX */
        statis->in_mpcp_gate = pon_stats.pon_mpcp_uc_gat_rev;
        statis->in_mpcp_register = pon_stats.pon_mpcp_bc_reg_reg_rev;

        /* ONU LINK TX */
        statis->out_mpcp_report = pon_stats.pon_mpcp_tx_rpt;
        statis->out_mpcp_reg_request = pon_stats.pon_mpcp_tx_req_reg;
        statis->out_mpcp_reg_ack = pon_stats.pon_mpcp_tx_ack_ack;


        /* STATS not used by ubiquos OLT */
        statis->in_discards = pon_stats.pon_mac_drop + pon_stats.pon_ipg_drop;
        statis->in_errors = pon_stats.pon_crcerr_cnt +  pon_stats.pon_sld_err + pon_stats.pon_crc8_err;
        statis->in_unknown_protos = 0;
        statis->out_discards = 0;
        statis->out_errors = pon_stats.pon_tx_crc_err;



        statis->alignment_errors = 0;
        statis->single_collision = 0;
        statis->multiple_collision = 0;
        statis->sqe_test_errors = 0;
        statis->deferred_transmissions = 0;
        statis->late_collisions = 0;
        statis->excessive_collisions = 0;
        statis->internal_mac_tx_errors = 0;
        statis->carrier_sense_errors = 0;
        statis->frame_too_longs = pon_stats.pon_oversize_cnt;
        statis->internal_mac_rx_errors = 0;
        statis->symbol_errors = pon_stats.pon_err_symbol;
        statis->in_pause_frames = pon_stats.pon_pauseframe_cnt;
        statis->out_pause_frames = pon_stats.pon_txpausecnt;

    } else {
        oam_onu_uni_stats_get(port, &uni_stats);

        /* ONU PORT RX */
        statis->in_octets = uni_stats.rxbyte_cnt;
        statis->in_unicast = uni_stats.rxucfrm_cnt;
        statis->in_multicast = uni_stats.rxmcfrm_cnt;
        statis->in_broadcast = uni_stats.rxbcfrm_cnt;
        statis->in_64_octets = uni_stats.rxstatsfrm64_cnt;
        statis->in_65to127_octets = uni_stats.rxstatsfrm65_127_cnt;
        statis->in_128to255_octets = uni_stats.rxstatsfrm128_255_cnt;
        statis->in_256to511_octets = uni_stats.rxstatsfrm256_511_cnt;
        statis->in_512to1023_octets = uni_stats.rxstatsfrm512_1023_cnt;
        statis->in_1024t01518_octets = uni_stats.rxstatsfrm1024_1518_cnt;
        statis->in_1519_plus_octets = uni_stats.rxstatsfrm1519_Max_cnt;
        statis->in_too_short_octets = uni_stats.rxundersizefrm_cnt;
        statis->frame_too_longs = uni_stats.rxoversizefrm_cnt;
        statis->in_pause_frames = uni_stats.rxpausefrm_cnt;
        /* length error */
        statis->alignment_errors = uni_stats.alignment_errors;
        statis->fcs_errors = uni_stats.rxcrcerrfrm_cnt;

        /* ONU PORT TX */
        statis->out_octets = uni_stats.txbyte_cnt;
        statis->out_unicast = uni_stats.txucfrm_cnt;
        statis->out_multicast = uni_stats.txmcfrm_cnt;
        statis->out_broadcast = uni_stats.txbcfrm_cnt;
        statis->out_64_octets = uni_stats.txstatsfrm64_cnt;
        statis->out_65to127_octets = uni_stats.txstatsfrm65_127_cnt;
        statis->out_128to255_octets = uni_stats.txstatsfrm128_255_cnt;
        statis->out_256to511_octets = uni_stats.txstatsfrm256_511_cnt;
        statis->out_512to1023_octets = uni_stats.txstatsfrm512_1023_cnt;
        statis->out_1024t01518_octets = uni_stats.txstatsfrm1024_1518_cnt;
        statis->out_1519_plus_octets = uni_stats.txstatsfrm1519_Max_cnt;
        statis->out_pause_frames = uni_stats.txpausefrm_cnt;

        statis->single_collision = uni_stats.single_collision;
        statis->multiple_collision = uni_stats.multiple_collision;
        statis->late_collisions = uni_stats.late_collisions;
        statis->excessive_collisions = uni_stats.excessive_collisions;

        /* STATS not used by ubiquos OLT */
        statis->in_discards = 0;
        statis->in_errors = uni_stats.rxcrcerrfrm_cnt;
        statis->out_discards = 0;
        statis->out_errors = uni_stats.txcrcerrfrm_cnt;
        statis->sqe_test_errors = 0;
        statis->deferred_transmissions = uni_stats.deferred_transmissions;
        statis->internal_mac_tx_errors = uni_stats.internal_mac_tx_errors;
        statis->carrier_sense_errors = 0;
        statis->internal_mac_rx_errors = uni_stats.internal_mac_rx_errors;
        statis->symbol_errors = uni_stats.symbol_errors;

    }

    return OAM_E_OK;
}

static oam_uint16 __queue_id_convert_adapt(oam_uint16 tk_queue)
{
    oam_uint16 oam_queue = 0;
    
    switch(tk_queue)
    {
    case 0:
        oam_queue = 6;
        break;
    case 1:
        oam_queue = 4;
        break;
    case 2:
        oam_queue = 2;
        break;
    case 3:
        oam_queue = 0;
        break;
    default:
        oam_queue = 0;
        break;
    }
    return oam_queue;
}

oam_status tk_oam_queue_stats_get_adapt(
    oam_uint16  port,
    oam_uint16  queue,
    tk_oam_queue_stats_t  *stats
)
{
    oam_uint16 oam_queue = 0;
    oam_queue_stats64_t queue_stats;
    oam_queue_stats64_t queue7_stats;

    OAM_PORT_ID_CHECK(port);
    OAM_TK_QUEUE_ID_CHECK(queue);

    oam_queue = __queue_id_convert_adapt(queue);
    
    memset(&queue_stats, 0, sizeof(oam_queue_stats64_t));
    oam_onu_queue_stats_get(port, oam_queue, &queue_stats);
    OAM_ORG_LOG("oam queue stats get: port %d, queue %lld, fwd_bytes %lld, fwd_frames %lld, drop_bytes %lld, drop_frames %lld\r\n\r\n", 
        port, oam_queue, queue_stats.fwd_bytes, queue_stats.fwd_frames, queue_stats.drop_bytes, queue_stats.drop_frames);
    
    /* onu-link-queue */
    stats->rx_octets = queue_stats.fwd_bytes + queue_stats.drop_bytes;
    stats->rx_frames = queue_stats.fwd_frames + queue_stats.drop_frames;
    stats->rx_bytes_drop = queue_stats.drop_bytes;   
    stats->rx_frames_drop = queue_stats.drop_frames;
    stats->rx_bytes_delay = 0;
    stats->rx_delay = 0;
    stats->rx_delay_threshold = 0;

    /* onu-port-queue */
    stats->tx_octets = queue_stats.fwd_bytes + queue_stats.drop_bytes;
    stats->tx_frames = queue_stats.fwd_frames + queue_stats.drop_frames;
    stats->tx_bytes_drop = queue_stats.drop_bytes;
    stats->tx_frames_drop = queue_stats.drop_frames;
    stats->tx_bytes_delay = 0;
    stats->tx_delay = 0;
    stats->tx_delay_threshold = 0;

    if(oam_queue == 6)
    {
        /* kt highest priority queue0: cortina queue6 is for data traffic, queue7 is for protocol.*/
        memset(&queue7_stats, 0, sizeof(oam_queue_stats64_t));
        oam_onu_queue_stats_get(port, 7, &queue7_stats);
        stats->rx_octets += queue7_stats.fwd_bytes + queue7_stats.drop_bytes;
        stats->rx_frames += queue7_stats.fwd_frames + queue7_stats.drop_frames;
        stats->rx_bytes_drop += queue7_stats.drop_bytes;   
        stats->rx_frames_drop += queue7_stats.drop_frames;

        stats->tx_octets += queue7_stats.fwd_bytes + queue7_stats.drop_bytes;
        stats->tx_frames += queue7_stats.fwd_frames + queue7_stats.drop_frames;
        stats->tx_bytes_drop += queue7_stats.drop_bytes;
        stats->tx_frames_drop += queue7_stats.drop_frames;
    }

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_igmp_grp_get_adapt                                   */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_igmp_grp_get_adapt(
    tk_oam_igmp_group_info_t  *igmp_grp
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{


    mc_ipv4_grp_t grp_tbl[64];
    oam_uint32 tbl_len = 0;
    oam_uint32 i;

    mc_fdb_get_grp_tbl(0, grp_tbl, &tbl_len, NULL, NULL);

    igmp_grp->group_count = tbl_len;

    memset(igmp_grp->groups, 0, sizeof(igmp_grp->groups));
    for (i = 0; i < tbl_len; i++) {
        igmp_grp->groups[i].group_addr = grp_tbl[i].grp;
        igmp_grp->groups[i].port_bitmap = grp_tbl[i].port_map;
    }


    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_igmp_grp_add_adapt                                   */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_igmp_grp_add_adapt(
    tk_oam_igmp_group_info_t  *grp_info
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_igmp_grp_del_adapt                                   */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_igmp_grp_del_adapt(
    tk_oam_igmp_group_info_t  *grp_info
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    return OAM_E_OK;
}

#ifdef HAVE_MPORTS
oam_uint8 g_kt_mc_qcnt[TK_MAX_UNI_NUM] = {0, 0, 0, 0};
#else
oam_uint8 g_kt_mc_qcnt[TK_MAX_UNI_NUM] = {0};
#endif
/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_igmp_config_set_adapt                                */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_igmp_config_set_adapt(
    tk_oam_igmp_config_get_t  *igmp_cfg
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_mc_protocol_cfg_t prot_cfg;
    oam_uint32 mask = 0;
    oam_port_id_t port_id;

    if (igmp_cfg->robust_count) {
        prot_cfg.robustness = igmp_cfg->robust_count;
        mask = MC_PRO_MASK_ROBUSTNESS;
    }

    prot_cfg.last_member_query_count = igmp_cfg->lmq_count;
    mask |= MC_PRO_MASK_LM_QUERY_COUNT;


    if(igmp_cfg->ports[0].group_count >0)
        mc_mode_set(0 , 2); //IGMP snooping mode
    else
        mc_mode_set(0 , 4); //disable IGMP snooping  and block DS mc data


    mc_protocol_set(0, mask, &prot_cfg);

    for (port_id = 0; port_id < TK_MAX_UNI_NUM; port_id++) {
        g_kt_mc_qcnt[port_id] = igmp_cfg->ports[port_id].queue_count;
    }

    if (igmp_cfg->ports[0].group_count > TK_IGMP_MAX_GROUP) {
        igmp_cfg->ports[0].group_count = TK_IGMP_MAX_GROUP;
    }

    return mc_max_grp_set(0, igmp_cfg->ports[0].group_count);


}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_igmp_config_get_adapt                                */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_igmp_config_get_adapt(
    tk_oam_igmp_config_get_t  *igmp_cfg
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_mc_protocol_cfg_t cfg;
    oam_port_id_t port_id;
    oam_uint16 max_grp;

    memset(igmp_cfg, 0, sizeof(tk_oam_igmp_config_get_t));

    igmp_cfg->uni_count = mc_device_port_num_get() - 1;

    //mib_get(MIB_HW_SWITCH_PORT_NUM,&igmp_cfg->uni_count);
    igmp_cfg->grp_fwd_qa = htons(0x0001);
    igmp_cfg->snoop_op = 0x00;

    mc_protocol_get(0, &cfg);

    igmp_cfg->robust_count = cfg.robustness;
    igmp_cfg->lmq_count = cfg.last_member_query_count;

    mc_max_grp_get(0, &max_grp);

    for (port_id = 0; port_id < TK_MAX_UNI_NUM; port_id++) {
        igmp_cfg->ports[port_id].group_count = max_grp;
        igmp_cfg->ports[port_id].queue_count = g_kt_mc_qcnt[port_id];
    }

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_report_threshold_set_adapt                           */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_report_threshold_set_adapt(
    oam_uint16  threshold
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = CA_E_OK;
    ca_port_id_t ca_port = OAM_PON_PORT_ID_TO_CA_PORT();
    if (!threshold)
        return OAM_E_ERROR;
    //return ca_epon_mpcp_queue_threshold_set(0, ca_port, 0, 0, 0, threshold);
    return rc;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_report_threshold_get_adapt                           */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_report_threshold_get_adapt(
    oam_uint16  *threshold
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = CA_E_OK;
    ca_port_id_t ca_port = OAM_PON_PORT_ID_TO_CA_PORT();
    if (!threshold)
        return OAM_E_ERROR;
    //return ca_epon_mpcp_queue_threshold_get(0, ca_port, 0, 0, 0, (ca_uint32_t*)threshold);
    return rc;
}

static ca_uint32_t __oam_kt_shapper_rule_idx = 0;
/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_onu_ds_shaping_set_adapt                             */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_onu_ds_shaping_set_adapt(
    oam_uint8 port,
    oam_uint8 traffic_type,
    oam_uint8 rate_units,
    epon_tk_ds_shapping_config_t *ds_shaping
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = OAM_E_OK;
    ca_port_id_t ca_port;
    ca_shaper_t config;
    ca_boolean_t enable = FALSE;
    ca_boolean_t perf_ena = FALSE ;
    ca_port_id_t pon_port;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t mask;
    ca_classifier_action_t action;
    ca_uint32_t rule_idx;
  //  ca_device_config_tlv_t      scfg_tlv;
    ca_uint8_t                          pon_shaper_ena;

    //memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    //scfg_tlv.type = CA_CFG_ID_PON_SHAPER_ENA;
    //rc |= ca_device_config_tlv_get(0, &scfg_tlv);
    //if (CA_E_OK == rc){
      //  pon_shaper_ena = ((scfg_tlv.value[0] == 1) ? 1 : 0);
    //}else{
      //  pon_shaper_ena = 0;
    //}
    pon_shaper_ena = 1;

    /* Disable shaping function if rate = 0, or rate > 5G */
    if(ds_shaping->rate == 0 || ds_shaping->rate > 5400000){
        enable = 0;
    }else{
        enable = 1;
    }

    if (port == 0x10) { /* shaping per ONU */
#if 1
        config.enable = enable;
        config.pps = rate_units;    /* 0 = Kbps, 1 = Frames/second */
        config.rate = ds_shaping->rate; /* Kbps */
        //config.burst_size = 25600/16; /* byte */
        config.burst_size = 17;

        OAM_ORG_LOG("start to %s shaper based on ONU, rate 0x%x, burst 0x%x\r\n", (enable)?"enable":"disable",
            ds_shaping->rate, ds_shaping->burst);
        rc |= ca_system_shaper_set(0, &config);

        //rc  |= ca_queue_iperf_get(0,&perf_ena);
        if(perf_ena== TRUE){
            config.enable = FALSE;
        }

        if(pon_shaper_ena ){
            rc |= ca_port_shaper_set(0, CA_PORT_ID(CA_PORT_TYPE_EPON,7), &config);
        }

        pon_port = OAM_PON_PORT_ID_TO_CA_PORT();
    	memset(&key, 0, sizeof(ca_classifier_key_t));
    	memset(&mask, 0, sizeof(ca_classifier_key_mask_t));
    	memset(&action, 0, sizeof(ca_classifier_action_t));

        /* ONU shaper is done on L3_WAN port, it needs to forward PON RX traffic to L3 WAN port */
    	if(enable == 1){
    	    if(!__oam_kt_shapper_rule_idx){    /* if rule idx is 0, no need to add one more time */
        	    mask.key_handle = 1;
        		mask.src_port = 1;
        		key.src_port = pon_port;
        		action.dest.port = CA_PORT_ID(CA_PORT_TYPE_L3, CA_PORT_ID_L3_WAN);
        		action.forward = CA_CLASSIFIER_FORWARD_PORT;
        	    rc |= ca_classifier_rule_add(0, 6, &key, &mask, &action, &__oam_kt_shapper_rule_idx);
        		OAM_ORG_LOG("add rule: forward PON RX traffic to L3 WAN port, rule_idx %d\r\n", __oam_kt_shapper_rule_idx);
            }

    	}else{
    	    ca_uint32_t  default_shaper;
    	    if(__oam_kt_shapper_rule_idx){
    		    rc |= ca_classifier_rule_delete(0, __oam_kt_shapper_rule_idx);
    		    OAM_ORG_LOG("del rule: forward PON RX traffic to L3 WAN port, rule_idx %d\r\n", __oam_kt_shapper_rule_idx);
    		    __oam_kt_shapper_rule_idx = 0;
            }
            //memset(&scfg_tlv, 0, sizeof(scfg_tlv));
           /* scfg_tlv.type = CA_CFG_ID_PON_SHAPER_DEFAULT_VALUE;
            rc |= ca_device_config_tlv_get(0, &scfg_tlv);
            if(rc != CA_E_OK){
            }else{
                    memcpy(&default_shaper, scfg_tlv.value, 4);
            }
            */
            
            default_shaper = 8410000;
            config.enable = (((pon_shaper_ena == 1) &&(perf_ena == FALSE)) ? 1:0);
            config.pps = rate_units;    /* 0 = Kbps, 1 = Frames/second */
            config.rate = default_shaper; /* Kbps */
        //config.burst_size = 25600/16; /* byte */
            config.burst_size = 17;
            if(pon_shaper_ena){
                rc |= ca_port_shaper_set(0, CA_PORT_ID(CA_PORT_TYPE_EPON,7), &config);
            }
    	}
       // if(perf_ena == FALSE){
            //rc |= ca_queue_length_adjust(0 ,config.rate);
       // }
#endif
    }
    else {

        OAM_PORT_ID_CHECK(port);
        ca_port = OAM_PORT_ID_TO_CA_PORT(port);
        memset(&config, 0, sizeof(ca_shaper_t));

        config.enable = enable;
        config.pps = rate_units;    /* 0 = Kbps, 1 = Frames/second */
        config.rate = ds_shaping->rate;
        config.burst_size = (ds_shaping->burst)/16;

        OAM_ORG_LOG("start to %s shaper on port 0x%x, rate 0x%x, burst 0x%x\r\n", (enable)?"enable":"disable", ca_port,
            ds_shaping->rate, ds_shaping->burst);
        if(ca_port == 0x30005){
            if(config.rate <= 5000000){
                //ca_eth_port_use_deep_queue(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET, 5),TRUE);
                ca_port_shaper_set(0, 0xC0005,&config);
            }else{
                //ca_eth_port_use_deep_queue(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET, 5),FALSE);
                ca_port_shaper_set(0, 0x30005,&config);
            }
       }else{
            ca_port = 0xC0000|(ca_port&0xFFFF);
            rc |= ca_port_shaper_set(0, ca_port, &config);
       }
    }
    return rc;
}

/*******************************************************************************/
/* $rtn_hdr_start  tk_oam_onu_ds_shaping_get_adapt                             */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status tk_oam_onu_ds_shaping_get_adapt(
    oam_uint8 port,
    epon_tk_ds_shapping_traffic_config_t *ds_shaping
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rc = OAM_E_OK;
    ca_port_id_t ca_port;
    ca_shaper_t config;

    if (port == 0x10) { /* shaping per ONU */
        rc = ca_system_shaper_get(0, &config);
    }else{

        OAM_PORT_ID_CHECK(port);
        ca_port = OAM_PORT_ID_TO_CA_PORT(port);

        rc = ca_port_shaper_get(0, ca_port, &config);
    }
    if(rc != OAM_E_OK)
        return rc;

    if(config.pps == 0){
        ds_shaping->rate_units = 1;
    }else{
        ds_shaping->rate_units = 0;
    }
    ds_shaping->num_shapers = 1;
    ds_shaping->traffic_type = 3;
    ds_shaping->config.queue_bitmap = 0xff;
    ds_shaping->config.rate = config.rate;
    ds_shaping->config.burst = config.burst_size * 16;
    ds_shaping->config.schedul_algorithm = 1; // Strict Priority
    ds_shaping->config.num_weights = 1;
    ds_shaping->config.weight = 1;

    return rc;
}

oam_status tk_oam_onu_us_policer_set_adapt(
    oam_uint8 port,
    oam_uint8 rate_units,
    epon_tk_us_policer_config_t *us_policer
)
{
    oam_status rc = OAM_E_OK;
    ca_port_id_t ca_port;
    ca_policer_t config;

    OAM_PORT_ID_CHECK(port);
    ca_port = OAM_PORT_ID_TO_CA_PORT(port);

    memset(&config, 0, sizeof(ca_shaper_t));

    if (us_policer->rate == 0) {
        config.mode = CA_POLICER_MODE_DISABLE;
    } else {
        config.mode = CA_POLICER_MODE_RATE_ONLY;
        if(rate_units == 0){ //Kbps
                    config.pps = 0;
        }else{  // pps
                    config.pps = 1;
        }
        config.cbs = 256;
        config.cir = us_policer->rate;
        config.pbs = 256;
        config.pir = us_policer->rate;
    }
    rc = ca_port_policer_set(0, ca_port, &config);
    return rc;
}


oam_status tk_oam_onu_us_policer_get_adapt(
    oam_uint8 port,
    epon_tk_us_policer_traffic_config_t *us_policer
)
{
    oam_status rc = OAM_E_OK;
    ca_port_id_t ca_port;
    ca_policer_t policer;

    OAM_PORT_ID_CHECK(port);
    ca_port = OAM_PORT_ID_TO_CA_PORT(port);

    memset(&policer, 0, sizeof(ca_policer_t));
    rc = ca_port_policer_get(0, ca_port, &policer);
    if(rc != OAM_E_OK)
        return rc;

    if(policer.pps == 0){
        us_policer->rate_units = 1;
    }else{
        us_policer->rate_units = 0;
    }
    us_policer->num_levels = 1;
    us_policer->config.traffic_type = 3;
    us_policer->config.rate = policer.cir;

    return rc;
}

oam_status tk_record_find  (
                            oam_uint8                    port,
                            oam_type_t             oam_type,
                            void                     *   rule,
                            tk_record_t        **  out_record

)
{
    oam_status               ret = CA_E_ERROR;
    oam_uint32               i=0;
    oam_uint8                *  data=NULL;
    oam_uint32               cmp_length=0;
    oam_uint32               offset=0;
    epon_tk_rule_t      * tk_rule=NULL;
    tk_record_t        *  record=NULL;

    switch ( oam_type ) {
         case   TK_OAM:
                    tk_rule=(epon_tk_rule_t *) rule;
                    cmp_length = tk_rule->num_rule * sizeof(epon_tk_rule_condition_t) + 2;
                    offset=8;

                    break;


          default:
                    break;

    }

    * out_record=NULL;

    for ( i=0 ; i < KT_OAM_MAX_RULE_RECORD ; i++ ) {
        record=&tk_manage.record[i];
        data= (oam_uint8 * )&record->rule;
        if ( record->valid ==0) {
            continue;
        }
        if (record->port !=port) {
            continue;
        }

        if (memcmp( data+offset, rule+offset, cmp_length)==0) {
            if(oam_type == TK_OAM)
            {
                tk_rule = (epon_tk_rule_t *) rule;
                if(tk_rule->cond[0].field == TK_RULE_FIELD_MASK_VLAN_ID)
                {
                    if(tk_rule->action_parameter[1] || tk_rule->action_parameter[2])
                    {
                        epon_tk_rule_t *rec = (epon_tk_rule_t *)&record->rule;
                        if(tk_rule->action_parameter[1] != rec->action_parameter[1] ||
                            tk_rule->action_parameter[2] != rec->action_parameter[2])
                            continue;
                    }
                }
            }
            ret=CA_E_OK;
            * out_record= (tk_record_t   *)record;
            break;
        }
    }

    return ret;
}


oam_status tk_record_add (
                            oam_uint8                    port,
                            oam_type_t               oam_type,
                            tk_cls_index_t   *      cls_index,
                            void                  *      rule
)
{
    oam_status               ret = CA_E_ERROR;
    oam_uint32               i=0;
    oam_uint32               length=0;
    tk_record_t          * record=NULL;

    switch ( oam_type ) {
 
         case   TK_OAM:
                    length=sizeof(epon_tk_rule_t);
                    break;


          default:
                    break;

    }

    for ( i=0 ; i < KT_OAM_MAX_RULE_RECORD ; i++ ) {
        record= &tk_manage.record[i];
        if ( record->valid ==1) {
            continue;
        }

        record->valid =1;
        record->port=port;
        record->oam_type=oam_type;
        record->cls_index=cls_index;
        memcpy ( &record->rule, rule, length);

        ret = CA_E_OK;
        break;

    }


    return ret;

}


oam_status tk_record_del (
    tk_record_t                    * record
)
{
    oam_status           ret = CA_E_OK;

    if (record->cls_index !=NULL) {
        record->cls_index->valid=0;
    }

    memset (record , 0, sizeof( tk_record_t));


    return ret;
}

oam_status tk_free_index_get (
                            tk_cls_index_t   * * cls_index
)
{
    oam_status               ret = CA_E_ERROR;
    oam_uint32               i=0;
    tk_cls_index_t        * temp_cls_index=NULL;

    *cls_index=NULL;

    for ( i=0 ; i < KT_OAM_MAX_CLS  ; i++ ) {
        temp_cls_index=&tk_manage.cls[i];

        if ( temp_cls_index->valid ==1) {
            continue;
        }
        * cls_index= temp_cls_index;

        ret = CA_E_OK;
        break;

    }


    return ret;
}


oam_status tk_vlan_rule_is (
                            oam_uint8     field,
                            oam_uint8     rule_action
)
{
    oam_status ret = CA_E_ERROR;

   if (field == TK_RULE_FIELD_MASK_VLAN_ID
        && ( (rule_action ==TK_RULE_ACTION_DISCARD)
                || (rule_action==TK_RULE_ACTION_DEL_TAG)
                || (rule_action==TK_RULE_ACTION_SETVID_ADD_TAG))  ) {

        ret = CA_E_OK;
   }


    return ret;
}
/*
static void  tk_vlan_clear_adapt ()
{
    ca_callback_context_t context;
    if (epon_request_onu_vlan_del(context, 0, 0, CA_PON_PORT_ID, NULL, 0xFFFF) != CA_E_OK)
    {
        oam_printf("failed to delete vlan on port %d\n", CA_PON_PORT_ID);
    }

    if (epon_request_onu_vlan_del(context, 0, 0, CA_UNI_PORT_ID1, NULL, 0xFFFF) != CA_E_OK)
    {
        oam_printf("failed to delete vlan on port %d\n", CA_UNI_PORT_ID1);
    }

    if (epon_request_onu_pvid_set(context, 0, 0, CA_UNI_PORT_ID1, 0) != CA_E_OK)
    {
        oam_printf("failed to set pvid %d for port %d\n", 1, CA_UNI_PORT_ID1);
    }
}
*/
oam_status  tk_vlan_add_adapt (oam_uint8  port, epon_tk_rule_t    *oam_config)
{
    oam_uint16 vlan = 0;
    ca_port_id_t port_id;
    ca_port_discard_control_t control;
    ca_status_t result;
    ca_port_id_t member_ports[2];
    ca_port_id_t untagged_ports;
    ca_vlan_port_control_t port_control;

    if(!oam_config)
        return CA_E_PARAM;

    memset(&control, 0, sizeof(ca_port_discard_control_t));
    if ((port == 0) && (TK_RULE_DIRECTION_DOWNSTREAM == oam_config->direction))
    {
        if ((TK_RULE_ACTION_DISCARD == oam_config->rule_action) &&
            (TK_RULE_FIELD_MASK_VLAN_ID == oam_config->cond[0].field) &&
            (TK_RULE_OPERATOR_NOT_EQUAL == oam_config->cond[0].operator))
        {// downstream packets: VLAN not equal to defined, drop the packets
            vlan = oam_config->cond[0].match_value[7] +
                            ((oam_config->cond[0].match_value[6] & 0xf) << 8);
            port_id = CA_PORT_ID(CA_PORT_TYPE_EPON, 1);
            control.drop_priority_tag = 1;
            control.drop_untag = 1;
            ca_port_discard_control_set(0,port_id, &control);

            ca_l2_vlan_port_control_get(0, port_id, &port_control);
            port_control.drop_unknown_vlan = 1;
            port_control.ingress_membership_check_enable = 1;
            ca_l2_vlan_port_control_set(0, port_id, &port_control);

            result = ca_l2_vlan_create(0,vlan);
            if (result == CA_E_EXISTS || result == CA_E_OK)
            {
                member_ports[0] = OAM_PON_PORT_ID_TO_CA_PORT();
                member_ports[1] = OAM_UNI_PORT_ID_TO_CA_PORT(port);
                untagged_ports = OAM_UNI_PORT_ID_TO_CA_PORT(port);
                ca_l2_vlan_port_set(0,vlan, 2, member_ports, 1, &untagged_ports);
            }
        }
        else if ((TK_RULE_ACTION_DEL_TAG == oam_config->rule_action) &&
                    (TK_RULE_OPERATOR_EXIST == oam_config->cond[0].operator) &&
                    (TK_RULE_FIELD_MASK_VLAN_ID == oam_config->cond[0].field))
        {// downstream tagged packets untag out
            // set in above rule
        }
    }
    else if ((port == 1) && (TK_RULE_DIRECTION_UPSTREAM == oam_config->direction))
    {
        if ((TK_RULE_ACTION_SETVID_ADD_TAG == oam_config->rule_action) &&
            (TK_RULE_FIELD_MASK_VLAN_ID == oam_config->cond[0].field) &&
            (TK_RULE_OPERATOR_NOT_EXIST == oam_config->cond[0].operator))
        {// upstream untagged packet add tag
            vlan = oam_config->action_parameter[2] |
                        (oam_config->action_parameter[1] << 8);
            port_id = OAM_UNI_PORT_ID_TO_CA_PORT(port);
            ca_port_pvid_vlan_set(0, port_id, vlan);
            control.drop_priority_tag = 1;
            control.drop_multiple_tag = 1;
            ca_port_discard_control_set(0,port_id, &control);
        }
        else if ((TK_RULE_ACTION_DISCARD == oam_config->rule_action) &&
            (TK_RULE_FIELD_MASK_VLAN_ID == oam_config->cond[0].field) &&
            (TK_RULE_OPERATOR_NOT_EQUAL == oam_config->cond[0].operator))
        {// upstream packets: VLAN ID not equal to defined, drop packets
            /*vlan = oam_config->cond[0].match_value[7] +
                            ((oam_config->cond[0].match_value[6] & 0xf) << 8); */
            port_id = OAM_UNI_PORT_ID_TO_CA_PORT(port);
            ca_l2_vlan_port_control_get(0, port_id, &port_control);
            port_control.drop_unknown_vlan = 1;
            port_control.ingress_membership_check_enable = 1;
            ca_l2_vlan_port_control_set(0, port_id, &port_control);
        }
    }
    else
        return CA_E_ERROR;

    return CA_E_OK;
}

oam_status  tk_vlan_del_adapt (oam_uint8  port, epon_tk_rule_t    *oam_config)
{
    oam_uint16 vlan = 0;
    ca_port_id_t port_id;
    ca_port_discard_control_t control;
    ca_status_t result;
    ca_port_id_t member_ports[2];
    ca_port_id_t untagged_ports;
    ca_vlan_port_control_t port_control;

    if(!oam_config)
        return CA_E_PARAM;

    if ((port == 0) && (TK_RULE_DIRECTION_DOWNSTREAM == oam_config->direction))
    {
        if ((TK_RULE_ACTION_DISCARD == oam_config->rule_action) &&
            (TK_RULE_FIELD_MASK_VLAN_ID == oam_config->cond[0].field) &&
            (TK_RULE_OPERATOR_NOT_EQUAL == oam_config->cond[0].operator))
        {// downstream packets: VLAN not equal to defined, drop the packets
            vlan = oam_config->cond[0].match_value[7] +
                            ((oam_config->cond[0].match_value[6] & 0xf) << 8);
            memset(&control, 0, sizeof(ca_port_discard_control_t));
            port_id = OAM_PON_PORT_ID_TO_CA_PORT();
            ca_port_discard_control_set(0,port_id, &control);

            ca_l2_vlan_port_control_get(0, port_id, &port_control);
            port_control.drop_unknown_vlan = 0;
            port_control.ingress_membership_check_enable = 0;
            ca_l2_vlan_port_control_set(0, port_id, &port_control);
            //printf("del pon port downstream vlan %d ,mode = %d \n",vlan_cfg.c_vlan,vlan_cfg.eg_mode);
            ca_l2_vlan_delete(0, vlan);
        }
        else if ((TK_RULE_ACTION_DEL_TAG == oam_config->rule_action) &&
                    (TK_RULE_OPERATOR_EXIST == oam_config->cond[0].operator) &&
                    (TK_RULE_FIELD_MASK_VLAN_ID == oam_config->cond[0].field))
        {// downstream tagged packets untag out
            vlan = oam_config->cond[0].match_value[7] +
                            ((oam_config->cond[0].match_value[6] & 0xf) << 8);

            //printf("del pon port downstream vlan %d ,mode = %d \n",vlan_cfg.c_vlan,vlan_cfg.eg_mode);

        }
    }
    else if ((port == 1) && (TK_RULE_DIRECTION_UPSTREAM == oam_config->direction))
    {
        if ((TK_RULE_ACTION_SETVID_ADD_TAG == oam_config->rule_action) &&
            (TK_RULE_FIELD_MASK_VLAN_ID == oam_config->cond[0].field) &&
            (TK_RULE_OPERATOR_NOT_EXIST == oam_config->cond[0].operator))
        {// upstream untagged packet add tag
            port_id = OAM_UNI_PORT_ID_TO_CA_PORT(port);
            ca_port_pvid_vlan_set(0, port_id, 0);
            ca_l2_vlan_port_control_get(0, port_id, &port_control);
            port_control.drop_unknown_vlan = 0;
            port_control.ingress_membership_check_enable = 0;
            ca_l2_vlan_port_control_set(0, port_id, &port_control);
            //      printf("set uni port vlanid %d ,\n",0);
        }
        else if ((TK_RULE_ACTION_DISCARD == oam_config->rule_action) &&
            (TK_RULE_FIELD_MASK_VLAN_ID == oam_config->cond[0].field) &&
            (TK_RULE_OPERATOR_NOT_EQUAL == oam_config->cond[0].operator))
        {// upstream packets: VLAN ID not equal to defined, drop packets
            vlan = oam_config->cond[0].match_value[7] +
                            ((oam_config->cond[0].match_value[6] & 0xf) << 8);
        }
    }
    else
        return CA_E_ERROR;

    return CA_E_OK;
}

oam_status  tk_cls_add_adapt (
            epon_tk_rule_t *    oam_config,
            tk_cls_index_t   * *   output_cls_index
)
{
    return 0;
}
oam_status  kt_ethertype_check ( oam_uint16 ether_type)
{
    oam_status   ret=CA_E_ERROR;
  /*
     if ( ((ether_type ==SDL_CLS_ETHERTYPE_IPv4)
                 ||(ether_type ==SDL_CLS_ETHERTYPE_IPv6)
                 || (ether_type ==SDL_CLS_ETHERTYPE_ARP)
                   || (ether_type ==SDL_CLS_ETHERTYPE_PPPoE_DISCOVERY)
                    || (ether_type ==SDL_CLS_ETHERTYPE_PPPoE_SESSION)
                  || (ether_type ==SDL_CLS_ETHERTYPE_SLOW_PROTOCOL)
                   || (ether_type ==SDL_CLS_ETHERTYPE_MAC_CONTROL)
                   || (ether_type ==SDL_CLS_ETHERTYPE_8021X)
                  || (ether_type ==SDL_CLS_ETHERTYPE_KT_LOOPBACK)
                   || (ether_type ==SDL_CLS_ETHERTYPE_KT_LLTD) )){
        ret=CA_E_OK;

     }
*/
     return ret;


}

oam_status  tk_oam_check (epon_tk_rule_t *oam_config)
{
    oam_uint8                    direction=0;
    oam_uint8                    num_rule=0;
    oam_uint8                    rule_action=0;
    oam_uint8                    field=0;
    epon_tk_rule_condition_t  * cond=NULL;
    oam_uint8                    * match_value=NULL;
    oam_uint8                    operator=0;
    oam_uint16                  ether_type=0;
    oam_uint16                  vlanid=0;
    oam_uint8                    priority=0;
    oam_int32                    i=0;
    oam_status                   ret=CA_E_OK;



    direction=oam_config->direction;
    rule_action=oam_config->rule_action;
    num_rule=oam_config->num_rule;



    if (direction ==0) {
        ret=CA_E_ERROR;
        /*diag_printf("\r\n in %s %d \r\n ", __FUNCTION__, __LINE__);*/
        goto END;
    }


    if ( !((rule_action ==TK_RULE_ACTION_SET_DEST_FORWARD)
          || (rule_action ==TK_RULE_ACTION_ADD_TAG)
           || (rule_action ==TK_RULE_ACTION_DEL_TAG)
           || (rule_action ==TK_RULE_ACTION_SETVID_ADD_TAG)
            || (rule_action ==TK_RULE_ACTION_SET_COS)
             || (rule_action ==TK_RULE_ACTION_VLAN_SWAP)
              || (rule_action ==TK_RULE_ACTION_DISCARD)
        || (rule_action ==0x12) )) {
        ret=CA_E_ERROR;
       /*diag_printf("\r\n in %s %d \r\n ", __FUNCTION__, __LINE__);*/
        goto END;
    }

    /*check set*/
 for (i = 0; i < num_rule; i++) {
    cond=&oam_config->cond[i];
    field=cond->field;
    operator=cond->operator;
    match_value=cond->match_value;


    switch (field ) {
        case    TK_RULE_FIELD_MASK_L2_DA:
                    if ( operator  !=TK_RULE_OPERATOR_EQUAL) {
                        ret=CA_E_ERROR;
                        /*diag_printf ("\r\n operator=%d    in %s %d \r\n ", operator  , __FUNCTION__, __LINE__);*/
                        goto END;
                    }

                     break;


        case    TK_RULE_FIELD_MASK_L2_SA:
                    if ( operator  !=TK_RULE_OPERATOR_EQUAL) {
                        ret=CA_E_ERROR;
                        /*diag_printf("\r\n in %s %d \r\n ", __FUNCTION__, __LINE__);*/
                        goto END;
                    }
                    break;


        case    TK_RULE_FIELD_MASK_LINK_INDEX:
                    break;

        case    TK_RULE_FIELD_MASK_ETH_TYPE:
                    if ( operator  !=TK_RULE_OPERATOR_EQUAL) {
                        ret=CA_E_ERROR;
                        goto END;
                    }

                    ether_type=(match_value[6]<<8) + match_value[7];
                    ret=kt_ethertype_check ( ether_type);
                    if ( ret !=CA_E_ERROR ) {
                        goto END;
                    }

                    break;


        case    TK_RULE_FIELD_MASK_VLAN_ID:
                    if (  ! ((operator  ==TK_RULE_OPERATOR_EQUAL)
                          || (operator  ==TK_RULE_OPERATOR_EXIST)
                          || (operator  ==TK_RULE_OPERATOR_NOT_EXIST)) ) {
                        ret=CA_E_ERROR;
                        goto END;
                    }

                    vlanid=(match_value[6]<<8) + match_value[7];

                    if (vlanid > KT_MAX_VLANID ) {
                        ret=CA_E_ERROR;
                        goto END;
                    }
                    break;

        case    0xa:
        case    TK_RULE_FIELD_MASK_PRIORITY:
                    if ( operator  !=TK_RULE_OPERATOR_EQUAL) {
                        ret=CA_E_ERROR;
                        goto END;
                    }
                    priority= match_value[7];
                    if ( priority > 7) {
                        ret=CA_E_ERROR;
                        goto END;
                    }

                    break;


        case    TK_RULE_FIELD_MASK_IP_PROTOCOAL:
                    if ( operator  !=TK_RULE_OPERATOR_EQUAL) {
                        ret=CA_E_ERROR;
                        goto END;
                    }
                    break;


        case    TK_RULE_FIELD_MASK_DSCP:
                    if ( operator  !=TK_RULE_OPERATOR_EQUAL) {
                        ret=CA_E_ERROR;
                        goto END;
                    }
                    break;

        default:
                    break;

    }

 }

 END:

        return ret;


}

void stp_send_oam_tk_org_event(ca_uint8_t raised, ca_uint16_t portid)
{
    ca_uint8_t  alarm_code = 0;
    ca_uint8_t context = 0;

    //ca_printf("<%s, %d>raised:%d, portid:%d, olt_type:%d\r\n", __FUNCTION__, __LINE__, raised, portid, oam_get_olt_type());
    alarm_code = TK_ALARM_ID_PORT_DISABLE;
    context = TK_ALARM_CONTEXT_PORT;
    oam_build_tk_org_event(alarm_code, raised, portid, context);
    
}

oam_status oam_tk_rstp_port_state_get_adapt(oam_uint8 ktIndex, oam_tk_rstp_state_t *state)
{
    ca_status_t ret = CA_E_OK;
    ca_port_id_t port;
    ca_port_stp_state_t stp_state;

    OAM_PORT_ID_CHECK(ktIndex);

    port = OAM_PORT_ID_TO_CA_PORT(ktIndex);
    ret = ca_port_stp_state_get(0, port, &stp_state);
    OAM_ORG_LOG("port 0x%x, state %d\r\n", port, stp_state);
    switch(stp_state)
    {
        case CA_PORT_STP_DISABLED:
            *state = OAM_TK_RSTP_DISABLED;
            break;
        case CA_PORT_STP_BLOCKING:
            *state = OAM_TK_RSTP_BLOCKING;
            break;
        case CA_PORT_STP_LEARNING:
            //*state = OAM_TK_RSTP_LISTENING;
            *state = OAM_TK_RSTP_BLOCKING;
            break;
        case CA_PORT_STP_FORWARDING:
            *state = OAM_TK_RSTP_FORWARDING;
            break;
        case CA_PORT_STP_FWD_NO_LEARN:
            *state = OAM_TK_RSTP_FORWARDING;
            break;
        default:
            *state = OAM_TK_RSTP_FORWARDING;
            break;
    }
    return OAM_E_OK;
    //return ((stp_state < 3) ? (stp_state + 1) : 5);
}


void oam_tk_ext_info_init_adapt(tk_oam_extended_info_t *p_tk_ext_info)
{
    oam_status ret = OAM_E_OK;
    oam_uint16 version = 0;
    unsigned char major, minor, rev, build;
    oam_if_t * oam_intf = oam_intf_find_valid();
    ca_img_info_t img_info;

    ca_device_config_tlv_t scfg_tlv;

    if(!p_tk_ext_info) return;

    OAM_ORG_LOG("tk ext info get\r\n");
    memset(p_tk_ext_info, 0, sizeof(tk_oam_extended_info_t));
    memset(&img_info, 0, sizeof(img_info));

    ca_img_info_get(0, 0, &img_info);
    if(!img_info.active_flag)
    {
        ca_img_info_get(0, 1, &img_info);
    }

    major = img_info.version[1]-0x30;
    minor = img_info.version[4]-0x30;
    rev = img_info.version[7]-0x30;
    build = img_info.version[10]-0x30;
    version = (major << 12) + (minor << 8) + (rev << 4) + build;

    p_tk_ext_info->tk_version = htons(version);
    p_tk_ext_info->tk_oui[0] = 0x00;
    p_tk_ext_info->tk_oui[1] = 0x13;
    p_tk_ext_info->tk_oui[2] = 0x25;

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_HW_VERSION;
    scfg_tlv.len = CFG_ID_HW_VERSION_LEN;

    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if (CA_E_OK != ret) {
        OAM_LOG_DEBUG("read CA_CFG_ID_HW_VERSION fail. ret = %d \r\n", ret);
        return OAM_E_ERROR;
    }
    memcpy(p_tk_ext_info->product,scfg_tlv.value,CFG_ID_HW_VERSION_LEN);
    memcpy(p_tk_ext_info->version,scfg_tlv.value,CFG_ID_HW_VERSION_LEN);

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_MODEL_NAME;
    scfg_tlv.len = CFG_ID_MODEL_NAME_LEN;

    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if (CA_E_OK != ret) {
        OAM_LOG_DEBUG("read CA_CFG_ID_MODEL_NAME fail. ret = %d \r\n", ret);
        return OAM_E_ERROR;
    }
    memset(p_tk_ext_info->extendedid, 0, 64);
    memcpy(p_tk_ext_info->extendedid,scfg_tlv.value,CFG_ID_MODEL_NAME_LEN);

#if 0
    memset(p_tk_ext_info->extendedid, 0, 16);
    p_tk_ext_info->extendedid[0] = 'D';
    p_tk_ext_info->extendedid[1] = '9';
    p_tk_ext_info->extendedid[2] = '0';
    p_tk_ext_info->extendedid[3] = '4';
#endif
    if(oam_intf)
        memcpy(p_tk_ext_info->base_mac, oam_intf->macaddr, OAM_MACADDR_LEN);
    else
        memcpy(p_tk_ext_info->base_mac, 0, OAM_MACADDR_LEN);

    p_tk_ext_info->max_link = 1;
    p_tk_ext_info->max_port = 5; /* UNI + EPON port */
    p_tk_ext_info->us_queue_num = 0x8;
    p_tk_ext_info->us_queue_per_link = 0x8;
    p_tk_ext_info->kb_per_us_incr = 0;

    p_tk_ext_info->ds_queue_num = 1;
    p_tk_ext_info->ds_queue_per_link = 0x8;
    p_tk_ext_info->tk_kedec_man_id = htons(0x0203);
    p_tk_ext_info->chip_id = htons(0xD904);
    p_tk_ext_info->chip_ver = htonl(0x8271);
    return;
}

#endif/*END_HAVE_KT_OAM*/

oam_status oam_tk_adapt_init()
{
    oam_uint32 i = 0;
#ifdef HAVE_DPOE_OAM
    memset(&g_tk_oam_queue_tbl, 0, sizeof(g_tk_oam_queue_tbl));
#endif/*END_HAVE_DPOE_OAM*/

#ifdef HAVE_KT_OAM
    memset(&__tk_vlan_ctrl, 0, sizeof(__tk_vlan_ctrl));
    for (i=0; i < KT_OAM_MAX_CLS; i++ ) {
        tk_manage.cls[i].valid=0;
        tk_manage.cls[i].rule_index=i+2;
    }
#endif/*END_HAVE_KT_OAM*/
    return OAM_E_OK;
}

#endif /*END_HAVE_TK_OAM*/

