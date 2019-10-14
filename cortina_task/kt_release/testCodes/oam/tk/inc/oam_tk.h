#ifndef __OAM_TK_H__
#define __OAM_TK_H__

#ifdef HAVE_TK_OAM
#include "oam_types.h"
#include "oam_std_pdu.h"

#define OFFSET_LEN (sizeof(tk_oam_tlv_t) - 1 - sizeof(oam_var_desc_t))

#ifdef HAVE_MPORTS
    #define TK_MAX_UNI_NUM  4
#else
    #define TK_MAX_UNI_NUM  1
#endif
#define ONU_MAX_CONDITION_RULE      32
#define ONU_MAX_FILTER      8

#define EPON_MAX_OAMPDU_EVENT_LEN         100

#define IS_TK_OUI(a)        (a[0] == 0 && a[1] == 0x0d && a[2] == 0xb6)

#define TK_IGMP_MAX_GROUP   64

#define TK_OAM_LEAF_LPORT_OBJ                  0x0001
#define TK_OAM_LEAF_PORT_QUEUE_OBJ             0x0002
#define TK_OAM_LEAF_PORT_OBJ                   0x0003
#define TK_OAM_LEAF_QUEUE_OBJ                  0x0004
#define TK_OAM_LEAF_BRIDGE_OBJ                 0x0082
#define TK_OAM_LEAF_BRIDGE_PORT_OBJ            0x0083

#define TK_RULE_FSELECT_L2_DA       0
#define TK_RULE_FSELECT_L2_SA       1
#define TK_RULE_FSELECT_LINK_IDX    2
#define TK_RULE_FSELECT_L2_LENTYPE  3
#define TK_RULE_FSELECT_VLANID      4
#define TK_RULE_FSELECT_PRIO        5
#define TK_RULE_FSELECT_IP_PROTO    6
#define TK_RULE_FSELECT_USER        7

#define TK_RULE_OPER_NOT_MATCH      0
#define TK_RULE_OPER_EQUAL          1
#define TK_RULE_OPER_NOT_EQ         2
#define TK_RULE_OPER_LESS_EQ        3
#define TK_RULE_OPER_GREATER_EQ     4
#define TK_RULE_OPER_EXIST          5
#define TK_RULE_OPER_NOT_EXIST      6
#define TK_RULE_OPER_ALWAYS_MATCH   7

/* TK RULE OPERATOR */
#define TK_RULE_OP_NM     0 /* never match */
#define TK_RULE_OP_EQ     1
#define TK_RULE_OP_NEQ    2
#define TK_RULE_OP_LEQ    3
#define TK_RULE_OP_GEQ    4
#define TK_RULE_OP_EX     5 /* field exists (value ignored) */
#define TK_RULE_OP_NEX    6 /* field not exist (value ignored) */
#define TK_RULE_OP_AM     7 /* always match */

/* TK FIELD SELECTOR */
#define TK_FIELD_L2_DEST_ADDR   0 /* L2 dest addr width 48 bits */
#define TK_FIELD_L2_SRC_ADDR    1 /* L2 src addr  width 48 bits */
#define TK_FIELD_LINK_INDEX     2 /* Link index   width 32 bits */
#define TK_FIELD_L2_LEN_TYPE    3 /* L2 len/Type  width 32 bits */
#define TK_FIELD_VLAN_ID        4 /* VLAN Id      width 32 bits */
#define TK_FIELD_PRIORITY       5 /* Priority     width 32 bits */
#define TK_FIELD_IPV4_PROTOCOL  6 /* IPV4 Proto   width 32 bits */
#define TK_FIELD_USER           7 /* User         width 32 bits */

#define TK_SCHEDULER_SP     1

typedef struct{
    oam_uint16 type;    /* re-use obj leaf */
    oam_uint16 port;
    oam_uint16 link;
    oam_uint16 queue;
}tk_oam_mgmt_obj_t;

typedef oam_uint32 (*tk_oam_get_rountine_t)(
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint8 branch,
    oam_uint16 leaf,
    oam_uint8 *pOut );

typedef oam_uint32 (*tk_oam_set_rountine_t)(
    oam_uint8 *pRecv,
    oam_uint32 *inLen,
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint32 *outLen,
    oam_uint8 *pOut);

typedef struct {
    oam_uint8 branch;
    oam_uint16 leaf;
    tk_oam_get_rountine_t get_func;
    tk_oam_set_rountine_t set_func;
}tk_oam_xlat_rountine_t;

#ifdef HAVE_DPOE_OAM
/******************************************************************************/
/*               TK MSG FOR DPOE project                                      */
/******************************************************************************/

/* TK private oam */
#define TK_OAM_LINK_NUM_MAX   16
#define TK_OAM_PORT_NUM_MAX   4

#define TK_OAM_QUEUE_NUM_MAX  8

#define TK_OAM_SUBTYPE_TERMINATOR   0
#define TK_OAM_SUBTYPE_HEADER       1
#define TK_OAM_SUBTYPE_LINK         2
#define TK_OAM_SUBTYPE_PORT         3

#define TK_OAM_REPORT_MODE_FRAME_ALIGNED_LINK16 1
#define TK_OAM_REPORT_MODE_MULTI_PRI3_LINK8     29
#define TK_OAM_REPORT_MODE_MULTI_PRI4_LINK6     20
#define TK_OAM_REPORT_MODE_MULTI_PRI8_LINK3     24

typedef struct {
    oam_uint8           pri;
    oam_uint8           weight;
    oam_uint8           queue_num;
    oam_uint16          queue_size[TK_OAM_QUEUE_NUM_MAX];
} __attribute__((packed)) tk_oam_queue_cfg_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           subtype;
    oam_uint8           report_mode;
} __attribute__((packed)) tk_oam_subtype_header_t;

typedef struct {
    tk_oam_subtype_header_t head;
    tk_oam_queue_cfg_t      link_queue;
} __attribute__((packed)) tk_oam_subtype_link_t;

typedef struct {
    tk_oam_subtype_header_t head;
    tk_oam_queue_cfg_t      port_queue;
} __attribute__((packed)) tk_oam_subtype_port_t;

typedef struct {
    tk_oam_subtype_header_t head;
} __attribute__((packed)) tk_oam_subtype_terminator_t;

oam_uint32 tk_oam_queue_get(
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint8 branch,
    oam_uint16 leaf,
    oam_uint8 *pOut );

oam_uint32 tk_oam_queue_set(
    oam_uint8 *pRecv,
    oam_uint32 *inLen,
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint32 *outPktLen,
    oam_uint8 *pOut);

oam_uint32 tk_oam_port_mtu_get(
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint8 branch,
    oam_uint16 leaf,
    oam_uint8 *pOut );

oam_uint32 tk_oam_port_mtu_set(
    oam_uint8 *pRecv,
    oam_uint32 *inLen,
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint32 *outPktLen,
    oam_uint8 *pOut);

#define  TK_DPOE_OAM_BUILD_RESP \
    xx(001, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_QUEUE_CONFIG, 0x7FF1, tk_oam_queue_get, tk_oam_queue_set) \
    xx(002, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_MTU_CONFIG,   0x7FF6, tk_oam_port_mtu_get, tk_oam_port_mtu_set)

typedef enum{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC) LEAF=LEAF_VAL,
TK_DPOE_OAM_BUILD_RESP
}tk_dpoe_oam_leaf_e;
#endif

/******************************************************************************/
/*               TK MSG FOR KT project                                        */
/******************************************************************************/

#ifdef HAVE_KT_OAM

#define TK_MAX_MAC_PER_PORT 64   //original vlaue  16

typedef struct {
    oam_uint8 flags;
    oam_uint16 mc_llid;
    oam_uint16 uc_llid;
}__attribute__((packed)) tk_mc_reg_t;

typedef struct {
    oam_uint8 oui[3];
    oam_uint8 opcode;
    tk_mc_reg_t mc_reg;
}__attribute__((packed)) tk_oam_mc_reg_frame_t;


typedef struct {
    oam_uint16 tk_version;
    oam_uint8 tk_oui[3];
    oam_uint8 product[2];
    oam_uint8 version[2];
    oam_uint8 extendedid[64];
    oam_uint8 base_mac[6];
    oam_uint8 max_link;
    oam_uint8 max_port;
    oam_uint8 us_queue_num;
    oam_uint8 us_queue_per_link;
    oam_uint8 kb_per_us_incr;
    oam_uint8 ds_queue_num;
    oam_uint8 ds_queue_per_link;
    oam_uint8 kb_per_ds_incr;
    oam_uint16 kb_us_buffer_av;
    oam_uint16 kb_ds_buffer_av;
    oam_uint16 tk_kedec_man_id;
    oam_uint16 chip_id;
    oam_uint32 chip_ver;
}__attribute__((packed)) tk_oam_extended_info_t;

typedef struct {
    oam_uint8 fselect;
    oam_uint8 mac[OAM_MACADDR_LEN];
    oam_uint8 oper;
}__attribute__((packed)) tk_oam_mac_rule_t;

typedef struct {
    oam_uint8 dir;
    oam_uint8 dest[6];
    oam_uint8 prio;
    oam_uint8 action;
    oam_uint8 count;
    tk_oam_mac_rule_t rules;
}__attribute__((packed)) tk_oam_mac_rules_t;

typedef struct {
    oam_uint8 min;   /*min=0, max=25*/
    oam_uint8 max;
}__attribute__((packed)) tk_oam_rate_t;

typedef struct {
    oam_uint32 threshold;
}__attribute__((packed)) tk_oam_report_threshold_set_t;


typedef struct {
    oam_uint8 group_count;
    oam_uint8 queue_count;
}__attribute__((packed)) tk_oam_igmp_config_get_port_t;

typedef struct {
    oam_uint8 robust_count;
    oam_uint8 lmq_count;
    oam_uint8 uni_count;
    tk_oam_igmp_config_get_port_t ports[TK_MAX_UNI_NUM];
    oam_uint16 grp_fwd_qa;
    oam_uint8 snoop_op;
}__attribute__((packed)) tk_oam_igmp_config_get_t;

typedef struct {
    oam_uint16 mac_table_size;
}__attribute__((packed)) tk_oam_mac_table_size_get_t;

typedef struct {
    oam_uint16 mac_age_limit;
}__attribute__((packed)) tk_oam_mac_age_limit_get_t;

typedef struct {
    oam_uint8 mac_lrn_mode;
}__attribute__((packed)) tk_oam_mac_lrn_mode_get_t;

typedef struct {
    oam_uint16    port;
} __attribute__((packed)) tk_oam_onu_port_statistics_get_t;

typedef struct {
    oam_uint32 group_addr;
    oam_uint8  port_bitmap;
    oam_uint8  reserved[2];
} __attribute__((packed)) tk_oam_igmp_group_t;


typedef struct {
    oam_uint8 group_count;
    tk_oam_igmp_group_t groups[TK_IGMP_MAX_GROUP];
}__attribute__((packed)) tk_oam_igmp_group_info_t;

typedef struct {
    oam_uint8    traffic_type;
    oam_uint32   rate;
} __attribute__((packed)) epon_tk_us_policer_config_t;

typedef struct {
    oam_uint8    rate_units;
    oam_uint8    num_levels;
    epon_tk_us_policer_config_t config;
}__attribute__((packed)) epon_tk_us_policer_traffic_config_t;


typedef struct {
    oam_uint8    queue_bitmap;
    oam_uint32   rate;
    oam_uint16   burst;
    oam_uint8    schedul_algorithm;
    oam_uint8    num_weights;
    oam_uint8    weight;
} __attribute__((packed)) epon_tk_ds_shapping_config_t;

typedef struct {
    oam_uint8    traffic_type;
    oam_uint8    rate_units;
    oam_uint8    num_shapers;
    epon_tk_ds_shapping_config_t config;
}__attribute__((packed)) epon_tk_ds_shapping_traffic_config_t;

typedef struct {
    oam_uint8    port;
    epon_tk_ds_shapping_traffic_config_t   config;
} __attribute__((packed)) tk_oam_onu_ds_shapping_set_t;

typedef struct {
    oam_uint8    port;
} __attribute__((packed)) tk_oam_onu_ds_shapping_get_t;

typedef struct {
    oam_uint8    port;
    epon_tk_ds_shapping_traffic_config_t   config;
} __attribute__((packed)) tk_oam_onu_ds_shapping_get_rsp_t;

typedef struct {
    oam_uint8    traffic_type;
    oam_uint32   max_rate;
} __attribute__((packed)) epon_tk_in_policing_rate_config_t;

typedef struct {
    oam_uint8    rate_unit;
    oam_uint8    num_rate_levels;
    epon_tk_in_policing_rate_config_t   config;
} __attribute__((packed)) tk_oam_onu_in_policing_set_t;

typedef struct {
    oam_uint8    rate_unit;
    oam_uint8    num_rate_levels;
    epon_tk_in_policing_rate_config_t   config;
} __attribute__((packed)) tk_oam_onu_in_policing_get_rsp_t;

typedef struct {
    oam_uint8    mode;
} __attribute__((packed)) tk_oam_onu_rstp_mode_set_t;

typedef struct {
    oam_uint16   min_mac_limit;
} __attribute__((packed)) tk_oam_onu_port_min_mac_limit_set_t;

typedef struct {
    oam_uint8    port;
} __attribute__((packed)) tk_oam_onu_port_min_mac_limit_get_t;

typedef struct {
    oam_uint8    port;
    oam_uint16   min_mac_limit;
} __attribute__((packed)) tk_oam_onu_port_min_mac_limit_get_rsp_t;


typedef struct {
    oam_uint16       fw_version;
} __attribute__((packed)) tk_oam_onu_fw_version_get_rsp_t;


typedef struct {
    oam_uint8       model_name[64];
} __attribute__((packed)) tk_oam_onu_model_name_get_rsp_t;


typedef struct {
    oam_uint16   reserved;
    oam_uint8    ingress_policy;
    oam_uint8    egress_policy;
    oam_uint16   vlan;
} __attribute__((packed)) epon_tk_port_vlan_policy_config_t;

typedef struct {
    oam_uint8    port;
    epon_tk_port_vlan_policy_config_t   config;
} __attribute__((packed)) tk_oam_onu_port_vlan_policy_set_t;

typedef struct {
    oam_uint8    port;
} __attribute__((packed)) tk_oam_onu_port_vlan_policy_get_t;

typedef struct {
    oam_uint8    port;
    epon_tk_port_vlan_policy_config_t   config;
} __attribute__((packed)) tk_oam_onu_port_vlan_policy_get_rsp_t;


typedef struct {
    oam_uint16    ingress_vlan;
    oam_uint16    egress_vlan;
    oam_uint8     ingress_tag;
    oam_uint8     egress_tag;
    oam_uint8     flags;
} __attribute__((packed)) epon_tk_port_vlan_member_config_t;

typedef struct {
    oam_uint16   vlan_count;
    epon_tk_port_vlan_member_config_t   config;
} __attribute__((packed)) tk_oam_onu_port_vlan_member_set_t2;

typedef struct {
    oam_uint8    port;
    oam_uint16   vlan_count;
    epon_tk_port_vlan_member_config_t   config;
} __attribute__((packed)) tk_oam_onu_port_vlan_member_set_t;

typedef struct {
    oam_uint8    port;
} __attribute__((packed)) tk_oam_onu_port_vlan_member_get_t;

typedef struct {
    oam_uint8    port;
    oam_uint16   vlan_count;
    epon_tk_port_vlan_member_config_t   config;
} __attribute__((packed)) tk_oam_onu_port_vlan_member_get_rsp_t;


typedef struct {
    oam_uint8    phy_address;
    oam_uint16   reg_address;
    oam_uint16   mask;
    oam_uint16   value;
} __attribute__((packed)) tk_onu_phy_addr_mdio_config_t;

typedef struct {
    tk_onu_phy_addr_mdio_config_t   config;
} __attribute__((packed)) tk_oam_onu_phy_addr_mdio_set_t;

typedef struct {
    tk_onu_phy_addr_mdio_config_t   config;
} __attribute__((packed)) tk_oam_onu_phy_addr_mdio_get_t;

typedef struct {
    tk_onu_phy_addr_mdio_config_t   config;
} __attribute__((packed)) tk_oam_onu_phy_addr_mdio_get_rsp_t;


typedef struct {
    oam_uint8    port;
    oam_uint8    mode;
} __attribute__((packed)) tk_oam_onu_loopback_enable_set_t;


typedef struct {
    oam_uint8    port;
    oam_uint8    mode;
} __attribute__((packed)) tk_oam_onu_loopback_disable_set_t;

typedef struct {
    oam_uint32   port_map;
    oam_uint32   value_map;
} __attribute__((packed)) tk_oam_onu_gpio_set_t;

typedef struct {
    oam_uint8    field_selector;
    oam_uint8    match_value[6];
    oam_uint8    operator;
} __attribute__((packed)) epon_tk_filter_table_clauses_t;

typedef struct {
    oam_uint8    port;
} __attribute__((packed)) tk_oam_onu_up_filter_table_get_t;

typedef struct {
    oam_uint8    port;
    oam_uint8    direction;
    oam_uint8    onu_port_label[6];
    oam_uint8    priority;
    oam_uint8    action;
    oam_uint8    num_clauses;
    epon_tk_filter_table_clauses_t   clauses[ONU_MAX_FILTER];
} __attribute__((packed)) tk_oam_onu_filter_table_get_rsp_t;

typedef struct {
    oam_uint8    port;
} __attribute__((packed)) tk_oam_onu_down_filter_table_get_t;


typedef struct {
    oam_uint8    field;
    oam_uint8    match_value[8];
    oam_uint8    operator;
} __attribute__((packed)) epon_tk_rule_condition_t;

typedef enum
{
      RuleFlagNonVolatile     = 0x00,
      RuleFlagVolatile        = 0x01            // do not store over reset event
} RuleFlags;                              // one byte long

typedef union
{
      // Old rule format destination, message 0xB1 and 0xB2
    struct
    {
        oam_uint16 port;
        oam_uint16 link;
        oam_uint16 queue;
    } destination;

      // New rule format parameters, message 0xA1
    struct
    {
        oam_uint8 flags;
        union
        {
            struct
            {
                oam_uint8 port_link; // port if downstream, link if upstream
                oam_uint8 queue;
            } destination;
            oam_uint16 vid_cos;
        } ruleParam;
        oam_uint8 reserved;
    } newNameParam;
} OamRuleParam;


typedef struct {
    oam_uint8    direction;
    oam_uint8    action_parameter[6];
    oam_uint8    rule_pre;
    oam_uint8    rule_action;
    oam_uint8    num_rule;
    epon_tk_rule_condition_t   cond[ONU_MAX_CONDITION_RULE];
} __attribute__((packed)) epon_tk_rule_t;

typedef struct {
    oam_uint8    port;
    epon_tk_rule_t  config;
} __attribute__((packed)) tk_oam_onu_down_rule_set_t;

typedef struct {
    oam_uint8    port;
} __attribute__((packed)) tk_oam_onu_down_rule_get_t;

typedef struct {
    oam_uint8    port;
    epon_tk_rule_t  config;
} __attribute__((packed)) tk_oam_onu_down_rule_get_rsp_t;

typedef struct {
    oam_uint8    port;
    epon_tk_rule_t  config;
} __attribute__((packed)) tk_oam_onu_up_rule_set_t;

typedef struct {
    oam_uint8    port;
} __attribute__((packed)) tk_oam_onu_up_rule_get_t;

typedef struct {
    oam_uint8    port;
    epon_tk_rule_t  config;
} __attribute__((packed)) tk_oam_onu_up_rule_get_rsp_t;


typedef struct {
    oam_uint8    port;
    oam_uint8    enable;
} __attribute__((packed)) tk_oam_onu_mac_st_enable_set_t;

typedef struct {
    oam_uint8    port;
} __attribute__((packed)) tk_oam_onu_mac_st_enable_get_t;

typedef struct {
    oam_uint8    port;
    oam_uint8    enable;
} __attribute__((packed)) tk_oam_onu_mac_st_enable_get_rsp_t;


typedef struct {
    oam_uint8    port;
    oam_uint16   value;
} __attribute__((packed)) tk_oam_onu_auto_neg_tech_adv_set_t;

typedef struct {
    oam_uint8    port;
} __attribute__((packed)) tk_oam_onu_auto_neg_tech_adv_get_t;

typedef struct {
    oam_uint8    port;
    oam_uint16   value;
} __attribute__((packed)) tk_oam_onu_auto_neg_tech_adv_get_rsp_t;


typedef struct {
    oam_uint8    port;
    oam_uint8    value;
} __attribute__((packed)) tk_oam_onu_auto_admin_ctrl_set_t;

typedef struct
{
    oam_uint8 phy_type;
    oam_uint16 mac_ctrl;
    oam_uint8 duplex;
    oam_uint8 auto_neg;
    oam_uint8 mac_admin;
    oam_uint8 link;
} __attribute__((packed)) tk_oam_port_status_t;

typedef struct
{
    oam_uint16 priority;
    oam_uint8 mac[6];
}__attribute__((packed)) tk_oam_rstp_designated_root_t;

/******************************************************************************/
/*                          TK MSG                                        */
/******************************************************************************/

typedef struct {
        oam_uint64   in_octets;
        oam_uint64   in_unicast;
        oam_uint64   in_multicast;
        oam_uint64   in_broadcast;
        oam_uint64   in_discards;
        oam_uint64   in_errors;
        oam_uint64   in_unknown_protos;
        oam_uint64   out_octets;
        oam_uint64   out_unicast;
        oam_uint64   out_multicast;
        oam_uint64   out_broadcast;
        oam_uint64   out_discards;
        oam_uint64   out_errors;

        oam_uint64    alignment_errors;
        oam_uint64    fcs_errors;
        oam_uint64    single_collision;
        oam_uint64    multiple_collision;
        oam_uint64    sqe_test_errors;
        oam_uint64    deferred_transmissions;
        oam_uint64    late_collisions;
        oam_uint64    excessive_collisions;
        oam_uint64    internal_mac_tx_errors;
        oam_uint64    carrier_sense_errors;
        oam_uint64    frame_too_longs;
        oam_uint64    internal_mac_rx_errors;
        oam_uint64    symbol_errors;
        oam_uint64   in_unknown_opcode;
        oam_uint64   in_pause_frames;
        oam_uint64   out_pause_frames;

        oam_uint64   in_too_short_octets;
        oam_uint64   in_64_octets;
        oam_uint64   in_65to127_octets;
        oam_uint64   in_128to255_octets;
        oam_uint64   in_256to511_octets;
        oam_uint64   in_512to1023_octets;
        oam_uint64   in_1024t01518_octets;
        oam_uint64   in_1519_plus_octets;
        oam_uint64   out_64_octets;
        oam_uint64   out_65to127_octets;
        oam_uint64   out_128to255_octets;
        oam_uint64   out_256to511_octets;
        oam_uint64   out_512to1023_octets;
        oam_uint64   out_1024t01518_octets;
        oam_uint64   out_1519_plus_octets;

        /* added for pon link */
        oam_uint64  in_mpcp_gate;
        oam_uint64  in_mpcp_register;
        oam_uint64  out_mpcp_report;
        oam_uint64  out_mpcp_reg_request;
        oam_uint64  out_mpcp_reg_ack;

} __attribute__((packed)) tk_oam_onu_port_statis_data_t;

typedef struct {
    oam_uint64 rx_octets;
    oam_uint64 rx_frames;
    oam_uint64 rx_bytes_drop;
    oam_uint64 rx_frames_drop;
    oam_uint64 rx_bytes_delay;
    oam_uint64 rx_delay;
    oam_uint64 rx_delay_threshold;

    oam_uint64 tx_octets;
    oam_uint64 tx_frames;
    oam_uint64 tx_bytes_drop;
    oam_uint64 tx_frames_drop;
    oam_uint64 tx_bytes_delay;
    oam_uint64 tx_delay;
    oam_uint64 tx_delay_threshold;

} __attribute__((packed)) tk_oam_queue_stats_t;

typedef struct {
	oam_uint32 ipAddr;
	oam_uint32 ipMask;
	oam_uint32 gw;
	oam_uint16 vlan;
	oam_uint8  priority;
}__attribute__((packed)) tk_oam_upg_tftp_addr_t;


#define  TK_KT_OAM_BUILD_RESP \
    xx(004, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_DYNC_MAC_GET,  0x00a4, tk_oam_dync_mac_get,0) \
    xx(005, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_DOWN_RULE,  0x00b1, tk_oam_onu_rule_get,0) \
    xx(006, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_UP_RULE,  0x00b2, tk_oam_onu_rule_get,0) \
    xx(007, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_REPORT_THRESHOLD,  0x00a8, tk_oam_report_threshold_get,tk_oam_report_threshold_set) \
    xx(008, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_GETMACTBLSIZE,  0x0082, tk_oam_tbl_size_get, tk_oam_tbl_size_set) \
    xx(009, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_GETAGELIMIT,  0x0083,  tk_oam_onu_age_time_get, tk_oam_onu_age_time_set) \
    xx(010, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_GETLRNMODE,  0x00d0,  tk_oam_onu_lrn_mode_get, tk_oam_onu_lrn_mode_set) \
    xx(011, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ENC_KEY_EXPIRES,  0x00a2, tk_oam_enc_key_expires_get, tk_oam_enc_key_expires_set) \
    xx(012, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_UNICAST_FRAMES,  0x0084, tk_oam_onu_port_statis_get,0) \
    xx(013, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_UNICAST_FRAMES,  0x0085, tk_oam_onu_port_statis_get,0) \
    xx(014, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_TOO_SHORT_FRAMES,  0x0086, tk_oam_onu_port_statis_get,0) \
    xx(015, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_64_FRAMES,  0x0087, tk_oam_onu_port_statis_get,0) \
    xx(016, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_65_127_FRAMES,  0x0088, tk_oam_onu_port_statis_get,0) \
    xx(017, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_128_255_FRAMES,  0x0089, tk_oam_onu_port_statis_get,0) \
    xx(018, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_256_511_FRAMES,  0x008a, tk_oam_onu_port_statis_get,0) \
    xx(019, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_512_1023_FRAMES,  0x008b, tk_oam_onu_port_statis_get,0) \
    xx(020, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_1024_1518_FRAMES,  0x008c, tk_oam_onu_port_statis_get,0) \
    xx(021, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_1519_PLUS_FRAMES,  0x008d, tk_oam_onu_port_statis_get,0) \
    xx(022, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_64_FRAMES,  0x008e,tk_oam_onu_port_statis_get,0) \
    xx(023, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_65_127_FRAMES,  0x008f,tk_oam_onu_port_statis_get,0) \
    xx(024, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_128_255_FRAMES,  0x0090, tk_oam_onu_port_statis_get,0) \
    xx(025, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_256_511_FRAMES,  0x0091, tk_oam_onu_port_statis_get,0) \
    xx(026, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_512_1023_FRAMES,  0x0092, tk_oam_onu_port_statis_get,0) \
    xx(027, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_1024_1518_FRAMES,  0x0093, tk_oam_onu_port_statis_get,0) \
    xx(028, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_1519_PLUS_FRAMES,  0x0094, tk_oam_onu_port_statis_get,0) \
    xx(029, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_DELAY_THRESHOLD,  0x0095, tk_oam_onu_port_statis_get,0) \
    xx(030, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_DELAY,  0x0096, tk_oam_onu_port_statis_get,0) \
    xx(031, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_DROPPED_FRAMES,  0x0097, tk_oam_onu_port_statis_get,0) \
    xx(032, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_DROPPED_BYTES,  0x0098, tk_oam_onu_port_statis_get,0) \
    xx(033, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_DELAYED_BYTES,  0x0099, tk_oam_onu_port_statis_get,0) \
    xx(034, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_UNUSED_BYTES,  0x009a, tk_oam_onu_port_statis_get,0) \
    xx(035, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_DELAY_THRESHOLD,  0x009b, tk_oam_onu_port_statis_get,0) \
    xx(036, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_DELAY,  0x009c, tk_oam_onu_port_statis_get,0) \
    xx(037, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_DROPPED_FRAMES,  0x009d, tk_oam_onu_port_statis_get,0) \
    xx(038, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_DROPPED_BYTES,  0x009e, tk_oam_onu_port_statis_get,0) \
    xx(039, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_DELAYED_BYTES,  0x009f, tk_oam_onu_port_statis_get,0) \
    xx(040, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_10,  0x0010, tk_oam_onu_port_statis_get, 0) \
    xx(041, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_11,  0x0011, tk_oam_onu_port_statis_get,0) \
    xx(042, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_MC_TX_COUNT,  0x0012, tk_oam_onu_port_statis_get,0) \
    xx(043, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_MODE,  0x00c3, tk_oam_onu_bridge_stp_get,tk_oam_onu_bridge_stp_set) \
    xx(044, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_FRAME_TX_OK,  0x0002, tk_oam_onu_port_statis_get,0) \
    xx(045, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_SINGLE_COLLISION_FRAME,  0x0003, tk_oam_onu_port_statis_get,0) \
    xx(046, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_MULTIPLE_COLLISION_FRAME,  0x0004, tk_oam_onu_port_statis_get,0) \
    xx(047, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_FRAMES_RX_OK,  0x0005, tk_oam_onu_port_statis_get,0) \
    xx(048, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_FCS_ERROR,  0x0006, tk_oam_onu_port_statis_get,0) \
    xx(049, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ALIGNMENT_ERROR,  0x0007, tk_oam_onu_port_statis_get,0) \
    xx(050, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_OCTETS_TX_OK,  0x0008, tk_oam_onu_port_statis_get,0) \
    xx(051, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_FRAMES_DEFERRED,  0x0009, tk_oam_onu_port_statis_get,0) \
    xx(052, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_LATE_COLLISIONS,  0x000a, tk_oam_onu_port_statis_get,0) \
    xx(053, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_EXCESSIVE_COLLISIONS,  0x000b, tk_oam_onu_port_statis_get,0) \
    xx(054, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_LOST_MAC_TX_ERROR,  0x000c, tk_oam_onu_port_statis_get,0) \
    xx(055, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_PON_SEN_ERR,  0x000d, tk_oam_onu_port_statis_get,0) \
    xx(056, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_OCTETS_RX_OK,  0x000e, tk_oam_onu_port_statis_get,0) \
    xx(058, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_14,  0x0014, tk_oam_onu_port_statis_get,0) \
    xx(059, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_MC_RX_COUNT,  0x0015, tk_oam_onu_port_statis_get,0) \
    xx(060, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_BC_RX_COUNT,  0x0016, tk_oam_onu_port_statis_get,0) \
    xx(061, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_IN_RANGE_LEN_ERROR,  0x0017, tk_oam_onu_port_statis_get,0) \
    xx(062, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_FRAME_TOO_LONG,  0x0019, tk_oam_onu_port_statis_get,0) \
    xx(063, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_BC_TX_COUNT,  0x0013, tk_oam_onu_port_statis_get,0) \
    xx(064, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_LOST_MAC_RX_ERROR,  0x000f, tk_oam_onu_port_statis_get,0) \
    xx(065, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_MAC_CTRL_PAUSE_TX,  0x0062, tk_oam_onu_port_statis_get,0) \
    xx(066, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_MAC_CTRL_PAUSE_RX,  0x0063, tk_oam_onu_port_statis_get,0) \
    xx(067, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_PHY_ERR,  0x0023,tk_oam_onu_port_statis_get,0) \
    xx(068, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_24,  0x0024,0,0) \
    xx(069, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_MAU_MED_AVAIL, 0x0047,tk_oam_port_status_get,0) \
    xx(070, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_F9,  0x00f9,tk_oam_onu_f9_get,0) \
    xx(071, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_MAC_ST_ENABLE, 0x001a,tk_oam_port_status_get, tk_oam_uni_phy_enable) \
    xx(072, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_LEN_ERR_DISCARD,  0x00a3,tk_oam_port_status_get,0) \
    xx(073, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_AUTONEG_ADMIN,  0x004f, tk_oam_port_status_get, 0) \
    xx(074, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_DUPLEX_STATUS,  0x005a,  tk_oam_port_status_get,0) \
    xx(075, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_MAC_CTRL_SUPP,  0x005d,  tk_oam_port_status_get,0) \
    xx(076, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_PHY_TYPE,  0x0020,  tk_oam_port_status_get,0) \
    xx(077, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_EGRESS_SHAPING,  0x00B8, tk_oam_onu_ds_shaping_get, tk_oam_onu_ds_shaping_set) \
    xx(078, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_INGRSS_POLICING,  0x00b9, tk_oam_port_us_policer_get, tk_oam_port_us_policer_set) \
    xx(079, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_PORT_MIN_MAC_LIMIT,  0x00d1, tk_oam_port_mac_limit_get, tk_oam_port_mac_limit_set) \
    xx(080, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_FW_VERSION,  0x0080,  tk_oam_onu_fw_version_get,0) \
    xx(081, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_MODEL_NAME,  0x0081, tk_oam_onu_model_name_get,0) \
    xx(082, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_PORT_COSTRANSLATE,  0x00ba, tk_oam_onu_port_costranslate_get,0) \
    xx(083, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_PORT_PRIENQUEUE,  0x00bb,   tk_oam_onu_port_prienqueue_get, 0) \
    xx(084, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_PORT_VLAN_POLICY,  0x00bc,  tk_oam_onu_port_vlan_policy_get, tk_oam_onu_port_vlan_policy_set) \
    xx(085, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_PORT_VLAN_MEMBER,  0x00bd, tk_oam_onu_port_vlan_member_get, tk_oam_onu_port_vlan_member_set) \
    xx(086, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_UP_FILTER_TABLE,  0x00a6, tk_oam_onu_filter_table_get,0) \
    xx(087, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_DOWN_FILTER_TABLE,  0x00a7, tk_oam_onu_filter_table_get,0) \
    xx(088, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ONU_AUTO_NEG_TECH_ADV,0x0053, tk_oam_port_auto_neg_tech_adv_get, tk_oam_port_auto_neg_tech_adv_set) \
    xx(089, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_ADD_NEW_RULE,  0x00a1,0, tk_oam_add_new_rule) \
    xx(090, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_DEL_NEW_RULE,  0x00ab,0,  tk_oam_del_new_rule) \
    xx(091, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_DEL_ALL_RULE,  0x00ac,0,   tk_oam_del_all_rule) \
    xx(092, TK_OAM_BRANCH_ACTION, TK_OAM_LEAF_OAM_RATE_SET,  0x00a6,0,  tk_oam_rate_set) \
    xx(093, TK_OAM_BRANCH_ACTION, TK_OAM_LEAF_OAM_RATE_GET,  0x00a7,    tk_oam_rate_get,0) \
    xx(094, TK_OAM_BRANCH_ACTION, TK_OAM_LEAF_GET_QUEUE_CONFIG,  0x0094,  tk_oam_queue_config_get,0) \
    xx(095, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_GET_IGMP_GRP,  0x0098,  0, tk_oam_igmp_grp_get) \
    xx(096, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_ADD_IGMP_GRP,  0x009a,0, tk_oam_igmp_grp_add) \
    xx(097, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_DEL_IGMP_GRP,  0x0099,0,  tk_oam_igmp_grp_del) \
    xx(098, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_SET_IGMP_CONFIG,  0x0096,0,    tk_oam_igmp_config_set) \
    xx(099, TK_OAM_BRANCH_ACTION, TK_OAM_LEAF_GET_IGMP_CONFIG,  0x0097,  0,  tk_oam_igmp_config_get) \
    xx(100, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_TRAFFIC_ENABLE,  0x00a2,0,     tk_oam_traffic_enable) \
    xx(101, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_TRAFFIC_DISABLE,  0x00a3,0,     tk_oam_traffic_disable) \
    xx(102, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_CLEAR_STATS,  0x0088, 0, tk_oam_onu_statis_clear) \
    xx(103, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_ONU_RESET,  0x0087,  0,  tk_oam_onu_reset) \
    xx(104, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_ONU_PHY_ADDR_MDIO_SET,  0x009f,0,     tk_oam_onu_phy_addr_mdio_set) \
    xx(105, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_ONU_PHY_ADDR_MDIO_GET,  0x00a0, tk_oam_onu_phy_addr_mdio_get,0) \
    xx(106, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_LOOPBACK_ENABLE,  0x008f,0,     tk_oam_loopback_enable) \
    xx(107, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_LOOPBACK_DISABLE,  0x0090,0,    tk_oam_loopback_disable) \
    xx(108, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_ONU_AUTO_ADMIN_CTRL,  0x000c,0,   tk_oam_autoneg_admin_set) \
    xx(109, TK_OAM_BRANCH_ACTION, TK_OAM_ACTION_LEAF_GPIO_SET,  0x008c,0,    tk_oam_gpio_set) \
    xx(110, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_MPCP_GATE,  0x0140, tk_oam_onu_port_statis_get,0) \
    xx(111, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_MPCP_REGISTER,  0x0142, tk_oam_onu_port_statis_get,0) \
    xx(112, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_MPCP_REPORT,  0x013f, tk_oam_onu_port_statis_get,0) \
    xx(113, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_MPCP_REG_REQUEST,  0x013e, tk_oam_onu_port_statis_get,0) \
    xx(114, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_MPCP_REG_ACK,  0x013c, tk_oam_onu_port_statis_get,0) \
    xx(115, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_TX_MAC_CTRL_FRAMES,  0x0118, tk_oam_onu_port_statis_get,0) \
    xx(116, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_MAC_CTRL_FRAMES,  0x0119, tk_oam_onu_port_statis_get,0) \
    xx(117, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_ECS_EVENT,  0x00e9, tk_oam_onu_port_statis_get,0) \
    xx(118, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_ONU_HOLDOVER,  0x00e6, tk_oam_onu_port_statis_get,0) \
    xx(119, TK_OAM_BRANCH_ATTRIB, TK_OAM_LEAF_RX_SLE_MODE,  0x00ec, tk_oam_onu_port_statis_get,0) \
    xx(120, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_UPG_TFTP_ADDR, 0x6FFB, 0, tk_oam_onu_upg_tftp_addr_set)\
    xx(121, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_UPG_AUTO_REBOOT, 0x6FFC, 0, tk_oam_onu_upg_tftp_auto_reboot_set)\
    xx(122, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_UPG_DL_STATE, 0x6FFD, tk_oam_onu_upg_tftp_dl_state_get, 0)

#define  TK_BRG_OAM_BUILD_RESP \
    xx(001, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_TIME_SINCE_TOPO_CHANGE,  0x0006, tk_oam_top_change_time_get,0) \
    xx(002, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_TOPO_CHANGE_CNT,  0x0007, tk_oam_topo_change_cnt_get,0) \
    xx(003, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_DESIGNATED_ROOT,  0x0009, tk_oam_designated_root_get,0) \
    xx(004, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ROOT_PATH_COST,  0x000a, tk_oam_root_path_cost_get,0) \
    xx(005, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_ROOT_PORT,  0x000b, tk_oam_root_port_get, 0) \
    xx(006, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_MAX_AGE,  0x000c,  tk_oam_max_age_get, 0) \
    xx(007, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_HELLO_TIME,  0x000d, tk_oam_hello_time_get,0) \
    xx(008, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_FORWARD_DELAY,  0x000e, tk_oam_foward_delay_get, 0) \
    xx(009, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_F,  0x000f, tk_oam_rstp_f_get, 0) \
    xx(010, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_10,  0x0010, tk_oam_rstp_10_get, 0) \
    xx(011, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_11,  0x0011, tk_oam_rstp_11_get, 0) \
    xx(012, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_HOLD_TIME,  0x0012,  tk_oam_hold_time_get, 0) \
    xx(013, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_13,  0x0013,  tk_oam_rstp_13_get, 0) \
    xx(043, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_BRG_RSTP_MODE,  0x00c3, tk_oam_onu_bridge_stp_get,tk_oam_onu_bridge_stp_set)

#define  TK_BRG_PORT_OAM_BUILD_RESP \
    xx(001, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_PORT_STATE,  0x0033, tk_oam_rstp_port_state_get,0) \
    xx(002, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_PORT_23,  0x0023, tk_oam_rstp_port_23_get,0) \
    xx(003, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_PORT_24,  0x0024, tk_oam_rstp_port_24_get,0) \
    xx(004, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_PORT_DESIGNATED_ROOT,  0x0025, tk_oam_rstp_port_designated_root_get,0) \
    xx(005, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_PORT_DESIGNATED_COST,  0x0026, tk_oam_rstp_port_designated_cost_get,0) \
    xx(006, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_PORT_DESIGNATED_BRIDGE,  0x0027, tk_oam_rstp_port_designated_bridge_get,0) \
    xx(007, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_PORT_DESIGNATED_PORT,  0x0028, tk_oam_rstp_port_designated_port_get, 0) \
    xx(008, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RSTP_PORT_TOPO_CHANGE,  0x0008,  tk_oam_rstp_port_topo_change_get, 0)

#define  TK_QUEUE_OAM_BUILD_RESP \
    xx(001, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_STD_MAC_OCTETS_RX_OK,  0x000E, tk_oam_queue_stats_get,0) \
    xx(002, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_STD_MAC_FRAMES_RX_OK,  0x0005, tk_oam_queue_stats_get,0) \
    xx(003, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RX_BYTES_DROPPED,  0x009E, tk_oam_queue_stats_get,0) \
    xx(004, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RX_FRAMES_DROPPED,  0x009D, tk_oam_queue_stats_get,0) \
    xx(005, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RX_BYTES_DELAYED,  0x009F, tk_oam_queue_stats_get,0) \
    xx(006, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RX_DELAY,  0x009C, tk_oam_queue_stats_get,0) \
    xx(007, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_RX_DELAY_THRESHOLD,  0x009B, tk_oam_queue_stats_get, 0) \
    xx(008, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_STD_MAC_OCTETS_TX_OK,  0x0008, tk_oam_queue_stats_get,0) \
    xx(009, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_STD_MAC_FRAMES_TX_OK,  0x0002, tk_oam_queue_stats_get,0) \
    xx(010, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_TX_BYTES_DROPPED,  0x0098, tk_oam_queue_stats_get,0) \
    xx(011, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_TX_FRAMES_DROPPED,  0x0097, tk_oam_queue_stats_get,0) \
    xx(012, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_TX_BYTES_DELAYED,  0x0099, tk_oam_queue_stats_get,0) \
    xx(013, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_TX_DELAY,  0x0096, tk_oam_queue_stats_get,0) \
    xx(014, TK_OAM_BRANCH_ATTRIB, TK_OAM_ATTRIB_LEAF_TX_DELAY_THRESHOLD,  0x0095, tk_oam_queue_stats_get, 0)


typedef enum{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC) LEAF=LEAF_VAL,
TK_KT_OAM_BUILD_RESP
 }tk_kt_oam_leaf_e;


typedef enum{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC) LEAF=LEAF_VAL,
TK_BRG_OAM_BUILD_RESP
 }tk_brg_oam_leaf_e;


typedef enum{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC) LEAF=LEAF_VAL,
TK_BRG_PORT_OAM_BUILD_RESP
 }tk_brg_port_oam_leaf_e;

 typedef enum{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC) LEAF=LEAF_VAL,
    TK_QUEUE_OAM_BUILD_RESP
}tk_queue_oam_leaf_e;
#endif

/*****************************************************************************/

void oam_tk_send(
        oam_uint32 port,
        oam_llid_t llid,
        oam_uint8 *buf,
        oam_int32 len
        );

void handle_tk_oams(
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 len);

void oam_tk_init(); 
#endif

#endif
