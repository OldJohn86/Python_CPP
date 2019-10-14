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

DEFINITIONS:  "DEVICE" means the Cortina Systems?Daytona SDK product.
"You" or "CUSTOMER" means the entity or individual that uses the SOFTWARE.
"SOFTWARE" means the Cortina Systems?SDK software.

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
#include <stdlib.h>
#include <string.h>

#include "osal_common.h"
#include "oam_cmn.h"
#include "oam_std_pdu.h"
#include "oam_port_stats.h"
#include "oam_adapt.h"
#include "oam_main.h"
#include "scfg.h"
#include "port.h"
#include "eth_port.h"

/* 
   OAM need to support 64 bits counters. 
   But current Cortina API, only support 32bits for PON port and 1G UNI port, support 64bits for 10G UNI port.
   So we add oam port stats function to keep all the stats.
   
   Also there's an issue that tx counters still increasing when uni link down.
   workaround for UNI stats error: (should subtract the counters when link down)
        when link down, update stats, and stop polling
        when link up, clear stats, and start polling
   
   PON stats used read-keep. 
   Because PON data LED(500ms), error symbol event(1000ms), standard PON MIB function also need to read counters.
   They will get counters from driver directly.

   UNI stats used read-clear.
   Because the workaround need to subtract the counters when link down, use read-clear is easier.

   guarddog average rate(30s, use the same timer with oam port stats), get UNI counters from OAM module.
   
   */

#define OAM_STATS_PORT_NUM_MAX     OAM_UNI_PORT_NUM_MAX+1

typedef struct{
    oam_pon_stats64_t pon_stats;
    oam_pon_stats_t pon_stats_last;
    oam_uni_stats64_t *uni_stats;
    oam_queue_stats64_t queue_stats[OAM_STATS_PORT_NUM_MAX][OAM_PORT_QUEUE_NUM_MAX];
    /* ca_queue_stats_get don't support read_keepï¼?It is read clear by default */
    //oam_queue_stats_t queue_stats_last[OAM_STATS_PORT_NUM_MAX][OAM_PORT_QUEUE_NUM_MAX];
    ca_uint pon_stats_mutex;
    ca_uint uni_stats_mutex;
    ca_uint queue_stats_mutex;
    ca_uint polling_enable_mutex;
    oam_boolean polling_enable[OAM_STATS_PORT_NUM_MAX];
    oam_uint64 stats_update_tick[OAM_STATS_PORT_NUM_MAX];
    oam_ext_mode_t ext_mode;
    oam_uint32 customer_bit;
}oam_port_stats_mgmt_t;

static oam_port_stats_mgmt_t g_oam_stats_mgmt;


/* used for stats read-keep */
/* use the current value and last value to make sure the value not overlap */
#define OAM_COUNT_STATS8(cur, last) \
        cur? ((cur) >= (last)? ( (cur) - (last) ) : (0xff - (last) + (cur) + 1)) : 0
#define OAM_COUNT_STATS16(cur, last) \
            cur? ((cur) >= (last)? ( (cur) - (last) ) : (0xffff - (last) + (cur) + 1)) : 0

#define OAM_COUNT_STATS32(cur, last) \
        cur? ((cur) >= (last)? ( (cur) - (last) ) : (0xffffffff - (last) + (cur) + 1)) : 0
#define OAM_COUNT_STATS64(cur, last) \
        cur? ((cur) >= (last)? ( (cur) - (last) ) : (0xffffffffffffLL - (last) + (cur) + 1)) : 0

/* used for special handle for rx mc counter */
static oam_uint32 OAM_COUNT_STATS32_offset(oam_uint32 cur, oam_uint32 last)
{
    if(cur >= last)
    {
        return cur - last;
    }
    else
    {
        if((last - cur) < 10000)
        {
            /*consider it's not a normal around*/
            return 0;
        }
        else
        {
            return (0xffffffff - last + cur + 1);
        }
    }
}


/* get counters from ASIC: update every 30s */
static oam_status oam_pon_stats_update()
{
    oam_status ret = OAM_E_OK;
    oam_pon_stats_t pon_stats;
    oam_boolean enable;

    oam_stats_polling_enable_get(0, &enable);

    if(!enable){ /* PON port */ 
        return OAM_E_OK;
    }
#if 0
    if(ca_current_time() - g_oam_stats_mgmt.stats_update_tick[0] <= 20){ /* < 200s, no update */
        ca_printf("port 0 stats update last time %lld, current time %lld\r\n", g_oam_stats_mgmt.stats_update_tick[0], ca_current_time());
        return OAM_E_OK;
    }
    g_oam_stats_mgmt.stats_update_tick[0] = ca_current_time();
#endif
    memset(&pon_stats, 0, sizeof(oam_pon_stats_t));
    ret = oam_pon_stats_get_adapt(FALSE, &pon_stats);
    if(ret != OAM_E_OK){
        OAM_LOG_DEBUG("oam_pon_stats_get_adapt return failed, ret = %d \r\n",ret);
        return ret;
    }

    ca_mutex_lock(g_oam_stats_mgmt.pon_stats_mutex);

    /* special handle for RX MPCP register, RX MPCP gate, RX oam, TX register request, TX register ACK, TX MPCP report, TX
    oam, they're 8bits or 16bits counters */
    g_oam_stats_mgmt.pon_stats.pon_byte_cnt            +=  OAM_COUNT_STATS64( pon_stats.pon_byte_cnt            ,g_oam_stats_mgmt.pon_stats_last.pon_byte_cnt          );
    g_oam_stats_mgmt.pon_stats.pon_tx_byte_cnt         +=  OAM_COUNT_STATS64( pon_stats.pon_tx_byte_cnt         ,g_oam_stats_mgmt.pon_stats_last.pon_tx_byte_cnt        );
    g_oam_stats_mgmt.pon_stats.pon_frame_cnt           +=  OAM_COUNT_STATS32( pon_stats.pon_frame_cnt           ,g_oam_stats_mgmt.pon_stats_last.pon_frame_cnt          );
    g_oam_stats_mgmt.pon_stats.pon_txframe_cnt         +=  OAM_COUNT_STATS32( pon_stats.pon_txframe_cnt         ,g_oam_stats_mgmt.pon_stats_last.pon_txframe_cnt        );
    g_oam_stats_mgmt.pon_stats.pon_crcerr_cnt          +=  OAM_COUNT_STATS32( pon_stats.pon_crcerr_cnt          ,g_oam_stats_mgmt.pon_stats_last.pon_crcerr_cnt         );
    g_oam_stats_mgmt.pon_stats.pon_ucframe_cnt         +=  OAM_COUNT_STATS32( pon_stats.pon_ucframe_cnt         ,g_oam_stats_mgmt.pon_stats_last.pon_ucframe_cnt        );
    g_oam_stats_mgmt.pon_stats.pon_mcframe_cnt         +=  OAM_COUNT_STATS32( pon_stats.pon_mcframe_cnt         ,g_oam_stats_mgmt.pon_stats_last.pon_mcframe_cnt        );
    g_oam_stats_mgmt.pon_stats.pon_bcframe_cnt         +=  OAM_COUNT_STATS32( pon_stats.pon_bcframe_cnt         ,g_oam_stats_mgmt.pon_stats_last.pon_bcframe_cnt        );
    g_oam_stats_mgmt.pon_stats.pon_txucframe_cnt       +=  OAM_COUNT_STATS32( pon_stats.pon_txucframe_cnt       ,g_oam_stats_mgmt.pon_stats_last.pon_txucframe_cnt      );
    g_oam_stats_mgmt.pon_stats.pon_txmcframe_cnt       +=  OAM_COUNT_STATS32( pon_stats.pon_txmcframe_cnt       ,g_oam_stats_mgmt.pon_stats_last.pon_txmcframe_cnt      );
    g_oam_stats_mgmt.pon_stats.pon_txbcframe_cnt       +=  OAM_COUNT_STATS32( pon_stats.pon_txbcframe_cnt       ,g_oam_stats_mgmt.pon_stats_last.pon_txbcframe_cnt      );
    g_oam_stats_mgmt.pon_stats.pon_ctrlframe_cnt       +=  OAM_COUNT_STATS32( pon_stats.pon_ctrlframe_cnt       ,g_oam_stats_mgmt.pon_stats_last.pon_ctrlframe_cnt      );
    g_oam_stats_mgmt.pon_stats.pon_txctrlframe_cnt     +=  OAM_COUNT_STATS32( pon_stats.pon_txctrlframe_cnt     ,g_oam_stats_mgmt.pon_stats_last.pon_txctrlframe_cnt    );
    g_oam_stats_mgmt.pon_stats.pon_pauseframe_cnt      +=  OAM_COUNT_STATS32( pon_stats.pon_pauseframe_cnt      ,g_oam_stats_mgmt.pon_stats_last.pon_pauseframe_cnt     );
    g_oam_stats_mgmt.pon_stats.pon_unknownop_cnt       +=  OAM_COUNT_STATS32( pon_stats.pon_unknownop_cnt       ,g_oam_stats_mgmt.pon_stats_last.pon_unknownop_cnt      );
    g_oam_stats_mgmt.pon_stats.pon_runt_cnt            +=  OAM_COUNT_STATS32( pon_stats.pon_runt_cnt            ,g_oam_stats_mgmt.pon_stats_last.pon_runt_cnt           );
    g_oam_stats_mgmt.pon_stats.pon_oversize_cnt        +=  OAM_COUNT_STATS32( pon_stats.pon_oversize_cnt        ,g_oam_stats_mgmt.pon_stats_last.pon_oversize_cnt       );
    g_oam_stats_mgmt.pon_stats.pon_rmon64_cnt          +=  OAM_COUNT_STATS32( pon_stats.pon_rmon64_cnt          ,g_oam_stats_mgmt.pon_stats_last.pon_rmon64_cnt         );
    g_oam_stats_mgmt.pon_stats.pon_rmon65_127_cnt      +=  OAM_COUNT_STATS32( pon_stats.pon_rmon65_127_cnt      ,g_oam_stats_mgmt.pon_stats_last.pon_rmon65_127_cnt     );
    g_oam_stats_mgmt.pon_stats.pon_rmon128_255_cnt     +=  OAM_COUNT_STATS32( pon_stats.pon_rmon128_255_cnt     ,g_oam_stats_mgmt.pon_stats_last.pon_rmon128_255_cnt    );
    g_oam_stats_mgmt.pon_stats.pon_rmon256_511_cnt     +=  OAM_COUNT_STATS32( pon_stats.pon_rmon256_511_cnt     ,g_oam_stats_mgmt.pon_stats_last.pon_rmon256_511_cnt    );
    g_oam_stats_mgmt.pon_stats.pon_rmon512_1023_cnt    +=  OAM_COUNT_STATS32( pon_stats.pon_rmon512_1023_cnt    ,g_oam_stats_mgmt.pon_stats_last.pon_rmon512_1023_cnt   );
    g_oam_stats_mgmt.pon_stats.pon_rmon1024_1518_cnt   +=  OAM_COUNT_STATS32( pon_stats.pon_rmon1024_1518_cnt   ,g_oam_stats_mgmt.pon_stats_last.pon_rmon1024_1518_cnt  );
    g_oam_stats_mgmt.pon_stats.pon_txrmon64_cnt        +=  OAM_COUNT_STATS32( pon_stats.pon_txrmon64_cnt        ,g_oam_stats_mgmt.pon_stats_last.pon_txrmon64_cnt       );
    g_oam_stats_mgmt.pon_stats.pon_txrmon65_127_cnt    +=  OAM_COUNT_STATS32( pon_stats.pon_txrmon65_127_cnt    ,g_oam_stats_mgmt.pon_stats_last.pon_txrmon65_127_cnt   );
    g_oam_stats_mgmt.pon_stats.pon_txrmon128_255_cnt   +=  OAM_COUNT_STATS32( pon_stats.pon_txrmon128_255_cnt   ,g_oam_stats_mgmt.pon_stats_last.pon_txrmon128_255_cnt  );
    g_oam_stats_mgmt.pon_stats.pon_txrmon256_511_cnt   +=  OAM_COUNT_STATS32( pon_stats.pon_txrmon256_511_cnt   ,g_oam_stats_mgmt.pon_stats_last.pon_txrmon256_511_cnt  );
    g_oam_stats_mgmt.pon_stats.pon_txrmon512_1023_cnt  +=  OAM_COUNT_STATS32( pon_stats.pon_txrmon512_1023_cnt  ,g_oam_stats_mgmt.pon_stats_last.pon_txrmon512_1023_cnt );
    g_oam_stats_mgmt.pon_stats.pon_txrmon1024_1518_cnt +=  OAM_COUNT_STATS32( pon_stats.pon_txrmon1024_1518_cnt ,g_oam_stats_mgmt.pon_stats_last.pon_txrmon1024_1518_cnt);
    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_rev_cnt     +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_uc_rev_cnt     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_rev_cnt    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_reg_ack     +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_uc_reg_ack     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_reg_ack    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_reg_nack    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_uc_reg_nack    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_reg_nack   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_reg_dereg   +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_uc_reg_dereg   ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_reg_dereg  );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_reg_rereg   +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_uc_reg_rereg   ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_reg_rereg  );

    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_reg_reg_rev +=  OAM_COUNT_STATS8( pon_stats.pon_mpcp_uc_reg_reg_rev ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_reg_reg_rev);
    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_gat_rev     +=  OAM_COUNT_STATS16( pon_stats.pon_mpcp_uc_gat_rev     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_gat_rev    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_gat_norm    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_uc_gat_norm    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_gat_norm   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_gat_frpt    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_uc_gat_frpt    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_gat_frpt   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_gat_udis    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_uc_gat_udis    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_gat_udis   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_uc_gat_bdis    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_uc_gat_bdis    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_uc_gat_bdis   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_rev_cnt     +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_rev_cnt     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_rev_cnt    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_reg_ack     +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_reg_ack     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_reg_ack    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_reg_nack    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_reg_nack    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_reg_nack   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_reg_dereg   +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_reg_dereg   ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_reg_dereg  );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_reg_rereg   +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_reg_rereg   ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_reg_rereg  );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_reg_reg_rev +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_reg_reg_rev ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_reg_reg_rev);
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_gat_rev     +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_gat_rev     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_gat_rev    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_gat_norm    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_gat_norm    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_gat_norm   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_gat_frpt    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_gat_frpt    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_gat_frpt   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_gat_udis    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_gat_udis    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_gat_udis   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_mc_gat_bdis    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_mc_gat_bdis    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_mc_gat_bdis   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_rev_cnt     +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_rev_cnt     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_rev_cnt    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_reg_ack     +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_reg_ack     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_reg_ack    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_reg_nack    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_reg_nack    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_reg_nack   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_reg_dereg   +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_reg_dereg   ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_reg_dereg  );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_reg_rereg   +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_reg_rereg   ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_reg_rereg  );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_reg_reg_rev +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_reg_reg_rev ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_reg_reg_rev);
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_gat_rev     +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_gat_rev     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_gat_rev    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_gat_norm    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_gat_norm    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_gat_norm   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_gat_frpt    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_gat_frpt    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_gat_frpt   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_gat_udis    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_gat_udis    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_gat_udis   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_bc_gat_bdis    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_bc_gat_bdis    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_bc_gat_bdis   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_rx_oam         +=  OAM_COUNT_STATS16( pon_stats.pon_mpcp_rx_oam         ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_rx_oam        );

    g_oam_stats_mgmt.pon_stats.pon_mpcp_tx_req_dereg   +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_tx_req_dereg   ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_tx_req_dereg  );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_tx_req_reg     +=  OAM_COUNT_STATS8( pon_stats.pon_mpcp_tx_req_reg     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_tx_req_reg    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_tx_ack_ack     +=  OAM_COUNT_STATS8( pon_stats.pon_mpcp_tx_ack_ack     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_tx_ack_ack    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_tx_ack_nack    +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_tx_ack_nack    ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_tx_ack_nack   );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_tx_rpt         +=  OAM_COUNT_STATS16( pon_stats.pon_mpcp_tx_rpt         ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_tx_rpt        );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_tx_oam         +=  OAM_COUNT_STATS16( pon_stats.pon_mpcp_tx_oam         ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_tx_oam        );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_da_mc_nslf     +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_da_mc_nslf     ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_da_mc_nslf    );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_da_mc_slf      +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_da_mc_slf      ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_da_mc_slf     );
    g_oam_stats_mgmt.pon_stats.pon_mpcp_da_uc          +=  OAM_COUNT_STATS32( pon_stats.pon_mpcp_da_uc          ,g_oam_stats_mgmt.pon_stats_last.pon_mpcp_da_uc         );

    g_oam_stats_mgmt.pon_stats.pon_txoversizecnt       +=  OAM_COUNT_STATS32( pon_stats.pon_txoversizecnt       ,g_oam_stats_mgmt.pon_stats_last.pon_txoversizecnt      );
    g_oam_stats_mgmt.pon_stats.pon_rx1519_maxcnt       +=  OAM_COUNT_STATS32( pon_stats.pon_rx1519_maxcnt       ,g_oam_stats_mgmt.pon_stats_last.pon_rx1519_maxcnt      );
    g_oam_stats_mgmt.pon_stats.pon_tx1519_maxcnt       +=  OAM_COUNT_STATS32( pon_stats.pon_tx1519_maxcnt       ,g_oam_stats_mgmt.pon_stats_last.pon_tx1519_maxcnt      );
    g_oam_stats_mgmt.pon_stats.pon_txpausecnt          +=  OAM_COUNT_STATS32( pon_stats.pon_txpausecnt         ,g_oam_stats_mgmt.pon_stats_last.pon_txpausecnt         );
    g_oam_stats_mgmt.pon_stats.pon_undersizecnt        +=  OAM_COUNT_STATS32( pon_stats.pon_undersizecnt        ,g_oam_stats_mgmt.pon_stats_last.pon_undersizecnt       );
    g_oam_stats_mgmt.pon_stats.pon_jabbercnt           +=  OAM_COUNT_STATS32( pon_stats.pon_jabbercnt           ,g_oam_stats_mgmt.pon_stats_last.pon_jabbercnt          );
    g_oam_stats_mgmt.pon_stats.pon_extensioncnt        +=  OAM_COUNT_STATS32( pon_stats.pon_extensioncnt        ,g_oam_stats_mgmt.pon_stats_last.pon_extensioncnt       );
    g_oam_stats_mgmt.pon_stats.pon_txextensioncnt      +=  OAM_COUNT_STATS32( pon_stats.pon_txextensioncnt      ,g_oam_stats_mgmt.pon_stats_last.pon_txextensioncnt     );
    g_oam_stats_mgmt.pon_stats.pon_err_symbol          +=  OAM_COUNT_STATS32( pon_stats.pon_err_symbol          ,g_oam_stats_mgmt.pon_stats_last.pon_err_symbol         );
    g_oam_stats_mgmt.pon_stats.pon_outofsync           +=  OAM_COUNT_STATS32( pon_stats.pon_outofsync           ,g_oam_stats_mgmt.pon_stats_last.pon_outofsync          );
    g_oam_stats_mgmt.pon_stats.pon_sld_err             +=  OAM_COUNT_STATS32( pon_stats.pon_sld_err             ,g_oam_stats_mgmt.pon_stats_last.pon_sld_err            );
    g_oam_stats_mgmt.pon_stats.pon_crc8_err            +=  OAM_COUNT_STATS32( pon_stats.pon_crc8_err            ,g_oam_stats_mgmt.pon_stats_last.pon_crc8_err           );
    g_oam_stats_mgmt.pon_stats.pon_mac_drop            +=  OAM_COUNT_STATS32( pon_stats.pon_mac_drop            ,g_oam_stats_mgmt.pon_stats_last.pon_mac_drop           );
    g_oam_stats_mgmt.pon_stats.pon_ipg_drop            +=  OAM_COUNT_STATS32( pon_stats.pon_ipg_drop            ,g_oam_stats_mgmt.pon_stats_last.pon_ipg_drop           );
    g_oam_stats_mgmt.pon_stats.pon_drain               +=  OAM_COUNT_STATS32( pon_stats.pon_drain               ,g_oam_stats_mgmt.pon_stats_last.pon_drain              );
    g_oam_stats_mgmt.pon_stats.pon_tx_crc_err          +=  OAM_COUNT_STATS32( pon_stats.pon_tx_crc_err          ,g_oam_stats_mgmt.pon_stats_last.pon_tx_crc_err         );
    //g_oam_stats_mgmt.pon_stats.pon_rx_fcs_err          +=  OAM_COUNT_STATS32( pon_stats.pon_rx_fcs_err          ,g_oam_stats_mgmt.pon_stats_last.pon_rx_fcs_err         );
    /* just for KT BMT test, pon_rx_fcs_err s hooked to FEC rx_err_pkt. and HW FEC stats is always read_clear, can not support read_keep */
    g_oam_stats_mgmt.pon_stats.pon_rx_fcs_err          +=  pon_stats.pon_rx_fcs_err;    
   
    memcpy(&g_oam_stats_mgmt.pon_stats_last,&pon_stats,sizeof(pon_stats));
    ca_mutex_unlock(g_oam_stats_mgmt.pon_stats_mutex);

    return ret;
}

static oam_status oam_uni_stats_update(oam_port_id_t port)
{
    oam_status ret = OAM_E_OK;
    oam_uint32 i=  port-1;
    oam_uni_stats64_t uni_stats;
    oam_boolean enable;

    OAM_UNI_PORT_ID_CHECK(port);

    oam_stats_polling_enable_get(port, &enable);

    if(!enable){
        return OAM_E_OK;
    }
#if 0
    if(ca_current_time() - g_oam_stats_mgmt.stats_update_tick[port] <= 20){
        ca_printf("port %d stats update last time %lld, current time %lld\r\n", port, g_oam_stats_mgmt.stats_update_tick[port], ca_current_time());
        return OAM_E_OK;
    }
    g_oam_stats_mgmt.stats_update_tick[port] = ca_current_time();
#endif    
    memset(&uni_stats,0x00,sizeof(oam_uni_stats64_t));
    ret = oam_uni_stats_get_adapt(port, TRUE, &uni_stats);
    if(ret != OAM_E_OK){
        OAM_LOG_DEBUG("oam_uni_stats_get_adapt return failed, port = %d ret = %d \r\n",port,ret);
        return ret;
    }

    ca_mutex_lock(g_oam_stats_mgmt.uni_stats_mutex);
    
    g_oam_stats_mgmt.uni_stats[i].rxucfrm_cnt               += uni_stats.rxucfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxmcfrm_cnt               += uni_stats.rxmcfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxbcfrm_cnt               += uni_stats.rxbcfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxpausefrm_cnt            += uni_stats.rxpausefrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxcrcerrfrm_cnt           += uni_stats.rxcrcerrfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxundersizefrm_cnt        += uni_stats.rxundersizefrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxruntfrm_cnt             += uni_stats.rxruntfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxoversizefrm_cnt         += uni_stats.rxoversizefrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxjabberfrm_cnt           += uni_stats.rxjabberfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxstatsfrm64_cnt          += uni_stats.rxstatsfrm64_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxstatsfrm65_127_cnt      += uni_stats.rxstatsfrm65_127_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxstatsfrm128_255_cnt     += uni_stats.rxstatsfrm128_255_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxstatsfrm256_511_cnt     += uni_stats.rxstatsfrm256_511_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxstatsfrm512_1023_cnt    += uni_stats.rxstatsfrm512_1023_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxstatsfrm1024_1518_cnt   += uni_stats.rxstatsfrm1024_1518_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxstatsfrm1519_Max_cnt    += uni_stats.rxstatsfrm1519_Max_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxbyte_cnt                += uni_stats.rxbyte_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxfrm_cnt                 += uni_stats.rxfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxfrmgreen_cnt            += uni_stats.rxfrmgreen_cnt;
    g_oam_stats_mgmt.uni_stats[i].rxerror_cnt               += uni_stats.rxerror_cnt;

    g_oam_stats_mgmt.uni_stats[i].txucfrm_cnt               += uni_stats.txucfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].txmcfrm_cnt               += uni_stats.txmcfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].txbcfrm_cnt               += uni_stats.txbcfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].txoamfrm_cnt              += uni_stats.txoamfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].txpausefrm_cnt            += uni_stats.txpausefrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].txcrcerrfrm_cnt           += uni_stats.txcrcerrfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].txoversizefrm_cnt         += uni_stats.txoversizefrm_cnt;

    g_oam_stats_mgmt.uni_stats[i].txstatsfrm64_cnt          += uni_stats.txstatsfrm64_cnt;
    g_oam_stats_mgmt.uni_stats[i].txstatsfrm65_127_cnt      += uni_stats.txstatsfrm65_127_cnt;
    g_oam_stats_mgmt.uni_stats[i].txstatsfrm128_255_cnt     += uni_stats.txstatsfrm128_255_cnt;
    g_oam_stats_mgmt.uni_stats[i].txstatsfrm256_511_cnt     += uni_stats.txstatsfrm256_511_cnt;
    g_oam_stats_mgmt.uni_stats[i].txstatsfrm512_1023_cnt    += uni_stats.txstatsfrm512_1023_cnt;
    g_oam_stats_mgmt.uni_stats[i].txstatsfrm1024_1518_cnt   += uni_stats.txstatsfrm1024_1518_cnt;
    g_oam_stats_mgmt.uni_stats[i].txstatsfrm1519_Max_cnt    += uni_stats.txstatsfrm1519_Max_cnt;
    g_oam_stats_mgmt.uni_stats[i].txbyte_cnt                += uni_stats.txbyte_cnt;
    g_oam_stats_mgmt.uni_stats[i].txfrm_cnt                 += uni_stats.txfrm_cnt;
    g_oam_stats_mgmt.uni_stats[i].txfrmgreen_cnt            += uni_stats.txfrmgreen_cnt;
    g_oam_stats_mgmt.uni_stats[i].txerror_cnt               += uni_stats.txerror_cnt;
    
    g_oam_stats_mgmt.uni_stats[i].alignment_errors          += uni_stats.alignment_errors;
    g_oam_stats_mgmt.uni_stats[i].single_collision          += uni_stats.single_collision;
    g_oam_stats_mgmt.uni_stats[i].multiple_collision        += uni_stats.multiple_collision;
    g_oam_stats_mgmt.uni_stats[i].deferred_transmissions    += uni_stats.deferred_transmissions;
    g_oam_stats_mgmt.uni_stats[i].foam_errors               += uni_stats.foam_errors;
    g_oam_stats_mgmt.uni_stats[i].late_collisions           += uni_stats.late_collisions;
    g_oam_stats_mgmt.uni_stats[i].excessive_collisions      += uni_stats.excessive_collisions;
    g_oam_stats_mgmt.uni_stats[i].internal_mac_rx_errors    += uni_stats.internal_mac_rx_errors;
    g_oam_stats_mgmt.uni_stats[i].internal_mac_tx_errors    += uni_stats.internal_mac_tx_errors;
    g_oam_stats_mgmt.uni_stats[i].symbol_errors             += uni_stats.symbol_errors;

    ca_mutex_unlock(g_oam_stats_mgmt.uni_stats_mutex);

    return ret;
}

static oam_status oam_queue_stats_update(oam_port_id_t port, oam_uint8 queue)
{
    oam_status ret = OAM_E_OK;
    oam_queue_stats_t queue_stats;
    oam_boolean enable;

    OAM_PORT_ID_CHECK(port);
    OAM_QUEUE_ID_CHECK(queue);

    oam_stats_polling_enable_get(port, &enable);

    if(!enable){
        return OAM_E_OK;
    }
  
    memset(&queue_stats,0x00,sizeof(oam_queue_stats_t));
    ret = oam_queue_stats_get_adapt(port, queue, FALSE, &queue_stats);
    if(ret != OAM_E_OK){
        OAM_LOG_DEBUG("oam_queue_stats_get_adapt return failed, port = %d queue = %d ret = %d \r\n",port,queue,ret);
        return ret;
    }

    ca_mutex_lock(g_oam_stats_mgmt.queue_stats_mutex);
    
    g_oam_stats_mgmt.queue_stats[port][queue].fwd_frames += queue_stats.fwd_frames;
    g_oam_stats_mgmt.queue_stats[port][queue].drop_frames+= queue_stats.drop_frames;
    g_oam_stats_mgmt.queue_stats[port][queue].fwd_bytes  += queue_stats.fwd_bytes;
    g_oam_stats_mgmt.queue_stats[port][queue].drop_bytes += queue_stats.drop_bytes;

    ca_mutex_unlock(g_oam_stats_mgmt.queue_stats_mutex);

    return ret;
}

oam_status oam_stats_polling_enable(oam_boolean enable)
{
    oam_port_id_t port;
    ca_port_id_t ca_port;
    ca_eth_port_link_status_t link_status;

    ca_mutex_lock(g_oam_stats_mgmt.polling_enable_mutex);

    /* fix bug51765 
    When ONU registered, port stats polling enabled.
    When link down, port stats polling disabled.
    If UNI port is link down, then reboot ONU, after ONU registered, there's no link down event.*/
    for(port = OAM_PON_PORT_ID; port < (OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM); port++)
    {
        if(port == OAM_PON_PORT_ID)
        {
            link_status = CA_PORT_LINK_UP;
        }
        else
        {
            ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);
            ca_eth_port_link_status_get(0, ca_port, &link_status);         
        }
        
        if((link_status == CA_PORT_LINK_UP) && enable)
        {
            g_oam_stats_mgmt.polling_enable[port] = TRUE;
        }
        else
        {
            g_oam_stats_mgmt.polling_enable[port] = FALSE;
        }
    }
    ca_mutex_unlock(g_oam_stats_mgmt.polling_enable_mutex);

    return OAM_E_OK;
}


oam_status oam_stats_polling_enable_set(oam_port_id_t port, oam_boolean enable)
{ 
    OAM_PORT_ID_CHECK(port);
    
    OAM_LOG_DEBUG("\r\noam port %d statistics polling %s\r\n", port, (enable)?"enabled":"disabled");

    ca_mutex_lock(g_oam_stats_mgmt.polling_enable_mutex);
    g_oam_stats_mgmt.polling_enable[port] = enable;
    ca_mutex_unlock(g_oam_stats_mgmt.polling_enable_mutex);

    return OAM_E_OK;
}

oam_status oam_stats_polling_enable_get(oam_port_id_t port, oam_boolean *enable)
{
    OAM_PORT_ID_CHECK(port);
    
    *enable=  g_oam_stats_mgmt.polling_enable[port];  

    OAM_LOG_DEBUG("\r\noam port %d statistics polling %s\r\n", port, (*enable)?"enabled":"disabled");
        
    return OAM_E_OK;
}

void oam_port_stats_polling()
{
    oam_port_id_t port;
    oam_uint8 queue;

    oam_pon_stats_update();

    for(port = OAM_UNI_PORT_ID1; port < (OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM); port++)
    {
        oam_uni_stats_update(port);
    }

    for(port = OAM_PON_PORT_ID; port < (OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM); port++)
    {
        for(queue = 0; queue < OAM_PORT_QUEUE_NUM_MAX; queue++)
        {
            oam_queue_stats_update(port, queue);
        }
    }

#ifdef HAVE_KT_OAM
    if (g_oam_stats_mgmt.ext_mode == OAM_EXT_MODE_KT)
    {
        extern void oam_guarddog_port_traffic_polling();
        oam_guarddog_port_traffic_polling();
    }
#endif
    
}

oam_status oam_pon_stats_reset()
{
    OAM_LOG_DEBUG("pon port stats clear\r\n");
    
    /*clr from ASIC*/
    oam_pon_stats_clear_adapt();

    /*clear from oam*/
    ca_mutex_lock(g_oam_stats_mgmt.pon_stats_mutex);
    
    memset(&g_oam_stats_mgmt.pon_stats, 0, sizeof(oam_pon_stats64_t));
    memset(&g_oam_stats_mgmt.pon_stats_last, 0, sizeof(oam_pon_stats_t));
    
    ca_mutex_unlock(g_oam_stats_mgmt.pon_stats_mutex);

    return OAM_E_OK;
}

oam_status oam_uni_stats_reset(oam_port_id_t port)
{
    oam_uint16 i = port-1;

    OAM_UNI_PORT_ID_CHECK(port);

    OAM_LOG_DEBUG("uni port %d stats clear\r\n", port);
    
    oam_uni_stats_clear_adapt(port);
    
	ca_mutex_lock(g_oam_stats_mgmt.uni_stats_mutex);
	
    memset(&g_oam_stats_mgmt.uni_stats[i], 0x00,sizeof(oam_uni_stats64_t));
    
    ca_mutex_unlock(g_oam_stats_mgmt.uni_stats_mutex);

    return OAM_E_OK;
}

oam_status oam_queue_stats_reset(oam_port_id_t port, oam_uint8 queue)
{
    OAM_PORT_ID_CHECK(port);
    OAM_QUEUE_ID_CHECK(queue);

    OAM_LOG_DEBUG("port %d queue %d stats clear\r\n", port);
    
    oam_queue_stats_clear_adapt(port, queue);
    
    ca_mutex_lock(g_oam_stats_mgmt.queue_stats_mutex);
	
    memset(&g_oam_stats_mgmt.queue_stats[port][queue], 0, sizeof(oam_queue_stats64_t));
    
    ca_mutex_unlock(g_oam_stats_mgmt.queue_stats_mutex);

    return OAM_E_OK;
}


oam_status oam_port_stats_reset()
{
    oam_port_id_t port;
    oam_uint8 queue;
    
    oam_pon_stats_reset();

    for(port = OAM_UNI_PORT_ID1; port < (OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM); port++){
        oam_uni_stats_reset(port);
    }
    for(port = OAM_PON_PORT_ID; port < (OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM); port++){
        for(queue = 0; queue < OAM_PORT_QUEUE_NUM_MAX; queue++){
            oam_queue_stats_reset(port, queue);
        }
    }
    return OAM_E_OK;
}

void oam_stats_timer_expire(void *data)
{
    oam_timer_msg_send(OAM_KT_STATS_TIMER_EXPIRED, data);
}

void oam_port_stats_init()
{
    oam_status ret = 0;
    oam_uint32 i = 0;
    ca_device_config_tlv_t scfg_tlv;

    memset(&g_oam_stats_mgmt, 0, sizeof(oam_port_stats_mgmt_t));
    
    if(0 != ca_mutex_init(&g_oam_stats_mgmt.pon_stats_mutex, "pon_stats_mutex", 0)) {
        OAM_ERROR("initialize pon_stats_mutex failed\r\n");
        return;
    }

    if(0 != ca_mutex_init(&g_oam_stats_mgmt.uni_stats_mutex, "uni_stats_mutex", 0)) {
        OAM_ERROR("initialize uni_stats_mutex failed\r\n");
        return;
    }

    if(0 != ca_mutex_init(&g_oam_stats_mgmt.queue_stats_mutex, "queue_stats_mutexs", 0)) {
        OAM_ERROR("initialize queue_stats_mutex failed\r\n");
        return;
    }

    if(0 != ca_mutex_init(&g_oam_stats_mgmt.polling_enable_mutex, "uni_stats_mutex", 0)) {
        OAM_ERROR("initialize polling_enable_mutex failed\r\n");
        return;
    }
    
    g_oam_stats_mgmt.uni_stats = OAM_MALLOC(sizeof(oam_uni_stats64_t)* (OAM_UNI_PORT_NUM));

    oam_port_stats_reset();

    g_oam_stats_mgmt.ext_mode = oam_ext_mode_get_adapt();

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_CUSTOMER_BIT;
    scfg_tlv.len  = sizeof(ca_uint32_t);
    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if(CA_E_OK == ret){
        memcpy(&g_oam_stats_mgmt.customer_bit, scfg_tlv.value, sizeof(ca_uint32_t));
    }

    ca_circle_timer_add(OAM_STATS_POLL_ACCURACY,oam_stats_timer_expire,NULL);

}

oam_status oam_onu_pon_stats_get(oam_pon_stats64_t *pon_stats)
{

    OAM_ASSERT_RET(pon_stats != NULL,OAM_E_PARAM);
    
    oam_pon_stats_update();

    memcpy(pon_stats,&g_oam_stats_mgmt.pon_stats,sizeof(g_oam_stats_mgmt.pon_stats));

    return OAM_E_OK;
}

oam_status oam_onu_std_pon_stats_get(epon_if_stats_t *if_stats)
{
    oam_pon_stats64_t pon_stats;

    OAM_ASSERT_RET(if_stats != NULL,OAM_E_PARAM);

    memset(&pon_stats, 0, sizeof(pon_stats));

    oam_onu_pon_stats_get(&pon_stats);

    if_stats->in_octets = pon_stats.pon_byte_cnt;
    if_stats->in_unicast = pon_stats.pon_ucframe_cnt;
    if_stats->in_multicast = pon_stats.pon_mcframe_cnt;
    if_stats->in_broadcast = pon_stats.pon_bcframe_cnt;
    if_stats->in_discards = 0;
    if_stats->in_errors = pon_stats.pon_crcerr_cnt +  pon_stats.pon_sld_err + pon_stats.pon_crc8_err;

    if_stats->in_unknown_protos = 0;
    if_stats->in_err_symbol = pon_stats.pon_err_symbol;
    if_stats->out_octets = pon_stats.pon_tx_byte_cnt;
    if_stats->out_unicast = pon_stats.pon_txucframe_cnt;
    if_stats->out_multicast = pon_stats.pon_txmcframe_cnt;
    if_stats->out_broadcast = pon_stats.pon_txbcframe_cnt;
    if_stats->out_discards = 0;
    if_stats->out_errors = pon_stats.pon_tx_crc_err;

    return OAM_E_OK;
}

void oam_onu_pon_stats_summary_get(
        oam_uint64 *in_frame,
        oam_uint64 *in_errors,
        oam_uint64 *in_err_symb,
        oam_uint64 *in_octets)
{
    epon_if_stats_t pon_stats;

    OAM_ASSERT(in_frame != NULL
            && in_errors != NULL
            && in_err_symb != NULL
            && in_octets != NULL);

    memset(&pon_stats,0x00,sizeof(pon_stats));

    oam_onu_std_pon_stats_get(&pon_stats);

    *in_frame = pon_stats.in_unicast + pon_stats.in_multicast
        + pon_stats.in_broadcast + pon_stats.in_errors;
    *in_errors = pon_stats.in_errors;
    *in_octets = pon_stats.in_octets;
    *in_err_symb = pon_stats.in_err_symbol;

}

oam_status oam_onu_uni_stats_get(
        oam_port_id_t port_id,
        oam_uni_stats64_t *uni_stats)
{
    oam_uint32 i = port_id-1;

    OAM_ASSERT_RET(uni_stats != NULL,OAM_E_PARAM);

    OAM_UNI_PORT_ID_CHECK(port_id);

    oam_uni_stats_update(port_id);

    memcpy(uni_stats,&g_oam_stats_mgmt.uni_stats[i],sizeof(g_oam_stats_mgmt.uni_stats[i]));
    
#if 0
    /* For below command, OLT will send few OAMPDUs, and each OAMPDU has many branch/leaf,
    each branch/leaf will call the handler, it will cause too long time (about 10s).
    Dasan(config-pon-olt[2/1])# show onu statistics

     So for "oam_onu_uni_stats_get" API, only update the uni stats from ASIC one time during 2s (200ticks),
     we get statistics from SW structures (g_uni_stats[port]) */

    if(ca_current_time() - g_oam_uni_stats_update_tick >= 200){
        //OAM_LOG_DEBUG("start to get statistic from ASIC, start time %lld\r\n", ca_current_time());
        if(port_id != 5)oam_uni_stats_count(port_id);
        //OAM_LOG_DEBUG("end to get statistic from ASIC, end time %lld\r\n", ca_current_time());
        g_oam_uni_stats_update_tick = ca_current_time();
    }

#endif

    
    return OAM_E_OK;

}

oam_status oam_onu_queue_stats_get(
        oam_port_id_t port_id,
        oam_uint8 queue,
        oam_queue_stats64_t *queue_stats)
{
    OAM_ASSERT_RET(queue_stats != NULL,OAM_E_PARAM);

    OAM_PORT_ID_CHECK(port_id);
    OAM_QUEUE_ID_CHECK(queue);

    oam_queue_stats_update(port_id, queue);

    memcpy(queue_stats,&g_oam_stats_mgmt.queue_stats[port_id][queue],sizeof(oam_queue_stats64_t));
    
    return OAM_E_OK;

}

/* This API is for guarddog polling, guarddog polling and oam stats polling in the same timer handle, 
    so no need to update stats from driver, just read them from OAM software table */
oam_status oam_onu_uni_stats_sw_get(
        oam_port_id_t port_id,
        oam_uni_stats64_t *uni_stats)
{
    oam_uint32 i = port_id-1;

    OAM_ASSERT_RET(uni_stats != NULL,OAM_E_PARAM);

    OAM_UNI_PORT_ID_CHECK(port_id);

    memcpy(uni_stats,&g_oam_stats_mgmt.uni_stats[i],sizeof(g_oam_stats_mgmt.uni_stats[i]));
      
    return OAM_E_OK;

}
        
/* workaround: when uni link down, counters still increased */
void oam_stats_uni_link_event_handle(oam_port_id_t port, oam_uint8 status)
{
  
    if(status){   /* when link up, clear the counters */ 
    
        OAM_LOG_DEBUG("oam port %d link %s, clear stats\r\n", port, (status)?"up":"down");
        oam_uni_stats_clear_adapt(port);
        oam_stats_polling_enable_set(port, TRUE);
       
    }else{  /* when link down, disable stats polling */ 
        OAM_LOG_DEBUG("oam port %d link %s, update port stats\r\n", port, (status)?"up":"down");
        oam_uni_stats_update(port);
        OAM_LOG_DEBUG("oam port %d link %s, disable stats polling\r\n", port, (status)?"up":"down");
        oam_stats_polling_enable_set(port, FALSE);
    }
}

void oam_port_stats_dump(oam_port_id_t port,  oam_boolean clear, oam_print func, void *fd)
/*                                                                           */
/* INPUTS  : o llid - llid                                                   */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* Dump llid interface information to to screen                              */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint32 i, j;
    oam_pon_stats64_t pon_stats;
    epon_if_stats_t if_stats;
    oam_uni_stats64_t uni_stats;

    if(port == OAM_PON_PORT_ID)
    {
        memset(&pon_stats, 0, sizeof(oam_pon_stats64_t));
        memset(&if_stats, 0, sizeof(epon_if_stats_t));
        oam_onu_pon_stats_get(&pon_stats);

        OAM_PRINTF("\r\nOAM STATS of PON port \r\n");
        OAM_PRINTF("pon_rx_byte_cnt:           %lld\r\n", pon_stats.pon_byte_cnt);
        OAM_PRINTF("pon_rx_frame_cnt:          %lld\r\n", pon_stats.pon_frame_cnt);
        OAM_PRINTF("pon_rx_fcserr_cnt:         %lld\r\n", pon_stats.pon_rx_fcs_err);
        OAM_PRINTF("pon_rx_crcerr_cnt:         %lld\r\n", pon_stats.pon_crcerr_cnt);
        OAM_PRINTF("pon_ucframe_cnt:        %lld\r\n", pon_stats.pon_ucframe_cnt);
        OAM_PRINTF("pon_mcframe_cnt:        %lld\r\n", pon_stats.pon_mcframe_cnt);
        OAM_PRINTF("pon_bcframe_cnt:        %lld\r\n\r\n", pon_stats.pon_bcframe_cnt);

        OAM_PRINTF("pon_rmon64_cnt:         %lld\r\n", pon_stats.pon_rmon64_cnt);
        OAM_PRINTF("pon_rmon65_127_cnt:     %lld\r\n", pon_stats.pon_rmon65_127_cnt);
        OAM_PRINTF("pon_rmon128_255_cnt:    %lld\r\n", pon_stats.pon_rmon128_255_cnt);
        OAM_PRINTF("pon_rmon256_511_cnt:    %lld\r\n", pon_stats.pon_rmon256_511_cnt);
        OAM_PRINTF("pon_rmon512_1023_cnt:   %lld\r\n", pon_stats.pon_rmon512_1023_cnt);
        OAM_PRINTF("pon_rmon1024_1518_cnt:  %lld\r\n", pon_stats.pon_rmon1024_1518_cnt);
        OAM_PRINTF("pon_rx1519_maxcnt:      %lld\r\n\r\n", pon_stats.pon_rx1519_maxcnt);

        OAM_PRINTF("pon_tx_byte_cnt:        %lld\r\n", pon_stats.pon_tx_byte_cnt);
        OAM_PRINTF("pon_tx_frame_cnt:        %lld\r\n", pon_stats.pon_txframe_cnt);
        OAM_PRINTF("pon_txucframe_cnt:        %lld\r\n", pon_stats.pon_txucframe_cnt);
        OAM_PRINTF("pon_txmcframe_cnt:        %lld\r\n", pon_stats.pon_txmcframe_cnt);
        OAM_PRINTF("pon_txbcframe_cnt:        %lld\r\n\r\n", pon_stats.pon_txbcframe_cnt);

        OAM_PRINTF("pon_txrmon64_cnt:       %lld\r\n", pon_stats.pon_txrmon64_cnt);
        OAM_PRINTF("pon_txrmon65_127_cnt:   %lld\r\n", pon_stats.pon_txrmon65_127_cnt);
        OAM_PRINTF("pon_txrmon128_255_cnt:  %lld\r\n", pon_stats.pon_txrmon128_255_cnt);
        OAM_PRINTF("pon_txrmon256_511_cnt:  %lld\r\n", pon_stats.pon_txrmon256_511_cnt);
        OAM_PRINTF("pon_txrmon512_1023_cnt: %lld\r\n", pon_stats.pon_txrmon512_1023_cnt);
        OAM_PRINTF("pon_txrmon1024_1518_cnt:%lld\r\n", pon_stats.pon_txrmon1024_1518_cnt);
        OAM_PRINTF("pon_tx1519_maxcnt:      %lld\r\n\r\n", pon_stats.pon_tx1519_maxcnt);

        OAM_PRINTF("pon_mpcp_uc_rev_cnt:    %lld\r\n", pon_stats.pon_mpcp_uc_rev_cnt);
        OAM_PRINTF("pon_mpcp_uc_reg_ack:    %lld\r\n", pon_stats.pon_mpcp_uc_reg_ack);
        OAM_PRINTF("pon_mpcp_uc_reg_nack:   %lld\r\n", pon_stats.pon_mpcp_uc_reg_nack);
        OAM_PRINTF("pon_mpcp_uc_reg_dereg:  %lld\r\n", pon_stats.pon_mpcp_uc_reg_dereg);
        OAM_PRINTF("pon_mpcp_uc_reg_rereg:  %lld\r\n", pon_stats.pon_mpcp_uc_reg_rereg);
        OAM_PRINTF("pon_mpcp_uc_reg_reg_rev:%lld\r\n", pon_stats.pon_mpcp_uc_reg_reg_rev);
        OAM_PRINTF("pon_mpcp_uc_gat_rev:    %lld\r\n\r\n", pon_stats.pon_mpcp_uc_gat_rev);

        OAM_PRINTF("pon_mpcp_bc_rev_cnt:    %lld\r\n", pon_stats.pon_mpcp_bc_rev_cnt);
        OAM_PRINTF("pon_mpcp_bc_reg_ack:    %lld\r\n", pon_stats.pon_mpcp_bc_reg_ack);
        OAM_PRINTF("pon_mpcp_bc_reg_nack:   %lld\r\n", pon_stats.pon_mpcp_bc_reg_nack);
        OAM_PRINTF("pon_mpcp_bc_reg_dereg:  %lld\r\n", pon_stats.pon_mpcp_bc_reg_dereg);
        OAM_PRINTF("pon_mpcp_bc_reg_rereg:  %lld\r\n", pon_stats.pon_mpcp_bc_reg_rereg);
        OAM_PRINTF("pon_mpcp_bc_reg_reg_rev:%lld\r\n", pon_stats.pon_mpcp_bc_reg_reg_rev);
        OAM_PRINTF("pon_mpcp_bc_gat_rev:    %lld\r\n", pon_stats.pon_mpcp_bc_gat_rev);
        OAM_PRINTF("pon_mpcp_rx_oam:        %lld\r\n\r\n", pon_stats.pon_mpcp_rx_oam);

        OAM_PRINTF("pon_mpcp_tx_req_dereg:  %lld\r\n", pon_stats.pon_mpcp_tx_req_dereg);
        OAM_PRINTF("pon_mpcp_tx_req_reg:    %lld\r\n", pon_stats.pon_mpcp_tx_req_reg);
        OAM_PRINTF("pon_mpcp_tx_ack_ack:    %lld\r\n", pon_stats.pon_mpcp_tx_ack_ack);
        OAM_PRINTF("pon_mpcp_tx_ack_nack:   %lld\r\n", pon_stats.pon_mpcp_tx_ack_nack);
        OAM_PRINTF("pon_mpcp_tx_rpt:        %lld\r\n", pon_stats.pon_mpcp_tx_rpt);
        OAM_PRINTF("pon_mpcp_tx_oam:        %lld\r\n\r\n", pon_stats.pon_mpcp_tx_oam);

        for(j = 0; j < OAM_PORT_QUEUE_NUM_MAX; j++)
        {
            oam_queue_stats64_t queue_stats;
            oam_onu_queue_stats_get(0, j, &queue_stats);
            OAM_PRINTF("QUEUE %d:\r\n", j);

            OAM_PRINTF("  fwd_frames: %lld, drop_frames: %lld, fwd_bytes: %lld, drop_bytes: %lld\r\n", 
                queue_stats.fwd_frames, queue_stats.drop_frames, queue_stats.fwd_bytes, queue_stats.drop_bytes);

        }
        OAM_PRINTF("\r\n");
        if(clear)
        {
            OAM_PRINTF("pon stats clear\r\n");
            oam_pon_stats_reset();
            for(j = 0; j < OAM_PORT_QUEUE_NUM_MAX; j++)
            {
                oam_queue_stats_reset(0, j);
            }
        }

    }else{

        memset(&uni_stats, 0, sizeof(oam_uni_stats64_t));
        oam_onu_uni_stats_get(port, &uni_stats);

        OAM_PRINTF("\r\nOAM STATS of UNI port %d\r\n", port);

        OAM_PRINTF("rxbyte_cnt:             %lld\r\n", uni_stats.rxbyte_cnt);
        OAM_PRINTF("rxfrm_cnt:              %lld\r\n", uni_stats.rxfrm_cnt);
        OAM_PRINTF("rxucfrm_cnt:            %lld\r\n", uni_stats.rxucfrm_cnt);
        OAM_PRINTF("rxmcfrm_cnt:            %lld\r\n", uni_stats.rxmcfrm_cnt);
        OAM_PRINTF("rxbcfrm_cnt:            %lld\r\n", uni_stats.rxbcfrm_cnt);
        OAM_PRINTF("rxoamfrm_cnt:           %lld\r\n", uni_stats.rxoamfrm_cnt);
        OAM_PRINTF("rxpausefrm_cnt:         %lld\r\n", uni_stats.rxpausefrm_cnt);
        OAM_PRINTF("rxcrcerrfrm_cnt:        %lld\r\n\r\n", uni_stats.rxcrcerrfrm_cnt);

        OAM_PRINTF("rxstatsfrm64_cnt:       %lld\r\n", uni_stats.rxstatsfrm64_cnt);
        OAM_PRINTF("rxstatsfrm65_127_cnt:   %lld\r\n", uni_stats.rxstatsfrm65_127_cnt);
        OAM_PRINTF("rxstatsfrm128_255_cnt:  %lld\r\n", uni_stats.rxstatsfrm128_255_cnt);
        OAM_PRINTF("rxstatsfrm256_511_cnt:  %lld\r\n", uni_stats.rxstatsfrm256_511_cnt);
        OAM_PRINTF("rxstatsfrm512_1023_cnt: %lld\r\n", uni_stats.rxstatsfrm512_1023_cnt);
        OAM_PRINTF("rxstatsfrm1024_1518_cnt:%lld\r\n", uni_stats.rxstatsfrm1024_1518_cnt);
        OAM_PRINTF("rxstatsfrm1519_Max_cnt: %lld\r\n\r\n", uni_stats.rxstatsfrm1519_Max_cnt);

        OAM_PRINTF("txbyte_cnt:             %lld\r\n", uni_stats.txbyte_cnt);
        OAM_PRINTF("txfrm_cnt:              %lld\r\n", uni_stats.txfrm_cnt);
        OAM_PRINTF("txucfrm_cnt:            %lld\r\n", uni_stats.txucfrm_cnt);
        OAM_PRINTF("txmcfrm_cnt:            %lld\r\n", uni_stats.txmcfrm_cnt);
        OAM_PRINTF("txbcfrm_cnt:            %lld\r\n", uni_stats.txbcfrm_cnt);
        OAM_PRINTF("txoamfrm_cnt:           %lld\r\n", uni_stats.txoamfrm_cnt);
        OAM_PRINTF("txpausefrm_cnt:         %lld\r\n", uni_stats.txpausefrm_cnt);
        OAM_PRINTF("txcrcerrfrm_cnt:        %lld\r\n\r\n", uni_stats.txcrcerrfrm_cnt);

        OAM_PRINTF("txstatsfrm64_cnt:       %lld\r\n", uni_stats.txstatsfrm64_cnt);
        OAM_PRINTF("txstatsfrm65_127_cnt:   %lld\r\n", uni_stats.txstatsfrm65_127_cnt);
        OAM_PRINTF("txstatsfrm128_255_cnt:  %lld\r\n", uni_stats.txstatsfrm128_255_cnt);
        OAM_PRINTF("txstatsfrm256_511_cnt:  %lld\r\n", uni_stats.txstatsfrm256_511_cnt);
        OAM_PRINTF("txstatsfrm512_1023_cnt: %lld\r\n", uni_stats.txstatsfrm512_1023_cnt);
        OAM_PRINTF("txstatsfrm1024_1518_cnt:%lld\r\n", uni_stats.txstatsfrm1024_1518_cnt);
        OAM_PRINTF("txstatsfrm1519_Max_cnt: %lld\r\n\r\n", uni_stats.txstatsfrm1519_Max_cnt);

        OAM_PRINTF("alignment_errors:       %lld\r\n", uni_stats.alignment_errors);
        OAM_PRINTF("single_collision:       %lld\r\n", uni_stats.single_collision);
        OAM_PRINTF("multiple_collision:     %lld\r\n", uni_stats.multiple_collision);
        OAM_PRINTF("deferred_transmissions: %lld\r\n", uni_stats.deferred_transmissions);
        OAM_PRINTF("foam_errors:            %lld\r\n", uni_stats.foam_errors);
        OAM_PRINTF("late_collisions:        %lld\r\n", uni_stats.late_collisions);
        OAM_PRINTF("txstatsfrm1519_Max_cnt: %lld\r\n", uni_stats.txstatsfrm1519_Max_cnt);
        OAM_PRINTF("internal_mac_rx_errors: %lld\r\n", uni_stats.internal_mac_rx_errors);
        OAM_PRINTF("internal_mac_tx_errors: %lld\r\n", uni_stats.internal_mac_tx_errors);
        OAM_PRINTF("symbol_errors:          %lld\r\n\r\n", uni_stats.symbol_errors);

        for(j = 0; j < OAM_PORT_QUEUE_NUM_MAX; j++)
        {
            oam_queue_stats64_t queue_stats;
            oam_onu_queue_stats_get(port, j, &queue_stats);
            OAM_PRINTF("QUEUE %d:\r\n", j);

            OAM_PRINTF("  fwd_frames: %lld, drop_frames: %lld, fwd_bytes: %lld, drop_bytes: %lld\r\n", 
                queue_stats.fwd_frames, queue_stats.drop_frames, queue_stats.fwd_bytes, queue_stats.drop_bytes);
        }
        OAM_PRINTF("\r\n");
        
        if(clear)
        {
            OAM_PRINTF("uni stats clear\r\n");
            oam_uni_stats_reset(port);
            for(j = 0; j < OAM_PORT_QUEUE_NUM_MAX; j++)
            {
                oam_queue_stats_reset(port, j);
            }
        }
    }
}
