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

DEFINITIONS:  "DEVICE" means the Cortina Systems? Daytona SDK product.
"You" or "CUSTOMER" means the entity or individual that uses the SOFTWARE.
"SOFTWARE" means the Cortina Systems? SDK software.

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
#include <string.h>

#include "oam_cmn.h"
#include "oam_control.h"
#include "oam_std_pdu.h"
#include "oam_supp.h"
#include "oam_mux_par.h"
#include "oam_client.h"
#include "oam_core.h"
#include "oam_siepon_client.h"
#include "oam_siepon.h"
#include "oam_siepon_adapt.h"
#include "oam_adapt.h"

#ifdef HAVE_SIEPON_OAM

oam_uint8 siepon_recode_map[] = {
    OAM_PDU_SIEPON_SNMP_NOERR,
    OAM_PDU_SIEPON_SNMP_GENERR,
    OAM_PDU_SIEPON_SNMP_BADVAL,
    OAM_PDU_SIEPON_SNMP_NOSUCHNAME,
    OAM_PDU_SIEPON_SNMP_GENERR,
    OAM_PDU_SIEPON_SNMP_GENERR,
    OAM_PDU_SIEPON_SNMP_NOSUCHNAME,
    OAM_PDU_SIEPON_SNMP_GENERR
};

static void siepon_oam_send(
    oam_llid_t llid,
    oam_uint8 *buf,
    oam_int32 len)
{
    oam_if_t *intf = oam_intf_find(llid);

    if (intf == NULL) {
        return;
    }

    if (len < OAM_MIN_OAMPDU_SIZE) {
        len = OAM_MIN_OAMPDU_SIZE;
    }

    if (intf->state_disc == OAM_SM_DISC_STATE_SEND_ANY) {
        oam_send(intf->llid, OAM_PON_PORT_ID, buf, len);
        intf->org_specific_tx ++;
    }

}

oam_status siepon_oam_pwrs_pkgb_sleep_mode_cap_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_sleep_mode_cap_t *pCap = (oam_siepon_sleep_mode_cap_t *)pOut;
    oam_uint8 cap = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pCap->datatype = pRecvVarCont->datatype;
    retCode = siepon_oam_pwrs_pkgb_sleep_mode_cap_get_adapt(&cap);

    pCap->datalen = sizeof(oam_siepon_sleep_mode_cap_t);
    pCap->sleep_mode_cap = cap;

    *pOutLen = sizeof(oam_siepon_sleep_mode_cap_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_early_wakeup_cap_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_early_wakeup_cap_t *pCap = (oam_siepon_early_wakeup_cap_t *)pOut;
    oam_uint8 cap = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pCap->datatype = pRecvVarCont->datatype;
    retCode = siepon_oam_pwrs_pkgb_early_wakeup_cap_get_adapt(&cap);

    pCap->datalen = sizeof(oam_siepon_early_wakeup_cap_t);
    pCap->early_wakeup_cap = cap;

    *pOutLen = sizeof(oam_siepon_early_wakeup_cap_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_sleep_ind_cap_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_sleep_ind_cap_t *pCap = (oam_siepon_sleep_ind_cap_t *)pOut;
    oam_uint8 cap = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pCap->datatype = pRecvVarCont->datatype;
    retCode = siepon_oam_pwrs_pkgb_sleep_ind_cap_get_adapt(&cap);

    pCap->datalen = sizeof(oam_siepon_sleep_ind_cap_t);
    pCap->sleep_ind_cap = cap;

    *pOutLen = sizeof(oam_siepon_sleep_ind_cap_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_ack_cap_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_ack_cap_t *pCap = (oam_siepon_ack_cap_t *)pOut;
    oam_uint8 cap = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pCap->datatype = pRecvVarCont->datatype;
    retCode = siepon_oam_pwrs_pkgb_sleep_ack_cap_get_adapt(&cap);

    pCap->datalen = sizeof(oam_siepon_ack_cap_t);
    pCap->ack_cap = cap;

    *pOutLen = sizeof(oam_siepon_ack_cap_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_poweron_time_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_poweron_time_t *pTime = (oam_siepon_poweron_time_t *)pOut;
    oam_uint16 time = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pTime->datatype = pRecvVarCont->datatype;
    pTime->datalen = sizeof(oam_siepon_poweron_time_t);

    retCode = siepon_oam_pwrs_pkgb_t_tx_wakeup_get_adapt(&time);
    pTime->tx_wakeup = htons(time);
    retCode = siepon_oam_pwrs_pkgb_t_trx_wakeup_get_adapt(&time);
    pTime->trx_wakeup = htons(time);

    *pOutLen = sizeof(oam_siepon_poweron_time_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_max_sleep_period_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_max_sleep_time_t *pTime = (oam_siepon_max_sleep_time_t *)pOut;
    oam_uint16 time = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pTime->datatype = pRecvVarCont->datatype;
    pTime->datalen = sizeof(oam_siepon_max_sleep_time_t);

    retCode = siepon_oam_pwrs_pkgb_tx_max_sleep_get_adapt(&time);
    pTime->max_sleep_1 = htons(time);
    retCode = siepon_oam_pwrs_pkgb_trx_max_sleep_get_adapt(&time);
    pTime->max_sleep_2 = htons(time);

    *pOutLen = sizeof(oam_siepon_max_sleep_time_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_min_aware_period_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_min_aware_time_t *pTime = (oam_siepon_min_aware_time_t *)pOut;
    oam_uint16 time = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pTime->datatype = pRecvVarCont->datatype;
    pTime->datalen = sizeof(oam_siepon_min_aware_time_t);

    retCode = siepon_oam_pwrs_pkgb_t_min_aware_get_adapt(&time);
    pTime->min_aware = htons(time);

    *pOutLen = sizeof(oam_siepon_min_aware_time_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_early_wakeup_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_early_wakeup_t *pTime = (oam_siepon_early_wakeup_t *)pOut;
    oam_uint8 enable = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pTime->datatype = pRecvVarCont->datatype;
    pTime->datalen = sizeof(oam_siepon_early_wakeup_t);

    retCode = siepon_oam_pwrs_pkgb_early_wakeup_get_adapt(&enable);
    pTime->enable = enable;

    *pOutLen = sizeof(oam_siepon_early_wakeup_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_sleep_ind_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_sleep_ind_t *pInd = (oam_siepon_sleep_ind_t *)pOut;
    oam_uint8 enable = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pInd->datatype = pRecvVarCont->datatype;
    pInd->datalen = sizeof(oam_siepon_sleep_ind_t);

    retCode = siepon_oam_pwrs_pkgb_sleep_ind_get_adapt(&enable);
    pInd->enable = enable;

    *pOutLen = sizeof(oam_siepon_sleep_ind_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_ack_enable_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_ack_enable_t *pAck = (oam_siepon_ack_enable_t *)pOut;
    oam_uint8 enable = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pAck->datatype = pRecvVarCont->datatype;
    pAck->datalen = sizeof(oam_siepon_ack_enable_t);

    retCode = siepon_oam_pwrs_pkgb_ack_enable_get_adapt(&enable);
    pAck->enable = enable;

    *pOutLen = sizeof(oam_siepon_ack_enable_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_sleep_period_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_sleep_time_t *pTime = (oam_siepon_sleep_time_t *)pOut;
    oam_uint16 time = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pTime->datatype = pRecvVarCont->datatype;
    pTime->datalen = sizeof(oam_siepon_sleep_time_t);

    retCode = siepon_oam_pwrs_pkgb_t_tx_sleep_get_adapt(&time);
    pTime->sleep_1 = htons(time);
    retCode = siepon_oam_pwrs_pkgb_t_trx_sleep_get_adapt(&time);
    pTime->sleep_2 = htons(time);

    *pOutLen = sizeof(oam_siepon_sleep_time_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_aware_period_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_aware_time_t *pTime = (oam_siepon_aware_time_t *)pOut;
    oam_uint16 time = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pTime->datatype = pRecvVarCont->datatype;
    pTime->datalen = sizeof(oam_siepon_aware_time_t);

    retCode = siepon_oam_pwrs_pkgb_t_aware_get_adapt(&time);
    pTime->aware = htons(time);

    *pOutLen = sizeof(oam_siepon_aware_time_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_hold_period_get(
    oam_int8     *pRecv,
    oam_int8     *pOut,
    oam_int32    *pInLen,
    oam_int32    *pOutLen)
{
    oam_siepon_hold_time_t *pTime = (oam_siepon_hold_time_t *)pOut;
    oam_uint16 time = 0;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont =
        (oam_snmp_siepon_var_descriptor_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    pTime->datatype = pRecvVarCont->datatype;
    pTime->datalen = sizeof(oam_siepon_hold_time_t);

    retCode = siepon_oam_pwrs_pkgb_t_hold_get_adapt(&time);
    pTime->hold = htons(time);

    *pOutLen = sizeof(oam_siepon_hold_time_t);
    *pInLen = sizeof(oam_snmp_siepon_var_descriptor_t);

    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_early_wakeup_set(
    oam_uint8 *pRecv,
    oam_int32  *pInLen)
{
    oam_status retCode;
    oam_siepon_early_wakeup_t *pWakeup = (oam_siepon_early_wakeup_t *)pRecv;

    if (pWakeup->datalen != OAM_PDU_SIEPON_SNMP_EARLY_WAKEUP_DATA_LEN) {
        return OAM_E_PARAM;
    }

    retCode = siepon_oam_pwrs_pkgb_early_wakeup_set_adapt(pWakeup->enable);

    *pInLen = pWakeup->datalen;
    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_sleep_ind_set(
    oam_uint8 *pRecv,
    oam_int32  *pInLen)
{
    oam_status retCode;
    oam_siepon_sleep_ind_t *pInd = (oam_siepon_sleep_ind_t *)pRecv;

    if (pInd->datalen != OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_DATA_LEN) {
        return OAM_E_PARAM;
    }

    retCode = siepon_oam_pwrs_pkgb_sleep_ind_set_adapt(pInd->enable);

    *pInLen = pInd->datalen;
    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_ack_enable_set(
    oam_uint8 *pRecv,
    oam_int32  *pInLen)
{
    oam_status retCode;
    oam_siepon_ack_enable_t *pAck = (oam_siepon_ack_enable_t *)pRecv;

    if (pAck->datalen != OAM_PDU_SIEPON_SNMP_ACK_ENABLE_DATA_LEN) {
        return OAM_E_PARAM;
    }

    retCode = siepon_oam_pwrs_pkgb_ack_enable_set_adapt(pAck->enable);

    *pInLen = pAck->datalen;
    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_sleep_period_set(
    oam_uint8 *pRecv,
    oam_int32  *pInLen)
{
    oam_status retCode;
    oam_siepon_sleep_time_t *pTime = (oam_siepon_sleep_time_t *)pRecv;
    oam_uint16 max = 0;


    if (pTime->datalen != OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_DATA_LEN) {
        return OAM_E_PARAM;
    }

    siepon_oam_pwrs_pkgb_tx_max_sleep_get_adapt(&max);

    if (ntohs(pTime->sleep_1) > max) {
        return OAM_E_PARAM;
    }
    else {
        retCode = siepon_oam_pwrs_pkgb_t_tx_sleep_set_adapt(ntohs(pTime->sleep_1));

        if (retCode != OAM_E_OK) {
            return retCode;
        }
    }

    siepon_oam_pwrs_pkgb_trx_max_sleep_get_adapt(&max);

    if (ntohs(pTime->sleep_2) > max) {
        return OAM_E_PARAM;
    }
    else {
        retCode = siepon_oam_pwrs_pkgb_t_trx_sleep_set_adapt(ntohs(pTime->sleep_2));
    }

    *pInLen = pTime->datalen;
    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_aware_period_set(
    oam_uint8 *pRecv,
    oam_int32  *pInLen)
{
    oam_status retCode;
    oam_siepon_aware_time_t *pTime = (oam_siepon_aware_time_t *)pRecv;
    oam_uint16 min  = 0;

    if (pTime->datalen != OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_DATA_LEN) {
        return OAM_E_PARAM;
    }

    siepon_oam_pwrs_pkgb_t_min_aware_get_adapt(&min);

    if (ntohs(pTime->aware) < min) {
        return OAM_E_PARAM;
    }

    retCode = siepon_oam_pwrs_pkgb_t_aware_set_adapt(ntohs(pTime->aware));

    *pInLen = pTime->datalen;
    return retCode;
}

oam_status siepon_oam_pwrs_pkgb_hold_period_set(
    oam_uint8 *pRecv,
    oam_int32  *pInLen)
{
    oam_status retCode;
    oam_siepon_hold_time_t *pTime = (oam_siepon_hold_time_t *)pRecv;

    if (pTime->datalen != OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_DATA_LEN) {
        return OAM_E_PARAM;
    }

    retCode = siepon_oam_pwrs_pkgb_t_hold_set_adapt(ntohs(pTime->hold));

    *pInLen = pTime->datalen;
    return retCode;
}

void
siepon_oam_pwrs_early_wkup_send(oam_uint32 tick)
{
    oam_uint8 oam_frm[OAM_MAX_OAMPDU_SIZE] = {0};
    oam_uint32 sleep_dur_tq = 0;
    oam_if_t *intf = oam_intf_find_valid();
    oam_siepon_pwrs_sleep_allowed_t *pdu = (oam_siepon_pwrs_sleep_allowed_t *)
                                           oam_frm;

    if (intf == NULL)
        return;

    memset(oam_frm, 0, OAM_MAX_OAMPDU_SIZE);

    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pdu->siepon_hdr.oui, oam_oui_siepon, OAM_OUI_LEN);
    pdu->opcode = OAM_PDU_SIEPON_EARLY_WAKEUP_ONU;
    /*TBD*/
    sleep_dur_tq = (oam_uint32)(tick * 10 * 62500);
    pdu->sleep_duration  = htonl(sleep_dur_tq);
    siepon_oam_send(0, (oam_uint8 *)pdu, sizeof(oam_siepon_pwrs_sleep_allowed_t));
}

siepon_oam_xlat_rountine_t g_siepon_oam_xlat_table[] = {
#undef PKGB
#define PKGB(SEQ,TYPE,TYPE_VAL,GET_FUNC,SET_FUNC)   {TYPE,GET_FUNC,SET_FUNC},
    SIEPON_OAM_BUILD_RESP
};

static oam_int32
siepon_oam_build_pkga_get_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_pdu_siepon_t *pOutPkt = (oam_pdu_siepon_t *)pdu;
    oam_pdu_siepon_t *pRecvPkt = NULL;
    oam_var_desc_t   *pRecvVarCont = NULL;
    oam_uint8 *pOutVarCont = NULL;
    oam_uint32 totalContentLen = 0;
    oam_uint32 outPktLen = 0;
    oam_uint32 len = 0;
    oam_var_cont_t *pOutVarRsp = NULL;
    oam_siepon_pwrs_cap_container_struct    *pPwsv_cap = NULL;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length >= sizeof(oam_pdu_siepon_t) + sizeof(oam_var_desc_t), 0);
    OAM_ASSERT_RET(intf != NULL, 0);

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    pOutPkt->opcode = OAM_PDU_SIEPON_GET_RESP;
    outPktLen =  sizeof(oam_pdu_siepon_t);
    pOutVarCont = pOutPkt->data;

    pRecvPkt = (oam_pdu_siepon_t *)frame;
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
    totalContentLen = length - sizeof(oam_pdu_siepon_t);
    pRecvVarCont = (oam_var_desc_t *)pRecvPkt->data;

    while (totalContentLen) {
        pRecvVarCont->leaf = ntohs(pRecvVarCont->leaf);

        if ((OAM_SIEPON_BRANCH_NULL == pRecvVarCont->branch)
            && (OAM_SIEPON_LEAF_NULL == pRecvVarCont->leaf)) {
            goto send_pkga_get_resp;
        }

        OAM_ORG_LOG_INFO("Get REQ branch 0x%x, leaf 0x%04x\n", pRecvVarCont->branch,
                         pRecvVarCont->leaf);

        /*set default*/
        pOutVarRsp            = (oam_var_cont_t *)pOutVarCont;
        pOutVarRsp->branch = pRecvVarCont->branch;
        pOutVarRsp->leaf      = htons(pRecvVarCont->leaf);
        pOutVarRsp->width       = OAM_SIEPON_ATTRIB_NOT_SUPPORTED;
        pOutVarCont          += sizeof(oam_var_cont_t);
        outPktLen            += sizeof(oam_var_cont_t);

        switch (pRecvVarCont->branch) {
            case OAM_SIEPON_BRANCH_E:
                switch (pRecvVarCont->leaf) {
                    case OAM_SIEPON_LEAF_PWR_SAVING_CAP_DISCOVER: {
                        oam_uint8 wakeup = 0;
                        pPwsv_cap = (oam_siepon_pwrs_cap_container_struct *)pOutVarCont;
                        len       = sizeof(oam_siepon_pwrs_cap_container_struct);

                        pOutVarRsp->width             = len;
                        siepon_oam_pwrs_pkga_discovery_cap_get_adapt(&pPwsv_cap->sleep_mode_cap,
                                &wakeup,
                                &pPwsv_cap->vendor_size);

                        if (wakeup == 1) {
                            pPwsv_cap->early_wakeup_cap = PWRS_CAP_EARLY_WAKEUP_SUPPORTED;
                        }
                        else {
                            pPwsv_cap->early_wakeup_cap = PWRS_CAP_EARLY_WAKEUP_NOT_SUPPORTED;
                        }

                        outPktLen   += len;
                        pOutVarCont += len;
                        break;
                    }

                    case OAM_SIEPON_LEAF_PWR_SAVING_CAP_DISCOVER_DPOE: {
                        oam_uint8 wakeup = 0;
                        oam_uint8 mode = 0;
                        pPwsv_cap = (oam_siepon_pwrs_cap_container_struct *)pOutVarCont;
                        len       = sizeof(oam_siepon_pwrs_cap_container_struct);

                        pOutVarRsp->width             = len;
                        siepon_oam_pwrs_pkga_discovery_cap_get_adapt(&mode,
                                &wakeup,
                                &pPwsv_cap->vendor_size);

                        if (wakeup == 1) {
                            pPwsv_cap->early_wakeup_cap = PWRS_CAP_EARLY_WAKEUP_SUPPORTED;
                        }
                        else {
                            pPwsv_cap->early_wakeup_cap = PWRS_CAP_EARLY_WAKEUP_NOT_SUPPORTED;
                        }

                        if (mode == 0) {
                            pPwsv_cap->sleep_mode_cap = 4;
                        }
                        else if (mode == 1) {
                            pPwsv_cap->sleep_mode_cap = 0;
                        }
                        else if (mode == 2) {
                            pPwsv_cap->sleep_mode_cap = 2;
                        }
                        else {
                            pPwsv_cap->sleep_mode_cap = 3;
                        }

                        outPktLen   += len;
                        pOutVarCont += len;
                        break;
                    }

                    case OAM_SIEPON_LEAF_PWR_SAVING_CFG: {
                        oam_siepon_pwrs_cfg_container_struct *pCfg =
                            (oam_siepon_pwrs_cfg_container_struct *)pOutVarCont;
                        len       = sizeof(oam_siepon_pwrs_cfg_container_struct);

                        pOutVarRsp->width             = len;
                        siepon_oam_pwrs_pkga_cfg_get_adapt(&pCfg->early_wakeup, pCfg->sleep_dur_max);

                        outPktLen   += len;
                        pOutVarCont += len;
                        break;
                    }

                    default:
                        OAM_ORG_LOG("Unsupport leaf %04x\n", pRecvVarCont->leaf);
                        break;
                } /* switch(pRecvVarCont->leaf)*/

                break;

            default:
                OAM_ORG_LOG("Unsupport ext branch %02x\n", pRecvVarCont->branch);
                break;
        } /* switch(pRecvVarCont->extBranch)*/


        totalContentLen -= sizeof(oam_var_desc_t);
        pRecvVarCont++;

        if (totalContentLen < sizeof(oam_var_desc_t))
            break;

        if (outPktLen + sizeof(oam_var_cont_t) > OAM_MAX_OAMPDU_SIZE)
            break;
    }

send_pkga_get_resp:
    /* packet 2  0's to mark the end - 57.6.3 a)*/
    *pOutVarCont++ = 0;
    *pOutVarCont++ = 0;
    outPktLen += 2;

    siepon_oam_send(0, pdu, (oam_int32)outPktLen);

    return (oam_int32)outPktLen;

}

static oam_int32
siepon_oam_build_pkga_set_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_pdu_siepon_t *pOutPkt = (oam_pdu_siepon_t *)pdu;
    oam_pdu_siepon_t *pRecvPkt = NULL;
    oam_var_cont_t *pRecvVarCont = NULL;
    oam_uint8 *pOutVarCont = NULL;
    oam_uint8 *pInVarCont = NULL;
    oam_uint32 totalContentLen = 0;
    oam_uint32 recvContainerLen = 0;
    oam_uint32 outPktLen = 0;
    oam_var_cont_t *pOutHdrContainer = NULL;
    oam_status ret = OAM_E_OK;

    oam_siepon_pwrs_cfg_container_struct *pPwsvCfg    = NULL;
    oam_siepon_pwrs_cap_container_struct *pPwsvCapCfg = NULL;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length > sizeof(oam_pdu_siepon_t) + sizeof(oam_var_cont_t), 0);
    OAM_ASSERT_RET(intf != NULL, 0);

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    pOutPkt->opcode = OAM_PDU_SIEPON_SET_RESP;
    pOutVarCont = pOutPkt->data;
    outPktLen = sizeof(oam_pdu_siepon_t);

    pRecvPkt = (oam_pdu_siepon_t *)frame;
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
    totalContentLen = length - sizeof(oam_pdu_siepon_t);
    pRecvVarCont = (oam_var_cont_t *)pRecvPkt->data;
    pInVarCont = (oam_uint8 *)pRecvVarCont;

    while (totalContentLen) {
        pRecvVarCont->leaf = ntohs(pRecvVarCont->leaf);

        if ((OAM_SIEPON_BRANCH_NULL == pRecvVarCont->branch)
            && (OAM_SIEPON_LEAF_NULL == pRecvVarCont->leaf)) {
            goto send_pkga_set_resp;
        }

        OAM_ORG_LOG_MIN("SET REQ branch 0x%x, leaf 0x%04x\n", pRecvVarCont->branch,
                        pRecvVarCont->leaf);

        pOutHdrContainer = (oam_var_cont_t *)pOutVarCont;
        pOutHdrContainer->branch = pRecvVarCont->branch;
        pOutHdrContainer->leaf = htons(pRecvVarCont->leaf);
        pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;

        switch (pRecvVarCont->branch) {
            case OAM_SIEPON_BRANCH_A:
                switch (pRecvVarCont->leaf) {
                    case OAM_SIEPON_LEAF_PWR_SAVING_CONTRL: {
                        oam_siepon_pwrs_sleep_control_container_struct *pControl =
                            (oam_siepon_pwrs_sleep_control_container_struct *)(pRecvVarCont + 1);
                        oam_uint8 mode = 0;

                        if (pRecvVarCont->width != sizeof(
                                oam_siepon_pwrs_sleep_control_container_struct)) {
                            pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                            break;
                        }

                        if (pControl->sleep_flag == 0) {
                            siepon_oam_pwrs_pkga_early_wake_up_adapt();
                        }
                        else if (pControl->sleep_flag == 1) {
                            if (ntohl(pControl->sleep_dur) > 0x3b9aca0) {
                                pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                                break;
                            }

                            if (pControl->sleep_mode == 1 ) {
                                mode = 0;
                            }
                            else if (pControl->sleep_mode == 2 ) {
                                mode = 2;
                            }
                            else {
                                pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                                break;
                            }

                            if (OAM_E_OK != siepon_oam_pwrs_pkga_sleep_mode_set_adapt(mode)) {
                                pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                                break;
                            }

                            siepon_oam_pwrs_pkga_sleep_allow_adapt(ntohl(pControl->sleep_dur));
                        }
                        else {
                            if (ntohl(pControl->sleep_dur) > 0x3b9aca0) {
                                pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                                break;
                            }

                            siepon_oam_pwrs_pkga_sleep_time_set_adapt(ntohl(pControl->sleep_dur));
                        }

                        pOutHdrContainer->width = OAM_SIEPON_VAR_SET_OK;

                        break;
                    }

                    default:
                        break;
                }

                break;

            case OAM_SIEPON_BRANCH_E:

                switch (pRecvVarCont->leaf) {
                    case OAM_SIEPON_LEAF_PWR_SAVING_CFG:
                        pPwsvCfg = (oam_siepon_pwrs_cfg_container_struct *)(pRecvVarCont + 1);

                        if (pRecvVarCont->width != sizeof(oam_siepon_pwrs_cfg_container_struct)) {
                            pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                            break;
                        }

                        pOutHdrContainer->width = OAM_SIEPON_VAR_SET_OK;

                        switch (pPwsvCfg->early_wakeup) {

                            case PWRS_CFG_EARLY_WARKUP_ENABLE:
                                ret = siepon_oam_pwrs_pkga_early_eake_up_enable_adapt(1);

                                if (ret != OAM_E_OK) {
                                    pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                                }

                                break;

                            case PWRS_CFG_EARLY_WARKUP_DISABLE:
                                ret = siepon_oam_pwrs_pkga_early_eake_up_enable_adapt(0);

                                if (ret != OAM_E_OK) {
                                    pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                                }

                                break;

                            case PWRS_CFG_EARLY_WARKUP_NOT_SUPPORT:
                                ret = siepon_oam_pwrs_pkga_early_eake_up_enable_adapt(0xff);

                                if (ret != OAM_E_OK) {
                                    pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                                }

                                break;

                            default:
                                pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                                break;
                        }

                        siepon_oam_pwrs_pkga_sleep_time_max_set_adapt(pPwsvCfg->sleep_dur_max);
                        OAM_ORG_DEBUG("wakeup=0x%x, time:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n",
                                      pPwsvCfg->early_wakeup,
                                      pPwsvCfg->sleep_dur_max[0], pPwsvCfg->sleep_dur_max[1],
                                      pPwsvCfg->sleep_dur_max[2], pPwsvCfg->sleep_dur_max[3],
                                      pPwsvCfg->sleep_dur_max[4], pPwsvCfg->sleep_dur_max[5]);
                        break;

                    case OAM_SIEPON_LEAF_PWR_SAVING_CAP_DISCOVER_DPOE:
                        pPwsvCapCfg = (oam_siepon_pwrs_cap_container_struct *)(pRecvVarCont + 1);

                        switch (pPwsvCapCfg->sleep_mode_cap) {
                            case PWRS_CAP_CFG_SLEEP_MODE_TX:
                            case PWRS_CAP_CFG_SLEEP_MODE_TRX:
                                ret = siepon_oam_pwrs_pkga_sleep_mode_set_adapt(pPwsvCapCfg->sleep_mode_cap);

                                if (ret ==  OAM_E_OK) {
                                    pOutHdrContainer->width = OAM_SIEPON_VAR_SET_OK;
                                }
                                else {
                                    pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                                }

                                break;

                            default :
                                pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                                break;
                        }

                        break;

                    default:
                        pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                        break;
                }

                break;

            default:
                pOutHdrContainer->width = OAM_SIEPON_VAR_SET_BAD_PARAM;
                OAM_ORG_LOG("Unsupport ext branch %02x\n", pRecvVarCont->branch);
                break;
        }

        recvContainerLen = sizeof(oam_var_cont_t) + pRecvVarCont->width;
        pInVarCont += recvContainerLen;

        pOutVarCont += sizeof(oam_var_cont_t);
        outPktLen += sizeof(oam_var_cont_t);

        totalContentLen -= recvContainerLen;

        if (totalContentLen < sizeof(oam_var_desc_t))
            break;

        if (outPktLen >= OAM_MAX_OAMPDU_SIZE - sizeof(oam_pdu_siepon_t))
            break;

        pRecvVarCont = (oam_var_cont_t *)pInVarCont;
    }

send_pkga_set_resp:
    siepon_oam_send(0, pdu, (oam_int32)outPktLen);
    return (oam_int32)outPktLen;
}

void siepon_oam_pwrs_pkga_sleep_allow(oam_uint32 duration)
{
    siepon_oam_pwrs_pkga_sleep_allow_adapt(duration);
}

void siepon_oam_pwrs_pkga_early_wake_up()
{
    siepon_oam_pwrs_pkga_early_wake_up_adapt();
}
#if 0
static oam_int32
siepon_oam_build_pkgb_get_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_snmp_pdu_siepon_t *pOutPkt = (oam_snmp_pdu_siepon_t *)pdu;
    oam_snmp_pdu_siepon_t *pRecvPkt = NULL;
    oam_snmp_siepon_var_descriptor_t   *pRecvVarCont = NULL;
    oam_snmp_siepon_short_t           *pShort       = NULL;
    oam_uint8 *pOutVarCont = NULL;
    oam_uint32 totalContentLen = 0;
    oam_uint32 outPktLen = 0;
    oam_uint32 len = 0;
    oam_snmp_siepon_var_descriptor_t *pOutVarRsp = NULL;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length > NULL, 0);

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    if (!frame ||
        length <= sizeof(oam_snmp_pdu_siepon_t) + sizeof(
            oam_snmp_siepon_var_descriptor_t)) {
        return 0;
    }

    if (!intf) {
        IROS_LOG_MSG(IROS_LOG_LEVEL_MAJ, IROS_MID_ORG_OAM,
                     "oam_build_siepon_get_resp at unknown llid 0x%x\n", llid);
        return 0;
    }

    /* we need to build header to pass sanity check */
    pRecvPkt = (oam_snmp_pdu_siepon_t *)frame;

    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    pOutPkt->opcode = OAM_PDU_SIEPON_SNMP_GET_RESP;
    pOutPkt->reqid  = pRecvPkt->reqid;
    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_NOERR;
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);

    outPktLen =  sizeof(oam_snmp_pdu_siepon_t);
    pOutVarCont = pOutPkt->data;
    totalContentLen = length - sizeof(oam_snmp_pdu_siepon_t);
    pRecvVarCont = (oam_snmp_siepon_var_descriptor_t *)pRecvPkt->data;

    while (totalContentLen) {
        if ((OAM_PDU_SIEPON_SNMP_END_OF_TLV_DT == pRecvVarCont->datatype) &&
            (OAM_PDU_SIEPON_SNMP_END_OF_TLV_DL == pRecvVarCont->datalen))
            goto send_pkgb_get_resp;
        else if ((OAM_PDU_SIEPON_SNMP_END_OF_TLV_DT == pRecvVarCont->datatype) &&
                 (OAM_PDU_SIEPON_SNMP_END_OF_TLV_DL != pRecvVarCont->datalen)) {
            pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
            goto send_pkgb_get_resp;
        }

        /*set default*/
        pOutVarRsp            = (oam_snmp_siepon_var_descriptor_t *)pOutVarCont;
        pOutVarRsp->datatype  = pRecvVarCont->datatype;
        pOutVarCont          += sizeof(oam_snmp_siepon_var_descriptor_t);
        outPktLen            += sizeof(oam_snmp_siepon_var_descriptor_t);

        switch (pRecvVarCont->datatype) {
            case OAM_PDU_SIEPON_SNMP_SLEEP_MOD_CAP_R:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 1;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                *(oam_uint8 *)pOutVarCont = siepon_oam_pwrs_pkgb_sleep_mode_cap_get_adapt();
                outPktLen   += len;
                pOutVarCont += len;
                break;

            case OAM_PDU_SIEPON_SNMP_EARLY_WAKEUP_CAP_R:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 1;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                *(oam_uint8 *)pOutVarCont = siepon_oam_pwrs_pkgb_early_wakeup_cap_get_adapt();
                outPktLen   += len;
                pOutVarCont += len;
                break;

            case OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_R :
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 1;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                *(oam_uint8 *)pOutVarCont = siepon_oam_pwrs_pkgb_sleep_ind_cap_get_adapt();
                outPktLen   += len;
                pOutVarCont += len;
                break;

            case OAM_PDU_SIEPON_SNMP_ACKCAPABLE_R :
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 1;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                *(oam_uint8 *)pOutVarCont = siepon_oam_pwrs_pkgb_sleep_ack_cap_get_adapt();
                outPktLen   += len;
                pOutVarCont += len;
                break;

            case OAM_PDU_SIEPON_SNMP_POWERONTIME_R :
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 4;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;

                pShort = (oam_snmp_siepon_short_t *)pOutVarCont;
                pShort->shortval = htons(siepon_oam_pwrs_pkgb_t_tx_wakeup_get_adapt());
                pOutVarCont += 2;
                pShort = (oam_snmp_siepon_short_t *)pOutVarCont;
                pShort->shortval = htons(siepon_oam_pwrs_pkgb_t_trx_wakeup_get_adapt());
                pOutVarCont += 2;
                outPktLen   += len;
                break;

            case OAM_PDU_SIEPON_SNMP_MAX_SLEEP_PERIOD_R:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 4;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;

                pShort = (oam_snmp_siepon_short_t *)pOutVarCont;
                pShort->shortval = htons(siepon_oam_pwrs_pkgb_t_max_sleep_get_adapt());
                pOutVarCont += 2;
                pShort = (oam_snmp_siepon_short_t *)pOutVarCont;
                pShort->shortval = htons(siepon_oam_pwrs_pkgb_t_max_sleep_get_adapt());
                pOutVarCont += 2;
                outPktLen   += len;
                break;

            case OAM_PDU_SIEPON_SNMP_MIN_AWARE_PERIOD_R:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 2;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                /*TBI*/
                pShort  = (oam_snmp_siepon_short_t *)pOutVarCont;
                pShort->shortval = htons(siepon_oam_pwrs_pkgb_t_min_aware_get_adapt());
                outPktLen   += len;
                pOutVarCont += len;
                break;

            case OAM_PDU_SIEPON_SNMP_EARLY_WAKEUUP_EN_RW:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 1;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                *(oam_uint8 *)pOutVarCont = siepon_oam_pwrs_pkgb_early_wakeup_get_adapt();
                outPktLen   += len;
                pOutVarCont += len;
                break;

            case OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_EN_RW:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 1;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                *(oam_uint8 *)pOutVarCont = siepon_oam_pwrs_pkgb_sleep_ind_get_adapt();
                outPktLen   += len;
                pOutVarCont += len;
                break;

            case OAM_PDU_SIEPON_SNMP_ACK_ENABLE_RW:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 1;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                *(oam_uint8 *)pOutVarCont = siepon_oam_pwrs_pkgb_ack_enable_get_adapt();
                outPktLen   += len;
                pOutVarCont += len;
                break;

            case OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_RW:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 4;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                /*TBI*/
                pShort  = (oam_snmp_siepon_short_t *)pOutVarCont;
                pShort->shortval = htons(siepon_oam_pwrs_pkgb_t_tx_sleep_get_adapt());
                pOutVarCont += 2;
                pShort  = (oam_snmp_siepon_short_t *)pOutVarCont;
                pShort->shortval = htons(siepon_oam_pwrs_pkgb_t_tx_sleep_get_adapt());
                pOutVarCont += 2;
                outPktLen   += len;
                break;

            case OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_RW:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 2;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                /*TBI*/
                pShort  = (oam_snmp_siepon_short_t *)pOutVarCont;
                pShort->shortval = htons(siepon_oam_pwrs_pkgb_t_aware_get_adapt());
                outPktLen   += len;
                pOutVarCont += len;
                break;

            case OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_RW:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                    goto send_pkgb_get_resp;
                }

                len = 2;
                pOutVarRsp->datalen  = sizeof(oam_snmp_siepon_var_descriptor_t) + len;
                /*TBI*/
                pShort  = (oam_snmp_siepon_short_t *)pOutVarCont;
                pShort->shortval = htons(siepon_oam_pwrs_pkgb_t_hold_get_adapt());
                outPktLen   += len;
                pOutVarCont += len;
                break;

            default:
                IROS_LOG_MSG(IROS_LOG_LEVEL_MAJ, IROS_MID_ORG_OAM,
                             "Unsupport ext datatype %02x datalen %02x\n", pRecvVarCont->datatype,
                             pRecvVarCont->datalen);
                pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                goto send_pkgb_get_resp;
                break;
        } /* switch(pRecvVarCont->extBranch) */

        totalContentLen -= sizeof(oam_snmp_siepon_var_descriptor_t);
        pRecvVarCont++;

        if (totalContentLen < sizeof(oam_snmp_siepon_var_descriptor_t))
            break;

        if (outPktLen >= OAM_MAX_OAMPDU_SIZE - sizeof(oam_snmp_pdu_siepon_t))
            break;
    }

send_pkgb_get_resp:
    /* packet 0x0002 to indicate end of TLV- SIEPON 12.1.3.3.2 */
    *pOutVarCont++ = OAM_PDU_SIEPON_SNMP_END_OF_TLV_DT;
    *pOutVarCont++ = OAM_PDU_SIEPON_SNMP_END_OF_TLV_DL;
    outPktLen += 2;

    /* packet 2  0's to mark the end - 57.6.3 a)*/
    *pOutVarCont++ = 0;
    *pOutVarCont++ = 0;
    outPktLen += 2;

    siepon_oam_send(0, (oam_uint8 *)pdu, outPktLen);

    return outPktLen;
}
#endif
static oam_int32
siepon_oam_build_pkgb_get_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_snmp_pdu_siepon_t *pOutPkt = (oam_snmp_pdu_siepon_t *)pdu;
    oam_snmp_pdu_siepon_t *pRecvPkt = NULL;
    oam_snmp_siepon_var_descriptor_t   *pRecvVarCont = NULL;
    oam_uint8 *pOutVarCont = NULL;
    oam_uint32 totalContentLen = 0;
    oam_uint32 outPktLen = 0;
    oam_uint32 i = 0, tmpIn = 0, tmpOut = 0;
    oam_status retCode = OAM_E_OK;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length > sizeof(oam_snmp_pdu_siepon_t) + sizeof(
                       oam_snmp_siepon_var_descriptor_t), 0);
    OAM_ASSERT_RET(intf != NULL, 0);

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    /* we need to build header to pass sanity check */
    pRecvPkt = (oam_snmp_pdu_siepon_t *)frame;

    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    pOutPkt->opcode = OAM_PDU_SIEPON_SNMP_GET_RESP;
    pOutPkt->reqid  = pRecvPkt->reqid;
    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_NOERR;
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);

    outPktLen =  sizeof(oam_snmp_pdu_siepon_t);
    pOutVarCont = pOutPkt->data;
    totalContentLen = length - sizeof(oam_snmp_pdu_siepon_t);
    pRecvVarCont = (oam_snmp_siepon_var_descriptor_t *)pRecvPkt->data;

    while (totalContentLen) {
        if ((OAM_PDU_SIEPON_SNMP_END_OF_TLV_DT == pRecvVarCont->datatype) &&
            (OAM_PDU_SIEPON_SNMP_END_OF_TLV_DL == pRecvVarCont->datalen)) {
            goto send_pkgb_get_resp;
        }

        if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_GET_TLV_LEN) {
            retCode = OAM_E_ERROR;
            memcpy(pOutVarCont, pRecvVarCont, sizeof(oam_snmp_siepon_var_descriptor_t));
            pOutVarCont += sizeof(oam_snmp_siepon_var_descriptor_t);
            outPktLen += sizeof(oam_snmp_siepon_var_descriptor_t);
            goto send_pkgb_get_resp;
        }

        for (i = 0;
             i < sizeof(g_siepon_oam_xlat_table) / sizeof(siepon_oam_xlat_rountine_t); i++) {
            if (g_siepon_oam_xlat_table[i].type == pRecvVarCont->datatype) {
                if (g_siepon_oam_xlat_table[i].get_func) {
                    retCode = g_siepon_oam_xlat_table[i].get_func((oam_int8 *)pRecvVarCont,
                              (oam_int8 *)pOutVarCont,
                              (oam_int32 *)&tmpIn, (oam_int32 *)&tmpOut);
                    break;
                }
                else {
                    retCode = OAM_E_NOT_FOUND;
                    goto send_pkgb_get_resp;
                }
            }
        }

        if (i >= sizeof(g_siepon_oam_xlat_table) / sizeof(siepon_oam_xlat_rountine_t)) {
            /*can not found handler*/
            retCode = OAM_E_NOT_FOUND;
            goto send_pkgb_get_resp;
        }

        outPktLen += tmpOut;
        pOutVarCont += tmpOut;

        if (retCode != OAM_E_OK)
            goto send_pkgb_get_resp;

        totalContentLen -= tmpIn;

        if (totalContentLen < sizeof(oam_snmp_siepon_var_descriptor_t))
            break;

        if (outPktLen >= OAM_MAX_OAMPDU_SIZE - sizeof(oam_snmp_siepon_var_descriptor_t))
            break;

        pRecvVarCont = (oam_snmp_siepon_var_descriptor_t *)((oam_uint8 *)pRecvVarCont +
                       tmpIn);

    }

send_pkgb_get_resp:
    /* packet 0x0002 to indicate end of TLV- SIEPON 12.1.3.3.2 */
    *pOutVarCont++ = OAM_PDU_SIEPON_SNMP_END_OF_TLV_DT;
    *pOutVarCont++ = OAM_PDU_SIEPON_SNMP_END_OF_TLV_DL;
    outPktLen += 2;

    /* packet 2  0's to mark the end - 57.6.3 a)*/
    *pOutVarCont++ = 0;
    *pOutVarCont++ = 0;
    outPktLen += 2;

    if (retCode == OAM_E_ERROR)
        retCode = OAM_E_PARAM;

    pOutPkt->errstatus = siepon_recode_map[retCode];

    siepon_oam_send(0, pdu, (oam_int32)outPktLen);

    return (oam_int32)outPktLen;

}

#if 0
static oam_int32
siepon_oam_build_pkgb_set_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_uint8   pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_snmp_pdu_siepon_t *pOutPkt = (oam_snmp_pdu_siepon_t *)pdu;
    oam_snmp_pdu_siepon_t *pRecvPkt = NULL;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont = NULL;
    oam_snmp_siepon_short_t          *pShort = NULL;
    oam_uint8   *pChar          = NULL;
    oam_uint32    totalContentLen = 0;
    oam_uint16  time_period     = 0;
    oam_uint16  t_tx_slp_dur    = 0;
    oam_uint16  t_trx_slp_dur   = 0;
    oam_boolean      ack_en          = 0;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    if (!frame ||
        length <= sizeof(oam_snmp_pdu_siepon_t) + sizeof(
            oam_snmp_siepon_var_descriptor_t)) {
        return 0;
    }

    if (!intf) {
        IROS_LOG_MSG(IROS_LOG_LEVEL_MAJ, IROS_MID_ORG_OAM,
                     "oam_build_siepon_get_resp at unknown llid 0x%x\n", llid);
        return 0;
    }

    if (length > OAM_MAX_OAMPDU_SIZE) {

        return 0;
    }

    /* we need to build header to pass sanity check */
    pRecvPkt = (oam_snmp_pdu_siepon_t *)frame;
    memcpy(pdu, frame, length);

    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    pOutPkt->opcode = OAM_PDU_SIEPON_SNMP_SET_RESP;
    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_NOERR;

    /*Verify the TLVs in the whole frame packet*/
    pRecvVarCont    = (oam_snmp_siepon_var_descriptor_t *)pRecvPkt->data;
    totalContentLen =  length - sizeof(oam_snmp_pdu_siepon_t);

    while (totalContentLen) {
        if ((OAM_PDU_SIEPON_SNMP_END_OF_TLV_DT == pRecvVarCont->datatype) &&
            (OAM_PDU_SIEPON_SNMP_END_OF_TLV_DL == pRecvVarCont->datalen)) {
            break;
        }

        switch (pRecvVarCont->datatype) {
            case OAM_PDU_SIEPON_SNMP_EARLY_WAKEUUP_EN_RW:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_EARLY_WAKEUP_DATA_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_EN_RW :
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_DATA_LEN) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_ACK_ENABLE_RW :
                if (OAM_PDU_SIEPON_SNMP_ACK_ENABLE_DATA_LEN != pRecvVarCont->datalen) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_RW:
                if (OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_DATA_LEN != pRecvVarCont->datalen) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_RW:
                if (OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_DATA_LEN != pRecvVarCont->datalen) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_RW:
                if (OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_DATA_LEN != pRecvVarCont->datalen) {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            default:
                IROS_LOG_MSG(IROS_LOG_LEVEL_MAJ, IROS_MID_ORG_OAM,
                             "Unsupport ext datatype %02x datalen %02x\n", pRecvVarCont->datatype,
                             pRecvVarCont->datalen);
                pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                goto send_pkgb_set_resp;
                break;
        }

        totalContentLen -= pRecvVarCont->datalen;
        pRecvVarCont  = (oam_snmp_siepon_var_descriptor_t *)((oam_uint8 *)pRecvVarCont +
                        pRecvVarCont->datalen);

        if (totalContentLen < sizeof(oam_snmp_siepon_var_descriptor_t))
            break;
    }

    /*really process TLVs*/
    pRecvVarCont    = (oam_snmp_siepon_var_descriptor_t *)pRecvPkt->data;
    totalContentLen =  length - sizeof(oam_snmp_pdu_siepon_t);

    while (totalContentLen) {

        if ((OAM_PDU_SIEPON_SNMP_END_OF_TLV_DT == pRecvVarCont->datatype) &&
            (OAM_PDU_SIEPON_SNMP_END_OF_TLV_DL == pRecvVarCont->datalen)) {
            goto send_pkgb_set_resp;
        }

        switch (pRecvVarCont->datatype) {
            case OAM_PDU_SIEPON_SNMP_EARLY_WAKEUUP_EN_RW:
                if (pRecvVarCont->datalen == OAM_PDU_SIEPON_SNMP_EARLY_WAKEUP_DATA_LEN) {
                    pChar = (oam_uint8 *)pRecvVarCont + sizeof(oam_snmp_siepon_var_descriptor_t);

                    if (*pChar == 0 || *pChar == 1) {
                        siepon_oam_pwrs_pkgb_early_wakeup_set_adapt(*pChar);
                    }
                    else {
                        pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                        goto send_pkgb_set_resp;
                    }
                }
                else {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_EN_RW :
                if (pRecvVarCont->datalen == OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_DATA_LEN) {
                    pChar = (oam_uint8 *)pRecvVarCont + sizeof(oam_snmp_siepon_var_descriptor_t);

                    if (*pChar == 0 || *pChar == 1) {
                        siepon_oam_pwrs_pkgb_sleep_ind_get_adapt(*pChar);
                    }
                    else {
                        pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                        goto send_pkgb_set_resp;
                    }
                }
                else {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_ACK_ENABLE_RW :
                if (OAM_PDU_SIEPON_SNMP_ACK_ENABLE_DATA_LEN == pRecvVarCont->datalen) {
                    pChar = (oam_uint8 *)pRecvVarCont + sizeof(oam_snmp_siepon_var_descriptor_t);
                    ack_en = (*pChar);

                    if (ack_en == 0 || ack_en == 1) {
                        siepon_oam_pwrs_pkgb_ack_enable_set_adapt(ack_en);
                    }
                    else {
                        pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                        goto send_pkgb_set_resp;
                    }
                }
                else {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_RW:
                if (OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_DATA_LEN == pRecvVarCont->datalen) {
                    pShort = (oam_snmp_siepon_short_t *)pRecvVarCont->data;
                    t_tx_slp_dur = ntohs(pShort->shortval);

                    if (t_tx_slp_dur > siepon_oam_pwrs_pkgb_t_max_sleep_get_adapt()) {
                        pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                        goto send_pkgb_set_resp;
                    }
                    else {
                        siepon_oam_pwrs_pkgb_t_tx_sleep_set_adapt(t_tx_slp_dur);
                    }

                    pShort = (oam_snmped_siepon_short_struct *)&pRecvVarCont->data[2];
                    t_trx_slp_dur = ntohs(pShort->shortval);

                    if (t_trx_slp_dur > siepon_oam_pwrs_pkgb_t_max_sleep_get_adapt()) {
                        pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                        goto send_pkgb_set_resp;
                    }
                    else {
                        siepon_oam_pwrs_pkgb_t_trx_sleep_set_adapt(t_trx_slp_dur);
                    }
                }
                else {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_RW:
                if (OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_DATA_LEN == pRecvVarCont->datalen) {
                    pShort = (oam_snmp_siepon_short_t *)pRecvVarCont->data;
                    time_period = ntohs(pShort->shortval);

                    if (time_period < siepon_oam_pwrs_pkgb_t_min_aware_get_adapt()) {
                        pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                        goto send_pkgb_set_resp;
                    }
                    else {
                        siepon_oam_pwrs_pkgb_t_aware_set_adapt(time_period);
                    }

                }
                else {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_RW:
                if (OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_DATA_LEN == pRecvVarCont->datalen) {
                    pShort = (oam_snmp_siepon_short_t *)pRecvVarCont->data;
                    time_period = ntohs(pShort->shortval);
                    siepon_oam_pwrs_pkgb_t_hold_set_adapt(time_period);
                }
                else {
                    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_BADVAL;
                    goto send_pkgb_set_resp;
                }

                break;

            default:
                IROS_LOG_MSG(IROS_LOG_LEVEL_MAJ, IROS_MID_ORG_OAM,
                             "Unsupport ext datatype %02x datalen %02x\n", pRecvVarCont->datatype,
                             pRecvVarCont->datalen);
                pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_GENERR;
                goto send_pkgb_set_resp;
                break;
        }

        totalContentLen -= pRecvVarCont->datalen;
        pRecvVarCont  = (oam_snmp_siepon_var_descriptor_t *)((oam_uint8 *)pRecvVarCont +
                        pRecvVarCont->datalen);

        if (totalContentLen < sizeof(oam_snmp_siepon_var_descriptor_t))
            break;
    }

send_pkgb_set_resp:

    siepon_oam_send(0, (oam_uint8 *)pOutPkt, length);

    return length;

}
#endif

static oam_int32
siepon_oam_build_pkgb_set_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_uint8   pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_snmp_pdu_siepon_t *pOutPkt = (oam_snmp_pdu_siepon_t *)pdu;
    oam_snmp_pdu_siepon_t *pRecvPkt = NULL;
    oam_snmp_siepon_var_descriptor_t *pRecvVarCont = NULL;
    oam_uint32    totalContentLen = 0;
    oam_uint32 i = 0, tmpIn = 0;
    oam_status retCode = OAM_E_OK;

    OAM_ASSERT_RET(frame != NULL, 0);
    OAM_ASSERT_RET(length > sizeof(oam_snmp_pdu_siepon_t) + sizeof(
                       oam_snmp_siepon_var_descriptor_t), 0);
    OAM_ASSERT_RET(intf != NULL, 0);

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);


    /* we need to build header to pass sanity check */
    pRecvPkt = (oam_snmp_pdu_siepon_t *)frame;
    memcpy(pdu, frame, length);

    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    pOutPkt->opcode = OAM_PDU_SIEPON_SNMP_SET_RESP;
    pOutPkt->errstatus = OAM_PDU_SIEPON_SNMP_NOERR;

    /*Verify the TLVs in the whole frame packet*/
    pRecvVarCont    = (oam_snmp_siepon_var_descriptor_t *)pRecvPkt->data;
    totalContentLen =  length - sizeof(oam_snmp_pdu_siepon_t);

    while (totalContentLen) {
        if ((OAM_PDU_SIEPON_SNMP_END_OF_TLV_DT == pRecvVarCont->datatype) &&
            (OAM_PDU_SIEPON_SNMP_END_OF_TLV_DL == pRecvVarCont->datalen)) {
            break;
        }

        switch (pRecvVarCont->datatype) {
            case OAM_PDU_SIEPON_SNMP_EARLY_WAKEUUP_EN_RW:
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_EARLY_WAKEUP_DATA_LEN) {
                    retCode = OAM_E_PARAM;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_EN_RW :
                if (pRecvVarCont->datalen != OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_DATA_LEN) {
                    retCode = OAM_E_PARAM;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_ACK_ENABLE_RW :
                if (OAM_PDU_SIEPON_SNMP_ACK_ENABLE_DATA_LEN != pRecvVarCont->datalen) {
                    retCode = OAM_E_PARAM;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_RW:
                if (OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_DATA_LEN != pRecvVarCont->datalen) {
                    retCode = OAM_E_PARAM;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_RW:
                if (OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_DATA_LEN != pRecvVarCont->datalen) {
                    retCode = OAM_E_PARAM;
                    goto send_pkgb_set_resp;
                }

                break;

            case OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_RW:
                if (OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_DATA_LEN != pRecvVarCont->datalen) {
                    retCode = OAM_E_PARAM;
                    goto send_pkgb_set_resp;
                }

                break;

            default:
                retCode = OAM_E_NOT_FOUND;
                goto send_pkgb_set_resp;
                break;
        }

        totalContentLen -= pRecvVarCont->datalen;
        pRecvVarCont  = (oam_snmp_siepon_var_descriptor_t *)((oam_uint8 *)pRecvVarCont +
                        pRecvVarCont->datalen);

        if (totalContentLen < sizeof(oam_snmp_siepon_var_descriptor_t)) {
            if (totalContentLen == 0) {
                break;
            }
            else {
                retCode = OAM_E_PARAM;
                goto send_pkgb_set_resp;
            }
        }
    }

    /*really process TLVs*/
    pRecvVarCont    = (oam_snmp_siepon_var_descriptor_t *)pRecvPkt->data;
    totalContentLen =  length - sizeof(oam_snmp_pdu_siepon_t);

    while (totalContentLen) {
        if ((OAM_PDU_SIEPON_SNMP_END_OF_TLV_DT == pRecvVarCont->datatype) &&
            (OAM_PDU_SIEPON_SNMP_END_OF_TLV_DL == pRecvVarCont->datalen)) {
            goto send_pkgb_set_resp;
        }

        for (i = 0;
             i < sizeof(g_siepon_oam_xlat_table) / sizeof(siepon_oam_xlat_rountine_t); i++) {
            if (g_siepon_oam_xlat_table[i].type == pRecvVarCont->datatype) {
                if (g_siepon_oam_xlat_table[i].set_func) {
                    retCode = g_siepon_oam_xlat_table[i].set_func((oam_uint8 *)pRecvVarCont,
                              (oam_int32 *)&tmpIn);
                    break;
                }
                else {
                    retCode = OAM_E_NOT_FOUND;
                    goto send_pkgb_set_resp;
                }
            }
        }

        if (i >= sizeof(g_siepon_oam_xlat_table) / sizeof(siepon_oam_xlat_rountine_t)) {
            /*can not found handler*/
            retCode = OAM_E_NOT_FOUND;
            goto send_pkgb_set_resp;
        }

        if (retCode != OAM_E_OK)
            goto send_pkgb_set_resp;

        totalContentLen -= tmpIn;

        if (totalContentLen < sizeof(oam_snmp_siepon_var_descriptor_t))
            break;

        pRecvVarCont = (oam_snmp_siepon_var_descriptor_t *)((oam_uint8 *)pRecvVarCont +
                       tmpIn);
    }

send_pkgb_set_resp:

    if (retCode == OAM_E_ERROR)
        retCode = OAM_E_PARAM;

    pOutPkt->errstatus = siepon_recode_map[retCode];

    siepon_oam_send(0, pdu, (oam_int32)length);

    return (oam_int32)length;
}


/*****************************************************************************/
/* $rtn_hdr_start  handle_siepon_oams                                        */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void
handle_siepon_oams(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
/*                                                                           */
/* INPUTS  : o  llid - LLID port ID                                          */
/*           o  frame - Container buffer to store the composed OAM frame     */
/*           o  len - Size of OAM frame                                      */
/* OUTPUTS : ----                                                            */
/* RETURNS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to handler SIEPON OAM frame                                           */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_pdu_siepon_t *pSIEPONOamFrame = (oam_pdu_siepon_t *)frame;
    oam_siepon_pwrs_sleep_allowed_t *pSleepAllow = NULL;
    oam_siepon_pkg_e type;

    OAM_ASSERT(frame != NULL);
    OAM_ASSERT(len != 0);

    oam_siepon_pkg_type_get(&type);

    switch (type) {
        case OAM_SIEPON_PKG_A:
            switch (pSIEPONOamFrame->opcode) {
                case OAM_PDU_SIEPON_GET_REQ:
                    siepon_oam_build_pkga_get_resp(llid, frame, len);
                    break;

                case OAM_PDU_SIEPON_SET_REQ:
                    /* set var on ONU */
                    siepon_oam_build_pkga_set_resp(llid, frame, len);
                    break;

                case OAM_PDU_SIEPON_SLEEP_ALLOWED:
                    pSleepAllow = (oam_siepon_pwrs_sleep_allowed_t *)frame;
                    siepon_oam_pwrs_pkga_sleep_allow(ntohl(pSleepAllow->sleep_duration));
                    break;

                case OAM_PDU_SIEPON_EARLY_WAKEUP_OLT:
                    siepon_oam_pwrs_pkga_early_wake_up();
                    break;

                default:
                    OAM_LOG_MIN("unsupported siepon pkgA opcode 0x%02x\n",
                                pSIEPONOamFrame->opcode);
                    break;
            }

            break;

        case OAM_SIEPON_PKG_B:
            switch (pSIEPONOamFrame->opcode) {
                case OAM_PDU_SIEPON_SNMP_GET_REQ:
                    siepon_oam_build_pkgb_get_resp(llid, frame, len);
                    break;

                case OAM_PDU_SIEPON_SNMP_SET_REQ:
                    /* set var on ONU */
                    siepon_oam_build_pkgb_set_resp(llid, frame, len);
                    break;

                default:
                    OAM_LOG_MIN("unsupported siepon pkgB opcode 0x%02x\n",
                                pSIEPONOamFrame->opcode);
                    break;
            }

            break;

        default:
            OAM_ORG_LOG("PKG type unsupport:%d\n", type);
            break;
    }

    return;

}


#endif

