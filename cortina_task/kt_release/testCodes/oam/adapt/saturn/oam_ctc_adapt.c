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

    #include "classifier.h"
    #include "epon.h"
    #include "eth_port.h"
    #include "ca_event.h"
//#include "event_type.h"
//#include "gpon.h"
    #include "l2.h"
    #include "mcast.h"
    #include "mirror.h"
    #include "nat.h"
    #include "pkt_tx.h"
    #include "port.h"
    #include "qos.h"
    #include "queue.h"
    #include "rate.h"
    #include "route.h"
    #include "rx.h"
    #include "special_packet.h"
    #include "sys.h"
    #include "tunnel.h"
    #include "vlan.h"
    #include "scfg.h"
    #include "loop_detect.h"
    #include "opm_api.h"
    #include "ca_gpio.h"
//#include "mc_ctc_onu_api.h"
//#include "mc_api.h"

    #include "ca_version.h"
    #if 0
        #include "swdlapp.h"
        #include "epon_port.h"
        #include "cs_version.h"
        #include "mpcp.h"
        #include "stats_api.h"
        #include "sdl_peri.h"
        #include "led.h"
        #include "gpio_api.h"
        #include "khook.h"
        #include "db_lynxe.h"
    #endif

    #ifdef ZTE_SPECIFIC
        #include "db_zte.h"
    #endif/*END_ZTE_SPECIFIC*/

extern ca_scfg_pon_mac_mode_t g_epon_mac_mod;

static oam_uint8 g_holdover_state = OAM_CTC_ONU_HOLDOVER_DEACTIVED;
static oam_uint32 g_holdover_time = 0;
static oam_uint32 g_normal_gate_time = 0;
static oam_uint32 g_laser_tx_power_timer = 0;
static oam_uint32 g_ctc_oam_cfg_counter = 0;
static ca_uint32            g_uni_num              = 0;
static ctc_port_info_t*     g_ctc_port_info        = NULL;
static ctc_oam_port_cls_t g_port_cls_cfg[MAX_OAM_PORTS + 1];


static void __laser_tx_power_on(void *data)
{
    ca_epon_port_laser_mode_set(0, (CA_PORT_TYPE_EPON << 8), CA_EPON_PORT_LASER_MODE_BURST);
    ca_gpio_set_value(CA_GPIO_PON_TX_POWRER_EN, 1);
//    led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_ON);
    g_laser_tx_power_timer = 0;
}

oam_status ctc_oam_cls_syn_db(oam_port_id_t port)
{
#if 0
    oam_uint8                   num = 0;
#ifdef ZTE_SPECIFIC
    db_zte_cls_t              db_cls;
#endif/*END_ZTE_SPECIFIC*/
    cs_sal_classification_t  cfg [8];
    ca_status_t                  ret = 0;

    ret = cs_sal_port_classification_get(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), &num, cfg);

    if (ret != CA_E_OK)
    {
        return CA_E_ERROR;
    }

#ifdef ZTE_SPECIFIC
    memset(&db_cls, 0x00, sizeof(db_zte_cls_t));
    db_cls.port_id = port;
    db_cls.num = num;
    memcpy (db_cls.rule, cfg , sizeof (cs_sal_classification_t )*num);

    cs_db_entry_mod(TB_ZTE_CLS , (oam_uint8 *) &db_cls, sizeof(db_zte_cls_t),
                    column_bit(TB_ZTE_CLS, "ALL"));
#endif/*END_ZTE_SPECIFIC*/
#endif
    return 0;

}


oam_status ctc_oam_mc_vlan_syn_db(oam_port_id_t port)
{
    // oam_uint16            number = 0;
#ifdef ZTE_SPECIFIC
    db_zte_mc_vlan_t      db_mc_vlan;
#endif/*END_ZTE_SPECIFIC*/
    //oam_uint16            vlan_cfg [8];
    ca_status_t             ret = 0;

//    cs_sal_mc_vlan_get (0, OAM_UNI_PORT_ID_TO_CA_PORT(port), vlan_cfg, &number);

    if (ret != CA_E_OK)
    {
        return CA_E_ERROR;
    }

#ifdef ZTE_SPECIFIC
    memset(&db_mc_vlan, 0x00, sizeof(db_zte_mc_vlan_t));
    db_mc_vlan.port_id = port;
    db_mc_vlan.num = number;
    memcpy(db_mc_vlan.vlan, vlan_cfg, sizeof(oam_uint16) * number);

    cs_db_entry_mod(TB_ZTE_MC_VLAN, (void *)&db_mc_vlan, sizeof(db_zte_mc_vlan_t),
        column_bit(TB_ZTE_MC_VLAN, "ALL"));
#endif/*END_ZTE_SPECIFIC*/

    return ret;

}


/***************** SATURN ADAPT START ****************************/
oam_status ctc_oam_eth_admin_state_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin)
{
    ca_boolean_t admin_status;

    if (admin == NULL)
    {
        return OAM_E_ERROR;
    }

    __PORT_CHECK(port);

    OAM_ORG_DEBUG("port = %d \r\n",  port);

    if (ca_port_enable_get(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), &admin_status))
    {
        return OAM_E_ERROR;
    }

    if (admin_status)
    {
        *admin = OAM_INTF_PHY_ADMIN_EN;
    }
    else
    {
        *admin = OAM_INTF_PHY_ADMIN_DIS;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_eth_auto_admin_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin)
{
    ca_boolean_t mode = 0;

    if (admin == NULL)
    {
        return OAM_E_ERROR;
    }

    OAM_ORG_DEBUG("port = %d \r\n",  port);

    __PORT_CHECK(port);

    if (ca_eth_port_autoneg_get(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), &mode))
    {
        return OAM_E_ERROR;
    }

    if (mode)
    {
        *admin = OAM_INTF_PHY_ADMIN_EN;
    }
    else
    {
        *admin = OAM_INTF_PHY_ADMIN_DIS;
    }

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

    if ((value == NULL) || (getNum == NULL))
    {
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

    if ((value == NULL) || (getNum == NULL))
    {
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
    oam_boolean                enable;

    if(cs_sal_epon_onu_fec_ability_get(0, -1, &enable))
    {
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

    oam_boolean                mode;
    oam_status                 retCode;
    ca_port_id_t port = (CA_PORT_TYPE_EPON << 8);
    oam_boolean      tx_enable = 0,
                     rx_enable = 0;
#ifdef ZTE_SPECIFIC
    db_zte_onu_svc_t          svc;
#endif/*END_ZTE_SPECIFIC*/

    if (fecMode == OAM_INTF_FEC_MODE_EN)
    {
        mode = 1;
        tx_enable = 1;
        rx_enable = 1;
    }
    else
    {
        mode = 0;
        tx_enable = 0;
        rx_enable = 0;
    }

    retCode =  ca_epon_port_fec_enable_set(
        0,
        port,
        tx_enable,
        rx_enable);
    if (retCode)
    {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC

    if (retCode == CA_E_OK)
    {
        memset(&svc, 0x00, sizeof(svc));
        svc.fec_mode = mode;
        cs_db_entry_mod(TB_ZTE_ONU_SVC, (void *)&svc, sizeof(svc),
            column_bit(TB_ZTE_ONU_SVC, "fec_mode"));
    }

#endif/*END_ZTE_SPECIFIC*/

    return OAM_E_OK;
}

oam_status ctc_oam_eth_fec_mode_get_adapt(
    oam_uint32 *fecMode)
{
    ca_status_t  ca_ret = CA_E_ERROR;
    ca_port_id_t pon_port = (CA_PORT_TYPE_EPON << 8);
    oam_boolean  tx_enable = 0,
                 rx_enable = 0;

    ca_ret = ca_epon_port_fec_enable_get(
        0,
        pon_port,
        &tx_enable,
        &rx_enable);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

    if (tx_enable && rx_enable)
    {
        *fecMode = OAM_INTF_FEC_MODE_EN;
    }
    else if (tx_enable == 0 && rx_enable == 0)
    {
        *fecMode = OAM_INTF_FEC_MODE_DIS;
    }
    else
    {
        *fecMode = OAM_INTF_FEC_MODE_UNKOWN;
    }

    return OAM_E_OK;

}


oam_status ctc_oam_onu_active_pon_get_adapt(
    oam_uint8 *pon)
{
    *pon = 0;
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
    ca_status_t  ca_ret = CA_E_ERROR;
    ca_eth_port_link_status_t status;

    if (linkState == NULL)
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d \r\n",  port);

    __PORT_CHECK(port);

    ca_ret = ca_eth_port_link_status_get(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), &status);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

    if (status == CA_PORT_LINK_DOWN)
    {
        *linkState = OAM_CTC_ONU_ETH_DOWN;
    }
    else
    {
        *linkState = OAM_CTC_ONU_ETH_UP;
    }

    return OAM_E_OK;

}


oam_status ctc_oam_eth_port_pause_get_adapt(
    oam_port_id_t port,
    oam_uint8  *flowControl)
{
    ca_status_t   ca_ret = CA_E_ERROR;
    ca_uint16_t   quanta = 0;

    if (flowControl == NULL)
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d \r\n", port);

    __PORT_CHECK(port);

    ca_ret = ca_eth_port_pause_quanta_get(
        0,
        OAM_UNI_PORT_ID_TO_CA_PORT(port),
        &quanta);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

    if (quanta)
    {
        *flowControl = 1;
    }
    else
    {
        *flowControl = 0;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_eth_port_pause_set_adapt(
    oam_port_id_t port,
    oam_uint8 flowControl)
{
    ca_status_t ca_ret = CA_E_ERROR;
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t port_info;
#endif/*END_ZTE_SPECIFIC*/

    OAM_ORG_DEBUG("port = %d, flowctrl = %d\r\n",  port, flowControl);

    __PORT_CHECK(port);

    ca_ret = ca_eth_port_pause_quanta_set(
        0,
        OAM_UNI_PORT_ID_TO_CA_PORT(port),
        flowControl);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC
    memset(&port_info, 0x00, sizeof(port_info));
    port_info.port_id = port;
    port_info.pause = flowControl;
    cs_db_entry_mod(TB_ETH_PORT, (void *)&port_info, sizeof(port_info),
        column_bit(TB_ETH_PORT, "pause"));
#endif/*END_ZTE_SPECIFIC*/
    return OAM_E_OK;
}


oam_status ctc_oam_eth_admin_state_set_adapt(
    oam_port_id_t port,
    oam_uint32 phyAdmin)
{
    ca_status_t  ca_ret = CA_E_ERROR;
    ca_boolean_t port_admin = 0;
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t port_info;
#endif/*END_ZTE_SPECIFIC*/


    OAM_ORG_DEBUG("port = %d, phyAdmin = %d\r\n", port, phyAdmin);

    __PORT_CHECK(port);

    if (phyAdmin == OAM_INTF_PHY_ADMIN_DIS)
    {
        port_admin = 0;
    }
    else
    {
        port_admin = 1;
    }

    ca_ret = ca_port_enable_set(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), port_admin);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC
    memset(&port_info, 0x00, sizeof(port_info));
    port_info.port_id = port;
    port_info.admin = port_admin;
    cs_db_entry_mod(TB_ETH_PORT, (void *)&port_info, sizeof(port_info),
        column_bit(TB_ETH_PORT, "admin"));
#endif/*END_ZTE_SPECIFIC*/
    return OAM_E_OK;
}


oam_status ctc_oam_auto_neg_restart_set_adapt(
    oam_port_id_t port)
{
    ca_status_t ca_ret = CA_E_ERROR;
    ca_boolean_t enable = 1;
    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    ca_ret = ca_eth_port_autoneg_set(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), enable);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_auto_neg_ctrl_set_adapt(
    oam_port_id_t port,
    oam_uint32 autoNegAdmin)
{
    ca_status_t ca_ret = CA_E_ERROR;
    ca_boolean_t mode = 0;
    oam_uint8  auto_neg = 0;
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t   port_info;
#endif/*END_ZTE_SPECIFIC*/

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    if (autoNegAdmin == OAM_INTF_PHY_ADMIN_DIS)
    {
        mode = 0;
    }
    else
    {
        mode = 1;
    }

    ca_ret = ca_eth_port_autoneg_set(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), mode);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0)
    {
        memset(&port_info, 0x00, sizeof(port_info));
        port_info.port_id = port;
        port_info.auto_neg = auto_neg;
        cs_db_entry_mod(TB_ETH_PORT, (void *)&port_info, sizeof(port_info),
            column_bit(TB_ETH_PORT, "auto_neg"));
    }

#endif/*END_ZTE_SPECIFIC*/
    return OAM_E_OK;
}

oam_status ctc_oam_eth_port_policing_get_adapt(
    oam_port_id_t port,
    oam_uint8  *oper,
    oam_uint32 *cir,
    oam_uint32 *cbs,
    oam_uint32 *ebs)
{
    ca_status_t ca_ret = CA_E_ERROR;
    ca_policer_t policy;

    ca_ret = ca_port_policer_get(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), &policy);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

    *oper = policy.mode;
    *cir = policy.cir;
    *cbs = policy.cbs;
    *ebs = policy.pbs;

    return OAM_E_OK;

}

oam_status ctc_oam_eth_port_policing_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  cbs,
    oam_uint32  ebs)
{
    ca_status_t  ca_ret = CA_E_ERROR;
    ca_policer_t policy;
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t   port_info;
#endif/*END_ZTE_SPECIFIC*/

    policy.cir = cir;
    policy.cbs = cbs;
    policy.pbs = ebs;

    if (oper == CTC_PORT_POLICY_OPER_DISABLE)
    {
        policy.mode = 0;
    }
    else
    {
        policy.mode = 1;
    }

    ca_ret = ca_port_policer_set(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), &policy);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC
    memset(&port_info, 0x00, sizeof(port_info));
    port_info.port_id = port;
    port_info.policy_rate = cir;
    port_info.policy_cbs = cbs;
    port_info.policy_ebs = ebs;
    port_info.policy_enable = policy.enable;
    cs_db_entry_mod(TB_ETH_PORT, (void *)&port_info, sizeof(port_info),
        column_bit(TB_ETH_PORT, "policy_rate") |
        column_bit(TB_ETH_PORT, "policy_cbs") |
        column_bit(TB_ETH_PORT, "policy_ebs") |
        column_bit(TB_ETH_PORT, "policy_enable"));
#endif/*END_ZTE_SPECIFIC*/
    return OAM_E_OK;
}


oam_status ctc_oam_eth_ds_rate_limit_get_adapt(
    oam_port_id_t port,
    oam_uint8 *oper,
    oam_uint32 *cir,
    oam_uint32 *pir)
{
    ca_status_t ca_ret = CA_E_ERROR;
    ca_shaper_t rate;

    ca_ret = ca_port_shaper_get(
        0,
        OAM_UNI_PORT_ID_TO_CA_PORT(port),
        &rate);
    if (ca_ret)
    {
        return OAM_E_ERROR;
    }

    *oper = rate.enable;
    *cir = rate.rate;
    *pir = rate.burst_size;

    return OAM_E_OK;
}

oam_status ctc_oam_eth_ds_rate_limit_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  pir)
{
    ca_status_t ca_ret = CA_E_ERROR;
    ca_shaper_t rate;
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t      port_info;
#endif/*END_ZTE_SPECIFIC*/

    rate.rate = cir;
    rate.burst_size = pir;

    if (oper == CTC_PORT_POLICY_OPER_DISABLE)
    {
        rate.enable = 0;
    }
    else
    {
        rate.enable = 1;
    }

    ca_ret = ca_port_shaper_set(
        0,
        OAM_UNI_PORT_ID_TO_CA_PORT(port),
        &rate);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0)
    {
        memset(&port_info, 0x00, sizeof(port_info));
        port_info.port_id = port;
        port_info.ds_rate_limit_enable = rate.enable;
        port_info.ds_rate_limit = rate.rate;
        port_info.ds_cbs = rate.cbs;
        cs_db_entry_mod(TB_ETH_PORT, (void *)&port_info, sizeof(port_info),
            column_bit(TB_ETH_PORT, "ds_rate_limit_enable") |
            column_bit(TB_ETH_PORT, "ds_rate_limit") |
            column_bit(TB_ETH_PORT, "ds_cbs"));
    }

#endif/*END_ZTE_SPECIFIC*/
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

    if (cs_sal_fdb_mac_limit_set(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), num))
    {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0)
    {
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

    if (num == NULL)
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_fdb_mac_limit_get(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), &mac_limit))
    {
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

    switch (mode)
    {
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

    if (cs_sal_eth_port_mode_set(0, port, port_mode))
    {
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
    return CA_E_PARAM;

    memset(&uni_stats, 0, sizeof(uni_stats));

    if (CA_E_OK != cs_stats_uni_get(port, &uni_stats))
    {
        return CA_E_ERROR;
    }

    stats->in_octets = uni_stats.rxbyte_cnt;
    stats->in_ucast_pkt = uni_stats.rxucfrm_cnt;
    stats->in_nucast_pkt = uni_stats.rxmcfrm_cnt + uni_stats.rxbcfrm_cnt;
    stats->in_discard = 0;
    stats->in_error = uni_stats.rxcrcerrfrm_cnt + uni_stats.rxundersizefrm_cnt +
    uni_stats.rxoversizefrm_cnt;
    stats->out_octets = uni_stats.txbyte_cnt;
    stats->out_ucast_pkt = uni_stats.txucfrm_cnt;
    stats->out_nucast_pkt = uni_stats.txmcfrm_cnt + uni_stats.txbcfrm_cnt;
    stats->out_discard = 0;
    stats->out_error = uni_stats.txcrcerrfrm_cnt + uni_stats.txoversizefrm_cnt;
#endif
    return CA_E_OK;
}



oam_status ctc_oam_pon_port_stat_get_adapt(
    oam_ctc_onu_llid_stats1 *stats1,
    oam_ctc_onu_llid_stats2 *stats2)
{
#if 0
    stats_pon_info_t pon_stats;

    if (!stats1 && !stats2)
    {
        return CA_E_PARAM;
    }

    memset(&pon_stats, 0, sizeof(pon_stats));

    cs_stats_pon_get(&pon_stats);

    if (stats1)
    {
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

    if (stats2)
    {
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
    return CA_E_OK;
}

oam_status ctc_oam_onu_port_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8  *mode)
{
#if 0
    cs_sal_eth_port_mode_t port_mode;

    if (mode == NULL)
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    if (cs_sal_eth_port_mode_get(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), &port_mode))
    {
        return OAM_E_ERROR;
    }

    switch (port_mode)
    {
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

    if (mode == NULL)
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  portId);

    __PORT_CHECK(portId);

    if (cs_sal_eth_port_status_get(0, portId + 3, &port_status))
    {
        return OAM_E_ERROR;
    }

    if (port_status.link_status == CS_SAL_LINK_DOWN)
    {
        *mode = CTC_OAM_PORT_MODE_10M_HALF;
    }
    else
    {
        switch (port_status.speed)
        {
            case CS_SAL_SPEED_10MBPS:
            if (port_status.duplex == CS_SAL_DUPLEX_FULL)
            {
                *mode = CTC_OAM_PORT_MODE_10M_FULL;
            }
            else
            {
                *mode = CTC_OAM_PORT_MODE_10M_HALF;
            }

            break;

            case CS_SAL_SPEED_100MBPS:
            if (port_status.duplex == CS_SAL_DUPLEX_FULL)
            {
                *mode = CTC_OAM_PORT_MODE_100M_FULL;
            }
            else
            {
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
    if (aging_time == NULL)
    {
        return OAM_E_PARAM;
    }

    if (ca_l2_aging_time_get(0, aging_time))
    {
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_eth_mac_aging_time_set_adapt(
    oam_uint32 aging_time)
{
#ifdef ZTE_SPECIFIC
    db_zte_onu_svc_t   svc;
#endif/*END_ZTE_SPECIFIC*/

    OAM_ORG_DEBUG("aging_time = %d\r\n",  aging_time);

    if (ca_l2_aging_time_set(0, aging_time))
    {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC
    memset(&svc, 0x00, sizeof(svc));
    svc.mac_aging_time = aging_time;
    cs_db_entry_mod(TB_ZTE_ONU_SVC, (oam_uint8 *)&svc, sizeof(svc),
        column_bit(TB_ZTE_ONU_SVC, "mac_aging_time"));
#endif/*END_ZTE_SPECIFIC*/
    return OAM_E_OK;
}

oam_status ctc_oam_onu_fast_leave_ability_get_adapt(
    oam_uint32 *num,
    oam_uint32 *mode)
{
//    mc_status ret = MC_E_ERROR;

//    ret = mc_ctc_leave_ability_get ( 0, mode, num );

//    return ret;
}

oam_status ctc_oam_onu_fast_leave_state_get_adapt(
    oam_uint32 *state)
{
//    mc_status ret = MC_E_ERROR;

//    ret = mc_ctc_leave_state_get(0, state);

//    return ret;
}

oam_status ctc_oam_fast_leave_ctrl_set_adapt(
    oam_uint32 fastLeaveAdmin)
{
#if 0
//    mc_status ret = MC_E_ERROR;
#ifdef ZTE_SPECIFIC
    db_zte_fast_leave_t   db_fast_leave;
#endif/*END_ZTE_SPECIFIC*/

//    ret = mc_ctc_leave_state_set (0, fastLeaveAdmin);
#ifdef ZTE_SPECIFIC

    if ( ret == CA_E_OK )
    {
        db_fast_leave.fast_leave = fastLeaveAdmin;
        cs_db_entry_mod( TB_ZTE_FAST_LEAVE, (void *) &db_fast_leave,
                        sizeof( db_zte_fast_leave_t), column_bit(TB_ZTE_FAST_LEAVE, "ALL"));
    }

#endif/*END_ZTE_SPECIFIC*/

    return ret;
#endif
}

oam_status ctc_oam_onucap_get_adapt (
    oam_ctc_onu_capabilities_t* onu_cap)
{
    oam_status ret        = OAM_E_OK;
    oam_uint32 uni_bmp    = 0;
    oam_uint8  hasBattery = 1; //cs_gpio_ups_valid();

    if ( !onu_cap )
    {
        ret = OAM_E_PARAM;
        goto ERROR_CTC_OAM_ONUCAP_GET_ADAPT;
    }
    /*Need update for Single port ONU, YiDing*/
    oam_uni_bmp_get_adapt ( &uni_bmp );
    onu_cap->serviceSupported = OAM_CTC_CHIP_CAP_GE_SUPP;
    onu_cap->numGEPorts = g_uni_num;
    onu_cap->geBitMap = htonll ( uni_bmp );
    onu_cap->numPOTSPorts = 0;
    onu_cap->numE1Ports = 0;
    onu_cap->numUSQueues = 8;
    onu_cap->maxQueueUSPort = 8;
    onu_cap->numDSQueues = 4;
    onu_cap->maxQueueDSPort = 4;
    onu_cap->BatteryBackup = hasBattery;

ERROR_CTC_OAM_ONUCAP_GET_ADAPT:
    return ret;
}

oam_status ctc_oam_onu_llid_queue_config_get_adapt(
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

oam_status ctc_oam_onu_llid_queue_config_set_adapt(
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

    cs_db_entry_get(TB_ZTE_ONU_SVC, (void *)&svc, sizeof(svc));
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
    cs_db_entry_mod(TB_ZTE_ONU_SVC, (void *)&svc, sizeof(svc),
        column_bit(TB_ZTE_ONU_SVC, "cfg_counter"));
    cs_db_lynxe_save();
#endif/*END_ZTE_SPECIFIC*/

    g_ctc_oam_cfg_counter = counter;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_cfg_mgmt_set_adapt(oam_uint8 action)
{
#ifdef ZTE_SPECIFIC

    switch (action)
    {
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
        return CA_E_ERROR;
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
    oam_status ret = CA_E_OK;
    /*oam_uint8 gate_timeout = 0;*/
    ca_boolean_t dereg_en;
    ca_port_id_t port = (CA_PORT_TYPE_EPON << 8);

    if (holdover_state == OAM_CTC_ONU_HOLDOVER_DEACTIVED)
    {
        /*startup_config_read(CFG_ID_MPCP_TIMEOUT, 1, &gate_timeout);
        ret = ca_epon_mpcp_timer_set(0, port, gate_timeout*10, 1);*/
        if (g_normal_gate_time != 0)
        {
            ret = ca_epon_mpcp_timer_set(0, port, g_normal_gate_time,  1);
        }
    }
    else if (holdover_state == OAM_CTC_ONU_HOLDOVER_ACTIVED
             && holdover_time >= OAM_CTC_ONU_HOLDOVER_TIME_MIN
             && holdover_time <= OAM_CTC_ONU_HOLDOVER_TIME_MAX)
    {

        ret = ca_epon_mpcp_timer_get(0, port, &g_normal_gate_time, &dereg_en);
        if (ret)
            return OAM_E_ERROR;

        ret = ca_epon_mpcp_timer_set(0, port, holdover_time, 0);
        if (ret)
            return OAM_E_ERROR;

    }
    else
    {
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
    if (!g_ctc_port_info)
    {
        return OAM_E_PARAM;
    }
    
    if ((tpid == NULL) || (tag == NULL))
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n", port);

    __PORT_CHECK(port);


    *tpid = g_ctc_port_info[port].def_vlan_tag.tpid;
    *tag  = g_ctc_port_info[port].def_vlan_tag.tag;

    return OAM_E_OK;
}


oam_status ctc_oam_onu_vlan_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8 *mode)
{
    if (!g_ctc_port_info)
    {
        return OAM_E_PARAM;
    }
    
    if (!mode)
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n", port);

    __PORT_CHECK(port);

    *mode = g_ctc_port_info[port].vlan_mode;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_translate_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *translate_num,
    oam_ctc_onu_vlan_xlate_entry *vlan)
{
    oam_uint16            nums;
    oam_uint32            i = 0;
    
    if (!g_ctc_port_info)
    {
        return OAM_E_PARAM;
    }
    

    if ((vlan == NULL) || (translate_num == NULL))
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n", port);

    __PORT_CHECK(port);

    nums = g_ctc_port_info[port].translate_num;

    while (i < nums)
    {
        vlan[i].tag2.tpid  = g_ctc_port_info[port].translate_vlan[i].tag2.tpid;
        vlan[i].tag1.tpid  = g_ctc_port_info[port].translate_vlan[i].tag1.tpid;
        vlan[i].tag2.tag = g_ctc_port_info[port].translate_vlan[i].tag2.tag;
        vlan[i].tag1.tag = g_ctc_port_info[port].translate_vlan[i].tag1.tag;
        ++i;
    }

    *translate_num = nums;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_trunk_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *trunk_num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    oam_uint16            nums;
    oam_uint32            i = 0;

    if (!g_ctc_port_info)
    {
        return OAM_E_PARAM;
    }
    
    if ((vlan == NULL) || (trunk_num == NULL))
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    nums = g_ctc_port_info[port].trunk_num;

    while (i < nums)
    {
        vlan[i].tpid = g_ctc_port_info[port].trunk_vlan[i].tpid;
        vlan[i].tag = g_ctc_port_info[port].trunk_vlan[i].tag;
        ++i;
    }

    *trunk_num = nums;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_agg_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *agg_num,
    ctc_oam_agg_vlan_t *vlan)
{
    oam_uint16            nums;
    oam_uint32            i = 0;
    
    if (!g_ctc_port_info)
    {
        return OAM_E_PARAM;
    }

    if ((vlan == NULL) || (agg_num == NULL))
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    nums = g_ctc_port_info[port].aggr_vlan[0].agg_vlan_num;

    while (i < nums)
    {
        vlan[0].agg_vlan[i].tpid = g_ctc_port_info[port].aggr_vlan[0].agg_vlan[i].tpid;
        vlan[0].agg_vlan[i].tag = g_ctc_port_info[port].aggr_vlan[0].agg_vlan[i].tag;
        ++i;
    }

    vlan[0].agg_dst_vlan.tpid  = g_ctc_port_info[port].aggr_vlan[0].agg_dst_vlan.tpid;
    vlan[0].agg_dst_vlan.tag  = g_ctc_port_info[port].aggr_vlan[0].agg_dst_vlan.tag;
    vlan[0].agg_vlan_num = nums;

    *agg_num = 1;

    return OAM_E_OK;
}

void _ctc_clear_vlan_member
(
    CA_IN oam_port_id_t port,
    CA_IN ca_uint32_t vid
    )
{
    oam_status ret = OAM_E_ERROR;
    ca_uint32_t  i,
                 j,
                 k,
                 m,
                 counter            = 0;
    ca_uint8_t   member_count = 0;
    ca_port_id_t  member_ports[16];
    ca_uint8_t   new_member_count = 0;
    ca_port_id_t  new_member_ports[16];
    ca_uint8_t   untagged_count = 0;
    ca_port_id_t  untagged_ports[16];
    ca_uint8_t   new_untagged_count = 0;
    ca_port_id_t  new_untagged_ports[16];
    ca_status_t ca_ret = CA_E_ERROR;
    ca_uint32_t ca_port = oam_port_to_ca_port_adapt(port);

    memset(member_ports, 0, sizeof(ca_port_id_t) * 16);
    memset(new_member_ports, 0, sizeof(ca_port_id_t) * 16);
    memset(untagged_ports, 0, sizeof(ca_port_id_t) * 16);
    memset(new_untagged_ports, 0, sizeof(ca_port_id_t) * 16);

    /* remove port from all the vlan */
//    for(i = 0; i < counter; i++){
    memset(member_ports, 0, sizeof(ca_port_id_t) * 16);
    memset(new_member_ports, 0, sizeof(ca_port_id_t) * 16);
    memset(untagged_ports, 0, sizeof(ca_port_id_t) * 16);
    memset(new_untagged_ports, 0, sizeof(ca_port_id_t) * 16);
    k = 0;
    m = 0;
    new_member_count = 0;
    new_untagged_count = 0;
    ca_l2_vlan_port_get(
        0,
        vid,
        &member_count,
        member_ports,
        &untagged_count,
        untagged_ports);
    for (j = 0; j < member_count; j++)
    {
        if (member_ports[j] == ca_port ||
            member_ports[j] == OAM_PON_PORT_ID_TO_CA_PORT())
        {
            continue;
        }
        else
        {
            new_member_ports[k] = member_ports[j];
            k++;
            new_member_count++;
        }
    }
    for (j = 0; j < untagged_count; j++)
    {
        if (untagged_ports[j] == ca_port ||
            member_ports[j] == OAM_PON_PORT_ID_TO_CA_PORT())
        {
            continue;
        }
        else
        {
            new_untagged_ports[m] = untagged_ports[j];
            m++;
            new_untagged_count++;
        }
    }

    ca_printf(
        "remove ca_port 0x%x from vid %d\r\nmembership\n",
        ca_port,
        vid);

    ca_ret = ca_l2_vlan_port_set(
        0,
        vid,
        new_member_count,
        new_member_ports,
        new_untagged_count,
        new_untagged_ports);
    if (ca_ret != CA_E_OK)
    {
        printf(
            "remove VLAN membership from port 0x%x failed, ret = %d\n",
            ca_port,
            ca_ret);
    }
//    }

    return OAM_E_OK;
}

void _ctc_add_vlan_member(
    ca_uint16_t vid,
    ca_port_id_t member_port,
    ca_port_id_t untagged_port)
{
    ca_uint8_t member_count = 0;
    ca_port_id_t member_ports[8];
    ca_uint8_t untagged_count = 0;
    ca_port_id_t untagged_ports[8];
    ca_status_t rc;

    if (!member_port && !untagged_port)
    {
        return;
    }

    printf("create vlan %u, mbr port 0x%x\n", vid, member_port);
    rc = ca_l2_vlan_create(0, vid);
    if (rc == CA_E_OK || CA_E_EXISTS)
    {
        ca_l2_vlan_port_get(0, vid, &member_count, member_ports, &untagged_count, untagged_ports);
        if (member_port)
        {
            member_ports[member_count] =  member_port;
            member_count++;
        }
        if (untagged_port)
        {
            untagged_ports[untagged_count] = untagged_port;
            untagged_count++;
        }
        rc = ca_l2_vlan_port_set(0, vid, member_count, member_ports, untagged_count, untagged_ports);
        if (rc != CA_E_OK)
        {
            printf("Failed to set mbr ports for vlan %u\n", vid);
        }
    }
    else
    {
        printf("Failed to create vlan %u\n", vid);
    }
}

ca_status_t
_ctc_vlan_mode_reset(
    CA_IN oam_port_id_t port)
{
    ca_status_t ret = CA_E_ERROR;
    oam_ctc_vlan_mode_t vlan_md = CTC_VLAN_MODE_TRANSPARENT;

    if (!g_ctc_port_info)
    {
        ret = CA_E_INIT;
        goto ERROR_CTC_VLAN_MODE_RESET;
    }

    if (port > MAX_OAM_PORTS)
    {
        ca_printf ( "%s: port %u exceeds max port number %u\n", __FUNCTION__, port,  MAX_OAM_PORTS);
        ret = CA_E_PARAM;
        goto ERROR_CTC_VLAN_MODE_RESET;
    }

    vlan_md = g_ctc_port_info[port].vlan_mode;

    ca_vlan_port_control_t port_config;
    ca_classifier_key_t      key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t   action;
    ca_uint32_t              idx = 0;
#if 0
    memset(&port_config, 0, sizeof(ca_vlan_port_control_t));

    port_config.drop_unknown_vlan = 0;

    ret = ca_l2_vlan_port_control_set(
                                      0,
                                      oam_port_to_ca_port_adapt(port),
                                      &port_config);
    if (ret != CA_E_OK)
    {
        goto ERROR_CTC_VLAN_MODE_RESET;
    }
#endif
    ca_port_discard_control_t port_cntrl;
    ca_vlan_port_control_t    vlan_p_cntrl;

    memset(&port_cntrl, 0, sizeof(ca_port_discard_control_t));
    memset(&vlan_p_cntrl, 0, sizeof(ca_vlan_port_control_t));

    ret = ca_port_discard_control_set(
        0,
        oam_port_to_ca_port_adapt(port),
        &port_cntrl);
    if (ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_l2_discard_control_set, port %u\n",
            port);
        goto ERROR_CTC_VLAN_MODE_RESET;
    }

    if (vlan_md == CTC_VLAN_MODE_TRANSPARENT)
    {
        ret = CA_E_OK;
        goto ERROR_CTC_VLAN_MODE_RESET;
    }

    ret = ca_l2_vlan_port_control_set(
        0,
        oam_port_to_ca_port_adapt(port),
        &vlan_p_cntrl);
    if (ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_l2_vlan_port_control_set , oam_port %u\n",
            port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_VLAN_MODE_RESET;
    }


    if (TAG2VID(g_ctc_port_info[port].def_vlan_tag.tag) != 0)
    {
        _ctc_clear_vlan_member(
            port,
            TAG2VID(g_ctc_port_info[port].def_vlan_tag.tag));
        ret = ca_port_pvid_vlan_set(
            0,
            oam_port_to_ca_port_adapt(port),
            0);
        if (ret != CA_E_OK)
        {
            printf(
                "%s: Failed to disable pvid on ca_port 0x%x\n",
                __FUNCTION__,
                TAG2VID(g_ctc_port_info[port].def_vlan_tag.tag));
        }
        memset(
            &g_ctc_port_info[port].def_vlan_tag,
            0x00,
            sizeof(oam_ctc_onu_vlan_t));
    }


    if (vlan_md == CTC_VLAN_MODE_TRUNK)
    {
        ca_uint32_t trnk_cnt = g_ctc_port_info[port].trunk_num;
        for (idx = 0; idx < trnk_cnt; idx++)
        {
            _ctc_clear_vlan_member(
                port,
                TAG2VID(g_ctc_port_info[port].trunk_vlan[idx].tag));
        }
        g_ctc_port_info[port].trunk_num = 0;
        memset(
            g_ctc_port_info[port].trunk_vlan,
            0x00,
            sizeof(oam_ctc_onu_vlan_t) * trnk_cnt);
        goto ERROR_CTC_VLAN_MODE_RESET;
    }

    oam_ctc_vlan_key_entry_t   zero_key;

    memset(&zero_key, 0x00, sizeof(oam_ctc_vlan_key_entry_t));


    if (vlan_md == CTC_VLAN_MODE_TRANSLATE)
    {
        ca_uint32_t trnslt_cnt = g_ctc_port_info[port].translate_num;
//        for (idx = 0; idx < trnslt_cnt; idx ++)
//        {
//        }
        for (idx = 0; idx < CTC_VLAN_MODE_TRANSLATE_MAX; idx++)
        {
            ca_int32_t cmp_ret = 0;
            ca_vlan_key_entry_t *tmp_key;
            cmp_ret = memcmp(
                g_ctc_port_info[port].translate_key + idx,
                &zero_key,
                sizeof(oam_ctc_vlan_key_entry_t));
            if (cmp_ret)
            {
                tmp_key = (ca_vlan_key_entry_t *)(g_ctc_port_info[port].translate_key + idx);
            }
            else
            {
                continue;
            }

            ret = ca_l2_vlan_ingress_action_delete(
                0,
                oam_port_to_ca_port_adapt(port),
                *tmp_key);
            if (ret != CA_E_OK)
            {
                printf("%s: Failed to delete l2_vlan_igr_action, idx %u\n", idx);
            }
            else
            {
                memset(
                    g_ctc_port_info[port].translate_key + idx,
                    0x00,
                    sizeof(oam_ctc_vlan_key_entry_t));
            }
        }

        g_ctc_port_info[port].translate_num = 0;
        memset(
            g_ctc_port_info[port].translate_vlan,
            0x00,
            sizeof(oam_ctc_onu_vlan_xlate_t) * trnslt_cnt);
        goto ERROR_CTC_VLAN_MODE_RESET;
    }

    if (vlan_md == CTC_VLAN_MODE_AGG)
    {
        ca_uint32_t idx1 = 0;
        ca_int32_t cmp_ret = 0;
        ca_vlan_key_entry_t *tmp_key;
        for (idx = 0; idx < CTC_ONU_AGGR_VLAN_MAX; idx++)
        {
            for (idx1 = 0; idx1 < CTC_ONU_AGGR_VLAN_NUM; idx1++)
            {
                cmp_ret = memcmp(
                    g_ctc_port_info[port].aggr_vlan[idx].agg_key + idx1,
                    &zero_key,
                    sizeof(oam_ctc_vlan_key_entry_t));
                if (cmp_ret)
                {
                    tmp_key = (ca_vlan_key_entry_t *)
                        (g_ctc_port_info[port].aggr_vlan[idx].agg_key + idx1);
                }
                else
                {
                    continue;
                }

                ret = ca_l2_vlan_ingress_action_delete(
                    0,
                    oam_port_to_ca_port_adapt(port),
                    *tmp_key);
                if (ret != CA_E_OK)
                {
                    printf("%s: Failed to delete l2_vlan_igr_action, idx %u\n", idx);
                }
            }

            ca_vlan_action_t vlan_act;

            memset(&vlan_act, 0x00, sizeof(ca_vlan_action_t));
            printf("del agg_dst_vlan %u\n", TAG2VID(g_ctc_port_info[port].aggr_vlan[idx].agg_dst_vlan.tag));
            ca_l2_vlan_action_set(0, TAG2VID(g_ctc_port_info[port].aggr_vlan[idx].agg_dst_vlan.tag), CA_L2_VLAN_ACTION_DIR_WAN_TO_LAN, &vlan_act);


            g_ctc_port_info[port].aggr_vlan[idx].agg_vlan_num = 0;
            memset(
                g_ctc_port_info[port].aggr_vlan + idx,
                0x00,
                sizeof(ctc_oam_agg_vlan_t));
        }
        goto ERROR_CTC_VLAN_MODE_RESET;
    }


ERROR_CTC_VLAN_MODE_RESET:
    g_ctc_port_info[port].vlan_mode = CTC_VLAN_MODE_TRANSPARENT;
    return ret;
}

oam_status ctc_oam_onu_transparent_vlan_set_adapt(
    oam_port_id_t port)
{
    oam_status ret = OAM_E_ERROR;

    OAM_ORG_DEBUG(
        "port = %d, vlan_mode: transparent\r\n",
        port);

    __PORT_CHECK(port);

    ret = _ctc_vlan_mode_reset(port);
    if (ret != OAM_E_OK)
    {
        printf(
            "%s: Failed to set vlan mode [Tansparent], port 0x%x\n",
            __FUNCTION__,
            port);
    }

    return ret;
}



oam_status ctc_oam_onu_tag_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t vlan)
{
    ca_status_t ca_ret = CA_E_ERROR;
    oam_status  ret = OAM_E_OK;
    ca_uint32_t               pvid          = 0,
                              ca_port       = 0;
    ca_port_discard_control_t port_cntrl;
    ca_vlan_port_control_t    vlan_p_cntrl;
    ca_port_id_t member_port[16] = { 0 };
    ca_port_id_t untagged_port[16] = { 0 };
    ca_uint32_t               mbr_cnt       = 0,
                              untag_cnt     = 0;
                              
    if (!g_ctc_port_info)
    {        
        ret = CA_E_INIT;
        goto ERROR_CTC_OAM_ONU_TAG_VLAN_SET_ADAPT;
    }
    

    OAM_ORG_DEBUG("port = %d, TPID = 0x%x, tag = 0x%x\r\n",
        port, vlan.tpid, vlan.tag);

    __PORT_CHECK(port);

    memset(&port_cntrl, 0, sizeof(ca_port_discard_control_t));
    memset(&vlan_p_cntrl, 0, sizeof(ca_vlan_port_control_t));

    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);
    //
    // reset vlan configuraiton on port
    //
    ret = _ctc_vlan_mode_reset(port);
    if (ret != OAM_E_OK)
    {
        printf(
            "%s: Failed to set vlan mode [Tansparent], port 0x%x\n",
            __FUNCTION__,
            port);
    }
    //
    // upstream
    //
    pvid = TAG2VID(vlan.tag);

    _ctc_add_vlan_member(pvid, ca_port, ca_port);
    _ctc_add_vlan_member(pvid, OAM_PON_PORT_ID_TO_CA_PORT(), 0);
#if 0
    ca_ret = ca_port_pvid_vlan_set ( 0, ca_port , pvid);
    if (ca_ret != CA_E_OK)
    {
//        OAM_ORG_LOG_INFO (
        printf(
               "Failed to ca_port_pvid_vlan_set, pvid %u, oam_port %u\n",
               pvid, port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_OAM_ONU_TAG_VLAN_SET_ADAPT;
    }
    printf("set pvid 0x%x on port 0x%0x\n", pvid, ca_port);
#endif
    port_cntrl.drop_priority_tag = 1;
    port_cntrl.drop_single_tag   = 1;
    port_cntrl.drop_multiple_tag = 1;

    ca_ret = ca_port_discard_control_set(0, ca_port, &port_cntrl);
    if (ca_ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_l2_discard_control_set, pvid %u, oam_port %u\n",
            pvid, port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_OAM_ONU_TAG_VLAN_SET_ADAPT;
    }
    //
    // downstream
    //
    vlan_p_cntrl.drop_unknown_vlan = 1;
    vlan_p_cntrl.egress_membership_check_enable = 1;
    vlan_p_cntrl.default_tag_add   = 1;
    vlan_p_cntrl.default_tag       = pvid;
    ca_ret = ca_l2_vlan_port_control_set(0, ca_port, &vlan_p_cntrl);
    if (ca_ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_l2_vlan_port_control_set , pvid %u, oam_port %u\n",
            pvid, port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_OAM_ONU_TAG_VLAN_SET_ADAPT;
    }


    g_ctc_port_info[port].vlan_mode = CTC_VLAN_MODE_TAG;
    memcpy(
        &g_ctc_port_info[port].def_vlan_tag,
        &vlan,
        sizeof(oam_ctc_onu_vlan_tag_t));

ERROR_CTC_OAM_ONU_TAG_VLAN_SET_ADAPT:
    return ret;
}

oam_status ctc_oam_onu_translate_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_xlate_entry *vlan)
{
    ca_status_t ca_ret = CA_E_ERROR;
    oam_status  ret = OAM_E_OK;
    ca_uint32_t      pvid        = 0,
                     ca_port     = 0;
    ca_vlan_action_t vlan_trnslt;
    
    if (!g_ctc_port_info)
    {        
        ret = CA_E_INIT;
        goto ERROR_CTC_OAM_ONU_TRANSLATE_VLAN_SET_ADAPT;
    }

    __PORT_CHECK(port);

    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);
    pvid = TAG2VID(defvlan.tag);

    //
    // reset vlan configuraiton on port
    //
    ret = _ctc_vlan_mode_reset(port);
    if (ret != OAM_E_OK)
    {
        printf(
            "%s: Failed to set vlan mode [Tansparent], port 0x%x\n",
            __FUNCTION__,
            port);
    }

    ca_port_discard_control_t port_cntrl;
    memset(&port_cntrl, 0, sizeof(ca_port_discard_control_t));
    port_cntrl.drop_priority_tag = 1;
    port_cntrl.drop_multiple_tag = 1;

    ca_ret = ca_port_discard_control_set(0, ca_port, &port_cntrl);
    if (ca_ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_l2_discard_control_set, pvid %u, oam_port %u\n",
            pvid, port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_OAM_ONU_TRANSLATE_VLAN_SET_ADAPT;
    }

    _ctc_add_vlan_member(pvid, ca_port, 0);
    _ctc_add_vlan_member(pvid, OAM_PON_PORT_ID_TO_CA_PORT(), 0);

    ca_ret = ca_port_pvid_vlan_set(0, ca_port, pvid);
    if (ca_ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_port_pvid_vlan_set, pvid %u, oam_port %u\n",
            pvid, port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_OAM_ONU_TRANSLATE_VLAN_SET_ADAPT;
    }

    ca_vlan_port_control_t    vlan_p_cntrl;
    memset(&vlan_p_cntrl, 0, sizeof(ca_vlan_port_control_t));

    vlan_p_cntrl.drop_unknown_vlan = 1;
    vlan_p_cntrl.ingress_vlan_action_enable     = 1;
    vlan_p_cntrl.egress_vlan_action_enable     = 1;
    vlan_p_cntrl.egress_membership_check_enable = 1;
    ca_ret = ca_l2_vlan_port_control_set(0, ca_port, &vlan_p_cntrl);
    if (ca_ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_l2_vlan_port_control_set , pvid %u, oam_port %u\n",
            pvid, port);
        ret = OAM_E_ERROR;

        goto ERROR_CTC_OAM_ONU_TRANSLATE_VLAN_SET_ADAPT;
    }
    memcpy(
        &g_ctc_port_info[port].def_vlan_tag,
        &defvlan,
        sizeof(oam_ctc_onu_vlan_tag_t));

    ca_vlan_key_entry_t ingr_ctag;
    ca_uint32_t idx = 0;
    for (idx = 0; idx < num; idx++)
    {
        memset(&ingr_ctag, 0, sizeof(ca_vlan_key_entry_t));
        memset(&vlan_trnslt, 0, sizeof(ca_vlan_action_t));

        ingr_ctag.key_type.outer_vid = 1;
        ingr_ctag.outer_vid = TAG2VID(vlan[idx].tag1.tag);

        vlan_trnslt.outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_SWAP;
        vlan_trnslt.new_outer_vlan = TAG2VID(vlan[idx].tag2.tag);
        vlan_trnslt.new_outer_pri  = 0xffffffff;
        vlan_trnslt.new_inner_pri  = 0xffffffff;
        vlan_trnslt.flow_id        = 0xffff;

        ca_ret = ca_l2_vlan_ingress_action_add(0, ca_port, &ingr_ctag, &vlan_trnslt);
        if (ca_ret != CA_E_OK)
        {
            OAM_ORG_LOG_INFO(
                "Failed to ca_l2_vlan_ingress_action_add, idx %u, ingrVid %u, egrVid %u, ret 0x%x\n",
                idx,
                ingr_ctag.inner_vid,
                vlan_trnslt.new_outer_vlan,
                ca_ret);
        }
        //
        // downstream
        //
        ingr_ctag.key_type.outer_vid = 1;
        ingr_ctag.outer_vid = TAG2VID(vlan[idx].tag2.tag);

        vlan_trnslt.outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_SWAP;
        vlan_trnslt.new_outer_vlan = TAG2VID(vlan[idx].tag1.tag);
        vlan_trnslt.new_inner_pri  = 0xffffffff;
        vlan_trnslt.new_outer_pri  = 0xffffffff;
        vlan_trnslt.flow_id        = 0xffff;

        ca_ret = ca_l2_vlan_egress_action_add(0, ca_port, &ingr_ctag, &vlan_trnslt);
        if (ca_ret != CA_E_OK)
        {
            OAM_ORG_LOG_INFO(
                "Failed to ca_l2_vlan_egress_action_add, idx %u, ingrVid %u, egrVid %u, ret 0x%x\n",
                idx,
                ingr_ctag.outer_vid,
                vlan_trnslt.new_outer_vlan,
                ca_ret);
        }
        _ctc_add_vlan_member(ingr_ctag.outer_vid, OAM_PON_PORT_ID_TO_CA_PORT(), 0);
        _ctc_add_vlan_member(ingr_ctag.outer_vid, ca_port, 0);
        memcpy(
            g_ctc_port_info[port].translate_key + idx,
            &ingr_ctag,
            sizeof(ca_vlan_key_entry_t));

        memcpy(
            g_ctc_port_info[port].translate_vlan + idx,
            vlan + idx,
            sizeof(oam_ctc_onu_vlan_xlate_t));
    }

    if (ret == OAM_E_OK)
    {
        g_ctc_port_info[port].translate_num = num;
        memcpy(&g_ctc_port_info[port].def_vlan_tag, &defvlan, sizeof(oam_ctc_onu_vlan_tag_t));

        g_ctc_port_info[port].vlan_mode     = CTC_VLAN_MODE_TRANSLATE;
    }

ERROR_CTC_OAM_ONU_TRANSLATE_VLAN_SET_ADAPT:

    return ret;
}


oam_status ctc_oam_onu_trunk_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    ca_status_t ca_ret = CA_E_ERROR;
    oam_status  ret = OAM_E_OK;
    ca_uint32_t ca_port = 0;

    if (!g_ctc_port_info)
    {
        ret = CA_E_INIT;
        goto ERROR_CTC_OAM_ONU_TRUNK_VLAN_SET_ADAPT;
    }
    
    __PORT_CHECK(port);

    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);
    //
    // reset vlan configuraiton on port
    //
    ret = _ctc_vlan_mode_reset(port);
    if (ret != OAM_E_OK)
    {
        printf(
            "%s: Failed to set vlan mode [Tansparent], port 0x%x\n",
            __FUNCTION__,
            port);
    }


    ca_port_discard_control_t port_cntrl;
    memset(&port_cntrl, 0, sizeof(ca_port_discard_control_t));
    port_cntrl.drop_priority_tag = 1;
//    port_cntrl.drop_untag        = 0;
    port_cntrl.drop_multiple_tag = 1;
    ca_ret = ca_port_discard_control_set(0, ca_port, &port_cntrl);
    if (ca_ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_port_discard_control_set, oam_port %u\n",
            port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_OAM_ONU_TRUNK_VLAN_SET_ADAPT;
    }

    ca_vlan_port_control_t    vlan_p_cntrl;

    memset(&vlan_p_cntrl, 0, sizeof(ca_vlan_port_control_t));

    vlan_p_cntrl.drop_unknown_vlan = 1;
    vlan_p_cntrl.ingress_membership_check_enable = 1;
    vlan_p_cntrl.egress_membership_check_enable  = 1;
    ca_ret = ca_l2_vlan_port_control_set(0, ca_port, &vlan_p_cntrl);
    if (ca_ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_l2_vlan_port_control_set , pvid %u, oam_port %u\n",
            TAG2VID(defvlan.tag), port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_OAM_ONU_TRUNK_VLAN_SET_ADAPT;
    }

    _ctc_add_vlan_member(TAG2VID(defvlan.tag), ca_port, 0);
    _ctc_add_vlan_member(TAG2VID(defvlan.tag), OAM_PON_PORT_ID_TO_CA_PORT(), 0);
    ca_ret = ca_port_pvid_vlan_set(0, ca_port, TAG2VID(defvlan.tag));
    if (ca_ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_port_pvid_vlan_set , pvid %u, oam_port %u\n",
            TAG2VID(defvlan.tag), port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_OAM_ONU_TRUNK_VLAN_SET_ADAPT;
    }

    ca_uint32_t idx = 0;
    for (idx = 0; idx < num; idx++)
    {
        _ctc_add_vlan_member(TAG2VID(vlan[idx].tag), ca_port, 0);
        _ctc_add_vlan_member(TAG2VID(vlan[idx].tag), OAM_PON_PORT_ID_TO_CA_PORT(), 0);
    }

    memcpy(
        g_ctc_port_info[port].trunk_vlan,
        vlan,
        sizeof(oam_ctc_onu_vlan_tag_t) * num);
    g_ctc_port_info[port].trunk_num = num;
    g_ctc_port_info[port].vlan_mode = CTC_VLAN_MODE_TRUNK;
    memcpy(&g_ctc_port_info[port].def_vlan_tag, &defvlan, sizeof(oam_ctc_onu_vlan_tag_t));

ERROR_CTC_OAM_ONU_TRUNK_VLAN_SET_ADAPT:
    return ret;
}

oam_status ctc_oam_onu_agg_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    ctc_oam_agg_vlan_t *vlan)
{
    ca_status_t ca_ret = CA_E_ERROR;
    oam_status  ret = OAM_E_OK;
    ca_uint32_t ca_port = 0,
                pvid    = 0;

    if (!g_ctc_port_info)
    {
        ret = CA_E_INIT;
        goto ERROR_CTC_OAM_ONU_AGG_VLAN_SET_ADAPT;
    }

    __PORT_CHECK(port);

    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);
    //
    // reset vlan configuraiton on port
    //
    ret = _ctc_vlan_mode_reset(port);
    if (ret != OAM_E_OK)
    {
        printf(
            "%s: Failed to set vlan mode [Tansparent], port 0x%x\n",
            __FUNCTION__,
            port);
    }


    ca_port_discard_control_t port_cntrl;
    memset(&port_cntrl, 0, sizeof(ca_port_discard_control_t));
    port_cntrl.drop_priority_tag = 1;
    port_cntrl.drop_multiple_tag = 1;

    ca_ret = ca_port_discard_control_set(0, ca_port, &port_cntrl);
    if (ca_ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_port_discard_control_set, oam_port %u\n",
            port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_OAM_ONU_AGG_VLAN_SET_ADAPT;
    }
    //
    // upstream
    //
    pvid = TAG2VID(defvlan.tag);

    _ctc_add_vlan_member(pvid, ca_port, ca_port);
    _ctc_add_vlan_member(pvid, OAM_PON_PORT_ID_TO_CA_PORT(), 0);
    ca_ret = ca_port_pvid_vlan_set(0, ca_port, pvid);
    if (ca_ret != CA_E_OK)
    {
        OAM_ORG_LOG_INFO(
            "Failed to ca_port_pvid_vlan_set, pvid %u, oam_port %u\n",
            pvid, port);
        ret = OAM_E_ERROR;
        goto ERROR_CTC_OAM_ONU_AGG_VLAN_SET_ADAPT;
    }
    memcpy(
        &g_ctc_port_info[port].def_vlan_tag,
        &defvlan,
        sizeof(oam_ctc_onu_vlan_tag_t));
    ca_uint32_t idx,
                idx1 = 0;
    for (idx = 0; idx < num; idx++)
    {
        g_ctc_port_info[port].aggr_vlan[idx].agg_vlan_num =
            vlan[idx].agg_vlan_num;
        memcpy(
            &g_ctc_port_info[port].aggr_vlan[idx].agg_dst_vlan,
            &vlan[idx].agg_dst_vlan,
            sizeof(oam_ctc_onu_vlan_tag_t));

        ca_vlan_key_entry_t aggr_vlan;
        ca_vlan_action_t    aggr_dst_vlan;
        for (idx1 = 0; idx1 < vlan[idx].agg_vlan_num; idx1++)
        {

            memset(&aggr_vlan, 0, sizeof(ca_vlan_key_entry_t));
            memset(&aggr_dst_vlan, 0, sizeof(ca_vlan_action_t));

            aggr_vlan.key_type.outer_vid = 1;
            aggr_vlan.outer_vid = TAG2VID(vlan[idx].agg_vlan[idx1].tag);

            aggr_dst_vlan.outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_SWAP;
            aggr_dst_vlan.new_outer_vlan = TAG2VID(vlan[idx].agg_dst_vlan.tag);
            aggr_dst_vlan.new_outer_pri  = 0xffffffff;
            aggr_dst_vlan.new_inner_pri  = 0xffffffff;
            aggr_dst_vlan.flow_id        = 0xffff;
            ca_ret = ca_l2_vlan_ingress_action_add(
                0,
                ca_port,
                &aggr_vlan,
                &aggr_dst_vlan);
            if (ca_ret != CA_E_OK)
            {
                OAM_ORG_LOG_INFO(
                    "Failed to ca_l2_vlan_ingress_action_add, idx %u, ingrVid %u, egrVid %u\n",
                    idx,
                    aggr_vlan.inner_vid,
                    aggr_dst_vlan.new_outer_vlan);
            }
            ca_vlan_action_t vlan_act;

            memset(&vlan_act, 0x00, sizeof(ca_vlan_action_t));
            vlan_act.new_outer_vlan_src = CA_L2_VLAN_NEW_VLAN_SRC_FDB;
            vlan_act.outer_vlan_cmd     = CA_L2_VLAN_TAG_ACTION_SWAP;
            ca_l2_vlan_action_set(0, TAG2VID(vlan[idx].agg_dst_vlan.tag), CA_L2_VLAN_ACTION_DIR_WAN_TO_LAN, &vlan_act);

            _ctc_add_vlan_member(aggr_dst_vlan.new_outer_vlan, OAM_PON_PORT_ID_TO_CA_PORT(), 0);
            _ctc_add_vlan_member(aggr_dst_vlan.new_outer_vlan, ca_port, 0);
            memcpy(
                g_ctc_port_info[port].aggr_vlan[idx].agg_key + idx1,
                &aggr_vlan,
                sizeof(ca_vlan_key_entry_t));

            memcpy(
                g_ctc_port_info[port].aggr_vlan[idx].agg_vlan + idx1,
                vlan[idx].agg_vlan + idx1,
                sizeof(oam_ctc_onu_vlan_tag_t));
        }
    }


    g_ctc_port_info[port].vlan_mode = CTC_VLAN_MODE_AGG;
    g_ctc_port_info[port].aggr_num  = num;
ERROR_CTC_OAM_ONU_AGG_VLAN_SET_ADAPT:
    return ret;
}


/**************** PLAT ADAPT START *********************************/

oam_status
ctc_oam_onu_opmdiag_get_adapt(
    oam_int16 *temp,
    oam_uint16 *volt,
    oam_uint16 *tx_bias,
    oam_uint16 *tx_power,
    oam_uint16 *rx_power
    )
{
    ca_status_t ret = CA_E_OK;
    opm_diag_info_t ddm_raw_data;

    memset(&ddm_raw_data, 0, sizeof(ddm_raw_data));

    ret = opm_diag_info_get(OAM_DEV_ID, OAM_PORT_ID, &ddm_raw_data);
    if (ret != CA_E_OK)
    {
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
    ca_status_t ca_ret = CA_E_ERROR;
    oam_uint8 onu_mac[OAM_MACADDR_LEN] = { 0x00, 0x13, 0x25, 0x00, 0x00, 0x01 };
    ca_port_id_t port = (CA_PORT_TYPE_EPON << 8);
    ca_device_config_tlv_t scfg;

    memset(&scfg, 0x00, sizeof(scfg));

    if (NULL == onuId)
    {
        return OAM_E_PARAM;
    }
    else if ((transId != TRANSID_MAJOR) && (transId != TRANSID_BOTH))
    {
        return OAM_E_NOT_SUPPORT;
    }

    scfg.type = CA_CFG_ID_MAC_ADDRESS;

    ca_ret = ca_device_config_tlv_get(0, &scfg);
    if (ca_ret != CA_E_OK)
    {
        ca_printf("%s: Failed to get scfg for CA_CFG_ID_MODEL_NAME\n", __FUNCTION__);
    }

    memcpy(onu_mac, scfg.value, CA_ETH_ADDR_LEN);

    if (((onuId[0] == onu_mac[0])
         && (onuId[1] == onu_mac[1])
         && (onuId[2] == onu_mac[2])
         && (onuId[3] == onu_mac[3])
         && (onuId[4] == onu_mac[4])
         && (onuId[5] == onu_mac[5]))
        ||
        ((0xff == onuId[0]) && (0xff == onuId[1]) && (0xff == onuId[2]) &&
         (0xff == onuId[3]) && (0xff == onuId[4]) && (0xff == onuId[5])))
    {
        if (g_laser_tx_power_timer != 0) /*stop previous timer */
        {
            ca_timer_del(g_laser_tx_power_timer);
            g_laser_tx_power_timer = 0;
        }

        if (0 == action) /* tx laser on */
        {
            ca_epon_port_laser_mode_set(0, port, CA_EPON_PORT_LASER_MODE_BURST);
            ca_gpio_set_value(CA_GPIO_PON_TX_POWRER_EN, 1);
            //            led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_ON);
        }
        else if (65535 == action)
        {
            //cs_gpio_onu_tx_pwr_enable(0, port, FALSE);
            ca_epon_port_laser_mode_set(0, port, CA_EPON_PORT_LASER_MODE_OFF);
            ca_gpio_set_value(CA_GPIO_PON_TX_POWRER_EN, 0);
            //            led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_OFF);
        }
        else
        {
            //cs_gpio_onu_tx_pwr_enable(0, port, FALSE);
            ca_epon_port_laser_mode_set(0, port, CA_EPON_PORT_LASER_MODE_OFF);
            ca_gpio_set_value(CA_GPIO_PON_TX_POWRER_EN, 0);
            //            led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_OFF);
            g_laser_tx_power_timer = ca_timer_add(action * 1000, __laser_tx_power_on, NULL);
        }
    }
    else
    {
        return OAM_E_PARAM;
    }

    return OAM_E_OK;
}



oam_status ctc_oam_onu_reset_set_adapt()
{
    ca_status_t ca_ret = CA_E_ERROR;

    ca_ret = ca_reset(0, CA_SYS_RESET_MODE_COLD);
    if (CA_E_OK != ca_ret)
    {
        return OAM_E_ERROR;
    }

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
    ca_status_t ca_ret = CA_E_ERROR;
    ca_boolean_t enable = 0;

    __PORT_CHECK(port);

    ca_ret = loop_detect_oper_get(OAM_UNI_PORT_ID_TO_CA_PORT(port), &enable);
    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

    *status = enable ? OAM_CTC_ONU_LOOPDETECT_ACTIVED : OAM_CTC_ONU_LOOPDETECT_DEACTIVED;

    return OAM_E_OK;
}

oam_status ctc_oam_loop_detect_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status)
{
    ca_status_t ca_ret = CA_E_ERROR;
    oam_uint32 enable = 0;
#ifdef ZTE_SPECIFIC
    cs_db_eth_port_t   port_info;
#endif/*END_ZTE_SPECIFIC*/

    enable = (status == OAM_CTC_ONU_LOOPDETECT_ACTIVED) ? 1 : 0;

    if (enable)
    {
        ca_ret = loop_detect_enable(OAM_UNI_PORT_ID_TO_CA_PORT(port));
    }
    else
    {
        ca_ret = loop_detect_enable(OAM_UNI_PORT_ID_TO_CA_PORT(port));
    }

    if (ca_ret != CA_E_OK)
    {
        return OAM_E_ERROR;
    }

#ifdef ZTE_SPECIFIC

    if (port <= DB_MAX_PORT && port > 0)
    {
        memset(&port_info, 0x00, sizeof(port_info));
        port_info.port_id = port;
        port_info.loop_detect = status == 2 ? 1 : 0;
        cs_db_entry_mod(TB_ETH_PORT, (void *)&port_info, sizeof(port_info),
            column_bit(TB_ETH_PORT, "loop_detect"));
    }

#endif/*END_ZTE_SPECIFIC*/
    return 0;
}


oam_status ctc_oam_disable_looped_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status)
{
    oam_uint32 block_mode = 0;

    block_mode = (status == OAM_CTC_ONU_DISABLE_LOOPED_ACTIVED) ? 1 : 0;

    loop_detect_block_enable(block_mode);

    return OAM_E_OK;
}


oam_status ctc_oam_onu_classification_get_adapt(
    oam_port_id_t port,
    oam_uint32 *num,
    oam_uint8 *data)
{
    oam_ctc_onu_classification_struct *pData = (oam_ctc_onu_classification_struct *)
        data;
    oam_ctc_onu_class_fselect_t       *pEntry;
    oam_ctc_onu_class_fselect_v6_t    *pEntry6;
    ctc_cls_node_t *cur = NULL;

    ctc_classification_rule_t   cls[CLASS_RULES_MAX];
    oam_uint8                  cls_num = 0;
    oam_uint32                         i           = 0,
                                       j           = 0,
                                       len         = 0,
                                       total_len   = 0;
    oam_uint8                  ctc_version = 0;

    if ((num == NULL) || (data == NULL))
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n",  port);

    __PORT_CHECK(port);

    memset(cls, 0x00, CLASS_RULES_MAX * sizeof(ctc_classification_rule_t));

    oam_ctc_version_get(oam_llid_get(), &ctc_version);

    *num = g_port_cls_cfg[port].rule_cnt;
    cur = g_port_cls_cfg[port].head;

    while (cur)
    {
        pData = (oam_ctc_onu_classification_struct *)(data + total_len);
        len = 3;
        total_len += sizeof(oam_ctc_onu_classification_struct) - 1;
        pData->precedence = cur->cls_rule.precedence;
        pData->queueMapped = cur->cls_rule.queueMapped;
        pData->priMark = cur->cls_rule.priMark;
        pData->entries = cur->cls_rule.entries;

        for (j = 0; j < pData->entries; j++)
        {
            if ((cur->cls_rule.fselect[j].fieldSelect >= CLASS_RULES_FSELECT_IPVER) &&
                (ctc_version == 0x30))
            {
                len += sizeof(oam_ctc_onu_class_fselect_v6_t);
                pEntry6 = (oam_ctc_onu_class_fselect_v6_t *)(data + total_len);
                total_len += sizeof(oam_ctc_onu_class_fselect_v6_t);
                pEntry6->fieldSelect = cur->cls_rule.fselect[j].fieldSelect;
                pEntry6->validationOper = cur->cls_rule.fselect[j].validationOper;
                memcpy(pEntry6->matchValue, cur->cls_rule.fselect[j].matchValue, CLASS_MATCH_VAL_LEN);
            }
            else
            {
                len += sizeof(oam_ctc_onu_class_fselect_t);
                pEntry = (oam_ctc_onu_class_fselect_t *)(data + total_len);
                total_len += sizeof(oam_ctc_onu_class_fselect_t);
                pEntry->fieldSelect = cur->cls_rule.fselect[j].fieldSelect;
                pEntry->validationOper = cur->cls_rule.fselect[j].validationOper;
                memcpy(pEntry->matchValue,
                    cur->cls_rule.fselect[j].matchValue + (CLASS_MATCH_VAL_LEN - CTC_CLASS_MATCH_VAL_LEN),
                    CTC_CLASS_MATCH_VAL_LEN);
            }
        }

        pData->len = len;
        cur = cur->nxt;
    }

    return OAM_E_OK;
}

ca_status_t _ctc_add_cls_fld_dmac(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_DMAC;
    }

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_DMAC;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->l2             = 1;
    key_mask->l2_mask.mac_da = 1;
    memcpy(
        key->l2.mac_da.mac_max,
        fld.matchValue,
        sizeof(ca_mac_addr_t));
    memcpy(
        key->l2.mac_da.mac_min,
        fld.matchValue,
        sizeof(ca_mac_addr_t));

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->l2             = 0;
        key_mask->l2_mask.mac_da = 0;
        memset(
            key->l2.mac_da.mac_max,
            0x00,
            sizeof(ca_mac_addr_t));
        memset(
            key->l2.mac_da.mac_min,
            0x00,
            sizeof(ca_mac_addr_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "mac_max: %02x:%02x:%02x:%02x:%02x:%02x\n",
            key->l2.mac_da.mac_max[0],
            key->l2.mac_da.mac_max[1],
            key->l2.mac_da.mac_max[2],
            key->l2.mac_da.mac_max[3],
            key->l2.mac_da.mac_max[4],
            key->l2.mac_da.mac_max[5]);
        printf(
            "\rmac_min: %02x:%02x:%02x:%02x:%02x:%02x\n",
            key->l2.mac_da.mac_min[0],
            key->l2.mac_da.mac_min[1],
            key->l2.mac_da.mac_min[2],
            key->l2.mac_da.mac_min[3],
            key->l2.mac_da.mac_min[4],
            key->l2.mac_da.mac_min[5]);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        memset(key->l2.mac_da.mac_min, 0x00, sizeof(ca_mac_addr_t));
        memset(key->l2.mac_da.mac_max, 0xFF, sizeof(ca_mac_addr_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        memset(key->l2.mac_da.mac_min, 0x00, sizeof(ca_mac_addr_t));
        memset(key->l2.mac_da.mac_max, 0xFF, sizeof(ca_mac_addr_t));

        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_DMAC:
    return ret;
}

oam_status _ctc_add_cls_fld_smac(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_SMAC;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_SMAC;
        //break;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->l2             = 1;
    key_mask->l2_mask.mac_sa = 1;
    memcpy(
        key->l2.mac_sa.mac_max,
        fld.matchValue,
        sizeof(ca_mac_addr_t));
    memcpy(
        key->l2.mac_sa.mac_min,
        fld.matchValue,
        sizeof(ca_mac_addr_t));

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->l2             = 0;
        key_mask->l2_mask.mac_sa = 0;
        memset(
            key->l2.mac_sa.mac_max,
            0x00,
            sizeof(ca_mac_addr_t));
        memset(
            key->l2.mac_sa.mac_min,
            0x00,
            sizeof(ca_mac_addr_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "mac_max: %02x:%02x:%02x:%02x:%02x:%02x\n",
            key->l2.mac_sa.mac_max[0],
            key->l2.mac_sa.mac_max[1],
            key->l2.mac_sa.mac_max[2],
            key->l2.mac_sa.mac_max[3],
            key->l2.mac_sa.mac_max[4],
            key->l2.mac_sa.mac_max[5]);
        printf(
            "\rmac_min: %02x:%02x:%02x:%02x:%02x:%02x\n",
            key->l2.mac_sa.mac_min[0],
            key->l2.mac_sa.mac_min[1],
            key->l2.mac_sa.mac_min[2],
            key->l2.mac_sa.mac_min[3],
            key->l2.mac_sa.mac_min[4],
            key->l2.mac_sa.mac_min[5]);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        memset(key->l2.mac_sa.mac_min, 0x00, sizeof(ca_mac_addr_t));
        memset(key->l2.mac_sa.mac_max, 0xFF, sizeof(ca_mac_addr_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        memset(key->l2.mac_sa.mac_min, 0x00, sizeof(ca_mac_addr_t));
        memset(key->l2.mac_sa.mac_max, 0xFF, sizeof(ca_mac_addr_t));

        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_SMAC:
    return ret;
}

oam_status _ctc_add_cls_fld_vlanpri(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_VLANPRI;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_VLANPRI;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->l2             = 1;
    key_mask->l2_mask.vlan_otag = 1;
    key_mask->l2_mask.vlan_otag_mask.pri = 1;
    key->l2.vlan_otag.vlan_min.pri = fld.matchValue[CTC_CLASS_MATCH_VAL_LEN - 1];
    key->l2.vlan_otag.vlan_max.pri = fld.matchValue[CTC_CLASS_MATCH_VAL_LEN - 1];

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->l2                = 0;
        key_mask->l2_mask.vlan_otag = 0;
        key_mask->l2_mask.vlan_otag_mask.pri = 0;
        key->l2.vlan_otag.vlan_min.pri = 0;
        key->l2.vlan_otag.vlan_max.pri = 0;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "pri_max: %02x, pri_min: %02x\n",
            key->l2.vlan_otag.vlan_max.pri,
            key->l2.vlan_otag.vlan_min.pri);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        key->l2.vlan_otag.vlan_min.pri = 0;
        key->l2.vlan_otag.vlan_max.pri = 7;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key->l2.vlan_otag.vlan_min.pri = 0;
        key->l2.vlan_otag.vlan_max.pri = 7;
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_VLANPRI:
    return ret;
}

oam_status _ctc_add_cls_fld_vlanid(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_VLANVID;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_VLANVID;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->l2                = 1;
    key_mask->l2_mask.vlan_otag = 1;
    key_mask->l2_mask.vlan_otag_mask.vid = 1;
    memcpy(
        &key->l2.vlan_otag.vlan_min.vid,
        fld.matchValue,
        sizeof(ca_uint16_t));
    memcpy(
        &key->l2.vlan_otag.vlan_max.vid,
        fld.matchValue,
        sizeof(ca_uint16_t));

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->l2                = 0;
        key_mask->l2_mask.vlan_otag = 0;
        key_mask->l2_mask.vlan_otag_mask.vid = 0;
        memset(
            &key->l2.vlan_otag.vlan_min.vid,
            0x00,
            sizeof(ca_uint16_t));
        memset(
            &key->l2.vlan_otag.vlan_max.vid,
            0x00,
            sizeof(ca_uint16_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "vlan_max: 0x%02x, vlan_min: 0x%02x\n",
            key->l2.vlan_otag.vlan_max.vid,
            key->l2.vlan_otag.vlan_min.vid);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        key->l2.vlan_otag.vlan_min.vid = 0;
        key->l2.vlan_otag.vlan_max.vid = 4095;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key->l2.vlan_otag.vlan_min.vid = 0;
        key->l2.vlan_otag.vlan_max.vid = 4095;
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_VLANVID:
    return ret;
}

oam_status _ctc_add_cls_fld_ethtype(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_ETHTYPE;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_ETHTYPE;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->l2                = 1;
    key_mask->l2_mask.ethertype = 1;
    memcpy(
        &key->l2.ethertype,
        fld.matchValue + (CTC_CLASS_MATCH_VAL_LEN - 2),
        sizeof(ca_uint16_t));

    key->l2.ethertype = ntohl(key->l2.ethertype);

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->l2                = 0;
        key_mask->l2_mask.ethertype = 0;
        memset(
            &key->l2.ethertype,
            0x00,
            sizeof(ca_uint16_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "ethertype: 0x%02x\n",
            key->l2.ethertype);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {}
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_ETHTYPE:
    return ret;
}

oam_status _ctc_add_cls_fld_destip(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_DESTIP;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_DESTIP;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->ip            = 1;
    key_mask->ip_mask.ip_version = 1;
    key_mask->ip_mask.ip_da = 1;
    key->ip.ip_version = CA_IPV4;
    key->ip.ip_da.afi  = CA_IPV4;
    key->ip.ip_da.addr_len = 4;
    memcpy(
        &key->ip.ip_da.ip_addr.ipv4_addr,
        fld.matchValue + (CTC_CLASS_MATCH_VAL_LEN - 4),
        sizeof(ca_uint32_t));

    key->ip.ip_da.ip_addr.ipv4_addr = ntohl(key->ip.ip_da.ip_addr.ipv4_addr);

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->ip            = 0;
        key_mask->ip_mask.ip_da = 0;
        key_mask->ip_mask.ip_version = 0;
        memset(&key->ip, 0x00, sizeof(ca_classifier_ip_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "dest ip: 0x%04x\n",
            key->ip.ip_da.ip_addr.ipv4_addr);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        key_mask->ip_mask.ip_da_max = 1;
        key->ip.ip_da.ip_addr.ipv4_addr = 0x00;
        key->ip.ip_da_max.ip_addr.ipv4_addr = 0xFFFFFFFF;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key_mask->ip_mask.ip_da_max = 1;
        key->ip.ip_da.ip_addr.ipv4_addr = 0x00;
        key->ip.ip_da_max.ip_addr.ipv4_addr = 0xFFFFFFFF;
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_DESTIP:
    return ret;
}


oam_status _ctc_add_cls_fld_srcip(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_SRCIP;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_SRCIP;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->ip            = 1;
    key_mask->ip_mask.ip_sa = 1;
    key_mask->ip_mask.ip_version = 1;
    key->ip.ip_version = CA_IPV4;
    key->ip.ip_sa.afi = CA_IPV4;
    key->ip.ip_sa.addr_len = 4;
    memcpy(
        &key->ip.ip_sa.ip_addr.ipv4_addr,
        fld.matchValue + (CTC_CLASS_MATCH_VAL_LEN - 4),
        sizeof(ca_uint32_t));

    key->ip.ip_sa.ip_addr.ipv4_addr = ntohl(key->ip.ip_sa.ip_addr.ipv4_addr);

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->ip            = 0;
        key_mask->ip_mask.ip_sa = 0;
        key_mask->ip_mask.ip_version = 0;

        memset(&key->ip, 0x00, sizeof(ca_classifier_ip_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "src ip: %x\n",
            key->ip.ip_sa.ip_addr.ipv4_addr);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        key_mask->ip_mask.ip_sa_max = 1;
        key->ip.ip_sa.ip_addr.ipv4_addr = 0x00;
        key->ip.ip_sa_max.ip_addr.ipv4_addr = 0xFFFFFFFF;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key_mask->ip_mask.ip_sa_max = 1;
        key->ip.ip_sa.ip_addr.ipv4_addr = 0x00;
        key->ip.ip_sa_max.ip_addr.ipv4_addr = 0xFFFFFFFF;
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_SRCIP:
    return ret;
}


oam_status _ctc_add_cls_fld_ipprot(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_IPPROT;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_IPPROT;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->ip                  = 1;
    key_mask->ip_mask.ip_protocol = 1;
    key_mask->ip_mask.ip_version  = 1;
    key->ip.ip_version = CA_IPV4;
    key->ip.ip_protocol = fld.matchValue[CTC_CLASS_MATCH_VAL_LEN - 1];

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->ip            = 0;
        key_mask->ip_mask.ip_protocol = 0;
        key_mask->ip_mask.ip_version  = 0;

        memset(&key->ip, 0x00, sizeof(ca_classifier_ip_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "ip prot: 0x%02x\n",
            key->ip.ip_protocol);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {}
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {}


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_IPPROT:
    return ret;
}

oam_status _ctc_add_cls_fld_ipdscp(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_IPDSCP;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_IPDSCP;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->ip                  = 1;
    key_mask->ip_mask.dscp        = 1;
    key_mask->ip_mask.ip_version  = 1;
    key->ip.ip_version = CA_IPV4;
    key->ip.dscp       = fld.matchValue[CTC_CLASS_MATCH_VAL_LEN - 1];

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->ip            = 0;
        key_mask->ip_mask.dscp  = 0;
        key_mask->ip_mask.ip_version  = 0;

        memset(&key->ip, 0x00, sizeof(ca_classifier_ip_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "ip dscp: 0x%02x\n",
            key->ip.dscp);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {}
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {}


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_IPDSCP:
    return ret;
}

oam_status _ctc_add_cls_fld_ip6dscp(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_IP6DSCP;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_IP6DSCP;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->ip                  = 1;
    key_mask->ip_mask.dscp        = 1;
    key_mask->ip_mask.ip_version  = 1;
    key->ip.ip_version = CA_IPV6;
    key->ip.dscp       = fld.matchValue[CTC_CLASS_MATCH_VAL_LEN - 1];

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->ip            = 0;
        key_mask->ip_mask.dscp  = 0;
        key_mask->ip_mask.ip_version  = 0;

        memset(&key->ip, 0x00, sizeof(ca_classifier_ip_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "ip6 dscp: 0x%02x\n",
            key->ip.dscp);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {}
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {}


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_IP6DSCP:
    return ret;
}


oam_status _ctc_add_cls_fld_l4sport(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_L4SPORT;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_L4SPORT;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->l4                  = 1;
    key_mask->l4_mask.l4_valid    = 1;
    key_mask->l4_mask.src_port    = 1;
    key->l4.l4_valid   = 1;
    memcpy(
        &key->l4.src_port.min,
        fld.matchValue + (CTC_CLASS_MATCH_VAL_LEN - 4),
        sizeof(ca_uint32_t));
    key->l4.src_port.min = ntohl(key->l4.src_port.min);
    memcpy(
        &key->l4.src_port.max,
        fld.matchValue + (CTC_CLASS_MATCH_VAL_LEN - 4),
        sizeof(ca_uint32_t));
    key->l4.src_port.max = ntohl(key->l4.src_port.max);

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->l4                  = 0;
        key_mask->l4_mask.l4_valid    = 0;
        key_mask->l4_mask.src_port    = 0;
        key->l4.l4_valid   = 0;
        memset(
            &key->l4.src_port.min,
            0x00,
            sizeof(ca_uint32_t));
        memset(
            &key->l4.src_port.max,
            0x00,
            sizeof(ca_uint32_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "l4 src port: 0x%04x\n",
            key->l4.src_port.min);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        key->l4.src_port.min = 0x0000;
        key->l4.src_port.max = 0xffff;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key->l4.src_port.min = 0x0000;
        key->l4.src_port.max = 0xffff;
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_L4SPORT:
    return ret;
}


oam_status _ctc_add_cls_fld_l4dport(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_L4DPORT;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_L4DPORT;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->l4                  = 1;
    key_mask->l4_mask.l4_valid    = 1;
    key_mask->l4_mask.dst_port    = 1;
    key->l4.l4_valid   = 1;
    memcpy(
        &key->l4.dst_port.min,
        fld.matchValue + (CTC_CLASS_MATCH_VAL_LEN - 4),
        sizeof(ca_uint32_t));
    key->l4.dst_port.min = ntohl(key->l4.dst_port.min);
    memcpy(
        &key->l4.dst_port.max,
        fld.matchValue + (CTC_CLASS_MATCH_VAL_LEN - 4),
        sizeof(ca_uint32_t));
    key->l4.dst_port.max = ntohl(key->l4.dst_port.max);

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->l4                  = 0;
        key_mask->l4_mask.l4_valid    = 0;
        key_mask->l4_mask.dst_port    = 0;
        key->l4.l4_valid   = 0;
        memset(
            &key->l4.dst_port.min,
            0x00,
            sizeof(ca_uint32_t));
        memset(
            &key->l4.dst_port.max,
            0x00,
            sizeof(ca_uint32_t));
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "l4 src port: 0x%04x\n",
            key->l4.dst_port.min);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        key->l4.dst_port.min = 0x0000;
        key->l4.dst_port.max = 0xffff;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key->l4.dst_port.min = 0x0000;
        key->l4.dst_port.max = 0xffff;
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_L4DPORT:
    return ret;
}

oam_status _ctc_add_cls_fld_ipver(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_IPVER;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_IPVER;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->ip                  = 1;
    key_mask->ip_mask.ip_valid    = 1;
    key_mask->ip_mask.ip_version  = 1;
    key->ip.ip_valid              = 1;
    key->ip.ip_version            = fld.matchValue[0];

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->ip                  = 0;
        key_mask->ip_mask.ip_valid    = 0;
        key_mask->ip_mask.ip_version  = 0;
        key->ip.ip_valid              = 0;
        key->ip.ip_version            = 0;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "ip ver: 0x%02x\n",
            key->ip.ip_version);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        key_mask->ip_mask.ip_version  = 0;
        key->ip.ip_version            = 0;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key_mask->ip_mask.ip_version  = 0;
        key->ip.ip_version            = 0;
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_IPVER:
    return ret;
}

oam_status _ctc_add_cls_fld_ip6flwlbl(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_IP6FLWLBL;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_IP6FLWLBL;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->ip                  = 1;
    key_mask->ip_mask.ip_valid    = 1;
    key_mask->ip_mask.ip_version  = 1;
    key_mask->ip_mask.flow_label  = 1;
    key->ip.ip_valid              = 1;
    key->ip.ip_version            = CA_IPV6;
    memcpy(
        &key->ip.flow_label,
        fld.matchValue,
        sizeof(ca_uint32_t));

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->ip                  = 0;
        key_mask->ip_mask.ip_valid    = 0;
        key_mask->ip_mask.ip_version  = 0;
        key_mask->ip_mask.flow_label  = 0;
        key->ip.ip_valid              = 0;
        key->ip.ip_version            = 0;
        key->ip.flow_label            = 0;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf(
            "ip6 flw lable: 0x%04x\n",
            key->ip.flow_label);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {}
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key->ip.ip_version  = CA_IPV6;
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_IP6FLWLBL:
    return ret;
}

oam_status _ctc_add_cls_fld_destip6(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_DESTIP6;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_DESTIP6;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->ip                  = 1;
    key_mask->ip_mask.ip_valid    = 1;
    key_mask->ip_mask.ip_version  = 1;
    key_mask->ip_mask.ip_da       = 1;
    key->ip.ip_valid              = 1;
    key->ip.ip_version            = CA_IPV6;
    key->ip.ip_da.afi             = CA_IPV6;
    key->ip.ip_da.addr_len        = 16;
    memcpy(
        &key->ip.ip_da.ip_addr.ipv6_addr,
        fld.matchValue,
        sizeof(ca_uint32_t) * 4);

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->ip                  = 0;
        key_mask->ip_mask.ip_valid    = 0;
        key_mask->ip_mask.ip_version  = 0;
        key_mask->ip_mask.ip_da       = 0;
        key->ip.ip_valid              = 0;
        key->ip.ip_version            = 0;
        key->ip.ip_da.afi             = 0;
        key->ip.ip_da.addr_len        = 0;
        memset(
            &key->ip.ip_da.ip_addr.ipv6_addr,
            0x00,
            sizeof(ca_uint32_t) * 4);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf("ip6 dst: ");
        ca_uint32_t idx = 0;
        for (idx = 0; idx < 16; idx++)
        {
            printf("%02X");
            if (idx < 0xF)
            {
                printf(": ");
            }
        }
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        key_mask->ip_mask.ip_da       = 0;
        key->ip.ip_da.afi             = 0;
        key->ip.ip_da.addr_len        = 0;
        memset(
            &key->ip.ip_da.ip_addr.ipv6_addr,
            0x00,
            sizeof(ca_uint32_t) * 4);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key_mask->ip_mask.ip_da       = 0;
        key->ip.ip_da.afi             = 0;
        key->ip.ip_da.addr_len        = 0;
        memset(
            &key->ip.ip_da.ip_addr.ipv6_addr,
            0x00,
            sizeof(ca_uint32_t) * 4);

        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_DESTIP6:
    return ret;
}

oam_status _ctc_add_cls_fld_srcip6(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_SRCIP6;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_SRCIP6;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->ip                  = 1;
    key_mask->ip_mask.ip_valid    = 1;
    key_mask->ip_mask.ip_version  = 1;
    key_mask->ip_mask.ip_sa       = 1;
    key->ip.ip_valid              = 1;
    key->ip.ip_version            = CA_IPV6;
    key->ip.ip_sa.afi             = CA_IPV6;
    key->ip.ip_sa.addr_len        = 16;
    memcpy(
        &key->ip.ip_sa.ip_addr.ipv6_addr,
        fld.matchValue,
        sizeof(ca_uint32_t) * 4);

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->ip                  = 0;
        key_mask->ip_mask.ip_valid    = 0;
        key_mask->ip_mask.ip_version  = 0;
        key_mask->ip_mask.ip_sa       = 0;
        key->ip.ip_valid              = 0;
        key->ip.ip_version            = 0;
        key->ip.ip_sa.afi             = 0;
        key->ip.ip_sa.addr_len        = 0;
        memset(
            &key->ip.ip_sa.ip_addr.ipv6_addr,
            0x00,
            sizeof(ca_uint32_t) * 4);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf("ip6 dst: ");
        ca_uint32_t idx = 0;
        for (idx = 0; idx < 16; idx++)
        {
            printf("%02X");
            if (idx < 0xF)
            {
                printf(": ");
            }
        }
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        key_mask->ip_mask.ip_sa       = 0;
        key->ip.ip_sa.afi             = 0;
        key->ip.ip_sa.addr_len        = 0;
        memset(
            &key->ip.ip_sa.ip_addr.ipv6_addr,
            0x00,
            sizeof(ca_uint32_t) * 4);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key_mask->ip_mask.ip_sa       = 0;
        key->ip.ip_sa.afi             = 0;
        key->ip.ip_sa.addr_len        = 0;
        memset(
            &key->ip.ip_sa.ip_addr.ipv6_addr,
            0x00,
            sizeof(ca_uint32_t) * 4);

        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_SRCIP6:
    return ret;
}

oam_status _ctc_add_cls_fld_nxthdr(
    oam_ctc_onu_class_fselect_t fld,
    ca_classifier_key_t *key,
    ca_classifier_key_mask_t *key_mask,
    ca_classifier_action_t *cls_action)
{
    oam_status ret = OAM_E_ERROR;

    if (!key || !key_mask || !cls_action)
    {
        goto ERROR_CTC_ADD_CLS_FLD_NXTHDR;
    }


    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NEVER)
    {
        goto ERROR_CTC_ADD_CLS_FLD_NXTHDR;
    }

    cls_action->forward = CA_CLASSIFIER_FORWARD_PORT;
    key_mask->ip                  = 1;
    key_mask->ip_mask.ip_valid    = 1;
    key_mask->ip_mask.ip_version  = 1;
    key_mask->ip_mask.ext_header  = 1;
    key->ip.ip_valid              = 1;
    key->ip.ip_version            = CA_IPV6;
    memcpy(
        &key->ip.ext_header,
        fld.matchValue,
        sizeof(ca_uint32_t));

    if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_ALWAYS)
    {
        key_mask->ip                  = 0;
        key_mask->ip_mask.ip_valid    = 0;
        key_mask->ip_mask.ip_version  = 0;
        key_mask->ip_mask.ext_header  = 0;
        key->ip.ip_valid              = 0;
        key->ip.ip_version            = 0;
        key->ip.ext_header            = 0;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EQL)
    {
        printf("ip6 next hdr: 0x%04x", key->ip.ext_header);
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EQL)
    {
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_EXIST)
    {
        key_mask->ip_mask.ext_header  = 0;
        key->ip.ext_header            = 0;
    }
    else if (fld.validationOper == CTC_CLASS_VALIDATION_OPER_NOT_EXIST)
    {
        key_mask->ip_mask.ext_header  = 0;
        key->ip.ext_header            = 0;
        cls_action->forward = CA_CLASSIFIER_FORWARD_DENY;
    }


    if (cls_action->forward == CA_CLASSIFIER_FORWARD_PORT)
    {
        cls_action->dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PON_PORT);
    }

    ret = OAM_E_OK;

ERROR_CTC_ADD_CLS_FLD_NXTHDR:
    return ret;
}

ca_status_t _ctc_cls_node_add(
    ca_uint32_t          port,
    ctc_classification_rule_t *cls_rule,
    ctc_cls_node_t **cur_node)
{
    ca_status_t ret = CA_E_ERROR;
    ca_uint32_t idx;
    ctc_cls_node_t* node = NULL,
                    *cur = NULL,
                    *pre = NULL;

    if (!cls_rule)
    {
        goto ERROR_CTC_CLS_NODE_ADD;
    }

    node = (ctc_cls_node_t *)malloc(sizeof(ctc_cls_node_t));
    if (!node)
    {
        goto ERROR_CTC_CLS_NODE_ADD;
    }

    memcpy(&node->cls_rule, cls_rule, sizeof(ctc_classification_rule_t));
    node->cls_idx = -1;
    node->nxt     = NULL;
    *cur_node = node;

    if (!g_port_cls_cfg[port].head)
    {
        g_port_cls_cfg[port].head = node;
        g_port_cls_cfg[port].tail = node;
        g_port_cls_cfg[port].rule_cnt++;
    }

    cur = g_port_cls_cfg[port].head;
    pre = g_port_cls_cfg[port].head;
    //
    // if cur == node, this node is head.
    //
    while (cur != node)
    {
        if (cur->cls_rule.precedence >= node->cls_rule.precedence)
        {
            if (cur == g_port_cls_cfg[port].head)
            {
                g_port_cls_cfg[port].head = node;
            }
            else
            {
                pre->nxt = node;
            }

            node->nxt = cur;

            if (cur->cls_rule.precedence == node->cls_rule.precedence)
            {
                cur->cls_rule.precedence++;
            }

            ctc_cls_node_t *node1 = cur;
            //
            // check precedence of the others
            //
            while (node1)
            {
                if (node1->nxt &&
                    node1->nxt->cls_rule.precedence == node1->cls_rule.precedence)
                {
                    node1->nxt->cls_rule.precedence++;
                }
                node1 = node1->nxt;
            }
            g_port_cls_cfg[port].rule_cnt++;
            OAM_ORG_DEBUG(
                "Add pre rule_cnt %u, port %u\r\n",
                g_port_cls_cfg[port].rule_cnt,
                port);
            break;
        }
        else if (!cur->nxt)
        {
            cur->nxt = node;
            node->nxt = NULL;
            g_port_cls_cfg[port].tail = node;
            g_port_cls_cfg[port].rule_cnt++;
            OAM_ORG_DEBUG(
                "add after rule rule_cnt %u, port %u\r\n",
                g_port_cls_cfg[port].rule_cnt,
                port);
            break;
        }

        if (pre != cur)
        {
            pre = cur;
        }

        cur = cur->nxt;
    }

    if (g_port_cls_cfg[port].rule_cnt > CA_PORT_CLS_RULE_MAX)
    {
        // find previous of tail
        cur = g_port_cls_cfg[port].head;
        while (cur->nxt)
        {
            if (pre != cur)
            {
                pre = cur;
            }

            cur = cur->nxt;
        }

        // remove tail
        ret = ca_classifier_rule_delete(0, g_port_cls_cfg[port].tail->cls_idx);
        if (ret != CA_E_OK)
        {
            OAM_ORG_DEBUG(
                "Faile to delete cls rule of tail, cls_idx %d\r\n",
                g_port_cls_cfg[port].tail->cls_idx);
        }

        OAM_ORG_DEBUG(
            "delete cls_idx %u rule_cnt %u, port %u\r\n",
            g_port_cls_cfg[port].tail->cls_idx,
            g_port_cls_cfg[port].rule_cnt,
            port);
        free(g_port_cls_cfg[port].tail);
        g_port_cls_cfg[port].tail = pre;
        pre->nxt = NULL;
        g_port_cls_cfg[port].rule_cnt--;
    }

    idx = 0;
    cur = g_port_cls_cfg[port].head;
    while (cur)
    {
        OAM_ORG_DEBUG(
            "port %u cls rule %u, pre %u\r\n",
            port, idx, cur->cls_rule.precedence);
        idx++;
        cur = cur->nxt;
    }

    ret = CA_E_OK;

ERROR_CTC_CLS_NODE_ADD:
    return ret;
}

ca_status_t 
_ctc_add_cls(
    ca_uint32_t                port,
    ctc_classification_rule_t* cls)
{
    ca_status_t ret = CA_E_ERROR;
    oam_uint32               i          = 0,
                             j          = 0,
                             ii         = 0;
    ca_classifier_key_t      key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t   cls_action;
    ctc_cls_node_t *cur_node = NULL;

    if (!cls)
    {
        ret = CA_E_PARAM;
        goto ERROR_CTC_ADD_CLS;
    }

    memset(&key, 0, sizeof(ca_classifier_key_t));
    memset(&key_mask, 0, sizeof(ca_classifier_key_mask_t));
    memset(&cls_action, 0, sizeof(ca_classifier_action_t));

    OAM_ORG_DEBUG("oam port 0x%x\r\n", port);

    ret = _ctc_cls_node_add(port, cls, &cur_node);
    if (ret != CA_E_OK)
    {
        OAM_ORG_DEBUG("Failed to add cls rule on port %u\r\n", port);
    }

    key_mask.src_port = 1;
    key.src_port      = OAM_UNI_PORT_ID_TO_CA_PORT(port);
    for (j = 0; j < cls->entries && j < CLASS_FIELD_SELECT_MAX; j++)
    {
        printf("cls field 0x%x\n", cls->fselect[j].fieldSelect);
        if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_DA_MAC)
        {
            ret = _ctc_add_cls_fld_dmac(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_SA_MAC)
        {
            ret = _ctc_add_cls_fld_smac(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_VLAN_PRI)
        {
            ret = _ctc_add_cls_fld_vlanpri(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_VLAN_ID)
        {
            ret = _ctc_add_cls_fld_vlanid(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_ETH_TYPE)
        {
            ret = _ctc_add_cls_fld_ethtype(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_DEST_IP)
        {
            ret = _ctc_add_cls_fld_destip(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_SRC_IP)
        {
            ret = _ctc_add_cls_fld_srcip(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_IP)
        {
            ret = _ctc_add_cls_fld_ipprot(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_IPV4_DSCP)
        {
            ret = _ctc_add_cls_fld_ipdscp(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_IPV6_DSCP)
        {
            ret = _ctc_add_cls_fld_ip6dscp(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_L4_DST_PORT)
        {
            ret = _ctc_add_cls_fld_l4dport(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_L4_SRC_PORT)
        {
            ret = _ctc_add_cls_fld_l4sport(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_IPV)
        {
            ret = _ctc_add_cls_fld_ipver(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_IPV6_FLOW_LABEL)
        {
            ret = _ctc_add_cls_fld_ip6flwlbl(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_DST_IPV6)
        {
            ret = _ctc_add_cls_fld_destip6(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_SRC_IPV6)
        {
            ret = _ctc_add_cls_fld_srcip6(cls->fselect[j], &key, &key_mask, &cls_action);
        }
        else if (cls->fselect[j].fieldSelect == CTC_CLASS_RULES_FSELECT_NEXT_HEADER)
        {
            ret = _ctc_add_cls_fld_nxthdr(cls->fselect[j], &key, &key_mask, &cls_action);
        }

        if (ret != CA_E_OK)
        {
            goto ERROR_CTC_ADD_CLS;
        }
    }

    cls_action.forward = CA_CLASSIFIER_FORWARD_FE;
    cls_action.options.masks.priority = 1;
    cls_action.options.priority       = cls->queueMapped;
    if (cls->priMark != 0xFF)
    {
        cls_action.options.masks.outer_dot1p    = 1;
        cls_action.options.outer_dot1p          = cls->priMark;
    }

    ret = ca_classifier_rule_add(
        0,
        3,
        &key, 
        &key_mask,
        &cls_action,
        &cur_node->cls_idx);
    if (ret != CA_E_OK)
    {
        printf("%s: Failed to add cls rules\n", __FUNCTION__);
    }
    else
    {
        printf(
            "%s: add cls rules, pri 0x%x, cls idx 0x%x\r\n",
            __FUNCTION__,
            cls->precedence,
            cur_node->cls_idx);
    }

ERROR_CTC_ADD_CLS:
    return ret;
}

ca_status_t _ctc_del_cls(
    ca_uint32_t port,
    ca_uint32_t num,
    ca_uint8_t *precendence)
{
    ca_status_t ret = CA_E_ERROR;
    ca_uint32_t idx = 0;
    ctc_cls_node_t *cur = g_port_cls_cfg[port].head;
    ctc_cls_node_t *pre = g_port_cls_cfg[port].head;
    ctc_cls_node_t *tmp = NULL;

    for (idx = 0; idx < num; idx++)
    {
        cur = g_port_cls_cfg[port].head;
        pre = g_port_cls_cfg[port].head;

        while (cur)
        {
            if (cur->cls_rule.precedence != precendence[idx])
            {
            }
            else
            {
                ret = ca_classifier_rule_delete(0, cur->cls_idx);
                if (ret != CA_E_OK)
                {
                    OAM_ORG_DEBUG(
                        "Faile to delete cls rule, cls_idx %d\r\n",
                        cur->cls_idx);
                }
                OAM_ORG_DEBUG(
                    "delete cls_idx %d precedence %u, port %u, pre[%u]\r\n",
                    cur->cls_idx,
                    cur->cls_rule.precedence,
                    port,
                    precendence[idx]);

                cur->cls_idx = -1;
                g_port_cls_cfg[port].rule_cnt--;

                tmp = cur;
                if (cur == g_port_cls_cfg[port].head)
                {
                    g_port_cls_cfg[port].head = cur->nxt;
                }
                else
                {
                    pre->nxt = cur->nxt;
                }
                free(tmp);
                tmp = NULL;
                break;
            }

            if (pre != cur)
            {
                pre = cur;
            }

            cur = cur->nxt;
        }
    }


    ret = CA_E_OK;

    return ret;
}

ca_status_t _ctc_clr_cls(
    ca_uint32_t oam_port)
{
    ca_status_t ret = CA_E_ERROR;
    ca_uint32_t idx = 0;
    ctc_cls_node_t *cur = g_port_cls_cfg[oam_port].head;
    ctc_cls_node_t *tmp = NULL;

    while (cur)
    {
        ret = ca_classifier_rule_delete(0, cur->cls_idx);
        if (ret != CA_E_OK)
        {
            OAM_ORG_DEBUG(
                "Faile to delete cls rule of tail, cls_idx %d\r\n",
                cur->cls_idx);
        }

        g_port_cls_cfg[oam_port].rule_cnt--;
        OAM_ORG_DEBUG(
            "delete cls_idx %u rule_cnt %u on oam_port %u\r\n",
            cur->cls_idx,
            g_port_cls_cfg[oam_port].rule_cnt,
            oam_port);
        tmp = cur;
        cur = cur->nxt;
        free(tmp);
        tmp = NULL;
        idx++;
    }

    g_port_cls_cfg[oam_port].head = NULL;
    g_port_cls_cfg[oam_port].tail = NULL;
    ret = CA_E_OK;

    return ret;
}

oam_status ctc_oam_onu_classification_set_adapt(
    oam_uint8 action,
    oam_port_id_t port,
    oam_uint32 num,
    oam_uint8 *data)
{
    ctc_classification_rule_t  cls[CLASS_RULES_MAX];
    oam_uint8            precedence[CLASS_RULES_MAX];

    oam_ctc_onu_classification_struct *pData;
    oam_ctc_onu_class_fselect_t *pEntry;
    oam_ctc_onu_class_fselect_v6_t *pEntry6;
    oam_uint32                         i           = 0,
                                       j           = 0,
                                       len         = 0,
                                       total_len   = 0;
    oam_uint8 ctc_version = 0;
    oam_uint32 ii = 0;
    ca_uint32_t ca_port = 0;

    if (!data && (action != CTC_CLASS_RULES_ACTION_CLR))
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d, action = %d, num = %d\r\n", port, action, num);

    __PORT_CHECK(port);

    memset(cls, 0x00, CLASS_RULES_MAX * sizeof(ctc_classification_rule_t));

    oam_ctc_version_get(oam_llid_get(), &ctc_version);

    switch (action)
    {
    case CTC_CLASS_RULES_ACTION_CLR:
        _ctc_clr_cls(port);
        break;

    case CTC_CLASS_RULES_ACTION_DEL:
        for (i = 0; i < num; i++)
        {
            pData = (oam_ctc_onu_classification_struct *)(data + total_len);
            total_len += (pData->len + 2);
            precedence[i] = pData->precedence;

            OAM_ORG_DEBUG("precedence = %d\r\n",  precedence[i]);
        }

        _ctc_del_cls(port, num, precedence);

        break;

    case CTC_CLASS_RULES_ACTION_ADD:
        for (i = 0; i < num && i < CLASS_RULES_MAX; i++)
        {
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

            for (j = 0; j < pData->entries && j < CLASS_FIELD_SELECT_MAX; j++)
            {
                pEntry = (oam_ctc_onu_class_fselect_t *)(pData->data + len);

                if ((pEntry->fieldSelect >= CLASS_RULES_FSELECT_IPVER) &&
                    (ctc_version > 0x20))
                {
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

                        for (ii = 0; ii < CTC_CLASS_MATCH_VAL_LEN; ii++)
                        {
                            OAM_ORG_DEBUG("CLS[%d].fselect[%d].matchValue    = %d\r\n", i, j,
                                cls[i].fselect[j].matchValue + ii);
                        }
                    }

                }
                else
                {
                    pEntry = (oam_ctc_onu_class_fselect_t *)(pData->data + len);
                    len += sizeof(oam_ctc_onu_class_fselect_t);
                    cls[i].fselect[j].fieldSelect = pEntry->fieldSelect;
                    cls[i].fselect[j].validationOper = pEntry->validationOper;
                    memcpy(
                        cls[i].fselect[j].matchValue + (CLASS_MATCH_VAL_LEN - CTC_CLASS_MATCH_VAL_LEN),
                        pEntry->matchValue,
                        CTC_CLASS_MATCH_VAL_LEN);
                }

                {
                    OAM_ORG_DEBUG("CLS[%d].fselect[%d].fieldselect    = %d\r\n", i, j,
                        cls[i].fselect[j].fieldSelect);
                    OAM_ORG_DEBUG("CLS[%d].fselect[%d].validationOper = %d\r\n", i, j,
                        cls[i].fselect[j].validationOper);

                    for (ii = 0; ii < CTC_CLASS_MATCH_VAL_LEN; ii++)
                    {
                        OAM_ORG_DEBUG(
                            "CLS[%d].fselect[%d].matchValue     = %02X\r\n",
                            i, j,
                            *(cls[i].fselect[j].matchValue +
                              (CLASS_MATCH_VAL_LEN - CTC_CLASS_MATCH_VAL_LEN) +
                              ii)
                            );
                    }
                }

            }
            _ctc_add_cls(port, cls + i);
        }

        break;
    }

#ifdef ZTE_SPECIFIC
    ctc_oam_cls_syn_db(port);
#endif/*END_ZTE_SPECIFIC*/

    return OAM_E_OK;

}



/******************************* MC ADAPT **************************/

oam_status ctc_oam_onu_mcvlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *num,
    oam_uint16 *vlanIds)
{
    oam_uint8 mc_mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;
    oam_status   rt = OAM_E_OK;

    if ((NULL == num) || (NULL == vlanIds))
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\n", port);

    __PORT_CHECK(port);

    rt = ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (rt)
    {
        return rt;
    }

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode)
    {
        return OAM_E_NOT_SUPPORT;
    }

//    if (cs_sal_mc_vlan_get(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), vlanIds, num)) {
//        return OAM_E_ERROR;
//    }

    return OAM_E_OK;
}

oam_status ctc_oam_onu_mcvlan_set_adapt(
    oam_port_id_t port,
    oam_uint8 oper,
    oam_uint16 num,
    oam_uint16 *vlanIds)
{
    oam_uint8 mc_mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;
    oam_uint32 i = 0;
    oam_status rt = OAM_E_OK;
    ca_uint32 idx = 0;

    if (oper > CTC_MCVLAN_OPER_CLR)
    {
        return OAM_E_PARAM;
    }

    if (NULL == vlanIds)
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("\rport = %d, oper = %d, num = %d\r\n", port, oper, num);

    for (i = 0; i < num; i++)
    {
        OAM_ORG_DEBUG("\rvlanid = %d\r\n", vlanIds[i]);
    }

    __PORT_CHECK(port);

    rt = ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (rt)
    {
        return rt;
    }

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode)
    {
        return OAM_E_NOT_SUPPORT;
    }

    if (CTC_MCVLAN_OPER_DEL == oper)
    {
        for (idx = 0; idx < num; idx++)
        {
            //mc_ctc_port_multicast_vlan_del ( 0, OAM_UNI_PORT_ID_TO_CA_PORT(port), vlanIds[ idx ] );
        }
    }
    else if (CTC_MCVLAN_OPER_ADD == oper)
    {
        for (idx = 0; idx < num; idx++)
        {
            //mc_ctc_port_multicast_vlan_add ( 0, OAM_UNI_PORT_ID_TO_CA_PORT(port), vlanIds[ idx ] );
        }
    }
    else if (CTC_MCVLAN_OPER_CLR == oper)
    {
        //        if (mc_ctc_port_multicast_vlan_clear (0, OAM_UNI_PORT_ID_TO_CA_PORT(port))) {
        //            return OAM_E_ERROR;
        //        }
    }
    else
    {
        return OAM_E_PARAM;
    }

#ifdef ZTE_SPECIFIC
    ctc_oam_mc_vlan_syn_db(port);
#endif/*END_ZTE_SPECIFIC*/

    return OAM_E_OK;
}


oam_status ctc_oam_onu_mctagstrip_get_adapt(
    oam_port_id_t port,
    oam_uint8 *tagStriped,
    oam_uint8 *num,
    oam_ctc_iptv_vid_t *vlans)
{
//    cs_sal_mc_vlan_act_t          vlan_act;
//    cs_sal_vlan_cfg_t             swap_rule[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16                     rule_nums;
    int i;

    if ((NULL == tagStriped) || (NULL == num) || (NULL == vlans))
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d\r\r\n", port);

    __PORT_CHECK(port);

//    if (mc_ctc_port_tagstrip_state_get (0, OAM_UNI_PORT_ID_TO_CA_PORT(port), tagStriped))
//    {
//        return OAM_E_ERROR;
//    }

//    if (SAL_MC_VLAN_TAG_SWAP == vlan_act) {
//        for (i = 0; i < rule_nums; ++i) {
//            vlans[i].vid = swap_rule[i].s_vlan.vid;
//            vlans[i].iptv_vid = swap_rule[i].c_vlan.vid;
//        }
//
//        *num = rule_nums;
//    }


    return OAM_E_OK;
}

oam_status ctc_oam_onu_mctagstrip_set_adapt(
    oam_port_id_t port,
    oam_uint8  tagStriped,
    oam_uint8  num,
    oam_ctc_iptv_vid_t *vlans)
{
//    cs_sal_mc_vlan_act_t          vlan_act;
//    cs_sal_vlan_cfg_t             swap_rule[CTC_ONU_AGGR_VLAN_NUM];
    int i;
#ifdef ZTE_SPECIFIC
    db_zte_mc_tagstrip_t        db_mc_tagstrip;
#endif/*END_ZTE_SPECIFIC*/

    if (vlans == NULL)
    {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("port = %d, tagstriped = %d, num = %d\r\n", port, tagStriped, num);

    for (i = 0; i < num; i++)
    {
        OAM_ORG_DEBUG("vlanid = %d, iptv_vid = %d\r\n", vlans[i].vid, vlans[i].iptv_vid);
    }

    __PORT_CHECK(port);

    if ((CTC_MCVLAN_TAG_OPER_SWAP == tagStriped) && (NULL == vlans))
    {
        return OAM_E_PARAM;
    }

    if (CTC_MCVLAN_TAG_OPER_SWAP == tagStriped)
    {
//        for (i = 0; i < num; ++i) {
//            swap_rule[i].s_vlan.vid = vlans[i].vid;
//            swap_rule[i].c_vlan.vid = vlans[i].iptv_vid;
//        }
    }

//    if (mc_ctc_port_tagstrip_mode_set(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), tagStriped)) {
//        return OAM_E_ERROR;
//    }

#ifdef ZTE_SPECIFIC
    memset(&db_mc_tagstrip, 0x00, sizeof(db_mc_tagstrip));
    db_mc_tagstrip.port_id = port;
    db_mc_tagstrip.tagStriped = tagStriped;
    db_mc_tagstrip.num = num;
    memcpy(db_mc_tagstrip.vlan, vlans, sizeof(oam_ctc_iptv_vid_t) * num);
    cs_db_entry_mod(TB_ZTE_MC_TAGSTRIP, (void *)&db_mc_tagstrip,
        sizeof(db_mc_tagstrip),
        column_bit(TB_ZTE_MC_TAGSTRIP, "ALL"));
#endif/*END_ZTE_SPECIFIC*/
    return OAM_E_OK;

}


oam_status
ctc_oam_onu_mc_switch_get_adapt
(
    oam_uint8 *mode
    )
{
#if 0
    mc_uint8 mc_mode;
    //mc_ctc_snoop_switch_get (0, &mc_mode);

    switch (mc_mode)
    {
        case MC_CTC_SNOOPING:
        *mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;
        break;

        case MC_CTC_CTRL:
        *mode = CTC_MC_SWITCH_START_CTC_IGMP;
        break;

        default:
        *mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;
#ifdef ZTE_SPECIFIC
        *mode = CTC_MC_SWITCH_TRANSPARENT_ZTE_EXT;
#endif/*END_ZTE_SPECIFIC*/
        break;
    }

    return OAM_E_OK;
#endif
}

oam_status ctc_oam_onu_mc_switch_set_adapt(
    oam_uint8 mode)
{
#if 0
    ca_status_t   ret = CA_E_OK;
    mc_uint8  mc_mode;
    mc_uint8  temp_mc_mode;
#ifdef ZTE_SPECIFIC
    db_zte_onu_svc_t   svc;
#endif/*END_ZTE_SPECIFIC*/

    switch ( mode )
    {
        case    CTC_MC_SWITCH_START_IMGP_SNOOPING:
        temp_mc_mode = MC_CTC_SNOOPING;
        break;

        case    CTC_MC_SWITCH_START_CTC_IGMP:
        temp_mc_mode = MC_CTC_CTRL;
        break;
#ifdef ZTE_SPECIFIC

        case    CTC_MC_SWITCH_TRANSPARENT_ZTE_EXT:
        temp_mc_mode = MC_DISABLE;
        break;
#endif/*END_ZTE_SPECIFIC*/

        default:
        return  CA_E_PARAM;

    }

    mc_mode_get(0, &mc_mode);

    if ( temp_mc_mode == mc_mode)
    {
        return CA_E_OK;
    }

    mc_mode_set ( 0, mc_mode);
#ifdef ZTE_SPECIFIC

    if (ret == CA_E_OK )
    {
        memset(&svc, 0x00, sizeof(svc));
        svc.mc_mode = temp_mc_mode;
        cs_db_entry_mod(TB_ZTE_ONU_SVC, (void *) &svc, sizeof(svc),
                        column_bit(TB_ZTE_ONU_SVC, "mc_mode"));
    }

#endif/*END_ZTE_SPECIFIC*/
    return 0;
#endif
}

oam_status ctc_oam_onu_mc_control_type_get_adapt(
    oam_uint8 *type)
{
    //*type = mc_get_mc_fwd_mode();
    return 0;
}

oam_status
ctc_oam_onu_mc_control_type_set_adapt
(
    oam_uint8  type
    )
{
//    mc_status ret = MC_E_ERROR;
//    mc_ctc_ctrl_entry_t portCtrl;

//    ret = mc_port_ctrl_entry_add( 0, 0, );

    return 0;
}

oam_status ctc_oam_onu_mc_control_vlan_mac_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_vlan_mac_t *vlanMac)
{
    //mc_ctc_ctrl_get_entries((oam_uint8 *)vlanMac, num);
    //cs_igmp_ctcctrl_vlan_mac_get(num, vlanMac);
    return CA_E_OK;

}

oam_status ctc_oam_onu_mc_control_mac_ip_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_mac_ip_t *macIp)
{
    //mc_ctc_ctrl_get_entries((oam_uint8 *)macIp, num);
    //cs_igmp_ctcctrl_mac_ip_get(num, macIp);
    return CA_E_OK;

}

oam_status ctc_oam_onu_mc_control_vlan_ip_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIp)
{
    //mc_ctc_ctrl_get_entries((oam_uint8 *)vlanIp, num);
    //cs_igmp_ctcctrl_vlan_ip_get(num, vlanIp);
    return CA_E_OK;

}

oam_status ctc_oam_onu_mc_control_clr_adapt()
{
    //mc_ctc_ctrl_clr_mfdb();
    //cs_igmp_ctcctrl_clr();
    return CA_E_OK;
}

oam_status ctc_oam_onu_mc_control_vlan_mac_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_vlan_mac_t *vlanMac)
{
    if (!num || !vlanMac)
        return CA_E_OK;

    //cs_igmp_ctcctrl_vlan_mac_set(action, num, vlanMac);
    return CA_E_OK;
}

oam_status ctc_oam_onu_mc_control_mac_ip_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_mac_ip_t *macIp)
{
    if (!num || !macIp)
        return CA_E_OK;

    //cs_igmp_ctcctrl_mac_ip_set(action, num, macIp);
    return CA_E_OK;
}

oam_status ctc_oam_onu_mc_control_vlan_ip_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIp)
{
    if (!num || !vlanIp)
        return CA_E_OK;

    //cs_igmp_ctcctrl_vlan_ip_set(action, num, vlanIp);

    return CA_E_OK;
}

oam_status ctc_oam_onu_grp_nmb_max_get_adapt(
    oam_port_id_t port,
    oam_uint8   *maxGrp)
{
    ca_status_t ret = 0;
    oam_uint8 mc_mode;
    ca_port_id_t port_id = 0;

    ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode)
    {
        return CA_E_RESOURCE;
    }

    if (port == 1)
        port_id = AAL_PORT_UNI0;
    else
        port_id = AAL_PORT_UNI1;

//    ret = mc_ctc_port_mc_group_get(0,  port_id, (oam_uint32 *)maxGrp);

    return ret;
}

oam_status ctc_oam_onu_grp_nmb_max_set_adapt(
    oam_port_id_t port,
    oam_uint8   maxGrp)
{
    oam_uint8 mc_mode;
    ca_status_t ret = 0;
    ca_port_id_t port_id = 0;
#ifdef ZTE_SPECIFIC
    db_zte_mc_grp_t     db_mc_grp;
#endif/*END_ZTE_SPECIFIC*/

    ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode)
    {
        return CA_E_NOT_SUPPORT;
    }

    if (port == 1)
        port_id = AAL_PORT_UNI0;
    else
        port_id = AAL_PORT_UNI1;

//    ret = mc_ctc_port_group_num_set(
//                0,
//                port_id,
//                (oam_uint32)maxGrp);

#ifdef ZTE_SPECIFIC

    if (ret == CA_E_OK)
    {
        memset(&db_mc_grp, 0x00, sizeof(db_mc_grp));
        db_mc_grp.port_id = port;
        db_mc_grp.mc_grp = maxGrp;
        cs_db_entry_mod(TB_ZTE_MAX_GRP, (void *)&db_mc_grp, sizeof(db_zte_mc_grp_t),
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

    if (entry.object_type == OAM_CTC_ALARM_TYPE_ONU)
    {
        *type = CTC_ONU_ALARM;
    }
    else if (entry.object_type == OAM_CTC_ALARM_TYPE_PON)
    {
        *type = CTC_PON_ALARM;
        *port = OAM_PON_PORT_ID;
    }
    else if (entry.object_type == OAM_CTC_ALARM_TYPE_UNI)
    {
        *type = CTC_PORT_ALARM;
        *port = (entry.object_inst & 0x000000ff);
    }
    else
    {
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

    if (ret != OAM_E_OK)
    {
        return ret;
    }

    ret = ctc_oam_get_alarm_config(type, port, entry.id, &alarm_config);

    if (ret != OAM_E_OK)
    {
        return ret;
    }

    if (alarm_config == CTC_ALARM_ON)
    {
        *status = OAM_CTC_EVENT_STATUS_ENABLE;
    }
    else
    {
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

    if (ret != OAM_E_OK)
    {
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

    if (ret != OAM_E_OK)
    {
        return ret;
    }

    if (config.status == OAM_CTC_EVENT_STATUS_ENABLE)
    {
        enable = CTC_ALARM_ON;
    }
    else
    {
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

    if (ret != OAM_E_OK)
    {
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
    if (!mgmt_idx.valid)
    {
        *type = CTC_ONU_ALARM;
    }
    else
    {
        if (mgmt_idx.idxLeaf == OAM_CTC_LEAF_CODE_ONU_PON)
        {
            *type = CTC_PON_ALARM;
            *port = OAM_PON_PORT_ID;
        }
        else if (mgmt_idx.idxLeaf == OAM_CTC_LEAF_CODE_ONU_PORT)
        {
            if (mgmt_idx.idxBranch == OAM_CTC_INDEX_BRANCH && mgmt_idx.idxValue == 0)
            {
                *type = CTC_PON_ALARM;
                *port = OAM_PON_PORT_ID;
            }
            else
            {
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
    switch (alarm_id)
    {
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

    if (type != CTC_PORT_ALARM)
    {
        ctc_oam_onu_alarm_syn_db();
    }
    else
    {

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

    if (type != CTC_PORT_ALARM)
    {
        ctc_oam_onu_alarm_syn_db();
    }
    else
    {
        ctc_oam_port_alarm_syn_db(port);
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
    ca_status_t ca_ret = CA_E_ERROR;
    ca_device_config_tlv_t scfg;

    OAM_ASSERT_RET((loid != NULL
                    && passwd != NULL),
        OAM_E_PARAM);

    memset(&scfg, 0x00, sizeof(scfg));

    scfg.type = CA_CFG_ID_LOID;
    scfg.len  = CFG_ID_LOID_LEN;

    ca_ret = ca_device_config_tlv_get(0, &scfg);
    if (ca_ret != CA_E_OK)
    {
        ca_printf("%s: Failed to get scfg for CA_CFG_ID_LOID\n", __FUNCTION__);
    }

    memcpy(loid, scfg.value, sizeof(oam_int8) * CFG_ID_LOID_LEN);

    memset(&scfg, 0x00, sizeof(scfg));

    scfg.type = CA_CFG_ID_PASSWD;
    scfg.len  = CFG_ID_PASSWD_LEN;

    ca_ret = ca_device_config_tlv_get(0, &scfg);
    if (ca_ret != CA_E_OK)
    {
        ca_printf("%s: Failed to get scfg for CA_CFG_ID_PASSWD\n", __FUNCTION__);
    }

    memcpy(passwd, scfg.value, sizeof(oam_int8) * CFG_ID_PASSWD_LEN);

    return OAM_E_OK;
}


oam_status ctc_oam_auth_keep_silence_set_adapt(oam_uint8 llid_index,
    oam_boolean enable)
{
    if (ca_epon_mpcp_keep_silence_set(0, 0, llid_index, enable, 60000))
    {
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_laser_alarm_status_get(oam_uint8 *status)
{
    ca_epon_port_laser_mode_t mode;

    ca_epon_port_laser_mode_get(0, OAM_PON_PORT_ID_TO_CA_PORT(), &mode);

    if (mode == CA_EPON_PORT_LASER_MODE_ON)
    {
        *status = 1;
    }
    else
    {
        *status = 0;
    }

    return OAM_E_OK;
}


oam_status ctc_oam_up_congest_status_get(oam_port_id_t port, oam_boolean *status)
{
    OAM_ASSERT_RET(status != NULL, OAM_E_PARAM);

    //cs_cosq_us_congest_get(0, OAM_UNI_PORT_ID_TO_CA_PORT(port), status, 0);

    return OAM_E_OK;
}

oam_status ctc_oam_chip_id_get_adapt(oam_ctc_onu_chip_id_t *chip_id)
{
    ca_status_t ca_ret = CA_E_ERROR;
    oam_uint32             a      = 0x01,
                           b      = 0xf2,
                           c;
    ca_device_config_tlv_t scfg;

    chip_id->venderId[0] = a;
    chip_id->venderId[1] = b;

    memset(&scfg, 0x00, sizeof(scfg));

    scfg.type = CA_CFG_ID_MODEL_NAME;
    scfg.len  = CFG_ID_MODEL_NAME_LEN;

    ca_ret = ca_device_config_tlv_get(0, &scfg);
    if (ca_ret != CA_E_OK)
    {
        ca_printf("%s: Failed to get scfg for CA_CFG_ID_MODEL_NAME\n", __FUNCTION__);
    }
    else
    {
        chip_id->chipModel[0] = scfg.value[8];
        chip_id->chipModel[1] = scfg.value[9];
    }

    chip_id->Revision = a;

    memset(&scfg, 0x00, sizeof(scfg));

    scfg.type = CA_CFG_ID_HW_YEAR_WEEK;
    scfg.len  = CFG_ID_HW_YEAR_WEEK_LEN;

    ca_ret = ca_device_config_tlv_get(0, &scfg);
    if (ca_ret != CA_E_OK)
    {
        ca_printf("%s: Failed to get scfg for CA_CFG_ID_HW_YEAR_WEEK\n", __FUNCTION__);
    }
    else
    {
        chip_id->Date[0] = scfg.value[1];
        chip_id->Date[1] = scfg.value[2];
        chip_id->Date[2] = scfg.value[3];
    }

    return OAM_E_OK;
}

oam_status ctc_oam_sn_get_adapt(oam_ctc_onu_sn_t *sn)
{
#ifdef SATURN_PORT_REMOVE
    /* oam_uint8 mac [OAM_MACADDR_LEN];*/
    oam_uint8 vendorId[OAM_CTC_VENDER_ID_LEN + 1] = { 0 };
    oam_uint8 model_name[OAM_CTC_MODEL_ID_LEN + 1] = { 0x46, 0x34, 0x30, 0x32 };
    oam_uint8 hwver[OAM_CTC_ONU_HW_LEN + 1] = { 0 };
    oam_uint8 swVer[OAM_CTC_ONU_SW_LEN + 1] = { 0 };
    tSwImgCfg    img_info;
    oam_uint8 onu_mac[OAM_MACADDR_LEN] = { 0 };

    memset(swVer, 0x00, OAM_CTC_ONU_SW_LEN);
    memset(&img_info, 0x00, sizeof(tSwImgCfg));
    cs_swdl_getswimginfo(CS_SWIMG_ID_0, &img_info);

    if (!img_info.isActiveB)
    {
        memset(&img_info, 0x00, sizeof(tSwImgCfg));
        cs_swdl_getswimginfo(CS_SWIMG_ID_1, &img_info);
    }

    memcpy(swVer, img_info.epon_version, OAM_CTC_ONU_SW_LEN);

    sn->hdr.width = sizeof(oam_ctc_onu_sn_t) - sizeof(oam_var_cont_t);

    /*startup_cfg_read(CS_SCFG_ID_83, OAM_CTC_VENDER_ID_LEN, vendorId);
    startup_cfg_read(CFG_ID_ONU_MOD_ID, OAM_CTC_MODEL_ID_LEN, model_name);*/
    strncpy((oam_int8 *)vendorId, ONU_VENDOR_ID, OAM_CTC_VENDER_ID_LEN);

    onu_hw_version_get(hwver);
#ifdef SATURN_PORT_REMOVE
    scfg_read(0, CFG_ID_MAC_ADDRESS, OAM_MACADDR_LEN, onu_mac);
#endif
    memcpy(sn->venderId + OAM_CTC_VENDER_ID_LEN - strlen((char *)vendorId),
        vendorId, strlen((char *)vendorId));
    memcpy(sn->model + OAM_CTC_MODEL_ID_LEN - strlen((char *)model_name),
        model_name, strlen((char *)model_name));
    memcpy(sn->onuId, onu_mac, OAM_MACADDR_LEN);

    memcpy(sn->hwVer + OAM_CTC_ONU_HW_LEN - strlen((char *)hwver), hwver,
        strlen((char *)hwver));

    memcpy(sn->swVer + OAM_CTC_ONU_SW_LEN - strlen((char *)swVer), swVer,
        strlen((char *)swVer));
#endif
    oam_uint8 mac[OAM_MACADDR_LEN] = { 0x00, 0x13, 0x25, 0xab, 0xcd, 0xef };
    oam_int8 vender[OAM_CTC_VENDOR_ID_LEN] = { 0x32, 0x30, 0x31, 0x31 };
    oam_int8 model[OAM_CTC_MODEL_ID_LEN] = { 0x38, 0x31, 0x30, 0x65 };
    oam_int8 hwver[OAM_CTC_ONU_HW_LEN] = { 0x48, 0x47, 0x38, 0x31, 0x30, 0x65, 0, 0 };

    oam_uint8 swver[OAM_CTC_ONU_SW_LEN] = { 0x56, 0x31, 0x30, 0x30, 0x52, 0x30, 0x30, 0x31, 0x43,
        0x30, 0x31, 0x42, 0x30, 0x35, 0x32, 00
    };

    sn->hdr.width = sizeof(oam_ctc_onu_sn_t) - sizeof(oam_var_cont_t);

    memcpy(sn->venderId,  vender, OAM_CTC_VENDOR_ID_LEN);
    memcpy(sn->model,  model, OAM_CTC_MODEL_ID_LEN);
    memcpy(sn->onuId, mac, OAM_MACADDR_LEN);
    memcpy(sn->hwVer, hwver, OAM_CTC_ONU_HW_LEN);
    memcpy(sn->swVer, swver, OAM_CTC_ONU_SW_LEN);
    return OAM_E_OK;
}

oam_status ctc_oam_fw_get_adapt(oam_uint8 *fw_ver, oam_uint8 *len)
{
    oam_uint8 current_ver[64] = { 0 };
    ca_fw_ver_info_get(0, current_ver);
    strcpy((char *)fw_ver, current_ver);
    *len = strlen((char *)fw_ver);
    return OAM_E_OK;
}

oam_status
ctc_oam_onucap2_get_adapt (
    oam_ctc_onu_capabilities2_t* onu_cap2)
{
    oam_status ret            = OAM_E_OK;
    oam_uint32 idx = 0;
    oam_uint8 numOfPon = 1;
    oam_uint8 protectionType = 0;
    oam_uint8 hasBattery = 1; //cs_gpio_ups_valid();

    if ( !onu_cap2 )
    {
        ret = OAM_E_PARAM;
        goto ERROR_CTC_OAM_ONUCAP2_GET_ADAPT;
    }
    
    onu_cap2->onuType = 0; /* SFU */
    onu_cap2->multillid = 1; /* only support single llid */
    onu_cap2->protectionType = protectionType;
    onu_cap2->numPONIf = numOfPon;
    onu_cap2->numSlot = 0;

    onu_cap2->intf[0].interfaceType = 0; /* GE */
    onu_cap2->intf[0].numPorts = g_uni_num; 
        onu_cap2->numInterface ++;

    onu_cap2->BatteryBackup = hasBattery;

ERROR_CTC_OAM_ONUCAP2_GET_ADAPT:
    return ret;
}

oam_status ctc_oam_onucap3_get_adapt (oam_ctc_onu_capabilities3_t* onu_cap3)
{
    oam_status ret   = OAM_E_OK;
    oam_uint8  ipv6  = CTC_IPV6_CAPABILTY_SUPPORT;
    oam_uint8  power = CTC_POWER_CONTROL_SUPPORT_TX_RX;
    oam_uint8  sla   = CTC_SERVICE_SLA_NOT_SUPPORT;

    if ( !onu_cap3 )
    {
        ret = OAM_E_PARAM;
        goto ERROR_CTC_OAM_ONUCAP3_GET_ADAPT;
    }

    onu_cap3->ipv6Support = ipv6;  /* SFU */
    onu_cap3->powerControl = power; /* only support single llid */
    onu_cap3->serviceSla = sla;

ERROR_CTC_OAM_ONUCAP3_GET_ADAPT:
    return ret;
}

oam_status ctc_oam_keep_silence_time_get_adapt (oam_uint8* keep)
{
    oam_status ret = OAM_E_OK;

    if ( !keep)
    {
        ret = OAM_E_PARAM;
        goto ERROR_CTC_OAM_KEEP_SILENCE_TIME_GET_ADAPT;
    }

    *keep = 60;

ERROR_CTC_OAM_KEEP_SILENCE_TIME_GET_ADAPT:
    return ret;
}

/* Random number generator. Ripped out of the C library.*/
static ca_int32_t
_key_rand (
    ca_int32_t* seed)
{
#define KEY_RAND_MAX 0x7fffffff
#define KEY_MM 2147483647    /* a Mersenne prime*/
#define KEY_AA 48271         /* this does well in the spectral test*/
#define KEY_QQ 44488         /* (long)(MM/AA)*/
#define KEY_RR 3399          /* MM % AA; it is important that KEY_RR<KEY_QQ*/

    if ( !seed )
    {
        return 0;
    }

    *seed = KEY_AA * (*seed % KEY_QQ) - KEY_RR * (ca_int32_t)(*seed / KEY_QQ);
    if (*seed < 0)
        *seed += KEY_MM;

    return (ca_int32_t)(*seed & KEY_RAND_MAX);
}

oam_status
ctc_oam_enc_key_get_adapt(
    oam_uint8  llid_index,
    oam_uint8  key_index,
    oam_uint8 *key,
    oam_uint8 *key_len)
{
    ca_status_t ret;
    ca_int32_t                    seed     = 0,
                                  i        = 0;
    ca_uint32_t rand_key[4];
    ca_epon_port_encryption_key_t  enc_key;

    memset(rand_key, 0x00, sizeof(ca_uint32_t) * 4);

    for (i = 0; i < 4; i++)
    {
        srand((unsigned)time(NULL));
        seed = (i + 1) * rand();
        rand_key[i] = _key_rand(&seed);
    }

    memcpy(&enc_key, rand_key, sizeof(ca_epon_port_encryption_key_t));

    ret = ca_epon_llid_encryption_key_set(
        0,
        CA_PORT_ID(CA_PORT_TYPE_EPON, 0x07),
        llid_index,
        key_index,
        &enc_key);
    if (ret)
    {
        printf(
            "Failed to set encryption on llid 0x%x with key idx 0x%x, ret 0x%x\r\n",
            llid_index,
            key_index,
            ret);
        goto ERROR_CTC_OAM_ENC_KEY_GET_ADAPT;
    }

    if (g_epon_mac_mod == ONU_PON_MAC_EPON_1G)
    {
        *key_len = CHURNING_KEY_LEN;
        memcpy(key, enc_key.data, CHURNING_KEY_LEN);
    }
    else if (g_epon_mac_mod == ONU_PON_MAC_EPON_D10G ||
             g_epon_mac_mod == ONU_PON_MAC_EPON_BI10G)
    {
        *key_len = CHURNING_KEY_10G_LEN;
        memcpy(key, enc_key.data, CHURNING_KEY_10G_LEN);
    }

    OAM_ORG_DEBUG("key = %s\r\n",  key);

    ret = ca_port_encryption_mode_set(
        0,
        CA_PORT_ID(CA_PORT_TYPE_EPON, 0x07),
        CA_DIRECTION_ENC_RX,
        CA_PORT_ENCRYPTION_MODE_HW_CTC_CHURNING);
    if (ret)
    {
        printf(
            "Failed to set encrypt mode with CTC Churning on RX direction, ret 0x%x\r\n",
            ret);
    }

ERROR_CTC_OAM_ENC_KEY_GET_ADAPT:
    return ret;
}

oam_status ctc_oam_onu_mpcp_queue_thresh_get_adapt(
    oam_uint8 queue, oam_uint16 *threshold)
{
#if 0
    oam_uint16 max_threshold;
    ca_status_t ret;

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
    ca_status_t ret;

    ret = cs_sal_epon_onu_mpcp_queue_threshold_set(0, -1, index, threshold, 0);

    if (ret)
    return OAM_E_PARAM;
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_pon_stat_get_adapt(oam_ctc_onu_port_stats_t *stats)
{
    ca_epon_port_stats_t pon_stas;
    ca_status_t ret = CA_E_OK;

    memset(&pon_stas, 0, sizeof(pon_stas));
    ret = ca_epon_port_stats_get(0, OAM_PON_PORT_ID_TO_CA_PORT(), TRUE, &pon_stas);
    if (ret != CA_E_OK)
    {
        return ret;
    }

    stats->dn_drop_events            = 0;
    stats->up_drop_events            = 0;
    stats->dn_octets                 = pon_stas.rx_good_octets;
    stats->up_octets                 = pon_stas.tx_good_octets;
    stats->dn_frames                 = pon_stas.rx_uc_frames +
        pon_stas.rx_mc_frames +
        pon_stas.rx_bc_frames;
    stats->up_frames                 = pon_stas.tx_uc_frames +
        pon_stas.tx_mc_frames +
        pon_stas.tx_bc_frames;
    stats->dn_bc_frames              = pon_stas.rx_bc_frames;
    stats->up_bc_frames              = pon_stas.tx_bc_frames;
    stats->dn_mc_frames              = pon_stas.rx_mc_frames;
    stats->up_mc_frames              = pon_stas.tx_mc_frames;
    stats->dn_crc_err_frames         = pon_stas.rx_fcs_error_frames;
    stats->up_crc_err_frames         = 0;

    stats->dn_undersize_frames       = pon_stas.rx_runt_frames;
    stats->up_undersize_frames       = 0;
    stats->dn_oversize_frames        = pon_stas.rx_oversize_frames;
    stats->up_oversize_frames        = 0;
    stats->dn_fragments                = 0;
    stats->up_fragments                = 0;
    stats->dn_jabbers                    = 0;
    stats->up_jabbers                    = 0;
    /*stats->dn_collisions                  = 0; */
    stats->dn_64_octets_frames       = pon_stas.rx_64_frames;
    stats->dn_65_127_octets_frames   = pon_stas.rx_65_127_frames;
    stats->dn_128_255_octets_frames  = pon_stas.rx_128_255_frames;
    stats->dn_256_511_octets_frames  = pon_stas.rx_256_511_frames;
    stats->dn_512_1023_octets_frames = pon_stas.rx_512_1023_frames;
    stats->dn_1024_1518_octets_frames = pon_stas.rx_1024_1518_frames;
    stats->up_64_octets_frames        = pon_stas.tx_64_frames;
    stats->up_65_127_octets_frames   = pon_stas.tx_65_127_frames;
    stats->up_128_255_octets_frames  = pon_stas.tx_128_255_frames;
    stats->up_256_511_octets_frames  = pon_stas.tx_256_511_frames;
    stats->up_512_1023_octets_frames = pon_stas.tx_512_1023_frames;
    stats->up_1024_1518_octets_frames = pon_stas.tx_1024_1518_frames;
    stats->dn_discard_frames         = 0;
    stats->up_discard_frames         = 0;
    stats->dn_err_frames             = pon_stas.rx_fcs_error_frames +
        pon_stas.rx_runt_frames +
        pon_stas.rx_oversize_frames;

    stats->up_err_frames = 0;
    stats->status_change_times       = 0;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_uni_stat_get_adapt(
    oam_port_id_t port,
    oam_ctc_onu_port_stats_t *stats)
{
    ca_eth_port_stats_t uni_stas;
    oam_uint32 change_count = 0;
    ca_status_t ret = CA_E_OK;

    if (!g_ctc_port_info)
    {
        return OAM_E_PARAM;
    }
    
    memset(&uni_stas, 0, sizeof(uni_stas));
    ret = ca_eth_port_stats_get(0, OAM_PORT_ID_TO_CA_PORT(port), TRUE, &uni_stas);

    if (ret != CA_E_OK)
    {
        printf("%s: Failed to get stats for oam_port %u\n", __FUNCTION__, port);
        return ret;
    }

//    cs_stats_uni_status_chng_get(port, &change_count);


    stats->dn_drop_events            = 0;
    stats->up_drop_events            = 0;
    stats->dn_octets                 = uni_stas.tx_good_octets;
    stats->up_octets                 = uni_stas.rx_good_octets;
    stats->dn_frames                 = uni_stas.tx_uc_frames + uni_stas.tx_mc_frames +
        uni_stas.tx_bc_frames;
    stats->up_frames                 = uni_stas.rx_uc_frames + uni_stas.rx_mc_frames +
        uni_stas.rx_bc_frames;
    stats->dn_bc_frames              = uni_stas.tx_bc_frames;
    stats->up_bc_frames              = uni_stas.rx_bc_frames;


    stats->dn_mc_frames              = uni_stas.tx_mc_frames;
    stats->up_mc_frames              = uni_stas.rx_mc_frames;
    stats->dn_crc_err_frames         = 0;
    stats->up_crc_err_frames         = uni_stas.rx_fcs_error_frames;

    stats->dn_undersize_frames       = 0;
    stats->up_undersize_frames       = uni_stas.rx_runt_frames;
    stats->dn_oversize_frames        = 0;
    stats->up_oversize_frames        = uni_stas.rx_oversize_frames;
    stats->dn_fragments              = 0;
    stats->up_fragments              = 0;
    stats->dn_jabbers                = 0;
    stats->up_jabbers                = 0;
    /*stats->dn_collisions             = uni_stas.single_collision + uni_stas.multiple_collision +
                                               uni_stas.late_collisions + uni_stas.excessive_collisions; */
    stats->dn_64_octets_frames       = uni_stas.tx_64_frames;
    stats->dn_65_127_octets_frames   = uni_stas.tx_65_127_frames;
    stats->dn_128_255_octets_frames  = uni_stas.tx_128_255_frames;
    stats->dn_256_511_octets_frames  = uni_stas.tx_256_511_frames;
    stats->dn_512_1023_octets_frames = uni_stas.tx_512_1023_frames;
    stats->dn_1024_1518_octets_frames = uni_stas.tx_1024_1518_frames;
    stats->up_64_octets_frames       = uni_stas.rx_64_frames;
    stats->up_65_127_octets_frames   = uni_stas.rx_65_127_frames;
    stats->up_128_255_octets_frames  = uni_stas.rx_128_255_frames;
    stats->up_256_511_octets_frames  = uni_stas.rx_256_511_frames;
    stats->up_512_1023_octets_frames = uni_stas.rx_512_1023_frames;
    stats->up_1024_1518_octets_frames = uni_stas.rx_1024_1518_frames;
    stats->dn_discard_frames         = 0;
    stats->up_discard_frames         = 0;
    stats->dn_err_frames             = 0;

    stats->up_err_frames            = uni_stas.rx_fcs_error_frames +
        uni_stas.rx_runt_frames +
        uni_stas.rx_oversize_frames;
    stats->status_change_times      = g_ctc_port_info[port].lnk_sts_chng_cnt;
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
    ca_img_info_t info0;
    memset(&info0, 0, sizeof(ca_img_info_t));
    ca_img_info_get(0, 0, &info0);

    if (info0.active_flag)
    {
        return 1;
    }
    return 0;
}

oam_uint8 ctc_oam_swimage_flash_state_get_adapt()
{
    ca_img_info_t info0;
    ca_img_info_t info1;
    ca_uint8_t image_idx = 0;

    memset(&info0, 0, sizeof(ca_img_info_t));
    memset(&info1, 0, sizeof(ca_img_info_t));
    ca_img_info_get(0, 0, &info0);
    ca_img_info_get(0, 1, &info1);

    if (info0.commit_flag && info0.active_flag)
    {
        OAM_ORG_LOG("image 0 is active and commit \n");
        return OAM_CTC_FLASH_IMG0_A_C;
    }

    if (info1.commit_flag && info1.active_flag)
    {
        OAM_ORG_LOG("image 1 is active and commit \n");
        return OAM_CTC_FLASH_IMG0_NA_NC;
    }

    if (info0.active_flag && info1.commit_flag)
    {
        OAM_ORG_LOG("image 0 is active and image 1 is commit \n");
        return OAM_CTC_FLASH_IMG0_A_NC;
    }

    if (info1.active_flag && info0.commit_flag)
    {
        OAM_ORG_LOG("image 1 is active and image 0 is commit \n");
        return OAM_CTC_FLASH_IMG0_NA_C;
    }

    return OAM_CTC_FLASH_IMG0_A_NC;
}

    #define OAM_CTC_IMAGE_FILE   "/tmp/oam_ctc.img"
oam_int32 ctc_img_file = -1;
oam_int32 ctc_img_ready = 0;
oam_status ctc_oam_swimage_open_adapt(oam_uint8 img_id)
{
    ctc_img_file = open(OAM_CTC_IMAGE_FILE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);

    if (ctc_img_file < 0)
    {
        OAM_ORG_LOG("open file %s failed \n", OAM_CTC_IMAGE_FILE);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_swimage_write_adapt(
    oam_uint8 img_id,
    oam_uint8 *buf,
    oam_uint16 len)
{
    write(ctc_img_file, buf, len);
    return OAM_E_OK;
}

oam_status ctc_oam_swimage_image_ready(oam_uint8 img_id)
{
    ctc_img_ready = 1;
    return OAM_E_OK;
}

oam_status ctc_oam_swimage_close_adapt(oam_uint8 img_id)
{
    char upgrade_cmd[64] = { 0 };

    close(ctc_img_file);
    ctc_img_file = -1;

    if (ctc_img_ready)
    {
        ctc_img_ready = 0;
        system("echo 0 > /tmp/upgrade_status");
        snprintf(upgrade_cmd, 63, "ca.upgrade %s ", OAM_CTC_IMAGE_FILE);
        if (system(upgrade_cmd) != 0)
            return OAM_E_ERROR;
    }
    else
    {
        snprintf(upgrade_cmd, 63, "rm %s ", OAM_CTC_IMAGE_FILE);
        system(upgrade_cmd);
    }

    return OAM_E_OK;
}


oam_status ctc_oam_swimage_active_adapt()
{
    ca_img_info_t info0;

    memset(&info0, 0, sizeof(ca_img_info_t));
    ca_img_info_get(0, 0, &info0);

    if (ca_img_active_set(0, info0.active_flag, 1))
    {
        OAM_ORG_LOG("active image id = %d failed \n", info0.active_flag);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}


oam_status ctc_oam_swimage_commit_adapt()
{
    ca_img_info_t info0;
    ca_img_info_t info1;
    ca_uint8_t image_idx = 0;

    memset(&info0, 0, sizeof(ca_img_info_t));
    memset(&info1, 0, sizeof(ca_img_info_t));
    ca_img_info_get(0, 0, &info0);
    ca_img_info_get(0, 1, &info1);

    if (info1.commit_flag && info1.exec_flag)
    {
        image_idx = 1;
    }

    if (ca_img_commit_set(0, image_idx, 1))
    {
        OAM_ORG_LOG("commit image id = %d failed \n", image_idx);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}


oam_status ctc_oam_multi_llid_admin_ctrl_set_adapt(oam_uint32 llid_num)
{

    return OAM_E_OK;
}

static void ctc_oam_eth_port_link_status_handler
(
    ca_device_id_t      device_id,
    ca_uint32_t         event_id,
    ca_void_t *event,
    ca_uint32_t         len,
    ca_void_t           *cookie
    )
{
    ca_event_eth_port_link_t *pinfo = (ca_event_eth_port_link_t *)event;
    oam_port_id_t port;
    oam_boolean   status;

    if (!g_ctc_port_info)
    {
        return OAM_E_PARAM;
    }
    
    OAM_ASSERT(pinfo != NULL);

    port = pinfo->port_id;
    status = pinfo->status;


    port = __OAM_ETH_PORT(port);

    if ((port == OAM_PON_PORT_ID) || (port > CTC_MAX_ONU_UNI_PORTS))
    {
        OAM_LOG_MAJ("oam_handler_port_link_status port %d error!\n", port);
        return;
    }

    g_ctc_port_info[port].lnk_sts_chng_cnt++;
    OAM_LOG_MAJ("port %d, status %d, cnt %u\n", port, status, g_ctc_port_info[port].lnk_sts_chng_cnt);

    if (status == OAM_TRUE)
    {
        ctc_oam_build_alarm_event(CTC_PORT_ALARM, port, CTC_ALARM_PORT_LOS,
            CTC_ALARM_CLEAR, 0);
    }
    else
    {
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

    if ((port == OAM_PON_PORT_ID) || (port > CTC_MAX_ONU_UNI_PORTS))
    {
        OAM_LOG_MAJ("oam_handler_port_link_status port %d error!\n", port);
        return;
    }

    if (status)
    {
        ctc_oam_build_alarm_event(CTC_PORT_ALARM, port, CTC_ALARM_AUTO_NEG_FAIL,
            CTC_ALARM_CLEAR, 0);
    }
    else
    {
        ctc_oam_build_alarm_event(CTC_PORT_ALARM, port, CTC_ALARM_AUTO_NEG_FAIL,
            CTC_ALARM_RAISE, 0);
    }

}

oam_status ctc_oam_eth_port_alarm_reg_adapt()
{
    ca_status_t    status;

    status = ca_event_register(
        0,
        CA_EVENT_ETH_PORT_LINK,
        ctc_oam_eth_port_link_status_handler,
        NULL);
    if (CA_E_OK != status)
    {
        OAM_LOG_INFO("Error1: Eth link status change Event reg failed. ret=0x%x\n",
            status);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

static void
ctc_oam_loop_detect_handle(
    ca_uint8  status,
    ca_uint16 port)
{
    oam_uint64 alarm_info = 0;
    oam_uint8 alarm_op = CTC_ALARM_RAISE;

    /*build CTC event*/
//    if (oam_is_cuc_oam()) {
//        alarm_info = (info ? CUC_OUT_LOOP_VALUE : CUC_INNER_LOOP_VALUE);
//    }

    alarm_op = (status ? CTC_ALARM_RAISE : CTC_ALARM_CLEAR);

    ctc_oam_build_alarm_event(
        CTC_PORT_ALARM,
        OAM_CA_PORT_TO_OAM_PORT(port),
        CTC_ALARM_PORT_LB,
        alarm_op,
        alarm_info);
}

oam_status ctc_oam_ld_alarm_reg_adapt()
{
    ca_status_t ret = CA_E_OK;

    ret = ld_oam_even_send_register(ctc_oam_loop_detect_handle);

    if (ret != CA_E_OK)
        return OAM_E_ERROR;

    return OAM_E_OK;
}

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

oam_status ctc_oam_gpio_alarm_reg_adapt()
{
    //cs_gpio_ctc_alarm_reg(ctc_oam_gpio_alarm_handle);

    return OAM_E_OK;
}

static int
ctc_oam_opm_alarm_handle(
    oam_uint8  type,
    oam_uint16 status)
{
    oam_uint16  alarm_code = 0;
    oam_uint8   alarm_op = CTC_ALARM_RAISE;

    switch (type)
    {
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
        return CA_E_ERROR;
    }

    if (status)
        alarm_op = CTC_ALARM_RAISE;
    else
        alarm_op = CTC_ALARM_CLEAR;

    ctc_oam_build_alarm_event(CTC_PORT_ALARM, 0, alarm_code, alarm_op, 0);

    return CA_E_OK;
}

oam_status ctc_oam_opm_alarm_reg_adapt()
{
    ca_status_t ret = CA_E_OK;

    ddm_event_send_register(ctc_oam_opm_alarm_handle);
    if (ret != CA_E_OK)
        return OAM_E_ERROR;

    return OAM_E_OK;
}

//
// vlan_cnt => 0: untag, 1: single tag, 2: double tag
//
inline
ca_status_t
_ctc_dflt_ingress_vlan_action_add (
    oam_port_id_t oam_port,
    ca_uint8_t    vlan_cnt,
    ca_classifier_vlan_action_t vlan_act,
    ca_uint16_t   vid,
    ca_uint16_t   dot1p)
{
    ca_status_t ret = CA_E_ERROR;
    ca_classifier_key_t      dflt_key;
    ca_classifier_key_mask_t dflt_key_msk;
    ca_classifier_action_t   dflt_act;
    char* vlan_cnt_str[  ]  = {
        "untag",
        "single tag"
    };

    if (vid > 4095 || dot1p > 8)
    {
        return CA_E_PARAM;
    }

    memset(&dflt_key, 0x00, sizeof(ca_classifier_key_t));
    memset(&dflt_key_msk, 0x00, sizeof(ca_classifier_key_mask_t));
    memset(&dflt_act, 0x00, sizeof(ca_classifier_action_t));

    dflt_key_msk.src_port = 1;
//    dflt_key_msk.dest_port = 1;
    dflt_key_msk.l2       = 1;
    dflt_key_msk.l2_mask.vlan_count = 1;
    dflt_key.src_port      = OAM_UNI_PORT_ID_TO_CA_PORT (oam_port);
//  dflt_key.dest_port      = OAM_PON_PORT_ID_TO_CA_PORT ();
    dflt_key.l2.vlan_count = vlan_cnt;
    dflt_act.forward       = CA_CLASSIFIER_FORWARD_FE;
    //
    // vid = 0, disable vid modification 
    //
    if (vid != 0)
    {
        dflt_act.options.masks.outer_vlan_act = 1;
        dflt_act.options.outer_vlan_act       = vlan_act;
        dflt_act.options.outer_vid            = vid;
    }
    //
    // dot1p = 8, disable dot1p remark
    //
    if (dot1p < 8)
    {
        dflt_act.options.masks.outer_dot1p    = 1;
        dflt_act.options.outer_dot1p          = 0;
    }


    dflt_act.options.masks.priority       = 1;
    dflt_act.options.priority             = 0;

    ret = ca_classifier_rule_add(
        0,
        0,
        &dflt_key,
        &dflt_key_msk,
        &dflt_act,
        &g_port_cls_cfg[oam_port].dflt_untg_rule_idx);
    if (ret != CA_E_OK)
    {
        ca_printf(
            "%s: Failed to add default cls rule on oam_port %u for %s\n",
            __FUNCTION__,
            oam_port,
            vlan_cnt_str[vlan_cnt]);
    }
    else
    {
        ca_printf(
            "%s: Add default cls rule [%u] on oam_port %u for %s\nn",
            __FUNCTION__,
            g_port_cls_cfg[oam_port].dflt_untg_rule_idx,
            oam_port,
            vlan_cnt_str[vlan_cnt]);
    }
}

void ctc_oam_init_adapt ()
{
    ca_uint32_t            port           = 0;
    ca_status_t            ret            = CA_E_ERROR;
    ca_device_id_t         dev_id         = 0;
    ca_uint8_t             mbr_cnt        = 0,
                           untagged_cnt   = 0;
    ca_qos_config_t        qos_cfg;
    ca_vlan_port_control_t vlan_p_cntrl;
    ca_port_id_t*          mbr_ports      = NULL;
    ca_port_id_t*          untagged_ports = NULL;

    g_uni_num = oam_uni_port_num_get_adapt ();

    ca_printf ( "%s: There're %u UNIs\n" , __FUNCTION__ , g_uni_num );

    mbr_ports = malloc ( sizeof(ca_port_id_t) * (g_uni_num + 1) );
    if ( !mbr_ports )
    {
        printf ( "%s: Failed to allocate for mbr_ports \n" , __FUNCTION__ );
        goto ERROR_CTC_OAM_INIT_ADAPT;
    }
    untagged_ports = malloc ( sizeof(ca_port_id_t) * (g_uni_num + 1) );
    if ( !untagged_ports )
    {
        printf ( "%s: Failed to allocate for untagged_ports \n" , __FUNCTION__ );
        goto ERROR_CTC_OAM_INIT_ADAPT;
    }

    g_ctc_port_info =
        (ctc_port_info_t*) malloc ( sizeof(ctc_port_info_t) * (g_uni_num + 1) );
    if ( !g_ctc_port_info )
    {
        printf ( "%s: Failed to allocate for g_ctc_port_info\n" , __FUNCTION__ );
        goto ERROR_CTC_OAM_INIT_ADAPT;
    }

    memset ( g_ctc_port_info , 0x00 , sizeof(ctc_port_info_t) * (g_uni_num + 1) );
    memset ( &qos_cfg , 0x00 , sizeof(ca_qos_config_t) );
    memset ( mbr_ports , 0x00 , sizeof(ca_port_id_t) * (g_uni_num + 2) );
    memset ( untagged_ports , 0x00 , sizeof(ca_port_id_t) * (g_uni_num + 2) );
    memset ( &vlan_p_cntrl , 0x00 , sizeof( ca_vlan_port_control_t ) );
    //
    // Flooding unknown traffic to pon port
    //
    ca_l2_vlan_port_control_get (
        0 ,
        OAM_PON_PORT_ID_TO_CA_PORT () ,
        &vlan_p_cntrl );
    vlan_p_cntrl.drop_unknown_vlan = 0;
    ret = ca_l2_vlan_port_control_set (
        0 ,
        OAM_PON_PORT_ID_TO_CA_PORT () ,
        &vlan_p_cntrl );
    if ( ret != CA_E_OK )
    {
        printf (
            "%s: failed to set drop_unknown_vlan = %u on pon port, ret 0x%x\n" ,
            __FUNCTION__ ,
            vlan_p_cntrl.drop_unknown_vlan ,
            ret );
    }
    //
    // Add pon port to member of vid 1
    //
    mbr_ports[mbr_cnt] = OAM_PON_PORT_ID_TO_CA_PORT ();
    mbr_cnt ++;
    //
    // add cpu port
    //
    mbr_ports[mbr_cnt] = CA_PORT_ID ( CA_PORT_TYPE_CPU , CA_PORT_ID_CPU0 );
    mbr_cnt ++;
    untagged_ports[untagged_cnt] = CA_PORT_ID ( CA_PORT_TYPE_CPU , CA_PORT_ID_CPU0 );
    untagged_cnt ++;

    ret = ca_l2_vlan_create ( 0 , 1 );
    if ( ret != CA_E_OK )
    {
        ca_printf ( "%s: Failed to create vid 1, ret 0x%0x\n" , __FUNCTION__ , ret );
        goto ERROR_CTC_OAM_INIT_ADAPT;
    }

    for ( port = 1; port < (g_uni_num + 1); port ++ )
    {
        //
        // Default of vlan mode: Transparent
        //
        _ctc_vlan_mode_reset ( OAM_UNI_PORT_ID_TO_CA_PORT ( port ) );
        ret = ca_qos_config_get (
            0 ,
            OAM_UNI_PORT_ID_TO_CA_PORT ( port ) ,
            &qos_cfg );
        if ( ret == CA_E_OK )
        {
            qos_cfg.qos_map_mode     = CA_QOS_MAP_CLASSIFIER;
            qos_cfg.dot1p_remap_mode = CA_DOT1P_MAP_CLASSIFIER;
            ca_qos_config_set (
                0 ,
                OAM_UNI_PORT_ID_TO_CA_PORT ( port ) ,
                &qos_cfg );
        }

        ret = ca_port_untagged_priority_set (
            0 ,
            OAM_UNI_PORT_ID_TO_CA_PORT ( port ) ,
            0 );
        if ( ret != CA_E_OK )
        {
            ca_printf ( "%s: Failed to set untagged priority on oam_port %u\n" , __FUNCTION__ , port );
        }
        else
        {
            ca_printf ( "%s: Set untagged priority on oam_port %u\n" , __FUNCTION__ , port );
        }

        mbr_ports[mbr_cnt] = OAM_UNI_PORT_ID_TO_CA_PORT ( port );
        mbr_cnt ++;

        untagged_ports[untagged_cnt] = OAM_UNI_PORT_ID_TO_CA_PORT ( port );
        untagged_cnt ++;

        ret = ca_port_pvid_vlan_set ( dev_id , OAM_UNI_PORT_ID_TO_CA_PORT ( port ) , 1 );
        if ( ret != CA_E_OK )
        {
            ca_printf ( "%s: Failed to set pvid 1 on oam_port %u\n" , __FUNCTION__ , port );
        }
    }

    ret = ca_l2_vlan_port_set (
        0 ,
        1 ,
        mbr_cnt ,
        mbr_ports ,
        untagged_cnt ,
        untagged_ports );
    if ( ret != CA_E_OK )
    {
        ca_printf ( "%s: Failed to set vlan 1, ret 0x%x\n" , __FUNCTION__ , ret );
        goto ERROR_CTC_OAM_INIT_ADAPT;
    }

    ca_printf ( "%s: Finished the initialization of CTC OAM\n" , __FUNCTION__ );

    return;

ERROR_CTC_OAM_INIT_ADAPT:
    if ( mbr_ports )
    {
        free ( mbr_ports );
        mbr_ports = NULL;
    }

    if ( untagged_ports )
    {
        free ( untagged_ports );
        untagged_ports = NULL;
    }

    if ( g_ctc_port_info )
    {
        free ( g_ctc_port_info );
        g_ctc_port_info = NULL;
    }

    return;
}
#endif
