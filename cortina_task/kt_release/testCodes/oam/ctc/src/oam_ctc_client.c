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
#include "oam_types.h"
#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_adapt.h"
#include "oam_oob_mgmt.h"
#include "oam_supp.h"
#include "oam_core.h"
#include "oam_control.h"
#include "oam_client.h"
#include "oam_mux_par.h"
#include "oam_ctc_client.h"
#include "oam_ctc_pdu.h"
#include "oam_ctc_intf.h"
#include "oam_ctc_adapt.h"
#include "oam_ctc_swimage.h"
#include "oam_ctc_stats.h"
#include "oam_ctc_alarm.h"

#ifdef ZTE_SPECIFIC
#include "oam_zte_intf.h"
#include "oam_zte_adapt.h"
#endif/*END_ZTE_SPECIFIC*/

oam_oui_t  oam_oui_ctc = {0x11, 0x11, 0x11};
oam_boolean g_ctc_oam_enable = TRUE;

oam_uint8 g_ctc_ver_list[OAM_MAX_EXT] = {0};
oam_uint8 ver_flag[OAM_MAX_EXT] = {0};
oam_uint32  CTC_MAX_VERSION   = 0;

oam_uint16 CTC_MAX_ONU_UNI_PORTS = 2;

oam_boolean g_supp_cuc_oam = FALSE;

extern oam_boolean cgi_get_web_upgrade_status();
extern void oam_timer_restart();

static void oam_ctc_set_oam_ext_status(
    oam_if_t *intf,
    oam_uint8 status)
{
    if (intf->ctc_OAM_Ext_Status != status) {
        OAM_LOG_DEBUG("CTC: OAM_Ext_Status changes from %d to %d\n",
                      intf->ctc_OAM_Ext_Status, status);
        intf->ctc_OAM_Ext_Status = status;

        if (status == OAM_CTC_EXT_ONU_COMPLETED) {
            oam_oob_send_ctc_event(OAM_OOB_EVENT_CTC_OAM_SUCCESS);
        }
        else if (status == OAM_CTC_EXT_ONU_NOTSUPPORTED
                 || status == OAM_CTC_EXT_ONU_SEND_INFO_NACK) {
            oam_oob_send_ctc_event(OAM_OOB_EVENT_CTC_OAM_FAILED);
        }
    }
}



void oam_ctc_proc_pdu_info(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
{
    oam_pdu_info_t *pdu = (oam_pdu_info_t *) frame;
    oam_tlv_org_spec_t *ctc_tlv;
    oam_ctc_tlv_val_t *ctc_val;
    oam_ctc_tlv_ext_t *ctc_ext;
    oam_ctc_tlv_ext_t *ctc_ext_temp = NULL;
    oam_int32 count, i;
    oam_uint8 tlv_type;
    oam_uint8 tlv_len;
    oam_uint8 ext_len = 0;
    oam_uint8 ctc_oui_spported = 0;
    oam_uint8 *pos;
    oam_if_t *intf = NULL;

    OAM_ASSERT(frame != NULL);

    intf = oam_intf_find(llid);

    if (intf == NULL) {
        OAM_LOG_MAJ("intf is NULL, llid id = %d \n", llid);
        return;
    }

    if (!g_ctc_oam_enable) {
        return;
    }

    pos = (oam_uint8 *)&pdu->local_info;
    tlv_type = *pos;
    tlv_len = *(pos + 1);

    while (tlv_type != 0) {

        if ((tlv_type != OAM_INFO_TLV_ORG_SPEC)
            || (!oam_is_oui_equal(pos + 2, oam_oui_ctc))) {
            pos += tlv_len;
            tlv_type = *pos;
            tlv_len = *(pos + 1);
            continue;
        }

        ctc_tlv = (oam_tlv_org_spec_t *)pos;
        ctc_val = (oam_ctc_tlv_val_t *)(ctc_tlv + 1);
        ctc_ext = (oam_ctc_tlv_ext_t *)(ctc_val + 1);
        count = ctc_tlv->length - sizeof(oam_tlv_org_spec_t) -
                sizeof(oam_ctc_tlv_val_t);


        if (ctc_tlv->length == (sizeof(oam_tlv_org_spec_t) +
                                sizeof(oam_ctc_tlv_val_t))) {

            if (intf->ctc_info_tlv.valid) {

                if (ctc_val->ext_support) {
                    if (intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_SEND_WAIT ||
                        intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_COMPLETED) {
                        oam_ctc_set_oam_ext_status(intf, OAM_CTC_EXT_ONU_SEND_INFO_RCV);
                    }
                    else if (intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_SEND_INFO_SNT) {
                        oam_ctc_set_oam_ext_status(intf, OAM_CTC_EXT_ONU_SEND_INFO_ACK_RCV);
                    }
                }
                else {
                    oam_ctc_set_oam_ext_status(intf, OAM_CTC_EXT_ONU_NOTSUPPORTED);
                    OAM_LOG_DEBUG("%d - STATE %d ONU CTC: ext NOT supported\n",
                                  __LINE__, intf->ctc_OAM_Ext_Status);
                }

                intf->ctc_info_tlv.ext_support = ctc_val->ext_support;
                intf->ctc_info_tlv.version = ctc_val->version;
                intf->ctc_OAM_Ext_version = ctc_val->version;
                intf->ctc_info_tlv.num_ext = 0;
            }
            else {
                OAM_LOG_DEBUG("%d - STATE %d ONU CTC: unexpected TLV format\n",
                              __LINE__, intf->ctc_OAM_Ext_Status);
            }
        }
        else {

            intf->ctc_info_tlv.valid = OAM_TRUE;
            intf->ctc_info_tlv.version = 0;
            intf->ctc_info_tlv.num_ext = CTC_MAX_VERSION;

            for (i = 0; i < CTC_MAX_VERSION; i++) {
                memcpy(intf->ctc_info_tlv.ext[i].oui, oam_oui_ctc,
                       OAM_OUI_LEN);
                intf->ctc_info_tlv.ext[i].ver = g_ctc_ver_list[CTC_MAX_VERSION - 1 - i];
            }

            ctc_oui_spported = FALSE;
            ctc_ext_temp = ctc_ext;
            ext_len = 0;

            while (ext_len < count) {

                OAM_LOG_DEBUG("%d - CTC OUI 0x%x 0x%x 0x%x version 0x%02x\n",
                              __LINE__, ctc_ext_temp->oui[0], ctc_ext_temp->oui[1], ctc_ext_temp->oui[2],
                              ctc_ext_temp->ver);

                /* CTC 2.0 */
                if (!memcmp(ctc_ext_temp->oui, oam_oui_ctc, OAM_OUI_LEN)) {
                    for (i = 0; i < CTC_MAX_VERSION; i++) {
                        if (ctc_ext_temp->ver == g_ctc_ver_list[i]) {
                            ver_flag[i] = 1;
                            ctc_oui_spported = TRUE;
                        }
                    }
                }

                ctc_ext_temp += 1;
                ext_len += sizeof(oam_ctc_tlv_ext_t);
            }

            if (ctc_oui_spported) {
                OAM_LOG_DEBUG("%d - STATE %d ONU CTC: extension matched ver 0x%02x\n",
                              __LINE__, intf->ctc_OAM_Ext_Status, ctc_ext_temp->ver);
                intf->ctc_info_tlv.valid = OAM_TRUE;
                intf->ctc_info_tlv.ext_support = 1;
                /* CTC 2.0 */
#if 0
                //intf->ctc_info_tlv.num_ext = 1;
                //intf->ctc_info_tlv.version = ctc_ext_temp->ver;
#endif
                memcpy(intf->ctc_OAM_Ext_OUI, oam_oui_ctc, OAM_OUI_LEN);
#if 0
                //intf->ctc_OAM_Ext_version = ctc_ext_temp->ver;
#endif

                for (i = 0; i < CTC_MAX_VERSION; i++) {
                    if (ver_flag[i]) {
                        intf->ctc_info_tlv.version = g_ctc_ver_list[i];
                        intf->ctc_OAM_Ext_version = g_ctc_ver_list[i];
                        break;
                    }
                }

                memset(ver_flag, 0, sizeof(oam_uint8)*CTC_MAX_VERSION);

                if (intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_SEND_WAIT ||
                    (intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_COMPLETED))
                    oam_ctc_set_oam_ext_status(intf, OAM_CTC_EXT_ONU_SEND_INFO_RCV);
                else if (intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_SEND_INFO_SNT)
                    oam_ctc_set_oam_ext_status(intf, OAM_CTC_EXT_ONU_SEND_INFO_ACK_RCV);

                OAM_LOG_DEBUG("%d - STATE %d ONU CTC: state changed\n",
                              __LINE__, intf->ctc_OAM_Ext_Status);
            }
            else {
                OAM_LOG_MAJ("%d - STATE %d ONU CTC: extension not matched\n",
                            __LINE__, intf->ctc_OAM_Ext_Status);
                intf->ctc_info_tlv.ext_support = 0;
                oam_ctc_set_oam_ext_status(intf, OAM_CTC_EXT_ONU_SEND_INFO_NACK);
            }
        }

        oam_discovery_send(intf, (oam_build_oampdu_t)oam_build_pdu_info, NULL);

        return;
        break;
    }

#if 0
    //oam_discovery_send(intf, (oam_build_oampdu_t)oam_build_pdu_info, NULL);
#endif
}

oam_uint32 oam_ctc_build_pdu_info(
    oam_llid_t  llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_uint8 *pos = NULL;
    oam_int32 len = 0;
    oam_tlv_org_spec_t *ctc_tlv;
    oam_ctc_tlv_val_t *ctc_val;
    oam_ctc_tlv_ext_t *ctc_ext;
    oam_if_t *intf = NULL;

    OAM_ASSERT_RET(frame != NULL, -1);

    intf = oam_intf_find(llid);

    if (intf == NULL) {
        OAM_LOG_MAJ("intf is NULL, llid id = %d \n", llid);
        return -1;
    }

    pos = frame;

    if (!g_ctc_oam_enable
        || !intf->ctc_info_tlv.valid
        || (intf->ctc_OAM_Ext_Status != OAM_CTC_EXT_ONU_SEND_INFO_RCV
            && intf->ctc_OAM_Ext_Status != OAM_CTC_EXT_ONU_SEND_INFO_ACK_RCV
            && intf->ctc_OAM_Ext_Status != OAM_CTC_EXT_ONU_SEND_INFO_NACK)
       ) {
        return len;
    }

    ctc_tlv = (oam_tlv_org_spec_t *)pos;
    pos += sizeof(oam_tlv_org_spec_t);
    len += sizeof(oam_tlv_org_spec_t);
    ctc_tlv->type = OAM_INFO_TLV_ORG_SPEC;
    ctc_tlv->length = 7 + intf->ctc_info_tlv.num_ext * 4;
    memcpy(ctc_tlv->oui, oam_oui_ctc, OAM_OUI_LEN);

    ctc_val = (oam_ctc_tlv_val_t *)pos;
    ctc_val->ext_support = intf->ctc_info_tlv.ext_support;
    pos += sizeof(oam_ctc_tlv_val_t);
    len += sizeof(oam_ctc_tlv_val_t);


    if (intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_SEND_INFO_RCV
        || intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_SEND_INFO_ACK_RCV) {
        oam_int32 i;

        for (i = 0; i < intf->ctc_info_tlv.num_ext; i++) {
            ctc_ext = (oam_ctc_tlv_ext_t *)pos;
            memcpy(ctc_ext->oui,
                   intf->ctc_info_tlv.ext[i].oui,
                   OAM_OUI_LEN);
            ctc_ext->ver = intf->ctc_info_tlv.ext[i].ver;
            pos += sizeof(oam_ctc_tlv_ext_t);
            len += sizeof(oam_ctc_tlv_ext_t);
        }

        if (intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_SEND_INFO_RCV) {
            ctc_val->version = 0;
            OAM_LOG_DEBUG("%d - STATE %d ONU CTC: extension OAM sent\n",
                          __LINE__, intf->ctc_OAM_Ext_Status);

            oam_ctc_set_oam_ext_status(intf, OAM_CTC_EXT_ONU_SEND_INFO_SNT);

        }
        else {
            ctc_val->version = intf->ctc_info_tlv.version;
            OAM_LOG_DEBUG("%d - STATE %d ONU CTC: extension OAM ACK sent\n",
                          __LINE__, intf->ctc_OAM_Ext_Status);
            oam_ctc_set_oam_ext_status(intf, OAM_CTC_EXT_ONU_COMPLETED);

            /* only process base llid */
            if (0 == intf->index) {
                oam_pon_led_set_adapt(OAM_LED_ON);
            }

            oam_timer_restart();

#ifdef ZTE_SPECIFIC
            zte_oam_db_alarm_restore_adapt( );
            zte_oam_db_port_alarm_restore_adapt( );
#endif/*END_ZTE_SPECIFIC*/
        }
    }
    else if (intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_SEND_INFO_NACK) {
        ctc_val->version = 0;
        intf->ctc_info_tlv.ext_support = 0;

        OAM_LOG_DEBUG("%d - STATE %d ONU CTC: extension OAM NACK sent\n",
                      __LINE__, intf->ctc_OAM_Ext_Status);
        intf->ctc_OAM_Ext_Status = OAM_CTC_EXT_ONU_NOTSUPPORTED;
    }


    return len;
}

#if 0


oam_uint32 oam_ctc_set_mib_variable(
    oam_if_t *intf,
    oam_uint8 branch,
    oam_uint16 leaf,
    oam_var_cont_t *pCont,
    oam_int32 value,
    oam_uint8 portId,
    oam_boolean incomingPortIndex,
    oam_boolean *outgoingPortIndex)
{
    oam_uint32 size = 0;
    unsigned long var = 0x10; /*test*/
    oam_uint8 *pData = (oam_uint8 *)pCont;
    oam_var_cont_val_t *pVar = NULL;

    if (!branch || !pCont || !intf)
        return 0;

    switch (branch) {
        case OAM_INTF_COUNTERS_MIB_BRANCH:
            switch (leaf) {
                case OAM_INTF_LEAF_ETH_ADM_STATE:
                    /*lynxd clean  size = oam_build_std_get_port_base_func(intf,(oam_uint8 *)pCont, portId, branch,
                      leaf, incomingPortIndex, outgoingPortIndex, (std_eth_func_t)onu_aal_get_phy_admin_state);
                      */
                    break;

                case OAM_INTF_LEAF_ETH_AUTONEG_ADM_STATE:
                    /* lynxd clean
                       size = oam_build_std_get_port_base_func(intf,(oam_uint8 *)pCont, portId, branch,
                       leaf, incomingPortIndex, outgoingPortIndex, (std_eth_func_t)onu_aal_get_auto_neg_admin_state);
                       */
                    break;

                case OAM_INTF_LEAF_ETH_AUTONEG_LOCAL_CAP:
                case OAM_INTF_LEAF_ETH_AUTONEG_ADVTS_CAP:
                    /* lynxd clean
                       size = oam_build_std_get_port_base_func(intf,(oam_uint8 *)pCont, portId, branch,
                       leaf, incomingPortIndex, outgoingPortIndex, NULL);
                       */
                    break;

                case OAM_INTF_LEAF_ETH_FEC_ABILITY:
                    pCont->width = sizeof(unsigned long);
                    pVar = (oam_var_cont_val_t *)pCont;
                    pVar->value1 = htonl(OAM_INTF_FEC_SUPPORDTED);
                    size = sizeof(oam_var_cont_t) + sizeof(unsigned long);
                    //var = onu_aal_get_port_fec_ability(intf->llid);
                    break;

                case OAM_INTF_LEAF_ETH_FEC_MODE:

                    //diag_printf("fec mode set len = %d, value = %d\n", pCont->width, value);
                    if (pCont->width == 4) {
                        if (value != OAM_INTF_FEC_MODE_UNKOWN &&
                            value != OAM_INTF_FEC_MODE_EN &&
                            value != OAM_INTF_FEC_MODE_DIS)
                            var = -1;
                        else

                            //lynxd clean var = onu_aal_FEC_Mode_set(value);

                            if (CS_OK == var) {
                                pCont->width = OAM_CTC_VAR_SET_OK;
                            }
                            else {
                                pCont->width = OAM_CTC_VAR_SET_BAD_PARAM;
                            }

                        size = sizeof(oam_var_cont_t);
                    }
                    else if (pCont->width == 0) {
                        pCont->width = sizeof(unsigned long);
                        pVar = (oam_var_cont_val_t *)pCont;
                        //lynxd clean pVar->value1 = htonl(onu_aal_get_port_fec_mode(intf->llid));
                        size = sizeof(oam_var_cont_t) + sizeof(unsigned long);
                    }

                    break;

                default:
                    pCont->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
                    size = sizeof(oam_var_cont_t);
                    break;
            }

            break;

        case OAM_INTF_CONTROL_MIB_BRANCH:
            switch (leaf) {
                case OAM_INTF_LEAF_ETH_FEC_MODE:
                    if (value != OAM_INTF_FEC_MODE_UNKOWN &&
                        value != OAM_INTF_FEC_MODE_EN &&
                        value != OAM_INTF_FEC_MODE_DIS)
                        var = -1;
                    else
                        //lynxd clean var = onu_aal_FEC_Mode_set(value);
                        size = sizeof(oam_var_cont_t);

                    if (CS_OK == var) {
                        pCont->width = OAM_CTC_VAR_SET_OK;
                    }
                    else {
                        pCont->width = OAM_CTC_VAR_SET_BAD_PARAM;
                    }

                    break;

                case OAM_INTF_LEAF_ETH_ADM_STATE_SET:
                    if (value != OAM_INTF_PHY_ADMIN_SET_DIS &&
                        value != OAM_INTF_PHY_ADMIN_SET_EN)
                        var = -1;
                    else {
                        /* lynxd clean
                           size = oam_build_std_set_port_base_func(intf,(oam_uint8 *)pCont, portId, branch,
                           leaf, value, incomingPortIndex, outgoingPortIndex,
                           (std_eth_set_func_t)onu_aal_phy_admin_state_set);
                           */
                    }

                    break;

                case OAM_INTF_LEAF_ETH_AUTO_NEG_RESTART:
                    /*lynxd clean
                      size = oam_build_std_set_port_base_func(intf,(oam_uint8 *)pCont, portId, branch,
                      leaf, value, incomingPortIndex, outgoingPortIndex,
                      (std_eth_set_func_t)onu_aal_auto_neg_reset);
                      */
                    break;

                case OAM_INTF_LEAF_ETH_AUTO_NEG_CONTRL:
                    if (value != OAM_INTF_AUTO_NEG_ADMIN_ST_EN &&
                        value != OAM_INTF_AUTO_NEG_ADMIN_ST_DIS)
                        var = -1;
                    else
                        /*lynxd clean
                          size = oam_build_std_set_port_base_func(intf,(oam_uint8 *)pCont, portId, branch,
                          leaf, value, incomingPortIndex, outgoingPortIndex,
                          (std_eth_set_func_t)onu_aal_auto_neg_set);
                          */
                        break;

                default:
                    var = -1;
                    size = sizeof(oam_var_cont_t);
                    pCont->width = OAM_CTC_VAR_SET_BAD_PARAM;
                    break;
            }

            break;

        default:
            size = sizeof(oam_var_cont_t) + sizeof(var);
            OAM_LOG_DEBUG("unknown branch 0x%x leaf 0x%x len %d\n",
                          branch, leaf, size);
            pData = (oam_uint8 *)pCont;
            pData += sizeof(oam_var_cont_t);
            memset(pData, 0, sizeof(var));
            size = sizeof(oam_var_cont_t) + sizeof(var);
            break;
    }

    return size;
}



static oam_uint32 oam_ctc_build_pdu_var_rsp(
    oam_if_t *intf,
    oam_parm_oampdu_var_rsp_t *parm,
    oam_pdu_var_rsp_t *pdu)
{
    oam_int32 totalSize = 0;
    oam_int32 size = 0;
    oam_uint8 *pData = (oam_uint8 *)&pdu->container[0];
    oam_parm_oampdu_var_rsp_t *pParm;
    oam_var_cont_t *pCont = NULL;
    oam_uint8 portId = 0;
    oam_boolean incomingPortIndex = false, outgoingPortIndex = false;

    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_VAR_RSP);
    pParm = (oam_parm_oampdu_var_rsp_t *)parm;
    pCont = (oam_var_cont_t *)pData;
    totalSize = sizeof(oam_pdu_hdr_t);

    while (pParm->branch) {
        unsigned long tmpl = 0;
        oam_uint16 tmps = 0;
        memset(pCont, 0, sizeof(oam_var_cont_t));
        pCont->branch = pParm->branch;
        pCont->leaf = pParm->leaf;
        pCont->width = pParm->width;

        if (OAM_CTC_INDEX_BRANCH == pParm->branch
            || OAM_CTC_INDEX_BRANCH_E == pParm->branch) {
            if (OAM_CTC_LEAF_CODE_ONU_PORT == ntohs(pParm->leaf)) {
                portId = pParm->value[0];
                incomingPortIndex = true;
                outgoingPortIndex = false;
            }
            else {
#if 0
                CTCOAMTRC("CTC extended standard OAM port instance index leaf error %d\n",
                          ntohs(pParm->leaf));
#endif
                return 0;
            }

            pParm++;
            continue;
        }

        if (pParm->width == sizeof(unsigned long)) {
            memcpy(&tmpl, pParm->value, pParm->width);
            tmpl = ntohl(tmpl);
            size = oam_ctc_set_mib_variable(intf, pParm->branch, ntohs(pParm->leaf), pCont,
                                            tmpl,
                                            portId, incomingPortIndex, &outgoingPortIndex);
        }
        else if (pParm->width == sizeof(oam_uint16)) {
            memcpy(&tmps, pParm->value, pParm->width);
            tmps = ntohs(tmps);
            size = oam_ctc_set_mib_variable(intf, pParm->branch, ntohs(pParm->leaf), pCont,
                                            tmps,
                                            portId, incomingPortIndex, &outgoingPortIndex);
        }
        else {
            size = oam_ctc_set_mib_variable(intf, pParm->branch, ntohs(pParm->leaf), pCont,
                                            pParm->value[0],
                                            portId, incomingPortIndex, &outgoingPortIndex);
        }

        totalSize += size;
        pData += size;
        pCont = (oam_var_cont_t *)pData;
        pParm++;
    }

    /* pack two 0 bytes to mark the end */
    *pData++ = 0;
    *pData = 0;
    totalSize += 2;

    intf->var_rsp_tx++;
    return totalSize;
}


void oam_ctc_proc_pdu_var_req(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
{
    oam_int32 num, max;
    oam_var_desc_t *curr, *next;
    oam_parm_oampdu_var_rsp_t parm[OAM_MAX_VAR_REQ_ENTRIES];
    oam_int32 count = 0;
    oam_var_cont_t *curr_container = NULL;
    oam_uint8 *pSetData = NULL;
    oam_if_t *intf = NULL;
    oam_pdu_var_req_t *pdu = (oam_pdu_var_req_t *) frame;

    OAM_ASSERT(frame != NULL);

    intf = oam_intf_find(llid);

    if (intf == NULL) {
        OAM_LOG_MAJ("intf is NULL, llid id = %d \n", llid);
        return ;
    }

    intf->var_req_rx++;

    num = 0;
    curr = &pdu->descriptor[0];
    max = len - sizeof(oam_pdu_hdr_t);

    while (curr) {
        oam_proc_var_desc(curr, &max, &next);

        if (curr->branch != 0) {
            OAM_LOG_DEBUG("rcv var req branch 0x%x leaf 0x%x\n", curr->branch,
                          ntohs(curr->leaf));
            parm[count].branch = curr->branch;
            parm[count].leaf = curr->leaf;
            parm[count].width = 0;

            if (curr->branch == OAM_CTC_INDEX_BRANCH
                || curr->branch == OAM_CTC_INDEX_BRANCH_E) {
                curr_container = (oam_var_cont_t *)curr;
                parm[count].width = curr_container->width;
                pSetData = (oam_uint8 *)curr_container;
                pSetData += sizeof(oam_var_cont_t);

                if (parm[count].width < OAM_VAR_REQ_LEN) {
                    memcpy(parm[count].value, pSetData, parm[count].width);
                }
            }
            else if (curr->branch == OAM_INTF_COUNTERS_MIB_BRANCH) {
                if (curr->leaf == htons(OAM_INTF_LEAF_ETH_FEC_MODE)) {
                    curr_container = (oam_var_cont_t *)curr;
                    parm[count].width = curr_container->width;

                    if (parm[count].width == 4) {
                        pSetData = (oam_uint8 *)curr_container;
                        pSetData += sizeof(oam_var_cont_t);
                        memcpy(parm[count].value, pSetData, parm[count].width);
                    }
                }
            }

            if (curr->branch == OAM_INTF_CONTROL_MIB_BRANCH) {
                curr_container = (oam_var_cont_t *)curr;
                parm[count].width = curr_container->width;
                pSetData = (oam_uint8 *)curr_container;
                pSetData += sizeof(oam_var_cont_t);

                if ((parm[count].width < OAM_VAR_REQ_LEN) && (parm[count].width > 0)) {
                    memcpy(parm[count].value, pSetData, parm[count].width);
                }
            }

            count++;

            if (count >= OAM_MAX_VAR_REQ_ENTRIES)
                break;
        }

        curr = next;
    }

    oam_req_oampdu(intf, (oam_build_oampdu_t)oam_ctc_build_pdu_var_rsp, &parm);
}
#endif

oam_boolean oam_is_cuc_oam()
{
    oam_if_t *oamif = oam_intf_find(oam_llid_get());

    if (oamif == NULL) {
        return FALSE;
    }

    if (oamif->ctc_OAM_Ext_version == 0xc1) {
        return TRUE;
    }

    return FALSE;
}

oam_status  oam_ctc_version_get(oam_llid_t llid, oam_uint8 *version)
{
    oam_if_t *oamif = oam_intf_find(llid);

    if (!version || !oamif) {
        return OAM_E_PARAM;
    }

    *version = oamif->ctc_OAM_Ext_version;

    return OAM_E_OK;
}

void oam_ctc_llid_registration(oam_llid_t llid, oam_uint8 index)
{

}

void oam_ctc_llid_deregistration(oam_llid_t llid, oam_uint8 index)
{
#if 0

    if (!cgi_get_web_upgrade_status()) {
        img_upgrade_clean();
    }

#endif

    ctc_oam_llid_deregister_handle(llid, index);

    ctc_oam_power_saving_reset();
}

void oam_ctc_init()
{
    oam_uint8 ctc_ver_list[] = {0xc1, 0x30, 0x21, 0x20, 0x13, 0x01};
    oam_vendor_handlers_t ctc_handle;

    oam_printf("oam_ctc_init.\r\n");

    oam_vendor_reg_event_handler_register(oam_ctc_llid_registration, oam_ctc_llid_deregistration);

    oam_vendor_handlers_init(&ctc_handle);

    CTC_MAX_ONU_UNI_PORTS = oam_uni_port_num_get_adapt();

    ctc_oam_oui_get_adapt(oam_oui_ctc);

    memset(ver_flag, 0x00, sizeof(ver_flag));
    memset(g_ctc_ver_list, 0x00, sizeof(g_ctc_ver_list));

    CTC_MAX_VERSION = sizeof(ctc_ver_list) / sizeof(ctc_ver_list[0]);

    if (CTC_MAX_VERSION > OAM_MAX_EXT) {
        oam_printf("ctc version list number too large.\r\n");
        return;
    }

    memcpy(g_ctc_ver_list, ctc_ver_list, sizeof(ctc_ver_list));


    ctc_handle.ext_info_proc_handler = oam_ctc_proc_pdu_info;
    ctc_handle.ext_info_build_handler = oam_ctc_build_pdu_info;

    ctc_handle.org_handler = handle_ctc_oams;

    oam_vendor_handler_register(oam_oui_ctc, ctc_handle);

    ctc_oam_alarm_init();
    ctc_onu_stats_monitor_init();
    oam_ctc_swimage_init();

#ifdef ZTE_SPECIFIC
    zte_oam_init();
#endif/*END_ZTE_SPECIFIC*/

    ctc_oam_power_saving_init();

    ctc_oam_init_adapt ();
}
#endif
