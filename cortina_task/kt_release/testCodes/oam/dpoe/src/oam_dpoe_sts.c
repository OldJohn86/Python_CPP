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
#include "queue.h"

oam_uint32 dpoe_oam_queue_delay_thr_get(
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

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->value = dpoe_oam_queue_delay_thr_get_adapt(mgmt_obj.obj.queue.number);
    data->hdr.width = sizeof(oam_uint8);
    pdu_len = sizeof(oam_dpoe_uint8_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_queue_delay_thr_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_queue_delay_thr_set_adapt(mgmt_obj.obj.queue.number, in->value);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_queue_delay_val_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->value = dpoe_oam_queue_delay_val_get_adapt(mgmt_obj.obj.queue.number);
    data->value = htonll(data->value);
    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_queue_delay_val_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_queue_delay_val_reset_adapt(mgmt_obj.obj.queue.number);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_frm_dropped_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->value = dpoe_oam_frm_dropped_get_adapt(mgmt_obj.obj.queue.number);
    data->value = htonll(data->value);
    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_frm_dropped_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_frm_dropped_reset_adapt(mgmt_obj.obj.queue.number);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_oct_dropped_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->value = dpoe_oam_oct_dropped_get_adapt(mgmt_obj.obj.queue.number);
    data->value = htonll(data->value);
    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_oct_dropped_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_oct_dropped_reset_adapt(mgmt_obj.obj.queue.number);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_oct_delayed_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    data->value = dpoe_oam_oct_delayed_get_adapt(mgmt_obj.obj.queue.number);
    data->value = htonll(data->value);
    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;
}


oam_uint32 dpoe_oam_oct_delayed_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_oct_delayed_reset_adapt(mgmt_obj.obj.queue.number);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_usoct_unused_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->value = dpoe_oam_usoct_unused_get_adapt(mgmt_obj.obj.queue.number);
    data->value = htonll(data->value);
    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_usoct_unused_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_usoct_unused_reset_adapt(mgmt_obj.obj.queue.number);

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_opt_mon_temp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint32 pdu_len = 0;

    data->value = dpoe_oam_opt_mon_temp_get_adapt();
    data->value = htons(data->value);
    data->hdr.width = sizeof(oam_uint16);
    pdu_len = sizeof(oam_dpoe_uint16_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_opt_mon_temp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    dpoe_oam_opt_mon_temp_reset_adapt();

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_opt_mon_vcc_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint32 pdu_len = 0;

    data->value = dpoe_oam_opt_mon_vcc_get_adapt();
    data->value = htons(data->value);
    data->hdr.width = sizeof(oam_uint16);
    pdu_len = sizeof(oam_dpoe_uint16_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_opt_mon_vcc_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    dpoe_oam_opt_mon_vcc_reset_adapt();

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_opt_mon_bias_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint32 pdu_len = 0;

    data->value = dpoe_oam_opt_mon_bias_get_adapt();
    data->value = htons(data->value);
    data->hdr.width = sizeof(oam_uint16);
    pdu_len = sizeof(oam_dpoe_uint16_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_opt_mon_bias_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    dpoe_oam_opt_mon_bias_reset_adapt();

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_opt_mon_txpwr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint32 pdu_len = 0;

    data->value = dpoe_oam_opt_mon_txpwr_get_adapt();
    data->value = htons(data->value);
    data->hdr.width = sizeof(oam_uint16);
    pdu_len = sizeof(oam_dpoe_uint16_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_opt_mon_txpwr_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    dpoe_oam_opt_mon_txpwr_reset_adapt();

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_opt_mon_rxpwr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint32 pdu_len = 0;

    data->value = dpoe_oam_opt_mon_rxpwr_get_adapt();
    data->value = htons(data->value);
    data->hdr.width = sizeof(oam_uint16);
    pdu_len = sizeof(oam_dpoe_uint16_t);
    
    return pdu_len;
}


oam_uint32 dpoe_oam_opt_mon_rxpwr_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    dpoe_oam_opt_mon_rxpwr_reset_adapt();

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_rx_frm_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
        (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
        (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    data->value = dpoe_oam_rx_frm_y_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_rx_frm_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    dpoe_oam_rx_frm_y_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_tx_frm_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    data->value = dpoe_oam_tx_frm_y_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_tx_frm_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    dpoe_oam_tx_frm_y_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_tx_oct_g_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    data->value = dpoe_oam_tx_oct_g_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_tx_oct_g_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    dpoe_oam_tx_oct_g_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_rx_oct_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    data->value = dpoe_oam_rx_oct_y_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_rx_oct_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    dpoe_oam_rx_oct_y_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_rx_oct_g_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    data->value = dpoe_oam_rx_oct_g_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_rx_oct_g_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    dpoe_oam_rx_oct_g_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_tx_oct_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    data->value = dpoe_oam_tx_oct_y_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_tx_oct_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        value = mgmt_obj.obj.llid.value;
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        value = mgmt_obj.obj.queue.number;
    }

    dpoe_oam_tx_oct_y_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_cnt_nmb_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;
    oam_uint32 pdu_len = 0;

    data->value = 64;
    data->hdr.width = sizeof(oam_uint8);
    pdu_len = sizeof(oam_dpoe_uint8_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_rx_frm_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    data->value = dpoe_oam_rx_frm_l2cp_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_rx_frm_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    dpoe_oam_rx_frm_l2cp_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_rx_oct_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    data->value = dpoe_oam_rx_oct_l2cp_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_rx_oct_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }
    dpoe_oam_rx_oct_l2cp_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_tx_frm_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    data->value = dpoe_oam_tx_frm_l2cp_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_tx_frm_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    dpoe_oam_tx_frm_l2cp_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_tx_oct_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    data->value = dpoe_oam_tx_oct_l2cp_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_tx_oct_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    dpoe_oam_tx_oct_l2cp_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_disc_frm_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    data->value = dpoe_oam_disc_frm_l2cp_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_disc_frm_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    dpoe_oam_disc_frm_l2cp_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}


oam_uint32 dpoe_oam_disc_oct_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    data->value = dpoe_oam_disc_oct_l2cp_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_disc_oct_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    dpoe_oam_disc_oct_l2cp_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_tx_err_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    data->value = dpoe_oam_tx_err_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_tx_err_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    dpoe_oam_tx_err_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_rx_err_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    data->value = dpoe_oam_rx_err_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;

}

oam_uint32 dpoe_oam_rx_err_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 value;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON)) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
    }

    dpoe_oam_rx_err_reset_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);

}


oam_uint32 dpoe_oam_alarm_threshold_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_alarm_threshold_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_eth_stats_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret;
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 obj_id;
    dpoe_oam_eth_stats_t stats;
    oam_var_desc_t *var_desc;
    oam_uint8 counter_branch;
    oam_uint16 counter_leaf;
    oam_uint64 value = 0;

    OAM_ORG_LOG("dpoe_oam_eth_stats_get \r\n");

#if 0    
    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
        (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
        (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
        (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif

    memset(&stats, 0, sizeof(dpoe_oam_eth_stats_t));
    
    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        obj_id = mgmt_obj.obj.uni.value;
        ret = dpoe_oam_uni_eth_stats_get_adapt(obj_id, &stats);
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        obj_id = mgmt_obj.obj.pon.value;
        ret = dpoe_oam_pon_eth_stats_get_adapt(&stats);
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        obj_id = mgmt_obj.obj.llid.value;
        ret = dpoe_oam_llid_eth_stats_get_adapt(obj_id, &stats);
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        obj_id = mgmt_obj.obj.queue.number;
        /* not support yet */
        //ret = dpoe_oam_queue_eth_stats_get_adapt(mgmt_obj.obj.queue.instance,obj_id,&queue_stats);
    }

    var_desc = (oam_var_desc_t*)recv;
    counter_branch  = var_desc->branch;
    counter_leaf = ntohs(var_desc->leaf);
    if(counter_branch == OAM_DPOE_ATTRIB_BRANCH)
    {
        switch(counter_leaf){
        case OAM_DPOE_LEAF_RX_FRM_GREEN:
            value = stats.rx_frames_green;
            break;
        case OAM_DPOE_LEAF_TX_FRM_GREEN:
            value = stats.tx_frames_green;
            break;

        case OAM_DPOE_LEAF_RX_UC:
            value = stats.rx_uc_frames;
            break;
        case OAM_DPOE_LEAF_RX_MC:
            value = stats.rx_mc_frames;
            break;
        case OAM_DPOE_LEAF_RX_BC:
            value = stats.rx_bc_frames;
            break;

        case OAM_DPOE_LEAF_TX_UC:
            value = stats.tx_uc_frames;
            break;
        case OAM_DPOE_LEAF_TX_MC:
            value = stats.tx_mc_frames;
            break;
        case OAM_DPOE_LEAF_TX_BC:
            value = stats.tx_bc_frames;
            break;
            
        case OAM_DPOE_LEAF_RX_FRM_SHORT:
            value = stats.rx_frames_short;
            break;
        case OAM_DPOE_LEAF_RX_FRM_64:
            value = stats.rx_frames_64;
            break;
        case OAM_DPOE_LEAF_RX_FRM_127:
            value = stats.rx_frames_65_127;
            break;
        case OAM_DPOE_LEAF_RX_FRM_255:
            value = stats.rx_frames_128_255;
            break;
        case OAM_DPOE_LEAF_RX_FRM_511:
            value = stats.rx_frames_256_511;
            break;
        case OAM_DPOE_LEAF_RX_FRM_1023:
            value = stats.rx_frames_512_1023;
            break;
        case OAM_DPOE_LEAF_RX_FRM_1518:
            value = stats.rx_frames_1024_1518;
            break;
        case OAM_DPOE_LEAF_RX_FRM_1519:
            value = stats.rx_frames_1519;
            break;

        case OAM_DPOE_LEAF_TX_FRM_64:
            value = stats.tx_frames_64;
            break;
        case OAM_DPOE_LEAF_TX_FRM_127:
            value = stats.tx_frames_65_127;
            break;
        case OAM_DPOE_LEAF_TX_FRM_255:
            value = stats.tx_frames_128_255;
            break;
        case OAM_DPOE_LEAF_TX_FRM_511:
            value = stats.tx_frames_256_511;
            break;
        case OAM_DPOE_LEAF_TX_FRM_1023:
            value = stats.tx_frames_512_1023;
            break;
        case OAM_DPOE_LEAF_TX_FRM_1518:
            value = stats.tx_frames_1024_1518;
            break;
        case OAM_DPOE_LEAF_TX_FRM_1519:
            value = stats.tx_frames_1519;
            break;
        case OAM_DPOE_LEAF_RX_ERR: 
            value = stats.rx_error;
            break;
        case OAM_DPOE_LEAF_TX_ERR:
            value = stats.tx_error;
            break;
        case OAM_DPOE_LEAF_OVERLIMIT_DROP:
            value = stats.overlimit_drop;
            break;
        case OAM_DPOE_LEAF_FRAME_DROPPED:   /* not support currently */
            //value = queue_stats.tail_dropped;
            break;
        case OAM_DPOE_LEAF_OCT_DROPPED:   /* not support currently */
            //value = queue_stats.tail_dropped_bytes;
            break;
        case OAM_DPOE_LEAF_USOCT_UNUSED:   /* not support currently */
            value = 0;
            break;
        default:
            break;
        }
    }
    else if(counter_branch == OAM_INTF_COUNTERS_MIB_BRANCH)
    {
        switch(counter_leaf){
        case OAM_DPOE_LEAF_TRANSOK:
            value = stats.tx_frames_ok;
            break;
        case OAM_DPOE_LEAF_FRMRECV:
            value = stats.rx_frames_ok;
            break;
        case OAM_DPOE_LEAF_FCSERR:
            value = stats.rx_fcs_error;
            break;
        case OAM_DPOE_LEAF_OCTTRANSOK:
            value = stats.tx_bytes_ok;
            break;
        case OAM_DPOE_LEAF_OCTRECVOK:
            value = stats.rx_bytes_ok;
            break;
        case OAM_DPOE_LEAF_MCXMIT:
            value = stats.tx_mc_frames;
            break;
        case OAM_DPOE_LEAF_BCXMIT:
            value = stats.tx_bc_frames;
            break;
        case OAM_DPOE_LEAF_MCRECV:
            value = stats.rx_mc_frames;
            break;
        case OAM_DPOE_LEAF_BCRECV:
            value = stats.rx_bc_frames;
            break;
        case OAM_DPOE_LEAF_OUTLENERR: /* not support currently */
            value = 0;
            break; 
        case OAM_DPOE_LEAF_GIANTERR:
            value = stats.rx_frames_oversize;
            break;
        
        case OAM_DPOE_LEAF_PAUSERECV:
            value = stats.rx_pause_frames;
            break;
        case OAM_DPOE_LEAF_PAUSEXMIT:
            value = stats.tx_pause_frames;
            break;    
        default:
            break;
        }
    }
    data->value = value;
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);
    
    return pdu_len;
}

oam_uint32 dpoe_oam_eth_stats_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
     oam_uint8 obj_id;
    dpoe_oam_eth_stats_t stats;

    OAM_ORG_LOG("dpoe_oam_eth_stats_reset \r\n");
    
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#if 0
        if ((mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) && 
            (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) &&
            (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) &&
            (mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE)) {
            data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
#endif


    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        obj_id = mgmt_obj.obj.uni.value;
        dpoe_oam_uni_eth_stats_reset_adapt(obj_id);
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        obj_id = mgmt_obj.obj.pon.value;
        dpoe_oam_pon_eth_stats_reset_adapt();
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        obj_id = mgmt_obj.obj.llid.value;
        dpoe_oam_llid_eth_stats_reset_adapt(obj_id);
    }
    else if (mgmt_obj.obj.queue.hdr.leaf == OAM_DPOE_LEAF_OBJ_QUEUE)
    {
        obj_id = mgmt_obj.obj.queue.number;
    }

    data->width = OAM_DPOE_RSP_OK;
    
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_clear_counters_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{

    oam_dpoe_uint8_t *in = (oam_dpoe_uint8_t *)recv;
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.onu.hdr.leaf != OAM_DPOE_LEAF_OBJ_ONU) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    oam_port_stats_reset();
    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);

}

oam_uint32 dpoe_oam_pause_mac_ctl_frame_rev_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint32 pdu_len = 0;
    oam_uint16 port = 0;
    oam_uint64 rev_frame = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    
    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    port = mgmt_obj.obj.uni.value;
    dpoe_oam_pause_mac_ctl_frame_rev_get_adapt(port,&rev_frame);

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_rx_frame_green_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    dpoe_oam_rx_frame_green_get_adapt(&rev_frame);

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_out_of_range_length_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }
    dpoe_oam_out_of_range_length_get_adapt(type,&rev_frame,port); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_tx_frame_1519_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_tx_frame_1519_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_rx_frame_1519_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_rx_frame_1519_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_count_tx_frame_1518_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_tx_frame_1518_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_rx_frame_1518_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_rx_frame_1518_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_tx_frame_1023_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_tx_frame_1023_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_rx_frame_1023_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_rx_frame_1023_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_tx_frame_511_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_tx_frame_511_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}
    
oam_uint32 dpoe_oam_count_rx_frame_511_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_rx_frame_511_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_tx_frame_255_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_tx_frame_255_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_rx_frame_255_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_rx_frame_255_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_tx_frame_127_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_tx_frame_127_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}
oam_uint32 dpoe_oam_count_rx_frame_127_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_rx_frame_127_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_tx_frame_64_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_tx_frame_64_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_rx_frame_64_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_rx_frame_64_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_count_rx_frame_short_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *) out;
    oam_uint64 rev_frame = 0;
    oam_uint32 pdu_len = 0;
    oam_uint8 type=0,port=0;;
    oam_uint16 llid = 0;

    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
       type = OAM_DPOE_LEAF_OBJ_UNI;
       port = mgmt_obj.obj.uni.value;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
       type = OAM_DPOE_LEAF_OBJ_PON;
    }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID){
        type = OAM_DPOE_LEAF_OBJ_LLID;
        llid = mgmt_obj.obj.llid.value;
    }
    dpoe_oam_count_rx_frame_short_get_adapt(type,&rev_frame,port,llid); 

    data->value = htonll(rev_frame);
    data->hdr.width = sizeof(oam_dpoe_uint64_t)-sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;
}

#endif

