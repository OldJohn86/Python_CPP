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

#ifdef HAVE_CTC_OAM
#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_ctc_client.h"
#include "oam_ctc_stats.h"
#include "oam_ctc_alarm.h"
#include "oam_ctc_adapt.h"

#define STATS_MONITOR_ALARM_CHECK(name, id)         value = ctrl->last_duration.name;  \
                                                    if(OAM_E_OK == ctc_oam_get_alarm_threshold(type, port,  \
                                                                              id, &raise_value, &clear_value)) \
                                                    {     \
                                                        if(value >= raise_value)  \
                                                        {     \
                                                            ctc_oam_build_alarm_event(type, port, id,  \
                                                                                                 CTC_ALARM_RAISE, value);  \
                                                        }  \
                                                        else if(value < clear_value) \
                                                        { \
                                                            ctc_oam_build_alarm_event(type, port, id,   \
                                                                                                   CTC_ALARM_CLEAR, value); \
                                                        }  \
                                                    }  \
 
/*pon port(index 0) and uni port(index 1...) monitor*/
ctc_stats_monitor_ctrl_t *g_monitor_stats = NULL;


void ctc_onu_stats_monitor_init()
{
    oam_uint8 i = 0;
    oam_uint32 len = 0;

    len = sizeof(ctc_stats_monitor_ctrl_t) * (CTC_MAX_ONU_UNI_PORTS + 1);

    g_monitor_stats = (ctc_stats_monitor_ctrl_t *)OAM_MALLOC(len);

    if (g_monitor_stats == NULL)
        return;

    memset(g_monitor_stats, 0, len);

    for (i = 0; i <= CTC_MAX_ONU_UNI_PORTS; i++) {
        g_monitor_stats[i].port = i;
    }
}

oam_status ctc_onu_stats_monitor_stop(oam_port_id_t port)
{
    ctc_stats_monitor_ctrl_t *ctrl = NULL;

    if (port > CTC_MAX_ONU_UNI_PORTS)
        return OAM_E_PARAM;

    ctrl = &g_monitor_stats[port];

    if (ctrl->timer != 0) {
        ca_timer_del(ctrl->timer);
        ctrl->timer = 0;
    }

    memset(ctrl, 0, sizeof(ctc_stats_monitor_ctrl_t));
    ctrl->port = port;

    return OAM_E_OK;
}

oam_status ctc_onu_performance_stats_get(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    if (port > CTC_MAX_ONU_UNI_PORTS || !stats)
        return OAM_E_PARAM;

    if (port == OAM_PON_PORT_ID) {
        ctc_oam_onu_pon_stat_get_adapt(stats);
    }
    else {
        ctc_oam_onu_uni_stat_get_adapt(port, stats);
    }

    return OAM_E_OK;
}

void ctc_onu_stats_monitor_alarm_check(ctc_stats_monitor_ctrl_t *ctrl)
{
    oam_uint64 value = 0;
    oam_uint32 raise_value = 0, clear_value  = 0;
    ctc_oam_alarm_class_e type = CTC_PON_ALARM;
    oam_port_id_t port = OAM_PON_PORT_ID;

    if (ctrl->port == 0) {
        type = CTC_PON_ALARM;
        port = OAM_PON_PORT_ID;

        /*check for CTC_ALARM_PON_RX_DROP*/
        STATS_MONITOR_ALARM_CHECK(dn_drop_events, CTC_ALARM_PON_RX_DROP)
        STATS_MONITOR_ALARM_CHECK(dn_drop_events, CTC_WARN_PON_RX_DROP)
        /*check for CTC_ALARM_PON_TX_DROP*/
        STATS_MONITOR_ALARM_CHECK(up_drop_events, CTC_ALARM_PON_TX_DROP)
        STATS_MONITOR_ALARM_CHECK(up_drop_events, CTC_WARN_PON_TX_DROP)
        /*check for CTC_ALARM_PON_RX_CRC*/
        STATS_MONITOR_ALARM_CHECK(dn_crc_err_frames, CTC_ALARM_PON_RX_CRC)
        STATS_MONITOR_ALARM_CHECK(dn_crc_err_frames, CTC_WARN_PON_RX_CRC)

        /*check for CTC_ALARM_PON_TX_CRC*/
        STATS_MONITOR_ALARM_CHECK(up_crc_err_frames, CTC_ALARM_PON_TX_CRC)
        STATS_MONITOR_ALARM_CHECK(up_crc_err_frames, CTC_WARN_PON_TX_CRC)

        /*check for CTC_ALARM_PON_RX_UNDER*/
        STATS_MONITOR_ALARM_CHECK(dn_undersize_frames, CTC_ALARM_PON_RX_UNDER)
        STATS_MONITOR_ALARM_CHECK(dn_undersize_frames, CTC_WARN_PON_RX_UNDER)
        /*check for CTC_ALARM_PON_TX_UNDER*/
        STATS_MONITOR_ALARM_CHECK(up_undersize_frames, CTC_ALARM_PON_TX_UNDER)
        STATS_MONITOR_ALARM_CHECK(up_undersize_frames, CTC_WARN_PON_TX_UNDER)
        /*check for CTC_ALARM_PON_RX_OVER*/
        STATS_MONITOR_ALARM_CHECK(dn_oversize_frames, CTC_ALARM_PON_RX_OVER)
        STATS_MONITOR_ALARM_CHECK(dn_oversize_frames, CTC_WARN_PON_RX_OVER)
        /*check for CTC_ALARM_PON_TX_OVER*/
        STATS_MONITOR_ALARM_CHECK(up_oversize_frames, CTC_ALARM_PON_TX_OVER)
        STATS_MONITOR_ALARM_CHECK(up_oversize_frames, CTC_WARN_PON_TX_OVER)
        /*check for CTC_ALARM_PON_RX_FRAG*/
        STATS_MONITOR_ALARM_CHECK(dn_fragments, CTC_ALARM_PON_RX_FRAG)
        STATS_MONITOR_ALARM_CHECK(dn_fragments, CTC_WARN_PON_RX_FRAG)

        /*check for CTC_ALARM_PON_TX_FRAG*/
        STATS_MONITOR_ALARM_CHECK(up_fragments, CTC_ALARM_PON_TX_FRAG)
        STATS_MONITOR_ALARM_CHECK(up_fragments, CTC_WARN_PON_TX_FRAG)

        /*check for CTC_ALARM_PON_RX_JABBER*/
        STATS_MONITOR_ALARM_CHECK(dn_jabbers, CTC_ALARM_PON_RX_JABBER)
        STATS_MONITOR_ALARM_CHECK(dn_jabbers, CTC_WARN_PON_RX_JABBER)

        /*check for CTC_ALARM_PON_TX_JABBER*/
        STATS_MONITOR_ALARM_CHECK(up_jabbers, CTC_ALARM_PON_TX_JABBER)
        STATS_MONITOR_ALARM_CHECK(up_jabbers, CTC_WARN_PON_TX_JABBER)


#if 0
        /*check for CTC_ALARM_PON_RX_COLLIS*/
        STATS_MONITOR_ALARM_CHECK(dn_collisions, CTC_ALARM_PON_RX_COLLIS)
        STATS_MONITOR_ALARM_CHECK(dn_collisions, CTC_WARN_PON_RX_COLLIS)
#endif

        /*check for CTC_ALARM_PON_RX_DISCA*/
        STATS_MONITOR_ALARM_CHECK(dn_discard_frames, CTC_ALARM_PON_RX_DISCA)
        STATS_MONITOR_ALARM_CHECK(dn_discard_frames, CTC_WARN_PON_RX_DISCA)
        /*check for CTC_ALARM_PON_TX_DISCA*/
        STATS_MONITOR_ALARM_CHECK(up_discard_frames, CTC_ALARM_PON_TX_DISCA)
        STATS_MONITOR_ALARM_CHECK(up_discard_frames, CTC_WARN_PON_TX_DISCA)
        /*check for CTC_ALARM_PON_RX_ERROR*/
        STATS_MONITOR_ALARM_CHECK(dn_err_frames, CTC_ALARM_PON_RX_ERROR)
        STATS_MONITOR_ALARM_CHECK(dn_err_frames, CTC_WARN_PON_RX_ERROR)

        /*check for CTC_ALARM_PON_TX_ERROR*/
        STATS_MONITOR_ALARM_CHECK(up_err_frames, CTC_ALARM_PON_TX_ERROR)
        STATS_MONITOR_ALARM_CHECK(up_err_frames, CTC_WARN_PON_TX_ERROR)


    }
    else {
        type = CTC_PORT_ALARM;
        port = ctrl->port;

        /*check for CTC_ALARM_PORT_RX_DROP*/
        STATS_MONITOR_ALARM_CHECK(dn_drop_events, CTC_ALARM_PORT_RX_DROP)
        STATS_MONITOR_ALARM_CHECK(dn_drop_events, CTC_WARN_PORT_RX_DROP)
        /*check for CTC_ALARM_PORT_TX_DROP*/
        STATS_MONITOR_ALARM_CHECK(up_drop_events, CTC_ALARM_PORT_TX_DROP)
        STATS_MONITOR_ALARM_CHECK(up_drop_events, CTC_WARN_PORT_TX_DROP)
        /*check for CTC_ALARM_PORT_RX_CRC*/
        STATS_MONITOR_ALARM_CHECK(dn_crc_err_frames, CTC_ALARM_PORT_RX_CRC)
        STATS_MONITOR_ALARM_CHECK(dn_crc_err_frames, CTC_WARN_PORT_RX_CRC)

        /*check for CTC_ALARM_PORT_TX_CRC*/
        STATS_MONITOR_ALARM_CHECK(up_crc_err_frames, CTC_ALARM_PORT_TX_CRC)
        STATS_MONITOR_ALARM_CHECK(up_crc_err_frames, CTC_WARN_PORT_TX_CRC)

        /*check for CTC_ALARM_PORT_RX_UNDER*/
        STATS_MONITOR_ALARM_CHECK(dn_undersize_frames, CTC_ALARM_PORT_RX_UNDER)
        STATS_MONITOR_ALARM_CHECK(dn_undersize_frames, CTC_WARN_PORT_RX_UNDER)
        /*check for CTC_ALARM_PORT_TX_UNDER*/
        STATS_MONITOR_ALARM_CHECK(up_undersize_frames, CTC_ALARM_PORT_TX_UNDER)
        STATS_MONITOR_ALARM_CHECK(up_undersize_frames, CTC_WARN_PORT_TX_UNDER)
        /*check for CTC_ALARM_PORT_RX_OVER*/
        STATS_MONITOR_ALARM_CHECK(dn_oversize_frames, CTC_ALARM_PORT_RX_OVER)
        STATS_MONITOR_ALARM_CHECK(dn_oversize_frames, CTC_WARN_PORT_RX_OVER)
        /*check for CTC_ALARM_PORT_TX_OVER*/
        STATS_MONITOR_ALARM_CHECK(up_oversize_frames, CTC_ALARM_PORT_TX_OVER)
        STATS_MONITOR_ALARM_CHECK(up_oversize_frames, CTC_WARN_PORT_TX_OVER)
        /*check for CTC_ALARM_PORT_RX_FRAG*/
        STATS_MONITOR_ALARM_CHECK(dn_fragments, CTC_ALARM_PORT_RX_FRAG)
        STATS_MONITOR_ALARM_CHECK(dn_fragments, CTC_WARN_PORT_RX_FRAG)
        /*check for CTC_ALARM_PORT_TX_FRAG*/
        STATS_MONITOR_ALARM_CHECK(up_fragments, CTC_ALARM_PORT_TX_FRAG)
        STATS_MONITOR_ALARM_CHECK(up_fragments, CTC_WARN_PORT_TX_FRAG)

        /*check for CTC_ALARM_PORT_RX_JABBER*/
        STATS_MONITOR_ALARM_CHECK(dn_jabbers, CTC_ALARM_PORT_RX_JABBER)
        STATS_MONITOR_ALARM_CHECK(dn_jabbers, CTC_WARN_PORT_RX_JABBER)

        /*check for CTC_ALARM_PORT_TX_JABBER*/
        STATS_MONITOR_ALARM_CHECK(up_jabbers, CTC_ALARM_PORT_TX_JABBER)
        STATS_MONITOR_ALARM_CHECK(up_jabbers, CTC_WARN_PORT_TX_JABBER)


#if 0
        /*check for CTC_ALARM_PORT_RX_COLLIS*/
        STATS_MONITOR_ALARM_CHECK(dn_collisions, CTC_ALARM_PORT_RX_COLLIS)
        STATS_MONITOR_ALARM_CHECK(dn_collisions, CTC_WARN_PORT_RX_COLLIS)
#endif

        /*check for CTC_ALARM_PORT_RX_DISCA*/
        STATS_MONITOR_ALARM_CHECK(dn_discard_frames, CTC_ALARM_PORT_RX_DISCA)
        STATS_MONITOR_ALARM_CHECK(dn_discard_frames, CTC_WARN_PORT_RX_DISCA)
        /*check for CTC_ALARM_PORT_TX_DISCA*/
        STATS_MONITOR_ALARM_CHECK(up_discard_frames, CTC_ALARM_PORT_TX_DISCA)
        STATS_MONITOR_ALARM_CHECK(up_discard_frames, CTC_WARN_PORT_TX_DISCA)
        /*check for CTC_ALARM_PORT_RX_ERROR*/
        STATS_MONITOR_ALARM_CHECK(dn_err_frames, CTC_ALARM_PORT_RX_ERROR)
        STATS_MONITOR_ALARM_CHECK(dn_err_frames, CTC_WARN_PORT_RX_ERROR)

        /*check for CTC_ALARM_PORT_TX_ERROR*/
        STATS_MONITOR_ALARM_CHECK(up_err_frames, CTC_ALARM_PORT_TX_ERROR)
        STATS_MONITOR_ALARM_CHECK(up_err_frames, CTC_WARN_PORT_TX_ERROR)

        /*check for CTC_ALARM_PORT_STATUS*/
        STATS_MONITOR_ALARM_CHECK(status_change_times, CTC_ALARM_PORT_STATUS)
        STATS_MONITOR_ALARM_CHECK(status_change_times, CTC_WARN_PORT_STATUS)
    }
}

void ctc_onu_stats_monitor_update(ctc_stats_monitor_ctrl_t *ctrl)
{
    oam_ctc_onu_port_stats_t port_stats;
    ctc_onu_performance_stats_get(ctrl->port, &port_stats);

    ctrl->last_duration.dn_drop_events            = port_stats.dn_drop_events -
            ctrl->last_save.dn_drop_events;
    ctrl->last_duration.up_drop_events            = port_stats.up_drop_events -
            ctrl->last_save.up_drop_events;
    ctrl->last_duration.dn_octets                 = port_stats.dn_octets -
            ctrl->last_save.dn_octets;
    ctrl->last_duration.up_octets                 = port_stats.up_octets -
            ctrl->last_save.up_octets;
    ctrl->last_duration.dn_frames                 = port_stats.dn_frames -
            ctrl->last_save.dn_frames;
    ctrl->last_duration.up_frames                 = port_stats.up_frames -
            ctrl->last_save.up_frames;
    ctrl->last_duration.dn_bc_frames              = port_stats.dn_bc_frames -
            ctrl->last_save.dn_bc_frames;
    ctrl->last_duration.up_bc_frames              = port_stats.up_bc_frames -
            ctrl->last_save.up_bc_frames;
    ctrl->last_duration.dn_mc_frames              = port_stats.dn_mc_frames -
            ctrl->last_save.dn_mc_frames;
    ctrl->last_duration.up_mc_frames              = port_stats.up_mc_frames -
            ctrl->last_save.up_mc_frames;
    ctrl->last_duration.dn_crc_err_frames         = port_stats.dn_crc_err_frames -
            ctrl->last_save.dn_crc_err_frames;
    ctrl->last_duration.up_crc_err_frames         = port_stats.up_crc_err_frames -
            ctrl->last_save.up_crc_err_frames;


    ctrl->last_duration.dn_undersize_frames       = port_stats.dn_undersize_frames -
            ctrl->last_save.dn_undersize_frames;
    ctrl->last_duration.up_undersize_frames       = port_stats.up_undersize_frames -
            ctrl->last_save.up_undersize_frames;
    ctrl->last_duration.dn_oversize_frames        = port_stats.dn_oversize_frames -
            ctrl->last_save.dn_oversize_frames;
    ctrl->last_duration.up_oversize_frames        = port_stats.up_oversize_frames -
            ctrl->last_save.up_oversize_frames;
    ctrl->last_duration.dn_fragments              = port_stats.dn_fragments -
            ctrl->last_save.dn_fragments;
    ctrl->last_duration.up_fragments              = port_stats.up_fragments -
            ctrl->last_save.up_fragments;
    ctrl->last_duration.dn_jabbers                = port_stats.dn_jabbers -
            ctrl->last_save.dn_jabbers;
    ctrl->last_duration.up_jabbers                = port_stats.up_jabbers -
            ctrl->last_save.up_jabbers;
    /* ctrl->last_duration.dn_collisions             = port_stats.dn_collisions - ctrl->last_save.dn_collisions;*/
    ctrl->last_duration.dn_64_octets_frames       = port_stats.dn_64_octets_frames -
            ctrl->last_save.dn_64_octets_frames;
    ctrl->last_duration.dn_65_127_octets_frames   =
        port_stats.dn_65_127_octets_frames - ctrl->last_save.dn_65_127_octets_frames;
    ctrl->last_duration.dn_128_255_octets_frames  =
        port_stats.dn_128_255_octets_frames - ctrl->last_save.dn_128_255_octets_frames;
    ctrl->last_duration.dn_256_511_octets_frames  =
        port_stats.dn_256_511_octets_frames - ctrl->last_save.dn_256_511_octets_frames;
    ctrl->last_duration.dn_512_1023_octets_frames =
        port_stats.dn_512_1023_octets_frames -
        ctrl->last_save.dn_512_1023_octets_frames;
    ctrl->last_duration.dn_1024_1518_octets_frames =
        port_stats.dn_1024_1518_octets_frames -
        ctrl->last_save.dn_1024_1518_octets_frames;
    ctrl->last_duration.up_64_octets_frames       = port_stats.up_64_octets_frames -
            ctrl->last_save.up_64_octets_frames;
    ctrl->last_duration.up_65_127_octets_frames   =
        port_stats.up_65_127_octets_frames - ctrl->last_save.up_65_127_octets_frames;
    ctrl->last_duration.up_128_255_octets_frames  =
        port_stats.up_128_255_octets_frames - ctrl->last_save.up_128_255_octets_frames;
    ctrl->last_duration.up_256_511_octets_frames  =
        port_stats.up_256_511_octets_frames - ctrl->last_save.up_256_511_octets_frames;
    ctrl->last_duration.up_512_1023_octets_frames =
        port_stats.up_512_1023_octets_frames -
        ctrl->last_save.up_512_1023_octets_frames;
    ctrl->last_duration.up_1024_1518_octets_frames =
        port_stats.up_1024_1518_octets_frames -
        ctrl->last_save.up_1024_1518_octets_frames;
    ctrl->last_duration.dn_discard_frames         = port_stats.dn_discard_frames -
            ctrl->last_save.dn_discard_frames;
    ctrl->last_duration.up_discard_frames         = port_stats.up_discard_frames -
            ctrl->last_save.up_discard_frames;
    ctrl->last_duration.dn_err_frames             = port_stats.dn_err_frames -
            ctrl->last_save.dn_err_frames;
    ctrl->last_duration.up_err_frames             = port_stats.up_err_frames -
            ctrl->last_save.up_err_frames;
    ctrl->last_duration.status_change_times       = port_stats.status_change_times -
            ctrl->last_save.status_change_times;

    memcpy(&(ctrl->last_save), &port_stats, sizeof(port_stats));
}

void ctc_onu_status_monitor_timer_expire(void *data)
{
    oam_timer_msg_send(OAM_CTC_STATS_TIMER_EXPIRED, data);
}

void ctc_onu_stats_monitor_timer_handle(void *data)
{
    ctc_stats_monitor_ctrl_t *ctrl = (ctc_stats_monitor_ctrl_t *)data;

    if (!ctrl)
        return;

    ctc_onu_stats_monitor_update(ctrl);

    /*check alarm*/
    ctc_onu_stats_monitor_alarm_check(ctrl);
}

void ctc_onu_stats_monitor_start_timer(ctc_stats_monitor_ctrl_t *ctrl)
{
    ctrl->timer = ca_circle_timer_add((ctrl->interval) * 1000,
                                      ctc_onu_status_monitor_timer_expire, (void *)ctrl);
}

oam_status ctc_onu_stats_monitor_start(
    oam_port_id_t port,
    oam_uint32 interval)
{
    ctc_stats_monitor_ctrl_t *ctrl = NULL;

    if (port > CTC_MAX_ONU_UNI_PORTS)
        return OAM_E_PARAM;

    ctc_onu_stats_monitor_stop(port);

    ctrl = &g_monitor_stats[port];
    ctrl->interval = interval;

    /* init data*/
    ctc_onu_stats_monitor_update(ctrl);
    memset(&ctrl->last_duration, 0, sizeof(oam_ctc_onu_port_stats_t));

    ctc_onu_stats_monitor_start_timer(ctrl);

    return OAM_E_OK;

}

oam_status ctc_onu_stats_monitor_restart(oam_port_id_t port)
{
    ctc_stats_monitor_ctrl_t *ctrl = NULL;
    oam_uint32 interval = 0;
    oam_ctc_onu_port_stats_t last_duration;

    if (port > CTC_MAX_ONU_UNI_PORTS)
        return OAM_E_PARAM;


    ctrl = &g_monitor_stats[port];

    if (ctrl->timer == 0)
        return OAM_E_ERROR;

    interval = ctrl->interval;
    last_duration = ctrl->last_duration;

    /*restart current stats monitor, history stats should keep unchange?*/
    ctc_onu_stats_monitor_start(port, interval);
    ctrl->last_duration = last_duration;
    return OAM_E_OK;
}

oam_status ctc_onu_stats_monitor_status_get(
    oam_port_id_t port,
    oam_uint16 *monitor_status,
    oam_uint32 *monitor_period)
{
    if (port > CTC_MAX_ONU_UNI_PORTS)
        return OAM_E_PARAM;

    if (g_monitor_stats[port].timer != 0) {
        *monitor_status = MONITOR_STATUS_ENABLE;
    }
    else
        *monitor_status = MONITOR_STATUS_DISABLE;

    *monitor_period = g_monitor_stats[port].interval;
    return OAM_E_OK;
}

oam_status ctc_onu_stats_monitor_status_set(
    oam_port_id_t port,
    oam_uint16 monitor_status,
    oam_uint32  monitor_period)
{
    if (monitor_status == MONITOR_STATUS_DISABLE) {
        /*stop monitor*/
        return ctc_onu_stats_monitor_stop(port);
    }
    else if (monitor_status == MONITOR_STATUS_ENABLE) {
        /*enable monitor*/
        return ctc_onu_stats_monitor_start(port, monitor_period);
    }
    else {
        return OAM_E_PARAM;
    }
}

oam_status ctc_onu_monitor_stats_current_get(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    oam_ctc_onu_port_stats_t port_stats;
    ctc_stats_monitor_ctrl_t *ctrl = NULL;

    if (port > CTC_MAX_ONU_UNI_PORTS || !stats)
        return OAM_E_PARAM;

    ctrl = &g_monitor_stats[port];
    ctc_onu_performance_stats_get(port, &port_stats);

    stats->dn_drop_events            = port_stats.dn_drop_events -
                                       ctrl->last_save.dn_drop_events;
    stats->up_drop_events            = port_stats.up_drop_events -
                                       ctrl->last_save.up_drop_events;
    stats->dn_octets                 = port_stats.dn_octets -
                                       ctrl->last_save.dn_octets;
    stats->up_octets                 = port_stats.up_octets -
                                       ctrl->last_save.up_octets;
    stats->dn_frames                 = port_stats.dn_frames -
                                       ctrl->last_save.dn_frames;
    stats->up_frames                 = port_stats.up_frames -
                                       ctrl->last_save.up_frames;
    stats->dn_bc_frames              = port_stats.dn_bc_frames -
                                       ctrl->last_save.dn_bc_frames;
    stats->up_bc_frames              = port_stats.up_bc_frames -
                                       ctrl->last_save.up_bc_frames;
    stats->dn_mc_frames              = port_stats.dn_mc_frames -
                                       ctrl->last_save.dn_mc_frames;
    stats->up_mc_frames              = port_stats.up_mc_frames -
                                       ctrl->last_save.up_mc_frames;
    stats->dn_crc_err_frames         = port_stats.dn_crc_err_frames -
                                       ctrl->last_save.dn_crc_err_frames;
    stats->up_crc_err_frames         = port_stats.up_crc_err_frames -
                                       ctrl->last_save.up_crc_err_frames;

    stats->dn_undersize_frames       = port_stats.dn_undersize_frames -
                                       ctrl->last_save.dn_undersize_frames;
    stats->up_undersize_frames       = port_stats.up_undersize_frames -
                                       ctrl->last_save.up_undersize_frames;
    stats->dn_oversize_frames        = port_stats.dn_oversize_frames -
                                       ctrl->last_save.dn_oversize_frames;
    stats->up_oversize_frames        = port_stats.up_oversize_frames -
                                       ctrl->last_save.up_oversize_frames;
    stats->dn_fragments              = port_stats.dn_fragments -
                                       ctrl->last_save.dn_fragments;
    stats->up_fragments              = port_stats.up_fragments -
                                       ctrl->last_save.up_fragments;
    stats->dn_jabbers                = port_stats.dn_jabbers -
                                       ctrl->last_save.dn_jabbers;
    stats->up_jabbers                = port_stats.up_jabbers -
                                       ctrl->last_save.up_jabbers;
    /* stats->dn_collisions             = port_stats.dn_collisions - ctrl->last_save.dn_collisions;*/
    stats->dn_64_octets_frames       = port_stats.dn_64_octets_frames -
                                       ctrl->last_save.dn_64_octets_frames;
    stats->dn_65_127_octets_frames   = port_stats.dn_65_127_octets_frames -
                                       ctrl->last_save.dn_65_127_octets_frames;
    stats->dn_128_255_octets_frames  = port_stats.dn_128_255_octets_frames -
                                       ctrl->last_save.dn_128_255_octets_frames;
    stats->dn_256_511_octets_frames  = port_stats.dn_256_511_octets_frames -
                                       ctrl->last_save.dn_256_511_octets_frames;
    stats->dn_512_1023_octets_frames = port_stats.dn_512_1023_octets_frames -
                                       ctrl->last_save.dn_512_1023_octets_frames;
    stats->dn_1024_1518_octets_frames = port_stats.dn_1024_1518_octets_frames -
                                        ctrl->last_save.dn_1024_1518_octets_frames;
    stats->up_64_octets_frames       = port_stats.up_64_octets_frames -
                                       ctrl->last_save.up_64_octets_frames;
    stats->up_65_127_octets_frames   = port_stats.up_65_127_octets_frames -
                                       ctrl->last_save.up_65_127_octets_frames;
    stats->up_128_255_octets_frames  = port_stats.up_128_255_octets_frames -
                                       ctrl->last_save.up_128_255_octets_frames;
    stats->up_256_511_octets_frames  = port_stats.up_256_511_octets_frames -
                                       ctrl->last_save.up_256_511_octets_frames;
    stats->up_512_1023_octets_frames = port_stats.up_512_1023_octets_frames -
                                       ctrl->last_save.up_512_1023_octets_frames;
    stats->up_1024_1518_octets_frames = port_stats.up_1024_1518_octets_frames -
                                        ctrl->last_save.up_1024_1518_octets_frames;
    stats->dn_discard_frames         = port_stats.dn_discard_frames -
                                       ctrl->last_save.dn_discard_frames;
    stats->up_discard_frames         = port_stats.up_discard_frames -
                                       ctrl->last_save.up_discard_frames;
    stats->dn_err_frames             = port_stats.dn_err_frames -
                                       ctrl->last_save.dn_err_frames;
    stats->up_err_frames             = port_stats.up_err_frames -
                                       ctrl->last_save.up_err_frames;
    stats->status_change_times       = port_stats.status_change_times -
                                       ctrl->last_save.status_change_times;
    return OAM_E_OK;
}

oam_status ctc_onu_monitor_stats_histroy_get(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    if (port > CTC_MAX_ONU_UNI_PORTS || !stats)
        return OAM_E_PARAM;

    memcpy(stats, &g_monitor_stats[port].last_duration,
           sizeof(oam_ctc_onu_port_stats_t));
    return OAM_E_OK;
}

void ctc_onu_stats_dump(oam_port_id_t port)
{
    if (port > CTC_MAX_ONU_UNI_PORTS)
        return;

    oam_ctc_onu_port_stats_t *stats = &g_monitor_stats[port].last_duration;
    oam_printf("last period:\n");

    oam_printf("dn_drop_events            = %llx \n",
               stats->dn_drop_events            );
    oam_printf("up_drop_events            = %llx \n",
               stats->up_drop_events            );
    oam_printf("dn_octets                 = %llx \n",
               stats->dn_octets                 );
    oam_printf("up_octets                 = %llx \n",
               stats->up_octets                 );
    oam_printf("dn_frames                 = %llx \n",
               stats->dn_frames                 );
    oam_printf("up_frames                 = %llx \n",
               stats->up_frames                 );
    oam_printf("dn_bc_frames              = %llx \n",
               stats->dn_bc_frames              );
    oam_printf("up_bc_frames              = %llx \n",
               stats->up_bc_frames              );
    oam_printf("dn_mc_frames              = %llx \n",
               stats->dn_mc_frames              );
    oam_printf("up_mc_frames              = %llx \n",
               stats->up_mc_frames              );
    oam_printf("dn_crc_err_frames         = %llx \n",
               stats->dn_crc_err_frames         );
    oam_printf("up_crc_err_frames         = %llx \n",
               stats->up_crc_err_frames         );

    oam_printf("dn_undersize_frames       = %llx \n",
               stats->dn_undersize_frames       );
    oam_printf("up_undersize_frames       = %llx \n",
               stats->up_undersize_frames       );
    oam_printf("dn_oversize_frames        = %llx \n",
               stats->dn_oversize_frames        );
    oam_printf("up_oversize_frames        = %llx \n",
               stats->up_oversize_frames        );
    oam_printf("dn_fragments              = %llx \n",
               stats->dn_fragments              );
    oam_printf("up_fragments              = %llx \n",
               stats->up_fragments              );
    oam_printf("dn_jabbers                = %llx \n",
               stats->dn_jabbers                );
    oam_printf("up_jabbers                = %llx \n",
               stats->up_jabbers                );
    /*    oam_printf("dn_collisions             = %llx \n",stats->dn_collisions             );    */
    oam_printf("dn_64_octets_frames       = %llx \n",
               stats->dn_64_octets_frames       );
    oam_printf("dn_65_127_octets_frames   = %llx \n",
               stats->dn_65_127_octets_frames   );
    oam_printf("dn_128_255_octets_frames  = %llx \n",
               stats->dn_128_255_octets_frames  );
    oam_printf("dn_256_511_octets_frames  = %llx \n",
               stats->dn_256_511_octets_frames  );
    oam_printf("dn_512_1023_octets_frames = %llx \n",
               stats->dn_512_1023_octets_frames );
    oam_printf("dn_1024_1518_octets_frames= %llx \n",
               stats->dn_1024_1518_octets_frames);
    oam_printf("up_64_octets_frames       = %llx \n",
               stats->up_64_octets_frames       );
    oam_printf("up_65_127_octets_frames   = %llx \n",
               stats->up_65_127_octets_frames   );
    oam_printf("up_128_255_octets_frames  = %llx \n",
               stats->up_128_255_octets_frames  );
    oam_printf("up_256_511_octets_frames  = %llx \n",
               stats->up_256_511_octets_frames  );
    oam_printf("up_512_1023_octets_frames = %llx \n",
               stats->up_512_1023_octets_frames );
    oam_printf("up_1024_1518_octets_frames= %llx \n",
               stats->up_1024_1518_octets_frames);
    oam_printf("dn_discard_frames         = %llx \n",
               stats->dn_discard_frames         );
    oam_printf("up_discard_frames         = %llx \n",
               stats->up_discard_frames         );
    oam_printf("dn_err_frames             = %llx \n",
               stats->dn_err_frames             );
    oam_printf("up_err_frames             = %llx \n",
               stats->up_err_frames             );
    oam_printf("status_change_times       = %llx \n",
               stats->status_change_times       );

    stats = &g_monitor_stats[port].last_save;
    oam_printf("last point:\n");

    oam_printf("dn_drop_events            = %llx \n",
               stats->dn_drop_events            );
    oam_printf("up_drop_events            = %llx \n",
               stats->up_drop_events            );
    oam_printf("dn_octets                 = %llx \n",
               stats->dn_octets                 );
    oam_printf("up_octets                 = %llx \n",
               stats->up_octets                 );
    oam_printf("dn_frames                 = %llx \n",
               stats->dn_frames                 );
    oam_printf("up_frames                 = %llx \n",
               stats->up_frames                 );
    oam_printf("dn_bc_frames              = %llx \n",
               stats->dn_bc_frames              );
    oam_printf("up_bc_frames              = %llx \n",
               stats->up_bc_frames              );
    oam_printf("dn_mc_frames              = %llx \n",
               stats->dn_mc_frames              );
    oam_printf("up_mc_frames              = %llx \n",
               stats->up_mc_frames              );
    oam_printf("dn_crc_err_frames         = %llx \n",
               stats->dn_crc_err_frames         );
    oam_printf("up_crc_err_frames         = %llx \n",
               stats->up_crc_err_frames         );

    oam_printf("dn_undersize_frames       = %llx \n",
               stats->dn_undersize_frames       );
    oam_printf("up_undersize_frames       = %llx \n",
               stats->up_undersize_frames       );
    oam_printf("dn_oversize_frames        = %llx \n",
               stats->dn_oversize_frames        );
    oam_printf("up_oversize_frames        = %llx \n",
               stats->up_oversize_frames        );
    oam_printf("dn_fragments              = %llx \n",
               stats->dn_fragments              );
    oam_printf("up_fragments              = %llx \n",
               stats->up_fragments              );
    oam_printf("dn_jabbers                = %llx \n",
               stats->dn_jabbers                );
    oam_printf("up_jabbers                = %llx \n",
               stats->up_jabbers                );
    /*oam_printf("dn_collisions             = %llx \n",stats->dn_collisions             ); */
    oam_printf("dn_64_octets_frames       = %llx \n",
               stats->dn_64_octets_frames       );
    oam_printf("dn_65_127_octets_frames   = %llx \n",
               stats->dn_65_127_octets_frames   );
    oam_printf("dn_128_255_octets_frames  = %llx \n",
               stats->dn_128_255_octets_frames  );
    oam_printf("dn_256_511_octets_frames  = %llx \n",
               stats->dn_256_511_octets_frames  );
    oam_printf("dn_512_1023_octets_frames = %llx \n",
               stats->dn_512_1023_octets_frames );
    oam_printf("dn_1024_1518_octets_frames= %llx \n",
               stats->dn_1024_1518_octets_frames);
    oam_printf("up_64_octets_frames       = %llx \n",
               stats->up_64_octets_frames       );
    oam_printf("up_65_127_octets_frames   = %llx \n",
               stats->up_65_127_octets_frames   );
    oam_printf("up_128_255_octets_frames  = %llx \n",
               stats->up_128_255_octets_frames  );
    oam_printf("up_256_511_octets_frames  = %llx \n",
               stats->up_256_511_octets_frames  );
    oam_printf("up_512_1023_octets_frames = %llx \n",
               stats->up_512_1023_octets_frames );
    oam_printf("up_1024_1518_octets_frames= %llx \n",
               stats->up_1024_1518_octets_frames);
    oam_printf("dn_discard_frames         = %llx \n",
               stats->dn_discard_frames         );
    oam_printf("up_discard_frames         = %llx \n",
               stats->up_discard_frames         );
    oam_printf("dn_err_frames             = %llx \n",
               stats->dn_err_frames             );
    oam_printf("up_err_frames             = %llx \n",
               stats->up_err_frames             );
    oam_printf("status_change_times       = %llx \n",
               stats->status_change_times       );
}

#endif

