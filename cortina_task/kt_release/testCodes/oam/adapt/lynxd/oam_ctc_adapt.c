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
#include "oam_types.h"
#include "plat_common.h"

#include "oam_std_pdu.h"
#include "oam_ctc_pdu.h"
#include "oam_ctc_client.h"
#include "oam_ctc_adapt.h"
#ifdef HAVE_LOOP_DETECT
#include "loop_detect.h"
#endif
#include "sdl_api.h"
#include "oam_ctc_stats.h"
#include "oam_ctc_alarm.h"


#ifdef HAVE_MC_CTRL
#include "mc_api.h"
#include "mc_ctcctrl.h"
#include "mc_core.h"
#endif


#ifdef HAVE_ZTE_OAM
#include "db_instance.h"
#endif

#include "sdl_vlan.h"
#include "port_stats.h"
#include "power_saving.h"
#include "oam_sdl_if.h"

#define TAG2VID(tag)    ((tag)&0x0fff)
#define TAG2PRI(tag)    (((tag)&0xe000)>>13)

extern oam_int32 onu_evt_reg(oam_int32 evt_type , void *func , void *cookie);

#ifdef HAVE_ZTE_OAM
oam_status ctc_oam_cls_syn_db (oam_port_id_t port);
oam_status ctc_oam_mc_vlan_syn_db (oam_port_id_t port);
oam_status ctc_oam_onu_alarm_syn_db (  );
oam_status ctc_oam_port_alarm_syn_db (oam_port_id_t port);
#endif

static oam_uint32 g_laser_tx_power_timer = 0;

static void __laser_tx_power_on(void *data)
{
    cs_callback_context_t   context;
    memset(&context, 0, sizeof(context));
    epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, 1);
    g_laser_tx_power_timer = 0;
}

oam_status oam_port_event_register_adapt (
    void                        *call_back)
{
    onu_evt_reg(EPON_EVENT_PORT_LINK_CHANGE, (void *)call_back, NULL);
    return OAM_E_OK;
}



/***************** SDL ADAPT START ****************************/
oam_status ctc_oam_eth_admin_state_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin)
{
    //TODO:not completed
    /* admin
    #define OAM_INTF_PHY_ADMIN_DIS                  1
    #define OAM_INTF_PHY_ADMIN_EN                   2
     */
    cs_status ret;
    cs_sdl_port_admin_t port_admin = 0;
    cs_callback_context_t context;

    ret = epon_request_onu_port_admin_get(context,
                                          ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &port_admin);

    if (ret) {
        return ret;
    }

    if (port_admin == SDL_PORT_ADMIN_DOWN) {
        *admin = OAM_INTF_PHY_ADMIN_DIS;
    }
    else {
        *admin = OAM_INTF_PHY_ADMIN_EN;
    }

    return OAM_E_OK;
}


//duchen: OAM_INTF_PHY_ADMIN_DIS?????
oam_status ctc_oam_eth_auto_admin_get_adapt(
    oam_port_id_t port,
    oam_uint8  *admin)
{
    //TODO:not completed
    /* admin
    #define OAM_INTF_PHY_ADMIN_DIS                  1
    #define OAM_INTF_PHY_ADMIN_EN                   2
     */
    cs_status ret;
    cs_sdl_port_autoneg_status_t auto_neg = 0;
    cs_callback_context_t context;

    ret = epon_request_onu_port_auto_neg_get(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &auto_neg);

    if (ret) {
        return ret;
    }

    if (auto_neg == SDL_PORT_ADMIN_DOWN) {
        *admin = OAM_INTF_PHY_ADMIN_DIS;
    }
    else {
        *admin = OAM_INTF_PHY_ADMIN_EN;
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
    //TODO:not completed
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

    cs_callback_context_t     context;
    cs_status                 retCode;
    oam_boolean                enable;

    retCode =  epon_request_onu_fec_ability_get(context,
               ONU_DEVICEID_FOR_API,
               ONU_LLIDPORT_FOR_API,
               &enable);

    if (enable == 0)
        *ability = OAM_INTF_FEC_NOT_SUPPORTED;
    else if (enable == 1)
        *ability = OAM_INTF_FEC_SUPPORDTED;
    else
        *ability = OAM_INTF_FEC_ABILITY_UNKOWN;

    return retCode;

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

    cs_callback_context_t     context;
    oam_boolean                mode;
    cs_status                retCode;
#ifdef NEED_SAVE
    db_zte_onu_svc_t          svc;
#endif

    if (fecMode == OAM_INTF_FEC_MODE_EN)
        mode = 1;
    else
        mode = 0;

    retCode =  epon_request_onu_fec_set(context,
                                        ONU_DEVICEID_FOR_API,
                                        ONU_LLIDPORT_FOR_API,
                                        mode);
#ifdef NEED_SAVE

    if ( retCode == CS_E_OK ) {
        memset(&svc, 0x00, sizeof(svc));
        db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
        svc.fec_mode = mode;
        db_write(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    }

#endif
    return retCode;

}

oam_status ctc_oam_eth_fec_mode_get_adapt(
    oam_uint32 *fecMode)
{
    //TODO:not completed
    /*
    *fecMode = OAM_INTF_FEC_MODE_EN;
    return CS_E_OK;
    */
    cs_callback_context_t     context;
    oam_boolean                mode;
    cs_status                 retCode;

    retCode =  epon_request_onu_fec_get(context,
                                        ONU_DEVICEID_FOR_API,
                                        ONU_LLIDPORT_FOR_API,
                                        &mode);


    if (mode == 1)
        *fecMode = OAM_INTF_FEC_MODE_EN;
    else if (mode == 0)
        *fecMode = OAM_INTF_FEC_MODE_DIS;
    else
        *fecMode = OAM_INTF_FEC_MODE_UNKOWN;

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
    //TODO:not completed
    /* linkState
     #define OAM_CTC_ONU_ETH_DOWN                0x0
     #define OAM_CTC_ONU_ETH_UP                  0x1
     */
    cs_status ret;
    cs_sdl_port_link_status_t    link_status = 0;
    cs_callback_context_t context;

    ret = epon_request_onu_port_link_status_get(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &link_status);

    if (ret) {
        return ret;
    }

    if (link_status == SDL_PORT_LINK_STATUS_DOWN) {
        *linkState = OAM_CTC_ONU_ETH_DOWN;
    }
    else {
        *linkState = OAM_CTC_ONU_ETH_UP;
    }

    return OAM_E_OK;

}


oam_status ctc_oam_eth_port_pause_get_adapt(
    oam_port_id_t port,
    oam_uint8  *flowControl)
{
    cs_status ret;
    cs_callback_context_t context;
    oam_boolean enable;

    ret = epon_request_onu_port_flow_ctrl_get(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &enable);

    *flowControl = enable;

    return ret;

}

oam_status ctc_oam_eth_port_pause_set_adapt(
    oam_port_id_t port,
    oam_uint8 flowControl)
{
    cs_status ret;
    cs_callback_context_t context;
    oam_boolean enable;
#ifdef HAVE_ZTE_OAM
    db_zte_port_t port_info;
#endif
    enable = flowControl > 0 ? 1 : 0;
    ret = epon_request_onu_port_flow_ctrl_set(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, enable);
#ifdef HAVE_ZTE_OAM
    memset(&port_info, 0x00, sizeof(port_info));
    db_read(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
    port_info.port[port - 1].pause = enable;
    db_write(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
#endif
    return ret;

}



oam_status ctc_oam_eth_admin_state_set_adapt(
    oam_port_id_t port,
    oam_uint32 phyAdmin)
{
    cs_status ret;
    cs_sdl_port_admin_t port_admin = 0;
    cs_callback_context_t context;
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE
    db_zte_port_t port_info;
#endif
#endif


    if (phyAdmin == OAM_INTF_PHY_ADMIN_DIS) {
        port_admin = SDL_PORT_ADMIN_DOWN;
    }
    else {
        port_admin = SDL_PORT_ADMIN_UP;
    }

    ret = epon_request_onu_port_admin_set(context,
                                          ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, port_admin);

#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE
    memset(&port_info, 0x00, sizeof(port_info));
    db_read(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
    port_info.port[port - 1].admin = port_admin;
    db_write(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
#endif
#endif


    return ret;
}


oam_status ctc_oam_auto_neg_restart_set_adapt(
    oam_port_id_t port)
{
    cs_status ret;
    cs_callback_context_t context;

    ret = epon_request_onu_port_autoneg_restart(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port);

    return ret;
}

oam_status ctc_oam_auto_neg_ctrl_set_adapt(
    oam_port_id_t port,
    oam_uint32 autoNegAdmin)
{
    oam_status ret;
    cs_sdl_port_speed_cfg_t cfg = 0;
    cs_callback_context_t context;
    oam_uint8  auto_neg = SDL_PORT_AUTONEG_DOWN;
#ifdef HAVE_ZTE_OAM
    db_zte_port_t   port_info;
#endif
    ret = epon_request_onu_port_status_get(context,
                                           ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &cfg);

    if (ret) {
        return ret;
    }

    if (autoNegAdmin == OAM_INTF_PHY_ADMIN_DIS) {
        if (cfg == SDL_PORT_AUTO_10_100_1000 ||
            cfg == SDL_PORT_AUTO_10_100 ||
            cfg == SDL_PORT_1000_FULL) {
            cfg = SDL_PORT_100_FULL;
            ret = epon_request_onu_port_status_set(context,
                                                   ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, cfg);
        }

        auto_neg = SDL_PORT_AUTONEG_DOWN;
    }
    else {
        if (cfg != SDL_PORT_AUTO_10_100_1000 &&
            cfg != SDL_PORT_AUTO_10_100 &&
            cfg != SDL_PORT_1000_FULL) {
            cfg = SDL_PORT_AUTO_10_100_1000;
            ret = epon_request_onu_port_status_set(context,
                                                   ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, cfg);
        }

        auto_neg = SDL_PORT_AUTONEG_UP;
    }

#ifdef HAVE_ZTE_OAM

    if (ret == CS_E_OK) {
        if (port <= DB_ZTE_MAX_PORT && port > 0) {
            memset(&port_info, 0x00, sizeof(port_info));
            db_read(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
            port_info.port[port - 1 ].auto_neg = auto_neg;
            db_write(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
        }
    }

#endif
    return ret;
}

oam_status ctc_oam_eth_port_policing_get_adapt(
    oam_port_id_t port,
    oam_uint8  *oper,
    oam_uint32 *cir,
    oam_uint32 *cbs,
    oam_uint32 *ebs)
{
    cs_status ret;
    cs_callback_context_t context;
    cs_sdl_policy_t       policy;

    ret = epon_request_onu_port_policy_get(context,
                                           ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &policy);

    if (ret) {
        return ret;
    }

    *oper = policy.enable;
    *cir = policy.rate;
    *cbs = policy.cbs;
    *ebs = policy.ebs;

    return OAM_E_OK;

}

oam_status ctc_oam_eth_port_policing_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  cbs,
    oam_uint32  ebs)
{
    cs_status ret;
    cs_callback_context_t context;
    cs_sdl_policy_t       policy;
#ifdef HAVE_ZTE_OAM
    db_zte_port_t   port_info;
#endif

    policy.rate = cir;
    policy.cbs = cbs;
    policy.ebs = ebs;

    if (oper == CTC_PORT_POLICY_OPER_DISABLE) {
        policy.enable = 0;
    }
    else {
        policy.enable = 1;
    }

    ret = epon_request_onu_port_policy_set(context,
                                           ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &policy);
#ifdef HAVE_ZTE_OAM
    memset(&port_info, 0x00, sizeof(port_info));
    db_read(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
    port_info.port[port - 1 ].policy_rate = cir;
    port_info.port[port - 1 ].policy_cbs = cbs;
    port_info.port[port - 1 ].policy_ebs = cbs;
    port_info.port[port - 1 ].policy_enable = policy.enable;
    db_write(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
#endif

    return ret;

}


oam_status ctc_oam_eth_ds_rate_limit_get_adapt(
    oam_port_id_t port,
    oam_uint8 *oper,
    oam_uint32 *cir,
    oam_uint32 *pir)
{
    cs_status ret;
    cs_callback_context_t context;
    cs_sdl_policy_t       rate;

    ret = epon_request_onu_port_ds_rate_limit_get(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &rate);

    if (ret) {
        return ret;
    }

    *oper = rate.enable;
    *cir = rate.rate;
    *pir = rate.cbs;

    return CS_E_OK;

}

oam_status ctc_oam_eth_ds_rate_limit_set_adapt(
    oam_port_id_t port,
    oam_uint8   oper,
    oam_uint32  cir,
    oam_uint32  pir)
{
    cs_status                   ret;
    cs_callback_context_t context;
    cs_sdl_policy_t           rate;
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE
    db_zte_port_t             port_info;
#endif
#endif


    rate.rate = cir;
    rate.cbs = pir;
    rate.ebs = 0;

    if (oper == CTC_PORT_POLICY_OPER_DISABLE) {
        rate.enable = 0;
    }
    else {
        rate.enable = 1;
    }

    ret = epon_request_onu_port_ds_rate_limit_set(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &rate);

#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if (ret == CS_E_OK) {
        if (port <= DB_ZTE_MAX_PORT && port > 0) {
            memset(&port_info, 0x00, sizeof(port_info));
            db_read(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
            port_info.port[port - 1 ].ds_rate_limit_enable = rate.enable;
            port_info.port[port - 1 ].ds_rate_limit =  rate.rate;
            port_info.port[port - 1 ].ds_cbs =   rate.cbs;

            db_write(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
        }
    }

#endif
#endif


    return ret;
}

oam_status ctc_oam_onu_mac_limit_set_adapt(
    oam_port_id_t port,
    oam_uint16  num)
{
    cs_callback_context_t context;
    cs_status rt = CS_E_OK;

#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE
    db_zte_mac_limit_t    mac_limit_cfg;
#endif
#endif

    rt = epon_request_onu_fdb_mac_limit_set(context, 0, 0, port, num);

#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if (rt == CS_E_OK
        && port <= DB_ZTE_MAX_PORT
        && port > 0) {
        memset(&mac_limit_cfg, 0x00, sizeof(db_zte_mac_limit_t));
        db_read(DB_ZTE_MAC_LIMIT_ID , (oam_uint8 *) &mac_limit_cfg,
                sizeof(db_zte_mac_limit_t));
        mac_limit_cfg.mac[port - 1].num = num;

        db_write(DB_ZTE_MAC_LIMIT_ID , (oam_uint8 *) &mac_limit_cfg,
                 sizeof(db_zte_mac_limit_t));
    }

#endif
#endif

    return rt;
}

oam_status ctc_oam_onu_mac_limit_get_adapt(
    oam_port_id_t port,
    oam_uint16  *num)
{

    cs_callback_context_t context;
    oam_uint32 number;
    cs_status rt = CS_E_OK;

    rt = epon_request_onu_fdb_mac_limit_get(context, 0, 0, port, &number);

    *num = number & 0xffff;

    return rt;

}


oam_status ctc_oam_onu_port_mode_set_adapt(
    oam_port_id_t port,
    oam_uint8  mode)
{
    cs_status ret;
    cs_sdl_port_speed_cfg_t cfg = 0;
    cs_callback_context_t context;


    switch (mode) {
        case CTC_OAM_PORT_MODE_10M_HALF:
            cfg = SDL_PORT_10_HALF;
            break;

        case CTC_OAM_PORT_MODE_10M_FULL:
            cfg = SDL_PORT_10_FULL;
            break;

        case CTC_OAM_PORT_MODE_100M_HALF:
            cfg = SDL_PORT_100_HALF;
            break;

        case CTC_OAM_PORT_MODE_100M_FULL:
            cfg = SDL_PORT_100_FULL;
            break;

        case CTC_OAM_PORT_MODE_1000M_FULL:
            cfg = SDL_PORT_1000_FULL;
            break;

        default:
            return CS_E_PARAM;
    }

    ret = epon_request_onu_port_status_set(context,
                                           ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, cfg);

    return ret;

}


oam_status ctc_oam_onu_port_stats_get_adapt(
    oam_port_id_t port,
    oam_ctc_eth_port_stats_t *stats)
{
    oam_uni_stats_t uni_stats;

    if (!stats)
        return CS_E_PARAM;

    memset(&uni_stats, 0, sizeof(uni_stats));

    if (CS_E_OK != app_onu_port_stats_get(port, &uni_stats)) {
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

    return CS_E_OK;
}


oam_status ctc_oam_pon_port_stat_get_adapt(
    oam_ctc_onu_llid_stats1 *stats1,
    oam_ctc_onu_llid_stats2 *stats2)
{
    oam_pon_stats_t pon_stats;

    if (!stats1 && !stats2) {
        return CS_E_PARAM;
    }

    memset(&pon_stats, 0, sizeof(pon_stats));

    app_onu_pon_stats_get(&pon_stats);

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
        cs_callback_context_t     context;
        cs_sdl_fec_cnt_t   fec_cnt;
        epon_request_onu_fec_cnt_get(context, ONU_DEVICEID_FOR_API,
                                     ONU_LLIDPORT_FOR_API, &fec_cnt);
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

    return OAM_E_OK;
}
oam_status ctc_oam_onu_port_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8  *mode)
{
    cs_status ret;
    cs_sdl_port_speed_cfg_t cfg = 0;
    cs_callback_context_t context;

    ret = epon_request_onu_port_status_get(context,
                                           ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &cfg);

    if (ret) {
        return ret;
    }

    switch (cfg) {
        case SDL_PORT_AUTO_10_100_1000:
        case SDL_PORT_AUTO_10_100:
            *mode = CTC_OAM_PORT_MODE_AUTO;
            break;

        case SDL_PORT_10_HALF:
            *mode = CTC_OAM_PORT_MODE_10M_HALF;
            break;

        case SDL_PORT_10_FULL:
            *mode = CTC_OAM_PORT_MODE_10M_FULL;
            break;

        case SDL_PORT_100_HALF:
            *mode = CTC_OAM_PORT_MODE_100M_HALF;
            break;

        case SDL_PORT_100_FULL:
            *mode = CTC_OAM_PORT_MODE_100M_FULL;
            break;

        case SDL_PORT_1000_FULL:
            *mode = CTC_OAM_PORT_MODE_1000M_FULL;
            break;

        default:
            break;
    }

    return ret;
}


oam_status ctc_oam_onu_port_work_mode_get_adapt(
    oam_port_id_t portId,
    oam_uint8  *mode)
{
    cs_sdl_port_link_status_t link_statu;
    cs_sdl_port_ether_speed_t port_speed;
    cs_sdl_port_ether_duplex_t port_duplex;
    cs_status rc = CS_E_OK;
    cs_callback_context_t context;

    rc = epon_request_onu_port_link_status_get(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, portId, &link_statu);

    if (rc) {
        goto end;
    }

    if (link_statu == SDL_PORT_LINK_STATUS_DOWN) {
        *mode = CTC_OAM_PORT_MODE_10M_HALF;
    }
    else {
        rc = epon_request_onu_port_speed_get(context,
                                             ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, portId, &port_speed);

        if (rc) {
            goto end;
        }

        rc = epon_request_onu_port_duplex_get(context,
                                              ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, portId, &port_duplex);

        if (rc) {
            goto end;
        }

        switch (port_speed) {
            case SDL_PORT_SPEED_10:
                if (port_duplex == SDL_PORT_DUPLEX_FULL) {
                    *mode = CTC_OAM_PORT_MODE_10M_FULL;
                }
                else {
                    *mode = CTC_OAM_PORT_MODE_10M_HALF;
                }

                break;

            case SDL_PORT_SPEED_100:
                if (port_duplex == SDL_PORT_DUPLEX_FULL) {
                    *mode = CTC_OAM_PORT_MODE_100M_FULL;
                }
                else {
                    *mode = CTC_OAM_PORT_MODE_100M_HALF;
                }

                break;

            case SDL_PORT_SPEED_1000:
                *mode = CTC_OAM_PORT_MODE_1000M_FULL;
                break;
        }

    }

end:
    return rc;
}

oam_status ctc_oam_eth_mac_aging_time_get_adapt(
    oam_uint32 *time)
{

    cs_callback_context_t context;
    cs_status ret = CS_E_OK;

    ret = epon_request_onu_fdb_age_get(context,
                                       ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, time);

    if (ret) {
        return ret;
    }

    return OAM_E_OK;
}

oam_status ctc_oam_eth_mac_aging_time_set_adapt(
    oam_uint32 time)
{
    cs_callback_context_t context;
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE
    db_zte_onu_svc_t   svc;
#endif
#endif
    cs_status ret = CS_E_OK;

    ret = epon_request_onu_fdb_age_set(context,
                                       ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API,  time);

#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if (ret == CS_E_OK) {
        memset(&svc, 0x00, sizeof(svc));
        db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
        svc.mac_aging_time = time;
        db_write(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    }

#endif
#endif

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
    mode[5] = 0x3; /* fast_leave at mld snoop mode */
    return CS_E_OK;
}

oam_status ctc_oam_onu_fast_leave_state_get_adapt(
    oam_uint32 *state)
{

#ifdef HAVE_MC_CTRL
    *state = mc_get_fast_leave_state();
#endif

    return OAM_E_OK;
}

oam_status ctc_oam_fast_leave_ctrl_set_adapt(
    oam_uint32 fastLeaveAdmin)
{
#ifdef HAVE_MC_CTRL
    cs_status             ret = CS_E_OK;
#ifdef NEED_SAVE
    db_zte_fast_leave_t   db_fast_leave;
#endif
    ret = mc_set_fast_leave_state(fastLeaveAdmin);
#ifdef NEED_SAVE

    if ( ret == CS_E_OK ) {
        db_fast_leave.fast_leave = fastLeaveAdmin;
        db_write ( DB_ZTE_FAST_LEAVE_ID , (oam_uint8 *) &db_fast_leave,
                   sizeof( db_zte_fast_leave_t));
    }

#endif
#endif
    return ret;
}



oam_status ctc_oam_onucap_get_adapt(
    oam_ctc_onu_capabilities_t *onu_cap)
{
    oam_uint8 hasBattery = 1;
    oam_uint8 numUSQueues, maxQueueUSPort;
    oam_uint8 numDSQueues, maxQueueDSPort;
    oam_uint64 portBitmap = 0;
    oam_uint8 i = 0;

    startup_config_read(CFG_ID_BAK_BATTERY_MODE, 1, &hasBattery);

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
    oam_pwrs_ctrl_t ctrl;

    ctrl.sleep_duration = sleep_duration;
    ctrl.sleep_flag = sleep_flag;
    ctrl.sleep_mode = sleep_mode;
    ctrl.wait_duration = wait_duration;

    return pwrs_pkgc_control_set(&ctrl);
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

static oam_uint32 g_ctc_oam_cfg_counter = 0 ;

oam_status ctc_oam_onu_cfg_counter_get_adapt(oam_uint32 *counter)
{
#ifdef HAVE_ZTE_OAM
    db_zte_onu_svc_t   svc;

    db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    g_ctc_oam_cfg_counter = svc.cfg_counter;
#endif
    *counter = g_ctc_oam_cfg_counter;
    return OAM_E_OK;
}
oam_status ctc_oam_onu_cfg_counter_set_adapt(oam_uint32 counter)
{
#ifdef HAVE_ZTE_OAM
    db_zte_onu_svc_t   svc;

    db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    svc.cfg_counter = counter;
    db_write(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    db_save_to_flash();
#endif
    g_ctc_oam_cfg_counter = counter;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_cfg_mgmt_set_adapt(oam_uint8 action)
{
#ifdef HAVE_ZTE_OAM

    switch (action) {
        case 1:
            db_save_to_flash();
            break;

        case 2:
            db_zte_set_default();
            db_save_to_flash();
            db_restore();

        case 3:
            db_zte_set_default();
            db_save_to_flash();
            break;

        default:
            return CS_E_ERROR;
    }

#endif
    return OAM_E_OK;
}



static oam_uint8 g_holdover_state = OAM_CTC_ONU_HOLDOVER_DEACTIVED;
static oam_uint32 g_holdover_time = 0;

oam_status ctc_oam_holdover_get_adapt(
    oam_uint32 *holdover_state,
    oam_uint32 *holdover_time)
{
    cs_status ret = CS_E_OK;
    oam_uint8 gate_timeout = 0;


    if (holdover_time == NULL
        || holdover_state == NULL) {
        return CS_E_PARAM;
    }

    /* not initialization, get it from start config */
    if (g_holdover_time == 0) {
        startup_config_read(CFG_ID_MPCP_TIMEOUT, 1, &gate_timeout);
        g_holdover_time = gate_timeout * 10;
    }

    *holdover_state = g_holdover_state;
    *holdover_time  = g_holdover_time;

    return ret;
}

oam_status ctc_oam_holdover_set_adapt(
    oam_uint32 holdover_state,
    oam_uint32 holdover_time)
{
    cs_callback_context_t context;
    oam_uint8 gate_timeout = 0;
    cs_status ret = CS_E_OK;

    if (holdover_state == OAM_CTC_ONU_HOLDOVER_DEACTIVED ) {
        startup_config_read(CFG_ID_MPCP_TIMEOUT, 1, &gate_timeout);

        if (pwrs_sleep_state_get()) {
            ret = epon_request_onu_mpcp_timer_set(context, 0, 0, 0, gate_timeout * 10);
        }
        else {
            ret = epon_request_onu_mpcp_timer_set(context, 0, 0, 1, gate_timeout * 10);
        }
    }
    else if (holdover_state == OAM_CTC_ONU_HOLDOVER_ACTIVED
             && holdover_time >= OAM_CTC_ONU_HOLDOVER_TIME_MIN
             && holdover_time <= OAM_CTC_ONU_HOLDOVER_TIME_MAX ) {
        ret = epon_request_onu_mpcp_timer_set(context, 0, 0, 0, holdover_time);
    }
    else {
        return CS_E_PARAM;
    }

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
    cs_status rt;
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode;
    cs_sdl_vlan_cfg_t    vlan_cfg[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16 nums;

    rt = epon_request_onu_vlan_get(context, 0, 0, port, &def_vlan, &vlan_mode,
                                   vlan_cfg, &nums);

    if (rt != CS_E_OK)
        return rt;

    *tpid = CTC_DEFAULT_TPID;
    *tag = def_vlan.vid | def_vlan.pri << 13;
    return CS_E_OK;
}


oam_status ctc_oam_onu_vlan_mode_get_adapt(
    oam_port_id_t port,
    oam_uint8 *mode)
{
    cs_status rt;
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode;
    cs_sdl_vlan_cfg_t    vlan_cfg[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16 nums;

    rt = epon_request_onu_vlan_get(context, 0, 0, port, &def_vlan, &vlan_mode,
                                   vlan_cfg, &nums);

    if (rt != CS_E_OK)
        return rt;

    switch (vlan_mode) {
        case SDL_VLAN_MODE_TRANSPARENT:
            *mode = CTC_VLAN_MODE_TRANSPARENT;
            break;

        case SDL_VLAN_MODE_TAG:
            *mode = CTC_VLAN_MODE_TAG;
            break;

        case SDL_VLAN_MODE_TRANSLATION:
            *mode = CTC_VLAN_MODE_TRANSLATE;
            break;

        case SDL_VLAN_MODE_AGGREGATION:
            *mode = CTC_VLAN_MODE_AGG;
            break;

        case SDL_VLAN_MODE_TRUNK:
            *mode = CTC_VLAN_MODE_TRUNK;
            break;

        case SDL_VLAN_MODE_STACKING:
            *mode = ZTE_VLAN_MODE_STACK;

        default:
            return CS_E_ERROR;
    }

    return CS_E_OK;
}

oam_status ctc_oam_onu_translate_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *translate_num,
    oam_ctc_onu_vlan_xlate_entry *vlan)
{
    cs_status rt;
    int i = 0;
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode;
    cs_sdl_vlan_cfg_t    vlan_cfg[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16 nums;

    rt = epon_request_onu_vlan_get(context, 0, 0, port, &def_vlan, &vlan_mode,
                                   vlan_cfg, &nums);

    if (rt != CS_E_OK)
        return rt;

    while (i < nums) {
        vlan[i].tag2.tpid = CTC_DEFAULT_TPID;
        vlan[i].tag1.tpid = CTC_DEFAULT_TPID;
        vlan[i].tag2.tag = vlan_cfg[i].s_vlan.vid;
        vlan[i].tag1.tag = vlan_cfg[i].c_vlan.vid;
        ++i;
    }

    *translate_num = nums;

    return CS_E_OK;
}

oam_status ctc_oam_onu_trunk_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *trunk_num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    cs_status rt;
    int i = 0;
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode;
    cs_sdl_vlan_cfg_t    vlan_cfg[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16 nums;

    rt = epon_request_onu_vlan_get(context, 0, 0, port, &def_vlan, &vlan_mode,
                                   vlan_cfg, &nums);

    if (rt != CS_E_OK)
        return rt;

    while (i < nums) {
        vlan[i].tpid = CTC_DEFAULT_TPID;
        vlan[i].tag = vlan_cfg[i].c_vlan.vid;
        ++i;
    }

    *trunk_num = nums;

    return CS_E_OK;
}

oam_status ctc_oam_onu_agg_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *agg_num,
    ctc_oam_agg_vlan_t *vlan)
{
    cs_status rt;
    int i = 0;
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode;
    cs_sdl_vlan_cfg_t    vlan_cfg[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16 nums;

    rt = epon_request_onu_vlan_get(context, 0, 0, port, &def_vlan, &vlan_mode,
                                   vlan_cfg, &nums);

    if (rt != CS_E_OK)
        return rt;

    while (i < nums) {
        vlan[0].agg_vlan[i].tpid = CTC_DEFAULT_TPID;
        vlan[0].agg_vlan[i].tag = vlan_cfg[i].c_vlan.vid;
        ++i;
    }

    vlan[0].agg_dst_vlan.tpid = CTC_DEFAULT_TPID;
    vlan[0].agg_dst_vlan.tag = vlan_cfg[0].s_vlan.vid;
    vlan[0].agg_vlan_num = nums;

    *agg_num = 1;

    return CS_E_OK;
}

oam_status ctc_oam_onu_transparent_vlan_set_adapt(
    oam_port_id_t port)
{
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode = SDL_VLAN_MODE_TRANSPARENT;
    cs_status ret = CS_E_OK;
#ifdef HAVE_ZTE_OAM
    db_zte_vlan_t vlan;
#endif

    def_vlan.vid = 0;
    def_vlan.pri = 0;

    ret = epon_request_onu_vlan_set(context, 0, 0, port, def_vlan, vlan_mode, NULL,
                                    0);

#ifdef HAVE_ZTE_OAM

    if (ret == CS_E_OK
        && port <= DB_ZTE_MAX_PORT
        && port > 0) {
        memset(&vlan, 0x00, sizeof(vlan));
        db_read(DB_ZTE_VLAN_ID, (oam_uint8 *) &vlan, sizeof(vlan));
        vlan.vlan[port - 1].vlan_mode = vlan_mode;
        db_write(DB_ZTE_VLAN_ID, (oam_uint8 *) &vlan, sizeof(vlan));
    }

#endif
    return ret;
}

oam_status ctc_oam_onu_tag_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t vlan)
{
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode = SDL_VLAN_MODE_TAG;
    cs_status ret = CS_E_OK;
#ifdef HAVE_ZTE_OAM
    db_zte_vlan_t vlan_cfg;
#endif
    def_vlan.vid = TAG2VID(vlan.tag);
    def_vlan.pri = TAG2PRI(vlan.tag);

    ret = epon_request_onu_vlan_set(context, 0, 0, port, def_vlan, vlan_mode, NULL,
                                    0);

#ifdef HAVE_ZTE_OAM

    if (ret == CS_E_OK
        && port <= DB_ZTE_MAX_PORT
        && port > 0) {
        memset(&vlan_cfg, 0x00, sizeof(vlan_cfg));
        db_read(DB_ZTE_VLAN_ID, (oam_uint8 *) &vlan_cfg, sizeof(vlan_cfg));
        vlan_cfg.vlan[port - 1].vlan_mode = vlan_mode;
        vlan_cfg.vlan[port - 1].def_vlan.tpid = vlan.tpid;
        vlan_cfg.vlan[port - 1].def_vlan.tag = vlan.tag;
        db_write(DB_ZTE_VLAN_ID, (oam_uint8 *) &vlan_cfg, sizeof(vlan_cfg));
    }

#endif
    return ret;
}

oam_status ctc_oam_onu_translate_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_xlate_entry *vlan)
{
    int i;
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode = SDL_VLAN_MODE_TRANSLATION;
    cs_sdl_vlan_cfg_t   sdl_vlan[CTC_ONU_AGGR_VLAN_NUM];
    cs_status ret = CS_E_OK;
#ifdef HAVE_ZTE_OAM
    db_zte_vlan_t vlan_cfg;
#endif

    def_vlan.vid = TAG2VID(defvlan.tag);
    def_vlan.pri = TAG2PRI(defvlan.tag);

    for (i = 0; i < num; ++i) {
        sdl_vlan[i].s_vlan.vid = vlan[i].tag2.tag;
        sdl_vlan[i].c_vlan.vid = vlan[i].tag1.tag;
    }

    ret =  epon_request_onu_vlan_set(context, 0, 0, port, def_vlan, vlan_mode,
                                     sdl_vlan, num);
#ifdef HAVE_ZTE_OAM

    if (ret == CS_E_OK
        && port <= DB_ZTE_MAX_PORT
        && port > 0) {
        memset(&vlan_cfg, 0x00, sizeof(vlan_cfg));
        db_read(DB_ZTE_VLAN_ID, (oam_uint8 *) &vlan_cfg, sizeof(vlan_cfg));
        vlan_cfg.vlan[port - 1].vlan_mode = vlan_mode;
        vlan_cfg.vlan[port - 1].def_vlan.tpid = defvlan.tpid;
        vlan_cfg.vlan[port - 1].def_vlan.tag = defvlan.tag;
        memset(vlan_cfg.vlan[port - 1 ].vlan, 0x00,
               sizeof(vlan_cfg.vlan[port - 1 ].vlan));
        vlan_cfg.vlan[port - 1 ].vlan_num = num * 2;

        for (i = 0 ; i < num; i ++) {
            vlan_cfg.vlan[port - 1 ].vlan[i * 2].tpid = vlan[i].tag1.tpid;
            vlan_cfg.vlan[port - 1 ].vlan[i * 2].tag = vlan[i].tag1.tag;
            vlan_cfg.vlan[port - 1 ].vlan[i * 2 + 1].tpid = vlan[i].tag2.tpid;
            vlan_cfg.vlan[port - 1 ].vlan[i * 2 + 1].tag = vlan[i].tag2.tag;
        }

        db_write(DB_ZTE_VLAN_ID, (oam_uint8 *) &vlan_cfg, sizeof(vlan_cfg));
    }

#endif
    return ret;
}


oam_status ctc_oam_onu_trunk_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    int i;
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode = SDL_VLAN_MODE_TRUNK;
    cs_sdl_vlan_cfg_t   sdl_vlan[CTC_ONU_AGGR_VLAN_NUM];
    cs_status ret = CS_E_OK;
#ifdef HAVE_ZTE_OAM
    db_zte_vlan_t vlan_cfg;
#endif
    def_vlan.vid = TAG2VID(defvlan.tag);
    def_vlan.pri = TAG2PRI(defvlan.tag);

    for (i = 0; i < num; ++i) {
        sdl_vlan[i].s_vlan.vid = vlan[i].tag;
        sdl_vlan[i].c_vlan.vid = vlan[i].tag;
    }

    ret =  epon_request_onu_vlan_set(context, 0, 0, port, def_vlan, vlan_mode,
                                     sdl_vlan, num);
#ifdef HAVE_ZTE_OAM

    if (ret == CS_E_OK
        && port <= DB_ZTE_MAX_PORT
        && port > 0) {
        memset(&vlan_cfg, 0x00, sizeof(vlan_cfg));
        db_read(DB_ZTE_VLAN_ID, (oam_uint8 *) &vlan_cfg, sizeof(vlan_cfg));
        vlan_cfg.vlan[port - 1].vlan_mode = vlan_mode;
        vlan_cfg.vlan[port - 1].def_vlan.tpid = defvlan.tpid;
        vlan_cfg.vlan[port - 1].def_vlan.tag = defvlan.tag;
        memset(vlan_cfg.vlan[port - 1 ].vlan, 0x00,
               sizeof(vlan_cfg.vlan[port - 1 ].vlan));
        vlan_cfg.vlan[port - 1 ].vlan_num = num;

        for (i = 0 ; i < num; i ++) {
            vlan_cfg.vlan[port - 1 ].vlan[i].tpid = vlan[i].tpid;
            vlan_cfg.vlan[port - 1 ].vlan[i].tag = vlan[i].tag;
        }

        db_write(DB_ZTE_VLAN_ID, (oam_uint8 *) &vlan_cfg, sizeof(vlan_cfg));
    }

#endif
    return ret;
}

oam_status ctc_oam_onu_agg_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t defvlan,
    oam_uint16 num,
    ctc_oam_agg_vlan_t *vlan)
{
    int i;
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode = SDL_VLAN_MODE_AGGREGATION;
    cs_sdl_vlan_cfg_t   sdl_vlan[CTC_ONU_AGGR_VLAN_NUM];
    cs_status ret = CS_E_OK;
#ifdef HAVE_ZTE_OAM
    db_zte_vlan_t vlan_cfg;
#endif

    if (num > 1) {
        return CS_E_NOT_SUPPORT;
    }

    def_vlan.vid = TAG2VID(defvlan.tag);
    def_vlan.pri = TAG2PRI(defvlan.tag);

    for (i = 0; i < vlan[0].agg_vlan_num; ++i) {
        sdl_vlan[i].c_vlan.vid = vlan[0].agg_vlan[i].tag;
        sdl_vlan[i].s_vlan.vid = vlan[0].agg_dst_vlan.tag;
    }

    ret =  epon_request_onu_vlan_set(context, 0, 0, port, def_vlan, vlan_mode,
                                     sdl_vlan, vlan[0].agg_vlan_num);

#ifdef HAVE_ZTE_OAM

    if (ret == CS_E_OK
        && port <= DB_ZTE_MAX_PORT
        && port > 0) {
        memset(&vlan_cfg, 0x00, sizeof(vlan_cfg));
        db_read(DB_ZTE_VLAN_ID, (oam_uint8 *) &vlan_cfg, sizeof(vlan_cfg));
        vlan_cfg.vlan[port - 1].vlan_mode = vlan_mode;
        vlan_cfg.vlan[port - 1].def_vlan.tpid = defvlan.tpid;
        vlan_cfg.vlan[port - 1].def_vlan.tag = defvlan.tag;
        memset(vlan_cfg.vlan[port - 1 ].vlan, 0x00,
               sizeof(vlan_cfg.vlan[port - 1 ].vlan));
        vlan_cfg.vlan[port - 1 ].vlan_num = 1;
        vlan_cfg.vlan[port - 1 ].agg_vlan_num = vlan[0].agg_vlan_num;

        vlan_cfg.vlan[port - 1 ].vlan[0].tpid = vlan[0].agg_dst_vlan.tpid;
        vlan_cfg.vlan[port - 1 ].vlan[0].tag = vlan[0].agg_dst_vlan.tag;

        for (i = 0 ; i < vlan[0].agg_vlan_num && i < CTC_ONU_AGGR_VLAN_NUM; i ++) {
            vlan_cfg.vlan[port - 1 ].vlan[i + 1].tpid = vlan[0].agg_vlan[i].tpid;
            vlan_cfg.vlan[port - 1 ].vlan[i + 1].tag = vlan[0].agg_vlan[i].tag;
        }

        db_write(DB_ZTE_VLAN_ID, (oam_uint8 *) &vlan_cfg, sizeof(vlan_cfg));
    }

#endif

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
    cs_status ret = CS_E_OK;
    cs_callback_context_t   context;

    ret = cs_plat_opm_status_get(context, 0, 0, temp,
                                 volt, tx_bias, tx_power, rx_power);

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
} oam_transid;

oam_status ctc_oam_onu_laser_control_set_adapt(
    oam_uint16 action,
    oam_uint8 *onuId,
    oam_transid transId)
{
    cs_mac_t onu_mac;
    cs_callback_context_t     context;

    if (NULL == onuId) {
        return CS_E_PARAM;
    }
    else if ((transId != TRANSID_MAJOR) && (transId != TRANSID_BOTH)) {
        return CS_E_NOT_SUPPORT;
    }

    memset(&context, 0, sizeof(context));

    epon_request_onu_pon_mac_addr_get(context, 0, 0, &onu_mac);

    if (((onuId[0] == onu_mac.addr[0])
         && (onuId[1] == onu_mac.addr[1])
         && (onuId[2] == onu_mac.addr[2])
         && (onuId[3] == onu_mac.addr[3])
         && (onuId[4] == onu_mac.addr[4])
         && (onuId[5] == onu_mac.addr[5]))
        ||
        ((0xff == onuId[0]) && (0xff == onuId[1]) && (0xff == onuId[2]) &&
         (0xff == onuId[3]) && (0xff == onuId[4]) && (0xff == onuId[5]))) {
        if (g_laser_tx_power_timer != 0) { /*stop previous timer */
            cs_timer_del(g_laser_tx_power_timer);
            g_laser_tx_power_timer = 0;
        }

        if (0 == action) { /* tx laser on */
            epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, 1);
        }
        else if (65535 == action) {
            epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, 0);
        }
        else {
            epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, 0);
            g_laser_tx_power_timer = cs_timer_add(action * 1000, __laser_tx_power_on, NULL);
        }
    }
    else {
        return CS_E_PARAM;
    }

    return OAM_E_OK;
}



oam_status ctc_oam_onu_reset_set_adapt()
{
    iros_system_reset(RESET_TYPE_FORCE);
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
#ifdef HAVE_LOOP_DETECT
    oam_boolean enable = TRUE;

    if (CS_E_OK != loop_detect_oper_get(port, &enable)) {
        return CS_E_ERROR;
    }

    if (enable) {
        *status = 2;
    }
    else {
        *status = 1;
    }

    return OAM_E_OK;

#else
    return CS_E_NOT_SUPPORT;
#endif

}



oam_status ctc_oam_loop_detect_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status)
{
#ifdef HAVE_LOOP_DETECT
#ifdef HAVE_ZTE_OAM
    db_zte_port_t   port_info;
#endif

    if (status == 1) {
        loop_detect_disable(port);
        /*also need unblock the port*/
        loop_detect_unblock(port);
    }
    else if (status == 2) {
        loop_detect_enable(port);
    }
    else {
        return CS_E_PARAM;
    }

#ifdef HAVE_ZTE_OAM

    if (port <= DB_ZTE_MAX_PORT && port > 0) {
        memset(&port_info, 0x00, sizeof(port_info));
        db_read(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
        port_info.port[port - 1 ].loop_detect = status == 2 ? 1 : 0;
        db_write(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
    }

#endif

    return OAM_E_OK;

#else
    return CS_E_NOT_SUPPORT;
#endif

}

oam_status ctc_oam_disable_looped_status_set_adapt(
    oam_port_id_t port,
    oam_uint32 status)
{
#ifdef HAVE_LOOP_DETECT
    oam_boolean enable = TRUE;

    if (status) {
        /*need block*/
        enable = TRUE;
    }
    else {
        /*no need block*/
        enable = FALSE;
    }

    loop_detect_port_block_enable(port, enable);

#endif
    return OAM_E_OK;

}


oam_status ctc_oam_onu_classification_get_adapt(
    oam_port_id_t port,
    oam_uint32 *num,
    oam_uint8 *data)
{
    oam_ctc_onu_classification_struct *pData =
        (oam_ctc_onu_classification_struct *) data;
    oam_ctc_onu_class_fselect_t *pEntry;
    oam_ctc_onu_class_fselect_v6_t *pEntry6;

    cs_sdl_classification_t  cls[CLASS_RULES_MAX];
    cs_callback_context_t     context;
    cs_status ret = CS_E_OK;
    oam_uint8 cls_num = 0;
    oam_uint32 i = 0, j = 0, len = 0, total_len = 0;
    oam_uint8 ctc_version = 0;

    memset(cls, 0x00, sizeof(cls));

    oam_ctc_version_get(oam_sdl_get_llid(), &ctc_version);

    ret = epon_request_onu_port_classification_get(
              context, 0, 0, port, &cls_num, cls);

    if (ret != CS_E_OK) {
        return ret;
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

    return OAM_E_OK;
}

oam_status ctc_oam_onu_classification_set_adapt(
    oam_uint8 action,
    oam_port_id_t port,
    oam_uint32 num,
    oam_uint8 *data)
{
    cs_status ret = CS_E_OK;
    cs_sdl_classification_t  cls[CLASS_RULES_MAX];
    cs_callback_context_t     context;
    oam_uint8 precedence[CLASS_RULES_MAX];

    oam_ctc_onu_classification_struct *pData;
    oam_ctc_onu_class_fselect_t *pEntry;
    oam_ctc_onu_class_fselect_v6_t *pEntry6;
    oam_uint32 i = 0, j = 0, len = 0, total_len = 0;
    oam_uint8 ctc_version = 0;

    oam_ctc_version_get(oam_sdl_get_llid(), &ctc_version);

    memset(cls, 0x00, sizeof(cls));

    switch (action) {
        case CTC_CLASS_RULES_ACTION_CLR:
            ret = epon_request_onu_port_classification_clr(
                      context, 0, 0, port);
            break;

        case CTC_CLASS_RULES_ACTION_DEL:
            for (i = 0; i < num; i++) {
                pData = (oam_ctc_onu_classification_struct *) (data + total_len);
                total_len += (pData->len + 2);
                precedence[i] = pData->precedence;
            }

            ret = epon_request_onu_port_classification_del(
                      context, 0, 0, port, num, precedence);
            break;

        case CTC_CLASS_RULES_ACTION_ADD:
            for (i = 0; i < num && i < CLASS_RULES_MAX; i ++) {
                pData = (oam_ctc_onu_classification_struct *)(data + total_len);
                total_len += (pData->len + 2);
                cls[i].precedence = pData->precedence;
                cls[i].queueMapped = pData->queueMapped;
                cls[i].priMark = pData->priMark;
                cls[i].entries = pData->entries;
                len = 0;

                for (j = 0; j < pData->entries && j < CLASS_FIELD_SELECT_MAX; j++) {
                    pEntry = (oam_ctc_onu_class_fselect_t *)(pData->data + len);

                    if ((pEntry->fieldSelect >= CLASS_RULES_FSELECT_IPVER) &&
                        (ctc_version == 0x30)) {
                        pEntry6 = (oam_ctc_onu_class_fselect_v6_t *)(pData->data + len);
                        len += sizeof(oam_ctc_onu_class_fselect_v6_t);
                        cls[i].fselect[j].fieldSelect = pEntry6->fieldSelect;
                        cls[i].fselect[j].validationOper = pEntry6->validationOper;
                        memcpy(cls[i].fselect[j].matchValue, pEntry6->matchValue, CLASS_MATCH_VAL_LEN);
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
                }
            }

            ret = epon_request_onu_port_classification_add(
                      context, 0, 0, port, num, cls);
            break;
    }

#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if (ret == CS_E_OK ) {
        ctc_oam_cls_syn_db( port);
    }

#endif
#endif

    return ret;

}



/******************************* MC ADAPT **************************/

oam_status ctc_oam_onu_mcvlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *num,
    oam_uint16 *vlanIds)
{
    cs_callback_context_t         context;
    oam_uint8 mc_mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;

    if ( (port < CS_UNI_PORT_ID1) || (port > CS_UNI_PORT_ID4) ) {
        return CS_E_PARAM;
    }

    if ( (NULL == num) || (NULL == vlanIds) ) {
        return CS_E_PARAM;
    }

    ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode) {
        return CS_E_NOT_SUPPORT;
    }

    return epon_request_onu_mc_vlan_get(context, 0, 0, port, vlanIds, num);
}

oam_status ctc_oam_onu_mcvlan_set_adapt(
    oam_port_id_t port,
    oam_uint8 oper,
    oam_uint16 num,
    oam_uint16 *vlanIds)
{
    cs_callback_context_t         context;
    oam_uint8 mc_mode = CTC_MC_SWITCH_START_IMGP_SNOOPING;
    cs_status  ret = CS_E_OK;

    if ( (port < CS_UNI_PORT_ID1) || (port > CS_UNI_PORT_ID4) ) {
        return CS_E_PARAM;
    }

    if (oper > CTC_MCVLAN_OPER_CLR) {
        return CS_E_PARAM;
    }

    if ( NULL == vlanIds ) {
        return CS_E_PARAM;
    }

    ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode) {
        return CS_E_NOT_SUPPORT;
    }

    if (CTC_MCVLAN_OPER_DEL == oper) {
        ret = epon_request_onu_mc_vlan_del(context, 0, 0, port, vlanIds, num);
    }
    else if (CTC_MCVLAN_OPER_ADD == oper) {
        ret = epon_request_onu_mc_vlan_add(context, 0, 0, port, vlanIds, num);
    }
    else if (CTC_MCVLAN_OPER_CLR == oper) {
        ret = epon_request_onu_mc_vlan_clr(context, 0, 0, port);
    }
    else {
        return CS_E_PARAM;
    }

#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if (ret == CS_E_OK ) {
        ctc_oam_mc_vlan_syn_db( port);
    }

#endif
#endif

    return ret;
}


oam_status ctc_oam_onu_mctagstrip_get_adapt(
    oam_port_id_t port,
    oam_uint8 *tagStriped,
    oam_uint8 *num,
    oam_ctc_iptv_vid_t *vlans)
{
    cs_callback_context_t         context;
    cs_sdl_mc_vlan_act_t          vlan_act;
    cs_sdl_vlan_cfg_t             swap_rule[CTC_ONU_AGGR_VLAN_NUM];
    oam_uint16                     rule_nums;
    int i;

    if ( (port < CS_UNI_PORT_ID1) || (port > CS_UNI_PORT_ID4) ) {
        return CS_E_PARAM;
    }

    if ( (NULL == tagStriped) || (NULL == num) || (NULL == vlans) ) {
        return CS_E_PARAM;
    }

    epon_request_onu_mc_vlan_action_get(context, 0, 0, port, &vlan_act, swap_rule,
                                        &rule_nums);

    if (SDL_MC_VLAN_TAG_SWAP == vlan_act) {
        for (i = 0; i < rule_nums; ++i) {
            vlans[i].vid = swap_rule[i].s_vlan.vid;
            vlans[i].iptv_vid = swap_rule[i].c_vlan.vid;
        }

        *num = rule_nums;
    }

    *tagStriped = (oam_uint8)vlan_act;

    return OAM_E_OK;
}

oam_status ctc_oam_onu_mctagstrip_set_adapt(
    oam_port_id_t port,
    oam_uint8  tagStriped,
    oam_uint8  num,
    oam_ctc_iptv_vid_t *vlans)
{
    cs_callback_context_t         context;
    cs_sdl_mc_vlan_act_t          vlan_act;
    cs_sdl_vlan_cfg_t             swap_rule[CTC_ONU_AGGR_VLAN_NUM];
    int i;
    cs_status                          ret = 0;
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE
    db_zte_mc_tagstrip_t        db_mc_tagstrip;
    db_zte_mc_tagstrip_entry_t   *entry = NULL;
#endif
#endif


    if ( (port < CS_UNI_PORT_ID1) || (port > CS_UNI_PORT_ID4) ) {
        return CS_E_PARAM;
    }

    if ( (CTC_MCVLAN_TAG_OPER_SWAP == tagStriped) && (NULL == vlans) ) {
        return CS_E_PARAM;
    }

    if (CTC_MCVLAN_TAG_OPER_SWAP == tagStriped) {
        for (i = 0; i < num; ++i) {
            swap_rule[i].s_vlan.vid = vlans[i].vid;
            swap_rule[i].c_vlan.vid = vlans[i].iptv_vid;
        }
    }

    vlan_act = (cs_sdl_mc_vlan_act_t)tagStriped;

    ret = epon_request_onu_mc_vlan_action_set(context, 0, 0, port, vlan_act,
            swap_rule, num);

#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if (ret == CS_E_OK) {
        memset(&db_mc_tagstrip, 0x00, sizeof(db_mc_tagstrip));
        db_read(DB_ZTE_MC_TAGSTRIP_ID, (oam_uint8 *) &db_mc_tagstrip,
                sizeof(db_mc_tagstrip));
        entry = &db_mc_tagstrip.entry[port - 1];
        entry->tagStriped = tagStriped;
        entry->num = num;
        memcpy (entry->vlan, vlans, sizeof(oam_ctc_iptv_vid_t)*num);
        db_write(DB_ZTE_MC_TAGSTRIP_ID , (oam_uint8 *) &db_mc_tagstrip,
                 sizeof(db_mc_tagstrip));
    }

#endif
#endif


    return ret;
}


oam_status ctc_oam_onu_mc_switch_get_adapt(
    oam_uint8 *mode)
{
#ifdef HAVE_MC_CTRL
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
#ifdef HAVE_ZTE_OAM
            *mode = CTC_MC_SWITCH_TRANSPARENT_ZTE_EXT;
#endif
            break;
    }

#endif
    return OAM_E_OK;

}

oam_status ctc_oam_onu_mc_switch_set_adapt(
    oam_uint8 mode)
{
    cs_status   ret = CS_E_OK;
    mc_mode_t mc_mode;
    mc_mode_t temp_mc_mode;

    switch ( mode ) {
        case    CTC_MC_SWITCH_START_IMGP_SNOOPING:
            temp_mc_mode = MC_SNOOPING;
            break;

        case    CTC_MC_SWITCH_START_CTC_IGMP:
            temp_mc_mode = MC_MANUAL;
            break;
#ifdef HAVE_ZTE_OAM

        case    CTC_MC_SWITCH_TRANSPARENT_ZTE_EXT:
            temp_mc_mode = MC_DISABLE;
            break;
#endif

        default:
            return  CS_E_PARAM;
            break;

    }

    mc_mode_get(0, &mc_mode);

    if ( temp_mc_mode == mc_mode) {
        return CS_E_OK;
    }

    ret = ctc_mc_mode_set_adapt (temp_mc_mode);

    return ret;

}

oam_status ctc_oam_onu_mc_control_type_get_adapt(
    oam_uint8 *type)
{
#ifdef HAVE_MC_CTRL
    *type = mc_get_mc_fwd_mode();
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mc_control_type_set_adapt(
    oam_uint8  type)
{
#ifdef HAVE_MC_CTRL
    return mc_set_mc_fwd_mode(type);
#else
    return OAM_E_NOT_SUPPORT;
#endif
}

oam_status ctc_oam_onu_mc_control_vlan_mac_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_vlan_mac_t *vlanMac)
{
#ifdef HAVE_MC_CTRL
    mc_ctc_ctrl_get_entries((oam_uint8 *)vlanMac, num);
#endif
    return OAM_E_OK;

}

oam_status ctc_oam_onu_mc_control_mac_ip_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_mac_ip_t *macIp)
{
#ifdef HAVE_MC_CTRL
    mc_ctc_ctrl_get_entries((oam_uint8 *)macIp, num);
#endif
    return OAM_E_OK;

}

oam_status ctc_oam_onu_mc_control_vlan_ip_get_adapt(
    oam_uint16 *num,
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIp)
{
#ifdef HAVE_MC_CTRL
    mc_ctc_ctrl_get_entries((oam_uint8 *)vlanIp, num);
#endif
    return OAM_E_OK;

}

oam_status ctc_oam_onu_mc_control_clr_adapt()
{
#ifdef HAVE_MC_CTRL
    mc_ctc_ctrl_clr_mfdb();
#endif
    return OAM_E_OK;
}

oam_status ctc_oam_onu_mc_control_vlan_mac_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_vlan_mac_t *vlanMac)
{
#ifdef HAVE_MC_CTRL
    oam_uint16 i;

    switch (action) {
        case CTC_MC_CONTROL_ACTION_DEL:
            for (i = 0; i < num; i++) {
                mc_ctc_ctrl_del_mac_vlan_entry(vlanMac[i].userId, vlanMac[i].mc_da,
                                               vlanMac[i].vlanId);
            }

            break;

        case CTC_MC_CONTROL_ACTION_ADD:
            for (i = 0; i < num; i++) {
                mc_ctc_ctrl_add_mac_vlan_entry(vlanMac[i].userId, vlanMac[i].mc_da,
                                               vlanMac[i].vlanId);
            }

            break;

        default:
            break;
    }

#endif

    return OAM_E_OK;
}

oam_status ctc_oam_onu_mc_control_mac_ip_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_mac_ip_t *macIp)
{
#ifdef HAVE_MC_CTRL
    oam_uint16 i;

    mc_ip_type_t ip_type;

    switch (action) {
        case CTC_MC_CONTROL_ACTION_DEL:
            for (i = 0; i < num; i++) {
                ip_type = (macIp[i].ip_addr[0]) ? MC_IPV6 : MC_IPV4;
                mc_ctc_ctrl_del_da_sip_entry(macIp[i].userId, macIp[i].mc_da, ip_type,
                                             macIp[i].ip_addr);
            }

            break;

        case CTC_MC_CONTROL_ACTION_ADD:
            for (i = 0; i < num; i++) {
                ip_type = (macIp[i].ip_addr[0]) ? MC_IPV6 : MC_IPV4;
                mc_ctc_ctrl_add_da_sip_entry(macIp[i].userId, macIp[i].mc_da, ip_type,
                                             macIp[i].ip_addr);
            }

            break;

        default:
            break;
    }

#endif

    return OAM_E_OK;
}

oam_status ctc_oam_onu_mc_control_vlan_ip_set_adapt(
    oam_uint8 action,
    oam_uint16 num,
    oam_ctc_onu_mc_control_vlan_ip_t *vlanIp)
{
#ifdef HAVE_MC_CTRL
    oam_uint16 i;

    mc_ip_type_t ip_type;
    oam_uint8 *ip_addr;

    switch (action) {
        case CTC_MC_CONTROL_ACTION_DEL:
            for (i = 0; i < num; i++) {
                if (vlanIp[i].ip_addr[0]) {
                    ip_type = MC_IPV6;
                    ip_addr = vlanIp[i].ip_addr;
                }
                else {
                    ip_type = MC_IPV4;
                    ip_addr = &vlanIp[i].ip_addr[2];
                }

                mc_ctc_ctrl_del_ip_vlan_entry(vlanIp[i].userId, vlanIp[i].ip_addr, ip_type,
                                              vlanIp[i].vlanId);
            }

            break;

        case CTC_MC_CONTROL_ACTION_ADD:
            for (i = 0; i < num; i++) {
                if (vlanIp[i].ip_addr[0]) {
                    ip_type = MC_IPV6;
                    ip_addr = vlanIp[i].ip_addr;
                }
                else {
                    ip_type = MC_IPV4;
                    ip_addr = &vlanIp[i].ip_addr[2];
                }

                mc_ctc_ctrl_add_ip_vlan_entry(vlanIp[i].userId, vlanIp[i].ip_addr, ip_type,
                                              vlanIp[i].vlanId);
            }

            break;

        default:
            break;
    }

#endif

    return OAM_E_OK;
}

oam_status ctc_oam_onu_grp_nmb_max_get_adapt(
    oam_port_id_t port,
    oam_uint8   *maxGrp)
{
#ifdef HAVE_MC_CTRL
    oam_uint16 max_grp;

    oam_uint8 mc_mode;

    ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode) {
        return CS_E_RESOURCE;
    }

    if (CS_E_OK == mc_port_max_grp_get(0, port, &max_grp)) {
        *maxGrp = max_grp;
        return CS_E_OK;
    }

#endif

    return OAM_E_ERROR;
}

oam_status ctc_oam_onu_grp_nmb_max_set_adapt(
    oam_port_id_t port,
    oam_uint8   maxGrp)
{
#ifdef HAVE_MC_CTRL
    oam_uint8 mc_mode;
    cs_status ret = 0;
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE
    db_zte_mc_grp_t     db_mc_grp;
#endif
#endif
    ctc_oam_onu_mc_switch_get_adapt(&mc_mode);

    if (CTC_MC_SWITCH_START_CTC_IGMP == mc_mode) {
        return CS_E_NOT_SUPPORT;
    }

    ret = mc_port_max_grp_set(0, port, maxGrp);

#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if (ret == CS_E_OK) {
        memset(&db_mc_grp, 0x00, sizeof(db_mc_grp));
        db_read(DB_ZTE_MC_MAX_GRP_ID, (oam_uint8 *) &db_mc_grp,
                sizeof(db_zte_mc_grp_t));
        db_mc_grp.mc_grp[port - 1] = maxGrp;
        db_write(DB_ZTE_MC_MAX_GRP_ID  , (oam_uint8 *) &db_mc_grp,
                 sizeof(db_zte_mc_grp_t));
    }

#endif
#endif

    return ret;
#else
    return CS_E_NOT_SUPPORT;
#endif
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
        return CS_E_PARAM;
    }

    return OAM_E_OK;
}
oam_status ctc_oam_event_status_get_adapt(
    oam_ctc_event_entry_t entry,
    oam_uint32 *status)
{
    ctc_oam_alarm_class_e type = CTC_ONU_ALARM;
    oam_port_id_t port = CS_PON_PORT_ID;
    cs_status ret = CS_E_OK;
    oam_uint32 alarm_config = 0;

    ret = ctc_oam_onu_event_type_get(entry, &type, &port);

    if (ret != CS_E_OK) {
        return ret;
    }

    ret = ctc_oam_get_alarm_config(type, port, entry.id, &alarm_config);

    if (ret != CS_E_OK) {
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
    cs_status ret = CS_E_OK;

    ret = ctc_oam_onu_event_type_get(entry, &type, &port);

    if (ret != CS_E_OK) {
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
    cs_status ret = CS_E_OK;
    oam_uint8 enable = 0;

    ret = ctc_oam_onu_event_type_get(config.entry, &type, &port);

    if (ret != CS_E_OK) {
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
    cs_status ret = CS_E_OK;

    ret = ctc_oam_onu_event_type_get(config.entry, &type, &port);

    if (ret != CS_E_OK) {
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
            return CS_E_PARAM;
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
    cs_status ret = CS_E_OK;

    ret = ctc_oam_onu_alarm_class_port_get(index, &type, &port);

    if (ret != CS_E_OK)
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
    cs_status ret = CS_E_OK;

    ret = ctc_oam_onu_alarm_class_port_get(index, &type, &port);

    if (ret != CS_E_OK)
        return ret;

    ret = ctc_oam_set_alarm_config(type, port, alarm_id, alarm_config);
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if ( ret == CS_E_OK ) {
        if ( type != CTC_PORT_ALARM ) {
            ctc_oam_onu_alarm_syn_db( );
        }
        else {

            ctc_oam_port_alarm_syn_db( port);
        }
    }

#endif
#endif


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
    cs_status ret = CS_E_OK;

    ret = ctc_oam_onu_alarm_class_port_get(index, &type, &port);

    if (ret != CS_E_OK)
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
    cs_status ret = CS_E_OK;

    ret = ctc_oam_onu_alarm_class_port_get(index, &type, &port);

    if (ret != CS_E_OK)
        return ret;

    ret = ctc_oam_set_alarm_threshold(type, port, alarm_id,
                                      raise_threshold, clear_threshold);
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if ( ret == CS_E_OK ) {
        if ( type != CTC_PORT_ALARM ) {
            ctc_oam_onu_alarm_syn_db( );
        }
        else {

            ctc_oam_port_alarm_syn_db( port);
        }
    }

#endif
#endif

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
    cs_callback_context_t     context;

    return epon_request_onu_mpcp_offline_set(context, 0, 0, enable);
}

oam_status ctc_get_port_info(oam_port_id_t port, ctc_port_info_t *info)
{
    cs_status ret;
    cs_sdl_port_admin_t port_admin = 0;
    cs_sdl_port_link_status_t    link_status = 0;
    cs_sdl_port_autoneg_status_t auto_neg = 0;
    oam_boolean pause;
    cs_sdl_port_speed_cfg_t cfg;
    oam_boolean loopdetect;
    cs_sdl_policy_t       policy;
    cs_sdl_policy_t       rate;
    cs_callback_context_t context;
    oam_uint16 def_tpid, def_tag;
    oam_uint8 vlan_mode = 1 ;

    oam_uint16 translate_num;
    oam_ctc_onu_vlan_xlate_entry translate_vlan[CTC_VLAN_MODE_TRANSLATE_MAX];
    oam_uint16 trunk_num ;
    oam_ctc_onu_vlan_tag_t trunk_vlan[CTC_VLAN_MODE_TRUNK_MAX];

    cs_sdl_mc_vlan_act_t          vlan_act = 0 ;
    cs_sdl_vlan_cfg_t             swap_rule;
    oam_uint16                     rule_nums;


    ret = epon_request_onu_port_admin_get(context,
                                          ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &port_admin);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->admin_status = port_admin;

    ret = epon_request_onu_port_link_status_get(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &link_status);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->link_status = link_status;

    ret = epon_request_onu_port_status_get(context,
                                           ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &cfg);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->work_mode = cfg;

    ret = epon_request_onu_port_auto_neg_get(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &auto_neg);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->auto_neg_mode = auto_neg;

    ret = epon_request_onu_port_flow_ctrl_get(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &pause);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->pause = pause;
#ifdef HAVE_LOOP_DETECT
    ret = loop_detect_oper_get(port, &loopdetect);
#endif

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->loopdected = loopdetect;

    ret = epon_request_onu_port_policy_get(context,
                                           ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &policy);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->policy.oper = policy.enable;
    info->policy.cir = policy.rate;
    info->policy.cbs = policy.cbs;
    info->policy.ebs = policy.ebs;

    ret = epon_request_onu_port_ds_rate_limit_get(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &rate);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->ds_rate.oper = rate.enable;
    info->ds_rate.cir = rate.rate;
    info->ds_rate.pir = rate.cbs;

    ret = epon_request_onu_mc_vlan_action_get(
              context, 0, 0, port, &vlan_act, &swap_rule, &rule_nums);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->mc_tag_strip = vlan_act;

    ret =  mc_port_max_grp_get(0, port, &info->mc_max_group);

    if (ret != CS_E_OK) {

        return CS_E_ERROR;
    }

    ret = ctc_oam_onu_vlan_mode_get_adapt(port, &vlan_mode);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->vlan_mode = vlan_mode;

    ret = ctc_oam_onu_default_vlan_tag_get_adapt(port,
            &def_tpid, &def_tag);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    info->def_vlan_tag.tpid = def_tpid;
    info->def_vlan_tag.tag = def_tag;

    switch (info->vlan_mode) {
        case CTC_VLAN_MODE_TRANSPARENT:
        case CTC_VLAN_MODE_TAG:
            break;

        case CTC_VLAN_MODE_TRANSLATE:
            ret = ctc_oam_onu_translate_vlan_get_adapt(port, &translate_num,
                    translate_vlan);

            if (ret != CS_E_OK) {
                return CS_E_ERROR;
            }

            info->translate_num = translate_num;
            memcpy((oam_uint8 *) info->translate_vlan, (oam_uint8 *)translate_vlan,
                   sizeof(translate_vlan));

            break;

        case CTC_VLAN_MODE_TRUNK:
            ret = ctc_oam_onu_trunk_vlan_get_adapt(port, &trunk_num,
                                                   trunk_vlan);

            if (ret != CS_E_OK) {
                return CS_E_ERROR;
            }

            info->trunk_num = trunk_num;
            memcpy((oam_uint8 *) info->trunk_vlan, (oam_uint8 *)trunk_vlan,
                   sizeof(trunk_vlan));
            break;

        default:
            break;
    }

    return CS_E_OK;
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
    oam_uint8 ctc_version = 0;

#ifndef HAVE_HW_OAM
    cs_int8 vendorId[OAM_CTC_VENDER_ID_LEN + 1] = {0};
    cs_int8 model_name[OAM_CTC_MODEL_ID_LEN + 1] = {0};
    cs_int8 hwver[OAM_CTC_ONU_HW_LEN + 1] = {0};
    cs_int8 swVer[OAM_CTC_ONU_SW_LEN + 1] = {0};
    oam_uint8 i = 0;
#endif
    oam_uint8 mac [CS_MACADDR_LEN];

    startup_config_read(CFG_ID_MAC_ADDRESS, CS_MACADDR_LEN, mac);
    sn->hdr.width = sizeof(oam_ctc_onu_sn_t) - sizeof(oam_var_cont_t);

#ifdef HAVE_HW_OAM
    cs_int8 vender[OAM_CTC_VENDER_ID_LEN] = {0x32, 0x30, 0x31, 0x31};
#ifdef HAVE_MPORTS
    cs_int8 model[OAM_CTC_MODEL_ID_LEN] = {0x38, 0x31, 0x33, 0x65};
    cs_int8 hwver[OAM_CTC_ONU_HW_LEN] = {0x48, 0x47, 0x38, 0x31, 0x33, 0x65, 0, 0};
#else
    cs_int8 model[OAM_CTC_MODEL_ID_LEN] = {0x38, 0x31, 0x30, 0x65};
    cs_int8 hwver[OAM_CTC_ONU_HW_LEN] = {0x48, 0x47, 0x38, 0x31, 0x30, 0x65, 0, 0};
#endif
    /*oam_uint8 swver[OAM_CTC_ONU_SW_LEN] = {0x56, 0x31, 0x30, 0x30, 0x52, 0x30, 0x30, 0x31, 0x43,
        0x30, 0x31, 0x42, 0x30, 0x35, 0x32, 00};*/
    cs_int8 swver[OAM_CTC_ONU_SW_LEN] =
        HW_OAM_APP_VERSION;/*V2R1C00{0x56,0x32,0x52,0x31,0x43,0x30,0x30,0x00};*/

    memcpy(sn->venderId,  vender, OAM_CTC_VENDER_ID_LEN);
    memcpy(sn->model,  model, OAM_CTC_MODEL_ID_LEN);
    memcpy(sn->onuId, mac, OAM_MACADDR_LEN);
    memcpy(sn->hwVer, hwver, OAM_CTC_ONU_HW_LEN);
    memcpy(sn->swVer, swver, OAM_CTC_ONU_SW_LEN);
#else

    startup_config_read(CFG_ID_VENDOR_ID, OAM_CTC_VENDER_ID_LEN, vendorId);
    startup_config_read(CFG_ID_ONU_MOD_ID, OAM_CTC_MODEL_ID_LEN, model_name);
    startup_config_read(CFG_ID_HW_VERSION, OAM_CTC_ONU_HW_LEN , hwver);

    memcpy(sn->venderId + OAM_CTC_VENDER_ID_LEN - strlen(vendorId),  vendorId,
           strlen(vendorId));
    memcpy(sn->model + OAM_CTC_MODEL_ID_LEN - strlen(model_name),  model_name,
           strlen(model_name));
    memcpy(sn->onuId, mac, CS_MACADDR_LEN);

    memcpy(sn->hwVer + OAM_CTC_ONU_HW_LEN - strlen(hwver), hwver, strlen(hwver));

    memset(swVer, 0x00, OAM_CTC_ONU_SW_LEN);
    strcpy(swVer, IROS_ONU_APP_VER_MAJOR);
    strcat(swVer, IROS_ONU_APP_VER_MINOR);
    strcat(swVer, IROS_ONU_APP_VER_REVISION);
    swVer[OAM_CTC_ONU_SW_LEN ] = 0;

    for (i = OAM_CTC_ONU_SW_LEN - strlen(swVer); i < OAM_CTC_ONU_SW_LEN; i++) {
        sn->swVer[i] = swVer[i + strlen(swVer) - OAM_CTC_ONU_SW_LEN];
    }

#endif
    oam_ctc_version_get(oam_sdl_get_llid(), &ctc_version);

    if (ctc_version == 0x30)
        memcpy(sn->extend_model + OAM_CTC_EXTEND_MODEL_ID - strlen(model_name),
               model_name, strlen(model_name));

    return CS_E_OK;
}

oam_status ctc_oam_fw_get_adapt(oam_ctc_onu_fw_ver_t *fw_ver)
{
#ifdef HAVE_HW_OAM
    fw_ver->hdr.width = sizeof(oam_uint8) * 4;
    fw_ver->fwVersion[0] = cs_atoi(IROS_ONU_APP_VER_MAJOR);
    fw_ver->fwVersion[1] = cs_atoi(IROS_ONU_APP_VER_MINOR);
    fw_ver->fwVersion[2] = cs_atoi(IROS_ONU_APP_VER_REVISION);
    fw_ver->fwVersion[3] = cs_atoi(IROS_ONU_APP_VER_BUILD);
#else

    if (oam_is_cuc_oam()) {
        memset(fw_ver->fwVersion, 0x00, sizeof(fw_ver->fwVersion));
        sprintf(fw_ver->fwVersion, "%s.%s.%s.%s",
                IROS_ONU_APP_VER_MAJOR,
                IROS_ONU_APP_VER_MINOR,
                IROS_ONU_APP_VER_REVISION,
                IROS_ONU_APP_VER_BUILD);
        fw_ver->hdr.width = strlen(fw_ver->fwVersion);
    }
    else {
        oam_image_version_t img_ver_info;

        fw_ver->hdr.width = sizeof(oam_image_version_t);
        img_ver_info.major = htons(atoi(IROS_ONU_APP_VER_MAJOR));
        img_ver_info.minor = htons(atoi(IROS_ONU_APP_VER_MINOR));
        img_ver_info.revision = htons(atoi(IROS_ONU_APP_VER_REVISION));
        img_ver_info.build_id = htons(atoi(IROS_ONU_APP_VER_BUILD));

        memcpy(fw_ver->fwVersion, &img_ver_info, sizeof(oam_image_version_t));
    }

#endif

    return CS_E_OK;
}

oam_status ctc_oam_onucap2_get_adapt(oam_ctc_onu_capabilities2_t *onu_cap2)
{
    oam_uint8 numOfPon = 1;
    oam_uint8 protectionType = 1;
    oam_uint8 hasBattery = 1;

    // startup_config_read(CFG_ID_PON_PORT_NUM,1 , &numOfPon);
    // startup_config_read(CFG_ID_PON_PROTECTION_TYPE,1 , &protectionType);
    startup_config_read(CFG_ID_BAK_BATTERY_MODE, 1 , &hasBattery);

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

oam_status ctc_oam_keep_silence_time_get_adapt(oam_uint8 *keep);
{
    cs_status ret = CS_E_OK;

    ret = startup_config_read(CFG_ID_KEEP_SILENCE_TIME, sizeof(oam_uint8), keep);

    if (ret != CS_E_OK)
        return CS_E_ERROR;

    return CS_E_OK;
}

oam_status ctc_oam_enc_key_get_adapt(oam_uint8 inUserKeyIdx, oam_uint32 *key)
{
    cs_callback_context_t     context;
    cs_status ret = CS_E_OK;

    ret = epon_request_onu_enc_key_get(context, ONU_DEVICEID_FOR_API,
                                       ONU_LLIDPORT_FOR_API,
                                       inUserKeyIdx, key);

    if (ret != CS_E_OK)
        return CS_E_ERROR;

    return CS_E_OK;
}

oam_status ctc_oam_onu_mpcp_queue_thresh_get_adapt(
    oam_uint8 index, oam_uint16 *threshold)
{
    cs_callback_context_t context;
    cs_status ret = CS_E_OK;
    oam_uint16 max_threshold = 0;

    ret = epon_request_onu_mpcp_queue_threshold_get(context, ONU_DEVICEID_FOR_API,
            ONU_LLIDPORT_FOR_API,
            index, threshold, &max_threshold);

    if (ret != CS_E_OK)
        return CS_E_ERROR;

    return CS_E_OK;
}

oam_status ctc_oam_onu_mpcp_queue_thresh_set_adapt(
    oam_uint8 index, oam_uint16 threshold)
{
    cs_callback_context_t context;
    cs_status ret = CS_E_OK;

    ret = epon_request_onu_mpcp_queue_threshold_set(context, ONU_DEVICEID_FOR_API,
            ONU_LLIDPORT_FOR_API,
            index, threshold, 65535);

    if (ret != CS_E_OK)
        return CS_E_ERROR;

    return CS_E_OK;
}

oam_status ctc_oam_onu_pon_stat_get_adapt(oam_ctc_onu_port_stats_t *stats)
{
    cs_status ret = CS_E_OK;
    oam_pon_stats_t pon_stas;

    memset(&pon_stas, 0, sizeof(pon_stas));
    ret = app_onu_pon_stats_get(&pon_stas);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    stats->dn_drop_events               = 0;
    stats->up_drop_events               = 0;
    stats->dn_octets                    = pon_stas.pon_byte_cnt;
    stats->up_octets                    = pon_stas.pon_tx_byte_cnt;
    stats->dn_frames                    = pon_stas.pon_frame_cnt +
                                          pon_stas.pon_mcframe_cnt +
                                          pon_stas.pon_bcframe_cnt;
    stats->up_frames                    = pon_stas.pon_txframe_cnt +
                                          pon_stas.pon_txmcframe_cnt +
                                          pon_stas.pon_txbcframe_cnt;
    stats->dn_bc_frames                 = pon_stas.pon_bcframe_cnt;
    stats->up_bc_frames                 = pon_stas.pon_txbcframe_cnt;
    stats->dn_mc_frames                 = pon_stas.pon_mcframe_cnt;
    stats->up_mc_frames                 = pon_stas.pon_txmcframe_cnt;
    stats->dn_crc_err_frames            = pon_stas.pon_crc8_err;
    stats->up_crc_err_frames            = pon_stas.pon_tx_crc_err;

    stats->dn_undersize_frames          = pon_stas.pon_undersizecnt;
    stats->up_undersize_frames          = 0;
    stats->dn_oversize_frames           = pon_stas.pon_oversize_cnt;
    stats->up_oversize_frames           = pon_stas.pon_txoversizecnt;
    stats->dn_fragments                 = 0;
    stats->up_fragments                 = 0;
    stats->dn_jabbers                   = pon_stas.pon_jabbercnt;
    stats->up_jabbers                   = 0;
    /*stats->dn_collisions                = 0; */
    stats->dn_64_octets_frames          = pon_stas.pon_rmon64_cnt;
    stats->dn_65_127_octets_frames      = pon_stas.pon_rmon65_127_cnt;
    stats->dn_128_255_octets_frames     = pon_stas.pon_rmon128_255_cnt;
    stats->dn_256_511_octets_frames     = pon_stas.pon_rmon256_511_cnt;
    stats->dn_512_1023_octets_frames    = pon_stas.pon_rmon512_1023_cnt;
    stats->dn_1024_1518_octets_frames   = pon_stas.pon_rmon1024_1518_cnt;
    stats->up_64_octets_frames          = pon_stas.pon_txrmon64_cnt;
    stats->up_65_127_octets_frames      = pon_stas.pon_txrmon65_127_cnt;
    stats->up_128_255_octets_frames     = pon_stas.pon_txrmon128_255_cnt;
    stats->up_256_511_octets_frames     = pon_stas.pon_txrmon256_511_cnt;
    stats->up_512_1023_octets_frames    = pon_stas.pon_txrmon512_1023_cnt;
    stats->up_1024_1518_octets_frames   = pon_stas.pon_txrmon1024_1518_cnt;
    stats->dn_discard_frames            = 0;
    stats->up_discard_frames            = 0;
    stats->dn_err_frames                = pon_stas.pon_crc8_err +
                                          pon_stas.pon_undersizecnt +
                                          pon_stas.pon_oversize_cnt + pon_stas.pon_jabbercnt;

    stats->up_err_frames                = pon_stas.pon_txoversizecnt ;
    stats->status_change_times          = 0;

    return CS_E_OK;
}

oam_status ctc_oam_onu_uni_stat_get_adapt(oam_ctc_onu_port_stats_t *stats)
{
    oam_uni_stats_t uni_stas;
    oam_uint32 change_count = 0;

    memset(&uni_stas, 0, sizeof(uni_stas));
    ret = app_onu_port_stats_get(port, &uni_stas);

    if (ret != CS_E_OK) {
        return ret;
    }

    app_onu_port_status_change_get(port, &change_count);

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

    return CS_E_OK;
}

void ctc_oam_power_saving_init()
{
    return;
}

void ctc_oam_power_saving_reset()
{
    pwrs_event(PWRS_MSG_RESET, NULL, 0);
    return;
}

oam_status ctc_oam_oui_get_adapt(oam_oui_t oui)
{
    startup_config_read(
        CFG_ID_OUI, sizeof(oam_oui_t), oam_oui_ctc);

    return CS_E_OK;
}

#ifdef HAVE_ZTE_OAM
oam_status ctc_oam_cls_syn_db ( oam_port_id_t port)
{
    cs_callback_context_t context;
    oam_uint8                   number = 0;
    db_zte_cls_t              db_cls;
    db_zte_cls_entry_t   *db_cls_entry = NULL;
    cs_sdl_classification_t  cfg [CLS_RULE_TBL_MAX];
    cs_status                  ret = 0;

    ret = epon_request_onu_port_classification_get ( context
            , ONU_DEVICEID_FOR_API,  ONU_LLIDPORT_FOR_API ,  port, &number,  cfg);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    memset(&db_cls, 0x00, sizeof(db_zte_cls_t));
    db_read (DB_ZTE_CLS_ID , (oam_uint8 *) &db_cls, sizeof(db_zte_cls_t));

    db_cls_entry = &db_cls.cls[port - 1];

    db_cls_entry->num = number;
    memcpy (db_cls_entry->rule, cfg, sizeof (cs_sdl_classification_t)*number) ;


    db_write(DB_ZTE_CLS_ID , (oam_uint8 *) &db_cls, sizeof(db_zte_cls_t));

    return ret;

}

oam_status ctc_oam_mc_vlan_syn_db ( oam_port_id_t port)
{
    cs_callback_context_t context;
    oam_uint16                   number = 0;
#ifdef NEED_SAVE
    db_zte_mc_vlan_t      db_mc_vlan;
    db_zte_mc_vlan_entry_t   *db_mc_entry = NULL;
#endif
    oam_uint16                       vlan_cfg [8];
    cs_status                  ret = 0;

    epon_request_onu_mc_vlan_get ( context,
                                   ONU_DEVICEID_FOR_API,  ONU_LLIDPORT_FOR_API ,  port, vlan_cfg, &number);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

#ifdef NEED_SAVE
    memset(&db_mc_vlan, 0x00, sizeof(db_zte_mc_vlan_t));
    db_read (DB_ZTE_MC_VLAN_ID , (oam_uint8 *) &db_mc_vlan,
             sizeof(db_zte_mc_vlan_t));

    db_mc_entry = &db_mc_vlan.entry[port - 1];
    db_mc_entry->num = number;
    memcpy (db_mc_entry->vlan, vlan_cfg , sizeof (oam_uint16 )*number) ;

    db_write(DB_ZTE_MC_VLAN_ID , (oam_uint8 *) &db_mc_vlan,
             sizeof(db_zte_mc_vlan_t));
#endif
    return ret;

}


oam_status ctc_mc_mode_set_adapt(
    oam_uint32 mode
)
{
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE
    db_zte_onu_svc_t   svc;
#endif
#endif
    cs_status               ret = CS_E_OK;

    ret = mc_mode_set ( ONU_DEVICEID_FOR_API,  (mc_mode_t)mode);
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if (ret == CS_E_OK ) {
        memset(&svc, 0x00, sizeof(svc));
        db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
        svc.mc_mode = mode;
        db_write(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    }

#endif
#endif

    return ret;
}

oam_status ctc_mc_gmi_set_adapt (
    oam_uint32 gmi
)
{
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE
    db_zte_onu_svc_t   svc;
#endif
#endif
    cs_status               ret = CS_E_OK;

    ret = mc_gmi_set ( ONU_DEVICEID_FOR_API ,  gmi);
#ifdef HAVE_ZTE_OAM
#ifdef NEED_SAVE

    if (ret == CS_E_OK ) {
        memset(&svc, 0x00, sizeof(svc));
        db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
        svc.mc_time = gmi;
        db_write(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    }

#endif
#endif
    return ret;

}

oam_status ctc_port_storm_control_set_adapt (
    oam_port_id_t            port_id,
    oam_uint32               storm_type,
    oam_boolean              enable,
    oam_uint32               rate
)
{
    cs_callback_context_t       context;
    cs_sdl_policy_t                 storm_control;
#ifdef NEED_SAVE
    db_zte_port_t                   port_info;
#endif
    cs_status                         ret = CS_E_OK;

    memset ( &storm_control, 0, sizeof(cs_sdl_policy_t ) );
    storm_control.enable = enable;
    storm_control.rate = rate;

    ret = epon_request_onu_port_storm_ctrl_set ( context
            , ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API ,  port_id,  SDL_RL_MOD_BPS,
            storm_type, &storm_control);
#ifdef NEED_SAVE

    if (ret == CS_E_OK ) {
        memset(&port_info, 0x00, sizeof(port_info));
        db_read(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
        port_info.port[port_id - 1].bc_ctrl_mode = storm_type;
        port_info.port[port_id - 1].bc_ctrl_rate = rate;
        port_info.port[port_id - 1].ctrl_enable = enable;
        db_write(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
    }

#endif
    return ret;
}

oam_status ctc_onu_schedule_mode_set_adapt (
    cs_sdl_sched_type_t       sche_mode
)
{
    cs_callback_context_t       context;
#ifdef NEED_SAVE
    db_zte_onu_svc_t            svc;
#endif
    cs_status                         ret = CS_E_OK;


    ret = epon_request_onu_schedule_mode_set(context, 0, 0, sche_mode);
#ifdef NEED_SAVE

    if ( ret == CS_E_OK ) {
        memset(&svc, 0x00, sizeof(svc));
        db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
        svc.qos_sched_mode = sche_mode;
        db_write(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    }

#endif
    return ret;

}

oam_status ctc_port_queue_size_set_adapt (
    oam_port_id_t                   port,
    oam_uint32                       queue_id,
    oam_uint32                       queue_size
)
{
    cs_callback_context_t     context;
#ifdef NEED_SAVE
    db_zte_voq_t                 voq;
#endif
    cs_status                       ret = CS_E_OK;

    ret = epon_request_onu_que_size_set(context, ONU_DEVICEID_FOR_API ,
                                        ONU_LLIDPORT_FOR_API , port, queue_id, queue_size);
#ifdef NEED_SAVE

    if ( ret == CS_E_OK ) {
        memset(&voq, 0x00, sizeof(voq));
        db_read(DB_ZTE_QOS_VOQ_ID, (oam_uint8 *) &voq, sizeof(voq));
        voq.voq_cfg[port][queue_id] = queue_size;
        db_write(DB_ZTE_QOS_VOQ_ID, (oam_uint8 *) &voq, sizeof(voq));
    }

#endif
    return ret;

}




oam_status ctc_onu_1pcos_set_adapt (
    oam_uint32                       pri,
    oam_uint32                       cos
)
{
    cs_callback_context_t     context;
    cs_pri_cos_map_t           queue_map;
#ifdef NEED_SAVE
    db_zte_onu_svc_t          svc;
#endif
    cs_status                       ret = CS_E_OK;

    ret = epon_request_onu_dot1p_map_get ( context,  ONU_DEVICEID_FOR_API,
                                           ONU_LLIDPORT_FOR_API, &queue_map);

    if (ret != CS_E_OK) {
        goto END;
    }

    queue_map.cos[pri] = cos;

    ret = epon_request_onu_dot1p_map_set ( context, ONU_DEVICEID_FOR_API ,
                                           ONU_LLIDPORT_FOR_API ,  &queue_map);
#ifdef NEED_SAVE

    if ( ret == CS_E_OK ) {
        memset(&svc, 0x00, sizeof(svc));
        db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
        memcpy(svc.pri_cos_map, queue_map.cos, DB_ZTE_QUEUE_PRIO_LEN);
        db_write(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    }

#endif
END:

    return ret;

}

oam_status ctc_onu_mtu_set_adapt (
    oam_uint16           mtu
)
{
    cs_callback_context_t     context;
    oam_uint32                       i = 0;
#ifdef NEED_SAVE
    db_zte_onu_svc_t          svc;
#endif
    cs_status                       ret = 0;

    for (i = 0; i < DB_ZTE_MAX_PORT; i++) {
        ret = epon_request_onu_port_mtu_set(
                  context, ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API,
                  i + 1, (oam_uint16)mtu);

        if ( ret != CS_E_OK ) {
            return CS_E_ERROR;
        }
    }

    ret = epon_request_onu_port_mtu_set(
              context, ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API,
              CS_PON_PORT_ID, (oam_uint16)mtu);

    if (ret != CS_E_OK ) {
        return CS_E_ERROR;
    }

#ifdef NEED_SAVE
    memset(&svc, 0x00, sizeof(svc));
    db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    svc.max_mtu = (oam_uint16)mtu;
    db_write(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
#endif
    return      CS_E_OK;

}

oam_status ctc_oam_onu_alarm_syn_db (  )
{
    oam_uint32                i = 0;
    oam_uint32                   number = 0;
    db_zte_onu_alarm_t   *db_onu_alarm = NULL;
    db_zte_alarm_entry_t     *entry = NULL;
    oam_uint32                 zte_onu_alarm_len = 0;
    cs_status                  ret = 0;

    zte_onu_alarm_len = sizeof(db_zte_onu_alarm_t);
    db_onu_alarm = (db_zte_onu_alarm_t *) OAM_MALLOC(zte_onu_alarm_len);

    if ( db_onu_alarm == NULL ) {
        ret = CS_E_ERROR;
        goto END;
    }

    memset( db_onu_alarm, 0x00, zte_onu_alarm_len);

    number = sizeof(g_onu_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);

    for (i = 0; i < number ; i++) {
        entry = &db_onu_alarm->entry[i];
        entry->alarm_code = g_onu_alarm_cfg[i].alarm_code;
        entry->alarm_type = CTC_ONU_ALARM;
        entry->enable = g_onu_alarm_cfg[i].enable;
        entry->threshold_type = g_onu_alarm_cfg[i].threshold_type;

        if ( entry->threshold_type == CTC_THRESH_SIGNED_LOW
             || entry->threshold_type == CTC_THRESH_SIGNED_HIGH ) {
            entry->raise_threshold = g_onu_alarm_cfg[i].raise_alarm_threshold.signValue;
            entry->clear_threshold = g_onu_alarm_cfg[i].clear_alarm_threshold.signValue;
        }

        if ( entry->threshold_type == CTC_THRESH_UNSIGNED_LOW
             || entry->threshold_type == CTC_THRESH_UNSIGNED_HIGH ) {
            entry->raise_threshold = g_onu_alarm_cfg[i].raise_alarm_threshold.unsignValue;
            entry->clear_threshold = g_onu_alarm_cfg[i].clear_alarm_threshold.unsignValue;
        }
    }

    db_onu_alarm->num += number;

    if (oam_is_cuc_oam() == TRUE) {
        number = sizeof(g_cuc_pon_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);

        for (i = 0; i < number ; i++) {
            entry = &db_onu_alarm->entry[i + db_onu_alarm->num];
            entry->alarm_code = g_cuc_pon_alarm_cfg[i].alarm_code;
            entry->alarm_type = CTC_PON_ALARM;
            entry->enable = g_cuc_pon_alarm_cfg[i].enable;
            entry->threshold_type = g_cuc_pon_alarm_cfg[i].threshold_type;

            if ( entry->threshold_type == CTC_THRESH_SIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_SIGNED_HIGH ) {
                entry->raise_threshold = g_cuc_pon_alarm_cfg[i].raise_alarm_threshold.signValue;
                entry->clear_threshold = g_cuc_pon_alarm_cfg[i].clear_alarm_threshold.signValue;
            }

            if ( entry->threshold_type == CTC_THRESH_UNSIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_UNSIGNED_HIGH ) {
                entry->raise_threshold =
                    g_cuc_pon_alarm_cfg[i].raise_alarm_threshold.unsignValue;
                entry->clear_threshold =
                    g_cuc_pon_alarm_cfg[i].clear_alarm_threshold.unsignValue;
            }

        }

        db_onu_alarm->num += number;
    }
    else {

        number = sizeof(g_pon_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);

        for (i = 0; i < number ; i++) {
            entry = &db_onu_alarm->entry[i + db_onu_alarm->num];
            entry->alarm_code = g_pon_alarm_cfg[i].alarm_code;
            entry->alarm_type = CTC_PON_ALARM;
            entry->enable = g_pon_alarm_cfg[i].enable;
            entry->threshold_type = g_pon_alarm_cfg[i].threshold_type;

            if ( entry->threshold_type == CTC_THRESH_SIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_SIGNED_HIGH ) {
                entry->raise_threshold = g_pon_alarm_cfg[i].raise_alarm_threshold.signValue;
                entry->clear_threshold = g_pon_alarm_cfg[i].clear_alarm_threshold.signValue;
            }

            if ( entry->threshold_type == CTC_THRESH_UNSIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_UNSIGNED_HIGH ) {
                entry->raise_threshold = g_pon_alarm_cfg[i].raise_alarm_threshold.unsignValue;
                entry->clear_threshold = g_pon_alarm_cfg[i].clear_alarm_threshold.unsignValue;
            }

        }

        db_onu_alarm->num += number;
    }



    db_write(DB_ZTE_ONU_ALARM_ID , (oam_uint8 *) db_onu_alarm, zte_onu_alarm_len);


END:

    if (db_onu_alarm != NULL) {
        OAM_FREE(db_onu_alarm);
        db_onu_alarm = NULL;
    }

    return ret;

}


oam_status ctc_oam_port_alarm_syn_db ( oam_port_id_t port  )
{
    oam_uint32                i = 0;
    db_zte_port_alarm_t   *db_port_alarm = NULL;
    db_zte_alarm_entry_t      *entry = NULL;
    oam_uint32                 zte_port_alarm_len = 0;
    oam_boolean               cuc_flag = TRUE;
    ctc_oam_alarm_info_t *item = NULL;
    cs_status                  ret = 0;

    zte_port_alarm_len = sizeof(db_zte_port_alarm_t);
    db_port_alarm = (db_zte_port_alarm_t *) iros_malloc(IROS_MID_OAM,
                    zte_port_alarm_len);

    if ( db_port_alarm == NULL ) {
        ret = CS_E_ERROR;
        goto END;
    }

    memset (db_port_alarm, 0, zte_port_alarm_len );

    cuc_flag = oam_is_cuc_oam();
    memset( db_port_alarm, 0x00, zte_port_alarm_len);

    db_read (DB_ZTE_PORT_ALARM_ID , (oam_uint8 * )db_port_alarm,
             zte_port_alarm_len);

    if (  cuc_flag == TRUE) {
        db_port_alarm->onu_entry[port - 1].num = g_cuc_port_alarm_num;

        for (i = 0; i < g_cuc_port_alarm_num; i++) {
            entry = &db_port_alarm->onu_entry[port - 1].entry[i];
            item = &g_cuc_port_alarm_cfg[port - 1][i];
            entry->alarm_code = item->alarm_code;
            entry->alarm_type = CTC_PORT_ALARM;
            entry->enable = item->enable;
            entry->threshold_type = item->threshold_type;

            if ( entry->threshold_type == CTC_THRESH_SIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_SIGNED_HIGH ) {
                entry->raise_threshold = item->raise_alarm_threshold.signValue;
                entry->clear_threshold = item->clear_alarm_threshold.signValue;
            }

            if ( entry->threshold_type == CTC_THRESH_UNSIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_UNSIGNED_HIGH ) {
                entry->raise_threshold = item->raise_alarm_threshold.unsignValue;
                entry->clear_threshold = item->clear_alarm_threshold.unsignValue;
            }
        }

    }
    else {

        db_port_alarm->onu_entry[port - 1].num = g_port_alarm_num;

        for (i = 0; i < g_port_alarm_num; i++) {
            entry = &db_port_alarm->onu_entry[port - 1].entry[i];
            item = &g_port_alarm_cfg[port - 1][i];
            entry->alarm_code = item->alarm_code;
            entry->alarm_type = CTC_PORT_ALARM;
            entry->enable = item->enable;
            entry->threshold_type = item->threshold_type;

            if ( entry->threshold_type == CTC_THRESH_SIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_SIGNED_HIGH ) {
                entry->raise_threshold = item->raise_alarm_threshold.signValue;
                entry->clear_threshold = item->clear_alarm_threshold.signValue;
            }

            if ( entry->threshold_type == CTC_THRESH_UNSIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_UNSIGNED_HIGH ) {
                entry->raise_threshold = item->raise_alarm_threshold.unsignValue;
                entry->clear_threshold = item->clear_alarm_threshold.unsignValue;
            }
        }

    }

    db_write(DB_ZTE_PORT_ALARM_ID , (oam_uint8 *) db_port_alarm,
             zte_port_alarm_len);

END:

    if (db_port_alarm != NULL) {
        OAM_FREE(db_port_alarm);
        db_port_alarm = NULL;
    }

    return ret;

}

#endif


