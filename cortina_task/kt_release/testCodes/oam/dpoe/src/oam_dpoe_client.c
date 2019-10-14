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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_DPOE_OAM
#include "oam_types.h"
#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_adapt.h"
#include "oam_supp.h"
#include "oam_core.h"
#include "oam_client.h"

#include "oam_dpoe_client.h"
#include "oam_dpoe_pdu.h"
#include "oam_dpoe.h"
#include "oam_dpoe_frame.h"
#include "oam_dpoe_upg.h"
#include "vlan.h"

#ifdef HAVE_TK_OAM
#include "oam_tk.h"
#endif
oam_oui_t  oam_oui_dpoe = {0x00, 0x10, 0x00};
static oam_oui_t oam_oui_tk = {0x00, 0x0d, 0xb6};

oam_boolean g_dpoe_oam_enable = TRUE;

#define DPOE_SUPPROT    0       /* support */
//#define DPOE_VERSION    0x20
#define DPOE_VERSION    0x22


void oam_dpoe_proc_pdu_info(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
{
    oam_pdu_info_t *pdu = (oam_pdu_info_t *) frame;
    oam_uint8 *pos = NULL;
    oam_uint8 tlv_type = 0;
    oam_uint8 tlv_len = 0;
    oam_if_t *intf = NULL;

    OAM_ASSERT(frame != NULL);

    OAM_LOG_DEBUG("LLID %d\r\n", llid);

    intf = oam_intf_find(llid);

    if (intf == NULL) {
        OAM_LOG_MAJ("intf is NULL, llid id = %d \n", llid);
        return;
    }

    if (!g_dpoe_oam_enable) {
        return;
    }

    pos = (oam_uint8 *)&pdu->local_info;
    tlv_type = *pos;
    tlv_len = *(pos + 1);
#if 0
    while (tlv_type != 0) {

        if ((tlv_type != OAM_INFO_TLV_ORG_SPEC)
            || (!oam_is_oui_equal(pos + 2, oam_oui_dpoe))) {
            pos += tlv_len;

            if (pos >= frame + len)
                return;

            tlv_type = *pos;
            tlv_len = *(pos + 1);
            continue;
        }

        if (OAM_DPOE_STATUS_NONE == intf->dpoe_status) {
            intf->dpoe_status = OAM_DPOE_STATUS_START;

            OAM_LOG_DEBUG("DPOE discovery start for llid %d\r\n", llid);
        }

        break;
    }
#endif
	if (OAM_DPOE_STATUS_NONE == intf->dpoe_status) {
			intf->dpoe_status = OAM_DPOE_STATUS_START;

            OAM_LOG_DEBUG("DPOE discovery for llid %d\r\n", llid);
     }
}

oam_uint32 oam_dpoe_build_pdu_info(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_uint8 *pos = NULL;
    oam_int32 len = 0;
    oam_dpoe_tlv_t *dpoe_tlv;
    oam_if_t *intf = NULL;

    OAM_ASSERT_RET(frame != NULL, -1);

    intf = oam_intf_find(llid);

    if (intf == NULL) {
        OAM_LOG_MAJ("intf is NULL, llid id = %d \n", llid);
        return len;
    }

    if (!g_dpoe_oam_enable) {
        return len;
    }

    /* DPoE version info must only present in discovery process */
    if (OAM_DPOE_STATUS_START != intf->dpoe_status)
        return len;

    pos = frame;

    dpoe_tlv = (oam_dpoe_tlv_t *)pos;
    dpoe_tlv->type = OAM_INFO_TLV_ORG_SPEC;
    dpoe_tlv->length = sizeof(oam_dpoe_tlv_t);
    memcpy(dpoe_tlv->oui, oam_oui_dpoe, OAM_OUI_LEN);

    dpoe_tlv->support = DPOE_SUPPROT;
    dpoe_tlv->version = DPOE_VERSION;

    len = sizeof(oam_dpoe_tlv_t);

    /* update dpoe discovery state */
    if (OAM_LOCAL_PDU_ANY == intf->local_pdu) {
        OAM_LOG_DEBUG("DPOE discovery finish for llid %d\r\n", llid);
        intf->dpoe_status = OAM_DPOE_STATUS_FINISH;
    }

    return len;
}

void oam_dpoe_send_dying_gasp(oam_llid_t llid)
{
    #if 0
    oam_pdu_info_t pdu;
    oam_uint32 len = 0;
    oam_uint8 num = OAM_DYING_GASP_NUM, i;

    /*TODO: if need support mLlid, we need change the llid */
    oam_if_t *oamif = oam_intf_find(llid);

    if (oamif == NULL)
        return;

    oamif->local_dying_gasp = TRUE;
    oamif->local_critical_event = TRUE;

    len = oam_build_pdu_info(oamif, NULL, &pdu);

    num = oam_dying_gasp_num_get_adapt();

    OAM_LOG_DEBUG("Send Dying Gasp %d times..\n", num);

    for (i = 0; i < num; i++) {
        oam_dying_gasp_send_adapt(&pdu, len);
    }

    oamif->local_dying_gasp = FALSE;
    oamif->local_critical_event = TRUE;
    #endif

}

void oam_dpoe_llid_registration(oam_llid_t llid)
{

}

void oam_dpoe_llid_deregistration(oam_llid_t llid)
{
    oam_pon_led_set_adapt(OAM_LED_OFF);

#if 0
    dope_oam_llid_deregister_handle();

    dpoe_oam_power_saving_reset();
#endif
}

void oam_dpoe_port_contl_set(){
    ca_vlan_port_control_t config;
    memset(&config,0,sizeof(ca_vlan_port_control_t));

    ca_l2_vlan_port_control_get(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x06),&config);
    config.drop_unknown_vlan = TRUE;
    ca_l2_vlan_port_control_set(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x06),&config);
    OAM_ORG_LOG("set drop_unknown_vlan to TRUE\n\r");
}

void oam_dpoe_init()
{
    oam_vendor_handlers_t dpoe_handle;
    oam_vendor_handlers_t tk_handle;
    
    oam_vendor_reg_event_handler_register(oam_dpoe_llid_registration, oam_dpoe_llid_deregistration);

    oam_vendor_handlers_init(&dpoe_handle);

    dpoe_handle.ext_info_proc_handler  = (oam_vendor_handler_t)oam_dpoe_proc_pdu_info;
    dpoe_handle.ext_info_build_handler = (oam_vendor_build_handler_t)oam_dpoe_build_pdu_info;

    dpoe_handle.org_handler = (oam_vendor_handler_t)handle_dpoe_oams;
    oam_vendor_handler_register(oam_oui_dpoe, dpoe_handle);

#ifdef HAVE_TK_OAM
    oam_tk_init();

    oam_vendor_handlers_init(&tk_handle);
    tk_handle.org_handler = (oam_vendor_handler_t)handle_tk_oams;
    oam_vendor_handler_register(oam_oui_tk, tk_handle);

#endif
    dpoe_oam_upg_init();
    dpoe_llid_init();
    dpoe_bridge_init();
    dpoe_init_cust_field();

    dpoe_encrypt_init();
    dpoe_port_map_cls_index_init();
    __dpoe_oam_default_traffic_cfg_init();

}
#endif
