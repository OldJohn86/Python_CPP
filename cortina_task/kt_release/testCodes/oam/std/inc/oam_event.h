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

#ifndef __OAM_EVENT_H__
#define __OAM_EVENT_H__

#include "oam_types.h"


#define OAM_LINK_EVENT_POLL_ACCURACY   1000 /* 1000ms */

#define OAM_LINK_ERROR_SYMBOL_PER_SECOND    125000000      /* 1G/s */
#define OAM_LINK_ERROR_SYMBOL_UNIT          1000      /* 1s */
#define OAM_LINK_ERROR_SYMBOL_PERIOD_WIN_MIN    1000      /* 1s */
#define OAM_LINK_ERROR_SYMBOL_PERIOD_WIN_MAX    1000*60   /* 60s */

#define OAM_LINK_ERROR_FRAME_UNIT       100       /* 100ms */
#define OAM_LINK_ERROR_FRAME_WIN_MIN    1000      /* 1s, unit 100ms */
#define OAM_LINK_ERROR_FRAME_WIN_MAX    1000*60   /* 1min */

#define OAM_LINK_ERROR_PERIOD_PER_SECOND    (OAM_LINK_ERROR_SYMBOL_PER_SECOND/84)    /* 64bytes frame/1s */
#define OAM_LINK_ERROR_PERIOD_UNIT       100            /* 100ms */
#define OAM_LINK_ERROR_PERIOD_WIN_MIN    100            /* 100ms */
#define OAM_LINK_ERROR_PERIOD_WIN_MAX    1000*60        /* 1min */

#define OAM_LINK_ERROR_FRAME_SECOND_SUMMERY_UNIT       100      /* 100ms */
#define OAM_LINK_ERROR_FRAME_SECOND_SUMMERY_WIN_MIN    10000      /* 10s, unit 100ms */
#define OAM_LINK_ERROR_FRAMESECOND_SUMMERY_WIN_MAX    10000*90   /* 900s */

typedef struct {
    oam_uint8       enable;
    oam_uint64      window;
    oam_uint32      count;
    oam_uint32      count_save;
    oam_uint64      cur_window;
    oam_uint64      threshold;
    oam_uint64      cur_value;
    oam_uint64      error_running_total;
    oam_uint32      event_running_total;
} oam_link_event_t;

typedef struct {
    oam_uint64      in_frames;
    oam_uint64      in_errors;
    oam_uint64      in_err_symb;
} oam_link_event_counter_t;

void oam_link_event_reset();
void oam_event_init();
oam_status oam_link_event_threshold_set(oam_uint8 type, oam_uint64 threshold);
oam_status oam_link_event_para_get(
    oam_uint8 type,
    oam_uint32 *window,
    oam_uint64 *threshold);

void oam_event_process_polling(void *timer);

oam_uint8 oam_link_event_window_set(oam_uint8 type, oam_uint32 window);
void oam_dump_link_event_table(oam_print func, void *fd);
#endif
