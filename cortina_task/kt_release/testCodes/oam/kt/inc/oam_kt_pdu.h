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

#ifndef __OAM_KT_PDU_H__
#define __OAM_KT_PDU_H__

//TODO:Why we need this
/*57-17 value 0x21 */
#define OAM_KT_ATTRIB_NOT_SUPPORTED        0x86
#define OAM_KT_VAR_SET_OK                  0x80
#define OAM_KT_VAR_SET_BAD_PARAM           0x86
#define OAM_KT_VAR_SET_NO_RES              0x87

#define OAM_KT_LEAF_CODE_ONU_PORT          0x0001

#define OAM_KT_INDEX_BRANCH                0x36
#define OAM_KT_INDEX_BRANCH_E              0x37

/* KT ext opcode definitions */

#define OAM_PDU_KT_VAR_REQ                 0x01
#define OAM_PDU_KT_VAR_RESP                0x02
#define OAM_PDU_KT_SET_REQ                 0x03
#define OAM_PDU_KT_SET_RESP                0x04
#define OAM_PDU_KT_ONU_AUTH                0x05
#define OAM_PDU_KT_FILE_IMG                0x06

#define OAM_KT_VAR_RSP_CODE_OK                  0x80
#define OAM_KT_VAR_RSP_CODE_TOO_LONG            0x81
#define OAM_KT_VAR_RSP_CODE_UNDETERM            0xA0
#define OAM_KT_VAR_RSP_CODE_UNSUPPORTED         0xA1
#define OAM_KT_VAR_RSP_CODE_CORRUPTED           0xA2
#define OAM_KT_VAR_RSP_CODE_HW_FAILED           0xA3
#define OAM_KT_VAR_RSP_CODE_OVERFLOW            0xA4
#define OAM_KT_VAR_RSP_CODE_BAD_PARAM           0xA8
#define OAM_KT_VAR_RSP_CODE_NO_RES              0xA9
#define OAM_KT_VAR_RSP_CODE_TO_BE_CONT          0xAA


/* KT variable container header */
typedef struct {
        oam_uint8   extBranch;
        oam_uint16  leaf;
        oam_uint8   len;
} __attribute__((packed)) oam_kt_onu_var_container_hdr_struct;

typedef struct {
        oam_uint8   extBranch;
        oam_uint16  leaf;
        oam_uint8   len;
        oam_uint8   data[1];
} __attribute__((packed)) oam_kt_onu_var_set_container_hdr_struct;

//////////////////////////////////////////////////////



#define OAM_OAMPDU_CODE_ONU_VERSION_REQ         0x73
#define OAM_OAMPDU_CODE_ONU_VERSION_RES         0x74

typedef struct {
        oam_pdu_hdr_t           hdr;
        oam_uint8           oui[OAM_OUI_LEN];
        oam_uint8           opcode;
        oam_uint8           data[1];
} __attribute__((packed)) oam_pdu_passave_t;

/* KT Extended Object Class */
#define OAM_KT_INDEX_BRANCH                0x36
#define OAM_KT_ATTRIB_BRANCH                0xa7
#define OAM_KT_ACTION_BRANCH                0xa9

#define OAM_INTF_COUNTERS_MIB_BRANCH_EX         0x87


#define OAM_INTF_ATTRIB_EX_LEAF_PON_OCT_RX_ONU_OK         0x0016
#define OAM_INTF_ATTRIB_EX_LEAF_PON_OCT_RX_ONU_ERR        0x0017
#define OAM_INTF_ATTRIB_EX_LEAF_HOST_PON_TX               0x0018
#define OAM_INTF_ATTRIB_EX_LEAF_HOST_PON_RX               0x0019
#define OAM_INTF_ATTRIB_EX_LEAF_RX_BYTE_ERR               0x001b
#define OAM_INTF_ATTRIB_EX_LEAF_PON_PAUSE_RX              0x001c
#define OAM_INTF_ATTRIB_EX_LEAF_PAUSE_RX                  0x001d
#define OAM_INTF_ATTRIB_EX_LEAF_PAUSE_TX                  0x001e
#define OAM_INTF_ATTRIB_EX_LEAF_OAM_TX_OK                 0x0022
#define OAM_INTF_ATTRIB_EX_LEAF_OAM_RX_OK                 0x0023
#define OAM_INTF_ATTRIB_EX_LEAF_HOST_RX                   0x0027
#define OAM_INTF_ATTRIB_EX_LEAF_RX_FRAME_ERR              0x0028
#define OAM_INTF_ATTRIB_EX_LEAF_TX_FRAME_OK               0x0029
#define OAM_INTF_ATTRIB_EX_LEAF_PON_DROP_FRAME_TX         0x002a
#define OAM_INTF_ATTRIB_EX_LEAF_UNI_OCT_RX_ERR            0x002b
#define OAM_INTF_ATTRIB_EX_LEAF_UNI_OCT_RX_OK             0x002c
#define OAM_INTF_ATTRIB_EX_LEAF_HOST_DROP                 0x002d
#define OAM_INTF_ATTRIB_EX_LEAF_RX_TOO_LONG               0x002e
#define OAM_INTF_ATTRIB_EX_LEAF_UNI_DROP_FRAME_TX         0x002f
#define OAM_INTF_ATTRIB_EX_LEAF_RX_PON_FCS_ERR            0x0030
#define OAM_INTF_ATTRIB_EX_LEAF_PON_PAUSE_TX              0x0031
#define OAM_INTF_ATTRIB_EX_LEAF_MATCH_MAC_DROP            0x0032
#define OAM_INTF_ATTRIB_EX_LEAF_PON_OCT_RX_ERR            0x0064  /*not support*/
#define OAM_INTF_ATTRIB_EX_LEAF_UNI_OCT_TX_OK             0x006b
#define OAM_INTF_ATTRIB_EX_LEAF_HOST_RX_ERR               0x007e
#define OAM_INTF_ATTRIB_EX_LEAF_HOST_TX                   0x0080
#define OAM_INTF_ATTRIB_EX_LEAF_PON_OCT_TOTAL_RX          0x00cb
#define OAM_INTF_ATTRIB_EX_LEAF_PON_OCT_TOTAL_RX_ONU      0x00cc
#define OAM_INTF_ATTRIB_EX_LEAF_UNI_OCT_TOTAL_RX          0x00cd
#define OAM_INTF_ATTRIB_EX_LEAF_BAD_FRAME_RX              0x00d6
#define OAM_INTF_ATTRIB_EX_LEAF_PON_MATCH_MAC_DROP_RX     0x00e9


#define OAM_INTF_LEAF_MAC_CTRL_PAUSE_TX         0x62
#define OAM_INTF_LEAF_MAC_CTRL_PAUSE_RX         0x63
#define OAM_INTF_LEAF_UNI_FRAME_RX              0x84
#define OAM_INTF_LEAF_UNI_FRAME_TX              0x85
#define OAM_INTF_LEAF_TOO_SHORT_FRAME_RX        0x86

#define OAM_INTF_ATTRIB_EX_LEAF_MPCP_REG_REQ_TX 0x013e
#define OAM_INTF_ATTRIB_EX_LEAF_MPCP_REG_RX     0x0142
#define OAM_INTF_ATTRIB_EX_LEAF_MPCP_REG_ACK_TX 0x013c
#define OAM_INTF_ATTRIB_EX_LEAF_MPCP_GATE_RX    0x0140
#define OAM_INTF_ATTRIB_EX_LEAF_MPCP_REP_TX     0x013f

/* KT Extended MO for Port */
#if 0
#define OAM_KT_LEAF_CODE_ONU_ETH_DS_SHAPING             0x0001
#define OAM_KT_LEAF_CODE_ONU_ETH_MAC_LIMIT              0x0002
#define OAM_KT_LEAF_CODE_ONU_BLOCK                      0x0003
#define OAM_KT_LEAF_CODE_ONU_DIAGNOSTIC                 0x0004
#define OAM_KT_LEAF_CODE_ONU_QUEUE_DROP_COUNTER         0x0005
#define OAM_KT_LEAF_CODE_ONU_PORT_COUNTER               0x0011
#define OAM_KT_LEAF_CODE_ONU_ETH_RSTP                   0x0012
#define OAM_KT_LEAF_CODE_ONU_LOOP_DETECT                0x0013
#define OAM_KT_LEAF_CODE_ONU_MPCP_COUNTER               0x0014

/* KT Extended Action */
#define OAM_KT_LEAF_CODE_ONU_COUNTER_CLEAR              0x0001
#define OAM_KT_LEAF_CODE_ONU_DEFAULT_RESTORE            0x0002
#define OAM_KT_LEAF_CODE_ONU_TX_POWER_OFF               0x0003
#endif

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           value;
} __attribute__((packed)) oam_pdu_mix_index_t;

typedef struct {
        oam_pdu_hdr_t           hdr;
        oam_uint8           oui[OAM_OUI_LEN];
        oam_uint8           opcode;
        oam_uint8           data[1];
} __attribute__((packed)) oam_pdu_kt_t;

typedef struct {
    oam_uint8 branch;
    oam_uint16 leaf;
    oam_uint8 len;
    oam_uint8 data[1];
}__attribute__((packed)) oam_kt_tlv_t;

/* KT variable container header */
typedef struct {
        oam_uint8   extBranch;
        oam_uint16  leaf;
        oam_uint8   len;
} __attribute__((packed)) oam_kt_var_cont_t;

/* KT variable descriptor */
typedef struct {
        oam_uint8   extBranch;
        oam_uint16  leaf;
} __attribute__((packed)) oam_kt_onu_var_descriptor_struct;

/* KT Set container */
typedef struct {
        oam_uint8   extBranch;
        oam_uint16  leaf;
        oam_uint8   len;
        oam_uint8   data[1];
} __attribute__((packed)) oam_kt_onu_set_container_hdr_struct;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint16    port;
} __attribute__((packed)) oam_pdu_kt_port_obj_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint16    port;
        oam_uint16    link;
} __attribute__((packed)) oam_pdu_kt_link_obj_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint16    port;
        oam_uint16    reserved;
        oam_uint16    queue;
} __attribute__((packed)) oam_pdu_kt_queue_obj_t;


#define KT_ONU_BLOCK_DEACTIVATED      0
#define KT_ONU_BLOCK_ACTIVATED        1
typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 action;
} __attribute__((packed)) oam_kt_onu_block_container_struct;


#define KT_MAC_LIMIT_OPER_DISABLE        0
#define KT_MAC_LIMIT_OPER_ENABLE         1

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 oper;
        oam_uint8 number;
} __attribute__((packed)) oam_kt_onu_eth_mac_limit_container_struct;

#define KT_PORT_DS_SHAPING_OPER_DISABLE        0
#define KT_PORT_DS_SHAPING_OPER_ENABLE         1

typedef struct {
    oam_kt_var_cont_t hdr;
    oam_uint8 portOper;
    oam_uint8 rate[3];
} __attribute__((packed)) oam_kt_onu_eth_ds_shaping_container_struct;

typedef struct {
    oam_kt_var_cont_t hdr;
    oam_uint16 mask;
    oam_uint64 tx_uni_frames;
    oam_uint64 tx_uni_bytes;
    oam_uint64 tx_uni_multicast;
    oam_uint64 tx_uni_broadcast;
    oam_uint64 tx_uni_dropped_frames;
    oam_uint64 rx_uni_frames;
    oam_uint64 rx_uni_bytes;
    oam_uint64 rx_uni_multicast;
    oam_uint64 rx_uni_broadcast;
    oam_uint64 rx_uni_oversize_frames;
    oam_uint64 rx_uni_undersize_frames;
    oam_uint64 rx_uni_crc_frames;
    oam_uint64 reserved2[3];
} __attribute__((packed)) oam_kt_onu_port_counter_container_struct;

typedef struct {
    oam_kt_var_cont_t hdr;
    oam_uint16 upstream_queue_count;
    oam_uint64 tx_pon_queue1_total_frames;
    oam_uint64 tx_pon_queue1_drop_frames;
    oam_uint64 tx_pon_queue2_total_frames;
    oam_uint64 tx_pon_queue2_drop_frames;
    oam_uint64 tx_pon_queue3_total_frames;
    oam_uint64 tx_pon_queue3_drop_frames;
    oam_uint64 tx_pon_queue4_total_frames;
    oam_uint64 tx_pon_queue4_drop_frames;
    oam_uint64 tx_pon_queue5_total_frames;
    oam_uint64 tx_pon_queue5_drop_frames;
    oam_uint64 tx_pon_queue6_total_frames;
    oam_uint64 tx_pon_queue6_drop_frames;
    oam_uint64 tx_pon_queue7_total_frames;
    oam_uint64 tx_pon_queue7_drop_frames;
    oam_uint64 tx_pon_queue8_total_frames;
    oam_uint64 tx_pon_queue8_drop_frames;
} __attribute__((packed)) oam_kt_onu_queue_drop_counter_container_struct;

typedef struct {
    oam_kt_var_cont_t hdr;
    oam_uint16 mask;
    oam_uint64 rx_hec_errors;
    oam_uint64 tx_register_request;
    oam_uint64 rx_register;
    oam_uint64 tx_register_ack;
    oam_uint64 rx_gate_frames;
    oam_uint64 tx_report_frames;
} __attribute__((packed)) oam_kt_onu_mpcp_counter_container_struct;

#define KT_ONU_RSTP_DEACTIVATED      0
#define KT_ONU_RSTP_ACTIVATED        1
typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 oper;
} __attribute__((packed)) oam_kt_onu_rstp_container_struct;

#define KT_COUNTER_CLEAR_DEACTIVATED      0
#define KT_COUNTER_CLEAR_ACTIVATED        1
typedef struct {
        oam_kt_var_cont_t hdr;
} __attribute__((packed)) oam_kt_onu_counter_clear_container_struct;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 enable;
} __attribute__((packed)) oam_kt_onu_ipv6_opt254_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 uni_port;
        oam_macaddr_t mac;
} __attribute__((packed)) oam_kt_onu_per_port_mac_entries_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	port_id;
        oam_uint16      mask;
        oam_uint64      txFrames;
        oam_uint64      txBytes;
        oam_uint64    txUnicastFrames;
        oam_uint64    txMCFrames;
        oam_uint64    txBCFrames;
        oam_uint64    txDropFrames;
        oam_uint64      rxFrames;
        oam_uint64      rxBytes;
        oam_uint64    rxUnicastFrames;
        oam_uint64    rxMCFrames;
        oam_uint64    rxBCFrames;
        oam_uint64    rxOverSizeFrames;
        oam_uint64    rxUnderSizeFrames;
        oam_uint64    rxCRCFrames;
        oam_uint64    inpause;
} __attribute__((packed)) oam_kt_onu_per_port_statistics_entries_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	port_id;
        oam_uint16      cable_length;
} __attribute__((packed)) oam_kt_onu_uni_port_rtct_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	port_id;
} __attribute__((packed)) oam_kt_onu_uni_port_stats_clr_t;


typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	port;
        oam_uint8      pkt_type;
        oam_uint16     threshold;
} __attribute__((packed)) oam_kt_onu_per_port_storm_control_set_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	status;
} __attribute__((packed)) oam_kt_onu_storm_control_status_set_t;


typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	filter;
        oam_uint8    fmac[6];
} __attribute__((packed)) oam_kt_onu_mac_filter_set_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	port;
        oam_uint8    admin;
} __attribute__((packed)) oam_kt_onu_uni_link_status_set_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	port;
        oam_uint16     rj45_failure;
        oam_uint16   cnt_up2down;
} __attribute__((packed)) oam_kt_onu_uni_link_status_cnt_get_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	port;
        oam_uint8     status;
} __attribute__((packed)) oam_kt_onu_uni_link_status_get_t;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	module;
        //oam_uint8    clear;
} __attribute__((packed)) oam_kt_onu_critical_event_log_set_t;


typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8     module;
        oam_uint8    trig_time[19];
        oam_uint8      faultId;
} __attribute__((packed)) oam_kt_onu_critical_event_log_get_t;
typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 	port;
        oam_uint16      bc_threshold;
        oam_uint16     mc_threshold;
        oam_uint16     uuc_threshold;
} __attribute__((packed)) oam_kt_onu_per_port_storm_control_get_t;


typedef struct {
        oam_kt_var_cont_t hdr;
        oam_int16 temperature;
        oam_uint16 tx_power;
        oam_uint16 rx_power;
} __attribute__((packed)) oam_kt_onu_diagnostioam_container_struct;

#define KT_LOOP_DECTECT_DEACTIVATED      0
#define KT_LOOP_DECTECT_ACTIVATED        1
typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 opr;
        oam_uint8 interval;
        oam_uint8 block_time;
} __attribute__((packed)) oam_kt_onu_loop_detect_container_struct;

#define KT_DHCP_OPTION_DEACTIVATED      0
#define KT_DHCP_OPTION_ACTIVATED        1
#define KT_DHCP_OPTION_UNSUPPORT        0xff
typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 cfg;
} __attribute__((packed)) oam_kt_onu_dhcp_option_container_struct;

#define KT_DEFAULT_RESTORE_DEACTIVATED      0
#define KT_DEFAULT_RESTORE_ACTIVATED        1
typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 action;
} __attribute__((packed)) oam_kt_onu_default_restore_container_struct;

#define KT_TX_POWER_OFF_DEACTIVATED      0
#define KT_TX_POWER_OFF_ACTIVATED        1
typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 action;
        oam_uint8 duration;
} __attribute__((packed)) oam_kt_onu_tx_power_off_container_struct;




#define KT_ORG_EVENT_NOTIFICATION_OP_CODE        0xf

#define KT_ORG_EVENT_LOOP_DETECT        1

#define KT_EVENT_LOOP_DETECTED            1
#define KT_EVENT_LOOP_REPAIRED            2

typedef struct {
        oam_uint8           type;
        oam_uint8           length;
        oam_uint8           info[1];
} __attribute__((packed)) oam_tlv_kt_event_t;

typedef struct {
        oam_pdu_hdr_t           hdr;
        oam_uint8           oui[OAM_OUI_LEN];
        oam_uint8           opcode;
        oam_tlv_kt_event_t      event[1];
} __attribute__((packed)) oam_pdu_kt_event_t;


typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 value;
} __attribute__((packed)) oam_kt_onu_opm_alarm_container_struct;

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 value;
} __attribute__((packed)) oam_kt_onu_vol_alarm_container_struct;

#define KT_ORG_MAX_STATIC_MAC        20
typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 count;
        oam_macaddr_t mac[KT_ORG_MAX_STATIC_MAC];
} __attribute__((packed)) oam_kt_onu_static_mac_container_struct;

#define KT_ORG_MAX_ONU_MAC        128
#define KT_ORG_MAX_ONU_MAC_PER_PORT        32

typedef struct {
        oam_kt_var_cont_t hdr;
        oam_uint8 count;
        oam_macaddr_t mac[KT_ORG_MAX_ONU_MAC];
} __attribute__((packed)) oam_kt_onu_mac_tbl_t;

/* OLT check the sensitivity exceed and config alarm state to ONU */
#define KT_SEN_EXCEED_ALARM_RAISE           1
#define KT_SEN_EXCEED_ALARM_RELEASE         2

typedef struct {
        oam_kt_var_cont_t       hdr;
        unsigned char           value;
} __attribute__((packed)) oam_kt_olt_sen_exceed_alarm_container_struct;

/* ONU DDM raw data from optical module */
typedef struct {
        oam_kt_var_cont_t       hdr;
        short                   temperature;
        unsigned short          voltage;
        unsigned short          tx_bias;
        unsigned short          tx_power;
        unsigned short          rx_power;
} __attribute__((packed)) oam_kt_onu_opm_ddm_container_struct;


/* ONU System Time setting data*/
typedef struct {
        oam_kt_var_cont_t       hdr;
        oam_uint8           year[2];
        oam_uint8           month;
        oam_uint8           day;
        oam_uint8           hour;
        oam_uint8           minute;
        oam_uint8           second;
} __attribute__((packed)) oam_kt_onu_system_time_t;

typedef struct{
    oam_uint8       id;
    union{
        oam_uint16  unsign_value;
        oam_int16   sign_value;
    } low_raise;

    union{
        oam_uint16  unsign_value;
        oam_int16   sign_value;
    } low_clear;

    union{
        oam_uint16  unsign_value;
        oam_int16   sign_value;
    } high_raise;

    union{
        oam_uint16  unsign_value;
        oam_int16   sign_value;
    } high_clear;

    oam_uint8       state;
}__attribute__((packed))ddm_threshold_t;

typedef struct {
        oam_kt_var_cont_t       hdr;
        ddm_threshold_t         threshold[5];
} __attribute__((packed)) oam_kt_onu_ddm_threshold_get_t;

typedef struct {
        oam_kt_var_cont_t   hdr;
        oam_uint8       id;
        oam_uint16      low_raise;
        oam_uint16      low_clear;
        oam_uint16      high_raise;
        oam_uint16      high_clear;
} __attribute__((packed)) oam_kt_onu_ddm_threshold_set_t;

#endif
