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

#include "oam_main.h"
#include "oam_cmn.h"
#include "oam_core.h"
#include "oam_supp.h"
#include "oam_event.h"
#include "oam_control.h"
#include "oam_timer.h"
#include "oam_adapt.h"
#include "oam_std_pdu.h"
#include "oam_client.h"
#include "oam_mux_par.h"

#include "oam_kt.h"
#include "oam_kt_client.h"
#include "oam_kt_pdu.h"
#include "oam_kt_upg.h"
#include "oam_kt_adapt.h"

#include "oam_tk.h"
#include "oam_tk_client.h"

#include "oam_port_stats.h"

#include "special_packet.h"
#include "classifier.h"
#include "rate.h"
#include "eth_port.h"
#include "port.h"


static oam_uint8 oam_tk_extended_info_resp = 0;

/* KT support OLT Vendor OUI  */
static oam_oui_t oam_oui_tk = {0x00, 0x0d, 0xb6};
static oam_oui_t oam_oui_kt = {0xaa, 0xaa, 0xaa};


/* This value came from other vendor */
static oam_uint8 tk_vendor[OAM_VENDOR_LEN] = {0x37, 0x21, 0, 1};
static oam_uint8 tk_new_vendor[OAM_VENDOR_LEN] = {0x37, 0x23, 0, 1};
static oam_uint8 tk_vendor1[2] = {0x37, 0};
static oam_uint8 tk_vendor2[2] = {0x11, 0};
static oam_uint8 tk_vendor3[2] = {0x23, 0};

//TODO: need ask them add to header files
extern void onu_start_detect_pon_act();
extern void onu_stop_detect_pon_act();
extern void oam_guarddog_reboot_init();

/*****************************************************************************/
/* $rtn_hdr_start  oam_build_kt_pdu_var_rsp                                  */
/* CATEGORY   : Device                                                       */
/* ACCESS     : public                                                       */
/* BLOCK      : General                                                      */
/* CHIP       : CS8030                                                       */
/*                                                                           */
static oam_int32 oam_build_kt_pdu_var_rsp(
        oam_if_t *intf,
        oam_parm_oampdu_var_rsp_t *parm,
        oam_pdu_var_rsp_t *pdu)
/*                                                                           */
/* INPUTS  : o intf - oam core interface                                     */
/*           o parm - parm for the pdu                                       */
/*           o pdu -  the oam packet                                         */
/* OUTPUTS : the oam length                                                  */
/* DESCRIPTION:                                                              */
/* Buid the kt OAM for var response                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_int32 totalSize = 0;
    oam_int32 size = 0;
    oam_uint8 *pData = (oam_uint8 *)&pdu->container[0];
    oam_parm_oampdu_var_rsp_t *pParm;
    oam_var_cont_t *pCont = NULL;
    oam_uint8 portId = 0;
    oam_boolean incomingPortIndex = FALSE, outgoingPortIndex = FALSE;

    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_VAR_RSP);
    pParm = (oam_parm_oampdu_var_rsp_t *)parm;
    pCont = (oam_var_cont_t *)pData;
    totalSize = sizeof(oam_pdu_hdr_t);
    while(pParm->branch)
    {
        unsigned long tmpl = 0;
        oam_uint16 tmps = 0;
        memset(pCont, 0, sizeof(oam_var_cont_t));
        pCont->branch = pParm->branch;
        pCont->leaf = pParm->leaf;
        pCont->width = pParm->width;

        if (OAM_KT_INDEX_BRANCH == pParm->branch
                ||OAM_KT_INDEX_BRANCH_E == pParm->branch) {
            if (OAM_KT_LEAF_CODE_ONU_PORT == ntohs(pParm->leaf)) {
                portId = pParm->value[0];
                incomingPortIndex = TRUE;
                outgoingPortIndex = FALSE;
            }
            else {
                //OAM_LOG_DEBUG("KT extended standard OAM port instance index leaf error %d\n",
                //    ntohs(pParm->leaf));
                return 0;
            }
            pParm++;
            continue;
        }
#if 0
        if(pParm->width == sizeof(unsigned long))
        {
            memcpy(&tmpl, pParm->value, pParm->width);
            tmpl = ntohl(tmpl);
            size = oam_kt_set_mib_variable(intf, pParm->branch, ntohs(pParm->leaf), pCont,
                    tmpl, portId, incomingPortIndex, &outgoingPortIndex);
        }
        else if(pParm->width == sizeof(oam_uint16))
        {
            memcpy(&tmps, pParm->value, pParm->width);
            tmps = ntohs(tmps);
            size = oam_kt_set_mib_variable(intf, pParm->branch, ntohs(pParm->leaf), pCont, tmps,
                    portId, incomingPortIndex, &outgoingPortIndex);
        }
        else {
            size = oam_kt_set_mib_variable(intf, pParm->branch, ntohs(pParm->leaf), pCont, pParm->value[0],
                    portId, incomingPortIndex, &outgoingPortIndex);
        }
#endif
        totalSize += size;
        pData += size;
        pCont = (oam_var_cont_t *)pData;
        pParm++;
    }

    //pack two 0 bytes to mark the end
    *pData++ = 0;
    *pData = 0;
    totalSize += 2;

    intf->var_rsp_tx++;
    return totalSize;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_build_kt_pdu_var_rsp                                  */
/* CATEGORY   : Device                                                       */
/* ACCESS     : public                                                       */
/* BLOCK      : General                                                      */
/* CHIP       : CS8030                                                       */
/*                                                                           */
void oam_kt_proc_pdu_var_req(
        oam_port_id_t port,
        oam_uint8 * frame,
        oam_uint32 len)
/*                                                                           */
/* INPUTS  : o port - onu llid                                               */
/*           o frame - the oam packet                                        */
/*           o len - oam packet len                                          */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* process the kt std var request oam                                        */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_int32  max;
    oam_var_desc_t *curr, *next;
    oam_parm_oampdu_var_rsp_t parm[OAM_MAX_VAR_REQ_ENTRIES];
    oam_int32 count = 0;
    oam_var_cont_t *curr_container = NULL;
    oam_uint8 *pSetData = NULL;
    oam_if_t *intf = oam_intf_find(port);
    oam_pdu_var_req_t *pdu = (oam_pdu_var_req_t*) frame;


    OAM_ASSERT(frame != NULL);

    if(intf == NULL){
        OAM_LOG_MAJ("oam core interface is null \n");
        return;
    }


    intf->var_req_rx++;

    memset(parm, 0, sizeof(parm));

    curr = &pdu->descriptor[0];
    max = len - sizeof(oam_pdu_hdr_t);
    while (curr) {
        oam_proc_var_desc(curr, &max, &next);
        if (curr->branch != 0) {
            OAM_LOG_INFO("rcv var req branch 0x%x leaf 0x%x\n", curr->branch, ntohs(curr->leaf));
            parm[count].branch = curr->branch;
            parm[count].leaf = curr->leaf;
            parm[count].width = 0;

            if (curr->branch == OAM_KT_INDEX_BRANCH) {
                curr_container = (oam_var_cont_t *)curr;
                parm[count].width = curr_container->width;
                pSetData = (oam_uint8 *)curr_container;
                pSetData += sizeof(oam_var_cont_t);
                if(parm[count].width < OAM_VAR_REQ_LEN) {
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
            else if (curr->branch == OAM_INTF_CONTROL_MIB_BRANCH) {
                curr_container = (oam_var_cont_t *)curr;
                parm[count].width = curr_container->width;
                pSetData = (oam_uint8 *)curr_container;
                pSetData += sizeof(oam_var_cont_t);
                if((parm[count].width < OAM_VAR_REQ_LEN) && (parm[count].width > 0)) {
                    memcpy(parm[count].value, pSetData, parm[count].width);
                }
            }

            count++;
            if(count >= OAM_MAX_VAR_REQ_ENTRIES)
                break;
        }
        curr = next;
    }

    oam_req_oampdu(intf, (oam_build_oampdu_t)oam_build_kt_pdu_var_rsp, &parm);

}

/*****************************************************************************/
/* $rtn_hdr_start  oam_kt_llid_registration                                  */
/* CATEGORY   : Device                                                       */
/* ACCESS     : public                                                       */
/* BLOCK      : General                                                      */
/* CHIP       : CS8030                                                       */
/*                                                                           */
void oam_kt_llid_registration(oam_llid_t llid, oam_uint8 index)
/*                                                                           */
/* INPUTS  : o llid - onu llid                                               */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* process the llid register for kt                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_kt_pon_led_set_adapt(OAM_LED_ON);
    CA_LED_PON_REGISTER_SET_ON();
    ca_led_pon_act_poll_start();
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_kt_llid_deregistration                                */
/* CATEGORY   : Device                                                       */
/* ACCESS     : public                                                       */
/* BLOCK      : General                                                      */
/* CHIP       : CS8030                                                       */
/*                                                                           */
void oam_kt_llid_deregistration(oam_llid_t llid, oam_uint8 index)
/*                                                                           */
/* INPUTS  : o llid - onu llid                                               */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* process the llid deregister for kt                                        */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_kt_pon_led_set_adapt(OAM_LED_OFF);
    CA_LED_PON_REGISTER_SET_OFF();
    ca_led_pon_act_poll_stop();

    oam_tk_upg_download_abort();

    loop_detect_disable(OAM_ALL_UNI_PORT_ID);
}

void oam_kt_init()
{
    oam_vendor_handlers_t tk_handle, kt_handle;

    oam_vendor_reg_event_handler_register(oam_kt_llid_registration, oam_kt_llid_deregistration);

    oam_vendor_handlers_init(&kt_handle);

    oam_std_handler_register(OAM_PDU_CODE_VAR_REQ,
            oam_kt_proc_pdu_var_req);

    kt_handle.org_handler = handle_kt_prv_oams;
    oam_vendor_handler_register(oam_oui_kt,kt_handle);
    
#ifdef HAVE_TK_OAM
    oam_vendor_handlers_init(&tk_handle);
    tk_handle.ext_info_proc_handler  = (oam_vendor_handler_t)oam_tk_proc_pdu_info;
    tk_handle.ext_info_build_handler = (oam_vendor_build_handler_t)oam_tk_build_pdu_info;

    tk_handle.org_handler = handle_tk_oams;
    oam_vendor_handler_register(oam_oui_tk,tk_handle);
    
    oam_tk_init();
#endif

    kt_prv_oam_init();
    tk_oam_onu_upg_init(); /* TFTP image upgrade */    
    oam_tk_upg_init(); /* TK OAM image upgrade */
    oam_guarddog_reboot_init(); /* guarddog reboot */

    oam_kt_adapt_init();    /* SDK init */
}

