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
#ifndef _OAM_DPOE_H_
#define _OAM_DPOE_H_

#ifdef HAVE_DPOE_OAM
#include "oam_dpoe_pdu.h"
#include "oam_supp.h"

#define DPOE_EVENT_LOS   0x11
#define DPOE_EVENT_PORT_DISABLED    0x21
#define DPOE_MAX_OAMPDU_EVENT_LEN         100

enum {
    OAM_DPOE_STATUS_NONE = 0,
    OAM_DPOE_STATUS_START,
    OAM_DPOE_STATUS_FINISH,

    OAM_DPOE_STATUS_MAX
};

typedef struct
{
    oam_uint32 valid;
    oam_uint16 llid;
    oam_uint16 time;
    oam_uint32 mode;
    oam_uint32 number;
    oam_uint32 key_exchange_handler;
    oam_uint32 ver_handler;
} key_exch_llid_time_t;

/* dpoe oam code clean */
#define  DPOE_OAM_BUILD_RESP \
    xx(02, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_ONU_ID,           0x0002, dpoe_oam_onu_id_get,            NULL) \
    xx(03, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_FW_INFO,          0x0003, dpoe_oam_fw_info_get,           NULL) \
    xx(04, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_CHIP_INFO,        0x0004, dpoe_oam_chip_info_get,         NULL) \
    xx(05, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_DATA_MANUF,       0x0005, dpoe_oam_data_manuf_get,        NULL) \
    xx(06, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_MANUF_INFO,       0x0006, dpoe_oam_manuf_info_get,        NULL) \
    xx(07, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_MAX_LLID,         0x0007, dpoe_oam_max_llid_get,          NULL) \
    xx(09, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_UNI_NUM,          0x0009, dpoe_oam_uni_num_get,           NULL) \
    xx(08, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_PON_NUM,          0x0008, dpoe_oam_pon_num_get,           NULL) \
    xx(10, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_PKT_BUFF,         0x000A, dpoe_oam_pkt_buff_get,          NULL) \
    xx(11, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_REPORT_THRESH,    0x000B, dpoe_oam_report_thresh_get,     dpoe_oam_report_thresh_set) \
    xx(12, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_LLID_FWD_STATE,   0x000C, dpoe_oam_llid_fwd_state_get,    NULL) \
    xx(13, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_FRAME_RATE,       0x000D, dpoe_oam_frame_rate_get,        dpoe_oam_frame_rate_set) \
    xx(14, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_MANUF_ORG_NAME,   0x000E, dpoe_oam_manuf_org_name_get,    NULL) \
    xx(15, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_FW_MFG_TIME_VARY, 0x000F, dpoe_oam_fw_mfg_time_vary_get,  dpoe_oam_fw_mfg_time_vary_set) \
    xx(16, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_PORT_TYPE,        0x0010, dpoe_oam_port_type_get,         NULL) \
    xx(50, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_VENDOR_NAME,      0x0011, dpoe_oam_vn_get,                NULL) \
    xx(42, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_MODEL,            0x0012, dpoe_oam_model_name_get,    NULL) \
    xx(43, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_HWVER,            0x0013, dpoe_oam_hw_ver_get,        NULL) \
    xx(44, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_EPON_MODE,        0x0014, dpoe_oam_epon_mode_get,        NULL) \
    xx(45, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_SOFTWARE_BUNDLE,  0x0015, dpoe_oam_software_bundle_get,        NULL) \
    xx(17, OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_RESET_ONU,        0x0001, NULL,                           dpoe_oam_reset_onu) \
    xx(18, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_DYNMAC_SIZE,      0x0101, dpoe_oam_dynmac_size_get,       NULL) \
    xx(19, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_AGING,            0x0102, dpoe_oam_aging_get,             dpoe_oam_aging_set) \
    xx(20, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_DYNMAC_TABLE,     0x0103, dpoe_oam_dynmac_table_get,      NULL) \
    xx(21, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_STCMAC_TABLE,     0x0104, dpoe_oam_stcmac_table_get,      NULL) \
    xx(22, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_AUTONEG,          0x0105, dpoe_oam_autoneg_get,           dpoe_oam_autoneg_set) \
    xx(23, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_SRCADDR_ADMCTRL,  0x0106, dpoe_oam_srcaddr_admctrl_get,   dpoe_oam_srcaddr_admctrl_set) \
    xx(24, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_UNI_MIN_MAC,      0x0107, dpoe_oam_uni_minmac_get,        dpoe_oam_uni_minmac_set) \
    xx(25, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_UNI_MAX_MAC,      0x0108, dpoe_oam_uni_maxmac_get,        dpoe_oam_uni_maxmac_set) \
    xx(26, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_ONU_MAX_MAC,      0x0109, dpoe_oam_onu_maxmac_get,        dpoe_oam_onu_maxmac_set) \
    xx(27, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_LENGTH_ERR_DISC,  0x010a, dpoe_oam_length_err_disc_get,   dpoe_oam_length_err_disc_set) \
    xx(28, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_UNKN_FLOODING,    0x010b, dpoe_oam_unkn_flooding_get,     dpoe_oam_unkn_flooding_set) \
    xx(29, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_LOCAL_SWITCH,     0x010c, dpoe_oam_local_switch_get,      dpoe_oam_local_switch_set) \
    xx(30, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_LLID_QUEUE_CFG,   0x010d, dpoe_oam_llid_queue_get,        dpoe_oam_llid_queue_set) \
    xx(31, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_FW_NAME_CFG,      0x010e, dpoe_oam_fw_name_get,           NULL) \
    xx(32, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_MAC_FULL_CFG,     0x010f, dpoe_oam_mac_full_get,          dpoe_oam_mac_full_set) \
    xx(35, OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_CLR_DYN_MAC_CFG,  0x0101, NULL,                           dpoe_oam_clr_dyn_mac) \
    xx(36, OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_ADD_DYN_MAC_CFG,  0x0102, NULL,                           dpoe_oam_add_dyn_mac) \
    xx(37, OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_DEL_DYN_MAC_CFG,  0x0103, NULL,                           dpoe_oam_del_dyn_mac) \
    xx(38, OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_CLR_STA_MAC_CFG,  0x0104, NULL,                           dpoe_oam_clr_sta_mac) \
    xx(39, OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_ADD_STA_MAC_CFG,  0x0105, NULL,                           dpoe_oam_add_sta_mac) \
    xx(40, OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_DEL_STA_MAC_CFG,  0x0106, NULL,                           dpoe_oam_del_sta_mac) \
    xx(51, OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_CFG_MC_LLID,      0x0107, NULL,                           dpoe_oam_cfg_mc_llid) \
    xx(53, OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_GET_UNI_MAC_LEARN,0x0108, dpoe_oam_uni_mac_learn_get,     dpoe_oam_uni_mac_learn_set) \
    xx(52, OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_CLEAR_COUNTERS,   0x0201, NULL,                           dpoe_oam_clear_counters_set) \
    xx(60, OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_MC_LLID,      	0x0110, dpoe_oam_multicast_llid_get,    NULL                ) \
    xx(101,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_GREEN,     0x0201, dpoe_oam_rx_frame_green_get,    dpoe_oam_eth_stats_reset) \
    xx(102,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_FRM_GREEN,     0x0202, dpoe_oam_eth_stats_get,      dpoe_oam_eth_stats_reset) \
    xx(103,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_SHORT,     0x0203, dpoe_oam_count_rx_frame_short_get,   dpoe_oam_eth_stats_reset) \
    xx(104,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_64,        0x0204, dpoe_oam_count_rx_frame_64_get,      dpoe_oam_eth_stats_reset) \
    xx(105,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_127,       0x0205, dpoe_oam_count_rx_frame_127_get,     dpoe_oam_eth_stats_reset) \
    xx(106,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_255,       0x0206, dpoe_oam_count_rx_frame_255_get,     dpoe_oam_eth_stats_reset) \
    xx(107,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_511,       0x0207, dpoe_oam_count_rx_frame_511_get,     dpoe_oam_eth_stats_reset) \
    xx(108,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_1023,      0x0208, dpoe_oam_count_rx_frame_1023_get,    dpoe_oam_eth_stats_reset) \
    xx(109,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_1518,      0x0209, dpoe_oam_count_rx_frame_1518_get,    dpoe_oam_eth_stats_reset) \
    xx(110,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_1519,      0x020a, dpoe_oam_count_rx_frame_1519_get,    dpoe_oam_eth_stats_reset) \
    xx(111,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_FRM_64,        0x020b, dpoe_oam_count_tx_frame_64_get,      dpoe_oam_eth_stats_reset) \
    xx(112,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_FRM_127,       0x020c, dpoe_oam_count_tx_frame_127_get,     dpoe_oam_eth_stats_reset) \
    xx(113,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_FRM_255,       0x020d, dpoe_oam_count_tx_frame_255_get,     dpoe_oam_eth_stats_reset) \
    xx(114,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_FRM_511,       0x020e, dpoe_oam_count_tx_frame_511_get,     dpoe_oam_eth_stats_reset) \
    xx(115,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_FRM_1023,      0x020f, dpoe_oam_count_tx_frame_1023_get,    dpoe_oam_eth_stats_reset) \
    xx(116,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_FRM_1518,      0x0210, dpoe_oam_count_tx_frame_1518_get,    dpoe_oam_eth_stats_reset) \
    xx(117,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_FRM_1519,      0x0211, dpoe_oam_count_tx_frame_1519_get,    dpoe_oam_eth_stats_reset) \
    xx(118,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_QUEUE_DELAY_THR,  0x0212, dpoe_oam_queue_delay_thr_get,dpoe_oam_queue_delay_thr_set) \
    xx(119,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_QUEUE_DELAY_VAL,  0x0213, dpoe_oam_queue_delay_val_get,dpoe_oam_queue_delay_val_reset) \
    xx(120,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_FRAME_DROPPED,    0x0214, dpoe_oam_eth_stats_get,      dpoe_oam_eth_stats_reset) \
    xx(121,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_OCT_DROPPED,      0x0215, dpoe_oam_eth_stats_get,      dpoe_oam_eth_stats_reset) \
    xx(122,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_OCT_DELAYED,      0x0216, dpoe_oam_oct_delayed_get,    dpoe_oam_oct_delayed_reset) \
    xx(123,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_USOCT_UNUSED,     0x0217, dpoe_oam_eth_stats_get,      dpoe_oam_eth_stats_reset) \
    xx(124,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_OPT_MON_TEMP,     0x021d, dpoe_oam_opt_mon_temp_get,      NULL) \
    xx(125,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_OPT_MON_VCC,      0x021e, dpoe_oam_opt_mon_vcc_get,       NULL) \
    xx(126,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_OPT_MON_BIAS,     0x021f, dpoe_oam_opt_mon_bias_get,      NULL) \
    xx(127,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_OPT_MON_TXPWR,    0x0220, dpoe_oam_opt_mon_txpwr_get,     NULL) \
    xx(128,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_OPT_MON_RXPWR,    0x0221, dpoe_oam_opt_mon_rxpwr_get,     NULL) \
    xx(129,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_Y,         0x0222, dpoe_oam_rx_frm_y_get,          dpoe_oam_rx_frm_y_reset) \
    xx(130,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_FRM_Y,         0x0223, dpoe_oam_tx_frm_y_get,          dpoe_oam_tx_frm_y_reset) \
    xx(131,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_OCT_G,         0x0224, dpoe_oam_tx_oct_g_get,          dpoe_oam_tx_oct_g_reset) \
    xx(132,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_OCT_Y,         0x0225, dpoe_oam_rx_oct_y_get,          dpoe_oam_rx_oct_y_reset) \
    xx(133,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_OCT_G,         0x0226, dpoe_oam_rx_oct_g_get,          dpoe_oam_rx_oct_g_reset) \
    xx(134,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_OCT_Y,         0x0227, dpoe_oam_tx_oct_y_get,          dpoe_oam_tx_oct_y_reset) \
    xx(135,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_UC,            0x0228, dpoe_oam_eth_stats_get,         dpoe_oam_eth_stats_reset) \
    xx(136,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_MC,            0x0229, dpoe_oam_eth_stats_get,         dpoe_oam_eth_stats_reset) \
    xx(137,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_BC,            0x022a, dpoe_oam_eth_stats_get,         dpoe_oam_eth_stats_reset) \
    xx(138,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_UC,            0x022b, dpoe_oam_eth_stats_get,         dpoe_oam_eth_stats_reset) \
    xx(139,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_MC,            0x022c, dpoe_oam_eth_stats_get,         dpoe_oam_eth_stats_reset) \
    xx(140,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_BC,            0x022d, dpoe_oam_eth_stats_get,         dpoe_oam_eth_stats_reset) \
    xx(141,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_CNT_NMB,          0x022e, dpoe_oam_cnt_nmb_get,           NULL) \
    xx(142,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_FRM_L2CP,      0x022f, dpoe_oam_rx_frm_l2cp_get,       dpoe_oam_rx_frm_l2cp_reset) \
    xx(143,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_OCT_L2CP,      0x0230, dpoe_oam_rx_oct_l2cp_get,       dpoe_oam_rx_oct_l2cp_reset) \
    xx(144,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_FRM_L2CP,      0x0231, dpoe_oam_tx_frm_l2cp_get,       dpoe_oam_tx_frm_l2cp_reset) \
    xx(145,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_OCT_L2CP,      0x0232, dpoe_oam_tx_oct_l2cp_get,       dpoe_oam_tx_oct_l2cp_reset) \
    xx(146,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_DISC_FRM_L2CP,    0x0233, dpoe_oam_disc_frm_l2cp_get,     dpoe_oam_disc_frm_l2cp_reset) \
    xx(147,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_DSIC_OCT_L2CP,    0x0234, dpoe_oam_disc_oct_l2cp_get,     dpoe_oam_disc_oct_l2cp_reset) \
    xx(148,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_TX_ERR,           0x0235, dpoe_oam_eth_stats_get,         dpoe_oam_eth_stats_reset) \
    xx(149,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RX_ERR,           0x0236, dpoe_oam_eth_stats_get,         dpoe_oam_eth_stats_reset) \
    xx(150,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_OVERLIMIT_DROP,   0x0237, dpoe_oam_eth_stats_get,         dpoe_oam_eth_stats_reset) \
    xx(151,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_ALARM_THRESHOLD,  0x0302, dpoe_oam_alarm_threshold_get,   dpoe_oam_alarm_threshold_set) \
    xx(200,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_KEY_EXCH_TIMER,   0x0401, dpoe_oam_key_exch_timer_get,    dpoe_oam_key_exch_timer_set) \
    xx(201,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_ENCRYPT_MODE,     0x0402, dpoe_oam_encrypt_mode_get,      dpoe_oam_encrypt_mode_set) \
    xx(202,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_RULE_CONF,        0x0501, dpoe_oam_rule_conf_get,         dpoe_oam_rule_conf_set) \
    xx(203,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_CUST_RULE_CONF,   0x0502, dpoe_oam_cust_rule_conf_get,    dpoe_oam_cust_rule_conf_set) \
    xx(204,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_CVLAN_TPID,       0x0503, dpoe_oam_cvlan_tpid_get,        dpoe_oam_cvlan_tpid_set) \
    xx(205,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_SVLAN_TPID,       0x0504, dpoe_oam_svlan_tpid_get,        dpoe_oam_svlan_tpid_set) \
    xx(206,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_IPMC_FWD_CONF,    0x0505, dpoe_oam_rule_pmc_fwr_cfg_get,  dpoe_oam_rule_pmc_fwr_cfg_set) \
    xx(207,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_I_TPID,           0x0506, NULL,                           NULL) \
    xx(208,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_B_TPID,           0x0507, NULL,                           NULL) \
    xx(209,OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_ADD_RULE,         0x0502, NULL,                           dpoe_oam_add_rule) \
    xx(210,OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_CLR_RULE,         0x0501, NULL,                           dpoe_oam_clr_rule) \
    xx(211,OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_DEL_RULE,         0x0503, NULL,                           dpoe_oam_del_rule) \
    xx(701,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_BCAST_RATE_LIMIT, 0x0601, dpoe_oam_bcast_limit_get,       dpoe_oam_bcast_limit_set) \
    xx(704,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_QUEUE_CIR,        0x0604, dpoe_oam_queue_cir_get,         dpoe_oam_queue_cir_set) \
    xx(705,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_FEC_MODE,         0x0605, dpoe_oam_fec_mode_get,          dpoe_oam_fec_mode_set) \
    xx(706,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_QUEUE_EIR,        0x0606, dpoe_oam_queue_eir_get,         dpoe_oam_queue_eir_set) \
    xx(707,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_QUEUE_MARKING,    0x0607, dpoe_oam_queue_mark_get,        dpoe_oam_queue_mark_set) \
    xx(708,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_QUEUE_LIMITER,    0x0608, dpoe_oam_queue_limiter_cap_get, dpoe_oam_queue_limiter_cap_set) \
    xx(709,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_COUPLING_FLAG,    0x0609, dpoe_oam_coupling_flag_get,     dpoe_oam_coupling_flag_set) \
    xx(710,OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_ENABLE_TRAFFIC,   0x0601, NULL,                           dpoe_oam_enable_traffic_set) \
    xx(711,OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_DISABLE_TRAFFIC,  0x0602, NULL,                           dpoe_oam_disable_traffic_set) \
    xx(712,OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_LOOPBACK_ENABLE,  0x0603, NULL,                           dpoe_oam_loopback_enable_set) \
    xx(713,OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_LOOPBACK_DISABLE, 0x0604, NULL,                           dpoe_oam_loopback_disable_set) \
    xx(714,OAM_DPOE_ACTION_BRANCH,  OAM_DPOE_LEAF_LASER_TX_OFF,     0x0605, NULL,                           dpoe_oam_laser_tx_off_set) \
    xx(715,OAM_DPOE_ATTRIB_BRANCH,  OAM_DPOE_LEAF_MEDIA_TYPE,       0x0822, dpoe_oam_media_type_get,        NULL) \
    xx(1001,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MACID,     0x0001, std_oam_macid_get,              NULL) \
    xx(1002,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_TRANSOK,   0x0002, dpoe_oam_eth_stats_get,         NULL) \
    xx(1003,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_SCOLLISION,0x0003, std_oam_scoll_get,              NULL) \
    xx(1004,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MCOLLISION,0x0004, std_oam_mcoll_get,              NULL) \
    xx(1005,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FRMRECV,   0x0005, dpoe_oam_eth_stats_get,         NULL) \
    xx(1006,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FCSERR,    0x0006, dpoe_oam_eth_stats_get,         NULL) \
    xx(1007,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_ALGERR,    0x0007, std_oam_alignerr_get,           NULL) \
    xx(1008,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_OCTTRANSOK,0x0008, dpoe_oam_eth_stats_get,         NULL) \
    xx(1009,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FRMDEFF,   0x0009, std_oam_frmdeff_get,            NULL) \
    xx(1010,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_LATECOLL,  0x000a, std_oam_latecoll_get,           NULL) \
    xx(1011,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FRMABORT,  0x000b, std_oam_frmabort_get,           NULL) \
    xx(1012,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FRMLOST1,  0x000c, std_oam_frmlost1_get,           NULL) \
    xx(1013,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_OCTRECVOK, 0x000e, dpoe_oam_eth_stats_get,         NULL) \
    xx(1014,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FRMLOST2,  0x000f, std_oam_frmlost2_get,           NULL) \
    xx(1015,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MCXMIT,    0x0012, dpoe_oam_eth_stats_get,         NULL) \
    xx(1016,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_BCXMIT,    0x0013, dpoe_oam_eth_stats_get,         NULL) \
    xx(1017,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FRMEXC,    0x0014, std_oam_frmexc_get,             NULL) \
    xx(1018,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MCRECV,    0x0015, dpoe_oam_eth_stats_get,         NULL) \
    xx(1019,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_BCRECV,    0x0016, dpoe_oam_eth_stats_get,         NULL) \
    xx(1019,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_INLENERR,  0x0017, std_oam_inlenerr_get,           NULL) \
    xx(1020,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_OUTLENERR, 0x0018, dpoe_oam_out_of_range_length_get,NULL) \
    xx(1021,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_GIANTERR,  0x0019, dpoe_oam_eth_stats_get,        NULL) \
    xx(1022,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MACSTAT,   0x001a, std_oam_macstat_get,            std_oam_macstat_set) \
    xx(1023,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MACADDR,   0x001d, std_oam_macaddr_get,            std_oam_macaddr_set) \
    xx(1024,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_COLLFRM,   0x001e, std_oam_collfrm_get,            NULL) \
    xx(1025,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_PHYTYPE,   0x0020, std_oam_phytype_get,            NULL) \
    xx(1026,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_SYMERR,    0x0023, std_oam_symerr_get,             NULL) \
    xx(1027,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_PHYADMIN,  0x0025, std_oam_phyadmin_get,           NULL) \
    xx(1028,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MEDAILAVAIL,0x0047, std_oam_medailavail_get,       NULL) \
    xx(1029,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_AUTONEGID, 0x004e, std_oam_autonegid_get,          NULL) \
    xx(1030,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_AUTONEGADM,0x004f, std_oam_autonegadmin_get,       NULL) \
    xx(1031,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_AUTONEGSIG,0x0050, std_oam_autonegsig_get,         NULL) \
    xx(1032,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_AUTONEGCFG,0x0051, std_oam_autonegcfg_get,         std_oam_autonegcfg_set) \
    xx(1033,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_AUTONEGLCTAB, 0x0052, std_oam_autoneglctab_get,    NULL) \
    xx(1034,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_AUTONEGADVTAB,0x0053, std_oam_autonegadtvab_get,   std_oam_autonegadtvab_set) \
    xx(1035,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_AUTONEGRECVTAB,0x0054, std_oam_autonegrecvtab_get, NULL) \
    xx(1036,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_AUTONEGCFGLCSSAB,0x0055, std_oam_autoneglcsab_get, NULL) \
    xx(1037,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_AUTONEGCFGADVSSAB,0x0056, std_oam_autonegadvsab_get,std_oam_autonegadvsab_set) \
    xx(1038,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_AUTONEGCFGRECVSSAB,0x0057, std_oam_autonegrecvsab_get,         NULL) \
    xx(1039,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_DUPLEX,    0x005a, std_oam_duplex_get,             std_oam_duplex_set) \
    xx(1040,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MACCTRL,   0x005d, std_oam_macctrl_get,            NULL) \
    xx(1041,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MACXMIT,   0x005e, std_oam_macxmit_get,            NULL) \
    xx(1042,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MACRECV,   0x005f, std_oam_macrecv_get,            NULL) \
    xx(1043,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_NOTSUPPORTOP,0x0060, std_oam_notspop_get,          NULL) \
    xx(1044,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_PAUSEALLOW,0x0061, std_oam_pauseallow_get,         std_oam_pauseallow_set) \
    xx(1045,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_PAUSEXMIT, 0x0062, dpoe_oam_eth_stats_get,          NULL) \
    xx(1046,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_PAUSERECV, 0x0063, dpoe_oam_pause_mac_ctl_frame_rev_get,          NULL) \
    xx(1067,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_UNDEF,     0x00e9, std_oam_undef_get,          NULL) \
    xx(1047,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MPCPXMIT,  0x0118, std_oam_mpcp_stats_get,           NULL) \
    xx(1048,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_MPCPRECV,  0x0119, std_oam_mpcp_stats_get,           NULL) \
    xx(1049,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_DSCWINSENT,0x0120, std_oam_dscwinsent_get,         NULL) \
    xx(1050,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_DSCTOUT,   0x0121, std_oam_dsctout_get,            NULL) \
    xx(1051,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FECCB,     0x0124, std_oam_feccb_get,              NULL) \
    xx(1052,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FECUCB,    0x0125, std_oam_fecucb_get,             NULL) \
    xx(1053,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FECAB,     0x0139, std_oam_fecab_get,              NULL) \
    xx(1054,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_FECMD,     0x013a, std_oam_fecmode_get,            std_oam_fecmode_set) \
    xx(1056,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_TXREGACK,  0x013c, std_oam_mpcp_stats_get,           NULL) \
    xx(1057,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_TXREGREQ,  0x013e, std_oam_mpcp_stats_get,           NULL) \
    xx(1058,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_TXREPORT,  0x013f, std_oam_mpcp_stats_get,           NULL) \
    xx(1059,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_RXGATE,    0x0140, std_oam_mpcp_stats_get,             NULL) \
    xx(1061,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_UNKNOWN1,  0x00F9, dpoe_unknown1_get,              NULL) \
    xx(1060,OAM_INTF_COUNTERS_MIB_BRANCH,  OAM_DPOE_LEAF_RXREG,     0x0142, std_oam_mpcp_stats_get,              NULL) \
    xx(1062,OAM_DPOE_ATTRIB_BRANCH,        OAM_DPOE_LEAF_PORT_CFG,  0x0114, dpoe_oam_port_cfg_get,          dpoe_oam_port_cfg_set) \
    xx(1063,OAM_DPOE_ATTRIB_BRANCH,        OAM_DPOE_LEAF_QUEUE_CFG, 0x0115, dpoe_oam_queue_cfg_get,         dpoe_oam_queue_cfg_set)\
    xx(1065,OAM_DPOE_ATTRIB_BRANCH,        OAM_DPOE_LEAF_ESAFE_CFG, 0x0116, dpoe_oam_esafe_cfg_get,         dpoe_oam_esafe_cfg_set )\
    xx(1064,OAM_DPOE_ATTRIB_BRANCH,        OAM_DPOE_LEAF_ENABLE_EMTA_EDVA, 0x0117, NULL,                    dpoe_oam_enable_emta_edva_set)\
    xx(1066,OAM_INTF_CONTROL_MIB_BRANCH,   OAM_DPOE_LEAF_AUTO_NEG_ADMIN_CTRL,0x000C, NULL,dpoe_oam_auto_neg_admin_ctrl_set)\

typedef enum {
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC) LEAF=LEAF_VAL,
    DPOE_OAM_BUILD_RESP
} dpoe_oam_leaf_e;

/* if any set oam without length, list here*/
#define DPOE_OAM_ODD_SET_OAM_LISTS  \
    { OAM_DPOE_ACTION_BRANCH, OAM_DPOE_LEAF_RESET_ONU }


typedef struct {
    oam_boolean     valid;
    oam_uint8       len;

    union {
        oam_pdu_dpoe_obj_onu_t      onu;
        oam_pdu_dpoe_obj_pon_t      pon;
        oam_pdu_dpoe_obj_llid_t     llid;
        oam_pdu_dpoe_obj_uni_t      uni;
        oam_pdu_dpoe_obj_queue_t    queue;
    } obj;
} dpoe_oam_mgmt_obj_t;

typedef oam_uint32 (*dpoe_oam_get_handler_t)(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8   *recV,
    oam_uint32 *proc_recv_len,
    oam_uint8   *out);

typedef oam_uint32 (* dpoe_oam_set_handler_t)(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32    recv_len,
    oam_uint8    *out);


#define OAM_DPOE_FUNCTION_ENTER(obj)  \
    OAM_ORG_DEBUG("%s, obj is %s, len = %d \n", __FUNCTION__, obj.valid ? "Valid" : "Invalid", obj.len);

typedef struct {
    oam_uint8               branch;
    oam_uint16              leaf;
    dpoe_oam_get_handler_t  get_func;
    dpoe_oam_set_handler_t  set_func;
} dpoe_oam_handle_t;

#define OAM_DPOE_MAX_STD_HANDLER  100
typedef struct {
    oam_oui_t           oui;
    oam_uint32          num;
    dpoe_oam_handle_t   handlers[OAM_DPOE_MAX_STD_HANDLER];
} g_dpoe_oam_handle_t;

void handle_dpoe_oams(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len
);

oam_uint32 dpoe_oam_onu_id_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_fw_info_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_chip_info_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_data_manuf_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_manuf_info_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_vn_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_max_llid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_uni_num_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_pon_num_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_pkt_buff_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_report_thresh_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_report_thresh_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32    proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_llid_fwd_state_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_frame_rate_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_frame_rate_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32    proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_manuf_org_name_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_fw_mfg_time_vary_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_port_type_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_model_name_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_hw_ver_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_reset_onu(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

void dpoe_bridge_init();
oam_uint32 dpoe_oam_dynmac_size_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);


oam_uint32 dpoe_oam_dynmac_table_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_aging_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_aging_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_stcmac_table_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_autoneg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_autoneg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_srcaddr_admctrl_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_srcaddr_admctrl_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_uni_minmac_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_uni_minmac_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_uni_maxmac_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_uni_maxmac_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_onu_maxmac_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_onu_maxmac_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_length_err_disc_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_length_err_disc_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_unkn_flooding_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_unkn_flooding_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_local_switch_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_local_switch_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_llid_queue_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_llid_queue_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_fw_name_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_mac_full_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_mac_full_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_clr_dyn_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_add_dyn_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_del_dyn_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_clr_sta_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_add_sta_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_del_sta_mac(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_cfg_mc_llid(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_queue_delay_thr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_queue_delay_thr_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_queue_delay_val_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_queue_delay_val_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_frm_dropped_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_frm_dropped_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_oct_dropped_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_oct_dropped_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_oct_delayed_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_oct_delayed_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_usoct_unused_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_usoct_unused_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_temp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_temp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_vcc_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_vcc_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_bias_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_bias_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_txpwr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_txpwr_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_rxpwr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_rxpwr_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_frm_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_frm_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_frm_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_frm_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_g_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_g_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_g_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_g_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_cnt_nmb_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_frm_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_frm_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_frm_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_frm_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_txpwr_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_rxpwr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_opt_mon_rxpwr_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_frm_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_frm_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_frm_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_frm_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_g_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_g_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_g_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_g_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_y_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_y_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_uni_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_uni_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_mc_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_mc_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_bc_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_bc_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_uni_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_uni_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_mc_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_mc_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_bc_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_bc_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_cnt_nmb_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_frm_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_frm_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_oct_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_frm_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_frm_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_oct_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_disc_frm_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_disc_frm_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_disc_oct_l2cp_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_disc_oct_l2cp_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_err_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_tx_err_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_err_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rx_err_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_alarm_threshold_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_alarm_threshold_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_key_exch_timer_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_key_exch_timer_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_encrypt_mode_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_encrypt_mode_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rule_conf_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_rule_conf_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_cust_rule_conf_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_cust_rule_conf_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_svlan_tpid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_svlan_tpid_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_cvlan_tpid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_cvlan_tpid_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_add_rule(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_clr_rule(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_bcast_limit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_bcast_limit_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_queue_cir_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_queue_cir_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_fec_mode_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_fec_mode_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_queue_eir_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_queue_eir_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_queue_mark_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_queue_mark_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_queue_limiter_cap_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_queue_limiter_cap_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_coupling_flag_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_coupling_flag_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_media_type_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_enable_traffic_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_disable_traffic_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_loopback_enable_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_loopback_disable_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_laser_tx_off_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_status dpoe_index2llid(oam_uint8 index, oam_uint16 *llid);
oam_status dpoe_llid2index(oam_uint8 *index, oam_uint16 llid);
void dpoe_llid_init();
oam_uint32 dpoe_oam_build_header(
    oam_uint8 *oui,
    oam_llid_t  llid,
    oam_uint8 *buf,
    oam_uint8  opcode);
oam_uint32 dpoe_oam_fec_mode_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_fec_mode_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 std_oam_macid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 std_oam_transok_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_scoll_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_mcoll_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_frm_recv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_fcserr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_alignerr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_octtransok_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_frmdeff_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_latecoll_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_frmabort_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_frmlost1_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_octok_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_frmlost2_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_mcxmit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_bcxmit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_frmexc_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_mcrecv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_bcrecv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_inlenerr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_outlenerr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_giantlenerr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_macstat_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_macstat_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_macaddr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_macaddr_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_collfrm_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_phytype_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_symerr_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_phyadmin_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_medailavail_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegadmin_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegsig_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegcfg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegcfg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autoneglctab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegadtvab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegadtvab_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegrecvtab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autoneglcsab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegadvsab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegadvsab_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_autonegrecvsab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_duplex_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_duplex_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_macctrl_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_macxmit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_macrecv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_notspop_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_pauseallow_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_pauseallow_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_pausexmit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_pauserecv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_mpcpxmit_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_mpcprecv_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_dscwinsent_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_dsctout_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_feccb_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_fecucb_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_fecab_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_fecmode_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 std_oam_fecmode_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_port_cfg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_port_cfg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_queue_cfg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_oam_queue_cfg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
oam_uint32 dpoe_unknown1_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

void oam_dpoe_llid_reg(oam_llid_t llid, oam_uint16 index);
void oam_dpoe_llid_dereg(oam_llid_t llid, oam_uint16 index, oam_uint32 dereg_cause);

void dpoe_oam_send(
    oam_llid_t llid,
    oam_uint8 *buf,
    oam_int32 len
);

oam_uint32 dpoe_oam_eth_stats_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_eth_stats_reset(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 std_oam_mpcp_stats_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_multicast_llid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_clear_counters_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_epon_mode_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_del_rule(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

void dpoe_oam_build_link_event(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8 event_code);

oam_uint32 dpoe_oam_software_bundle_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_enable_emta_edva_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_uni_mac_learn_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_uni_mac_learn_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 std_oam_undef_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_fw_mfg_time_vary_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32    proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_esafe_cfg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_esafe_cfg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_auto_neg_admin_ctrl_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_pause_mac_ctl_frame_rev_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_rule_pmc_fwr_cfg_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_rule_pmc_fwr_cfg_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_rx_frame_green_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_out_of_range_length_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_tx_frame_1519_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_tx_frame_1518_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_tx_frame_1023_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_tx_frame_511_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_tx_frame_255_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_tx_frame_127_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_tx_frame_64_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_rx_frame_1519_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_rx_frame_1518_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_rx_frame_1023_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_rx_frame_511_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_rx_frame_255_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_rx_frame_127_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_rx_frame_64_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);

oam_uint32 dpoe_oam_count_rx_frame_short_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out);
#endif

#endif /* _OAM_DPOE_H_ */

