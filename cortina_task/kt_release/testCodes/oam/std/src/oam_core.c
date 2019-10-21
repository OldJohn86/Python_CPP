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
#include <string.h>
#include "oam_types.h"
#include "oam_cmn.h"
#include "oam_adapt.h"
#include "oam_core.h"

#ifdef HAVE_DPOE_OAM
#include "oam_dpoe.h"
#endif

static oam_node_t g_oam_node;

int oam_llid_find(oam_uint8 llid_idx, oam_llid_t *llid)
{
    oam_uint32 i;

    for (i = 0; i < OAM_MAX_IF; i++) {
        if ((g_oam_node.intf[i].valid)
            && (g_oam_node.intf[i].index == llid_idx)) {
            *llid = g_oam_node.intf[i].llid;
            return 0;
        }
    }
    return -1;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_intf_find                                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_if_t *oam_intf_find(oam_llid_t llid)
/*                                                                           */
/* INPUTS  : o llid - onu llid                                               */
/* OUTPUTS : llid interface info                                             */
/* DESCRIPTION:                                                              */
/* API to find the llid interface information                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint32 i;

    for (i = 0; i < OAM_MAX_IF; i++) {
        if ((g_oam_node.intf[i].valid)
            && (g_oam_node.intf[i].llid == llid)) {
            return &g_oam_node.intf[i];
        }
    }

#ifdef HAVE_OOB_MGMT
    return NULL;
#else
    //return &g_oam_node.intf[OAM_MAX_IF];
    return NULL;
#endif
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_intf_find_valid                                       */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_if_t *oam_intf_find_valid()
/*                                                                           */
/* INPUTS  : none                                                            */
/* OUTPUTS : llid interface info                                             */
/* DESCRIPTION:                                                              */
/* API to find the first valid llid interface information                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint32 i;

    for (i = 0; i < OAM_MAX_IF; i++) {
        if (g_oam_node.intf[i].valid) {
            return &g_oam_node.intf[i];
        }
    }

    return NULL;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_intf_get                                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_if_t *oam_intf_get(oam_uint32 index)
/*                                                                           */
/* INPUTS  : o index - index of the intf                                     */
/* OUTPUTS : llid interface info                                             */
/* DESCRIPTION:                                                              */
/* API to get the llid interface information                                 */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    OAM_ASSERT_RET(index < OAM_MAX_IF, NULL);
    return &g_oam_node.intf[index];
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_intf_alloc                                            */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_if_t *oam_intf_alloc()
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : alloc llid interface info                                       */
/* DESCRIPTION:                                                              */
/* API to alloc the llid interface information                               */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint32 i;

    for (i = 0; i < OAM_MAX_IF; i++) {
        if (!g_oam_node.intf[i].valid) {
            g_oam_node.intf[i].valid = OAM_TRUE;
            g_oam_node.intf[i].pdu_func = NULL;
            return &g_oam_node.intf[i];
        }
    }

    return &g_oam_node.intf[0];
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_intf_free                                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_intf_free(oam_if_t *intf)
/*                                                                           */
/* INPUTS  : o intf - llid interface info                                    */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* API to free the llid interface information                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    intf->valid = 0;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_intf_init                                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_intf_init(oam_if_t *intf,
                   oam_llid_t llid,
                   oam_mode_t oam_mode)
/*                                                                           */
/* INPUTS  : o intf - llid interface info                                    */
/*           o llid - llid                                                   */
/*           o oam_mode - oam mode                                           */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* API to init the llid interface information                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    memset(intf, 0, sizeof(oam_if_t));

    intf->valid = OAM_TRUE;
    intf->d2_compatible = oam_d2_compatible_get_adapt();
    intf->llid = llid;
    intf->local_oampdu_config = OAM_MAX_OAMPDU_SIZE;
    intf->local_link_status = OAM_LINK_STATUS_OK;
    intf->local_mux_action = OAM_MUX_ACTION_FWD;
    intf->local_oam_mode = oam_mode;
    intf->local_par_action = OAM_PAR_ACTION_FWD;

    /* set to false for interop */
    if (intf->d2_compatible) {
        intf->local_unidir = OAM_FALSE;
    }
    else {
        intf->local_unidir = OAM_TRUE;
    }

    intf->pdu_req = OAM_PDU_REQ_NONE;
    intf->seq_num_last_rx = 0xffff;
    intf->revision_last_rx = 0xffff;
    intf->lb_state_self = OAM_LB_STATE_SELF_NORM;
    intf->lb_state_peer = OAM_LB_STATE_PEER_NORM;
    intf->remote_pdu_flags = 0;
    intf->frame_lost_seconds = 0;
    intf->frame_rx = 0;
    intf->frame_tx = 0;
    intf->reply = OAM_FALSE;

#ifdef HAVE_CTC_OAM
    intf->last_churning_idx = 0xFF;
#endif

#ifdef HAVE_OAM_DPOE
    intf->dpoe_status = OAM_DPOE_STATUS_NONE;
#endif

    oam_disable(intf);
}


/*****************************************************************************/
/* $rtn_hdr_start  oam_core_init                                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_core_init()
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* API to init the all llid interface information to empty                   */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    memset(&g_oam_node, 0, sizeof(g_oam_node));
#ifdef HAVE_OOB_MGMT
    /*add a default node for oob, not belong to the std stack*/
    g_oam_node.intf[OAM_MAX_IF].valid = OAM_TRUE;;
#endif
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_intf_dump                                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_intf_dump(oam_llid_t llid,  oam_print func, void *fd, oam_uint8 detail)
/*                                                                           */
/* INPUTS  : o llid - llid                                                   */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* Dump llid interface information to to screen                              */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint32 i;
    oam_if_t *intf = NULL;

    //OAM_PRINTF("\r\n------------\r\n");
    //OAM_PRINTF("OAM STATUS\r\n");

    for (i = 0; i < OAM_MAX_IF; i++) {
        intf = &g_oam_node.intf[i];

        if (!intf->valid || (intf->index != llid)) {
            continue;
        }

        OAM_PRINTF("\r\nllid_index %d, llid_value 0x%x %s\r\n", intf->index, intf->llid, 
            (intf->state_disc == OAM_SM_DISC_STATE_SEND_ANY)?"discovery complete":"discovery not complete");

        if(!detail){
            continue;
        }
        OAM_PRINTF("---->local_oampdu_config        : %d\r\n",
                   intf->local_oampdu_config);
        OAM_PRINTF("---->d2_compatible              : %d\r\n", intf->d2_compatible);
        OAM_PRINTF("---->local_dying_gasp           : %d\r\n", intf->local_dying_gasp);
        OAM_PRINTF("---->macaddr                    : %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                   intf->macaddr[0], intf->macaddr[1], intf->macaddr[2],
                   intf->macaddr[3], intf->macaddr[4], intf->macaddr[5]);
        OAM_PRINTF("---->macaddr_peer               : %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                   intf->macaddr_peer[0], intf->macaddr_peer[1], intf->macaddr_peer[2],
                   intf->macaddr_peer[3], intf->macaddr_peer[4], intf->macaddr_peer[5]);
        OAM_PRINTF("---->local pdu                  : 0x%x\r\n", intf->local_pdu);
        OAM_PRINTF("---->state_disc                 : 0x%x\r\n", intf->state_disc);
        OAM_PRINTF("---->state_tx                   : 0x%x\r\n", intf->state_tx);
        OAM_PRINTF("---->local_link_status          : 0x%x\r\n",
                   intf->local_link_status);
        OAM_PRINTF("---->local_lost_link_timer_done : 0x%x\r\n",
                   intf->local_lost_link_timer_done);
        OAM_PRINTF("---->local_oam_enable           : 0x%x\r\n",
                   intf->local_oam_enable);
        OAM_PRINTF("---->local_oam_mode             : 0x%x\r\n", intf->local_oam_mode);
        OAM_PRINTF("---->local_pdu                  : 0x%x\r\n", intf->local_pdu);
        OAM_PRINTF("---->local_satisfied            : 0x%x\r\n", intf->local_satisfied);
        OAM_PRINTF("---->local_stable               : 0x%x\r\n", intf->local_stable);
        OAM_PRINTF("---->local_unidir               : 0x%x\r\n", intf->local_unidir);
        OAM_PRINTF("---->pdu_cnt                    : 0x%x\r\n", intf->pdu_cnt);
        OAM_PRINTF("---->pdu_req                    : 0x%x\r\n", intf->pdu_req);
        OAM_PRINTF("---->pdu_timer_done             : 0x%x\r\n", intf->pdu_timer_done);
        OAM_PRINTF("---->remote_stable              : 0x%x\r\n", intf->remote_stable);
        OAM_PRINTF("---->remote_state_valid         : 0x%x\r\n",
                   intf->remote_state_valid);
        OAM_PRINTF("---->lb_state_self              : 0x%x\r\n", intf->lb_state_self);
        OAM_PRINTF("---->lb_state_peer              : 0x%x\r\n", intf->lb_state_peer);
        OAM_PRINTF("---->remote_flags               : 0x%x\r\n", intf->remote_flags);
        OAM_PRINTF("---->remote_oam_config          : 0x%x\r\n",
                   intf->remote_oam_config);
        OAM_PRINTF("---->local_lost_link_timer      : 0x%x\r\n",
                   intf->local_lost_link_timer);
        OAM_PRINTF("---->info_rx                    : 0x%x\r\n", intf->info_rx);
        OAM_PRINTF("---->info_tx                    : 0x%x\r\n", intf->info_tx);
        OAM_PRINTF("---->frame_rx                   : 0x%x\r\n", intf->frame_rx);
        OAM_PRINTF("---->frame_tx                   : 0x%x\r\n", intf->frame_tx);
        OAM_PRINTF("---->org_specific_rx            : 0x%x\r\n", intf->org_specific_rx);
        OAM_PRINTF("---->org_specific_tx            : 0x%x\r\n", intf->org_specific_tx);
        
        OAM_PRINTF("---->frame_lost                 : 0x%x\r\n", intf->frame_lost);
        OAM_PRINTF("---->frame_lost_seconds         : 0x%x\r\n",
                   intf->frame_lost_seconds);

#ifdef HAVE_CTC_OAM
        OAM_PRINTF("---->last_churning_idx          : %d\r\n", intf->last_churning_idx);
        OAM_PRINTF("---->last_churning_key          : 0x[%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x]\r\n",
                   intf->last_churning_key[0], intf->last_churning_key[1],
                   intf->last_churning_key[2],
                   intf->last_churning_key[3], intf->last_churning_key[4],
                   intf->last_churning_key[5],
                   intf->last_churning_key[6], intf->last_churning_key[7],
                   intf->last_churning_key[8]);
        OAM_PRINTF("---->ctc_version                : 0x%x\r\n", intf->ctc_OAM_Ext_version);
#endif

#ifdef HAVE_DPOE_OAM
        OAM_PRINTF("---->dpoe_status                : 0x%x\r\n", intf->dpoe_status);
#endif
    }
}

