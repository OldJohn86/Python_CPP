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
#ifndef __OAM_KT_H_
#define __OAM_KT_H_

#ifdef HAVE_KT_OAM

#include "oam_kt_pdu.h"
/* KT Extended Object Class */

#define OAM_KT_ATTRIB_BRANCH                0xa7
#define OAM_KT_ACTION_BRANCH                0xa9

#define OAM_KT_SYS_TIME_YEAR_MAX                9999

#define OAM_KT_SYS_TIME_MONTH_MIN                1
#define OAM_KT_SYS_TIME_MONTH_MAX                12

#define OAM_KT_SYS_TIME_DAY_MIN                1
#define OAM_KT_SYS_TIME_DAY_MAX                31

#define OAM_KT_SYS_TIME_HOUR_MAX                23

#define OAM_KT_SYS_TIME_MINUTE_MAX                59

#define OAM_KT_OVER_VOLTAGE_RAISE        1
#define OAM_KT_OVER_VOLTAGE_RELEASE      2


/* KT Extended MO Instance */
//efine OAM_KT_LEAF_CODE_ONU_PORT          OAM_CTC_LEAF_CODE_ONU_PORT

typedef struct {
    oam_pdu_hdr_t           hdr;
    unsigned char           oui[OAM_OUI_LEN];
    unsigned char           data[1];
} __attribute__((packed)) oam_pdu_kt_pdu_t;


typedef struct {
        ca_uint8       port;
        ca_uint8       enable;
        ca_uint16      bc_threshold;
        ca_uint16      mc_threshold;
        ca_uint16      uuc_threshold;
} strom_ctrl_port_threshold_t;


typedef struct
{
    ca_uint8 status; // enable:1, disable:0
    ca_uint8 mac[6];

}ca_onu_mac_filter_ctrl_t;

typedef struct {
    oam_uint8  admin;       /* enable or disable link event */
    oam_uint16 rj45_failure;
    oam_uint16 cnt_up2down;
}ca_uni_link_status_stats;

typedef struct{
    oam_pdu_hdr_t  pdu_hdr;
    oam_uint8 oui[3];
    oam_uint8 extCode;
    oam_uint8 event_type ;
    oam_uint8 len ;
    oam_uint8 port;
    oam_uint8 link_status;
}oam_uni_link_event_t;

typedef struct {
    oam_kt_var_cont_t hdr;
    oam_uint16  down_rate;
    oam_uint16  up_rate;
    oam_uint16  start_hour;
    oam_uint16  start_min;
    oam_uint16  end_hour;
    oam_uint16  end_min;
}oam_guarddog_reboot_t;


/******************************************************************************/
/*                          KT STUFF MSG                                        */
/******************************************************************************/

#define  KT_PRV_OAM_BUILD_RESP \
    xx(002, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_ETH_DS_SHAPING,          0x0001, kt_prv_oam_ds_shaping_get, kt_prv_oam_ds_shaping_set) \
    xx(003, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_ETH_MAC_LIMIT,           0x0002, kt_prv_oam_eth_mac_limit_get, kt_prv_oam_eth_mac_limit_set) \
    xx(004, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_BLOCK,                   0x0003, kt_prv_oam_onu_block_get, kt_prv_oam_onu_block_set) \
    xx(005, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_QUEUE_DROP_COUNTER,      0x0005, kt_prv_oam_onu_queue_drop_counter_get,0) \
    xx(006, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_DIAGNOSTIC,              0x0004, kt_prv_oam_diagnostics_get, 0) \
    xx(007, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_PORT_COUNTER,            0x0011, 0, 0) \
    xx(008, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_ETH_RSTP,                0x0012, kt_prv_oam_eth_rstp_get, kt_prv_oam_eth_rstp_set) \
    xx(009, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_LOOP_DETECT,             0x0013, kt_prv_oam_loop_detect_get, kt_prv_oam_loop_detect_set) \
    xx(010, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_MPCP_COUNTER,            0x0014, kt_prv_oam_onu_mpcp_counter_get, 0) \
    xx(011, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_STATIC_MAC,              0x0015, kt_prv_oam_static_mac_get, kt_prv_oam_static_mac_set) \
    xx(012, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_OPM_ALARM,               0x0016, kt_prv_oam_opm_alarm_status_get, 0) \
    xx(013, OAM_KT_ACTION_BRANCH, OAM_KT_LEAF_CODE_ONU_COUNTER_CLEAR,           0x0001, 0, kt_prv_oam_counter_clear) \
    xx(014, OAM_KT_ACTION_BRANCH, OAM_KT_LEAF_CODE_ONU_DEFAULT_RESTORE,         0x0002, 0, kt_prv_oam_default_restore) \
    xx(015, OAM_KT_ACTION_BRANCH, OAM_KT_LEAF_CODE_ONU_TX_POWER_OFF,            0x0003, 0,kt_prv_oam_tx_power_off_set) \
    xx(016, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_VOL_ALARM,               0x0017, kt_prv_oam_vol_alarm_status_get, 0)  \
    xx(017, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_DHCP_OPTION,             0x0018, kt_prv_oam_dhcp_option_cfg_get, kt_prv_oam_dhcp_option_cfg_set) \
    xx(018, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_MAC_TABLE,               0x0019, kt_prv_oam_onu_mac_table_get, 0) \
    xx(019, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_DYNAMIC_MAC_TABLE_CLR,   0x0020, 0 ,kt_prv_oam_dynamic_mac_table_clr) \
    xx(020, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_OLT_SEN_EXCEED_ALARM,        0x001A, kt_prv_oam_olt_sen_exceed_alarm_get ,kt_prv_oam_olt_sen_exceed_alarm_set) \
    xx(021, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_OPM_DDM,                 0x001B, kt_prv_oam_onu_opm_ddm_get ,0) \
    xx(022, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_TIME,                    0x001C, 0 ,kt_prv_oam_onu_time_set) \
    xx(024, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_IPV6_OPT254_ENABLE,          0x0021, kt_prv_oam_ipv6_opt254_get, kt_prv_oam_ipv6_opt254_set) \
    xx(025, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_PER_PORT_MAC_ENTRIES_GET,    0x0022, kt_prv_oam_per_port_fdb_entries_get, 0) \
    xx(026, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_PER_PORT_STATISTICS_GET,     0x001D, kt_prv_oam_per_port_statistics_get, 0) \
    xx(027, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_PER_PORT_RTCT_GET,           0x0023, kt_prv_oam_per_port_rtct_get, 0) \
    xx(029, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_PER_PORT_STORM_CONTROL,      0x0024, kt_prv_oam_per_port_storm_control_get ,kt_prv_oam_per_port_storm_control_set ) \
    xx(030, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_PER_PORT_STATISTICS_CLR,     0x001E, 0 ,kt_prv_oam_per_port_statistics_clr) \
    xx(031, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_STORM_CONTROL_STATUS,        0x0025, kt_prv_oam_storm_control_status_get, kt_prv_oam_storm_control_status_set) \
    xx(032, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_MAC_FILTER_SET,              0x0026, kt_prv_oam_mac_filter_get, kt_prv_oam_mac_filter_set) \
    xx(033, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_UNI_LINK_STATUS_CNT,         0x0027, kt_prv_oam_uni_link_status_cnt_get, kt_prv_oam_uni_link_status_cnt_set) \
    xx(034, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_UNI_LINK_STATUS,             0x0029, kt_prv_oam_uni_link_status_get, 0) \
    xx(035, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_CRITICAL_EVENT,          0x0028, kt_prv_oam_onu_reserve_get, kt_prv_oam_onu_reserve_set) \
    xx(036, OAM_KT_ACTION_BRANCH, OAM_KT_LEAF_CODE_ONU_POWER_RESET,            0x0007, 0,kt_prv_oam_power_reset) \
    xx(037, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_ONU_DDM_THRESHOLD,           0x001f, kt_prv_oam_onu_ddm_threshold_get, kt_prv_oam_onu_ddm_threshold_set) \
    xx(038, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_FW_UPGRADE_GUARDDOG_REBOOT,  0x002A, 0, kt_prv_oam_guarddog_reboot) \
    xx(039, OAM_KT_ATTRIB_BRANCH, OAM_KT_LEAF_CODE_REGULAR_GUARDDOG_REBOOT,     0x002B, 0, kt_prv_oam_guarddog_reboot) \

typedef enum{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC) LEAF=LEAF_VAL,
KT_PRV_OAM_BUILD_RESP
 }kt_prv_oam_leaf_e;

typedef oam_uint32 (*onu_kt_prv_oam_get_rountine_t)(oam_uint8 *pOut);

typedef oam_uint32 (*onu_kt_prv_oam_set_rountine_t)(
    oam_uint8 *pRecv,
    oam_uint8 *pOut);

typedef struct {
    oam_uint8 branch;
    oam_uint16 leaf;
    onu_kt_prv_oam_get_rountine_t get_func;
    onu_kt_prv_oam_set_rountine_t set_func;
}onu_kt_prv_oam_xlat_rountine_t;

typedef oam_uint32 (*kt_eth_func_t)(oam_uint8 *bufBase, oam_uint8 portId);

void handle_kt_prv_oams(
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 len);

void
kt_oam_send(
        oam_uint32 port,
        oam_llid_t llid,
        oam_uint8 *buf,
        oam_int32 len
        );

void kt_prv_oam_init();

#endif
#endif /* __OAM_KT_H_ */
