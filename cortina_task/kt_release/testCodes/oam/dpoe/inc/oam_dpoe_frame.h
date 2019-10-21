#ifndef __DPOE_OAM_FRAME_H__
#define __DPOE_OAM_FRAME_H__
#ifdef HAVE_DPOE_OAM
#include "oam_types.h"
#include "oam_dpoe_pdu.h"
#include "oam_dpoe.h"

#define CLAUSE_FIELD_LLID       0
#define CLAUSE_FIELD_DA         1
#define CLAUSE_FIELD_SA         2
#define CLAUSE_FIELD_ETYPE      3
#define CLAUSE_FIELD_STAG       7
#define CLAUSE_FIELD_CTAG       8
#define CLAUSE_FIELD_IPTOS      10
#define CLAUSE_FIELD_IPTTL      11
#define CLAUSE_FIELD_IPPT       12
/*Start:This different between 1904.1 and DPOE .pls note that*/ 
#define CLAUSE_FIELD_IPv4DA     15
#define CLAUSE_FIELD_IPv6DA     16
#define CLAUSE_FIELD_IPv4SA     13
#define CLAUSE_FIELD_IPv6SA     14
/*End:This different between 1904.1 and DPOE .pls note that*/ 
#define CLAUSE_FIELD_NEXTHDR    17
#define CLAUSE_FIELD_FL         18
#define CLAUSE_FIELD_L4SP       19
#define CLAUSE_FIELD_L4DP       20
#define CLAUSE_FIELD_BTAG       0x15
#define CLAUSE_FIELD_CUST0      0x18
#define CLAUSE_FIELD_CUST1      0x19
#define CLAUSE_FIELD_CUST2      0x1a
#define CLAUSE_FIELD_CUST3      0x1b
#define CLAUSE_FIELD_CUST4      0x1c
#define CLAUSE_FIELD_CUST5      0x1d
#define CLAUSE_FIELD_CUST6      0x1e
#define CLAUSE_FIELD_CUST7      0x1f
#define CLAUSE_FIELD_LLID_VALUE   0x21

#define RULE_RESULT_NOP     0
#define RULE_RESULT_DISC    1
#define RULE_RESULT_FWD     2
#define RULE_RESULT_QUEUE   3
#define RULE_RESULT_SET     4
#define RULE_RESULT_CPY     5
#define RULE_RESULT_DEL     6
#define RULE_RESULT_INS     7
#define RULE_RESULT_REP     8
#define RULE_RESULT_CLRDEL  9
#define RULE_RESULT_CLRINS  10
#define RULE_RESULT_INC     11

#define RULE_VLAN_OPR_PUSH  1
#define RULE_VLAN_OPR_POP   2

typedef struct
{
    oam_uint8 result_action;
    oam_uint8 result_action_flag;
    oam_uint8 result_queue_flag;
    oam_uint16 result_queue_type;
    oam_uint8 result_queue_instance[8];
    oam_uint8 result_queue_number;
    oam_uint8 result_llid_flag;
    oam_uint16 result_llid;
    oam_uint8 result_l2_da_flag;
    oam_uint8 result_l2_da[6];
    oam_uint8 result_l2_sa_flag;
    oam_uint8 result_l2_sa[6];
    oam_uint8 result_l2_type_flag;
    oam_uint16 result_l2_type;
    oam_uint8 result_vlan_act;
    oam_uint8 result_svlan_flag;
    oam_uint32 result_svlan;
    oam_uint8 result_cvlan_flag;
    oam_uint32 result_cvlan;
    oam_uint8 result_tos_flag;
    oam_uint8 result_tos;
    oam_uint8 result_ip_ttl_flag;
    oam_uint8 result_ip_ttl;
    oam_uint8 result_protocol_flag;
    oam_uint8 result_protocol;
    oam_uint8 result_ipv4_sa_flag;
    oam_uint32 result_ipv4_sa;
    oam_uint8 result_ipv6_sa_flag;
    oam_uint64 result_ipv6_sa;
    oam_uint8 result_ipv4_da_flag;
    oam_uint32 result_ipv4_da;
    oam_uint8 result_ipv6_da_flag;
    oam_uint64 result_ipv6_da;
    oam_uint8 result_ipv6_next_header_flag;
    oam_uint8 result_ipv6_next_header;
    oam_uint8 result_l4_sport_flag;
    oam_uint16 result_l4_sport;
    oam_uint8 result_l4_dport_flag;
    oam_uint16 result_l4_dport;
} dpoe_oam_frame_result_t;

typedef struct
{
    oam_uint8 precedence;
    oam_uint8 llid_flag;
    oam_uint16 llid;
    oam_uint8 l2_da_flag;
    oam_uint8 l2_da[6];
    oam_uint8 l2_da_max_flag;
    oam_uint8 l2_da_max[6];
    oam_uint8 l2_da_min_flag;
    oam_uint8 l2_da_min[6];
    oam_uint8 l2_sa_flag;
    oam_uint8 l2_sa[6];
    oam_uint8 l2_sa_max[6];
    oam_uint8 l2_sa_max_flag;
    oam_uint8 l2_sa_min[6];
    oam_uint8 l2_sa_min_flag;
    oam_uint8 l2_type_flag;
    oam_uint16 l2_type;
    oam_uint8 stpid_flag;
    oam_uint16 stpid;
    oam_uint8 svid_flag;
    oam_uint16 svid;
    oam_uint8 svid_max_flag;
    oam_uint16 svid_max;
    oam_uint8 svid_min_flag;
    oam_uint16 svid_min;
    oam_uint8 ctpid_flag;
    oam_uint16 ctpid;
    oam_uint8 cvid_flag;
    oam_uint16 cvid;
    oam_uint8 cvid_max_flag;
    oam_uint16 cvid_max;
    oam_uint8 cvid_min_flag;
    oam_uint16 cvid_min;
    oam_uint8 cvlan_not_exist;
    oam_uint8 svlan_not_exist;
    oam_uint8 tos_flag;
    oam_uint8 tos;
    oam_uint8 ip_ttl_flag;
    oam_uint8 ip_ttl;
    oam_uint8 protocol_flag;
    oam_uint8 protocol;
    oam_uint8 ipv4_sa_flag;
    oam_uint32 ipv4_sa;
    oam_uint32 ipv4_sa_max;
    oam_uint8 ipv4_sa_max_flag;
    oam_uint8 ipv6_sa_flag;
    oam_uint32 ipv6_sa[4];
    oam_uint8 ipv6_sa_max_flag;
    oam_uint32 ipv6_sa_max[4];
    oam_uint8 ipv4_da_flag;
    oam_uint32 ipv4_da;
    oam_uint8 ipv4_da_max_flag;
    oam_uint32 ipv4_da_max;
    oam_uint8 ipv6_da_flag;
    oam_uint32 ipv6_da[4];
    oam_uint8 ipv6_da_max_flag;
    oam_uint32 ipv6_da_max[4];
    oam_uint8 ipv6_next_header_flag;
    oam_uint8 ipv6_next_header;
    oam_uint8 flow_lable_flag;
    oam_uint32 flow_lable;
    oam_uint8 l4_sport_flag;
    oam_uint16 l4_sport;
    oam_uint8 l4_sport_min_flag;
    oam_uint16 l4_sport_min;
    oam_uint8 l4_sport_max_flag;
    oam_uint16 l4_sport_max;
    oam_uint8 l4_dport_flag;
    oam_uint16 l4_dport;
    oam_uint8 l4_dport_min_flag;
    oam_uint16 l4_dport_min;
    oam_uint8 l4_dport_max_flag;
    oam_uint16 l4_dport_max;
    oam_uint8 lsb_mask;
    oam_uint8 lsb_mask_flag;
    oam_uint8 ecn_flag;
    oam_uint8 ecn;
    oam_uint8 ip_version_flag;
    oam_uint32 ip_version;

    dpoe_oam_frame_result_t result;
} dpoe_oam_frm_process_t;

typedef struct {
    oam_uint8 valid;
    dpoe_oam_mgmt_obj_t mgmt_obj;
    oam_uint8 rule_len;
    oam_uint8 rule[128];
    dpoe_oam_frm_process_t frm_prc;
    oam_uint32 cls_index;
    oam_uint8 is_set;
} dpoe_rule_configured;

typedef struct {
    oam_uint8 valid;
    oam_uint32 cls_index;
} dpoe_cls_traffic;


typedef struct {   
    oam_uint8 port_type;
    oam_uint8 port_id;
    oam_uint32 cls_index;
} dpoe_cls_port_map;


void dpoe_init_cust_field();
void dpoe_port_map_cls_index_init();



#endif


#endif
