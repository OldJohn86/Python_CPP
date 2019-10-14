#ifndef __OAM_TK_ADAPT_H__
#define __OAM_TK_ADAPT_H__

#ifdef HAVE_TK_OAM

#include "oam_types.h"
#include "oam_tk.h"
#include "ca_version.h"

#ifdef HAVE_DPOE_OAM
oam_status tk_oam_queue_report_mode_get_adapt(oam_uint32 *report_mode);
oam_status tk_oam_queue_report_mode_set_adapt(oam_uint32 report_mode);
oam_status tk_oam_link_queue_get_adapt(oam_uint16 llid_idx, tk_oam_queue_cfg_t *cfg);
oam_status tk_oam_link_queue_set_adapt(oam_uint16 llid_idx, tk_oam_queue_cfg_t cfg);
oam_status tk_oam_port_queue_get_adapt(oam_uint16 port, tk_oam_queue_cfg_t *cfg);
oam_status tk_oam_port_queue_set_adapt(oam_uint16 port, tk_oam_queue_cfg_t cfg);

oam_status tk_oam_port_mtu_get_adapt(oam_uint16 port, oam_uint32 *mtu);
oam_status tk_oam_port_mtu_set_adapt(oam_uint16 port, oam_uint32 mtu);

void tk_oam_mllid_reg_enable_adapt(oam_uint8 llid_num);

#endif

#ifdef HAVE_KT_OAM
#define KT_MAX_VLANID                                   4095

#define KT_NORMAL_RESET_DELAY_ADAPT      200         /* 200ms */

#define KT_OAM_MAX_RULE_RECORD                 24
#define KT_OAM_MAX_CLS                                  (16-2)
#define TK_DS_RULES_MAX_NUM                 7
#define TK_US_RULES_MAX_NUM                 12
#define SIOCSIFVLAN                         0x8983  /* Set 802.1Q VLAN options */

/* TK OAM UNI configuration: auto-neg, phy ability, duplex, admin ...  */

/* 07 00 53: port speed set */
#define TK_OAM_AUTO_ENG_ADV_TECH_10M_HALF   0x000e
#define TK_OAM_AUTO_ENG_ADV_TECH_10M_FULL   0x008e


#define TK_OAM_AUTO_ENG_ADV_TECH_100M_HALF  0x0019
#define TK_OAM_AUTO_ENG_ADV_TECH_100M_FULL  0x00fc

#define TK_OAM_AUTO_ENG_ADV_TECH_1G         0x0028
#define TK_OAM_AUTO_ENG_ADV_TECH_1G_FULL    0x0192

#define TK_OAM_AUTO_ENG_ADV_TECH_2_5G       0x0134
#define TK_OAM_AUTO_ENG_ADV_TECH_5G         0x0135
#define TK_OAM_AUTO_ENG_ADV_TECH_10G        0x0137

/* 70 00 20: port speed get */
#define TK_OAM_STD_PHY_TYPE_10M             0x0007
#define TK_OAM_STD_PHY_TYPE_100M            0x0018
#define TK_OAM_STD_PHY_TYPE_1G              0x0024

#define TK_OAM_STD_PHY_TYPE_2_5G            0x0134
#define TK_OAM_STD_PHY_TYPE_5G              0x0135
#define TK_OAM_STD_PHY_TYPE_10G_BASE_X      0x0130
#define TK_OAM_STD_PHY_TYPE_10G_BASE_R      0x0131
#define TK_OAM_STD_PHY_TYPE_10G_BASE_W      0x0132
#define TK_OAM_STD_PHY_TYPE_10G_BASE_T      0x0137

/* 07 00 5d: MAC control */
#define TK_OAM_PORT_MAC_CTRL_ENABLE         0x0138
#define TK_OAM_PORT_MAC_CTRL_DISABLE        0x80

/* 07 00 5a: duplex */
#define TK_OAM_PORT_DUPLEX_MODE_HALF        1
#define TK_OAM_PORT_DUPLEX_MODE_FULL        2

/* 07 00 4f: auto-neg status */
#define TK_OAM_PORT_AUTO_NEG_OFF            1
#define TK_OAM_PORT_AUTO_NEG_ON             2

/* 07 00 1a: mac admin status */
#define TK_OAM_PORT_MAC_ADMIN_DISABLE       0
#define TK_OAM_PORT_MAC_ADMIN_ENABLE        1

/* 07 00 47: link status */
#define TK_OAM_PORT_LINK_DOWN               4
#define TK_OAM_PORT_LINK_UP                 3

/* 07 00 33: RSTP state */
typedef enum {
    OAM_TK_RSTP_DISABLED = 1,
    OAM_TK_RSTP_BLOCKING = 2,
    OAM_TK_RSTP_LISTENING = 3,
    OAM_TK_RSTP_LEARNING = 4,
    OAM_TK_RSTP_FORWARDING = 5,
    OAM_TK_RSTP_BROKEN = 6,
} oam_tk_rstp_state_t;

typedef enum {
    TK_OAM=0,

}oam_type_t;


typedef  struct {
    oam_boolean           valid;
    oam_uint8               rule_index;
}tk_cls_index_t;

typedef  struct {
    oam_boolean           valid;
    oam_uint16              port;
    tk_cls_index_t    * cls_index;
    oam_type_t          oam_type;
    union{

        epon_tk_rule_t          tk;
    }rule;

}tk_record_t;


typedef struct {
    tk_cls_index_t   cls[KT_OAM_MAX_CLS];
    tk_record_t      record[KT_OAM_MAX_RULE_RECORD];
}kt_oam_manage_t;

oam_status kt_oam_action_reset_adapt (
                            oam_uint8  force
);

oam_status tk_oam_onu_loop_detect_set_adapt (
                            oam_uint8  enable
);

oam_status kt_oam_onu_clear_statis_adapt ( );


typedef struct{
    oam_int32 maj;
    oam_int32 min;
    oam_int32 rev;
    oam_int32 build;
}oam_kt_fw_version_t;

void tk_oam_fw_version_get_adapt_2(
        oam_kt_fw_version_t * ver);

oam_status tk_oam_fw_version_get_adapt(
        oam_kt_fw_version_t * ver);

oam_status tk_oam_onu_fw_version_get_adapt (
                            tk_oam_onu_fw_version_get_rsp_t  *p_version
);

oam_status tk_oam_onu_model_name_get_adapt (
                            tk_oam_onu_model_name_get_rsp_t  *p_model
);

oam_status tk_oam_block_data_path_set_adapt (
                            oam_uint16  enable
);

oam_status tk_oam_onu_stp_set_adapt (
                            oam_uint8  enable
);

oam_status tk_oam_onu_stp_get_adapt (
                            oam_uint8  *enable
);

oam_status tk_oam_onu_stp_enable_set_adapt(
                            oam_uint8  enable
);

oam_status tk_oam_onu_bpdu2cpu_get_adapt(
                            oam_uint8  *action
);

oam_status tk_oam_onu_loop_detect_set_adapt (
                            oam_uint8       enable
);

oam_status tk_oam_onu_loop_detect_get_adapt (
                            oam_uint8  *enable
);

oam_status tk_oam_gpio_set_adapt (
                            oam_uint32  port_map,
                            oam_uint32  value_map
);

oam_status tk_oam_add_new_rule_adapt (
                            oam_uint8            port,
                            epon_tk_rule_t *p_rule
);

oam_status tk_oam_del_all_rule_adapt (
                            oam_uint8            port
);

oam_status tk_oam_del_new_rule_adapt (
                            oam_uint8            port,
                            epon_tk_rule_t *p_rule
);

oam_status tk_oam_loopback_enable_adapt (
                            oam_port_id_t  port,
                            oam_boolean  enable
);

oam_status tk_oam_mac_limit_set_adapt (
                            oam_uint8   port,
                            oam_uint16  mac_limit
);

oam_status tk_oam_mac_limit_get_adapt (
                            oam_uint8   port,
                            oam_uint16  *mac_limit
);

oam_status tk_oam_mac_limit_00d1_set_adapt (
                            oam_uint8   port,
                            oam_uint16  mac_limit
);

oam_status tk_oam_mac_limit_00d1_get_adapt (
                            oam_uint8   port,
                            oam_uint16  *mac_limit
);

oam_status tk_oam_fdb_age_time_set_adapt (
                            oam_uint16  age_time
);

oam_status tk_oam_fdb_age_time_get_adapt (
                            oam_uint16  *age_time
);

oam_status tk_oam_lrn_mode_set_adapt (
                            oam_uint8  port,
                            oam_uint8  lrn_mode
);

oam_status tk_oam_lrn_mode_get_adapt (
                            oam_uint8  port,
                            oam_uint8  *lrn_mode
);

oam_status tk_oam_uni_phy_admin_adapt (
                            oam_uint8  port,
                            oam_uint8  enable
);

oam_status tk_oam_port_flow_ctrl_enable_adapt (
                            oam_uint8  port,
                            oam_uint8  enable
);

oam_status tk_oam_port_auto_admin_set_adapt (
                            oam_uint8  port,
                            oam_uint8  enable
);

oam_status tk_oam_port_duplex_set_adapt (
                            oam_uint8  port,
                            oam_uint16  duplex
);

oam_status tk_oam_port_auto_neg_tech_adv_set_adapt (
                            oam_uint8  port,
                            oam_uint16  param
);

oam_status tk_oam_port_auto_neg_tech_adv_get_adapt (
                            oam_uint8  port,
                            oam_uint16  *rsp_value
);

oam_status tk_oam_port_status_get_adapt (
                            oam_uint8  port,
                            tk_oam_port_status_t  *p_status
);

oam_status tk_oam_port_vlan_policy_set_adapt (
                            oam_uint16  port,
                            epon_tk_port_vlan_policy_config_t * vlan_policy
);

oam_status tk_oam_port_vlan_policy_get_adapt (
                            oam_uint16  port,
                            epon_tk_port_vlan_policy_config_t * rsp_value
);

oam_status tk_oam_port_vlan_member_clear_adapt(
    oam_uint16  port
);

oam_status tk_oam_port_vlan_member_set_adapt (
                            oam_uint16  port,
                            epon_tk_port_vlan_member_config_t * vlan_mbr
);

oam_status tk_oam_port_vlan_member_get_adapt (
                            oam_uint16  port,
                            oam_uint16 * vlan_count,
                            epon_tk_port_vlan_member_config_t * rsp_value
);

void tk_oam_vlan_member_cfg_adapt(oam_uint16 port);
void tk_oam_sw_vlan_member_printf(oam_uint16 port);
void tk_oam_vlan_debug_set(oam_uint8 enable);


oam_status tk_oam_dync_mac_get_adapt (
                            oam_uint8  port,
                            tk_oam_mac_rules_t  *rules,
                            oam_uint8 *mac_num
);

oam_status tk_oam_port_statistics_get_adapt (
                            oam_uint8  port,
                            tk_oam_onu_port_statis_data_t  *statis
);

oam_status tk_oam_port_mac_limit_get_adapt (
                            oam_uint8  port,
                            oam_uint16  *mac_limit
);

oam_status tk_oam_igmp_grp_get_adapt (
                            tk_oam_igmp_group_info_t  *igmp_grp
);

oam_status tk_oam_igmp_grp_add_adapt (
                            tk_oam_igmp_group_info_t  *grp_info
);

oam_status tk_oam_igmp_grp_del_adapt (
                            tk_oam_igmp_group_info_t  *grp_info
);

oam_status tk_oam_igmp_config_set_adapt (
                            tk_oam_igmp_config_get_t  *igmp_cfg
);

oam_status tk_oam_igmp_config_get_adapt (
                            tk_oam_igmp_config_get_t  *igmp_cfg
);

oam_status tk_oam_report_threshold_set_adapt (
                            oam_uint16  threshold
);

oam_status tk_oam_report_threshold_get_adapt (
                            oam_uint16  *threshold
);

oam_status tk_oam_onu_ds_shaping_set_adapt (
                            oam_uint8 port,
                            oam_uint8 traffic_type,
                            oam_uint8 rate_units,
                            epon_tk_ds_shapping_config_t *ds_shaping
);

oam_status tk_oam_onu_ds_shaping_get_adapt (
                            oam_uint8 port,
                            epon_tk_ds_shapping_traffic_config_t *ds_shaping
);

oam_status tk_oam_onu_us_policer_set_adapt(
    oam_uint8 port,
    oam_uint8 rate_units,
    epon_tk_us_policer_config_t *us_policer
);

oam_status tk_oam_onu_us_policer_get_adapt(
    oam_uint8 port,
    epon_tk_us_policer_traffic_config_t *us_policer
);


oam_status tk_record_find  (
                            oam_uint8                  port,
                            oam_type_t             oam_type,
                            void                     *   rule,
                            tk_record_t        **  out_record

);

oam_status tk_record_add (
                            oam_uint8                    port,
                            oam_type_t              oam_type,
                            tk_cls_index_t   *      cls_index,
                            void                  *      rule
);

oam_status tk_record_del (
                            tk_record_t * record
);


oam_status tk_vlan_rule_is (
                            oam_uint8     field, 
                            oam_uint8     rule_action
);

oam_status _kt_cls_record_del  (
        oam_type_t                   oam_type,
        void                             *oam_config
);

oam_status tk_free_index_get ( tk_cls_index_t   * * cls_index);
oam_status tk_vlan_rule_is (oam_uint8     field, oam_uint8     rule_action);



oam_status  tk_oam_check (epon_tk_rule_t *oam_config);
oam_status  tk_cls_add_adapt (
            epon_tk_rule_t  *       oam_config,
            tk_cls_index_t   * *   output_cls_index
);
oam_status  tk_vlan_add_adapt (oam_uint8  port, epon_tk_rule_t    *oam_config);
oam_status  tk_vlan_del_adapt (oam_uint8  port, epon_tk_rule_t    *oam_config);


void tk_rule_getindex_clr_adapt();

oam_status tk_oam_onu_rules_get_adapt(
                            oam_uint16 port_id,
                            oam_uint8 rsp[],
                            oam_int32 *p_len
);

void stp_send_oam_tk_org_event(ca_uint8_t raised, ca_uint16_t portid);

#define KT_TK_UNI_PORT_FIRST 1
#define KT_TK_PON_PORT       0

#define KT_TK_MAX_MAC_PER_PORT 128

#define TK_RULE_DIRECTION_DOWNSTREAM    0
#define TK_RULE_DIRECTION_UPSTREAM      1

#define TK_RULE_OPERATOR_NEVER_MATCH    0
#define TK_RULE_OPERATOR_EQUAL          1
#define TK_RULE_OPERATOR_NOT_EQUAL      2
#define TK_RULE_OPERATOR_LESS_THAN      3
#define TK_RULE_OPERATOR_GREATER_THAN   4
#define TK_RULE_OPERATOR_EXIST          5
#define TK_RULE_OPERATOR_NOT_EXIST      6
#define TK_RULE_OPERATOR_ALWAYLS_MATCH  7

#define TK_RULE_FIELD_MASK_L2_DA        0
#define TK_RULE_FIELD_MASK_L2_SA        1
#define TK_RULE_FIELD_MASK_LINK_INDEX   2
#define TK_RULE_FIELD_MASK_ETH_TYPE     3
#define TK_RULE_FIELD_MASK_VLAN_ID      4
#define TK_RULE_FIELD_MASK_PRIORITY     5
#define TK_RULE_FIELD_MASK_IP_PROTOCOAL 6
#define TK_RULE_FIELD_MASK_USER         7
#define TK_RULE_FIELD_MASK_DSCP         9

#define TK_RULE_ACTION_SET_DEST_FORWARD 2
#define TK_RULE_ACTION_ADD_TAG          3
#define TK_RULE_ACTION_DEL_TAG          4
#define TK_RULE_ACTION_SETVID_ADD_TAG   5
#define TK_RULE_ACTION_SET_COS          6
#define TK_RULE_ACTION_VLAN_SWAP        8
#define TK_RULE_ACTION_DISCARD          0xe

typedef enum
{
    TK_VLAN_EG_KEEP = 0,
    TK_VLAN_EG_TAG_ADD,
    TK_VLAN_EG_TAG_POP,
    TK_VLAN_EG_TAG_SWAP,
    TK_VLAN_EG_STACKING,
    TK_VLAN_EG_END,
}tk_oam_vlan_eg_mode_e;

#define OAM_TK_PORT_VLAN_NUM_MAX    16
#define OAM_TK_QUEUE_NUM_MAX        4

#define OAM_TK_QUEUE_ID_CHECK(queue)\
        if(queue >= OAM_TK_QUEUE_NUM_MAX) { \
            OAM_ERROR("queue id invalid. %d\r\n",  (queue)); \
            return OAM_E_ERROR; \
        }

oam_status oam_kt_pon_led_set_adapt(oam_uint8 action);

void oam_tk_ext_info_init_adapt(tk_oam_extended_info_t *p_tk_ext_info);

oam_uint8 oam_tk_rstp_port_state_get(oam_uint8 index);

#endif  // end HAVE_KT_OAM

oam_status oam_tk_adapt_init();

#endif // end HAVE_TK_OAM

#endif // end __OAM_TK_ADAPT_H

