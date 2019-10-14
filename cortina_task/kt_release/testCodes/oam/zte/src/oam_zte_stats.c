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

Copyright (c) 2009 by Cortina Systems Incorporated
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ZTE_SPECIFIC
#include "oam_cmn.h"
#include "oam_zte_pdu.h"
#include "oam_ctc_client.h"
#include "oam_zte_stats.h"
#include "oam_zte_adapt.h"

static zte_onu_port_flux_stats_ctrl_t *g_zte_port_flux_stats = NULL;

void zte_flux_stat_update(zte_onu_port_flux_stats_ctrl_t *ctrl)
{
    oam_zte_onu_port_stats stats;

    OAM_ORG_DEBUG("Enter.\n");

    memset(&stats, 0, sizeof(stats));
    zte_port_statistics_get(ctrl->port, &stats);
    /*update avail counter*/
    ctrl->avail_data.in_octets = stats.in_octets - ctrl->last_record.in_octets;
    ctrl->avail_data.in_uc_pkts = stats.in_ucast_pkt - ctrl->last_record.in_uc_pkts;
    ctrl->avail_data.in_nuc_pkts = stats.in_nucast_pkt -
                                   ctrl->last_record.in_nuc_pkts;
    ctrl->avail_data.in_discard_pkts = stats.in_discard -
                                       ctrl->last_record.in_discard_pkts;
    ctrl->avail_data.in_error_pkts = stats.in_error  -
                                     ctrl->last_record.in_error_pkts;
    ctrl->avail_data.out_octets = stats.out_octets - ctrl->last_record.out_octets;
    ctrl->avail_data.out_uc_pkts = stats.out_ucast_pkt -
                                   ctrl->last_record.out_uc_pkts;
    ctrl->avail_data.out_nuc_pkts = stats.out_nucast_pkt -
                                    ctrl->last_record.out_nuc_pkts;
    ctrl->avail_data.out_discard_pkts = stats.out_discard -
                                        ctrl->last_record.out_discard_pkts;
    ctrl->avail_data.out_error_pkts = stats.out_error -
                                      ctrl->last_record.out_error_pkts;

    /*store current counter*/
    ctrl->last_record.in_octets = stats.in_octets;
    ctrl->last_record.in_uc_pkts = stats.in_ucast_pkt;
    ctrl->last_record.in_nuc_pkts = stats.in_nucast_pkt;
    ctrl->last_record.in_discard_pkts = stats.in_discard;
    ctrl->last_record.in_error_pkts = stats.in_error;
    ctrl->last_record.out_octets = stats.out_octets;
    ctrl->last_record.out_uc_pkts = stats.out_ucast_pkt;
    ctrl->last_record.out_nuc_pkts = stats.out_nucast_pkt;
    ctrl->last_record.out_discard_pkts = stats.out_discard;
    ctrl->last_record.out_error_pkts = stats.out_error;
}

void zte_flux_stat_get_tmfunc(void *data)
{
    zte_onu_port_flux_stats_ctrl_t *ctrl = (zte_onu_port_flux_stats_ctrl_t *)data;

    zte_flux_stat_update(ctrl);

    if (ctrl->duration) {
        oam_uint32 time;

        if (ctrl->duration < ctrl->interval) {
            time = ctrl->duration;
        }
        else {
            time = ctrl->interval;
        }

        ctrl->duration -= time;

        ctrl->timer = ca_timer_add(1000 * time, zte_flux_stat_get_tmfunc, data);

    }
    else {
        /* duration timer is expired */
        memset(ctrl, 0, sizeof(zte_onu_port_flux_stats_ctrl_t));
        ctrl->timer = 0;
    }

}

oam_status zte_flux_stat_stop(
    oam_port_id_t port)
{
    zte_onu_port_flux_stats_ctrl_t *ctrl = NULL;

    if (port < OAM_UNI_PORT_ID1 || port > CTC_MAX_ONU_UNI_PORTS)
        return OAM_E_PARAM;

    ctrl = &g_zte_port_flux_stats[port - 1];

    if (ctrl->timer != 0) {
        ca_timer_del(ctrl->timer);
        ctrl->timer = 0;
    }

    memset(ctrl, 0, sizeof(zte_onu_port_flux_stats_ctrl_t));
    ctrl->port = port;

    return OAM_E_OK;
}

oam_status zte_flux_stat_start(
    oam_port_id_t port,
    oam_uint32  interval,
    oam_uint32  duration)
{
    zte_onu_port_flux_stats_ctrl_t *ctrl = NULL;

    if (port < OAM_UNI_PORT_ID1 || port > CTC_MAX_ONU_UNI_PORTS)
        return OAM_E_PARAM;

    ctrl = &g_zte_port_flux_stats[port - 1];
    /*stop first*/
    zte_flux_stat_stop(port);
    ctrl->interval = interval;
    ctrl->duration = duration;
    ctrl->port = port;
    ctrl->end_tick = ca_current_time() + (oam_uint64)duration * (oam_uint64)100;
    zte_flux_stat_get_tmfunc((void *)ctrl);
    memset(&ctrl->avail_data, 0, sizeof(zte_onu_port_flux_stats_t));

    return OAM_E_OK;
}

oam_status zte_port_stat_reset(oam_port_id_t port)
{
    if (port < OAM_UNI_PORT_ID1 || port > CTC_MAX_ONU_UNI_PORTS)
        return OAM_E_PARAM;

    memset(&(g_zte_port_flux_stats[port - 1].last_record), 0,
           sizeof(zte_onu_port_flux_stats_t));
    zte_oam_stats_reset_uni_adapt(port);

    return OAM_E_OK;
}

oam_status zte_flux_stat_ctrl_get(
    oam_port_id_t port,
    oam_uint8 *action,
    oam_uint32 *interval,
    oam_uint32 *duration)
{
    oam_uint32 remain = 0;
    oam_uint64 curr_tick = 0;

    if (port < OAM_UNI_PORT_ID1 || port > CTC_MAX_ONU_UNI_PORTS)
        return OAM_E_PARAM;

    *action = g_zte_port_flux_stats[port - 1].interval ? 1 : 0;
    *interval = g_zte_port_flux_stats[port - 1].interval;

    if (g_zte_port_flux_stats[port - 1].interval != 0) {
        curr_tick = ca_current_time();

        if (g_zte_port_flux_stats[port - 1].end_tick >= curr_tick) {
            remain = (g_zte_port_flux_stats[port - 1].end_tick - curr_tick) / 100;
        }
        else {
            remain = (0xffffffffffffffffLL - curr_tick +
                      g_zte_port_flux_stats[port - 1].end_tick + 1) / 100;
        }
    }

    *duration = remain;

    return OAM_E_OK;
}

oam_status zte_flux_statistics_get(
    oam_port_id_t port,
    zte_oam_onu_port_flux_stats_t *stats)
{
    zte_onu_port_flux_stats_ctrl_t *ctrl = NULL;

    if (port < OAM_UNI_PORT_ID1 || port > CTC_MAX_ONU_UNI_PORTS)
        return OAM_E_PARAM;

    if (!stats)
        return OAM_E_PARAM;

    ctrl = &g_zte_port_flux_stats[port - 1];

    stats->ctrl_ind         = ctrl->interval ? 1 : 0;
    stats->in_octets        = ctrl->avail_data.in_octets;
    stats->in_uc_pkts       = ctrl->avail_data.in_uc_pkts;
    stats->in_nuc_pkts      = ctrl->avail_data.in_nuc_pkts;
    stats->in_discard_pkts  = ctrl->avail_data.in_discard_pkts;
    stats->in_error_pkts    = ctrl->avail_data.in_error_pkts;
    stats->out_octets       = ctrl->avail_data.out_octets;
    stats->out_uc_pkts      = ctrl->avail_data.out_uc_pkts;
    stats->out_nuc_pkts     = ctrl->avail_data.out_nuc_pkts;
    stats->out_discard_pkts = ctrl->avail_data.out_discard_pkts;
    stats->out_error_pkts   = ctrl->avail_data.out_error_pkts;

    return OAM_E_OK;
}


void   zte_flux_stat_init()
{
    oam_uint32 len = 0;

    len = sizeof(zte_onu_port_flux_stats_ctrl_t) * CTC_MAX_ONU_UNI_PORTS;

    g_zte_port_flux_stats = (zte_onu_port_flux_stats_ctrl_t *)OAM_MALLOC(len);

    if (!g_zte_port_flux_stats)
        return;

    memset(g_zte_port_flux_stats, 0, len);
}

#endif/*END_ZTE_SPECIFIC*/

