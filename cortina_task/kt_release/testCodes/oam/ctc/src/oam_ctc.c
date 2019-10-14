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

Copyright (c) 2011 by Cortina Systems Incorporated
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CTC_OAM

#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_control.h"
#include "oam_supp.h"
#include "oam_ctc.h"
#include "oam_ctc_pdu.h"
#include "oam_client.h"
#include "oam_mux_par.h"
#include "oam_ctc_alarm.h"
#include "oam_ctc_adapt.h"
#include "oam_ctc_client.h"
#include "oam_ctc_swimage.h"
#include "oam_adapt.h"
#ifdef ZTE_SPECIFIC
#include "oam_zte.h"
#endif/*END_ZTE_SPECIFIC*/
#ifdef HAVE_DB_MANAGMENT
#include "db_instance.h"
#endif

#define CTC_OAM_STATS_GET_TYPE_CURR         0
#define CTC_OAM_STATS_GET_TYPE_HISTORY      1


/*Global variables*/

#if 0
static oam_uint8 g_ctc_image_upgrade_err_msg[6][64] = {
    "Wrong tid",
    "Flash write error",
    "Wrong image file",
    "Wrong upgrade operation",
    "Wrong block or data size",
    "CTC ONU upgrade failed"
};
#endif

ctc_oam_img_upgrade_status g_ctc_upgrade_status;

static g_oam_handle_t g_ctc_oam_vendor_handles;

char ctc_vendor_id[OAM_CTC_VENDOR_ID_LEN] = { 'C', 'O', 'R', 'T' };
char ctc_model_id[OAM_CTC_MODEL_ID_LEN] = { 0x20, '0', '0', '0' };

static oam_var_desc_t g_ctc_oam_odd_sets [] = {
    CTC_OAM_ODD_SET_OAM_LISTS
};

oam_boolean g_auth_fail = FALSE;
oam_boolean g_keep_silence = FALSE;

unsigned char oam_loid_status_get()
{
    return g_keep_silence;
}

extern oam_port_id_t g_oam_port;
extern oam_boolean cgi_get_web_upgrade_status();
extern unsigned int blob_active();
extern oam_boolean current_blob_commited();
oam_boolean ctc_oam_is_odd_set_oam(
    oam_uint8 branch,
    oam_uint16 leaf)
{
    oam_uint32 i;

    for (i = 0 ; i < sizeof(g_ctc_oam_odd_sets) / sizeof(g_ctc_oam_odd_sets[0]);
         i++) {
        if (branch == g_ctc_oam_odd_sets[i].branch
            && leaf == g_ctc_oam_odd_sets[i].leaf) {
            return TRUE;
        }
    }

    return FALSE;
}

void ctc_oam_vendor_std_handler_reg(
    oam_oui_t oui,
    ctc_oam_handle_t *handlers,
    oam_uint32 num)
{
    oam_uint32 i = 0;

    OAM_ASSERT(handlers != NULL);
    OAM_ASSERT(num <= OAM_CTC_MAX_STD_HANDLER);


    memcpy(g_ctc_oam_vendor_handles.oui, oui, sizeof(oam_oui_t));

    for (; i < num; i++) {
        memcpy(&g_ctc_oam_vendor_handles.handlers[i], &handlers[i],
               sizeof(g_ctc_oam_vendor_handles.handlers[i]));
    }

    g_ctc_oam_vendor_handles.num = num;
}

/* CTC Handler Start */

oam_uint32 ctc_oam_eth_admin_state_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_uint8 state;
    oam_status ret = OAM_E_OK;
    oam_var_cont_val_32_t *pData = (oam_var_cont_val_32_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid, valid 0x%x, idxType 0x%x\n", index.valid, index.idxType);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_eth_admin_state_get_adapt(index.idxValue, &state);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("admin state = %d \n", state);

    pData->hdr.width = sizeof(oam_uint32);
    pData->value = state;
    pData->value = htonl(pData->value);

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}


oam_uint32 ctc_oam_eth_auto_admin_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_uint8 state;
    oam_status ret = OAM_E_OK;
    oam_var_cont_val_32_t *pData = (oam_var_cont_val_32_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_eth_auto_admin_get_adapt(index.idxValue, &state);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("admin state = %d \n", state);

    pData->hdr.width = sizeof(oam_uint32);
    pData->value = state;
    pData->value = htonl(pData->value);

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}

oam_uint32 ctc_oam_eth_auto_local_cap_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_auto_neg_tech_t *pData
        = (oam_ctc_onu_auto_neg_tech_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 num = 0, i;
    oam_uint32 value[9];

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_eth_auto_local_cap_get_adapt(
              index.idxValue,
              value,
              &num);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("num = %d \n", num);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("value[%d] = %d \n", i, value[i]);
        pData->value2[i] = htonl(value[i]);
    }

    pData->hdr.width = sizeof(oam_uint32) * (num + 1);
    pData->value1 = htonl(num);

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}



oam_uint32 ctc_oam_eth_auto_advts_cap_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_auto_neg_tech_t *pData
        = (oam_ctc_onu_auto_neg_tech_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 num = 0, i;
    oam_uint32 value[9];

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_eth_auto_advts_cap_get_adapt(
              index.idxValue,
              value,
              &num);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("num = %d \n", num);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("value[%d] = %#x \n", i, value[i]);
        pData->value2[i] = htonl(value[i]);
    }

    pData->hdr.width = sizeof(oam_uint32) * (num + 1);
    pData->value1 = htonl(num);

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}


oam_uint32 ctc_oam_eth_fec_ability_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_var_cont_val_32_t *pData = (oam_var_cont_val_32_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 ability;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_eth_fec_ability_get_adapt(
              &ability);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("fec ability = %d \n", ability);

    pData->hdr.width = sizeof(oam_uint32);

    pData->value = ability;
    pData->value = htonl(pData->value);

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}


oam_uint32  ctc_oam_eth_fec_mode_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pOutHdrContainer
        = (oam_var_cont_t *)pOut;
    oam_var_cont_val_32_t *pData = (oam_var_cont_val_32_t *)pRecv;
    oam_status ret = OAM_E_OK;
    oam_uint32 fecMode;

    OAM_CTC_FUNCTION_ENTER(index);

    fecMode = ntohl(pData->value);

    OAM_ORG_DEBUG( "fec mode = %d \n", fecMode);
    ret = ctc_oam_eth_fec_mode_set_adapt(fecMode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    pOutHdrContainer->width = OAM_CTC_VAR_SET_OK;

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_eth_fec_mode_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_var_cont_val_32_t *pData =  (oam_var_cont_val_32_t *)pOut;
    oam_uint32 fecMode = OAM_INTF_FEC_MODE_EN;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_eth_fec_mode_get_adapt(&fecMode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG( "fec mode = %d \n", fecMode);

    pData->hdr.width = sizeof(oam_uint32);

    pData->value = htonl(fecMode);

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}

oam_uint32 ctc_oam_sn_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_sn_t *pData =
        (oam_ctc_onu_sn_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 ctc_version = 0;
    oam_uint32 pdu_len = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    oam_ctc_version_get(oam_llid_get(), &ctc_version);

    ret = ctc_oam_sn_get_adapt(pData);
    if (ret != OAM_E_OK) 
    {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    if ( ctc_version == 0x30) 
    {
        pData->hdr.width = sizeof(oam_ctc_onu_sn_t) - sizeof(oam_var_cont_t);
        memcpy(
            pData->extend_model + OAM_CTC_EXTEND_MODEL_ID - strlen((char *)pData->model),
            pData->model, 
            strlen((char *)pData->model));
        pdu_len = sizeof(oam_ctc_onu_sn_t);
    }
    else {
        pData->hdr.width = sizeof(oam_ctc_onu_sn_t) - sizeof(oam_var_cont_t) -
                           OAM_CTC_EXTEND_MODEL_ID;
        pdu_len = sizeof(oam_ctc_onu_sn_t) - OAM_CTC_EXTEND_MODEL_ID;
    }

    return pdu_len;
}

oam_uint32 ctc_oam_fw_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_fw_ver_t *pData =
        (oam_ctc_onu_fw_ver_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8           fw_ver[OAM_CTC_ONU_FW_VERSION_LEN];
    
    OAM_CTC_FUNCTION_ENTER(index);

    memset(pData->fwVersion, 0x00, sizeof(pData->fwVersion));
    
    /* return both data and header width */
    ret = ctc_oam_fw_get_adapt(fw_ver, &pData->hdr.width);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    memcpy(pData->fwVersion, fw_ver, OAM_CTC_ONU_FW_VERSION_LEN);

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}

oam_uint32 ctc_oam_chipid_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_chip_id_t *pData =
        (oam_ctc_onu_chip_id_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

#ifdef HAVE_HW_OAM
    oam_uint8 temp[8] = {0x45, 0x36, 0x63, 0x01, 0, 0x6, 0x9, 0x1b};
    pData->hdr.width = 8;
    memcpy(pData->venderId,  temp, 8);
#else

    if (oam_is_cuc_oam()) {
        pData->Date[0] = 11;
        pData->Date[1] = 1;
        pData->Date[2] = 29;
    }
    else {
        ctc_oam_chip_id_get_adapt(pData);
    }

    pData->hdr.width = sizeof(oam_ctc_onu_chip_id_t) - sizeof(pData->hdr);
#endif

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}

oam_uint32 ctc_oam_onucap_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_capabilities_t *pData =
        (oam_ctc_onu_capabilities_t *)pOut;
    oam_status ret = OAM_E_OK;

    oam_ctc_onu_capabilities_t onu_cap;

    memset(&onu_cap, 0, sizeof(onu_cap));

    OAM_CTC_FUNCTION_ENTER(index);

    if (oam_is_cuc_oam()) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    pData->hdr.width = sizeof(oam_ctc_onu_capabilities_t) -
                       sizeof(pData->hdr);

    ret = ctc_oam_onucap_get_adapt(&onu_cap);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    pData->serviceSupported = onu_cap.serviceSupported;
    pData->numGEPorts       = onu_cap.numGEPorts;
    pData->geBitMap         = htonll(onu_cap.geBitMap);
    pData->numFEPorts       = onu_cap.numFEPorts;
    pData->feBitMap         = htonll(onu_cap.feBitMap);
    pData->numPOTSPorts     = onu_cap.numPOTSPorts;
    pData->numE1Ports       = onu_cap.numE1Ports;
    pData->numUSQueues      = onu_cap.numUSQueues;
    pData->maxQueueUSPort   = onu_cap.maxQueueUSPort;
    pData->numDSQueues      = onu_cap.numDSQueues;
    pData->maxQueueDSPort   = onu_cap.maxQueueDSPort;
    pData->BatteryBackup    = onu_cap.BatteryBackup;

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}


oam_uint32 ctc_oam_onu_opmdiag_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_opm_diagnosis_t *pData =
        (oam_ctc_onu_opm_diagnosis_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_int16 temp = 0;
    oam_uint16 volt = 0, tx_bias = 0, tx_power = 0, rx_power = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (oam_is_cuc_oam()) {
        if (!index.valid || index.idxType != OAM_CTC_PON_PORT ) {
            OAM_ORG_LOG("port index is invalid \n");
            pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
    }

    pData->hdr.width = sizeof(oam_ctc_onu_opm_diagnosis_t)
                       - sizeof(pData->hdr);

    ret = ctc_oam_onu_opmdiag_get_adapt(&temp, &volt, &tx_bias, &tx_power,
                                        &rx_power);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("temp = %d, volt = %d, tx_bias = %d, tx_power = %d, rx_power = %d \n",
                  temp, volt, tx_bias, tx_power, rx_power);

    pData->temp     = htons(temp);
    pData->volt     = htons(volt);
    pData->tx_bias  = htons(tx_bias);
    pData->tx_power = htons(tx_power);
    pData->rx_power = htons(rx_power);

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}

oam_uint32 ctc_oam_sla_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_sla_t *pData =
        (oam_ctc_onu_sla_t *)pOut;
    oam_uint8    oper;
    oam_uint8    scheduling_scheme;
    oam_uint8    high_pri_boundary;
    oam_uint32   cycle_len;
    oam_uint8    num_of_service;
    oam_sla_queue_t sla[OAM_CTC_MAX_SLA_NUM];
    oam_status ret = OAM_E_OK;
    oam_uint8 i, len = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_sla_get_adapt(
              &oper, &scheduling_scheme, &high_pri_boundary,
              &cycle_len, &num_of_service, sla);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("oper = %#x, schedule = %#x, pri_boundary = %#x, cycle_len = %#x,num = %#x \n",
                  oper, scheduling_scheme, high_pri_boundary,
                  cycle_len, num_of_service);

    if (num_of_service > OAM_CTC_MAX_SLA_NUM) {
        OAM_ORG_LOG("num_of_service > %d, num_of_service: %d \n", OAM_CTC_MAX_SLA_NUM,
                    num_of_service);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    pData->oper = oper;
    pData->scheduling_scheme = scheduling_scheme;
    pData->high_pri_boundary = high_pri_boundary;
    pData->cycle_len = htonl(cycle_len);
    pData->num_of_service = num_of_service;

    len = sizeof(oam_ctc_onu_sla_t)  - sizeof(oam_sla_queue_t) * OAM_CTC_MAX_SLA_NUM
          -
          sizeof(oam_var_cont_t);

    for (i = 0; i < num_of_service; i++) {
        OAM_ORG_DEBUG("[%d]: Qnum = %#x,fixed_pkt_size = %#x, fixed_band =%#x, \
                grant_band = %#x ,best_effort_band = %#x,wrr_weight = %#x \n",
                      i, sla[i].queue_num, sla[i].fixed_packet_size,
                      sla[i].fixed_bandwidth, sla[i].guaranteed_bandwidth,
                      sla[i].best_effort_bandwith, sla[i].wrr_weigth);

        pData->sla_info[i].queue_num = sla[i].queue_num;
        pData->sla_info[i].fixed_bandwidth = htons(sla[i].fixed_bandwidth);
        pData->sla_info[i].best_effort_bandwith = htons(sla[i].best_effort_bandwith);
        pData->sla_info[i].fixed_packet_size = htons(sla[i].fixed_packet_size);
        pData->sla_info[i].guaranteed_bandwidth = htons(sla[i].guaranteed_bandwidth);
        pData->sla_info[i].wrr_weigth = sla[i].wrr_weigth;
        len += sizeof(oam_sla_queue_t);
    }

    pData->hdr.width = len;

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}


oam_uint32 ctc_oam_onu_sla_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pData = (oam_var_cont_t *)pOut;
    oam_ctc_onu_sla_t *pInData =
        (oam_ctc_onu_sla_t *)pRecv;
    oam_uint8  oper;
    oam_uint8   scheduling_scheme;
    oam_uint8   high_pri_boundary;
    oam_uint32    cycle_len;
    oam_uint8   num_of_service;
    oam_sla_queue_t sla[OAM_CTC_MAX_SLA_NUM];
    oam_uint8 i;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    oper = pInData->oper;
    scheduling_scheme = pInData->scheduling_scheme;
    high_pri_boundary = pInData->high_pri_boundary;
    cycle_len = ntohl(pInData->cycle_len);
    num_of_service = pInData->num_of_service;

    OAM_ORG_DEBUG("oper = %#x, schedule = %#x, pri_boundary = %#x, cycle_len = %#x,num = %#x \n",
                  oper, scheduling_scheme, high_pri_boundary,
                  cycle_len, num_of_service);

    if (num_of_service > OAM_CTC_MAX_SLA_NUM) {
        OAM_ORG_LOG("num_of_service > %d, num_of_service: %d \n", OAM_CTC_MAX_SLA_NUM,
                    num_of_service);
        pData->width = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    for (i = 0; i < num_of_service; i++) {

        sla[i].queue_num = pInData->sla_info[i].queue_num;
        sla[i].fixed_bandwidth = ntohs(pInData->sla_info[i].fixed_bandwidth);
        sla[i].best_effort_bandwith = ntohs(pInData->sla_info[i].best_effort_bandwith);
        sla[i].fixed_packet_size = ntohs(pInData->sla_info[i].fixed_packet_size);
        sla[i].guaranteed_bandwidth = ntohs(pInData->sla_info[i].guaranteed_bandwidth);
        sla[i].wrr_weigth = sla[i].wrr_weigth;

        OAM_ORG_DEBUG("[%d]: Qnum = %#x,fixed_pkt_size = %#x, fixed_band =%#x, \
                grant_band = %#x ,best_effort_band = %#x,wrr_weight = %#x \n",
                      i, sla[i].queue_num, sla[i].fixed_packet_size,
                      sla[i].fixed_bandwidth, sla[i].guaranteed_bandwidth,
                      sla[i].best_effort_bandwith, sla[i].wrr_weigth);
    }

    ret = ctc_oam_sla_set_adapt(
              oper, scheduling_scheme, high_pri_boundary,
              cycle_len, num_of_service, sla);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG_MIN("set adapt return failed, ret = %d \n", ret);
        pData->width = OAM_CTC_VAR_SET_BAD_PARAM;
    }
    else {
        pData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onucap2_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_capabilities2_t *pData =
        (oam_ctc_onu_capabilities2_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_capabilities2_t  onu_cap2;
    oam_uint8 *battery_backup = NULL;
    oam_uint16 i = 0;

    memset(&onu_cap2, 0, sizeof(onu_cap2));

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onucap2_get_adapt(&onu_cap2);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    if (0 == onu_cap2.numInterface ||
        onu_cap2.numInterface > CTC_OAM_MAX_INTF_NUM) {
        OAM_ORG_LOG("invalid numInterface %d.\n", onu_cap2.numInterface);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    pData->onuType          = htonl(onu_cap2.onuType);
    pData->multillid        = onu_cap2.multillid;
    pData->protectionType   = onu_cap2.protectionType;
    pData->numPONIf         = onu_cap2.numPONIf;
    pData->numSlot          = onu_cap2.numSlot;
    pData->numInterface     = onu_cap2.numInterface;

    for (i = 0; i < pData->numInterface && i < CTC_OAM_MAX_INTF_NUM; i++) {
        pData->intf[i].interfaceType = htonl(onu_cap2.intf[i].interfaceType);
        pData->intf[i].numPorts      = htons(onu_cap2.intf[i].numPorts);
    }

    battery_backup = (oam_uint8 *)((oam_ctc_onu_interface_t *)&pData->intf[0] +
                                   pData->numInterface);
    *battery_backup = onu_cap2.BatteryBackup;

    pData->hdr.width = sizeof(oam_ctc_onu_capabilities2_t) - sizeof(
                           oam_var_cont_t) -
                       (CTC_OAM_MAX_INTF_NUM - pData->numInterface) * sizeof(oam_ctc_onu_interface_t);

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}


oam_uint32 ctc_oam_holdover_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_holdover_t *pData =
        (oam_ctc_onu_holdover_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint32 hState, hTime;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_holdover_get_adapt(&hState, &hTime);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("state = %d , time = %d \n", hState, hTime);

    pData->holdover_state = htonl(hState);
    pData->holdover_time = htonl(hTime);

    pData->hdr.width = sizeof(oam_ctc_onu_holdover_t)
                       - sizeof(oam_var_cont_t);

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

oam_uint32 ctc_oam_holdover_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)(pOut);
    oam_ctc_onu_holdover_t *pInData
        = (oam_ctc_onu_holdover_t *)pRecv;
    oam_status ret = OAM_E_OK;
    oam_uint32 hState, hTime;

    OAM_CTC_FUNCTION_ENTER(index);

    hState = ntohl(pInData->holdover_state);
    hTime = ntohl(pInData->holdover_time);

    OAM_ORG_DEBUG("state = %d , time = %d \n", hState, hTime);

    ret = ctc_oam_holdover_set_adapt(hState, hTime);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_mgmt_config_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_mgmt_config_t *pData =
        (oam_ctc_onu_mgmt_config_t *)pOut;
    oam_ctc_onu_mgmt_config_v6_t *pDataV6 =
        (oam_ctc_onu_mgmt_config_v6_t *)pOut;
    ctc_oam_mgmt_parm_t mgmt;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onu_mgmt_config_get_adapt(&mgmt);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("mgmt ip type = %d , ipv4 = %#x ,mask = %#x\n",
                  mgmt.mgmt_ip.ipver, mgmt.mgmt_ip.addr.ipv4, mgmt.mask);

    OAM_ORG_DEBUG("mgmt gate type = %d , ipv4 = %#x ,\n",
                  mgmt.mgmt_gateway.ipver, mgmt.mgmt_gateway.addr.ipv4);

    OAM_ORG_DEBUG("svlan = %#x, cvlan = %#x, data_pri = %#x \n",
                  mgmt.mgmt_svlan, mgmt.mgmt_cvlan, mgmt.mgmt_data_pri);

    if (mgmt.mgmt_ip.ipver == OAM_IPV4
        && mgmt.mgmt_gateway.ipver == OAM_IPV4) {
        pData->ip_addr = htonl(mgmt.mgmt_ip.addr.ipv4);
        pData->ip_mask = htonl(mgmt.mask);
        pData->gateway = htonl(mgmt.mgmt_gateway.addr.ipv4);
        pData->svlan = htons(mgmt.mgmt_svlan);
        pData->cvlan = htons(mgmt.mgmt_cvlan);
        pData->priority = htons(mgmt.mgmt_data_pri);

        pData->hdr.width = sizeof(oam_ctc_onu_mgmt_config_t)
                           - sizeof(oam_var_cont_t);
    }
    else if (mgmt.mgmt_ip.ipver == OAM_IPV6
             && mgmt.mgmt_gateway.ipver == OAM_IPV6) {
        memcpy(pDataV6->ip_addr, mgmt.mgmt_ip.addr.ipv6, 16);
        memcpy(pDataV6->gateway, mgmt.mgmt_gateway.addr.ipv6, 16);
        mgmt.mask = htonl(mgmt.mask);
        memcpy(pDataV6->ip_mask, (oam_uint8 *)&mgmt.mask, 4);
        pDataV6->svlan = htons(mgmt.mgmt_svlan);
        pDataV6->cvlan = htons(mgmt.mgmt_cvlan);
        pDataV6->priority = htons(mgmt.mgmt_data_pri);
        pData->hdr.width = sizeof(oam_ctc_onu_mgmt_config_v6_t)
                           - sizeof(oam_var_cont_t);
    }
    else {
        OAM_ORG_LOG("Ip type chaoes \n");
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    return sizeof(oam_var_cont_t) + pData->hdr.width;

}


oam_uint32 ctc_oam_onu_mgmt_config_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_mgmt_config_t *pInData =
        (oam_ctc_onu_mgmt_config_t *)pRecv;
    oam_ctc_onu_mgmt_config_v6_t *pInDataV6 =
        (oam_ctc_onu_mgmt_config_v6_t *)pRecv;
    ctc_oam_mgmt_parm_t mgmt;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);
    memset(&mgmt, 0, sizeof(mgmt));

    if (pInData->hdr.width ==
        ( sizeof(oam_ctc_onu_mgmt_config_t)
          - sizeof(oam_var_cont_t))) {
        /* Ipv4 */
        mgmt.mgmt_ip.ipver = OAM_IPV4;
        mgmt.mgmt_ip.addr.ipv4 = ntohl(pInData->ip_addr);
        mgmt.mask = ntohl(pInData->ip_mask);

        mgmt.mgmt_gateway.ipver = OAM_IPV4;
        mgmt.mgmt_gateway.addr.ipv4 = ntohl(pInData->gateway);

        mgmt.mgmt_svlan = ntohs(pInData->svlan);
        mgmt.mgmt_cvlan = ntohs(pInData->cvlan);
        mgmt.mgmt_data_pri = pInData->priority;
    }
    else if (pInData->hdr.width ==
             ( sizeof(oam_ctc_onu_mgmt_config_v6_t)
               - sizeof(oam_var_cont_t))) {
        /* Ipv6 */
        mgmt.mgmt_ip.ipver = OAM_IPV6;
        memcpy(mgmt.mgmt_ip.addr.ipv6, pInDataV6->ip_addr, 16);

        memcpy((oam_uint8 *)&mgmt.mask, pInDataV6->ip_mask, 4);
        mgmt.mask = ntohl(mgmt.mask);

        mgmt.mgmt_gateway.ipver = OAM_IPV6;
        memcpy(mgmt.mgmt_gateway.addr.ipv6, pInDataV6->gateway, 16);

        mgmt.mgmt_svlan = ntohs(pInDataV6->svlan);
        mgmt.mgmt_cvlan = ntohs(pInDataV6->cvlan);
        mgmt.mgmt_data_pri = pInDataV6->priority;
    }
    else {
        OAM_ORG_LOG("tlv length wrong, len = %d \n", pInData->hdr.width );
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("mgmt ip type = %d , ipv4 = %#x ,mask = %#x\n",
                  mgmt.mgmt_ip.ipver, mgmt.mgmt_ip.addr.ipv4, mgmt.mask);

    OAM_ORG_DEBUG("mgmt gate type = %d , ipv4 = %#x ,\n",
                  mgmt.mgmt_gateway.ipver, mgmt.mgmt_gateway.addr.ipv4);

    OAM_ORG_DEBUG("svlan = %#x, cvlan = %#x, data_pri = %#x \n",
                  mgmt.mgmt_svlan, mgmt.mgmt_cvlan, mgmt.mgmt_data_pri);

    ret = ctc_oam_onu_mgmt_config_set_adapt(&mgmt);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}



oam_uint32 ctc_oam_onu_active_pon_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_active_pon_t *pData =
        (oam_ctc_onu_active_pon_t *)pOut;
    oam_uint8 actPonPort = 1;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onu_active_pon_get_adapt(&actPonPort);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("pon port = %d \n", actPonPort);

    pData->pon_port = actPonPort;
    pData->hdr.width = sizeof(oam_uint8);

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

oam_uint32 ctc_oam_onu_active_pon_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_active_pon_t *pInData  =
        (oam_ctc_onu_active_pon_t *)pRecv;
    oam_status ret = OAM_E_OK;
    oam_uint8 actPonPort;

    OAM_CTC_FUNCTION_ENTER(index);
    actPonPort = pInData->pon_port;
    OAM_ORG_DEBUG("pon port = %d \n", actPonPort);

    ret = ctc_oam_onu_active_pon_set_adapt(actPonPort);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_VAR_SET_NO_RES;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onucap3_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_capabilities3_t *pData =
        (oam_ctc_onu_capabilities3_t *)pOut;
    oam_ctc_onu_capabilities3_t onu_cap3;

    memset(&onu_cap3, 0, sizeof(onu_cap3));

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onucap3_get_adapt(&onu_cap3);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    pData->ipv6Support  = onu_cap3.ipv6Support;
    pData->powerControl = onu_cap3.powerControl;
    pData->serviceSla   = onu_cap3.serviceSla;

    OAM_ORG_DEBUG("ipv6 = %d, power = %d, sla = %d \n",
                  pData->ipv6Support, pData->powerControl, pData->serviceSla);

    pData->hdr.width = sizeof(oam_ctc_onu_capabilities3_t)
                       - sizeof(oam_var_cont_t);

    if (oam_is_cuc_oam()) {
        pData->hdr.width -= sizeof(pData->serviceSla);
    }

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

oam_uint32 ctc_oam_onu_power_saving_cap_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_power_saving_cap_t *pData =
        (oam_ctc_onu_power_saving_cap_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 sleep_mode = CTC_OAM_SLEEP_MODE_NOT_SUPP;
    oam_uint8 early_wakeup = CTC_OAM_EARLY_WAKEUP_NOT_SUPP;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onu_power_saving_cap_get_adapt(&sleep_mode, &early_wakeup);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("sleep_mode = %d, early_wakeup = %d \n",
                  sleep_mode, early_wakeup);

    pData->hdr.width = sizeof(oam_ctc_onu_power_saving_cap_t)
                       - sizeof(oam_var_cont_t);
    pData->sleep_mode_cap = sleep_mode;
    pData->early_wakeup_cap = early_wakeup;

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

oam_uint32 ctc_oam_onu_power_saving_config_get (
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_power_saving_config_t *pData =
        (oam_ctc_onu_power_saving_config_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 early_wakeup = CTC_OAM_EARLY_WAKEUP_DISABLE;
    oam_uint64 max_sleep_duration = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onu_power_saving_config_get_adapt(&early_wakeup,
            &max_sleep_duration);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG(" early_wakeup = %d ,max_sleep_duration = %#x \n",
                  early_wakeup, max_sleep_duration);


    pData->hdr.width = sizeof(oam_ctc_onu_power_saving_config_t)
                       - sizeof(oam_var_cont_t);
#if 0
    pData->max_sleep_duration = htonl(max_sleep_duration);
#endif
    pData->max_sleep_duration[5] = (oam_uint8)max_sleep_duration & 0xff;
    pData->max_sleep_duration[4] = (oam_uint8)((max_sleep_duration >> 8) & 0xff);
    pData->max_sleep_duration[3] = (oam_uint8)((max_sleep_duration >> 16) & 0xff);
    pData->max_sleep_duration[2] = (oam_uint8)((max_sleep_duration >> 24) & 0xff);
    pData->max_sleep_duration[1] = (oam_uint8)((max_sleep_duration >> 32) & 0xff);
    pData->max_sleep_duration[0] = (oam_uint8)((max_sleep_duration >> 40) & 0xff);

    pData->early_wakeup = early_wakeup;

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}


oam_uint32 ctc_oam_onu_power_saving_config_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_power_saving_config_t *pInData  =
        (oam_ctc_onu_power_saving_config_t *)pRecv;
    oam_status ret = OAM_E_OK;
    oam_uint8 early_wakeup = CTC_OAM_EARLY_WAKEUP_DISABLE;
    oam_uint64 max_sleep_duration = 0;

    OAM_CTC_FUNCTION_ENTER(index);

#if 0
    max_sleep_duration = ntohl(pInData->max_sleep_duration);
#endif
    early_wakeup = pInData->early_wakeup;
    oam_printf("%x%x%x%x%x%x \n", pInData->max_sleep_duration[0],
               pInData->max_sleep_duration[1], pInData->max_sleep_duration[2],
               pInData->max_sleep_duration[3], pInData->max_sleep_duration[4],
               pInData->max_sleep_duration[5]);

    max_sleep_duration = (oam_uint64)pInData->max_sleep_duration[0] << 40 |
                         (oam_uint64)pInData->max_sleep_duration[1] << 32 |
                         (oam_uint64)pInData->max_sleep_duration[2] << 24 | (oam_uint64)
                         pInData->max_sleep_duration[3] << 16 |
                         (oam_uint64)pInData->max_sleep_duration[4] << 8 | (oam_uint64)
                         pInData->max_sleep_duration[5];
    OAM_ORG_DEBUG(" early_wakeup = %d ,max_sleep_duration = %#llx \n",
                  early_wakeup, max_sleep_duration);

    ret = ctc_oam_onu_power_saving_config_set_adapt(early_wakeup,
            max_sleep_duration);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}


oam_uint32 ctc_oam_eth_mac_aging_time_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_mac_aging_time_t *pData =
        (oam_ctc_onu_mac_aging_time_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint32 aging_time = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_eth_mac_aging_time_get_adapt(&aging_time);


    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("aging time = %#x \n",
                  aging_time);

    pData->time = htonl(aging_time);
    pData->hdr.width = sizeof(oam_ctc_onu_mac_aging_time_t)
                       - sizeof(oam_var_cont_t);

    return  sizeof(oam_var_cont_t) + pData->hdr.width;
}


oam_uint32 ctc_oam_eth_mac_aging_time_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_mac_aging_time_t *pInData  =
        (oam_ctc_onu_mac_aging_time_t *)pRecv;
    oam_uint32 aging_time = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (pInData->hdr.width != sizeof(aging_time)) {
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    aging_time = ntohl(pInData->time);

    OAM_ORG_DEBUG("aging time = %#x \n", aging_time);

    ret = ctc_oam_eth_mac_aging_time_set_adapt(aging_time);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}


oam_uint32 ctc_oam_onu_0x000e_config_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    if (oam_is_cuc_oam()) {
        return ctc_oam_eth_mac_aging_time_get(index, pRecV, procRecvLen, pOut);
    }
    else {
        return ctc_oam_onu_power_saving_config_get(index, pRecV, procRecvLen, pOut);
    }
}
oam_uint32 ctc_oam_onu_0x000e_config_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    if (oam_is_cuc_oam()) {
        return ctc_oam_eth_mac_aging_time_set(index, pRecv, Recvlen, pOut);
    }
    else {
        return  ctc_oam_onu_power_saving_config_set(index, pRecv, Recvlen, pOut);
    }

}
oam_uint32 ctc_oam_onu_pon_protect_parm_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_pon_protect_parm_t *pData =
        (oam_ctc_onu_pon_protect_parm_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint16  opt_time;
    oam_uint16  mpcp_time;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onu_pon_protect_parm_get_adapt(&opt_time, &mpcp_time);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG(" opt time = %#x ,mpcp time = %#x \n",
                  opt_time, mpcp_time);


    pData->hdr.width = sizeof(oam_ctc_onu_pon_protect_parm_t)
                       - sizeof(oam_var_cont_t);
    pData->los_optical_time = htons(opt_time);
    pData->los_mpcp_time = htons(mpcp_time);

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

oam_uint32 ctc_oam_onu_pon_protect_parm_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_pon_protect_parm_t *pInData  =
        (oam_ctc_onu_pon_protect_parm_t *)pRecv;
    oam_status ret = OAM_E_OK;
    oam_uint16  opt_time;
    oam_uint16  mpcp_time;

    OAM_CTC_FUNCTION_ENTER(index);

    opt_time = ntohs(pInData->los_optical_time);
    mpcp_time = ntohs(pInData->los_mpcp_time);

    OAM_ORG_DEBUG(" opt time = %#x ,mpcp time = %#x",
                  opt_time, mpcp_time);

    ret = ctc_oam_onu_pon_protect_parm_set_adapt(opt_time, mpcp_time);


    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}



oam_uint32 ctc_oam_onu_cfg_counter_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_cfg_counter_t *pData =
        (oam_ctc_onu_cfg_counter_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint32  cfg_counter;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onu_cfg_counter_get_adapt(&cfg_counter);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG(" cfg counter = %#x\n",
                  cfg_counter);


    pData->hdr.width = sizeof(oam_ctc_onu_pon_protect_parm_t)
                       - sizeof(oam_var_cont_t);
    pData->cfg_counter = htonl(cfg_counter);

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

oam_uint32 ctc_oam_onu_cfg_counter_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_cfg_counter_t *pInData  =
        (oam_ctc_onu_cfg_counter_t *)pRecv;
    oam_status ret = OAM_E_OK;
    oam_uint32  cfg_counter;


    OAM_CTC_FUNCTION_ENTER(index);

    cfg_counter = ntohl(pInData->cfg_counter);

    OAM_ORG_DEBUG(" cfg counter = %#x\n",
                  cfg_counter);

    ret = ctc_oam_onu_cfg_counter_set_adapt(cfg_counter);


    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_0x000f_config_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    if (oam_is_cuc_oam()) {
        return ctc_oam_onu_cfg_counter_get(index, pRecV, procRecvLen, pOut);
    }
    else {
        return  ctc_oam_onu_pon_protect_parm_get(index, pRecV, procRecvLen, pOut);
    }
}
oam_uint32 ctc_oam_onu_0x000f_config_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    if (oam_is_cuc_oam()) {
        return ctc_oam_onu_cfg_counter_set(index, pRecv, Recvlen, pOut);
    }
    else {
        return  ctc_oam_onu_pon_protect_parm_set(index, pRecv, Recvlen, pOut);
    }

}

oam_uint32 ctc_oam_eth_port_link_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_uint8 state;
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_link_state_t *pData =
        (oam_ctc_onu_link_state_t *) pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_eth_port_link_get_adapt(index.idxValue, &state);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("Link state = %d \n", state);

    pData->hdr.width = sizeof(oam_uint8);
    pData->linkState = state;

    return sizeof(oam_ctc_onu_link_state_t);
}



oam_uint32 ctc_oam_eth_port_pause_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_uint8 flowControl;
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_eth_pause_t *pData =
        (oam_ctc_onu_eth_pause_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }


    ret = ctc_oam_eth_port_pause_get_adapt(index.idxValue, &flowControl);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("flow control = %d \n", flowControl);

    pData->hdr.width = sizeof(oam_uint8);
    pData->portOper = flowControl;

    return sizeof(oam_ctc_onu_eth_pause_t);
}

oam_uint32 ctc_oam_eth_port_pause_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_uint8 flowControl;
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_eth_pause_t *pInData  =
        (oam_ctc_onu_eth_pause_t *)pRecv;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    flowControl = pInData->portOper;
    OAM_ORG_DEBUG("flow control = %d \n", flowControl);

    ret = ctc_oam_eth_port_pause_set_adapt(index.idxValue, flowControl);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);

}


oam_uint32 ctc_oam_eth_port_policing_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{

    oam_ctc_onu_port_policing_t *pData =
        (oam_ctc_onu_port_policing_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 oper = CTC_PORT_POLICY_OPER_DISABLE;
    oam_uint32 cir = 0, cbs = 0 , ebs = 0;


    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_eth_port_policing_get_adapt(index.idxValue, &oper, &cir, &cbs,
            &ebs);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }


    OAM_ORG_DEBUG("oper = %d ,cir = %#x ,cbs = %#x,ebs = %#x \n",
                  oper, cir, cbs, ebs);

    if (CTC_PORT_POLICY_OPER_ENBALE == oper) {
        pData->hdr.width = sizeof(oam_ctc_onu_port_policing_t)
                           - sizeof(oam_var_cont_t);
        pData->portCIR[0] = (cir >> 16) & 0xff;
        pData->portCIR[1] = (cir >> 8) & 0xff;
        pData->portCIR[2] = (cir) & 0xff;

        pData->portCBS[0] = (cbs >> 16) & 0xff;
        pData->portCBS[1] = (cbs >> 8) & 0xff;
        pData->portCBS[2] = (cbs) & 0xff;

        pData->portEBS[0] = (ebs >> 16) & 0xff;
        pData->portEBS[1] = (ebs >> 8) & 0xff;
        pData->portEBS[2] = (ebs) & 0xff;
    }
    else {
        pData->hdr.width = sizeof(oam_uint8);
    }

    pData->portOper = oper;


    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

oam_uint32 ctc_oam_eth_port_policing_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_port_policing_t *pInData  =
        (oam_ctc_onu_port_policing_t *)pRecv;
    oam_uint8 oper = CTC_PORT_POLICY_OPER_DISABLE;
    oam_uint32 cir = 0, cbs = 0, ebs = 0;


    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    oper = pInData->portOper;

    if (oper != CTC_PORT_POLICY_OPER_DISABLE) {
        cir = pInData->portCIR[2] | (pInData->portCIR[1] << 8) |
              (pInData->portCIR[0] << 16);
        cbs = pInData->portCBS[2] | (pInData->portCBS[1] << 8) |
              (pInData->portCBS[0] << 16);
        ebs = pInData->portEBS[2] | (pInData->portEBS[1] << 8) |
              (pInData->portEBS[0] << 16);
    }

    OAM_ORG_DEBUG("oper = %d ,cir = %#x ,cbs = %#x,ebs = %#x \n",
                  oper, cir, cbs, ebs);

    ret = ctc_oam_eth_port_policing_set_adapt(index.idxValue, oper, cir, cbs, ebs);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_eth_ds_rate_limit_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_ds_rate_limit_t *pData =
        (oam_ctc_onu_ds_rate_limit_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 oper = CTC_PORT_DS_RATE_LIMIT_DISABLE;
    oam_uint32 cir = 0, pir = 0;


    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_eth_ds_rate_limit_get_adapt(index.idxValue, &oper, &cir, &pir);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("oper = %d ,cir = %#x ,pir = %#x \n",
                  oper, cir, pir);

    if (CTC_PORT_DS_RATE_LIMIT_ENABLE == oper) {
        pData->hdr.width = sizeof(oam_ctc_onu_ds_rate_limit_t)
                           - sizeof(oam_var_cont_t);
        pData->portCIR[0] = (cir >> 16) & 0xff;
        pData->portCIR[1] = (cir >> 8) & 0xff;
        pData->portCIR[2] = (cir) & 0xff;

        pData->portPIR[0] = (pir >> 16) & 0xff;
        pData->portPIR[1] = (pir >> 8) & 0xff;
        pData->portPIR[2] = (pir) & 0xff;
    }
    else {
        pData->hdr.width = sizeof(oam_uint8);
    }

    pData->portOper = oper;

    return sizeof(oam_var_cont_t) + pData->hdr.width;

}

oam_uint32 ctc_oam_eth_ds_rate_limit_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_ds_rate_limit_t *pInData  =
        (oam_ctc_onu_ds_rate_limit_t *)pRecv;
    oam_uint8 oper = CTC_PORT_DS_RATE_LIMIT_DISABLE;
    oam_uint32 cir = 0, pir = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    oper = pInData->portOper;

    if (oper != CTC_PORT_DS_RATE_LIMIT_DISABLE) {
        cir = pInData->portCIR[2] | (pInData->portCIR[1] << 8) |
              (pInData->portCIR[0] << 16);
        pir = pInData->portPIR[2] | (pInData->portPIR[1] << 8) |
              (pInData->portPIR[0] << 16);
    }

    OAM_ORG_DEBUG("oper = %d ,cir = %#x ,pir = %#x \n",
                  oper, cir, pir);

    ret = ctc_oam_eth_ds_rate_limit_set_adapt(index.idxValue, oper, cir, pir);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);

}

oam_uint32 ctc_oam_loop_detect_status_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_loopdetect_t *pData =
        (oam_ctc_onu_loopdetect_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint32 status = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_loop_detect_status_get_adapt(index.idxValue, &status);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("status = %d\n", status);

    pData->hdr.width = sizeof(oam_uint32);
    pData->status = htonl(status);

    return sizeof(oam_var_cont_t) + pData->hdr.width;

}


oam_uint32 ctc_oam_loop_detect_status_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_loopdetect_t *pInData  =
        (oam_ctc_onu_loopdetect_t *)pRecv;
    oam_uint32 status = OAM_CTC_ONU_LOOPDETECT_DEACTIVED;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    status = ntohl(pInData->status);

    OAM_ORG_DEBUG("loopdect status = %d \n", status);

    ret = ctc_oam_loop_detect_status_set_adapt(index.idxValue, status);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_disable_looped_status_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_disable_loopded_t *pInData  =
        (oam_ctc_onu_disable_loopded_t *)pRecv;
    oam_uint32 status = OAM_CTC_ONU_DISABLE_LOOPED_DEACTIVED;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    status = ntohl(pInData->status);

    OAM_ORG_DEBUG("disable looped status = %d \n", status);

    ret = ctc_oam_disable_looped_status_set_adapt(index.idxValue, status);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_mac_limit_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_mac_limit_t *pData =
        (oam_ctc_onu_mac_limit_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint16 mac_limit = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!oam_is_cuc_oam()) {
        OAM_ORG_LOG("CTC OAM, not support it \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }


    ret = ctc_oam_onu_mac_limit_get_adapt(index.idxValue, &mac_limit);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("mac limit = %d \n", mac_limit);

    pData->hdr.width = sizeof(oam_uint16);
    pData->mac_limit = htons(mac_limit);

    return sizeof(oam_var_cont_t) + pData->hdr.width;

}


oam_uint32 ctc_oam_onu_mac_limit_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_mac_limit_t *pInData  =
        (oam_ctc_onu_mac_limit_t *)pRecv;
    oam_uint32 mac_limit = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!oam_is_cuc_oam()) {
        OAM_ORG_LOG("CTC OAM, not support it \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }


    mac_limit = ntohs(pInData->mac_limit);

    OAM_ORG_DEBUG("mac limit = %d \n", mac_limit);

    ret = ctc_oam_onu_mac_limit_set_adapt(index.idxValue, mac_limit);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}



oam_uint32 ctc_oam_onu_port_mode_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_port_mode_t *pData =
        (oam_ctc_onu_port_mode_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 port_mode = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!oam_is_cuc_oam()) {
        OAM_ORG_LOG("CTC OAM, not support it \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }


    ret = ctc_oam_onu_port_mode_get_adapt(index.idxValue, &port_mode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("port mode = %d \n", port_mode);

    pData->hdr.width = sizeof(pData->port_mode);
    pData->port_mode = port_mode;

    return sizeof(oam_var_cont_t) + pData->hdr.width;

}


oam_uint32 ctc_oam_onu_port_mode_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_port_mode_t *pInData  =
        (oam_ctc_onu_port_mode_t *)pRecv;
    oam_uint8 port_mode = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!oam_is_cuc_oam()) {
        OAM_ORG_LOG("CTC OAM, not support it \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }


    port_mode = pInData->port_mode;

    OAM_ORG_DEBUG("port mode  = %d \n", port_mode);

    ret = ctc_oam_onu_port_mode_set_adapt(index.idxValue, port_mode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}


oam_uint32 ctc_oam_onu_port_work_mode_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_port_mode_t *pData =
        (oam_ctc_onu_port_mode_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 port_mode = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!oam_is_cuc_oam()) {
        OAM_ORG_LOG("CTC OAM, not support it \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }


    ret = ctc_oam_onu_port_work_mode_get_adapt(index.idxValue, &port_mode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("port mode = %d \n", port_mode);

    pData->hdr.width = sizeof(pData->port_mode);
    pData->port_mode = port_mode;

    return sizeof(oam_var_cont_t) + pData->hdr.width;

}


oam_uint32 ctc_oam_onu_port_stats_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_eth_port_stats_t *pData =
        (oam_ctc_onu_eth_port_stats_t *)pOut;
    oam_ctc_eth_port_stats_t *pStats = &(pData->stats);
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!oam_is_cuc_oam()) {
        OAM_ORG_LOG("CTC OAM, not support it \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }


    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_onu_port_stats_get_adapt(index.idxValue, pStats);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    pStats->in_octets = htonll(pStats->in_octets);
    pStats->in_ucast_pkt = htonll(pStats->in_ucast_pkt);
    pStats->in_nucast_pkt = htonll(pStats->in_nucast_pkt);
    pStats->in_discard = htonll(pStats->in_discard);
    pStats->in_error = htonll(pStats->in_error);
    pStats->out_octets = htonll(pStats->out_octets);
    pStats->out_ucast_pkt = htonll(pStats->out_ucast_pkt);
    pStats->out_nucast_pkt = htonll(pStats->out_nucast_pkt);
    pStats->out_discard = htonll(pStats->out_discard);
    pStats->out_error = htonll(pStats->out_error);

    pData->hdr.width = sizeof(oam_ctc_eth_port_stats_t);



    return sizeof(oam_var_cont_t) + pData->hdr.width;

}



oam_uint32 ctc_oam_pon_port_stat_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_var_desc_t *pInDesc = (oam_var_desc_t *)pRecv;
    oam_var_cont_t *pOutCont = (oam_var_cont_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!oam_is_cuc_oam()) {
        OAM_ORG_LOG("CTC OAM, not support it \n");
        pOutCont->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (index.valid) {
        if (index.idxValue != 0) {
            OAM_ORG_LOG("port index is invalid \n");
            pOutCont->width  = OAM_CTC_VAR_SET_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
    }

    if (ntohs(pInDesc->leaf) == OAM_CTC_LEAF_PON_PORT_STAT1) {
        oam_ctc_onu_llid_stats1 *stats1 = (oam_ctc_onu_llid_stats1 *)pOut;
        ctc_oam_pon_port_stat_get_adapt(stats1, NULL);
        stats1->out_pkts = htonll(stats1->out_pkts);
        stats1->out_octets = htonll(stats1->out_octets);
        stats1->out_mc_pkts = htonll(stats1->out_mc_pkts);
        stats1->out_bc_pkts = htonll(stats1->out_bc_pkts);
        stats1->in_pkts = htonll(stats1->in_pkts);
        stats1->in_octets = htonll(stats1->in_octets);
        stats1->in_mc_pkts = htonll(stats1->in_mc_pkts);
        stats1->in_bc_pkts = htonll(stats1->in_bc_pkts);

        stats1->hdr.width = sizeof(oam_ctc_onu_llid_stats1) - sizeof(oam_var_cont_t);

        return sizeof(oam_ctc_onu_llid_stats1);
    }
    else {
        oam_ctc_onu_llid_stats2 *stats2 = (oam_ctc_onu_llid_stats2 *)pOut;
        ctc_oam_pon_port_stat_get_adapt(NULL, stats2);
        stats2->in_crc8_err_pkts = htonll(stats2->in_crc8_err_pkts);
        stats2->in_fec_crct_blks = htonll(stats2->in_fec_crct_blks);
        stats2->in_fec_uncrct_blks = htonll(stats2->in_fec_uncrct_blks);
        stats2->out_mpcp_pkts = htonll(stats2->out_mpcp_pkts);
        stats2->in_mpcp_pkts = htonll(stats2->in_mpcp_pkts);
        stats2->out_mpcp_reg_pkts = htonll(stats2->out_mpcp_reg_pkts);
        stats2->out_mpcp_rreq_pkts = htonll(stats2->out_mpcp_rreq_pkts);
        stats2->out_mpcp_rpt_pkts = htonll(stats2->out_mpcp_rpt_pkts);
        stats2->in_mpcp_gate_pkts = htonll(stats2->in_mpcp_gate_pkts);
        stats2->in_mpcp_reg_pkts = htonll(stats2->in_mpcp_reg_pkts);

        stats2->hdr.width = sizeof(oam_ctc_onu_llid_stats2) - sizeof(oam_var_cont_t);
        return sizeof(oam_ctc_onu_llid_stats2);
    }

}


oam_status ctc_oam_onu_default_vlan_tag_get(
    oam_port_id_t port,
    oam_uint8 *data
)
{
    oam_ctc_onu_vlan_tag_t *defVlan =
        (oam_ctc_onu_vlan_tag_t *)data;
    oam_uint16 tpid = 0 , tag = 0 ;
    oam_status ret = OAM_E_OK;

    ret = ctc_oam_onu_default_vlan_tag_get_adapt(port, &tpid, &tag);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        return ret;
    }

    OAM_ORG_DEBUG("default tip = %#x ,tag = %#x\n", tpid, tag);

    defVlan->tpid = htons(tpid);
    defVlan->tag = htons(tag);

    return OAM_E_OK;
}

oam_status ctc_oam_onu_translate_vlan_get(
    oam_port_id_t port,
    oam_uint32 *num,
    oam_uint8 *data
)
{
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_vlan_xlate_entry vlan[CTC_VLAN_MODE_TRANSLATE_MAX];
    oam_ctc_onu_vlan_xlate_entry *pData =
        (oam_ctc_onu_vlan_xlate_entry *) data;
    oam_uint8 i = 0;
    oam_uint16 translate_num;

    ret = ctc_oam_onu_translate_vlan_get_adapt(port, &translate_num, vlan);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        return ret;
    }

    OAM_ORG_DEBUG("translate num = %d \n", translate_num);

    if (translate_num > CTC_VLAN_MODE_TRANSLATE_MAX) {
        OAM_ORG_LOG("translate number %d is more the max %d ",
                    translate_num, CTC_VLAN_MODE_TRANSLATE_MAX);
        return OAM_E_PARAM;
    }

    for (i = 0; i < translate_num; i++) {
        OAM_ORG_DEBUG("translate [%d]:tpid %#x, tag %#x --> tpid %#x, tag %#x \n",
                      i, vlan[i].tag1.tpid, vlan[i].tag1.tag, vlan[i].tag2.tpid,
                      vlan[i].tag2.tag);
        pData->tag1.tpid = htons(vlan[i].tag1.tpid);
        pData->tag2.tpid = htons(vlan[i].tag2.tpid);
        pData->tag1.tag = htons(vlan[i].tag1.tag);
        pData->tag2.tag = htons(vlan[i].tag2.tag);
        pData ++;
    }

    *num = translate_num;
    return OAM_E_OK;
}
oam_status ctc_oam_onu_translate_vlan_set(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defVlan,
    oam_uint8 *data,
    oam_uint32 len)
{
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_vlan_xlate_entry vlan[CTC_VLAN_MODE_TRANSLATE_MAX];
    oam_ctc_onu_vlan_xlate_entry *pData =
        (oam_ctc_onu_vlan_xlate_entry *) data;
    oam_uint8 i = 0;
    oam_uint16 translate_num = 0;


    translate_num =  len / sizeof(oam_ctc_onu_vlan_xlate_entry);

    OAM_ORG_DEBUG("translate num = %d \n", translate_num);
    if (!data)
    {
        return OAM_E_PARAM;
    }

    if (translate_num == 0 || translate_num > CTC_VLAN_MODE_TRANSLATE_MAX) 
    {
        OAM_ORG_LOG(
            "translate number %d is not supported\n ",
            translate_num, 
            CTC_VLAN_MODE_TRANSLATE_MAX);
        return OAM_E_OK;
    }

    for (i = 0; i < translate_num; i++) {
        vlan[i].tag1.tpid = ntohs(pData->tag1.tpid);
        vlan[i].tag2.tpid = ntohs(pData->tag2.tpid);
        vlan[i].tag1.tag = ntohs(pData->tag1.tag);
        vlan[i].tag2.tag = ntohs(pData->tag2.tag);
        OAM_ORG_DEBUG("translate [%d]:tpid %#x, tag %#x --> tpid %#x, tag %#x \n",
                      i, vlan[i].tag1.tpid, vlan[i].tag1.tag, vlan[i].tag2.tpid,
                      vlan[i].tag2.tag);
        pData ++;
        if (!pData)
        {
            break;
        }
    }

    ret = ctc_oam_onu_translate_vlan_set_adapt(port, defVlan, translate_num, vlan);
    if (ret != OAM_E_OK) 
    {
        OAM_ORG_LOG("set adapter failed, ret = %d \n", ret);
    }

    return ret;
}

oam_status ctc_oam_onu_trunk_vlan_get(
    oam_port_id_t port,
    oam_uint32 *num,
    oam_uint8 *data
)
{
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_vlan_tag_t vlan[CTC_VLAN_MODE_TRUNK_MAX];
    oam_ctc_onu_vlan_tag_t *pData =
        (oam_ctc_onu_vlan_tag_t *) data;
    oam_uint8 i = 0;
    oam_uint16 trunk_num = 0;

    ret = ctc_oam_onu_trunk_vlan_get_adapt(port, &trunk_num, vlan);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        return ret;
    }

    OAM_ORG_DEBUG("trunk num = %d \n", trunk_num);

    if (trunk_num > CTC_VLAN_MODE_TRUNK_MAX) {
        OAM_ORG_LOG("trunk number %d is more the max %d ",
                    trunk_num, CTC_VLAN_MODE_TRUNK_MAX);
        return OAM_E_PARAM;
    }

    for (i = 0; i < trunk_num; i++) {
        OAM_ORG_DEBUG("trunk [%d]: tpid %#x , tag %#x \n",
                      i, vlan[i].tpid, vlan[i].tag);
        pData->tpid = htons(vlan[i].tpid);
        pData->tag = htons(vlan[i].tag);
        pData ++;
    }

    *num = trunk_num;
    return OAM_E_OK;
}

oam_status ctc_oam_onu_trunk_vlan_set(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defVlan,
    oam_uint8 *data,
    oam_uint32 len)
{
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_vlan_tag_t vlan[CTC_VLAN_MODE_TRUNK_MAX];
    oam_ctc_onu_vlan_tag_t *pData =
        (oam_ctc_onu_vlan_tag_t *) data;
    oam_uint8 i = 0;
    oam_uint16 trunk_num = len / sizeof(oam_ctc_onu_vlan_tag_t);

    OAM_ORG_DEBUG("trunk num = %d \n", trunk_num);

    if (trunk_num > CTC_VLAN_MODE_TRUNK_MAX) {
        OAM_ORG_LOG("trunk number %d is more the max %d ",
                    trunk_num, CTC_VLAN_MODE_TRUNK_MAX);
        return OAM_E_PARAM;
    }

    for (i = 0; i < trunk_num; i++) {
        vlan[i].tpid = ntohs(pData->tpid);
        vlan[i].tag = ntohs(pData->tag);
        pData ++;
        OAM_ORG_DEBUG("trunk [%d]: tpid %#x , tag %#x \n",
                      i, vlan[i].tpid, vlan[i].tag);
    }

    ret = ctc_oam_onu_trunk_vlan_set_adapt(port, defVlan, trunk_num, vlan);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapter failed, ret = %d \n", ret);
    }

    return ret;
}

oam_status ctc_oam_onu_agg_vlan_get(
    oam_port_id_t port,
    oam_uint32 *len,
    oam_uint8 *data
)
{
    oam_status ret = OAM_E_OK;
    oam_uint16 agg_num;
    ctc_oam_agg_vlan_t vlan[CTC_ONU_AGGR_VLAN_MAX];
    oam_ctc_onu_vlan_aggre_table_t *pData =
        (oam_ctc_onu_vlan_aggre_table_t *) data;
    oam_uint8 i = 0, j = 0;
    oam_ctc_onu_port_vlan_agg_table_t *aggVlanTbl = NULL;
    oam_ctc_onu_vlan_tag_t *aggVlans = NULL;
    oam_uint32 buildLen = 0;

    ret = ctc_oam_onu_agg_vlan_get_adapt(port, &agg_num, vlan);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        return ret;
    }

    OAM_ORG_DEBUG("agg num = %d \n", agg_num);

    if (agg_num > CTC_ONU_AGGR_VLAN_MAX) {
        OAM_ORG_LOG("agg number %d is more the max %d ",
                    agg_num, CTC_ONU_AGGR_VLAN_MAX);
        return OAM_E_PARAM;
    }

    pData->num = htons(agg_num);

    buildLen = sizeof(pData->num);

    aggVlanTbl = (oam_ctc_onu_port_vlan_agg_table_t *) (data + buildLen);

    for (i = 0; i < agg_num; i++) {
        /*build agg num and default vlan*/
        OAM_ORG_DEBUG("agg [%d]: num %d, default tpid %#x,tag %#x \n",
                      i, vlan[i].agg_vlan_num, vlan[i].agg_dst_vlan.tpid, vlan[i].agg_dst_vlan.tag);

        aggVlanTbl->num_of_agg_vlan = htons(vlan[i].agg_vlan_num);
        aggVlanTbl->agg_dst_vlan.tpid = htons(vlan[i].agg_dst_vlan.tpid);
        aggVlanTbl->agg_dst_vlan.tag = htons(vlan[i].agg_dst_vlan.tag);
        buildLen += sizeof(aggVlanTbl->num_of_agg_vlan) + sizeof(
                        aggVlanTbl->agg_dst_vlan);
        aggVlans = (oam_ctc_onu_vlan_tag_t *) (data + buildLen);

        /* build the agg vlan*/
        for (j = 0; j < vlan[i].agg_vlan_num; j++) {
            OAM_ORG_DEBUG("Agged vlan [%d]: tpid %#x , tag %#x \n",
                          j, vlan[i].agg_vlan[j].tpid, vlan[i].agg_vlan[j].tag);

            aggVlans->tpid = htons(vlan[i].agg_vlan[j].tpid);
            aggVlans->tag = htons(vlan[i].agg_vlan[j].tag);
            buildLen += sizeof(oam_ctc_onu_vlan_tag_t);
            aggVlans = (oam_ctc_onu_vlan_tag_t *) (data + buildLen);
        }

        aggVlanTbl = (oam_ctc_onu_port_vlan_agg_table_t *) (data + buildLen);
    }

    *len = buildLen;
    return OAM_E_OK;
}



oam_status ctc_oam_onu_agg_vlan_set(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defVlan,
    oam_uint8 *data,
    oam_uint32 len)
{
    oam_status ret = OAM_E_OK;
    oam_uint16 agg_num;
    ctc_oam_agg_vlan_t vlan[CTC_ONU_AGGR_VLAN_MAX];
    oam_ctc_onu_vlan_aggre_table_t *pData =
        (oam_ctc_onu_vlan_aggre_table_t *) data;
    oam_uint8 i = 0, j = 0;
    oam_ctc_onu_port_vlan_agg_table_t *aggVlanTbl = NULL;
    oam_ctc_onu_vlan_tag_t *aggVlans = NULL;
    oam_uint32 procLen = 0;


    agg_num = ntohs(pData->num);
    OAM_ORG_DEBUG("agg num = %d \n", agg_num);

    if (agg_num > CTC_ONU_AGGR_VLAN_MAX) {
        OAM_ORG_LOG("agg number %d is more the max %d ",
                    agg_num, CTC_ONU_AGGR_VLAN_MAX);
        return OAM_E_PARAM;
    }

    procLen = sizeof(pData->num);

    aggVlanTbl = (oam_ctc_onu_port_vlan_agg_table_t *) (data + procLen);

    for (i = 0 ; i < agg_num; i++) {
        /*build agg num and default vlan*/
        procLen += sizeof(aggVlanTbl->num_of_agg_vlan) + sizeof(
                       aggVlanTbl->agg_dst_vlan);

        if (procLen > len) {
            OAM_ORG_DEBUG("bad tlv \n");
            return OAM_E_PARAM;
        }

        vlan[i].agg_vlan_num = ntohs(aggVlanTbl->num_of_agg_vlan);
        vlan[i].agg_dst_vlan.tpid = ntohs(aggVlanTbl->agg_dst_vlan.tpid);
        vlan[i].agg_dst_vlan.tag = ntohs(aggVlanTbl->agg_dst_vlan.tag);

        OAM_ORG_DEBUG("agg [%d]: num %d, default tpid %#x,tag %#x \n",
                      i, vlan[i].agg_vlan_num, vlan[i].agg_dst_vlan.tpid, vlan[i].agg_dst_vlan.tag);
        aggVlans = (oam_ctc_onu_vlan_tag_t *) (data + procLen);

        if (vlan[i].agg_vlan_num > CTC_ONU_AGGR_VLAN_NUM) {
            OAM_ORG_LOG("agg number %d is more the max %d ",
                        vlan[i].agg_vlan_num, CTC_ONU_AGGR_VLAN_NUM);
            return OAM_E_PARAM;
        }
        
        /* build the agg vlan*/
        for (j = 0; j < vlan[i].agg_vlan_num; j++) {
            procLen += sizeof(oam_ctc_onu_vlan_tag_t);

            if (procLen > len) {
                OAM_ORG_DEBUG("bad tlv \n");
                return OAM_E_PARAM;
            }

            vlan[i].agg_vlan[j].tpid = ntohs(aggVlans->tpid);
            vlan[i].agg_vlan[j].tag = ntohs(aggVlans->tag);
            OAM_ORG_DEBUG("Agged vlan [%d]: tpid %#x , tag %#x \n",
                          j, vlan[i].agg_vlan[j].tpid, vlan[i].agg_vlan[j].tag);
            aggVlans = (oam_ctc_onu_vlan_tag_t *) (data + procLen);
        }

        aggVlanTbl = (oam_ctc_onu_port_vlan_agg_table_t *) (data + procLen);
    }

    ret = ctc_oam_onu_agg_vlan_set_adapt(port, defVlan, agg_num, vlan);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapter failed, ret = %d \n", ret);
    }

    return ret;
}


oam_uint32 ctc_oam_onu_vlan_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_vlan_t *pData =
        (oam_ctc_onu_vlan_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 vlanMode = CTC_VLAN_MODE_TRANSPARENT;
    oam_uint32 num = 0, len = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_onu_vlan_mode_get_adapt(index.idxValue, &vlanMode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get vlan mode adapt return failed, ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    OAM_ORG_DEBUG("vlan mode = %d \n", vlanMode);

    pData->hdr.width = 0;

    if (vlanMode != CTC_VLAN_MODE_TRANSPARENT) {
        ret = ctc_oam_onu_default_vlan_tag_get(index.idxValue, pData->data);

        if (ret != OAM_E_OK) {
            OAM_ORG_LOG("ctc_oam_onu_default_vlan_tag_get failed,ret = %d \n", ret);
            goto ERROR_EXIT;
        }

        pData->hdr.width += sizeof(oam_ctc_onu_vlan_tag_t);
    }

    ret = OAM_E_OK;

    switch (vlanMode) {
        case CTC_VLAN_MODE_TRANSPARENT:
        case CTC_VLAN_MODE_TAG:
            break;

        case CTC_VLAN_MODE_TRANSLATE:
            ret = ctc_oam_onu_translate_vlan_get(index.idxValue, &num,
                                                 pData->data + pData->hdr.width);

            if (ret != OAM_E_OK) {
                OAM_ORG_LOG("ctc_oam_onu_translate_vlan_get failed,ret = %d \n", ret);
                goto ERROR_EXIT;
            }

            pData->hdr.width += num * sizeof(oam_ctc_onu_vlan_xlate_entry);
            break;

        case CTC_VLAN_MODE_AGG:
            ret = ctc_oam_onu_agg_vlan_get(index.idxValue, &len,
                                           pData->data + pData->hdr.width);

            if (ret != OAM_E_OK) {
                OAM_ORG_LOG("ctc_oam_onu_agg_vlan_get failed,ret = %d \n", ret);
                goto ERROR_EXIT;
            }

            pData->hdr.width  += len;
            break;

        case CTC_VLAN_MODE_TRUNK:
            ret = ctc_oam_onu_trunk_vlan_get(index.idxValue, &num,
                                             pData->data + pData->hdr.width);

            if (ret != OAM_E_OK) {
                OAM_ORG_LOG("ctc_oam_onu_trunk_vlan_get failed,ret = %d \n", ret);
                goto ERROR_EXIT;
            }

            pData->hdr.width += ( num * sizeof(oam_ctc_onu_vlan_tag_t));
            break;

        default:
            OAM_ORG_LOG("unknow vlan mode  vlan mode = %d \n", vlanMode);
            goto ERROR_EXIT;
    }

    pData->vlanMode = vlanMode;
    pData->hdr.width +=  sizeof(pData->vlanMode);

    return  sizeof(oam_var_cont_t) + pData->hdr.width;

ERROR_EXIT:
    pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    return sizeof(oam_var_cont_t);

}



oam_uint32 ctc_oam_onu_vlan_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_vlan_t *pInData  =
        (oam_ctc_onu_vlan_t *)pRecv;
    oam_uint8 vlanMode = CTC_VLAN_MODE_TRANSPARENT;
    oam_uint32 len = 0;
    oam_ctc_onu_vlan_tag_t defVlan;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    vlanMode = pInData->vlanMode;

    OAM_ORG_DEBUG("vlan mode = %d \n", vlanMode);

    Recvlen -= sizeof(pInData->vlanMode);

    if (vlanMode != CTC_VLAN_MODE_TRANSPARENT) {
        oam_ctc_onu_vlan_tag_t *vlan  =
            (oam_ctc_onu_vlan_tag_t *) pInData->data;
        defVlan.tpid = ntohs(vlan->tpid);
        defVlan.tag = ntohs(vlan->tag);
        OAM_ORG_DEBUG("default vlan tpid:%#x, tag:%#x \n", defVlan.tpid, defVlan.tag);
        len = sizeof(oam_ctc_onu_vlan_tag_t);
        Recvlen -= len;
    }

    switch (vlanMode) {
        case CTC_VLAN_MODE_TRANSPARENT:
            ret = ctc_oam_onu_transparent_vlan_set_adapt(index.idxValue);
            break;

        case CTC_VLAN_MODE_TAG:
            ret = ctc_oam_onu_tag_vlan_set_adapt(index.idxValue, defVlan);
            break;

        case CTC_VLAN_MODE_TRANSLATE:
            ret = ctc_oam_onu_translate_vlan_set(index.idxValue, defVlan,
                                                 pInData->data + len, Recvlen);
            break;

        case CTC_VLAN_MODE_AGG:
            ret = ctc_oam_onu_agg_vlan_set(index.idxValue, defVlan, pInData->data + len,
                                           Recvlen);
            break;

        case CTC_VLAN_MODE_TRUNK:
            ret = ctc_oam_onu_trunk_vlan_set(index.idxValue, defVlan, pInData->data + len,
                                             Recvlen);
            break;

        default:
            OAM_ORG_LOG("unknow vlan mode  vlan mode = %d \n", vlanMode);
            goto ERROR_EXIT;
    }

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set_adapt failed,ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    pOutData->width = OAM_CTC_VAR_SET_OK;
    return sizeof(oam_var_cont_t);
ERROR_EXIT:
    pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    return sizeof(oam_var_cont_t);
}


oam_uint32 ctc_oam_onu_classification_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_classification_t *pData =
        (oam_ctc_onu_classification_t *)pOut;
    oam_ctc_onu_classification_t *pInData =
        (oam_ctc_onu_classification_t *) pRecV;
    oam_status ret = OAM_E_OK;
    oam_uint32 num = 0, len = 0;
    oam_uint32 i = 0, j = 0;
    oam_uint32 entry_num = 0;
    oam_ctc_onu_classification_struct *entry = NULL;
    oam_ctc_onu_class_fselect_t *tmpSelect = NULL;
    oam_ctc_onu_class_fselect_t *selectV4 = NULL;
    oam_ctc_onu_class_fselect_v6_t *selectV6 = NULL;



    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        goto ERROR_EXIT;
    }

    if (pInData->action != CTC_CLASS_RULES_ACTION_LST) {
        OAM_ORG_LOG("not support action %d \n", pData->action);
        goto ERROR_EXIT;
    }

    ret = ctc_oam_onu_classification_get_adapt(index.idxValue, &num, pData->data);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    OAM_ORG_DEBUG("num = %d \n", num);

    len = 0;

    /*caulate the the response len*/
    for (i = 0; i < num; i++) {
        entry = (oam_ctc_onu_classification_struct *) (pData->data + len);
        entry_num = entry->entries;
        len += (sizeof(oam_ctc_onu_classification_struct) - 1);
        OAM_ORG_DEBUG("classification[%d]:precedence %#x , len %d, map %#x,pri %#x,entry num %d \n",
                      i, entry->precedence, entry->len, entry->queueMapped, entry->priMark,
                      entry_num);
        tmpSelect = (oam_ctc_onu_class_fselect_t *) entry->data ;

        for (j = 0; j < entry_num; j++) {
            /*0 - 0x0B*/
            if (tmpSelect->fieldSelect <= CTC_CLASS_RULES_FSELECT_L4_DST_PORT) {
                selectV4 = (oam_ctc_onu_class_fselect_t *)(pData->data + len);
                OAM_ORG_DEBUG("entry [%d]: select %d, match value %#x:%#x:%#x:%#x:%#x:%#x, oper %d\n",
                              j, selectV4->fieldSelect,
                              selectV4->matchValue[0],
                              selectV4->matchValue[1],
                              selectV4->matchValue[2],
                              selectV4->matchValue[3],
                              selectV4->matchValue[4],
                              selectV4->matchValue[5],
                              selectV4->validationOper);
                len += sizeof(oam_ctc_onu_class_fselect_t);
            }
            else {
                selectV6 = (oam_ctc_onu_class_fselect_v6_t *) (pData->data + len);
                OAM_ORG_DEBUG("entry [%d]: select %d, match value\n\
                        %#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x \n \
                        %#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x \n \
                        , oper %d\n",
                              j, selectV6->fieldSelect,
                              selectV6->matchValue[0],
                              selectV6->matchValue[1],
                              selectV6->matchValue[2],
                              selectV6->matchValue[3],
                              selectV6->matchValue[4],
                              selectV6->matchValue[5],
                              selectV6->matchValue[6],
                              selectV6->matchValue[7],
                              selectV6->matchValue[8],
                              selectV6->matchValue[9],
                              selectV6->matchValue[10],
                              selectV6->matchValue[11],
                              selectV6->matchValue[12],
                              selectV6->matchValue[13],
                              selectV6->matchValue[14],
                              selectV6->matchValue[15],
                              selectV6->validationOper);
                len += sizeof(oam_ctc_onu_class_fselect_v6_t);
            }

            tmpSelect = (oam_ctc_onu_class_fselect_t *) (pData->data + len);
        }

    }

    /* add action and numRules length*/
    pData->numRules = num;
    pData->action = CTC_CLASS_RULES_ACTION_LST;
    pData->hdr.width  = len + sizeof(oam_uint8) * 2;

    /*put the right process length in here*/
    *procRecvLen = sizeof(oam_var_cont_t) + sizeof(oam_uint8);
    return sizeof(oam_var_cont_t) + pData->hdr.width;
ERROR_EXIT:

    *procRecvLen = sizeof(oam_var_cont_t) + sizeof(oam_uint8);
    pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_classification_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_classification_t *pInData  =
        (oam_ctc_onu_classification_t *)pRecv;
    oam_uint32 num = 0;
    oam_uint8 *pData = NULL;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        goto ERROR_EXIT;
    }

    OAM_ORG_DEBUG("action = %d \n", pInData->action);

    if (pInData->action != CTC_CLASS_RULES_ACTION_DEL
        && pInData->action != CTC_CLASS_RULES_ACTION_ADD
        && pInData->action != CTC_CLASS_RULES_ACTION_CLR) {
        OAM_ORG_LOG("Unknow action %d \n", pInData->action);
        goto ERROR_EXIT;
    }

    if (pInData->action != CTC_CLASS_RULES_ACTION_CLR) {
        num = pInData->numRules;
        OAM_ORG_DEBUG("num = %d \n", num);
        pData = pInData->data;
    }

    ret = ctc_oam_onu_classification_set_adapt(
        pInData->action, 
        index.idxValue, 
        num,
        pData);

    if (ret != OAM_E_OK) 
    {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    pOutData->width  = OAM_CTC_VAR_SET_OK;
    return sizeof(oam_var_cont_t);

ERROR_EXIT:
    pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
    return sizeof(oam_var_cont_t);
}


oam_uint32 ctc_oam_onu_mcvlan_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_mc_vlan_t *pData =
        (oam_ctc_onu_mc_vlan_t *)pOut;
    oam_ctc_onu_mc_vlan_t *pInData =
        (oam_ctc_onu_mc_vlan_t *) pRecV;
    oam_status ret = OAM_E_OK;
    oam_uint16 mcVlan[CTC_MC_VLAN_MAX];
    oam_uint16 num = 0, i = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        goto ERROR_EXIT;
    }

    if (pInData->vlanOper != CTC_MC_VLAN_OPR_LST) {
        OAM_ORG_LOG("Unknow oper = %d \n", pInData->vlanOper);
        goto ERROR_EXIT;
    }

    ret = ctc_oam_onu_mcvlan_get_adapt(index.idxValue, &num, mcVlan);

    if (ret != OAM_E_OK ) {
        OAM_ORG_LOG("get adapt failed, ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    OAM_ORG_DEBUG("vlan num = %d \n", num);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("vlan[%d]= %#x\n", i, mcVlan[i]);
        pData->vlanIds[i] = htons(mcVlan[i]);
    }

    pData->vlanOper = CTC_MC_VLAN_OPR_LST;
    pData->hdr.width = sizeof(oam_uint16) * num + sizeof(oam_uint8);
    *procRecvLen = sizeof(oam_var_cont_t) + sizeof(oam_uint8);
    return sizeof(oam_var_cont_t) + pData->hdr.width;

ERROR_EXIT:
    pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
    *procRecvLen = sizeof(oam_var_cont_t) + sizeof(oam_uint8);
    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_mcvlan_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_mc_vlan_t *pInData  =
        (oam_ctc_onu_mc_vlan_t *)pRecv;
    oam_uint16 mcVlan[CTC_MC_VLAN_MAX];
    oam_uint16 num = 0, i;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        goto ERROR_EXIT;
    }


    OAM_ORG_DEBUG("oper = %d \n", pInData->vlanOper);

    if (pInData->vlanOper != CTC_MC_VLAN_OPR_DEL
        && pInData->vlanOper != CTC_MC_VLAN_OPR_ADD
        && pInData->vlanOper != CTC_MC_VLAN_OPR_CLR) {
        OAM_ORG_LOG("Unknow oper %d \n", pInData->vlanOper);
        goto ERROR_EXIT;
    }

    if (CTC_MC_VLAN_OPR_CLR !=  pInData->vlanOper ) {
        /* get the num */
        Recvlen -= sizeof(oam_uint8);
        num = Recvlen / sizeof(oam_uint16);
        OAM_ORG_DEBUG("vlan num = %d \n", num);

        if (num > CTC_MC_VLAN_MAX) {
            OAM_ORG_LOG("vlan num %d is greater than max %d \n",
                        num, CTC_MC_VLAN_MAX);
            goto NO_RES_EXIT;
        }

        for (i = 0; i < num; i++) {
            mcVlan[i] = ntohs(pInData->vlanIds[i]);
            OAM_ORG_DEBUG("vlan[%d] = %#x \n", i, mcVlan[i]);
        }
    }

    ret = ctc_oam_onu_mcvlan_set_adapt(index.idxValue, pInData->vlanOper, num,
                                       mcVlan);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    pOutData->width = OAM_CTC_VAR_SET_OK;
    return sizeof(oam_var_cont_t);

ERROR_EXIT:
    pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
    return sizeof(oam_var_cont_t);

NO_RES_EXIT:
    pOutData->width  = OAM_CTC_VAR_SET_NO_RES;
    return sizeof(oam_var_cont_t);

}

oam_uint32 ctc_oam_onu_mctagstrip_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_mc_tagstrip_t *pData =
        (oam_ctc_onu_mc_tagstrip_t *)pOut;
    oam_ctc_iptv_vid_t *pIptv = (oam_ctc_iptv_vid_t *) pData->data;
    oam_status ret = OAM_E_OK;
    oam_uint8 tagStriped;
    oam_uint8 num;
    oam_ctc_iptv_vid_t  iptvVlans[CTC_ONU_MAX_IPTV_VLAN];
    oam_uint32 i ;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }


    ret = ctc_oam_onu_mctagstrip_get_adapt(index.idxValue, &tagStriped, &num,
                                           iptvVlans);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("tagStriped = %d , num = %d \n", tagStriped, num);

    if (tagStriped == CTC_MC_TAGSTRIP_NOT_STRIP
        || tagStriped == CTC_MC_TAGSTRIP_STRIP) {
        pData->opr = tagStriped;
        pData->hdr.width = sizeof(oam_uint8);
        return sizeof(oam_var_cont_t) + pData->hdr.width;
    }

    if (num > CTC_ONU_MAX_IPTV_VLAN) {
        OAM_ORG_LOG("num %d is greater than max %d \n",
                    num, CTC_ONU_MAX_IPTV_VLAN);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("vlan[%d]: vid %#x, iptv vid %#x \n",
                      iptvVlans[i].vid, iptvVlans[i].iptv_vid);

        pIptv->vid = htons(iptvVlans[i].vid);
        pIptv->iptv_vid = htons(iptvVlans[i].iptv_vid);
        pIptv++;
    }

    pData->opr = tagStriped;
    pData->count = num;
    pData->hdr.width = sizeof(oam_ctc_iptv_vid_t) * num + sizeof(oam_uint8) * 2;

    return sizeof(oam_var_cont_t) + pData->hdr.width;

}

oam_uint32 ctc_oam_onu_mctagstrip_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_mc_tagstrip_t *pInData  =
        (oam_ctc_onu_mc_tagstrip_t *)pRecv;
    oam_ctc_iptv_vid_t *pIptv = (oam_ctc_iptv_vid_t *) pInData->data;

    oam_uint8 tagStriped = 0;
    oam_uint8 num = 0;
    oam_ctc_iptv_vid_t  iptvVlans[CTC_ONU_MAX_IPTV_VLAN];
    oam_uint32 i = 0 ;


    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    tagStriped = pInData->opr;
    OAM_ORG_DEBUG("tagStriped = %d \n", tagStriped);
    
    if (tagStriped == CTC_MC_TAGSTRIP_IPTV) {
        num = pInData->count;

        OAM_ORG_DEBUG("num = %d \n", num);

        if (num > CTC_ONU_MAX_IPTV_VLAN) {
            OAM_ORG_LOG("num %d is greater than max %d \n",
                        num, CTC_ONU_MAX_IPTV_VLAN);
            pOutData->width  = OAM_CTC_VAR_SET_NO_RES;
            return sizeof(oam_var_cont_t);
        }

        for (i = 0; i < num; i++) {
            iptvVlans[i].vid = ntohs( pIptv->vid);
            iptvVlans[i].iptv_vid = ntohs(pIptv->iptv_vid);
            OAM_ORG_DEBUG("vlan[%d]: vid %#x, iptv vid %#x \n",
                          iptvVlans[i].vid, iptvVlans[i].iptv_vid);
            pIptv++;
        }
    }

    ret = ctc_oam_onu_mctagstrip_set_adapt(index.idxValue, tagStriped, num,
                                           iptvVlans);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_mc_switch_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_mc_switch_t *pData =
        (oam_ctc_onu_mc_switch_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 switch_mode;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onu_mc_switch_get_adapt(&switch_mode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("Switch mode = %d \n", switch_mode);
    pData->hdr.width = sizeof(oam_uint8);

    pData->opr = switch_mode;

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

oam_uint32 ctc_oam_onu_mc_switch_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_mc_switch_t *pInData  =
        (oam_ctc_onu_mc_switch_t *)pRecv;
    oam_uint8 switch_mode;

    OAM_CTC_FUNCTION_ENTER(index);

    switch_mode = pInData->opr;

    OAM_ORG_DEBUG("Switch mode = %d \n", switch_mode);

    ret = ctc_oam_onu_mc_switch_set_adapt(switch_mode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}


oam_uint32 ctc_oam_onu_mc_control_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_mc_control_t *pData =
        (oam_ctc_onu_mc_control_t *)pOut;
    oam_ctc_onu_mc_control_t *pInData =
        (oam_ctc_onu_mc_control_t *) pRecV;
    oam_status ret = OAM_E_OK;
    oam_uint8 type = CTC_MC_TYPE_CONTROL_DA_MAC;
    oam_ctc_onu_mc_control_vlan_mac_t   *vlanMacEntry = NULL;
    oam_ctc_onu_mc_control_mac_ip_t  *macIpEntry = NULL;
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIpEntry = NULL;
    oam_ctc_onu_mc_control_vlan_mac_t   *pVlanMac =
        (oam_ctc_onu_mc_control_vlan_mac_t *) pData->data;
    oam_ctc_onu_mc_control_mac_ip_t  *pMacIp =
        (oam_ctc_onu_mc_control_mac_ip_t *) pData->data;
    oam_ctc_onu_mc_control_vlan_ip_t *pVlanIp =
        (oam_ctc_onu_mc_control_vlan_ip_t *) pData->data;
    oam_uint16 num = 0, i = 0;
    oam_uint32 entryLen = 0, totalLen = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    vlanMacEntry = OAM_MALLOC(sizeof(oam_ctc_onu_mc_control_vlan_mac_t) *
                              CTC_MC_CONTROL_MAX_ENTRY);
    macIpEntry = OAM_MALLOC(sizeof(oam_ctc_onu_mc_control_mac_ip_t) *
                            CTC_MC_CONTROL_MAX_ENTRY);
    vlanIpEntry = OAM_MALLOC(sizeof(oam_ctc_onu_mc_control_vlan_ip_t) *
                             CTC_MC_CONTROL_MAX_ENTRY);

    if (vlanMacEntry == NULL
        || macIpEntry == NULL
        || vlanIpEntry == NULL) {
        OAM_ORG_LOG("Malloc failed \n");
        goto ERROR_EXIT2;
    }

    memset(vlanMacEntry, 0x00, sizeof(vlanMacEntry[0])*CTC_MC_CONTROL_MAX_ENTRY);
    memset(macIpEntry, 0x00, sizeof(macIpEntry[0])*CTC_MC_CONTROL_MAX_ENTRY);
    memset(vlanIpEntry, 0x00, sizeof(vlanIpEntry[0])*CTC_MC_CONTROL_MAX_ENTRY);

    OAM_ORG_DEBUG("action = %d \n", pInData->action );

    if (pInData->action != CTC_MC_CONTROL_ACTION_LST) {
        OAM_ORG_LOG("Unknow oper = %d \n", pInData->action);
        goto ERROR_EXIT;
    }

    ret = ctc_oam_onu_mc_control_type_get_adapt(&type);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("ctc_oam_onu_mc_control_type_get_adapt failed, ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    OAM_ORG_DEBUG("type = %d \n", type);

    switch (type) {
        case CTC_MC_TYPE_CONTROL_DA_MAC:
        case CTC_MC_TYPE_CONTROL_DA_VLAN:
        case CTC_MC_TYPE_CONTROL_DA_IPV4_VLAN:
            ret = ctc_oam_onu_mc_control_vlan_mac_get_adapt(&num, vlanMacEntry);
            entryLen = sizeof(oam_ctc_onu_mc_control_vlan_mac_t);
            break;

        case CTC_MC_TYPE_CONTROL_MAC_SA_IP:
            ret = ctc_oam_onu_mc_control_mac_ip_get_adapt(&num, macIpEntry);
            entryLen = sizeof(oam_ctc_onu_mc_control_mac_ip_t);
            break;

        case CTC_MC_TYPE_CONTROL_DA_IPV6_VLAN:
            ret = ctc_oam_onu_mc_control_vlan_ip_get_adapt(&num, vlanIpEntry);
            entryLen = sizeof(oam_ctc_onu_mc_control_vlan_ip_t);
            break;

        default:
            OAM_ORG_DEBUG("unknown mc type\n");
            goto ERROR_EXIT;
    }

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get entry adapt failed, ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    OAM_ORG_DEBUG("num = %d \n", num);

    if (num > CTC_MC_CONTROL_MAX_ENTRY) {
        OAM_ORG_LOG("num %d is greater than the max %d \n",
                    num, CTC_MC_CONTROL_MAX_ENTRY);
        goto ERROR_EXIT;
    }

    pData->action = CTC_MC_CONTROL_ACTION_LST;
    pData->mcType = type;
    pData->entries = num;
    pData->hdr.width = 3 * sizeof(oam_uint8);
    totalLen = sizeof(oam_var_cont_t);

    /* put it into out pkt*/
    for (i = 0; i < num; i++) {
        /* need split more than one tlv*/
        if (pData->hdr.width + entryLen >= CTC_MAX_DATA_LEN) {
            totalLen += pData->hdr.width ;
            pData = (oam_ctc_onu_mc_control_t *)
                    (pOut + totalLen);
            totalLen += sizeof(oam_var_cont_t);
            /* Fill the header */
            pData->hdr.branch = pInData->hdr.branch;
            pData->hdr.leaf = pInData->hdr.leaf;
            pData->hdr.width = entryLen;
            /*shift the data after the header*/
            pVlanMac =  (oam_ctc_onu_mc_control_vlan_mac_t *)
                        ((oam_uint8 *)pData + sizeof(oam_var_cont_t));
            pMacIp =  (oam_ctc_onu_mc_control_mac_ip_t *)
                      ((oam_uint8 *)pData + sizeof(oam_var_cont_t));
            pVlanIp = (oam_ctc_onu_mc_control_vlan_ip_t *)
                      ((oam_uint8 *)pData + sizeof(oam_var_cont_t));

        }
        else {
            pData->hdr.width += entryLen;
        }

        switch (type) {
            case CTC_MC_TYPE_CONTROL_DA_MAC:
            case CTC_MC_TYPE_CONTROL_DA_VLAN:
            case CTC_MC_TYPE_CONTROL_DA_IPV4_VLAN:
                OAM_ORG_DEBUG("VlanMac[%d]: userId %#x, vlan %#x, mac %#x:%#x:%#x:%#x:%#x:%#x \n",
                              i, vlanMacEntry[i].userId,
                              vlanMacEntry[i].vlanId,
                              vlanMacEntry[i].mc_da[0],
                              vlanMacEntry[i].mc_da[1],
                              vlanMacEntry[i].mc_da[2],
                              vlanMacEntry[i].mc_da[3],
                              vlanMacEntry[i].mc_da[4],
                              vlanMacEntry[i].mc_da[5]);
                pVlanMac->userId = htons(vlanMacEntry[i].userId);
                pVlanMac->vlanId = htons(vlanMacEntry[i].vlanId);
                memcpy(pVlanMac->mc_da, vlanMacEntry[i].mc_da, OAM_MACADDR_LEN);
                pVlanMac++;
                break;

            case CTC_MC_TYPE_CONTROL_MAC_SA_IP:
                OAM_ORG_DEBUG("MacIp[%d]: userId %#x,mac %#x:%#x:%#x:%#x%#x:%#x \n",
                              i, macIpEntry[i].userId,
                              macIpEntry[i].mc_da[0],
                              macIpEntry[i].mc_da[1],
                              macIpEntry[i].mc_da[2],
                              macIpEntry[i].mc_da[3],
                              macIpEntry[i].mc_da[4],
                              macIpEntry[i].mc_da[5]);
                OAM_ORG_DEBUG("ip %#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x\n %#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x\n",
                              macIpEntry[i].ip_addr[0],
                              macIpEntry[i].ip_addr[1],
                              macIpEntry[i].ip_addr[2],
                              macIpEntry[i].ip_addr[3],
                              macIpEntry[i].ip_addr[4],
                              macIpEntry[i].ip_addr[5],
                              macIpEntry[i].ip_addr[6],
                              macIpEntry[i].ip_addr[7],
                              macIpEntry[i].ip_addr[8],
                              macIpEntry[i].ip_addr[9],
                              macIpEntry[i].ip_addr[10],
                              macIpEntry[i].ip_addr[11],
                              macIpEntry[i].ip_addr[12],
                              macIpEntry[i].ip_addr[13],
                              macIpEntry[i].ip_addr[14],
                              macIpEntry[i].ip_addr[15]);
                pMacIp->userId = htons(macIpEntry[i].userId);
                memcpy(pMacIp->mc_da, macIpEntry[i].mc_da, OAM_MACADDR_LEN);
                memcpy(pMacIp->ip_addr, macIpEntry[i].ip_addr, OAM_IPV6_ADDR_LEN);
                pMacIp++;
                break;

            case CTC_MC_TYPE_CONTROL_DA_IPV6_VLAN:
                OAM_ORG_DEBUG("VlanIp[%d]: userId %#x, vlan %#x \n",
                              i, vlanIpEntry[i].userId,
                              vlanIpEntry[i].vlanId);
                OAM_ORG_DEBUG("ip %#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x\n %#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x\n",
                              vlanIpEntry[i].ip_addr[0],
                              vlanIpEntry[i].ip_addr[1],
                              vlanIpEntry[i].ip_addr[2],
                              vlanIpEntry[i].ip_addr[3],
                              vlanIpEntry[i].ip_addr[4],
                              vlanIpEntry[i].ip_addr[5],
                              vlanIpEntry[i].ip_addr[6],
                              vlanIpEntry[i].ip_addr[7],
                              vlanIpEntry[i].ip_addr[8],
                              vlanIpEntry[i].ip_addr[9],
                              vlanIpEntry[i].ip_addr[10],
                              vlanIpEntry[i].ip_addr[11],
                              vlanIpEntry[i].ip_addr[12],
                              vlanIpEntry[i].ip_addr[13],
                              vlanIpEntry[i].ip_addr[14],
                              vlanIpEntry[i].ip_addr[15]);
                pVlanIp->userId = htons(vlanIpEntry[i].userId);
                pVlanIp->vlanId = htons(vlanIpEntry[i].vlanId);
                memcpy(pVlanIp->ip_addr, vlanIpEntry[i].ip_addr, OAM_IPV6_ADDR_LEN);
                pVlanIp++;
                break;
        }
    }

    totalLen += pData->hdr.width ;

    *procRecvLen = sizeof(oam_var_cont_t) + sizeof(oam_uint8);
    OAM_FREE(vlanMacEntry);
    OAM_FREE(macIpEntry);
    OAM_FREE(vlanIpEntry);

    return totalLen;

ERROR_EXIT:
    OAM_FREE(vlanMacEntry);
    OAM_FREE(macIpEntry);
    OAM_FREE(vlanIpEntry);
ERROR_EXIT2:
    pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
    *procRecvLen = sizeof(oam_var_cont_t) + sizeof(oam_uint8);
    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_mc_control_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_mc_control_t *pInData  =
        (oam_ctc_onu_mc_control_t *)pRecv;

    oam_ctc_onu_mc_control_vlan_mac_t   vlanMacEntry[CTC_MC_CONTROL_MAX_ENTRY];
    oam_ctc_onu_mc_control_mac_ip_t  macIpEntry[CTC_MC_CONTROL_MAX_ENTRY];
    oam_ctc_onu_mc_control_vlan_ip_t vlanIpEntry[CTC_MC_CONTROL_MAX_ENTRY];
    oam_ctc_onu_mc_control_vlan_mac_t   *pVlanMac =
        (oam_ctc_onu_mc_control_vlan_mac_t *) pInData->data;
    oam_ctc_onu_mc_control_mac_ip_t  *pMacIp =
        (oam_ctc_onu_mc_control_mac_ip_t *) pInData->data;
    oam_ctc_onu_mc_control_vlan_ip_t *pVlanIp =
        (oam_ctc_onu_mc_control_vlan_ip_t *) pInData->data;

    oam_uint8 type = CTC_MC_TYPE_CONTROL_DA_MAC;
    oam_uint8 action = CTC_MC_CONTROL_ACTION_LST;
    oam_uint16 num = 0, i = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    memset(vlanMacEntry, 0x00, sizeof(vlanMacEntry[0])*CTC_MC_CONTROL_MAX_ENTRY);
    memset(macIpEntry, 0x00, sizeof(macIpEntry[0])*CTC_MC_CONTROL_MAX_ENTRY);
    memset(vlanIpEntry, 0x00, sizeof(vlanIpEntry[0])*CTC_MC_CONTROL_MAX_ENTRY);

    action = pInData->action;
    OAM_ORG_DEBUG("action = %d \n", action);

    if (action != CTC_MC_CONTROL_ACTION_DEL
        && action != CTC_MC_CONTROL_ACTION_ADD
        && action != CTC_MC_CONTROL_ACTION_CLR) {
        OAM_ORG_LOG("Unknow action = %d \n", pInData->action);
        goto ERROR_EXIT;
    }

    /*clear */
    if ( action == CTC_MC_CONTROL_ACTION_CLR) {
        ret = ctc_oam_onu_mc_control_clr_adapt();

        if (ret != OAM_E_OK) {
            OAM_ORG_LOG("clear adapt failed, ret = %d \n", ret);
            goto ERROR_EXIT;
        }

        pOutData->width  = OAM_CTC_VAR_SET_OK;
        return sizeof(oam_var_cont_t);
    }

    type = pInData->mcType;
    num = pInData->entries;

    OAM_ORG_DEBUG("type = %d, num = %d  \n", type, num);

    if (num > CTC_MC_CONTROL_MAX_ENTRY) {
        OAM_ORG_LOG("num %d is greater than the max %d \n",
                    num, CTC_MC_CONTROL_MAX_ENTRY);
        goto NO_RES_EXIT;
    }

    for (i = 0; i < num; i++) {
        switch (type) {
            case CTC_MC_TYPE_CONTROL_DA_MAC:
            case CTC_MC_TYPE_CONTROL_DA_VLAN:
            case CTC_MC_TYPE_CONTROL_DA_IPV4_VLAN:
                vlanMacEntry[i].userId = ntohs(pVlanMac->userId);
                vlanMacEntry[i].vlanId = ntohs(pVlanMac->vlanId);
                memcpy(vlanMacEntry[i].mc_da, pVlanMac->mc_da, OAM_MACADDR_LEN);
                OAM_ORG_DEBUG("VlanMac[%d]: userId %#x, vlan %#x, mac %#x:%#x:%#x:%#x:%#x:%#x \n",
                              i, vlanMacEntry[i].userId,
                              vlanMacEntry[i].vlanId,
                              vlanMacEntry[i].mc_da[0],
                              vlanMacEntry[i].mc_da[1],
                              vlanMacEntry[i].mc_da[2],
                              vlanMacEntry[i].mc_da[3],
                              vlanMacEntry[i].mc_da[4],
                              vlanMacEntry[i].mc_da[5]);
                pVlanMac++;
                break;

            case CTC_MC_TYPE_CONTROL_MAC_SA_IP:
                macIpEntry[i].userId = ntohs(pMacIp->userId);
                memcpy(macIpEntry[i].mc_da, pMacIp->mc_da, OAM_MACADDR_LEN);
                memcpy(macIpEntry[i].ip_addr, pMacIp->ip_addr, OAM_IPV6_ADDR_LEN);
                OAM_ORG_DEBUG("MacIp[%d]: userId %#x,mac %#x:%#x:%#x:%#x%#x:%#x \n",
                              i, macIpEntry[i].userId,
                              macIpEntry[i].mc_da[0],
                              macIpEntry[i].mc_da[1],
                              macIpEntry[i].mc_da[2],
                              macIpEntry[i].mc_da[3],
                              macIpEntry[i].mc_da[4],
                              macIpEntry[i].mc_da[5]);
                OAM_ORG_DEBUG("ip %#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x\n %#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x\n",
                              macIpEntry[i].ip_addr[0],
                              macIpEntry[i].ip_addr[1],
                              macIpEntry[i].ip_addr[2],
                              macIpEntry[i].ip_addr[3],
                              macIpEntry[i].ip_addr[4],
                              macIpEntry[i].ip_addr[5],
                              macIpEntry[i].ip_addr[6],
                              macIpEntry[i].ip_addr[7],
                              macIpEntry[i].ip_addr[8],
                              macIpEntry[i].ip_addr[9],
                              macIpEntry[i].ip_addr[10],
                              macIpEntry[i].ip_addr[11],
                              macIpEntry[i].ip_addr[12],
                              macIpEntry[i].ip_addr[13],
                              macIpEntry[i].ip_addr[14],
                              macIpEntry[i].ip_addr[15]);
                pMacIp++;
                break;

            case CTC_MC_TYPE_CONTROL_DA_IPV6_VLAN:
                vlanIpEntry[i].userId = ntohs(pVlanIp->userId);
                vlanIpEntry[i].vlanId = ntohs(pVlanIp->vlanId);
                memcpy(vlanIpEntry[i].ip_addr, pVlanIp->ip_addr, OAM_IPV6_ADDR_LEN);
                OAM_ORG_DEBUG("VlanIp[%d]: userId %#x, vlan %#x \n",
                              i, vlanIpEntry[i].userId,
                              vlanIpEntry[i].vlanId);
                OAM_ORG_DEBUG("ip %#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x\n %#x:%#x:%#x:%#x:%#x:%#x:%#x:%#x\n",
                              vlanIpEntry[i].ip_addr[0],
                              vlanIpEntry[i].ip_addr[1],
                              vlanIpEntry[i].ip_addr[2],
                              vlanIpEntry[i].ip_addr[3],
                              vlanIpEntry[i].ip_addr[4],
                              vlanIpEntry[i].ip_addr[5],
                              vlanIpEntry[i].ip_addr[6],
                              vlanIpEntry[i].ip_addr[7],
                              vlanIpEntry[i].ip_addr[8],
                              vlanIpEntry[i].ip_addr[9],
                              vlanIpEntry[i].ip_addr[10],
                              vlanIpEntry[i].ip_addr[11],
                              vlanIpEntry[i].ip_addr[12],
                              vlanIpEntry[i].ip_addr[13],
                              vlanIpEntry[i].ip_addr[14],
                              vlanIpEntry[i].ip_addr[15]);
                pVlanIp++;
                break;
        }
    }

    ret = ctc_oam_onu_mc_control_type_set_adapt(type);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("ctc_oam_onu_mc_control_type_set_adapt failed, ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    switch (type) {
        case CTC_MC_TYPE_CONTROL_DA_MAC:
        case CTC_MC_TYPE_CONTROL_DA_VLAN:
        case CTC_MC_TYPE_CONTROL_DA_IPV4_VLAN:
            ret = ctc_oam_onu_mc_control_vlan_mac_set_adapt(action, num, vlanMacEntry);
            break;

        case CTC_MC_TYPE_CONTROL_MAC_SA_IP:
            ret = ctc_oam_onu_mc_control_mac_ip_set_adapt(action, num, macIpEntry);
            break;

        case CTC_MC_TYPE_CONTROL_DA_IPV6_VLAN:
            ret = ctc_oam_onu_mc_control_vlan_ip_set_adapt(action, num, vlanIpEntry);
            break;
    }

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt failed, ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    pOutData->width  = OAM_CTC_VAR_SET_OK;
    return sizeof(oam_var_cont_t);
ERROR_EXIT:
    pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
    return sizeof(oam_var_cont_t);
NO_RES_EXIT:
    pOutData->width  = OAM_CTC_VAR_SET_NO_RES;
    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_grp_nmb_max_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_mc_max_grp_t *pData =
        (oam_ctc_onu_mc_max_grp_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 maxGrp;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_onu_grp_nmb_max_get_adapt(index.idxValue, &maxGrp);

    if (ret == OAM_E_RESOURCE) {
        OAM_ORG_LOG("set adapt return failed, ret = %d  \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_NO_RES;
        return sizeof(oam_var_cont_t);
    }
    else if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d  \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("max group = %d \n", maxGrp);

    pData->maxGrp = maxGrp;

    pData->hdr.width = sizeof(oam_uint8);

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

oam_uint32 ctc_oam_onu_grp_nmb_max_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_mc_max_grp_t *pInData  =
        (oam_ctc_onu_mc_max_grp_t *)pRecv;
    oam_uint8 maxGrp;


    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    maxGrp = pInData->maxGrp;

    OAM_ORG_DEBUG("max group = %d \n", maxGrp);

    ret = ctc_oam_onu_grp_nmb_max_set_adapt(index.idxValue, maxGrp);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}


oam_uint32 ctc_oam_onu_fast_leave_ability_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_fast_leave_ability_t *pData =
        (oam_ctc_onu_fast_leave_ability_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint32 num = 0, i = 0;
    oam_uint32 mode[CTC_ONU_FAST_LEAVE_MODE_MAX];

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onu_fast_leave_ability_get_adapt(&num, mode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d  \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("num = %d \n", num);

    if (num > CTC_ONU_FAST_LEAVE_MODE_MAX) {
        OAM_ORG_LOG("num %d is greater than the max %d \n",
                    num, CTC_ONU_FAST_LEAVE_MODE_MAX);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pData->mode_num = htonl(num);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("mode[%d]: %#x \n", i, mode[i]);
        pData->mode[i] = htonl(mode[i]);
    }

    pData->hdr.width = sizeof(oam_uint32) + sizeof(oam_uint32) * num;

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}


oam_uint32 ctc_oam_onu_fast_leave_state_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_fast_leave_t *pData =
        (oam_ctc_onu_fast_leave_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint32 state;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onu_fast_leave_state_get_adapt(&state);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d  \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("state = %d \n", state);

    pData->enable = htonl(state);

    pData->hdr.width = sizeof(oam_uint32);

    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

oam_uint32 ctc_oam_onu_llid_queue_config_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_llid_queue_config_t *pData =
        (oam_ctc_onu_llid_queue_config_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 num = 0, i = 0;
    oam_ctc_onu_llid_config_t config[MDU_MAX_QUEUES_PER_LLID];

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_onu_llid_queue_config_get_adapt(index.idxValue, &num, config);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d  \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("num = %d \n", num);

    if (num > MDU_MAX_QUEUES_PER_LLID) {
        OAM_ORG_LOG("num %d is greater than the max %d \n",
                    num, MDU_MAX_QUEUES_PER_LLID);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pData->queue_num = num;

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("config[%d]: qid %#x, wrr %#x \n", i, config[i].qid,
                      config[i].wrr);
        pData->config[i].qid = htons(config[i].qid);
        pData->config[i].wrr = htons(config[i].wrr);
    }

    pData->hdr.width = sizeof(oam_uint8) + sizeof(oam_ctc_onu_llid_config_t) * num;

    return sizeof(oam_var_cont_t) + pData->hdr.width;

}


oam_uint32 ctc_oam_onu_llid_queue_config_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_llid_queue_config_t *pInData  =
        (oam_ctc_onu_llid_queue_config_t *)pRecv;
    oam_uint8 num , i = 0;
    oam_ctc_onu_llid_config_t config[MDU_MAX_QUEUES_PER_LLID];



    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    num = pInData->queue_num;

    OAM_ORG_DEBUG("num = %d \n", num);

    if (num > MDU_MAX_QUEUES_PER_LLID) {
        OAM_ORG_LOG("num %d is greater than the max %d \n",
                    num, MDU_MAX_QUEUES_PER_LLID);
        pOutData->width  = OAM_CTC_VAR_SET_NO_RES;
        return sizeof(oam_var_cont_t);
    }

    for (i = 0; i < num; i++) {
        config[i].qid = ntohs(pInData->config[i].qid);
        config[i].wrr = ntohs(pInData->config[i].wrr);
        OAM_ORG_DEBUG("config[%d]: qid %#x, wrr %#x \n", i, config[i].qid,
                      config[i].wrr);
    }

    ret = ctc_oam_onu_llid_queue_config_set_adapt(index.idxValue, num, config);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_alarm_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_alarm_t *pData =
        (oam_ctc_onu_alarm_t *)pOut;
    oam_ctc_onu_alarm_t *pInData =
        (oam_ctc_onu_alarm_t *)pRecV;
    oam_status ret = OAM_E_OK;
    oam_uint16 alarm_id;
    oam_uint32 alarm_config ;

    OAM_CTC_FUNCTION_ENTER(index);

    alarm_id = ntohs(pInData->alarm_id);
    *procRecvLen = sizeof(oam_var_cont_t) + sizeof(oam_uint16);

    OAM_ORG_DEBUG("alarm id = %#x \n", alarm_id);

    ret = ctc_oam_onu_alarm_get_adapt(index, alarm_id, &alarm_config);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d  \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pData->alarm_id = htons(alarm_id);
    pData->alarm_config = htonl(alarm_config);
    pData->hdr.width = sizeof(oam_ctc_onu_alarm_t) -
                       sizeof(oam_var_cont_t);

    return sizeof(oam_var_cont_t) + pData->hdr.width ;
}

oam_uint32 ctc_oam_onu_alarm_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_alarm_t *pInData  =
        (oam_ctc_onu_alarm_t *)pRecv;
    oam_uint16 alarm_id;
    oam_uint32 alarm_config ;

    OAM_CTC_FUNCTION_ENTER(index);

    alarm_id = ntohs(pInData->alarm_id);
    alarm_config = ntohl(pInData->alarm_config);

    OAM_ORG_DEBUG("alarm_id = %#x, config %#x\n",
                  alarm_id, alarm_config);

    ret = ctc_oam_onu_alarm_set_adapt(index, alarm_id, alarm_config);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}


oam_uint32 ctc_oam_onu_alarm_threshold_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_alarm_threshold_t *pData =
        (oam_ctc_onu_alarm_threshold_t *)pOut;
    oam_ctc_onu_alarm_threshold_t *pInData =
        (oam_ctc_onu_alarm_threshold_t *)pRecV;
    oam_status ret = OAM_E_OK;
    oam_uint16 alarm_id;
    oam_uint32 alarm_value;
    oam_uint32 clear_value;

    OAM_CTC_FUNCTION_ENTER(index);

    alarm_id = ntohs(pInData->alarm_id);
    *procRecvLen = sizeof(oam_var_cont_t) + sizeof(oam_uint16);

    OAM_ORG_DEBUG("alarm id = %#x \n", alarm_id);

    ret = ctc_oam_onu_alarm_threshold_get_adapt(index, alarm_id, &alarm_value,
            &clear_value);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d  \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pData->alarm_id = htons(alarm_id);
    pData->alarm_threshold = htonl(alarm_value);
    pData->clear_threshold = htonl(clear_value);
    pData->hdr.width = sizeof(oam_ctc_onu_alarm_threshold_t) -
                       sizeof(oam_var_cont_t);

    return sizeof(oam_var_cont_t) + pData->hdr.width ;
}

oam_uint32 ctc_oam_onu_alarm_threshold_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_alarm_threshold_t *pInData  =
        (oam_ctc_onu_alarm_threshold_t *)pRecv;
    oam_uint16 alarm_id;
    oam_uint32 alarm_value;
    oam_uint32 clear_value;

    OAM_CTC_FUNCTION_ENTER(index);

    alarm_id = ntohs(pInData->alarm_id);
    alarm_value = ntohl(pInData->alarm_threshold);
    clear_value = ntohl(pInData->clear_threshold);

    OAM_ORG_DEBUG("alarm_id = %#x, alarm %#x, clear %#x\n",
                  alarm_id, alarm_value, clear_value);

    ret = ctc_oam_onu_alarm_threshold_set_adapt(index, alarm_id, alarm_value,
            clear_value);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_laser_control_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_laser_control_t *pInData  =
        (oam_ctc_onu_laser_control_t *)pRecv;
    oam_uint16 action;
    oam_uint8  onuId[OAM_MACADDR_LEN];
    oam_uint32 transceiverId;
    oam_ctc_onu_laser_control_old_t *pInOldData = NULL;
    oam_uint8 version = 0x21;
    oam_uint32 action_old = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    oam_ctc_version_get(oam_llid_get(), &version);

    if (version >= 0x30) {
        action = ntohs(pInData->action);
        memcpy(onuId, pInData->mac_address, OAM_MACADDR_LEN);
        transceiverId = ntohl(pInData->transceiver_id);
    }
    else {
        pInOldData = (oam_ctc_onu_laser_control_old_t *)pRecv;
        action_old = ntohl(pInOldData->action);

        if (action_old > 65535) {
            pOutData->width = OAM_CTC_VAR_SET_NO_RES;
            return sizeof(oam_var_cont_t);
        }

        action = (oam_uint16)action_old;
        memcpy(onuId, pInOldData->mac_address, OAM_MACADDR_LEN);
        transceiverId = ntohl(pInOldData->transceiver_id);
    }

    OAM_ORG_DEBUG("action %d , onuId: %#x:%#x:%#x:%#x:%#x:%#x , trans id = %d \n",
                  action, onuId[0], onuId[1], onuId[2], onuId[3], onuId[4], onuId[5],
                  transceiverId);

    ret = ctc_oam_onu_laser_control_set_adapt(action, onuId, transceiverId);

    if (ret == OAM_E_PARAM) {
        return 0; /* onu id not match, ingore the oam*/
    }
    else if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_stats_monitor_status_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_stats_monitor_t *pData =
        (oam_ctc_onu_stats_monitor_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint16 monitor_status;
    oam_uint32 monitor_period;
    oam_port_id_t port;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    port = index.idxValue;

    if (index.idxLeaf == OAM_CTC_LEAF_CODE_ONU_PON) {
        OAM_ORG_DEBUG("port id is pon port \n");
        /* pon port */
        port = OAM_PON_PORT_ID;
    }


    ret = ctc_oam_onu_stats_monitor_status_get_adapt(port, &monitor_status,
            &monitor_period);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d  \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("status %d , period %#x \n", monitor_status, monitor_period);

    pData->monitor_status = htons(monitor_status);
    pData->monitor_period = htonl(monitor_period);
    pData->hdr.width = sizeof(oam_ctc_onu_stats_monitor_t) -
                       sizeof(oam_var_cont_t);

    return sizeof(oam_var_cont_t) + pData->hdr.width ;
}

oam_uint32 ctc_oam_onu_stats_monitor_status_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_stats_monitor_t *pInData  =
        (oam_ctc_onu_stats_monitor_t *)pRecv;

    oam_uint16 monitor_status;
    oam_uint32 monitor_period;
    oam_port_id_t port;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    port = index.idxValue;

    if (index.idxLeaf == OAM_CTC_LEAF_CODE_ONU_PON) {
        OAM_ORG_DEBUG("port id is pon port \n");
        /* pon port */
        port = OAM_PON_PORT_ID;
    }

    monitor_status = ntohs(pInData->monitor_status);
    monitor_period = ntohl(pInData->monitor_period);

    OAM_ORG_DEBUG("status %d , period %#x \n", monitor_status, monitor_period);

    ret = ctc_oam_onu_stats_monitor_status_set_adapt(port, monitor_status,
            monitor_period);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);

}




void ctc_oam_onu_stats_dump(oam_ctc_onu_port_stats_t *stats)
{
    OAM_ORG_DEBUG("dn_drop_events            = %#x \n",
                  stats->dn_drop_events            );
    OAM_ORG_DEBUG("up_drop_events            = %#x \n",
                  stats->up_drop_events            );
    OAM_ORG_DEBUG("dn_octets                 = %#x \n",
                  stats->dn_octets                 );
    OAM_ORG_DEBUG("up_octets                 = %#x \n",
                  stats->up_octets                 );
    OAM_ORG_DEBUG("dn_frames                 = %#x \n",
                  stats->dn_frames                 );
    OAM_ORG_DEBUG("up_frames                 = %#x \n",
                  stats->up_frames                 );
    OAM_ORG_DEBUG("dn_bc_frames              = %#x \n",
                  stats->dn_bc_frames              );
    OAM_ORG_DEBUG("up_bc_frames              = %#x \n",
                  stats->up_bc_frames              );
    OAM_ORG_DEBUG("dn_mc_frames              = %#x \n",
                  stats->dn_mc_frames              );
    OAM_ORG_DEBUG("up_mc_frames              = %#x \n",
                  stats->up_mc_frames              );
    OAM_ORG_DEBUG("dn_crc_err_frames         = %#x \n",
                  stats->dn_crc_err_frames         );
    OAM_ORG_DEBUG("up_crc_err_frames         = %#x \n",
                  stats->up_crc_err_frames         );
    OAM_ORG_DEBUG("dn_undersize_frames       = %#x \n",
                  stats->dn_undersize_frames       );
    OAM_ORG_DEBUG("up_undersize_frames       = %#x \n",
                  stats->up_undersize_frames       );
    OAM_ORG_DEBUG("dn_oversize_frames        = %#x \n",
                  stats->dn_oversize_frames        );
    OAM_ORG_DEBUG("up_oversize_frames        = %#x \n",
                  stats->up_oversize_frames        );
    OAM_ORG_DEBUG("dn_fragments              = %#x \n",
                  stats->dn_fragments              );
    OAM_ORG_DEBUG("up_fragments              = %#x \n",
                  stats->up_fragments              );
    OAM_ORG_DEBUG("dn_jabbers                = %#x \n",
                  stats->dn_jabbers                );
    OAM_ORG_DEBUG("up_jabbers                = %#x \n",
                  stats->up_jabbers                );
    /*OAM_ORG_DEBUG("dn_collisions             = %#x \n",stats->dn_collisions             );*/
    OAM_ORG_DEBUG("dn_64_octets_frames       = %#x \n",
                  stats->dn_64_octets_frames       );
    OAM_ORG_DEBUG("dn_65_127_octets_frames   = %#x \n",
                  stats->dn_65_127_octets_frames   );
    OAM_ORG_DEBUG("dn_128_255_octets_frames  = %#x \n",
                  stats->dn_128_255_octets_frames  );
    OAM_ORG_DEBUG("dn_256_511_octets_frames  = %#x \n",
                  stats->dn_256_511_octets_frames  );
    OAM_ORG_DEBUG("dn_512_1023_octets_frames = %#x \n",
                  stats->dn_512_1023_octets_frames );
    OAM_ORG_DEBUG("dn_1024_1518_octets_frames= %#x \n",
                  stats->dn_1024_1518_octets_frames);
    OAM_ORG_DEBUG("up_64_octets_frames       = %#x \n",
                  stats->up_64_octets_frames       );
    OAM_ORG_DEBUG("up_65_127_octets_frames   = %#x \n",
                  stats->up_65_127_octets_frames   );
    OAM_ORG_DEBUG("up_128_255_octets_frames  = %#x \n",
                  stats->up_128_255_octets_frames  );
    OAM_ORG_DEBUG("up_256_511_octets_frames  = %#x \n",
                  stats->up_256_511_octets_frames  );
    OAM_ORG_DEBUG("up_512_1023_octets_frames = %#x \n",
                  stats->up_512_1023_octets_frames );
    OAM_ORG_DEBUG("up_1024_1518_octets_frames= %#x \n",
                  stats->up_1024_1518_octets_frames);
    OAM_ORG_DEBUG("dn_discard_frames         = %#x \n",
                  stats->dn_discard_frames         );
    OAM_ORG_DEBUG("up_discard_frames         = %#x \n",
                  stats->up_discard_frames         );
    OAM_ORG_DEBUG("dn_err_frames             = %#x \n",
                  stats->dn_err_frames             );
    OAM_ORG_DEBUG("up_err_frames             = %#x \n",
                  stats->up_err_frames             );
    OAM_ORG_DEBUG("status_change_times       = %#x \n",
                  stats->status_change_times       );
}

oam_uint32 ctc_oam_onu_stats_get(
    oam_uint8 type,
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_stats_data_t *pData =
        (oam_ctc_onu_stats_data_t *)pOut;
    oam_var_cont_t *pInData
        = (oam_var_cont_t *)pRecV;
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_port_stats_t stats;
    oam_ctc_onu_port_stats_frag1_t   *frag1 = &pData->frag1;
    oam_ctc_onu_port_stats_frag2_t   *frag2 = &pData->frag2;
    oam_ctc_onu_port_stats_frag3_t   *frag3 = &pData->frag3;
    oam_port_id_t port;
    oam_uint32 totalLen = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    port = index.idxValue;

    if (index.idxLeaf == OAM_CTC_LEAF_CODE_ONU_PON) {
        OAM_ORG_DEBUG("port id is pon port \n");
        /* pon port */
        port = OAM_PON_PORT_ID;
    }

    if (type == CTC_OAM_STATS_GET_TYPE_CURR) {
        ret = ctc_oam_onu_curr_stats_get_adapt(port, &stats);
    }
    else {
        ret = ctc_oam_onu_histroy_stats_get_adapt(port, &stats);
    }

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d  \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ctc_oam_onu_stats_dump(&stats);

    pData->hdr.branch                  = pInData->branch;
    pData->hdr.leaf                      = pInData->leaf;
    pData->hdr.width                    = 0x0;

    frag1->dn_drop_events            = htonll( stats.dn_drop_events            );
    frag1->up_drop_events            = htonll( stats.up_drop_events            );
    frag1->dn_octets                 = htonll( stats.dn_octets                 );
    frag1->up_octets                 = htonll( stats.up_octets                 );
    frag1->dn_frames                 = htonll( stats.dn_frames                 );
    frag1->up_frames                 = htonll( stats.up_frames                 );
    frag1->dn_bc_frames              = htonll( stats.dn_bc_frames              );
    frag1->up_bc_frames              = htonll( stats.up_bc_frames              );
    frag1->dn_mc_frames              = htonll( stats.dn_mc_frames              );
    frag1->up_mc_frames              = htonll( stats.up_mc_frames              );
    frag1->dn_crc_err_frames         = htonll( stats.dn_crc_err_frames         );
    frag1->up_crc_err_frames         = htonll( stats.up_crc_err_frames         );
    frag1->dn_undersize_frames       = htonll( stats.dn_undersize_frames       );
    frag1->up_undersize_frames       = htonll( stats.up_undersize_frames       );
    frag1->dn_oversize_frames        = htonll( stats.dn_oversize_frames        );
    frag1->up_oversize_frames        = htonll( stats.up_oversize_frames        );
    pData->hdr2.branch                  = pInData->branch;
    pData->hdr2.leaf                      = pInData->leaf;
    pData->hdr2.width                    = 0x0;

    frag2->dn_fragments              = htonll( stats.dn_fragments              );
    frag2->up_fragments              = htonll( stats.up_fragments              );
    frag2->dn_jabbers                = htonll( stats.dn_jabbers                );
    frag2->up_jabbers                = htonll( stats.up_jabbers                );
    frag2->dn_64_octets_frames       = htonll( stats.dn_64_octets_frames       );
    frag2->dn_65_127_octets_frames   = htonll( stats.dn_65_127_octets_frames   );
    frag2->dn_128_255_octets_frames  = htonll( stats.dn_128_255_octets_frames  );
    frag2->dn_256_511_octets_frames  = htonll( stats.dn_256_511_octets_frames  );
    frag2->dn_512_1023_octets_frames = htonll( stats.dn_512_1023_octets_frames );
    frag2->dn_1024_1518_octets_frames = htonll( stats.dn_1024_1518_octets_frames);
    frag2->up_64_octets_frames       = htonll( stats.up_64_octets_frames       );
    frag2->up_65_127_octets_frames   = htonll( stats.up_65_127_octets_frames   );
    frag2->up_128_255_octets_frames  = htonll( stats.up_128_255_octets_frames  );
    frag2->up_256_511_octets_frames  = htonll( stats.up_256_511_octets_frames  );
    frag2->up_512_1023_octets_frames = htonll( stats.up_512_1023_octets_frames );
    frag2->up_1024_1518_octets_frames = htonll( stats.up_1024_1518_octets_frames);

    pData->hdr3.branch                  = pInData->branch;
    pData->hdr3.leaf                      = pInData->leaf;
    pData->hdr3.width                    = 0x28;
    frag3->dn_discard_frames         = htonll( stats.dn_discard_frames         );
    frag3->up_discard_frames         = htonll( stats.up_discard_frames         );
    frag3->dn_err_frames            = htonll(stats.dn_err_frames);
    frag3->up_err_frames               = htonll (stats.up_err_frames);
    frag3->status_change_times      = htonll ( stats.status_change_times);

    totalLen = sizeof(oam_var_cont_t);
    totalLen += 128 ;

    totalLen += sizeof(oam_var_cont_t);
    totalLen += 128 ;

    totalLen += sizeof(oam_var_cont_t);
    totalLen += pData->hdr3.width ;

    return totalLen;
}

oam_uint32 ctc_oam_onu_curr_stats_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    return ctc_oam_onu_stats_get(CTC_OAM_STATS_GET_TYPE_CURR, index, pRecV,
                                 procRecvLen, pOut);
}

oam_uint32 ctc_oam_onu_curr_stats_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_stats_data_t *pInData  =
        (oam_ctc_onu_stats_data_t *)pRecv;
    oam_ctc_onu_port_stats_t stats;
    oam_ctc_onu_port_stats_frag1_t   *frag1 = &pInData->frag1;
    oam_ctc_onu_port_stats_frag2_t   *frag2 = &pInData->frag2;
    oam_ctc_onu_port_stats_frag3_t   *frag3 = &pInData->frag3;


    oam_port_id_t port;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    port = index.idxValue;

    if (index.idxLeaf == OAM_CTC_LEAF_CODE_ONU_PON) {
        OAM_ORG_DEBUG("port id is pon port \n");
        /* pon port */
        port = OAM_PON_PORT_ID;
    }

    stats.dn_drop_events                    = ntohll( frag1->dn_drop_events) ;
    stats.up_drop_events                    = ntohll (frag1->up_drop_events );
    stats.dn_octets                             = ntohll (frag1->dn_octets );
    stats.up_octets                             = ntohll ( frag1->up_octets );
    stats.dn_frames                            = ntohll (frag1->dn_frames  );
    stats.up_frames                           = ntohll (frag1->up_frames);
    stats.dn_bc_frames                      = ntohll ( frag1->dn_bc_frames);
    stats.up_bc_frames                      = ntohll (frag1->up_bc_frames );
    stats.dn_mc_frames                    = ntohll (frag1->dn_mc_frames  );
    stats.up_mc_frames                    = ntohll (frag1->up_mc_frames);
    stats.dn_crc_err_frames              = ntohll ( frag1->dn_crc_err_frames);
    stats.up_crc_err_frames              = ntohll (frag1->up_crc_err_frames);
    stats.dn_undersize_frames           = ntohll (frag1->dn_undersize_frames);
    stats.up_undersize_frames           = ntohll (frag1->up_undersize_frames) ;
    stats.dn_oversize_frames             = ntohll (frag1->dn_oversize_frames);
    stats.up_oversize_frames              = ntohll (frag1->up_oversize_frames) ;

    stats.dn_fragments                      = ntohll (frag2->dn_fragments);
    stats.up_fragments                      = ntohll (frag2->up_fragments);
    stats.dn_jabbers                          = ntohll (frag2->dn_jabbers) ;
    stats.up_jabbers                          = ntohll (frag2->up_jabbers) ;
    stats.dn_64_octets_frames           = ntohll ( frag2->dn_64_octets_frames)  ;
    stats.dn_65_127_octets_frames     = ntohll (frag2->dn_65_127_octets_frames);
    stats.dn_128_255_octets_frames  = ntohll (frag2->dn_128_255_octets_frames);
    stats.dn_256_511_octets_frames  = ntohll (frag2->dn_256_511_octets_frames);
    stats.dn_512_1023_octets_frames = ntohll (frag2->dn_512_1023_octets_frames);
    stats.dn_1024_1518_octets_frames = ntohll (frag2->dn_1024_1518_octets_frames);
    stats.up_64_octets_frames              = ntohll (frag2->up_64_octets_frames);
    stats.up_65_127_octets_frames       = ntohll (frag2->up_65_127_octets_frames);
    stats.up_128_255_octets_frames      = ntohll (frag2->up_128_255_octets_frames);
    stats.up_256_511_octets_frames      = ntohll (frag2->up_256_511_octets_frames);
    stats.up_512_1023_octets_frames     = ntohll (frag2->up_512_1023_octets_frames);
    stats.up_1024_1518_octets_frames    = ntohll (
            frag2->up_1024_1518_octets_frames);

    stats.dn_discard_frames                   = ntohll (frag3->dn_discard_frames)  ;
    stats.up_discard_frames                   = ntohll (frag3->up_discard_frames)  ;

    stats.dn_err_frames                         = ntohll (frag3->dn_err_frames)   ;
    stats.up_err_frames                         = ntohll (frag3->up_err_frames);
    stats.status_change_times                = ntohll (frag3->status_change_times) ;


    ctc_oam_onu_stats_dump(&stats);

    ret = ctc_oam_onu_curr_stats_set_adapt(port, &stats);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_histroy_stats_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    return ctc_oam_onu_stats_get(CTC_OAM_STATS_GET_TYPE_HISTORY, index, pRecV,
                                 procRecvLen, pOut);
}



oam_uint32 ctc_oam_eth_admin_state_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_var_cont_val_32_t *pInData  =
        (oam_var_cont_val_32_t *)pRecv;
    oam_uint32 phyAdmin;


    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    phyAdmin = ntohl(pInData->value);

    OAM_ORG_DEBUG("phy admin = %d \n", phyAdmin);

    ret = ctc_oam_eth_admin_state_set_adapt(index.idxValue, phyAdmin);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}


oam_uint32 ctc_oam_auto_neg_restart_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }


    ret = ctc_oam_auto_neg_restart_set_adapt(index.idxValue);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}


oam_uint32 ctc_oam_auto_neg_ctrl_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_var_cont_val_32_t *pInData  =
        (oam_var_cont_val_32_t *)pRecv;
    oam_uint32 autoNegAdmin;


    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    autoNegAdmin = ntohl(pInData->value);

    OAM_ORG_DEBUG("auto neg admin = %d \n", autoNegAdmin);

    ret = ctc_oam_auto_neg_ctrl_set_adapt(index.idxValue, autoNegAdmin);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_multi_llid_admin_ctrl_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_multi_llid_admin_ctrl_t *pInData  =
        (oam_ctc_onu_multi_llid_admin_ctrl_t *)pRecv;
    oam_uint32 llid_num = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    llid_num = ntohl(pInData->llid_num);

    OAM_ORG_DEBUG("llid number = %d \n", llid_num);

    ret = ctc_oam_multi_llid_admin_ctrl_set_adapt(llid_num);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_reset_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;


    OAM_CTC_FUNCTION_ENTER(index);

    ret = ctc_oam_onu_reset_set_adapt();

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_sleep_control_set (
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_sleep_ctrl_t *pInData  =
        (oam_ctc_onu_sleep_ctrl_t *)pRecv;
    oam_uint32 sleep_duration, wait_duration;
    oam_uint8 sleep_flag, sleep_mode;

    OAM_CTC_FUNCTION_ENTER(index);

    sleep_duration = ntohl(pInData->sleep_duration);
    wait_duration = ntohl(pInData->wait_duration);
    sleep_flag = pInData->sleep_flag;
    sleep_mode = pInData->sleep_mode;

    OAM_ORG_DEBUG("sleep duration:%#x, wait duration %#x \n",
                  sleep_duration , wait_duration );
    OAM_ORG_DEBUG("sleep flag %d, sleep mode: %d \n",
                  sleep_flag, sleep_mode);

    ret = ctc_oam_onu_sleep_control_set_adapt(sleep_duration,
            wait_duration, sleep_flag, sleep_mode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}



oam_uint32 ctc_oam_onu_cfg_mgmt_set (
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_cfg_mgmt_t *pInData  =
        (oam_ctc_onu_cfg_mgmt_t *)pRecv;
    oam_uint8 action;

    OAM_CTC_FUNCTION_ENTER(index);

    action = pInData->action;

    OAM_ORG_DEBUG("action = %d \n",
                  action);

    ret = ctc_oam_onu_cfg_mgmt_set_adapt(action);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 ctc_oam_onu_0xc9_0x0002_config_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    if (oam_is_cuc_oam()) {
        return ctc_oam_onu_cfg_mgmt_set(index, pRecv, Recvlen, pOut);
    }
    else {
        return  ctc_oam_onu_sleep_control_set(index, pRecv, Recvlen, pOut);
    }

}


oam_uint32 ctc_oam_fast_leave_ctrl_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_var_cont_val_32_t *pInData  =
        (oam_var_cont_val_32_t *)pRecv;
    oam_uint32 fastLeaveAdmin;


    OAM_CTC_FUNCTION_ENTER(index);

    fastLeaveAdmin = ntohl(pInData->value);

    OAM_ORG_DEBUG("fast leave admin = %d \n", fastLeaveAdmin);

    ret = ctc_oam_fast_leave_ctrl_set_adapt(fastLeaveAdmin);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}
/* don't remove the code, this is the template for get and set*/

#if 0
oam_uint32 ctc_oam_template_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_template_t *pData =
        (oam_ctc_onu_template_t *)pOut;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    /* if the oam is port base, need add below, other remove it */
    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_template_get_adapt(xxxx);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d  \n", ret);
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    /* put the data to the pData and convert to network order */
    /* set the pData.hdr.width */

    return return sizeof(oam_var_cont_t) + pData->hdr.width ;
}

oam_uint32 ctc_oam_template_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_template_t *pInData  =
        (oam_ctc_onu_template_t *)pRecv;


    OAM_CTC_FUNCTION_ENTER(index);

    /* if the oam is port base, need add below, other remove it */
    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    /* Get the data from pInData  and convert to host order */

    ret = ctc_oam_template_set_adapt(XXXX);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapt return failed, ret = %d \n", ret);
        pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    }
    else {
        pOutData->width = OAM_CTC_VAR_SET_OK;
    }

    return sizeof(oam_var_cont_t);
}
#endif
/* CTC Handler end*/


ctc_oam_handle_t g_ctc_oam_xlat_table[] = {
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC)   { BRANCH, LEAF, GET_FUNC, SET_FUNC },
    CTC_OAM_BUILD_RESP
};

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_send                                              */
/* CATEGORY   : Device                                                       */
/* ACCESS     : public                                                       */
/* BLOCK      : General                                                      */
/* CHIP       : 8030                                                         */
/*                                                                           */
void ctc_oam_send(
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
/* API to set CTC OAM frame                                                  */
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

    if (g_oam_port == OAM_UNI_PORT_ID1 || g_oam_port == OAM_MGMT_PORT_ID) {
        memcpy(buf + OAM_MACADDR_LEN, intf->macaddr, OAM_MACADDR_LEN);
    }

    if (intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_COMPLETED) {
        oam_send(llid, g_oam_port, buf, len);
        intf->org_specific_tx++;
    }
    else {
        oam_printf("ctc not complete\r\n");
    }

}

/*****************************************************************************/
/* $rtn_hdr_start  oam_get_ctc_port_index_info                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_get_ctc_port_index_info(
    oam_uint8   *data,
    ctc_oam_mgmt_obj_idx_t *mgmtIdx
)
/*                                                                           */
/* INPUTS  : o data - oam branch leaf data                                   */
/* OUTPUTS : o mgmtIdx - ctc mgmt obj index value                            */
/* DESCRIPTION:                                                              */
/* Get the port index according to the branch and leaf                       */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t *hdr = (oam_var_cont_t * )data;
    oam_pdu_ctc_index_t *pIndex = NULL;
    oam_pdu_ctc_port_index_t *pPortIndex = NULL;

    OAM_ASSERT_RET(data != NULL  && mgmtIdx != NULL, OAM_E_PARAM);

    switch (hdr->branch) {
        case OAM_CTC_INDEX_BRANCH: {
            pIndex = (oam_pdu_ctc_index_t *) data;
            mgmtIdx->valid = TRUE;
            mgmtIdx->idxLeaf = ntohs(hdr->leaf);
            mgmtIdx->idxBranch = hdr->branch;
            mgmtIdx->idxValue = pIndex->value;

            if ((mgmtIdx->idxValue >= 0x01 && mgmtIdx->idxValue <= 0x4f)
                || mgmtIdx->idxValue == CTC_PORT_ID_ALL_UNI ) {
                mgmtIdx->idxType = OAM_CTC_ETH_PORT;
            }

            if (mgmtIdx->idxValue >= 0xa1 && mgmtIdx->idxValue <= 0xaf) {
                mgmtIdx->idxType = OAM_CTC_RF_PORT;
            }

            mgmtIdx->idxLen = sizeof(oam_pdu_ctc_index_t);
            return OAM_E_OK;
        }

        case OAM_CTC_INDEX_BRANCH_E: {
            if (ntohs(hdr->leaf) == OAM_CTC_LEAF_CODE_ONU_PORT) {
                pPortIndex = (oam_pdu_ctc_port_index_t *)data;
                mgmtIdx->valid = TRUE;
                mgmtIdx->idxLeaf = ntohs(hdr->leaf);
                mgmtIdx->idxBranch = hdr->branch;
                mgmtIdx->idxValue = ntohs(pPortIndex->portIndex);
                mgmtIdx->idxType = pPortIndex->portType;
                mgmtIdx->idxLen = sizeof(oam_pdu_ctc_port_index_t);
                return OAM_E_OK;
            }
            else if (ntohs(hdr->leaf) == OAM_CTC_LEAF_CODE_ONU_PON_PORT) {
                oam_pdu_ctc_pon_index_t *pPonIndex = (oam_pdu_ctc_pon_index_t *)data;
                mgmtIdx->valid = TRUE;
                mgmtIdx->idxLeaf = ntohs(hdr->leaf);
                mgmtIdx->idxBranch = hdr->branch;
                mgmtIdx->idxValue = ntohl(pPonIndex->ponIndex);
                mgmtIdx->idxType = OAM_CTC_PON_PORT;
                mgmtIdx->idxLen = sizeof(oam_pdu_ctc_pon_index_t);
                return OAM_E_OK;
            }
            else if (ntohs(hdr->leaf) == OAM_CTC_LEAF_CODE_ONU_LLID) {
                oam_pdu_ctc_llid_index_t *pLlidIndex = (oam_pdu_ctc_llid_index_t *)data;
                mgmtIdx->valid = TRUE;
                mgmtIdx->idxLeaf = ntohs(hdr->leaf);
                mgmtIdx->idxBranch = hdr->branch;
                mgmtIdx->idxValue = ntohl(pLlidIndex->llidIndex);
                mgmtIdx->idxLen = sizeof(oam_pdu_ctc_llid_index_t);
                return OAM_E_OK;
            }
        }
    }

    return OAM_E_ERROR;
}


/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_mgmt_index                                  */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32 ctc_oam_build_mgmt_index(
    oam_uint8 *pOut,
    oam_uint8 branch,
    oam_uint16 leaf,
    oam_uint32 index,
    oam_uint8  type)
/*                                                                           */
/* INPUTS  : o  pOut - oam packet                                            */
/*           o  branch - index branch                                        */
/*           o  leaf - index leaf                                            */
/*           o  index - index value                                          */
/*           o  type - port type                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the index TLV for the ctc mgmt                                      */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    OAM_ASSERT_RET(pOut != NULL, 0);

    switch (branch) {
        case OAM_CTC_INDEX_BRANCH: {
            oam_pdu_ctc_index_t *pIndexCont = (oam_pdu_ctc_index_t *)pOut;

            pIndexCont->hdr.branch = OAM_CTC_INDEX_BRANCH;
            pIndexCont->hdr.leaf = htons(OAM_CTC_LEAF_CODE_ONU_PORT);
            pIndexCont->hdr.width = sizeof(oam_uint8);
            pIndexCont->value = index;

            return sizeof(oam_pdu_ctc_index_t);
        }

        case OAM_CTC_INDEX_BRANCH_E: {
            if (leaf == OAM_CTC_LEAF_CODE_ONU_PORT) {
                oam_uint16 portId = index;
                oam_pdu_ctc_port_index_t *pIndex = (oam_pdu_ctc_port_index_t *)pOut;
                pIndex->hdr.branch = branch;
                pIndex->hdr.leaf = htons(leaf);
                pIndex->hdr.width = sizeof(oam_uint32);
                pIndex->portType = type;
                pIndex->slotInfo = 0x00;
                pIndex->portIndex = htons(portId);
                return sizeof(oam_pdu_ctc_port_index_t);
            }
            else if (leaf == OAM_CTC_LEAF_CODE_ONU_PON_PORT) {
                oam_pdu_ctc_pon_index_t *pIndex = (oam_pdu_ctc_pon_index_t *)pOut;
                pIndex->hdr.branch = branch;
                pIndex->hdr.leaf = htons(leaf);
                pIndex->hdr.width = sizeof(oam_uint32);
                pIndex->ponIndex = htonl(index);
                return sizeof(oam_pdu_ctc_pon_index_t);
            }
            else if (leaf == OAM_CTC_LEAF_CODE_ONU_LLID) {
                oam_pdu_ctc_llid_index_t *pIndex = (oam_pdu_ctc_llid_index_t *)pOut;
                pIndex->hdr.branch = branch;
                pIndex->hdr.leaf = htons(leaf);
                pIndex->hdr.width = sizeof(oam_uint32);
                pIndex->llidIndex = htonl(index);
                return sizeof(oam_pdu_ctc_llid_index_t);
            }
        }

    }

    return 0;

}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_header                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32 ctc_oam_build_header(
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
/* build the ctc oam packet header                                           */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_if_t *intf = oam_intf_find(llid);
    oam_pdu_ctc_t *pOutPkt = (oam_pdu_ctc_t *)buf;

    OAM_ASSERT_RET(buf != NULL, 0);

    if (intf == NULL) {
        OAM_ORG_LOG("Unknown llid 0x%x\n", llid);
        return 0;
    }

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)buf, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pOutPkt->oui, oui, OAM_OUI_LEN);
    pOutPkt->opcode = opcode;

    return sizeof(oam_pdu_ctc_t) - 1;
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_get_not_support                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32  ctc_oam_build_get_not_support(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
/*                                                                           */
/* INPUTS  : o  index - ctc oam mgmt index                                   */
/*           o  pRecv - received oam packet                                  */
/*           o  procRecvLen - process the received oam packet length         */
/*           o  pOut - The out oam packet                                    */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the get not support oam                                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t *pOutHdrContainer
        = (oam_var_cont_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    /*For HW OAM, Do NOT reply anything to those unknown OAM TLVs */
#ifdef HAVE_HW_OAM
    return 0;
#endif/*END_HAVE_HW_OAM*/

    pOutHdrContainer->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;

    return sizeof(oam_var_cont_t);
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_set_not_support                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32  ctc_oam_build_set_not_support(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    RecvLen,
    oam_uint8    *pOut)
/*                                                                           */
/* INPUTS  : o  index - ctc oam mgmt index                                   */
/*           o  pRecv - received oam packet                                  */
/*           o  RecvLen - the received oam packet length                     */
/*           o  pOut - The out oam packet                                    */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the set not support oam                                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t *pOutHdrContainer
        = (oam_var_cont_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    /*For HW OAM, Do NOT reply anything to those unknown OAM TLVs */
#ifdef HAVE_HW_OAM
    return 0;
#endif/*END_HAVE_HW_OAM*/

    pOutHdrContainer->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;

    return sizeof(oam_var_cont_t);
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_set_dumb_support                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32  ctc_oam_build_set_dumb_support(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    RecvLen,
    oam_uint8    *pOut)
/*                                                                           */
/* INPUTS  : o  index - ctc oam mgmt index                                   */
/*           o  pRecv - received oam packet                                  */
/*           o  RecvLen - the received oam packet length                     */
/*           o  pOut - The out oam packet                                    */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the set not support oam                                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t *pOutHdrContainer
        = (oam_var_cont_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    /*For HW OAM, Do NOT reply anything to those unknown OAM TLVs */
#ifdef HAVE_HW_OAM
    return 0;
#endif/*END_HAVE_HW_OAM*/

    pOutHdrContainer->width = OAM_CTC_VAR_SET_OK;

    return sizeof(oam_var_cont_t);
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_find_get_handler                                  */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
ctc_oam_get_handler_t ctc_oam_find_get_handler(
    oam_oui_t oui,
    oam_uint8 branch,
    oam_uint16 leaf)
/*                                                                           */
/* INPUTS  : o  branch - the ctc oam branch id                               */
/*           o  leaf - the ctc oam leaf id                                   */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* find the get handler for special branch and leaf                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    ctc_oam_get_handler_t  get_handler = NULL;
    oam_uint32 i = 0;

    /* Call vendor handle first*/
    if (oam_is_oui_equal(oui, g_ctc_oam_vendor_handles.oui)) {

        for (i = 0 ; i < g_ctc_oam_vendor_handles.num; i ++) {

            if (g_ctc_oam_vendor_handles.handlers[i].branch == branch
                && g_ctc_oam_vendor_handles.handlers[i].leaf == leaf) {

                OAM_ORG_DEBUG("ctc oam handler found by vendor: branch %#x leaf %#x\n",
                              branch, leaf);
                get_handler = g_ctc_oam_vendor_handles.handlers[i].get_func;
                break;
            }
        }
    }

    /* if vendor not found, call ctc std handler*/
    if (get_handler == NULL ) {
        for (i = 0 ;
             i < sizeof(g_ctc_oam_xlat_table) / sizeof(g_ctc_oam_xlat_table[0]) ; i++) {

            if (g_ctc_oam_xlat_table[i].branch == branch
                && g_ctc_oam_xlat_table[i].leaf == leaf) {

                OAM_ORG_DEBUG("ctc oam handler found: branch %#x leaf %#x\n",
                              branch, leaf);
                get_handler =  g_ctc_oam_xlat_table[i].get_func;
                break;
            }
        }
    }

    return get_handler;
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_find_set_handler                                  */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
ctc_oam_set_handler_t ctc_oam_find_set_handler(
    oam_oui_t oui,
    oam_uint8 branch,
    oam_uint16 leaf)
/*                                                                           */
/* INPUTS  : o  branch - the ctc oam branch id                               */
/*           o  leaf - the ctc oam leaf id                                   */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* find the set handler for special branch and leaf                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    ctc_oam_set_handler_t  set_handler = NULL;
    oam_uint32 i = 0;

    /* Call vendor handle first*/
    if (oam_is_oui_equal(oui, g_ctc_oam_vendor_handles.oui)) {

        for (i = 0 ; i < g_ctc_oam_vendor_handles.num; i ++) {

            if (g_ctc_oam_vendor_handles.handlers[i].branch == branch
                && g_ctc_oam_vendor_handles.handlers[i].leaf == leaf) {

                OAM_ORG_DEBUG("ctc oam handler found by vendor: branch %#x leaf %#x\n",
                              branch, leaf);
                set_handler = g_ctc_oam_vendor_handles.handlers[i].set_func;
                break;
            }
        }
    }

    /* if vendor not found, call ctc std handler*/
    if (set_handler == NULL ) {
        for (i = 0 ;
             i < sizeof(g_ctc_oam_xlat_table) / sizeof(g_ctc_oam_xlat_table[0]) ; i++) {

            if (g_ctc_oam_xlat_table[i].branch == branch
                && g_ctc_oam_xlat_table[i].leaf == leaf) {

                OAM_ORG_DEBUG("ctc oam handler found: branch %#x leaf %#x\n",
                              branch, leaf);
                set_handler =  g_ctc_oam_xlat_table[i].set_func;
                break;
            }
        }
    }

    return set_handler;
}


/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_get_tlv                                     */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32 ctc_oam_build_get_tlv(
    oam_oui_t oui,
    oam_uint8 *pOutVarCont,
    oam_var_desc_t *pRecvVarCont,
    oam_uint32 *procLen,
    ctc_oam_mgmt_obj_idx_t mgmtIndex,
    oam_boolean *idxBuild)
/*                                                                           */
/* INPUTS  : o  pOutVarCont - the send oam packet                            */
/*           o  pRecevVarCont - the recevied oam packet                      */
/*           o  procLen - the length of process the received packet          */
/*           o  mgmtIndex - the ctc obj mgmt index                           */
/*           o  idxBuild - the index tlv build flag                          */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* build the get branch leaf packet                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_var_cont_t *pOut
        = (oam_var_cont_t *) pOutVarCont;
    oam_uint32 len = 0, outPktLen = 0;
    oam_uint32 length = 0;
    ctc_oam_get_handler_t get_handler = NULL;
    oam_boolean buildFlag = FALSE;

    OAM_ASSERT_RET(idxBuild != NULL
                   && pRecvVarCont != NULL
                   && pOutVarCont != NULL
                   && procLen != NULL, 0);

    /* default get only process the branch and leaf,
     * but the alarm config need process more than it */
    *procLen = sizeof(oam_var_desc_t);

    get_handler = ctc_oam_find_get_handler(oui, pRecvVarCont->branch,
                                           ntohs(pRecvVarCont->leaf));

    if (get_handler == NULL) {
        get_handler = ctc_oam_build_get_not_support;
    }

    /*no index include*/
    if (!mgmtIndex.valid) {
        OAM_ORG_DEBUG(" No index branch %#x, leaf %#x \n",
                      pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));
        pOut->branch = pRecvVarCont->branch;
        pOut->leaf = pRecvVarCont->leaf;
        outPktLen = get_handler(mgmtIndex, (oam_uint8 *)pRecvVarCont,
                                procLen, pOutVarCont);
    }
    else {
        /* single index */
        if (mgmtIndex.idxValue != CTC_PORT_ID_ALL_UNI
            && mgmtIndex.idxValue != CTC_2_1_PORT_ID_ALL_UNI) {
            OAM_ORG_DEBUG(" single index branch %#x, leaf %#x \n",
                          pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));

            if (!*idxBuild) {
                OAM_ORG_DEBUG("build index branch = %#x,leaf = %#x,value = %d \n",
                              mgmtIndex.idxBranch,
                              mgmtIndex.idxLeaf,
                              mgmtIndex.idxValue);

                outPktLen = ctc_oam_build_mgmt_index(pOutVarCont,
                                                     mgmtIndex.idxBranch,
                                                     mgmtIndex.idxLeaf,
                                                     mgmtIndex.idxValue,
                                                     mgmtIndex.idxType);

                OAM_ORG_DEBUG("build index len = %d \n", outPktLen);
                *idxBuild = TRUE;
                buildFlag = TRUE;
                pOutVarCont += outPktLen;
            }

            pOut = (oam_var_cont_t *) pOutVarCont;
            pOut->branch = pRecvVarCont->branch;
            pOut->leaf = pRecvVarCont->leaf;

            if ((mgmtIndex.idxType == OAM_CTC_ETH_PORT) &&
                (mgmtIndex.idxValue > CTC_MAX_ONU_UNI_PORTS) &&
                (get_handler != ctc_oam_build_get_not_support)) {
                OAM_ORG_LOG("index = %d  is  greater than max %d \n",
                            mgmtIndex.idxValue, CTC_MAX_ONU_UNI_PORTS);
                pOut->width = OAM_CTC_VAR_SET_BAD_PARAM;
                outPktLen += sizeof(oam_var_cont_t);
            }
            else {
                len = get_handler(mgmtIndex, (oam_uint8 *)pRecvVarCont,
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
        else { /* All index*/
            oam_uint32 i = 0;
            OAM_ORG_DEBUG("all indexs branch %#x, leaf %#x \n",
                          pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));

            for (i = OAM_UNI_PORT_ID1; i <= CTC_MAX_ONU_UNI_PORTS; i++) {
                mgmtIndex.idxValue = i;
                len = ctc_oam_build_mgmt_index(pOutVarCont,
                                               mgmtIndex.idxBranch,
                                               mgmtIndex.idxLeaf,
                                               mgmtIndex.idxValue,
                                               mgmtIndex.idxType);
                *idxBuild = TRUE;
                /*shift the index TLV*/
                pOutVarCont += len;
                outPktLen += len;
                pOut = (oam_var_cont_t *)pOutVarCont;
                pOut->branch = pRecvVarCont->branch;
                pOut->leaf = pRecvVarCont->leaf;

                OAM_ORG_DEBUG("build index branch = %#x,leaf = %#x,value = %d \n",
                              mgmtIndex.idxBranch,
                              mgmtIndex.idxLeaf,
                              mgmtIndex.idxValue);

                length = get_handler(mgmtIndex, (oam_uint8 *)pRecvVarCont,
                                     procLen, pOutVarCont);

                if (length == 0) {
                    pOutVarCont -= len;
                    outPktLen -= len;
                    *idxBuild = FALSE;
                    break;
                }

                /*shift the real TLV*/
                pOutVarCont += length;
                outPktLen += length;
            }

        }
    }

    return outPktLen;
}


/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_set_tlv                                     */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32 ctc_oam_build_set_tlv(
    oam_oui_t oui,
    oam_uint8 *pOutVarCont,
    oam_uint8 *pRecv,
    oam_uint32  tlvLen,
    ctc_oam_mgmt_obj_idx_t mgmtIndex,
    oam_boolean *idxBuild)
/*                                                                           */
/* INPUTS  : o  pOutVarCont - the send oam packet                            */
/*           o  pRecv - the recevied oam packet                              */
/*           o  tlvLen - the received packet  length                         */
/*           o  mgmtIndex - the ctc obj mgmt index                           */
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
    ctc_oam_set_handler_t set_handler = NULL;
    oam_boolean buildFlag = FALSE;

    OAM_ASSERT_RET(idxBuild != NULL
                   && pRecvVarCont != NULL
                   && pOutVarCont != NULL, 0);

    /*oam_buf_print(pRecv, tlvLen + sizeof(oam_var_cont_t));*/

    set_handler = ctc_oam_find_set_handler(oui, pRecvVarCont->branch,
                                           ntohs(pRecvVarCont->leaf));

    if (set_handler == NULL) {
        set_handler = ctc_oam_build_set_dumb_support;
    }

    /*no index include*/
    if (!mgmtIndex.valid) {
        OAM_ORG_DEBUG("No index branch %#x, leaf %#x \n",
                      pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));
        pOut->branch = pRecvVarCont->branch;
        pOut->leaf = pRecvVarCont->leaf;
        outPktLen = set_handler(mgmtIndex, pRecv, tlvLen, pOutVarCont);
    }
    else {
        /* single index */
        if (mgmtIndex.idxValue != CTC_PORT_ID_ALL_UNI
            && mgmtIndex.idxValue != CTC_2_1_PORT_ID_ALL_UNI) {
            OAM_ORG_DEBUG(" single index branch %#x, leaf %#x \n",
                          pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));

            if (!*idxBuild) {
                OAM_ORG_DEBUG("build index branch = %#x,leaf = %#x, value = %d \n",
                              mgmtIndex.idxBranch,
                              mgmtIndex.idxLeaf,
                              mgmtIndex.idxValue);

                outPktLen = ctc_oam_build_mgmt_index(pOutVarCont,
                                                     mgmtIndex.idxBranch,
                                                     mgmtIndex.idxLeaf,
                                                     mgmtIndex.idxValue,
                                                     mgmtIndex.idxType);

                OAM_ORG_DEBUG("build index len = %d \n", outPktLen);
                *idxBuild = TRUE;
                buildFlag = TRUE;
                pOutVarCont += outPktLen;
            }

            pOut = (oam_var_cont_t *) pOutVarCont;
            pOut->branch = pRecvVarCont->branch;
            pOut->leaf = pRecvVarCont->leaf;


            if ((mgmtIndex.idxValue > CTC_MAX_ONU_UNI_PORTS)
                && (set_handler != ctc_oam_build_set_not_support)) {
                OAM_ORG_LOG("index = %d  is  greater than max %d \n",
                            mgmtIndex.idxValue, CTC_MAX_ONU_UNI_PORTS);
                pOut->width = OAM_CTC_VAR_SET_BAD_PARAM;
                outPktLen += sizeof(oam_var_cont_t);
            }
            else {
                len = set_handler(mgmtIndex, pRecv, tlvLen, pOutVarCont);

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
        else { /* All index*/
            oam_uint32 i = 0;
            OAM_ORG_DEBUG("all indexs branch %#x, leaf %#x \n",
                          pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));

            if (!*idxBuild) {
                outPktLen = ctc_oam_build_mgmt_index(pOutVarCont,
                                                     mgmtIndex.idxBranch,
                                                     mgmtIndex.idxLeaf,
                                                     mgmtIndex.idxValue,
                                                     mgmtIndex.idxType);
                *idxBuild = TRUE;
                buildFlag = TRUE;
                /*shift the index TLV*/
                pOutVarCont += outPktLen;
            }

            /* we only need build one response with port index 0xffff*/
            pOut = (oam_var_cont_t *)pOutVarCont;
            pOut->branch = pRecvVarCont->branch;
            pOut->leaf = pRecvVarCont->leaf;

            for (i = OAM_UNI_PORT_ID1; i <= CTC_MAX_ONU_UNI_PORTS; i++) {
                mgmtIndex.idxValue = i;

                OAM_ORG_DEBUG("build index branch = %#x,leaf = %#x,value = %d \n",
                              mgmtIndex.idxBranch,
                              mgmtIndex.idxLeaf,
                              mgmtIndex.idxValue);
                len = set_handler(mgmtIndex, pRecv, tlvLen, pOutVarCont);

                if ((len == 0) && buildFlag) {
                    pOutVarCont -= outPktLen;
                    outPktLen = 0;
                    *idxBuild = FALSE;
                    break;
                }

                /*check the set return right or not */
                if (pOut->width != OAM_CTC_VAR_SET_OK) {
                    break;
                }
            }

            /*shift the real TLV*/
            pOutVarCont += len;
            outPktLen += len;

        }
    }

    return outPktLen;
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_var_resp                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32 ctc_oam_build_var_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*           o  frame - Container buffer to store the composed OAM frame     */
/*           o  lenght - OAM frame lengtjh                                   */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* API to handler CTC OAM var request                                        */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_uint8    outPdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_ctc_t   *pOutPkt = (oam_pdu_ctc_t *)pdu;
    oam_pdu_ctc_t   *pRecvPkt = NULL;
    oam_var_desc_t   *pRecvVarCont = NULL;
    oam_uint8    *pOutVarCont = NULL;
    oam_int32   totalContentLen = 0, outPktLen = 0;
    oam_boolean  isFirstGetTlv = TRUE,  idxBuild = FALSE;
    ctc_oam_mgmt_obj_idx_t  mgmtIndex;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length >=
                   sizeof(oam_pdu_ctc_t) + sizeof(oam_var_desc_t) - 1, 0);

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    pRecvPkt = (oam_pdu_ctc_t *)frame;
    outPktLen = ctc_oam_build_header(pRecvPkt->oui, llid, pdu,
                                     OAM_PDU_CTC_VAR_RESP);
    pOutVarCont = pOutPkt->data;

    /*shift the header of received packet*/
    totalContentLen = length - sizeof(oam_pdu_ctc_t) + 1;
    pRecvVarCont = (oam_var_desc_t *)pRecvPkt->data;

    memset(&mgmtIndex, 0, sizeof(ctc_oam_mgmt_obj_idx_t));
    mgmtIndex.valid = FALSE;

    while (totalContentLen > 0) {
        oam_uint32 len = 0;
        oam_uint32 procLen = 0;
        OAM_ORG_DEBUG("VAR REQ branch 0x%x, leaf 0x%x\n",
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

        if (OAM_E_OK == oam_get_ctc_port_index_info(
                (oam_uint8 *)pRecvVarCont, &mgmtIndex)) {
            /*shift the index TLV*/
            totalContentLen -= mgmtIndex.idxLen;
            pRecvVarCont = (oam_var_desc_t *)
                           ((oam_uint8 *)pRecvVarCont + mgmtIndex.idxLen);
            idxBuild = FALSE;
            OAM_ORG_DEBUG("Index branch :%#x , leaf:%#x value:%#x \n",
                          mgmtIndex.idxBranch, mgmtIndex.idxLeaf, mgmtIndex.idxValue);
            continue;
        }

        memset(outPdu, 0, OAM_MAX_OAMPDU_SIZE);

        len = ctc_oam_build_get_tlv(pRecvPkt->oui, outPdu, pRecvVarCont,
                                    &procLen, mgmtIndex, &idxBuild);

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
        if (totalContentLen
            < sizeof(oam_var_desc_t)) {
            break;
        }

        pRecvVarCont = (oam_var_desc_t *)
                       ((oam_uint8 *)pRecvVarCont +  procLen);
        isFirstGetTlv = FALSE;
    }

    /* packet 2  0's to mark the end - 57.6.3 a)*/
    *pOutVarCont++ = 0;
    *pOutVarCont++ = 0;
    outPktLen += 2;

    if (outPktLen > (sizeof(oam_pdu_ctc_t) - 1 + 2)) {
        ctc_oam_send(llid, pdu, outPktLen);
    }

    return outPktLen;
}


/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_set_resp                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32 ctc_oam_build_set_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*           o  frame - Container buffer to store the composed OAM frame     */
/*           o  lenght - OAM frame lengtjh                                   */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* API to handler CTC OAM set request                                        */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_uint8    outPdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_ctc_t *pOutPkt = (oam_pdu_ctc_t *)pdu;
    oam_pdu_ctc_t *pRecvPkt = NULL;
    oam_ctc_onu_var_set_hdr_t *pRecvVarCont = NULL;
    oam_uint8 *pOutVarCont = NULL;
    oam_uint8 *pInVarCont = NULL;
    oam_int32 totalContentLen = 0;
    oam_uint32 outPktLen = 0;
    oam_int32  len = 0;

    oam_boolean  isFirstSetTLV = TRUE,  idxBuild = FALSE;
    ctc_oam_mgmt_obj_idx_t  mgmtIndex;
    oam_uint8 lastBranch = OAM_ATTRIB_BRANCH_NULL;
    oam_uint16 lastLeaf = 0;
    oam_uint8 tlvLen = 0;
    oam_uint8 *pLastRecvVarCont = NULL;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length >=
                   sizeof(oam_pdu_ctc_t) + sizeof(oam_var_desc_t) - 1, 0);

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    pRecvPkt = (oam_pdu_ctc_t *)frame;
    outPktLen = ctc_oam_build_header(pRecvPkt->oui, llid, pdu,
                                     OAM_PDU_CTC_SET_RESP);
    pOutVarCont = pOutPkt->data;

    /*shift the header of received packet*/
    totalContentLen = length - sizeof(oam_pdu_ctc_t) + 1;
    pRecvVarCont = (oam_ctc_onu_var_set_hdr_t *)pRecvPkt->data;

    pInVarCont = (oam_uint8 *)pRecvVarCont;

    mgmtIndex.valid = FALSE;


    while (totalContentLen > 0) {
        OAM_ORG_DEBUG("SET REQ branch 0x%x, leaf 0x%x\n",
                      pRecvVarCont->branch, ntohs(pRecvVarCont->leaf));

        if (pRecvVarCont->branch == OAM_ATTRIB_BRANCH_NULL) {
            if (isFirstSetTLV) {
                OAM_ORG_LOG("Bad packet \n");
                return 0;
            }
            else {
                /*End TLV, break and send*/
                break;
            }
        }

        if (OAM_E_OK == oam_get_ctc_port_index_info((oam_uint8 *)pRecvVarCont,
                &mgmtIndex)) {
            /*shift the index TLV*/
            totalContentLen -= mgmtIndex.idxLen;
            pInVarCont += mgmtIndex.idxLen;
            pRecvVarCont = (oam_ctc_onu_var_set_hdr_t *)pInVarCont;

            idxBuild = FALSE;
            OAM_ORG_DEBUG("Index branch :%#x , leaf:%#x value:%#x \n",
                          mgmtIndex.idxBranch, mgmtIndex.idxLeaf, mgmtIndex.idxValue);
            continue;
        }

        /* check the duplicate TLV */
        if (lastBranch == pRecvVarCont->branch
            && lastLeaf == ntohs(pRecvVarCont->leaf)) {
            oam_uint32 tmpLen;

            OAM_ORG_DEBUG("duplicate branch:%#x, leaf:%#x , len = %d\n",
                          pRecvVarCont->branch , ntohs(pRecvVarCont->leaf),
                          pRecvVarCont->width);

            /* special set oam only has branch and leaf, no length*/
            if (ctc_oam_is_odd_set_oam(
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
            /*is not first tlv*/
            if (lastBranch != OAM_ATTRIB_BRANCH_NULL
                && lastLeaf != 0) {
                OAM_ORG_DEBUG("process the previous TLV, branch:%#x, leaf:%#x \n",
                              lastBranch, lastLeaf);
                /* process the pLastRecvVarCont */
                memset(outPdu, 0, OAM_MAX_OAMPDU_SIZE);
                len = ctc_oam_build_set_tlv(pRecvPkt->oui, outPdu,
                                            pLastRecvVarCont, tlvLen, mgmtIndex, &idxBuild);

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

            lastBranch = pRecvVarCont->branch;
            lastLeaf = ntohs(pRecvVarCont->leaf);
            pLastRecvVarCont = pInVarCont;

            /* special set oam only has branch and leaf, no length*/
            if (ctc_oam_is_odd_set_oam(
                    pRecvVarCont->branch, ntohs(pRecvVarCont->leaf))) {
                tlvLen = 0;
                pInVarCont += (sizeof(oam_var_cont_t) - 1);
                totalContentLen -= (sizeof(oam_var_cont_t) - 1);
            }
            else {
                tlvLen = pRecvVarCont->width;
                pInVarCont += tlvLen + sizeof(oam_var_cont_t);
                totalContentLen -= (tlvLen + sizeof(oam_var_cont_t));
            }

        }

        if (totalContentLen
            < sizeof(oam_var_desc_t)) {
            break;
        }

        isFirstSetTLV = FALSE;
        pRecvVarCont = (oam_ctc_onu_var_set_hdr_t *)pInVarCont;
    }

    /* process the Last TLV */
    if (pLastRecvVarCont != NULL) {
        OAM_ORG_DEBUG("process the final one TLV \n");
        memset(outPdu, 0, OAM_MAX_OAMPDU_SIZE);
        len = ctc_oam_build_set_tlv(pRecvPkt->oui, outPdu,
                                    pLastRecvVarCont, tlvLen, mgmtIndex, &idxBuild);

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
    *pOutVarCont++ = 0;
    outPktLen += 2;

    if (outPktLen > (sizeof(oam_pdu_ctc_t) - 1 + 2)) {
        ctc_oam_send(llid, pdu, outPktLen);
    }

    return outPktLen;
}

void ctc_oam_keep_silence_timer_expire(void *data)
{
    oam_timer_msg_send(OAM_KEEP_SILENCE_TIMER_EXPIRE, data);
}

void ctc_oam_keep_silence_proc(void *data)
{
    oam_uint8 *index = (oam_uint8 *)data;
    g_keep_silence = FALSE;
    g_auth_fail = FALSE;

    if (index == NULL) {
        OAM_ORG_LOG("Invalid data\n");
        return;
    }

    OAM_ORG_DEBUG("End keep silence for llid index %d\r\n", *index);

    ctc_oam_auth_keep_silence_set_adapt(*index, FALSE);

    OAM_FREE(data);
}

void ctc_oam_llid_deregister_handle(oam_llid_t llid, oam_uint8 index)
{
    oam_uint8   time = 60;
    oam_uint8   *data = NULL;

    if (OAM_E_OK != ctc_oam_keep_silence_time_get_adapt(&time)) {
        time = 60;
    }

    if (g_auth_fail) {
        if (!g_keep_silence) {
            OAM_ORG_DEBUG("Start keep silence for llid index %d\r\n", index);

            ctc_oam_auth_keep_silence_set_adapt(index, TRUE);
            g_keep_silence = TRUE;

            data = OAM_MALLOC(sizeof(oam_uint8));

            if (data) {
                *data = index;
                ca_timer_add((time * 1000), ctc_oam_keep_silence_timer_expire, (void *)data);
            }
        }
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_build_ctc_auth_resp                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32
ctc_oam_build_auth_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
/*                                                                           */
/* INPUTS  : o  llid   - llid port                                           */
/*           o  frame  - Container buffer to store the composed OAM frame    */
/*           o  length - Size of OAM frame                                   */
/* OUTPUTS : ----                                                            */
/* RETURNS : Size of OAM frame                                               */
/* DESCRIPTION:                                                              */
/* API to compose CTC auth response OAM frame                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8   pdu[OAM_MAX_OAMPDU_SIZE];
    oam_uint32  outPktLen = 0;
    oam_uint8   loid[CTC_AUTH_LOID_LEN + 1];
    oam_uint8   pswd[CTC_AUTH_PSWD_LEN + 1];
    oam_int32   loid_len = 0;
    oam_int32   pswd_len = 0;
    oam_status  ret = OAM_E_OK;

    oam_ctc_pdu_auth_t *pInAuth  = (oam_ctc_pdu_auth_t *)frame;
    oam_ctc_pdu_auth_t *pOutAuth = (oam_ctc_pdu_auth_t *)pdu;

    oam_ctc_auth_data_t *pInAuthData  = (oam_ctc_auth_data_t *)pInAuth->auth_data;
    oam_ctc_auth_data_t *pOutAuthData = (oam_ctc_auth_data_t *)pOutAuth->auth_data;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    /*build response header */
    ctc_oam_build_header(pInAuth->oui, llid, pdu, OAM_PDU_CTC_ONU_AUTH);

    outPktLen =  sizeof(oam_ctc_pdu_auth_t) - 1;

    if (pInAuth->auth_code == OAM_CTC_AUTH_FAILED) {
        g_auth_fail = TRUE;
        ctc_oam_auth_keep_silence_set_adapt(0, TRUE);
    }
    else {
        g_auth_fail = FALSE;
    }

    switch (pInAuth->auth_code) {
        case OAM_CTC_AUTH_REQUEST: {
            pOutAuth->auth_code = OAM_CTC_AUTH_RESPONSE;

            if (OAM_CTC_AUTH_TYPE_LOID_PASSWD == pInAuthData->auth_type) {
                pOutAuthData->auth_type = OAM_CTC_AUTH_TYPE_LOID_PASSWD;

                memset(loid, 0, CTC_AUTH_LOID_LEN + 1);
                memset(pswd, 0, CTC_AUTH_PSWD_LEN + 1);

                ret = ctc_oam_loid_passwd_get_adapt(loid, pswd);

                if (ret != OAM_E_OK) {
                    OAM_ORG_LOG_INFO("Get loid password fail!\n");
                }

                loid_len = strlen((oam_char *)loid);
                pswd_len = strlen((oam_char *)pswd);

                memset(pOutAuthData->u.s.loid, 0, CTC_AUTH_LOID_LEN);
                memset(pOutAuthData->u.s.pswd, 0, CTC_AUTH_PSWD_LEN);

                /* loid should put into the end of the buffer, and fill ASSIC NUL in the front */
                if (loid_len > 0 && loid_len <= CTC_AUTH_LOID_LEN) {
                    memcpy(&pOutAuthData->u.s.loid[CTC_AUTH_LOID_LEN - loid_len],
                           loid,
                           loid_len);
                }

                /* password should put into the end of the buffer, and fill ASSIC NUL in the front */
                if (pswd_len > 0 && pswd_len <= CTC_AUTH_PSWD_LEN) {
                    memcpy(&pOutAuthData->u.s.pswd[CTC_AUTH_PSWD_LEN - pswd_len],
                           pswd,
                           pswd_len);
                }

                pOutAuth->len = htons(sizeof(oam_ctc_auth_data_t));
                outPktLen += sizeof(oam_ctc_auth_data_t);
            }
            else {
                pOutAuthData->auth_type = OAM_CTC_AUTH_TYPE_UNSUPPORT;
                pOutAuthData->u.data[0] = OAM_CTC_AUTH_TYPE_LOID_PASSWD;
                pOutAuth->len = htons(0x02);
                outPktLen += 0x02;
            }

            ctc_oam_send(llid, pdu, outPktLen);

            break;
        }

        case OAM_CTC_AUTH_SUCCESS:
            /*auth success and keep register*/
            OAM_ORG_LOG_INFO("Auth successful!\n");
            break;

        case OAM_CTC_AUTH_FAILED:
            OAM_ORG_LOG("Auth fail for reason(%d)\n", pInAuth->auth_data[0]);
            break;

        default:
            break;
    }

    return 0;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_proc_ctc_churning                                     */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32
ctc_oam_proc_churning(
    oam_uint32 llid,
    oam_uint8 *frame,
    oam_uint32 length )
/*                                                                           */
/* INPUTS  : o  llid   - llid port                                           */
/*           o  frame  - Container buffer to store the composed OAM frame    */
/*           o  length - Size of OAM frame                                   */
/* OUTPUTS : ----                                                            */
/* RETURNS : Size of OAM frame                                               */
/* DESCRIPTION:                                                              */
/* API to compose CTC churning response OAM frame                            */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_uint32 outPktLen = 0;
    oam_if_t *intf = oam_intf_find(llid);
    oam_ctc_onu_new_key_req_t *churning_req = NULL;
    oam_ctc_onu_new_key_t *churning_rsp = NULL;
    oam_pdu_ctc_t *pPkt = (oam_pdu_ctc_t *)pdu;
    oam_pdu_ctc_t *pRecv = (oam_pdu_ctc_t *)frame;
    oam_uint8   key[CHURNING_KEY_10G_LEN];
    oam_uint8   key_len = 0;
    oam_status ret = OAM_E_OK;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length >=  sizeof(oam_pdu_ctc_t) + sizeof(
                       oam_ctc_onu_new_key_req_t) - 1, 0);
    OAM_ASSERT_RET(intf != NULL, 0);

    memset(pdu, 0x00, OAM_MAX_OAMPDU_SIZE);
    memset(key, 0x00, CHURNING_KEY_10G_LEN);

    /* build respone header*/
    ctc_oam_build_header(pRecv->oui, llid, pdu, OAM_PDU_CTC_CHURNING);

    churning_req = (oam_ctc_onu_new_key_req_t *)(frame + sizeof(
                       oam_pdu_ctc_t) - 1 );
    churning_rsp = (oam_ctc_onu_new_key_t *)&pPkt->data[0];

    if (churning_req->churningCode != OAM_PDU_CTC_CODE_KEY_REQ
        || KEY_INDEX_MAX < churning_req->inUserKeyIdx ) {
        OAM_ORG_DEBUG("Wrong churning code:%d or wrong using index %d \n",
                      churning_req->churningCode, churning_req->inUserKeyIdx);
        return 0;
    }

    churning_rsp->churningCode = OAM_PDU_CTC_CODE_NEW_KEY;
    churning_rsp->inUserKeyIdx = !(churning_req->inUserKeyIdx);

    OAM_ORG_DEBUG("LLID %d key index is %d\n", llid, churning_rsp->inUserKeyIdx);

    /* process key index retransmition */
    #if 0  /*Bug 42748, encryption is implemented by hw, if the request key index is same, oam still call sal api to get key*/
    if (intf->last_churning_idx == churning_rsp->inUserKeyIdx) {
        OAM_ORG_DEBUG("LLID %d key index %d retransmition.\n", llid,
                      churning_rsp->inUserKeyIdx);

        key_len = intf->churning_key_len;
        memcpy(key, intf->last_churning_key, CHURNING_KEY_10G_LEN);
    }
    else 
    #endif
    {
        /* return random value */
        ret = ctc_oam_enc_key_get_adapt(intf->index, churning_rsp->inUserKeyIdx,
                                        &key[0], &key_len);

        if (ret != OAM_E_OK) {
            OAM_ORG_DEBUG("Get encryption key failed, ret = %d.\n", ret);
            return 0;
        }
    }

    if (CHURNING_KEY_LEN == key_len)
    {  /* 1G */
        memcpy(&churning_rsp->churningKey[0], &key[0], CHURNING_KEY_LEN);

        OAM_ORG_DEBUG("RSP Churning key: 0x[%02x, %02x, %02x]\n",
                      churning_rsp->churningKey[0], churning_rsp->churningKey[1],
                      churning_rsp->churningKey[2]);

        outPktLen = sizeof(oam_pdu_ctc_t) - 1 + sizeof(oam_ctc_onu_new_key_t) -
                    (CHURNING_KEY_10G_LEN - CHURNING_KEY_LEN);
    }
    else if (CHURNING_KEY_10G_LEN == key_len)
    { /* 10G */
        memcpy(&churning_rsp->churningKey[0], &key[0], CHURNING_KEY_10G_LEN);

        OAM_ORG_DEBUG("RSP Churning key: 0x[%02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x]\n",
                      churning_rsp->churningKey[0], churning_rsp->churningKey[1],
                      churning_rsp->churningKey[2], churning_rsp->churningKey[3],
                      churning_rsp->churningKey[4], churning_rsp->churningKey[5],
                      churning_rsp->churningKey[6], churning_rsp->churningKey[7],
                      churning_rsp->churningKey[8]);

        outPktLen = sizeof(oam_pdu_ctc_t) - 1 + sizeof(oam_ctc_onu_new_key_t);
    }
    else
    {
        /* Invalid len */
        OAM_ORG_DEBUG("Wrong key len %d.\n", key_len);

        return 0;
    }

    /* update key info in intf */
    intf->churning_key_len  = key_len;
    intf->last_churning_idx = churning_rsp->inUserKeyIdx;
    memcpy(intf->last_churning_key, key, CHURNING_KEY_10G_LEN);

    ctc_oam_send(llid, pdu, outPktLen);

    return 0;
}


#if 0
/*****************************************************************************/
/* $rtn_hdr_start  oam_build_ctc_var_indication                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32
oam_build_ctc_var_indication(
    oam_llid_t port,
    oam_int8 *frame,
    oam_int32 length)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*     o  frame - Container buffer to store the composed OAM frame           */
/*     o  length - Size of OAM frame                                         */
/* OUTPUTS : ----                                                            */
/* RETURNS : Size of OAM frame                                               */
/* DESCRIPTION:                                                              */
/* API to compose CTC variable indication OAM frame                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_pdu_ctc_t *pPkt = (oam_pdu_ctc_t *)pdu;
    oam_pdu_ctc_t *pRecvPkt = NULL;
    oam_var_cont_t *pVarCont = NULL;
    oam_var_cont_t *pRecvVarCont = NULL;
    oam_uint32 outPktLen = 0;
    oam_callback_context_t     context;

    if (!frame ||
        length <= sizeof(oam_pdu_ctc_t) + sizeof(oam_var_cont_t) - 1)
        return 0;

    if (!intf) {
        OAM_ORG_LOG("oam_build_ctc_var_indication at unknown llid 0x%x\n", llid);
        return 0;
    }

    /*first copy received packet into output packet - sec 6.5.3.2*/
    memcpy(pdu, frame, length);
    outPktLen =  length;

    pRecvPkt = (oam_pdu_ctc_t *)frame;
    pRecvVarCont = (oam_var_cont_t *)pRecvPkt->data;

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pPkt->oui, oam_oui_ctc, OAM_OUI_LEN);
    pVarCont = (oam_var_cont_t *)pPkt->data;
    pVarCont->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;

    oam_send(llid, OAM_PON_PORT_ID, pdu, outPktLen);
    intf->org_specific_tx++;

    return outPktLen;
}
#endif

/*****************************************************************************/
/* $rtn_hdr_start  oam_build_pdu_dbagetrsp                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32
oam_build_pdu_dbagetrsp(
    oam_llid_t llid,
    oam_ctc_onu_dba_struct *queueSets)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*     o  queueSets - Container buffer to store the composed OAM frame       */
/* OUTPUTS : ----                                                            */
/* RETURNS : Size of OAM frame                                               */
/* DESCRIPTION:                                                              */
/* API to compose CTC DBA get response OAM frame                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 frame[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dbagetrsp_t *pdu = NULL;
    oam_int32 len = 0, i = 0, j = 0;
    oam_uint8 *pos = NULL;
    oam_uint16 val16 = 0;

    pdu = (oam_pdu_dbagetrsp_t *) frame;
    len = sizeof(oam_pdu_dbagetrsp_t) - 1;

    /*build response header*/
    ctc_oam_build_header(oam_oui_ctc, llid, frame, OAM_DBA_EXT_OPCODE);

    pdu->dba_code = OAM_DBA_CODE_GET_RSP;
    pos = &(pdu->queueset_start);

    for (i = 0; i < queueSets->numQueueSet; i++) {

        *pos = queueSets->queueSet[i].reportBitmap;
        pos = pos + 1;
        len = len + 1;

        for (j = 0; j < CTC_ONU_MAX_QUEUE; j++) {
            if ((queueSets->queueSet[i].reportBitmap & (1 << j)) != 0) {
                val16 = htons(queueSets->queueSet[i].queueThreshold[j]);
                memcpy(pos, &val16, sizeof(val16));
                pos = pos + sizeof(val16);
                len = len + sizeof(val16);
            }
        }
    }

    pdu->num_queueset = queueSets->numQueueSet + 1;

    *pos = 0;
    len++;
    ctc_oam_send(llid, frame, len);

    return len;
}


/*****************************************************************************/
/* $rtn_hdr_start  oam_build_pdu_dbasetrsp                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32 oam_build_pdu_dbasetrsp(
    oam_if_t *intf,
    oam_pdu_dbasetreq_t *pdu,
    oam_int32 len,
    oam_uint8 setack)
/*                                                                           */
/* INPUTS  : o  intf - llid interface                                           */
/*     o  queueSets - Container buffer to store the DBA set data             */
/* OUTPUTS : ----                                                            */
/* RETURNS : Size of OAM frame                                               */
/* DESCRIPTION:                                                              */
/* API to compose CTC DBA set response OAM frame                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 frame[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_dbasetrsp_t *pout;

    OAM_ASSERT_RET(intf != NULL && pdu != NULL, 0);

    pout = (oam_pdu_dbasetrsp_t *) frame;

#if 0
    memcpy(pout, pdu, sizeof(oam_pdu_dbasetreq_t) - 2);
#endif
    ctc_oam_build_header(pdu->oui, intf->llid, frame, OAM_PDU_CTC_DBA);
    pout->dba_code = OAM_DBA_CODE_SET_RSP;

    memcpy(&(pout->num_queueset), &(pdu->num_queueset),
           len - sizeof(oam_pdu_dbasetreq_t) + 2);
    pout->setack = setack;

    ctc_oam_send(intf->llid, frame , len + 1);

    return len + 1;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_proc_dba_pdu_get_req                                  */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void
oam_proc_dba_pdu_get_req(
    oam_if_t *intf,
    oam_pdu_dbagetreq_t *pdu,
    oam_int32 len)
/*                                                                           */
/* INPUTS  : o  intf - OAM interface                                         */
/*     o  pdu - Container buffer to store the composed OAM frame             */
/*     o  len - Size of OAM frame                                            */
/* OUTPUTS : ----                                                            */
/* RETURNS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to compose CTC DBA get response OAM frame                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 i = 0;
    oam_ctc_onu_dba_struct tempqueueSet;
    oam_uint16 threshold = 0;
    oam_status result = OAM_E_OK;

    OAM_ASSERT(intf != NULL);

    memset(&tempqueueSet, 0, sizeof(oam_ctc_onu_dba_struct));

    for (i = 0; i < CTC_ONU_MAX_QUEUE; i++) {
        result = ctc_oam_onu_mpcp_queue_thresh_get_adapt(i, &threshold);

        if (result == OAM_E_OK) {
            tempqueueSet.queueSet[0].reportBitmap |= (1 << i);
            tempqueueSet.queueSet[0].queueThreshold[i] = threshold;
        }
    }

    tempqueueSet.numQueueSet = 1;

    oam_build_pdu_dbagetrsp(intf->llid, &tempqueueSet);
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_get_ctc_numqueues                                     */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32
oam_get_ctc_numqueues(oam_uint8 bitmap)
/*                                                                           */
/* INPUTS  : o  bitmap - DBA report bitmap                                   */
/* OUTPUTS : ----                                                            */
/* RETURNS : Number of queues                                                */
/* DESCRIPTION:                                                              */
/* API to compose CTC DBA get number of queues                               */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_int32 i, result = 0;

    for (i = 0; i <= 7; i++)
        result = result + ((bitmap >> i) & (0x1));

    return (result);
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_parse_dba_set_thresh                                  */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_parse_dba_set_thresh(
    oam_pdu_dbasetreq_t *pdu,
    oam_int32 len,
    oam_ctc_onu_dba_struct *queueSets)
/*                                                                           */
/* INPUTS  : o  pdu - Container buffer to store the composed OAM frame       */
/*     o  len - Size of OAM frame                                            */
/* OUTPUTS : o  queueSets - Container buffer to store the DBA set data       */
/* RETURNS : result of parse                                                 */
/* DESCRIPTION:                                                              */
/* API to parse CTC DBA set request OAM frame                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_int32 i, j;
    oam_uint8 *pduend = (oam_uint8 *)pdu + len;
    oam_uint8 *pos = &(pdu->queueset_start);
    oam_uint16 val16;

    queueSets->numQueueSet = pdu->num_queueset - 1 ;

    if (queueSets->numQueueSet > 1)
        return OAM_E_ERROR;

    for (i = 0; i < pdu->num_queueset - 1; i++) {
        if (pos + sizeof(oam_uint16)*oam_get_ctc_numqueues(*pos) >  pduend) {
            OAM_ORG_LOG("error: oam dba pdu length mismatch\n");
            return OAM_E_ERROR;
        }

        queueSets->queueSet[i].reportBitmap = *pos;
        pos++;

        for (j = 0; j < CTC_ONU_MAX_QUEUE; j++) {
            if ((queueSets->queueSet[i].reportBitmap & (1 << j)) != 0) {
                memcpy(&val16, pos, sizeof(val16));
                queueSets->queueSet[i].queueThreshold[j] = ntohs(val16);
                pos = pos + sizeof(val16);
            }
        }

    }

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_proc_dba_pdu_set_req                                  */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_proc_dba_pdu_set_req(
    oam_if_t *intf,
    oam_pdu_dbasetreq_t *pdu,
    oam_int32 len)
/*                                                                           */
/* INPUTS  : o  intf - OAM interface                                         */
/*     o  pdu - Container buffer to store the composed OAM frame             */
/*     o  len - Size of OAM frame                                            */
/* OUTPUTS : ----                                                            */
/* RETURNS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to compose CTC DBA set response OAM frame                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_ctc_onu_dba_struct tempqueueSet;
    oam_int32 setack = 1;
    oam_status result = OAM_E_OK;
    oam_uint8 i = 0, j = 0;
    oam_uint8 bitmap = 0;

    OAM_ASSERT(pdu != NULL && intf != NULL);

    memset(&tempqueueSet, 0, sizeof(oam_ctc_onu_dba_struct));
    result = oam_parse_dba_set_thresh(pdu, len, &tempqueueSet);

    if (result == OAM_E_OK) {
        for (i = 0; i < tempqueueSet.numQueueSet; i++) {
            bitmap = tempqueueSet.queueSet[0].reportBitmap;

            for (j = 0; j < CTC_ONU_MAX_QUEUE; j++) {
                if ((bitmap & (1 << j)) != 0 ) {
                    result = ctc_oam_onu_mpcp_queue_thresh_set_adapt(j,
                             tempqueueSet.queueSet[0].queueThreshold[j]);

                    if (result != OAM_E_OK)
                        setack = 0;
                }
            }
        }
    }

    oam_build_pdu_dbasetrsp(intf, pdu, len, setack);
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_proc_pdu_dba                                          */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void
ctc_oam_proc_pdu_dba(
    oam_uint32 llid,
    oam_uint8 *frame,
    oam_int32 len)
/*                                                                           */
/* INPUTS  : o  llid - LLID port                                             */
/*     o  frame - Container buffer to store the composed OAM frame           */
/*     o  len - Size of OAM frame                                            */
/* OUTPUTS : ----                                                            */
/* RETURNS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to handler CTC DBA OAM frame                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_if_t *intf = oam_intf_find(llid);
    oam_pdu_orgspec_t *pdu = (oam_pdu_orgspec_t *)frame;

    OAM_ASSERT(frame != NULL);
    OAM_ASSERT(intf != NULL);

    switch (pdu->dba_code) {
        case OAM_DBA_CODE_GET_REQ:
            oam_proc_dba_pdu_get_req(intf, (oam_pdu_dbagetreq_t *)pdu, len);
            break;

        case OAM_DBA_CODE_SET_REQ:
            oam_proc_dba_pdu_set_req(intf, (oam_pdu_dbasetreq_t *)pdu, len);
            break;

        default:
            OAM_ORG_LOG("Unkown dbacode 0x%x\n", pdu->dba_code);
            break;
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_proc_event_thresh_set                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32
ctc_oam_proc_event_thresh_set(
    oam_ctc_event_hdr_t *pIn,
    oam_uint32 in_len,
    oam_ctc_event_hdr_t *pOut,
    oam_uint32 out_len)
/*                                                                           */
/* INPUTS  : o  port - OAM port ID                                           */
/*     o  frame - Container buffer to store the composed OAM frame           */
/*     o  length - Size of OAM frame                                         */
/* OUTPUTS : ----                                                            */
/* RETURNS : Size of OAM frame                                               */
/* DESCRIPTION:                                                              */
/* API to compose CTC auth response OAM frame                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_ctc_event_hdr_t *pEventHdr = pIn;
    oam_ctc_event_hdr_t *pOutEventHdr = pOut;
    oam_ctc_event_thresh_t *pInThresh = NULL;
    oam_ctc_event_thresh_t *pOutThresh = NULL;
    oam_uint16 count = 0, i = 0, j = 0;
    oam_uint32 raise_thresh = 0, clear_thresh = 0;
    oam_ctc_event_thresh_t tmp;

    count = ntohs(pEventHdr->entry_count);

    if (count == 0 ||
        count * sizeof(oam_ctc_event_thresh_t) > (in_len - sizeof(
                    oam_ctc_event_hdr_t) + 1)
        || out_len < in_len)
        return 0;

    memcpy((void *)pOut, (void *)pIn, in_len);

    pInThresh = (oam_ctc_event_thresh_t *)(pEventHdr->event_data);
    pOutThresh = (oam_ctc_event_thresh_t *)(pOutEventHdr->event_data);

    for (i = 0; i < count; i++) {
        tmp.entry.object_type = ntohs(pInThresh->entry.object_type);
        tmp.entry.object_inst = ntohl(pInThresh->entry.object_inst);
        tmp.entry.id = ntohs(pInThresh->entry.id);
        tmp.raise_thresh = ntohl(pInThresh->raise_thresh);
        tmp.clear_thresh = ntohl(pInThresh->clear_thresh);

        if (tmp.entry.object_type == OAM_CTC_ALARM_TYPE_UNI &&
            (tmp.entry.object_inst & 0x0000ffff) == CTC_2_1_PORT_ID_ALL_UNI) {
            /*handler all uni case*/
            for (j = OAM_UNI_PORT_ID1; j <= CTC_MAX_ONU_UNI_PORTS; j++) {
                tmp.entry.object_inst = j;

                if (OAM_E_OK != ctc_oam_event_thresh_set_adapt(tmp)) {
                    raise_thresh = clear_thresh = OAM_CTC_EVENT_THRESH_NOT_FOUND;
                    pOutThresh->raise_thresh = htonl(raise_thresh);
                    pOutThresh->clear_thresh = htonl(clear_thresh);
                    break;
                }
            }
        }
        else {
            /*set the event status*/
            if (OAM_E_OK != ctc_oam_event_thresh_set_adapt(tmp)) {
                raise_thresh = clear_thresh = OAM_CTC_EVENT_THRESH_NOT_FOUND;
                pOutThresh->raise_thresh = htonl(raise_thresh);
                pOutThresh->clear_thresh = htonl(clear_thresh);
            }

        }

        pInThresh++;
        pOutThresh++;

    }

    pOutEventHdr->subtype = OAM_CTC_EVENT_THRESH_RESP;

    /*retur total len include header*/
    return in_len;

}


/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_proc_event_status_set                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32
ctc_oam_proc_event_status_set(
    oam_ctc_event_hdr_t *pIn,
    oam_uint32 in_len,
    oam_ctc_event_hdr_t *pOut,
    oam_uint32 out_len)
/*                                                                           */
/* INPUTS  : o  port - OAM port ID                                           */
/*     o  frame - Container buffer to store the composed OAM frame           */
/*     o  length - Size of OAM frame                                         */
/* OUTPUTS : ----                                                            */
/* RETURNS : Size of OAM frame                                               */
/* DESCRIPTION:                                                              */
/* API to compose CTC auth response OAM frame                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_ctc_event_hdr_t *pEventHdr = pIn;
    oam_ctc_event_hdr_t *pOutEventHdr = pOut;
    oam_ctc_event_status_t *pInStatus = NULL;
    oam_ctc_event_status_t *pOutStatus = NULL;
    oam_uint16 count = 0, i = 0, j = 0;
    oam_uint32 status = 0;
    oam_ctc_event_status_t tmp;

    count = ntohs(pEventHdr->entry_count);

    if (count == 0 ||
        count * sizeof(oam_ctc_event_status_t) > (in_len - sizeof(
                    oam_ctc_event_hdr_t) + 1)
        || out_len < in_len)
        return 0;

    memcpy((void *)pOut, (void *)pIn, in_len);

    pInStatus = (oam_ctc_event_status_t *)(pEventHdr->event_data);
    pOutStatus = (oam_ctc_event_status_t *)(pOutEventHdr->event_data);

    for (i = 0; i < count; i++) {
        tmp.entry.object_type = ntohs(pInStatus->entry.object_type);
        tmp.entry.object_inst = ntohl(pInStatus->entry.object_inst);
        tmp.entry.id = ntohs(pInStatus->entry.id);
        tmp.status = ntohl(pInStatus->status);

        if (tmp.entry.object_type == OAM_CTC_ALARM_TYPE_UNI &&
            (tmp.entry.object_inst & 0x0000ffff) == CTC_2_1_PORT_ID_ALL_UNI) {
            /*handler all uni case*/
            for (j = OAM_UNI_PORT_ID1; j <= CTC_MAX_ONU_UNI_PORTS; j++) {
                tmp.entry.object_inst = j;

                if (OAM_E_OK != ctc_oam_event_status_set_adapt(tmp)) {
                    status = OAM_CTC_EVENT_STATUS_NOT_FOUND;
                    pOutStatus->status = htonl(status);
                    break;
                }
            }
        }
        else {

            /*set the event status*/
            if (OAM_E_OK != ctc_oam_event_status_set_adapt(tmp)) {
                status = OAM_CTC_EVENT_STATUS_NOT_FOUND;
                pOutStatus->status = htonl(status);
            }

        }

        pInStatus++;
        pOutStatus++;

    }

    pOutEventHdr->subtype = OAM_CTC_EVENT_STATUS_RESP;

    /*retur total len include header*/
    return in_len;

}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_proc_event_thresh_get               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32
ctc_oam_proc_event_thresh_get(
    oam_ctc_event_hdr_t *pIn,
    oam_uint32 in_len,
    oam_ctc_event_hdr_t *pOut,
    oam_uint32 out_len)
/*                                                                           */
/* INPUTS  : o  port - OAM port ID                                           */
/*     o  frame - Container buffer to store the composed OAM frame           */
/*     o  length - Size of OAM frame                                         */
/* OUTPUTS : ----                                                            */
/* RETURNS : Size of out put                                                 */
/* DESCRIPTION:                                                              */
/* API to compose CTC auth response OAM frame                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_ctc_event_hdr_t *pEventHdr = pIn;
    oam_ctc_event_hdr_t *pOutEventHdr = pOut;
    oam_ctc_event_entry_t *pEntry = NULL;
    oam_ctc_event_thresh_t *pThresh = NULL;
    oam_uint16 count = 0, i = 0, j = 0;
    oam_uint32 raise_thresh = 0, clear_thresh = 0;
    oam_uint32 out_max = 0, out_num = 0;
    oam_uint32 obj_inst = 0;
    oam_ctc_event_entry_t tmp;

    count = ntohs(pEventHdr->entry_count);

    if (count == 0 ||
        count * sizeof(oam_ctc_event_entry_t) > (in_len - sizeof(
                    oam_ctc_event_hdr_t) + 1))
        return 0;

    out_max = (out_len - sizeof(oam_ctc_event_hdr_t) + 1) / sizeof(
                  oam_ctc_event_thresh_t);

    pEntry = (oam_ctc_event_entry_t *)pEventHdr->event_data;
    pThresh = (oam_ctc_event_thresh_t *)(pOutEventHdr->event_data);

    for (i = 0; i < count; i++) {

        tmp.object_type = ntohs(pEntry->object_type);
        tmp.object_inst = ntohl(pEntry->object_inst);
        tmp.id = ntohs(pEntry->id);

        if (tmp.object_type == OAM_CTC_ALARM_TYPE_UNI &&
            (tmp.object_inst & 0x0000ffff) == CTC_2_1_PORT_ID_ALL_UNI) {
            /*handler all uni case*/
            obj_inst = (tmp.object_inst) & 0xffff0000;

            for (j = OAM_UNI_PORT_ID1; j <= CTC_MAX_ONU_UNI_PORTS; j++) {
                tmp.object_inst = obj_inst | j;

                if (OAM_E_OK != ctc_oam_event_thresh_get_adapt(tmp, &raise_thresh,
                        &clear_thresh)) {
                    raise_thresh = clear_thresh =  OAM_CTC_EVENT_THRESH_NOT_FOUND;
                }
                else {
                    /*success, handler the default value*/
                    if (raise_thresh == 0xffffffff) {
                        raise_thresh = 0xfffffffe;
                    }

                    if (clear_thresh == 0xffffffff) {
                        clear_thresh = 0xfffffffe;
                    }
                }

                memcpy(&pThresh->entry, pEntry, sizeof(oam_ctc_event_entry_t));
                pThresh->entry.object_inst = htonl(tmp.object_inst);
                pThresh->raise_thresh = htonl(raise_thresh);
                pThresh->clear_thresh = htonl(clear_thresh);
                pThresh++;
                out_num++;

                if (out_num == out_max)
                    break;
            }

            if (out_num == out_max)
                break;

            pEntry++;
        }
        else {
            memcpy(&pThresh->entry, pEntry, sizeof(oam_ctc_event_entry_t));

            /*get the event status*/
            if (OAM_E_OK != ctc_oam_event_thresh_get_adapt(tmp, &raise_thresh,
                    &clear_thresh)) {
                raise_thresh = clear_thresh =  OAM_CTC_EVENT_THRESH_NOT_FOUND;
            }
            else {
                /*success, handler the default value*/
                if (raise_thresh == 0xffffffff) {
                    raise_thresh = 0xfffffffe;
                }

                if (clear_thresh == 0xffffffff) {
                    clear_thresh = 0xfffffffe;
                }
            }

            pThresh->raise_thresh = htonl(raise_thresh);
            pThresh->clear_thresh = htonl(clear_thresh);

            pEntry++;
            pThresh++;
            out_num++;

            if (out_num == out_max)
                break;
        }

    }

    pOutEventHdr->subtype = OAM_CTC_EVENT_THRESH_RESP;
    pOutEventHdr->entry_count = htons(out_num);

    /*retur total len include header*/
    return (sizeof(oam_ctc_event_hdr_t) - 1 + out_num * sizeof(
                oam_ctc_event_thresh_t));

}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_proc_event_status_get                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_int32
ctc_oam_proc_event_status_get(
    oam_ctc_event_hdr_t *pIn,
    oam_uint32 in_len,
    oam_ctc_event_hdr_t *pOut,
    oam_uint32 out_len)
/*                                                                           */
/* INPUTS  : o  port - OAM port ID                                           */
/*     o  frame - Container buffer to store the composed OAM frame           */
/*     o  length - Size of OAM frame                                         */
/* OUTPUTS : ----                                                            */
/* RETURNS : Size of out put                                                 */
/* DESCRIPTION:                                                              */
/* API to compose CTC auth response OAM frame                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_ctc_event_hdr_t *pEventHdr = pIn;
    oam_ctc_event_hdr_t *pOutEventHdr = pOut;
    oam_ctc_event_entry_t *pEntry = NULL;
    oam_ctc_event_status_t *pStatus = NULL;
    oam_uint16 count = 0, i = 0, j = 0;
    oam_uint32 status = 0;
    oam_uint32 out_max = 0, out_num = 0;
    oam_uint32 obj_inst = 0;
    oam_ctc_event_entry_t tmp;

    count = ntohs(pEventHdr->entry_count);

    if (count == 0 ||
        count * sizeof(oam_ctc_event_entry_t) > (in_len - sizeof(
                    oam_ctc_event_hdr_t) + 1))
        return 0;

    out_max = (out_len - sizeof(oam_ctc_event_hdr_t) + 1) / sizeof(
                  oam_ctc_event_status_t);

    pEntry = (oam_ctc_event_entry_t *)pEventHdr->event_data;
    pStatus = (oam_ctc_event_status_t *)(pOutEventHdr->event_data);

    for (i = 0; i < count; i++) {

        tmp.object_type = ntohs(pEntry->object_type);
        tmp.object_inst = ntohl(pEntry->object_inst);
        tmp.id = ntohs(pEntry->id);

        if (tmp.object_type == OAM_CTC_ALARM_TYPE_UNI &&
            (tmp.object_inst & 0x0000ffff) == CTC_2_1_PORT_ID_ALL_UNI) {
            /*handler all uni case*/
            obj_inst = (tmp.object_inst) & 0xffff0000;

            for (j = OAM_UNI_PORT_ID1; j <= CTC_MAX_ONU_UNI_PORTS; j++) {
                tmp.object_inst = obj_inst | j;

                if (OAM_E_OK != ctc_oam_event_status_get_adapt(tmp, &status)) {
                    status = OAM_CTC_EVENT_STATUS_NOT_FOUND;
                }

                memcpy(&pStatus->entry, pEntry, sizeof(oam_ctc_event_entry_t));
                pStatus->entry.object_inst = htonl(tmp.object_inst);
                pStatus->status = htonl(status);
                pStatus++;
                out_num++;

                if (out_num == out_max)
                    break;
            }

            if (out_num == out_max)
                break;

            pEntry++;
        }
        else {
            memcpy(&pStatus->entry, pEntry, sizeof(oam_ctc_event_entry_t));

            /*get the event status*/
            if (OAM_E_OK != ctc_oam_event_status_get_adapt(tmp, &status)) {
                status = OAM_CTC_EVENT_STATUS_NOT_FOUND;
            }

            pStatus->status = htonl(status);

            pEntry++;
            pStatus++;
            out_num++;

            if (out_num == out_max)
                break;
        }

    }

    pOutEventHdr->subtype = OAM_CTC_EVENT_STATUS_RESP;
    pOutEventHdr->entry_count = htons(out_num);

    /*retur total len include header*/
    return (sizeof(oam_ctc_event_hdr_t) - 1 + out_num * sizeof(
                oam_ctc_event_status_t));

}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_event_resp                                  */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void
ctc_oam_build_event_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*     o  frame - Container buffer to store the composed OAM frame           */
/*     o  length - Size of OAM frame                                         */
/* OUTPUTS : ----                                                            */
/* RETURNS : Size of OAM frame                                               */
/* DESCRIPTION:                                                              */
/* API to compose CTC auth response OAM frame                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8    pdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_ctc_t   *pRecvPkt = (oam_pdu_ctc_t *)frame;
    oam_pdu_ctc_t   *pOutPkt = (oam_pdu_ctc_t *)pdu;
    oam_ctc_event_hdr_t *pEventHdr = NULL;
    oam_uint8    *pOutVarCont = NULL;
    oam_int32 out_len = 0;
    oam_uint32 outPktLen = 0;
    oam_uint32 out_max = 0;
    oam_uint32 in_len = 0;

    if (!frame ||
        length < (sizeof(oam_pdu_ctc_t) + sizeof(oam_ctc_event_hdr_t) - 2))
        return ;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    outPktLen = ctc_oam_build_header(pRecvPkt->oui, llid, pdu, OAM_PDU_CTC_EVENT);

    pOutVarCont = pOutPkt->data;
    out_max = OAM_MAX_OAMPDU_SIZE - outPktLen - 2;

    pEventHdr = (oam_ctc_event_hdr_t *)(pRecvPkt->data);
    in_len = length - sizeof(oam_pdu_ctc_t) + 1;

    switch (pEventHdr->subtype) {
        case OAM_CTC_EVENT_STATUS_GET:
            out_len = ctc_oam_proc_event_status_get(pEventHdr, in_len,
                                                    (oam_ctc_event_hdr_t *)pOutVarCont, out_max);
            break;

        case OAM_CTC_EVENT_STATUS_SET:
            out_len = ctc_oam_proc_event_status_set(pEventHdr, in_len,
                                                    (oam_ctc_event_hdr_t *)pOutVarCont, out_max);
            break;

        case OAM_CTC_EVENT_THRESH_GET:
            out_len = ctc_oam_proc_event_thresh_get(pEventHdr, in_len,
                                                    (oam_ctc_event_hdr_t *)pOutVarCont, out_max);
            break;

        case OAM_CTC_EVENT_THRESH_SET:
            out_len = ctc_oam_proc_event_thresh_set(pEventHdr, in_len,
                                                    (oam_ctc_event_hdr_t *)pOutVarCont, out_max);
            break;

        default:
            goto EVENT_IGNORE;
    }

    if (out_len == 0)
        goto EVENT_IGNORE;

    outPktLen += out_len;
    pOutVarCont += out_len;
    /* packet 2  0's to mark the end - 57.6.3 a)*/
    *pOutVarCont++ = 0;
    *pOutVarCont++ = 0;
    outPktLen += 2;

    ctc_oam_send(llid, pdu, outPktLen);

EVENT_IGNORE:
    return ;
}

/*****************************************************************************/
/* $rtn_hdr_start  handle_ctc_oams                                           */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void
handle_ctc_oams(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
/*                                                                           */
/* INPUTS  : o  llid - llid port                                             */
/*           o  frame - Container buffer to store the composed OAM frame     */
/*           o  len - Size of OAM frame                                      */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* API to handler CTC OAM frame                                              */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_pdu_ctc_t *pCTCOamFrame = (oam_pdu_ctc_t *)frame;

    OAM_ASSERT(pCTCOamFrame != NULL);

    switch (pCTCOamFrame->opcode) {
        case OAM_PDU_CTC_VAR_REQ:
            OAM_ORG_DEBUG("Recv OAM_PDU_CTC_VAR_REQ \n");
            ctc_oam_build_var_resp(llid, frame, len);
            break;

        case OAM_PDU_CTC_SET_REQ:
            OAM_ORG_DEBUG("Recv OAM_PDU_CTC_SET_REQ \n");
            ctc_oam_build_set_resp(llid, frame, len);
            break;

        case OAM_PDU_CTC_FILE_IMG:
            OAM_ORG_DEBUG("Recv OAM_PDU_CTC_FILE_IMG \n");
            ctc_oam_build_swimage(llid, frame, len);
            break;

        case OAM_PDU_CTC_DBA:
            OAM_ORG_DEBUG("Recv OAM_PDU_CTC_DBA \n");
            ctc_oam_proc_pdu_dba(llid, frame, len);
            break;

        case OAM_PDU_CTC_CHURNING:
            OAM_ORG_DEBUG("Recv OAM_PDU_CTC_CHURNING \n");
            ctc_oam_proc_churning(llid, frame, len);
            break;

        case OAM_PDU_CTC_ONU_AUTH:
            OAM_ORG_DEBUG("Recv OAM_PDU_CTC_ONU_AUTH \n");
            ctc_oam_build_auth_resp(llid, frame, len);
            break;

        case OAM_PDU_CTC_EVENT:
            OAM_ORG_DEBUG("Recv OAM_PDU_CTC_EVENT \n");

            if (!oam_is_cuc_oam()) {
                ctc_oam_build_event_resp(llid, frame, len);
            }

            break;

        default:
            OAM_ORG_LOG("Recv Unknow Opcode = %d \n", pCTCOamFrame->opcode);
            break;
    }
}

#endif


