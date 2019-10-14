#ifdef HAVE_DPOE_OAM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_dpoe.h"
#include "oam_dpoe_pdu.h"
#include "oam_dpoe_client.h"
#include "oam_dpoe_adapt.h"
#include "oam_mux_par.h"
#include "oam_client.h"
#include "oam_timer.h"
#include "oam_dpoe_frame.h"
#include "classifier.h"
#include "qos.h"



#define CLS_CUST0   0x18
#define CLS_CUST1   0x19
#define CLS_CUST2   0x1a
#define CLS_CUST3   0x1b
#define CLS_CUST4   0x1c
#define CLS_CUST5   0x1d
#define CLS_CUST6   0x1e
#define CLS_CUST7   0x1f

#define CLS_LAYER_PREAMBLE      0
#define CLS_LAYER_8021ah        1
#define CLS_LAYER_ETHTYPE       2
#define CLS_LAYER_STAG          3
#define CLS_LAYER_CTAG          4
#define CLS_LAYER_MPLS          5
#define CLS_LAYER_IPv4          6
#define CLS_LAYER_IPv6          7
#define CLS_LAYER_L3            8
#define CLS_LAYER_TCPUDP        9
#define CLS_LAYER_L4            0xa

typedef struct {
    oam_uint8 valid;
    oam_dpoe_onu_cust_cls_t cust_rule;
} dpoe_cust_field_t;

dpoe_cust_field_t g_cust_field[8];

dpoe_rule_configured g_dpoe_rule[16];
dpoe_rule_configured g_dpoe_cust_rule;
oam_uint8 g_rule_start = FALSE;
dpoe_cls_port_map g_dpoe_cls_port_map[16];
oam_uint8 g_dpoe_cls_port_map_counter = 0;

extern void dpoe_port_map_cls_index_init();

void dpoe_port_map_cls_index_init(){

    memset(g_dpoe_cls_port_map,0,16*sizeof(dpoe_cls_port_map));
}


void dpoe_decode_frm_rule(dpoe_oam_mgmt_obj_t mgmt_obj,oam_uint8 length, oam_uint8 *data);


void dpoe_init_cust_field()
{
    memset(g_cust_field, 0, sizeof(dpoe_cust_field_t)*8);
}

void _dpoe_save_cust_field(oam_dpoe_onu_cust_cls_t *data)
{
    int sel = data->cust - 0x18;
    if (sel >= 8)
        return;
    g_cust_field[sel].valid = 1;
    memcpy(&g_cust_field[sel], data, sizeof(oam_dpoe_onu_cust_cls_t));
}

void _dpoe_get_cust_field(oam_dpoe_onu_cust_cls_t *data){

    int sel = data->cust - 0x18;
    if (sel >= 8)
        return;

    memcpy(data,&g_cust_field[sel],sizeof(oam_dpoe_onu_cust_cls_t));
}


static dpoe_rule_configured *_get_empty_rule()
{
    int i;
    for (i = 0; i < 16; i++)
    {
        if (!g_dpoe_rule[i].valid)
        {
            OAM_ORG_LOG("Now malloc index=%d to store the rule\n\r",i);
            memset(&g_dpoe_rule[i],0,sizeof(dpoe_rule_configured));
            g_dpoe_rule[i].valid = TRUE;
            return &g_dpoe_rule[i];
        }
    }
    return NULL;
}

void _clr_rule_list()
{
    int i;
    memset(g_dpoe_rule, 0, sizeof(dpoe_rule_configured) * 16);
}

oam_uint32 dpoe_oam_rule_conf_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_onu_var_set_hdr_t *data = (oam_dpoe_onu_var_set_hdr_t *)out;
    oam_uint32 pdu_len = 0;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
    if (!g_dpoe_rule.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    memcpy(data->data, g_dpoe_rule.rule, g_dpoe_rule.rule_len);

    data->width = g_dpoe_rule.rule_len;

    pdu_len = sizeof(oam_var_cont_t) + g_dpoe_rule.rule_len;
#endif
    return pdu_len;
}

oam_uint8 is_need_set_drop_unkown_vlan = 0;

oam_uint32 dpoe_oam_rule_conf_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_onu_var_set_hdr_t *data = (oam_dpoe_onu_var_set_hdr_t *)recv;
    oam_var_cont_t *resp = (oam_var_cont_t *)out;
#if 0
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    //diag_printf("dpoe_oam_rule_conf_set\n");
    if(OAM_DPOE_LEAF_OBJ_UNI == mgmt_obj.obj.uni.hdr.leaf){
        is_need_set_drop_unkown_vlan = 1;
        //OAM_ORG_LOG("is_need_set_drop_unkown_vlan = %d\n",is_need_set_drop_unkown_vlan);
    }
    dpoe_decode_frm_rule(mgmt_obj,data->width, data->data);
    is_need_set_drop_unkown_vlan = 0;
    resp->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_cust_rule_conf_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_onu_cust_cls_t *data = (oam_dpoe_onu_cust_cls_t *)out;
    oam_uint32 pdu_len = 0;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    _dpoe_get_cust_field(data);
    data->hdr.width = sizeof(oam_dpoe_onu_cust_cls_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_onu_cust_cls_t);

    return pdu_len;

}

oam_uint32 dpoe_oam_cust_rule_conf_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_onu_cust_cls_t *data = (oam_dpoe_onu_cust_cls_t *)recv;
    oam_var_cont_t *resp = (oam_var_cont_t *)out;
#if 0
    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    _dpoe_save_cust_field(data);

    resp->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);

}
static oam_uint16 g_dpoe_svlan_tpid_value = 0x88A8;
static oam_uint8 g_dpoe_svlan_tpid_default = 0x00;
oam_uint32 dpoe_oam_svlan_tpid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_tpid_t *data = (oam_dpoe_tpid_t *)out;
    oam_uint32 pdu_len = 0;
    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    data->tpid = g_dpoe_svlan_tpid_value;
    data->tpid = htons(data->tpid);
    data->insert = g_dpoe_svlan_tpid_default;
    data->hdr.width = 3;
    pdu_len = sizeof(oam_dpoe_tpid_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_svlan_tpid_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_tpid_t *data = (oam_dpoe_tpid_t *)recv;
    oam_var_cont_t *resp = (oam_var_cont_t *)out;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->tpid = ntohs(data->tpid);
    g_dpoe_svlan_tpid_default = data->insert;
    g_dpoe_svlan_tpid_value = data->tpid;
    resp->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_cvlan_tpid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_tpid_t *data = (oam_dpoe_tpid_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 tpid = 0;
    oam_uint8 insert_flag = 0;
    oam_uint8 port =0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_cvlan_tpid_get_adapt(port,&tpid,&insert_flag);
    data->tpid = tpid;
    data->tpid = htons(data->tpid);
    data->insert = insert_flag;
    data->hdr.width = 3;
    pdu_len = sizeof(oam_dpoe_tpid_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_cvlan_tpid_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    //oam_dpoe_tpid_t *data = (oam_dpoe_tpid_t *)recv;
    oam_var_cont_t *resp = (oam_var_cont_t *)out;
    oam_dpoe_tpid_t *in = (oam_dpoe_tpid_t *)recv;
    oam_uint16 tpid = 0;
    oam_uint8 insert_flag = 0;
    oam_uint8 port =0;

    if (!mgmt_obj.valid) {
        resp->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    port = mgmt_obj.obj.uni.value;
    tpid = ntohs(in->tpid);
    insert_flag = in->insert;

    dpoe_oam_cvlan_tpid_set_adapt(port,tpid,insert_flag);
    resp->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

#define RULE_HEADER_IND     1
#define RULE_CLAUSE_IND     2
#define RULE_RESULT_IND     3
#define RULE_TERMIN_IND     0


oam_uint32 dpoe_decode_rule_clause(dpoe_oam_rule_clause_t *clause, dpoe_rule_configured *rule)
{
    oam_uint8 llid_index;
    oam_uint16 llid;
    oam_uint32 svlan, cvlan;
    oam_uint8 layer;
    oam_uint8 i=0;

    switch(clause->field)
    {
        case CLAUSE_FIELD_LLID:

            rule->frm_prc.llid_flag = 1;
            llid_index = *clause->data;
            OAM_ORG_LOG("    clause field LLID index 0x%x\r\n", llid_index);
            if (llid_index > 7) return -1;
            //if (OAM_E_OK == dpoe_index2llid(llid_index,&llid))
            OAM_ORG_LOG(" llid index %d\r\n", llid_index);
            rule->frm_prc.llid = llid_index;
            break;

        case CLAUSE_FIELD_DA:
            rule->frm_prc.l2_da_flag = 1;
            if (clause->opr == RULE_OPR_EQUAL)
            {
                memcpy(rule->frm_prc.l2_da, clause->data, 6);
                OAM_ORG_LOG("    clause field DA %x %x %x %x %x %x\r\n", rule->frm_prc.l2_da[0], rule->frm_prc.l2_da[1],
                    rule->frm_prc.l2_da[2], rule->frm_prc.l2_da[3], rule->frm_prc.l2_da[4], rule->frm_prc.l2_da[5]);
            }
            else if (clause->opr == RULE_OPR_ALWAYS_MATCH)
            {   rule->frm_prc.l2_da_flag = 0;
                OAM_ORG_LOG(" clause field DA : always match DA\r\n");
            }else if(clause->opr == RULE_OPR_LESS_THAN){
                memcpy(rule->frm_prc.l2_da_max,clause->data, 6);
                rule->frm_prc.l2_da_max_flag = 1;
                OAM_ORG_LOG("clause field DA :RULE_OPR_LESS_THAN\r\n ");
            }else if(clause->opr == RULE_OPR_GREAT_THAN){
                memcpy(rule->frm_prc.l2_da_min, clause->data, 6);
                for(i=0;i<6;i++){
                    rule->frm_prc.l2_da_max[i]=0xFF;
                }
                rule->frm_prc.l2_da_min_flag = 1;
                rule->frm_prc.l2_da_max_flag = 1;
                OAM_ORG_LOG("clause field DA :RULE_OPR_GREAT_THAN\r\n ");
            }
            break;

        case CLAUSE_FIELD_SA:
            rule->frm_prc.l2_sa_flag = 1;
            if (clause->opr == RULE_OPR_EQUAL){
                memcpy(rule->frm_prc.l2_sa, clause->data, 6);
                OAM_ORG_LOG("    clause field SA %x %x %x %x %x %x\r\n", rule->frm_prc.l2_sa[0], rule->frm_prc.l2_sa[1],
                    rule->frm_prc.l2_sa[2], rule->frm_prc.l2_sa[3], rule->frm_prc.l2_sa[4], rule->frm_prc.l2_sa[5]);
            }else if (clause->opr == RULE_OPR_ALWAYS_MATCH){
                rule->frm_prc.l2_sa_flag = 0;
                OAM_ORG_LOG(" clause field SA : always match SA\r\n");
            }else if(clause->opr == RULE_OPR_LESS_THAN){
                 memcpy(rule->frm_prc.l2_sa_max, clause->data, 6);
                 rule->frm_prc.l2_sa_max_flag = 1;
                 OAM_ORG_LOG("clause field SA :RULE_OPR_LESS_THAN\r\n ");
            }else if(clause->opr == RULE_OPR_GREAT_THAN){
                memcpy(rule->frm_prc.l2_sa_min, clause->data, 6);
                for(i=0;i<6;i++){
                    rule->frm_prc.l2_sa_max[i]=0xFF;
                }
                rule->frm_prc.l2_sa_min_flag = 1;
                rule->frm_prc.l2_sa_max_flag = 1;
                OAM_ORG_LOG("clause field SA :RULE_OPR_GREAT_THAN\r\n ");
            }

            break;

        case CLAUSE_FIELD_ETYPE:
            rule->frm_prc.l2_type_flag = 1;
            memcpy(&rule->frm_prc.l2_type, clause->data, 2);
            rule->frm_prc.l2_type = ntohs(rule->frm_prc.l2_type);
            if(rule->frm_prc.l2_type == 0x86DD){
                rule->frm_prc.ip_version_flag = 1;
                rule->frm_prc.ip_version = 6;
                rule->frm_prc.l2_type_flag = 0;
            }
            OAM_ORG_LOG("    clause field ether type 0x%x\r\n", rule->frm_prc.l2_type);
            break;

        case CLAUSE_FIELD_STAG:

            //rule->frm_prc.svid_flag = 1;
            //rule->frm_prc.svid = ntohs(rule->frm_prc.svid);
            //OAM_ORG_LOG("    clause field svlan 0x%x\r\n", rule->frm_prc.svid);
            if (clause->opr == RULE_OPR_EQUAL)
            {
                if (clause->lsb == 16 && clause->msb == 0)
                {
                    memcpy(&rule->frm_prc.stpid, clause->data, 2);
                    rule->frm_prc.stpid_flag = 1;
                    rule->frm_prc.stpid = ntohs(rule->frm_prc.stpid);
                    OAM_ORG_LOG("        clause field tpid 0x%x\r\n", rule->frm_prc.stpid);
                }
                else if (clause->lsb == 0 && clause->msb == 20)
                {
                    memcpy(&rule->frm_prc.svid, clause->data, 2);
                    rule->frm_prc.svid_flag = 1;
                    rule->frm_prc.svid = ntohs(rule->frm_prc.svid);
                    OAM_ORG_LOG("        clause field svid 0x%x\r\n", rule->frm_prc.svid);
                }
#if 0
                if(is_need_set_drop_unkown_vlan){
                    oam_dpoe_port_contl_set();
                    is_need_set_drop_unkown_vlan = 0;
                }
#endif                
            }
            else if (clause->opr == RULE_OPR_NOT_EXIST)
            {
                rule->frm_prc.svlan_not_exist = 1;
                OAM_ORG_LOG("        clause field svid not exist\r\n");
            }else if(clause->opr == RULE_OPR_GREAT_THAN ){
                memcpy(&rule->frm_prc.svid_min, clause->data, 2);
                rule->frm_prc.svid_min_flag = 1;
                rule->frm_prc.svid_min = ntohs(rule->frm_prc.svid_min);
                rule->frm_prc.svid_max_flag = 1;
                rule->frm_prc.svid_max = 4095;

            }else if(clause->opr == RULE_OPR_LESS_THAN ){
                memcpy(&rule->frm_prc.svid_max, clause->data, 2);
                rule->frm_prc.svid_max_flag = 1;
                rule->frm_prc.svid_max = ntohs(rule->frm_prc.svid_max);
                rule->frm_prc.svid_min_flag = 1;
                rule->frm_prc.svid_min = 0;
            }
            #if 0
            else if (clause->lsb == 13 && clause->msb == 16)
            {
                rule->frm_prc.spri_flag = 1;
                rule->frm_prc.spri = (svlan & 0xe000) >> 13;
                OAM_ORG_LOG("        priority 0x%x\r\n", rule->frm_prc.spri);
            }

            #endif
            break;

        case CLAUSE_FIELD_CTAG:
            if (clause->opr == RULE_OPR_EQUAL)
            {
                if (clause->lsb == 16 && clause->msb == 0)
                {
                    memcpy(&rule->frm_prc.ctpid, clause->data, 2);
                    rule->frm_prc.ctpid_flag = 1;
                    rule->frm_prc.ctpid = ntohs(rule->frm_prc.ctpid);
                    OAM_ORG_LOG("        clause field ctpid 0x%x\r\n", rule->frm_prc.ctpid);
                }
                else if (clause->lsb == 0 && clause->msb == 20)
                {
                    memcpy(&rule->frm_prc.cvid, clause->data, 2);
                    rule->frm_prc.cvid_flag = 1;
                    rule->frm_prc.cvid = ntohs(rule->frm_prc.cvid);
                    OAM_ORG_LOG("        clause field cvid 0x%x\r\n", rule->frm_prc.cvid);
                }
#if 0                
                if(is_need_set_drop_unkown_vlan){
                    oam_dpoe_port_contl_set();
                    is_need_set_drop_unkown_vlan = 0;
                }
#endif                
            }
            else if (clause->opr == RULE_OPR_NOT_EXIST)
            {
                rule->frm_prc.cvlan_not_exist = 1;
                OAM_ORG_LOG("        clause field cvid not exist\r\n");
            }else if(clause->opr == RULE_OPR_GREAT_THAN ){
                rule->frm_prc.cvid_flag = 1;
                memcpy(&rule->frm_prc.cvid_min, clause->data, 2);
                rule->frm_prc.cvid_min_flag = 1;
                rule->frm_prc.cvid_min = ntohs(rule->frm_prc.cvid_min);
                rule->frm_prc.cvid_max_flag = 1;
                rule->frm_prc.cvid_max = 4095;
                OAM_ORG_LOG(" RULE_OPR_GREAT_THAN: clause field cvid min 0x%x ,max 0x%x  \r\n", rule->frm_prc.cvid_min,rule->frm_prc.cvid_max);

            }else if(clause->opr == RULE_OPR_LESS_THAN ){
                rule->frm_prc.cvid_flag = 1;
                memcpy(&rule->frm_prc.cvid_max, clause->data, 2);
                rule->frm_prc.cvid_max_flag = 1;
                rule->frm_prc.cvid_max = ntohs(rule->frm_prc.cvid_max);
                rule->frm_prc.cvid_min_flag = 1;
                rule->frm_prc.cvid_min = 0;
                OAM_ORG_LOG("RULE_OPR_LESS_THAN: clause field cvid max 0x%x\r\n", rule->frm_prc.cvid_max);
            }

            break;

        case CLAUSE_FIELD_IPTOS:
            rule->frm_prc.tos_flag = 1;
            rule->frm_prc.ecn_flag = 1;
            rule->frm_prc.tos = *clause->data;
            rule->frm_prc.lsb_mask_flag = 1;
            rule->frm_prc.lsb_mask = clause->lsb;
            if(rule->frm_prc.lsb_mask>=2){
                rule->frm_prc.ecn_flag = 0;
            }
            OAM_ORG_LOG("    clause field tos 0x%x ,lsb_mask=0x%x\r\n", rule->frm_prc.tos,rule->frm_prc.lsb_mask);
            break;

        case CLAUSE_FIELD_IPTTL:
            rule->frm_prc.ip_ttl_flag = 1;
            rule->frm_prc.ip_ttl = *clause->data;
            OAM_ORG_LOG("    clause IP TTL 0x%x\r\n", rule->frm_prc.ip_ttl);
            break;

        case CLAUSE_FIELD_IPPT:
            rule->frm_prc.protocol_flag = 1;
            rule->frm_prc.protocol = *clause->data;
            OAM_ORG_LOG("    clause protocol 0x%x\r\n", rule->frm_prc.protocol);
            break;

        case CLAUSE_FIELD_IPv4DA:
            rule->frm_prc.ipv4_da_flag = 1;
            rule->frm_prc.ip_version_flag = 1;
            rule->frm_prc.ip_version = 4;
            if (clause->opr == RULE_OPR_EQUAL){
                memcpy(&rule->frm_prc.ipv4_da, clause->data, 4);
                rule->frm_prc.ipv4_da = ntohl(rule->frm_prc.ipv4_da);
                OAM_ORG_LOG("IPv4DA:RULE_OPR_EQUAL\n\r");
            }else if(clause->opr == RULE_OPR_ALWAYS_MATCH){
                rule->frm_prc.ipv4_da_flag = 0;
                OAM_ORG_LOG("IPv4DA:RULE_OPR_ALWAYS_MATCH\n\r");
            }else if(clause->opr == RULE_OPR_LESS_THAN){
                memcpy(&rule->frm_prc.ipv4_da_max,clause->data, 4);
                rule->frm_prc.ipv4_da_max_flag = 1;
                rule->frm_prc.ipv4_da_max = ntohl(rule->frm_prc.ipv4_da_max);
                rule->frm_prc.ipv4_da = 0x1;
                OAM_ORG_LOG("IPv4DA:RULE_OPR_LESS_THAN\n\r");
            }else if(clause->opr == RULE_OPR_GREAT_THAN){
                memcpy(&rule->frm_prc.ipv4_da, clause->data, 4);
                rule->frm_prc.ipv4_da = ntohl(rule->frm_prc.ipv4_da);
                rule->frm_prc.ipv4_da_max = 0xFFFFFFFF;
                rule->frm_prc.ipv4_da_max_flag = 1;
                OAM_ORG_LOG("IPv4DA:RULE_OPR_GREAT_THAN\n\r");

            }else if(clause->opr == RULE_OPR_NOT_EQUAL){
                OAM_ORG_LOG("IPv4DA:RULE_OPR_NOT_EQUAL not support\n\r");
                return OAM_E_NOT_SUPPORT;
            }
            rule->frm_prc.lsb_mask_flag = 1;
            rule->frm_prc.lsb_mask = clause->lsb;
            OAM_ORG_LOG("    clause ip da 0x%x, lsb_mask = 0x%x\r\n", rule->frm_prc.ipv4_da,rule->frm_prc.lsb_mask);
            break;

        case CLAUSE_FIELD_IPv6DA:
            rule->frm_prc.ipv6_da_flag = 1;
            rule->frm_prc.ip_version_flag = 1;
            rule->frm_prc.ip_version = 6;
            if (clause->opr == RULE_OPR_EQUAL){
                memcpy(rule->frm_prc.ipv6_da, clause->data, 16);
                for(i=0;i<4;i++){
                    rule->frm_prc.ipv6_da[i] = ntohl(rule->frm_prc.ipv6_da[i]);
                }
                OAM_ORG_LOG("IPv6DA: RULE_OPR_EQUAL\n\r");
            }else if(clause->opr == RULE_OPR_ALWAYS_MATCH){
                rule->frm_prc.ipv6_da_flag = 0;
                OAM_ORG_LOG("IPv6DA:  RULE_OPR_ALWAYS_MATCH\n\r");
            }else if(clause->opr == RULE_OPR_LESS_THAN){
                memcpy(rule->frm_prc.ipv6_da_max, clause->data, 16);
                for(i=0;i<4;i++){
                    rule->frm_prc.ipv6_da_max[i] = ntohl(rule->frm_prc.ipv6_da_max[i]);
                }
                rule->frm_prc.ipv6_da[3]=0x0001;
                rule->frm_prc.ipv6_da_max_flag = 1;
                OAM_ORG_LOG("IPv6DA: RULE_OPR_LESS_THAN\n\r");
            }else if(clause->opr == RULE_OPR_GREAT_THAN){
                memcpy(rule->frm_prc.ipv6_da, clause->data, 16);
                for(i=0;i<4;i++){
                    rule->frm_prc.ipv6_da[i] = ntohl(rule->frm_prc.ipv6_da[i]);
                }
                for(i=0;i<4;i++){
                    rule->frm_prc.ipv6_da_max[i] = 0xFFFFFFFF;
                }

                rule->frm_prc.ipv6_da_max_flag = 1;
                OAM_ORG_LOG("IPv6DA: RULE_OPR_GREAT_THAN\n\r");
            }
            OAM_ORG_LOG("    clause ip v6 da 0x%08x:0x%08x:0x%08x:0x%08x\r\n",
                rule->frm_prc.ipv6_da[0],rule->frm_prc.ipv6_da[1],
                rule->frm_prc.ipv6_da[2],rule->frm_prc.ipv6_da[3]);
            break;

        case CLAUSE_FIELD_IPv4SA:
            rule->frm_prc.ipv4_sa_flag = 1;
            rule->frm_prc.ip_version_flag = 1;
            rule->frm_prc.ip_version = 4;
            if (clause->opr == RULE_OPR_EQUAL){
                memcpy(&rule->frm_prc.ipv4_sa, clause->data, 4);
                rule->frm_prc.ipv4_sa = ntohl(rule->frm_prc.ipv4_sa);
                OAM_ORG_LOG("IPv4SA:RULE_OPR_EQUAL\n\r");
            }else if (clause->opr == RULE_OPR_ALWAYS_MATCH){
                rule->frm_prc.ipv4_sa_flag = 0;
                OAM_ORG_LOG("    always match IPV4_SA\r\n");
            }else if(clause->opr == RULE_OPR_LESS_THAN){
                rule->frm_prc.ipv4_sa_max_flag = 1;
                memcpy(&rule->frm_prc.ipv4_sa_max, clause->data, 4);
                rule->frm_prc.ipv4_sa_max = ntohl(rule->frm_prc.ipv4_sa_max);
                rule->frm_prc.ipv4_sa = 0x1;
                OAM_ORG_LOG("IPv4SA:RULE_OPR_LESS_THAN\n\r");
            }else if(clause->opr == RULE_OPR_GREAT_THAN){
                memcpy(&rule->frm_prc.ipv4_sa, clause->data, 4);
                rule->frm_prc.ipv4_sa = ntohl(rule->frm_prc.ipv4_sa);
                rule->frm_prc.ipv4_sa_max = 0xFFFFFFFF;
                rule->frm_prc.ipv4_sa_max_flag = 1;
                OAM_ORG_LOG("IPv4SA: RULE_OPR_GREAT_THAN\n\r");
            }else if(clause->opr == RULE_OPR_NOT_EQUAL){
                OAM_ORG_LOG("IPv4SA:RULE_OPR_NOT_EQUAL not support\n\r");
                return OAM_E_NOT_SUPPORT;
            }
            rule->frm_prc.lsb_mask_flag = 1;
            rule->frm_prc.lsb_mask = clause->lsb;
            OAM_ORG_LOG("    clause ip sa 0x%08x, lsb_mask = 0x%x\r\n", rule->frm_prc.ipv4_sa,rule->frm_prc.lsb_mask);
            break;

        case CLAUSE_FIELD_IPv6SA:
            rule->frm_prc.ipv6_sa_flag = 1;
            rule->frm_prc.ip_version_flag = 1;
            rule->frm_prc.ip_version = 6;
            if (clause->opr == RULE_OPR_EQUAL){
                memcpy(rule->frm_prc.ipv6_sa, clause->data, 16);
                for(i=0;i<4;i++){
                    rule->frm_prc.ipv6_sa[i] = ntohl(rule->frm_prc.ipv6_sa[i]);
                }
                OAM_ORG_LOG("IPV6SA RULE_OPR_EQUAL\n\r");
            }else if(clause->opr == RULE_OPR_ALWAYS_MATCH){
                rule->frm_prc.ipv6_sa_flag = 0;
                OAM_ORG_LOG("IPV6SA RULE_OPR_ALWAYS_MATCH\n\r");
            }else if(clause->opr == RULE_OPR_LESS_THAN){
                memcpy(rule->frm_prc.ipv6_sa_max, clause->data, 16);
                rule->frm_prc.ipv6_sa_max_flag = 1;
                for(i=0;i<4;i++){
                    rule->frm_prc.ipv6_sa_max[i] = ntohl(rule->frm_prc.ipv6_sa_max[i]);
                }
                rule->frm_prc.ipv6_sa[3]=0x0001;
                OAM_ORG_LOG("IPV6SA  RULE_OPR_LESS_THAN\n\r");
            }else if (clause->opr == RULE_OPR_GREAT_THAN){
                memcpy(rule->frm_prc.ipv6_sa, clause->data, 16);
                for(i=0;i<4;i++){
                    rule->frm_prc.ipv6_sa[i] = ntohl(rule->frm_prc.ipv6_sa[i]);
                }
                for(i=0;i<4;i++){
                    rule->frm_prc.ipv6_sa_max[i] =0xFFFFFFFF;
                }
                rule->frm_prc.ipv6_sa_max_flag = 1;
                OAM_ORG_LOG("IPV6SA  RULE_OPR_GREAT_THAN\n\r");
            }

            OAM_ORG_LOG("    clause ip v6 sa 0x%08x: 0x%08x: 0x%08x: 0x%08x:\r\n",
                rule->frm_prc.ipv6_sa[0],rule->frm_prc.ipv6_sa[1],
                rule->frm_prc.ipv6_sa[2],rule->frm_prc.ipv6_sa[3]);
            break;

        case CLAUSE_FIELD_NEXTHDR:
            rule->frm_prc.ipv6_next_header_flag = 1;
            rule->frm_prc.ipv6_next_header = *clause->data;
            OAM_ORG_LOG("    clause ip v6 next header 0x%x\r\n", rule->frm_prc.ipv6_next_header);
            break;

        case CLAUSE_FIELD_FL:
            rule->frm_prc.flow_lable_flag = 1;
            memcpy(&rule->frm_prc.flow_lable, clause->data, clause->len);
            rule->frm_prc.flow_lable = ntohl(rule->frm_prc.flow_lable);
            OAM_ORG_LOG("clause filed IPv6_FLOWLABEL 0x%0x8x\n\r",rule->frm_prc.flow_lable);
            break;

        case CLAUSE_FIELD_L4SP:
            rule->frm_prc.l4_sport_flag = 1;
            if(clause->opr == RULE_OPR_LESS_THAN){
                rule->frm_prc.l4_sport_max_flag = 1;
                memcpy(&rule->frm_prc.l4_sport_max, clause->data, 2);
                rule->frm_prc.l4_sport_max = ntohs(rule->frm_prc.l4_sport_max);
                OAM_ORG_LOG("    clause ip source port max =0x%x\r\n", rule->frm_prc.l4_sport_max);
            }else if(clause->opr == RULE_OPR_GREAT_THAN){
                rule->frm_prc.l4_sport_min_flag = 1;
                memcpy(&rule->frm_prc.l4_sport_min, clause->data, 2);
                rule->frm_prc.l4_sport_min = ntohs(rule->frm_prc.l4_sport_min);
                OAM_ORG_LOG("    clause ip source port min =0x%x\r\n", rule->frm_prc.l4_sport_min);
            }else{
                memcpy(&rule->frm_prc.l4_sport, clause->data, 2);
                rule->frm_prc.l4_sport = ntohs(rule->frm_prc.l4_sport);
                OAM_ORG_LOG("    clause ip source port = 0x%x\r\n", rule->frm_prc.l4_sport);
            }
            break;

        case CLAUSE_FIELD_L4DP:
            rule->frm_prc.l4_dport_flag = 1;
            if(clause->opr == RULE_OPR_LESS_THAN){
                rule->frm_prc.l4_dport_max_flag = 1;
                memcpy(&rule->frm_prc.l4_dport_max, clause->data, 2);
                rule->frm_prc.l4_dport_max = ntohs(rule->frm_prc.l4_dport_max);
                OAM_ORG_LOG("    clause ip dest port max =0x%x\r\n", rule->frm_prc.l4_dport_max);
            }else if(clause->opr == RULE_OPR_GREAT_THAN){
                rule->frm_prc.l4_dport_min_flag = 1;
                memcpy(&rule->frm_prc.l4_dport_min, clause->data, 2);
                rule->frm_prc.l4_dport_min = ntohs(rule->frm_prc.l4_dport_min);
                OAM_ORG_LOG("    clause ip dest port min =0x%x\r\n", rule->frm_prc.l4_dport_min);

            }else {
                memcpy(&rule->frm_prc.l4_dport, clause->data, 2);
                rule->frm_prc.l4_dport = htons(rule->frm_prc.l4_dport);
                OAM_ORG_LOG("    clause ip dest port 0x%x\r\n", rule->frm_prc.l4_dport);
            }
            break;

        case CLAUSE_FIELD_CUST0:
            if (g_cust_field[0].valid)
            {
                layer = g_cust_field[0].cust_rule.layer;
                switch (layer)
                {
                    case CLS_LAYER_ETHTYPE:
                        rule->frm_prc.l2_type_flag = 1;
                        memcpy(&rule->frm_prc.l2_type, clause->data, 2);
                        rule->frm_prc.l2_type = ntohs(rule->frm_prc.l2_type);
                        break;

                    default:
                        OAM_ORG_LOG("    cust_0 field\r\n");
                        //rule->valid = 0;
                }
            }
            else
                //rule->valid = 0;
            OAM_ORG_LOG("    cust_0 field\r\n");
            break;

        case CLAUSE_FIELD_CUST1:
            if (g_cust_field[1].valid)
            {
                layer = g_cust_field[1].cust_rule.layer;
                switch (layer)
                {
                    case CLS_LAYER_ETHTYPE:
                        rule->frm_prc.l2_type_flag = 1;
                        memcpy(&rule->frm_prc.l2_type, clause->data, 2);
                        rule->frm_prc.l2_type = ntohs(rule->frm_prc.l2_type);
                        break;

                    default:
                        //rule->valid = 0;
                        OAM_ORG_LOG("    cust_1 field\r\n");
                }
            }
            else
                //rule->valid = 0;
            OAM_ORG_LOG("    cust_1 field\r\n");
            break;

        case CLAUSE_FIELD_CUST2:
            if (g_cust_field[2].valid)
            {
                layer = g_cust_field[2].cust_rule.layer;
                switch (layer)
                {
                    case CLS_LAYER_ETHTYPE:
                        rule->frm_prc.l2_type_flag = 1;
                        memcpy(&rule->frm_prc.l2_type, clause->data, 2);
                        rule->frm_prc.l2_type = ntohs(rule->frm_prc.l2_type);
                        break;

                    default:
                        //rule->valid = 0;
                        OAM_ORG_LOG("    cust_2 field\r\n");
                }
            }
            else
                //rule->valid = 0;
            OAM_ORG_LOG("    cust_2 field\r\n");
            break;

        case CLAUSE_FIELD_CUST3:
            if (g_cust_field[3].valid)
            {
                layer = g_cust_field[3].cust_rule.layer;
                switch (layer)
                {
                    case CLS_LAYER_ETHTYPE:
                        rule->frm_prc.l2_type_flag = 1;
                        memcpy(&rule->frm_prc.l2_type, clause->data, 2);
                        rule->frm_prc.l2_type = ntohs(rule->frm_prc.l2_type);
                        break;

                    default:
                        //rule->valid = 0;
                        OAM_ORG_LOG("    cust_3 field\r\n");
                }
            }
            else
                //rule->valid = 0;
            OAM_ORG_LOG("    cust_3 field\r\n");
            break;

        case CLAUSE_FIELD_CUST4:
            if (g_cust_field[4].valid)
            {
                layer = g_cust_field[4].cust_rule.layer;
                switch (layer)
                {
                    case CLS_LAYER_ETHTYPE:
                        rule->frm_prc.l2_type_flag = 1;
                        memcpy(&rule->frm_prc.l2_type, clause->data, 2);
                        rule->frm_prc.l2_type = ntohs(rule->frm_prc.l2_type);
                        break;

                    default:
                        //rule->valid = 0;
                        OAM_ORG_LOG("    cust_4 field\r\n");
                }
            }
            else
                //rule->valid = 0;
            OAM_ORG_LOG("    cust_4 field\r\n");
            break;

        case CLAUSE_FIELD_CUST5:
            if (g_cust_field[5].valid)
            {
                layer = g_cust_field[5].cust_rule.layer;
                switch (layer)
                {
                    case CLS_LAYER_ETHTYPE:
                        rule->frm_prc.l2_type_flag = 1;
                        memcpy(&rule->frm_prc.l2_type, clause->data, 2);
                        rule->frm_prc.l2_type = ntohs(rule->frm_prc.l2_type);
                        break;

                    default:
                        //rule->valid = 0;
                        OAM_ORG_LOG("    cust_5 field\r\n");
                }
            }
            else
                //rule->valid = 0;
            OAM_ORG_LOG("    cust_5 field\r\n");
            break;

        case CLAUSE_FIELD_CUST6:
            if (g_cust_field[6].valid)
            {
                layer = g_cust_field[6].cust_rule.layer;
                switch (layer)
                {
                    case CLS_LAYER_ETHTYPE:
                        rule->frm_prc.l2_type_flag = 1;
                        memcpy(&rule->frm_prc.l2_type, clause->data, 2);
                        rule->frm_prc.l2_type = ntohs(rule->frm_prc.l2_type);
                        break;

                    default:
                        //rule->valid = 0;
                        OAM_ORG_LOG("    cust_5 field\r\n");
                }
            }
            else
                //rule->valid = 0;
            OAM_ORG_LOG("    cust_6 field\r\n");
            break;

        case CLAUSE_FIELD_CUST7:
            if (g_cust_field[7].valid)
            {
                layer = g_cust_field[7].cust_rule.layer;
                switch (layer)
                {
                    case CLS_LAYER_ETHTYPE:
                        rule->frm_prc.l2_type_flag = 1;
                        memcpy(&rule->frm_prc.l2_type, clause->data, 2);
                        rule->frm_prc.l2_type = ntohs(rule->frm_prc.l2_type);
                        break;

                    default:
                        //rule->valid = 0;
                        OAM_ORG_LOG("    cust_7 field\r\n");
                }
            }
            else
                //rule->valid = 0;
            OAM_ORG_LOG("    cust_7 field\r\n");
            break;

        case CLAUSE_FIELD_LLID_VALUE:
            memcpy(&llid, clause->data, 2);
            llid = ntohs(llid);
            OAM_ORG_LOG("    clause field LLID VALUE(0x21)  0x%x\r\n", llid);
            rule->frm_prc.llid_flag = 1;
            rule->frm_prc.llid = llid;
#if 0
            if (OAM_E_OK == dpoe_llid2index(&llid_index, llid))
            {
                rule->frm_prc.llid_flag = 1;

                OAM_ORG_LOG(" llid index %d\r\n", llid_index);
                rule->frm_prc.llid = llid_index;
            }
            else
            {
                OAM_ORG_LOG("cannot find LLID value %d\r\n", llid);
            }
#endif
            break;

        default:
            rule->valid = 0;
            break;
    }

    return OAM_E_OK;
}



oam_uint8 dpoe_decode_rule_result(dpoe_rule_configured *rule, oam_uint8 *data)
{
    oam_uint8 *code = data;
    oam_uint8 len = 0;
    dpoe_oam_rule_result_queue_t *queue = (dpoe_oam_rule_result_queue_t *)data;
    dpoe_oam_rule_result_set_t *set = (dpoe_oam_rule_result_set_t *)data;
    //dpoe_oam_rule_result_copy_t *copy = (dpoe_oam_rule_result_copy_t *)data;
    dpoe_oam_rule_result_delete_t *del = (dpoe_oam_rule_result_delete_t *)data;

    code ++;
    OAM_ORG_LOG("    result %d \r\n", *code);

    switch (*code)
    {
        case RULE_RESULT_NOP:
        case RULE_RESULT_DISC:
        case RULE_RESULT_FWD:
            rule->frm_prc.result.result_action = *code;
            rule->frm_prc.result.result_action_flag = 1;
            len = sizeof(dpoe_oam_rule_result_hdr_t);
            break;

        case RULE_RESULT_QUEUE:
            rule->frm_prc.result.result_queue_flag = 1;
            rule->frm_prc.result.result_queue_type = ntohs(queue->type);
            rule->frm_prc.result.result_queue_instance[rule->frm_prc.result.result_queue_number++] = queue->instance;           
            OAM_ORG_LOG("    result queue type %d, instance %d, number %d\r\n", ntohs(queue->type), queue->instance, rule->frm_prc.result.result_queue_number);
            len = sizeof(dpoe_oam_rule_result_queue_t);
            break;

        case RULE_RESULT_SET:
            switch (set->field)
            {
                case CLAUSE_FIELD_STAG:
                    rule->frm_prc.result.result_svlan_flag = 1;
                    memcpy(&rule->frm_prc.result.result_svlan, set->data, 4);
                    rule->frm_prc.result.result_svlan = ntohl(rule->frm_prc.result.result_svlan);
                    len = sizeof(dpoe_oam_rule_result_set_t) + 4;
                    OAM_ORG_LOG("    result set SVLAN %x\r\n", rule->frm_prc.result.result_svlan);
                    break;

                case CLAUSE_FIELD_CTAG:
                    rule->frm_prc.result.result_cvlan_flag = 1;
                    memcpy(&rule->frm_prc.result.result_cvlan, set->data, 4);
                    rule->frm_prc.result.result_cvlan = ntohl(rule->frm_prc.result.result_cvlan);
                    len = sizeof(dpoe_oam_rule_result_set_t) + 4;
                    OAM_ORG_LOG("    result set CVLAN %x\r\n", rule->frm_prc.result.result_cvlan);
                    break;

                case CLAUSE_FIELD_IPTOS:
                    rule->frm_prc.result.result_tos_flag = 1;
                    rule->frm_prc.result.result_tos = *(set->data+3);
                    len = sizeof(dpoe_oam_rule_result_set_t) + 1;
                    OAM_ORG_LOG("    result set IPTOS %x\r\n", rule->frm_prc.result.result_tos);
                    break;

                default:
                    len = sizeof(dpoe_oam_rule_result_set_t);
                    break;
            }
            break;

        case RULE_RESULT_CPY:
            len = sizeof(dpoe_oam_rule_result_copy_t);
            break;

        case RULE_RESULT_INS:
            rule->frm_prc.result.result_vlan_act = RULE_VLAN_OPR_PUSH;
            len = sizeof(dpoe_oam_rule_result_delete_t);
            break;

        case RULE_RESULT_DEL:
            if (del->field == CLAUSE_FIELD_CTAG || del->field == CLAUSE_FIELD_STAG)
                rule->frm_prc.result.result_vlan_act = RULE_VLAN_OPR_POP;
            len = sizeof(dpoe_oam_rule_result_delete_t);
            break;

        case RULE_RESULT_REP:
        case RULE_RESULT_CLRDEL:
        case RULE_RESULT_CLRINS:
            len = sizeof(dpoe_oam_rule_result_delete_t);
            break;

        case RULE_RESULT_INC:
            len = sizeof(dpoe_oam_rule_result_inc_t);
            break;
    }

    return len;
}


void dpoe_decode_frm_rule(dpoe_oam_mgmt_obj_t mgmt_obj,oam_uint8 length, oam_uint8 *data)
{
    oam_uint8 *indicater = data;
    dpoe_oam_rule_clause_t *clause;
    oam_uint8 remain_len = length;
    static dpoe_rule_configured *rule = NULL;
    int result_len;
    OAM_ORG_LOG("rule indicator %d \n\r", *indicater);
    while (*indicater != RULE_TERMIN_IND && (remain_len > 0))
    {
        switch (*indicater)
        {
            case RULE_HEADER_IND:
                OAM_ORG_LOG(" RULE_HEADER_IND \n\r");
                if(g_rule_start){
                    _clr_rule_list();
                    g_rule_start = FALSE;
                }
                remain_len -= 2;
                // header indicater
                ++indicater;

                rule = _get_empty_rule();
                if (rule == NULL)
                    return;
                rule->frm_prc.precedence = *indicater;
                memcpy(&rule->mgmt_obj,&mgmt_obj,sizeof(dpoe_oam_mgmt_obj_t));
                OAM_ORG_LOG(" precedence in RX Frame is  %d \r\n", *indicater);
                indicater++;
                break;

            case RULE_CLAUSE_IND:
                OAM_ORG_LOG("RULE_CLAUSE_IND \r\n");
                if (rule == NULL)
                    return;
                clause = (dpoe_oam_rule_clause_t *)indicater;
                if((length-7)!=clause->len){
                    //clause->len = length-7;
                    rule->valid = FALSE;
                    OAM_ORG_LOG("Wrong clause len within this rule ,it is invalid\n\r");
                    return;
                }
                dpoe_decode_rule_clause(clause, rule);
                indicater += sizeof(dpoe_oam_rule_clause_t) + clause->len;
                remain_len -= sizeof(dpoe_oam_rule_clause_t) + clause->len;
                break;

            case RULE_RESULT_IND:
                OAM_ORG_LOG(" RULE_RESULT_IND \r\n");
                if (rule == NULL)
                    return;
                result_len = dpoe_decode_rule_result(rule, indicater);
                indicater += result_len;
                remain_len -= result_len;
                break;

            case RULE_TERMIN_IND:
                g_rule_start = FALSE;
                return;
                break;
        }
    }
}

oam_uint32 dpoe_oam_add_rule(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *resp = (oam_var_cont_t *)out;
    dpoe_oam_frm_process_t frm_rule;
    //oam_uint8 port_type;
    oam_uint8 port;
    oam_uint32 cls_index = 0;
    ca_port_id_t ca_port;
    ca_classifier_default_action_t cls_default_action;
    int i;

    memset(&cls_default_action,0,sizeof(ca_classifier_default_action_t));
    memset(&frm_rule, 0, sizeof(dpoe_oam_frm_process_t));
    /*
    if (!g_dpoe_rule.valid || !mgmt_obj.valid) {
        resp->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }*/

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        port = mgmt_obj.obj.uni.value;
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

        ca_classifier_port_default_action_get(0,ca_port,&cls_default_action);
        cls_default_action.action_type = CA_CLASSIFIER_FORWARD_DENY;
        ca_classifier_port_default_action_set(0,ca_port,&cls_default_action);
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        port = mgmt_obj.obj.pon.value;
        for (i = 0; i < 16; i++){
            if (g_dpoe_rule[i].valid){
                dpoe_cls_rule_merge(&g_dpoe_rule[i].frm_prc);
            }
        }

    }
    else
    {
        OAM_ORG_LOG("port type is %d, error!!!\r\n", mgmt_obj.obj.uni.hdr.leaf);
        resp->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    //dpoe_decode_frm_rule(g_dpoe_rule.rule_len, g_dpoe_rule.rule, &frm_rule);

    for (i = 0; i < 16; i++)
    {
        if (g_dpoe_rule[i].valid && (0 == g_dpoe_rule[i].is_set))
        {
            memset(&frm_rule, 0, sizeof(dpoe_oam_frm_process_t));
            frm_rule = g_dpoe_rule[i].frm_prc;
            cls_index = dpoe_config_rule(mgmt_obj.obj.uni.hdr.leaf, port, &frm_rule);
            g_dpoe_rule[i].cls_index = cls_index;
            g_dpoe_rule[i].is_set = 1;
            OAM_ORG_LOG("\n \r cls_index=%d ,cvid_flag = %d ,l2_da_flag =%d ,cvid = %d\n\r",cls_index,frm_rule.cvid_flag,frm_rule.l2_da_flag ,frm_rule.cvid);
            if(g_dpoe_cls_port_map_counter == 16){
                g_dpoe_cls_port_map_counter = 0;
            }
            g_dpoe_cls_port_map[g_dpoe_cls_port_map_counter].cls_index = cls_index;
            if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI){
                g_dpoe_cls_port_map[g_dpoe_cls_port_map_counter].port_type = OAM_DPOE_LEAF_OBJ_UNI;
                g_dpoe_cls_port_map[g_dpoe_cls_port_map_counter].port_id = mgmt_obj.obj.uni.value;
            }else if(mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON){
                g_dpoe_cls_port_map[g_dpoe_cls_port_map_counter].port_type = OAM_DPOE_LEAF_OBJ_PON;
                g_dpoe_cls_port_map[g_dpoe_cls_port_map_counter].port_id = mgmt_obj.obj.pon.value;
            }
            g_dpoe_cls_port_map_counter++;
        }
    }

    resp->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


static void _dpoe_oam_revet_qos_mode(ca_port_id_t port_id){
    ca_qos_config_t qos_cfg;
    memset(&qos_cfg,0,sizeof(ca_qos_config_t));
    OAM_ORG_LOG("\n\r revert dscp_remap_mode \n\r");
    ca_qos_config_get(0,port_id,&qos_cfg);
    qos_cfg.dscp_remap_mode = CA_DSCP_MAP_DSCP_TC_MODE;
    ca_qos_config_set(0,port_id,&qos_cfg);
}

oam_uint32 dpoe_oam_clr_rule(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *resp = (oam_var_cont_t *)out;
    oam_uint8 port;
    ca_port_id_t ca_port;
    ca_classifier_default_action_t cls_default_action;

    memset(&cls_default_action,0,sizeof(ca_classifier_default_action_t));

    dpoe_oam_clr_rule_adapt(mgmt_obj);

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        port = mgmt_obj.obj.uni.value;
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

        ca_classifier_port_default_action_get(0,ca_port,&cls_default_action);
        cls_default_action.action_type = CA_CLASSIFIER_FORWARD_FE;
        ca_classifier_port_default_action_set(0,ca_port,&cls_default_action);
         _dpoe_oam_revet_qos_mode(ca_port);
    }
    resp->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_del_rule(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *resp = (oam_var_cont_t *)out;
    oam_uint8 i= 0;
    oam_uint8 port;
    ca_port_id_t ca_port;
    ca_classifier_default_action_t cls_default_action;

    memset(&cls_default_action,0,sizeof(ca_classifier_default_action_t));

    if((OAM_DPOE_LEAF_OBJ_UNI != mgmt_obj.obj.uni.hdr.leaf)&& (OAM_DPOE_LEAF_OBJ_PON != mgmt_obj.obj.pon.hdr.leaf )){
        OAM_ORG_LOG("port type is %d, error!!!\r\n", mgmt_obj.obj.uni.hdr.leaf);
        resp->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    for (i = 0; i < 16; i++){

        if(g_dpoe_rule[i].valid){
          dpoe_oam_del_rule_adapt(g_dpoe_rule[i].cls_index);
        }
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        port = mgmt_obj.obj.uni.value;
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

        ca_classifier_port_default_action_get(0,ca_port,&cls_default_action);
        cls_default_action.action_type = CA_CLASSIFIER_FORWARD_FE;
        ca_classifier_port_default_action_set(0,ca_port,&cls_default_action);
    }

    resp->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_rule_pmc_fwr_cfg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint32 pdu_len = 0;

    data->hdr.width = 2;
    dpoe_oam_rule_pmc_fwr_cfg_get_adapt(&data->value);
    data->value = htons(data->value);

    pdu_len = sizeof(oam_dpoe_uint16_t);
    return pdu_len;
}


oam_uint32 dpoe_oam_rule_pmc_fwr_cfg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *resp = (oam_var_cont_t *)out;
    oam_dpoe_uint16_t *in = (oam_dpoe_uint16_t *)recv;

    in->value = ntohs(in->value);
    dpoe_oam_rule_pmc_fwr_cfg_set_adapt(in->value);

    resp->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

#endif
