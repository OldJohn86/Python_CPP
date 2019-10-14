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

Copyright (c) 2013 by Cortina Systems Incorporated
****************************************************************************/
#ifndef _OAM_DPOE_ADAPT_H_
#define _OAM_DPOE_ADAPT_H_

#include "oam_types.h"
#include "oam_dpoe_frame.h"
#include "ca_version.h"
#include "queue.h"
#include "oam_dpoe.h"

/* DPOE UNI port id start with 0, add offset 1 to OAM_UNI_PORT_ID1 */
#define DPOE_OAM_UNI_PORT_ID_TO_OAM_PORT(port)    (port+1);

#define DPOE_OAM_UNI_PORT_ID_CHECK(port)    OAM_UNI_PORT_ID_CHECK(port+1);

#define DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port)    oam_port_to_ca_port_adapt(port+1);

#define DPOE_OAM_PON_PORT_ID_TO_CA_PORT()        oam_port_to_ca_port_adapt(OAM_PON_PORT_ID);

#define DPOE_L2_MCAST_LLID_MAX_NUM 8

/* SIEPON-1904, 14.4.2 */
typedef struct{
    oam_uint64   tx_mac_ctrl;
    oam_uint64   tx_reg_ack;
    oam_uint64   tx_reg_request;
    oam_uint64   tx_report;

    oam_uint64   rx_mac_ctrl;
    oam_uint64   rx_gate;
    oam_uint64   rx_register;
    
}dpoe_oam_mpcp_stats_t;

/* SIEPON-1904, 14.4.3 */
typedef struct{
    oam_uint64   rx_frames_ok;
    oam_uint64   tx_frames_ok;

    oam_uint64   rx_bytes_ok;
    oam_uint64   tx_bytes_ok;

    oam_uint64   rx_frames_green;
    oam_uint64   tx_frames_green;

    oam_uint64   rx_uc_frames;
    oam_uint64   rx_mc_frames;
    oam_uint64   rx_bc_frames;

    oam_uint64   tx_uc_frames;
    oam_uint64   tx_mc_frames;
    oam_uint64   tx_bc_frames;

    oam_uint64   rx_pause_frames;
    oam_uint64   tx_pause_frames;
    
    oam_uint64   rx_frames_short;
    oam_uint64   rx_frames_64;
    oam_uint64   rx_frames_65_127;
    oam_uint64   rx_frames_128_255;
    oam_uint64   rx_frames_256_511;
    oam_uint64   rx_frames_512_1023;
    oam_uint64   rx_frames_1024_1518;
    oam_uint64   rx_frames_1519;
    oam_uint64   rx_frames_oversize;

    oam_uint64   tx_frames_64;
    oam_uint64   tx_frames_65_127;
    oam_uint64   tx_frames_128_255;
    oam_uint64   tx_frames_256_511;
    oam_uint64   tx_frames_512_1023;
    oam_uint64   tx_frames_1024_1518;
    oam_uint64   tx_frames_1519;

    oam_uint64   rx_fcs_error;
    oam_uint64   rx_error;  /* crc, oversize, undersize */
    oam_uint64   tx_error;
    oam_uint64   overlimit_drop;
    
}dpoe_oam_eth_stats_t;

typedef enum {
    DPOE_OAM_SFP_MEDIA = 0x00 ,
    DPOE_OAM_BASE_T_MEDIA = 0x01,    
}dpoe_oam_media_type_t;

typedef enum{
    DPOE_OAM_UNSPEC = 0x00,
    DPOE_OAM_EMTA = 0x01,
    DPOE_OAM_ESTB_IP = 0x02,
    DPOE_OAM_ESTB_DSG = 0x03,
    DPOE_OAM_ETEA = 0x04,
    DPOE_OAM_ESG = 0x05,
    DPOE_OAM_EROUTER = 0x06,
    DPOE_OAM_EDVA = 0x07,
    DPOE_OAM_SEB_ESTB_IP = 0x08,
    DPOE_OAM_CMCI = 0x09,
    DPOE_OAM_MU = 0x0A,
    DPOE_OAM_MI = 0x0B,
    DPOE_OAM_OTHER_INTERNAL = 0x0C,   
}dpoe_oam_s_interface_type_t;

oam_status dpoe_oam_onu_id_get_adapt(oam_uint8 *onu_id);
oam_status dpoe_oam_max_llid_get_adapt(oam_uint16 *bidir, oam_uint16 *ds_only);
oam_status dpoe_oam_report_thresh_get_adapt(
    oam_uint8 index,
    oam_uint8 *queueset_num,
    oam_uint8 *value_per_set,
    oam_uint16 *report_thresh);
oam_status dpoe_oam_report_thresh_set_adapt(
    oam_uint8 index,
    oam_uint8 queueset_num,
    oam_uint8 value_per_set,
    oam_uint16 *report_thresh);
oam_uint8 dpoe_oam_llid_fwd_state_get_adapt(oam_uint8 llid);
void dpoe_oam_fw_info_get_adapt(fw_info_t *info);
void dpoe_oam_chip_info_get_adapt(chip_info_t *info);
void dpoe_oam_manuday_get_adapt(manuday_t *info);
void dpoe_oam_fw_mfg_get_adapt(fw_mfg_time_t *time);
void dpoe_oam_port_type_get_adapt(oam_uint8 *total_port, oam_uint8 *value);
void dpoe_oam_reset_onu_adapt();
oam_uint32 dpoe_oam_dynmac_size_get_adapt();
oam_uint32 dpoe_oam_dynmac_table_get_adapt(oam_uint16 port, oam_uint16 *mac_number, oam_uint8 *buf);
void dpoe_oam_aging_set_adapt(oam_uint16 time);
oam_uint16 dpoe_oam_aging_get_adapt();
oam_uint32 dpoe_oam_stcmac_table_get_adapt(oam_uint16 port, oam_uint16 *mac_number, oam_uint8 *buf);
void dpoe_oam_autonet_get_adapt(oam_uint16 port, oam_uint16 *max_cap, oam_uint16 *curr_cap);
void dpoe_oam_autonet_set_adapt(oam_uint16 port, oam_uint16 cap);
void dpoe_oam_uni_adm_ctrl_get_adapt(oam_uint16 port, oam_uint8 *enabled);
void dpoe_oam_uni_adm_ctrl_set_adapt(oam_uint16 port, oam_uint8 enable);
void dpoe_oam_uni_minmac_get_adapt(oam_uint16 port, oam_uint16 *nmb);
void dpoe_oam_uni_minmac_set_adapt(oam_uint16 port, oam_uint16 nmb);
void dpoe_oam_uni_maxmac_get_adapt(oam_uint16 port, oam_uint16 *nmb);
void dpoe_oam_uni_maxmac_set_adapt(oam_uint16 port, oam_uint16 nmb);
void dpoe_oam_onu_maxmac_get_adapt(oam_uint16 *nmb);
void dpoe_oam_onu_maxmac_set_adapt(oam_uint16 nmb);
void dpoe_oam_unkn_flooding_get_adapt(oam_uint8 *enable);
void dpoe_oam_unkn_flooding_set_adapt(oam_uint32 enable);
void dpoe_oam_local_switch_get_adapt(oam_uint16 port, oam_uint8 *enable);
void dpoe_oam_local_switch_set_adapt(oam_uint16 port, oam_uint32 enable);
void dpoe_oam_llid_queue_get_adapt(oam_dpoe_llid_queue_int_t *llid_queue);
void dpoe_oam_mac_full_get_adapt(oam_uint16 port, oam_uint8 *beh);
void dpoe_oam_mac_full_set_adapt(oam_uint16 port, oam_uint8 beh);
void dpoe_oam_clr_dyn_mac_adapt(oam_uint16 port);
void dpoe_oam_add_dyn_mac_adapt(oam_uint16 port, oam_uint8 nmb, oam_uint8 *mac);
void dpoe_oam_del_dyn_mac_adapt(oam_uint16 port, oam_uint8 nmb, oam_uint8 *mac);
void dpoe_oam_clr_sta_mac_adapt(oam_uint16 port);
void dpoe_oam_add_sta_mac_adapt(oam_uint16 port, oam_uint8 nmb, oam_uint8 *mac);
void dpoe_oam_del_sta_mac_adapt(oam_uint16 port, oam_uint8 nmb, oam_uint8 *mac);

oam_uint8 dpoe_oam_queue_delay_thr_get_adapt(oam_uint8 queue);
void dpoe_oam_queue_delay_thr_set_adapt(oam_uint8 queue, oam_uint8 value);
oam_uint64 dpoe_oam_queue_delay_val_get_adapt(oam_uint8 queue);
void dpoe_oam_queue_delay_val_reset_adapt(oam_uint8 queue);
oam_uint64 dpoe_oam_frm_dropped_get_adapt(oam_uint8 queue);
void dpoe_oam_frm_dropped_reset_adapt(oam_uint8 queue);
oam_uint64 dpoe_oam_oct_dropped_get_adapt(oam_uint8 queue);
void dpoe_oam_oct_dropped_reset_adapt(oam_uint8 queue);
oam_uint64 dpoe_oam_oct_delayed_get_adapt(oam_uint8 queue);
void dpoe_oam_oct_delayed_reset_adapt(oam_uint8 queue);
oam_uint64 dpoe_oam_usoct_unused_get_adapt(oam_uint8 queue);
void dpoe_oam_usoct_unused_reset_adapt(oam_uint8 queue);
oam_uint16 dpoe_oam_opt_mon_temp_get_adapt();
void dpoe_oam_opt_mon_temp_reset_adapt();
oam_uint16 dpoe_oam_opt_mon_vcc_get_adapt();
void dpoe_oam_opt_mon_vcc_reset_adapt();
oam_uint16 dpoe_oam_opt_mon_bias_get_adapt();
void dpoe_oam_opt_mon_bias_reset_adapt();
oam_uint16 dpoe_oam_opt_mon_txpwr_get_adapt();
void dpoe_oam_opt_mon_txpwr_reset_adapt();
oam_uint16 dpoe_oam_opt_mon_rxpwr_get_adapt();
void dpoe_oam_opt_mon_rxpwr_reset_adapt();
void dpoe_oam_opt_mon_rxpwr_reset_adapt();

oam_uint64 dpoe_oam_rx_frm_y_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_rx_frm_y_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_tx_frm_y_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_tx_frm_y_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_tx_oct_g_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_tx_oct_g_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_rx_oct_y_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_rx_oct_y_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_rx_oct_g_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_rx_oct_g_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_tx_oct_y_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_tx_oct_y_reset_adapt(oam_uint8 type, oam_uint8 value);

oam_uint64 dpoe_oam_rx_frm_l2cp_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_rx_frm_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_rx_oct_l2cp_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_rx_oct_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_tx_frm_l2cp_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_tx_frm_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_tx_oct_l2cp_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_tx_oct_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_disc_frm_l2cp_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_disc_frm_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_disc_oct_l2cp_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_disc_oct_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_tx_err_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_tx_err_reset_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_rx_err_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_rx_err_reset_adapt(oam_uint8 type, oam_uint8 value);

/* service level aggrement */
oam_status dpoe_oam_bcast_limit_get_adapt(oam_uint16 port, oam_uint32 *value);
oam_status dpoe_oam_bcast_limit_set_adapt(oam_uint16 port, oam_uint32 value);
oam_status dpoe_oam_queue_cir_get_adapt(oam_uint16 port, oam_uint8 queue, oam_uint16 *cbs, oam_uint32 *cir);
oam_status dpoe_oam_queue_cir_set_adapt(oam_uint16 port, oam_uint8 queue, oam_uint16 cbs, oam_uint32 cir);
oam_status dpoe_oam_fec_get_adapt(oam_uint16 port, oam_uint32 *ds, oam_uint32 *us);
oam_status dpoe_oam_fec_set_adapt(oam_uint16 port, oam_uint32 ds, oam_uint32 us);
oam_status dpoe_oam_queue_eir_get_adapt(oam_uint16 port, oam_uint8 queue, oam_uint16 *ebs, oam_uint32 *eir);
oam_status dpoe_oam_queue_eir_set_adapt(oam_uint16 port, oam_uint8 queue, oam_uint16 ebs, oam_uint32 eir);
oam_status dpoe_oam_queue_mark_get_adapt(oam_uint16 port, oam_uint8 queue, oam_dpoe_queue_mark_value_t *data);
oam_status dpoe_oam_queue_mark_set_adapt(oam_uint16 port, oam_uint8 queue, oam_dpoe_queue_mark_value_t data);
oam_status dpoe_oam_queue_limiter_cap_get_adapt(oam_uint16 port, oam_uint8 queue, oam_dpoe_queue_limiter_cap_value_t *data);
oam_status dpoe_oam_queue_limiter_cap_set_adapt(oam_uint16 port, oam_uint8 queue, oam_dpoe_queue_limiter_cap_value_t data);
oam_status dpoe_oam_coupling_flag_get_adapt(oam_uint16 port, oam_uint8 queue, oam_uint8 *value);
oam_status dpoe_oam_coupling_flag_set_adapt(oam_uint16 port, oam_uint8 queue, oam_uint8 value);
oam_status dpoe_oam_onu_traffic_set_adapt(oam_boolean en);
oam_status dpoe_oam_llid_traffic_set_adapt(oam_uint16 port, oam_boolean en);
oam_status dpoe_oam_llid_loopabck_set_adapt(oam_uint16 llid, oam_boolean en);
oam_status dpoe_oam_loopback_set_adapt(oam_uint16 port, oam_boolean en, oam_uint8 value);
oam_status dpoe_oam_laser_tx_off_get_adapt(oam_uint16 *value);
oam_status dpoe_oam_laser_tx_off_set_adapt(oam_uint16 value);


oam_status dpoe_oam_mc_ctrl_set_adapt(oam_dpoe_onu_mc_ctrl_t *cfg);
oam_status dpoe_oam_mc_reg_set_adapt(oam_dpoe_onu_mc_reg_t *cfg);
oam_uint32 dpoe_oam_transok_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_scoll_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_mcoll_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_frm_recv_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_fcserr_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_alignerr_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_octtranok_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_frmdef_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_latecoll_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_frmabort_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_frmlost1_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_octok_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_frmlost2_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_mcxmit_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_bcxmit_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_frmexc_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_mcrecv_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_bcrecv_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_inlenerr_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_outlenerr_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_giantlenerr_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_macstat_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_macstat_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 state);
oam_uint32 dpoe_oam_collfrm_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_symerr_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_mac_addr_get_adapt(oam_uint8 *mac);
void dpoe_oam_mac_addr_set_adapt(oam_uint8 *mac);
oam_uint8 dpoe_oam_mdavail_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint8 dpoe_oam_autonegadmin_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint8 dpoe_oam_autonegsig_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint8 dpoe_oam_autonegcfg_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_autonegcfg_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 set);
oam_uint8 dpoe_oam_autoneglctab_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint8 dpoe_oam_autonegadvtab_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_autonegadvtab_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 set);
oam_uint8 dpoe_oam_autonegrcvtab_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint8 dpoe_oam_autoneglcsab_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint8 dpoe_oam_autonegadvsab_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_autonegadvsab_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 set);
oam_uint8 dpoe_oam_autonegrecvsab_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint8 dpoe_oam_duplex_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_duplex_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 set);
oam_uint32 dpoe_oam_macxmit_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_macrecv_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_notspop_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_pauseallow_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_pauseallow_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint32 set);
oam_uint64 dpoe_oam_pausexmit_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint64 dpoe_oam_pauserecv_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_mpcpxmit_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_mpcprecv_get_adapt(oam_uint8 type, oam_uint8 value);
oam_uint32 dpoe_oam_feccb_get_adapt(oam_uint8 type, oam_uint32 *value);
oam_uint32 dpoe_oam_fecucb_get_adapt(oam_uint8 type, oam_uint32 *value);
oam_uint8 dpoe_oam_fecmode_get_adapt(oam_uint8 type, oam_uint8 value);
void dpoe_oam_fecmode_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 set);

ca_uint32_t dpoe_config_rule(oam_uint8 type, oam_uint16 port, dpoe_oam_frm_process_t *rule);
void dpoe_oam_clr_rule_adapt(dpoe_oam_mgmt_obj_t mgmt_obj);

oam_status dpoe_oam_uni_eth_stats_get_adapt(oam_uint16 port, dpoe_oam_eth_stats_t *stats);
oam_status dpoe_oam_pon_eth_stats_get_adapt(dpoe_oam_eth_stats_t *stats);
oam_status dpoe_oam_llid_eth_stats_get_adapt(oam_llid_t llid, dpoe_oam_eth_stats_t *stats);
oam_status dpoe_oam_uni_eth_stats_reset_adapt(oam_uint16 port);
oam_status dpoe_oam_pon_eth_stats_reset_adapt();
oam_status dpoe_oam_llid_eth_stats_reset_adapt(oam_llid_t llid);
oam_uint32 dpoe_oam_pon_mpcp_stats_get_adapt(dpoe_oam_mpcp_stats_t *stats);
oam_uint32 dpoe_oam_llid_mpcp_stats_get_adapt(oam_llid_t llid, dpoe_oam_mpcp_stats_t *stats);
oam_uint32 dpoe_oam_media_type_get_adapt(oam_uint8 *media_type);
oam_uint32 dpoe_oam_epon_mode_get_adapt(oam_uint16 *epon_mode);
void dpoe_oam_del_rule_adapt(oam_uint32 cls_index);

oam_int32 dpoe_oam_upg_img_open(void);
oam_int32 dpoe_oam_upg_img_write(oam_int32 fd, void *data, oam_uint32 data_len);
oam_int32 dpoe_oam_upg_img_close(oam_int32 fd);
void dpoe_oam_upg_img_remove(void);
void dpoe_oam_upg_img_upgrade(void);
oam_int32 dpoe_oam_upg_img_info_get(
    ca_device_id_t device_id,
    ca_uint8_t     img_idx,
    ca_img_info_t  *img_info);
oam_int32 dpoe_oam_upg_img_commit_set(
    ca_device_id_t device_id,
    ca_uint8_t img_idx, 
    ca_uint8_t flag);
oam_int32 dpoe_oam_upg_img_executed_set(
    ca_device_id_t device_id,
    ca_uint8_t img_idx, 
    ca_uint8_t flag);

void dpoe_oam_software_bundle_get_adapt(ca_uint8  *value);
oam_uint32 dpoe_oam_uni_mac_learn_set_adapt(oam_uint16 port,oam_uint8 *buf);
oam_uint32 dpoe_oam_uni_mac_learn_get_adapt(oam_uint16 port, oam_uint16 *mac_number, oam_uint8 *buf);
oam_uint32 dpoe_oam_uni_num_get_adapt(oam_uint16 *num);
oam_status dpoe_oam_queue_eth_stats_get_adapt(oam_uint16 port,ca_uint32_t queue,ca_queue_stats_t *queue_stats);
oam_uint32 dpoe_std_oam_phytype_get_adapt(oam_uint8 type,oam_uint16 port,oam_uint8 *phy_type);
oam_uint32 dpoe_std_oam_macctrl_get_adapt(oam_uint16 port);
void dpoe_oam_manuf_org_name_get_adapt(oam_uint8 *org_name);
void dpoe_oam_mllid_reg_enable_adapt(oam_uint8 llid_num);
oam_uint32 dpoe_oam_esafe_cfg_set_adapt(oam_uint8 value);
oam_uint32 dpoe_oam_esafe_cfg_get_adapt(oam_uint8 *value);
void dpoe_cls_rule_merge(dpoe_oam_frm_process_t *rule);
void dpoe_oam_config_qos_adapt(ca_port_id_t port_id);
oam_uint32 dpoe_oam_cvlan_tpid_set_adapt(oam_uint16 port,oam_uint16 tpid,oam_uint8 insert_flag);
oam_uint32 dpoe_oam_cvlan_tpid_get_adapt(oam_uint16 port,oam_uint16 *tpid,oam_uint8 *insert_flag);
void dpoe_oam_auto_neg_admin_ctrl_get_adapt(oam_uint16 port ,oam_uint8 *ctrl_flag);
void dpoe_oam_auto_neg_admin_ctrl_set_adapt(oam_uint16 port ,oam_uint8 ctrl_flag);
void dpoe_oam_pause_mac_ctl_frame_rev_get_adapt(oam_uint16 port, oam_uint64 *frame);
void __dpoe_vlan_delete_for_cls(ca_uint8 is_uni,ca_port_id_t ca_port,ca_uint32 cls_index);
void dpoe_oam_rule_pmc_fwr_cfg_get_adapt(oam_uint16 *value);
void dpoe_oam_rule_pmc_fwr_cfg_set_adapt(oam_uint16 value);
void dpoe_oam_rx_frame_green_get_adapt(oam_uint64 *frame);
void __dpoe_mcgrp_delete_for_cls(ca_uint32 cls_index);
void dpoe_oam_multicast_llid_get_adapt(oam_uint8          *num,oam_uint16 *mc_llid );
oam_status __dpoe_oam_traffic_cfg(oam_boolean en,ca_llid_t llid_index, oam_llid_t llid);
void __dpoe_oam_default_traffic_cfg_init();
void dpoe_oam_out_of_range_length_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port);
void dpoe_oam_count_tx_frame_1519_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_tx_frame_1518_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_tx_frame_1023_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_tx_frame_511_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_tx_frame_255_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_tx_frame_127_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_tx_frame_64_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_rx_frame_1519_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_rx_frame_1518_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_rx_frame_1023_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_rx_frame_511_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_rx_frame_255_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_rx_frame_127_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_rx_frame_64_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
void dpoe_oam_count_rx_frame_short_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid);
#endif

