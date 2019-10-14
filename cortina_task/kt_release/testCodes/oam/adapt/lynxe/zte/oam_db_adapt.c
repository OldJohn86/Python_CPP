
#ifdef ZTE_SPECIFIC

#include "cs_types.h"
#include "db_api.h"
#include "db_lynxe.h"
#include "db_zte.h"


#include "cli_api.h"
#include "sal_policer.h"
#include "sal_l2.h"
#include "sdl_ext_port.h"

#include "mc_api.h"


#ifdef CS_HAVE_EPON
#include "sal_epon_qos.h"
#include "sal_epon_fdb.h"
#include "sal_epon_vlan.h"
#include "sal_epon_cls.h"
#include "sal_epon.h"
#endif
#include "sal_cos_map.h"
#include "sal_port.h"
#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_control.h"
#include "oam_supp.h"
#include "oam_ctc.h"
#include "oam_ctc_pdu.h"
#include "oam_client.h"
#include "oam_mux_par.h"
#include "oam_ctc_alarm.h"
#include "oam_zte_adapt.h"
#include "oam_zte_pdu.h"
#include "oam_ctc_adapt.h"
#include "mc_ctcctrl.h"
#include "cs_ip_api.h"
#include "cs_igmp_api.h"
#include "gpio_api.h"
#include "loop_detect_api.h"

#if 1 /*def HAVE_DB_MANAGMENT*/
/*lint -e64*/
int db_mgmt_restore( cs_db_op_code op_code, void *data, cs_uint64 bitmask )
{
    db_zte_mgmt_t  *mgmt_info = (db_zte_mgmt_t *)data;
    cs_ip_host_if_cfg_t cfg;
    cs_int32 ret = 0;
    cs_uint16 vlan = 0;
    cs_uint16 enable = 0;

    memset(&cfg, 0, sizeof(cs_ip_host_if_cfg_t));
    cfg.ip_addr.ip_ver =  CS_IPV4;
    cfg.ip_mask.ip_ver =  CS_IPV4;
    cfg.gw_ip.ip_ver =  CS_IPV4;

    enable = (mgmt_info->inband_enable == 1) ? 1 : 0;

    if (enable) {
        cfg.ip_addr.addr.v4.s_addr = mgmt_info->inband_ip;
        cfg.ip_mask.addr.v4.s_addr = mgmt_info->inband_mask;
        cfg.gw_ip.addr.v4.s_addr = mgmt_info->gw;
    }
    else {
        cfg.ip_addr.ip_ver = CS_IPV4;
        cfg.ip_mask.ip_ver = CS_IPV4;
        cfg.ip_addr.addr.v4.s_addr = mgmt_info->ip;
        cfg.ip_mask.addr.v4.s_addr = mgmt_info->mask;
        cfg.gw_ip.addr.v4.s_addr = mgmt_info->ip;
    }

    ret = cs_l3_intf_set(0, 0, &cfg);

    if (ret != 0) {
        OAM_ORG_LOG("set l3 intf fail:%d\r\n", ret);
    }

    vlan |= (((mgmt_info->inband_pri) << 13) & 0xE000);
    vlan |= (mgmt_info->inband_vlan & 0xFFF);
    ret = cs_inband_mgnt_set(0, 0, vlan, enable);

    if (ret != 0) {
        OAM_ORG_LOG("set inband vlan fail:%d\r\n", ret);
    }

    return 0;

}

int db_voq_restore( cs_db_op_code op_code, void *data, cs_uint64 bitmask )
{
    db_zte_voq_t *voq = (db_zte_voq_t *)data;
    cs_uint8 i = 0;
    int ret = CS_E_OK;

    for (i = 0; i < DB_MAX_VOQ_SIZE; i++) {
        ret = cs_sal_epon_onu_que_size_set(0, 0, __SAL_ETH_PORT(voq->port_id), i,
                                           voq->voq_cfg[i]);

        if (ret != CS_E_OK) {
            OAM_ORG_LOG("set qos threshold failed, port = %d, q = %d , v = %d ret = %d \r\n",
                        voq->port_id, i, voq->voq_cfg[i], ret);
        }
    }

    return ret;
}

int db_battery_restore( cs_db_op_code op_code, void *data, cs_uint64 bitmask )
{
    db_zte_backup_battery_t *battery = (db_zte_backup_battery_t *)data;
    cs_int32 i;
    cs_uint16 alarm_code = 0;

    for (i = 0; i < DB_ZTE_BATTERY_CFG_NUM; i++) {
        switch (battery[i].alarm_sig) {
            case BATTERY_AC_POWER_LOST_DETECTION:/*AC power lost detection*/
                alarm_code = CTC_ALARM_POWER;
                break;

            case BATTERY_UPS_MODULE_DETECTION:/*UPS module detection*/
                alarm_code = CTC_ALARM_BATT_MISSING;
                break;

            case BATTERY_UPS_POWER_LOW_DETECTION:/*UPS power low detection*/
                alarm_code = CTC_ALARM_BATT_VOLTLOW;
                break;

            case BATTERY_SWITCH_A_INPUT:/*Switch A input*/
                alarm_code = CTC_ALARM_ONU_DRY_CONTACTA;
                break;

            case BATTERY_SWITCH_B_INPUT:/*Switch B input*/
                alarm_code = CTC_ALARM_ONU_DRY_CONTACTB;
        }

        cs_gpio_battery_logical_set(i, battery->log_state);

        if (battery[i].alarm_status == 0) {
            ctc_oam_set_alarm_config(CTC_ONU_ALARM, 0, alarm_code, CTC_ALARM_OFF);
        }
        else {
            ctc_oam_set_alarm_config(CTC_ONU_ALARM, 0, alarm_code, CTC_ALARM_ON);
        }

    }

    return 0;
}

int db_svc_restore( cs_db_op_code op_code, void *data, cs_uint64 bitmask )
{
    db_zte_onu_svc_t *svc = (db_zte_onu_svc_t *)data;
    cs_status ret = CS_E_OK;
#ifdef HAVE_ONU_RSTP
    cs_onu_stp_config_t  rstp_cfg;
#endif
    cs_pri_cos_map_t           queue_map;

    ret = cs_sal_fdb_age_set(0, svc->mac_aging_time);

    if (ret != CS_E_OK) {
        OAM_ORG_LOG("set mac aging time failed, ret = %d ", ret);
    }

    ret = cs_igmp_ctcctrl_switch_set(svc->mc_mode);

    if (ret != CS_E_OK) {
        OAM_ORG_LOG("set mc mode failed ,ret = %d \n", ret);
    }

    ret = mc_gmi_set(0, svc->mc_time);

    if (ret != CS_E_OK) {
        OAM_ORG_LOG("set mc time failed ,ret = %d \n", ret);
    }



    cs_sal_eth_port_mtu_set(0, CS_PORT_SNI0, svc->max_mtu);

    cs_sal_eth_port_mtu_set(0, CS_PORT_ETH0, svc->max_mtu);

    cs_l3_intf_mtu_set(svc->cpu_mtu);

    ret = cs_l2_port_isolation_set(0, svc->port_iso);

    if (ret != CS_E_OK) {
        OAM_ORG_LOG("set port iso fail, ret=%d\n", ret);
    }

#ifdef HAVE_PPPOE
    /*ret = pppoe_plus_enable(svc.ppoe_plus_en);

    if(ret != CS_E_OK){
        OAM_ORG_LOG("set pppoe fail, ret=%d\n", ret);
    }*/
#endif
    ret = cs_sal_epon_onu_fec_set(0, -1, svc->fec_mode);

    if (ret != CS_E_OK) {
        OAM_ORG_LOG("set fec mode failed ,ret = %d \n", ret);
    }

    ret = cs_sal_epon_onu_schedule_mode_set(0, __SAL_ETH_PORT(DB_PORT_IDX),
                                            svc->qos_sched_mode);

    if (ret != CS_E_OK) {
        OAM_ORG_LOG("set qos algo to sdl failed, ret = %d ", ret);
    }


    memset(&queue_map, 0, sizeof(queue_map));
    memcpy(queue_map.cos, svc->pri_cos_map, CS_SAL_MAX_DOT1P);

    ret = cs_sal_epon_onu_dot1p_map_set(0, -1, &queue_map);

    if (ret != CS_E_OK) {
        OAM_ORG_LOG("set qos user_pri_que to sdl failed, ret = %d ", ret);
    }

#if 0
    ds_rate.enable = svc->ds_shape_en;
    ds_rate.cbs = svc->ds_cbs;
    ds_rate.rate = svc->ds_shape_rate;

    ret = cs_sal_epon_onu_port_ds_rate_limit_set(0, -1, __SAL_ETH_PORT(DB_PORT_IDX),
            &ds_rate);

    if (ret != CS_E_OK) {
        OAM_ORG_LOG("set ds rate to sdl failed, port = %d , ret = %d \n", DB_PORT_IDX,
                    ret);
    }
#endif

#ifdef HAVE_ONU_RSTP
    memset(&rstp_cfg, 0, sizeof(rstp_cfg));
    rstp_cfg.enable = svc.rstp;
    rstp_cfg.mask = ONU_MASK_STP_BRIDGE_ENABLE;

    ret = app_stp_bridge_cfg_set(&rstp_cfg);

    if (ret != CS_E_OK ) {
        OAM_ORG_LOG("set rstp to SDL failed, ret = %d \n", ret);
    }

#endif

#ifdef HAVE_MPORTS
    ret = epon_request_onu_big_burst_size_set(
              context, ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, svc.tcp);

    if (ret != CS_E_OK ) {
        OAM_ORG_LOG("set tcp to SDL failed, ret = %d \n", ret);
    }

#endif

    return ret;
}

int db_mac_bind_restore( cs_db_op_code op_code, void *data, cs_uint64 bitmask )
{
    db_zte_mac_bind_t *mac_bind = (db_zte_mac_bind_t *)data;
    cs_uint8 i = 0;
    cs_status ret = CS_E_OK;
    cs_sal_mac_entry_t        mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];


    memset(mac_entry, 0, ZTE_MAX_ENTRIES_PER_CONTAINER * sizeof(
               cs_sal_mac_entry_t));

    for (i = 0; i < mac_bind->num; i++) {
        mac_entry[i].mode  = CS_SAL_MAC_ENTRY_MODE_FWD;
        mac_entry[i].type = CS_SAL_MAC_TYPE_SRC;
        memcpy(mac_entry[i].mac, mac_bind->mac[i].addr, sizeof(cs_mac_addr_t));
    }

    if (cs_sal_mac_entry_add(0, __SAL_ETH_PORT(mac_bind->port_id), mac_bind->num,
                             mac_entry)) {
        OAM_ORG_LOG("set from sdl failed, ret = %d \n", ret);
    }

    return ret;
}

#ifdef HAVE_MPORTS
int db_port_mirror_restore( cs_db_op_code op_code, void *data,
                            cs_uint64 bitmask )
{
    db_zte_mirror_t *mirror = (db_zte_mirror_t *)data;
    cs_callback_context_t     context;
    cs_status ret = CS_E_OK;

    memset(&mirror, 0x00, sizeof(mirror));
    cs_db_entry_get(DB_ZTE_PORT_MIRROR_ID, (cs_uint8 *) &mirror, sizeof(mirror));
    ret = epon_request_onu_port_mirror_set(context, 0, 0, mirror.enable,
                                           mirror.rx_mask, mirror.tx_mask);

    if (CS_E_OK != ret) {
        OAM_ORG_LOG("mirror set fail, ret=%d\n", ret);
    }

    return ret;
}
#endif


int db_qos_remark_restore( cs_db_op_code op_code, void *data,
                           cs_uint64 bitmask )
{
    db_zte_remark_t *db_cfg = (db_zte_remark_t *)data;
    cs_uint8 i = 0;
    cs_status ret = CS_E_OK;
    cs_pri_remark_t remark;

    remark.en = db_cfg->remark_en;

    for (i = 0; i < DB_ZTE_QUEUE_PRIO_LEN; i++)
        remark.priority[i] = db_cfg->remark[i];

    ret = cs_sal_epon_onu_dot1p_remark_set(0, -1, __SAL_ETH_PORT(DB_PORT_IDX),
                                           &remark);

    if (ret != CS_E_OK)
        OAM_ORG_LOG("set qos user_pri_regen to sal failed, port = %d  ret = %d ",
                    i + 1 , ret);

    return ret;
}

int db_vlan_restore( cs_db_op_code op_code, void *data, cs_uint64 bitmask )
{
    db_zte_vlan_t *vlan = (db_zte_vlan_t *)data;
    cs_uint16 translate_num;
    oam_ctc_onu_vlan_xlate_entry translate_vlan[CTC_VLAN_MODE_TRANSLATE_MAX];
    cs_uint16 trunk_num ;
    oam_ctc_onu_vlan_tag_t trunk_vlan[CTC_VLAN_MODE_TRUNK_MAX];
    ctc_oam_agg_vlan_t  agg_vlan;
    cs_uint8 vlan_mode;
    oam_ctc_onu_vlan_tag_t def_vlan;


    cs_uint32 j = 0;
    cs_status ret = CS_E_OK;

    vlan_mode = vlan->vlan_mode ;
    def_vlan.tpid = vlan->def_vlan.tpid ;
    def_vlan.tag = vlan->def_vlan.tag ;

    switch (vlan_mode) {
        case CTC_VLAN_MODE_TRANSPARENT:
            ret = ctc_oam_onu_transparent_vlan_set_adapt(vlan->port_id);

            if (ret != CS_E_OK) {
                OAM_ORG_LOG("set vlan transparent falied, ret = %d ", ret);
            }

            break;

        case CTC_VLAN_MODE_TAG:
            ret = ctc_oam_onu_tag_vlan_set_adapt(vlan->port_id, def_vlan);

            if (ret != CS_E_OK) {
                OAM_ORG_LOG("set vlan tag falied, ret = %d ", ret);
            }

            break;

        case CTC_VLAN_MODE_TRANSLATE:
            translate_num =  vlan->vlan_num / 2;

            if (translate_num <= DB_ZTE_VLAN_MAX) {
                for (j = 0; j < translate_num; j++) {
                    translate_vlan[j].tag1.tpid = vlan->vlan[j * 2].tpid ;
                    translate_vlan[j].tag1.tag = vlan->vlan[j * 2].tag ;
                    translate_vlan[j].tag2.tpid = vlan->vlan[j * 2 + 1].tpid ;
                    translate_vlan[j].tag2.tag = vlan->vlan[j * 2 + 1].tag ;
                }

                ret = ctc_oam_onu_translate_vlan_set_adapt(vlan->port_id, def_vlan,
                        translate_num, translate_vlan);
            }
            else {
                ret = CS_E_ERROR;
            }

            if (ret != CS_E_OK) {
                OAM_ORG_LOG("set vlan translate falied, ret = %d ", ret);
            }

            break;

        case CTC_VLAN_MODE_TRUNK:
        case ZTE_VLAN_MODE_HYBRID:
            trunk_num = vlan->vlan_num;

            for (j = 0 ; j < trunk_num; j++) {
                trunk_vlan[j].tpid = vlan->vlan[j].tpid;
                trunk_vlan[j].tag = vlan->vlan[j].tag;
            }

            ret = ctc_oam_onu_trunk_vlan_set_adapt( vlan->port_id, def_vlan, trunk_num,
                                                    trunk_vlan);

            if (ret != CS_E_OK) {
                OAM_ORG_LOG("set vlan translate falied, ret = %d ", ret);
            }

            break;

        case CTC_VLAN_MODE_AGG:
            agg_vlan.agg_vlan_num = vlan->agg_vlan_num;
            agg_vlan.agg_dst_vlan.tpid =  vlan->vlan[0].tpid;
            agg_vlan.agg_dst_vlan.tag =  vlan->vlan[0].tag;

            for (j = 0; j < agg_vlan.agg_vlan_num  && j < CTC_ONU_AGGR_VLAN_NUM; j++) {
                agg_vlan.agg_vlan[j].tpid =  vlan->vlan[j + 1].tpid;
                agg_vlan.agg_vlan[j].tag =  vlan->vlan[j + 1].tag;
            }

            ret = ctc_oam_onu_agg_vlan_set_adapt(vlan->port_id, def_vlan, 1, &agg_vlan);

            if (ret != CS_E_OK) {
                OAM_ORG_LOG("set vlan agg falied, ret = %d ", ret);
            }

            break;

        case ZTE_VLAN_MODE_STACK:
            ret = zte_oam_onu_stack_vlan_set_adapt(vlan->port_id, def_vlan);

            if (ret != CS_E_OK) {
                OAM_ORG_LOG("set vlan tag falied, ret = %d ", ret);
            }

            break;
    }


    return ret;
}

int db_mac_limit_restore( cs_db_op_code op_code, void *data, cs_uint64 bitmask )
{
    db_zte_mac_limit_t *db_mac_limit = (db_zte_mac_limit_t *)data;
    cs_status       ret = CS_E_OK;


    ret = cs_sal_fdb_mac_limit_set(0, __SAL_ETH_PORT(db_mac_limit->port_id),
                                   db_mac_limit->num);

    return ret;
}


int db_mac_filter_restore( cs_db_op_code op_code, void *data,
                           cs_uint64 bitmask )
{
    db_zte_mac_bind_t *db_mac_filter = (db_zte_mac_bind_t *)data;
    cs_uint32       i = 0;
    cs_sal_mac_entry_t        mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];


    memset(mac_entry, 0, ZTE_MAX_ENTRIES_PER_CONTAINER * sizeof(
               cs_sal_mac_entry_t));

    for (i = 0; i < db_mac_filter->num; i++) {
        mac_entry[i].mode  = CS_SAL_MAC_ENTRY_MODE_FILTER;
        mac_entry[i].type = CS_SAL_MAC_TYPE_SRC;
        memcpy(mac_entry[i].mac, &db_mac_filter->mac[i], sizeof(cs_mac_addr_t));
    }

    if (cs_sal_mac_entry_add(0, __SAL_ETH_PORT(db_mac_filter->port_id),
                             db_mac_filter->num, mac_entry)) {
        OAM_ORG_LOG("set mac filter failed \n");
    }

    return 0;
}




int db_cls_restore( cs_db_op_code op_code, void *data, cs_uint64 bitmask )
{
    db_zte_cls_t *db_cls = (db_zte_cls_t *)data;

    if (cs_sal_port_classification_add(0, __SAL_ETH_PORT(db_cls->port_id),
                                       db_cls->num, (cs_sal_classification_t *)db_cls->rule))
        OAM_ORG_LOG("set port class\n");

    return 0;
}


int db_mc_vlan_restore( cs_db_op_code op_code, void *data, cs_uint64 bitmask )
{
    db_zte_mc_vlan_t *db_mc_vlan = (db_zte_mc_vlan_t *)data;


    if (cs_sal_mc_vlan_add(0, __SAL_ETH_PORT(db_mc_vlan->port_id), db_mc_vlan->vlan,
                           db_mc_vlan->num)) {
        OAM_ORG_LOG("add mc vlan fail\n");
    }

    return 0;
}

#define CTC_ONU_AGGR_VLAN_NUM               16
#define CTC_MCVLAN_TAG_OPER_SWAP 2

int db_mc_tagstrip_restore( cs_db_op_code op_code, void *data,
                            cs_uint64 bitmask )
{
    db_zte_mc_tagstrip_t *db_mc_tagstrip = (db_zte_mc_tagstrip_t *)data;
    cs_uint32       vlan_pair_index = 0;
    cs_sal_vlan_cfg_t             swap_rule[CTC_ONU_AGGR_VLAN_NUM];

    memset ( swap_rule, 0, sizeof (swap_rule ) );

    if (db_mc_tagstrip->tagStriped == CTC_MCVLAN_TAG_OPER_SWAP) {
        for (vlan_pair_index = 0; vlan_pair_index < db_mc_tagstrip->num;
             ++vlan_pair_index) {
            swap_rule[vlan_pair_index].s_vlan.vid =
                db_mc_tagstrip->vlan[vlan_pair_index * 2];
            swap_rule[vlan_pair_index].c_vlan.vid =
                db_mc_tagstrip->vlan[vlan_pair_index * 2 + 1];
        }

        if (cs_sal_mc_vlan_action_set(0, __SAL_ETH_PORT(db_mc_tagstrip->port_id),
                                      CTC_MCVLAN_TAG_OPER_SWAP, swap_rule, db_mc_tagstrip->num)) {
            OAM_ORG_LOG("add mc vlan tagstrip fail\n");
        }
    }

    return 0;
}


int db_mc_max_grp_restore( cs_db_op_code op_code, void *data,
                           cs_uint64 bitmask )
{
    db_zte_mc_grp_t  *db_mc_grp = (db_zte_mc_grp_t *)data;
    cs_status       ret = CS_E_OK;

    ret = mc_port_max_grp_set (0, db_mc_grp->port_id, db_mc_grp->mc_grp);

    if (ret)
        OAM_ORG_LOG("set mc max grp fail, ret:%d\r\n", ret);


    return ret;
}

int db_fast_leave_restore( cs_db_op_code op_code, void *data,
                           cs_uint64 bitmask )
{
    db_zte_fast_leave_t *db_fast_leave = (db_zte_fast_leave_t *)data;
    cs_status       ret = CS_E_OK;

    ret = mc_set_fast_leave_state( db_fast_leave->fast_leave);

    if (ret)
        OAM_ORG_LOG("set fast leave fail, ret:%d\r\n", ret);

    return ret;
}


int db_alarm_restore( cs_db_op_code op_code, void *data, cs_uint64 bitmask )
{
    db_zte_onu_alarm_t *db_alarm = (db_zte_onu_alarm_t *)data;
    cs_uint32       i = 0;
    db_zte_alarm_entry_t     *db_entry = NULL;
    cs_status       ret = CS_E_OK;

    for (i = 0; i < db_alarm->num; i++) {
        db_entry = &db_alarm->entry[i];
        ctc_oam_set_alarm_config (db_entry->alarm_type, 0, db_entry->alarm_code,
                                  db_entry->enable);

        if ( db_entry->threshold_type != CTC_THRESH_UNSUPPORT) {
            ret = ctc_oam_set_alarm_threshold(db_entry->alarm_type, 0, db_entry->alarm_code,
                                              db_entry->raise_threshold, db_entry->clear_threshold);

            if (ret)
                OAM_ORG_LOG("set alarm threshold fail, idx:%d\r\n", i);
        }
    }

    return ret;
}


int db_port_alarm_restore ( cs_db_op_code op_code, void *data,
                            cs_uint64 bitmask )
{
    db_zte_port_alarm_t *db_port_alarm = (db_zte_port_alarm_t *)data;
    cs_uint32       i = 0;
    db_zte_alarm_entry_t     *db_entry = NULL;
    cs_status       ret = CS_E_OK;

    for (i = 0; i < db_port_alarm->onu_entry.num; i++) {
        db_entry = &db_port_alarm->onu_entry.entry[i];
        ctc_oam_set_alarm_config (db_entry->alarm_type , db_port_alarm->port_id,
                                  db_entry->alarm_code, db_entry->enable);

        if (db_entry->threshold_type != CTC_THRESH_UNSUPPORT) {
            ret = ctc_oam_set_alarm_threshold(db_entry->alarm_type,
                                              db_port_alarm->port_id,
                                              db_entry->alarm_code,
                                              db_entry->raise_threshold,
                                              db_entry->clear_threshold);

            if (ret)
                OAM_ORG_LOG("set oam alarm fail, idx:%d\r\n", i);
        }
    }

    return ret;
}


int db_static_mac_restore( cs_db_op_code op_code, void *data,
                           cs_uint64 bitmask )
{
    db_zte_static_mac_t *db_mac_table = (db_zte_static_mac_t *)data;
    cs_uint32       i = 0;
    cs_sal_entry_fdb_t sdl_entry;


    for (i = 0; i < db_mac_table->num; i++) {
        memset(&sdl_entry, 0, sizeof(cs_sal_entry_fdb_t));
        memcpy(sdl_entry.mac, &db_mac_table->mac[i], sizeof(cs_mac_addr_t));
        sdl_entry.port = __SAL_ETH_PORT(db_mac_table->port_id);
        sdl_entry.type = SAL_FDB_ENTRY_STATIC; /*static entry*/
        sdl_entry.vlan_id = db_mac_table->vlan[i];

        if (cs_sal_fdb_entry_add(0, &sdl_entry)) {
            OAM_ORG_LOG("fdb add fail\n");
        }
    }

    return 0;
}

void db_restore_register()
{
/*    cs_db_register_restore(TB_ZTE_USER, NULL);*/
    cs_db_register_restore(TB_ZTE_MGMT, db_mgmt_restore);
    cs_db_register_restore(TB_ZTE_SN, NULL);
    cs_db_register_restore(TB_ZTE_BAK_BATTERY, db_battery_restore);
    cs_db_register_restore(TB_ZTE_ONU_SVC, db_svc_restore);
#ifdef HAVE_MPORTS
    /*cs_db_register_restore(TB_ZTE_PORT_MIRROR, db_port_mirror_restore);*/
#endif
    cs_db_register_restore(TB_ZTE_QOS_VOQ, db_voq_restore);
    cs_db_register_restore(TB_ZTE_QOS_REMARK, db_qos_remark_restore);
/*    cs_db_register_restore(TB_ETH_PORT, db_port_info_restore);*/
    cs_db_register_restore(TB_ZTE_MAC_BIND, db_mac_bind_restore);
    cs_db_register_restore(TB_ZTE_VLAN, db_vlan_restore);
    cs_db_register_restore(TB_ZTE_MAC_LIMIT, db_mac_limit_restore);
    cs_db_register_restore(TB_ZTE_MAC_FILTER, db_mac_filter_restore);
    cs_db_register_restore(TB_ZTE_STATIC_MAC, db_static_mac_restore);
    cs_db_register_restore(TB_ZTE_CLS, db_cls_restore);
    cs_db_register_restore(TB_ZTE_MC_VLAN, db_mc_vlan_restore);
    cs_db_register_restore(TB_ZTE_MC_TAGSTRIP, db_mc_tagstrip_restore);
    cs_db_register_restore(TB_ZTE_MAX_GRP, db_mc_max_grp_restore);
    cs_db_register_restore(TB_ZTE_FAST_LEAVE, db_fast_leave_restore);
    cs_db_register_restore(TB_ZTE_ONU_ALARM, db_alarm_restore);
    cs_db_register_restore(TB_ZTE_PORT_ALARM, db_port_alarm_restore);
    cs_db_register_restore(TB_ZTE_TIMEOUT, NULL);
    return;
}


/*lint +e64*/
#endif
#endif/*END_ZTE_SPECIFIC*/

