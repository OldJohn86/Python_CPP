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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oam_types.h"
#include "oam_cmn.h"
#include "oam_std_pdu.h"
#include "oam_ctc_pdu.h"
#include "oam_ctc_client.h"
#include "oam_ctc_adapt.h"
#include "oam_ctc_stats.h"
#include "oam_ctc_alarm.h"


#ifdef HAVE_MC_CTRL
#include "mc_api.h"
#include "mc_ctcctrl.h"
#include "mc_core.h"
#endif



#include "cs_aal_mpcp.h"
#include "cs_aal_pkt.h"


#define TAG2VID(tag)    ((tag)&0x0fff)
#define TAG2PRI(tag)    (((tag)&0xe000)>>13)

extern oam_int32 onu_evt_reg(oam_int32 evt_type, void *func , void *cookie);

#ifdef HAVE_ZTE_OAM
oam_status ctc_oam_cls_syn_db ( oam_port_id_t port);
oam_status ctc_oam_mc_vlan_syn_db ( oam_port_id_t port);
oam_status ctc_oam_onu_alarm_syn_db (  );
oam_status ctc_oam_port_alarm_syn_db ( oam_port_id_t port  );
#endif/*END_HAVE_ZTE_OAM*/

oam_status oam_port_event_register_adapt (
    void                        *call_back)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}



/***************** SDL ADAPT START ****************************/
oam_status ctc_oam_eth_admin_state_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_auto_admin_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_auto_local_cap_get_adapt(
    oam_port_id_t port,
    oam_uint32 *value,
    oam_uint8 *getNum)
{
    /* TODO:not completed */
    /* max value num is 9 */
    /*
       global(0),--reserved for future use.
       other(1),--undefined
       unknown(2),--initializing, true ability not yet known.
       10BASE-T(14),--10BASE-T as defined in Clause 14
       10BASE-TFD(142), --Full duplex 10BASE-T as defined in Clauses 14 and 31
       100BASE-T4(23),--100BASE-T4 as defined in Clause 23
       100BASE-TX(25),--100BASE-TX as defined in Clause 25
       100BASE-TXFD(252), --Full duplex 100BASE-TX as defined in Clauses 25 and 31
       10GBASE-KX4(483), --10GBASE-KX4 PHY as defined in Clause 71
       10GBASE-KR(495), --10GBASE-KR PHY as defined in Clause 72
       10GBASE-T(55),--10GBASE-T PHY as defined in Clause 55
       FDX PAUSE(312), --PAUSE operation for full duplex links as defined in
       Annex 31BFDX APAUSE(313), --Asymmetric PAUSE operation for full duplex links as defined
       in Clause 37 and Annex 31B
       FDX SPAUSE(314), --Symmetric PAUSE operation for full duplex links as defined
       in Clause 37 and Annex 31B
       FDX BPAUSE(315), --Asymmetric and Symmetric PAUSE operation for full duplex
       links as defined in Clause 37 and Annex 31B
       100BASE-T2(32),--100BASE-T2 as defined in Clause 32
       100BASE-T2FD(322), --Full duplex 100BASE-T2 as defined in Clauses 31 and 32
       1000BASE-X(36),--1000BASE-X as defined in Clause 36
       1000BASE-XFD(362), --Full duplex 1000BASE-X as defined in Clause 36
       1000BASE-KX(393), --1000BASE-KX PHY as defined in Clause 70
       1000BASE-T(40),--1000BASE-T UTP PHY as defined in Clause 40
       1000BASE-TFD(402), --Full duplex 1000BASE-T UTP PHY to be defined in Clause 40
       Rem Fault1(37),--Remote fault bit 1 (RF1) as specified in Clause 37
       Rem Fault2(372), --Remote fault bit 2 (RF1) as specified in Clause 37
       isoethernet(8029) --802.9 ISLAN-16T
       */

    /* TOPO: code from LynxC, not sure the Lynxd has change the cap */
#ifndef HAVE_MPORTS
    value[0] = 0x28;
    value[1] = 0x192;
    value[2] = 0x142;
    *getNum = 3;
#else
    value[0] = 14;
    value[1] = 142;
    value[2] = 23;
    value[3] = 25;
    value[4] = 252;
    value[5] = 312;
    value[6] = 314;
    value[7] = 32;
    value[8] = 322;
    *getNum = 9;
#endif

    return OAM_E_OK;
}



oam_status ctc_oam_eth_auto_advts_cap_get_adapt(
    oam_port_id_t port,
    oam_uint32 *value,
    oam_uint8 *getNum)
{
    /* TODO:not completed */
#ifndef HAVE_MPORTS
    value[0] = 0x28;
    value[1] = 0x192;
    value[2] = 0x142;
    *getNum = 3;
#else
    value[0] = 14;
    value[1] = 142;
    value[2] = 23;
    value[3] = 25;
    value[4] = 252;
    value[5] = 312;
    value[6] = 314;
    value[7] = 32;
    value[8] = 322;
    *getNum = 9;
#endif

    return OAM_E_OK;
}

oam_status ctc_oam_eth_fec_ability_get_adapt(
    oam_uint8 *ability)
{
    /*
    #define OAM_INTF_FEC_ABILITY_UNKOWN             1
    #define OAM_INTF_FEC_SUPPORDTED                 2
    #define OAM_INTF_FEC_NOT_SUPPORTED              3
    */

    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_fec_mode_set_adapt(
    oam_uint32 fecMode)
{
    /*
    * fec Mode: 1. unknow
    *           2. enable
    *           3. Disable
    #define OAM_INTF_FEC_MODE_UNKOWN                1
    #define OAM_INTF_FEC_MODE_EN                    2
    #define OAM_INTF_FEC_MODE_DIS                   3
    */

    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_fec_mode_get_adapt(
    oam_uint32 *fecMode)
{
    oam_status  ret = OAM_E_OK;

    return ret;
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
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_eth_port_pause_get_adapt(
    oam_port_id_t port,
    oam_uint8  *flowControl)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_port_pause_set_adapt(
    oam_port_id_t port,
    oam_uint8 flowControl)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}



oam_status ctc_oam_eth_admin_state_set_adapt(
    oam_port_id_t port,
    oam_uint32 phyAdmin)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_auto_neg_restart_set_adapt(
    oam_port_id_t port)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_auto_neg_ctrl_set_adapt(
    oam_port_id_t port,
    oam_uint32 autoNegAdmin)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_port_policing_get_adapt(
    oam_port_id_t port,
    oam_uint8  *oper,
    oam_uint32 *cir,
    oam_uint32 *cbs,
    oam_uint32 *ebs)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_port_policing_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  cbs,
    oam_uint32  ebs)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_eth_ds_rate_limit_get_adapt(
    oam_port_id_t port,
    oam_uint8 *oper,
    oam_uint32 *cir,
    oam_uint32 *pir)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_ds_rate_limit_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  pir)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_mac_limit_set_adapt(
    oam_port_id_t port,
    oam_uint16  num)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_mac_limit_get_adapt(
    oam_port_id_t port,
    oam_uint16  *num)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_port_mode_set_adapt(
    oam_port_id_t port,
    oam_uint8  mode)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_port_stats_get_adapt(
    oam_port_id_t port,
    oam_ctc_eth_port_stats_t *stats)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_pon_port_stat_get_adapt(
    oam_ctc_onu_llid_stats1 *stats1,
    oam_ctc_onu_llid_stats2 *stats2)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}
oam_status ctc_oam_onu_port_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8  *mode)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_port_work_mode_get_adapt(
    oam_port_id_t portId,
    oam_uint8  *mode)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_mac_aging_time_get_adapt(
    oam_uint32 *time)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_eth_mac_aging_time_set_adapt(
    oam_uint32 time)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}



oam_status ctc_oam_onu_fast_leave_ability_get_adapt(
    oam_uint32 *num,
    oam_uint32 *mode)
{
    *num = 6;
    mode[0] = 0;    /* non_fast_leave at snoop mode */
    mode[1] = 0x01; /* fast_leave at snoop mode */
    mode[2] = 0x10; /* non_fast_leave at ctrl mode */
    mode[3] = 0x11; /* fast_leave at ctrl mode */
    mode[4] = 0x2;  /* non_fast_leave at mld snoop mode */
    mode[5] = 0x3;  /* fast_leave at mld snoop mode */
    return OAM_E_OK;
}

oam_status ctc_oam_onu_fast_leave_state_get_adapt(
    oam_uint32 *state)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_fast_leave_ctrl_set_adapt(
    oam_uint32 fastLeaveAdmin)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}



oam_status ctc_oam_onucap_get_adapt(
    oam_ctc_onu_capabilities_t *onu_cap)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_llid_queue_config_get_adapt (
    oam_llid_t llid,
    oam_uint8 *num,
    oam_ctc_onu_llid_config_t *config)
{
    *num = 3;
    config[0].qid = 0x100;
    config[0].wrr = 0x200;
    config[1].qid = 0x100;
    config[1].wrr = 0x200;
    config[2].qid = 0x100;
    config[2].wrr = 0x200;
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
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_power_saving_cap_get_adapt(
    oam_uint8 *sleep_mode,
    oam_uint8 *early_wakeup)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_power_saving_config_get_adapt(
    oam_uint8 *early_wakeup,
    oam_uint64 *max_sleep_duration)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_power_saving_config_set_adapt(
    oam_uint8 early_wakeup,
    oam_uint64 max_sleep_duration)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


/****************** PON protect ADAPT **************************/

oam_status ctc_oam_onu_pon_protect_parm_get_adapt(
    oam_uint16 *opt_time,
    oam_uint16 *mpcp_time)
{
    *opt_time = 0x1234;
    *mpcp_time = 0x5678;
    return OAM_E_NOT_SUPPORT;
}

oam_status ctc_oam_onu_pon_protect_parm_set_adapt(
    oam_uint16 opt_time,
    oam_uint16 mpcp_time)
{

    return OAM_E_NOT_SUPPORT;
}

oam_status ctc_oam_onu_cfg_counter_get_adapt(oam_uint32 *counter)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}
oam_status ctc_oam_onu_cfg_counter_set_adapt(oam_uint32 counter)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_cfg_mgmt_set_adapt(oam_uint8 action)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}



oam_status ctc_oam_holdover_get_adapt(
    oam_uint32 *holdover_state,
    oam_uint32 *holdover_time)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_holdover_set_adapt(
    oam_uint32 holdover_state,
    oam_uint32 holdover_time)
{
    cs_status  ret = CS_E_OK;
    cs_uint32 mask = 0;
    cs_aal_mpcp_cfg_t mpcp_cfg;

    memset(&mpcp_cfg, 0, sizeof(mpcp_cfg));

    if (OAM_CTC_ONU_HOLDOVER_DEACTIVED == holdover_state) {
        /* dereg_ctrl value should be discuss later */
        mpcp_cfg.dereg_ctrl = 0x03;
        mpcp_cfg.gate_timer = holdover_time * 10;   /* unit to 0.1ms */
    }
    else {
        /* dereg_ctrl value should be discuss later */
        mpcp_cfg.dereg_ctrl = 0x00;
        mpcp_cfg.gate_timer = holdover_time * 10;   /* unit to 0.1ms */
    }

    mask = CS_AAL_MPCP_CFG_MASK_DEREG_CTRL | CS_AAL_MPCP_CFG_MASK_GATE_TIMER;

    ret = cs_aal_mpcp_cfg_set(OAM_DEV_ID, OAM_PORT_ID, mask, &mpcp_cfg);

    if (ret != CS_E_OK) {
        OAM_ORG_LOG_MAJ("cs_aal_mpcp_cfg_set failed, ret = %d\r\n", ret);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}




/************************VLAN ADAPT START ***************************/




oam_status ctc_oam_onu_default_vlan_tag_get_adapt(
    oam_port_id_t port,
    oam_uint16 *tpid,
    oam_uint16 *tag)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_vlan_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8 *mode)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_translate_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *translate_num,
    oam_ctc_onu_vlan_xlate_entry *vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_trunk_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *trunk_num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_agg_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *agg_num,
    ctc_oam_agg_vlan_t *vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_transparent_vlan_set_adapt(
    oam_port_id_t port)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_tag_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_translate_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_xlate_entry *vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_trunk_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_agg_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    ctc_oam_agg_vlan_t *vlan)
{
    oam_status  ret = OAM_E_OK;

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
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_mgmt_config_get_adapt(
    ctc_oam_mgmt_parm_t *mgmt)
{
    /*
     mgmt->mgmt_ip.ipver = CS_IPV4;
     mgmt->mgmt_gateway.ipver = CS_IPV4;
     mgmt->mgmt_ip.addr.ipv4 = 0x10101010;
     mgmt->mask = 0xffffff00;
     mgmt->mgmt_gateway.addr.ipv4  = 0x10101001;
     mgmt->mgmt_svlan  = mgmt->mgmt_cvlan  = 1;
     mgmt->mgmt_data_pri = 1;
    */
    return OAM_E_NOT_SUPPORT;
}
oam_status ctc_oam_onu_mgmt_config_set_adapt(
    ctc_oam_mgmt_parm_t *parm)
{
    return  OAM_E_NOT_SUPPORT;
}

typedef enum {
    TRANSID_MAJOR   = 0,
    TRANSID_STANDBY = 1,
    TRANSID_BOTH    = 2
} cs_transid;

oam_status ctc_oam_onu_laser_control_set_adapt(
    oam_uint16 action,
    oam_uint8 *onuId,
    cs_transid transId)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}



oam_status ctc_oam_onu_reset_set_adapt()
{
    oam_status  ret = OAM_E_OK;

    return ret;
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
#ifdef HAVE_LOOP_DETECT
    return OAM_E_OK;
#else
    return OAM_E_NOT_SUPPORT;
#endif

}



oam_status ctc_oam_loop_detect_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status)
{
#ifdef HAVE_LOOP_DETECT
    return OAM_E_OK;
#else
    return OAM_E_NOT_SUPPORT;
#endif

}

oam_status ctc_oam_disable_looped_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status ctc_oam_onu_classification_get_adapt(
    oam_port_id_t port,
    oam_uint32 *num,
    oam_uint8 *data)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status ctc_oam_onu_classification_set_adapt(
    oam_uint8 action,
    oam_port_id_t port,
    oam_uint32 num,
    oam_uint8 *data)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}



/******************************* MC ADAPT **************************/

oam_status ctc_oam_onu_mcvlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *num,
    oam_uint16 *vlanIds)
{
    oam_status  ret = OAM_E_OK;

    return ret;
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
    return OAM_E_OK;
}


oam_status ctc_oam_onu_mc_switch_get_adapt(
    oam_uint8 *mode)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mc_switch_set_adapt(
    oam_uint8 mode)
{
    return OAM_E_OK;
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
    return OAM_E_NOT_SUPPORT;
}

oam_status ctc_oam_onu_grp_nmb_max_set_adapt(
    oam_port_id_t port,
    oam_uint8   maxGrp)
{
    return OAM_E_NOT_SUPPORT;
}

/********************* Event ****************/
oam_status ctc_oam_onu_event_type_get(
    oam_ctc_event_entry_t entry,
    ctc_oam_alarm_class_e *type,
    oam_port_id_t *port)
{
    if (entry.object_type == OAM_CTC_ALARM_TYPE_ONU) {
        *type = CTC_ONU_ALARM;
    }
    else if (entry.object_type == OAM_CTC_ALARM_TYPE_PON) {
        *type = CTC_PON_ALARM;
        *port = CS_PON_PORT_ID;
    }
    else if (entry.object_type == OAM_CTC_ALARM_TYPE_UNI) {
        *type = CTC_PORT_ALARM;
        *port = (entry.object_inst & 0x000000ff);
    }
    else {
        return OAM_E_PARAM;
    }

    return OAM_E_OK;
}
oam_status ctc_oam_event_status_get_adapt(
    oam_ctc_event_entry_t entry,
    oam_uint32 *status)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = CS_PON_PORT_ID;
    oam_status ret = OAM_E_OK;
    oam_uint32 alarm_config = 0;

    ret = ctc_oam_onu_event_type_get(entry, &type, &port);

    if (ret != OAM_E_OK) {
        return ret;
    }

    ret = ctc_oam_get_alarm_config(type, port, entry.id, &alarm_config);

    if (ret != OAM_E_OK) {
        return ret;
    }

    if (alarm_config == CTC_ALARM_ON) {
        *status = OAM_CTC_EVENT_STATUS_ENABLE;
    }
    else {
        *status = OAM_CTC_EVENT_STATUS_DISABLE;
    }

    return ret;
}

oam_status ctc_oam_event_thresh_get_adapt(
    oam_ctc_event_entry_t entry,
    oam_uint32 *raise_thresh,
    oam_uint32 *clear_thresh)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = CS_PON_PORT_ID;
    oam_status ret = OAM_E_OK;

    ret = ctc_oam_onu_event_type_get(entry, &type, &port);

    if (ret != OAM_E_OK) {
        return ret;
    }

    ret = ctc_oam_get_alarm_threshold(type, port, entry.id, raise_thresh,
                                      clear_thresh);

    return ret;
}

oam_status ctc_oam_event_status_set_adapt(
    oam_ctc_event_status_t config)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = CS_PON_PORT_ID;
    oam_status ret = OAM_E_OK;
    oam_uint8 enable = 0;

    ret = ctc_oam_onu_event_type_get(config.entry, &type, &port);

    if (ret != OAM_E_OK) {
        return ret;
    }

    if (config.status == OAM_CTC_EVENT_STATUS_ENABLE) {
        enable = CTC_ALARM_ON;
    }
    else {
        enable = CTC_ALARM_OFF;
    }

    ret = ctc_oam_set_alarm_config(type, port, config.entry.id, enable);

    return ret;
}

oam_status ctc_oam_event_thresh_set_adapt(
    oam_ctc_event_thresh_t config)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = CS_PON_PORT_ID;
    oam_status ret = OAM_E_OK;

    ret = ctc_oam_onu_event_type_get(config.entry, &type, &port);

    if (ret != OAM_E_OK) {
        return ret;
    }

    ret = ctc_oam_set_alarm_threshold(type, port, config.entry.id,
                                      config.raise_thresh, config.clear_thresh);

    return ret;
}


/********************* ALARM ADAPT ****************/
oam_status ctc_oam_onu_alarm_class_port_get(
    ctc_oam_mgmt_obj_idx_t index,
    ctc_oam_alarm_class_e *type,
    oam_port_id_t *port)
{
    if (!index.valid) {
        *type = CTC_ONU_ALARM;
    }
    else {
        if (index.idxLeaf == OAM_CTC_LEAF_CODE_ONU_PON) {
            *type = CTC_PON_ALARM;
            *port = CS_PON_PORT_ID;
        }
        else if (index.idxLeaf == OAM_CTC_LEAF_CODE_ONU_PORT) {
            if (index.idxBranch == OAM_CTC_INDEX_BRANCH && index.idxValue == 0) {
                *type = CTC_PON_ALARM;
                *port = CS_PON_PORT_ID;
            }
            else {
                *type = CTC_PORT_ALARM;
                *port = index.idxValue;
            }
        }
        else
            return OAM_E_PARAM;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_onu_alarm_get_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32 *alarm_config)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = CS_PON_PORT_ID;
    oam_status ret = OAM_E_OK;

    ret = ctc_oam_onu_alarm_class_port_get(index, &type, &port);

    if (ret != OAM_E_OK)
        return ret;

    return ctc_oam_get_alarm_config(type, port, alarm_id, alarm_config);
}

oam_status ctc_oam_onu_alarm_set_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  alarm_config)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = 0;
    oam_status ret = OAM_E_OK;

    ret = ctc_oam_onu_alarm_class_port_get(index, &type, &port);

    if (ret != OAM_E_OK)
        return ret;

    ret = ctc_oam_set_alarm_config(type, port, alarm_id, alarm_config);
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if ( ret == OAM_E_OK ) {
        if ( type != CTC_PORT_ALARM ) {
            ctc_oam_onu_alarm_syn_db( );
        }
        else {

            ctc_oam_port_alarm_syn_db( port);
        }
    }

#endif
#endif/*END_HAVE_ZTE_OAM*/

    return ret;

}

oam_status ctc_oam_onu_alarm_threshold_get_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  *raise_threshold,
    oam_uint32  *clear_threshold)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = 0;
    oam_status ret = OAM_E_OK;

    ret = ctc_oam_onu_alarm_class_port_get(index, &type, &port);

    if (ret != OAM_E_OK)
        return ret;

    return ctc_oam_get_alarm_threshold(type, port, alarm_id,
                                       raise_threshold, clear_threshold);

}

oam_status ctc_oam_onu_alarm_threshold_set_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  raise_threshold,
    oam_uint32  clear_threshold)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = 0;
    oam_status ret = OAM_E_OK;

    ret = ctc_oam_onu_alarm_class_port_get(index, &type, &port);

    if (ret != OAM_E_OK)
        return ret;

    ret = ctc_oam_set_alarm_threshold(type, port, alarm_id,
                                      raise_threshold, clear_threshold);
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if ( ret == OAM_E_OK ) {
        if ( type != CTC_PORT_ALARM ) {
            ctc_oam_onu_alarm_syn_db( );
        }
        else {

            ctc_oam_port_alarm_syn_db( port);
        }
    }

#endif
#endif/*END_HAVE_ZTE_OAM*/

    return ret;

}



/***************** Statics Adapt START ***********************/
oam_status ctc_oam_onu_stats_monitor_status_get_adapt(
    oam_port_id_t port,
    oam_uint16 *monitor_status,
    oam_uint32 *monitor_period)
{
    return ctc_onu_stats_monitor_status_get(port, monitor_status, monitor_period);
}

oam_status ctc_oam_onu_stats_monitor_status_set_adapt(
    oam_port_id_t port,
    oam_uint16 monitor_status,
    oam_uint32  monitor_period)
{
    return ctc_onu_stats_monitor_status_set(port, monitor_status, monitor_period);
}

oam_status ctc_oam_onu_curr_stats_get_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    return ctc_onu_monitor_stats_current_get(port, stats);
}

oam_status ctc_oam_onu_curr_stats_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    return ctc_onu_stats_monitor_restart(port);
}

oam_status ctc_oam_onu_histroy_stats_get_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    return ctc_onu_monitor_stats_histroy_get(port, stats);
}

oam_status ctc_oam_auth_keep_silence_set_adapt(oam_boolean enable)
{
    return OAM_E_OK;
}

#ifdef HAVE_ZTE_OAM
oam_status ctc_get_port_info(oam_port_id_t port, ctc_port_info_t *info)
{
    return OAM_E_OK;
}
#endif/*END_HAVE_ZTE_OAM*/

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
    return OAM_E_OK;
}

oam_status ctc_oam_fw_get_adapt(oam_ctc_onu_fw_ver_t *fw_ver)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onucap2_get_adapt(oam_ctc_onu_capabilities2_t *onu_cap2)
{
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

oam_status ctc_oam_keep_silence_time_get_adapt(oam_uint8 *keep)
{
    *keep = 60;

    return OAM_E_OK;
}

oam_status ctc_oam_enc_key_get_adapt(oam_uint8 inUserKeyIdx, oam_uint32 *key)
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

oam_status ctc_oam_onu_uni_stat_get_adapt(oam_ctc_onu_port_stats_t *stats)
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
    return OAM_E_OK;
}

#ifdef HAVE_ZTE_OAM
oam_status ctc_oam_cls_syn_db (oam_port_id_t port)
{
    return OAM_E_OK;
}

oam_status ctc_oam_mc_vlan_syn_db ( oam_port_id_t port)
{
    return OAM_E_OK;
}


oam_status ctc_mc_mode_set_adapt(
    oam_uint32 mode
)
{
    return OAM_E_OK;
}

oam_status ctc_mc_gmi_set_adapt (
    oam_uint32 gmi
)
{
    return OAM_E_OK;
}

oam_status ctc_port_storm_control_set_adapt (
    oam_port_id_t            port_id,
    oam_uint32               storm_type,
    oam_boolean              enable,
    oam_uint32               rate
)
{
    return OAM_E_OK;
}

#if 0
oam_status ctc_onu_schedule_mode_set_adapt (
    cs_sdl_sched_type_t       sche_mode
)
{
    return OAM_E_OK;
}
#endif

oam_status ctc_port_queue_size_set_adapt (
    oam_port_id_t                   port,
    oam_uint32                       queue_id,
    oam_uint32                       queue_size
)
{
    return OAM_E_OK;
}




oam_status ctc_onu_1pcos_set_adapt (
    oam_uint32                       pri,
    oam_uint32                       cos
)
{
    return OAM_E_OK;
}

oam_status ctc_onu_mtu_set_adapt (
    oam_uint16           mtu
)
{
    return OAM_E_OK;
}

oam_status ctc_oam_onu_alarm_syn_db (  )
{
    return OAM_E_OK;
}


oam_status ctc_oam_port_alarm_syn_db ( oam_port_id_t port  )
{
    return OAM_E_OK;
}

#endif/*END_HAVE_ZTE_OAM*/


