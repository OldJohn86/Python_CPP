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
#ifndef __OAM_CTC_ADAPT_H__
#define __OAM_CTC_ADAPT_H__

#ifdef HAVE_CTC_OAM

#include "oam_types.h"
#include "oam_adapt.h"
#include "oam_ctc.h"
#include "oam_ctc_pdu.h"

#define CA_PORT_CLS_RULE_MAX 0x08
#define CLASS_RULES_MAX 0XFF
#define CLASS_FIELD_SELECT_MAX 0x12
#define CLASS_MATCH_VAL_LEN 6
#define CLASS_RULES_FSELECT_IPVER 0x0C
#define CA_PON_PORT 0
#define MAX_OAM_PORTS 4

#define PORT_BIT(idx) ( 1 << idx ) 
#define TAG2VID(tag)    ((tag)&0x0fff)
#define TAG2PRI(tag)    (((tag)&0xe000)>>13)

/*oam uni port number start from 1, SAL port number start from 3*/
#define __SAL_ETH_PORT(port) ((port) + 2)
#define __OAM_ETH_PORT(port) (((port) > 0)?((port) - 2) : 0)

/* UNI ports */
#define __PORT_CHECK(port) \
        if((port) < OAM_UNI_PORT_ID1 || (port) >= (OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM)) { \
            OAM_ERROR("port id invalid. %d? port num = %d\r\n",  port, OAM_UNI_PORT_NUM); \
            return OAM_E_ERROR; \
        }

typedef enum {
    CTC_INTER_TYPE_GE = 0,
    CTC_INTER_TYPE_FE,
    MAX_INTER_TYPE
} ctc_interface_type_e;

typedef enum {
    TRANSID_MAJOR   = 0,
    TRANSID_STANDBY = 1,
    TRANSID_BOTH    = 2
} oam_transid;


#define CTC_DEFAULT_TPID 0x8100
#define CTC_MCVLAN_OPER_DEL 0
#define CTC_MCVLAN_OPER_ADD 1
#define CTC_MCVLAN_OPER_CLR 2
#define CTC_MCVLAN_OPER_GET 3

typedef enum {
    CTC_INGR_FRM_UNTAG = 0,
    CTC_INGR_FRM_SNGLTAG,
    CTC_INGR_FRM_DBLTAG,
    MAX_CTC_INGR_FRM
} ctc_ingr_frame_tag_e;

typedef enum {
    CTC_MCVLAN_TAG_OPER_NONSTRIP,
    CTC_MCVLAN_TAG_OPER_STRIP,
    CTC_MCVLAN_TAG_OPER_SWAP,
    MAX_CTC_MCVLAN_TAG_OPER
} ctc_mc_tagoper_type_e;

typedef struct ctc_classification_rule_s {
    ca_uint8_t precedence;
    ca_uint8_t queueMapped;
    ca_uint8_t priMark;
    ca_uint8_t entries;
    oam_ctc_onu_class_fselect_t fselect[CLASS_FIELD_SELECT_MAX];
} ctc_classification_rule_t;

typedef struct ctc_cls_node_s {
    struct ctc_cls_node_s *nxt;
    ca_int32_t cls_idx;
    ctc_classification_rule_t cls_rule;
} ctc_cls_node_t;

typedef struct {
    ctc_cls_node_t *head;
    ctc_cls_node_t *tail;
    ca_uint32_t rule_cnt;
    ca_uint32_t dflt_untg_rule_idx;
    ca_uint32_t dflt_sngl_rule_idx;
} ctc_oam_port_cls_t; 

typedef struct {
    oam_ip_t        mgmt_ip;
    oam_uint32      mask;
    oam_ip_t        mgmt_gateway;
    oam_uint16      mgmt_svlan;
    oam_uint16      mgmt_cvlan;
    oam_uint8       mgmt_data_pri;
} ctc_oam_mgmt_parm_t;

typedef struct {
    oam_uint8       oper;
    oam_uint32      cir;
    oam_uint32      cbs;
    oam_uint32      ebs;
} ctc_port_us_policy_t;

typedef struct {
    oam_uint8       oper;
    oam_uint32      cir;
    oam_uint32      pir;
} ctc_port_ds_rate_limit_t;


typedef struct {
    ca_uint32_t outer_pbits :1;
    ca_uint32_t inner_pbits :1;
    ca_uint32_t outer_tpid :1;
    ca_uint32_t inner_tpid :1;
    ca_uint32_t outer_dei :1;
    ca_uint32_t inner_dei :1;
    ca_uint32_t outer_vid :1;
    ca_uint32_t inner_vid :1;
} oam_ctc_vlan_key_type_t;


typedef struct {
    oam_ctc_vlan_key_type_t key_type;   /*vlan key type*/
    ca_uint8_t outer_pbits;    /*outer pbits for key matching*/
    ca_uint8_t inner_pbits;    /*innner pbits for key matching*/
    ca_uint16_t outer_tpid; /*outer tpid for key matching*/
    ca_uint16_t inner_tpid;  /*inner tpid for key matching*/
    ca_uint16_t outer_dei;  /*outer dei for key matching*/
    ca_uint16_t inner_dei;    /*inner dei for key matching*/
    ca_uint16_t outer_vid;  /*outer vid for key matching*/
    ca_uint16_t inner_vid;    /*inner vid for key matching*/
} oam_ctc_vlan_key_entry_t;



typedef struct {
    oam_uint16 agg_vlan_num;
    oam_ctc_onu_vlan_tag_t agg_dst_vlan;
    oam_ctc_onu_vlan_tag_t agg_vlan[CTC_ONU_AGGR_VLAN_NUM];
    oam_ctc_vlan_key_entry_t agg_key[CTC_ONU_AGGR_VLAN_NUM];
} ctc_oam_agg_vlan_t;

typedef struct {
    oam_uint16 valid;
    ctc_mc_tagoper_type_e tagOper;
    oam_ctc_iptv_vid_t iptvVlans;
} ctc_oam_mc_tagOper_ent_t;
typedef struct {
    oam_uint8                       admin_status;
    ctc_interface_type_e inter_type;
    oam_uint8                       link_status;
    oam_uint32                      lnk_sts_chng_cnt;
    oam_uint8                       work_mode;
    oam_uint8                       auto_neg_mode;
    oam_uint8                       pause;
    oam_uint8                       loopdected;
    oam_uint8                       mc_tag_strip;
    oam_uint16                      mc_max_group;
    ctc_port_us_policy_t            policy;
    ctc_port_ds_rate_limit_t        ds_rate;

    oam_uint8                       vlan_mode;
    oam_ctc_onu_vlan_tag_t          def_vlan_tag;
    oam_uint16                      translate_num;
    oam_ctc_onu_vlan_xlate_entry    translate_vlan[CTC_VLAN_MODE_TRANSLATE_MAX];
    oam_ctc_vlan_key_entry_t translate_key[CTC_VLAN_MODE_TRANSLATE_MAX];
    oam_uint16                      trunk_num;
    oam_ctc_onu_vlan_tag_t          trunk_vlan[CTC_VLAN_MODE_TRUNK_MAX];
    oam_uint16                      aggr_num;
    ctc_oam_agg_vlan_t aggr_vlan[CTC_ONU_AGGR_VLAN_MAX];
    oam_uint16 mc_iptvVlans_num;
    ctc_oam_mc_tagOper_ent_t mc_iptvVlans[ 8 ];
} ctc_port_info_t;



oam_status ctc_oam_eth_admin_state_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin);

oam_status ctc_oam_eth_auto_admin_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin);

oam_status ctc_oam_eth_auto_local_cap_get_adapt(
    oam_port_id_t port,
    oam_uint32 *value,
    oam_uint8 *getNum);

oam_status ctc_oam_eth_auto_advts_cap_get_adapt(
    oam_port_id_t port,
    oam_uint32 *value,
    oam_uint8 *getNum);

oam_status ctc_oam_eth_fec_ability_get_adapt(
    oam_uint8 *ability);

oam_status ctc_oam_eth_fec_mode_get_adapt(
    oam_uint32 *fecMode);

oam_status ctc_oam_eth_fec_mode_set_adapt(
    oam_uint32 fecMode);

oam_status ctc_oam_onu_opmdiag_get_adapt(
    oam_int16 *temp,
    oam_uint16 *volt,
    oam_uint16 *tx_bias,
    oam_uint16 *tx_power,
    oam_uint16 *rx_power);

oam_status ctc_oam_sla_get_adapt(
    oam_uint8   *oper,
    oam_uint8   *scheduling_scheme,
    oam_uint8   *high_pri_boundary,
    oam_uint32 *cycle_len,
    oam_uint8   *num_of_service,
    oam_sla_queue_t *sla);

oam_status ctc_oam_sla_set_adapt(
    oam_uint8   oper,
    oam_uint8   scheduling_scheme,
    oam_uint8   high_pri_boundary,
    oam_uint32  cycle_len,
    oam_uint8   num_of_service,
    oam_sla_queue_t *sla);


oam_status ctc_oam_holdover_get_adapt(
    oam_uint32 *holdover_state,
    oam_uint32 *holdover_time);

oam_status ctc_oam_holdover_set_adapt(
    oam_uint32 holdover_state,
    oam_uint32 holdover_time);


oam_status ctc_oam_onu_mgmt_config_get_adapt(
    ctc_oam_mgmt_parm_t *mgmt);

oam_status ctc_oam_onu_mgmt_config_set_adapt(
    ctc_oam_mgmt_parm_t *parm);

oam_status ctc_oam_onu_active_pon_get_adapt(
    oam_uint8 *pon);

oam_status ctc_oam_onu_active_pon_set_adapt(
    oam_uint8 pon);

oam_status ctc_oam_eth_port_link_get_adapt(
    oam_port_id_t port,
    oam_uint8  *linkState);

oam_status ctc_oam_eth_port_pause_get_adapt(
    oam_port_id_t port,
    oam_uint8  *flowControl);

oam_status ctc_oam_eth_port_pause_set_adapt(
    oam_port_id_t port,
    oam_uint8 flowControl);

oam_status ctc_oam_eth_port_policing_get_adapt(
    oam_port_id_t port,
    oam_uint8 *oper,
    oam_uint32 *cir,
    oam_uint32 *cbs,
    oam_uint32 *ebs);

oam_status ctc_oam_eth_port_policing_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  cbs,
    oam_uint32  ebs);

oam_status ctc_oam_eth_ds_rate_limit_get_adapt(
    oam_port_id_t port,
    oam_uint8 *oper,
    oam_uint32 *cir,
    oam_uint32 *pir);

oam_status ctc_oam_eth_ds_rate_limit_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  pir);

oam_status ctc_oam_loop_detect_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status) ;

oam_status ctc_oam_loop_detect_status_get_adapt(
    oam_port_id_t port,
    oam_uint32 *status) ;

oam_status ctc_oam_disable_looped_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status) ;

oam_status ctc_oam_eth_mac_aging_time_get_adapt(
    oam_uint32 *time) ;

oam_status ctc_oam_eth_mac_aging_time_set_adapt(
    oam_uint32 time) ;

oam_status ctc_oam_onu_mac_limit_get_adapt(
    oam_port_id_t port,
    oam_uint16  *num);

oam_status ctc_oam_onu_mac_limit_set_adapt(
    oam_port_id_t port,
    oam_uint16  num);

oam_status ctc_oam_onu_port_mode_set_adapt(
    oam_port_id_t port,
    oam_uint8  mode);

oam_status ctc_oam_onu_port_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8  *mode);
oam_status ctc_oam_onu_port_work_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8  *mode);

oam_status ctc_oam_onu_port_stats_get_adapt(
    oam_port_id_t port,
    oam_ctc_eth_port_stats_t *stats);

oam_status ctc_oam_pon_port_stat_get_adapt(
    oam_ctc_onu_llid_stats1 *stats1,
    oam_ctc_onu_llid_stats2 *stats2);

oam_status ctc_oam_onu_cfg_counter_get_adapt(oam_uint32 *counter);

oam_status ctc_oam_onu_cfg_counter_set_adapt(oam_uint32 counter);

oam_status ctc_oam_onu_cfg_mgmt_set_adapt(oam_uint8);

oam_status ctc_oam_onu_default_vlan_tag_get_adapt(
    oam_port_id_t port,
    oam_uint16 *tpid,
    oam_uint16 *tag);

oam_status ctc_oam_onu_vlan_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8 *mode);

oam_status ctc_oam_onu_translate_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *translate_num,
    oam_ctc_onu_vlan_xlate_entry *vlan);

oam_status ctc_oam_onu_trunk_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *trunk_num,
    oam_ctc_onu_vlan_tag_t *vlan);

oam_status ctc_oam_onu_agg_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *agg_num,
    ctc_oam_agg_vlan_t *vlan);

oam_status ctc_oam_onu_transparent_vlan_set_adapt(
    oam_port_id_t port);

oam_status ctc_oam_onu_tag_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t vlan);

oam_status ctc_oam_onu_translate_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_xlate_entry *vlan);

oam_status ctc_oam_onu_trunk_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_tag_t *vlan);

oam_status ctc_oam_onu_agg_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    ctc_oam_agg_vlan_t *vlan);

oam_status ctc_oam_onu_classification_get_adapt(
    oam_port_id_t port,
    oam_uint32 *num,
    oam_uint8 *data);

oam_status ctc_oam_onu_classification_set_adapt(
    oam_uint8 action,
    oam_port_id_t port,
    oam_uint32 num,
    oam_uint8 *data);

oam_status ctc_oam_onu_mcvlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *num,
    oam_uint16 *vlanIds);

oam_status ctc_oam_onu_mcvlan_set_adapt(
    oam_port_id_t port,
    oam_uint8 oper,
    oam_uint16 num,
    oam_uint16 *vlanIds);

oam_status ctc_oam_onu_mctagstrip_get_adapt(
    oam_port_id_t port,
    oam_uint8 *tagStriped,
    oam_uint8 *num,
    oam_ctc_iptv_vid_t *vlans);

oam_status ctc_oam_onu_mctagstrip_set_adapt(
    oam_port_id_t port,
    oam_uint8  tagStriped,
    oam_uint8  num,
    oam_ctc_iptv_vid_t *vlans);

oam_status ctc_oam_onu_mc_switch_get_adapt(
    oam_uint8 *mode);

oam_status ctc_oam_onu_mc_switch_set_adapt(
    oam_uint8 mode);

oam_status ctc_oam_onu_mc_control_type_get_adapt(
    oam_uint8 *type);

oam_status ctc_oam_onu_mc_control_type_set_adapt(
    oam_uint8  type);

oam_status ctc_oam_onu_mc_control_vlan_mac_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_vlan_mac_t *vlanMac);

oam_status ctc_oam_onu_mc_control_mac_ip_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_mac_ip_t *macIp);

oam_status ctc_oam_onu_mc_control_vlan_ip_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIp);

oam_status ctc_oam_onu_mc_control_clr_adapt();

oam_status ctc_oam_onu_mc_control_vlan_mac_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_vlan_mac_t *vlanMac);

oam_status ctc_oam_onu_mc_control_mac_ip_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_mac_ip_t *macIp);

oam_status ctc_oam_onu_mc_control_vlan_ip_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIp);

oam_status ctc_oam_onu_grp_nmb_max_get_adapt(
    oam_port_id_t port,
    oam_uint8   *maxGrp);

oam_status ctc_oam_onu_grp_nmb_max_set_adapt(
    oam_port_id_t port,
    oam_uint8   maxGrp);

oam_status ctc_oam_onu_fast_leave_ability_get_adapt(
    oam_uint32 *num,
    oam_uint32 *mode);

oam_status ctc_oam_onu_fast_leave_state_get_adapt(
    oam_uint32 *state);

oam_status ctc_oam_onu_llid_queue_config_get_adapt (
    oam_llid_t llid,
    oam_uint8 *num,
    oam_ctc_onu_llid_config_t *config);

oam_status ctc_oam_onu_llid_queue_config_set_adapt (
    oam_llid_t llid,
    oam_uint8  num,
    oam_ctc_onu_llid_config_t *config);

oam_status ctc_oam_onu_alarm_get_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32 *alarm_config);

oam_status ctc_oam_onu_alarm_set_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  alarm_config);

oam_status ctc_oam_onu_alarm_threshold_get_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  *alarm_threshold,
    oam_uint32  *clear_threshold);

oam_status ctc_oam_onu_alarm_threshold_set_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  raise_threshold,
    oam_uint32  clear_threshold);

oam_status ctc_oam_onu_laser_control_set_adapt(
    oam_uint16 action,
    oam_uint8 *onuId,
    oam_uint32  transId);

oam_status ctc_oam_eth_admin_state_set_adapt(
    oam_port_id_t port,
    oam_uint32 phyAdmin);

oam_status ctc_oam_auto_neg_restart_set_adapt(
    oam_port_id_t port);

oam_status ctc_oam_auto_neg_ctrl_set_adapt(
    oam_port_id_t port,
    oam_uint32 autoNegAdmin);

oam_status ctc_oam_onu_reset_set_adapt();

oam_status ctc_oam_fast_leave_ctrl_set_adapt(
    oam_uint32 fastLeaveAdmin);

oam_status ctc_oam_onu_sleep_control_set_adapt(
    oam_uint32  sleep_duration,
    oam_uint32  wait_duration,
    oam_uint8   sleep_flag,
    oam_uint8   sleep_mode);

oam_status ctc_oam_onu_stats_monitor_status_get_adapt(
    oam_port_id_t   port,
    oam_uint16      *monitor_status,
    oam_uint32      *monitor_period);

oam_status ctc_oam_onu_stats_monitor_status_set_adapt(
    oam_port_id_t   port,
    oam_uint16      monitor_status,
    oam_uint32      monitor_period);

oam_status ctc_oam_onu_curr_stats_get_adapt(
    oam_port_id_t               port,
    oam_ctc_onu_port_stats_t    *stats);

oam_status ctc_oam_onu_curr_stats_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats);

oam_status ctc_oam_onu_histroy_stats_get_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats);

oam_status ctc_oam_onu_power_saving_cap_get_adapt(
    oam_uint8 *sleep_mode,
    oam_uint8 *early_wakeup);

oam_status ctc_oam_onu_power_saving_config_get_adapt(
    oam_uint8 *early_wakeup,
    oam_uint64 *max_sleep_duration);

oam_status ctc_oam_onu_power_saving_config_set_adapt(
    oam_uint8 early_wakeup,
    oam_uint64 max_sleep_duration);

oam_status ctc_oam_onu_pon_protect_parm_get_adapt(
    oam_uint16 *opt_time,
    oam_uint16 *mpcp_time);

oam_status ctc_oam_onu_pon_protect_parm_set_adapt(
    oam_uint16 opt_time,
    oam_uint16 mpcp_time);

oam_status ctc_oam_onucap_get_adapt(
    oam_ctc_onu_capabilities_t *onu_cap);

oam_status ctc_oam_event_status_get_adapt(
    oam_ctc_event_entry_t entry,
    oam_uint32 *status);

oam_status ctc_oam_event_thresh_set_adapt(
    oam_ctc_event_thresh_t config);

oam_status ctc_oam_event_status_set_adapt(
    oam_ctc_event_status_t config);

oam_status ctc_oam_event_thresh_get_adapt(
    oam_ctc_event_entry_t entry,
    oam_uint32 *raise_thresh,
    oam_uint32 *clear_thresh);

oam_status ctc_oam_loid_passwd_get_adapt(oam_uint8 *loid, oam_uint8 *passwd);

oam_status ctc_oam_auth_keep_silence_set_adapt(oam_uint8 llid_index,
        oam_boolean enable);

oam_status ctc_oam_laser_alarm_status_get(oam_uint8 *status);

oam_status ctc_oam_up_congest_status_get(oam_port_id_t port, oam_boolean *status);

oam_status ctc_oam_sn_get_adapt(oam_ctc_onu_sn_t *sn);

oam_status ctc_oam_fw_get_adapt(oam_uint8 *fw_ver, oam_uint8 *len);

oam_status ctc_oam_chip_id_get_adapt(oam_ctc_onu_chip_id_t *chip_id);

oam_status ctc_oam_onucap2_get_adapt(oam_ctc_onu_capabilities2_t *onu_cap2);

oam_status ctc_oam_onucap3_get_adapt(oam_ctc_onu_capabilities3_t *onu_cap3);

oam_status ctc_oam_keep_silence_time_get_adapt(oam_uint8 *time);

oam_status ctc_oam_enc_key_get_adapt(oam_uint8 llid_index, oam_uint8 key_index,
                                     oam_uint8 *key, oam_uint8 *key_len);

oam_status ctc_oam_onu_mpcp_queue_thresh_get_adapt(
    oam_uint8 index, oam_uint16 *threshold);

oam_status ctc_oam_onu_mpcp_queue_thresh_set_adapt(
    oam_uint8 index, oam_uint16 threshold);

oam_status ctc_oam_onu_pon_stat_get_adapt(oam_ctc_onu_port_stats_t *stats);

oam_status ctc_oam_onu_uni_stat_get_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats);

void ctc_oam_power_saving_init();

void ctc_oam_power_saving_reset();

oam_status ctc_oam_oui_get_adapt(oam_oui_t oui);

oam_status ctc_oam_multi_llid_admin_ctrl_set_adapt(oam_uint32 llid_num);

oam_uint8 ctc_oam_swimage_id_get_adapt();

oam_uint8 ctc_oam_swimage_flash_state_get_adapt();

oam_status ctc_oam_swimage_open_adapt(oam_uint8 img_id);

oam_status ctc_oam_swimage_write_adapt(
    oam_uint8 img_id,
    oam_uint8 *buf,
    oam_uint16 len);

oam_status ctc_oam_swimage_close_adapt(oam_uint8 img_id);

oam_status ctc_oam_swimage_image_ready(oam_uint8 img_id);

oam_status ctc_oam_swimage_active_adapt();

oam_status ctc_oam_swimage_commit_adapt();

oam_status ctc_oam_eth_port_alarm_reg_adapt();

oam_status ctc_oam_ld_alarm_reg_adapt();

oam_status ctc_oam_opm_alarm_reg_adapt();

oam_status ctc_oam_gpio_alarm_reg_adapt();

void ctc_oam_init_adapt ();
#endif

#endif
