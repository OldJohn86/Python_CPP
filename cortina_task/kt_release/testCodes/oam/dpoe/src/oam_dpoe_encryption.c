#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_DPOE_OAM


#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_dpoe.h"
#include "oam_dpoe_pdu.h"
#include "oam_dpoe_client.h"
#include "oam_dpoe_adapt.h"
#include "oam_mux_par.h"
#include "oam_client.h"
#include "oam_timer.h"
#include "oam_dpoe_key_exchange.h"
#include "epon.h"


extern oam_oui_t  oam_oui_dpoe;

oam_uint32 dpoe_oam_key_exch_timer_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 time = 0;
#if 0
    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
	mgmt_obj.obj.llid.value = 0;
    ret = oam_dpoe_key_exchange_expir_timer_get(mgmt_obj.obj.llid.value, &time);
    OAM_ORG_DEBUG("ret = %d, llid = 0x%x, time = %d\r\n", ret, mgmt_obj.obj.llid.value, time);

    if(time<10){
        time = 10;
        OAM_ORG_LOG("dpoe_oam_key_exch_timer_get:the timer min is 10 , so chang it\n\r");
    }

    if(ret == OAM_E_OK){
        data->value = htons(time);
    }else{
        data->value = 0;
    }
    data->hdr.width = sizeof(oam_uint16);
    pdu_len = sizeof(oam_dpoe_uint16_t);
    return pdu_len;
}

oam_uint32 dpoe_oam_key_exch_timer_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint16_t *in = (oam_dpoe_uint16_t *)recv;

#if 0
    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
	mgmt_obj.obj.llid.value = 0;
	in->value = ntohs(in->value);
    if((in->value)<10 && (in->value)>0){
        in->value = 10;
        OAM_ORG_LOG("dpoe_oam_key_exch_timer_set: the timer min is 10 , so chang it\n\r");
    }
    ret = oam_dpoe_key_exchange_expir_timer_set(mgmt_obj.obj.llid.value, in->value);
    OAM_ORG_DEBUG("ret = %d, llid = 0x%x, value = %d\r\n", ret, mgmt_obj.obj.llid.value, in->value);

	if(ret != OAM_E_OK){
        data->width = OAM_DPOE_RSP_HW_FAILURE;
    }else{
        data->width = OAM_DPOE_RSP_OK;
    }

    ca_timer_add(100,dpoe_key_exch_timer_expire, mgmt_obj.obj.llid.value);
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_encrypt_mode_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
	oam_status ret = OAM_E_OK;
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    ca_oam_key_exchange_mode_t mode;
#if 0
    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
	mgmt_obj.obj.llid.value = 0;
    ret = oam_dpoe_key_exhange_mode_get(&mode);
    OAM_ORG_DEBUG("ret = %d, llid = 0x%x, mode = %d\r\n", ret, mgmt_obj.obj.llid.value, mode);

    if(ret == OAM_E_OK){
        data->value = mode;
        data->hdr.width = sizeof(oam_uint8);
        pdu_len = sizeof(oam_dpoe_uint8_t);
        return pdu_len;
    }

    data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
    return sizeof(oam_var_cont_t);
}

ca_oam_key_exchange_mode_t g_encrypt_mode = CA_OAM_KEY_EXCHANGE_MODE_NONE ;

oam_uint32 dpoe_oam_encrypt_mode_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
#if 0
    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
	mgmt_obj.obj.llid.value = 0;
    ret = oam_dpoe_key_exhange_mode_set(in->value);
    OAM_ORG_DEBUG("ret = %d, llid = 0x%x, value = %d\r\n", ret, mgmt_obj.obj.llid.value, in->value);

	if(ret != OAM_E_OK){
        data->width = OAM_DPOE_RSP_HW_FAILURE;
    }else{
        data->width = OAM_DPOE_RSP_OK;
    }

    g_encrypt_mode = in->value;
    if( (CA_OAM_KEY_EXCHANGE_MODE_1G_DOWN_ONLY == g_encrypt_mode) ||
        (CA_OAM_KEY_EXCHANGE_MODE_10G_DOWN_ONLY == g_encrypt_mode)){
        system("echo 1 > /etc/dpoe_encrypt_mode.txt");
    }else if(CA_OAM_KEY_EXCHANGE_MODE_10G_BI  == g_encrypt_mode){
        system("echo 2 > /etc/dpoe_encrypt_mode.txt");
    }else{
        system("echo 0 > /etc/dpoe_encrypt_mode.txt");
    }
    OAM_ORG_DEBUG("dpoe_oam_encrypt_mode_set: in->value = %d ,g_encrypt_mode = %d\n\r",in->value,g_encrypt_mode );
#if 0
    ca_timer_add(300,oam_dpoe_key_exhange_mode_set_callback,(void *)&g_encrypt_mode);

    data->width = OAM_DPOE_RSP_OK;
#endif

    return sizeof(oam_var_cont_t);
}

void dpoe_oam_handle_key_exchange(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_status ret = OAM_E_OK;
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dpoe_t   *pRecvPkt = NULL;
    oam_uint8 key_op = 0, raise = 0;
    oam_dpoe_onu_key_assign_t *key_assign = NULL;
    oam_llid_t assign_llid;
    oam_dpoe_onu_key_ack_t *key_ack = NULL;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length >=
                   sizeof(oam_pdu_dpoe_t) + sizeof(oam_var_desc_t) - 1, 0);

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    pRecvPkt = (oam_pdu_dpoe_t *)frame;

    key_op = *(pRecvPkt->data);

    oam_llid_find(llid, &assign_llid);

    //if(key_op == 0){ /* received key assign from OLT, for multi-LLID encryption */
        key_assign = (oam_dpoe_onu_key_assign_t *)(pRecvPkt->data);

        //assign_llid = ntohs(key_assign->llid);
        OAM_ORG_DEBUG("received key assign OAMPDU, assign llid = 0x%x, number = 0x%x, length = 0x%x, key = 0x%x\r\n",
            assign_llid, key_assign->number, key_assign->length, key_assign->key[0]);

        ret = oam_dpoe_key_from_olt_set(assign_llid, key_assign->number, key_assign->length, key_assign->key);
        if(ret != OAM_E_OK){
            oam_key_exchange_failure_alarm(llid, raise);
        }
        //TODO: send ACK by OAM or send ACK by encryption APP?
    //}else{


    //}

}

void oam_key_exchange_request(oam_uint16 llid, oam_uint8 number, oam_uint8 *key)
{
	oam_llid_t llid_value;
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_dpoe_onu_key_assign_t *key_assign;
    oam_int32   outPktLen = 0;
    oam_pdu_dpoe_t   *pOutPkt = (oam_pdu_dpoe_t *)pdu;

    OAM_ORG_DEBUG("llid = 0x%x, number = 0x%x, key = 0x%x\r\n", llid, number, *key);

    if(oam_llid_find(llid, &llid_value) == -1){
        OAM_ORG_DEBUG("not found llid, llid_index %d, llid_value %d\r\n", llid, llid_value);
        return;
    }

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    outPktLen = dpoe_oam_build_header(oam_oui_dpoe, llid_value, pdu, OAM_PDU_DPOE_KEY_EXCHANGE);

    key_assign = (oam_dpoe_onu_key_assign_t *)(pOutPkt->data);
    // assignment
    //key_assign->key_op = 0;
    //key_assign->llid = htons(llid_value);
    key_assign->number = number;
    memcpy(key_assign->key, key, 16);
    key_assign->length = 16;

    outPktLen += sizeof(oam_dpoe_onu_key_assign_t);

    dpoe_oam_send(llid_value, pOutPkt, outPktLen);
}

void oam_key_exchange_ack(oam_uint16 llid, oam_uint8 number)
{
	oam_llid_t llid_value;
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_dpoe_onu_key_ack_t *key_ack;
    oam_int32   outPktLen = 0;
    oam_pdu_dpoe_t   *pOutPkt = (oam_pdu_dpoe_t *)pdu;

    OAM_ORG_DEBUG("llid = 0x%x, number = 0x%x\r\n", llid, number);

    if(oam_llid_find(llid, &llid_value) == -1){
        OAM_ORG_DEBUG("not found llid, llid_index %d, llid_value %d\r\n", llid, llid_value);
        return;
    }
    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    outPktLen = dpoe_oam_build_header(oam_oui_dpoe, llid, pdu,  OAM_PDU_DPOE_KEY_EXCHANGE);

    key_ack = (oam_dpoe_onu_key_ack_t *)(pOutPkt->data);
    // ack
    //key_ack->key_op = 1;
    //key_ack->llid = htons(llid);
    key_ack->number = number;

    outPktLen += sizeof(oam_dpoe_onu_key_ack_t);

    dpoe_oam_send(llid_value, pOutPkt, outPktLen);
}

void oam_key_exchange_failure_alarm(oam_uint16 llid, oam_uint8 raise)
{
	oam_llid_t llid_value;
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_int32   outPktLen = 0;
    oam_pdu_dpoe_t   *pOutPkt = (oam_pdu_dpoe_t *)pdu;

    OAM_ORG_DEBUG("llid = 0x%x, raise = 0x%x\r\n", llid, raise);

    if(oam_llid_find(llid, &llid_value) == -1){
        OAM_ORG_DEBUG("not found llid, llid_index %d, llid_value %d\r\n", llid, llid_value);
        return;
    }

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    outPktLen = dpoe_oam_build_header(oam_oui_dpoe, llid, pdu, OAM_PDU_DPOE_KEY_EXCHANGE);

    //TODO send alarm
    dpoe_oam_send(llid_value, pOutPkt, outPktLen);
}



#if 0
oam_uint32 dpoe_oam_key_exch_timer_set(
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

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->width = OAM_DPOE_RSP_OK;
    g_key_exch_timer[mgmt_obj.obj.llid.value].time = ntohs(in->value);
    if (g_key_exch_timer[mgmt_obj.obj.llid.value].time != 0)
        dpoe_encrypt_enable(mgmt_obj.obj.llid.value, TRUE);
    else dpoe_encrypt_enable(mgmt_obj.obj.llid.value, FALSE);
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_key_exch_timer_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint32 pdu_len = 0;
    int i;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    for (i=0;i<8;i++)
    {
        if (g_key_exch_timer[i].valid)
        {
            data->value = htons(g_key_exch_timer[mgmt_obj.obj.llid.value].time);
            data->hdr.width = sizeof(oam_uint16);
            pdu_len = sizeof(oam_dpoe_uint16_t);
            return pdu_len;
        }
    }
    data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_encrypt_mode_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;
    oam_uint32 pdu_len = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->value = g_key_exch_timer[mgmt_obj.obj.llid.value].mode;

    data->hdr.width = sizeof(oam_uint8);
    pdu_len = sizeof(oam_dpoe_uint8_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_encrypt_mode_set(
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

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->width = OAM_DPOE_RSP_OK;
    g_key_exch_timer[mgmt_obj.obj.llid.value].mode = in->value;
    //ca_epon_port_speed_get(CA_IN ca_device_id_t device_id,CA_IN ca_port_id_t port_id,CA_OUT ca_epon_port_speed_t * speed)
    return sizeof(oam_var_cont_t);
}

#endif

#endif

