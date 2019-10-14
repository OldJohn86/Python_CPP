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

#ifndef _OAM_CONTROL_H_
#define _OAM_CONTROL_H_

#include "oam_types.h"
#include "oam_sm.h"
#include "oam_core.h"
#include "oam_timer.h"

#define OAM_CTL_REQ_MASK_LOCAL_UNIDIR           0x0001
#define OAM_CTL_REQ_MASK_LOCAL_LINK_STATUS      0x0002
#define OAM_CTL_REQ_MASK_LOCAL_DYING_GASP       0x0004
#define OAM_CTL_REQ_MASK_LOCAL_CRITICAL_EVENT   0x0008
#define OAM_CTL_REQ_MASK_LOCAL_SATISFIED        0x0010
#define OAM_CTL_REQ_MASK_REMOTE_STATE_VALID     0x0020
#define OAM_CTL_REQ_MASK_REMOTE_STABLE          0x0040
#define OAM_CTL_REQ_MASK_LOCAL_MUX_ACTION       0x0080
#define OAM_CTL_REQ_MASK_LOCAL_PAR_ACTION       0x0100


#define OAM_PDU_TIMER               10  /* oam_keep_alive_rate_get() */
#define OAM_LINK_LOST_TIMER         5

typedef struct {
    oam_uint16              mask;
    oam_uint8               local_unidir;
    oam_link_status_t       local_link_status;
    oam_uint8               local_dying_gasp;
    oam_uint8               local_critical_event;
    oam_uint8               local_satisfied;
    oam_uint8               remote_state_valid;
    oam_uint8               remote_stable;
    oam_mux_action_t        local_mux_action;
    oam_par_action_t        local_par_action;
} oam_parm_req_oam_ctl_t;

typedef void (* oam_handler_t)(oam_if_t *intf);

typedef struct {
    oam_handler_t           oam_link_hdr;
    oam_handler_t           oam_lost_hdr;
    oam_build_oampdu_t      oam_build_info_hdr;
} oam_core_handler_t;

void oam_control_handler_init(
    oam_handler_t link_hdr,
    oam_handler_t lost_hdr,
    oam_build_oampdu_t info_hdr);

void oam_req_oampdu(oam_if_t *, oam_build_oampdu_t, void *);

void oam_req_oam_ctl(oam_if_t *, oam_parm_req_oam_ctl_t *);

void oam_sm_run(oam_if_t *);

oam_uint8 oam_rx_rule_passed(oam_if_t *intf, oam_pdu_hdr_t *pdu);

void oam_start_local_lost_link_timer(oam_if_t *intf);

void oam_start_pdu_timer(oam_if_t *intf);

oam_boolean oam_sm_discovery_completed(oam_llid_t llid);

void oam_discovery_send(oam_if_t *intf, oam_build_oampdu_t func, void *parm);
#endif
