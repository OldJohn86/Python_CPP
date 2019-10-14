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

#ifndef __OAM_DPOE_PDU_H__
#define __OAM_DPOE_PDU_H__

#ifdef HAVE_DPOE_OAM
#include "oam_std_pdu.h"
#include "oam_types.h"
#include "oam_main.h"

/* DPOE ext opcode definitions */
#define OAM_PDU_DPOE_VAR_REQ                0x01
#define OAM_PDU_DPOE_VAR_RSP                0x02
#define OAM_PDU_DPOE_SET_REQ                0x03
#define OAM_PDU_DPOE_SET_RSP                0x04
#define OAM_PDU_DPOE_IP_MCAST_CTRL          0x05
#define OAM_PDU_DPOE_MCAST_REG              0x06
#define OAM_PDU_DPOE_MCAST_REG_RSP          0x07
#define OAM_PDU_DPOE_KEY_EXCHANGE           0x08
#define OAM_PDU_DPOE_FILE_TRANS             0x09
#define OAM_PDU_DPOE_IP_MCAST_CTRL_RSP      0x0A


/* DPOE set response codes */
#define OAM_DPOE_RSP_OK                     0x80
#define OAM_DPOE_RSP_TOO_LONG               0x81
//#define OAM_DPOE_RSP_BAD_PARAM              0x86
#define OAM_DPOE_RSP_BAD_PARAM              OAM_DPOE_RSP_UNSUPPORT
#define OAM_DPOE_RSP_NO_RES                 0x87
#define OAM_DPOE_RSP_SYS_BUSY               0x88
#define OAM_DPOE_RSP_UNKNOWN                0xA0
#define OAM_DPOE_RSP_UNSUPPORT              0xA1
#define OAM_DPOE_RSP_CORRUPTED              0xA2
#define OAM_DPOE_RSP_HW_FAILURE             0xA3
#define OAM_DPOE_RSP_OVERFLOW               0xA4

/* DPOE Extended Object Class */
#define OAM_DPOE_OBJICT_BRANCH              0xD6
#define OAM_DPOE_ATTRIB_BRANCH              0xD7
#define OAM_DPOE_ACTION_BRANCH              0xD9


/* DPOE Extended MO Instance */
#define OAM_DPOE_LEAF_OBJ_ONU               0x0000
#define OAM_DPOE_LEAF_OBJ_PON               0x0001
#define OAM_DPOE_LEAF_OBJ_LLID              0x0002
#define OAM_DPOE_LEAF_OBJ_UNI               0x0003
#define OAM_DPOE_LEAF_OBJ_QUEUE             0x0004

/* queue type in queue object */
#define OAM_DPOE_QUEUE_TYPE_UNI             0x0003
#define OAM_DPOE_QUEUE_TYPE_LLID            0x0002

/* the action for mc control */
#define OAM_DPOE_MC_CTRL_ACTION_ADD     0x00    /* Add UNI ports to the group forwarding list */
#define OAM_DPOE_MC_CTRL_ACTION_RMV     0x01    /* Remove UNI ports to the group forwarding list */
#define OAM_DPOE_MC_CTRL_ACTION_RMV_DER 0x02    /* Remove all ports and deregister the mLLID */

/* the flag for mc register */
#define OAM_DPOE_MC_REG_FLAG_REG        0x01    /* (Re)Register */
#define OAM_DPOE_MC_REG_FLAG_DEALLOC    0x02    /* Deallocate */
#define OAM_DPOE_MC_REG_FLAG_SUCCESS    0x03    /* Success */
#define OAM_DPOE_MC_REG_FLAG_NACK       0x04    /* NACK */

/* the location for loopback */
#define OAM_DPOE_LOOPBACK_PHY   0   /* phy */
#define OAM_DPOE_LOOPBACK_MAC   1   /* mac */
#define OAM_DPOE_LOOPBACK_PON   2   /* pon */

/* convert dpoe uni id to oam_port_id_t: DPOE UNI index is begin with 0 */
#define OAM_DPOE_UNI_PORT_ID_TO_OAM_PORT(x)     (x+1)    

typedef struct {
    oam_uint8           type;
    oam_uint8           length;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           support;
    oam_uint8           version;
} __attribute__((packed)) oam_dpoe_tlv_t;

typedef struct {
    oam_uint8           type;
    oam_uint8           length;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           event_code;
    oam_uint8           raised;
    oam_uint16          object_type;
    oam_uint32          object_instance;
} __attribute__((packed)) oam_dpoe_link_event_tlv_t;


typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           opcode;
    oam_uint8           data[1];
} __attribute__((packed)) oam_pdu_dpoe_t;

typedef struct {
    oam_uint8           branch;
    oam_uint16          leaf;
} __attribute__((packed)) oam_dpoe_onu_var_req_hdr_t;

typedef struct {
    oam_uint8           branch;
    oam_uint16          leaf;
    oam_uint8           width;
    oam_uint8           data[1];
} __attribute__((packed)) oam_dpoe_onu_var_set_hdr_t;

typedef struct {
    oam_uint8   action;
    oam_uint16  llid;
    oam_uint8   ip_sa[16];
    oam_uint8   ip_da[16];
    oam_uint8   count;
    oam_uint8   port[OAM_MAX_UNI_PORT_NUM];
} __attribute__((packed)) oam_dpoe_onu_mc_ctrl_t;

typedef struct {
    oam_uint8   flag;
    oam_uint16  mc_llid;
    oam_uint16  uc_llid;
} __attribute__((packed)) oam_dpoe_onu_mc_reg_t;

/* DPoE index of instance container */
typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           value;
} __attribute__((packed)) oam_pdu_dpoe_obj_onu_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           value;
} __attribute__((packed)) oam_pdu_dpoe_obj_pon_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           value;
} __attribute__((packed)) oam_pdu_dpoe_obj_llid_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           value;
} __attribute__((packed)) oam_pdu_dpoe_obj_uni_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint16          type;
    oam_uint8           instance;
    oam_uint8           number;
} __attribute__((packed)) oam_pdu_dpoe_obj_queue_t;

#define OAM_DPOE_ONU_ID_LEN             6
typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           onu_id[OAM_DPOE_ONU_ID_LEN];
} __attribute__((packed)) oam_dpoe_onu_id_t;

#define OAM_DPOE_MODEL_NAME_LEN             0x1f
typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           model_name[OAM_DPOE_MODEL_NAME_LEN];
} __attribute__((packed)) oam_dpoe_model_name_t;

typedef struct {
    oam_uint16          jedecid;
    oam_uint32          chip_model;
    oam_uint32          chip_ver;
} __attribute__((packed)) chip_info_t;

typedef struct {
    oam_var_cont_t      hdr;
    chip_info_t         info;
}__attribute__((packed)) oam_dpoe_chip_info_t;


typedef struct {
    oam_uint16          bootver;
    oam_uint32          bootcrc;
    oam_uint16          fwver;
    oam_uint32          fwcrc;
} __attribute__((packed)) fw_info_t;

typedef struct {
    oam_var_cont_t      hdr;
    fw_info_t           info;
}__attribute__((packed)) oam_dpoe_fw_info_t;


typedef struct {
    oam_uint16          year;
    oam_uint8           month;
    oam_uint8           day;
} __attribute__((packed)) manuday_t;

typedef struct {
    oam_var_cont_t      hdr;
    manuday_t           info;
}__attribute__((packed)) oam_dpoe_manuday_t;


typedef struct {
    oam_var_cont_t      hdr;
    oam_uint16          bidir;      /* Bidirectional */
    oam_uint16          ds_only;    /* Downstream-only */
} __attribute__((packed)) oam_dpoe_max_llid_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint16           nmb;
} __attribute__((packed)) oam_dpoe_port_nmb_t;


typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           up_queue;
    oam_uint8           up_queue_link;
    oam_uint8           up_queue_increment;
    oam_uint8           ds_queue;
    oam_uint8           ds_queue_link;
    oam_uint8           ds_queue_increment;
    oam_uint16          total_buf;
    oam_uint16          up_buf;
    oam_uint16          ds_buf;
} __attribute__((packed)) oam_dpoe_pkt_buf_t;



#define OAM_DPOE_QUEUESET_NUM_MAX       4       /* Number of Queue Sets */
#define OAM_DPOE_VALUE_PER_SET_MAX      8       /* Report Values Per Queue Set */

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           queueset_num;   /* Number of Queue Sets */
    oam_uint8           value_per_set;  /* Report Values Per Queue Set */
    oam_uint16
    report_thresh[OAM_DPOE_QUEUESET_NUM_MAX *OAM_DPOE_VALUE_PER_SET_MAX];
} __attribute__((packed)) oam_dpoe_report_thresh_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           max_rate;   /* oam pdu rate */
    oam_uint8           min_rate;   /* heartbeat rate */
} __attribute__((packed)) oam_dpoe_frame_rate_t;


typedef struct {
    oam_uint8 code[13];
    oam_uint8 cvc[13];
} __attribute__((packed)) fw_mfg_time_t;

typedef struct {
    oam_var_cont_t      hdr;
    fw_mfg_time_t       time;
} __attribute__((packed)) oam_dpoe_fw_mfg_time_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint64          value;
} __attribute__((packed)) oam_dpoe_uint64_t;


typedef struct {
    oam_var_cont_t      hdr;
    oam_uint32          value;
} __attribute__((packed)) oam_dpoe_uint32_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint16          value;
} __attribute__((packed)) oam_dpoe_uint16_t;


typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           value;
} __attribute__((packed)) oam_dpoe_uint8_t;


typedef struct {
    oam_var_cont_t      hdr;
    ca_mac_addr_t       mac[0];
} __attribute__((packed)) oam_dpoe_mac_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint16          max_cap;
    oam_uint16          curr_cap;
} __attribute__((packed)) oam_dpoe_autoneg_t;

typedef struct {
    oam_uint8           n_queue;
    oam_uint8           q[8];
} __attribute__((packed)) dpoe_queue_t;

typedef struct {
    //oam_uint8           key_op;
    //oam_uint16          llid;
    oam_uint8           number;
    oam_uint8           length;
    oam_uint8           key[16];
} __attribute__((packed)) oam_dpoe_onu_key_assign_t;

typedef struct {
    //oam_uint8           key_op;
    //oam_uint16          llid;
    oam_uint8           number;
} __attribute__((packed)) oam_dpoe_onu_key_ack_t;

typedef struct {
    oam_uint8 n_llid;
    dpoe_queue_t llid[8];
    oam_uint8 n_port;
    dpoe_queue_t port[8];
} __attribute__((packed)) oam_dpoe_llid_queue_int_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint16          tpid;
    oam_uint8           insert;
} __attribute__((packed)) oam_dpoe_tpid_t;

#define RULE_OPR_NEVER_MATCH    0
#define RULE_OPR_EQUAL          1
#define RULE_OPR_NOT_EQUAL      2
#define RULE_OPR_LESS_THAN      3
#define RULE_OPR_GREAT_THAN     4
#define RULE_OPR_EXIST          5
#define RULE_OPR_NOT_EXIST      6
#define RULE_OPR_ALWAYS_MATCH   7

typedef struct {
    oam_uint8 ind;
    oam_uint8 field;
    oam_uint8 instance;
    oam_uint8 msb;
    oam_uint8 lsb;
    oam_uint8 opr;
    oam_uint8 len;
    oam_uint8 data[0];
} __attribute__((packed)) dpoe_oam_rule_clause_t;

typedef struct {
    oam_uint8 ind;
    oam_uint8 action;
} __attribute__((packed)) dpoe_oam_rule_result_hdr_t;

typedef struct {
    dpoe_oam_rule_result_hdr_t hdr;
    oam_uint16          type;
    oam_uint8           instance;
    oam_uint8           number;
} __attribute__((packed)) dpoe_oam_rule_result_queue_t;

typedef struct {
    dpoe_oam_rule_result_hdr_t hdr;
    oam_uint8 field;
    oam_uint8 instance;
    oam_uint8 data[0];
} __attribute__((packed)) dpoe_oam_rule_result_set_t;

typedef struct {
    dpoe_oam_rule_result_hdr_t hdr;
    oam_uint8 field;
    oam_uint8 instance;
    oam_uint8 msb;
    oam_uint8 lsb;
} __attribute__((packed)) dpoe_oam_rule_result_copy_t;

typedef struct {
    dpoe_oam_rule_result_hdr_t hdr;
    oam_uint8 field;
    oam_uint8 instance;
} __attribute__((packed)) dpoe_oam_rule_result_delete_t;

typedef struct {
    dpoe_oam_rule_result_hdr_t hdr;
    oam_uint16 count_index;
} __attribute__((packed)) dpoe_oam_rule_result_inc_t;

/* service level agreementss */
typedef struct {
    oam_var_cont_t      hdr;
    oam_uint16          cbs; /* unit is 256 Bytes*/
    oam_uint32          cir; /* unit is 1 Kbps*/
} __attribute__((packed)) oam_dpoe_queue_cir_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           ds;
    oam_uint8           us;
} __attribute__((packed)) oam_dpoe_onu_fec_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint16          ebs; /* unit is 256 Bytes*/
    oam_uint32          eir; /* unit is 1 Kbps*/
} __attribute__((packed)) oam_dpoe_queue_eir_t;

typedef struct {
    oam_boolean         enable;     /* enable color marking */
    oam_uint8           field;      /* Field Code */
    oam_uint8           instance;   /* Field Instance*/
    oam_uint8           msb;        /* MSB mask, bits to ingnore */
    oam_uint8           lsb;        /* LSB mask, bits to ingnore */
    oam_uint8           green;      /* green value */
    oam_uint8           yellow;     /* yellow value */
} __attribute__((packed)) oam_dpoe_queue_mark_value_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_dpoe_queue_mark_value_t value;
} __attribute__((packed)) oam_dpoe_queue_mark_t;

typedef struct {
    oam_uint16          num_limiter;     /* Number of Rate Limiters */
    oam_uint16          cbs_increment;   /* CBS Min Increment */
    oam_uint16          cir_increment;   /* CIR Min Increment */
    oam_uint16          ebs_increment;   /* EBS Min Increment */
    oam_uint16          eir_increment;   /* EIR Min Increment */
    oam_uint8           color_aware;        /* Clolor Aware? */
    oam_uint8           coupling_config;    /* Coupling Configurable? */
    oam_uint8           coupling_behavior;  /* Coupling Behavior Default */
    oam_uint8           color_marking;      /* Color Marking Support? */
    oam_uint8           color_drop;         /* Smart Color Drop? */
} __attribute__((packed)) oam_dpoe_queue_limiter_cap_value_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_dpoe_queue_limiter_cap_value_t value;
} __attribute__((packed)) oam_dpoe_queue_limiter_cap_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8 mac[6];
}__attribute__((packed)) oam_dpoe_onu_mac_t;

#define DPOE_MC_LLID_ADD    0
#define DPOE_MC_LLID_DEL    1
#define DPOE_MC_LLID_DELALL 2
typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8           action;
    oam_uint16          llid;
}__attribute__((packed)) oam_dpoe_mc_llid_act_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8 llid_count;
    oam_uint8 uni_count;
}__attribute__((packed)) oam_dpoe_port_cfg_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8       cust;
    oam_uint8       layer;
    oam_uint8       offset_dword;
    oam_uint8       offset_lsb;
    oam_uint8       width;
    oam_uint8       refer;
} __attribute__((packed)) oam_dpoe_onu_cust_cls_t;

typedef struct {
    oam_var_cont_t      hdr;
    oam_uint8 queue_count;
    oam_uint32 size[0];
}__attribute__((packed)) oam_dpoe_queue_cfg_t;

typedef struct{
    oam_var_cont_t      hdr;
    oam_uint16             onu_max_frame_size_capability;
}__attribute__((packed)) oam_dpoe_onu_max_frame_capability_t;

typedef struct{
    oam_var_cont_t      hdr;
    oam_uint8           mac_address[6];
    oam_uint8           uni_port;
}__attribute__((packed)) oam_dpoe_uni_mac_learned_tlv_t;

typedef struct{
    oam_var_cont_t      hdr;
    oam_uint8           mac_address[6];
}__attribute__((packed)) oam_dpoe_uni_mac_learned_set_tlv_t;

typedef struct{
    oam_var_cont_t      hdr;
    oam_uint16          mc_llid[0];
}__attribute__((packed)) oam_dpoe_onu_mc_llid_tlv_t;

#endif
#endif
