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

#ifdef HAVE_CTC_OAM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oam_types.h"

#include "oam_std_pdu.h"
#include "oam_ctc_pdu.h"
#include "oam_ctc_client.h"
#include "oam_ctc_adapt.h"
#include "oam_ctc_stats.h"
#include "oam_ctc_alarm.h"

#define TAG2VID(tag)    ((tag)&0x0fff)
#define TAG2PRI(tag)    (((tag)&0xe000)>>13)

extern oam_int32 onu_evt_reg(oam_int32 evt_type , void *func , void *cookie);

#ifdef ZTE_SPECIFIC
oam_status ctc_oam_cls_syn_db ( oam_port_id_t port);
oam_status ctc_oam_mc_vlan_syn_db ( oam_port_id_t port);
#endif/*END_ZTE_SPECIFIC*/

void oam_port_event_register_adapt (
    void                        *call_back)
{
    return ;
}



/***************** SDL ADAPT START ****************************/
oam_status ctc_oam_eth_admin_state_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin)
{
    return OAM_E_OK;
}


//duchen: OAM_INTF_PHY_ADMIN_DIS?????
oam_status ctc_oam_eth_auto_admin_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin)
{
    return OAM_E_OK;
}

oam_status ctc_oam_eth_auto_local_cap_get_adapt(
    oam_port_id_t port,
    oam_uint32 *value,
    oam_uint8 *getNum)
{
    return OAM_E_OK;
}



oam_status ctc_oam_eth_auto_advts_cap_get_adapt(
    oam_port_id_t port,
    oam_uint32 *value,
    oam_uint8 *getNum)
{
    return OAM_E_OK;
}

oam_status ctc_oam_eth_fec_ability_get_adapt(
    oam_uint8 *ability)
{
    oam_status                 retCode;

    return retCode;

}

oam_status ctc_oam_eth_fec_mode_set_adapt(
    oam_uint32 fecMode)
{
    oam_status                retCode;

    return retCode;

}

oam_status ctc_oam_eth_fec_mode_get_adapt(
    oam_uint32 *fecMode)
{
    oam_status                 retCode;

    return retCode;
}




oam_status ctc_oam_onu_active_pon_get_adapt(
    oam_uint8 *pon)
{
    *pon = 1;
    return OAM_E_OK;
}

oam_status ctc_oam_onu_active_pon_set_adapt(
    oam_uint8 pon)
{
    return OAM_E_NOT_SUPPORT;
}

oam_status ctc_oam_eth_port_link_get_adapt(
    oam_port_id_t port,
    oam_uint8  *linkState)
{
    return OAM_E_OK;

}


oam_status ctc_oam_eth_port_pause_get_adapt(
    oam_port_id_t port,
    oam_uint8  *flowControl)
{
    oam_status ret;

    return ret;

}

oam_status ctc_oam_eth_port_pause_set_adapt(
    oam_port_id_t port,
    oam_uint8 flowControl)
{
    oam_status ret;

    return ret;

}



oam_status ctc_oam_eth_admin_state_set_adapt(
    oam_port_id_t port,
    oam_uint32 phyAdmin)
{
    oam_status ret;

    return ret;
}


oam_status ctc_oam_auto_neg_restart_set_adapt(
    oam_port_id_t port)
{
    oam_status ret;

    return ret;
}

oam_status ctc_oam_auto_neg_ctrl_set_adapt(
    oam_port_id_t port,
    oam_uint32 autoNegAdmin)
{
    oam_status ret;

    return ret;
}

oam_status ctc_oam_eth_port_policing_get_adapt(
    oam_port_id_t port,
    oam_uint8  *oper,
    oam_uint32 *cir,
    oam_uint32 *cbs,
    oam_uint32 *ebs)
{
    return OAM_E_OK;

}

oam_status ctc_oam_eth_port_policing_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  cbs,
    oam_uint32  ebs)
{
    oam_status ret = OAM_E_OK;

    return ret;

}


oam_status ctc_oam_eth_ds_rate_limit_get_adapt(
    oam_port_id_t port,
    oam_uint8 *oper,
    oam_uint32 *cir,
    oam_uint32 *pir)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_ds_rate_limit_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  pir)
{
    oam_status                   ret;

    return ret;
}

oam_status ctc_oam_onu_mac_limit_set_adapt(
    oam_port_id_t port,
    oam_uint16  num)
{
    oam_status rt = OAM_E_OK;

    return rt;
}

oam_status ctc_oam_onu_mac_limit_get_adapt(
    oam_port_id_t port,
    oam_uint16  *num)
{
    oam_status rt = OAM_E_OK;

    return rt;

}


oam_status ctc_oam_onu_port_mode_set_adapt(
    oam_port_id_t port,
    oam_uint8  mode)
{
    oam_status ret;

    return ret;

}


oam_status ctc_oam_onu_port_stats_get_adapt(
    oam_port_id_t port,
    oam_ctc_eth_port_stats_t *stats)
{
    return OAM_E_OK;
}


oam_status ctc_oam_pon_port_stat_get_adapt(
    oam_ctc_onu_llid_stats1 *stats1,
    oam_ctc_onu_llid_stats2 *stats2)
{
    return OAM_E_OK;
}
oam_status ctc_oam_onu_port_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8  *mode)
{

    oam_status ret;

    return ret;
}


oam_status ctc_oam_onu_port_work_mode_get_adapt(
    oam_port_id_t portId,
    oam_uint8  *mode)
{
    oam_status rc = OAM_E_OK;

    return rc;
}

oam_status ctc_oam_eth_mac_aging_time_get_adapt(
    oam_uint32 *time)
{
    return OAM_E_OK;
}

oam_status ctc_oam_eth_mac_aging_time_set_adapt(
    oam_uint32 time)
{
    oam_status ret = OAM_E_OK;

    return ret;

}



oam_status ctc_oam_onu_fast_leave_ability_get_adapt(
    oam_uint32 *num,
    oam_uint32 *mode)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_fast_leave_state_get_adapt(
    oam_uint32 *state)
{
    return OAM_E_OK;
}

oam_status ctc_oam_fast_leave_ctrl_set_adapt(
    oam_uint32 fastLeaveAdmin)
{
    oam_status   ret = OAM_E_OK;

    return ret;
}



oam_status ctc_oam_onucap_get_adapt(
    oam_ctc_onu_capabilities_t *onu_cap)
{
    oam_uint8 hasBattery = 1;
    oam_uint64 portBitmap = 0;
    oam_uint8 i = 0;

    hasBattery = 0;

    for (i = 0; i < CTC_MAX_ONU_UNI_PORTS; i++) {
        portBitmap |= (1 << i);
    }

#ifdef HAVE_MPORTS
    onu_cap->serviceSupported = OAM_CTC_CHIP_CAP_FE_SUPP;
    onu_cap->numGEPorts = 0;
    onu_cap->geBitMap = 0;

    onu_cap->numFEPorts = CTC_MAX_ONU_UNI_PORTS;
    onu_cap->feBitMap = htonll(portBitmap);
#else
    onu_cap->serviceSupported = OAM_CTC_CHIP_CAP_GE_SUPP;
    onu_cap->numGEPorts = CTC_MAX_ONU_UNI_PORTS;
    onu_cap->geBitMap = htonll(portBitmap);
    onu_cap->numFEPorts = 0;
    onu_cap->feBitMap = 0;
#endif
    onu_cap->numPOTSPorts = 0;
    onu_cap->numE1Ports = 0;
    onu_cap->numUSQueues = 8;
    onu_cap->maxQueueUSPort = 8;
    onu_cap->numDSQueues = 4;
    onu_cap->maxQueueDSPort = 4;
    onu_cap->BatteryBackup = hasBattery;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_llid_queue_config_get_adapt (
    oam_llid_t llid,
    oam_uint8 *num,
    oam_ctc_onu_llid_config_t *config)
{
    return OAM_E_NOT_SUPPORT;
}

oam_status ctc_oam_onu_llid_queue_config_set_adapt (
    oam_llid_t llid,
    oam_uint8  num,
    oam_ctc_onu_llid_config_t *config)
{

    return OAM_E_NOT_SUPPORT;
}



/********************* POWER SAVING ADAPT START *********************/


oam_status ctc_oam_onu_sleep_control_set_adapt(
    oam_uint32 sleep_duration,
    oam_uint32 wait_duration,
    oam_uint8 sleep_flag,
    oam_uint8 sleep_mode)
{
    return OAM_E_OK;
}


oam_status ctc_oam_onu_power_saving_cap_get_adapt(
    oam_uint8 *sleep_mode,
    oam_uint8 *early_wakeup)
{
    return OAM_E_OK;
}


oam_status ctc_oam_onu_power_saving_config_get_adapt(
    oam_uint8 *early_wakeup,
    oam_uint64 *max_sleep_duration)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_power_saving_config_set_adapt(
    oam_uint8 early_wakeup,
    oam_uint64 max_sleep_duration)
{
    return OAM_E_OK;
}


/****************** PON protect ADAPT **************************/

oam_status ctc_oam_onu_pon_protect_parm_get_adapt(
    oam_uint16 *opt_time,
    oam_uint16 *mpcp_time)
{
    return OAM_E_NOT_SUPPORT;
}

oam_status ctc_oam_onu_pon_protect_parm_set_adapt(
    oam_uint16 opt_time,
    oam_uint16 mpcp_time)
{

    return OAM_E_NOT_SUPPORT;
}

static oam_uint32 g_ctc_oam_cfg_counter = 0 ;

oam_status ctc_oam_onu_cfg_counter_get_adapt(oam_uint32 *counter)
{
    *counter = g_ctc_oam_cfg_counter;
    return OAM_E_OK;
}
oam_status ctc_oam_onu_cfg_counter_set_adapt(oam_uint32 counter)
{
    g_ctc_oam_cfg_counter = counter;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_cfg_mgmt_set_adapt(oam_uint8 action)
{
    return OAM_E_OK;
}



static oam_uint8 g_holdover_state = OAM_CTC_ONU_HOLDOVER_DEACTIVED;
static oam_uint32 g_holdover_time = 0;

oam_status ctc_oam_holdover_get_adapt(
    oam_uint32 *holdover_state,
    oam_uint32 *holdover_time)
{
    oam_status ret = OAM_E_OK;

    *holdover_state = g_holdover_state;
    *holdover_time  = g_holdover_time;

    return ret;
}

oam_status ctc_oam_holdover_set_adapt(
    oam_uint32 holdover_state,
    oam_uint32 holdover_time)
{
    oam_status ret = OAM_E_OK;

    g_holdover_state = holdover_state;
    g_holdover_time  = holdover_time;

    return ret;
}




/************************VLAN ADAPT START ***************************/




oam_status ctc_oam_onu_default_vlan_tag_get_adapt(
    oam_port_id_t port,
    oam_uint16 *tpid,
    oam_uint16 *tag)
{
    oam_status ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_vlan_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8 *mode)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_translate_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *translate_num,
    oam_ctc_onu_vlan_xlate_entry *vlan)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_trunk_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *trunk_num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_agg_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *agg_num,
    ctc_oam_agg_vlan_t *vlan)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_transparent_vlan_set_adapt(
    oam_port_id_t port)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_tag_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t vlan)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_translate_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_xlate_entry *vlan)
{
    oam_status ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_trunk_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_agg_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    ctc_oam_agg_vlan_t *vlan)
{
    oam_status ret = OAM_E_OK;

    return ret;
}








/**************** PLAT ADAPT START *********************************/

oam_status ctc_oam_onu_opmdiag_get_adapt(
    oam_int16 *temp,
    oam_uint16 *volt,
    oam_uint16 *tx_bias,
    oam_uint16 *tx_power,
    oam_uint16 *rx_power)
{
    oam_status ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_mgmt_config_get_adapt(
    ctc_oam_mgmt_parm_t *mgmt)
{
    return OAM_E_NOT_SUPPORT;
}
oam_status ctc_oam_onu_mgmt_config_set_adapt(
    ctc_oam_mgmt_parm_t *parm)
{
    return  OAM_E_NOT_SUPPORT;
}

oam_status ctc_oam_onu_laser_control_set_adapt(
    oam_uint16 action,
    oam_uint8 *onuId,
    oam_transid transId)
{
    return OAM_E_OK;
}



oam_status ctc_oam_onu_reset_set_adapt()
{
    return OAM_E_OK;
}




/************** APP ADAPT START ****************************/

oam_status ctc_oam_sla_get_adapt(
    oam_uint8   *oper,
    oam_uint8   *scheduling_scheme,
    oam_uint8   *high_pri_boundary,
    oam_uint32 *cycle_len,
    oam_uint8   *num_of_service,
    oam_sla_queue_t *sla)
{
    return OAM_E_NOT_SUPPORT;
}

oam_status ctc_oam_sla_set_adapt(
    oam_uint8   oper,
    oam_uint8   scheduling_scheme,
    oam_uint8   high_pri_boundary,
    oam_uint32  cycle_len,
    oam_uint8   num_of_service,
    oam_sla_queue_t *sla)
{
    return OAM_E_NOT_SUPPORT;

}

oam_status ctc_oam_loop_detect_status_get_adapt(
    oam_port_id_t port,
    oam_uint32 *status)
{
    return OAM_E_OK;
}



oam_status ctc_oam_loop_detect_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status)
{
    return OAM_E_OK;
}

oam_status ctc_oam_disable_looped_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status)
{
    return OAM_E_OK;

}


oam_status ctc_oam_onu_classification_get_adapt(
    oam_port_id_t port,
    oam_uint32 *num,
    oam_uint8 *data)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_classification_set_adapt(
    oam_uint8 action,
    oam_port_id_t port,
    oam_uint32 num,
    oam_uint8 *data)
{
    oam_status ret = OAM_E_OK;

    return ret;

}



/******************************* MC ADAPT **************************/

oam_status ctc_oam_onu_mcvlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *num,
    oam_uint16 *vlanIds)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mcvlan_set_adapt(
    oam_port_id_t port,
    oam_uint8 oper,
    oam_uint16 num,
    oam_uint16 *vlanIds)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_mctagstrip_get_adapt(
    oam_port_id_t port,
    oam_uint8 *tagStriped,
    oam_uint8 *num,
    oam_ctc_iptv_vid_t *vlans)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mctagstrip_set_adapt(
    oam_port_id_t port,
    oam_uint8  tagStriped,
    oam_uint8  num,
    oam_ctc_iptv_vid_t *vlans)
{
    oam_status   ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_mc_switch_get_adapt(
    oam_uint8 *mode)
{
    return OAM_E_OK;

}

oam_status ctc_oam_onu_mc_switch_set_adapt(
    oam_uint8 mode)
{
    oam_status   ret = OAM_E_OK;

    return ret;

}

oam_status ctc_oam_onu_mc_control_type_get_adapt(
    oam_uint8 *type)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mc_control_type_set_adapt(
    oam_uint8  type)
{
    return OAM_E_NOT_SUPPORT;
}

oam_status ctc_oam_onu_mc_control_vlan_mac_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_vlan_mac_t *vlanMac)
{
    return OAM_E_OK;

}

oam_status ctc_oam_onu_mc_control_mac_ip_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_mac_ip_t *macIp)
{
    return OAM_E_OK;

}

oam_status ctc_oam_onu_mc_control_vlan_ip_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIp)
{
    return OAM_E_OK;

}

oam_status ctc_oam_onu_mc_control_clr_adapt()
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mc_control_vlan_mac_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_vlan_mac_t *vlanMac)
{

    return OAM_E_OK;
}

oam_status ctc_oam_onu_mc_control_mac_ip_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_mac_ip_t *macIp)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mc_control_vlan_ip_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIp)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_grp_nmb_max_get_adapt(
    oam_port_id_t port,
    oam_uint8   *maxGrp)
{
    return OAM_E_ERROR;
}

oam_status ctc_oam_onu_grp_nmb_max_set_adapt(
    oam_port_id_t port,
    oam_uint8   maxGrp)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

/********************* Event ****************/
oam_status ctc_oam_onu_event_type_get(
    oam_ctc_event_entry_t entry,
    ctc_oam_alarm_class_e *type,
    oam_port_id_t *port)
{
    return OAM_E_OK;
}
oam_status ctc_oam_event_status_get_adapt(
    oam_ctc_event_entry_t entry,
    oam_uint32 *status)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_event_thresh_get_adapt(
    oam_ctc_event_entry_t entry,
    oam_uint32 *raise_thresh,
    oam_uint32 *clear_thresh)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_event_status_set_adapt(
    oam_ctc_event_status_t config)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_event_thresh_set_adapt(
    oam_ctc_event_thresh_t config)
{
    oam_status ret = OAM_E_OK;

    return ret;
}


/********************* ALARM ADAPT ****************/
oam_status ctc_oam_onu_alarm_class_port_get(
    ctc_oam_mgmt_obj_idx_t index,
    ctc_oam_alarm_class_e *type,
    oam_port_id_t *port)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_alarm_get_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32 *alarm_config)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_alarm_set_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  alarm_config)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_alarm_threshold_get_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  *raise_threshold,
    oam_uint32  *clear_threshold)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_alarm_threshold_set_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  raise_threshold,
    oam_uint32  clear_threshold)
{
    oam_status ret = OAM_E_OK;

    return ret;
}



/***************** Statioam Adapt START ***********************/
oam_status ctc_oam_onu_stats_monitor_status_get_adapt(
    oam_port_id_t port,
    oam_uint16 *monitor_status,
    oam_uint32 *monitor_period)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_stats_monitor_status_set_adapt(
    oam_port_id_t port,
    oam_uint16 monitor_status,
    oam_uint32  monitor_period)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_curr_stats_get_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_curr_stats_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_histroy_stats_get_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    return OAM_E_OK;
}

oam_status ctc_oam_auth_keep_silence_set_adapt(oam_uint8 llid_index,
        oam_boolean enable)
{
    return OAM_E_OK;
}

oam_status ctc_get_port_info(oam_port_id_t port, ctc_port_info_t *info)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_laser_alarm_status_get(oam_uint8 *status)
{
    return OAM_E_OK;
}

oam_status ctc_oam_port_link_status_reg()
{
    return OAM_E_OK;
}

oam_status ctc_oam_port_auto_neg_reg()
{
    return OAM_E_OK;
}

oam_status ctc_oam_port_up_congest_reg()
{
    return OAM_E_OK;
}

oam_status ctc_oam_sn_get_adapt(oam_ctc_onu_sn_t *sn)
{
    oam_uint8 mac [OAM_MACADDR_LEN] = {0x00, 0x13, 0x25, 0xab, 0xcd, 0xef};
    oam_int8 vender[OAM_CTC_VENDER_ID_LEN] = {0x32, 0x30, 0x31, 0x31};
    oam_int8 model[OAM_CTC_MODEL_ID_LEN] = {0x38, 0x31, 0x30, 0x65};
    oam_int8 hwver[OAM_CTC_ONU_HW_LEN] = {0x48, 0x47, 0x38, 0x31, 0x30, 0x65, 0, 0};

    oam_uint8 swver[OAM_CTC_ONU_SW_LEN] = {0x56, 0x31, 0x30, 0x30, 0x52, 0x30, 0x30, 0x31, 0x43,
                                           0x30, 0x31, 0x42, 0x30, 0x35, 0x32, 00
                                          };

    sn->hdr.width = sizeof(oam_ctc_onu_sn_t) - sizeof(oam_var_cont_t);

    memcpy(sn->venderId,  vender, OAM_CTC_VENDER_ID_LEN);
    memcpy(sn->model,  model, OAM_CTC_MODEL_ID_LEN);
    memcpy(sn->onuId, mac, OAM_MACADDR_LEN);
    memcpy(sn->hwVer, hwver, OAM_CTC_ONU_HW_LEN);
    memcpy(sn->swVer, swver, OAM_CTC_ONU_SW_LEN);

    return OAM_E_OK;
}

oam_status ctc_oam_fw_get_adapt(oam_uint8 *fw_ver, oam_uint8 *len)
{
    sprintf((char *)fw_ver, "2.1.0.8");

    return OAM_E_OK;
}

oam_status ctc_oam_chip_id_get_adapt(oam_ctc_onu_chip_id_t *chip_id)
{
    if (!chip_id)
        return OAM_E_PARAM;

    chip_id->Revision = 1;

    if (oam_is_cuc_oam()) {
        chip_id->Date[0] = 11;
        chip_id->Date[1] = 1;
        chip_id->Date[2] = 29;
    }
    else {
        chip_id->Date[0] = 0x11;
        chip_id->Date[1] = 0x01;
        chip_id->Date[2] = 0x29;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_onucap2_get_adapt(oam_ctc_onu_capabilities2_t *onu_cap2)
{
    oam_uint8 numOfPon = 1;
    oam_uint8 protectionType = 1;
    oam_uint8 hasBattery = 1;

#if 0
    //startup_config_read(CFG_ID_PON_PORT_NUM,1 , &numOfPon);
    //startup_config_read(CFG_ID_PON_PROTECTION_TYPE,1 , &protectionType);
    //startup_config_read(CFG_ID_BAK_BATTERY_MODE,1 , &hasBattery);
#endif

    onu_cap2->onuType = htonl(0); /* SFU */
    onu_cap2->multillid = 1; /* only support single llid */
    onu_cap2->protectionType = protectionType;
    onu_cap2->numPONIf = numOfPon;
    onu_cap2->numSlot = 0;
    onu_cap2->numInterface = 1;
#ifdef HAVE_MPORTS
    onu_cap2->intf[0].interfaceType = htonl(1); /* GE */
#else
    onu_cap2->intf[0].interfaceType = htonl(0); /* FE */
#endif
    onu_cap2->intf[0].numPorts = htons(CTC_MAX_ONU_UNI_PORTS);
    onu_cap2->BatteryBackup = hasBattery;

    return OAM_E_OK;
}

oam_status ctc_oam_onucap3_get_adapt(oam_ctc_onu_capabilities3_t *onu_cap3)
{
    oam_uint8 ipv6 = CTC_IPV6_CAPABILTY_SUPPORT;
    oam_uint8 power = CTC_POWER_CONTROL_SUPPORT_TX_RX ;
    oam_uint8 sla = CTC_SERVICE_SLA_NOT_SUPPORT ;

    onu_cap3->ipv6Support  = ipv6;  /* SFU */
    onu_cap3->powerControl = power; /* only support single llid */
    onu_cap3->serviceSla   = sla;

    return OAM_E_OK;
}

oam_status ctc_oam_loid_passwd_get_adapt(oam_uint8 *loid, oam_uint8 *passwd)
{
    return OAM_E_OK;
}

oam_status ctc_oam_keep_silence_time_get_adapt(oam_uint8 *keep)
{
    *keep = 60;

    return OAM_E_OK;
}

oam_status ctc_oam_enc_mode_set_adapt(oam_uint8 mode)
{
    return OAM_E_OK;
}

oam_status ctc_oam_enc_enable_set_adapt(oam_uint8 llid_index,
                                        oam_boolean enable)
{
    return OAM_E_OK;
}

oam_status ctc_oam_enc_key_get_adapt(oam_uint8 llid_index, oam_uint8 key_index,
                                     oam_uint8 *key, oam_uint8 *key_len)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mpcp_queue_thresh_get_adapt(
    oam_uint8 index, oam_uint16 *threshold)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mpcp_queue_thresh_set_adapt(
    oam_uint8 index, oam_uint16 threshold)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_pon_stat_get_adapt(oam_ctc_onu_port_stats_t *stats)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_uni_stat_get_adapt(oam_port_id_t port,oam_ctc_onu_port_stats_t *stats)
{
    return OAM_E_OK;
}

void ctc_oam_power_saving_init()
{
    return;
}

void ctc_oam_power_saving_reset()
{
    return;
}

oam_status ctc_oam_oui_get_adapt(oam_oui_t oui)
{
    oui[0] = 0x11;
    oui[1] = 0x11;
    oui[2] = 0x11;

    return OAM_E_OK;
}

oam_uint8 ctc_oam_swimage_id_get_adapt()
{
    return 0;
}

oam_uint8 ctc_oam_swimage_flash_state_get_adapt()
{
    return 0;
}


oam_status ctc_oam_swimage_open_adapt(oam_uint8 img_id)
{
    return OAM_E_OK;
}

oam_status ctc_oam_swimage_write_adapt(
    oam_uint8 img_id,
    oam_uint8 *buf,
    oam_uint16 len)
{
    return OAM_E_OK;
}

oam_status ctc_oam_swimage_image_ready(oam_uint8 img_id)
{
    return OAM_E_OK;
}

oam_status ctc_oam_swimage_close_adapt(oam_uint8 img_id)
{
    return OAM_E_OK;
}


oam_status ctc_oam_swimage_active_adapt()
{
    return OAM_E_OK;
}


oam_status ctc_oam_swimage_commit_adapt()
{
    return OAM_E_OK;
}


oam_status ctc_oam_multi_llid_admin_ctrl_set_adapt(oam_uint32 llid_num)
{
    return OAM_E_OK;
}


oam_status ctc_oam_eth_port_alarm_reg_adapt()
{
    
    return OAM_E_OK;
}

oam_status ctc_oam_ld_alarm_reg_adapt()
{
    
    return OAM_E_OK;
}

oam_status ctc_oam_opm_alarm_reg_adapt()
{
    return OAM_E_OK;
}

oam_status ctc_oam_gpio_alarm_reg_adapt()
{
    return OAM_E_OK;
}


oam_status ctc_oam_up_congest_status_get(oam_port_id_t port, oam_boolean *status)
{
    
    return OAM_E_OK;
}

#ifdef ZTE_SPECIFIC
oam_status ctc_oam_cls_syn_db ( oam_port_id_t port)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_mc_vlan_syn_db ( oam_port_id_t port)
{
    oam_status ret = OAM_E_OK;

    return ret;
}


oam_status ctc_mc_mode_set_adapt(
    oam_uint32 mode
)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_mc_gmi_set_adapt (
    oam_uint32 gmi
)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_port_storm_control_set_adapt (
    oam_port_id_t            port_id,
    oam_uint32               storm_type,
    oam_boolean              enable,
    oam_uint32               rate
)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

#if 0
oam_status ctc_onu_schedule_mode_set_adapt (
    oam_sdl_sched_type_t       sche_mode
)
{
    oam_status ret = OAM_E_OK;

    return ret;
}
#endif

oam_status ctc_port_queue_size_set_adapt (
    oam_port_id_t                   port,
    oam_uint32                       queue_id,
    oam_uint32                       queue_size
)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_onu_1pcos_set_adapt (
    oam_uint32                       pri,
    oam_uint32                       cos
)
{
    oam_status ret = OAM_E_OK;

    return ret;
}

oam_status ctc_onu_mtu_set_adapt (
    oam_uint16           mtu
)
{
    oam_status ret = OAM_E_OK;

    return ret;
}


#endif/*END_ZTE_SPECIFIC*/

#endif

