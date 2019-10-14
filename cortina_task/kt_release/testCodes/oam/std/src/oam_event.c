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
#include <arpa/inet.h>
#include <time.h>

#include "oam_main.h"
#include "oam_cmn.h"
#include "oam_std_pdu.h"
#include "oam_adapt.h"
#include "oam_core.h"
#include "oam_client.h"
#include "oam_mux_par.h"
#include "oam_event.h"

static  oam_link_event_t   oam_link_event_table[OAM_EVENT_TLV_RERV];
static oam_link_event_counter_t g_last_counter;
static oam_uint32     g_link_event_handler = 0;

static void oam_err_event_init();
static void oam_event_polling_timer_expire(void *data);
static oam_status oam_event_timer_init();

/*****************************************************************************/
/* $rtn_hdr_start  oam_event_init                                            */
/* CATEGORY   : App                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ---                                                          */
/*                                                                           */
void oam_event_init()
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* oam event init                                                            */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_err_event_init();

    oam_event_timer_init();
}

static void oam_err_event_init()
{
    memset(oam_link_event_table, 0x00, sizeof(oam_link_event_table));

    oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].window
        = OAM_LINK_ERROR_SYMBOL_PER_SECOND;

    oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].threshold = 1;

    oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].count_save
        = oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].window /
          OAM_LINK_ERROR_SYMBOL_PER_SECOND * OAM_LINK_ERROR_SYMBOL_UNIT /
          OAM_LINK_EVENT_POLL_ACCURACY;

    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].window
        = OAM_LINK_ERROR_FRAME_WIN_MIN;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].threshold = 1;

    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].count_save
        = oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].window /
          OAM_LINK_EVENT_POLL_ACCURACY;

    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].window
        = OAM_LINK_ERROR_PERIOD_PER_SECOND;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].threshold = 1;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].count_save
        = oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].window /
          OAM_LINK_ERROR_PERIOD_PER_SECOND * 1000 / OAM_LINK_EVENT_POLL_ACCURACY;

    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].window
        = OAM_LINK_ERROR_FRAME_SECOND_SUMMERY_WIN_MIN * 6;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].threshold = 10;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].count_save
        = OAM_LINK_ERROR_FRAME_WIN_MIN / OAM_LINK_EVENT_POLL_ACCURACY;

    memset(&g_last_counter, 0, sizeof(g_last_counter));

}

void oam_link_event_reset()
{
    oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].enable = 1;
    oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].count =
        oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].count_save;
    oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].cur_value = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].cur_window = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].error_running_total = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].event_running_total = 0;

    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].enable = 1;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].count =
        oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].count_save;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].cur_value = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].cur_window = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].error_running_total = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].event_running_total = 0;

    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].enable = 1;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].count =
        oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].count_save;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].cur_value = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].cur_window = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].error_running_total = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].event_running_total = 0;

    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].enable = 1;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].count =
        oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].count_save;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].cur_value = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].cur_window = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].error_running_total = 0;
    oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].event_running_total = 0;

    memset(&g_last_counter, 0, sizeof(g_last_counter));
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_event_timer_init                                      */
/* CATEGORY   : App                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ---                                                          */
/*                                                                           */
static oam_status oam_event_timer_init()
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* oam event init                                                            */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    g_link_event_handler = ca_circle_timer_add( OAM_LINK_EVENT_POLL_ACCURACY,
                           oam_event_polling_timer_expire, NULL);

    if (g_link_event_handler == 0) {
        OAM_LOG_MAJ("ca_circle_timer_add failed.\n");
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}


oam_uint32 oam_event_build_evt(
    oam_uint8   type,
    oam_uint8  *buf,
    oam_uint16  timestamp,
    oam_uint64  window,
    oam_uint64  threshold,
    oam_uint64  errors,
    oam_uint64  error_running_total,
    oam_uint32  event_running_total
)
{
    /* optimization of the alarm feature */
    oam_tlv_link_event_t event;
    oam_tlv_link_errored_event_t errored_event;
    oam_tlv_link_errored_period_event_t errored_period_event;
    oam_tlv_link_errored_seconds_summary_event_t errored_summary_event;
    oam_uint32 len = 0;

    OAM_ASSERT_RET(buf != NULL, 0);

    memset(&event, 0, sizeof(oam_tlv_link_event_t));
    memset(&errored_event, 0, sizeof(oam_tlv_link_errored_event_t));
    memset(&errored_period_event, 0,
           sizeof(oam_tlv_link_errored_period_event_t));
    memset(&errored_summary_event, 0,
           sizeof(oam_tlv_link_errored_seconds_summary_event_t));

    switch (type) {
        case OAM_EVENT_TLV_ERR_SYM_PERIOD:
            event.type = OAM_EVENT_TLV_ERR_SYM_PERIOD;
            event.length = sizeof(oam_tlv_link_event_t);
            event.timestamp = htons(timestamp);
            event.window = htonll(window);
            event.threshold = htonll(threshold);
            event.errors = htonll(errors);
            event.error_running_total = htonll(error_running_total);
            event.event_running_total = htonl(event_running_total);
            len = sizeof(event);
            memcpy(buf, &event, len);

            break;

        case OAM_EVENT_TLV_ERR_FRAME_PERIOD:
            errored_period_event.type = OAM_EVENT_TLV_ERR_FRAME_PERIOD;
            errored_period_event.length =
                sizeof(oam_tlv_link_errored_period_event_t);
            errored_period_event.timestamp = htons(timestamp);
            errored_period_event.window = htonl(( unsigned long )window);
            errored_period_event.threshold = htonl(( unsigned long )threshold);
            errored_period_event.errors = htonl(( unsigned long )errors);
            errored_period_event.error_running_total = htonll(error_running_total);
            errored_period_event.event_running_total = htonl(event_running_total);
            len = sizeof(errored_period_event);
            memcpy(buf, &errored_period_event, len);

            break;

        case OAM_EVENT_TLV_ERR_FRAME:
            errored_event.type = OAM_EVENT_TLV_ERR_FRAME;
            errored_event.length = sizeof(oam_tlv_link_errored_event_t);
            errored_event.timestamp = htons(timestamp);
            errored_event.window = htons((oam_uint16)(window *
                                         10)); /* translate 1 second to 100ms */
            errored_event.threshold = htonl((unsigned long)threshold);
            errored_event.errors = htonl((unsigned long)errors);
            errored_event.error_running_total = htonll(error_running_total);
            errored_event.event_running_total = htonl(event_running_total);

            len = sizeof(errored_event);
            memcpy(buf, &errored_event, len);

            break;

        case OAM_EVENT_TLV_ERR_FRAME_SUMM:
            errored_summary_event.type = OAM_EVENT_TLV_ERR_FRAME_SUMM;
            errored_summary_event.length =
                sizeof(oam_tlv_link_errored_seconds_summary_event_t);
            errored_summary_event.timestamp = htons(timestamp);
            errored_summary_event.window = htons((oam_uint16)(window *
                                                 10)); /* translate 1 second to 100ms */
            errored_summary_event.threshold = htons((oam_uint16)threshold);
            errored_summary_event.errors = htons((oam_uint16)errors);
            errored_summary_event.error_running_total =
                htonl((unsigned long)error_running_total);
            errored_summary_event.event_running_total =
                htonl(event_running_total);

            len = sizeof(errored_summary_event);
            memcpy(buf, &errored_summary_event, len);

            break;

        default:
            OAM_LOG_MAJ("Unknown alarm type 0x%x not sent\n", type);

            break;
    }

    return len;
}

void oam_event_period_counter_get(
    oam_uint64 *in_frames,
    oam_uint64 *in_errors,
    oam_uint64 *in_err_symb)
{
    oam_uint64 frames = 0, errors = 0, err_symb = 0;
    oam_uint32 enable = 0;

    /* PON port */
    oam_stats_polling_enable_get(0, &enable);

    if(!enable)
    { 
        *in_frames = 0;
    	*in_errors = 0;
    	*in_err_symb = 0;
        return;
    }
    
    oam_pon_stats_summary_get_adapt(FALSE, &frames, &errors, &err_symb);
    
    *in_frames = frames - g_last_counter.in_frames;
    *in_errors = errors - g_last_counter.in_errors;
    *in_err_symb = err_symb - g_last_counter.in_err_symb;

    g_last_counter.in_frames = frames;
    g_last_counter.in_errors = errors;
    g_last_counter.in_err_symb = err_symb;

    return;
}

static void oam_event_polling_timer_expire(void *data)
{
    oam_timer_msg_send(OAM_LINK_EVENT_TIMER_EXPIRED, data);
}


void oam_event_process_polling(void *timer)
{
        oam_link_event_t *evt;
        oam_uint64 in_frames = 0, in_errors = 0;
        oam_uint64 in_err_symb = 0;
        oam_boolean bm_status = FALSE;
        oam_uint8 pdu[OAM_MAX_OAMPDU_SIZE];
        oam_uint32 len = 0;
        oam_uint8 send_flag = 0;
        oam_pdu_event_t *pevent = (oam_pdu_event_t *)pdu;
        oam_if_t *oam_intf = NULL;
        oam_uint8  event_list[] = {
            OAM_EVENT_TLV_ERR_SYM_PERIOD,
            OAM_EVENT_TLV_ERR_FRAME,
            OAM_EVENT_TLV_ERR_FRAME_PERIOD,
            OAM_EVENT_TLV_ERR_FRAME_SUMM
        };
        oam_uint8 i = 0;
    
        memset(pdu, 0, sizeof(pdu));
    
        /*TODO:Geesun, need add mLLID support here */
        oam_intf = oam_intf_find_valid();
    
        if (oam_intf == NULL)
            return;
    
        oam_event_period_counter_get(&in_frames, &in_errors, &in_err_symb);
    
        oam_build_pdu_hdr(oam_intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_EVENT);
        len += sizeof(oam_pdu_hdr_t);
        ++oam_intf->seq_num_next_tx;
        pevent->seq_num = htons(oam_intf->seq_num_next_tx);
        len += 2;
    
        for (i = 0; i < sizeof(event_list) / sizeof(oam_uint8); i++) {
            oam_boolean send_event_cond = FALSE;
            oam_uint64  window = 0 ;
            evt = &oam_link_event_table[event_list[i]];
    
            if (!evt->enable) {
                continue;
            }

        switch(event_list[i]){
            case OAM_EVENT_TLV_ERR_SYM_PERIOD:
                evt->cur_value += in_err_symb;
                evt->cur_window += OAM_LINK_ERROR_SYMBOL_PER_SECOND / OAM_LINK_ERROR_SYMBOL_UNIT
                                   * OAM_LINK_EVENT_POLL_ACCURACY;
                window = evt->window;

                send_event_cond = (evt->cur_value >= evt->threshold) &&
                    (evt->cur_window >= evt->window) ;
                break;
            case OAM_EVENT_TLV_ERR_FRAME:
                evt->cur_value += in_errors;
                window = evt->window/OAM_LINK_EVENT_POLL_ACCURACY;

                send_event_cond = (evt->cur_value >= evt->threshold);
                break;
            case OAM_EVENT_TLV_ERR_FRAME_PERIOD:
                evt->cur_value += in_errors;
                evt->cur_window += in_frames + in_errors;
                window = evt->window;

                send_event_cond = (evt->cur_value >= evt->threshold) &&
                    (evt->cur_window >= evt->window) ;
                break;
            case OAM_EVENT_TLV_ERR_FRAME_SUMM:
                evt->cur_window += OAM_LINK_EVENT_POLL_ACCURACY;
                window = evt->window/OAM_LINK_EVENT_POLL_ACCURACY;
                
                if((evt->count - 1 ) == 0) {
                    if(in_errors > 0)
                        evt->cur_value += 1;
                }

                send_event_cond = (evt->cur_value >= evt->threshold) &&
                    (evt->cur_window >= evt->window) ;
                break;
        }

        if(--evt->count == 0) {
            if(send_event_cond){
                evt->error_running_total += evt->cur_value;
                evt->event_running_total++;
                len += oam_event_build_evt(
                        event_list[i], 
                        &pdu[len],
                        ca_current_time()/10,
                        window, 
                        evt->threshold, 
                        evt->cur_value, 
                        evt->error_running_total,
                        evt->event_running_total);
                send_flag = 1;
            }
        
           if(event_list[i] == OAM_EVENT_TLV_ERR_FRAME
                    || evt->cur_window >= evt->window){
                evt->cur_window = 0;
                evt->cur_value = 0;
            }

           evt->count = evt->count_save;

        }

    }

    /*judge need send*/
    if (send_flag
        && oam_intf->state_disc == OAM_SM_DISC_STATE_SEND_ANY) {
        oam_send(oam_intf->llid, OAM_PON_PORT_ID, pdu, len);
    }
    else {
        --oam_intf->seq_num_next_tx;
    }

    oam_buf_int_status_get_adapt(&bm_status);

    if (bm_status)
        oam_intf->local_critical_event = TRUE;
    else
        oam_intf->local_critical_event = FALSE;
}


oam_uint8 oam_link_event_window_set(
    oam_uint8   type,
    oam_uint32  window)
{
    switch (type) {
        case OAM_EVENT_TLV_ERR_SYM_PERIOD:
            if (window < OAM_LINK_ERROR_SYMBOL_PERIOD_WIN_MIN ||
                window > OAM_LINK_ERROR_SYMBOL_PERIOD_WIN_MAX)
                return OAM_E_PARAM;

            oam_link_event_table[OAM_EVENT_TLV_ERR_SYM_PERIOD].window =
                window / 1000 * OAM_LINK_ERROR_SYMBOL_PER_SECOND;
            break;

        case OAM_EVENT_TLV_ERR_FRAME:
            if (window < OAM_LINK_ERROR_FRAME_WIN_MIN ||
                window > OAM_LINK_ERROR_FRAME_WIN_MAX)
                return OAM_E_PARAM;

            oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME].window = window;
            break;

        case OAM_EVENT_TLV_ERR_FRAME_PERIOD:
            if (window < OAM_LINK_ERROR_PERIOD_WIN_MIN ||
                window > OAM_LINK_ERROR_PERIOD_WIN_MAX)
                return OAM_E_PARAM;

            oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_PERIOD].window =
                (oam_uint64)window * (oam_uint64)OAM_LINK_ERROR_PERIOD_PER_SECOND / 1000;
            break;

        case OAM_EVENT_TLV_ERR_FRAME_SUMM:
            if (window < OAM_LINK_ERROR_FRAME_SECOND_SUMMERY_WIN_MIN ||
                window > OAM_LINK_ERROR_FRAMESECOND_SUMMERY_WIN_MAX)
                return OAM_E_PARAM;

            oam_link_event_table[OAM_EVENT_TLV_ERR_FRAME_SUMM].window = window;
            break;

        default:
            return OAM_E_PARAM;
    }

    return OAM_E_OK;
}

oam_status oam_link_event_threshold_set(
    oam_uint8   type,
    oam_uint64  threshold)
{
    if (type <= OAM_EVENT_TLV_END || type >= OAM_EVENT_TLV_RERV)
        return OAM_E_PARAM;

    oam_link_event_table[type].threshold = threshold;

    return OAM_E_OK;
}

oam_status oam_link_event_para_get(
    oam_uint8   type,
    oam_uint32  *window,
    oam_uint64  *threshold)
{
    OAM_ASSERT_RET(window != NULL
                   && threshold != NULL, OAM_E_PARAM);

    if (type <= OAM_EVENT_TLV_END || type >= OAM_EVENT_TLV_RERV)
        return OAM_E_PARAM;

    *window = oam_link_event_table[type].window;
    *threshold = oam_link_event_table[type].threshold;

    return OAM_E_OK;
}

void oam_dump_link_event_table(oam_print func, void *fd)
{
    oam_uint8 i;

    for (i = OAM_EVENT_TLV_ERR_SYM_PERIOD; i < OAM_EVENT_TLV_RERV; i++) {
        OAM_PRINTF("\r\n================type: %d===================\r\n", i);
        OAM_PRINTF("enable              : %d  \r\n", oam_link_event_table[i].enable);
        OAM_PRINTF("window              : %llu\r\n", oam_link_event_table[i].window);
        OAM_PRINTF("count               : %d  \r\n", oam_link_event_table[i].count);
        OAM_PRINTF("cur_window          : %llu\r\n", oam_link_event_table[i].cur_window);
        OAM_PRINTF("threshold           : %llu\r\n", oam_link_event_table[i].threshold);
        OAM_PRINTF("cur_value           : %llu\r\n", oam_link_event_table[i].cur_value);
        OAM_PRINTF("error_running_total : %llu\r\n",
                   oam_link_event_table[i].error_running_total);
        OAM_PRINTF("event_running_total : %d  \r\n",
                   oam_link_event_table[i].event_running_total);
        OAM_PRINTF("\r\n");
    }
}

