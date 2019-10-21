#ifdef HAVE_DPOE_OAM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oam_core.h"
#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_dpoe.h"
#include "oam_dpoe_pdu.h"
#include "oam_dpoe_client.h"
#include "oam_dpoe_adapt.h"
#include "oam_mux_par.h"
#include "oam_client.h"
#include "oam_timer.h"
#include "ca_version.h"
#include "scfg_defs.h"
#include "scfg.h"
#include "mcast.h"
#include "classifier.h"

static void __dpoe_add_cls_to_discard_bc_llid();
static void __dpoe_add_cls_to_discard_mc_llid(ca_llid_t llid_index);
static void __dpoe_delete_discard_mc_llid_cls(ca_llid_t llid_index);

static ca_uint32 g_stcmac_timer_id =0;
static ca_uint32 g_dynmac_timer_id =0;
static oam_uint16 g_dynmac_number = 0;
static oam_uint8 g_dynmac_table[4096*6];
static oam_uint16 g_dynmac_upload_batch = 0;
static oam_uint8 g_llid_count = 1;
static oam_uint8 g_uni_count = 1;
typedef struct
{
    oam_uint8 valid;
    oam_uint16 llid;
    ca_llid_t  llid_instance;
} mc_llid_t;

mc_llid_t g_mc_llid[64];
void dpoe_bridge_init()
{
    memset(g_dynmac_table, 0, sizeof(oam_uint8) * 6 * 4096);
    memset(g_mc_llid, 0, sizeof(mc_llid_t) * 64);
}


void _dpoe_add_mc_llid(oam_uint16 mc_llid,ca_llid_t llid_index)
{
    int i = 0;
    for (i = 0; i < 64; i++)
    {
        if (g_mc_llid[i].valid && g_mc_llid[i].llid == mc_llid)
            return;
    }
    for (i = 0; i < 64; i++)
    {
        if (!g_mc_llid[i].valid)
        {
            g_mc_llid[i].valid = 1;
            g_mc_llid[i].llid = mc_llid;
            g_mc_llid[i].llid_instance = llid_index;
            return;
        }
    }
}

void _dpoe_remove_mc_llid(oam_uint16 mc_llid)
{
    int i =0;
    for (i = 0; i < 64; i++)
    {
        if (g_mc_llid[i].valid && g_mc_llid[i].llid == mc_llid)
        {
            g_mc_llid[i].valid = 0;
            g_mc_llid[i].llid = 0;
            g_mc_llid[i].llid_instance = 0;
            return;
        }
    }
}

void _dpoe_del_all_mc_llid()
{
    memset(g_mc_llid, 0, sizeof(mc_llid_t) * 64);
}

void _dpoe_find_mc_llid_index(oam_uint16 mc_llid,ca_llid_t *llid_index){
    int i =0;
    for (i = 0; i < 64; i++)
    {
        if (g_mc_llid[i].valid && g_mc_llid[i].llid == mc_llid)
        {
            *llid_index = g_mc_llid[i].llid_instance;
            return;
        }
    }

    return;
}
oam_uint32 dpoe_oam_dynmac_size_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint32_t *data = (oam_dpoe_uint32_t *)out;
    oam_uint32 pdu_len = 0;

    data->value = dpoe_oam_dynmac_size_get_adapt();
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_uint32);
    pdu_len = sizeof(oam_dpoe_uint32_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_aging_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint32 pdu_len = 0;

    data->value = dpoe_oam_aging_get_adapt();
    data->value = htons(data->value);

    data->hdr.width = sizeof(oam_uint16);
    pdu_len = sizeof(oam_dpoe_uint16_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_aging_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint16 value;
    oam_dpoe_uint16_t *aging16 = (oam_dpoe_uint16_t *)recv;
    oam_dpoe_uint8_t *aging8 = (oam_dpoe_uint8_t *)recv;
    if (aging8->hdr.width == 1)
        value = aging8->value;
    else if (aging16->hdr.width == 2)
        value = htons(aging16->value);
    else
    {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    dpoe_oam_aging_set_adapt(value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

extern oam_llid_t g_curr_llid;
extern oam_oui_t  oam_oui_dpoe;
static oam_uint32 dpoe_oam_dynmac_table_pkt_send(oam_uint8 *buf,oam_uint8 buf_len,void *cb_data){
    oam_llid_t llid_value;
    oam_int32   outPktLen = 0;
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dpoe_t   *pOutPkt = (oam_pdu_dpoe_t *)pdu;
    oam_pdu_dpoe_obj_uni_t      uni;

    memset(&uni,0,sizeof(oam_pdu_dpoe_obj_uni_t ));
    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    OAM_ORG_LOG("Start to send the rest dymac mac table\n\r");
    llid_value = oam_llid_get();
    if(llid_value == 0xffff){
        OAM_ORG_LOG("not found llid,, llid_value %d\r\n", llid_value);
        return;
    }

    outPktLen = dpoe_oam_build_header(oam_oui_dpoe, llid_value, pdu, OAM_PDU_DPOE_VAR_RSP);

    uni.hdr.branch=0xD6;
    uni.hdr.leaf = htons(0x0003);
    uni.hdr.width = 1;
    uni.value =(oam_uint8*)cb_data;

    memcpy(pOutPkt->data,&uni,sizeof(oam_pdu_dpoe_obj_uni_t));
    outPktLen+=sizeof(oam_pdu_dpoe_obj_uni_t);

    memcpy(pOutPkt->data+sizeof(oam_pdu_dpoe_obj_uni_t),buf,buf_len);

    outPktLen+=buf_len;
    dpoe_oam_send(llid_value,pOutPkt,outPktLen);

    return CA_E_OK;
}


static void dpoe_oam_dynmac_table_callback(void *cb_data){
    oam_uint16 remain_not_send_num=0,sequence_id =0,remain_num =0,buf_len =0;
    oam_uint8 *ptr_mac=NULL;
    oam_uint8 buf[OAM_MAX_OAMPDU_SIZE];
    oam_uint8 i=0;
    oam_dpoe_uint16_t *sequence_tlv=NULL ;
    oam_dpoe_mac_t *data=NULL;

    memset(buf, 0, OAM_MAX_OAMPDU_SIZE);
    ptr_mac = g_dynmac_table;
    remain_not_send_num=g_dynmac_number-21;
    remain_num = g_dynmac_number-21;
    ptr_mac+=21*6;

    data = (oam_dpoe_mac_t *)buf;

    if(remain_not_send_num<=21){
        OAM_ORG_LOG("First ,remain_not_send_num=%d\n\r",remain_not_send_num);
        data->hdr.branch = 0xd7;
        data->hdr.leaf = htons(0x0103);
        data->hdr.width = 6*remain_not_send_num;
        memcpy(data->mac,ptr_mac,remain_not_send_num);

        buf_len+=remain_not_send_num*6+sizeof(oam_var_cont_t);
        data = (oam_uint8 *)data+buf_len;

        sequence_tlv = (oam_dpoe_uint16_t *)((oam_uint8 *)data);
        sequence_tlv->hdr.branch = 0xD7;
        sequence_tlv->hdr.leaf = htons(0x0001);
        sequence_tlv->hdr.width = 0x02;
        sequence_id = 0x8001;
        sequence_tlv->value = htons(sequence_id);

        buf_len+=sizeof(oam_dpoe_uint16_t);
        OAM_ORG_LOG("sequence_id = 0x%x,sequence_tlv->value = 0x%x,buf_len=%d\n\r",sequence_id,sequence_tlv->value,buf_len);
        dpoe_oam_dynmac_table_pkt_send(buf,buf_len,cb_data);
        return;
    }


    for(i=0;i<remain_not_send_num/21;i++){
        buf_len =0;
        memset(buf, 0, OAM_MAX_OAMPDU_SIZE);
        data = (oam_dpoe_mac_t *)buf;

        data->hdr.branch = 0xd7;
        data->hdr.leaf = htons(0x0103);
        data->hdr.width = 21*6;
        memcpy(data->mac,ptr_mac,21*6);

        buf_len+= data->hdr.width+sizeof(oam_var_cont_t);
        data = (oam_uint8 *)data+buf_len;

        sequence_tlv = (oam_dpoe_uint16_t *)((oam_uint8 *)data);
        sequence_tlv->hdr.branch = 0xD7;
        sequence_tlv->hdr.leaf = htons(0x0001);
        sequence_tlv->hdr.width = 0x02;
        sequence_id = i+1;
        sequence_tlv->value = htons(sequence_id);

        buf_len+=sizeof(oam_var_cont_t)+2;
        OAM_ORG_LOG("sequence_id = 0x%x,sequence_tlv->value = 0x%x,buf_len=%d\n\r",sequence_id,sequence_tlv->value,buf_len);

        dpoe_oam_dynmac_table_pkt_send(buf,buf_len,cb_data);
        remain_num-=21;
        //data = (oam_dpoe_mac_t *)((oam_uint8*)sequence_tlv+sizeof(oam_dpoe_uint16_t));
        ptr_mac+=21*6;
    }


    if(remain_num<=21){
        OAM_ORG_LOG("remain_num=%d\n\r",remain_num);
        buf_len =0;
        data = (oam_dpoe_mac_t *)buf;

        data->hdr.branch = 0xd7;
        data->hdr.leaf = htons(0x0103);
        data->hdr.width = 6*remain_num;
        memcpy(data->mac,ptr_mac,remain_num);

        buf_len+=remain_num*6+sizeof(oam_var_cont_t);
        data = (oam_uint8 *)data+buf_len;

        sequence_tlv = (oam_dpoe_uint16_t *)((oam_uint8 *)data);
        sequence_tlv->hdr.branch = 0xD7;
        sequence_tlv->hdr.leaf = htons(0x0001);
        sequence_tlv->hdr.width = 0x02;
        sequence_id = 0x8000+i+1;
        sequence_tlv->value = htons(sequence_id);

        buf_len+=sizeof(oam_var_cont_t)+2;
        OAM_ORG_LOG("sequence_id = 0x%x,sequence_tlv->value = 0x%x,buf_len=%d\n\r",sequence_id,sequence_tlv->value,buf_len);
        dpoe_oam_dynmac_table_pkt_send(buf,buf_len,cb_data);
    }
    g_dynmac_timer_id = 0;
    return;
}

oam_uint32 dpoe_oam_dynmac_table_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_mac_t *data = (oam_dpoe_mac_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 *ptr_mac;
    int i,k;
    oam_uint16 total_number, remain_number,sequence_id;
    oam_dpoe_mac_t *ptr_data = data;
    oam_dpoe_uint16_t *sequence_tlv ;
    oam_var_cont_t tlv_end;

    memset(&tlv_end,0,sizeof(oam_dpoe_uint8_t));

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_dynmac_table_get_adapt(mgmt_obj.obj.uni.value, &g_dynmac_number, g_dynmac_table);
    total_number = (g_dynmac_number>210)?210:g_dynmac_number;
    OAM_ORG_LOG("dpoe_oam_dynmac_table_get:g_dynmac_number =%d,total_number=%d\n\r",g_dynmac_number,total_number);

    if(g_dynmac_number == 0){
        data->hdr.width = OAM_DPOE_RSP_OK;
        return sizeof(oam_var_cont_t);
    }

    if (g_dynmac_number <= 21)
      {
          memcpy(data->mac, g_dynmac_table, g_dynmac_number*6);
          data->hdr.width = g_dynmac_number*6;
          pdu_len = sizeof(oam_var_cont_t) + g_dynmac_number*6;

          tlv_end.branch = 0xD7;
          tlv_end.leaf = htons(0x0103);
          tlv_end.width = 0x80;
          memcpy((oam_uint8 *)data+sizeof(oam_var_cont_t)+g_dynmac_number*6,&tlv_end,sizeof(oam_var_cont_t));      
          pdu_len+=sizeof(oam_var_cont_t);

          return pdu_len;
      }

    ptr_mac = g_dynmac_table;
    remain_number = total_number;
    pdu_len = 0;

    memcpy(ptr_data->mac, ptr_mac, 21*6);
    ptr_data->hdr.width=21*6;
    pdu_len += sizeof(oam_var_cont_t)+21*6;
    ptr_mac +=21*6;
    ptr_data = (oam_dpoe_mac_t *)((oam_uint8 *)ptr_data+sizeof(oam_var_cont_t)+21*6);
    remain_number-=21;
    total_number-=21;

    for(i=0;i<total_number/21;i++){
        ptr_data->hdr.branch = 0xD7;
        ptr_data->hdr.leaf = htons(0x0103);
        ptr_data->hdr.width = 21*6;
        memcpy(ptr_data->mac,ptr_mac,21*6);
        ptr_data = (oam_dpoe_mac_t *)((oam_uint8 *)ptr_data+sizeof(oam_var_cont_t)+21*6);
        ptr_mac+=21*6;
        pdu_len+=21*6+sizeof(oam_var_cont_t);
        remain_number-=21;
    }

    if(remain_number<=21){
        ptr_data->hdr.branch = 0xD7;
        ptr_data->hdr.leaf = htons(0x0103);
        ptr_data->hdr.width = remain_number*6;
        memcpy(ptr_data->mac,ptr_mac,remain_number*6);
        pdu_len+=sizeof(oam_var_cont_t)+remain_number*6;

        tlv_end.branch = 0xD7;
        tlv_end.leaf = htons(0x0103);
        tlv_end.width = 0x80;

        memcpy((oam_uint8 *)ptr_data+sizeof(oam_var_cont_t)+remain_number*6,&tlv_end,sizeof(oam_var_cont_t));
        remain_number = 0;
        pdu_len+=sizeof(oam_var_cont_t);
    }


    //implement for sequence
#if 0
    dpoe_oam_dynmac_table_get_adapt(mgmt_obj.obj.uni.value, &g_dynmac_number, g_dynmac_table);
    total_number = g_dynmac_number;
    OAM_ORG_LOG("dpoe_oam_dynmac_table_get:g_dynmac_number =%d,total_number=%d\n\r",g_dynmac_number,total_number);
    if (g_dynmac_number <= 21)
    {
        memcpy(data->mac, g_dynmac_table, g_dynmac_number*6);
        data->hdr.width = g_dynmac_number*6;
        pdu_len = sizeof(oam_var_cont_t) + g_dynmac_number*6;

        return pdu_len;
    }

    ptr_mac = g_dynmac_table;
    remain_number = total_number;
    pdu_len = 0;

    memcpy(ptr_data->mac, ptr_mac, 21*6);
    data->hdr.width=21*6;
    pdu_len += sizeof(oam_var_cont_t)+21*6;

    ptr_mac += 21*6;
    sequence_tlv = (oam_dpoe_uint16_t *)((oam_uint8 *)ptr_data + sizeof(oam_var_cont_t) + ptr_data->hdr.width);
    sequence_tlv->hdr.branch = 0xD7;
    sequence_tlv->hdr.leaf = htons(0x0001);
    sequence_tlv->hdr.width = 0x02;
    sequence_id = 0;
    sequence_tlv->value = htons(sequence_id);

    pdu_len += sizeof(oam_var_cont_t)+2;

    if(g_dynmac_timer_id!=0){
        ca_timer_del(g_dynmac_timer_id);
        g_dynmac_timer_id = 0;
    }

    g_dynmac_timer_id =ca_timer_add(100,dpoe_oam_dynmac_table_callback,(void *)mgmt_obj.obj.uni.value);
#endif
    return pdu_len;
}


static oam_uint16 g_stcmac_number = 0;
static oam_uint8 g_stcmac_table[4096*6];
static oam_uint16 g_stcmac_upload_batch = 0;

static oam_uint32 dpoe_oam_stcmac_table_pkt_send(oam_uint8 *buf,oam_uint8 buf_len,void *cb_data){
    oam_llid_t llid_value;
    oam_int32   outPktLen = 0;
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dpoe_t   *pOutPkt = (oam_pdu_dpoe_t *)pdu;
    oam_pdu_dpoe_obj_uni_t      uni;

    memset(&uni,0,sizeof(oam_pdu_dpoe_obj_uni_t ));
    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    OAM_ORG_LOG("Start to send the rest stcmac mac table\n\r");
    llid_value = oam_llid_get();
    if(llid_value == 0xffff){
        OAM_ORG_LOG("not found llid,, llid_value %d\r\n", llid_value);
        return;
    }

    outPktLen = dpoe_oam_build_header(oam_oui_dpoe, llid_value, pdu, OAM_PDU_DPOE_VAR_RSP);

    uni.hdr.branch=0xD6;
    uni.hdr.leaf = htons(0x0003);
    uni.hdr.width = 1;
    uni.value =(oam_uint8*)cb_data;

    memcpy(pOutPkt->data,&uni,sizeof(oam_pdu_dpoe_obj_uni_t));
    outPktLen+=sizeof(oam_pdu_dpoe_obj_uni_t);

    memcpy(pOutPkt->data+sizeof(oam_pdu_dpoe_obj_uni_t),buf,buf_len);

    outPktLen+=buf_len;
    dpoe_oam_send(llid_value,pOutPkt,outPktLen);

    return CA_E_OK;
}


static void dpoe_oam_stcmac_table_callback(void *cb_data){
    oam_uint16 remain_not_send_num=0,sequence_id =0,remain_num =0,buf_len =0;
    oam_uint8 *ptr_mac=NULL;
    oam_uint8 buf[OAM_MAX_OAMPDU_SIZE];
    oam_uint8 i=0;
    oam_dpoe_uint16_t *sequence_tlv=NULL ;
    oam_dpoe_mac_t *data=NULL;

    memset(buf, 0, OAM_MAX_OAMPDU_SIZE);
    ptr_mac = g_stcmac_table;
    remain_not_send_num=g_stcmac_number-21;
    remain_num = g_stcmac_number-21;
    ptr_mac+=21*6;

    data = (oam_dpoe_mac_t *)buf;

    if(remain_not_send_num<=21){
        OAM_ORG_LOG("First ,remain_not_send_num=%d\n\r",remain_not_send_num);
        data->hdr.branch = 0xd7;
        data->hdr.leaf = htons(0x0103);
        data->hdr.width = 6*remain_not_send_num;
        memcpy(data->mac,ptr_mac,remain_not_send_num);

        buf_len+=remain_not_send_num*6+sizeof(oam_var_cont_t);
        data = (oam_uint8 *)data+buf_len;

        sequence_tlv = (oam_dpoe_uint16_t *)((oam_uint8 *)data);
        sequence_tlv->hdr.branch = 0xD7;
        sequence_tlv->hdr.leaf = htons(0x0001);
        sequence_tlv->hdr.width = 0x02;
        sequence_id = 0x8001;
        sequence_tlv->value = htons(sequence_id);

        buf_len+=sizeof(oam_dpoe_uint16_t);
        OAM_ORG_LOG("sequence_id = 0x%x,sequence_tlv->value = 0x%x,buf_len=%d\n\r",sequence_id,sequence_tlv->value,buf_len);
        dpoe_oam_stcmac_table_pkt_send(buf,buf_len,cb_data);
        return;
    }


    for(i=0;i<remain_not_send_num/21;i++){
        buf_len =0;
        memset(buf, 0, OAM_MAX_OAMPDU_SIZE);
        data = (oam_dpoe_mac_t *)buf;

        data->hdr.branch = 0xd7;
        data->hdr.leaf = htons(0x0103);
        data->hdr.width = 21*6;
        memcpy(data->mac,ptr_mac,21*6);

        buf_len+= data->hdr.width+sizeof(oam_var_cont_t);
        data = (oam_uint8 *)data+buf_len;

        sequence_tlv = (oam_dpoe_uint16_t *)((oam_uint8 *)data);
        sequence_tlv->hdr.branch = 0xD7;
        sequence_tlv->hdr.leaf = htons(0x0001);
        sequence_tlv->hdr.width = 0x02;
        sequence_id = i+1;
        sequence_tlv->value = htons(sequence_id);

        buf_len+=sizeof(oam_var_cont_t)+2;
        OAM_ORG_LOG("sequence_id = 0x%x,sequence_tlv->value = 0x%x,buf_len=%d\n\r",sequence_id,sequence_tlv->value,buf_len);

        dpoe_oam_stcmac_table_pkt_send(buf,buf_len,cb_data);
        remain_num-=21;
        //data = (oam_dpoe_mac_t *)((oam_uint8*)sequence_tlv+sizeof(oam_dpoe_uint16_t));
        ptr_mac+=21*6;
    }


    if(remain_num<=21){
        OAM_ORG_LOG("remain_num=%d\n\r",remain_num);
        buf_len =0;
        data = (oam_dpoe_mac_t *)buf;

        data->hdr.branch = 0xd7;
        data->hdr.leaf = htons(0x0103);
        data->hdr.width = 6*remain_num;
        memcpy(data->mac,ptr_mac,remain_num);

        buf_len+=remain_num*6+sizeof(oam_var_cont_t);
        data = (oam_uint8 *)data+buf_len;

        sequence_tlv = (oam_dpoe_uint16_t *)((oam_uint8 *)data);
        sequence_tlv->hdr.branch = 0xD7;
        sequence_tlv->hdr.leaf = htons(0x0001);
        sequence_tlv->hdr.width = 0x02;
        sequence_id = 0x8000+i+1;
        sequence_tlv->value = htons(sequence_id);

        buf_len+=sizeof(oam_var_cont_t)+2;
        OAM_ORG_LOG("sequence_id = 0x%x,sequence_tlv->value = 0x%x,buf_len=%d\n\r",sequence_id,sequence_tlv->value,buf_len);
        dpoe_oam_stcmac_table_pkt_send(buf,buf_len,cb_data);
    }
    g_stcmac_timer_id = 0;
    return;
}

oam_uint32 dpoe_oam_stcmac_table_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_mac_t *data = (oam_dpoe_mac_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 *ptr_mac;
    int i;
    oam_uint16 total_number, remain_number;
    oam_dpoe_mac_t *ptr_data = data;
    oam_dpoe_uint16_t *sequence_tlv ;
    oam_uint16 sequence_id = 0;
    oam_var_cont_t tlv_end;

    memset(&tlv_end,0,sizeof(oam_dpoe_uint8_t));

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_stcmac_table_get_adapt(mgmt_obj.obj.uni.value, &g_stcmac_number, g_stcmac_table);
    total_number = (g_stcmac_number>210)?210:g_stcmac_number;
    OAM_ORG_LOG("dpoe_oam_stcmac_table_get:g_stcmac_number =%d,total_number=%d\n\r",g_stcmac_number,total_number);

   if(g_stcmac_number == 0){
        data->hdr.width = OAM_DPOE_RSP_OK;
        return sizeof(oam_var_cont_t);
    }

    if (g_stcmac_number <= 21)
    {
        memcpy(data->mac, g_stcmac_table, g_stcmac_number*6);
        data->hdr.width = g_stcmac_number*6;
        pdu_len = sizeof(oam_var_cont_t) + g_stcmac_number*6;
        return pdu_len;
    }

    ptr_mac = g_stcmac_table;
    remain_number = total_number;
    pdu_len = 0;

    memcpy(ptr_data->mac, ptr_mac, 21*6);
    ptr_data->hdr.width=21*6;
    pdu_len += sizeof(oam_var_cont_t)+21*6;
    ptr_mac += 21*6;
    ptr_data = (oam_dpoe_mac_t *)((oam_uint8 *)ptr_data+sizeof(oam_var_cont_t)+21*6);
    remain_number-=21;
    total_number-=21;

    for(i=0;i<total_number/21;i++){
        ptr_data->hdr.branch = 0xD7;
        ptr_data->hdr.leaf = htons(0x0104);
        ptr_data->hdr.width = 21*6;
        memcpy(ptr_data->mac,ptr_mac,21*6);
        ptr_data = (oam_dpoe_mac_t *)((oam_uint8 *)ptr_data+sizeof(oam_var_cont_t)+21*6);
        ptr_mac+=21*6;
        pdu_len+=21*6+sizeof(oam_var_cont_t);
        remain_number-=21;
    }

    if(remain_number<=21){
        ptr_data->hdr.branch = 0xD7;
        ptr_data->hdr.leaf = htons(0x0104);
        ptr_data->hdr.width = remain_number*6;
        memcpy(ptr_data->mac,ptr_mac,remain_number*6);
        pdu_len+=sizeof(oam_var_cont_t)+remain_number*6;

        tlv_end.branch = 0xD7;
        tlv_end.leaf = htons(0x0104);
        tlv_end.width = 0x80;

        memcpy((oam_uint8 *)ptr_data+sizeof(oam_var_cont_t)+remain_number*6,&tlv_end,sizeof(oam_var_cont_t));
        remain_number = 0;
        pdu_len+=sizeof(oam_var_cont_t);
    }
    return pdu_len;
}

oam_uint32 dpoe_oam_autoneg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_autoneg_t *data = (oam_dpoe_autoneg_t *)out;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    dpoe_oam_autonet_get_adapt(mgmt_obj.obj.uni.value, &data->max_cap, &data->curr_cap);
    data->max_cap = htons(data->max_cap);
    data->curr_cap = htons(data->curr_cap);
    data->hdr.width = 4;
    pdu_len = sizeof(oam_dpoe_autoneg_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_autoneg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_autoneg_t *in = (oam_dpoe_autoneg_t *)recv;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    in->max_cap = ntohs(in->max_cap);
    in->curr_cap = ntohs(in->curr_cap);
    dpoe_oam_autonet_set_adapt(mgmt_obj.obj.uni.value, in->curr_cap);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_srcaddr_admctrl_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_uint8 enable;
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_uni_adm_ctrl_get_adapt(mgmt_obj.obj.uni.value, &enable);

    data->hdr.width = 1;
    data->value = enable;
    pdu_len = sizeof(oam_dpoe_uint8_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_srcaddr_admctrl_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{

    ca_device_config_tlv_t scfg_tlv;
    oam_uint8 olt_type[CFG_ID_PON_OLT_TYPE_LEN];
    oam_uint8 enable;
    ca_status_t ret = CA_E_OK;

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    memset(olt_type,0,CFG_ID_PON_OLT_TYPE_LEN);

    scfg_tlv.type = CA_CFG_ID_PON_OLT_TYPE;
    scfg_tlv.len =  CFG_ID_PON_OLT_TYPE_LEN;

    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if(CA_E_OK != ret)
    {
        OAM_LOG_DEBUG("read CA_CFG_ID_PON_OLT_TYPE. ret = %d \r\n", ret);
        return OAM_E_ERROR;
    }

    memcpy(olt_type,scfg_tlv.value,CFG_ID_PON_OLT_TYPE_LEN);

    ret = strcmp(olt_type,"ARRIS");
    if(CA_E_OK == ret){
        oam_dpoe_uint16_t *in = (oam_dpoe_uint16_t *)recv;
        enable = (htons(in->value))&0xFF;
    }else{
        oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
        enable = in->value;
    }
    oam_var_cont_t *data = (oam_var_cont_t *)out;


    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_uni_adm_ctrl_set_adapt(mgmt_obj.obj.uni.value, enable);
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_uni_minmac_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint16 nmb;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_uni_minmac_get_adapt(mgmt_obj.obj.uni.value, &nmb);
    data->hdr.width = 2;
    data->value = htons(nmb);
    pdu_len = sizeof(oam_dpoe_uint16_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_uni_minmac_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint16_t *in = (oam_dpoe_uint16_t *)recv;
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->width = OAM_DPOE_RSP_OK;
    //dpoe_oam_uni_minmac_set_adapt(mgmt_obj.obj.uni.value, ntohs(in->value));
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_uni_maxmac_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint16 nmb = 0;
    oam_uint16 a=0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_uni_maxmac_get_adapt(mgmt_obj.obj.uni.value, &nmb);
    a = nmb;
    data->hdr.width = 2;
    data->value = htons(a);
    pdu_len = sizeof(oam_dpoe_uint16_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_uni_maxmac_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint16_t *in16 = (oam_dpoe_uint16_t *)recv;
    oam_dpoe_uint8_t *in8 = (oam_dpoe_uint8_t *)recv;
    oam_uint16 value;
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->width = OAM_DPOE_RSP_OK;
    if (in8->hdr.width == 1)
        value = in8->value;
    else if (in8->hdr.width == 2)
        value = ntohs(in16->value);
    dpoe_oam_uni_maxmac_set_adapt(mgmt_obj.obj.uni.value, value);
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_onu_maxmac_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint16 nmb = 0;;

    dpoe_oam_onu_maxmac_get_adapt(&nmb);
    data->hdr.width = 2;
    data->value = htons(nmb);
    pdu_len = sizeof(oam_dpoe_uint16_t);
    return pdu_len;
}


oam_uint32 dpoe_oam_onu_maxmac_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint16_t *in = (oam_dpoe_uint16_t *)recv;

    data->width = OAM_DPOE_RSP_OK;
    dpoe_oam_onu_maxmac_set_adapt(ntohs(in->value));
    return sizeof(oam_var_cont_t);
}

oam_uint8 g_length_err_disc = 0x01;
oam_uint32 dpoe_oam_length_err_disc_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;
    oam_uint16 nmb;

    data->hdr.width = 1;
    data->value = g_length_err_disc;
    pdu_len = sizeof(oam_dpoe_uint8_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_length_err_disc_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;

    data->width = OAM_DPOE_RSP_OK;
    g_length_err_disc = in->value;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_unkn_flooding_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;
    oam_uint8 enable;

    dpoe_oam_unkn_flooding_get_adapt(&enable);
    data->hdr.width = 1;
    data->value = enable;
    pdu_len = sizeof(oam_dpoe_uint8_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_unkn_flooding_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;

    dpoe_oam_unkn_flooding_set_adapt(in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_local_switch_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;
    oam_uint8 enable;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_local_switch_get_adapt(mgmt_obj.obj.uni.value,&enable);
    data->hdr.width = 1;
    data->value = enable;
    pdu_len = sizeof(oam_dpoe_uint8_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_local_switch_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
#if 0
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    dpoe_oam_local_switch_set_adapt(mgmt_obj.obj.uni.value,in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

//This feature has been Obsoleted in spec version 180228
oam_uint32 dpoe_oam_llid_queue_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_llid_queue_int_t llid_queue;
    //oam_var_cont_t *in = (oam_var_cont_t *)recv;
    oam_uint8 *ptr = (oam_uint8 *)((oam_uint8 *)out + sizeof(oam_var_cont_t));
    int i, j;
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    memset(&llid_queue, 0, sizeof(oam_dpoe_llid_queue_int_t));
    dpoe_oam_llid_queue_get_adapt(&llid_queue);

    *ptr++ = llid_queue.n_llid;
    data->width = 1;
    for(i=0;i<llid_queue.n_llid;i++){
        *ptr++ = llid_queue.llid[i].n_queue;
        data->width++;
        for(j=0;j<llid_queue.llid[i].n_queue;j++){
            *ptr++ = llid_queue.llid[i].q[j];
            data->width++;
        }
    }

    *ptr++ = llid_queue.n_port;
    data->width++;
    for(i=0;i<llid_queue.n_port;i++){
        *ptr++ = llid_queue.port[i].n_queue;
        data->width++;
        for (j=0; j < llid_queue.port[i].n_queue; j++)
        {
            *ptr++ = llid_queue.port[i].q[j];
            data->width++;
        }
    }
    pdu_len = data->width + sizeof(oam_var_cont_t);
    return pdu_len;
}

//This feature has been Obsoleted in spec version 180228
//2019-02-22 restore it for ARRIS OLT within old DPOE
oam_uint32 dpoe_oam_llid_queue_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_llid_queue_int_t llid_queue;
    oam_uint8 *ptr = (oam_uint8 *)((oam_uint8 *)recv + sizeof(oam_var_cont_t));
    int i, j;

    memset(&llid_queue, 0, sizeof(oam_dpoe_llid_queue_int_t));
    llid_queue.n_llid = *ptr++;
    if (llid_queue.n_llid > 8)
    {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    for (i = 0; i < llid_queue.n_llid; i++)
    {
        llid_queue.llid[i].n_queue = *ptr++;
        if (llid_queue.llid[i].n_queue > 8)
        {
            data->width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
        for (j = 0; j < llid_queue.llid[i].n_queue; j++)
        {
            llid_queue.llid[i].q[j] = *ptr++;
        }
    }
    llid_queue.n_port = *ptr++;
    if (llid_queue.n_port > 4)
    {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    for (i = 0; i < llid_queue.n_port; i++)
    {
        llid_queue.port[i].n_queue = *ptr++;
        for (j = 0; j < llid_queue.port[i].n_queue; j++)
        {
            llid_queue.port[i].q[j] = *ptr++;
        }
    }

    dpoe_oam_llid_queue_set_adapt(&llid_queue);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_fw_name_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 *ptr = (oam_uint8 *)(out + sizeof(oam_var_cont_t));
    char fw_name[ENV_INFO_LEN+1]={0};
    ca_img_info_t img_info;
    oam_uint8 i = 0;

    memset(&img_info,0,sizeof(ca_img_info_t));

    ca_img_info_get(0,0,&img_info);
    memcpy(fw_name,img_info.info, ENV_INFO_LEN);

    for(i=0;i<ENV_INFO_LEN+1;i++){
        if(0x2A == fw_name[i])
            break;
    }

    memcpy(ptr,img_info.info, i);
    data->width = i;
    pdu_len = data->width + sizeof(oam_var_cont_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_mac_full_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;
    oam_uint8 beh;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_mac_full_get_adapt(mgmt_obj.obj.uni.value,&beh);
    data->hdr.width = 1;
    data->value = beh;
    pdu_len = sizeof(oam_dpoe_uint8_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_mac_full_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_mac_full_set_adapt(mgmt_obj.obj.uni.value,in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_clr_dyn_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
#if 0
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI || mgmt_obj.obj.onu.hdr.leaf != OAM_DPOE_LEAF_OBJ_ONU) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI && mgmt_obj.obj.uni.value > 3) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        dpoe_oam_clr_dyn_mac_adapt(mgmt_obj.obj.uni.value);
    }
    else
    {
        dpoe_oam_clr_dyn_mac_adapt(0);
        dpoe_oam_clr_dyn_mac_adapt(1);
        dpoe_oam_clr_dyn_mac_adapt(2);
        dpoe_oam_clr_dyn_mac_adapt(3);
    }
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_add_dyn_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
    oam_uint8 number;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    number = in->hdr.width/6;

    dpoe_oam_add_dyn_mac_adapt(mgmt_obj.obj.uni.value, number, &in->value);
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_del_dyn_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
    oam_uint8 number;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    number = in->hdr.width/6;

    dpoe_oam_del_dyn_mac_adapt(mgmt_obj.obj.uni.value, number, &in->value);
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_clr_sta_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
#if 0
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI || mgmt_obj.obj.onu.hdr.leaf != OAM_DPOE_LEAF_OBJ_ONU) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI && mgmt_obj.obj.uni.value > 3) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        dpoe_oam_clr_sta_mac_adapt(mgmt_obj.obj.uni.value);
    }
    else
    {
        dpoe_oam_clr_sta_mac_adapt(0);
        dpoe_oam_clr_sta_mac_adapt(1);
        dpoe_oam_clr_sta_mac_adapt(2);
        dpoe_oam_clr_sta_mac_adapt(3);
    }
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_add_sta_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_onu_mac_t *in = (oam_dpoe_onu_mac_t *)recv;
    oam_uint8 number = 0;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    number = in->hdr.width/6;

    dpoe_oam_add_sta_mac_adapt(mgmt_obj.obj.uni.value, number, in->mac);
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_del_sta_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_onu_mac_t *in = (oam_dpoe_onu_mac_t *)recv;
    oam_uint8 number =0;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.value > 3) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    number = in->hdr.width/6;

    dpoe_oam_del_sta_mac_adapt(mgmt_obj.obj.uni.value, number, in->mac);
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

static ca_uint32_t g_is_first_mc_llid_cls_index = 0;

static void __dpoe_add_cls_to_discard_bc_llid(){
    ca_classifier_key_t      key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t   action;
    ca_uint32_t              index =0;
    ca_port_id_t ca_port;
    ca_status_t ret = CA_E_OK;
    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));
    ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
    
    action.forward = CA_CLASSIFIER_FORWARD_DENY;
    key.src_port = ca_port;
    key_mask.src_port = 1;
    key.handle_type =  CA_KEY_HANDLE_TYPE_LLID_COS;
    key_mask.key_handle = 1;
    key.key_handle.flow_id = 63;
    ret = ca_classifier_rule_add(0,0,&key,&key_mask,&action,&index);
    if(ret!= CA_E_OK){
        OAM_ORG_LOG("ca_classifier_rule_add failed, ret=%d",ret);
        return;
    }
    g_is_first_mc_llid_cls_index = index;
    OAM_ORG_LOG("cls rule index to discard bc llid = %d\n\r ",g_is_first_mc_llid_cls_index);
    return;
}


static void __dpoe_add_cls_to_discard_mc_llid(ca_llid_t llid_index){
    ca_classifier_key_t      key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t   action;
    ca_uint32_t              index =0;
    ca_port_id_t ca_port;
    ca_status_t ret = CA_E_OK;
    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));
    ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
    
    action.forward = CA_CLASSIFIER_FORWARD_DENY;
    key.src_port = ca_port;
    key_mask.src_port = 1;
    key.handle_type =  CA_KEY_HANDLE_TYPE_LLID_COS;
    key_mask.key_handle = 1;
    key.key_handle.flow_id = llid_index;
    ret = ca_classifier_rule_add(0,0,&key,&key_mask,&action,&index);
    if(ret!= CA_E_OK){
        OAM_ORG_LOG("ca_classifier_rule_add failed, ret=%d",ret);
        return;
    }
    OAM_ORG_LOG("cls rule index to discard mc llid = %d\n\r ",index);
    return;

}

static void __dpoe_delete_discard_mc_llid_cls(ca_llid_t llid_index){
    ca_classifier_key_t      key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t   action;
    ca_uint32 i=0;
    ca_uint32_t              priority = 0;
    ca_status_t ret = CA_E_OK;
    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    OAM_ORG_LOG("try to delete the rule to discard mc llid \n\r");
    for(i=1;i<17;i++){
        ret = ca_classifier_rule_get(0,i,&priority,&key,&key_mask,&action);
        if(ret != CA_E_OK){
            continue;
        }else{
            if(key.handle_type == CA_KEY_HANDLE_TYPE_LLID_COS
                && llid_index == key.key_handle.flow_id){
                OAM_ORG_LOG("Found the rule to discard mc llid ,cls_index = %d\n\r",i);
                ret = ca_classifier_rule_delete(0,i);
                if(ret != CA_E_OK){
                    OAM_ORG_LOG("ca_classifier_rule_delete failed\n\r");
                }
                break;
            }
        }        
    }
    return;
}

oam_uint32 dpoe_oam_cfg_mc_llid(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_mc_llid_act_t *mc_llid = (oam_dpoe_mc_llid_act_t *)recv;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    ca_uint32_t mc_llid_nums = 64;
    ca_mpcp_llid_t tmp_llid[64];
    oam_uint8 i = 0;
    oam_uint16 llid = 0;
    ca_status_t ret = CA_E_OK;
    ca_l2_mcast_config_t dpoe_l2_mcast_config;
    ca_llid_t llid_index = 0;
    oam_llid_t llid_value = 0;
    oam_if_t *oam_intf = NULL;

    memset(tmp_llid,0,64*sizeof(ca_mpcp_llid_t));
    memset(&dpoe_l2_mcast_config,0,sizeof(ca_l2_mcast_config_t));

    llid = ntohs(mc_llid->llid);

    OAM_ORG_LOG("configure multicast LLID opr = %d, llid = 0x%x\r\n", mc_llid->action, llid);
    if (mc_llid->action == DPOE_MC_LLID_ADD)
    {
        oam_intf = oam_intf_find(llid);
        if(oam_intf != NULL){
            OAM_ORG_LOG("multicast LLID is the same as unicast llid ,so drop it\n\r");
            data->width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);

        }
        ret= ca_siepon_mc_llid_list(0, &mc_llid_nums,tmp_llid);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("DPOE_MC_LLID_ADD:ca_siepon_mc_llid_list failed ,ret = %d\n\r",ret);
            data->width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }

        if(mc_llid_nums>32){
            OAM_ORG_LOG("MAX multicast LLID only support 32 in HW ,so drop it \n\r");
            data->width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }

        if(0 == mc_llid_nums){
            __dpoe_add_cls_to_discard_bc_llid();
        }        
        ret= ca_siepon_mc_llid_add(0,llid,&llid_index);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("DPOE_MC_LLID_ADD:ca_siepon_mc_llid_add failed ,ret = %d\n\r",ret);
            data->width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
        _dpoe_add_mc_llid(llid,llid_index);
        if((llid!=0xFFFF)&& (llid!=0xFFFE) 
            && (llid!=0x7FFF) && (llid!=0x7FFE)){
            __dpoe_add_cls_to_discard_mc_llid(llid_index);
        }
        ret = ca_l2_mcast_config_get(0,&dpoe_l2_mcast_config);
        if(ret!= CA_E_OK)
        {
           OAM_ORG_LOG("DPOE_MC_LLID_ADD:ca_l2_mcast_config_set() failed ,ret = \n\r",ret);
           data->width = OAM_DPOE_RSP_BAD_PARAM;
           return sizeof(oam_var_cont_t);
        }
        dpoe_l2_mcast_config.mode = CA_MC_MODE_LLID;
        dpoe_l2_mcast_config.unknown_multicast_flooding_enable = 0;
        ret = ca_l2_mcast_config_set(0,&dpoe_l2_mcast_config);
        if(ret!= CA_E_OK)
        {
           OAM_ORG_LOG("DPOE_MC_LLID_ADD:ca_l2_mcast_config_set() failed ,ret = \n\r",ret);
           data->width = OAM_DPOE_RSP_BAD_PARAM;
           return sizeof(oam_var_cont_t);
        }

    }
    else if (mc_llid->action == DPOE_MC_LLID_DEL)
    {
        ret = ca_siepon_mc_llid_delete(0,llid);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("DPOE_MC_LLID_DEL:ca_siepon_mc_llid_delete failed ,ret = %d\n\r",ret);
            data->width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }

        ret= ca_siepon_mc_llid_list(0, &mc_llid_nums,tmp_llid);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("DPOE_MC_LLID_DEL:ca_siepon_mc_llid_list failed ,ret = %d\n\r",ret);
            data->width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
        if((llid!=0xFFFF)&& (llid!=0xFFFE) 
            && (llid!=0x7FFF) && (llid!=0x7FFE)){
            _dpoe_find_mc_llid_index(llid,&llid_index);
            __dpoe_delete_discard_mc_llid_cls(llid_index);        
        }

        if(0 == mc_llid_nums){
            if(g_is_first_mc_llid_cls_index!=0){
                ret = ca_classifier_rule_delete(0,g_is_first_mc_llid_cls_index);
                if(ret!=CA_E_OK){
                    OAM_ORG_LOG("DPOE_MC_LLID_DEL:ca_classifier_rule_delete 2 failed\n\r");
                }
            }
            memset(&dpoe_l2_mcast_config,0,sizeof(ca_l2_mcast_config_t));
            ret = ca_l2_mcast_config_get(0,&dpoe_l2_mcast_config);
            if(ret!= CA_E_OK)
            {
               OAM_ORG_LOG(" DPOE_MC_LLID_DEL:ca_l2_mcast_config_get() failed ,ret = \n\r",ret);
               data->width = OAM_DPOE_RSP_BAD_PARAM;
               return sizeof(oam_var_cont_t);
            }
            dpoe_l2_mcast_config.mode = CA_MC_MODE_IP;
            dpoe_l2_mcast_config.unknown_multicast_flooding_enable = 1;
            ret = ca_l2_mcast_config_set(0,&dpoe_l2_mcast_config);
            if(ret!= CA_E_OK)
            {
               OAM_ORG_LOG(" DPOE_MC_LLID_DEL:ca_l2_mcast_config_set() failed ,ret = \n\r",ret);
               data->width = OAM_DPOE_RSP_BAD_PARAM;
               return sizeof(oam_var_cont_t);
            }
        }
        _dpoe_remove_mc_llid(llid);
    }
    else if (mc_llid->action == DPOE_MC_LLID_DELALL)
    {
        ret= ca_siepon_mc_llid_list(0, &mc_llid_nums,tmp_llid);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("DPOE_MC_LLID_DELALL:ca_siepon_mc_llid_list failed ,ret = %d\n\r",ret);
            data->width =  OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
        //OAM_ORG_LOG("dpoe_oam_cfg_mc_llid:   mc_llid_nums = %d\n\r",mc_llid_nums);
        for(i=0;i<mc_llid_nums;i++){
            //OAM_ORG_LOG("dpoe_oam_cfg_mc_llid: tmp_llid[%d] = %d\n\r",i, tmp_llid[i]);
            ret = ca_siepon_mc_llid_delete(0,tmp_llid[i]);
            if(ret!= CA_E_OK){
                OAM_ORG_LOG("DPOE_MC_LLID_DELALL:delete mc_llid=%d failed ,skip it\n\r",tmp_llid[i]);
                continue;
            }
            if((tmp_llid[i]!=0xFFFF)&& (tmp_llid[i]!=0xFFFE) 
                && (tmp_llid[i]!=0x7FFF) && (tmp_llid[i]!=0x7FFE)){
                llid_index = 0;
                _dpoe_find_mc_llid_index(tmp_llid[i],&llid_index);
                __dpoe_delete_discard_mc_llid_cls(llid_index); 
            }
        }
        mc_llid_nums = 0xFF;
        ret= ca_siepon_mc_llid_list(0, &mc_llid_nums,tmp_llid);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("DPOE_MC_LLID_DELALL:ca_siepon_mc_llid_list2 failed ,ret = %d\n\r",ret);
            data->width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
        if(0 == mc_llid_nums){
            memset(&dpoe_l2_mcast_config,0,sizeof(ca_l2_mcast_config_t));
            ret = ca_l2_mcast_config_get(0,&dpoe_l2_mcast_config);
            if(ret!= CA_E_OK)
            {
               OAM_ORG_LOG(" DPOE_MC_LLID_DELALL:ca_l2_mcast_config_get() failed ,ret = \n\r",ret);
               data->width = OAM_DPOE_RSP_BAD_PARAM;
               return sizeof(oam_var_cont_t);
            }
            dpoe_l2_mcast_config.mode = CA_MC_MODE_IP;
            dpoe_l2_mcast_config.unknown_multicast_flooding_enable = 1;
            ret = ca_l2_mcast_config_set(0,&dpoe_l2_mcast_config);
            if(ret!= CA_E_OK)
            {
               OAM_ORG_LOG(" DPOE_MC_LLID_DELALL:ca_l2_mcast_config_set() failed ,ret = \n\r",ret);
               data->width = OAM_DPOE_RSP_BAD_PARAM;
               return sizeof(oam_var_cont_t);
            }
             _dpoe_del_all_mc_llid();
        }
        if(g_is_first_mc_llid_cls_index!=0){
            ret = ca_classifier_rule_delete(0,g_is_first_mc_llid_cls_index);
            if(ret!=CA_E_OK){
                OAM_ORG_LOG("DPOE_MC_LLID_DELALL:ca_classifier_rule_delete 2 failed\n\r");
            }
            g_is_first_mc_llid_cls_index= 0;
        }

    }

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_port_cfg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_port_cfg_t *port_cfg = (oam_dpoe_uint8_t *)out;

    port_cfg->hdr.width = 2;
    port_cfg->llid_count = g_llid_count;
    port_cfg->uni_count = g_uni_count;
    pdu_len = sizeof(oam_dpoe_port_cfg_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_port_cfg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_port_cfg_t *port_cfg = (oam_dpoe_port_cfg_t *)recv;
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    OAM_ORG_LOG("port configure LLID = %d, UNI = %d\r\n", port_cfg->llid_count, port_cfg->uni_count);
    g_llid_count = port_cfg->llid_count;
    g_uni_count = port_cfg->uni_count;
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_queue_cfg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_queue_cfg_t *queue_cfg = (oam_dpoe_uint8_t *)out;
    oam_uint8 type  = 0;
    oam_uint16 port = 0;

    type = mgmt_obj.obj.uni.hdr.leaf;

    if (type == OAM_DPOE_LEAF_OBJ_LLID){
        port = mgmt_obj.obj.llid.value;
    }else if(type == OAM_DPOE_LEAF_OBJ_UNI){
        port = mgmt_obj.obj.uni.value;
    }

    dpoe_oam_queue_cfg_get_adapt(mgmt_obj.obj.uni.hdr.leaf,port,&queue_cfg->queue_count,queue_cfg->size);

    if(type == OAM_DPOE_LEAF_OBJ_LLID){
        queue_cfg->queue_count = 0x1;
    }

    queue_cfg->hdr.width = 1;

    pdu_len = sizeof(oam_dpoe_queue_cfg_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_queue_cfg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_queue_cfg_t *queue_cfg = (oam_dpoe_port_cfg_t *)recv;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 type;
    oam_uint16 port;

    type = mgmt_obj.obj.uni.hdr.leaf;
    if (type == OAM_DPOE_LEAF_OBJ_UNI)
        port = mgmt_obj.obj.uni.value;
    else if (type == OAM_DPOE_LEAF_OBJ_LLID)
        port = mgmt_obj.obj.llid.value;
    //OAM_ORG_LOG("queue configure LLID = %d, UNI = %d\r\n", queue_cfg->llid_count, port_cfg->uni_count);
    //g_llid_count = port_cfg->llid_count;
    //g_uni_count = port_cfg->uni_count;
    dpoe_oam_queue_cfg_set_adapt(mgmt_obj.obj.uni.hdr.leaf, port, queue_cfg->queue_count,queue_cfg->size);
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


/*
    This function according to [1904.1A], subclause 14.4.3.2.15.
*/
oam_uint32 dpoe_oam_multicast_llid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_uint8 num = 0;
    oam_uint16 mc_llid[64];
    oam_dpoe_onu_mc_llid_tlv_t *onu_mc_llid = (oam_dpoe_onu_mc_llid_tlv_t *)out;
    memset(mc_llid,0,64*sizeof(oam_uint16));
    
    dpoe_oam_multicast_llid_get_adapt(&num,mc_llid);
    onu_mc_llid->hdr.width = 2*num;
    memcpy(onu_mc_llid->mc_llid,mc_llid,2*num);
    pdu_len = sizeof(oam_var_cont_t)+2*num;
    return pdu_len;
}


oam_uint32 dpoe_oam_enable_emta_edva_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;


    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_uni_mac_learn_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uni_mac_learned_set_tlv_t *in = (oam_dpoe_uni_mac_learned_set_tlv_t *)recv;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_uni_mac_learn_set_adapt(mgmt_obj.obj.uni.value,in->mac_address);
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_uni_mac_learn_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint16 mac_number = 0;
    oam_uint32 pdu_len = 0;
    oam_dpoe_uni_mac_learned_tlv_t *data = (oam_dpoe_uni_mac_learned_tlv_t *) out;
    oam_dpoe_uni_mac_learned_tlv_t *in = (oam_dpoe_uni_mac_learned_tlv_t *)recv;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI)||(mgmt_obj.obj.onu.hdr.leaf != OAM_DPOE_LEAF_OBJ_ONU)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    dpoe_oam_uni_mac_learn_get_adapt(in->uni_port,&mac_number,in->mac_address);
    if(0 == mac_number){
        data->uni_port = 0xFF;
    }else{
        data->uni_port = in->uni_port;
    }
    memcpy(data->mac_address,in->mac_address,6*sizeof(oam_uint8));
    data->hdr.width = sizeof(oam_dpoe_uni_mac_learned_tlv_t) -sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uni_mac_learned_tlv_t);
    return pdu_len;
}


oam_uint32 dpoe_oam_esafe_cfg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *) out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value = 0;
    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.onu.hdr.leaf != OAM_DPOE_LEAF_OBJ_ONU) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_esafe_cfg_get_adapt(&value);
    data->value = value;

    data->hdr.width = sizeof(oam_dpoe_uint8_t) -sizeof(oam_var_cont_t) ;
    pdu_len = sizeof(oam_dpoe_uint8_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_esafe_cfg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;


    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.onu.hdr.leaf != OAM_DPOE_LEAF_OBJ_ONU) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }


    if(in->value>=4 && in->value<= 255){
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    dpoe_oam_esafe_cfg_set_adapt(in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_auto_neg_admin_ctrl_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
    oam_uint8 port = 0;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    port = mgmt_obj.obj.uni.value;
    dpoe_oam_auto_neg_admin_ctrl_set_adapt(port,in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

#endif

