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
#ifdef HAVE_KT_OAM
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <fcntl.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <errno.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/route.h>
#include <assert.h>

#include "sys.h"
#include "oam_main.h"
#include "oam_cmn.h"
#include "oam_core.h"
#include "oam_supp.h"
#include "oam_event.h"
#include "oam_control.h"
#include "oam_timer.h"
#include "oam_adapt.h"
#include "oam_std_pdu.h"
#include "oam_client.h"
#include "oam_mux_par.h"

#include "oam_kt_client.h"
#include "oam_kt_pdu.h"
#include "oam_kt_adapt.h"
#include "oam_kt.h"
#include "oam_kt_event.h"
#include "oam_tk_event.h"
#include "oam_tk_upg.h"
#include "oam_port_stats.h"

#include "l2.h"
#include "rate.h"
#include "port.h"
#include "pon.h"
#include "epon.h"
#include "vlan.h"
#include "eth_port.h"
#include "qos.h"
#include "opm_api.h"
#include "ca_event.h"

#ifdef HAVE_ONU_RSTP
#include "stp_mgmt.h"
#endif
#ifdef HAVE_LOOP_DETECT
#include "loop_detect.h"
#endif
#ifdef RM_KT_LINUX
#include "i2c.h"
#endif

#ifdef HAVE_CA_GPIO
#include "ca_gpio.h"
#endif
#include "dhcp.h"

oam_uint32 g_laser_tx_power_timer = 0;

void kt_oam_onu_tmfunc_reset_adapt(void* data)
/* INPUTS     : o timer data                                    */
/* OUTPUTS    : o NA                                            */
/* RETURNS    : ---                                             */
/* DESCRIPTION:                                                 */
/* reset timer handler                                          */
/* $rtn_hdr_end                                                 */
/****************************************************************/
{
    oam_boolean status = 0;

    status = oam_tk_upg_upgrading();

    if(status){
        OAM_ORG_LOG_MAJ("OAM image upgrading...reset delay\r\n");
        ca_timer_add(3000, kt_oam_onu_tmfunc_reset_adapt, NULL);
    }else{
        system("echo 2 > /overlay/reset");
        system("sync");/*Sync Flash data*/
        ca_reset(0, 0);
    }
}

oam_status kt_oam_action_reset_adapt(
    oam_uint8  force
)
{
    oam_uint32 delay = KT_NORMAL_RESET_DELAY_ADAPT;

    OAM_ORG_LOG_MAJ("start to reset ONU\r\n");
    if(force){
        delay = KT_NORMAL_RESET_DELAY_ADAPT;
    }

    ca_timer_add(delay, kt_oam_onu_tmfunc_reset_adapt, NULL);
    return OAM_E_OK;
}

void __laser_tx_power_on(void *data)
{
    ca_epon_port_laser_mode_t mode;
    //ca_pon_laser_tx_set(0, 1);
    mode = CA_EPON_PORT_LASER_MODE_BURST;
    ca_epon_port_laser_mode_set(0,CA_PORT_ID(CA_PORT_TYPE_EPON, 7), mode);
    CA_GPIO_SET_VALUE(CA_GPIO_PON_TX_POWRER_EN, 1);
    CA_GPIO_SET_VALUE(CA_GPIO_TX_DISABLE, 0);
    g_laser_tx_power_timer = 0;
    cap_log_msg(0, CA_MOD_ORG_OAM_APP, CAP_LOG_EMERGENCY,0, "Tx power off end, turn on TX power\n");
}
oam_status kt_oam_onu_clear_statis_adapt()
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Set vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status rt = 0;
    oam_port_stats_reset();

    return rt;
}

oam_status kt_oam_onu_ipv6_opt254_set_adapt(oam_uint8 enable)
{
    oam_status rt = 0;
    dhcpv6_enable(enable);
    return rt;
}

oam_status kt_oam_onu_ipv6_opt254_get_adapt(oam_uint8 *enable)
{
    oam_status rt = 0;

    if (!enable) {
        return OAM_E_PARAM;
    }

    dhcpv6_cfg_get(enable);
    return rt;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_onu_block_set_adapt                              */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_onu_block_set_adapt (
                            oam_uint8 action
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_status_t ret = OAM_E_OK;
    ca_uint32_t i = 0;
    ca_port_stp_state_t stp_state = CA_PORT_STP_FORWARDING;
    ca_port_id_t ca_port;

    if (action){
        stp_state = CA_PORT_STP_FORWARDING;
    }
    else{
        stp_state = CA_PORT_STP_BLOCKING;
    }

    for (i = OAM_PON_PORT_ID; i <= OAM_UNI_PORT_NUM; i++ ){
        ca_port = OAM_PORT_ID_TO_CA_PORT(i);
        ret = ca_port_stp_state_set(0, ca_port, stp_state);
    }

    return ret;

}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_onu_block_get_adapt                              */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_onu_block_get_adapt (
                            oam_uint8 *action
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_status_t ret = OAM_E_OK;
    ca_port_stp_state_t stp_state = CA_PORT_STP_FORWARDING;
    ca_port_id_t ca_port;

    /* TODO API only supports eth port */
    ca_port = OAM_PORT_ID_TO_CA_PORT(OAM_UNI_PORT_ID1);
    ret = ca_port_stp_state_get(0, ca_port, &stp_state);

    if (stp_state == CA_PORT_STP_FORWARDING)
        *action = 1;
    else
        *action = 0;

    return ret;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_eth_mac_limit_set_adapt                          */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_eth_mac_limit_set_adapt (
                            oam_uint8 oper,
                            oam_uint32 num
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_port_id_t port_id = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
    ca_status_t ret = CA_E_OK;

    if((oper == 0) ||
       ((oper != 0) && (num > KT_ORG_MAX_ONU_MAC)))
    {
        num = KT_ORG_MAX_ONU_MAC;
    }
    if(num == 0)
        num = KT_ORG_MAX_ONU_MAC;

    ret = ca_l2_mac_limit_set(0, CA_L2_MAC_LIMIT_SYSTEM, port_id, num);

    return (ret == CA_E_OK ? 0 : 1);
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_eth_mac_limit_get_adapt                          */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_eth_mac_limit_get_adapt (
                            oam_uint8 *oper,
                            oam_uint8 *num
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_port_id_t port_id = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
    ca_status_t ret = CA_E_OK;
    ca_uint32_t mac_num;

    ret = ca_l2_mac_limit_get(0, CA_L2_MAC_LIMIT_SYSTEM, port_id, &mac_num);

    *num = mac_num;

    if (mac_num >= KT_ORG_MAX_ONU_MAC)
        *oper = 0;
    else
        *oper = 1;

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_onu_mpcp_counter_get_adapt                       */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_onu_mpcp_counter_get_adapt (
                    oam_kt_onu_mpcp_counter_container_struct   *counter
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_pon_stats64_t pon_stats;
    memset(counter, 0, sizeof(oam_kt_onu_mpcp_counter_container_struct));
    memset(&pon_stats,    0, sizeof(oam_pon_stats64_t ));
    if(oam_onu_pon_stats_get(&pon_stats)!= CA_E_OK){
        return CA_E_ERROR;
    }

    counter->mask = 0x3f;
    counter->rx_hec_errors = 0;
    counter->tx_register_request = pon_stats.pon_mpcp_tx_req_reg + pon_stats.pon_mpcp_tx_req_dereg;
    counter->rx_register = pon_stats.pon_mpcp_bc_reg_reg_rev;
    counter->tx_register_ack = pon_stats.pon_mpcp_tx_ack_ack + pon_stats.pon_mpcp_tx_ack_nack;
    counter->rx_gate_frames  = pon_stats.pon_mpcp_uc_gat_rev + pon_stats.pon_mpcp_uc_gat_norm + pon_stats.pon_mpcp_uc_gat_frpt;
    counter->tx_report_frames = pon_stats.pon_mpcp_tx_rpt;

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_onu_queue_drop_counter_get_adapt                 */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_onu_queue_drop_counter_get_adapt (
                            oam_kt_onu_queue_drop_counter_container_struct *counter
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
     return OAM_E_ERROR;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_port_counter_get_adapt                           */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_port_counter_get_adapt (
                            oam_uint8  port_id,
                            oam_kt_onu_port_counter_container_struct *counter
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
     return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_diagnostics_get_adapt                            */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_diagnostics_get_adapt (
                            oam_uint16 *temp,
                            oam_uint16 *tx_power,
                            oam_uint16 *rx_power
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
     return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_loop_detect_set_adapt                            */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_loop_detect_set_adapt (
                            oam_uint8 oper,
                            oam_uint8 interval,
                            oam_uint8 block_time

)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
#ifdef HAVE_LOOP_DETECT
    if(oper)
    {
        loop_detect_interval_set(interval*1000);
        loop_detect_block_time_set(block_time*1000);
        loop_detect_enable(OAM_ALL_UNI_PORT_ID);
    }
    else
    {
        loop_detect_disable(OAM_ALL_UNI_PORT_ID);
    }
#endif
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_loop_detect_get_adapt                            */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_loop_detect_get_adapt (
                            oam_uint8 *oper,
                            oam_uint8 *interval,
                            oam_uint8 *block_time
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
#ifdef HAVE_LOOP_DETECT
    oam_boolean enable = 0;
    oam_uint32 loop_interval = 0;
    oam_uint32 loop_block = 0;

    if(!oper || !interval || !block_time)
    {
        return OAM_E_PARAM;
    }
    loop_detect_oper_get(OAM_ALL_UNI_PORT_ID, &enable);
    loop_interval = loop_detect_interval_get();
    loop_block = loop_detect_block_time_get();
    *oper = enable;
    *interval = (oam_uint8)(loop_interval/1000);
    *block_time = (oam_uint8)(loop_block/1000);
#endif
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_dhcp_option_cfg_get_adapt                        */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_dhcp_option_cfg_get_adapt (
                            oam_uint8 *cfg)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    dhcp_cfg_get(cfg);

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_dhcp_option_cfg_set_adapt                        */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_dhcp_option_cfg_set_adapt (
                            oam_uint8 cfg)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    dhcp_enable(cfg);

    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_tx_power_off_set_adapt                           */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
extern void __laser_tx_power_on(void *data);
extern oam_uint32 g_laser_tx_power_timer;

oam_status kt_prv_oam_tx_power_off_set_adapt (
                            oam_uint8 action,
                            oam_uint8 duration

)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
   #if 1
    //ca_callback_context_t   context;
    oam_uint8 expire = duration;
    ca_epon_port_laser_mode_t mode;

    if(g_laser_tx_power_timer!=0) /*stop timer incase called before previous timer expired */
    {
        ca_timer_del(g_laser_tx_power_timer);
        g_laser_tx_power_timer = 0;
    }

    if(action == 0)
    {
        /*onu_auto_reboot_enable(1);
        onu_reboot_prot_enable(1);*/
        //ca_pon_laser_tx_set(0, 0);
        CA_GPIO_SET_VALUE(CA_GPIO_PON_TX_POWRER_EN, 0);
        CA_GPIO_SET_VALUE(CA_GPIO_TX_DISABLE, 1);
        mode = CA_EPON_PORT_LASER_MODE_OFF;
        cap_log_msg(0, CA_MOD_ORG_OAM_APP, CAP_LOG_EMERGENCY,0, "Tx power off start, turn off TX power\n");
        ca_epon_port_laser_mode_set(0,CA_PORT_ID(CA_PORT_TYPE_EPON, 7), mode);
        //epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, 1);/***action =0, turn off power off function****/
        return OAM_E_OK;
    }

    /*Bug 43399*/
    if(expire == 0){
        /*When 0x00 received, LynxD set TX power as off mode first and make it on state because the value was 0x00.
        It will make ONU deregistration during short time and register again.*/

        expire = 2;
    }

    if(expire!=0xff)
    {
        cap_log_msg(0, CA_MOD_ORG_OAM_APP, CAP_LOG_EMERGENCY,0, "Tx power off start, turn off TX power, time = %d seconds\n", expire);
        g_laser_tx_power_timer = ca_timer_add(expire*1000, __laser_tx_power_on, NULL);
    }
    /* if expiration value ==0xff, turn off permanently */
    /*
    onu_auto_reboot_enable(0);

    epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, 0);
    epon_request_onu_pon_rx_enable_set(context, 0, 0, 0);
    */
    //ca_pon_laser_tx_set(0, 0);
    CA_GPIO_SET_VALUE(CA_GPIO_PON_TX_POWRER_EN, 0);
    CA_GPIO_SET_VALUE(CA_GPIO_TX_DISABLE, 1);

    mode = CA_EPON_PORT_LASER_MODE_OFF;
    ca_epon_port_laser_mode_set(0,CA_PORT_ID(CA_PORT_TYPE_EPON, 7), mode);
#endif
    return OAM_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_ds_shaping_set_adapt                             */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_ds_shaping_set_adapt (
                            oam_uint8 port_id,
                            oam_uint8 oper,
                            oam_uint32 rate
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_shaper_t shaper;
    ca_status_t rc = 0;
    ca_port_id_t ca_port;

    ca_port = OAM_PORT_ID_TO_CA_PORT(port_id);

    shaper.enable = oper;
    shaper.pps = FALSE;
    shaper.rate = rate;
    shaper.burst_size = 500;
    rc = ca_port_shaper_set(0, ca_port, &shaper);
    return rc;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_ds_shaping_get_adapt                             */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_ds_shaping_get_adapt (
                            oam_uint8 port_id,
                            oam_uint8 *oper,
                            oam_uint8 *rate
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_shaper_t  shaper;
    ca_status_t rc = 0;
    ca_port_id_t ca_port;

    ca_port = OAM_PORT_ID_TO_CA_PORT(port_id);

    memset(&shaper, 0, sizeof(ca_shaper_t));
    rc = ca_port_shaper_get(0, ca_port, &shaper);
    *oper = shaper.enable;
    *rate = (shaper.rate >> 16)&0xff;
    *(rate + 1) = (shaper.rate >> 8)&0xff;
    *(rate + 2) = shaper.rate & 0xff;

    return rc;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_eth_rstp_set_adapt                               */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_eth_rstp_set_adapt (
                            oam_uint8 oper
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status ret = OAM_E_OK;

#ifdef HAVE_ONU_RSTP
    if(KT_ONU_RSTP_ACTIVATED == oper)
        tk_oam_onu_stp_set_adapt(1);
    else
        tk_oam_onu_stp_set_adapt(0);
#endif

     return ret;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_eth_rstp_get_adapt                               */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_eth_rstp_get_adapt (
                            oam_uint8 *oper
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_status ret = OAM_E_OK;

#ifdef HAVE_ONU_RSTP
    tk_oam_onu_stp_get_adapt(oper);
#endif

     return ret;
}


/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_static_mac_get_adapt                             */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_static_mac_get_adapt (
                            oam_uint8 *mac,
                            oam_uint8 *num
)
/* INPUTS     :                                                                */
/*      o port -- port                                                         */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* Get vlan filter value                                                       */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    ca_l2_addr_entry_t *entry = NULL, l2_entry;
    ca_iterator_t return_entry;
    ca_status_t status;
    ca_uint8 *ptr = NULL;

    memset(&return_entry, 0, sizeof(ca_iterator_t));
    memset(&l2_entry, 0, sizeof(ca_l2_addr_entry_t));
    l2_entry.static_flag = 0x10;
    *num = 0;
    ptr = mac;
    return_entry.entry_count = 1;
    return_entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * return_entry.entry_count;
    return_entry.entry_data = &l2_entry;

    status = ca_l2_addr_iterate(0, &return_entry);

    while (CA_E_OK == status && *num < KT_ORG_MAX_STATIC_MAC){
        if (return_entry.entry_count == 1){
            entry = (ca_l2_addr_entry_t *)return_entry.entry_data;
            if (entry->static_flag && entry->mc_group_id == CA_UINT32_INVALID){
                memcpy(ptr + *num * CA_ETH_ADDR_LEN, entry->mac_addr, CA_ETH_ADDR_LEN);
                (*num)++;
            }
        }
        /* to retrieve static */
        entry->static_flag = 0x10;
        status = ca_l2_addr_iterate(0, &return_entry);
    }
    return 0;
}

oam_status kt_prv_oam_all_static_fdb_entries_del_adapt ()
{
    ca_l2_addr_delete_all(0, CA_L2_ADDR_OPERATION_STATIC);
    return 0;
}


oam_status kt_prv_oam_all_dynamic_fdb_entries_clr_adapt ()
{
    ca_l2_addr_delete_all(0, CA_L2_ADDR_OPERATION_DYNAMIC);
    return 0;
}

oam_status kt_prv_oam_static_fdb_entries_add_adapt (
                            oam_uint8 *mac,
                            oam_uint8 num
)
{
    ca_l2_addr_entry_t l2_addr;
    oam_uint8 i = 0;

    if (!mac || !num)
        return OAM_E_ERROR;

    kt_prv_oam_all_static_fdb_entries_del_adapt();

    memset(&l2_addr, 0, sizeof(l2_addr));

    for (i=0; i<num; i++){
        memcpy(l2_addr.mac_addr, mac + i * CA_ETH_ADDR_LEN, CA_ETH_ADDR_LEN);
        l2_addr.static_flag = 1;
        l2_addr.sa_permit = 1;
        l2_addr.da_permit = 1;
        l2_addr.port_id = CA_UINT16_INVALID;//OAM_PORT_ID_TO_CA_PORT(OAM_UNI_PORT_ID1);
        if (ca_l2_addr_add(0, &l2_addr) != OAM_E_OK)
            return OAM_E_ERROR;
    }
    return OAM_E_OK;
}

oam_status kt_prv_oam_default_restore_adapt ()
{
     return OAM_E_OK;
}

oam_status kt_prv_oam_power_reset_adapt ()
{
    /* GPIO output */
    return OAM_E_OK;
}

oam_status kt_prv_oam_onu_per_port_mac_entries_get_adapt (ca_uint8 port, oam_kt_onu_per_port_mac_entries_t *pFdbEntries, ca_uint8 *num)
{
    ca_l2_addr_entry_t l2_entry;
    ca_iterator_t return_entry;
    ca_status_t status;
    ca_port_id_t ca_port;
    ca_boolean_t get_all = FALSE;
    int port_cnt[4] = {0};
    int j = 0;
    ca_uint32_t oam_port;

    if(OAM_ALL_UNI_PORT_ID == port)
        get_all = TRUE;

    memset(&return_entry, 0, sizeof(ca_iterator_t));
    memset(&l2_entry, 0, sizeof(ca_l2_addr_entry_t));
    *num = 0;
    return_entry.entry_count = 1;
    return_entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * return_entry.entry_count;
    return_entry.entry_data = &l2_entry;

    status = ca_l2_addr_iterate(0, &return_entry);

    while (CA_E_OK == status && return_entry.entry_count == 1){
        if (l2_entry.mc_group_id == CA_UINT32_INVALID){
            oam_port = OAM_CA_PORT_TO_OAM_PORT(l2_entry.port_id);
            if(get_all){
                if(oam_port >= OAM_UNI_PORT_ID1 && oam_port <= OAM_UNI_PORT_NUM){
                    if(port_cnt[oam_port - 1] < KT_ORG_MAX_ONU_MAC_PER_PORT){
                        pFdbEntries->hdr.extBranch= 0xA7;
                        pFdbEntries->hdr.leaf = htons(0x0022);
                        pFdbEntries->hdr.len = 0x07;
                        pFdbEntries->uni_port = oam_port;
                        memcpy(pFdbEntries->mac, l2_entry.mac_addr, 6);
                        pFdbEntries++;
                        j++;
                        port_cnt[oam_port - 1]++;
                    }
                }
            }
            else{
                if (oam_port == port && oam_port != OAM_PON_PORT_ID){
                    pFdbEntries->hdr.extBranch = 0xA7;
                    pFdbEntries->hdr.leaf = htons(0x0022);
                    pFdbEntries->hdr.len = 0x07;
                    pFdbEntries->uni_port = port;
                    memcpy(pFdbEntries->mac, l2_entry.mac_addr, 6);
                    pFdbEntries++;
                    j++;
                    /* single port max 64 entries */
                    if(j >= KT_ORG_MAX_ONU_MAC) {
                        break;
                    }
                }
            }
        }
        l2_entry.static_flag = 0;
        status = ca_l2_addr_iterate(0, &return_entry);

    }

    *num = j;

    return 0;
}
oam_status kt_prv_oam_onu_mac_table_get_adapt (
                            oam_uint8 *mac,
                            oam_uint32 *num)
{
    ca_l2_addr_entry_t *entry = NULL, l2_entry;
    ca_iterator_t return_entry;
    ca_status_t status;
    ca_uint8 *ptr = NULL;
    ca_uint32_t mac_count = 0;
    ca_uint32_t oam_port;

    memset(&return_entry, 0, sizeof(ca_iterator_t));
    memset(&l2_entry, 0, sizeof(ca_l2_addr_entry_t));
    l2_entry.static_flag = 0;
    ptr = mac;
    return_entry.entry_count = 1;
    return_entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * return_entry.entry_count;
    return_entry.entry_data = &l2_entry;

    status = ca_l2_addr_iterate(0, &return_entry);
    while (CA_E_OK == status && mac_count < KT_ORG_MAX_ONU_MAC){
        if (return_entry.entry_count == 1){
            entry = (ca_l2_addr_entry_t *)return_entry.entry_data;
            oam_port = OAM_CA_PORT_TO_OAM_PORT(entry->port_id);
            if (entry->static_flag == 0 && oam_port != OAM_PON_PORT_ID && entry->mc_group_id == CA_UINT32_INVALID){
                memcpy(ptr + mac_count * CA_ETH_ADDR_LEN, entry->mac_addr, CA_ETH_ADDR_LEN);
                mac_count++;
            }
            /* indicate get dynamic */
            entry->static_flag = 0;
        }
        status = ca_l2_addr_iterate(0, &return_entry);
    }

    if (num) *num = mac_count;
    OAM_ORG_LOG("dynamic count: %d\n", *num);
    return 0;
}


/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_opm_ddm_get_adapt                                */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_status kt_prv_oam_opm_ddm_get_adapt(
    oam_int16    *temperature,
    oam_uint16   *voltage,
    oam_uint16   *tx_bias,
    oam_uint16   *tx_power,
    oam_uint16   *rx_power
)
/* INPUTS     :                                                                */
/*      o temperature -- temperature                                           */
/*      o voltage     -- voltage                                               */
/*      o tx_bias     -- current tx bias                                       */
/*      o tx_power    -- tx power                                             */
/*      o rx_power    -- rx power                                              */
/* OUTPUTS    :                                                                */
/*      o status                                                               */
/* RETURNS    : o result code, 0 - success, 1 - fail,                          */
/*              6 - invalid parameters                                         */
/* DESCRIPTION:                                                                */
/* action  reset                                                               */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    opm_diag_info_t diag_info;


    if(NULL == temperature || NULL == tx_power || NULL == rx_power || NULL == voltage || NULL == tx_bias)
        return CA_E_ERROR;

    memset(&diag_info, 0, sizeof(opm_diag_info_t));

    opm_ddm_raw_data_get(0, 0, &diag_info);

    *temperature = diag_info.temp;
    *voltage = diag_info.volt;
    *tx_bias = diag_info.tx_bias;
    *tx_power = diag_info.tx_pwr;
    *rx_power = diag_info.rx_pwr;

    OAM_ORG_DEBUG("temp = %#x vol = %#x, bias = %#x,txpower = %#x,rxpower = %#x \n",
            *temperature, *voltage, *tx_bias, *tx_power, *rx_power);

    return CA_E_OK;
}

/*******************************************************************************/
/* $rtn_hdr_start  kt_prv_oam_is_new_board_get_adapt                           */
/* CATEGORY   : Device                                                         */
/* ACCESS     : Public                                                         */
/* BLOCK      : SAL                                                            */
/* CHIP       : LynxD                                                          */
oam_boolean kt_prv_oam_is_new_board_get_adapt()
/* INPUTS     :                                                                */
/* OUTPUTS    :                                                                */
/* RETURNS    : o true: new board, false: old board                            */
/* DESCRIPTION:                                                                */
/* action  reset                                                               */
/*                                                                             */
/* $rtn_hdr_end                                                                */
/*******************************************************************************/
{
    oam_uint32 ret = 0;
   #if 0
#ifdef RM_KT_LINUX
    ret = i2c_KT_newboard();
#endif
#endif
    if (ret)
        return TRUE;
    else
        return FALSE;
}


oam_status kt_prv_oam_onu_time_set_adapt (
    kt_prv_system_time_t *sys_time

)
{
    if (!sys_time)
        return OAM_E_ERROR;

    struct timeval tv;
    struct tm _tm;
    time_t timep;

    _tm.tm_sec = sys_time->second;
    _tm.tm_min = sys_time->minute;
    _tm.tm_hour = sys_time->hour;
    _tm.tm_mday = sys_time->day;
    _tm.tm_mon = sys_time->month - 1;
    _tm.tm_year = sys_time->year - 1900;

    timep = mktime(&_tm);
    tv.tv_sec = timep;
    tv.tv_usec = 0;

    if(settimeofday(&tv, (struct timezone *)0) < 0){
        OAM_ORG_DEBUG("settimeofday failed\r\n");
        return OAM_E_ERROR;
    }
    return OAM_E_OK;
}


oam_status kt_prv_oam_ddm_threshold_set_adapt(ddm_threshold_t *data)
{
    ca_uint32_t alarm_id;

    if (data == NULL) return CA_E_PARAM;

    thres_id_2_alarm_id(data->id - 1, &alarm_id);
    ca_opm_alarm_thresh_set(0, 0, alarm_id,
                            data->high_raise.unsign_value, data->high_clear.unsign_value);

    ca_opm_alarm_thresh_set(0, 0, alarm_id + 1,
                            data->low_raise.unsign_value, data->low_clear.unsign_value);
    return CA_E_OK;
}

oam_status kt_prv_oam_ddm_threshold_get_adapt(ddm_threshold_t *data)
{
    ca_uint32_t alarm_id;

    if (data == NULL) return CA_E_PARAM;

    thres_id_2_alarm_id(data->id - 1, &alarm_id);

    ca_opm_alarm_thresh_get(0, 0, alarm_id, &data->high_raise.unsign_value, &data->high_clear.unsign_value);
    ca_opm_alarm_thresh_get(0, 0, alarm_id + 1, &data->low_raise.unsign_value, &data->low_clear.unsign_value);
    ca_opm_thres_status_get(0, 0, data->id - 1, &data->state);

    return CA_E_OK;
}

oam_status oam_kt_adapt_set_ipAddr(oam_uint8 *dev, oam_uint32 ipAddr)
{
    struct ifreq       ifr;
    struct sockaddr_in *host = NULL;

    if(!dev)
        return OAM_E_PARAM;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("socket");
        return OAM_E_ERROR;
    }

    bzero(&ifr, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, dev, KT_MAX_IFNAME_LEN);
    host = (struct sockaddr_in*)&ifr.ifr_addr;
    host->sin_family = AF_INET;
    host->sin_addr.s_addr = htonl(ipAddr);

    if ( ioctl(sockfd, SIOCSIFADDR, &ifr) < 0 )
    {
        perror("ioctl");
        close(sockfd);
        return OAM_E_ERROR;
    }

    close(sockfd);

    return OAM_E_OK;
}

oam_status oam_kt_adapt_set_ipMask(oam_uint8 *dev, oam_uint32 mask)
{
    struct ifreq       ifr;
    struct sockaddr_in *host = NULL;

    if(!dev)
        return OAM_E_PARAM;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("socket");
        return OAM_E_ERROR;
    }

    //cfg mask
    bzero(&ifr, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, dev, KT_MAX_IFNAME_LEN);
    host = (struct sockaddr_in*)&ifr.ifr_addr;
    host->sin_family = AF_INET;
    host->sin_addr.s_addr = htonl(mask);

    if ( ioctl(sockfd, SIOCSIFNETMASK, &ifr) < 0 )
    {
        perror("ioctl");
        close(sockfd);
        return OAM_E_ERROR;
    }

    close(sockfd);

    return OAM_E_OK;
}

oam_status oam_kt_adapt_set_dev_up(oam_uint8 *dev)
{
    int          sock_fd;
    struct ifreq ifr;
    int          selector;

   if(!dev)
       return OAM_E_PARAM;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        perror("socket");
        return OAM_E_ERROR;
    }

    bzero(&ifr, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, dev, KT_MAX_IFNAME_LEN);

    if(ioctl(sock_fd, SIOCGIFFLAGS, &ifr) < 0)
    {
        perror("ioctl SIOCGIFFLAGS");
        close(sock_fd);
        return OAM_E_ERROR;
    }

    selector = (IFF_UP | IFF_RUNNING);
    ifr.ifr_flags |= selector;
    if(ioctl(sock_fd, SIOCSIFFLAGS, &ifr) < 0)
    {
        perror("ioctl SIOCGIFFLAGS");
        close(sock_fd);
        return OAM_E_ERROR;
    }

    close(sock_fd);

    return OAM_E_OK;
}

oam_status oam_kt_adapt_dev_vlan_add(oam_uint8 *dev, oam_uint16 vlanId)
{
    int fd;
    int ret;
    struct vlan_ioctl_args ifr;

    if(!dev)
        return OAM_E_PARAM;

    if(vlanId > KT_MAX_VLANID || KT_MAX_VLANID < 0)
        return OAM_E_PARAM;

    memset(&ifr, 0x0, sizeof(ifr));
    ifr.cmd = ADD_VLAN_CMD;
    strncpy(ifr.device1, dev, KT_MAX_IFNAME_LEN);
    ifr.u.VID = vlanId;

    fd  = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
        return OAM_E_ERROR;

    ret = ioctl(fd, SIOCSIFVLAN, &ifr);
    if(ret < 0)
    {
        perror("ioctl");
        close(fd);
        return OAM_E_ERROR;
    }

    close(fd);
    return OAM_E_OK;
}

oam_status oam_kt_adapt_dev_vlan_del(oam_uint8 *dev)
{
    int fd;
    int ret;
    struct vlan_ioctl_args ifr;

    if(!dev)
        return OAM_E_PARAM;

    memset(&ifr, 0x0, sizeof(ifr));
    ifr.cmd = DEL_VLAN_CMD;
    strncpy(ifr.device1, dev, KT_MAX_IFNAME_LEN);

    fd  = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
        return OAM_E_ERROR;

    ret = ioctl(fd, SIOCSIFVLAN, &ifr);
    if(ret < 0)
    {
        perror("ioctl");
        close(fd);
        return OAM_E_ERROR;
    }

    close(fd);
    return OAM_E_OK;
}

oam_int32 oam_kt_adapt_vlan_port_set(oam_uint32 vid)
{
    oam_status   rc = 0;
    ca_port_id_t ca_port;
    ca_port_id_t member_ports[16];
    oam_uint8    member_count = 0;
    oam_uint8    untagged_count = 0;
    ca_port_id_t untagged_ports[16];
    int          idx;

    memset(member_ports, 0x0, sizeof(ca_port_id_t)*16);
    memset(untagged_ports, 0x0, sizeof(ca_port_id_t)*16);

    rc = ca_l2_vlan_create(0, vid);
    if(rc == CA_E_EXISTS){
        OAM_ORG_DEBUG("vlan %d is existed\r\n", vid);
    }

    if(rc == CA_E_OK){
        OAM_ORG_DEBUG("vlan %d is created\r\n", vid);
    }

    if (rc == CA_E_EXISTS || rc == CA_E_OK)
    {
        ca_l2_vlan_port_get(0, vid, &member_count, member_ports, &untagged_count, untagged_ports);

        OAM_ORG_DEBUG("member ports count %d untagged_count %d\r\n", member_count, untagged_count);
        for(idx = 0; idx < 16; idx++)
        {
            OAM_ORG_DEBUG("member_ports 0x%x ", member_ports[idx]);
            OAM_ORG_DEBUG("untagged_ports 0x%x ", untagged_ports[idx]);
        }

        ca_port = OAM_PORT_ID_TO_CA_PORT(OAM_PON_PORT_ID);

        member_ports[member_count] = ca_port;
        OAM_ORG_DEBUG("add port %x to vlan %d membership \n", member_ports[member_count], vid);
        member_count ++;
        //add cpu port to vlan member
        member_ports[member_count] = OAM_PORT_ID_TO_CA_PORT(OAM_CPU0_PORT_ID);
        OAM_ORG_DEBUG("add port %x to vlan %d membership \n", member_ports[member_count], vid);
        member_count ++;
        member_ports[member_count] = OAM_PORT_ID_TO_CA_PORT(OAM_CPU1_PORT_ID);
        OAM_ORG_DEBUG("add port %x to vlan %d membership \n", member_ports[member_count], vid);
        member_count ++;

        rc = ca_l2_vlan_port_set(0,vid, member_count, member_ports, untagged_count, untagged_ports);
        OAM_ORG_DEBUG("ca_l2_vlan_port_set return %d\n", rc);
    }

    return OAM_E_OK;
}

oam_int32 oam_kt_adapt_vlan_port_del(oam_uint32 vid)
{
    oam_status   rc                  = 0;
    ca_port_id_t ca_port;
    ca_port_id_t cpu_port0;
    ca_port_id_t cpu_port1;
    ca_port_id_t member_ports[16];
    oam_uint8    member_count        = 0;
    ca_port_id_t new_member_ports[16];
    oam_int32    new_member_count    = 0;
    oam_uint8    untagged_count      = 0;
    ca_port_id_t untagged_ports[16];
    oam_int32    idx                 = 0;
    oam_int32    newMemberPortIdx    = 0;
    ca_status_t  ret                 = CA_E_OK;

    memset(member_ports, 0x0, sizeof(ca_port_id_t)*16);
    memset(new_member_ports, 0x0, sizeof(ca_port_id_t)*16);
    memset(untagged_ports, 0x0, sizeof(ca_port_id_t)*16);

    ret = ca_l2_vlan_port_get(0, vid, &member_count, member_ports, &untagged_count, untagged_ports);
    if (CA_E_OK != ret)
    {
        OAM_ORG_LOG_MAJ("l2 vlan port get err\r\n");
        return -1;
    }

    OAM_ORG_DEBUG("member ports count %d untagged_count %d\r\n", member_count, untagged_count);
    for(idx = 0; idx < 16; idx++)
    {
        OAM_ORG_DEBUG("member_ports 0x%x ", member_ports[idx]);
        OAM_ORG_DEBUG("untagged_ports 0x%x ", untagged_ports[idx]);
    }

    ca_port   = OAM_PORT_ID_TO_CA_PORT(OAM_PON_PORT_ID);
    cpu_port0 = OAM_PORT_ID_TO_CA_PORT(OAM_CPU0_PORT_ID);
    cpu_port1 = OAM_PORT_ID_TO_CA_PORT(OAM_CPU1_PORT_ID);

    for(idx = 0; idx < member_count; idx++)
    {
        if((member_ports[idx] == ca_port) ||
            (member_ports[idx] == cpu_port0) ||
            (member_ports[idx] == cpu_port1))
            continue;

        new_member_ports[newMemberPortIdx] = member_ports[idx];
        newMemberPortIdx++;
        new_member_count++;
    }

    rc = ca_l2_vlan_port_set(0, vid, new_member_count, new_member_ports, untagged_count, untagged_ports);
    OAM_ORG_DEBUG("ca_l2_vlan_port_set return %d\n", rc);

    return OAM_E_OK;
}

void oam_kt_adapt_over_voltage_status_get(oam_uint32 *status)
{
    ca_uint32 value ;

    ca_gpio_get_value_exe(CA_GPIO_OVER_VOLTAGE_ID, &value);
    if(value == 0){
        *status = OAM_KT_OVER_VOLTAGE_RAISE; /* over voltage */
    }else{
        *status = OAM_KT_OVER_VOLTAGE_RELEASE; /* normal voltage */
    }
    return;
}

oam_int32 oam_kt_adapt_build_over_voltage_event(void *arg, void *data)
{
    ca_uint32 *value = (ca_uint32 *)data;
    ca_uint8 status;

    if(*value == 0){
        status = OAM_KT_OVER_VOLTAGE_RAISE; /* over voltage */
    }else{
        status = OAM_KT_OVER_VOLTAGE_RELEASE; /* normal voltage */
    }
    oam_build_voltage_event(status, 1);
}

oam_int32 oam_kt_adapt_build_lao_detect_event(void *arg, void *data)
{
    oam_build_lao_event(1);
}

void oam_kt_pon_act_handle(void *arg, void *data)
{
    ca_epon_port_stats_t port_stats;
    ca_uint64            stats    = 0;;
    ca_led_pon_mgr_t     *pon_mgr = (ca_led_pon_mgr_t *)arg;
    ca_status_t          ret;
    ca_eth_port_link_status_t link = CA_PORT_LINK_DOWN, temp_link;
    ca_eth_port_speed_t speed;
    int i;
    oam_uint32 enable = 0;

    /* PON port */
    oam_stats_polling_enable_get(0, &enable);

    if(!enable)
    {
        return;
    }

    memset(&port_stats, 0x0, sizeof(ca_epon_port_stats_t));

    if(!pon_mgr)
        return;

    for (i = 3; i < 7; i++)
    {
        if (i == 4 || i ==6)

            ca_eth_port_link_status_get(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET, i), &temp_link);
        
        if (temp_link) link = CA_PORT_LINK_UP;
    }

    if (!link)
    {
        ca_led_pon_data_set(CA_LED_OFF_EVT);
        return;
    }

    ret = ca_epon_port_stats_get(0, CA_LED_PON_PORT_ID(), FALSE, &port_stats);
    if(CA_E_OK != ret)
    {
        OAM_ORG_DEBUG("get pon port stats err\r\n");
        return;
    }

    stats = port_stats.rx_uc_frames + port_stats.rx_mc_frames + port_stats.rx_bc_frames +
            port_stats.tx_uc_frames + port_stats.tx_mc_frames + port_stats.tx_bc_frames;

    //OAM_ORG_DEBUG("pon port stats %ld\r\n", stats);

    if(pon_mgr->stats != stats)
    {
        //OAM_ORG_DEBUG("stats change \r\n");
        ca_led_pon_data_set(CA_LED_BLINK_EVT);
        pon_mgr->stats = stats;

        return;
    }

    //OAM_ORG_DEBUG("stats no change\r\n");
    ca_led_pon_data_set(CA_LED_OFF_EVT);

    return;
}


oam_status oam_kt_pon_led_set_adapt(oam_uint8 action)
{
#ifdef HAVE_CA_LED
    if (OAM_LED_BLINK == action)
        CA_LED_PON_REGISTER_SET_BLINK();
    else if (OAM_LED_OFF == action)
        CA_LED_PON_REGISTER_SET_OFF();
    else
        CA_LED_PON_REGISTER_SET_ON();
#endif
    return OAM_E_OK;
}

static void __kt_event_register()
{
    ca_status_t status = CA_E_OK;
    
#ifdef HAVE_LOOP_DETECT
    loop_detect_special_handler_reg(kt_oam_loop_detect_handle);
#endif

    status = ca_event_register(0, CA_EVENT_ETH_PORT_LINK, oam_port_link_event_handler, NULL);
    if (CA_E_OK != status) {
        OAM_LOG_INFO("Error1: Port Link Event reg failed. ret=0x%x\n", status);
        return OAM_E_ERROR;
    }
    
    status = ca_event_register(0, CA_EVENT_MAC_LIMIT_3, oam_build_mac_limit_3_event, NULL);
    if (CA_E_OK != status) {
        OAM_LOG_INFO("Error1: Reg state Event reg failed. ret=0x%x\n", status);
        return OAM_E_ERROR;
    }

    status = ca_event_register(0, CA_EVENT_SWITCH_HANG, oam_switch_event_handler, NULL);
    if (CA_E_OK != status) {
        OAM_LOG_INFO("Error2: switch hang Event reg failed. ret=0x%x\n", status);
        return OAM_E_ERROR;
    }
    
    ld_oam_even_send_register(stp_send_oam_tk_org_event);
    ddm_event_send_register(oam_build_ddm_event);
    ca_gpio_trigger_cb_register(CA_GPIO_OVER_VOLTAGE_ID, oam_kt_adapt_build_over_voltage_event);
    ca_gpio_trigger_cb_register(CA_GPIO_LAO_DETEC_ID, oam_kt_adapt_build_lao_detect_event);
    ca_led_pon_act_handle_register(oam_kt_pon_act_handle);
    ca_led_opm_alarm_register(oam_build_opm_event);
}

static void __kt_vlan_init()
{
    oam_uint32 i = 0;
    ca_port_id_t ca_port;
    epon_tk_port_vlan_member_config_t member;
    epon_tk_port_vlan_policy_config_t policy;
    ca_vlan_port_control_t config;

    memset(&policy, 0, sizeof(epon_tk_port_vlan_policy_config_t));
    policy.egress_policy = 3;
    policy.ingress_policy = 3;
    policy.vlan = 1;

    memset(&member, 0, sizeof(epon_tk_port_vlan_member_config_t));
    member.ingress_vlan = 1;
    member.egress_vlan = 1;
    member.egress_tag = 2;

    /* set PVID = 1; and always drop unknown vlan */
    for(i = 0; i < OAM_UNI_PORT_NUM+1; i++){
        tk_oam_port_vlan_policy_set_adapt(i,&policy);
        tk_oam_port_vlan_member_set_adapt(i,&member);
        tk_oam_vlan_member_cfg_adapt(i);

        ca_port = OAM_PORT_ID_TO_CA_PORT(i);
        ca_l2_vlan_port_control_get(0,ca_port, &config);
        config.drop_unknown_vlan = 1;
        ca_l2_vlan_port_control_set(0,ca_port, &config);
    }

    /* ONU downstream shaper: add rule that foward PON data to L3FE. so need to set the pvid of L3WAN and L3LAN port to 1 */
    ca_port_pvid_vlan_set(0, CA_PORT_ID(CA_PORT_TYPE_L2RP, CA_PORT_ID_L3_WAN), 1);
    ca_port_pvid_vlan_set(0, CA_PORT_ID(CA_PORT_TYPE_L2RP, CA_PORT_ID_L3_LAN), 1);

    ca_port_pvid_vlan_set(0, CA_PORT_ID(CA_PORT_TYPE_CPU, 0x10), 1);
    
    return;
}

#define __CLS_ETH_TYPE_ARP      0x0806
#define __CLS_ETH_TYPE_88D9     0x88d9
#define __CLS_LOGIC_PORT_MIN    3
#define __CLS_LOGIC_PORT_WAN    7
#define __CLS_ARP_PRIORITY      6
#define __CLS_ICMP_PRIORITY     6
#define __CLS_IGMP_PRIORITY     6
#define __CLS_DHCP_PRIORITY     6
#define __CLS_PRPTOCOL_ICMP     1
#define __CLS_PRPTOCOL_EIGRP    88
#define __CLS_IP_ADDR_LENGTH    32
#define __CLS_DRP_IPDA_1        0xe0000001

#define __CLS_DRP_IPDA_2        0xeffffffa

#define __CLS_DEF_RULE_PRECEDENCE 1

static void __kt_default_rule_set(CA_IN ca_device_id_t device_id)
{
    ca_classifier_key_t         cls_key;
    ca_classifier_key_mask_t    cls_key_mask;
    ca_classifier_action_t      cls_action;
    ca_uint32_t                 cls_index = 0;
    ca_uint32_t                 logic_port= 0;
    ca_port_id_t                src_port;
    ca_status_t                 ret = CA_E_OK;
#if 0
    ca_classifier_handle_t      action_handle = {0};
    ca_policer_t                policer;

    memset(&action_handle, 0, sizeof(ca_classifier_handle_t));
    memset(&policer, 0, sizeof(ca_policer_t));

    for (logic_port = 3; logic_port < 7; logic_port ++)
    {
        src_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, logic_port);
        action_handle.flow_id = CA_FLOW_ID_US_ARP;
        ca_special_packet_set(0, src_port, CA_PKT_TYPE_ARP,FALSE,
            CA_PORT_ID(CA_PORT_TYPE_EPON, 7),7, 1, &action_handle);
        action_handle.flow_id = CA_FLOW_ID_US_DHCP;
        ca_special_packet_set(0, src_port, CA_PKT_TYPE_DHCP,FALSE,
            CA_PORT_ID(CA_PORT_TYPE_EPON, 7),7, 1, &action_handle);
        action_handle.flow_id = CA_FLOW_ID_US_DHCP;
        ca_special_packet_set(0, src_port, CA_PKT_TYPE_ICMP,FALSE,
            CA_PORT_ID(CA_PORT_TYPE_EPON, 7),2, 0, &action_handle);
    }
    src_port = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
    action_handle.flow_id = CA_FLOW_ID_DS_ARP;
    ca_special_packet_set(0, src_port, CA_PKT_TYPE_ARP,FALSE,
            CA_PORT_ID(CA_PORT_TYPE_ETHERNET, 3),7, 1, &action_handle);
    action_handle.flow_id = CA_FLOW_ID_DS_DHCP;
    ca_special_packet_set(0, src_port, CA_PKT_TYPE_DHCP,FALSE,
            CA_PORT_ID(CA_PORT_TYPE_ETHERNET, 3),7, 0, &action_handle);
    ca_special_packet_set(0, src_port, CA_PKT_TYPE_ICMP,FALSE,
            CA_PORT_ID(CA_PORT_TYPE_ETHERNET, 3),2, 0, &action_handle);

    policer.mode = CA_POLICER_MODE_TRTCM;
    policer.pps = 1;
    policer.cir = 50;
    policer.cbs = 10;
    policer.pbs = 10;
    policer.pir = 50;

    ca_flow_policer_set ( 0, CA_FLOW_ID_US_ARP , &policer);
    ca_flow_policer_set ( 0, CA_FLOW_ID_DS_ARP , &policer);
    ca_flow_policer_set ( 0, CA_FLOW_ID_US_DHCP , &policer);
#endif

    memset(&cls_key, 0, sizeof(ca_classifier_key_t));
    memset(&cls_key_mask, 0, sizeof(ca_classifier_key_mask_t));
    memset(&cls_action, 0, sizeof(ca_classifier_action_t));
    cls_key_mask.src_port               = 1;
    cls_key_mask.l2                     = 1;
    cls_key_mask.l2_mask.ethertype      = 1;
    cls_key.l2.ethertype                = __CLS_ETH_TYPE_88D9;
    cls_action.forward                  = CA_CLASSIFIER_FORWARD_DENY;
    for(logic_port = __CLS_LOGIC_PORT_MIN; logic_port<__CLS_LOGIC_PORT_WAN;logic_port++){
        cls_key.src_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, logic_port);
        ret = ca_classifier_rule_add(device_id,__CLS_DEF_RULE_PRECEDENCE,&cls_key,&cls_key_mask,&cls_action,&cls_index);
        CA_RET_VALUE_CHECK(ret);
    }
    memset(&cls_key, 0, sizeof(ca_classifier_key_t));
    memset(&cls_key_mask, 0, sizeof(ca_classifier_key_mask_t));
    memset(&cls_action, 0, sizeof(ca_classifier_action_t));
    cls_key_mask.src_port               = 1;
    cls_key_mask.ip                     = 1;
    cls_key_mask.ip_mask.ip_da          = 1;
    cls_key.ip.ip_da.addr_len           = __CLS_IP_ADDR_LENGTH;
    cls_key.ip.ip_da.ip_addr.ipv4_addr  = __CLS_DRP_IPDA_1;
    cls_action.forward                  = CA_CLASSIFIER_FORWARD_DENY;
    for(logic_port = __CLS_LOGIC_PORT_MIN; logic_port<__CLS_LOGIC_PORT_WAN;logic_port++){
        cls_key.src_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, logic_port);
        ca_classifier_rule_add(device_id,__CLS_DEF_RULE_PRECEDENCE,&cls_key,&cls_key_mask,&cls_action,&cls_index);
        CA_RET_VALUE_CHECK(ret);
    }
    memset(&cls_key, 0, sizeof(ca_classifier_key_t));
    memset(&cls_key_mask, 0, sizeof(ca_classifier_key_mask_t));
    memset(&cls_action, 0, sizeof(ca_classifier_action_t));
    cls_key_mask.src_port               = 1;
    cls_key_mask.ip                     = 1;
    cls_key_mask.ip_mask.ip_da          = 1;
    cls_key.ip.ip_da.addr_len           = __CLS_IP_ADDR_LENGTH;
    cls_key.ip.ip_da.ip_addr.ipv4_addr  = __CLS_DRP_IPDA_2;
    cls_action.forward                  = CA_CLASSIFIER_FORWARD_DENY;
    for(logic_port = __CLS_LOGIC_PORT_MIN; logic_port<__CLS_LOGIC_PORT_WAN;logic_port++){
        cls_key.src_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, logic_port);
        ca_classifier_rule_add(device_id, 7 ,&cls_key,&cls_key_mask,&cls_action,&cls_index);
        CA_RET_VALUE_CHECK(ret);
    }

}


static void __kt_qos_init()
{
    oam_uint32 i = 0;
    ca_port_id_t ca_port;
    ca_qos_config_t config;

    for (i = OAM_PON_PORT_ID; i <= OAM_UNI_PORT_NUM; i++)
    {
        ca_port = OAM_PORT_ID_TO_CA_PORT(i);
        ca_qos_config_get(0, ca_port, &config);
        config.qos_map_mode = CA_QOS_MAP_CLASSIFIER;
        if (CA_E_OK != ca_qos_config_set(0, ca_port, &config))
        {
            cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_ERROR,0, "ca_qos_config_set set DSCP mode failed at port %x\n", ca_port);
        }
    }

    ca_port = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x10);
    ca_qos_config_get(0, ca_port, &config);
    config.qos_map_mode = CA_QOS_MAP_CLASSIFIER;
    if (CA_E_OK != ca_qos_config_set(0, ca_port, &config))
    {
        cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_ERROR,0, "ca_qos_config_set set DSCP mode failed at port %x\n", ca_port);
    }
    
}

oam_status __kt_deepq_init()
{
    int i;
    ca_status_t status;
    ca_eth_port_link_status_t link;
    ca_eth_port_speed_t speed;
    ca_shaper_t shaper;
    
    for (i = 4; i < 7; i++)
    {
        ca_eth_port_link_status_get(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET, i), &link);
        ca_eth_port_phy_speed_get(0, CA_PORT_ID(CA_PORT_TYPE_ETHERNET, i), &speed);
        

        if (link == CA_PORT_LINK_UP)
        {
            shaper.pps = 0;
            shaper.enable = 1;

            switch (speed)
            {
               case CA_PORT_SPEED_5G:
                    shaper.burst_size = 16;
                    shaper.rate = 5000000;
                    break;

                case CA_PORT_SPEED_2_5G:
                    shaper.burst_size = 16;
                    shaper.rate = 2500000;
                    break;

                case CA_PORT_SPEED_1G:
                    shaper.burst_size = 32;
                    shaper.rate = 1000000;
                    break;

                case CA_PORT_SPEED_100M:
                    shaper.burst_size = 32;
                    shaper.rate = 100000;
                    break;

                case CA_PORT_SPEED_10M:
                    shaper.burst_size = 32;
                    shaper.rate = 10000;
                    break;

                default:
                    shaper.enable = 0;
                    shaper.rate = 0;
                    break;
            }
            ca_port_shaper_set(0, CA_PORT_ID(CA_PORT_TYPE_L3, i), &shaper);
        }
#if 0    /* USE DEEPQ */        
        if(i == 5){
            ca_eth_port_phy_speed_get(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET, i),&speed);
            if((speed == CA_PORT_SPEED_100M) ||(speed == CA_PORT_SPEED_1G) ||
                (speed == CA_PORT_SPEED_2_5G) ||(speed == CA_PORT_SPEED_5G)){
                ca_eth_port_use_deep_queue(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET, i),TRUE);
            }else{
                ca_eth_port_use_deep_queue(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET, i),FALSE);
            }
        }
#endif
    }
}

/* this configure is for IGMP APP, need to move to IGMP module */
static int __trap_general_query_rule_to_cpu0_init(void )
{
    //ca_classifier_handle_t      action_handle = {0};
    ca_uint32_t                 priority = 7;
    ca_classifier_key_t         cls_key;
    ca_classifier_key_mask_t    cls_key_mask;
    ca_classifier_action_t      cls_action;
    ca_uint32_t                 cls_index = 0;
    ca_port_id_t                ca_portid = 0;
    ca_port_id_t                dst_port = 0;
    ca_uint8                    general_query_mac[] ={0x01, 0x00, 0x5e, 0x00, 0x00, 0x01};
    int                         ret = 0;
    ca_policer_t    policer;

    memset (&policer, 0, sizeof(ca_policer_t));

    /*set arp*/
    ca_portid = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7);
    dst_port =   CA_PORT_ID(CA_PORT_TYPE_CPU, CA_PORT_ID_CPU0);


    /*set my mac*/
    memset(&cls_key, 0, sizeof(ca_classifier_key_t));
    memset(&cls_key_mask, 0, sizeof(ca_classifier_key_mask_t));
    memset(&cls_action, 0, sizeof(ca_classifier_action_t));

    cls_key_mask.src_port = 1;

    cls_key_mask.key_handle = 1;

    cls_key_mask.l2 = 1;
    cls_key_mask.l2_mask.mac_da = 1;
    cls_key_mask.ip = 1;
    cls_key_mask.ip_mask.ip_version = 1;
    cls_key_mask.ip_mask.ip_protocol = 1;
    cls_key_mask.ip_mask.igmp_type = 1;

    cls_key.src_port = ca_portid;


    memcpy(cls_key.l2.mac_da.mac_min, general_query_mac, 6);
    memcpy(cls_key.l2.mac_da.mac_max, general_query_mac, 6);
    cls_key.handle_type             = CA_KEY_HANDLE_TYPE_GEM_INDEX;
    cls_key.key_handle.flow_id = 0x0;

    cls_key.ip.ip_valid = TRUE;
    cls_key.ip.ip_version = 4;
    cls_key.ip.ip_protocol = 0x02;
    cls_key.ip.igmp_type = 0x11;

    cls_action.options.masks.priority= 1;
    cls_action.options.masks.action_handle = 1;

    cls_action.forward = CA_CLASSIFIER_FORWARD_PORT;
    cls_action.dest.port = dst_port;
    cls_action.options.priority = 6;

    cls_action.options.action_handle.flow_id = CA_FLOW_ID_DS_IGMP;

    ret = ca_classifier_rule_add (   0,
                                     priority,
                                     &cls_key,
                                     &cls_key_mask ,
                                     &cls_action,
                                     &cls_index);

    policer.mode = CA_POLICER_MODE_TRTCM;
    policer.pps = 1;
    policer.cir = 32;
    policer.cbs = 10;
    policer.pbs = 10;

    policer.pir = 32;

    ret = ca_flow_policer_set ( 0, CA_FLOW_ID_DS_IGMP , &policer);

}

void oam_kt_adapt_init(void)
{
    __kt_event_register();
    __kt_vlan_init();
    __trap_general_query_rule_to_cpu0_init(); 
    __kt_default_rule_set(0);
    __kt_qos_init();
    __kt_deepq_init();
        
}

#endif