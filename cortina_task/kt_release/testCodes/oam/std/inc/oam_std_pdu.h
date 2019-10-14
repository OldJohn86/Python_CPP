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

#ifndef __OAM_STD_PDU_H__
#define __OAM_STD_PDU_H__

#include "oam_types.h"

#define OAM_VERSION                             0x01
#define OAM_MACADDR_LEN                         6
#define OAM_IPV6_ADDR_LEN                       16
#define OAM_IPV4_ADDR_LEN                       4
#define OAM_OUI_LEN                             3
#define OAM_VENDOR_LEN                          4
#define OAM_SLOW_PROTOCOLS_TYPE                 0x8809
#define OAM_SUBTYPE                             0x03
#define OAM_MAX_OAMPDU_SIZE                     1518
#define OAM_MIN_OAMPDU_SIZE                     60


#define OAM_DBA_EXT_OPCODE                      0x0a
#define OAM_DBA_CODE_GET_REQ                    0x00
#define OAM_DBA_CODE_GET_RSP                    0x01
#define OAM_DBA_CODE_SET_REQ                    0x02
#define OAM_DBA_CODE_SET_RSP                    0x03
#define OAM_MAX_QUEUESET                        4

#define OAM_FLAG_LINK_FAULT                     0x0001
#define OAM_FLAG_DYING_GASP                     0x0002
#define OAM_FLAG_CRITICAL_EVENT                 0x0004
#define OAM_FLAG_LOCAL_EVALUATING               0x0008
#define OAM_FLAG_LOCAL_STABLE                   0x0010
#define OAM_FLAG_REMOTE_EVALUATING              0x0020
#define OAM_FLAG_REMOTE_STABLE                  0x0040
#define OAM_FLAG_MASK_EVENTS                    0x0007
#define OAM_FLAG_PASSAVE_PING                   0x0100

#define OAM_PDU_CODE_INFO                       0x00
#define OAM_PDU_CODE_EVENT                      0x01
#define OAM_PDU_CODE_VAR_REQ                    0x02
#define OAM_PDU_CODE_VAR_RSP                    0x03
#define OAM_PDU_CODE_LOOPBACK                   0x04
#define OAM_PDU_CODE_ORG_SPEC                   0xfe
#define OAM_OAMPDU_CODE_SET_THRESH              0x70
#define OAM_OAMPDU_CODE_REREGISTER              0x71
#define OAM_OAMPDU_CODE_REMOTE_ACCESS           0x75
#define OAM_PDU_PASSAVE_PING                    0x80
#define OAM_CODE_PASSAVE_UNI_GET                0x84
#define OAM_CODE_PASSAVE_UNI_RSP                0x85
#define OAM_PDU_PASSAVE_UNI_SET                 0x86


#define OAM_ATTRIB_NOT_SUPPORTED                0x86
#define OAM_VAR_SET_OK                          0x80
#define OAM_VAR_SET_BAD_PARAM                   0x86
#define OAM_VAR_SET_NO_RES                      0x87


#define OAM_INFO_TLV_LOCAL_INFO                 0x01
#define OAM_INFO_TLV_REMOTE_INFO                0x02
#define OAM_INFO_TLV_ORG_SPEC                   0xfe

#define OAM_INFO_CONFIG_MASK                    0x1f
#define OAM_INFO_CONFIG_OAM_MODE                0x01
#define OAM_INFO_CONFIG_UNIDIR                  0x02
#define OAM_INFO_CONFIG_REMOTE_LB               0x04
#define OAM_INFO_CONFIG_LINK_EVENT              0x08
#define OAM_INFO_CONFIG_VAR_RETR                0x10

#define OAM_INFO_PDU_CONFIG_MASK                0x7ff

#define OAM_EVENT_TLV_END                       0x00
#define OAM_EVENT_TLV_ERR_SYM_PERIOD            0x01
#define OAM_EVENT_TLV_ERR_FRAME                 0x02
#define OAM_EVENT_TLV_ERR_FRAME_PERIOD          0x03
#define OAM_EVENT_TLV_ERR_FRAME_SUMM            0x04
#define OAM_EVENT_TLV_RERV                      0x05


#define OAM_2DOT0_FLAG_LOCAL_STABLE             0x10
#define OAM_2DOT0_FLAG_REMOTE_STABLE            0x08

#define OAM_INFO_STATE_MUX_DISCARD              0x04
#define OAM_INFO_STATE_PAR_MASK                 0x03
#define OAM_INFO_STATE_PAR_FWD                  0x00
#define OAM_INFO_STATE_PAR_LB                   0x01
#define OAM_INFO_STATE_PAR_DISCARD              0x02

#define OAM_LOOPBACK_COMMAND_ENABLE             0x01
#define OAM_LOOPBACK_COMMAND_DISABLE            0x02

#define OAM_VAR_WIDTH_IND_PRESENT               0x80
#define OAM_VAR_WIDTH_IND_TOO_LONG              0x01

#define OAM_2DOT0_INFO_STATE_MUX_DISCARD        0x08
#define OAM_2DOT0_INFO_STATE_PAR_MASK           0x06
#define OAM_2DOT0_INFO_STATE_PAR_FWD            0x00
#define OAM_2DOT0_INFO_STATE_PAR_LB             0x02
#define OAM_2DOT0_INFO_STATE_PAR_DISCARD        0x04

#define OAM_2DOT0_INFO_CONFIG_MASK              0xff
#define OAM_2DOT0_INFO_CONFIG_ORG_SPEC_OAMPDU   0x20
#define OAM_2DOT0_INFO_CONFIG_ORG_SPEC_EVENTS   0x40
#define OAM_2DOT0_INFO_CONFIG_ORG_SPEC_INFOTLV  0x80
#define OAM_2DOT0_MAX_VAR_REQ_ENTRIES           15
#define OAM_MAX_VAR_REQ_ENTRIES                 15


#define OAM_ATTRIB_BRANCH_NULL                  0

#define OAM_INTF_COUNTERS_MIB_BRANCH            0x7
#define OAM_INTF_CONTROL_MIB_BRANCH             0x9


#define OAM_INTF_LEAF_PON_OCT_TX                0x8
#define OAM_INTF_LEAF_PON_SING_COLL             0x3
#define OAM_INTF_LEAF_PON_MULTI_COLL            0x4
#define OAM_INTF_LEAF_PON_UNIC_RX               0x5
#define OAM_INTF_LEAF_PON_SEQ_ERR               0x6
#define OAM_INTF_LEAF_PON_ALG_ERR               0x7
#define OAM_INTF_LEAF_PON_UNIC_TX               0x2
#define OAM_INTF_LEAF_PON_DEFR                  0x9
#define OAM_INTF_LEAF_PON_LATE_COLL             0xa
#define OAM_INTF_LEAF_PON_EXC_COLL              0xb
#define OAM_INTF_LEAF_PON_MAXTX_ERR             0xc
#define OAM_INTF_LEAF_PON_SEN_ERR               0xd
#define OAM_INTF_LEAF_PON_OCT_RX                0xe
#define OAM_INTF_LEAF_PON_RX_ERR                0xf
#define OAM_INTF_LEAF_TX_MC_FRAME_OK            0x12
#define OAM_INTF_LEAF_TX_BC_FRAME_OK            0x13
#define OAM_INTF_LEAF_14                        0x14
#define OAM_INTF_LEAF_RX_MC_FRAME_OK            0x15
#define OAM_INTF_LEAF_RX_BC_FRAME_OK            0x16
#define OAM_INTF_LEAF_IN_RANGE_LEN_ERR          0x17
#define OAM_INTF_LEAF_PON_TOO_LNG               0x19

#define OAM_INTF_LEAF_ETH_ADM_STATE             0x25
#define OAM_INTF_LEAF_ETH_AUTONEG_ADM_STATE     0x4f
#define OAM_INTF_LEAF_ETH_AUTONEG_LOCAL_CAP     0x52
#define OAM_INTF_LEAF_ETH_AUTONEG_ADVTS_CAP     0x53
#define OAM_INTF_LEAF_ETH_FEC_ABILITY           0x139
#define OAM_INTF_LEAF_ETH_FEC_MODE              0x13a
#define OAM_INTF_LEAF_QUSET_MODE                0x13a

#define OAM_INTF_LEAF_ETH_ADM_STATE_SET         0x5
#define OAM_INTF_LEAF_ETH_AUTO_NEG_RESTART      0x0b
#define OAM_INTF_LEAF_ETH_AUTO_NEG_CONTRL       0x0c

/*LOID AUTH*/
#define LOID_MAX_LEN                            25
#define PASSWORD_MAX_LEN                        13

/*aPhyAdminState*/
#define OAM_INTF_PHY_ADMIN_DIS                  1
#define OAM_INTF_PHY_ADMIN_EN                   2
#define OAM_INTF_PHY_ADMIN_TST                  3

/*acPHYAdminControl*/
#define OAM_INTF_PHY_ADMIN_SET_DIS              1
#define OAM_INTF_PHY_ADMIN_SET_EN               2

/*AutoNegAdminState*/
#define OAM_INTF_AUTO_NEG_ADMIN_ST_DIS          1
#define OAM_INTF_AUTO_NEG_ADMIN_ST_EN           2

/*aFECAbility*/
#define OAM_INTF_FEC_ABILITY_UNKOWN             1
#define OAM_INTF_FEC_SUPPORDTED                 2
#define OAM_INTF_FEC_NOT_SUPPORTED              3

/*aFECMode*/
#define OAM_INTF_FEC_MODE_UNKOWN                1
#define OAM_INTF_FEC_MODE_EN                    2
#define OAM_INTF_FEC_MODE_DIS                   3


typedef struct {
    oam_uint8           branch;
    oam_uint16          leaf;
} __attribute__((packed)) oam_var_desc_t;

typedef struct {
    oam_uint8           branch;
    oam_uint16          leaf;
    oam_uint8           width;
} __attribute__((packed)) oam_var_cont_t;

typedef struct {
    oam_uint8           branch;
    oam_uint16          leaf;
    oam_uint8           width;
    oam_uint8           data[1];
} __attribute__((packed)) oam_var_cont_val_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8          value;
} __attribute__((packed)) oam_var_cont_val_8_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint16          value;
} __attribute__((packed)) oam_var_cont_val_16_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint32          value;
} __attribute__((packed)) oam_var_cont_val_32_t;

typedef struct {
        oam_var_cont_t  hdr;
        oam_uint64      value;
} __attribute__((packed)) oam_var_cont_val_64_t;

typedef struct {
    oam_uint8           dst_mac[OAM_MACADDR_LEN];
    oam_uint8           src_mac[OAM_MACADDR_LEN];
    oam_uint16          eth_type;
    oam_uint8           sub_type;
    oam_uint16          flags;
    oam_uint8           code;
} __attribute__((packed)) oam_pdu_hdr_t;


typedef struct {
    oam_uint8           type;
    oam_uint8           length;
    oam_uint8           version;
    oam_uint16          revision;
    oam_uint8           state;
    oam_uint8           oam_config;
    oam_uint16          oampdu_config;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           vendor[OAM_VENDOR_LEN];
} __attribute__((packed)) oam_tlv_info_t;

typedef struct {
    oam_uint8           type;
    oam_uint8           length;
    oam_uint8           oui[OAM_OUI_LEN];
} __attribute__((packed)) oam_tlv_org_spec_t;

typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_tlv_info_t      local_info;
    oam_tlv_info_t      remote_info;
} __attribute__((packed)) oam_pdu_info_t;

typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           ext_opcode;
    oam_uint8           dba_code;
} __attribute__((packed)) oam_pdu_orgspec_t;

typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           ext_opcode;
    oam_uint8           dba_code;
} __attribute__((packed)) oam_pdu_dbagetreq_t;


typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           ext_opcode;
    oam_uint8           dba_code;
    oam_uint8           num_queueset;
    oam_uint8           queueset_start;
} __attribute__((packed)) oam_pdu_dbagetrsp_t;


typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           ext_opcode;
    oam_uint8           dba_code;
    oam_uint8           num_queueset;
    oam_uint8           queueset_start;
} __attribute__((packed)) oam_pdu_dbasetreq_t;

typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           ext_opcode;
    oam_uint8           dba_code;
    oam_uint8           setack;
    oam_uint8           num_queueset;
    oam_uint8           queueset_start;
} __attribute__((packed)) oam_pdu_dbasetrsp_t;



typedef struct {
    oam_uint8           type;
    oam_uint8           length;
    oam_uint16          timestamp;
    oam_uint64          window;
    oam_uint64          threshold;
    oam_uint64          errors;
    oam_uint64          error_running_total;
    oam_uint32          event_running_total;
} __attribute__((packed)) oam_tlv_link_event_t;


/* optimization of the alarm feature */
/* define the oam tlv for errored frame event,errored frame Period event and errored frame
  * second summary event
  */
typedef struct {
    oam_uint8           type;
    oam_uint8           length;
    oam_uint16          timestamp;
    oam_uint16          window; /* This field is two-octet length */
    oam_uint32          threshold;/* This field is four-octet length */
    oam_uint32          errors;/* This field is four-octet length */
    oam_uint64          error_running_total;
    oam_uint32          event_running_total;
} __attribute__((packed)) oam_tlv_link_errored_event_t;

typedef struct {
    oam_uint8           type;
    oam_uint8           length;
    oam_uint16          timestamp;
    oam_uint32          window; /* This field is four-octet length */
    oam_uint32          threshold;/* This field is four-octet length */
    oam_uint32          errors;/* This field is four-octet length */
    oam_uint64          error_running_total;
    oam_uint32          event_running_total;
} __attribute__((packed)) oam_tlv_link_errored_period_event_t;

typedef struct {
    oam_uint8           type;
    oam_uint8           length;
    oam_uint16          timestamp;
    oam_uint16          window; /* This field is two-octet length */
    oam_uint16          threshold;/* This field is two-octet length */
    oam_uint16          errors;/* This field is two-octet length */
    oam_uint32          error_running_total; /* This field is four-octet length */
    oam_uint32          event_running_total;
} __attribute__((packed)) oam_tlv_link_errored_seconds_summary_event_t;


typedef struct {
    oam_pdu_hdr_t           hdr;
    oam_uint16              seq_num;
    oam_tlv_link_event_t    event[1];
} __attribute__((packed)) oam_pdu_event_t;


typedef struct {
    oam_pdu_hdr_t           hdr;
    oam_var_desc_t          descriptor[1];
} __attribute__((packed)) oam_pdu_var_req_t;


typedef struct {
    oam_pdu_hdr_t           hdr;
    oam_var_cont_t          container[1];
} __attribute__((packed)) oam_pdu_var_rsp_t;

typedef struct {
    oam_pdu_hdr_t           hdr;
    oam_uint8               command;
} __attribute__((packed)) oam_pdu_loopback_t;

#define OAM_VAR_REQ_LEN 32
typedef struct {
    oam_uint8           branch;
    oam_uint16          leaf;
    oam_uint8           width;
    oam_uint8           value[OAM_VAR_REQ_LEN];
} oam_parm_oampdu_var_rsp_t;

typedef struct {
    oam_uint8           branch;
    oam_uint8           leaf;
} oam_d2_parm_oampdu_var_rsp_t;

typedef struct {
    oam_uint8           branch;
    oam_uint8           leaf;
    oam_uint8           width;
    oam_uint8           value[1];
} __attribute__((packed)) oam_d2_var_cont_t;


typedef struct {
    oam_uint8           branch;
    oam_uint8           leaf;
} __attribute__((packed)) oam_d2_var_desc_t;

typedef struct {
    oam_pdu_hdr_t              hdr;
    oam_d2_var_desc_t          descriptor[1];
} __attribute__((packed)) oam_d2_pdu_var_req_t;


typedef struct {
    oam_var_cont_t           hdr;
    oam_uint32          value;
} __attribute__((packed)) oam_pdu_uint32_t;

typedef struct {
    oam_var_cont_t           hdr;
    oam_uint64          value;
} __attribute__((packed)) oam_pdu_uint64_t;

typedef struct {
    oam_var_cont_t           hdr;
    oam_uint8          value;
} __attribute__((packed)) oam_pdu_uint8_t;


/* interface statistics per RFC 2863 */
typedef struct {
    oam_uint64  in_octets;
    oam_uint64  in_unicast;
    oam_uint64  in_multicast;
    oam_uint64  in_broadcast;
    oam_uint64  in_discards;
    oam_uint64  in_errors;
    oam_uint64  in_unknown_protos;
    oam_uint64  in_err_symbol;
    oam_uint64  out_octets;
    oam_uint64  out_unicast;
    oam_uint64  out_multicast;
    oam_uint64  out_broadcast;
    oam_uint64  out_discards;
    oam_uint64  out_errors;
} __attribute__((packed)) epon_if_stats_t;

/* Ethernet-like interface stats per RFC 3635 */
typedef struct {
    oam_int64   alignment_errors;
    oam_int64   fcs_errors;
    oam_int32   single_collision;
    oam_int32   multiple_collision;
    oam_int32   sqe_test_errors;
    oam_int32   deferred_transmissions;
    oam_int32   late_collisions;
    oam_int32   excessive_collisions;
    oam_int64   internal_mac_tx_errors;
    oam_int32   carrier_sense_errors;
    oam_int64   frame_too_longs;
    oam_int64   internal_mac_rx_errors;
    oam_int64   symbol_errors;
    oam_uint64  in_unknown_opcode;
    oam_uint64  in_pause_frames;
    oam_uint64  out_pause_frames;
} __attribute__((packed)) epon_ether_stats_t;

/* EPON statistics per EFM EPON MIB */
typedef struct {
    oam_uint32  mac_ctrl_frame_rx;
    oam_uint32  disc_windows_sent;
    oam_uint32  disc_timeout;
    oam_uint32  tx_reg_request;
    oam_uint32  rx_reg_request;
    oam_uint32  tx_reg_ack;
    oam_uint32  rx_reg_ack;
    oam_uint32  tx_report;
    oam_uint32  rx_report;
    oam_uint32  tx_gate;
    oam_uint32  rx_gate;
    oam_uint32  tx_register;
    oam_uint32  rx_register;
    oam_uint32  rx_not_supported;
    oam_uint32  sld_errors;
    oam_uint32  crc8_errors;
    oam_uint32  bad_llid;
    oam_uint32  good_llid;
    oam_uint32  onu_pon_cast_llid;
    oam_uint32  olt_pon_cast_llid;
    oam_uint32  bcast_llid_not_onu_id;
    oam_uint32  onu_llid_not_bcast;
    oam_uint32  bcast_llid_plus_onu_id;
    oam_uint32  not_bcast_llid_not_onu_id;
    oam_uint32  pcs_coding_violation;
    oam_uint32  fec_corrected_blocks;
    oam_uint32  fec_uncorrectable_blocks;
    oam_uint32  buffer_head_coding_violation;
} __attribute__((packed)) epon_epon_stats_t;

typedef struct {
    oam_uint64  pon_byte_cnt           ;
    oam_uint64  pon_tx_byte_cnt        ;
    oam_uint32  pon_frame_cnt          ;
    oam_uint32  pon_txframe_cnt        ;
    oam_uint32  pon_crcerr_cnt         ;
    oam_uint32  pon_ucframe_cnt        ;
    oam_uint32  pon_mcframe_cnt        ;
    oam_uint32  pon_bcframe_cnt        ;
    oam_uint32  pon_txucframe_cnt      ;
    oam_uint32  pon_txmcframe_cnt      ;
    oam_uint32  pon_txbcframe_cnt      ;
    oam_uint32  pon_ctrlframe_cnt      ;
    oam_uint32  pon_txctrlframe_cnt    ;
    oam_uint32  pon_pauseframe_cnt     ;
    oam_uint32  pon_unknownop_cnt      ;
    oam_uint32  pon_runt_cnt           ;
    oam_uint32  pon_oversize_cnt       ;
    oam_uint32  pon_rmon64_cnt         ;
    oam_uint32  pon_rmon65_127_cnt     ;
    oam_uint32  pon_rmon128_255_cnt    ;
    oam_uint32  pon_rmon256_511_cnt    ;
    oam_uint32  pon_rmon512_1023_cnt   ;
    oam_uint32  pon_rmon1024_1518_cnt  ;
    oam_uint32  pon_txrmon64_cnt       ;
    oam_uint32  pon_txrmon65_127_cnt   ;
    oam_uint32  pon_txrmon128_255_cnt  ;
    oam_uint32  pon_txrmon256_511_cnt  ;
    oam_uint32  pon_txrmon512_1023_cnt ;
    oam_uint32  pon_txrmon1024_1518_cnt;
    oam_uint32  pon_mpcp_uc_rev_cnt     ;
    oam_uint32  pon_mpcp_uc_reg_ack     ;
    oam_uint32  pon_mpcp_uc_reg_nack    ;
    oam_uint32  pon_mpcp_uc_reg_dereg   ;
    oam_uint32  pon_mpcp_uc_reg_rereg   ;
    oam_uint32  pon_mpcp_uc_reg_reg_rev ;
    oam_uint32  pon_mpcp_uc_gat_rev     ;
    oam_uint32  pon_mpcp_uc_gat_norm    ;
    oam_uint32  pon_mpcp_uc_gat_frpt    ;
    oam_uint32  pon_mpcp_uc_gat_udis    ;
    oam_uint32  pon_mpcp_uc_gat_bdis    ;
    oam_uint32  pon_mpcp_mc_rev_cnt     ;
    oam_uint32  pon_mpcp_mc_reg_ack     ;
    oam_uint32  pon_mpcp_mc_reg_nack    ;
    oam_uint32  pon_mpcp_mc_reg_dereg   ;
    oam_uint32  pon_mpcp_mc_reg_rereg   ;
    oam_uint32  pon_mpcp_mc_reg_reg_rev ;
    oam_uint32  pon_mpcp_mc_gat_rev     ;
    oam_uint32  pon_mpcp_mc_gat_norm    ;
    oam_uint32  pon_mpcp_mc_gat_frpt    ;
    oam_uint32  pon_mpcp_mc_gat_udis    ;
    oam_uint32  pon_mpcp_mc_gat_bdis    ;
    oam_uint32  pon_mpcp_bc_rev_cnt     ;
    oam_uint32  pon_mpcp_bc_reg_ack     ;
    oam_uint32  pon_mpcp_bc_reg_nack    ;
    oam_uint32  pon_mpcp_bc_reg_dereg   ;
    oam_uint32  pon_mpcp_bc_reg_rereg   ;
    oam_uint32  pon_mpcp_bc_reg_reg_rev ;
    oam_uint32  pon_mpcp_bc_gat_rev     ;
    oam_uint32  pon_mpcp_bc_gat_norm    ;
    oam_uint32  pon_mpcp_bc_gat_frpt    ;
    oam_uint32  pon_mpcp_bc_gat_udis    ;
    oam_uint32  pon_mpcp_bc_gat_bdis    ;
    oam_uint32  pon_mpcp_rx_oam    ;
    oam_uint32  pon_mpcp_tx_req_dereg   ;
    oam_uint32  pon_mpcp_tx_req_reg     ;
    oam_uint32  pon_mpcp_tx_ack_ack     ;
    oam_uint32  pon_mpcp_tx_ack_nack    ;
    oam_uint32  pon_mpcp_tx_rpt    ;
    oam_uint32  pon_mpcp_tx_oam    ;
    oam_uint32  pon_mpcp_da_mc_nslf;
    oam_uint32  pon_mpcp_da_mc_slf ;
    oam_uint32  pon_mpcp_da_uc     ;
    oam_uint32  pon_txoversizecnt  ;
    oam_uint32  pon_rx1519_maxcnt  ;
    oam_uint32  pon_tx1519_maxcnt  ;
    oam_uint32  pon_txpausecnt     ;
    oam_uint32  pon_undersizecnt   ;
    oam_uint32  pon_jabbercnt      ;
    oam_uint32  pon_extensioncnt   ;
    oam_uint32  pon_txextensioncnt ;
    oam_uint32  pon_err_symbol     ;
    oam_uint32  pon_outofsync      ;
    oam_uint32  pon_sld_err        ;
    oam_uint32  pon_crc8_err       ;
    oam_uint32  pon_mac_drop       ;
    oam_uint32  pon_ipg_drop       ;
    oam_uint32  pon_drain          ;
    oam_uint32  pon_tx_crc_err     ;
    oam_uint32  pon_rx_fcs_err     ;
    oam_uint32  pon_rx_green       ;
    oam_uint32  pon_tx_green       ;
} oam_pon_stats_t;

typedef struct {
    oam_uint64 rxucfrm_cnt                 ;
    oam_uint64 rxmcfrm_cnt                 ;
    oam_uint64 rxbcfrm_cnt                 ;
    oam_uint64 rxfrmgreen_cnt              ;
    oam_uint64 rxoamfrm_cnt                ;
    oam_uint64 rxpausefrm_cnt              ;
    oam_uint64 rxunknownopfrm_cnt          ;
    oam_uint64 rxcrcerrfrm_cnt             ;
    oam_uint64 rxundersizefrm_cnt          ;
    oam_uint64 rxruntfrm_cnt               ;
    oam_uint64 rxoversizefrm_cnt           ;
    oam_uint64 rxjabberfrm_cnt             ;
    oam_uint64 rxstatsfrm64_cnt            ;
    oam_uint64 rxstatsfrm65_127_cnt        ;
    oam_uint64 rxstatsfrm128_255_cnt       ;
    oam_uint64 rxstatsfrm256_511_cnt       ;
    oam_uint64 rxstatsfrm512_1023_cnt      ;
    oam_uint64 rxstatsfrm1024_1518_cnt     ;
    oam_uint64 rxstatsfrm1519_Max_cnt      ;
    oam_uint64 rxbyte_cnt                  ;
    oam_uint64 rxfrm_cnt                   ;
    oam_uint64 rxerror_cnt                 ;
    
    oam_uint64 txucfrm_cnt                 ;
    oam_uint64 txmcfrm_cnt                 ;
    oam_uint64 txbcfrm_cnt                 ;
    oam_uint64 txfrmgreen_cnt              ;
    oam_uint64 txoamfrm_cnt                ;
    oam_uint64 txpausefrm_cnt              ;
    oam_uint64 txcrcerrfrm_cnt             ;
    oam_uint64 txoversizefrm_cnt           ;

    oam_uint64 txstatsfrm64_cnt            ;
    oam_uint64 txstatsfrm65_127_cnt        ;
    oam_uint64 txstatsfrm128_255_cnt       ;
    oam_uint64 txstatsfrm256_511_cnt       ;
    oam_uint64 txstatsfrm512_1023_cnt      ;
    oam_uint64 txstatsfrm1024_1518_cnt     ;
    oam_uint64 txstatsfrm1519_Max_cnt      ;
    oam_uint64 txbyte_cnt                  ;
    oam_uint64 txfrm_cnt                   ;
    oam_uint64 txerror_cnt                 ;

    oam_uint32 alignment_errors            ;
    oam_uint32 single_collision            ;
    oam_uint32 multiple_collision          ;
    oam_uint32 deferred_transmissions      ;
    oam_uint32 foam_errors                 ;
    oam_uint32 late_collisions             ;
    oam_uint32 excessive_collisions        ;
    oam_uint32 internal_mac_rx_errors      ;
    oam_uint32 internal_mac_tx_errors      ;
    oam_uint32 symbol_errors               ;

}oam_uni_stats64_t;

#endif

