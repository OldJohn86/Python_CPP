/****************************************************************************
            Software License for Customer Use of Cortina Software
                          Grant Terms and Conditions

IMPORTANT NOTICE - READ CAREFULLY: This Software License for Customer Use
of Cortina Software ("LICENSE") is the agreement which governs use of
software of Cortina Systems, Inc. and its subsidiaries ("CORTINA"),
including computer software (source code and object code) and associated
printed materials ("SOFTWARE").  The SOFTWARE is protected by copyright laws
and international copyright treaties, as well as other intellectual property
laws and treaties.  The SOFTWARE is not sold, and instead is only licensed
for use, strictly in accordance with this document.  Any hardware sold by
CORTINA is protected by various patents, and is sold but this LICENSE does
not cover that sale, since it may not necessarily be sold as a package with
the SOFTWARE.  This LICENSE sets forth the terms and conditions of the
SOFTWARE LICENSE only.  By downloading, installing, copying, or otherwise
using the SOFTWARE, you agree to be bound by the terms of this LICENSE.
If you do not agree to the terms of this LICENSE, then do not download the
SOFTWARE.

DEFINITIONS:  "DEVICE" means the Cortina Systems(TM) SDK product.
"You" or "CUSTOMER" means the entity or individual that uses the SOFTWARE.
"SOFTWARE" means the Cortina Systems(TM) SDK software.

GRANT OF LICENSE:  Subject to the restrictions below, CORTINA hereby grants
CUSTOMER a non-exclusive, non-assignable, non-transferable, royalty-free,
perpetual copyright license to (1) install and use the SOFTWARE for
reference only with the DEVICE; and (2) copy the SOFTWARE for your internal
use only for use with the DEVICE.

RESTRICTIONS:  The SOFTWARE must be used solely in conjunction with the
DEVICE and solely with Your own products that incorporate the DEVICE.  You
may not distribute the SOFTWARE to any third party.  You may not modify
the SOFTWARE or make derivatives of the SOFTWARE without assigning any and
all rights in such modifications and derivatives to CORTINA.  You shall not
through incorporation, modification or distribution of the SOFTWARE cause
it to become subject to any open source licenses.  You may not
reverse-assemble, reverse-compile, or otherwise reverse-engineer any
SOFTWARE provided in binary or machine readable form.  You may not
distribute the SOFTWARE to your customers without written permission
from CORTINA.

OWNERSHIP OF SOFTWARE AND COPYRIGHTS. All title and copyrights in and the
SOFTWARE and any accompanying printed materials, and copies of the SOFTWARE,
are owned by CORTINA. The SOFTWARE protected by the copyright laws of the
United States and other countries, and international treaty provisions.
You may not remove any copyright notices from the SOFTWARE.  Except as
otherwise expressly provided, CORTINA grants no express or implied right
under CORTINA patents, copyrights, trademarks, or other intellectual
property rights.

DISCLAIMER OF WARRANTIES. THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING ANY IMPLIED WARRANTIES
OF MERCHANTABILITY, NONINFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE,
TITLE, AND NON-INFRINGEMENT.  CORTINA does not warrant or assume
responsibility for the accuracy or completeness of any information, text,
graphics, links or other items contained within the SOFTWARE.  Without
limiting the foregoing, you are solely responsible for determining and
verifying that the SOFTWARE that you obtain and install is the appropriate
version for your purpose.

LIMITATION OF LIABILITY. IN NO EVENT SHALL CORTINA OR ITS SUPPLIERS BE
LIABLE FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, LOST
PROFITS, BUSINESS INTERRUPTION, OR LOST INFORMATION) OR ANY LOSS ARISING OUT
OF THE USE OF OR INABILITY TO USE OF OR INABILITY TO USE THE SOFTWARE, EVEN
IF CORTINA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
TERMINATION OF THIS LICENSE. This LICENSE will automatically terminate if
You fail to comply with any of the terms and conditions hereof. Upon
termination, You will immediately cease use of the SOFTWARE and destroy all
copies of the SOFTWARE or return all copies of the SOFTWARE in your control
to CORTINA.  IF you commence or participate in any legal proceeding against
CORTINA, then CORTINA may, in its sole discretion, suspend or terminate all
license grants and any other rights provided under this LICENSE during the
pendency of such legal proceedings.
APPLICABLE LAWS. Claims arising under this LICENSE shall be governed by the
laws of the State of California, excluding its principles of conflict of
laws.  The United Nations Convention on Contracts for the International Sale
of Goods is specifically disclaimed.  You shall not export the SOFTWARE
without first obtaining any required export license or other approval from
the applicable governmental entity, if required.  This is the entire
agreement and understanding between You and CORTINA relating to this subject
matter.
GOVERNMENT RESTRICTED RIGHTS. The SOFTWARE is provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to
restrictions as set forth in FAR52.227-14 and DFAR252.227-7013 et seq. or
its successor. Use of the SOFTWARE by the Government constitutes
acknowledgment of CORTINA's proprietary rights therein. Contractor or
Manufacturer is CORTINA.

Copyright (c) 2013 by Cortina Systems Incorporated
****************************************************************************/
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
#include "oam_dpoe_upg.h"

extern oam_uint8 g_rule_start; 
extern void dpoe_llid_queue_adapt_init();

static g_dpoe_oam_handle_t g_dpoe_oam_vendor_handles;

static oam_var_desc_t g_dpoe_oam_odd_sets [] = {
    DPOE_OAM_ODD_SET_OAM_LISTS
};


typedef struct {
    oam_uint8 valid;
    oam_llid_t llid;
} dpoe_llid_index_t;

dpoe_llid_index_t g_dpoe_llid_index[OAM_MAX_LLID_NUM];

oam_llid_t g_curr_llid = 0;

void dpoe_llid_init()
{
    memset(g_dpoe_llid_index, 0, sizeof(dpoe_llid_index_t) * OAM_MAX_LLID_NUM);
    dpoe_llid_queue_adapt_init();
}

oam_status dpoe_index2llid(oam_uint8 index, oam_uint16 *llid)
{
    if (index > OAM_MAX_LLID_NUM)
        return OAM_E_ERROR;
    if (!g_dpoe_llid_index[index].valid) return OAM_E_ERROR;
    *llid = g_dpoe_llid_index[index].llid;
    return OAM_E_OK;
}

oam_status dpoe_llid2index(oam_uint8 *index, oam_uint16 llid)
{
    int i;
    for (i = 0; i < OAM_MAX_LLID_NUM; i++)
    {
        if (g_dpoe_llid_index[i].valid)
        {
            if (g_dpoe_llid_index[i].llid == llid)
                *index = i;
            return OAM_E_OK;
        }
    }
    return OAM_E_ERROR;
}

oam_boolean dpoe_oam_is_odd_set_oam(
    oam_uint8 branch,
    oam_uint16 leaf)
{
    oam_uint32 i;

    for (i = 0 ; i < sizeof(g_dpoe_oam_odd_sets) / sizeof(g_dpoe_oam_odd_sets[0]);
         i++) {
        if (branch == g_dpoe_oam_odd_sets[i].branch
            && leaf == g_dpoe_oam_odd_sets[i].leaf) {
            return TRUE;
        }
    }

    return FALSE;
}

void dpoe_oam_vendor_std_handler_reg(
    oam_oui_t oui,
    dpoe_oam_handle_t *handlers,
    oam_uint32 num)
{
    oam_uint32 i = 0;

    OAM_ASSERT(handlers != NULL);
    OAM_ASSERT(num <= OAM_DPOE_MAX_STD_HANDLER);


    memcpy(g_dpoe_oam_vendor_handles.oui, oui, sizeof(oam_oui_t));

    for (; i < num; i++) {
        memcpy(&g_dpoe_oam_vendor_handles.handlers[i], &handlers[i],
               sizeof(g_dpoe_oam_vendor_handles.handlers[i]));
    }

    g_dpoe_oam_vendor_handles.num = num;
}


dpoe_oam_handle_t g_dpoe_oam_xlat_table[] = {
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC)   { BRANCH, LEAF, GET_FUNC, SET_FUNC },
    DPOE_OAM_BUILD_RESP
};


/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_send                                             */
/* CATEGORY   : Device                                                       */
/* ACCESS     : public                                                       */
/* BLOCK      : General                                                      */
/* CHIP       : 8030                                                         */
/*                                                                           */
void dpoe_oam_send(
    oam_llid_t llid,
    oam_uint8 *buf,
    oam_int32 len
)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*           o  frame - Container buffer to store the composed OAM frame     */
/*           o  len - Size of OAM frame                                      */
/* OUTPUTS : ----                                                            */
/* RETURNS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to set DPoE OAM frame                                                 */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_if_t *intf = oam_intf_find(llid);

    if (intf == NULL) {
        OAM_LOG_MAJ("intf not found \n");
        return;
    }

    if (len < OAM_MIN_OAMPDU_SIZE)
        len = OAM_MIN_OAMPDU_SIZE;

    if (intf->dpoe_status == OAM_DPOE_STATUS_FINISH) {
        oam_send(llid, oam_port_id_get(), buf, len);
        intf->org_specific_tx++;
    }
    else {
        OAM_LOG_MAJ("dpoe not complete\n");
    }

}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_get_mgmt_obj                                     */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status dpoe_oam_get_mgmt_obj(
    oam_uint8           *data,
    dpoe_oam_mgmt_obj_t *mgmt_obj)
/*                                                                           */
/* INPUTS  : o data     - oam branch leaf data                               */
/* OUTPUTS : o mgmt_obj - dpoe mgmt object value                             */
/* DESCRIPTION:                                                              */
/* Get the port index according to the branch and leaf                       */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t *hdr = (oam_var_cont_t * )data;

    OAM_ASSERT_RET(data != NULL && mgmt_obj != NULL, OAM_E_PARAM);

    if (OAM_DPOE_OBJICT_BRANCH == hdr->branch) {
        if (ntohs(hdr->leaf) == OAM_DPOE_LEAF_OBJ_ONU) {
            oam_pdu_dpoe_obj_onu_t *obj_onu = (oam_pdu_dpoe_obj_onu_t *)data;

            mgmt_obj->valid = OAM_TRUE;
            mgmt_obj->len   = sizeof(oam_var_cont_t) + hdr->width;
            mgmt_obj->obj.onu.hdr.branch = hdr->branch;
            mgmt_obj->obj.onu.hdr.leaf   = ntohs(hdr->leaf);
            mgmt_obj->obj.onu.hdr.width  = 1;
            mgmt_obj->obj.onu.value      = obj_onu->value;

            OAM_ORG_DEBUG("Obj = ONU, Index branch :%#x , leaf:%#x value:%#x \n",
                          hdr->branch, ntohs(hdr->leaf), obj_onu->value);

            return OAM_E_OK;
        }
        else if (ntohs(hdr->leaf) == OAM_DPOE_LEAF_OBJ_PON) {
            oam_pdu_dpoe_obj_pon_t *obj_pon = (oam_pdu_dpoe_obj_pon_t *)data;

            mgmt_obj->valid = OAM_TRUE;
            mgmt_obj->len   = sizeof(oam_var_cont_t) + hdr->width;
            mgmt_obj->obj.pon.hdr.branch = hdr->branch;
            mgmt_obj->obj.pon.hdr.leaf   = ntohs(hdr->leaf);
            mgmt_obj->obj.pon.hdr.width  = 1;
            mgmt_obj->obj.pon.value      = obj_pon->value;

            OAM_ORG_DEBUG("Obj = PON, Index branch :%#x , leaf:%#x value:%#x \n",
                          hdr->branch, ntohs(hdr->leaf), obj_pon->value);

            return OAM_E_OK;
        }
        else if (ntohs(hdr->leaf) == OAM_DPOE_LEAF_OBJ_LLID) {
            oam_pdu_dpoe_obj_llid_t *obj_llid = (oam_pdu_dpoe_obj_llid_t *)data;

            mgmt_obj->valid = OAM_TRUE;
            mgmt_obj->len   = sizeof(oam_var_cont_t) + hdr->width;
            mgmt_obj->obj.llid.hdr.branch = hdr->branch;
            mgmt_obj->obj.llid.hdr.leaf   = ntohs(hdr->leaf);
            mgmt_obj->obj.llid.hdr.width  = 1;
            mgmt_obj->obj.llid.value      = obj_llid->value;

            OAM_ORG_DEBUG("Obj = LLID, Index branch :%#x , leaf:%#x value:%#x \n",
                          hdr->branch, ntohs(hdr->leaf), obj_llid->value);

            return OAM_E_OK;
        }
        else if (ntohs(hdr->leaf) == OAM_DPOE_LEAF_OBJ_UNI) {
            oam_pdu_dpoe_obj_uni_t *obj_uni = (oam_pdu_dpoe_obj_uni_t *)data;

            mgmt_obj->valid = OAM_TRUE;
            mgmt_obj->len   = sizeof(oam_var_cont_t) + hdr->width;
            mgmt_obj->obj.uni.hdr.branch = hdr->branch;
            mgmt_obj->obj.uni.hdr.leaf   = ntohs(hdr->leaf);
            mgmt_obj->obj.uni.hdr.width  = 1;
            mgmt_obj->obj.uni.value      = obj_uni->value;

            OAM_ORG_DEBUG("Obj = UNI, Index branch :%#x , leaf:%#x value:%#x \n",
                          hdr->branch, ntohs(hdr->leaf), obj_uni->value);

            return OAM_E_OK;
        }
        else if (ntohs(hdr->leaf) == OAM_DPOE_LEAF_OBJ_QUEUE) {
            oam_pdu_dpoe_obj_queue_t *obj_queue = (oam_pdu_dpoe_obj_queue_t *)data;

            mgmt_obj->valid = OAM_TRUE;
            mgmt_obj->len   = sizeof(oam_var_cont_t) + hdr->width;
            mgmt_obj->obj.queue.hdr.branch = hdr->branch;
            mgmt_obj->obj.queue.hdr.leaf   = ntohs(hdr->leaf);
            mgmt_obj->obj.queue.hdr.width  = hdr->width;
            mgmt_obj->obj.queue.type       = ntohs(obj_queue->type);
            mgmt_obj->obj.queue.instance   = obj_queue->instance;
            mgmt_obj->obj.queue.number     = obj_queue->number;

            OAM_ORG_DEBUG("Obj = QUEUE, Index branch :%#x , leaf:%#x type:%#x instance:%#x number:%#x \n",
                          hdr->branch, ntohs(hdr->leaf), obj_queue->type, obj_queue->instance,
                          obj_queue->number);

            return OAM_E_OK;
        }
    }

    return OAM_E_ERROR;
}


/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_build_mgmt_obj                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32 dpoe_oam_build_mgmt_obj(
    oam_uint8            *data,
    dpoe_oam_mgmt_obj_t *mgmt_obj)
/*                                                                           */
/* INPUTS  : o  data     - oam packet                                        */
/*           o  mgmt_obj - dpoe mgmt object value                            */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the obj context TLV for the dpoe mgmt                               */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t  *var_cont = NULL;

    OAM_ASSERT_RET(data != NULL, OAM_E_PARAM);
    OAM_ASSERT_RET(mgmt_obj != NULL, OAM_E_PARAM);

    var_cont = (oam_var_cont_t *)(&mgmt_obj->obj);
    OAM_ORG_DEBUG("Start to build DPOE Object\n");

    if (OAM_DPOE_OBJICT_BRANCH == var_cont->branch) {
        if (var_cont->leaf == OAM_DPOE_LEAF_OBJ_ONU) {
            oam_pdu_dpoe_obj_onu_t *obj_onu = (oam_pdu_dpoe_obj_onu_t *)data;

            obj_onu->hdr.branch = var_cont->branch;
            obj_onu->hdr.leaf   = htons(var_cont->leaf);
            obj_onu->hdr.width  = var_cont->width;
            obj_onu->value      = mgmt_obj->obj.onu.value;

            return mgmt_obj->len;
        }
        else if (var_cont->leaf == OAM_DPOE_LEAF_OBJ_PON) {
            oam_pdu_dpoe_obj_pon_t *obj_pon = (oam_pdu_dpoe_obj_pon_t *)data;

            obj_pon->hdr.branch = var_cont->branch;
            obj_pon->hdr.leaf   = htons(var_cont->leaf);
            obj_pon->hdr.width  = var_cont->width;
            obj_pon->value      = mgmt_obj->obj.pon.value;

            return mgmt_obj->len;
        }
        else if (var_cont->leaf == OAM_DPOE_LEAF_OBJ_LLID) {
            oam_pdu_dpoe_obj_llid_t *obj_llid = (oam_pdu_dpoe_obj_llid_t *)data;

            obj_llid->hdr.branch = var_cont->branch;
            obj_llid->hdr.leaf   = htons(var_cont->leaf);
            obj_llid->hdr.width  = var_cont->width;
            obj_llid->value      = mgmt_obj->obj.llid.value;

            return mgmt_obj->len;
        }
        else if (var_cont->leaf == OAM_DPOE_LEAF_OBJ_UNI) {
            oam_pdu_dpoe_obj_uni_t *obj_uni = (oam_pdu_dpoe_obj_uni_t *)data;

            obj_uni->hdr.branch = var_cont->branch;
            obj_uni->hdr.leaf   = htons(var_cont->leaf);
            obj_uni->hdr.width  = var_cont->width;
            obj_uni->value      = mgmt_obj->obj.uni.value;

            return mgmt_obj->len;
        }
        else if (var_cont->leaf == OAM_DPOE_LEAF_OBJ_QUEUE) {
            oam_pdu_dpoe_obj_queue_t *obj_queue = (oam_pdu_dpoe_obj_queue_t *)data;

            obj_queue->hdr.branch = var_cont->branch;
            obj_queue->hdr.leaf   = htons(var_cont->leaf);
            obj_queue->hdr.width  = var_cont->width;
            obj_queue->type       = htons(mgmt_obj->obj.queue.type);
            obj_queue->instance   = mgmt_obj->obj.queue.instance;
            obj_queue->number     = mgmt_obj->obj.queue.number;

            return mgmt_obj->len;
        }
    }

    return 0;

}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_check_mgmt_obj                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_boolean dpoe_oam_check_mgmt_obj(
    dpoe_oam_mgmt_obj_t *mgmt_obj)
/*                                                                           */
/* INPUTS  : o  mgmt_obj - dpoe mgmt object value                            */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* check the obj context TLV for the dpoe mgmt                               */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t  *var_cont = NULL;

    OAM_ASSERT_RET(mgmt_obj != NULL, OAM_FALSE);

    var_cont = (oam_var_cont_t *)(&mgmt_obj->obj);

    if (OAM_DPOE_OBJICT_BRANCH == var_cont->branch) {
        if (var_cont->leaf == OAM_DPOE_LEAF_OBJ_ONU) {
            return OAM_TRUE;
        }
        else if (var_cont->leaf == OAM_DPOE_LEAF_OBJ_PON) {
            return OAM_TRUE;
        }
        else if (var_cont->leaf == OAM_DPOE_LEAF_OBJ_LLID) {
            return OAM_TRUE;
        }
        else if (var_cont->leaf == OAM_DPOE_LEAF_OBJ_UNI) {
            return OAM_TRUE;
        }
        else if (var_cont->leaf == OAM_DPOE_LEAF_OBJ_QUEUE) {
            return OAM_TRUE;
        }
    }

    return OAM_FALSE;

}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_build_header                                     */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32 dpoe_oam_build_header(
    oam_uint8 *oui,
    oam_llid_t  llid,
    oam_uint8 *buf,
    oam_uint8  opcode)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*           o  buf - the oam packet                                         */
/*           o  opcode - the opcode for the oam packet                       */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the dpoe oam packet header                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_if_t *intf = oam_intf_find(llid);
    oam_pdu_dpoe_t *pdu = (oam_pdu_dpoe_t *)buf;

    OAM_ASSERT_RET(buf != NULL, 0);

    if (intf == NULL) {
        OAM_ORG_LOG("Unknown llid 0x%x\n", llid);
        return 0;
    }

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)buf, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pdu->oui, oui, OAM_OUI_LEN);
    pdu->opcode = opcode;

    return sizeof(oam_pdu_dpoe_t) - 1;
}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_build_get_not_support                            */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32  dpoe_oam_build_get_not_support(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
/*                                                                           */
/* INPUTS  : o  mgmt_obj - oam mgmt object                                   */
/*           o  recv - received oam packet                                   */
/*           o  proc_recv_len - process the received oam packet length       */
/*           o  out - The out oam packet                                     */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the get not support oam                                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t *var_cont = (oam_var_cont_t *)out;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    var_cont->width = OAM_DPOE_RSP_UNSUPPORT;

    return sizeof(oam_var_cont_t);
}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_build_set_not_support                            */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32  dpoe_oam_build_set_not_support(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32    recv_len,
    oam_uint8    *out)
/*                                                                           */
/* INPUTS  : o  mgmt_obj - oam mgmt object                                   */
/*           o  recv - received oam packet                                   */
/*           o  recv_len - the received oam packet length                    */
/*           o  out - The out oam packet                                     */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the set not support oam                                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t *var_cont = (oam_var_cont_t *)out;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    var_cont->width = OAM_DPOE_RSP_UNSUPPORT;

    return sizeof(oam_var_cont_t);
}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_find_get_handler                                 */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
dpoe_oam_get_handler_t dpoe_oam_find_get_handler(
    oam_oui_t oui,
    oam_uint8 branch,
    oam_uint16 leaf)
/*                                                                           */
/* INPUTS  : o  branch - the dpoe oam branch id                              */
/*           o  leaf - the dpoe oam leaf id                                  */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* find the get handler for special branch and leaf                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    dpoe_oam_get_handler_t  get_handler = NULL;
    oam_uint32 i = 0;

    /* Call vendor handle first*/
    if (oam_is_oui_equal(oui, g_dpoe_oam_vendor_handles.oui)) {

        for (i = 0 ; i < g_dpoe_oam_vendor_handles.num; i ++) {

            if (g_dpoe_oam_vendor_handles.handlers[i].branch == branch
                && g_dpoe_oam_vendor_handles.handlers[i].leaf == leaf) {

                OAM_ORG_DEBUG("dpoe oam handler found by vendor: branch %#x leaf %#x\n",
                              branch, leaf);
                get_handler = g_dpoe_oam_vendor_handles.handlers[i].get_func;
                break;
            }
        }
    }

    /* if vendor not found, call dpoe std handler*/
    if (get_handler == NULL ) {
        for (i = 0 ;
             i < sizeof(g_dpoe_oam_xlat_table) / sizeof(g_dpoe_oam_xlat_table[0]) ; i++) {

            if (g_dpoe_oam_xlat_table[i].branch == branch
                && g_dpoe_oam_xlat_table[i].leaf == leaf) {

                OAM_ORG_DEBUG("dpoe oam handler found: branch %#x leaf %#x\n",
                              branch, leaf);
                get_handler =  g_dpoe_oam_xlat_table[i].get_func;
                break;
            }
        }
    }

    return get_handler;
}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_find_set_handler                                 */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
dpoe_oam_set_handler_t dpoe_oam_find_set_handler(
    oam_oui_t oui,
    oam_uint8 branch,
    oam_uint16 leaf)
/*                                                                           */
/* INPUTS  : o  branch - the dpoe oam branch id                              */
/*           o  leaf - the dpoe oam leaf id                                  */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* find the set handler for special branch and leaf                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    dpoe_oam_set_handler_t  set_handler = NULL;
    oam_uint32 i = 0;

    /* Call vendor handle first*/
    if (oam_is_oui_equal(oui, g_dpoe_oam_vendor_handles.oui)) {

        for (i = 0 ; i < g_dpoe_oam_vendor_handles.num; i ++) {

            if (g_dpoe_oam_vendor_handles.handlers[i].branch == branch &&
                g_dpoe_oam_vendor_handles.handlers[i].leaf   == leaf) {

                OAM_ORG_DEBUG("dpoe oam handler found by vendor: branch %#x leaf %#x\n",
                              branch, leaf);
                set_handler = g_dpoe_oam_vendor_handles.handlers[i].set_func;
                break;
            }
        }
    }

    /* if vendor not found, call dpoe std handler*/
    if (set_handler == NULL ) {
        for (i = 0 ;
             i < sizeof(g_dpoe_oam_xlat_table) / sizeof(g_dpoe_oam_xlat_table[0]); i++) {

            if (g_dpoe_oam_xlat_table[i].branch == branch &&
                g_dpoe_oam_xlat_table[i].leaf   == leaf) {

                OAM_ORG_DEBUG("dpoe oam handler found: branch %#x leaf %#x\n",
                              branch, leaf);
                set_handler =  g_dpoe_oam_xlat_table[i].set_func;
                break;
            }
        }
    }

    return set_handler;
}


/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_build_get_tlv                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32 dpoe_oam_build_get_tlv(
    oam_oui_t oui,
    oam_uint8 *pOutVarCont,
    oam_var_desc_t *pRecvVarCont,
    oam_uint32 *procLen,
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_boolean *idxBuild)
/*                                                                           */
/* INPUTS  : o  pOutVarCont - the send oam packet                            */
/*           o  pRecevVarCont - the recevied oam packet                      */
/*           o  procLen - the length of process the received packet          */
/*           o  mgmtIndex - the dpoe obj mgmt index                          */
/*           o  idxBuild - the index tlv build flag                          */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the get branch leaf packet                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t *pOut = (oam_var_cont_t *) pOutVarCont;
    oam_uint32 len = 0, outPktLen = 0;
    dpoe_oam_get_handler_t get_handler = NULL;
    oam_boolean buildFlag = FALSE;

    OAM_ASSERT_RET(idxBuild != NULL
                   && pRecvVarCont != NULL
                   && pOutVarCont != NULL
                   && procLen != NULL, 0);

    /* default get only process the branch and leaf,
     * but the alarm config need process more than it */
    *procLen = sizeof(oam_var_desc_t);

    get_handler = dpoe_oam_find_get_handler(oui, pRecvVarCont->branch,
                                            ntohs(pRecvVarCont->leaf));

    if (get_handler == NULL) {
        get_handler = dpoe_oam_build_get_not_support;
    }

    /*no index include*/
    if (!mgmt_obj.valid) {
        OAM_ORG_DEBUG(" No index branch %#x, leaf %#x \n",
                      pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));
        pOut->branch = pRecvVarCont->branch;
        pOut->leaf = pRecvVarCont->leaf;
        outPktLen = get_handler(mgmt_obj, (oam_uint8 *)pRecvVarCont,
                                procLen, pOutVarCont);
    }
    else {
        OAM_ORG_DEBUG(" Branch %#x, leaf %#x \n",
                      pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));

        /* obj index build only once for each mgmt obj */
        if (!*idxBuild) {
            outPktLen = dpoe_oam_build_mgmt_obj(pOutVarCont, &mgmt_obj);

            OAM_ORG_DEBUG("build index len = %d \n", outPktLen);
            *idxBuild = TRUE;
            buildFlag = TRUE;
            pOutVarCont += outPktLen;
        }

        pOut = (oam_var_cont_t *) pOutVarCont;
        pOut->branch = pRecvVarCont->branch;
        pOut->leaf = pRecvVarCont->leaf;

        if (OAM_FALSE == dpoe_oam_check_mgmt_obj(&mgmt_obj) &&
            (get_handler != dpoe_oam_build_get_not_support)) {
            pOut->width = OAM_DPOE_RSP_BAD_PARAM;
            outPktLen += sizeof(oam_var_cont_t);
        }
        else {
            len = get_handler(mgmt_obj, (oam_uint8 *)pRecvVarCont,
                              procLen, pOutVarCont);

            if ((len == 0) && buildFlag) {
                pOutVarCont -= outPktLen;
                *idxBuild = FALSE;
                outPktLen = 0;
            }
            else {
                outPktLen += len;
            }
        }
    }

    return outPktLen;
}


/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_build_set_tlv                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32 dpoe_oam_build_set_tlv(
    oam_oui_t oui,
    oam_uint8 *pOutVarCont,
    oam_uint8 *pRecv,
    oam_uint32  tlvLen,
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_boolean *idxBuild)
/*                                                                           */
/* INPUTS  : o  pOutVarCont - the send oam packet                            */
/*           o  pRecv - the recevied oam packet                              */
/*           o  tlvLen - the received packet  length                         */
/*           o  mgmt_obj - the dpoe obj mgmt index                           */
/*           o  idxBuild - the index tlv build flag                          */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the set branch leaf packet                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t *pOut
        = (oam_var_cont_t * ) pOutVarCont;
    oam_var_cont_t *pRecvVarCont
        = (oam_var_cont_t * ) pRecv;
    oam_uint32 len = 0, outPktLen = 0;
    dpoe_oam_set_handler_t set_handler = NULL;
    oam_boolean buildFlag = FALSE;

    OAM_ASSERT_RET(idxBuild != NULL
                   && pRecvVarCont != NULL
                   && pOutVarCont != NULL, 0);

    /*oam_buf_print(pRecv, tlvLen + sizeof(oam_var_cont_t));*/

    set_handler = dpoe_oam_find_set_handler(oui, pRecvVarCont->branch,
                                            ntohs(pRecvVarCont->leaf));

    if (set_handler == NULL) {
        set_handler = dpoe_oam_build_set_not_support;
    }

    /*no index include*/
    if (!mgmt_obj.valid) {
        OAM_ORG_DEBUG("No index branch %#x, leaf %#x \n",
                      pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));
        pOut->branch = pRecvVarCont->branch;
        pOut->leaf = pRecvVarCont->leaf;
        outPktLen = set_handler(mgmt_obj, pRecv, tlvLen, pOutVarCont);
    }
    else {
        OAM_ORG_DEBUG(" single index branch %#x, leaf %#x \n",
                      pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));

        if (!*idxBuild) {

            outPktLen = dpoe_oam_build_mgmt_obj(pOutVarCont, &mgmt_obj);

            OAM_ORG_DEBUG("build index len = %d \n", outPktLen);
            *idxBuild = TRUE;
            buildFlag = TRUE;
            pOutVarCont += outPktLen;
        }

        pOut = (oam_var_cont_t *) pOutVarCont;
        pOut->branch = pRecvVarCont->branch;
        pOut->leaf = pRecvVarCont->leaf;


        if (OAM_FALSE == dpoe_oam_check_mgmt_obj(&mgmt_obj)
            && (set_handler != dpoe_oam_build_set_not_support)) {
            pOut->width = OAM_DPOE_RSP_BAD_PARAM;
            outPktLen += sizeof(oam_var_cont_t);
        }
        else {
            len = set_handler(mgmt_obj, pRecv, tlvLen, pOutVarCont);

            if ((len == 0) && buildFlag) {
                pOutVarCont -= outPktLen;
                *idxBuild = FALSE;
                outPktLen = 0;
            }
            else {
                outPktLen += len;
            }
        }
    }

    return outPktLen;
}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_build_var_rsp                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32 dpoe_oam_build_var_rsp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*           o  frame - Container buffer to store the composed OAM frame     */
/*           o  lenght - OAM frame length                                    */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* API to handler DPoE OAM var request                                       */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_uint8    outPdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dpoe_t   *pOutPkt = (oam_pdu_dpoe_t *)pdu;
    oam_pdu_dpoe_t   *pRecvPkt = NULL;
    oam_var_desc_t   *pRecvVarCont = NULL;
    oam_uint8    *pOutVarCont = NULL;
    oam_int32   totalContentLen = 0, outPktLen = 0;
    oam_boolean  isFirstGetTlv = TRUE,  idxBuild = FALSE;
    dpoe_oam_mgmt_obj_t  mgmt_obj;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length >=
                   sizeof(oam_pdu_dpoe_t) + sizeof(oam_var_desc_t) - 1, 0);

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    pRecvPkt = (oam_pdu_dpoe_t *)frame;
    outPktLen = dpoe_oam_build_header(pRecvPkt->oui, llid, pdu,
                                      OAM_PDU_DPOE_VAR_RSP);
    pOutVarCont = pOutPkt->data;

    /*shift the header of received packet*/
    totalContentLen = length - sizeof(oam_pdu_dpoe_t) + 1;
    pRecvVarCont = (oam_var_desc_t *)pRecvPkt->data;

    memset(&mgmt_obj, 0, sizeof(dpoe_oam_mgmt_obj_t));
    mgmt_obj.valid = FALSE;

    while (totalContentLen > 0) {
        oam_uint32 len = 0;
        oam_uint32 procLen = 0;
        OAM_ORG_LOG("VAR REQ branch 0x%02x, leaf 0x%04x \n",
                      pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));

        if (pRecvVarCont->branch == OAM_ATTRIB_BRANCH_NULL) {
            if (isFirstGetTlv) {
                OAM_ORG_LOG("Bad packet \n");
                return 0;
            }
            else {
                /*End TLV, break and send*/
                break;
            }
        }

        if (OAM_E_OK == dpoe_oam_get_mgmt_obj((oam_uint8 *)pRecvVarCont, &mgmt_obj)) {
            /*shift the index TLV*/
            totalContentLen -= mgmt_obj.len;
            pRecvVarCont = (oam_var_desc_t *)
                           ((oam_uint8 *)pRecvVarCont + mgmt_obj.len);
            idxBuild = FALSE;
            isFirstGetTlv = TRUE;
            continue;
        }

        memset(outPdu, 0, OAM_MAX_OAMPDU_SIZE);

        len = dpoe_oam_build_get_tlv(pRecvPkt->oui, outPdu, pRecvVarCont,
                                     &procLen, mgmt_obj, &idxBuild);

        if (outPktLen + len > (OAM_MAX_OAMPDU_SIZE - 2)) {
            OAM_ORG_LOG("build length %d great than max len %d \n",
                        outPktLen + len, OAM_MAX_OAMPDU_SIZE - 2);
            break;
        }

        if (len > 0) {
            memcpy(pOutVarCont, outPdu, len);
        }

        outPktLen += len;
        pOutVarCont += len;

        totalContentLen -= procLen;

        /* the remain pkt is less than branch and leaf's length*/
        if (totalContentLen < sizeof(oam_var_desc_t)) {
            break;
        }

        pRecvVarCont = (oam_var_desc_t *)((oam_uint8 *)pRecvVarCont + procLen);
        isFirstGetTlv = FALSE;
    }

    /* packet 2  0's to mark the end - 57.6.3 a)*/
    *pOutVarCont++ = 0;
    //*pOutVarCont++ = 0;
    outPktLen += 1;

    if (outPktLen > (sizeof(oam_pdu_dpoe_t) - 1 + 2)) {
        dpoe_oam_send(llid, pdu, outPktLen);
    }

    return outPktLen;
}


/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_build_set_rsp                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32 dpoe_oam_build_set_rsp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*           o  frame - Container buffer to store the composed OAM frame     */
/*           o  lenght - OAM frame length                                    */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* API to handler DPoE OAM set request                                       */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_uint8    outPdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dpoe_t *pOutPkt = (oam_pdu_dpoe_t *)pdu;
    oam_pdu_dpoe_t *pRecvPkt = NULL;
    oam_dpoe_onu_var_set_hdr_t *pRecvVarCont = NULL;
    oam_uint8 *pOutVarCont = NULL;
    oam_uint8 *pInVarCont = NULL;
    oam_int32 totalContentLen = 0;
    oam_uint32 outPktLen = 0;
    oam_int32  len = 0;

    oam_boolean  isFirstSetTLV = TRUE,  idxBuild = FALSE;
    dpoe_oam_mgmt_obj_t  mgmt_obj;
    oam_uint8 lastBranch = OAM_ATTRIB_BRANCH_NULL;
    oam_uint16 lastLeaf = 0;
    oam_uint8 tlvLen = 0;
    oam_uint8 *pLastRecvVarCont = NULL;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length >=
                   sizeof(oam_pdu_dpoe_t) + sizeof(oam_var_desc_t) - 1, 0);

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    pRecvPkt = (oam_pdu_dpoe_t *)frame;
    outPktLen = dpoe_oam_build_header(pRecvPkt->oui, llid, pdu,
                                      OAM_PDU_DPOE_SET_RSP);
    pOutVarCont = pOutPkt->data;

    /*shift the header of received packet*/
    totalContentLen = length - sizeof(oam_pdu_dpoe_t) + 1;
    pRecvVarCont = (oam_dpoe_onu_var_set_hdr_t *)pRecvPkt->data;

    pInVarCont = (oam_uint8 *)pRecvVarCont;

    mgmt_obj.valid = FALSE;
    g_rule_start = TRUE;

    while (totalContentLen > 0) {
        OAM_ORG_DEBUG("SET REQ branch 0x%x, leaf 0x%x\n",
                      pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));

        if (pRecvVarCont->branch == OAM_ATTRIB_BRANCH_NULL) {
            if (isFirstSetTLV) {
                OAM_ORG_LOG("Bad packet \n");
                if (mgmt_obj.valid)
                {
                    memset(outPdu, 0, OAM_MAX_OAMPDU_SIZE);
                    len = dpoe_oam_build_mgmt_obj(pOutVarCont, &mgmt_obj);
                    outPktLen += len;
                    pOutVarCont += len;
                }
            }
            else {
                /*End TLV, break and send*/

            }

            break;
        }

#if 0
        /* check the duplicate TLV */
        if (lastBranch == pRecvVarCont->branch
            && lastLeaf == ntohs(pRecvVarCont->leaf)) {
            oam_uint32 tmpLen;

            OAM_ORG_DEBUG("duplicate branch:%#x, leaf:%#x , len = %d\n",
                          pRecvVarCont->branch , ntohs(pRecvVarCont->leaf),
                          pRecvVarCont->width);

            /* special set oam only has branch and leaf, no length*/
            if (dpoe_oam_is_odd_set_oam(
                    pRecvVarCont->branch, ntohs(pRecvVarCont->leaf))) {
                /* Only remove the duplicate tlv from the pdu*/
                tmpLen = sizeof(oam_var_cont_t) - 1;
                memmove(pInVarCont,
                        pInVarCont + tmpLen,
                        totalContentLen - tmpLen);
                totalContentLen -= tmpLen;
            }
            else {
                tmpLen = pRecvVarCont->width;
                tlvLen += tmpLen;
                memmove(pInVarCont,
                        pInVarCont + sizeof(oam_var_cont_t),
                        totalContentLen - sizeof(oam_var_cont_t));
                totalContentLen -= (tmpLen + sizeof(oam_var_cont_t));
                pInVarCont += tmpLen;
            }

        }
        else {
#endif
            /*is not first tlv*/
            if (lastBranch != OAM_ATTRIB_BRANCH_NULL
                && lastLeaf != 0) {
                OAM_ORG_DEBUG("process the previous TLV, branch:%#x, leaf:%#x \n",
                              lastBranch, lastLeaf);
                /* process the pLastRecvVarCont */
                memset(outPdu, 0, OAM_MAX_OAMPDU_SIZE);
                len = dpoe_oam_build_set_tlv(pRecvPkt->oui, outPdu,
                                             pLastRecvVarCont, tlvLen, mgmt_obj, &idxBuild);

                if (outPktLen + len > (OAM_MAX_OAMPDU_SIZE)) {
                    OAM_ORG_LOG("build length %d great than max len %d \n",
                                outPktLen + len, OAM_MAX_OAMPDU_SIZE );
                    break;
                }
                else {
                    memcpy(pOutVarCont, outPdu, len);
                    outPktLen += len;
                    pOutVarCont += len;
                }
            }

            if (OAM_E_OK == dpoe_oam_get_mgmt_obj((oam_uint8 *)pRecvVarCont, &mgmt_obj)) {
                /*shift the index TLV*/
                totalContentLen -= mgmt_obj.len;
                pInVarCont += mgmt_obj.len;
                pRecvVarCont = (oam_dpoe_onu_var_set_hdr_t *)pInVarCont;

                idxBuild = FALSE;
                lastBranch = OAM_ATTRIB_BRANCH_NULL;
                lastLeaf = 0;
                continue;
            }

            lastBranch = pRecvVarCont->branch;
            lastLeaf = ntohs(pRecvVarCont->leaf);
            pLastRecvVarCont = pInVarCont;

            OAM_ORG_DEBUG("lastBranch =  0x%x, lastLeaf =  0x%x\n",lastBranch, lastLeaf);

#if 0
            /* special set oam only has branch and leaf, no length*/
            if (dpoe_oam_is_odd_set_oam(
                    pRecvVarCont->branch, ntohs(pRecvVarCont->leaf))) {
                tlvLen = 0;
                pInVarCont += (sizeof(oam_var_cont_t) - 1);
                totalContentLen -= (sizeof(oam_var_cont_t) - 1);
            }
            else {
#endif
                if (pRecvVarCont->width != 0x80)
                {
                    tlvLen = pRecvVarCont->width;
                    pInVarCont += tlvLen + sizeof(oam_var_cont_t);
                    totalContentLen -= (tlvLen + sizeof(oam_var_cont_t));
                    OAM_ORG_DEBUG("pRecvVarCont->width != 0x80 , tlvLen = %d, tlvLen + sizeof(oam_var_cont_t) = %d",tlvLen,tlvLen + sizeof(oam_var_cont_t));
                }
                else
                {
                    tlvLen = 0;
                    pInVarCont += tlvLen + sizeof(oam_var_cont_t);
                    totalContentLen -= (tlvLen + sizeof(oam_var_cont_t));
                    OAM_ORG_DEBUG("pRecvVarCont->width == 0x80");
                }
            //}

        //}

        if (totalContentLen
            < sizeof(oam_var_desc_t)) {
            break;
        }

        isFirstSetTLV = FALSE;
        pRecvVarCont = (oam_dpoe_onu_var_set_hdr_t *)pInVarCont;
    }

    /* process the Last TLV */
    if (pLastRecvVarCont != NULL) {
        OAM_ORG_DEBUG("process the final one TLV \n");
        memset(outPdu, 0, OAM_MAX_OAMPDU_SIZE);
        len = dpoe_oam_build_set_tlv(pRecvPkt->oui, outPdu,
                                     pLastRecvVarCont, tlvLen, mgmt_obj, &idxBuild);

        if (outPktLen + len > (OAM_MAX_OAMPDU_SIZE)) {
            OAM_ORG_LOG("build length %d great than max len %d \n",
                        outPktLen + len, OAM_MAX_OAMPDU_SIZE );
        }
        else {
            memcpy(pOutVarCont, outPdu, len);
            outPktLen += len;
            pOutVarCont += len;
        }
    }

    /* packet 2  0's to mark the end - 57.6.3 a)*/
    *pOutVarCont++ = 0;
    //*pOutVarCont++ = 0;
    outPktLen += 1;

    if (outPktLen > (sizeof(oam_pdu_dpoe_t) - 1 + 2)) {
        dpoe_oam_send(llid, pdu, outPktLen);
    }

    g_rule_start = FALSE;
    return outPktLen;
}

oam_int32 dpoe_oam_build_mc_ctrl_rsp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_status ret = OAM_E_OK;
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dpoe_t *pOutPkt = (oam_pdu_dpoe_t *)pdu;
    oam_pdu_dpoe_t *pRecvPkt = NULL;
    oam_dpoe_onu_mc_ctrl_t *mc_ctrl = NULL;
    oam_uint32 outPktLen = 0;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    pRecvPkt = (oam_pdu_dpoe_t *)frame;
    outPktLen = dpoe_oam_build_header(pRecvPkt->oui, llid, pdu,
      OAM_PDU_DPOE_IP_MCAST_CTRL_RSP);
    mc_ctrl = (oam_dpoe_onu_mc_ctrl_t *)pRecvPkt->data;

    mc_ctrl->llid = ntohs(mc_ctrl->llid);
    ret = dpoe_oam_mc_ctrl_set_adapt(mc_ctrl);

    if(ret == OAM_E_OK){
        pOutPkt->data[0] = OAM_DPOE_RSP_OK;
    }else{
        pOutPkt->data[0] = OAM_DPOE_RSP_HW_FAILURE;
    }

    outPktLen += sizeof(pOutPkt->data);
    dpoe_oam_send(llid, pdu, outPktLen);
    return outPktLen;
}

oam_int32 dpoe_oam_build_mc_reg_rsp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_status ret = OAM_E_OK;
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dpoe_t *pOutPkt = (oam_pdu_dpoe_t *)pdu;
    oam_pdu_dpoe_t *pRecvPkt = NULL;
    oam_dpoe_onu_mc_reg_t *mc_in = NULL, *mc_out = NULL;
    oam_uint32 outPktLen = 0;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    pRecvPkt = (oam_pdu_dpoe_t *)frame;
    outPktLen = dpoe_oam_build_header(pRecvPkt->oui, llid, pdu,
      OAM_PDU_DPOE_MCAST_REG_RSP);
    mc_in = (oam_dpoe_onu_mc_reg_t *)pRecvPkt->data;
    mc_out = (oam_dpoe_onu_mc_reg_t *)pOutPkt->data;

    mc_in->mc_llid = ntohs(mc_in->mc_llid);
    mc_in->uc_llid = ntohs(mc_in->uc_llid);
    ret = dpoe_oam_mc_reg_set_adapt(mc_in);

    memcpy(mc_out, mc_in, sizeof(oam_dpoe_onu_mc_reg_t));
    mc_out->flag = OAM_DPOE_MC_REG_FLAG_REG;
    mc_out->mc_llid = 0x7Eff;
    mc_out->mc_llid = htons(mc_out->mc_llid);
    //mc_out->uc_llid = htons(llid);
    outPktLen += sizeof(oam_dpoe_onu_mc_reg_t);
    dpoe_oam_send(llid, pdu, outPktLen);
    return outPktLen;
}


extern void dpoe_encrypt_llid_reg(oam_llid_t llid, oam_uint16 index);
extern void dpoe_encrypt_llid_dereg(oam_llid_t llid, oam_uint16 index);

void oam_dpoe_llid_reg(oam_llid_t llid, oam_uint16 index)
{
    dpoe_encrypt_llid_reg(llid, index);
    if (index > OAM_MAX_LLID_NUM) return;
    g_dpoe_llid_index[index].valid = 1;
    g_dpoe_llid_index[index].llid = llid;
    if(0 == index){
        dpoe_llid_queue_adapt_init();
    }
}

void oam_dpoe_llid_dereg(oam_llid_t llid, oam_uint16 index, oam_uint32 dereg_cause)
{
    oam_uint8 olt_str[8] = "SUMITOMO";
    oam_uint8 olt_type[20];
    oam_uint32 len = 0;
    oam_uint32 i = 0;
    oam_ext_mode_t ext_mode = OAM_EXT_MODE_DPOE;
    ext_mode = oam_ext_mode_get_adapt();
    
    /* bug51956 Reset ONU when the base LLID is deregistered by Sumitomo OLT in DPoE mode */
    if(ext_mode == OAM_EXT_MODE_DPOE && index == 0 && dereg_cause == OAM_MPCP_DEREG_CAUSE_OLT_DEREGISTER)
    {
        oam_olt_type_get_adapt(&olt_type[0], &len);
        if(!memcmp(olt_type, olt_str, len))
        {
            oam_printf("\r\nstart to reset ONU\r\n");
            ca_reset(0, 0);
        }
    }

    dpoe_encrypt_llid_dereg(llid, index);
    if (index > OAM_MAX_LLID_NUM) return;
    g_dpoe_llid_index[index].valid = 0;
    g_dpoe_llid_index[index].llid = 0;
}


/*****************************************************************************/
/* $rtn_hdr_start  handle_dpoe_oams                                          */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void
handle_dpoe_oams(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*           o  frame - Container buffer to store the composed OAM frame     */
/*           o  len - Size of OAM frame                                      */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* API to handler DPoE OAM frame                                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_pdu_dpoe_t *dpoe_frame = (oam_pdu_dpoe_t *)frame;

    OAM_ASSERT(dpoe_frame != NULL || len != 0 );

    g_curr_llid = llid;

    switch (dpoe_frame->opcode) {
        case OAM_PDU_DPOE_VAR_REQ:
            OAM_ORG_LOG("Recv OAM_PDU_DPOE_VAR_REQ \n");
            dpoe_oam_build_var_rsp(llid, frame, len);
            break;

        case OAM_PDU_DPOE_SET_REQ:
            OAM_ORG_LOG("Recv OAM_PDU_DPOE_SET_REQ \n");
            dpoe_oam_build_set_rsp(llid, frame, len);
            break;

        case OAM_PDU_DPOE_IP_MCAST_CTRL: /* 0x05 */
            OAM_ORG_LOG("Recv OAM_PDU_DPOE_IP_MCAST_CTRL \n");
            dpoe_oam_build_mc_ctrl_rsp(llid, frame, len);
            break;

        case OAM_PDU_DPOE_MCAST_REG: /* 0x06 */
            OAM_ORG_LOG("Recv OAM_PDU_DPOE_MCAST_REG \n");
            dpoe_oam_build_mc_reg_rsp(llid, frame, len);
            break;

        case OAM_PDU_DPOE_KEY_EXCHANGE: /* 0x08 */
            OAM_ORG_DEBUG("Recv OAM_PDU_DPOE_KEY_EXCHANGE \n");
            dpoe_oam_handle_key_exchange(llid, frame, len);
            break;

        case OAM_PDU_DPOE_FILE_TRANS:/*0x09*/
            OAM_ORG_DEBUG("Recv OAM_PDU_DPOE_FILE_TRANS \n");
            dpoe_oam_handle_file_trans(llid, frame, len);
            break;

        default:
            OAM_ORG_LOG("Recv Unknow Opcode = %d \n", dpoe_frame->opcode);
            break;
    }

}

void dpoe_oam_build_link_event(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8 event_code)
{
    oam_uint32 len = 0;
    oam_uint8 pdu[DPOE_MAX_OAMPDU_EVENT_LEN];
    oam_dpoe_link_event_tlv_t *p_link_event_oam = NULL;

    oam_if_t *oamif = oam_intf_find_valid(); 

    if(!oamif){
            return;
    }

    memset(pdu, 0, DPOE_MAX_OAMPDU_EVENT_LEN);

    if (!mgmt_obj.valid) {
        OAM_ORG_DEBUG("mgmt_obj is invalid \n");
        return;
    }
   
    oam_build_pdu_hdr(oamif, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_EVENT);
    
    p_link_event_oam = (oam_dpoe_link_event_tlv_t *)(pdu + sizeof(oam_pdu_hdr_t));
    p_link_event_oam->type = 0xFE;
    p_link_event_oam->length = sizeof(oam_dpoe_link_event_tlv_t);
    p_link_event_oam->oui[0] = 0x00;
    p_link_event_oam->oui[1] = 0x10;
    p_link_event_oam->oui[2] = 0x00;
    p_link_event_oam->event_code = event_code;
    p_link_event_oam->raised = TRUE;
    p_link_event_oam->object_type = 0xD6;

    if(DPOE_EVENT_LOS == event_code){
        if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_ONU){
             p_link_event_oam->object_instance = mgmt_obj.obj.llid.value;
        }else if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON){
            p_link_event_oam->object_instance = mgmt_obj.obj.pon.value;
        }else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI){
            p_link_event_oam->object_instance = mgmt_obj.obj.uni.value;
        }else{
            OAM_ORG_DEBUG("Not support object context in DPOE_EVENT_LOS\n");
            return;
        }
    }

    if(DPOE_EVENT_PORT_DISABLED == event_code){
        if(mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON){
            p_link_event_oam->object_instance = mgmt_obj.obj.pon.value;
        }else if (mgmt_obj.obj.llid.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI){
            p_link_event_oam->object_instance = mgmt_obj.obj.uni.value;
        }else{
            OAM_ORG_DEBUG("Not support object context in DPOE_EVENT_PORT_DISABLED\n");
            return;
        }
    }

    if(mgmt_obj.obj.queue.hdr.leaf != OAM_DPOE_LEAF_OBJ_QUEUE){
         len = sizeof(oam_pdu_hdr_t) + sizeof(oam_dpoe_link_event_tlv_t)-2;
    }else{
         len = sizeof(oam_pdu_hdr_t) + sizeof(oam_dpoe_link_event_tlv_t);
    }
 
    dpoe_oam_send(oamif->llid,pdu,len);

}


#endif


