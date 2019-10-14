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
#include "ca_types.h"
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
#include "oam_port_stats.h"

#include "opm_api.h"
#include "oam_kt.h"
#include "oam_kt_client.h"
#include "oam_kt_pdu.h"

#include "oam_kt_adapt.h"

#ifdef HAVE_KT_LOG
#include "kt_log.h"
#endif
#include "l2.h"
#include "port.h"
#include "eth_port.h"

#include "rate.h"


#ifdef  CONFIG_ARCH_CORTINA_SATURN_SFU
#include "mc_api.h"
#endif



/*Global variable definitions*/
oam_uint8 g_opm_status = 2;
oam_uint8 g_highvolt_status = OAM_KT_OVER_VOLTAGE_RELEASE;
oam_uint8 g_lowvolt_status = 2;
oam_uint8 g_olt_sen_alarm = KT_SEN_EXCEED_ALARM_RELEASE;

extern oam_status kt_oam_onu_clear_statis_adapt ( );

strom_ctrl_port_threshold_t  g_storm_ctrl_config[5];
static ca_uint8 g_storm_control_status_per_onu = 0x1;
ca_uint8 get_per_onu_storm_control_status()
{
    return g_storm_control_status_per_onu;

}

void set_per_onu_storm_control_status(ca_uint8 status)
{
    g_storm_control_status_per_onu = status;

}

static ca_uint32 get_per_port_storm_ctrl_threshold(ca_uint8 port, strom_ctrl_port_threshold_t *pConfig)
{
    OAM_UNI_PORT_ID_CHECK(port);

    memcpy(pConfig, &g_storm_ctrl_config[port], sizeof(strom_ctrl_port_threshold_t));

    return CA_E_OK;
}

static ca_uint32 set_per_port_storm_ctrl_threshold(ca_uint8 port, strom_ctrl_port_threshold_t *pConfig)
{
    OAM_UNI_PORT_ID_CHECK(port);

    memcpy(&g_storm_ctrl_config[port], pConfig, sizeof(strom_ctrl_port_threshold_t));

    return CA_E_OK;
}

void init_per_port_storm_ctrl()
{
    ca_int32 pid;
    ca_uint32_t rate = 2000;
    ca_port_id_t ca_port;

    set_per_onu_storm_control_status(1); //enable:1, disable:0

    for (pid = 1; pid <= OAM_UNI_PORT_NUM; pid++) {
        g_storm_ctrl_config[pid].port = pid;
        g_storm_ctrl_config[pid].enable = 1; //enable: 1, disable: 0
        g_storm_ctrl_config[pid].bc_threshold = 972;    //kbps
        g_storm_ctrl_config[pid].mc_threshold = 182;    //kbps
        g_storm_ctrl_config[pid].uuc_threshold = 182;   //kbps
    }


    for (pid = 1; pid <= OAM_UNI_PORT_NUM; pid++){
        ca_port = OAM_PORT_ID_TO_CA_PORT(pid);
        rate = g_storm_ctrl_config[pid].bc_threshold;
        ca_l2_flooding_rate_set(0,
                                ca_port,
                                CA_L2_FLOODING_TYPE_BC,
                                0,
                                rate);
        rate = g_storm_ctrl_config[pid].mc_threshold;
        ca_l2_flooding_rate_set(0,
                                ca_port,
                                CA_L2_FLOODING_TYPE_MC,
                                0,
                                rate);

#ifdef  CONFIG_ARCH_CORTINA_SATURN_SFU
        mc_umc_policer_set ( pid , rate);
#endif

        rate = g_storm_ctrl_config[pid].uuc_threshold;
        ca_l2_flooding_rate_set(0,
                                ca_port,
                                CA_L2_FLOODING_TYPE_UUC,
                                0,
                                rate);
    }
#if 0
    int i = 0;
    ca_status ret = 0;
    ca_callback_context_t  context;
    ca_sdl_policy_t val;

    set_per_onu_storm_control_status(1); //enable:1, disable:0

    for (i = 1; i <= OAM_UNI_PORT_NUM; i ++) {
        g_storm_ctrl_config[i].port = i;
        g_storm_ctrl_config[i].enable = 1; //enable: 1,   disable: 0
        g_storm_ctrl_config[i].bc_threshold = 972;  //kbps
        g_storm_ctrl_config[i].mc_threshold = 182;  //kbps
        g_storm_ctrl_config[i].uuc_threshold = 182; //kbps
    }

    memset(&val, 0 , sizeof(ca_sdl_policy_t));
    //set PON and UNI port defaut storm control value
    for (i = 1 ; i <= OAM_UNI_PORT_NUM ; i++) { //remove PON port storm control
        val.enable = 1;
        val.rate = 972; //972;
        ret = epon_request_onu_port_storm_control_set2(context, 0, 0x40000000, i , SDL_RL_MOD_BPS, 0 , &val);

        val.rate = 182; //182;
        ret = epon_request_onu_port_storm_control_set2(context, 0, 0x40000000, i , SDL_RL_MOD_BPS, 1 , &val);

        val.rate = 182;
        ret = epon_request_onu_port_storm_control_set2(context, 0, 0x40000000, i , SDL_RL_MOD_BPS, 2 , &val);
    }
#endif
}

static oam_uint32
kt_prv_oam_onu_block_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_block_container_struct *p_set = (oam_kt_onu_block_container_struct *)pRecv;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    OAM_ORG_LOG("\r\nset action: %d\r\n", p_set->action);

    if (OAM_E_OK != kt_prv_oam_onu_block_set_adapt(p_set->action)) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
        p_rsp->len = OAM_VAR_SET_OK;
    }

    return 0;
}

static oam_uint32
kt_prv_oam_onu_block_get(oam_uint8 *pOut )
{
    oam_kt_onu_block_container_struct *pOnuBlockContainer =
        (oam_kt_onu_block_container_struct *)pOut;
    oam_uint8 action = 0;

    pOnuBlockContainer->hdr.len = 1;

    if (OAM_E_OK != kt_prv_oam_onu_block_get_adapt(&action)) {
        pOnuBlockContainer->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }
    else {
        pOnuBlockContainer->action = action;
    }

    OAM_ORG_LOG("\r\nget action: %d\r\n", pOnuBlockContainer->action);

    return sizeof(oam_kt_var_cont_t)+pOnuBlockContainer->hdr.len;
}

static oam_uint32
kt_prv_oam_eth_mac_limit_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_eth_mac_limit_container_struct *p_set = (oam_kt_onu_eth_mac_limit_container_struct *)pRecv;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    OAM_ORG_LOG("\r\nset oper: %d, num: %d\r\n", p_set->oper, p_set->number);

    if (OAM_E_OK != kt_prv_oam_eth_mac_limit_set_adapt(p_set->oper, p_set->number)) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
        p_rsp->len = OAM_VAR_SET_OK;
    }

    return OAM_E_OK;
}


static oam_uint32
kt_prv_oam_eth_mac_limit_get(oam_uint8 *pOut )
{
    oam_kt_onu_eth_mac_limit_container_struct *pEthMacLimitContainer =
        (oam_kt_onu_eth_mac_limit_container_struct *)pOut;
    unsigned char opr = 0, number = 0;

    pEthMacLimitContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pEthMacLimitContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_ETH_MAC_LIMIT);
    if (OAM_E_OK != kt_prv_oam_eth_mac_limit_get_adapt(&opr, &number)) {
        pEthMacLimitContainer->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }
    else {
        pEthMacLimitContainer->oper = opr;
        if(pEthMacLimitContainer->oper == KT_MAC_LIMIT_OPER_DISABLE)
                pEthMacLimitContainer->hdr.len = 1;
        else {
                pEthMacLimitContainer->number = number;
                pEthMacLimitContainer->hdr.len = 2;
        }
    }

    OAM_ORG_LOG("\r\nget oper: %d, num: %d\r\n", pEthMacLimitContainer->oper, pEthMacLimitContainer->number);

    return sizeof(oam_kt_var_cont_t)+pEthMacLimitContainer->hdr.len;
}

static oam_uint32
kt_prv_oam_onu_queue_drop_counter_get(oam_uint8 *pOut )
{
    oam_kt_onu_queue_drop_counter_container_struct *pOnuQueueDropCounterContainer =
        (oam_kt_onu_queue_drop_counter_container_struct *)pOut;
    oam_kt_onu_queue_drop_counter_container_struct counter;

    pOnuQueueDropCounterContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pOnuQueueDropCounterContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_QUEUE_DROP_COUNTER);
    pOnuQueueDropCounterContainer->hdr.len = sizeof(oam_kt_onu_queue_drop_counter_container_struct) -
                                             sizeof(oam_kt_var_cont_t);

    if (OAM_E_OK != kt_prv_oam_onu_queue_drop_counter_get_adapt(&counter)) {
        pOnuQueueDropCounterContainer->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }
    memcpy(&pOnuQueueDropCounterContainer->upstream_queue_count, &counter.upstream_queue_count,
        sizeof(oam_kt_onu_queue_drop_counter_container_struct) - sizeof(oam_kt_var_cont_t));

    return sizeof(oam_kt_onu_queue_drop_counter_container_struct);
}

static oam_uint32
kt_prv_oam_onu_mpcp_counter_get(oam_uint8 *pOut )
{
    oam_kt_onu_mpcp_counter_container_struct *pOnuMpcpCounterContainer =
        (oam_kt_onu_mpcp_counter_container_struct *)pOut;
    oam_kt_onu_mpcp_counter_container_struct counter;

    pOnuMpcpCounterContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pOnuMpcpCounterContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_MPCP_COUNTER);
    /* 10 int64 are reserved */
    pOnuMpcpCounterContainer->hdr.len = sizeof(oam_kt_onu_mpcp_counter_container_struct) -
                                        sizeof(oam_kt_var_cont_t);

    if (OAM_E_OK != kt_prv_oam_onu_mpcp_counter_get_adapt(&counter)) {
        pOnuMpcpCounterContainer->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }

    counter.mask = htons(counter.mask);
    counter.rx_hec_errors = htonll(counter.rx_hec_errors);
    counter.tx_register_request = htonll(counter.tx_register_request);
    counter.rx_register = htonll(counter.rx_register);
    counter.tx_register_ack = htonll(counter.tx_register_ack);
    counter.rx_gate_frames = htonll(counter.rx_gate_frames);
    counter.tx_report_frames = htonll(counter.tx_report_frames);

    memcpy(&pOnuMpcpCounterContainer->mask, &counter.mask, sizeof(
oam_kt_onu_mpcp_counter_container_struct) - sizeof(oam_kt_var_cont_t));

    return sizeof(oam_kt_onu_mpcp_counter_container_struct);
}

static oam_uint32
kt_prv_oam_diagnostics_get(oam_uint8 *pOut )
{
    oam_kt_onu_diagnostioam_container_struct *pDiagnosticsContainer =
        (oam_kt_onu_diagnostioam_container_struct *)pOut;
    oam_uint16 temp = 0;
    unsigned short tx_power = 0, rx_power = 0;

    pDiagnosticsContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pDiagnosticsContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_DIAGNOSTIC);
    if (OAM_E_OK != kt_prv_oam_diagnostics_get_adapt(&temp, &tx_power, &rx_power)) {
        pDiagnosticsContainer->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }
    else {
        pDiagnosticsContainer->hdr.len = 6;
        pDiagnosticsContainer->temperature = htons(temp);
        pDiagnosticsContainer->tx_power = htons(tx_power);
        pDiagnosticsContainer->rx_power = htons(rx_power);
    }

    OAM_ORG_LOG("\r\ntemp: %d, tx_power: %d, rx_power: %d\r\n", pDiagnosticsContainer->temperature, pDiagnosticsContainer->tx_power,
    pDiagnosticsContainer->rx_power);
    return sizeof(oam_kt_var_cont_t)+pDiagnosticsContainer->hdr.len;
}

static oam_uint32
kt_prv_oam_loop_detect_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_loop_detect_container_struct *p_set = (oam_kt_onu_loop_detect_container_struct *)pRecv;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    OAM_ORG_LOG("\r\nset opr: %d, interval: %d, block_time: %d\r\n", p_set->opr, p_set->interval, p_set->block_time);
    if (OAM_E_OK != kt_prv_oam_loop_detect_set_adapt(p_set->opr, p_set->interval,
    p_set->block_time)) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
        p_rsp->len = OAM_VAR_SET_OK;
    }
    return OAM_E_OK;
}

static oam_uint32
kt_prv_oam_loop_detect_get(oam_uint8 *pOut )
{
    oam_kt_onu_loop_detect_container_struct *pLoopDetectContainer =
        (oam_kt_onu_loop_detect_container_struct *)pOut;
    unsigned char opr = 0, interval = 0, block_time = 0;

    pLoopDetectContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pLoopDetectContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_LOOP_DETECT);
    if (OAM_E_OK != kt_prv_oam_loop_detect_get_adapt(&opr, &interval, &block_time)) {
        pLoopDetectContainer->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }
    else {
        pLoopDetectContainer->opr = opr;
        if(pLoopDetectContainer->opr == KT_LOOP_DECTECT_DEACTIVATED)
                pLoopDetectContainer->hdr.len = 1;
        else {
                pLoopDetectContainer->interval = interval;
                pLoopDetectContainer->block_time = block_time;
                pLoopDetectContainer->hdr.len = 3;
        }
    }

    OAM_ORG_LOG("\r\nget opr: %d, interval: %d, block_time: %d\r\n", pLoopDetectContainer->opr, pLoopDetectContainer->interval, pLoopDetectContainer->block_time);
    return sizeof(oam_kt_var_cont_t)+pLoopDetectContainer->hdr.len;
}

static oam_uint32
kt_prv_oam_dhcp_option_cfg_get(oam_uint8 *pOut )
{
    oam_kt_onu_dhcp_option_container_struct *pDhcpOptionContainer =
        (oam_kt_onu_dhcp_option_container_struct *)pOut;
    oam_uint8 cfg = 0;

    pDhcpOptionContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pDhcpOptionContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_DHCP_OPTION);
    if (OAM_E_OK != kt_prv_oam_dhcp_option_cfg_get_adapt(&cfg)) {
        pDhcpOptionContainer->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }
    else {
        pDhcpOptionContainer->cfg = cfg;
        pDhcpOptionContainer->hdr.len = 1;
    }

    OAM_ORG_LOG("\r\nget cfg: %d\r\n",pDhcpOptionContainer->cfg);
    return sizeof(oam_kt_var_cont_t)+pDhcpOptionContainer->hdr.len;
}

static oam_uint32
kt_prv_oam_dhcp_option_cfg_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_dhcp_option_container_struct *p_set = (oam_kt_onu_dhcp_option_container_struct *)pRecv;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    OAM_ORG_LOG("\r\nset cfg: %d\r\n", p_set->cfg);
    p_rsp->extBranch = OAM_KT_ATTRIB_BRANCH;
    p_rsp->leaf = htons(OAM_KT_LEAF_CODE_ONU_DHCP_OPTION);
    if(p_set->cfg != KT_DHCP_OPTION_DEACTIVATED && p_set->cfg != KT_DHCP_OPTION_ACTIVATED)
    {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
        return OAM_E_OK;
    }

    if (OAM_E_OK != kt_prv_oam_dhcp_option_cfg_set_adapt(p_set->cfg )) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
        p_rsp->len = OAM_VAR_SET_OK;
    }
    return OAM_E_OK;
}

static oam_uint32
kt_prv_oam_counter_clear(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    if (OAM_E_OK != kt_oam_onu_clear_statis_adapt()) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
        p_rsp->len = OAM_VAR_SET_OK;
    }
    return OAM_E_OK;
}

static oam_uint32
kt_prv_oam_ipv6_opt254_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_ipv6_opt254_t *p_set =
        (oam_kt_onu_ipv6_opt254_t *)pRecv;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    if (OAM_E_OK != kt_oam_onu_ipv6_opt254_set_adapt(p_set->enable)) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
        p_rsp->len = OAM_VAR_SET_OK;
    }
    return OAM_E_OK;
}

static oam_uint32
kt_prv_oam_ipv6_opt254_get(oam_uint8 *pOut )
{
    oam_kt_onu_ipv6_opt254_t *pIpv6Opt254 =
        (oam_kt_onu_ipv6_opt254_t *)pOut;
    oam_uint8 enable;

    pIpv6Opt254->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pIpv6Opt254->hdr.leaf = htons(OAM_KT_LEAF_CODE_IPV6_OPT254_ENABLE);
    pIpv6Opt254->hdr.len = sizeof(unsigned char);
    if (OAM_E_OK != kt_oam_onu_ipv6_opt254_get_adapt(&enable)) {
        pIpv6Opt254->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }
    pIpv6Opt254->enable = enable;

    OAM_ORG_LOG("\r\nget enable: %d\r\n", pIpv6Opt254->enable);
    return sizeof(oam_kt_onu_ipv6_opt254_t);
}

static ca_uint32 kt_prv_oam_per_port_fdb_entries_get(ca_uint8 *pOut)
{
    oam_kt_onu_per_port_mac_entries_t *pRepEntry = (oam_kt_onu_per_port_mac_entries_t *)pOut;
    oam_kt_onu_per_port_mac_entries_t  fdbEntries[128] ;
    ca_uint8 fdb_nums = 0;
    ca_uint32 len = 0;

    pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_PER_PORT_MAC_ENTRIES_GET);
    pRepEntry->hdr.len = 7;

    memset(fdbEntries, 0 , sizeof(fdbEntries));
    if ((pRepEntry->uni_port >= OAM_UNI_PORT_ID1 && pRepEntry->uni_port <= OAM_UNI_PORT_NUM) || OAM_ALL_UNI_PORT_ID == pRepEntry->uni_port) { //single port: 1 ~ 4 or all 4ports
        kt_prv_oam_onu_per_port_mac_entries_get_adapt(pRepEntry->uni_port, fdbEntries, &fdb_nums);
    } else { //invlaid port parameter, return errorr to OLT
        pRepEntry->hdr.extBranch = 0xA7;
        pRepEntry->hdr.leaf =  htons(0x0022);
        pRepEntry->hdr.len = 0x86;
        return sizeof(oam_kt_var_cont_t);
    }

    if (fdb_nums > 0) {
        memcpy(pOut, (ca_uint8 *)fdbEntries, sizeof(oam_kt_onu_per_port_mac_entries_t)*fdb_nums);
        oam_kt_onu_per_port_mac_entries_t *ptr = (oam_kt_onu_per_port_mac_entries_t *)(pOut + sizeof(oam_kt_onu_per_port_mac_entries_t) * fdb_nums);
        ptr->hdr.extBranch = 0xA7;
        ptr->hdr.leaf = htons(0x0022);
        ptr->hdr.len = 0x80;
        len = sizeof(oam_kt_onu_per_port_mac_entries_t) * fdb_nums + 4;
    } else { //dynamic fdb entry is  0
        pRepEntry->hdr.extBranch = 0xA7;
        pRepEntry->hdr.leaf =  htons(0x0022);
        pRepEntry->hdr.len = 0x80;
        len = sizeof(oam_kt_var_cont_t); ;
    }

    return len;

}

ca_uint32 kt_uni_port_stats_get(oam_uint16 port , ca_uint8 *pStats)
{
    oam_kt_onu_per_port_statistics_entries_t *p_rsp_tlv = (oam_kt_onu_per_port_statistics_entries_t *)pStats;
    ca_status_t ret = CA_E_OK;
    oam_uni_stats64_t uni_stats;

    oam_onu_uni_stats_get(port, &uni_stats);

    if (CA_E_OK == ret) {
        p_rsp_tlv->txFrames = uni_stats.txfrm_cnt;
        p_rsp_tlv->txFrames = htonll(p_rsp_tlv->txFrames);

        p_rsp_tlv->txBytes = htonll(uni_stats.txbyte_cnt);

        p_rsp_tlv->txUnicastFrames = uni_stats.txucfrm_cnt;
        p_rsp_tlv->txUnicastFrames = htonll(p_rsp_tlv->txUnicastFrames);

        p_rsp_tlv->txMCFrames = uni_stats.txmcfrm_cnt;
        p_rsp_tlv->txMCFrames = htonll(p_rsp_tlv->txMCFrames);

        p_rsp_tlv->txBCFrames = uni_stats.txbcfrm_cnt;
        p_rsp_tlv->txBCFrames = htonll(p_rsp_tlv->txBCFrames);

        //p_rsp_tlv->txDropFrames = mib.txcrcerrfrm_cnt - mib.rxcrcerrfrm_cnt ;
        /*
        p_rsp_tlv->txDropFrames = mib.txcrcerrfrm_cnt;
        p_rsp_tlv->txDropFrames = htonll(p_rsp_tlv->txDropFrames);
        */

        p_rsp_tlv->rxFrames =  uni_stats.rxfrm_cnt;
        p_rsp_tlv->rxFrames = htonll(p_rsp_tlv->rxFrames);

        p_rsp_tlv->rxBytes = htonll(uni_stats.rxbyte_cnt);
        p_rsp_tlv->rxUnicastFrames = uni_stats.rxucfrm_cnt;
        p_rsp_tlv->rxUnicastFrames = htonll(p_rsp_tlv->rxUnicastFrames);

        p_rsp_tlv->rxMCFrames = uni_stats.rxmcfrm_cnt;
        p_rsp_tlv->rxMCFrames = htonll(p_rsp_tlv->rxMCFrames);

        p_rsp_tlv->rxBCFrames = uni_stats.rxbcfrm_cnt;
        p_rsp_tlv->rxBCFrames = htonll(p_rsp_tlv->rxBCFrames);

        p_rsp_tlv->rxOverSizeFrames = uni_stats.rxoversizefrm_cnt;
        p_rsp_tlv->rxOverSizeFrames = htonll(p_rsp_tlv->rxOverSizeFrames);

        p_rsp_tlv->rxUnderSizeFrames = uni_stats.rxundersizefrm_cnt;
        p_rsp_tlv->rxUnderSizeFrames = htonll(p_rsp_tlv->rxUnderSizeFrames);
        /*
        p_rsp_tlv->rxCRCFrames = mib.rxcrcerrfrm_cnt;
        p_rsp_tlv->rxCRCFrames = htonll(p_rsp_tlv->rxCRCFrames);

        p_rsp_tlv->inpause = mib.rxpausefrm_cnt;
        p_rsp_tlv->inpause = htonll(p_rsp_tlv->inpause);
        */
        p_rsp_tlv->mask = htons(0x7FFF);
        /*
        ca_printf("uni port= 0x%x txTybes = %lld    rxBytes =%lld\n", port, mib.txbyte_cnt,mib.rxbyte_cnt);
        ca_printf("uni port= 0x%x txfrm = %d  rxfrm = %d\n", port, mib.txfrm_cnt,mib.rxfrm_cnt);
        */
    } else {
        //ca_printf("err ret = 0x%x\n", ret);

    }

    return ret;

}

ca_uint32 kt_pon_port_stats_get(ca_uint8 *pStats)
{
    oam_kt_onu_per_port_statistics_entries_t *p_rsp_tlv = (oam_kt_onu_per_port_statistics_entries_t *)pStats;
    ca_status_t ret = CA_E_OK;
    oam_pon_stats64_t pon_stats;
    epon_if_stats_t if_stats;

    oam_onu_pon_stats_get(&pon_stats);
    oam_onu_std_pon_stats_get(&if_stats);

    if (CA_E_OK == ret) {
        p_rsp_tlv->txFrames = pon_stats.pon_txframe_cnt;
        p_rsp_tlv->txFrames = htonll(p_rsp_tlv->txFrames);

        p_rsp_tlv->txBytes = htonll(pon_stats.pon_tx_byte_cnt);

        p_rsp_tlv->txUnicastFrames = if_stats.out_unicast;
        p_rsp_tlv->txUnicastFrames = htonll(p_rsp_tlv->txUnicastFrames);

        p_rsp_tlv->txMCFrames = if_stats.out_multicast;
        p_rsp_tlv->txMCFrames = htonll(p_rsp_tlv->txMCFrames);

        p_rsp_tlv->txBCFrames =  if_stats.out_broadcast;
        p_rsp_tlv->txBCFrames = htonll(p_rsp_tlv->txBCFrames);

        p_rsp_tlv->txDropFrames = if_stats.out_discards;
        p_rsp_tlv->txDropFrames = htonll(p_rsp_tlv->txDropFrames);

        p_rsp_tlv->rxFrames =  if_stats.in_unicast + if_stats.in_multicast + if_stats.in_broadcast;
        p_rsp_tlv->rxFrames = htonll(p_rsp_tlv->rxFrames);

        p_rsp_tlv->rxBytes = htonll(if_stats.out_octets);

        p_rsp_tlv->rxUnicastFrames = if_stats.in_unicast;
        p_rsp_tlv->rxUnicastFrames = htonll(p_rsp_tlv->rxUnicastFrames);

        p_rsp_tlv->rxMCFrames = if_stats.in_multicast;
        p_rsp_tlv->rxMCFrames = htonll(p_rsp_tlv->rxMCFrames);

        p_rsp_tlv->rxBCFrames = (if_stats.in_broadcast);
        p_rsp_tlv->rxBCFrames = htonll(p_rsp_tlv->rxBCFrames);

        p_rsp_tlv->rxOverSizeFrames = (pon_stats.pon_oversize_cnt);
        p_rsp_tlv->rxOverSizeFrames = htonll(p_rsp_tlv->rxOverSizeFrames);

        p_rsp_tlv->rxUnderSizeFrames = (pon_stats.pon_undersizecnt);
        p_rsp_tlv->rxUnderSizeFrames = htonll(p_rsp_tlv->rxUnderSizeFrames);

        //p_rsp_tlv->rxCRCFrames = (pon_stats.pon_crcerr_cnt);
        //p_rsp_tlv->rxCRCFrames = htonll(p_rsp_tlv->rxCRCFrames);

        /* just for KT BMT test: pon_rx_fcs_err is hooked FEC rx_err_pkt */
        p_rsp_tlv->rxCRCFrames = (pon_stats.pon_rx_fcs_err);
        p_rsp_tlv->rxCRCFrames = htonll(p_rsp_tlv->rxCRCFrames);

        p_rsp_tlv->inpause = (pon_stats.pon_pauseframe_cnt);
        p_rsp_tlv->inpause = htonll(p_rsp_tlv->inpause);

        p_rsp_tlv->mask = htons(0x7FFF);
        /*
        ca_printf("PON port:  txBytes = %lld  rxBytes =%lld\n", stats.pon_tx_byte_cnt,stats.pon_byte_cnt);
        ca_printf("PON port:  txFrm = %lld  rxFrm =%lld\n", stats.pon_txframe_cnt + stats.pon_txmcframe_cnt + stats.pon_txbcframe_cnt,stats.pon_frame_cnt + stats.pon_mcframe_cnt + stats.pon_bcframe_cnt);
        */
    } else {
        //ca_printf("err ret = 0x%x\n", ret);

    }

    return ret;

}

static ca_uint32  kt_prv_oam_per_port_statistics_get(ca_uint8 *pOut)
{
    oam_kt_onu_per_port_statistics_entries_t *pRepEntry = (oam_kt_onu_per_port_statistics_entries_t *)pOut;
    ca_uint32 ret = CA_E_OK;

    pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_PER_PORT_STATISTICS_GET);
    pRepEntry->hdr.len = 0x7B;

    //ca_printf("port=0x%x \n",pRepEntry->port_id);

    if (pRepEntry->port_id <= OAM_UNI_PORT_NUM) { //0: pon, 1 ~4: UNI
        //memset(((ca_uint8*)pRepEntry) + 5 , 0 , sizeof(oam_kt_onu_per_port_statistics_entries_t) -  5);
        if (0x0 == pRepEntry->port_id){
            ret = kt_pon_port_stats_get((ca_uint8 *)pRepEntry);
        }else
            ret = kt_uni_port_stats_get(pRepEntry->port_id, (ca_uint8 *)pRepEntry);

        if (CA_E_OK == ret)
            return sizeof(oam_kt_onu_per_port_statistics_entries_t);
        else {
            pRepEntry->hdr.extBranch = 0xA7;
            pRepEntry->hdr.leaf =  htons(0x001D);
            pRepEntry->hdr.len = 0x86;
            return sizeof(oam_kt_var_cont_t);
        }
    } else { //invlaid port parameter, return errorr to OLT
        pRepEntry->hdr.extBranch = 0xA7;
        pRepEntry->hdr.leaf =  htons(0x001D);
        pRepEntry->hdr.len = 0x86;
        return sizeof(oam_kt_var_cont_t);
    }

    return ret;
}


static ca_uint32  kt_prv_oam_per_port_statistics_clr(ca_uint8 *pRecv, ca_uint8 *pOut)
{
#if 1
    oam_kt_onu_uni_port_stats_clr_t *pRepEntry = (oam_kt_onu_uni_port_stats_clr_t *)pRecv;
    oam_kt_var_cont_t *p_rsp = (oam_kt_var_cont_t *)pOut;
    //ca_callback_context_t  context;
    ca_uint8 port_id = 0;
    ca_uint32 ret = CA_E_OK;

    p_rsp->extBranch = OAM_KT_ATTRIB_BRANCH;
    p_rsp->leaf =  htons(0x001E);
    p_rsp->len = 0x80;

    //ca_printf("port=0x%x \n",pRepEntry->port_id);

    if (pRepEntry->port_id <= OAM_UNI_PORT_NUM) { //0: pon, 1 ~4: UNI
        if (0x0 == pRepEntry->port_id)
            ret = oam_pon_stats_reset();
        else {
            ret = oam_uni_stats_reset(port_id);
        }

        if (CA_E_OK == ret)
            return CA_E_OK;
        else {
            p_rsp->extBranch = 0xA7;
            p_rsp->leaf =  htons(0x001E);
            p_rsp->len = 0x86;
        }
    } else if (0xff == pRepEntry->port_id) { //clear all ports
        ret = oam_port_stats_reset();

        if (CA_E_OK == ret)
            return CA_E_OK;
        else {
            p_rsp->extBranch = 0xA7;
            p_rsp->leaf =  htons(0x001E);
            p_rsp->len = 0x86;
        }
    } else { //invlaid port parameter, return errorr to OLT
        p_rsp->extBranch = 0xA7;
        p_rsp->leaf =  htons(0x001E);
        p_rsp->len = 0x86;
    }
#endif
    return CA_E_OK;

}

#if _RTCT_DEBUG_
extern unsigned int rtk_port_cableLength_set(unsigned int port);
extern unsigned int rtk_port_cableLength_get(unsigned int port, ca_uint32 *length);
#endif
static ca_uint32  kt_prv_oam_per_port_rtct_get(ca_uint8 *pOut)
{
    oam_kt_onu_uni_port_rtct_t *pRepEntry = (oam_kt_onu_uni_port_rtct_t *)pOut;

#if _RTCT_DEBUG_
    ca_uint32 cable_length = 0;
    ca_uint32 uni_port = 0;
    if ((pRepEntry->port_id >= 1 && pRepEntry->port_id <= OAM_UNI_PORT_NUM) || (0xFF == pRepEntry->port_id)) { // 1 ~4: UNI, 0xff: 4ports
        if (pRepEntry->port_id != 0xFF) { // single port
            pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
            pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_PER_PORT_RTCT_GET);
            pRepEntry->hdr.len = 0x03;
            rtk_port_cableLength_set(pRepEntry->port_id);
            rtk_port_cableLength_get(pRepEntry->port_id, &cable_length);
            pRepEntry->cable_length = htons(cable_length);
            return sizeof(oam_kt_onu_uni_port_rtct_t);
        } else { // all the 4 ports
            for (uni_port = 1; uni_port <= 4; uni_port ++) {
                pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
                pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_PER_PORT_RTCT_GET);
                pRepEntry->hdr.len = 0x03;
                rtk_port_cableLength_set(uni_port);
                cable_length = 0 ;
                rtk_port_cableLength_get(uni_port, &cable_length);
                pRepEntry->port_id = uni_port;
                pRepEntry->cable_length = htons(cable_length);
                pRepEntry++;
            }
            pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
            pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_PER_PORT_RTCT_GET);
            pRepEntry->hdr.len = 0x80;
            return sizeof(oam_kt_onu_uni_port_rtct_t)*4 + 4;
        }

    } else { //invlaid port parameter, return errorr to OLT
        pRepEntry->hdr.extBranch = 0xA7;
        pRepEntry->hdr.leaf =  htons(0x001D);
        pRepEntry->hdr.len = 0x86;
        return sizeof(oam_kt_var_cont_t);
    }
#else
    pRepEntry->hdr.extBranch = 0xA7;
    pRepEntry->hdr.leaf =  htons(0x001D);
    pRepEntry->hdr.len = 0x86;
    return sizeof(oam_kt_var_cont_t);
#endif

}

static ca_uint32  kt_prv_oam_per_port_storm_control_get(ca_uint8 *pOut)
{
    oam_kt_onu_per_port_storm_control_get_t  *pRepEntry = (oam_kt_onu_per_port_storm_control_get_t *)pOut;
    ca_uint32 len = 0;
    strom_ctrl_port_threshold_t  thres_hold;
    ca_uint8  uni_port = 0;


    //cs_printf("input, port: %d  \n", pRepEntry->port);
    if ((pRepEntry->port < 1)  || (pRepEntry->port > 4 && pRepEntry->port != 0xFF)) {
        pRepEntry->hdr.len = 0x86;
        return sizeof(oam_kt_onu_per_port_storm_control_get_t);
    }

    if (0xff == pRepEntry->port) {
#if 0 // remove downstream PON port storm control
        //get PON port storm control configuration
        pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
        pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_PER_PORT_STORM_CONTROL);
        pRepEntry->hdr.len = 0x7;

        memset(&thres_hold, 0, sizeof(strom_ctrl_port_threshold_t));
        get_per_port_storm_ctrl_threshold(0, &thres_hold);
        pRepEntry->port = 0;
        pRepEntry->bc_threshold = htons(thres_hold.bc_threshold);
        pRepEntry->mc_threshold = htons(thres_hold.mc_threshold);
        pRepEntry->uuc_threshold = htons(thres_hold.uuc_threshold);
        len += sizeof(oam_kt_onu_per_port_storm_control_get_t);

        //cs_printf("cpu port,  bc=%d, mc=%d, uuc=%d, len: %d \n", thres_hold.bc_threshold,thres_hold.mc_threshold,thres_hold.uuc_threshold, len);

        //get UNI port 1 ~ 4 storm control configuration
        pRepEntry++;
#endif

        for (uni_port = 1; uni_port <= 4; uni_port++) {
            pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
            pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_PER_PORT_STORM_CONTROL);
            pRepEntry->hdr.len = 0x7;

            //to call API to get reated UNI storm control configuration data
            memset(&thres_hold, 0, sizeof(strom_ctrl_port_threshold_t));
            get_per_port_storm_ctrl_threshold(uni_port, &thres_hold);
            pRepEntry->port = uni_port;
            pRepEntry->bc_threshold = htons(thres_hold.bc_threshold);
            pRepEntry->mc_threshold = htons(thres_hold.mc_threshold);
            pRepEntry->uuc_threshold = htons(thres_hold.uuc_threshold);
            pRepEntry++;
            len += sizeof(oam_kt_onu_per_port_storm_control_get_t);
            //cs_printf("uni port %d, bc=%d, mc=%d, uuc=%d, len: %d \n", uni_port, thres_hold.bc_threshold,thres_hold.mc_threshold,thres_hold.uuc_threshold, len);
        }

        /* remove it according Andrew's requirement
        pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
        pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_PER_PORT_STORM_CONTROL);
        pRepEntry->hdr.len = 0x80;
        len += sizeof(oam_kt_var_cont_t);
        */

        //cs_printf("total len: %d \n", len);
    } else {
        pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
        pRepEntry->hdr.leaf =   htons(OAM_KT_LEAF_CODE_PER_PORT_STORM_CONTROL);
        pRepEntry->hdr.len = 0x7;
        //get storm control configuration data
        memset(&thres_hold, 0, sizeof(strom_ctrl_port_threshold_t));
        get_per_port_storm_ctrl_threshold(pRepEntry->port, &thres_hold);
        pRepEntry->bc_threshold = htons(thres_hold.bc_threshold);
        pRepEntry->mc_threshold = htons(thres_hold.mc_threshold);
        pRepEntry->uuc_threshold = htons(thres_hold.uuc_threshold);
        len = sizeof(oam_kt_onu_per_port_storm_control_get_t);
    }

    return len;
}


static ca_uint32
kt_prv_oam_per_port_storm_control_set(ca_uint8 *pRecv, ca_uint8 *pOut)
{
#if 1
    oam_kt_onu_per_port_storm_control_set_t *pRepEntry = (oam_kt_onu_per_port_storm_control_set_t *)pRecv;
    oam_kt_var_cont_t *p_rsp = (oam_kt_var_cont_t *)pOut;
    //ca_callback_context_t  context;
    //ca_sdl_policy_t   rate;
    ca_uint32_t rate;
    strom_ctrl_port_threshold_t thres_hold;
    ca_status_t  ret ;
    ca_uint32  port_id = pRepEntry->port;
    ca_uint8    pkt_type = pRepEntry->pkt_type;
    ca_uint16   threshold = ntohs(pRepEntry->threshold);
    ca_l2_flooding_type_t flood_type;
    ca_port_id_t ca_port;

    /* KT change OAM format, only support port == 0xff */
    if (port_id != 0xFF || pkt_type > 2) {
        p_rsp->len = 0x86;
        return 0;
    }

    /* if global status is disable, just return failed */
    if (!get_per_onu_storm_control_status()) {
        OAM_ORG_LOG("g_storm_control_status_per_onu %d\r\n", g_storm_control_status_per_onu);
        p_rsp->len = 0x86;
        return 0;
    }

    if (threshold < 8)
        threshold = 8;

    /* 0: PON prot, 1 ~4: UNI port */
    for (port_id = 1; port_id <= OAM_UNI_PORT_NUM; port_id++) {
        memset(&thres_hold, 0, sizeof(strom_ctrl_port_threshold_t));
        get_per_port_storm_ctrl_threshold(port_id, &thres_hold);

        switch (pkt_type) {
        case 0:
            flood_type = CA_L2_FLOODING_TYPE_BC;
            if (thres_hold.bc_threshold == threshold) {
                p_rsp->len = 0x80;
                continue;
            } else
                thres_hold.bc_threshold = threshold;
            break;
        case 1:
            flood_type = CA_L2_FLOODING_TYPE_MC;
            if (thres_hold.mc_threshold == threshold) {
                p_rsp->len = 0x80;
                continue;
            } else{
                thres_hold.mc_threshold = threshold;
            }


#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
            mc_umc_policer_set ( port_id,  threshold);
#endif

            break;
        case 2:
            flood_type = CA_L2_FLOODING_TYPE_UUC;
            if (thres_hold.uuc_threshold == threshold) {
                p_rsp->len = 0x80;
                continue;
            } else
                thres_hold.uuc_threshold = threshold;

            break;
        default:
            p_rsp->len = 0x86;
            return 0;
        }

        /*
        rate.rate = threshold;
        rate.enable = 1;
        */
        rate = threshold;
        ca_port = OAM_PORT_ID_TO_CA_PORT(port_id);

        //ret = epon_request_onu_port_storm_control_set2(context, 0, 0x40000000, port_id, SDL_RL_MOD_BPS, pkt_type, &rate);
        ret = ca_l2_flooding_rate_set(0, ca_port, flood_type, 0, rate);
        if (ret) {
            p_rsp->len = 0x86;
            OAM_ORG_LOG("configure storm control failed\r\n");
        } else {
            p_rsp->len = 0x80;
            thres_hold.port = port_id;
            thres_hold.enable = 1;
            set_per_port_storm_ctrl_threshold(port_id, &thres_hold);
        }

    }
#endif
    return CA_E_OK;
}

static ca_uint32  kt_prv_oam_storm_control_status_get(ca_uint8 *pOut)
{
    oam_kt_onu_storm_control_status_set_t  *pRepEntry = (oam_kt_onu_storm_control_status_set_t *)pOut;

    //cs_printf("input, port: %d  \n", pRepEntry->port);
    pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pRepEntry->hdr.leaf =   htons(0x0025);
    pRepEntry->hdr.len = 0x1;
    pRepEntry->status = get_per_onu_storm_control_status();

    return sizeof(oam_kt_onu_storm_control_status_set_t);
}


static ca_uint32 kt_prv_oam_storm_control_status_set(ca_uint8 *pRecv, ca_uint8 *pOut)
{
#if 1
    oam_kt_onu_storm_control_status_set_t *pRepEntry = (oam_kt_onu_storm_control_status_set_t *)pRecv;
    oam_kt_var_cont_t *p_rsp = (oam_kt_var_cont_t *)pOut;
    ca_uint32_t rate;
    ca_uint32 port_id;
    ca_port_id_t ca_port;

    //to check input parameter invalid or not
    if (pRepEntry->status > 1) {
        p_rsp->len = 0x86;
        return CA_E_OK;
    }

    if (pRepEntry->status == get_per_onu_storm_control_status()) {
        p_rsp->len = 0x80;
        return CA_E_OK;
    }

    //disable to enable
    if (pRepEntry->status) {
        init_per_port_storm_ctrl();
    } else { // enable to disable
        set_per_onu_storm_control_status(0); //enable:1, disable:0
        rate = CA_UINT32_INVALID;
        for (port_id = 1; port_id <= OAM_UNI_PORT_NUM; port_id ++) {
            ca_port = OAM_PORT_ID_TO_CA_PORT(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU            
            mc_umc_policer_set( port_id, rate);
#endif  

            ca_l2_flooding_rate_set(0,
                            ca_port,
                            CA_L2_FLOODING_TYPE_BC,
                            0,
                            rate);
            ca_l2_flooding_rate_set(0,
                            ca_port,
                            CA_L2_FLOODING_TYPE_UUC,
                            0,
                            rate);

            /*val.enable = 1;
            val.rate = 1000000; //unit kbps
            ret = epon_request_onu_port_storm_control_set2(context, 0, 0x40000000, port_id , SDL_RL_MOD_BPS, 0 , &val);

            val.rate = 1000000; //unit kbps
            ret = epon_request_onu_port_storm_control_set2(context, 0, 0x40000000, port_id , SDL_RL_MOD_BPS, 1 , &val);

            val.rate = 1000000; //unit kbps
            ret = epon_request_onu_port_storm_control_set2(context, 0, 0x40000000, port_id , SDL_RL_MOD_BPS, 2 , &val);
            */
        }
    }

    p_rsp->len = 0x80;
#endif

    return CA_E_OK;

}

ca_onu_mac_filter_ctrl_t  g_onu_mac_filter_status;


void  get_onu_mac_filter_stauts(ca_onu_mac_filter_ctrl_t *pEntry)
{
    pEntry->status = g_onu_mac_filter_status.status;
    memcpy(pEntry->mac, g_onu_mac_filter_status.mac, 6);
}

void set_onu_mac_filter_status(ca_onu_mac_filter_ctrl_t filter)
{
    g_onu_mac_filter_status.status = filter.status;
    memcpy(g_onu_mac_filter_status.mac, filter.mac, 6);

    OAM_ORG_LOG("status: %d\n", g_onu_mac_filter_status.status);
    OAM_ORG_LOG("mac %02x:%02x:%02x:%02x:%02x:%02x\n", g_onu_mac_filter_status.mac[0], g_onu_mac_filter_status.mac[1], g_onu_mac_filter_status.mac[2], g_onu_mac_filter_status.mac[3], g_onu_mac_filter_status.mac[4], g_onu_mac_filter_status.mac[5]);
}

void  init_onu_mac_filter_ctrl()
{
#if 1
    ca_uint8  port_id = 0;
    ca_onu_mac_filter_ctrl_t entry;

    memset(&entry, 0, sizeof(ca_onu_mac_filter_ctrl_t));
    entry.status = 1;
    set_onu_mac_filter_status(entry);
#else
    ca_callback_context_t  context;
    ca_sdl_mac_tbl_entry_t mac_entry;
    ca_uint8  port_id = 0;
    ca_onu_mac_filter_ctrl_t entry;

    memset(&entry, 0, sizeof(ca_onu_mac_filter_ctrl_t));
    entry.status = 1;
    set_onu_mac_filter_status(entry);

    mac_entry.vlanid = 0;
    mac_entry.mac_type = SDL_SRC_MAC;
    mac_entry.mac.addr[0] = 0x0;
    mac_entry.mac.addr[1] = 0x0;
    mac_entry.mac.addr[2] = 0x0;
    mac_entry.mac.addr[3] = 0x0;
    mac_entry.mac.addr[4] = 0x0;
    mac_entry.mac.addr[5] = 0x0;

    for (port_id = 1; port_id  <= OAM_UNI_PORT_NUM; port_id ++) {
        epon_request_onu_mac_filter_add_entry(context, 0, 0, port_id, 1 , &mac_entry);
        //epon_request_onu_mac_bind_add_entry(context,0,0,port_id, 1 ,&mac_entry);
    }
#endif
}

static ca_uint32  kt_prv_oam_mac_filter_get(ca_uint8 *pOut)
{
    oam_kt_onu_mac_filter_set_t  *pRepEntry = (oam_kt_onu_mac_filter_set_t *)pOut;
    ca_onu_mac_filter_ctrl_t filter;

    get_onu_mac_filter_stauts(&filter);
    pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pRepEntry->hdr.leaf =   htons(0x0026);
    pRepEntry->hdr.len = 0x7;
    pRepEntry->filter = filter.status;
    memcpy(pRepEntry->fmac, filter.mac, 6);
    return sizeof(oam_kt_onu_mac_filter_set_t);
}

static ca_uint32 kt_prv_oam_mac_filter_set(ca_uint8 *pRecv, ca_uint8 *pOut)
{
    oam_kt_onu_mac_filter_set_t *pRepEntry = (oam_kt_onu_mac_filter_set_t *)pRecv;
    oam_kt_var_cont_t *p_rsp = (oam_kt_var_cont_t *)pOut;
    ca_l2_mac_filter_entry_t  mac_entry;
    ca_onu_mac_filter_ctrl_t  filter;
    ca_uint32 port_id, ca_port;
    oam_status ret = OAM_E_OK;

    //to check input parameter invalid  or not
    if (pRepEntry->filter > 1) {
        p_rsp->len = 0x86;
        return CA_E_OK;
    }

    get_onu_mac_filter_stauts(&filter);

    if (pRepEntry->filter == filter.status && pRepEntry->filter > 0) {
        if (!memcmp(filter.mac, pRepEntry->fmac, 6)) {
            p_rsp->len = 0x80;
            return CA_E_OK;
        } else {
            /* to clean up prevous configured MAC filter entries */
            for (port_id = OAM_UNI_PORT_ID1; port_id <= OAM_UNI_PORT_NUM; port_id ++) {
                ca_port = OAM_PORT_ID_TO_CA_PORT(port_id);
                ret = ca_l2_mac_filter_delete_all(0, ca_port);
            }
        }
    }

    memset(&mac_entry, 0, sizeof(ca_l2_mac_filter_entry_t));
    mac_entry.vid = 0;
    mac_entry.mask = CA_L2_MAC_FILTER_MASK_MAC_ONLY;
    mac_entry.drop_flag = 1;
    memcpy(mac_entry.mac, pRepEntry->fmac, 6);

    //disable to enable
    if (pRepEntry->filter) {
        for (port_id = OAM_UNI_PORT_ID1; port_id <= OAM_UNI_PORT_NUM; port_id ++){
            ca_port = OAM_PORT_ID_TO_CA_PORT(port_id);
            ret = ca_l2_mac_filter_add(0, ca_port, &mac_entry);
        }
    } else { // enable to disable
        for (port_id = OAM_UNI_PORT_ID1; port_id <= OAM_UNI_PORT_NUM; port_id ++){
            ca_port = OAM_PORT_ID_TO_CA_PORT(port_id);
            ret = ca_l2_mac_filter_delete_all(0, ca_port);
        }
        //init_onu_mac_filter_ctrl();
    }

    filter.status = pRepEntry->filter;
    memcpy(filter.mac, pRepEntry->fmac, 6);
    set_onu_mac_filter_status(filter);

    if (ret)
        p_rsp->len = 0x86;
    else
        p_rsp->len = 0x80;

    return CA_E_OK;

}

ca_uni_link_status_stats  g_uni_link_status[OAM_UNI_PORT_NUM_MAX];
oam_uint8 g_link_up_port_flag[OAM_UNI_PORT_NUM_MAX];

void uni_link_event_status_init()
{
    int i = 0;
    for ( i = 0; i < OAM_UNI_PORT_NUM_MAX; i ++) {
        g_uni_link_status[i].admin = 1; // 1: enable, 0 : disable
        g_uni_link_status[i].rj45_failure = 0;
        g_uni_link_status[i].cnt_up2down = 0;
        g_link_up_port_flag[i] = 0;
    }
}

oam_status uni_link_event_admin_set(oam_uint16 port, ca_uint8 admin)
{
    OAM_UNI_PORT_ID_CHECK(port);

    OAM_ORG_DEBUG("port %d, admin %d\r\n", port, admin);
    g_uni_link_status[port - 1].admin = admin;

    if(!g_uni_link_status[port - 1].admin)
    {
        g_uni_link_status[port - 1].rj45_failure = 0;
        g_uni_link_status[port - 1].cnt_up2down = 0;
    }
    return OAM_E_OK;
}

oam_status uni_link_event_admin_get(oam_uint16 port, ca_uint8 *admin)
{
    OAM_UNI_PORT_ID_CHECK(port);

    OAM_ORG_DEBUG("port %d, admin %d\r\n", port, g_uni_link_status[port - 1].admin);
    *admin = g_uni_link_status[port - 1].admin;

    return OAM_E_OK;
}


oam_status uni_link_event_cnt_add(oam_uint16 port, ca_uint8 status)
{
    OAM_UNI_PORT_ID_CHECK(port);

    if(status == CA_PORT_LINK_UP){
        OAM_ORG_DEBUG("oam port = %d cnt_up2down = %d\r\n", port, g_uni_link_status[port - 1].cnt_up2down);
        g_uni_link_status[port - 1].cnt_up2down += 1;
    }
    else{
        OAM_ORG_DEBUG("oam port = %d rj45_failure = %d\r\n", port, g_uni_link_status[port - 1].rj45_failure);
        g_uni_link_status[port - 1].rj45_failure += 1;
    }

    return OAM_E_OK;
}


void uni_link_up_polling(void *data)
{
    ca_uint8 port = *(ca_uint8*)data;
    ca_port_id_t ca_port;
    ca_eth_port_link_status_t link_status;

#if defined(CONFIG_ARCH_CORTINA_SATURN_SFP_PLUS)
    if (port != OAM_UNI_PORT_ID1 )
        return;
#else
    if(port < OAM_UNI_PORT_ID1 || port > OAM_UNI_PORT_NUM)
        return;
#endif

    ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_link_status_get(0, ca_port, &link_status);
    
    OAM_ORG_DEBUG("ca_port = 0x%x link %s\r\n", ca_port, (link_status == CA_PORT_LINK_UP)?"up":"down");

    uni_link_event_cnt_add( port, link_status);
}


oam_status uni_link_event_stats_get(oam_uint16 port, ca_uni_link_status_stats *lint_evt_stats)
{
    OAM_UNI_PORT_ID_CHECK(port);

    memcpy(lint_evt_stats, &g_uni_link_status[port -1], sizeof(ca_uni_link_status_stats));

    return CA_E_OK;

}

oam_status uni_link_event_stats_show()
{
    oam_uint16 port;

    for(port = OAM_UNI_PORT_ID1; port < OAM_UNI_PORT_ID1+OAM_UNI_PORT_NUM; port++){

        ca_printf("port: %d\r\n", port);
        ca_printf("  link event admin: %d\r\n", g_uni_link_status[port -1].admin);
        ca_printf("  RJ45 failure conter: %d\r\n", g_uni_link_status[port -1].rj45_failure);
        ca_printf("  up->down counter: %d\r\n", g_uni_link_status[port -1].cnt_up2down);
    }
    return OAM_E_OK ;
}

void oam_uni_link_event_send(oam_uint16 port, oam_uint8 status)
{
    oam_uni_link_event_t link_evt;
    oam_uint8 da_mac[OAM_MACADDR_LEN] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x02};
    oam_uint8 sa_mac[OAM_MACADDR_LEN];

    memcpy(link_evt.pdu_hdr.dst_mac, da_mac, OAM_MACADDR_LEN);
    oam_llid_mac_get_adapt(0, &sa_mac[0]);
    memcpy(link_evt.pdu_hdr.src_mac, sa_mac, OAM_MACADDR_LEN);
    link_evt.pdu_hdr.eth_type = htons(0x8809);
    link_evt.pdu_hdr.sub_type = 0x03;
    link_evt.pdu_hdr.flags = htons(0x0);
    link_evt.pdu_hdr.code = 0xFE;
    link_evt.oui[0] = 0xAA;
    link_evt.oui[1] = 0xAA;
    link_evt.oui[2] = 0xAA;
    link_evt.extCode = 0x0f;
    link_evt.event_type = 0x09;
    link_evt.len = 0x2;
    link_evt.port = port;
    link_evt.link_status = status;
    oam_pkt_send_adapt(OAM_PON_PORT_ID, 0, (oam_uint8 *)&link_evt, sizeof(oam_uni_link_event_t));
}


oam_status oam_uni_link_event_handler(oam_uint16 port, ca_uint8 status)
{
    oam_uint8 admin = 0;
    ca_eth_port_link_status_t link;
    ca_eth_port_speed_t speed;
    ca_shaper_t shaper;
    oam_status ret = OAM_E_OK;

    OAM_UNI_PORT_ID_CHECK(port);
    ca_port_id_t ca_port = OAM_UNI_PORT_ID_TO_CA_PORT(port);

    if (status)
    {
        if (port != 1)
        {
            ca_eth_port_link_status_get(0,ca_port, &link);
            ca_eth_port_phy_speed_get(0, ca_port, &speed);
            
            memset(&shaper, 0, sizeof(ca_shaper_t));
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
            ca_port = 0xC0000|(ca_port&0xFFFF);
            ret = ca_port_shaper_set(0, ca_port, &shaper);
        }
#if 0        /* USE DEEPQ */        
        if(port == 4){
            if((speed == CA_PORT_SPEED_100M) ||(speed == CA_PORT_SPEED_1G) ||
                (speed == CA_PORT_SPEED_2_5G) ||(speed == CA_PORT_SPEED_5G) ){
                   ca_eth_port_use_deep_queue(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET, 5),TRUE);
               }else{
                   ca_eth_port_use_deep_queue(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET, 5),FALSE);
               }
        }
#endif
    }

    uni_link_event_admin_get(port, &admin);
    if(!admin)
    {
        OAM_ORG_DEBUG("link status checking disabled, port %d\r\n", port);
        return OAM_E_OK;
    }

    /* to send link event to  OLT */
    //oam_uni_link_event_send(port,  status);

    /* to startup timer */
    g_link_up_port_flag[port - 1] = port;

    if(status){
        ca_timer_add(2500, uni_link_up_polling, (void*)&g_link_up_port_flag[port - 1]);
    }
    return OAM_E_OK;
}

static  ca_uint32 kt_prv_oam_uni_link_status_cnt_set(ca_uint8 *pRecv, ca_uint8 *pOut){
    oam_kt_onu_uni_link_status_set_t *pRepEntry = (oam_kt_onu_uni_link_status_set_t *)pRecv;
    oam_kt_var_cont_t *p_rsp = (oam_kt_var_cont_t *)pOut;

    OAM_ORG_DEBUG("port %d, admin %d\r\n", pRepEntry->port, pRepEntry->admin);
    /*port ID: 1 ~4, or 0xff     admin: 1-> enable, 0 -> disable*/
    if (pRepEntry->admin > 1 || pRepEntry->port < OAM_UNI_PORT_ID1 ||
        (pRepEntry->port > OAM_UNI_PORT_NUM && pRepEntry->port != OAM_ALL_UNI_PORT_ID)) {
        p_rsp->len = OAM_VAR_SET_BAD_PARAM;
        return CA_E_OK;
    }

    /*to set global UNI port link up/down configuration parameter*/
    if(pRepEntry->port != OAM_ALL_UNI_PORT_ID)
        uni_link_event_admin_set(pRepEntry->port, pRepEntry->admin);
    else
    {
        ca_uint8 port = 0;
        for(port =OAM_UNI_PORT_ID1 ; port <=OAM_UNI_PORT_NUM; port++)
            uni_link_event_admin_set(port, pRepEntry->admin);
    }

    p_rsp->len = OAM_VAR_SET_OK;
    return CA_E_OK;

}

static ca_uint32  kt_prv_oam_uni_link_status_cnt_get(ca_uint8 *pOut){

    oam_kt_onu_uni_link_status_cnt_get_t *pRepEntry = (oam_kt_onu_uni_link_status_cnt_get_t *)pOut;
    ca_uni_link_status_stats lint_evt_stats;
    ca_uint8 uni_port = 0;

    if ((pRepEntry->port >= OAM_UNI_PORT_ID1 && pRepEntry->port <= OAM_UNI_PORT_NUM) || (OAM_ALL_UNI_PORT_ID == pRepEntry->port)) { // 1 ~4: UNI, 0xff: 4ports
        if (pRepEntry->port != OAM_ALL_UNI_PORT_ID) { // single port
            pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
            pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_UNI_LINK_STATUS_CNT);
            pRepEntry->hdr.len = 0x05;
            uni_link_event_stats_get(pRepEntry->port, &lint_evt_stats);
            pRepEntry->rj45_failure = htons(lint_evt_stats.rj45_failure);
            pRepEntry->cnt_up2down = htons(lint_evt_stats.cnt_up2down);
            return sizeof(oam_kt_onu_uni_link_status_cnt_get_t);
        } else { // all the 4 ports
            for (uni_port = OAM_UNI_PORT_ID1; uni_port <= OAM_UNI_PORT_NUM; uni_port ++) {
                pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
                pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_UNI_LINK_STATUS_CNT);
                pRepEntry->hdr.len = 0x05;
                uni_link_event_stats_get(uni_port, &lint_evt_stats);
                pRepEntry->port = uni_port;
                pRepEntry->rj45_failure = htons(lint_evt_stats.rj45_failure);
                pRepEntry->cnt_up2down = htons(lint_evt_stats.cnt_up2down);;
                pRepEntry++;
            }
            pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
            pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_UNI_LINK_STATUS_CNT);
            pRepEntry->hdr.len = OAM_VAR_SET_OK;
            return sizeof(oam_kt_onu_uni_link_status_cnt_get_t)*4 + 4;
        }

    } else { //invlaid port parameter, return errorr to OLT
        pRepEntry->hdr.extBranch = 0xA7;
        pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_UNI_LINK_STATUS_CNT);
        pRepEntry->hdr.len = OAM_VAR_SET_BAD_PARAM;
        return sizeof(oam_kt_var_cont_t);
    }

}


static ca_uint32  kt_prv_oam_uni_link_status_get(ca_uint8 *pOut){

    oam_kt_onu_uni_link_status_get_t *pRepEntry = (oam_kt_onu_uni_link_status_get_t *)pOut;
    //ca_uni_link_status_stats lint_evt_stats;
    ca_uint8 uni_port = 0;
    ca_port_id_t ca_port;
    ca_eth_port_link_status_t status;

    if ((pRepEntry->port >= OAM_UNI_PORT_ID1 && pRepEntry->port <= OAM_UNI_PORT_NUM) || (OAM_ALL_UNI_PORT_ID == pRepEntry->port)) { // 1 ~4: UNI, 0xff: 4ports
        if (pRepEntry->port != OAM_ALL_UNI_PORT_ID) { // single port
            pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
            pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_UNI_LINK_STATUS);
            pRepEntry->hdr.len = 0x02;
            //uni_link_event_stats_get(pRepEntry->port, &lint_evt_stats);
            ca_port = OAM_PORT_ID_TO_CA_PORT(pRepEntry->port);
            ca_eth_port_link_status_get(0, ca_port, &status);
            pRepEntry->status = status;
            return sizeof(oam_kt_onu_uni_link_status_get_t);
        } else { // all the 4 ports
            for (uni_port = OAM_UNI_PORT_ID1; uni_port <= OAM_UNI_PORT_NUM; uni_port ++) {
                pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
                pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_UNI_LINK_STATUS);
                pRepEntry->hdr.len = 0x02;
                //uni_link_event_stats_get(uni_port, &lint_evt_stats);
                ca_port = OAM_PORT_ID_TO_CA_PORT(uni_port);
                ca_eth_port_link_status_get(0, ca_port, &status);
                pRepEntry->port = uni_port;
                pRepEntry->status = status;
                pRepEntry++;
            }
            pRepEntry->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
            pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_UNI_LINK_STATUS);
            pRepEntry->hdr.len = OAM_VAR_SET_OK;
            return sizeof(oam_kt_onu_uni_link_status_get_t)*4 + 4;
        }

    } else { //invlaid port parameter, return errorr to OLT
        pRepEntry->hdr.extBranch = 0xA7;
        pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_UNI_LINK_STATUS);
        pRepEntry->hdr.len = OAM_VAR_SET_BAD_PARAM;
        return sizeof(oam_kt_var_cont_t);
    }

}


#if 1
static ca_uint32  kt_prv_oam_onu_reserve_get(ca_uint8 *pOut)
{
    oam_kt_onu_critical_event_log_get_t *pRepEntry = (oam_kt_onu_critical_event_log_get_t *)pOut;
    pRepEntry->hdr.extBranch = 0xA7;
    pRepEntry->hdr.leaf =  htons(OAM_KT_LEAF_CODE_ONU_CRITICAL_EVENT);
    pRepEntry->hdr.len = 0x86;
    return sizeof(oam_kt_var_cont_t);

}

static ca_uint32  kt_prv_oam_onu_reserve_set(ca_uint8 *pRecv, ca_uint8 *pOut)
{
    //m_kt_onu_critical_event_log_set_t *pRepEntry = (oam_kt_onu_critical_event_log_set_t *)pRecv;
    oam_kt_var_cont_t *p_rsp = (oam_kt_var_cont_t *)pOut;
    //(pRepEntry->module != 0xff)
    {
        p_rsp->len = 0x86;
        return CA_E_OK;
    }
}
#endif
static oam_uint32
kt_prv_oam_default_restore(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    if (OAM_E_OK != kt_prv_oam_default_restore_adapt()) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
        p_rsp->len = OAM_VAR_SET_OK;
    }

    return OAM_E_OK;
}

static oam_uint32
kt_prv_oam_power_reset(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    if (OAM_E_OK != kt_prv_oam_power_reset_adapt()) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
        p_rsp->len = OAM_VAR_SET_OK;
    }

    return OAM_E_OK;
}


static oam_uint32
kt_prv_oam_tx_power_off_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_tx_power_off_container_struct *p_set = (oam_kt_onu_tx_power_off_container_struct *)pRecv;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    if(p_set->action == KT_TX_POWER_OFF_DEACTIVATED)
    {
        OAM_ORG_LOG("\r\nTx power off deactivated\r\n");
        p_rsp->len = OAM_VAR_SET_OK;
    }
    else
    {
        OAM_ORG_LOG("\r\nset action: %d, duration: %d\r\n", p_set->action, p_set->duration);
        if (OAM_E_OK != kt_prv_oam_tx_power_off_set_adapt(p_set->action, p_set->duration)) {
            p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
        }
        else {
            p_rsp->len = OAM_VAR_SET_OK;
        }
    }
    return OAM_E_OK;
}

static oam_uint32
kt_prv_oam_ds_shaping_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_eth_ds_shaping_container_struct *p_set = (oam_kt_onu_eth_ds_shaping_container_struct *)pRecv;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;
    oam_uint32 ds_rate = 0;

    OAM_ORG_LOG("\r\nset portOper: %d, rate0: %d, rate1: %d, rate2: %d\r\n", p_set->portOper,
    p_set->rate[0],p_set->rate[1],p_set->rate[2]);

    if(p_set->portOper == KT_PORT_DS_SHAPING_OPER_DISABLE)
            memset(p_set->rate,0, sizeof(p_set->rate));

    ds_rate = (p_set->rate[0] << 16) + (p_set->rate[1] << 8) + p_set->rate[2];

    if (OAM_E_OK != kt_prv_oam_ds_shaping_set_adapt(0, p_set->portOper,ds_rate)) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
        p_rsp->len = OAM_VAR_SET_OK;
    }

    return OAM_E_OK;
}

static oam_uint32
kt_prv_oam_ds_shaping_get(oam_uint8 *pOut )
{
    oam_kt_onu_eth_ds_shaping_container_struct *pEthDsShapingContainer =
        (oam_kt_onu_eth_ds_shaping_container_struct *)pOut;

    pEthDsShapingContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pEthDsShapingContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_ETH_DS_SHAPING);
    if(OAM_E_OK != kt_prv_oam_ds_shaping_get_adapt(0, &pEthDsShapingContainer->portOper, pEthDsShapingContainer->rate))
    {
            pEthDsShapingContainer->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
            return sizeof(oam_kt_var_cont_t);
    }
    else
    {
            if(pEthDsShapingContainer->portOper == KT_PORT_DS_SHAPING_OPER_DISABLE)
            {
                    pEthDsShapingContainer->hdr.len = 1;
                    return  sizeof(oam_kt_onu_eth_ds_shaping_container_struct) - 3;
            }
            else
            {
                    pEthDsShapingContainer->hdr.len = 4;
                    OAM_ORG_LOG("\r\nget portOper: %d, rate0: %d, rate1: %d, rate2: %d\r\n", pEthDsShapingContainer->portOper,
                    pEthDsShapingContainer->rate[0],pEthDsShapingContainer->rate[1],pEthDsShapingContainer->rate[2]);
                    return sizeof(oam_kt_onu_eth_ds_shaping_container_struct);
            }
    }
}

static oam_uint32
kt_prv_oam_eth_rstp_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_rstp_container_struct *p_set = (oam_kt_onu_rstp_container_struct *)pRecv;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    OAM_ORG_LOG("\r\nset oper: %d\r\n", p_set->oper);
    if (OAM_E_OK != kt_prv_oam_eth_rstp_set_adapt(p_set->oper)) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
        p_rsp->len = OAM_VAR_SET_OK;
    }
    return OAM_E_OK;
}

static oam_uint32
kt_prv_oam_eth_rstp_get(oam_uint8 *pOut )
{
    oam_kt_onu_rstp_container_struct *pEthRstpContainer =
        (oam_kt_onu_rstp_container_struct *)pOut;
    unsigned char oper;

    pEthRstpContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pEthRstpContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_ETH_RSTP);
    pEthRstpContainer->hdr.len = sizeof(unsigned char);
    if (OAM_E_OK != kt_prv_oam_eth_rstp_get_adapt(&oper)) {
        pEthRstpContainer->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }
    pEthRstpContainer->oper = oper;

    OAM_ORG_LOG("\r\nget oper: %d\r\n", pEthRstpContainer->oper);
    return sizeof(oam_kt_onu_rstp_container_struct);
}

static oam_uint32
kt_prv_oam_static_mac_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_static_mac_container_struct *pStaticMacContainer = (oam_kt_onu_static_mac_container_struct *)pRecv ;
    oam_uint16 i = 0;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    if(pStaticMacContainer->hdr.len == 1)
    {/*clear*/
        OAM_ORG_LOG("\r\nclear fdb entries\r\n");
        kt_prv_oam_all_static_fdb_entries_del_adapt();
    }
    else
    {/*set*/
        if(pStaticMacContainer->count > KT_ORG_MAX_STATIC_MAC)
            i = KT_ORG_MAX_STATIC_MAC;
        else
            i = pStaticMacContainer->count;

        OAM_ORG_LOG("\r\nAdd fdb entries, count: %d\r\n", i);

        /*kt_prv_oam_static_fdb_entries_add_adapt((unsigned char *)pStaticMacContainer->mac, i);*/

        if (OAM_E_OK != kt_prv_oam_static_fdb_entries_add_adapt((unsigned char *)pStaticMacContainer->mac, i)) {
            p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
        }
        else {
            p_rsp->len = OAM_VAR_SET_OK;
        }
    }

    return OAM_E_OK;
}

static oam_uint32
kt_prv_oam_static_mac_get(oam_uint8 *pOut )
{
    oam_kt_onu_static_mac_container_struct *pStaticMacContainer =
        (oam_kt_onu_static_mac_container_struct *)pOut;
    unsigned char num = 0;

    pStaticMacContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pStaticMacContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_STATIC_MAC);

    kt_prv_oam_static_mac_get_adapt((unsigned char *)pStaticMacContainer->mac, &num);
    pStaticMacContainer->count = num;
    pStaticMacContainer->hdr.len = 1 + num*OAM_MACADDR_LEN;

    OAM_ORG_LOG("\r\ncount: %d\r\n", num);
    return sizeof(oam_kt_var_cont_t)+pStaticMacContainer->hdr.len;
}

static oam_uint32
kt_prv_oam_opm_alarm_status_get(oam_uint8 *pOut )
{
    oam_kt_onu_opm_alarm_container_struct *pOpmContainer =
        (oam_kt_onu_opm_alarm_container_struct *)pOut;

    pOpmContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pOpmContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_OPM_ALARM);

    pOpmContainer->hdr.len = 1;
    pOpmContainer->value = g_opm_status;

    OAM_ORG_LOG("\r\nopm alarm status: %d\r\n", pOpmContainer->value);
    return sizeof(oam_kt_var_cont_t)+pOpmContainer->hdr.len;
}

static oam_uint32
kt_prv_oam_vol_alarm_status_get(oam_uint8 *pOut )
{
    oam_kt_onu_vol_alarm_container_struct *pVolContainer =
        (oam_kt_onu_vol_alarm_container_struct *)pOut;

    pVolContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pVolContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_VOL_ALARM);

    pVolContainer->hdr.len = 1;

    pVolContainer->value = g_highvolt_status;

    OAM_ORG_LOG("\r\nvoltage alarm status: %d\r\n", pVolContainer->value);
    return sizeof(oam_kt_var_cont_t)+pVolContainer->hdr.len;
}


static oam_uint32
kt_prv_oam_onu_mac_table_get(oam_uint8 *pOut )
{
    oam_kt_tlv_t *p_rsp_tlv =(oam_kt_tlv_t *)pOut;
    oam_uint32 mac_num = 0;
    oam_boolean first = FALSE;
    oam_uint16 i = 0;
    oam_macaddr_t mac[KT_ORG_MAX_ONU_MAC];
    oam_uint32 len = 0;

    p_rsp_tlv->branch = OAM_KT_ATTRIB_BRANCH;
    p_rsp_tlv->leaf = htons(OAM_KT_LEAF_CODE_ONU_MAC_TABLE);
    len += sizeof(oam_kt_tlv_t) - 1;

    kt_prv_oam_onu_mac_table_get_adapt((oam_uint8 *)mac, &mac_num);

    OAM_ORG_LOG("\r\ncount: %d\r\n", mac_num);

    if(mac_num > 0)
    {
        for(i = 0; i< mac_num; i ++)
        {
            if(!first)
            {
                first = TRUE;
                p_rsp_tlv->len = sizeof(oam_macaddr_t);
                memcpy(p_rsp_tlv->data, &mac[i], sizeof(oam_macaddr_t));
                len += sizeof(oam_macaddr_t);
            }
            else
            {
                pOut += sizeof(oam_kt_tlv_t) - 1 + sizeof(oam_macaddr_t);
                p_rsp_tlv = (oam_kt_tlv_t *)pOut;
                p_rsp_tlv->branch = OAM_KT_ATTRIB_BRANCH;
                p_rsp_tlv->leaf = htons(OAM_KT_LEAF_CODE_ONU_MAC_TABLE);
                p_rsp_tlv->len = sizeof(oam_macaddr_t);
                memcpy(p_rsp_tlv->data, &mac[i], sizeof(oam_macaddr_t));
                len += sizeof(oam_kt_tlv_t) - 1 + sizeof(oam_macaddr_t);
            }
        }
    }

    if(first){
        pOut += sizeof(oam_kt_tlv_t) - 1 + sizeof(oam_macaddr_t);
        p_rsp_tlv = (oam_kt_tlv_t *)pOut;
        p_rsp_tlv->branch = OAM_KT_ATTRIB_BRANCH;
        p_rsp_tlv->leaf = htons(OAM_KT_LEAF_CODE_ONU_MAC_TABLE);
        p_rsp_tlv->len = OAM_KT_VAR_RSP_CODE_OK;
        len += sizeof(oam_kt_tlv_t) - 1;
    }
    else{
        p_rsp_tlv->len = OAM_KT_VAR_RSP_CODE_OK;
    }

    return len;
}

static oam_uint32
kt_prv_oam_dynamic_mac_table_clr(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_var_cont_t *p_set = (oam_kt_var_cont_t *)pRecv ;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;

    if(p_set->len != 0)
    {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    } else {
        /*clear*/

        OAM_ORG_LOG("\r\nClr fdb entries, count: %d\r\n" );

        kt_prv_oam_all_dynamic_fdb_entries_clr_adapt ();

        p_rsp->len = OAM_VAR_SET_OK;
    }

    return OAM_E_OK;
}

static oam_uint32
oam_build_kt_get_port_counter(oam_uint8 *pOut, oam_uint8 ktIndex)
{
    oam_kt_onu_port_counter_container_struct *pPortCounterContainer =
        (oam_kt_onu_port_counter_container_struct *)pOut;
    oam_kt_onu_port_counter_container_struct counter;

    pPortCounterContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pPortCounterContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_PORT_COUNTER);
    /* 3 int64 are reseved */
    pPortCounterContainer->hdr.len = sizeof(oam_kt_onu_port_counter_container_struct) -
                                     sizeof(oam_kt_var_cont_t) - (8 *4);
    if (OAM_E_OK != kt_prv_oam_port_counter_get_adapt(ktIndex, &counter)) {
        pPortCounterContainer->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }
    memcpy(&pPortCounterContainer->mask, &counter.mask, sizeof(oam_kt_onu_port_counter_container_struct)
        - sizeof(oam_kt_var_cont_t));
    return sizeof(oam_kt_onu_port_counter_container_struct);
}

oam_int32
oam_build_kt_prv_port_instance(
    oam_uint8 *bufBase,
    oam_uint8 portId
)
{
#if 0
    oam_pdu_mix_index_t *pIndexCont = (oam_pdu_mix_index_t *)bufBase;

    pIndexCont->hdr.extBranch = OAM_KT_INDEX_BRANCH;
    pIndexCont->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_PORT);
    pIndexCont->hdr.len = sizeof(unsigned char);
    pIndexCont->value = portId;
#endif
    return sizeof(oam_pdu_mix_index_t);
}

static oam_uint32
kt_prv_oam_get_port_base_func(
    unsigned char *pOut,
    unsigned char portId,
    unsigned short leaf,
    oam_boolean incomingPortIndex,
    oam_boolean *outgoingPortIndex,
    kt_eth_func_t func)
{
    unsigned char *ptr = pOut;
    unsigned int  len = 0, retLen = 0, i;
    oam_kt_var_cont_t *varHdr =
        (oam_kt_var_cont_t *)pOut;


    OAM_ORG_LOG("\r\nportId %d, incoming %d,  outgoing %d\r\n", portId, incomingPortIndex, *outgoingPortIndex );

    if (!incomingPortIndex) {
        varHdr->extBranch = OAM_KT_ATTRIB_BRANCH;
        varHdr->leaf = htons(leaf);
        varHdr->len = OAM_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }

    if (portId == OAM_ALL_UNI_PORT_ID) {
        for (i = OAM_UNI_PORT_ID1; i <= OAM_UNI_PORT_NUM; i++) {
            len = oam_build_kt_prv_port_instance(ptr, i);
            ptr += len;
            retLen += len;

            len = func(ptr, i);
            ptr += len;
            retLen += len;
        }
    }
    else {
        if (!*outgoingPortIndex) {
            len = oam_build_kt_prv_port_instance(ptr, portId);
            retLen += len;
            ptr += len;
            *outgoingPortIndex = TRUE;
        }

        len = func(ptr, portId);
        ptr += len;
        retLen += len;
    }
    return retLen;
}

static oam_uint32
kt_prv_oam_olt_sen_exceed_alarm_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_olt_sen_exceed_alarm_container_struct *p_set =
        (oam_kt_olt_sen_exceed_alarm_container_struct *)pRecv;
    oam_kt_var_cont_t *p_rsp = (oam_kt_var_cont_t *)pOut;


    if (KT_SEN_EXCEED_ALARM_RAISE == p_set->value)
    {
        //ca_led_sensitivity_set(TRUE);
        cap_log_msg(0, CA_MOD_ORG_OAM_APP, CAP_LOG_EMERGENCY,0, "OLT광파워알람발생\n");
        kt_log_sen_exceed_raised_alarm();
        OAM_ORG_LOG( "OLT sensitivity exceed alarm raised\n");
    }
    else if (KT_SEN_EXCEED_ALARM_RELEASE == p_set->value)
    {
        //ca_led_sensitivity_set(FALSE);
        cap_log_msg(0, CA_MOD_ORG_OAM_APP, CAP_LOG_EMERGENCY,0, "OLT광파워알람해제\n");
        OAM_ORG_LOG( "OLT sensitivity exceed alarm released\n");
    }
    else
    {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
        return OAM_E_OK;
    }

    p_rsp->len = OAM_VAR_SET_OK;
    g_olt_sen_alarm = p_set->value;


    return OAM_E_OK;
}

static oam_uint32
kt_prv_oam_olt_sen_exceed_alarm_get(oam_uint8 *pOut )
{
    oam_kt_olt_sen_exceed_alarm_container_struct *pContainer =
        (oam_kt_olt_sen_exceed_alarm_container_struct *)pOut;

    pContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_OLT_SEN_EXCEED_ALARM);

    pContainer->hdr.len = sizeof(oam_kt_olt_sen_exceed_alarm_container_struct) -
                            sizeof(oam_kt_var_cont_t);
    pContainer->value = g_olt_sen_alarm;

    OAM_ORG_LOG("\r\nvalue: %d\r\n", pContainer->value);

    return  sizeof(oam_kt_olt_sen_exceed_alarm_container_struct);
}

static oam_uint32
kt_prv_oam_onu_opm_ddm_get(oam_uint8 *pOut )
{
    oam_kt_onu_opm_ddm_container_struct *pContainer =
        (oam_kt_onu_opm_ddm_container_struct *)pOut;
    oam_status ret = OAM_E_OK;
    oam_int16 temp = 0;
    oam_uint16 volt = 0, tx_bias = 0, tx_power = 0, rx_power = 0;

    pContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_OPM_DDM);

    OAM_ORG_LOG("\r\nddm get\r\n");
#if 0
    /* return not support for old board */
    if (!kt_prv_oam_is_new_board_get_adapt())
    {
        pContainer->hdr.len = OAM_KT_VAR_RSP_CODE_UNSUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }
#endif

    ret = kt_prv_oam_opm_ddm_get_adapt(&temp, &volt, &tx_bias, &tx_power, &rx_power);
    if(OAM_E_OK != ret)
    {
        pContainer->hdr.len = OAM_KT_VAR_RSP_CODE_UNSUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }

    pContainer->temperature = htons(temp);
    pContainer->voltage     = htons(volt);
    pContainer->tx_bias     = htons(tx_bias);
    pContainer->tx_power    = htons(tx_power);
    pContainer->rx_power    = htons(rx_power);

    pContainer->hdr.len = sizeof(oam_kt_onu_opm_ddm_container_struct) -
                            sizeof(oam_kt_var_cont_t);
    return  sizeof(oam_kt_onu_opm_ddm_container_struct);
}


static oam_uint32
kt_prv_oam_onu_time_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_system_time_t *p_set = (oam_kt_onu_system_time_t *)pRecv;
    oam_kt_var_cont_t *p_rsp =
        (oam_kt_var_cont_t *)pOut;
    kt_prv_system_time_t sys_time;

    OAM_ORG_LOG("\r\nset system time: year0: %d, year1: %d, month: %d, day: %d\r\n",
                p_set->year[0],p_set->year[1], p_set->month, p_set->day);
    OAM_ORG_LOG("\r\nset system time: hour: %d, minute: %d, second: %d\r\n",
                p_set->hour, p_set->minute, p_set->second);

    memset(&sys_time, 0, sizeof(kt_prv_system_time_t));
    sys_time.year = p_set->year[0] * 100 + p_set->year[1];
    sys_time.month = p_set->month;
    sys_time.day = p_set->day;
    sys_time.hour = p_set->hour;
    sys_time.minute = p_set->minute;
    sys_time.second = p_set->second;

    OAM_ORG_LOG("\r\nyear: %d\r\n", sys_time.year);

     if((sys_time.year > OAM_KT_SYS_TIME_YEAR_MAX) ||
        (sys_time.month < OAM_KT_SYS_TIME_MONTH_MIN)||
        (sys_time.month > OAM_KT_SYS_TIME_MONTH_MAX)||
        (sys_time.day < OAM_KT_SYS_TIME_DAY_MIN)||
        (sys_time.day > OAM_KT_SYS_TIME_DAY_MAX)||
        (sys_time.hour > OAM_KT_SYS_TIME_HOUR_MAX)||
        (sys_time.minute > OAM_KT_SYS_TIME_MINUTE_MAX)||
        (sys_time.second > OAM_KT_SYS_TIME_MINUTE_MAX))
    {
         p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
         return OAM_E_OK;
    }

    if (OAM_E_OK != kt_prv_oam_onu_time_set_adapt(&sys_time)) {
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else {
#if 0/*Wait for Andrew confirm*/
        cap_log_msg(0, CA_MOD_ORG_OAM_APP, CAP_LOG_EMERGENCY,0,
            "ONU Timer Set to %04d-%02d-%02d %02d:%02d:%02d\n",
            sys_time.year, sys_time.month, sys_time.day, sys_time.hour, sys_time.minute, sys_time.second);
#else
        ca_printf("ONU Timer Set to %04d-%02d-%02d %02d:%02d:%02d\n",
            sys_time.year, sys_time.month, sys_time.day, sys_time.hour, sys_time.minute, sys_time.second);
#endif
        p_rsp->len = OAM_VAR_SET_OK;
    }

    return OAM_E_OK;
}


static oam_uint32
kt_prv_oam_onu_ddm_threshold_set(
    oam_uint8 *pRecv,
    oam_uint8 *pOut )
{
    oam_kt_onu_ddm_threshold_set_t *pContainer = (oam_kt_onu_ddm_threshold_set_t *)pRecv;
    oam_kt_var_cont_t *p_rsp = (oam_kt_var_cont_t *)pOut;
    oam_status ret = OAM_E_OK;
    ddm_threshold_t threshold;

    if (pContainer == NULL || p_rsp == NULL){
        OAM_ORG_LOG("NULL pointer");
        return OAM_E_PARAM;
    }

    memset(&threshold, 0, sizeof(ddm_threshold_t));

    threshold.id = pContainer->id;
    /*if (threshold.id == OPM_TEMPERATURE){
        threshold.high_raise.sign_value = pContainer->high_raise;
        threshold.high_clear.sign_value = pContainer->high_clear;
        threshold.low_raise.sign_value = pContainer->low_raise;
        threshold.low_clear.sign_value = pContainer->low_clear;
    }
    else*/{
        threshold.high_raise.unsign_value = htons(pContainer->high_raise);
        threshold.high_clear.unsign_value = htons(pContainer->high_clear);
        threshold.low_raise.unsign_value = htons(pContainer->low_raise);
        threshold.low_clear.unsign_value = htons(pContainer->low_clear);
    }

    ret = kt_prv_oam_ddm_threshold_set_adapt(&threshold);
    if(OAM_E_OK != ret){
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }
    else{
        p_rsp->len = OAM_VAR_SET_OK;
    }

    return OAM_E_OK;
}


static oam_uint32
kt_prv_oam_onu_ddm_threshold_get(oam_uint8 *pOut )
{
    oam_kt_onu_ddm_threshold_get_t *pContainer = (oam_kt_onu_ddm_threshold_get_t *)pOut;
    ddm_threshold_t threshold;
    oam_uint16 i = 0;
    oam_status ret = OAM_E_OK;

    if (pContainer == NULL){
        OAM_ORG_LOG("NULL pointer");
        return OAM_E_PARAM;
    }

    memset(&threshold, 0, sizeof(ddm_threshold_t));

    pContainer->hdr.extBranch = OAM_KT_ATTRIB_BRANCH;
    pContainer->hdr.leaf = htons(OAM_KT_LEAF_CODE_ONU_DDM_THRESHOLD);

    OAM_ORG_LOG("\r\nthreshold get\r\n");
    for(i = 0; i < OPM_THRES_MAX; i ++){
        threshold.id = i + 1;
        ret = kt_prv_oam_ddm_threshold_get_adapt(&threshold);
        if(OAM_E_OK != ret) break;
        pContainer->threshold[i].id = i + 1;
        pContainer->threshold[i].low_raise.unsign_value = htons(threshold.low_raise.unsign_value);
        pContainer->threshold[i].low_clear.unsign_value = htons(threshold.low_clear.unsign_value);
        pContainer->threshold[i].high_raise.unsign_value = htons(threshold.high_raise.unsign_value);
        pContainer->threshold[i].high_clear.unsign_value = htons(threshold.high_clear.unsign_value);
        pContainer->threshold[i].state = threshold.state;
    }

    if(OAM_E_OK != ret){
        pContainer->hdr.len = OAM_KT_VAR_RSP_CODE_UNSUPPORTED;
        return sizeof(oam_kt_var_cont_t);
    }

    pContainer->hdr.len = sizeof(oam_kt_onu_ddm_threshold_get_t) - sizeof(oam_kt_var_cont_t);

    return sizeof(oam_kt_onu_ddm_threshold_get_t);
}

extern oam_status oam_guarddog_reboot_enable(
    oam_uint16 up_rate, oam_uint16 down_rate,
    oam_uint16 start_hour, oam_uint16 start_min,
    oam_uint16 end_hour, oam_uint16 end_min);

static oam_uint32  kt_prv_oam_guarddog_reboot(oam_uint8 *pRecv, oam_uint8 *pOut)
{
    oam_uint16 up_rate, down_rate, start_hour, start_min, end_hour, end_min;
    oam_guarddog_reboot_t *pRepEntry = (oam_guarddog_reboot_t *)pRecv;
    oam_kt_var_cont_t *p_rsp = (oam_kt_var_cont_t *)pOut;

    up_rate = ntohs(pRepEntry->up_rate);
    down_rate = ntohs(pRepEntry->down_rate);
    start_hour = ntohs(pRepEntry->start_hour);
    start_min = ntohs(pRepEntry->start_min);
    end_hour = ntohs(pRepEntry->end_hour);
    end_min = ntohs(pRepEntry->end_min);

    OAM_ORG_LOG("\r\ndown_rate: %d, up_rate: %d, start_hour: %d, start_min: %d, end_hour: %d, end_min: %d\r\n",
                down_rate, up_rate, start_hour, start_min, end_hour, end_min);

    if(start_hour > 23 || start_min > 59 || end_hour > 23 || end_min > 59)
    {
        p_rsp->len = OAM_VAR_SET_BAD_PARAM;
        return OAM_E_PARAM;
    }

    if((start_hour * 60 + start_min) > (end_hour * 60 + end_min))
    {
        p_rsp->len = OAM_VAR_SET_BAD_PARAM;
        return OAM_E_PARAM;
    }

    //if(cs8032_pkt_padding_flag_get()){
        //oam_printf("OAM 0x002A dn %d, up %d, start[%d,%d], end[%d,%d]\n", pRepEntry->down_rate, pRepEntry->up_rate, \
        //pRepEntry->start_hour, pRepEntry->start_min, pRepEntry->end_hour, pRepEntry->end_min);
    //}

    if(oam_guarddog_reboot_enable(up_rate, down_rate,start_hour, start_min,end_hour, end_min)){
        p_rsp->len = OAM_ATTRIB_NOT_SUPPORTED;
    }

    p_rsp->len = OAM_VAR_SET_OK;
    return OAM_E_OK;
}

onu_kt_prv_oam_xlat_rountine_t g_kt_prv_oam_xlat_table[] =
{
   #undef xx
   #define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC)   {BRANCH,LEAF,GET_FUNC,SET_FUNC},
   KT_PRV_OAM_BUILD_RESP
};

static oam_int32
oam_build_kt_prv_var_resp (
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 length)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_pdu_kt_t *pRecvPkt = (oam_pdu_kt_t *)frame;
    oam_pdu_kt_t *pOutPkt = (oam_pdu_kt_t *)pdu;
    oam_kt_onu_var_descriptor_struct *pRecvVarCont = NULL;
    oam_uint8 *pInVarCont = NULL;
    oam_uint8 *pOutVarCont = NULL;
    oam_pdu_mix_index_t *pIndexCont = NULL;
    oam_uint32 totalContentLen = 0;
    oam_boolean incomingPortIndex = FALSE, outgoingPortIndex = FALSE;
    oam_uint32 outPktLen = 0;
    oam_uint32 i=0;
    oam_uint8 ktIndex=0;
    oam_uint8 good_packet = 0;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    if(!frame ||
            length <= (sizeof(oam_pdu_kt_t) - 1))
        return 0;

    if (!intf) {
        OAM_ORG_LOG( "\r\nat unknown port 0x%x\r\n", llid);
        return 0;
    }

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
    pOutPkt->opcode = OAM_PDU_KT_VAR_RESP;
    outPktLen =  sizeof(oam_pdu_kt_t) - 1;
    totalContentLen = length - sizeof(oam_pdu_kt_t) + 1;

    pRecvVarCont = (oam_kt_onu_var_descriptor_struct *)pRecvPkt->data;
    pOutVarCont = pOutPkt->data;

    while(totalContentLen>= (sizeof(oam_kt_tlv_t)-1)){
        oam_kt_var_cont_t *pOut = (oam_kt_var_cont_t *)pOutVarCont;

        pInVarCont = (oam_uint8 *)pRecvVarCont;
        pRecvVarCont->leaf = ntohs(pRecvVarCont->leaf);

        OAM_ORG_LOG( "\r\nbranch 0x%x leaf 0x%x\r\n", pRecvVarCont->extBranch, pRecvVarCont->leaf);

        /*if index-of-instance branch, save index value*/
        if(pRecvVarCont->extBranch == OAM_KT_INDEX_BRANCH) {
            switch(pRecvVarCont->leaf) {
                case OAM_KT_LEAF_CODE_ONU_PORT:
                    pIndexCont = (oam_pdu_mix_index_t *)pRecvVarCont;
                    ktIndex = pIndexCont->value;
                    incomingPortIndex = TRUE;
                    outgoingPortIndex = FALSE;
                    break;
                default:
                    return 0;
                    break;
            }
            totalContentLen -= sizeof(oam_pdu_mix_index_t);
            pRecvVarCont = (oam_kt_onu_var_descriptor_struct *)((unsigned char *)pRecvVarCont +
                sizeof(oam_pdu_mix_index_t));
            continue;
        }

        switch(pRecvVarCont->extBranch)
            {
                    case OAM_ATTRIB_BRANCH_NULL:
                        if(!good_packet)
                                goto ignored;/*clause57 57.6.3 a)*/
                        else
                                goto send_resp;
                    case OAM_KT_ATTRIB_BRANCH:
                            pOut->extBranch = pRecvVarCont->extBranch;
                            pOut->leaf = htons(pRecvVarCont->leaf);

                            if(OAM_KT_LEAF_CODE_PER_PORT_STATISTICS_GET == pRecvVarCont->leaf){
                                oam_kt_onu_per_port_statistics_entries_t *pRspEntry  = (oam_kt_onu_per_port_statistics_entries_t *)pOut;
                                oam_kt_onu_per_port_statistics_entries_t *pRcvEntry  = (oam_kt_onu_per_port_statistics_entries_t *)pRecvVarCont;
                                pRspEntry->port_id = pRcvEntry->port_id;
                            }

                            if(OAM_KT_LEAF_CODE_ONU_PORT_COUNTER == pRecvVarCont->leaf)
                            {
                                    oam_uint32 len=0;

                                    len = kt_prv_oam_get_port_base_func(pOutVarCont, ktIndex,
                                            OAM_KT_LEAF_CODE_ONU_PORT_COUNTER, incomingPortIndex,
                                            &outgoingPortIndex, oam_build_kt_get_port_counter);

                                    outPktLen += len;
                                    pOutVarCont += len;
                                    pInVarCont += sizeof(oam_kt_onu_var_descriptor_struct);
                            }
                            else
                            {
                                OAM_ORG_LOG("\r\nstart to find handler for branch 0x%x leaf 0x%x\r\n", pRecvVarCont->extBranch, pRecvVarCont->leaf);
                                for(i =0 ; i<sizeof(g_kt_prv_oam_xlat_table)/sizeof(g_kt_prv_oam_xlat_table[0]) ; i++)
                                {
                                    if((g_kt_prv_oam_xlat_table[i].branch == pRecvVarCont->extBranch)&&
                                        (g_kt_prv_oam_xlat_table[i].leaf == pRecvVarCont->leaf))
                                    {
                                            oam_uint32 len=0;
                                            //add a patch to input addition port parameter for 0xA7/0x0022 and 0xA7/0x001D
                                            switch (pRecvVarCont->leaf) {
                                            case 0x001D:
                                            case 0x001E:
                                            case 0x0022:
                                            case 0x0023:
                                            case 0x0024:
                                            case 0x0025:
                                            case 0x0026:
                                            case 0x0027:
                                            case 0x0028:
                                            case 0x0029:
                                            case 0x002A:
                                                pOutVarCont[sizeof(oam_kt_onu_var_descriptor_struct)] = frame[sizeof(oam_pdu_kt_t) - 1  + sizeof(oam_kt_onu_var_descriptor_struct)];
                                                pOutVarCont[sizeof(oam_kt_onu_var_descriptor_struct) + 1] = frame[sizeof(oam_pdu_kt_t)  + sizeof(oam_kt_onu_var_descriptor_struct)];
                                                break;
                                            default:
                                                // do nothing
                                                break;

                                            }

                                            if(g_kt_prv_oam_xlat_table[i].get_func)
                                            {
                                                len = g_kt_prv_oam_xlat_table[i].get_func(pOutVarCont);
                                            }
                                            else{
                                                pOut->len = OAM_ATTRIB_NOT_SUPPORTED;
                                                goto send_resp;
                                            }

                                            outPktLen += len;
                                            pOutVarCont += len;
                                            pInVarCont += sizeof(oam_kt_onu_var_descriptor_struct);
                                            break;
                                }
                            }

                            if(i == sizeof(g_kt_prv_oam_xlat_table)/sizeof(g_kt_prv_oam_xlat_table[0])) {
                                OAM_ORG_LOG("\r\nhandler for branch 0x%x leaf 0x%x not find\r\n", pRecvVarCont->extBranch, pRecvVarCont->leaf);
                                pOut->len = OAM_ATTRIB_NOT_SUPPORTED;
                                goto send_resp;
                            }
                        }

                        incomingPortIndex = FALSE;
                        outgoingPortIndex = FALSE;
                        break;
                    default:
                        if(!good_packet)
                                goto ignored;/*clause57 57.6.3 a)*/
                        else
                                goto send_resp;
                        OAM_ORG_LOG("\r\nunknown KT private branch 0x%x\r\n", pRecvVarCont->extBranch);
                        break;
            }

        totalContentLen -= sizeof(oam_kt_onu_var_descriptor_struct);
        if(totalContentLen < sizeof(oam_kt_onu_var_descriptor_struct))
            break;
        if(outPktLen >= OAM_MAX_OAMPDU_SIZE-1)
            break;

        pRecvVarCont = (oam_kt_onu_var_descriptor_struct *)pInVarCont;
        good_packet = 1;
    }

send_resp:
    kt_oam_send(OAM_PON_PORT_ID, llid, pdu, outPktLen);
    intf->org_specific_tx++;

    return outPktLen;

ignored:
    return outPktLen;
}

static oam_int32
oam_build_kt_prv_set_resp(
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_int32 length)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_pdu_kt_t *pOutPkt = (oam_pdu_kt_t *)pdu;
    oam_pdu_kt_t *pRecvPkt = (oam_pdu_kt_t *)frame;
    oam_kt_onu_var_set_container_hdr_struct *pRecvVarCont = NULL;
    oam_uint8 *pOutVarCont = NULL;
    oam_uint8 *pInVarCont = NULL;
    oam_uint32 recvContainerLen = 0;
    oam_uint32 outPktLen = 0;
    oam_int32 i = 0;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    if(!frame ||
         length <= sizeof(oam_pdu_kt_t) + sizeof(oam_kt_onu_var_container_hdr_struct) - 1)
            return 0;

    if (!intf) {
        OAM_ORG_LOG("\r\nat unknown port 0x%x\r\n", llid);
        return 0;
    }

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
    pOutPkt->opcode = OAM_PDU_KT_SET_RESP;
    pOutVarCont = pOutPkt->data;
    outPktLen += sizeof(oam_pdu_kt_t) - 1;

    pRecvVarCont = (oam_kt_onu_var_set_container_hdr_struct *)pRecvPkt->data;
    pInVarCont = (oam_uint8 *)pRecvVarCont;

    pRecvVarCont->leaf = ntohs(pRecvVarCont->leaf);

    switch(pRecvVarCont->extBranch)
    {
        case OAM_KT_ATTRIB_BRANCH:
        case OAM_KT_ACTION_BRANCH:
            {
            oam_kt_onu_var_container_hdr_struct *pOut = (oam_kt_onu_var_container_hdr_struct *)pOutVarCont;
            pOut->extBranch = pRecvVarCont->extBranch;
            pOut->leaf = htons(pRecvVarCont->leaf);
            for( ; i<sizeof(g_kt_prv_oam_xlat_table)/sizeof(g_kt_prv_oam_xlat_table[0]) ; i++)
            {
                if((g_kt_prv_oam_xlat_table[i].branch == pRecvVarCont->extBranch)&&
                    (g_kt_prv_oam_xlat_table[i].leaf == pRecvVarCont->leaf))
                {
                    OAM_ORG_LOG("\r\nstart to find handler for branch 0x%x leaf 0x%x\r\n", pRecvVarCont->extBranch, pRecvVarCont->leaf);

                    if(g_kt_prv_oam_xlat_table[i].set_func != NULL)
                        g_kt_prv_oam_xlat_table[i].set_func((oam_uint8 *)pRecvVarCont, pOutVarCont);
                    else{
                        pOut->len = OAM_ATTRIB_NOT_SUPPORTED;
                        goto send_resp;
                    }

                    /*if(OAM_KT_LEAF_CODE_ONU_STATIC_MAC == pRecvVarCont->leaf)
                        goto ignored;*/

                    outPktLen += sizeof(oam_kt_onu_var_container_hdr_struct);
                    pOutVarCont += sizeof(oam_kt_onu_var_container_hdr_struct);
                    recvContainerLen += sizeof(oam_kt_onu_set_container_hdr_struct) -1
                                                      + pRecvVarCont->len;
                    pInVarCont += recvContainerLen;
                    break;
                }
            }

            if(i == sizeof(g_kt_prv_oam_xlat_table)/sizeof(g_kt_prv_oam_xlat_table[0])){
                OAM_ORG_LOG("\r\nhandler for branch 0x%x leaf 0x%x not find\r\n", pRecvVarCont->extBranch, pRecvVarCont->leaf);
                pOut->len = OAM_ATTRIB_NOT_SUPPORTED;
                goto send_resp;
            }

            break;
            }
        case OAM_ATTRIB_BRANCH_NULL:
        default:
            OAM_ORG_LOG("\r\nunknown KT private branch 0x%x\r\n", pRecvVarCont->extBranch);
            goto ignored;/*clause57 57.6.3 a)*/
            break;

     }

    if(outPktLen >= OAM_MAX_OAMPDU_SIZE - 1)
        goto ignored;

send_resp:
    kt_oam_send(OAM_PON_PORT_ID, llid, pdu, outPktLen);
    intf->org_specific_tx++;

    return outPktLen;

ignored:
    return outPktLen;
}

static oam_uint32
oam_build_kt_resp(oam_llid_t llid, oam_uint8 *frame, int length)
{
    oam_pdu_kt_pdu_t *pdu = (oam_pdu_kt_pdu_t *)frame;
    oam_kt_var_cont_t *pRecvVarCont = NULL;
    oam_kt_var_cont_t *pRspVarCont = NULL;
    oam_uint8  rsp[OAM_MAX_OAMPDU_SIZE];
    oam_uint8  len = sizeof(oam_pdu_kt_pdu_t) - 1;
    oam_pdu_kt_pdu_t *pRspPdu = (oam_pdu_kt_pdu_t *)rsp;
    oam_if_t *intf = oam_intf_find(llid);

    if (!intf) {
        OAM_ORG_LOG("\r\noam_build_kt_resp at unknown port 0x%x\r\n", llid);
        return 0;
    }

    if(!frame ||
         length <= sizeof(oam_pdu_kt_pdu_t) + sizeof(oam_kt_var_cont_t) - 1)
            return 0;

    memset(rsp, 0, sizeof(rsp));

    memcpy(rsp, frame, sizeof(oam_pdu_kt_pdu_t)-1+sizeof(oam_kt_var_cont_t));

    pRecvVarCont = (oam_kt_var_cont_t *)pdu->data;
    pRecvVarCont->leaf = ntohs(pRecvVarCont->leaf);

    len += sizeof(oam_kt_var_cont_t);
    pRspVarCont = (oam_kt_var_cont_t *)pRspPdu->data;
    switch(pRecvVarCont->extBranch)  {
        case OAM_KT_ATTRIB_BRANCH:
            switch(pRecvVarCont->leaf) {
                case OAM_KT_LEAF_CODE_ONU_OPM_ALARM:
                    {
                        oam_kt_onu_opm_alarm_container_struct *pOpm = (oam_kt_onu_opm_alarm_container_struct *)pRspPdu->data;
                        pOpm->hdr.len = 1;
                        pOpm->value = g_opm_status;
                    }
                    break;
                case OAM_KT_LEAF_CODE_ONU_VOL_ALARM:
                    {
                        oam_kt_onu_vol_alarm_container_struct *pVol = (oam_kt_onu_vol_alarm_container_struct *)pRspPdu->data;
                        pVol->hdr.len = 1;
                        pVol->value = g_highvolt_status;
                    }
                    break;
                case OAM_KT_LEAF_CODE_ONU_DHCP_OPTION:
                {
                    oam_kt_onu_dhcp_option_container_struct *pDhcpIn = (oam_kt_onu_dhcp_option_container_struct *)pdu->data;
                    oam_kt_onu_dhcp_option_container_struct *pDhcpOut = (oam_kt_onu_dhcp_option_container_struct *)pRspPdu->data;
                    if(pDhcpIn->hdr.len == 0)
                    {
                        /*get*/
                        kt_prv_oam_dhcp_option_cfg_get_adapt(&(pDhcpOut->cfg));
                        pDhcpOut->hdr.len = 1;
                    }
                    else if(pDhcpIn->hdr.len == 1)
                    {
                        /*set*/
                        if(pDhcpIn->cfg != KT_DHCP_OPTION_DEACTIVATED && pDhcpIn->cfg != KT_DHCP_OPTION_ACTIVATED)
                        {
                            pDhcpOut->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
                            break;
                        }

                        if (OAM_E_OK != kt_prv_oam_dhcp_option_cfg_set_adapt(pDhcpIn->cfg )) {
                            pDhcpOut->hdr.len = OAM_ATTRIB_NOT_SUPPORTED;
                        }
                        else {
                            pDhcpOut->hdr.len = OAM_VAR_SET_OK;
                        }
                    }
                    else
                    {
                        return 0;
                    }
                    break;
                }
                case OAM_KT_LEAF_CODE_ONU_STATIC_MAC:
                    {
                        oam_kt_onu_static_mac_container_struct *pStaticMacIn = (oam_kt_onu_static_mac_container_struct *)pdu->data;
                        oam_kt_onu_static_mac_container_struct *pStaticMacOut = (oam_kt_onu_static_mac_container_struct *)pRspPdu->data;
                        unsigned char num = 0;
                        if(pStaticMacIn->hdr.len == 0)
                        {/*get*/
                            kt_prv_oam_static_mac_get_adapt((unsigned char *)pStaticMacOut->mac, &num);
                            pStaticMacOut->count = num;
                            pStaticMacOut->hdr.len = 1 + num*OAM_MACADDR_LEN;
                        }
                        else if(pStaticMacIn->hdr.len == 1)
                        {/*clear*/
                            kt_prv_oam_all_static_fdb_entries_del_adapt();
                            return 0;
                        }
                        else
                        {/*set*/
                            if(pStaticMacIn->count > KT_ORG_MAX_STATIC_MAC)
                                num = KT_ORG_MAX_STATIC_MAC;
                            else
                                num = pStaticMacIn->count;

                            kt_prv_oam_static_fdb_entries_add_adapt((unsigned char *)pStaticMacIn->mac, num);
                            return 0;
                        }
                    }
                    break;
                default:
                    OAM_ORG_LOG("\r\noam_build_kt_resp recv unknown leaf 0x%x\r\n",pRecvVarCont->leaf);
                    break;
            }
            break;
        default:
            OAM_ORG_LOG("\r\noam_build_kt_resp recv unknown branch 0x%x\r\n", pRecvVarCont->extBranch);
            break;
    }
    if((pRspVarCont->len & 0x80) != 0x80)
        len += pRspVarCont->len;

    kt_oam_send(OAM_PON_PORT_ID, llid, rsp, len);
    intf->org_specific_tx++;
    return len;
}

void
handle_kt_prv_oams(
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 len)
{
    oam_pdu_kt_t *pKTOamFrame = (oam_pdu_kt_t *)frame;

    if(!frame || !len)
        return;

    switch(pKTOamFrame->opcode) {
            case OAM_PDU_KT_VAR_REQ:
                    oam_build_kt_prv_var_resp(llid, frame, len);
                    break;
            case OAM_PDU_KT_SET_REQ:
                    /* set var on ONU */
                    oam_build_kt_prv_set_resp(llid, frame, len);
                    break;
            case OAM_KT_ATTRIB_BRANCH:
                    oam_build_kt_resp(llid, frame, len);
                    break;
            default:
                    break;
    }
    return;

}

void
kt_oam_send(
        oam_uint32 port,
        oam_llid_t llid,
        oam_uint8 *buf,
        oam_int32 len
        )
{
    oam_int32 result = OAM_E_OK;

    if(len < OAM_MIN_OAMPDU_SIZE - 4){
        len = OAM_MIN_OAMPDU_SIZE - 4;
    }

    oam_send(llid, OAM_PON_PORT_ID, buf, len);

    if (result != OAM_E_OK) {
        OAM_ORG_LOG("\r\nkt_oam_send: error 0x%x\r\n", result);
    }
}

void kt_prv_oam_init()
{
    init_onu_mac_filter_ctrl();
    init_per_port_storm_ctrl();
    uni_link_event_status_init();
}

#endif
