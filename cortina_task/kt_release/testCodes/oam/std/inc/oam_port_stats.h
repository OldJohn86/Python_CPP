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
#ifndef __OAM_PORT_STATS_H__
#define __OAM_PORT_STATS_H__
#include "oam_std_pdu.h"

typedef struct {
    oam_uint64  pon_byte_cnt           ;
    oam_uint64  pon_tx_byte_cnt        ;
    oam_uint64  pon_frame_cnt          ;
    oam_uint64  pon_txframe_cnt        ;
    oam_uint64  pon_crcerr_cnt         ;
    oam_uint64  pon_ucframe_cnt        ;
    oam_uint64  pon_mcframe_cnt        ;
    oam_uint64  pon_bcframe_cnt        ;
    oam_uint64  pon_txucframe_cnt      ;
    oam_uint64  pon_txmcframe_cnt      ;
    oam_uint64  pon_txbcframe_cnt      ;
    oam_uint64  pon_ctrlframe_cnt      ;
    oam_uint64  pon_txctrlframe_cnt    ;
    oam_uint64  pon_pauseframe_cnt     ;
    oam_uint64  pon_unknownop_cnt      ;
    oam_uint64  pon_runt_cnt           ;
    oam_uint64  pon_oversize_cnt       ;
    oam_uint64  pon_rmon64_cnt         ;
    oam_uint64  pon_rmon65_127_cnt     ;
    oam_uint64  pon_rmon128_255_cnt    ;
    oam_uint64  pon_rmon256_511_cnt    ;
    oam_uint64  pon_rmon512_1023_cnt   ;
    oam_uint64  pon_rmon1024_1518_cnt  ;
    oam_uint64  pon_txrmon64_cnt       ;
    oam_uint64  pon_txrmon65_127_cnt   ;
    oam_uint64  pon_txrmon128_255_cnt  ;
    oam_uint64  pon_txrmon256_511_cnt  ;
    oam_uint64  pon_txrmon512_1023_cnt ;
    oam_uint64  pon_txrmon1024_1518_cnt;
    oam_uint64  pon_mpcp_uc_rev_cnt     ;
    oam_uint64  pon_mpcp_uc_reg_ack     ;
    oam_uint64  pon_mpcp_uc_reg_nack    ;
    oam_uint64  pon_mpcp_uc_reg_dereg   ;
    oam_uint64  pon_mpcp_uc_reg_rereg   ;
    oam_uint64  pon_mpcp_uc_reg_reg_rev ;
    oam_uint64  pon_mpcp_uc_gat_rev     ;
    oam_uint64  pon_mpcp_uc_gat_norm    ;
    oam_uint64  pon_mpcp_uc_gat_frpt    ;
    oam_uint64  pon_mpcp_uc_gat_udis    ;
    oam_uint64  pon_mpcp_uc_gat_bdis    ;
    oam_uint64  pon_mpcp_mc_rev_cnt     ;
    oam_uint64  pon_mpcp_mc_reg_ack     ;
    oam_uint64  pon_mpcp_mc_reg_nack    ;
    oam_uint64  pon_mpcp_mc_reg_dereg   ;
    oam_uint64  pon_mpcp_mc_reg_rereg   ;
    oam_uint64  pon_mpcp_mc_reg_reg_rev ;
    oam_uint64  pon_mpcp_mc_gat_rev     ;
    oam_uint64  pon_mpcp_mc_gat_norm    ;
    oam_uint64  pon_mpcp_mc_gat_frpt    ;
    oam_uint64  pon_mpcp_mc_gat_udis    ;
    oam_uint64  pon_mpcp_mc_gat_bdis    ;
    oam_uint64  pon_mpcp_bc_rev_cnt     ;
    oam_uint64  pon_mpcp_bc_reg_ack     ;
    oam_uint64  pon_mpcp_bc_reg_nack    ;
    oam_uint64  pon_mpcp_bc_reg_dereg   ;
    oam_uint64  pon_mpcp_bc_reg_rereg   ;
    oam_uint64  pon_mpcp_bc_reg_reg_rev ;
    oam_uint64  pon_mpcp_bc_gat_rev     ;
    oam_uint64  pon_mpcp_bc_gat_norm    ;
    oam_uint64  pon_mpcp_bc_gat_frpt    ;
    oam_uint64  pon_mpcp_bc_gat_udis    ;
    oam_uint64  pon_mpcp_bc_gat_bdis    ;
    oam_uint64  pon_mpcp_rx_oam         ;
    oam_uint64  pon_mpcp_tx_req_dereg   ;
    oam_uint64  pon_mpcp_tx_req_reg     ;
    oam_uint64  pon_mpcp_tx_ack_ack     ;
    oam_uint64  pon_mpcp_tx_ack_nack    ;
    oam_uint64  pon_mpcp_tx_rpt    ;
    oam_uint64  pon_mpcp_tx_oam    ;
    oam_uint64  pon_mpcp_da_mc_nslf;
    oam_uint64  pon_mpcp_da_mc_slf ;
    oam_uint64  pon_mpcp_da_uc     ;
    oam_uint64  pon_txoversizecnt  ;
    oam_uint64  pon_rx1519_maxcnt  ;
    oam_uint64  pon_tx1519_maxcnt  ;
    oam_uint64  pon_txpausecnt     ;
    oam_uint64  pon_undersizecnt   ;
    oam_uint64  pon_jabbercnt      ;
    oam_uint64  pon_extensioncnt   ;
    oam_uint64  pon_txextensioncnt ;
    oam_uint64  pon_err_symbol     ;
    oam_uint64  pon_outofsync      ;
    oam_uint64  pon_sld_err        ;
    oam_uint64  pon_crc8_err       ;
    oam_uint64  pon_mac_drop       ;
    oam_uint64  pon_ipg_drop       ;
    oam_uint64  pon_drain          ;
    oam_uint64  pon_tx_crc_err     ;
    oam_uint64  pon_rx_fcs_err     ;
} oam_pon_stats64_t;

typedef struct{
    oam_uint64 fwd_frames;
    oam_uint64 drop_frames;
    oam_uint64 fwd_bytes;
    oam_uint64 drop_bytes;
}oam_queue_stats64_t;

/* PON, UNI MIB stats, CRC stats, guarddog share the same polling timer */
#define OAM_STATS_POLL_ACCURACY             10000 /* 10s timer */
#define OAM_STATS_CRC_POLL_ACCURARY         OAM_STATS_POLL_ACCURACY
#define OAM_STATS_MIB_POLL_ACCURARY         30000  /* PON, UNI port stats, guarddog */

void oam_port_stats_init();
oam_status oam_stats_polling_enable(oam_boolean enable);
oam_status oam_stats_polling_enable_set(oam_port_id_t port, oam_boolean enable);
oam_status oam_stats_polling_enable_get(oam_port_id_t port, oam_boolean *enable);
void oam_stats_uni_link_event_handle(oam_port_id_t port, oam_uint8 status);

void oam_port_stats_polling();

oam_status oam_onu_pon_stats_get(oam_pon_stats64_t *pon_stats);
oam_status oam_onu_std_pon_stats_get(epon_if_stats_t *if_stats);

void oam_onu_pon_stats_summary_get(
    oam_uint64 *in_frame,
    oam_uint64 *in_errors,
    oam_uint64 *in_err_symb,
    oam_uint64 *in_octets);

oam_status oam_onu_uni_stats_get(
        oam_port_id_t port_id,
        oam_uni_stats64_t *uni_stats);

oam_status oam_onu_queue_stats_get(
        oam_port_id_t port_id,
        oam_uint8 queue,
        oam_queue_stats64_t *queue_stats);

oam_status oam_pon_stats_reset();
oam_status oam_uni_stats_reset(oam_port_id_t port_id);
oam_status oam_queue_stats_reset(oam_port_id_t port_id, oam_uint8 queue);
oam_status oam_port_stats_reset();

oam_status oam_onu_uni_stats_sw_get(
        oam_port_id_t port_id,
        oam_uni_stats64_t *uni_stats);


#endif
