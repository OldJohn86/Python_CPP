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
#ifndef _OAM_ZTE_H_
#define _OAM_ZTE_H_

#ifdef ZTE_SPECIFIC

#define ZTE_VLAN_MAX 4094

#define ZTE_OAM_SUPPORT 1
#define ZTE_OAM_UNSUPPORT 2

/*CTC oui zte oam table*/
#define  CTC_ZTE_OAM_BUILD_RESP \
    xx(002, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_ALARM_REPORT, 0x8000, 0, 0) \
    xx(003, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_ALARM_CONFIG, 0x8001, \
            zte_oam_alarm_config_get, zte_oam_alarm_config_set) \
    xx(004, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_MAC_LIMIT, 0x8003, \
            zte_oam_mac_limit_get, zte_oam_mac_limit_set) \
    xx(005, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_MAC_AGING, 0x8004, \
            zte_oam_mac_aging_get, zte_oam_mac_aging_set) \
    xx(006, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PORT_MAC_FILTER, 0x8005, \
            zte_oam_port_mac_get, zte_oam_port_mac_set) \
    xx(007, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PORT_MAC_BIND, 0x8006, \
            zte_oam_port_mac_get, zte_oam_port_mac_set) \
    xx(008, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PORT_MAC_STATIC, 0x8007, \
            zte_oam_port_mac_get, zte_oam_port_mac_set) \
    xx(009, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PORT_ISOLATION, 0x8008, \
            zte_oam_port_isolation_get, zte_oam_port_isolation_set) \
    xx(010, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PORT_STATISTICS, 0x8010, \
            zte_oam_port_statistics_get, 0) \
    xx(011, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_MAC_POOL_QUERY, 0x8012, \
            zte_oam_mac_pool_query, 0) \
    xx(012, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PON_DN_SHAPING, 0x8013, \
            zte_oam_pon_dn_shaping_get, zte_oam_pon_dn_shaping_set) \
    xx(013, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_VOQ_BUFFER, 0x8014, 0, \
            zte_oam_voq_buffer_set) \
    xx(014, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_BUFF_MGT_CAP, 0x8015, \
            zte_oam_buff_mgt_cap_get, 0) \
    xx(015, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PORT_FLUX_STAT_CTRL, 0x8016, \
            zte_oam_flux_stat_ctrl_get, zte_oam_flux_stat_ctrl_set) \
    xx(016, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PORT_STAT_RESET, 0x8017,  \
            0, zte_oam_port_stat_reset) \
    xx(017, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PORT_FLUX_STATISTICS, 0x8018, \
            zte_oam_port_flux_stat_get, 0) \
    xx(018, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_TX_LASER_CTRL, 0x801A, \
            0, zte_oam_tx_laser_ctrl_set) \
    xx(019, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PON_PORT_STAT1, 0x801B, \
            zte_oam_pon_port_stat_get, 0)  \
    xx(020, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PON_PORT_STAT2, 0x801C,  \
            zte_oam_pon_port_stat_get, 0)  \
    xx(021, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_ROUGUE_ONU_EXCL_ABILITY, 0x801D,  \
            zte_oam_rougue_onu_excl_ability_get, zte_oam_rougue_onu_excl_ability_set)  \
    xx(022, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PPPOE_DISCOVERY_OP, 0x80A3,   \
            0, zte_oam_pppoe_simul_test) \
    xx(023, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_PPPOE_DISCOVERY_STATE, 0x80A4, \
            zte_oam_pppoe_simul_result_get, 0) \
    xx(024, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_REPORT_MATCHED_MAC, 0x6000, 0, 0) \
    xx(025, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_OPERATE_MAC_ADDRESS_POOL, 0x6001, \
            zte_oam_hg_mac_pool_get, zte_oam_hg_mac_pool_set) \
    xx(026, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_OPERATE_MANAGEMENT_VLAN, 0x6002, \
            zte_oam_hg_mgmt_vlan_get, zte_oam_hg_mgmt_vlan_set) \
    xx(027, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_OPERATE_HG_REPORT_STATE, 0x6003, \
            zte_oam_hg_report_state_get, zte_oam_hg_report_state_set) \
    xx(028, OAM_CTC_ATTRIB_BRANCH, OAM_ZTE_LEAF_CODE_ONU_SN, 0x0001,\
          zte_oam_sn_get, NULL) \
    xx(029, OAM_CTC_ATTRIB_BRANCH, OAM_ZTE_LEAF_CODE_ONU_FRM_VER, 0x0002,\
          zte_oam_fw_get, NULL) \
    xx(030, OAM_CTC_ATTRIB_BRANCH, OAM_ZTE_LEAF_CODE_ONU_CHIP_ID, 0x0003,\
            zte_oam_chipid_get, NULL) \
    xx(031, OAM_CTC_ATTRIB_BRANCH, OAM_ZTE_LEAF_CODE_ONU_VLAN, 0x0021, \
            zte_oam_onu_vlan_get, zte_oam_onu_vlan_set) \
    xx(032, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_IPOE_EMULATION_SET, 0x8026,  \
            0, zte_oam_ipoe_simul_test) \
    xx(033, OAM_CTC_ATTRIB_BRANCH, ZTE_OAM_LEAF_IPOE_EMULATION_GET, 0x8027, \
            zte_oam_ipoe_simul_result_get, 0) \
 
typedef enum {
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC) LEAF=LEAF_VAL,
    CTC_ZTE_OAM_BUILD_RESP
} ctc_zte_oam_leaf_e;

void oam_build_zte_extend_alarm_raised(oam_uint8 port, oam_uint16 alarm_id );
#endif/*END_ZTE_SPECIFIC*/
#endif /* _OAM_ZTE_H_ */
