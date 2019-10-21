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

#include "oam_main.h"
#include "oam_cmn.h"
#include "oam_std_pdu.h"
#include "oam_ctc_pdu.h"
#include "oam_ctc_client.h"
#include "oam_ctc_adapt.h"
#include "oam_ctc_stats.h"
#include "oam_ctc_alarm.h"
#include "oam_ctc_swimage.h"

#include "swdlapp.h"
#include "opm_api.h"
#include "epon_port.h"
#include "cs_version.h"
#include "scfg.h"
#include "mpcp.h"
#include "stats_api.h"
#include "sdl_peri.h"
#include "led.h"
#include "gpio_api.h"
#include "khook.h"
#include "db_lynxe.h"

#ifdef ZTE_SPECIFIC
#include "db_zte.h"
#endif/*END_ZTE_SPECIFIC*/

static oam_uint8 g_holdover_state = OAM_CTC_ONU_HOLDOVER_DEACTIVED;
static oam_uint32 g_holdover_time = 0;
static oam_uint32 g_normal_gate_time = 0;
static oam_uint32 g_laser_tx_power_timer = 0;
static oam_uint32 g_ctc_oam_cfg_counter = 0 ;

static void __laser_tx_power_on(void *data)
{
    cs_epon_port_laser_mode_set(0, (CS_PORT_TYPE_EPON << 8), CS_EPON_PORT_LASER_MODE_BURST);
    //cs_gpio_onu_tx_pwr_enable(0, 0, TRUE);
    cs_gpio_write(0, 6/*GPIO_PIN6*/, 1);
    led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_ON);
    
    g_laser_tx_power_timer = 0;
}

oam_status ctc_oam_cls_syn_db ( oam_port_id_t port)
{
#if 0
    oam_uint8                   num = 0;
#ifdef ZTE_SPECIFIC    
    db_zte_cls_t              db_cls;
#endif/*END_ZTE_SPECIFIC*/
    cs_sal_classification_t  cfg [8];
    cs_status                  ret = 0;

    ret = cs_sal_port_classification_get(0, __SAL_ETH_PORT(port), &num, cfg);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

#ifdef ZTE_SPECIFIC    
    memset(&db_cls, 0x00, sizeof(db_zte_cls_t));
    db_cls.port_id = port;
    db_cls.num = num;
    memcpy (db_cls.rule, cfg , sizeof (cs_sal_classification_t )*num) ;

    cs_db_entry_mod(TB_ZTE_CLS , (oam_uint8 *) &db_cls, sizeof(db_zte_cls_t),
                    column_bit(TB_ZTE_CLS, "ALL"));
#endif/*END_ZTE_SPECIFIC*/
#endif
    return 0;

}


oam_status ctc_oam_mc_vlan_syn_db ( oam_port_id_t port)
{
   // oam_uint16            number = 0;
#ifdef ZTE_SPECIFIC    
    db_zte_mc_vlan_t      db_mc_vlan;
#endif/*END_ZTE_SPECIFIC*/
    //oam_uint16            vlan_cfg [8];
    cs_status             ret = 0;

//    cs_sal_mc_vlan_get (0, __SAL_ETH_PORT(port), vlan_cfg, &number);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

#ifdef ZTE_SPECIFIC    
    memset(&db_mc_vlan, 0x00, sizeof(db_zte_mc_vlan_t));
    db_mc_vlan.port_id = port;
    db_mc_vlan.num = number;
    memcpy (db_mc_vlan.vlan, vlan_cfg , sizeof (oam_uint16 )*number) ;

    cs_db_entry_mod(TB_ZTE_MC_VLAN , (void *) &db_mc_vlan, sizeof(db_zte_mc_vlan_t),
                    column_bit(TB_ZTE_MC_VLAN, "ALL"));
#endif/*END_ZTE_SPECIFIC*/

    return ret;

}


/***************** SAL ADAPT START ****************************/
oam_status ctc_oam_eth_admin_state_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin)
{
#if 0
    cs_sal_eth_port_admin_t   admin_status;

    if (admin == NULL) {
        return OAM_E_ERROR;
    }

    __PORT_CHECK(port);

    OAM_ORG_DEBUG("port = %d \r\n",  port);

    if (cs_sal_eth_port_admin_get (0, __SAL_ETH_PORT(port), &admin_status)) {
        return OAM_E_ERROR;
    }

    if (admin_status == CS_SAL_ETH_PORT_ADMIN_DOWN) {
        *admin = OAM_INTF_PHY_ADMIN_DIS;
    }
    else {
        *admin = OAM_INTF_PHY_ADMIN_EN;
    }
#endif
    return OAM_E_OK;
}


oam_status ctc_oam_eth_auto_admin_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin)
{
#if 0
    cs_sal_eth_port_mode_t mode = 0;

    if (admin == NULL) {
        return OAM_E_ERROR;
    }

    OAM_ORG_DEBUG("port = %d \r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_eth_port_mode_get(0, __SAL_ETH_PORT(port), &mode)) {
        return OAM_E_ERROR;
    }

    if (mode >= CS_SAL_ETH_PORT_MODE_FORCE_RATE_10_DUPLEX_FULL) {
        *admin = OAM_INTF_PHY_ADMIN_DIS;
    }
    else {
        *admin = OAM_INTF_PHY_ADMIN_EN;
    }
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_eth_auto_local_cap_get_adapt(
    oam_port_id_t port,
    oam_uint32 *value,
    oam_uint8 *getNum)
{
    //TODO:not completed
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

    //TOPO: code from LynxC, not sure the Lynxd has change the cap

    if ((value == NULL) || (getNum == NULL)) {
        return OAM_E_ERROR;
    }

    OAM_ORG_DEBUG("port = %d \r\n",  port);

    __PORT_CHECK(port);

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

    if ((value == NULL) || (getNum == NULL)) {
        return OAM_E_ERROR;
    }

    OAM_ORG_DEBUG("port = %d \r\n",  port);

    __PORT_CHECK(port);

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
#if 0
    /*
     #define OAM_INTF_FEC_ABILITY_UNKOWN             1
     #define OAM_INTF_FEC_SUPPORDTED                 2
     #define OAM_INTF_FEC_NOT_SUPPORTED              3
     */
    cs_boolean                enable;

    if(cs_sal_epon_onu_fec_ability_get(0, -1, &enable)){
        return OAM_E_ERROR;
    }

    if (enable == 0)
        *ability = OAM_INTF_FEC_NOT_SUPPORTED;
    else if (enable == 1)
        *ability = OAM_INTF_FEC_SUPPORDTED;
    else
        *ability = OAM_INTF_FEC_ABILITY_UNKOWN;
#endif

    *ability = OAM_INTF_FEC_SUPPORDTED;

    return OAM_E_OK;

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

    cs_boolean                mode;
    oam_status                 retCode;
    cs_port_id_t port = (CS_PORT_TYPE_EPON << 8);
    cs_boolean  tx_enable = 0, rx_enable = 0;
#ifdef ZTE_SPECIFIC    
    db_zte_onu_svc_t          svc;
#endif/*END_ZTE_SPECIFIC*/

    if (fecMode == OAM_INTF_FEC_MODE_EN){
        mode = 1;
        tx_enable = 1;
        rx_enable = 1;
    }
    else{
        mode = 0;
        tx_enable = 0;
        rx_enable = 0;
    }

    retCode =  cs_epon_port_fec_enable_set(0,
                                       port,
                                       tx_enable, rx_enable);

    if (retCode)
        return OAM_E_ERROR;

#ifdef ZTE_SPECIFIC    

    if ( retCode == CS_E_OK ) {
        memset(&svc, 0x00, sizeof(svc));
        svc.fec_mode = mode;
        cs_db_entry_mod(TB_ZTE_ONU_SVC, (void *) &svc, sizeof(svc),
                        column_bit(TB_ZTE_ONU_SVC, "fec_mode"));
    }

#endif/*END_ZTE_SPECIFIC*/

    return OAM_E_OK;
}

oam_status ctc_oam_eth_fec_mode_get_adapt(
    oam_uint32 *fecMode)
{
#if 0
    /*
    *fecMode = OAM_INTF_FEC_MODE_EN;
    return OAM_E_OK;
    */
    cs_boolean                mode;

    if(cs_sal_epon_onu_fec_get(0, -1, &mode)){
        return OAM_E_ERROR;
    }

    if (mode == 1)
        *fecMode = OAM_INTF_FEC_MODE_EN;
    else if (mode == 0)
        *fecMode = OAM_INTF_FEC_MODE_DIS;
    else
        *fecMode = OAM_INTF_FEC_MODE_UNKOWN;
#endif
    return OAM_E_OK;

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
#if 0
    cs_sal_eth_port_status_t    status;

    if (linkState == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d \r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_eth_port_status_get(0, __SAL_ETH_PORT(port), &status)) {
        return OAM_E_ERROR;
    }

    if (status.link_status == CS_SAL_LINK_DOWN) {
        *linkState = OAM_CTC_ONU_ETH_DOWN;
    }
    else {
        *linkState = OAM_CTC_ONU_ETH_UP;
    }
#endif
    return OAM_E_OK;

}


oam_status ctc_oam_eth_port_pause_get_adapt(
    oam_port_id_t port,
    oam_uint8  *flowControl)
{
#if 0
    oam_uint16    quanta;

    if (flowControl == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d \r\n", port);

    __PORT_CHECK(port);

    if (cs_sal_eth_port_pause_quanta_get(0, __SAL_ETH_PORT(port), &quanta)) {
        return OAM_E_ERROR;
    }

    if (quanta) {
        *flowControl = 1;
    }
    else {
        *flowControl = 0;
    }
#endif
    return OAM_E_OK;

}

oam_status ctc_oam_eth_port_pause_set_adapt(
    oam_port_id_t port,
    oam_uint8 flowControl)
{
#if 0
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t port_info;
#endif/*END_ZTE_SPECIFIC*/

    OAM_ORG_DEBUG("port = %d, flowctrl = %d\r\n",  port, flowControl);

    __PORT_CHECK(port);

    if (cs_sal_eth_port_pause_quanta_set(0, __SAL_ETH_PORT(port), flowControl)) {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC
    memset(&port_info, 0x00, sizeof(port_info));
    port_info.port_id = port;
    port_info.pause = flowControl;
    cs_db_entry_mod(TB_ETH_PORT, (void *) &port_info, sizeof(port_info),
                    column_bit(TB_ETH_PORT, "pause"));
#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}


oam_status ctc_oam_eth_admin_state_set_adapt(
    oam_port_id_t port,
    oam_uint32 phyAdmin)
{
#if 0
    cs_sal_eth_port_admin_t     port_admin = CS_SAL_ETH_PORT_ADMIN_DOWN;
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t port_info;
#endif/*END_ZTE_SPECIFIC*/


    OAM_ORG_DEBUG("port = %d, phyAdmin = %d\r\n", port, phyAdmin);

    __PORT_CHECK(port);

    if (phyAdmin == OAM_INTF_PHY_ADMIN_DIS) {
        port_admin = CS_SAL_ETH_PORT_ADMIN_DOWN;
    }
    else {
        port_admin = CS_SAL_ETH_PORT_ADMIN_UP;
    }

    if (cs_sal_eth_port_admin_set(0, __SAL_ETH_PORT(port), port_admin)) {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC
    memset(&port_info, 0x00, sizeof(port_info));
    port_info.port_id = port;
    port_info.admin = port_admin;
    cs_db_entry_mod(TB_ETH_PORT, (void *) &port_info, sizeof(port_info),
                    column_bit(TB_ETH_PORT, "admin"));
#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}


oam_status ctc_oam_auto_neg_restart_set_adapt(
    oam_port_id_t port)
{
#if 0
    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_eth_port_autoneg_restart(0, __SAL_ETH_PORT(port))) {
        return OAM_E_ERROR;
    }
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_auto_neg_ctrl_set_adapt(
    oam_port_id_t port,
    oam_uint32 autoNegAdmin)
{
#if 0
    cs_sal_eth_port_mode_t mode;
    cs_uint8  auto_neg = 0;
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t   port_info;
#endif/*END_ZTE_SPECIFIC*/

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_eth_port_mode_get(0, __SAL_ETH_PORT(port), &mode)) {
        return OAM_E_ERROR;
    }

    if (autoNegAdmin == OAM_INTF_PHY_ADMIN_DIS) {
        if ((mode == CS_SAL_ETH_PORT_MODE_AUTO_RATE_AUTO_DUPLEX_AUTO) ||
            (mode == CS_SAL_ETH_PORT_MODE_AUTO_RATE_10_100_DUPLEX_AUTO) ||
            (mode == CS_SAL_ETH_PORT_MODE_FORCE_RATE_1000_DUPLEX_FULL)) {

            mode = CS_SAL_ETH_PORT_MODE_FORCE_RATE_100_DUPLEX_FULL;
            auto_neg = 0;
        }
    }
    else {
        if ((mode != CS_SAL_ETH_PORT_MODE_AUTO_RATE_AUTO_DUPLEX_AUTO) &&
            (mode != CS_SAL_ETH_PORT_MODE_AUTO_RATE_10_100_DUPLEX_AUTO) &&
            (mode != CS_SAL_ETH_PORT_MODE_FORCE_RATE_1000_DUPLEX_FULL)) {
            mode = CS_SAL_ETH_PORT_MODE_AUTO_RATE_AUTO_DUPLEX_AUTO;
            auto_neg = 0;
        }
    }

    if (cs_sal_eth_port_mode_set(0, __SAL_ETH_PORT(port), mode)) {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0) {
        memset(&port_info, 0x00, sizeof(port_info));
        port_info.port_id = port;
        port_info.auto_neg = auto_neg;
        cs_db_entry_mod(TB_ETH_PORT, (void *) &port_info, sizeof(port_info),
                        column_bit(TB_ETH_PORT, "auto_neg"));
    }

#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_eth_port_policing_get_adapt(
    oam_port_id_t port,
    oam_uint8  *oper,
    oam_uint32 *cir,
    oam_uint32 *cbs,
    oam_uint32 *ebs)
{
#if 0
    oam_status ret;
    cs_sdl_policy_t       policy;

    ret = cs_sal_epon_onu_port_policy_get(0, -1, __SAL_ETH_PORT(port), &policy);

    if (ret) {
        return OAM_E_ERROR;
    }

    *oper = policy.enable;
    *cir = policy.rate;
    *cbs = policy.cbs;
    *ebs = policy.ebs;
#endif
    return OAM_E_OK;

}

oam_status ctc_oam_eth_port_policing_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  cbs,
    oam_uint32  ebs)
{
#if 0
    oam_status ret;
    cs_sdl_policy_t       policy;
#ifdef ZTE_SPECIFIC    
    cs_db_eth_port_t   port_info;
#endif/*END_ZTE_SPECIFIC*/

    policy.rate = cir;
    policy.cbs = cbs;
    policy.ebs = ebs;

    if (oper == CTC_PORT_POLICY_OPER_DISABLE) {
        policy.enable = 0;
    }
    else {
        policy.enable = 1;
    }

    ret = cs_sal_epon_onu_port_policy_set(0, -1, __SAL_ETH_PORT(port), &policy);

    if (ret)
        return OAM_E_ERROR;

#ifdef ZTE_SPECIFIC    
    memset(&port_info, 0x00, sizeof(port_info));
    port_info.port_id = port;
    port_info.policy_rate = cir;
    port_info.policy_cbs = cbs;
    port_info.policy_ebs = ebs;
    port_info.policy_enable = policy.enable;
    cs_db_entry_mod(TB_ETH_PORT, (void *) &port_info, sizeof(port_info),
                    column_bit(TB_ETH_PORT, "policy_rate") |
                    column_bit(TB_ETH_PORT, "policy_cbs") |
                    column_bit(TB_ETH_PORT, "policy_ebs") |
                    column_bit(TB_ETH_PORT, "policy_enable"));
#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}


oam_status ctc_oam_eth_ds_rate_limit_get_adapt(
    oam_port_id_t port,
    oam_uint8 *oper,
    oam_uint32 *cir,
    oam_uint32 *pir)
{
#if 0
    oam_status ret;
    cs_sdl_policy_t       rate;

    ret = cs_sal_epon_onu_port_ds_rate_limit_get(0, -1, __SAL_ETH_PORT(port),
            &rate);

    if (ret) {
        return OAM_E_ERROR;
    }

    *oper = rate.enable;
    *cir = rate.rate;
    *pir = rate.cbs;
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_eth_ds_rate_limit_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  pir)
{
#if 0
    oam_status ret;
    cs_sdl_policy_t       rate;
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t      port_info;
#endif/*END_ZTE_SPECIFIC*/

    rate.rate = cir;
    rate.cbs = pir;
    rate.ebs = 0;

    if (oper == CTC_PORT_POLICY_OPER_DISABLE) {
        rate.enable = 0;
    }
    else {
        rate.enable = 1;
    }

    ret = cs_sal_epon_onu_port_ds_rate_limit_set(0, -1, __SAL_ETH_PORT(port),
            &rate);

    if (ret)
        return OAM_E_ERROR;

#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0) {
        memset(&port_info, 0x00, sizeof(port_info));
        port_info.port_id = port;
        port_info.ds_rate_limit_enable = rate.enable;
        port_info.ds_rate_limit = rate.rate;
        port_info.ds_cbs = rate.cbs;
        cs_db_entry_mod(TB_ETH_PORT, (void *) &port_info, sizeof(port_info),
                        column_bit(TB_ETH_PORT, "ds_rate_limit_enable") |
                        column_bit(TB_ETH_PORT, "ds_rate_limit") |
                        column_bit(TB_ETH_PORT, "ds_cbs"));
    }

#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mac_limit_set_adapt(
    oam_port_id_t port,
    oam_uint16  num)
{
#if 0
#ifdef ZTE_SPECIFIC
    db_zte_mac_limit_t    mac_limit_cfg;
#endif/*END_ZTE_SPECIFIC*/

    OAM_ORG_DEBUG("port = %d, mac_limt = %d\r\n",  port, num);

    __PORT_CHECK(port);

    if (cs_sal_fdb_mac_limit_set(0, __SAL_ETH_PORT(port), num)) {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0) {
        memset(&mac_limit_cfg, 0x00, sizeof(db_zte_mac_limit_t));
        mac_limit_cfg.port_id = port;
        mac_limit_cfg.num = num;

        cs_db_entry_mod(TB_ZTE_MAC_LIMIT, (void *) &mac_limit_cfg,
                        sizeof(db_zte_mac_limit_t), column_bit(TB_ZTE_MAC_LIMIT, "ALL"));
    }

#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mac_limit_get_adapt(
    oam_port_id_t port,
    oam_uint16  *num)
{
#if 0
    oam_uint32    mac_limit = 0;

    if (num == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_fdb_mac_limit_get(0, __SAL_ETH_PORT(port), &mac_limit)) {
        return OAM_E_ERROR;
    }

    *num = mac_limit & 0xffff;
#endif
    return OAM_E_OK;

}


oam_status ctc_oam_onu_port_mode_set_adapt(
    oam_port_id_t port,
    oam_uint8  mode)
{
#if 0
    cs_sal_eth_port_mode_t port_mode = 0;

    OAM_ORG_DEBUG("port = %d, mode = %d\r\n", port, mode);

    __PORT_CHECK(port);

    switch (mode) {
        case CTC_OAM_PORT_MODE_10M_HALF:
            port_mode = CS_SAL_ETH_PORT_MODE_FORCE_RATE_10_DUPLEX_HALF;
            break;

        case CTC_OAM_PORT_MODE_10M_FULL:
            port_mode = CS_SAL_ETH_PORT_MODE_FORCE_RATE_10_DUPLEX_FULL;
            break;

        case CTC_OAM_PORT_MODE_100M_HALF:
            port_mode = CS_SAL_ETH_PORT_MODE_FORCE_RATE_100_DUPLEX_HALF;
            break;

        case CTC_OAM_PORT_MODE_100M_FULL:
            port_mode = CS_SAL_ETH_PORT_MODE_FORCE_RATE_100_DUPLEX_FULL;
            break;

        case CTC_OAM_PORT_MODE_1000M_FULL:
            port_mode = CS_SAL_ETH_PORT_MODE_AUTO_RATE_1000_DUPLEX_AUTO;
            break;

        default:
            return OAM_E_PARAM;
    }

    if (cs_sal_eth_port_mode_set(0, port, port_mode)) {
        return OAM_E_ERROR;
    }
#endif
    return OAM_E_OK;
}


oam_status ctc_oam_onu_port_stats_get_adapt(
    oam_port_id_t port,
    oam_ctc_eth_port_stats_t *stats)
{
#if 0
    stats_uni_info_t uni_stats;

    if (!stats)
        return CS_E_PARAM;

    memset(&uni_stats, 0, sizeof(uni_stats));

    if (CS_E_OK != cs_stats_uni_get(port, &uni_stats)) {
        return CS_E_ERROR;
    }

    stats->in_octets = uni_stats.rxbyte_cnt;
    stats->in_ucast_pkt = uni_stats.rxucfrm_cnt;
    stats->in_nucast_pkt = uni_stats.rxmcfrm_cnt + uni_stats.rxbcfrm_cnt;
    stats->in_discard = 0;
    stats->in_error = uni_stats.rxcrcerrfrm_cnt + uni_stats.rxundersizefrm_cnt +
                      uni_stats.rxoversizefrm_cnt ;
    stats->out_octets = uni_stats.txbyte_cnt;
    stats->out_ucast_pkt = uni_stats.txucfrm_cnt;
    stats->out_nucast_pkt = uni_stats.txmcfrm_cnt + uni_stats.txbcfrm_cnt;
    stats->out_discard = 0;
    stats->out_error = uni_stats.txcrcerrfrm_cnt + uni_stats.txoversizefrm_cnt;
#endif
    return CS_E_OK;
}



oam_status ctc_oam_pon_port_stat_get_adapt(
    oam_ctc_onu_llid_stats1 *stats1,
    oam_ctc_onu_llid_stats2 *stats2)
{
#if 0
    stats_pon_info_t pon_stats;

    if (!stats1 && !stats2) {
        return CS_E_PARAM;
    }

    memset(&pon_stats, 0, sizeof(pon_stats));

    cs_stats_pon_get(&pon_stats);

    if (stats1) {
        stats1->out_pkts = pon_stats.pon_txframe_cnt + pon_stats.pon_txmcframe_cnt +
                           pon_stats.pon_txbcframe_cnt;
        stats1->out_octets = pon_stats.pon_tx_byte_cnt;
        stats1->out_mc_pkts = pon_stats.pon_txmcframe_cnt;
        stats1->out_bc_pkts = pon_stats.pon_txbcframe_cnt;
        stats1->in_pkts = pon_stats.pon_frame_cnt + pon_stats.pon_mcframe_cnt +
                          pon_stats.pon_bcframe_cnt;
        stats1->in_octets = pon_stats.pon_byte_cnt;
        stats1->in_mc_pkts = pon_stats.pon_mcframe_cnt;
        stats1->in_bc_pkts = pon_stats.pon_bcframe_cnt;
    }

    if (stats2) {
        cs_sdl_fec_cnt_t   fec_cnt;
        //epon_request_onu_fec_cnt_get(context, ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, &fec_cnt);
        cs_sal_epon_onu_fec_cnt_get(0, -1, &fec_cnt);
        stats2->in_crc8_err_pkts = pon_stats.pon_crc8_err;
        stats2->in_fec_crct_blks = fec_cnt.rx_correct_blks;
        stats2->in_fec_uncrct_blks = fec_cnt.rx_uncorrect_blks;

        stats2->out_mpcp_reg_pkts = 0;
        stats2->out_mpcp_rreq_pkts = pon_stats.pon_mpcp_tx_req_reg;
        stats2->out_mpcp_rpt_pkts = pon_stats.pon_mpcp_tx_rpt;
        stats2->in_mpcp_gate_pkts = pon_stats.pon_mpcp_uc_gat_norm +
                                    pon_stats.pon_mpcp_uc_gat_frpt + pon_stats.pon_mpcp_bc_gat_bdis;
        stats2->in_mpcp_reg_pkts = pon_stats.pon_mpcp_bc_reg_ack;

        stats2->out_mpcp_pkts = stats2->out_mpcp_rreq_pkts + stats2->out_mpcp_rpt_pkts +
                                pon_stats.pon_mpcp_tx_ack_ack;
        stats2->in_mpcp_pkts = stats2->in_mpcp_gate_pkts + stats2->in_mpcp_reg_pkts;
    }
#endif
    return CS_E_OK;
}

oam_status ctc_oam_onu_port_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8  *mode)
{
#if 0
    cs_sal_eth_port_mode_t port_mode;

    if (mode == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_eth_port_mode_get(0, __SAL_ETH_PORT(port), &port_mode)) {
        return OAM_E_ERROR;
    }

    switch (port_mode) {
        case CS_SAL_ETH_PORT_MODE_AUTO_RATE_AUTO_DUPLEX_AUTO:
        case CS_SAL_ETH_PORT_MODE_AUTO_RATE_10_100_DUPLEX_AUTO:
            *mode = CTC_OAM_PORT_MODE_AUTO;
            break;

        case CS_SAL_ETH_PORT_MODE_FORCE_RATE_10_DUPLEX_HALF:
            *mode = CTC_OAM_PORT_MODE_10M_HALF;
            break;

        case CS_SAL_ETH_PORT_MODE_FORCE_RATE_10_DUPLEX_FULL:
            *mode = CTC_OAM_PORT_MODE_10M_FULL;
            break;

        case CS_SAL_ETH_PORT_MODE_FORCE_RATE_100_DUPLEX_HALF:
            *mode = CTC_OAM_PORT_MODE_100M_HALF;
            break;

        case CS_SAL_ETH_PORT_MODE_FORCE_RATE_100_DUPLEX_FULL:
            *mode = CTC_OAM_PORT_MODE_100M_FULL;
            break;

        case CS_SAL_ETH_PORT_MODE_AUTO_RATE_1000_DUPLEX_AUTO:
            *mode = CTC_OAM_PORT_MODE_1000M_FULL;
            break;

        default:
            break;
    }
#endif
    return OAM_E_OK;
}


oam_status ctc_oam_onu_port_work_mode_get_adapt(
    oam_port_id_t portId,
    oam_uint8  *mode)
{
#if 0
    cs_sal_eth_port_status_t port_status;

    if (mode == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  portId);

    __PORT_CHECK(portId);

    if (cs_sal_eth_port_status_get(0, portId + 3, &port_status)) {
        return OAM_E_ERROR;
    }

    if (port_status.link_status == CS_SAL_LINK_DOWN) {
        *mode = CTC_OAM_PORT_MODE_10M_HALF;
    }
    else {
        switch (port_status.speed) {
            case CS_SAL_SPEED_10MBPS:
                if (port_status.duplex == CS_SAL_DUPLEX_FULL) {
                    *mode = CTC_OAM_PORT_MODE_10M_FULL;
                }
                else {
                    *mode = CTC_OAM_PORT_MODE_10M_HALF;
                }

                break;

            case CS_SAL_SPEED_100MBPS:
                if (port_status.duplex == CS_SAL_DUPLEX_FULL) {
                    *mode = CTC_OAM_PORT_MODE_100M_FULL;
                }
                else {
                    *mode = CTC_OAM_PORT_MODE_100M_HALF;
                }

                break;

            case CS_SAL_SPEED_1000MBPS:
                *mode = CTC_OAM_PORT_MODE_1000M_FULL;
                break;

            default:
                *mode = CTC_OAM_PORT_MODE_10M_HALF;
                break;
        }

    }
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_eth_mac_aging_time_get_adapt(
    oam_uint32 *aging_time)
{
#if 0
    if (aging_time == NULL) {
        return OAM_E_PARAM;
    }

    if (cs_sal_fdb_age_get(0, aging_time)) {
        return OAM_E_ERROR;
    }
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_eth_mac_aging_time_set_adapt(
    oam_uint32 aging_time)
{
#if 0
#ifdef ZTE_SPECIFIC
    db_zte_onu_svc_t   svc;
#endif/*END_ZTE_SPECIFIC*/

    OAM_ORG_DEBUG("aging_time = %d\r\n",  aging_time);

    if (cs_sal_fdb_age_set(0, aging_time)) {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC
    memset(&svc, 0x00, sizeof(svc));
    svc.mac_aging_time = aging_time;
    cs_db_entry_mod(TB_ZTE_ONU_SVC, (cs_uint8 *) &svc, sizeof(svc),
                    column_bit(TB_ZTE_ONU_SVC, "mac_aging_time"));
#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
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
    mode[5] = 0x3; /* fast_leave at mld snoop mode */
    return OAM_E_OK;
}

oam_status ctc_oam_onu_fast_leave_state_get_adapt(
    oam_uint32 *state)
{
    *state = cs_trsvc_igmpproxy_quickleave_get();
    return OAM_E_OK;
}

oam_status ctc_oam_fast_leave_ctrl_set_adapt(
    oam_uint32 fastLeaveAdmin)
{
    cs_status             ret = CS_E_OK;
#ifdef ZTE_SPECIFIC    
    db_zte_fast_leave_t   db_fast_leave;
#endif/*END_ZTE_SPECIFIC*/
    //cs_boolean fastleave = 0;

    ret = cs_trsvc_igmpproxy_quickleave_set(fastLeaveAdmin);

#ifdef ZTE_SPECIFIC    

    if ( ret == CS_E_OK ) {
        db_fast_leave.fast_leave = fastLeaveAdmin;
        cs_db_entry_mod( TB_ZTE_FAST_LEAVE, (void *) &db_fast_leave,
                         sizeof( db_zte_fast_leave_t), column_bit(TB_ZTE_FAST_LEAVE, "ALL"));
    }

#endif/*END_ZTE_SPECIFIC*/

    return 0;
}

oam_status ctc_oam_onucap_get_adapt(
    oam_ctc_onu_capabilities_t *onu_cap)
{
    oam_uint8 hasBattery = 1;//cs_gpio_ups_valid();
    oam_uint64 portBitmap = 0;
    oam_uint8 i = 0;

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
    onu_cap->numGEPorts = 1;
    onu_cap->geBitMap = htonll(portBitmap);
    onu_cap->numFEPorts = 1;
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
    return OAM_E_NOT_SUPPORT;
}


oam_status ctc_oam_onu_power_saving_cap_get_adapt(
    oam_uint8 *sleep_mode,
    oam_uint8 *early_wakeup)
{
    return OAM_E_NOT_SUPPORT;
}


oam_status ctc_oam_onu_power_saving_config_get_adapt(
    oam_uint8 *early_wakeup,
    oam_uint64 *max_sleep_duration)
{
    return OAM_E_NOT_SUPPORT;
}

oam_status ctc_oam_onu_power_saving_config_set_adapt(
    oam_uint8 early_wakeup,
    oam_uint64 max_sleep_duration)
{
    return OAM_E_NOT_SUPPORT;
}


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

oam_status ctc_oam_onu_cfg_counter_get_adapt(oam_uint32 *counter)
{
#ifdef ZTE_SPECIFIC
    db_zte_onu_svc_t   svc;

    cs_db_entry_get(TB_ZTE_ONU_SVC, (void *) &svc, sizeof(svc));
    g_ctc_oam_cfg_counter = svc.cfg_counter;
#endif/*END_ZTE_SPECIFIC*/

    *counter = g_ctc_oam_cfg_counter;
    return OAM_E_OK;
}

oam_status ctc_oam_onu_cfg_counter_set_adapt(oam_uint32 counter)
{
#ifdef ZTE_SPECIFIC
    db_zte_onu_svc_t   svc;

    memset(&svc, 0, sizeof(db_zte_onu_svc_t));
    svc.cfg_counter = counter;
    cs_db_entry_mod(TB_ZTE_ONU_SVC, (void *) &svc, sizeof(svc),
                    column_bit(TB_ZTE_ONU_SVC, "cfg_counter"));
    cs_db_lynxe_save();
#endif/*END_ZTE_SPECIFIC*/

    g_ctc_oam_cfg_counter = counter;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_cfg_mgmt_set_adapt(oam_uint8 action)
{
#ifdef ZTE_SPECIFIC

    switch (action) {
        case 1:
            cs_db_lynxe_save();
            break;

        case 2:
            cs_db_zte_init();
            cs_db_lynxe_save();
            /*
            cs_db_restore_zte_table();
            */

        case 3:
            cs_db_zte_init();
            cs_db_lynxe_save();
            break;

        default:
            return CS_E_ERROR;
    }

#endif/*END_ZTE_SPECIFIC*/

    return OAM_E_OK;
}

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
    oam_status ret = CS_OK;
    /*oam_uint8 gate_timeout = 0;*/
    cs_boolean dereg_en;
    cs_port_id_t port = (CS_PORT_TYPE_EPON << 8);

    if (holdover_state == OAM_CTC_ONU_HOLDOVER_DEACTIVED ) {
        /*startup_config_read(CFG_ID_MPCP_TIMEOUT, 1, &gate_timeout);
        ret = cs_epon_mpcp_timer_set(0, port, gate_timeout*10, 1);*/
        if (g_normal_gate_time != 0) {
            ret = cs_epon_mpcp_timer_set(0, port, g_normal_gate_time,  1);
        }

    }
    else if (holdover_state == OAM_CTC_ONU_HOLDOVER_ACTIVED
             && holdover_time >= OAM_CTC_ONU_HOLDOVER_TIME_MIN
             && holdover_time <= OAM_CTC_ONU_HOLDOVER_TIME_MAX ) {

        ret = cs_epon_mpcp_timer_get(0, port, &g_normal_gate_time, &dereg_en);

        if (ret)
            return OAM_E_ERROR;

        ret = cs_epon_mpcp_timer_set(0, port, holdover_time, 0);

        if (ret)
            return OAM_E_ERROR;

    }
    else {
        return OAM_E_PARAM;
    }

    g_holdover_state = holdover_state;
    g_holdover_time = holdover_time;

    return 0;
}


/************************VLAN ADAPT START ***************************/
oam_status ctc_oam_onu_default_vlan_tag_get_adapt(
    oam_port_id_t port,
    oam_uint16 *tpid,
    oam_uint16 *tag)
{
#if 0
    cs_sal_vlan_tag_t    def_vlan;
    cs_sal_vlan_mode_t   vlan_mode;
    cs_sal_vlan_cfg_t    vlan_cfg[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16            nums;

    if ((tpid == NULL) || (tag == NULL)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n", port);

    __PORT_CHECK(port);

    if (cs_sal_onu_vlan_get(0, __SAL_ETH_PORT(port), &def_vlan, &vlan_mode,
                            vlan_cfg, &nums)) {
        return OAM_E_ERROR;
    }

    *tpid = CTC_DEFAULT_TPID;
    *tag = def_vlan.vid | def_vlan.pri << 13;
#endif
    return OAM_E_OK;
}


oam_status ctc_oam_onu_vlan_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8 *mode)
{
#if 0
    cs_sal_vlan_tag_t    def_vlan;
    cs_sal_vlan_mode_t   vlan_mode;
    cs_sal_vlan_cfg_t    vlan_cfg[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16            nums;

    if (mode == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_onu_vlan_get(0, __SAL_ETH_PORT(port), &def_vlan, &vlan_mode,
                            vlan_cfg, &nums)) {
        return OAM_E_ERROR;
    }

    *mode = (oam_uint8)vlan_mode;
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_translate_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *translate_num,
    oam_ctc_onu_vlan_xlate_entry *vlan)
{
#if 0
    cs_sal_vlan_tag_t    def_vlan;
    cs_sal_vlan_mode_t   vlan_mode;
    cs_sal_vlan_cfg_t    vlan_cfg[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16            nums;
    oam_uint32            i = 0;

    if ((vlan == NULL) || (translate_num == NULL)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n", port);

    __PORT_CHECK(port);

    if (cs_sal_onu_vlan_get(0, __SAL_ETH_PORT(port), &def_vlan, &vlan_mode,
                            vlan_cfg, &nums)) {
        return OAM_E_ERROR;
    }

    while (i < nums) {
        vlan[i].tag2.tpid = CTC_DEFAULT_TPID;
        vlan[i].tag1.tpid = CTC_DEFAULT_TPID;
        vlan[i].tag2.tag = vlan_cfg[i].s_vlan.vid;
        vlan[i].tag1.tag = vlan_cfg[i].c_vlan.vid;
        ++i;
    }

    *translate_num = nums;
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_trunk_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *trunk_num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
#if 0
    cs_sal_vlan_tag_t    def_vlan;
    cs_sal_vlan_mode_t   vlan_mode;
    cs_sal_vlan_cfg_t    vlan_cfg[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16            nums;
    oam_uint32            i = 0;

    if ((vlan == NULL) || (trunk_num == NULL)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_onu_vlan_get(0, __SAL_ETH_PORT(port), &def_vlan, &vlan_mode,
                            vlan_cfg, &nums)) {
        return OAM_E_ERROR;
    }

    while (i < nums) {
        vlan[i].tpid = CTC_DEFAULT_TPID;
        vlan[i].tag = vlan_cfg[i].c_vlan.vid;
        ++i;
    }

    *trunk_num = nums;
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_agg_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *agg_num,
    ctc_oam_agg_vlan_t *vlan)
{
#if 0
    cs_sal_vlan_tag_t    def_vlan;
    cs_sal_vlan_mode_t   vlan_mode;
    cs_sal_vlan_cfg_t    vlan_cfg[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16            nums;
    oam_uint32            i = 0;

    if ((vlan == NULL) || (agg_num == NULL)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_onu_vlan_get(0, __SAL_ETH_PORT(port), &def_vlan, &vlan_mode,
                            vlan_cfg, &nums)) {
        return OAM_E_ERROR;
    }

    while (i < nums) {
        vlan[0].agg_vlan[i].tpid = CTC_DEFAULT_TPID;
        vlan[0].agg_vlan[i].tag = vlan_cfg[i].c_vlan.vid;
        ++i;
    }

    vlan[0].agg_dst_vlan.tpid = CTC_DEFAULT_TPID;
    vlan[0].agg_dst_vlan.tag = vlan_cfg[0].s_vlan.vid;
    vlan[0].agg_vlan_num = nums;

    *agg_num = 1;
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_transparent_vlan_set_adapt(
    oam_port_id_t port)
{
#if 0
    cs_sal_vlan_tag_t    def_vlan;
    app_vlan_tag_t app_dft_vlan;
    cs_sal_vlan_mode_t   vlan_mode = SAL_VLAN_MODE_TRANSPARENT;
#ifdef ZTE_SPECIFIC
    db_zte_vlan_t vlan_cfg;
#endif/*END_ZTE_SPECIFIC*/

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    def_vlan.vid = 0;
    def_vlan.pri = 0;

    if (cs_sal_onu_vlan_set(0, __SAL_ETH_PORT(port), def_vlan, vlan_mode, NULL,
                            0)) {
        return OAM_E_ERROR;
    }
    memcpy(&app_dft_vlan, &def_vlan, sizeof(app_vlan_tag_t));
    cs_sw_vlan_set(0, __SAL_ETH_PORT(port), app_dft_vlan,
                   (app_vlan_mode_t)vlan_mode, 0, NULL);
#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0) {
        memset(&vlan_cfg, 0x00, sizeof(vlan_cfg));
        vlan_cfg.port_id = port;
        vlan_cfg.vlan_mode = vlan_mode;
        cs_db_entry_mod(TB_ZTE_VLAN, (void *) &vlan_cfg, sizeof(vlan_cfg),
                        column_bit(TB_ZTE_VLAN, "vlan_mode"));
    }

#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_tag_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t vlan)
{
#if 0
    cs_sal_vlan_tag_t    def_vlan;
    app_vlan_tag_t app_dft_vlan;
    cs_sal_vlan_mode_t   vlan_mode = SAL_VLAN_MODE_TAG;
#ifdef ZTE_SPECIFIC
    db_zte_vlan_t vlan_cfg;
#endif/*END_ZTE_SPECIFIC*/

    OAM_ORG_DEBUG("port = %d, TPID = 0x%x, tag = 0x%x\r\n",
                  port, vlan.tpid, vlan.tag);

    __PORT_CHECK(port);

    def_vlan.vid = TAG2VID(vlan.tag);
    def_vlan.pri = TAG2PRI(vlan.tag);

    if (cs_sal_onu_vlan_set(0, __SAL_ETH_PORT(port), def_vlan, vlan_mode, NULL,
                            0)) {
        return OAM_E_ERROR;
    }
    memcpy(&app_dft_vlan, &def_vlan, sizeof(app_vlan_tag_t));
    cs_sw_vlan_set(0, __SAL_ETH_PORT(port), app_dft_vlan,
                   (app_vlan_mode_t)vlan_mode, 0, NULL);
#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0) {
        memset(&vlan_cfg, 0x00, sizeof(vlan_cfg));
        vlan_cfg.port_id = port;
        vlan_cfg.vlan_mode = vlan_mode;
        vlan_cfg.def_vlan.tpid = vlan.tpid;
        vlan_cfg.def_vlan.tag = vlan.tag;
        cs_db_entry_mod(TB_ZTE_VLAN, (void *) &vlan_cfg, sizeof(vlan_cfg),
                        column_bit(TB_ZTE_VLAN, "vlan_mode") |
                        column_bit(TB_ZTE_VLAN, "def_vlan"));
    }

#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_translate_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_xlate_entry *vlan)
{
#if 0
    cs_sal_vlan_tag_t    def_vlan;
    app_vlan_tag_t app_dft_vlan;
    cs_sal_vlan_mode_t   vlan_mode = SAL_VLAN_MODE_TRANSLATION;
    cs_sal_vlan_cfg_t    sal_vlan[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint32            i = 0;
#ifdef ZTE_SPECIFIC
    db_zte_vlan_t vlan_cfg;
#endif/*END_ZTE_SPECIFIC*/

    if (vlan == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d, def tpid = 0x%x, def tag = 0x%x\r\n",
                  port, defvlan.tpid, defvlan.tag);

    for (i = 0; i < num; ++i) {
        OAM_ORG_DEBUG("SVLAN tpid = 0x%x, SVLAN tag = 0x%x\r\n",
                      vlan[i].tag2.tpid, vlan[i].tag2.tag);
        OAM_ORG_DEBUG("CVLAN tpid = 0x%x, CVLAN tag = 0x%x\r\n",
                      vlan[i].tag1.tpid, vlan[i].tag1.tag);
    }

    __PORT_CHECK(port);

    def_vlan.vid = TAG2VID(defvlan.tag);
    def_vlan.pri = TAG2PRI(defvlan.tag);

    for (i = 0; i < num; ++i) {
        sal_vlan[i].s_vlan.vid = vlan[i].tag2.tag;
        sal_vlan[i].c_vlan.vid = vlan[i].tag1.tag;
    }

    if (cs_sal_onu_vlan_set(0, __SAL_ETH_PORT(port), def_vlan, vlan_mode, sal_vlan,
                            num)) {
        return OAM_E_ERROR;
    }
    memcpy(&app_dft_vlan, &def_vlan, sizeof(app_vlan_tag_t));

    cs_sw_vlan_set(0, __SAL_ETH_PORT(port), app_dft_vlan,
                   (app_vlan_mode_t)vlan_mode, num, (app_vlan_cfg_t *)sal_vlan);

#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0) {
        memset(&vlan_cfg, 0x00, sizeof(vlan_cfg));
        vlan_cfg.port_id = port;
        vlan_cfg.vlan_mode = vlan_mode;
        vlan_cfg.def_vlan.tpid = def_vlan.tpid;
        vlan_cfg.def_vlan.tag = def_vlan.vid;

        memset(vlan_cfg.vlan, 0x00, sizeof(vlan_cfg.vlan));
        vlan_cfg.vlan_num = num * 2;

        for (i = 0 ; i < num; i ++) {
            vlan_cfg.vlan[i * 2].tpid = vlan[i].tag1.tpid;
            vlan_cfg.vlan[i * 2].tag = vlan[i].tag1.tag;
            vlan_cfg.vlan[i * 2 + 1].tpid = vlan[i].tag2.tpid;
            vlan_cfg.vlan[i * 2 + 1].tag = vlan[i].tag2.tag;
        }

        cs_db_entry_mod(TB_ZTE_VLAN, (void *) &vlan_cfg, sizeof(vlan_cfg),
                        column_bit(TB_ZTE_VLAN, "vlan_mode") |
                        column_bit(TB_ZTE_VLAN, "def_vlan") |
                        column_bit(TB_ZTE_VLAN, "vlan_num") |
                        column_bit(TB_ZTE_VLAN, "vlan"));
    }

#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;

}


oam_status ctc_oam_onu_trunk_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
#if 0
    cs_sal_vlan_tag_t    def_vlan;
    app_vlan_tag_t app_dft_vlan;
    cs_sal_vlan_mode_t   vlan_mode = SAL_VLAN_MODE_TRUNK;
    cs_sal_vlan_cfg_t    sal_vlan[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint32            i = 0;
#ifdef ZTE_SPECIFIC
    db_zte_vlan_t vlan_cfg;
#endif/*END_ZTE_SPECIFIC*/

    if (vlan == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d, def tpid = 0x%x, def tag = 0x%x\r\n",
                  port, defvlan.tpid, defvlan.tag);

    for (i = 0; i < num; ++i) {
        OAM_ORG_DEBUG("SVLAN tpid = 0x%x, SVLAN tag = 0x%x\r\n",
                      vlan[i].tpid, vlan[i].tag);
        OAM_ORG_DEBUG(" SVLAN tag  = 0x%x, CVLAN tag = 0x%x\r\n",
                      vlan[i].tpid, vlan[i].tag);
    }

    __PORT_CHECK(port);

    def_vlan.vid = TAG2VID(defvlan.tag);
    def_vlan.pri = TAG2PRI(defvlan.tag);

    for (i = 0; i < num; ++i) {
        sal_vlan[i].s_vlan.vid = vlan[i].tag;
        sal_vlan[i].c_vlan.vid = vlan[i].tag;
    }


    if (cs_sal_onu_vlan_set(0, __SAL_ETH_PORT(port), def_vlan, vlan_mode, sal_vlan,
                            num)) {
        return OAM_E_ERROR;
    }
    memcpy(&app_dft_vlan, &def_vlan, sizeof(app_vlan_tag_t));
    cs_sw_vlan_set(0, __SAL_ETH_PORT(port), app_dft_vlan,
                   (app_vlan_mode_t)vlan_mode, num, (app_vlan_cfg_t *)sal_vlan);
#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0) {
        memset(&vlan_cfg, 0x00, sizeof(vlan_cfg));
        vlan_cfg.port_id = port;
        vlan_cfg.vlan_mode = vlan_mode;
        vlan_cfg.def_vlan.tpid = def_vlan.tpid;
        vlan_cfg.def_vlan.tag = def_vlan.vid;

        memset(vlan_cfg.vlan, 0x00, sizeof(vlan_cfg.vlan));
        vlan_cfg.vlan_num = num;

        for (i = 0 ; i < num; i ++) {
            vlan_cfg.vlan[i].tpid = vlan[i].tpid;
            vlan_cfg.vlan[i].tag = vlan[i].tag;
        }

        cs_db_entry_mod(TB_ZTE_VLAN, (void *) &vlan_cfg, sizeof(vlan_cfg),
                        column_bit(TB_ZTE_VLAN, "vlan_mode") |
                        column_bit(TB_ZTE_VLAN, "def_vlan") |
                        column_bit(TB_ZTE_VLAN, "vlan_num") |
                        column_bit(TB_ZTE_VLAN, "vlan"));
    }

#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_agg_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    ctc_oam_agg_vlan_t *vlan)
{
#if 0
    cs_sal_vlan_tag_t    def_vlan;
    app_vlan_tag_t app_dft_vlan;
    cs_sal_vlan_mode_t   vlan_mode = SAL_VLAN_MODE_AGGREGATION;
    cs_sal_vlan_cfg_t    sal_vlan[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint32            i = 0;
#ifdef ZTE_SPECIFIC
    db_zte_vlan_t vlan_cfg;
#endif/*END_ZTE_SPECIFIC*/

    if (vlan == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d, def tpid = 0x%x, def tag = 0x%x\r\n",
                  port, defvlan.tpid, defvlan.tag);
    OAM_ORG_DEBUG("SVLAN tpid = 0x%x, SVLAN tag =  0x%x\r\n",
                  vlan[0].agg_dst_vlan.tpid, vlan[0].agg_dst_vlan.tag);

    for (i = 0; i < vlan[0].agg_vlan_num ; ++i) {
        OAM_ORG_DEBUG("CVLAN tpid = 0x%x, CVLAN tag = 0x%x\r\n",
                      vlan[0].agg_vlan[i].tpid, vlan[0].agg_vlan[i].tag);
    }

    __PORT_CHECK(port);

    if (num > 1) {
        return OAM_E_NOT_SUPPORT;
    }

    def_vlan.vid = TAG2VID(defvlan.tag);
    def_vlan.pri = TAG2PRI(defvlan.tag);

    for (i = 0; i < vlan[0].agg_vlan_num; ++i) {
        sal_vlan[i].c_vlan.vid = vlan[0].agg_vlan[i].tag;
        sal_vlan[i].s_vlan.vid = vlan[0].agg_dst_vlan.tag;
    }


    if (cs_sal_onu_vlan_set(0, __SAL_ETH_PORT(port), def_vlan, vlan_mode, sal_vlan,
                             vlan[0].agg_vlan_num)) {
        return OAM_E_ERROR;
    }
    memcpy(&app_dft_vlan, &def_vlan, sizeof(app_vlan_tag_t));
    cs_sw_vlan_set(0, __SAL_ETH_PORT(port), app_dft_vlan,
                   (app_vlan_mode_t)vlan_mode, num, (app_vlan_cfg_t *)sal_vlan);
#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0) {
        memset(&vlan_cfg, 0x00, sizeof(vlan_cfg));
        vlan_cfg.port_id = port;
        vlan_cfg.vlan_mode = vlan_mode;
        vlan_cfg.def_vlan.tpid = def_vlan.tpid;
        vlan_cfg.def_vlan.tag = def_vlan.vid;

        memset(vlan_cfg.vlan, 0x00, sizeof(vlan_cfg.vlan));
        vlan_cfg.vlan_num = 1;
        vlan_cfg.agg_vlan_num = vlan[0].agg_vlan_num;

        vlan_cfg.vlan[0].tpid = vlan[0].agg_dst_vlan.tpid;
        vlan_cfg.vlan[0].tag = vlan[0].agg_dst_vlan.tag;

        for (i = 0 ; i < vlan[0].agg_vlan_num && i < CTC_ONU_AGGR_VLAN_NUM; i ++) {
            vlan_cfg.vlan[i + 1].tpid = vlan[0].agg_vlan[i].tpid;
            vlan_cfg.vlan[i + 1].tag = vlan[0].agg_vlan[i].tag;
        }

        cs_db_entry_mod(TB_ZTE_VLAN, (void *) &vlan_cfg, sizeof(vlan_cfg),
                        column_bit(TB_ZTE_VLAN, "ALL"));
    }

#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}


/**************** PLAT ADAPT START *********************************/

oam_status ctc_oam_onu_opmdiag_get_adapt(
    oam_int16 *temp,
    oam_uint16 *volt,
    oam_uint16 *tx_bias,
    oam_uint16 *tx_power,
    oam_uint16 *rx_power)
{
    cs_opm_status ret = CS_OPM_OK;
    cs_opm_diag_info_t ddm_raw_data;

    memset(&ddm_raw_data, 0, sizeof(ddm_raw_data));

    ret = cs_opm_ddm_raw_data_get(OAM_DEV_ID, OAM_PORT_ID, &ddm_raw_data);

    if (ret != CS_OPM_OK) {
        return  OAM_E_OK;
    }

    *temp     = ddm_raw_data.temp;
    *volt     = ddm_raw_data.volt;
    *tx_bias  = ddm_raw_data.tx_bias;
    *tx_power = ddm_raw_data.tx_pwr;
    *rx_power = ddm_raw_data.rx_pwr;

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
    oam_uint8 onu_mac[OAM_MACADDR_LEN] = {0x00, 0x13, 0x25, 0x00, 0x00, 0x01};
    cs_port_id_t port = (CS_PORT_TYPE_EPON << 8);

    if (NULL == onuId) {
        return OAM_E_PARAM;
    }
    else if ((transId != TRANSID_MAJOR) && (transId != TRANSID_BOTH)) {
        return OAM_E_NOT_SUPPORT;
    }
    
    scfg_read(0, CFG_ID_MAC_ADDRESS, OAM_MACADDR_LEN, onu_mac);
    
    if (((onuId[0] == onu_mac[0])
         && (onuId[1] == onu_mac[1])
         && (onuId[2] == onu_mac[2])
         && (onuId[3] == onu_mac[3])
         && (onuId[4] == onu_mac[4])
         && (onuId[5] == onu_mac[5]))
        ||
        ((0xff == onuId[0]) && (0xff == onuId[1]) && (0xff == onuId[2]) &&
         (0xff == onuId[3]) && (0xff == onuId[4]) && (0xff == onuId[5]))) {
        if (g_laser_tx_power_timer != 0) { /*stop previous timer */
            cs_timer_del(g_laser_tx_power_timer);
            g_laser_tx_power_timer = 0;
        }

        if (0 == action) { /* tx laser on */
            //cs_gpio_onu_tx_pwr_enable(0, port, TRUE);
            cs_epon_port_laser_mode_set(0, port, CS_EPON_PORT_LASER_MODE_BURST);
            cs_gpio_write(0, 6/*GPIO_PIN6*/, 1);
            led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_ON);
        }
        else if (65535 == action) {
            //cs_gpio_onu_tx_pwr_enable(0, port, FALSE);
            cs_epon_port_laser_mode_set(0, port, CS_EPON_PORT_LASER_MODE_OFF);
            cs_gpio_write(0, 6/*GPIO_PIN6*/, 0);
            led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_OFF);
        }
        else {
            //cs_gpio_onu_tx_pwr_enable(0, port, FALSE);
            cs_epon_port_laser_mode_set(0, port, CS_EPON_PORT_LASER_MODE_OFF);
            cs_gpio_write(0, 6/*GPIO_PIN6*/, 0);
            led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_OFF);
            g_laser_tx_power_timer = cs_timer_add(action * 1000, __laser_tx_power_on, NULL);
        }
    }
    else {
        return OAM_E_PARAM;
    }

    return OAM_E_OK;
}



oam_status ctc_oam_onu_reset_set_adapt()
{
    cs_status_e ret = CS_OK;

    ret = cs_reset(0, CS_RESET_CHIP);

    if (CS_OK != ret)
        return OAM_E_ERROR;

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
#if 0
    cs_ld_status ret = CS_LD_OK;
    oam_uint32 enable = 0;

    ret = cs_ld_enable_get(OAM_DEV_ID, port, &enable);

    if (ret != CS_LD_OK)
        return OAM_E_ERROR;

    *status = enable ? OAM_CTC_ONU_LOOPDETECT_ACTIVED :
              OAM_CTC_ONU_LOOPDETECT_DEACTIVED;
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_loop_detect_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status)
{
#if 0
    cs_ld_status ret = CS_LD_OK;
    oam_uint32 enable = 0;
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t   port_info;
#endif/*END_ZTE_SPECIFIC*/

    enable = (status == OAM_CTC_ONU_LOOPDETECT_ACTIVED) ? 1 : 0;

    ret = cs_ld_enable_set(OAM_DEV_ID, port, enable);

    if (ret != CS_LD_OK)
        return OAM_E_ERROR;

#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0) {
        memset(&port_info, 0x00, sizeof(port_info));
        port_info.port_id = port;
        port_info.loop_detect = status == 2 ? 1 : 0;
        cs_db_entry_mod(TB_ETH_PORT, (void *) &port_info, sizeof(port_info),
                        column_bit(TB_ETH_PORT, "loop_detect"));
    }

#endif/*END_ZTE_SPECIFIC*/
#endif
    return 0;
}


oam_status ctc_oam_disable_looped_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status)
{
#if 0
    cs_ld_status ret = CS_LD_OK;
    oam_uint32 block_mode = 0;

    block_mode = (status == OAM_CTC_ONU_DISABLE_LOOPED_ACTIVED) ? 1 : 0;

    ret = cs_ld_block_mode_set(OAM_DEV_ID, port, block_mode);

    if (ret != CS_LD_OK)
        return OAM_E_ERROR;
#endif
    return OAM_E_OK;
}


oam_status ctc_oam_onu_classification_get_adapt(
    oam_port_id_t port,
    oam_uint32 *num,
    oam_uint8 *data)
{
#if 0
    oam_ctc_onu_classification_struct *pData = (oam_ctc_onu_classification_struct *)
            data;
    oam_ctc_onu_class_fselect_t       *pEntry;
    oam_ctc_onu_class_fselect_v6_t    *pEntry6;

    cs_sal_classification_t   cls[CLASS_RULES_MAX];
    oam_uint8                  cls_num = 0;
    oam_uint32                 i = 0, j = 0, len = 0, total_len = 0;
    oam_uint8                  ctc_version = 0;

    if ((num == NULL) || (data == NULL)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    memset(cls, 0x00, CLASS_RULES_MAX * sizeof(cs_sal_classification_t));

    oam_ctc_version_get(oam_llid_get(), &ctc_version);

    if (cs_sal_port_classification_get(0, __SAL_ETH_PORT(port), &cls_num, cls)) {
        return OAM_E_ERROR;
    }

    *num = cls_num;

    for (i = 0; i < cls_num; i++) {
        pData = (oam_ctc_onu_classification_struct *) (data + total_len);
        len = 3;
        total_len += sizeof(oam_ctc_onu_classification_struct) - 1;
        pData->precedence = cls[i].precedence;
        pData->queueMapped = cls[i].queueMapped;
        pData->priMark = cls[i].priMark;
        pData->entries = cls[i].entries;

        for (j = 0; j < pData->entries; j++) {
            if ((cls[i].fselect[j].fieldSelect >= CLASS_RULES_FSELECT_IPVER) &&
                (ctc_version == 0x30)) {
                len += sizeof(oam_ctc_onu_class_fselect_v6_t);
                pEntry6 = (oam_ctc_onu_class_fselect_v6_t * ) (data + total_len);
                total_len += sizeof(oam_ctc_onu_class_fselect_v6_t);
                pEntry6->fieldSelect = cls[i].fselect[j].fieldSelect;
                pEntry6->validationOper = cls[i].fselect[j].validationOper;
                memcpy(pEntry6->matchValue, cls[i].fselect[j].matchValue, CLASS_MATCH_VAL_LEN);
            }
            else {
                len += sizeof(oam_ctc_onu_class_fselect_t);
                pEntry = (oam_ctc_onu_class_fselect_t * ) (data + total_len);
                total_len += sizeof(oam_ctc_onu_class_fselect_t);
                pEntry->fieldSelect = cls[i].fselect[j].fieldSelect;
                pEntry->validationOper = cls[i].fselect[j].validationOper;
                memcpy(pEntry->matchValue,
                       cls[i].fselect[j].matchValue + (CLASS_MATCH_VAL_LEN - CTC_CLASS_MATCH_VAL_LEN),
                       CTC_CLASS_MATCH_VAL_LEN);
            }
        }

        pData->len = len;

    }
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_classification_set_adapt(
    oam_uint8 action,
    oam_port_id_t port,
    oam_uint32 num,
    oam_uint8 *data)
{
#if 0
    cs_sal_classification_t  cls[CLASS_RULES_MAX];
    oam_uint8                 precedence[CLASS_RULES_MAX];

    oam_ctc_onu_classification_struct *pData;
    oam_ctc_onu_class_fselect_t *pEntry;
    oam_ctc_onu_class_fselect_v6_t *pEntry6;
    oam_uint32 i = 0, j = 0, len = 0, total_len = 0;
    oam_uint8 ctc_version = 0;
    oam_uint32 ii = 0;

    if (!data && (action != CTC_CLASS_RULES_ACTION_CLR)){
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d, action = %d, num = %d\r\n", port, action, num);

    __PORT_CHECK(port);

    memset(cls, 0x00, CLASS_RULES_MAX * sizeof(cs_sal_classification_t));

    oam_ctc_version_get(oam_llid_get(), &ctc_version);

    switch (action) {
        case CTC_CLASS_RULES_ACTION_CLR:
            if (cs_sal_port_classification_clr(0, __SAL_ETH_PORT(port))) {
                return OAM_E_ERROR;
            }

            break;

        case CTC_CLASS_RULES_ACTION_DEL:
            for (i = 0; i < num; i++) {
                pData = (oam_ctc_onu_classification_struct *) (data + total_len);
                total_len += (pData->len + 2);
                precedence[i] = pData->precedence;

                OAM_ORG_DEBUG("precedence = %d\r\n",  precedence[i]);
            }

            if (cs_sal_port_classification_del(0, __SAL_ETH_PORT(port), num, precedence)) {
                return OAM_E_ERROR;
            }

            break;

        case CTC_CLASS_RULES_ACTION_ADD:
            for (i = 0; i < num && i < CLASS_RULES_MAX; i ++) {
                pData = (oam_ctc_onu_classification_struct *)(data + total_len);
                total_len += (pData->len + 2);
                cls[i].precedence = pData->precedence;
                cls[i].queueMapped = pData->queueMapped;
                cls[i].priMark = pData->priMark;
                cls[i].entries = pData->entries;

                {
                    OAM_ORG_DEBUG("precedence  = %d\r\n",  cls[i].precedence);
                    OAM_ORG_DEBUG("queueMapped = %d\r\n",  cls[i].queueMapped);
                    OAM_ORG_DEBUG("priMark     = %d\r\n", cls[i].priMark);
                    OAM_ORG_DEBUG("entries     = %d\r\n", cls[i].entries);
                }

                len = 0;

                for (j = 0; j < pData->entries && j < CLASS_FIELD_SELECT_MAX; j++) {
                    pEntry = (oam_ctc_onu_class_fselect_t *)(pData->data + len);

                    if ((pEntry->fieldSelect >= CLASS_RULES_FSELECT_IPVER) &&
                        (ctc_version > 0x20)) {
                        pEntry6 = (oam_ctc_onu_class_fselect_v6_t *)(pData->data + len);
                        len += sizeof(oam_ctc_onu_class_fselect_v6_t);
                        cls[i].fselect[j].fieldSelect = pEntry6->fieldSelect;
                        cls[i].fselect[j].validationOper = pEntry6->validationOper;
                        memcpy(cls[i].fselect[j].matchValue, pEntry6->matchValue, CLASS_MATCH_VAL_LEN);

                        {
                            OAM_ORG_DEBUG("CLS[%d].fselect[%d].fieldselect    = %d\r\n", i, j,
                                          cls[i].fselect[j].fieldSelect);
                            OAM_ORG_DEBUG("CLS[%d].fselect[%d].validationOper = %d\r\n", i, j,
                                          cls[i].fselect[j].validationOper);

                            for (ii = 0; ii < CTC_CLASS_MATCH_VAL_LEN; ii++) {
                                OAM_ORG_DEBUG("CLS[%d].fselect[%d].matchValue    = %d\r\n", i, j,
                                              cls[i].fselect[j].matchValue + ii);
                            }
                        }

                    }
                    else {
                        pEntry = (oam_ctc_onu_class_fselect_t *)(pData->data + len);
                        len += sizeof(oam_ctc_onu_class_fselect_t);
                        cls[i].fselect[j].fieldSelect = pEntry->fieldSelect;
                        cls[i].fselect[j].validationOper = pEntry->validationOper;
                        memcpy(cls[i].fselect[j].matchValue + (CLASS_MATCH_VAL_LEN -
                                                               CTC_CLASS_MATCH_VAL_LEN),
                               pEntry->matchValue, CTC_CLASS_MATCH_VAL_LEN);
                    }

                    {
                        OAM_ORG_DEBUG("CLS[%d].fselect[%d].fieldselect    = %d\r\n", i, j,
                                      cls[i].fselect[j].fieldSelect);
                        OAM_ORG_DEBUG("CLS[%d].fselect[%d].validationOper = %d\r\n", i, j,
                                      cls[i].fselect[j].validationOper);

                        for (ii = 0; ii < CTC_CLASS_MATCH_VAL_LEN; ii++) {
                            OAM_ORG_DEBUG("CLS[%d].fselect[%d].matchValue     = %d\r\n", i, j,
                                          cls[i].fselect[j].matchValue + (CLASS_MATCH_VAL_LEN - CTC_CLASS_MATCH_VAL_LEN) +
                                          ii);
                        }
                    }
                }
            }

            if (cs_sal_port_classification_add(0, __SAL_ETH_PORT(port), num, cls)) {
                return OAM_E_ERROR;
            }

            break;
    }

#ifdef ZTE_SPECIFIC
    ctc_oam_cls_syn_db( port);
#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;

}



/******************************* MC ADAPT **************************/

oam_status ctc_oam_onu_mcvlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *num,
    oam_uint16 *vlanIds)
{
#if 0
    oam_uint8 mc_mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;
    oam_status   rt = OAM_E_OK;

    if ( (NULL == num) || (NULL == vlanIds) ) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n", port);

    __PORT_CHECK(port);

    rt = ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (rt) {
        return rt;
    }

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode) {
        return OAM_E_NOT_SUPPORT;
    }

    if (cs_sal_mc_vlan_get(0, __SAL_ETH_PORT(port), vlanIds, num)) {
        return OAM_E_ERROR;
    }
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mcvlan_set_adapt(
    oam_port_id_t port,
    oam_uint8 oper,
    oam_uint16 num,
    oam_uint16 *vlanIds)
{
#if 0
    oam_uint8 mc_mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;
    oam_uint32 i = 0;
    oam_status rt = OAM_E_OK;

    if (oper > CTC_MCVLAN_OPER_CLR) {
        return OAM_E_PARAM;
    }

    if (NULL == vlanIds ) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("\rport = %d, oper = %d, num = %d\r\n", port, oper, num);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("\rvlanid = %d\r\n", vlanIds[i]);
    }

    __PORT_CHECK(port);

    rt = ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (rt) {
        return rt;
    }

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode) {
        return OAM_E_NOT_SUPPORT;
    }

    if (CTC_MCVLAN_OPER_DEL == oper) {
        if (cs_sal_mc_vlan_del(0, __SAL_ETH_PORT(port), vlanIds, num)) {
            return OAM_E_ERROR;
        }
    }
    else if (CTC_MCVLAN_OPER_ADD == oper) {
        if (cs_sal_mc_vlan_add(0, __SAL_ETH_PORT(port), vlanIds, num)) {
            return OAM_E_ERROR;
        }
    }
    else if (CTC_MCVLAN_OPER_CLR == oper) {
        if (cs_sal_mc_vlan_clr(0, __SAL_ETH_PORT(port))) {
            return OAM_E_ERROR;
        }
    }
    else {
        return OAM_E_PARAM;
    }

#ifdef ZTE_SPECIFIC
    ctc_oam_mc_vlan_syn_db( port);
#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;
}


oam_status ctc_oam_onu_mctagstrip_get_adapt(
    oam_port_id_t port,
    oam_uint8 *tagStriped,
    oam_uint8 *num,
    oam_ctc_iptv_vid_t *vlans)
{
#if 0
    cs_sal_mc_vlan_act_t          vlan_act;
    cs_sal_vlan_cfg_t             swap_rule[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16                     rule_nums;
    int i;

    if ( (NULL == tagStriped) || (NULL == num) || (NULL == vlans) ) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\r\n", port);

    __PORT_CHECK(port);

    if (cs_sal_mc_vlan_action_get(0, __SAL_ETH_PORT(port), &vlan_act, swap_rule,
                                  &rule_nums)) {
        return OAM_E_ERROR;
    }

    if (SAL_MC_VLAN_TAG_SWAP == vlan_act) {
        for (i = 0; i < rule_nums; ++i) {
            vlans[i].vid = swap_rule[i].s_vlan.vid;
            vlans[i].iptv_vid = swap_rule[i].c_vlan.vid;
        }

        *num = rule_nums;
    }

    *tagStriped = (oam_uint8)vlan_act;
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mctagstrip_set_adapt(
    oam_port_id_t port,
    oam_uint8  tagStriped,
    oam_uint8  num,
    oam_ctc_iptv_vid_t *vlans)
{
#if 0
    cs_sal_mc_vlan_act_t          vlan_act;
    cs_sal_vlan_cfg_t             swap_rule[CTC_ONU_AGGR_VLAN_NUM];
    int i;
#ifdef ZTE_SPECIFIC
    db_zte_mc_tagstrip_t        db_mc_tagstrip;
#endif/*END_ZTE_SPECIFIC*/

    if (vlans == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d, tagstriped = %d, num = %d\r\n", port, tagStriped, num);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("vlanid = %d, iptv_vid = %d\r\n", vlans[i].vid, vlans[i].iptv_vid);
    }

    __PORT_CHECK(port);

    if ( (CTC_MCVLAN_TAG_OPER_SWAP == tagStriped) && (NULL == vlans) ) {
        return OAM_E_PARAM;
    }

    if (CTC_MCVLAN_TAG_OPER_SWAP == tagStriped) {
        for (i = 0; i < num; ++i) {
            swap_rule[i].s_vlan.vid = vlans[i].vid;
            swap_rule[i].c_vlan.vid = vlans[i].iptv_vid;
        }
    }

    vlan_act = (cs_sal_mc_vlan_act_t)tagStriped;

    if (cs_sal_mc_vlan_action_set(0, __SAL_ETH_PORT(port), vlan_act, swap_rule,
                                  num)) {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC
    memset(&db_mc_tagstrip, 0x00, sizeof(db_mc_tagstrip));
    db_mc_tagstrip.port_id = port;
    db_mc_tagstrip.tagStriped = tagStriped;
    db_mc_tagstrip.num = num;
    memcpy (db_mc_tagstrip.vlan, vlans, sizeof(oam_ctc_iptv_vid_t)*num);
    cs_db_entry_mod(TB_ZTE_MC_TAGSTRIP , (void *) &db_mc_tagstrip,
                    sizeof(db_mc_tagstrip),
                    column_bit(TB_ZTE_MC_TAGSTRIP, "ALL"));
#endif/*END_ZTE_SPECIFIC*/
#endif
    return OAM_E_OK;

}


oam_status ctc_oam_onu_mc_switch_get_adapt(
    oam_uint8 *mode)
{
#if 0
    mc_mode_t mc_mode;
    mc_mode_get(0, &mc_mode);

    switch (mc_mode) {
        case MC_SNOOPING:
            *mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;
            break;

        case MC_MANUAL:
            *mode = CTC_MC_SWITCH_START_CTC_IGMP;
            break;
            
        case MC_DISABLE:
        default:
            *mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;
#ifdef ZTE_SPECIFIC
            *mode = CTC_MC_SWITCH_TRANSPARENT_ZTE_EXT;
#endif/*END_ZTE_SPECIFIC*/
            break;
    }
#endif

    *mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;

    return OAM_E_OK;

}

oam_status ctc_oam_onu_mc_switch_set_adapt(
    oam_uint8 mode)
{
#if 0
    cs_status   ret = CS_E_OK;
    mc_mode_t mc_mode;
    mc_mode_t temp_mc_mode;
#ifdef ZTE_SPECIFIC
    db_zte_onu_svc_t   svc;
#endif/*END_ZTE_SPECIFIC*/

    switch ( mode ) {
        case    CTC_MC_SWITCH_START_IMGP_SNOOPING:
            temp_mc_mode = MC_SNOOPING;
            break;

        case    CTC_MC_SWITCH_START_CTC_IGMP:
            temp_mc_mode = MC_MANUAL;
            break;
#ifdef ZTE_SPECIFIC

        case    CTC_MC_SWITCH_TRANSPARENT_ZTE_EXT:
            temp_mc_mode = MC_DISABLE;
            break;
#endif/*END_ZTE_SPECIFIC*/

        default:
            return  CS_E_PARAM;

    }

    mc_mode_get(0, &mc_mode);

    if ( temp_mc_mode == mc_mode) {
        return CS_E_OK;
    }

    ret = cs_igmp_ctcctrl_switch_set(temp_mc_mode);

#ifdef ZTE_SPECIFIC

    if (ret == CS_E_OK ) {
        memset(&svc, 0x00, sizeof(svc));
        svc.mc_mode = temp_mc_mode;
        cs_db_entry_mod(TB_ZTE_ONU_SVC, (void *) &svc, sizeof(svc),
                        column_bit(TB_ZTE_ONU_SVC, "mc_mode"));
    }

#endif/*END_ZTE_SPECIFIC*/
#endif
    return 0;
}

oam_status ctc_oam_onu_mc_control_type_get_adapt(
    oam_uint8 *type)
{
    //*type = mc_get_mc_fwd_mode();
    return 0;
}

oam_status ctc_oam_onu_mc_control_type_set_adapt(
    oam_uint8  type)
{
    //return mc_set_mc_fwd_mode(type);
    // cs_igmp_ctcctrl_type_set(type);
    return 0;
}

oam_status ctc_oam_onu_mc_control_vlan_mac_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_vlan_mac_t *vlanMac)
{
    //mc_ctc_ctrl_get_entries((oam_uint8 *)vlanMac, num);
    //cs_igmp_ctcctrl_vlan_mac_get(num, vlanMac);
    return CS_E_OK;

}

oam_status ctc_oam_onu_mc_control_mac_ip_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_mac_ip_t *macIp)
{
    //mc_ctc_ctrl_get_entries((oam_uint8 *)macIp, num);
    //cs_igmp_ctcctrl_mac_ip_get(num, macIp);
    return CS_E_OK;

}

oam_status ctc_oam_onu_mc_control_vlan_ip_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIp)
{
    //mc_ctc_ctrl_get_entries((oam_uint8 *)vlanIp, num);
    //cs_igmp_ctcctrl_vlan_ip_get(num, vlanIp);
    return CS_E_OK;

}

oam_status ctc_oam_onu_mc_control_clr_adapt()
{
    //mc_ctc_ctrl_clr_mfdb();
    //cs_igmp_ctcctrl_clr();
    return CS_E_OK;
}

oam_status ctc_oam_onu_mc_control_vlan_mac_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_vlan_mac_t *vlanMac)
{
    if (!num || !vlanMac)
        return CS_E_OK;

    //cs_igmp_ctcctrl_vlan_mac_set(action, num, vlanMac);
    return CS_E_OK;
}

oam_status ctc_oam_onu_mc_control_mac_ip_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_mac_ip_t *macIp)
{
    if (!num || !macIp)
        return CS_E_OK;

    //cs_igmp_ctcctrl_mac_ip_set(action, num, macIp);
    return CS_E_OK;
}

oam_status ctc_oam_onu_mc_control_vlan_ip_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIp)
{
    if (!num || !vlanIp)
        return CS_E_OK;

    //cs_igmp_ctcctrl_vlan_ip_set(action, num, vlanIp);

    return CS_E_OK;
}

oam_status ctc_oam_onu_grp_nmb_max_get_adapt(
    oam_port_id_t port,
    oam_uint8   *maxGrp)
{
    cs_status ret = 0;
    oam_uint8 mc_mode;
    cs_port_id_t port_id = 0;
    
    ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode) {
        return CS_E_RESOURCE;
    }
    
    if(port == 1)
        port_id = AAL_PORT_UNI0;
    else
        port_id = AAL_PORT_UNI1;
    
    ret = cs_kh_mcast_get_max_group_number(0,  port_id, (oam_uint32 *)maxGrp);
    
    return ret;
}

oam_status ctc_oam_onu_grp_nmb_max_set_adapt(
    oam_port_id_t port,
    oam_uint8   maxGrp)
{
    oam_uint8 mc_mode;
    cs_status ret = 0;
    cs_port_id_t port_id = 0;
#ifdef ZTE_SPECIFIC
    db_zte_mc_grp_t     db_mc_grp;
#endif/*END_ZTE_SPECIFIC*/

    ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode) {
        return CS_E_NOT_SUPPORT;
    }

    if(port == 1)
        port_id = AAL_PORT_UNI0;
    else
        port_id = AAL_PORT_UNI1;
    
    ret = cs_kh_mcast_set_max_group_number(0,  port_id, maxGrp);

#ifdef ZTE_SPECIFIC

    if (ret == CS_E_OK) {
        memset(&db_mc_grp, 0x00, sizeof(db_mc_grp));
        db_mc_grp.port_id = port;
        db_mc_grp.mc_grp = maxGrp;
        cs_db_entry_mod(TB_ZTE_MAX_GRP  , (void *) &db_mc_grp, sizeof(db_zte_mc_grp_t),
                        column_bit(TB_ZTE_MAX_GRP, "mc_grp"));
    }

#endif/*END_ZTE_SPECIFIC*/

    return ret;
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
        *port = OAM_PON_PORT_ID;
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
    oam_port_id_t port = OAM_PON_PORT_ID;
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
    oam_port_id_t port = OAM_PON_PORT_ID;
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
    oam_port_id_t port = OAM_PON_PORT_ID;
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
    oam_port_id_t port = OAM_PON_PORT_ID;
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
    ctc_oam_mgmt_obj_idx_t mgmt_idx,
    ctc_oam_alarm_class_e *type,
    oam_port_id_t *port)
{
    if (!mgmt_idx.valid) {
        *type = CTC_ONU_ALARM;
    }
    else {
        if (mgmt_idx.idxLeaf == OAM_CTC_LEAF_CODE_ONU_PON) {
            *type = CTC_PON_ALARM;
            *port = OAM_PON_PORT_ID;
        }
        else if (mgmt_idx.idxLeaf == OAM_CTC_LEAF_CODE_ONU_PORT) {
            if (mgmt_idx.idxBranch == OAM_CTC_INDEX_BRANCH && mgmt_idx.idxValue == 0) {
                *type = CTC_PON_ALARM;
                *port = OAM_PON_PORT_ID;
            }
            else {
                *type = CTC_PORT_ALARM;
                *port = mgmt_idx.idxValue;
            }
        }
        else
            return OAM_E_PARAM;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_get_opm_alarm_type(oam_uint16 alarm_id,
                                      oam_uint32 *alarm_type)
{
    switch (alarm_id) {
            /* alarm rx power */
        case CTC_ALARM_RX_POW_HIGH:
            *alarm_type = OPM_ALARM_RX_PWR_HIGH_ALARM;
            break;

        case CTC_ALARM_RX_POW_LOW:
            *alarm_type = OPM_ALARM_RX_PWR_LOW_ALARM;
            break;

            /* alarm tx power */
        case CTC_ALARM_TX_POW_HIGH:
            *alarm_type = OPM_ALARM_TX_PWR_HIGH_ALARM;
            break;

        case CTC_ALARM_TX_POW_LOW:
            *alarm_type = OPM_ALARM_TX_PWR_LOW_ALARM;
            break;

            /* alarm tx bias */
        case CTC_ALARM_TX_BIAS_HIGH:
            *alarm_type = OPM_ALARM_TX_BIAS_HIGH_ALARM;
            break;

        case CTC_ALARM_TX_BIAS_LOW:
            *alarm_type = OPM_ALARM_TX_BIAS_LOW_ALARM;
            break;

            /* alarm vcc */
        case CTC_ALARM_VCC_HIGH:
            *alarm_type = OPM_ALARM_VOLT_HIGH_ALARM;
            break;

        case CTC_ALARM_VCC_LOW:
            *alarm_type = OPM_ALARM_VOLT_LOW_ALARM;
            break;

            /* alarm temperature */
        case CTC_ALARM_PON_TEMP_HIGH:
            *alarm_type = OPM_ALARM_TEMP_HIGH_ALARM;
            break;

        case CTC_ALARM_PON_TEMP_LOW:
            *alarm_type = OPM_ALARM_TEMP_LOW_ALARM;
            break;

            /* warning rx power */
        case CTC_WARN_RX_POW_HIGH:
            *alarm_type = OPM_ALARM_RX_PWR_HIGH_WARNING;
            break;

        case CTC_WARN_RX_POW_LOW:
            *alarm_type = OPM_ALARM_RX_PWR_LOW_WARNING;
            break;

            /* warning tx power */
        case CTC_WARN_TX_POW_HIGH:
            *alarm_type = OPM_ALARM_TX_PWR_HIGH_WARNING;
            break;

        case CTC_WARN_TX_POW_LOW:
            *alarm_type = OPM_ALARM_TX_PWR_LOW_WARNING;
            break;

            /* warning tx bias */
        case CTC_WARN_TX_BIAS_HIGH:
            *alarm_type = OPM_ALARM_TX_BIAS_HIGH_WARNING;
            break;

        case CTC_WARN_TX_BIAS_LOW:
            *alarm_type = OPM_ALARM_TX_BIAS_LOW_WARNING;
            break;

            /* warning vcc */
        case CTC_WARN_VCC_HIGH:
            *alarm_type = OPM_ALARM_VOLT_HIGH_WARNING;
            break;

        case CTC_WARN_VCC_LOW:
            *alarm_type = OPM_ALARM_VOLT_LOW_WARNING;
            break;

            /* warning temperature */
        case CTC_WARN_PON_TEMP_HIGH:
            *alarm_type = OPM_ALARM_TEMP_HIGH_WARNING;
            break;

        case CTC_WARN_PON_TEMP_LOW:
            *alarm_type = OPM_ALARM_TEMP_LOW_WARNING;
            break;

        default:
            return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_onu_alarm_get_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32 *alarm_config)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = OAM_PON_PORT_ID;
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

    if (ret != OAM_E_OK)
        return ret;

#ifdef ZTE_SPECIFIC

    if ( type != CTC_PORT_ALARM ) {
        ctc_oam_onu_alarm_syn_db( );
    }
    else {

        ctc_oam_port_alarm_syn_db(port);
    }

#endif/*END_ZTE_SPECIFIC*/


    return ret;
}


oam_status ctc_oam_onu_alarm_threshold_get_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  *raise_val,
    oam_uint32  *clear_val)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = 0;
    oam_status ret = OAM_E_OK;

    ret = ctc_oam_onu_alarm_class_port_get(index, &type, &port);

    if (ret != OAM_E_OK)
        return ret;

    return ctc_oam_get_alarm_threshold(type, port, alarm_id,
                                       raise_val, clear_val);
}

oam_status ctc_oam_onu_alarm_threshold_set_adapt(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint16 alarm_id,
    oam_uint32  raise_val,
    oam_uint32  clear_val)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = 0;
    oam_status ret = OAM_E_OK;

    ret = ctc_oam_onu_alarm_class_port_get(index, &type, &port);

    if (ret != OAM_E_OK)
        return ret;

    ret = ctc_oam_set_alarm_threshold(type, port, alarm_id,
                                      raise_val, clear_val);

    if (ret != OAM_E_OK)
        return ret;

#ifdef ZTE_SPECIFIC

    if ( type != CTC_PORT_ALARM ) {
        ctc_oam_onu_alarm_syn_db( );
    }
    else {
        ctc_oam_port_alarm_syn_db( port);
    }

#endif/*END_ZTE_SPECIFIC*/

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


oam_status ctc_oam_loid_passwd_get_adapt(oam_uint8 *loid, oam_uint8 *passwd)
{
    cs_db_ctc_loid_pwd_t ctc_loid;

    OAM_ASSERT_RET((loid != NULL
                    && passwd != NULL),
                    OAM_E_PARAM);

    cs_db_entry_get(TB_CTC_LOID, (void *) &ctc_loid, sizeof(ctc_loid));
    memcpy(loid, ctc_loid.loid, strlen((cs_int8*)ctc_loid.loid));
    memcpy(passwd, ctc_loid.passwd, strlen((cs_int8*)ctc_loid.passwd));

    return OAM_E_OK;
}


oam_status ctc_oam_auth_keep_silence_set_adapt(oam_uint8 llid_index,
        oam_boolean enable)
{
    /*if(cs_epon_mpcp_registration_set(0, 0, 0, !enable)){
        return OAM_E_ERROR;
    }*/
    
    if(cs_epon_mpcp_keep_silence_set(0, 0, enable, 60000)){
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_laser_alarm_status_get(oam_uint8 *status)
{
#if 0
    cs_sdl_pon_laser_mode_t mode;

    cs_sal_epon_onu_pon_laser_mode_get(0, -1, &mode);

    if (mode == SDL_PON_LASER_MODE_ON) {
        *status = 1;
    }
    else {
        *status = 0;
    }
#endif

    return OAM_E_OK;
}

oam_status ctc_oam_up_congest_status_get(oam_port_id_t port, oam_boolean *status)
{
    OAM_ASSERT_RET(status != NULL, OAM_E_PARAM);
    
    //cs_cosq_us_congest_get(0, __SAL_ETH_PORT(port), status, 0);
    
    return OAM_E_OK;
}

oam_status ctc_oam_chip_id_get_adapt(oam_ctc_onu_chip_id_t *chip_id)
{
    cs_uint32 a, b, c;

    sscanf(ONU_CHIPSET_VENDOR_ID,"%2x%2x",&a,&b);
    chip_id->venderId[0] = a;
    chip_id->venderId[1] = b;

    chip_id->chipModel[0] = 0x82;
    chip_id->chipModel[1] = 0x32;

    sscanf(ONU_CHIPSET_VER,"%2x",&a);
    chip_id->Revision = a;
    
    sscanf(ONU_CHIPSET_MADE_DATE,"%2d",&a);
    sscanf(ONU_CHIPSET_MADE_DATE,"%*[^/]/%2d",&b);
    sscanf(ONU_CHIPSET_MADE_DATE,"%*[^/]/%*[^/]/%2d",&c);
    chip_id->Date[0] = a;
    chip_id->Date[1] = b;
    chip_id->Date[2] = c;

    return OAM_E_OK;
}

oam_status ctc_oam_sn_get_adapt(oam_ctc_onu_sn_t *sn)
{
   /* oam_uint8 mac [CS_MACADDR_LEN];*/
    oam_uint8 vendorId[OAM_CTC_VENDER_ID_LEN + 1] = {0};
    oam_uint8 model_name[OAM_CTC_MODEL_ID_LEN + 1] = {0x46, 0x34, 0x30, 0x32};
    oam_uint8 hwver[OAM_CTC_ONU_HW_LEN + 1] = {0};
    oam_uint8 swVer[OAM_CTC_ONU_SW_LEN + 1] = {0};
    tSwImgCfg    img_info;
    oam_uint8 onu_mac[OAM_MACADDR_LEN] = {0};

    memset(swVer, 0x00, OAM_CTC_ONU_SW_LEN);
    memset(&img_info, 0x00, sizeof(tSwImgCfg));
    cs_swdl_getswimginfo(CS_SWIMG_ID_0, &img_info);

    if (!img_info.isActiveB) {
        memset(&img_info, 0x00, sizeof(tSwImgCfg));
        cs_swdl_getswimginfo(CS_SWIMG_ID_1, &img_info);
    }

    memcpy(swVer, img_info.epon_version, OAM_CTC_ONU_SW_LEN);

    sn->hdr.width = sizeof(oam_ctc_onu_sn_t) - sizeof(oam_var_cont_t);

    /*startup_cfg_read(CS_SCFG_ID_83, OAM_CTC_VENDER_ID_LEN, vendorId);
    startup_cfg_read(CFG_ID_ONU_MOD_ID, OAM_CTC_MODEL_ID_LEN, model_name);*/
    strncpy((oam_int8 *)vendorId, ONU_VENDOR_ID, OAM_CTC_VENDER_ID_LEN);

    onu_hw_version_get(hwver);
    scfg_read(0, CFG_ID_MAC_ADDRESS, OAM_MACADDR_LEN, onu_mac);

    memcpy(sn->venderId + OAM_CTC_VENDER_ID_LEN - strlen((char *)vendorId),
           vendorId, strlen((char *)vendorId));
    memcpy(sn->model + OAM_CTC_MODEL_ID_LEN - strlen((char *)model_name),
           model_name, strlen((char *)model_name));
    memcpy(sn->onuId, onu_mac, CS_MACADDR_LEN);

    memcpy(sn->hwVer + OAM_CTC_ONU_HW_LEN - strlen((char *)hwver), hwver,
           strlen((char *)hwver));

    memcpy(sn->swVer + OAM_CTC_ONU_SW_LEN - strlen((char *)swVer), swVer,
           strlen((char *)swVer));

    return OAM_E_OK;
}

oam_status ctc_oam_fw_get_adapt(oam_uint8 *fw_ver, oam_uint8 *len)
{
    tSwImgCfg    img_info;
    cs_uint32 a;

    memset(&img_info, 0x00, sizeof(tSwImgCfg));
    cs_swdl_getswimginfo(CS_SWIMG_ID_0, &img_info);

    if (!img_info.isActiveB) {
        memset(&img_info, 0x00, sizeof(tSwImgCfg));
        cs_swdl_getswimginfo(CS_SWIMG_ID_1, &img_info);
    }

    if (oam_is_cuc_oam()) {
        sprintf((char *)fw_ver, "2.1.0.8");
        *len = strlen((char *)fw_ver);
    }
    else {
        sscanf(&img_info.cs_version[0],"%d",&a);
        fw_ver[0] = a;
        sscanf(&img_info.cs_version[2],"%d",&a);
        fw_ver[1] = a;
        sscanf(&img_info.cs_version[4],"%d",&a);
        fw_ver[2] = a;
        sscanf(&img_info.cs_version[6],"%d",&a);
        fw_ver[3] = a;
        *len = sizeof(oam_uint8) * 4;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_onucap2_get_adapt(oam_ctc_onu_capabilities2_t *onu_cap2)
{
    oam_uint8 numOfPon = 1;
    oam_uint8 protectionType = 0;
    oam_uint8 hasBattery = 1; //cs_gpio_ups_valid();

    onu_cap2->onuType = 1; /* HGU */
    onu_cap2->multillid = 1; /* only support single llid */
    onu_cap2->protectionType = protectionType;
    onu_cap2->numPONIf = numOfPon;
    onu_cap2->numSlot = 0;
    onu_cap2->numInterface = 2;
    onu_cap2->intf[0].interfaceType = 0; /* GE */
    onu_cap2->intf[0].numPorts = 1;
    onu_cap2->intf[1].interfaceType = 1; /* FE */
    onu_cap2->intf[1].numPorts = 1;
    
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

oam_status ctc_oam_keep_silence_time_get_adapt(oam_uint8 *keep)
{
    *keep = 60;

    return OAM_E_OK;
}

oam_status ctc_oam_enc_key_get_adapt(oam_uint8 llid_index, oam_uint8 key_index,
                                     oam_uint8 *key, oam_uint8 *key_len)
{
    cs_status_e ret;
    cs_epon_port_encryption_key_t  enc_key;
    
    memset(&enc_key, 0x00, sizeof(enc_key));
    
    ret = cs_epon_port_encryption_key_get(0, (CS_PORT_TYPE_EPON << 8), llid_index, key_index, &enc_key);
    
    *key_len = CHURNING_KEY_LEN;
    memcpy(key, enc_key.data, CHURNING_KEY_LEN);

    OAM_ORG_DEBUG("key = %s\r\n",  key);
    
    if (ret)
        return OAM_E_PARAM;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_mpcp_queue_thresh_get_adapt(
    oam_uint8 queue, oam_uint16 *threshold)
{
#if 0
    oam_uint16 max_threshold;
    cs_status_e ret;

    ret = cs_sal_epon_onu_mpcp_queue_threshold_get(0, -1, queue, threshold,
            &max_threshold);

    if (ret)
        return OAM_E_PARAM;
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mpcp_queue_thresh_set_adapt(
    oam_uint8 index, oam_uint16 threshold)
{
#if 0
    cs_status_e ret;

    ret = cs_sal_epon_onu_mpcp_queue_threshold_set(0, -1, index, threshold, 0);

    if (ret)
        return OAM_E_PARAM;
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_pon_stat_get_adapt(oam_ctc_onu_port_stats_t *stats)
{
    stats_pon_info_t pon_stas;
    cs_status ret = CS_E_OK;

    memset(&pon_stas, 0, sizeof(pon_stas));
    ret = cs_stats_pon_get(&pon_stas);

    if (ret != CS_E_OK) {
        return ret;
    }

    stats->dn_drop_events            = 0;
    stats->up_drop_events            = 0;
    stats->dn_octets                 = pon_stas.pon_byte_cnt;
    stats->up_octets                 = pon_stas.pon_tx_byte_cnt;
    stats->dn_frames                 = pon_stas.pon_frame_cnt +
                                       pon_stas.pon_mcframe_cnt +
                                       pon_stas.pon_bcframe_cnt;
    stats->up_frames                 = pon_stas.pon_txframe_cnt +
                                       pon_stas.pon_txmcframe_cnt +
                                       pon_stas.pon_txbcframe_cnt;
    stats->dn_bc_frames              = pon_stas.pon_bcframe_cnt;
    stats->up_bc_frames              = pon_stas.pon_txbcframe_cnt;
    stats->dn_mc_frames              = pon_stas.pon_mcframe_cnt;
    stats->up_mc_frames              = pon_stas.pon_txmcframe_cnt;
    stats->dn_crc_err_frames         = pon_stas.pon_crc8_err;
    stats->up_crc_err_frames         = pon_stas.pon_tx_crc_err;

    stats->dn_undersize_frames       = pon_stas.pon_undersizecnt;
    stats->up_undersize_frames       = 0;
    stats->dn_oversize_frames        = pon_stas.pon_oversize_cnt;
    stats->up_oversize_frames        = pon_stas.pon_txoversizecnt;
    stats->dn_fragments                = 0;
    stats->up_fragments                = 0;
    stats->dn_jabbers                    = pon_stas.pon_jabbercnt;
    stats->up_jabbers                    = 0;
    /*stats->dn_collisions                  = 0; */
    stats->dn_64_octets_frames       = pon_stas.pon_rmon64_cnt;
    stats->dn_65_127_octets_frames   = pon_stas.pon_rmon65_127_cnt;
    stats->dn_128_255_octets_frames  = pon_stas.pon_rmon128_255_cnt;
    stats->dn_256_511_octets_frames  = pon_stas.pon_rmon256_511_cnt;
    stats->dn_512_1023_octets_frames = pon_stas.pon_rmon512_1023_cnt;
    stats->dn_1024_1518_octets_frames = pon_stas.pon_rmon1024_1518_cnt;
    stats->up_64_octets_frames          = pon_stas.pon_txrmon64_cnt;
    stats->up_65_127_octets_frames   = pon_stas.pon_txrmon65_127_cnt;
    stats->up_128_255_octets_frames  = pon_stas.pon_txrmon128_255_cnt;
    stats->up_256_511_octets_frames  = pon_stas.pon_txrmon256_511_cnt;
    stats->up_512_1023_octets_frames = pon_stas.pon_txrmon512_1023_cnt;
    stats->up_1024_1518_octets_frames = pon_stas.pon_txrmon1024_1518_cnt;
    stats->dn_discard_frames         = 0;
    stats->up_discard_frames         = 0;
    stats->dn_err_frames             = pon_stas.pon_crc8_err +
                                       pon_stas.pon_undersizecnt +
                                       pon_stas.pon_oversize_cnt + pon_stas.pon_jabbercnt;

    stats->up_err_frames = pon_stas.pon_txoversizecnt ;
    stats->status_change_times       = 0;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_uni_stat_get_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    stats_uni_info_t uni_stas;
    oam_uint32 change_count = 0;
    cs_status ret = CS_E_OK;

    memset(&uni_stas, 0, sizeof(uni_stas));
    ret = cs_stats_uni_get(port, &uni_stas);

    if (ret != CS_E_OK) {
        return ret;
    }

    cs_stats_uni_status_chng_get(port, &change_count);

    stats->dn_drop_events            = 0;
    stats->up_drop_events            = 0;
    stats->dn_octets                 = uni_stas.txbyte_cnt;
    stats->up_octets                 = uni_stas.rxbyte_cnt;
    stats->dn_frames                 = uni_stas.txucfrm_cnt + uni_stas.txmcfrm_cnt +
                                       uni_stas.txbcfrm_cnt;
    stats->up_frames                 = uni_stas.rxucfrm_cnt + uni_stas.rxmcfrm_cnt +
                                       uni_stas.rxbcfrm_cnt;
    stats->dn_bc_frames              = uni_stas.txbcfrm_cnt;
    stats->up_bc_frames              = uni_stas.rxbcfrm_cnt ;


    stats->dn_mc_frames              = uni_stas.txmcfrm_cnt;
    stats->up_mc_frames              = uni_stas.rxmcfrm_cnt;
    stats->dn_crc_err_frames         = uni_stas.txcrcerrfrm_cnt;
    stats->up_crc_err_frames         = uni_stas.rxcrcerrfrm_cnt;

    stats->dn_undersize_frames       = 0;
    stats->up_undersize_frames       = uni_stas.rxundersizefrm_cnt;
    stats->dn_oversize_frames        = uni_stas.txoversizefrm_cnt;
    stats->up_oversize_frames        = uni_stas.rxoversizefrm_cnt;
    stats->dn_fragments              = 0;
    stats->up_fragments              = 0;
    stats->dn_jabbers                = 0;
    stats->up_jabbers                = uni_stas.rxjabberfrm_cnt;
    /*stats->dn_collisions             = uni_stas.single_collision + uni_stas.multiple_collision +
                                               uni_stas.late_collisions + uni_stas.excessive_collisions; */
    stats->dn_64_octets_frames       = uni_stas.txstatsfrm64_cnt;
    stats->dn_65_127_octets_frames   = uni_stas.txstatsfrm65_127_cnt;
    stats->dn_128_255_octets_frames  = uni_stas.txstatsfrm128_255_cnt;
    stats->dn_256_511_octets_frames  = uni_stas.txstatsfrm256_511_cnt;
    stats->dn_512_1023_octets_frames = uni_stas.txstatsfrm512_1023_cnt;
    stats->dn_1024_1518_octets_frames = uni_stas.txstatsfrm1024_1518_cnt;
    stats->up_64_octets_frames       = uni_stas.rxstatsfrm64_cnt;
    stats->up_65_127_octets_frames   = uni_stas.rxstatsfrm65_127_cnt;
    stats->up_128_255_octets_frames  = uni_stas.rxstatsfrm128_255_cnt;
    stats->up_256_511_octets_frames  = uni_stas.rxstatsfrm256_511_cnt;
    stats->up_512_1023_octets_frames = uni_stas.rxstatsfrm512_1023_cnt;
    stats->up_1024_1518_octets_frames = uni_stas.rxstatsfrm1024_1518_cnt;
    stats->dn_discard_frames         = 0;
    stats->up_discard_frames         = 0;
    stats->dn_err_frames             = uni_stas.txcrcerrfrm_cnt +
                                       uni_stas.txoversizefrm_cnt ;

    stats->up_err_frames            = uni_stas.rxcrcerrfrm_cnt +
                                      uni_stas.rxundersizefrm_cnt +
                                      uni_stas.rxoversizefrm_cnt ;
    stats->status_change_times    = change_count;

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
    tSwImgCfg img_info ;

    memset(&img_info, 0x00, sizeof(tSwImgCfg));

    if (cs_swdl_getswimginfo(0, &img_info)) {
        OAM_ORG_LOG("get image info failed \n");
        return 0;
    }

    if (img_info.isValidB && img_info.isActiveB) {
        return 1; /*return the backup image id*/
    }

    return 0;
}

oam_uint8 ctc_oam_swimage_flash_state_get_adapt()
{
    tSwImgCfg img_info ;
    tSwImgCfg img_info1 ;

    memset(&img_info, 0x00, sizeof(tSwImgCfg));
    memset(&img_info1, 0x00, sizeof(tSwImgCfg));

    if (cs_swdl_getswimginfo(0, &img_info)) {
        OAM_ORG_LOG("get image info failed \n");
        return OAM_CTC_FLASH_IMG0_NA_NC;
    }

    if (cs_swdl_getswimginfo(1, &img_info1)) {
        OAM_ORG_LOG("get image info failed \n");
        return OAM_CTC_FLASH_IMG0_A_C;
    }

    if (!img_info.isValidB) {
        OAM_ORG_LOG("image 0 is invalid \n");
        return OAM_CTC_FLASH_IMG0_NA_NC;
    }

    if (!img_info1.isValidB) {
        OAM_ORG_LOG("image 1 is invalid \n");
        return OAM_CTC_FLASH_IMG0_A_C;
    }

    if (img_info.isCommittedB && img_info.isActiveB) {
        OAM_ORG_LOG("image 0 is active and commit \n");
        return OAM_CTC_FLASH_IMG0_A_C;
    }

    if (img_info1.isCommittedB && img_info1.isActiveB) {
        OAM_ORG_LOG("image 1 is active and commit \n");
        return OAM_CTC_FLASH_IMG0_NA_NC;
    }

    if (img_info.isActiveB && img_info1.isCommittedB) {
        OAM_ORG_LOG("image 0 is active and image 1 is commit \n");
        return OAM_CTC_FLASH_IMG0_A_NC;
    }

    if (img_info1.isActiveB && img_info.isCommittedB) {
        OAM_ORG_LOG("image 1 is active and image 0 is commit \n");
        return OAM_CTC_FLASH_IMG0_NA_C;
    }

    OAM_ORG_LOG("Flash get wrong \n");
    /* should not get here */
    return OAM_CTC_FLASH_IMG0_A_NC;
}


oam_status ctc_oam_swimage_open_adapt(oam_uint8 img_id)
{

    if (0 != cs_swdl_startdownload(img_id)) {
        OAM_ORG_LOG("image start failed \n");
        return OAM_E_ERROR;
    }

    /* LED bink */
    led_send_message(0, LED_MSG_UPDATE, LED_EVENT_UPGRADING);
    return OAM_E_OK;
}

oam_status ctc_oam_swimage_write_adapt(
    oam_uint8 img_id,
    oam_uint8 *buf,
    oam_uint16 len)
{

    if (0 != cs_swdl_writeimageblock(
            img_id, (char *)buf, len)) {
        OAM_ORG_LOG("write failed \n");
    }

    return OAM_E_OK;
}

oam_status ctc_oam_swimage_image_ready(oam_uint8 img_id)
{
    return OAM_E_OK;
}

oam_status ctc_oam_swimage_close_adapt(oam_uint8 img_id)
{
    cs_int32 ret = 0;
    oam_status status = OAM_E_OK;

    ret = cs_swdl_writeimage(img_id, NULL);

    if (ret != 0) {
        OAM_ORG_LOG("write the image to flash failed ret = %d \n", ret);
        status = OAM_E_ERROR;
    }

    cs_swdl_clean();
    /* LED end blink */
    led_send_message(0, LED_MSG_UPDATE, LED_EVENT_UPGRADED);
    return status;
}


oam_status ctc_oam_swimage_active_adapt()
{
    oam_int32 ret = 0;
    oam_uint8 img_id = ctc_oam_swimage_id_get_adapt();

    ret = cs_swdl_activeimage(img_id);

    if (ret != 0) {
        OAM_ORG_LOG("active image %d failed, ret = %d \n",
                    img_id, ret);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}


oam_status ctc_oam_swimage_commit_adapt()
{
    tSwImgCfg img_info ;
    oam_uint8 img_id = 0;
    oam_int32 ret = 0;

    memset(&img_info, 0x00, sizeof(tSwImgCfg));

    /* get the no commit image id */
    if (cs_swdl_getswimginfo(0, &img_info)) {
        OAM_ORG_LOG("get image info failed \n");
        return OAM_E_ERROR;
    }

    if (img_info.isValidB && img_info.isCommittedB) {
        img_id = 1;
    }

    ret = cs_swdl_commitimage(img_id);

    if (ret != 0) {
        OAM_ORG_LOG("commit image id = %d failed, ret = %d \n", img_id, ret);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}


oam_status ctc_oam_multi_llid_admin_ctrl_set_adapt(oam_uint32 llid_num)
{

    return OAM_E_OK;
}

#if 0
static void ctc_oam_eth_port_link_status_handler(sdl_event_info_s *pinfo)
{
    oam_port_id_t port;
    oam_boolean   status;

    OAM_ASSERT(pinfo != NULL);

    port = pinfo->port_num;
    status = pinfo->event_data.link_chng_data.link_up;

    OAM_LOG_MAJ("port %d, status %d\n", port, status);

    port = __OAM_ETH_PORT(port);

    if ((port == OAM_PON_PORT_ID) || (port > CTC_MAX_ONU_UNI_PORTS)) {
        OAM_LOG_MAJ("oam_handler_port_link_status port %d error!\n", port);
        return;
    }

    if (status == OAM_TRUE) {
        ctc_oam_build_alarm_event(CTC_PORT_ALARM, port, CTC_ALARM_PORT_LOS,
                                  CTC_ALARM_CLEAR, 0);
    }
    else {
        ctc_oam_build_alarm_event(CTC_PORT_ALARM, port, CTC_ALARM_PORT_LOS,
                                  CTC_ALARM_RAISE, 0);
    }

}

static void ctc_oam_eth_port_auto_neg_handler(const sdl_event_info_s *pinfo)
{
    oam_port_id_t port;
    oam_boolean   status = OAM_FALSE;

    OAM_ASSERT(pinfo != NULL);

    port = pinfo->port_num;
    status = pinfo->event_data.auto_nego_status.status;

    OAM_LOG_MAJ("port %d, status %d\n", port, status);

    port = __OAM_ETH_PORT(port);

    if ((port == OAM_PON_PORT_ID) || (port > CTC_MAX_ONU_UNI_PORTS)) {
        OAM_LOG_MAJ("oam_handler_port_link_status port %d error!\n", port);
        return;
    }

    if (status) {
        ctc_oam_build_alarm_event(CTC_PORT_ALARM, port, CTC_ALARM_AUTO_NEG_FAIL,
                                  CTC_ALARM_CLEAR, 0);
    }
    else {
        ctc_oam_build_alarm_event(CTC_PORT_ALARM, port, CTC_ALARM_AUTO_NEG_FAIL,
                                  CTC_ALARM_RAISE, 0);
    }

}
#endif

oam_status ctc_oam_eth_port_alarm_reg_adapt()
{
#if 0
    cs_event_cfg_s   evt_cfg;
    cs_status_e       status;

    memset(&evt_cfg, 0, sizeof(cs_event_cfg_s));
    //evt_cfg.callback   = ctc_oam_eth_port_link_status_handler;
    evt_cfg.user_data  = 0;
    evt_cfg.context_id = 1;
    evt_cfg.priority   = 1;
    evt_cfg.context    = SDL_EVENT_CONTEXT_USER;
    evt_cfg.port_type_bitmap = CS_PORT_TYPE_ETHERNET;
    evt_cfg.port_num =  CS_PORT_ETH0;
    status = cs_event_register(0, CS_EVENT_ETH_LINK_STATUS_CHNG, &evt_cfg);

    if (CS_OK != status) {
        OAM_LOG_INFO("Error1: Eth link status change Event reg failed. ret=0x%x\n",
                     status);
        return OAM_E_ERROR;
    }

    memset(&evt_cfg, 0, sizeof(cs_event_cfg_s));
    //evt_cfg.callback   = ctc_oam_eth_port_auto_neg_handler;
    evt_cfg.user_data  = 0;
    evt_cfg.context_id = 1;
    evt_cfg.priority   = 1;
    evt_cfg.context    = SDL_EVENT_CONTEXT_USER;
    evt_cfg.port_type_bitmap = CS_PORT_TYPE_ETHERNET;
    evt_cfg.port_num =  CS_PORT_ETH0;
    status = cs_event_register(0, CS_EVENT_ETH_PHY_AUTO_NEGO_STATUS, &evt_cfg);

    if (CS_OK != status) {
        OAM_LOG_INFO("Error1: Eth auto negotiation Event reg failed. ret=0x%x\n",
                     status);
        return OAM_E_ERROR;
    }
#endif
    return OAM_E_OK;
}

#if 0
static void ctc_oam_loop_detect_handle(oam_port_id_t port, oam_boolean status,
                                       oam_uint32 info)
{
    oam_uint64 alarm_info = 0;
    oam_uint8 alarm_op = CTC_ALARM_RAISE;

    /*build CTC event*/
    if (oam_is_cuc_oam()) {
        alarm_info = (info ? CUC_OUT_LOOP_VALUE : CUC_INNER_LOOP_VALUE);
    }

    alarm_op = (status ? CTC_ALARM_RAISE : CTC_ALARM_CLEAR);

    ctc_oam_build_alarm_event(CTC_PORT_ALARM, port, CTC_ALARM_PORT_LB,
                              alarm_op, alarm_info);
}
#endif

oam_status ctc_oam_ld_alarm_reg_adapt()
{
#if 0
    cs_ld_status ret = CS_LD_OK;

    ret = cs_ld_alarm_func_reg(ctc_oam_loop_detect_handle);

    if (ret != CS_LD_OK)
        return OAM_E_ERROR;
#endif
    return OAM_E_OK;
}

#if 0
static void ctc_oam_gpio_alarm_handle(
    oam_boolean status,
    oam_uint16  alarm_code,
    oam_uint64 alarm_info)
{
    oam_uint8 alarm_op = CTC_ALARM_RAISE;

    alarm_op = (status ? CTC_ALARM_RAISE : CTC_ALARM_CLEAR);

    ctc_oam_build_alarm_event(CTC_ONU_ALARM, 0, alarm_code,
                              alarm_op, alarm_info);
}
#endif

oam_status ctc_oam_gpio_alarm_reg_adapt()
{
    //cs_gpio_ctc_alarm_reg(ctc_oam_gpio_alarm_handle);

    return OAM_E_OK;
}

static int ctc_oam_opm_alarm_handle(cs_dev_id_t dev_id, oam_uint32 port,
                                    oam_uint32 type, cs_boolean status, oam_uint32 info)
{
    cs_uint16  alarm_code = 0;
    cs_uint8   alarm_op = CTC_ALARM_RAISE;

    switch (type) {
            /* temperature */
        case OPM_ALARM_TEMP_HIGH_ALARM:
            alarm_code = CTC_ALARM_PON_TEMP_HIGH;
            break;

        case OPM_ALARM_TEMP_LOW_ALARM:
            alarm_code = CTC_ALARM_PON_TEMP_LOW;
            break;

        case OPM_ALARM_TEMP_HIGH_WARNING:
            alarm_code = CTC_WARN_PON_TEMP_HIGH;
            break;

        case OPM_ALARM_TEMP_LOW_WARNING:
            alarm_code = CTC_WARN_PON_TEMP_LOW;
            break;

            /* voltage */
        case OPM_ALARM_VOLT_HIGH_ALARM:
            alarm_code = CTC_ALARM_VCC_HIGH;
            break;

        case OPM_ALARM_VOLT_LOW_ALARM:
            alarm_code = CTC_ALARM_VCC_LOW;
            break;

        case OPM_ALARM_VOLT_HIGH_WARNING:
            alarm_code = CTC_WARN_VCC_HIGH;
            break;

        case OPM_ALARM_VOLT_LOW_WARNING:
            alarm_code = CTC_WARN_VCC_LOW;
            break;

            /* tx bias current */
        case OPM_ALARM_TX_BIAS_HIGH_ALARM:
            alarm_code = CTC_ALARM_TX_BIAS_HIGH;
            break;

        case OPM_ALARM_TX_BIAS_LOW_ALARM:
            alarm_code = CTC_ALARM_TX_BIAS_LOW;
            break;

        case OPM_ALARM_TX_BIAS_HIGH_WARNING:
            alarm_code = CTC_WARN_TX_BIAS_HIGH;
            break;

        case OPM_ALARM_TX_BIAS_LOW_WARNING:
            alarm_code = CTC_WARN_TX_BIAS_LOW;
            break;

            /* tx power */
        case OPM_ALARM_TX_PWR_HIGH_ALARM:
            alarm_code = CTC_ALARM_TX_POW_HIGH;
            break;

        case OPM_ALARM_TX_PWR_LOW_ALARM:
            alarm_code = CTC_ALARM_TX_POW_LOW;
            break;

        case OPM_ALARM_TX_PWR_HIGH_WARNING:
            alarm_code = CTC_WARN_TX_POW_HIGH;
            break;

        case OPM_ALARM_TX_PWR_LOW_WARNING:
            alarm_code = CTC_WARN_TX_POW_LOW;
            break;

            /* rx power */
        case OPM_ALARM_RX_PWR_HIGH_ALARM:
            alarm_code = CTC_ALARM_RX_POW_HIGH;
            break;

        case OPM_ALARM_RX_PWR_LOW_ALARM:
            alarm_code = CTC_ALARM_RX_POW_LOW;
            break;

        case OPM_ALARM_RX_PWR_HIGH_WARNING:
            alarm_code = CTC_WARN_RX_POW_HIGH;
            break;

        case OPM_ALARM_RX_PWR_LOW_WARNING:
            alarm_code = CTC_WARN_RX_POW_LOW;
            break;

        default:
            return CS_OPM_E_ERROR;
    }

    if (status)
        alarm_op = CTC_ALARM_RAISE;
    else
        alarm_op = CTC_ALARM_CLEAR;

    ctc_oam_build_alarm_event(CTC_PORT_ALARM, port, alarm_code, alarm_op, info);

    return CS_OPM_OK;
}

oam_status ctc_oam_opm_alarm_reg_adapt()
{
    cs_opm_status ret = CS_OPM_OK;

    cs_opm_alarm_func_reg(ctc_oam_opm_alarm_handle);

    if (ret != CS_OPM_OK)
        return OAM_E_ERROR;

    return OAM_E_OK;
}

#endif
