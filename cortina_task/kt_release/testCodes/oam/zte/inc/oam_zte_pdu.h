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
#ifndef _OAM_ZTE_PDU_H_
#define _OAM_ZTE_PDU_H_
#include "oam_types.h"
#include "oam_ctc_pdu.h"

#ifdef ZTE_SPECIFIC

#define   ZTE_MAX_MAC_PER_SESSION   20

/*57-17 value 0x21 */
#define OAM_ZTE_ATTRIB_NOT_SUPPORTED        OAM_ATTRIB_NOT_SUPPORTED
#define OAM_ZTE_VAR_SET_OK                  OAM_VAR_SET_OK
#define OAM_ZTE_VAR_SET_BAD_PARAM           OAM_VAR_SET_BAD_PARAM
#define OAM_ZTE_VAR_SET_NO_RES              OAM_VAR_SET_NO_RES

#define TW_OAM_RESULT_OK                    0x80
#define TW_OAM_RESULT_TOO_BIG               0xca
#define TW_OAM_RESULT_NO_SUCH_NAME          0xcb
#define TW_OAM_RESULT_BAD_VALUE             0xcc
#define TW_OAM_RESULT_READ_ONLY             0xcd
#define TW_OAM_RESULT_GENERAL_ERROR         0xce
#define TW_OAM_RESULT_NO_ACCESS             0xcf
#define TW_OAM_RESULT_WRONG_TYPE            0xd0
#define TW_OAM_RESULT_WRONG_LENGTH          0xd1
#define TW_OAM_RESULT_WRONG_ENCODING        0xd2
#define TW_OAM_RESULT_WRONG_VALUE           0xd3
#define TW_OAM_RESULT_NO_CREATION           0xd4
#define TW_OAM_RESULT_INCONSISTENT_VALUE    0xd5
#define TW_OAM_RESULT_RESOURCE_UNVAILABLE   0xd6
#define TW_OAM_RESULT_COMMIT_FAILED         0xd7
#define TW_OAM_RESULT_UNDO_FAILED           0xd8
#define TW_OAM_RESULT_AUTHORIZATION_ERROR   0xd9

#define TW_OAM_RESULT_NEXT_NULL             0xc0

#define ZTE_VLAN_MODE_STACK                 0xA0
#define ZTE_VLAN_MODE_MDUTRUNK              0x13
#define ZTE_VLAN_MODE_HYBRID                0x14

#define ZTE_VLAN_MODE_TRUNK_MAX             16

typedef enum {
    OAM_ZTE_EXT_CTC_HANDLED =  0,
    OAM_ZTE_EXT_CTC_NOT_HANDLED,
    OAM_ZTE_EXT_CTC_HANDLED_OVER
} zte_handle_ctc_code_t;

/*ZTE OUI OAM DEFINE*/
typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           opcode;
    oam_uint8           type;
    oam_uint8           port;
    oam_uint8           data[1];
} __attribute__((packed)) oam_pdu_zte_t;


#define OAM_PDU_ZTE_VAR_REQ                 0x01
#define OAM_PDU_ZTE_VAR_RESP                0x02
#define OAM_PDU_ZTE_SET_REQ                 0x03
#define OAM_PDU_ZTE_SET_RESP                0x04


#define ZTE_OAM_EXT_BRANCH_PORT             0x10
#define ZTE_OAM_EXT_BRANCH_ONU              0x0f


#define ZTE_OAM_EXT_LEAF_PORT_ATTR      0x0204
#define ZTE_OAM_EXT_LEAF_GET_PORT_WORKING_ATTR  0x0205

#define ZTE_OAM_EXT_LEAF_ONU_SERIAL                    0x104
#define ZTE_OAM_EXT_LEAF_ONU_CONFIGURATION            0x0106
#define ZTE_OAM_EXT_LEAF_REMOTE_CONNECT          0x107
#define ZTE_OAM_EXT_LEAF_ZTE_OAM_CAPABILITY     0x101
#define ZTE_OAM_EXT_LEAF_TYPEC_PROTECT_CONFIG          0x109
#define ZTE_OAM_EXT_LEAF_TYPEC_PROTECT_STATUS     0x10A

typedef enum {
    ZTE_OAM_PORT_MODE_AUTO =  1,
    ZTE_OAM_PORT_MODE_UNKOWN =  1,
    ZTE_OAM_PORT_MODE_10M_HALF,
    ZTE_OAM_PORT_MODE_10M_FULL,
    ZTE_OAM_PORT_MODE_100M_HALF,
    ZTE_OAM_PORT_MODE_100M_FULL,
    ZTE_OAM_PORT_MODE_1000M_FULL
} zte_oam_port_mode_e;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       mode;
} __attribute__((packed)) oam_zte_onu_port_attr;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8
    k1;        /*Bit3-Bit0表示发出请求的信道号： 0000-protection channel；0001-working channel
    发送的K1字节指示本地最小信道号上的最高优先级请求。
    如： 1100 0001表示工作通道出现SF*/
    oam_uint8       k2;       /*Bit7-Bit4指示信道号，同K1的Bit3-Bit0定义。
    Bit3表示保护架构模式（1表示1：n架构；0表示1+1架构），
    Bit2-Bit0用来指示切换模式（100-单向，101双向）
    目前K2低4BIT缺省为0xd（1101*/
} __attribute__((packed)) oam_zte_onu_typec_t;

/* Alarm ID */
#define ZTE_OAM_SWITCH_ERROR            0x0001
#define ZTE_OAM_LINK_UP                 0x1001
#define ZTE_OAM_LINK_DOWN               0x1002
#define ZTE_OAM_AUTO_NEG_FAIL           0x1003
#define ZTE_OAM_LOOP_DETECT             0x1004
#define ZTE_OAM_LOOP_RECOVER            0x1005
#define ZTE_OAM_TEMP_NORMAL             0x4001
#define ZTE_OAM_TEMP_HIGH_ALARM         0x4002
#define ZTE_OAM_TEMP_LOW_ALARM          0x4003
#define ZTE_OAM_TEMP_HIGH_WARNING       0x4004
#define ZTE_OAM_TEMP_LOW_WARNING        0x4005
#define ZTE_OAM_VOLT_NORMAL             0x4006
#define ZTE_OAM_VOLT_HIGH_ALARM         0x4007
#define ZTE_OAM_VOLT_LOW_ALARM          0x4008
#define ZTE_OAM_VOLT_HIGH_WARNING       0x4009
#define ZTE_OAM_VOLT_LOW_WARNING        0x400a
#define ZTE_OAM_BIAS_NORMAL             0x400b
#define ZTE_OAM_BIAS_HIGH_ALARM         0x400c
#define ZTE_OAM_BIAS_LOW_ALARM          0x400d
#define ZTE_OAM_BIAS_HIGH_WARNING       0x400e
#define ZTE_OAM_BIAS_LOW_WARNING        0x400f
#define ZTE_OAM_TX_POWER_NORMAL         0x4010
#define ZTE_OAM_TX_POWER_HIGH_ALARM     0x4011
#define ZTE_OAM_TX_POWER_LOW_ALARM      0x4012
#define ZTE_OAM_TX_POWER_HIGH_WARNING   0x4013
#define ZTE_OAM_TX_POWER_LOW_WARNING    0x4014
#define ZTE_OAM_RX_POWER_NORMAL         0x4015
#define ZTE_OAM_RX_POWER_HIGH_ALARM     0x4016
#define ZTE_OAM_RX_POWER_LOW_ALARM      0x4017
#define ZTE_OAM_RX_POWER_HIGH_WARNING   0x4018
#define ZTE_OAM_RX_POWER_LOW_WARNING    0x4019

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint16      alarm_id;
    oam_uint8       reserved[3];
} __attribute__((packed)) oam_zte_onu_alarm_report;


#define ZTE_OAM_ALARM_CONFIG_DISABLE               0
#define ZTE_OAM_ALARM_CONFIG_ENABLE                1

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       enable;
} __attribute__((packed)) oam_zte_onu_alarm_config;

typedef struct {
    oam_uint8       oui[3];
    oam_uint8       opCode ;
} __attribute__((packed))  oam_zte_onu_alarm_info_struct;



#define ZTE_OAM_MAC_UNLIMIT                     0xffff
#define ZTE_OAM_MAC_LEARNING_DISABLE            0
typedef struct {
    oam_var_cont_t  hdr;
    oam_uint16      number;
} __attribute__((packed)) oam_zte_onu_mac_limit;


typedef struct {
    oam_var_cont_t  hdr;
    oam_uint32      result;
    oam_uint16      len;
    oam_uint16      sessionId;
    oam_uint16      seqNo;
    oam_uint16      totalNum;
} __attribute__((packed)) oam_zte_onu_port_mac_pool_query;

typedef struct {
    oam_uint8       extBranch;
    oam_uint16      leaf;
    oam_uint8       result;
    oam_uint16      len;
    oam_uint16      sessionId;
    oam_uint16      seqNo;
    oam_uint16      totalEntryNum;
} __attribute__((packed)) oam_zte_onu_port_mac_pool_header;

typedef struct {
    oam_uint32      blockEntryNum;
    oam_uint16      reserved;
    oam_uint16      attr;
    oam_uint8       data[0];  /*max mac address is 100*/
} __attribute__((packed)) oam_zte_onu_port_mac_pool_block;

#define ZTE_FDB_TYPE_DYNAMIC   0
#define ZTE_FDB_TYPE_STATIC    0x1000
#define ZTE_FDB_TYPE_MULTICAST  0x2000


#define ZTE_OAM_MAC_AGING_MIN                     1
#define ZTE_OAM_MAC_AGING_MAX                     3825

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint32      time;
} __attribute__((packed)) zte_oam_onu_mac_aging;


typedef struct {
    oam_uint16      vlan;
    oam_uint8       mac[OAM_MACADDR_LEN];
} __attribute__((packed)) zte_oam_onu_mac_entry_t;

#define ZTE_MAX_ENTRIES_PER_CONTAINER           16
typedef enum {
    ZTE_MAC_ENTRY_ACTION_DEL =  0,
    ZTE_MAC_ENTRY_ACTION_ADD,
    ZTE_MAC_ENTRY_ACTION_CLR,
    ZTE_MAC_ENTRY_ACTION_LST
} zte_mac_entry_action_e;


typedef struct {
    oam_var_cont_t              hdr;
    oam_uint8                   action;
    oam_uint8                   num;
    zte_oam_onu_mac_entry_t     entry[ZTE_MAX_ENTRIES_PER_CONTAINER];
} __attribute__((packed)) oam_zte_onu_mac_entry;

#define ZTE_OAM_ISOLATION_CONFIG_DISABLE               0
#define ZTE_OAM_ISOLATION_CONFIG_ENABLE                1
typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       enable;
} __attribute__((packed)) oam_zte_onu_isolation_config;


#define ZTE_OAM_BUFFER_MANAGEMENT_DISABLE   0
#define ZTE_OAM_BUFFER_MANAGEMENT_ENABLE    1

#define ZTE_OAM_BUFFER_UP_STREAM     0
#define ZTE_OAM_BUFFER_DOWN_STREAM     1
#define ZTE_OAM_BUFFER_BOTH_STREAM     2


#define ZTE_OAM_Q0_BUFFER_MIN_SIZE          1488
#define ZTE_OAM_Q0_UP_BUFFER_MAX_SIZE          393216
#define ZTE_OAM_Q0_DN_BUFFER_MAX_SIZE          235520
#define ZTE_OAM_Q0_BUFFER_CAP               0x01
typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       enable;
    oam_uint8       direction;
    oam_uint8       buffer_size[3];
} __attribute__((packed)) oam_zte_onu_queue_buffer_set;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       capability;
    oam_uint8       min_ds_buff_size[3];
    oam_uint8       max_ds_buff_size[3];
    oam_uint8       min_us_buff_size[3];
    oam_uint8       max_us_buff_size[3];
} __attribute__((packed)) oam_zte_onu_queue_buffer_get;

typedef struct {
    oam_uint64      in_octets;
    oam_uint64      in_ucast_pkt;
    oam_uint64      in_nucast_pkt;
    oam_uint64      in_discard;
    oam_uint64      in_error;
    oam_uint64      out_octets;
    oam_uint64      out_ucast_pkt;
    oam_uint64      out_nucast_pkt;
    oam_uint64      out_discard;
    oam_uint64      out_error;
    oam_uint64      resverved1;
    oam_uint64      resverved2;
    oam_uint64      resverved3;
    oam_uint64      resverved4;
} __attribute__((packed)) oam_zte_onu_port_stats;

typedef struct {
    oam_var_cont_t          hdr;
    oam_zte_onu_port_stats  stats;
} __attribute__((packed)) oam_zte_onu_port_stats_rsp;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       reset;
} __attribute__((packed)) oam_zte_onu_port_stats_reset;

#define PPPOE_USER_MAX_LEN  64
#define PPPOE_PWD_MAX_LEN   32

typedef struct {
    oam_uint8       indication;
    oam_int8        name[PPPOE_USER_MAX_LEN];
    oam_int8        pwd[PPPOE_PWD_MAX_LEN];
    oam_uint8       mode;
    oam_uint16      vlan;
} __attribute__((packed)) oam_zte_onu_pppoe_test;

typedef struct {
    oam_var_cont_t          hdr;
    oam_zte_onu_pppoe_test  config;
} __attribute__((packed)) oam_zte_onu_pppoe_test_rsp;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       state;
    oam_uint8       reason;
} __attribute__((packed)) oam_zte_onu_pppoe_state;

#define IPOE_USER_MAX_LEN       64
#define IPOE_PWD_MAX_LEN        16
#define IPOE_OPTION60_MAX_LEN  128

typedef struct {
    oam_uint8       action;
    oam_uint8       emulation_method;
    oam_int8        user_name[IPOE_USER_MAX_LEN];
    oam_int8        pwd[IPOE_PWD_MAX_LEN];
    oam_uint8       mac[6];
    oam_uint8       option60[IPOE_OPTION60_MAX_LEN];
    oam_uint16      vlan;
    oam_uint8       vpi;
    oam_uint16      vci;
    oam_uint32      port;
    oam_uint8       timeout;
} __attribute__((packed)) oam_zte_onu_ipoe_test;

typedef struct {
    oam_var_cont_t          hdr;
    oam_zte_onu_ipoe_test   config;
} __attribute__((packed)) oam_zte_onu_ipoe_test_rsp;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       state;
    oam_uint8       conclusion;
    oam_uint8       reason;
    oam_uint32      ip_addr;
    oam_uint32      gw_addr;
    oam_uint32      vlan_used;
    oam_uint8       vpi;
    oam_uint16      vci;
    oam_uint16      ping_times;
    oam_uint16      ping_ack_timers;
    oam_uint16      min_delay;
    oam_uint16      max_delay;
    oam_uint16      avg_delay;
} __attribute__((packed)) oam_zte_onu_ipoe_state;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       ctrl_ind;
    oam_uint64      in_octets;
    oam_uint64      in_uc_pkts;
    oam_uint64      in_nuc_pkts;
    oam_uint64      in_discard_pkts;
    oam_uint64      in_error_pkts;
    oam_uint64      out_octets;
    oam_uint64      out_uc_pkts;
    oam_uint64      out_nuc_pkts;
    oam_uint64      out_discard_pkts;
    oam_uint64      out_error_pkts;
} __attribute__((packed)) oam_zte_onu_port_flux_stats;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint32      action  : 8,
                    interval: 24;
    oam_uint32      duration;
} __attribute__((packed)) oam_zte_onu_port_flux_stats_ctrl;

typedef struct {
    oam_uint64      in_octets;
    oam_uint64      in_uc_pkts;
    oam_uint64      in_nuc_pkts;
    oam_uint64      in_discard_pkts;
    oam_uint64      in_error_pkts;
    oam_uint64      out_octets;
    oam_uint64      out_uc_pkts;
    oam_uint64      out_nuc_pkts;
    oam_uint64      out_discard_pkts;
    oam_uint64      out_error_pkts;
} zte_onu_port_flux_stats_t;

/*typedef struct {
    cs_timer_t          timer;
    oam_uint32          port;
    oam_uint32          duration;
    oam_uint32          interval;
    zte_onu_port_flux_stats_t avail_data;
    zte_onu_port_flux_stats_t last_record;
} oam_zte_onu_port_flux_stats_ctrl_t;*/

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint64      out_pkts;
    oam_uint64      out_octets;
    oam_uint64      out_mc_pkts;
    oam_uint64      out_bc_pkts;
    oam_uint64      in_pkts;
    oam_uint64      in_octets;
    oam_uint64      in_mc_pkts;
    oam_uint64      in_bc_pkts;
} __attribute__((packed)) oam_zte_onu_llid_stats1;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint64      in_crc8_err_pkts;
    oam_uint64      in_fec_crct_blks;
    oam_uint64      in_fec_uncrct_blks;
    oam_uint64      out_mpcp_pkts;
    oam_uint64      in_mpcp_pkts;
    oam_uint64      out_mpcp_reg_pkts;
    oam_uint64      out_mpcp_rreq_pkts;
    oam_uint64      out_mpcp_rpt_pkts;
    oam_uint64      in_mpcp_gate_pkts;
    oam_uint64      in_mpcp_reg_pkts;
} __attribute__((packed)) oam_zte_onu_llid_stats2;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint32      rtt;
    oam_uint32      reserved1;
    oam_uint32      reserved2;
} __attribute__((packed)) oam_zte_onu_rtt;


typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       action;
    oam_uint8       entries;
    oam_uint8       mac[1];
} __attribute__((packed)) oam_zte_onu_mc_address_pool;


typedef struct {
    oam_var_cont_t  hdr;
    oam_uint16      manage_vlan;
} __attribute__((packed)) oam_zte_onu_manage_vlan;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint16      message_id;
    oam_uint8       message_info[1];
} __attribute__((packed)) oam_zte_report_matched_mac_t;

typedef struct {
    oam_uint8                       oui[3];
    oam_uint8                       opCode ;
    oam_pdu_ctc_index_t             index ;
    oam_zte_report_matched_mac_t    container ;
} __attribute__((packed))  oam_zte_hg_report_match_mac_event_t;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       home_gateway[OAM_MACADDR_LEN];
    oam_uint8       hg_report_state;
} __attribute__((packed)) oam_zte_hg_report_state;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       match_mac;
    oam_uint8       mac[6];
    oam_uint8       action;
    oam_uint16      duration;
} __attribute__((packed)) oam_zte_pon_tx_ctrl;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       op;
    oam_uint8       rate[3];
    oam_uint8       burst[3];
} __attribute__((packed)) oam_zte_pon_dn_shaping_t;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       port_num;
    oam_uint8       active_pon_status;
    oam_uint8       standby_pon_status;
} __attribute__((packed)) oam_zte_pon_lao_rpt_containe_struct;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       enable;
} __attribute__((packed)) oam_zte_rougue_onu_excl_ability_t;

#define ZTE_OAM_LEAF_ONU_LAO_RPT                0x8019
#define ZTE_OAM_LEAF_MPCP_RTT                   0x8020

#define   HG_PORT_MAX_MAC_POOL_NUM              32
#define   HG_PORT_MAX_MANAGE_VLAN_NUM           1
#define   HG_IPTV_VLAN_INDEX_BASE               1200
#define   HG_VOIP_VLAN_INDEX_BASE               2400

#define HG_PORT_MAX_HOME_GATE_WAY_NUM           1
#define HG_PORT_MAX_RECORD_MAC_NUM              4
#ifdef HAVE_MPORTS
#define HG_MAX_PORT_NUM                         4
#else
#define HG_MAX_PORT_NUM                         1
#endif

#define OAM_ZTE_AUTH_SN_LEN                     128

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       sn[OAM_ZTE_AUTH_SN_LEN];
} __attribute__((packed)) oam_pdu_tw_onu_sn;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       cap;
} __attribute__((packed)) oam_pdu_zte_oam_cap;

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint8       venderId[4];
    oam_uint8       model[4];
    oam_uint8       onuId[6];
    oam_uint8       hwVer[8];
    oam_uint8       swVer[16];
} __attribute__((packed)) oam_pdu_tw_onu_serial;

typedef struct {
    oam_uint8       extBranch;
    oam_uint16      leaf;
    oam_uint8       len;
    oam_uint8       action;
} __attribute__((packed)) oam_pdu_tw_onu_configuration;

typedef struct {
    oam_uint8       extBranch;
    oam_uint16      leaf;
    oam_uint8       len;
    oam_uint32      remote_ip;
    oam_uint32      mask;
    oam_uint8       priority;
    oam_uint16      vlan;
    oam_uint8       enable;
    oam_uint32      host_ip;
    oam_uint32      host_mask;
    oam_uint32      gateway;
} __attribute__((packed)) oam_pdu_tw_onu_remote_connect;

typedef struct {
    oam_uint32      remote_ip;
    oam_uint32      mask;
    oam_uint8       priority;
    oam_uint16      vlan;
    oam_uint8       enable;
    oam_uint32      host_ip;
    oam_uint32      host_mask;
    oam_uint32      gateway;
} __attribute__((packed)) oam_zte_onu_remote_conn_t;

#define ZTE_MAX_ENTRY_TABLE  1024
#define   ZTE_MAX_MAC_PER_SESSION   20

typedef struct {
    oam_var_cont_t  hdr;
    oam_uint32      result;
    oam_uint16      len;
    oam_uint16      sessionId;
    oam_uint16      seqNo;
    oam_uint16      totalNum;
} __attribute__((packed)) oam_zte_onu_port_mac_pool_query_t;

typedef struct {
    oam_uint8       extBranch;
    oam_uint16      leaf;
    oam_uint8       result;
    oam_uint16      len;
    oam_uint16      sessionId;
    oam_uint16      seqNo;
    oam_uint16      totalEntryNum;
} __attribute__((packed)) oam_zte_onu_port_mac_pool_hdr_t;

typedef struct {
    oam_uint32      blockEntryNum;
    oam_uint16      reserved;
    oam_uint16      attr;
    oam_uint8       data[0];  /*max mac address is 100*/
} __attribute__((packed)) oam_zte_onu_port_mac_pool_block_t;

typedef struct {
    oam_uint8       enable; /*0 disbale, 1 enable*/
    oam_uint32      cir;
    oam_uint32      cbs;
} zte_oam_onu_rate_t;

typedef struct {
    oam_uint8       capability;
    oam_uint32      min_ds;
    oam_uint32      max_ds;
    oam_uint32      min_us;
    oam_uint32      max_us;
} zte_oam_onu_queue_buffer_t;

typedef struct {
    oam_uint8       ctrl_ind;
    oam_uint64      in_octets;
    oam_uint64      in_uc_pkts;
    oam_uint64      in_nuc_pkts;
    oam_uint64      in_discard_pkts;
    oam_uint64      in_error_pkts;
    oam_uint64      out_octets;
    oam_uint64      out_uc_pkts;
    oam_uint64      out_nuc_pkts;
    oam_uint64      out_discard_pkts;
    oam_uint64      out_error_pkts;
} zte_oam_onu_port_flux_stats_t;


#endif/*END_ZTE_SPECIFIC*/
#endif
