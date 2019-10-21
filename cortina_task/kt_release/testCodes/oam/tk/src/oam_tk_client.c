#ifdef HAVE_TK_OAM
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

#include "oam_tk_client.h"
#include "oam_tk_pdu.h"
#include "oam_tk.h"

static oam_uint8 oam_tk_extended_info_resp = 0;

/* KT support OLT Vendor OUI  */
static oam_oui_t oam_oui_tk = {0x00, 0x0d, 0xb6};

void oam_tk_proc_pdu_info(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
{
    oam_pdu_info_t *pdu = (oam_pdu_info_t *) frame;
    oam_tk_info_tlv_t *tk_tlv = NULL;
    oam_uint8 *pos = NULL;
    oam_uint8 tlv_type = 0;
    oam_uint8 tlv_len = 0;
    oam_if_t *intf = NULL;

    OAM_ASSERT(frame != NULL);

    intf = oam_intf_find(llid);

    if (intf == NULL) {
        OAM_LOG_MAJ("intf is NULL, llid id = %d \n", llid);
        return;
    }

    pos = (oam_uint8 *)&pdu->local_info;
    tlv_type = *pos;
    tlv_len = *(pos + 1);

    while (tlv_type != 0) {

        if ((tlv_type != OAM_INFO_TLV_ORG_SPEC)
            || (!oam_is_oui_equal(pos + 2, oam_oui_tk))) {
            pos += tlv_len;

            if (pos >= frame + len)
                return;

            tlv_type = *pos;
            tlv_len = *(pos + 1);
            continue;
        }
        break;
    }
    if(tlv_type == OAM_INFO_TLV_ORG_SPEC){
        oam_tk_extended_info_resp = 1;
    }
}

oam_uint32 oam_tk_build_pdu_info(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_int32 len = 0;
    oam_tk_info_tlv_t *tk_tlv;
    oam_if_t *intf = NULL;

    OAM_ASSERT_RET(frame != NULL, -1);

    if(oam_tk_extended_info_resp == 0)
    {
        return len;
    }

    intf = oam_intf_find(llid);

    if (intf == NULL) {
        OAM_LOG_MAJ("intf is NULL, llid id = %d \n", llid);
        return len;
    }

    tk_tlv = (oam_tk_info_tlv_t *)frame;
    tk_tlv->type = OAM_INFO_TLV_ORG_SPEC;
    tk_tlv->length = sizeof(oam_tk_info_tlv_t);
    memcpy(tk_tlv->oui, oam_oui_tk, OAM_OUI_LEN);

    tk_tlv->vendor_type = OAM_TK_TYPE;
    tk_tlv->version = OAM_TK_VERSION;
    tk_tlv->report_mode = OAM_TK_REPORT_MODE_TK;

    len = sizeof(oam_tk_info_tlv_t);
    oam_tk_extended_info_resp = 0;
    return len;
}

void oam_tk_llid_registration(oam_llid_t llid)

{
    
}

void oam_tk_llid_deregistration(oam_llid_t llid)
{
    
}

void oam_tk_client_init()
{
    
}
#endif