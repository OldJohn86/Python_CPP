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
#ifndef _OAM_CORE_H_
#define _OAM_CORE_H_

/* Include files. */

#include "oam_types.h"
#include "oam_sm.h"
#include "oam_std_pdu.h"
#include "oam_cmn.h"

#ifdef HAVE_CTC_OAM
#include "oam_ctc_pdu.h"
#endif

/* Macro constant definitions */

#define OAM_MAX_IF             32

#define OAM_SOURCE_UNKOWN      0
#define OAM_SOURCE_PON         1
#define OAM_SOURCE_SC_OOB      2

#define OAM_MAX_EXT            8

/* Type definitions */

typedef struct {
    oam_uint8   valid;
    oam_uint8   ext_support;
    oam_uint8   version;
    oam_uint32  num_ext;
    struct {
        oam_uint8   oui[OAM_OUI_LEN];
        oam_uint8   ver;
    } ext[OAM_MAX_EXT];
} oam_ctc_info_tlv_t;

struct oam_if;
typedef oam_int32 (* oam_build_oampdu_t)(struct oam_if *intf, void *parm,
        char *);

typedef struct oam_if {
    oam_uint8           valid;
    oam_uint8           d2_compatible;
    oam_llid_t          llid;   /* llid value: 0-65535 */
    oam_uint16          index;  /* llid index: 0-7 */
    oam_uint8           macaddr[OAM_MACADDR_LEN];
    oam_uint8           macaddr_peer[OAM_MACADDR_LEN];
    oam_uint16          local_oampdu_config;

    /* state machine states */
    oam_sm_disc_state_t     state_disc;
    oam_sm_tx_state_t       state_tx;

    /* state machine related variables */
    oam_uint8           BEGIN;
    oam_uint8           local_critical_event;
    oam_uint8           local_dying_gasp;
    oam_link_status_t   local_link_status;
    oam_uint8           local_lost_link_timer_done;
    oam_mux_action_t    local_mux_action;
    oam_enable_t        local_oam_enable;
    oam_mode_t          local_oam_mode;
    oam_par_action_t    local_par_action;
    oam_local_pdu_t     local_pdu;
    oam_uint8           local_satisfied;
    oam_uint8           local_stable;
    oam_uint8           local_unidir;
    oam_uint32          pdu_cnt;
    oam_pdu_req_t       pdu_req;
    oam_uint8           pdu_timer_done;
    oam_uint8           remote_stable;
    oam_uint8           remote_state_valid;

    /* state machine timers */
    oam_uint32          pdu_timer;
    oam_uint32          local_lost_link_timer;

    /* sequence number and revision */
    oam_uint16          seq_num_next_tx;
    oam_uint16          seq_num_last_rx;
    oam_uint16          revision_next_tx;
    oam_uint16          revision_last_rx;
    oam_uint8           info_tlv_changed;

    /* loop back state variables */
    oam_lb_state_self_t     lb_state_self;
    oam_lb_state_peer_t     lb_state_peer;

    /* recorded peer info */
    oam_uint16          remote_flags;
    oam_uint8           remote_oam_config;
    oam_uint16          remote_oampdu_config;
    oam_tlv_info_t      remote_tlv_info;
    oam_uint16          remote_pdu_flags;

    /* pdu build func and parm */
    oam_build_oampdu_t  pdu_func;
    void                *pdu_parm;

    /* stats */
    oam_uint32          info_tx;
    oam_uint32          info_rx;
    oam_uint32          unique_event_tx;
    oam_uint32          unique_event_rx;
    oam_uint32          dup_event_tx;
    oam_uint32          dup_event_rx;
    oam_uint32          lb_control_tx;
    oam_uint32          lb_control_rx;
    oam_uint32          var_req_tx;
    oam_uint32          var_req_rx;
    oam_uint32          var_rsp_tx;
    oam_uint32          var_rsp_rx;
    oam_uint32          org_specific_tx;
    oam_uint32          org_specific_rx;
    oam_uint32          unsupported_tx;
    oam_uint32          unsupported_rx;
    oam_uint32          frame_lost;
    oam_uint32          frame_tx;
    oam_uint32          frame_rx;
    oam_uint32          frame_lost_seconds;

    oam_uint8           ctc_OAM_Ext_Status;
    oam_ctc_info_tlv_t  ctc_info_tlv;
    oam_uint8           ctc_OAM_Ext_OUI[OAM_OUI_LEN];
    oam_uint8           ctc_OAM_Ext_version;

#ifdef HAVE_CTC_OAM
    oam_uint8           churning_key_len;
    oam_uint8           last_churning_idx;
    oam_uint8           last_churning_key[CHURNING_KEY_10G_LEN];
#endif

#ifdef HAVE_DPOE_OAM
    /* DPoE info */
    oam_uint8           dpoe_status;   /* DPoE information attach or not */
#endif

    oam_uint8		    disable_oampdu_limit;

    /* fields used for oam loopback test */
    oam_uint32          lb_test_req_id;
    oam_uint8           ctc_oam_transition_count;
    oam_uint8           oam_src;
    oam_uint8           reply;
} oam_if_t;

typedef struct {
    oam_if_t                intf[OAM_MAX_IF + 1];
} oam_node_t;

/* External function declarations */

extern void oam_core_init();
extern oam_if_t *oam_intf_find(oam_llid_t);
extern oam_if_t *oam_intf_find_valid();
extern oam_if_t *oam_intf_get(oam_uint32 index);
extern oam_if_t *oam_intf_alloc();
extern void oam_intf_free(oam_if_t *);
extern void oam_intf_init(oam_if_t *, oam_llid_t, oam_mode_t);
extern void oam_enable(oam_if_t *);
extern void oam_disable(oam_if_t *);
extern void oam_intf_dump(oam_llid_t llid,  oam_print func, void *fd, oam_uint8 detail);
extern int oam_llid_find(oam_uint8 llid_idx, oam_llid_t *llid);/* Macro API definitions */


/* Global variable declarations */


#endif /* _OAM_CORE_H_ */
