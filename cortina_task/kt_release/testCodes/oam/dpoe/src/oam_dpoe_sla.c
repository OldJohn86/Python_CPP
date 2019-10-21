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

oam_uint8 us_fec = 0, ds_fec = 0;

oam_uint32 dpoe_oam_bcast_limit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_dpoe_uint32_t *data = (oam_dpoe_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    /* the maximum number of broadcast packets allowed from Ethernet port 1 in 1 second */
    port = mgmt_obj.obj.uni.value;
    ret = dpoe_oam_bcast_limit_get_adapt(port, &data->value);
    OAM_ORG_LOG("ret = %d, port = 0x%x, value = %d\r\n", ret, port, data->value);

    if(ret != OAM_E_OK){
        data->hdr.width = OAM_DPOE_RSP_HW_FAILURE;
        return sizeof(oam_var_cont_t);
    }

    data->value = htonl(data->value);
    data->hdr.width = sizeof(oam_dpoe_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint32_t);
    return pdu_len;
}

oam_uint32 dpoe_oam_bcast_limit_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint32_t *in = (oam_dpoe_uint32_t *)recv;
    oam_uint16 port;
    
    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    /* the maximum number of broadcast packets allowed from Ethernet port 1 in 1 second */
    in->value = ntohl(in->value);
    port = mgmt_obj.obj.uni.value;
    ret = dpoe_oam_bcast_limit_set_adapt(port, in->value);
    OAM_ORG_LOG("ret = %d, port = 0x%x, value = %d\r\n", ret, port, in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_queue_cir_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_dpoe_queue_cir_t *data = (oam_dpoe_queue_cir_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    port = mgmt_obj.obj.queue.instance;
    ret = dpoe_oam_queue_cir_get_adapt(port, mgmt_obj.obj.queue.number, &data->cbs, &data->cir);

    OAM_ORG_LOG("ret = %d, port = 0x%x, queue = %d, cbs = %d, cir = %d\r\n",
        ret, port, mgmt_obj.obj.queue.number, data->cbs, data->cir);

    data->cbs = htons(data->cbs);
    data->cir = htonl(data->cir);
    data->hdr.width = sizeof(oam_dpoe_queue_cir_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_queue_cir_t);
    return pdu_len;
}

oam_uint32 dpoe_oam_queue_cir_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_queue_cir_t *in = (oam_dpoe_queue_cir_t *)recv;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    in->cbs = ntohs(in->cbs);
    in->cir = ntohl(in->cir);

    port = mgmt_obj.obj.queue.instance;
    ret = dpoe_oam_queue_cir_set_adapt(port, mgmt_obj.obj.queue.number, in->cbs, in->cir);

    OAM_ORG_LOG("ret = %d, port = 0x%x, queue = %d, cbs = %d, cir = %d\r\n",
        ret, port, mgmt_obj.obj.queue.number, in->cbs, in->cir);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);    
}

oam_uint32 dpoe_oam_fec_mode_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_dpoe_onu_fec_t *data = (oam_dpoe_onu_fec_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 port;
    oam_uint32 ds, us;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
#if 0
    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->hdr.width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);
    }
    else if (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) {
        port = OAM_PON_PORT_ID;
        ret = dpoe_oam_fec_get_adapt(port, &ds, &us);
        
    }else{
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    port = OAM_PON_PORT_ID;
    ret = dpoe_oam_fec_get_adapt(port, &ds, &us);
    data->us = us;
    data->ds = ds;
    OAM_ORG_LOG("ret = %d, port = 0x%x, ds = %d, us = %d\r\n", ret, port, data->ds, data->us);

    data->hdr.width = sizeof(oam_dpoe_onu_fec_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_onu_fec_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_fec_mode_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_onu_fec_t *in = (oam_dpoe_onu_fec_t *)recv;
    oam_uint16 port;
    oam_uint32 ds, us;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
    
#if 0
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);
    }
    else if (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) {
        ds = in->ds;
        us = in->us;
        port = OAM_PON_PORT_ID;
        ret = dpoe_oam_fec_set_adapt(port, ds, us);
        
    }else{
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    ds = in->ds;
    us = in->us;
    port = OAM_PON_PORT_ID;
    ret = dpoe_oam_fec_set_adapt(port, ds, us);
    OAM_ORG_LOG("ret = %d, port = 0x%x, ds = %d, us = %d\r\n", ret, port, in->ds, in->us);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_queue_eir_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_dpoe_queue_eir_t *data = (oam_dpoe_queue_eir_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if(mgmt_obj.obj.queue.type == OAM_DPOE_QUEUE_TYPE_LLID){
        data->hdr.width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);
        
    }else if(mgmt_obj.obj.queue.type == OAM_DPOE_QUEUE_TYPE_UNI){
        port = mgmt_obj.obj.queue.instance;
        ret = dpoe_oam_queue_cir_get_adapt(port, mgmt_obj.obj.queue.number, &data->ebs, &data->eir);

    }else{
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_LOG("ret = %d, port = 0x%x, queue = %d, ebs = %d, eir = %d\r\n",
        ret, port, mgmt_obj.obj.queue.number, data->ebs, data->eir);

    data->ebs = htons(data->ebs);
    data->eir = htonl(data->eir);
    data->hdr.width = sizeof(oam_dpoe_queue_eir_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_queue_eir_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_queue_eir_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_queue_eir_t *in = (oam_dpoe_queue_eir_t *)recv;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    in->ebs = ntohs(in->ebs);
    in->eir = ntohl(in->eir);

    if(mgmt_obj.obj.queue.type == OAM_DPOE_QUEUE_TYPE_LLID){
        data->width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);

    }else if(mgmt_obj.obj.queue.type == OAM_DPOE_QUEUE_TYPE_UNI){
        port = mgmt_obj.obj.queue.instance;
        ret = dpoe_oam_queue_cir_set_adapt(port, mgmt_obj.obj.queue.number, in->ebs, in->eir);

    }else{
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_LOG("ret = %d, port = 0x%x, queue = %d, ebs = %d, eir = %d\r\n",
        ret, port, mgmt_obj.obj.queue.number, in->ebs, in->eir);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_queue_mark_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_dpoe_queue_mark_t *data = (oam_dpoe_queue_mark_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if(mgmt_obj.obj.queue.type == OAM_DPOE_QUEUE_TYPE_LLID){
        data->hdr.width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);

    }else if(mgmt_obj.obj.queue.type == OAM_DPOE_QUEUE_TYPE_UNI){
        port = mgmt_obj.obj.queue.instance;
        ret = dpoe_oam_queue_mark_get_adapt(port, mgmt_obj.obj.queue.number, &data->value);

    }else{
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_LOG("ret = %d, port = 0x%x, queue = %d, value = %d\r\n",
        ret, port, mgmt_obj.obj.queue.number, data->value);

    data->hdr.width = sizeof(oam_dpoe_queue_mark_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_queue_mark_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_queue_mark_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_queue_mark_t *in = (oam_dpoe_queue_mark_t *)recv;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if(mgmt_obj.obj.queue.type == OAM_DPOE_QUEUE_TYPE_LLID){

        data->width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);

    }else if(mgmt_obj.obj.queue.type == OAM_DPOE_QUEUE_TYPE_UNI){
        port = mgmt_obj.obj.queue.instance;
        ret = dpoe_oam_queue_mark_set_adapt(port, mgmt_obj.obj.queue.number, in->value);

    }else{
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_LOG("ret = %d, port = 0x%x, queue = %d, value = %d\r\n",
        ret, port, mgmt_obj.obj.queue.number, in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_queue_limiter_cap_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_dpoe_queue_limiter_cap_t *data = (oam_dpoe_queue_limiter_cap_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.onu.hdr.leaf != OAM_DPOE_LEAF_OBJ_ONU) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_queue_limiter_cap_get_adapt(0, 0, &data->value);

    data->value.num_limiter = htons(data->value.num_limiter);
    data->value.cbs_increment = htons(data->value.cbs_increment);
    data->value.cir_increment = htons(data->value.cir_increment);
    data->value.ebs_increment = htons(data->value.ebs_increment);
    data->value.eir_increment = htons(data->value.eir_increment);
    data->hdr.width = sizeof(oam_dpoe_queue_limiter_cap_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_queue_limiter_cap_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_queue_limiter_cap_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_coupling_flag_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    port = mgmt_obj.obj.queue.instance;
    ret = dpoe_oam_coupling_flag_get_adapt(port, mgmt_obj.obj.queue.number, &data->value);

    OAM_ORG_LOG("ret = %d, port = 0x%x, queue = %d, value = %d\r\n",
        ret, port, mgmt_obj.obj.queue.number, data->value);

    data->hdr.width = sizeof(oam_dpoe_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint8_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_coupling_flag_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    port = mgmt_obj.obj.queue.instance;
    ret = dpoe_oam_coupling_flag_set_adapt(port, mgmt_obj.obj.queue.number, in->value);

    OAM_ORG_LOG("ret = %d, port = 0x%x, queue = %d, value = %d\r\n",
        ret, port, mgmt_obj.obj.queue.number, in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_media_type_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 media_type = 0xFF;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_media_type_get_adapt(&media_type);
    data->value = media_type;

    data->hdr.width = sizeof(oam_dpoe_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint8_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_enable_traffic_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_var_cont_t *in = (oam_var_cont_t *)recv;
    oam_uint8 llid;
    
    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
    ca_printf("oam traffic enable\r\n");
#if 0
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.onu.hdr.leaf != OAM_DPOE_LEAF_OBJ_ONU) &&
        (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    
    if (mgmt_obj.obj.onu.hdr.leaf == OAM_DPOE_LEAF_OBJ_ONU)
    {
        ret = dpoe_oam_onu_traffic_set_adapt(TRUE);
    }
    else
    {
        llid = mgmt_obj.obj.llid.value;
        ret = dpoe_oam_llid_traffic_set_adapt(llid, TRUE);
    }

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_disable_traffic_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_var_cont_t *in = (oam_var_cont_t *)recv;
    oam_uint16 port;
    oam_uint8 llid;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    ca_printf("oam traffic disable\r\n");
#if 0  
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.onu.hdr.leaf != OAM_DPOE_LEAF_OBJ_ONU) &&
        (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif    
    if (mgmt_obj.obj.onu.hdr.leaf == OAM_DPOE_LEAF_OBJ_ONU)
    {
        ret = dpoe_oam_onu_traffic_set_adapt(FALSE);
    }
    else
    {
        llid = mgmt_obj.obj.llid.value;
        ret = dpoe_oam_llid_traffic_set_adapt(llid, FALSE);
    }

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_loopback_enable_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
    oam_uint16 port ,llid;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
        (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        llid = mgmt_obj.obj.llid.value;
        if(0x02 == in->value){
            ret=dpoe_oam_llid_loopabck_set_adapt(llid,TRUE);
        }       
    }else{
        port = mgmt_obj.obj.uni.value;
        ret = dpoe_oam_loopback_set_adapt(port, TRUE, in->value);
    }

    OAM_ORG_LOG("ret = %d, port = 0x%x, enable, value = %d\r\n", ret, port, in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_loopback_disable_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
        (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        data->width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);

    }else{
        port = mgmt_obj.obj.uni.value;
        //ret = dpoe_oam_loopback_set_adapt(port, FALSE, in->value);

    }

    ret = dpoe_oam_loopback_set_adapt(port, FALSE, in->value);
        OAM_ORG_LOG("ret = %d, port = 0x%x, disable, value = %d\r\n", ret, port, in->value);


    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_laser_tx_off_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_dpoe_uint16_t *in = (oam_dpoe_uint16_t *)recv;
    oam_uint16 port;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    in->value = ntohs(in->value);
    ret = dpoe_oam_laser_tx_off_set_adapt(in->value);
    OAM_ORG_LOG("dpoe_oam_laser_tx_off_set:ret = %d, value = %d\r\n", ret, in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}
#endif

