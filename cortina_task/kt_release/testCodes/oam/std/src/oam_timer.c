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
#include "oam_types.h"
#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_oob_mgmt.h"
#include "oam_core.h"
#include "oam_control.h"
#include "oam_timer.h"
#include "oam_event.h"

static oam_boolean g_hold_oam_link_lost  = FALSE;
static oam_uint32 g_keep_alive_handler = 0;
static oam_uint8    g_keep_alive_rate = 10;     /* UNIT: 100ms */

void oam_keep_alive_timer_expire();
void oam_keep_alive();

/*****************************************************************************/
/* $rtn_hdr_start  oam_timer_init                                            */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_timer_init()
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* oam keep alive timer init                                                 */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    g_keep_alive_handler = ca_circle_timer_add(ONU_OAM_INTERVAL_NEW,
                           oam_keep_alive_timer_expire, NULL);

    if (g_keep_alive_handler == 0) {
        OAM_LOG_MAJ("ca_circle_timer_add failed.\n");
        return OAM_E_ERROR;
    }
    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_keep_alive_timer_expire                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_keep_alive_timer_expire(void *data)
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* oam keep alive timer process                                              */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_timer_msg_send(OAM_KEEP_ALIVE_TIMER_EXPIRED, data);
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_time_msg_proc                                         */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_time_msg_proc()
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* oam keep alive timer process                                              */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_keep_alive();
}


void oam_timer_restart()
{
    if (g_keep_alive_handler != 0) {
        ca_timer_del(g_keep_alive_handler);

        g_keep_alive_handler = ca_circle_timer_add(ONU_OAM_INTERVAL_NEW,
                               oam_keep_alive_timer_expire, NULL);

        if (g_keep_alive_handler == 0) {
            OAM_LOG_MAJ("ca_circle_timer_add failed.\n");
            return;
        }
    }
}


void oam_hold_link_lost(oam_boolean enable)
{
    g_hold_oam_link_lost = enable;
}

static oam_uint32 g_timer_count = 0;
void oam_keep_alive()
{
    oam_uint32 i;
    oam_boolean  expired = FALSE;
    oam_if_t *intf = NULL;

    /* keep alive disabled */
    if (0 == g_keep_alive_rate)
        return;

    for (i = 0; i < OAM_MAX_IF; i++) {

        intf = oam_intf_get(i);

        if (intf == NULL || !intf->valid) {
            continue;
        }

        expired = FALSE;

        if (intf->pdu_timer > 0) {
            intf->pdu_timer--;

            if (intf->pdu_timer == 0) {
                intf->pdu_timer_done = TRUE;
                expired = TRUE;
            }
        }

        /* check link lost per second */
        if (((g_timer_count + 1) % (1000 / ONU_OAM_INTERVAL_NEW)) == 0) {
            if (intf->local_lost_link_timer < (OAM_LINK_LOST_TIMER - 1))
                intf->frame_lost_seconds++;

            if (intf->local_lost_link_timer > 0
                && !g_hold_oam_link_lost) {
                intf->local_lost_link_timer--;

                if (intf->local_lost_link_timer == 0) {
                    OAM_LOG_DEBUG("OAM: local lost link timer done, oam state machine reset\n");
                    intf->local_lost_link_timer_done = TRUE;
                    expired = TRUE;
                    OAM_LOG_DEBUG("OAM: link lost for llid 0x%08x \n",
                                intf->llid);
#if defined (HAVE_OOB_MGMT) && defined (HAVE_ONU_L2FTP)

                    if (!oam_is_image_updating_adapt()) {
                        oam_oob_send_ctc_event(OAM_OOB_EVENT_CTC_OAM_LOSS);
                    }

#endif
                }
            }
        }

        if (expired) {
            oam_sm_run(intf);
        }
    }

    g_timer_count++;
}

