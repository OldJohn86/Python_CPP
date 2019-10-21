#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "oam_cmn.h"
#include "oam_core.h"
#include "oam_supp.h"
#include "oam_adapt.h"
#include "oam_std_pdu.h"
#include "oam_control.h"
#include "oam_oob_mgmt.h"
#include "oam_dpoe.h"
#include "oam_client.h"
#include "oam_dpoe_adapt.h"
#include "scfg.h"

oam_uint32 std_oam_macid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_macid_get\r\n");

    data->value = 0;

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;
}

oam_uint32 std_oam_transok_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_transok_get\r\n");

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

    data->value = dpoe_oam_transok_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);
    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;
}

oam_uint32 std_oam_scoll_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_scoll_get\r\n");
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

    data->value = dpoe_oam_scoll_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);
    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;
}

oam_uint32 std_oam_mcoll_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_mcoll_get\r\n");

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

    data->value = dpoe_oam_mcoll_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;
}

oam_uint32 std_oam_frm_recv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_frm_recv_get\r\n");

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

    data->value = dpoe_oam_frm_recv_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;
}

oam_uint32 std_oam_fcserr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_fcserr_get\r\n");

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

    data->value = dpoe_oam_fcserr_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;
}

oam_uint32 std_oam_alignerr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_alignerr_get\r\n");

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

    data->value = dpoe_oam_alignerr_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;
}

oam_uint32 std_oam_octtransok_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_octtransok_get\r\n");

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

    data->value = dpoe_oam_octtranok_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;
}

oam_uint32 std_oam_frmdeff_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_frmdeff_get\r\n");

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

    data->value = dpoe_oam_frmdef_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);
    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;

}

oam_uint32 std_oam_latecoll_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_latecoll_get\r\n");

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

    data->value = dpoe_oam_latecoll_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;

}

oam_uint32 std_oam_frmabort_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_frmabort_get\r\n");

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

    data->value = dpoe_oam_frmabort_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_frmlost1_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_frmlost1_get\r\n");

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

    data->value = dpoe_oam_frmlost1_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_octok_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_octok_get\r\n");

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

    data->value = dpoe_oam_octok_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_frmlost2_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_frmlost2_get\r\n");

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

    data->value = dpoe_oam_frmlost2_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_mcxmit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_mcxmit_get\r\n");

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

    data->value = dpoe_oam_mcxmit_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_bcxmit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_bcxmit_get\r\n");

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

    data->value = dpoe_oam_bcxmit_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_frmexc_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_frmexc_get\r\n");

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

    data->value = dpoe_oam_frmexc_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);


    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_mcrecv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_mcrecv_get\r\n");

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

    data->value = dpoe_oam_mcrecv_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_bcrecv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_bcrecv_get\r\n");

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

    data->value = dpoe_oam_bcrecv_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_inlenerr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_inlenerr_get\r\n");

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

    data->value = dpoe_oam_inlenerr_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;

}

oam_uint32 std_oam_outlenerr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_outlenerr_get\r\n");

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

    data->value = dpoe_oam_outlenerr_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_giantlenerr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_giantlenerr_get\r\n");

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

    data->value = dpoe_oam_giantlenerr_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_macstat_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_macstat_get\r\n");

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

    data->value = dpoe_oam_macstat_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}


oam_uint32 std_oam_macstat_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)recv;
    oam_var_cont_t *out_data = (oam_var_cont_t *)out;
    oam_uint8 value;

    OAM_ORG_LOG("\r\n");
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

    dpoe_oam_macstat_set_adapt(mgmt_obj.obj.uni.hdr.leaf, value, data->value);

    out_data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 std_oam_macaddr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_onu_mac_t *data = (oam_dpoe_onu_mac_t *)out;
    oam_uint32 pdu_len = 0;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_macaddr_get\r\n");

    dpoe_oam_mac_addr_get_adapt(data->mac);

    data->hdr.width = sizeof(oam_dpoe_onu_mac_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_onu_mac_t);

    return pdu_len;
}

oam_uint32 std_oam_macaddr_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_onu_mac_t *data = (oam_dpoe_onu_mac_t *)recv;
    oam_var_cont_t *out_data = (oam_var_cont_t *)out;

    OAM_ORG_LOG("std_oam_macaddr_set\r\n");

    dpoe_oam_mac_addr_set_adapt(data->mac);
    out_data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


oam_uint32 std_oam_collfrm_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_collfrm_get\r\n");

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

    data->value = dpoe_oam_collfrm_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_phytype_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;
    oam_uint8 phy_type;
    oam_uint8 obj_type;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_phytype_get\r\n");

    
    if (mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI)
    {
        value = mgmt_obj.obj.uni.value;
        obj_type = OAM_DPOE_LEAF_OBJ_UNI;
    }
    else if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        value = mgmt_obj.obj.pon.value;
        obj_type = OAM_DPOE_LEAF_OBJ_PON;
    }

    dpoe_std_oam_phytype_get_adapt(obj_type,value,&phy_type);
    data->value = phy_type;

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_symerr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("std_oam_symerr_get\r\n");

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

    data->value = dpoe_oam_symerr_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;

}

oam_uint32 std_oam_phyadmin_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
    OAM_ORG_LOG("std_oam_phyadmin_get\r\n");

    data->value = 1;
    data->value = htonl(data->value);
    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;
}

oam_uint32 std_oam_medailavail_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

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

    data->value = dpoe_oam_mdavail_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_autonegid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    data->value = 0;

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_autonegadmin_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    value = mgmt_obj.obj.uni.value;

    data->value = dpoe_oam_autonegadmin_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_autonegsig_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    value = mgmt_obj.obj.uni.value;

    data->value = dpoe_oam_autonegsig_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_autonegcfg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    value = mgmt_obj.obj.uni.value;

    data->value = dpoe_oam_autonegcfg_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_autonegcfg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)recv;
    oam_var_cont_t *out_data = (oam_var_cont_t *)out;
    oam_uint8 value;

    OAM_ORG_LOG("\r\n");
    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    value = mgmt_obj.obj.uni.value;

    dpoe_oam_autonegcfg_set_adapt(mgmt_obj.obj.uni.hdr.leaf, value, data->value);

    out_data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 std_oam_autoneglctab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    data->value = dpoe_oam_autoneglctab_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_autonegadtvab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    data->value = dpoe_oam_autonegadvtab_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_autonegadtvab_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)recv;
    oam_var_cont_t *out_data = (oam_var_cont_t *)out;
    oam_uint8 value;

    OAM_ORG_LOG("\r\n");
    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    dpoe_oam_autonegadvtab_set_adapt(mgmt_obj.obj.uni.hdr.leaf, value, data->value);

    out_data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 std_oam_autonegrecvtab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    data->value = dpoe_oam_autonegrcvtab_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_autoneglcsab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    data->value = dpoe_oam_autoneglcsab_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_autonegadvsab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    data->value = dpoe_oam_autonegadvsab_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_autonegadvsab_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)recv;
    oam_var_cont_t *out_data = (oam_var_cont_t *)out;
    oam_uint8 value;

    OAM_ORG_LOG("\r\n");
    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    dpoe_oam_autonegadvsab_set_adapt(mgmt_obj.obj.uni.hdr.leaf, value, data->value);

    out_data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 std_oam_autonegrecvsab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    data->value = dpoe_oam_autonegrecvsab_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_duplex_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    data->value = dpoe_oam_duplex_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_duplex_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)recv;
    oam_pdu_uint8_t *out_data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        out_data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    dpoe_oam_duplex_set_adapt(mgmt_obj.obj.uni.hdr.leaf, value, data->value);

    out_data->hdr.width = OAM_DPOE_RSP_OK;
    pdu_len = sizeof(oam_var_cont_t);

    return pdu_len;

}

oam_uint32 std_oam_macctrl_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    value = mgmt_obj.obj.uni.value;
     
    data->value = dpoe_std_oam_macctrl_get_adapt(value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_macxmit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

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

    data->value = dpoe_oam_macxmit_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_macrecv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

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

    data->value = dpoe_oam_macrecv_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_notspop_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

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
    data->value = dpoe_oam_notspop_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_pauseallow_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");
    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->value = dpoe_oam_pauseallow_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_pauseallow_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)recv;
    oam_pdu_uint8_t *out_data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        out_data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    dpoe_oam_pauseallow_set_adapt(mgmt_obj.obj.uni.hdr.leaf, value, data->value);

    out_data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_pausexmit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    data->value = dpoe_oam_pausexmit_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;

}

oam_uint32 std_oam_pauserecv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.uni.hdr.leaf != OAM_DPOE_LEAF_OBJ_UNI) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.uni.value;
    data->value = dpoe_oam_pauserecv_get_adapt(mgmt_obj.obj.uni.hdr.leaf, value);
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;

}

oam_uint32 std_oam_undef_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint64_t *data = (oam_dpoe_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    
    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");
    
    data->value = 0;
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_uint64);
    pdu_len = sizeof(oam_dpoe_uint64_t);

    return pdu_len;

}

oam_uint32 std_oam_mpcpxmit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.pon.value;
    data->value = dpoe_oam_mpcpxmit_get_adapt(mgmt_obj.obj.pon.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_mpcprecv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.pon.value;
    data->value = dpoe_oam_mpcprecv_get_adapt(mgmt_obj.obj.pon.hdr.leaf, value);
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_dscwinsent_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    data->value = 0;

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_dsctout_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    data->value = 0;

    data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 std_oam_feccb_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;
    oam_uint32 FECCorrectedBlocks = 0;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

#if 0
    if (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif    
    value = mgmt_obj.obj.pon.value;
    dpoe_oam_feccb_get_adapt(mgmt_obj.obj.pon.hdr.leaf, &FECCorrectedBlocks);
    data->value = FECCorrectedBlocks;
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;

}

oam_uint32 std_oam_fecucb_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;
    oam_uint32 FECUncorrectableBlocks = 0;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");
#if 0
    if (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif    
    value = mgmt_obj.obj.pon.value;
    dpoe_oam_fecucb_get_adapt(mgmt_obj.obj.pon.hdr.leaf, &FECUncorrectableBlocks);
    data->value = FECUncorrectableBlocks;
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;

}

oam_uint32 std_oam_fecab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    data->value = 2;
    data->value = htonl(data->value);

    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_fecmode_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint32_t *data = (oam_pdu_uint32_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint32 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.pon.value;
    data->value = dpoe_oam_fecmode_get_adapt(mgmt_obj.obj.pon.hdr.leaf, value);
    data->value = htonl(data->value);
    
    data->hdr.width = sizeof(oam_pdu_uint32_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint32_t);

    return pdu_len;

}

oam_uint32 std_oam_fecmode_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint8_t *data = (oam_pdu_uint8_t *)recv;
    oam_pdu_uint8_t *out_data = (oam_pdu_uint8_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.pon.hdr.leaf != OAM_DPOE_LEAF_OBJ_PON) {
        out_data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
    value = mgmt_obj.obj.pon.value;
    dpoe_oam_fecmode_set_adapt(mgmt_obj.obj.pon.hdr.leaf, value, data->value);

    out_data->hdr.width = sizeof(oam_pdu_uint8_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint8_t);

    return pdu_len;

}

oam_uint32 dpoe_unknown1_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    data->value = 0;

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;
}


oam_uint32 std_oam_mpcp_stats_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_status ret;
    oam_pdu_uint64_t *data = (oam_pdu_uint64_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 obj_id;
    dpoe_oam_mpcp_stats_t stats;
    oam_var_desc_t *var_desc;
    oam_uint8 counter_branch;
    oam_uint16 counter_leaf;
    oam_uint64 value;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    OAM_ORG_LOG("\r\n");

    if (mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON)
    {
        obj_id = mgmt_obj.obj.pon.value;
        ret = dpoe_oam_pon_mpcp_stats_get_adapt(&stats);
    }
    else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_LLID)
    {
        obj_id = mgmt_obj.obj.llid.value;
        ret = dpoe_oam_llid_mpcp_stats_get_adapt(obj_id, &stats);
    }
    else 
    {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    var_desc = (oam_var_desc_t*)recv;
    counter_branch  = var_desc->branch;
    counter_leaf = ntohs(var_desc->leaf);
    
    if(counter_branch == OAM_INTF_COUNTERS_MIB_BRANCH)
    {
        switch(counter_leaf){
        case OAM_DPOE_LEAF_MPCPXMIT:
            value = stats.tx_mac_ctrl;
            break;
        case OAM_DPOE_LEAF_MPCPRECV:
            value = stats.rx_mac_ctrl;
            break;
            
        case OAM_DPOE_LEAF_TXREGACK:
            value = stats.tx_reg_ack;
            break;
        case OAM_DPOE_LEAF_TXREGREQ:
            value = stats.tx_reg_request;
            break;
        case OAM_DPOE_LEAF_TXREPORT:
            value = stats.tx_report;
            break;
        case OAM_DPOE_LEAF_RXGATE:
            value = stats.rx_gate;
            break;
        case OAM_DPOE_LEAF_RXREG:
            value = stats.rx_register;
            break;

        }
    }
    data->value = value;
    data->value = htonll(data->value);

    data->hdr.width = sizeof(oam_pdu_uint64_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_pdu_uint64_t);

    return pdu_len;

}

