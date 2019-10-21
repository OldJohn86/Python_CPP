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

DEFINITIONS:  "DEVICE" means the Cortina Systems(TM) SDK product.
"You" or "CUSTOMER" means the entity or individual that uses the SOFTWARE.
"SOFTWARE" means the Cortina Systems(TM) SDK software.

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

#include <stdio.h>
#include <string.h>

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
#include "port.h"
#include "oam_port_stats.h"
#ifdef HAVE_CTC_OAM
#include "oam_ctc_pdu.h"
#include "oam_ctc_client.h"
#include "oam_ctc_adapt.h"
#include "oam_ctc_stats.h"
#endif


#ifdef HAVE_DPOE_OAM
#include "oam_dpoe_client.h"
#include "oam_dpoe_upg.h"
#endif

#ifdef HAVE_KT_OAM
#include "oam_kt_client.h"
#include "oam_kt_event.h"
#include "oam_kt_upg.h"
#include "oam_tk_upg.h"
#include "ca_event.h"
#endif


#include "rx.h"
#include "special_packet.h"
#include "scfg.h"


/* TODO:Find a solution for this global */
oam_port_id_t g_oam_port;
ca_scfg_pon_mac_mode_t g_epon_mac_mod = ONU_PON_MAC_EPON_1G;

static oam_llid_t g_oam_llid = 0;
static oam_ctrl_t g_oam_ctrl;
static oam_vendor_reg_event_handler_t g_oam_vendor_reg_handler = NULL;
static oam_vendor_reg_event_handler_t g_oam_vendor_dereg_handler = NULL;

static oam_uint32 port_polling_period_count = 0;

extern oam_ext_mode_t      oam_ext_mode;

static void oam_msg_proc(oam_int8 *msg, oam_uint32 len);
static void oam_pkt_proc(oam_msg_t *oam_msg);
static oam_status oam_pkt_parser(oam_uint8 *pkt);
static oam_status oam_laser_tx_ctrl_set(oam_uint8 *pRecv);

oam_status oam_vendor_reg_event_handler_register(
    oam_vendor_reg_event_handler_t   reg_handler,
    oam_vendor_reg_event_handler_t   dereg_handler)
{
    g_oam_vendor_reg_handler = reg_handler;
    g_oam_vendor_dereg_handler = dereg_handler;
    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_llid_reg_event_handler                                */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
static void oam_llid_reg_event_handler(
    oam_llid_t llid, oam_onu_reg_evt_t *reg_evt)
/*                                                                           */
/* INPUTS  : o llid - llid                                                   */
/*           o reg_evt - event info                                          */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* process the llid register event                                           */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    OAM_ASSERT(reg_evt != NULL);

    if (reg_evt->status) {
        OAM_LOG_MAJ("onu register, llid = %d, index %d\r\n", llid, reg_evt->index);
        oam_llid_registration(llid, reg_evt->index);

        if(g_oam_vendor_reg_handler != NULL)
        {
            g_oam_vendor_reg_handler(llid, reg_evt->index);
        }
        oam_stats_polling_enable(TRUE);
        __dpoe_oam_traffic_cfg(FALSE,reg_evt->index,llid);
    }
    else 
    {
        OAM_LOG_MAJ("onu deregister, llid = %d, index %d, dereg_cause %d\r\n", 
            llid, reg_evt->index, reg_evt->dereg_cause);
        oam_llid_deregistration(llid);

        if(g_oam_vendor_dereg_handler != NULL)
        {
            g_oam_vendor_dereg_handler(llid, reg_evt->index);
        }

        oam_stats_polling_enable(FALSE);
        __dpoe_oam_traffic_cfg(FALSE,reg_evt->index,llid);
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_link_complete                                         */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_link_complete(oam_if_t *intf)
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* call back for oam link discovery completed                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    if (intf == NULL)
        return;

    oam_dying_gasp_init_adapt(intf);
    oam_traffic_set_adapt(1);
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
#ifdef HAVE_LOOP_DETECT
    if (oam_ext_mode == OAM_EXT_MODE_KT)
        oam_enable_loop_detect_adapt(1);
#endif
#endif

#ifdef HAVE_KT_OAM
    if (oam_ext_mode == OAM_EXT_MODE_KT)
        oam_build_voltag_event_check();
#endif

#if 0
    len = oam_build_pdu_info(intf, NULL, (oam_pdu_info_t *)pdu);
    cs_pal_oam_keepalive_set(0, ONU_OAM_INTERVAL_NEW*10, pdu, len);
#endif
    /*oam_buf_print(pdu, len);*/
#if 0
    //oam_link_event_reset();
    //onu_pon_event_process(EPON_ONU_OAM_DISCOVERIED);
#endif
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_link_lost                                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_link_lost(oam_if_t *intf)
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* call back for oam link lost                                               */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_pon_led_set_adapt(0);

    if(intf->BEGIN == 0){
        oam_traffic_set_adapt(0);
    }
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
#ifdef HAVE_LOOP_DETECT
    if (oam_ext_mode == OAM_EXT_MODE_KT)
        oam_enable_loop_detect_adapt(0);
#endif
#endif

#if 0
    //onu_pon_event_process(EPON_ONU_OAM_RESTART);
#endif
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_init                                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
int oam_init()
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* oam init process                                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    int ret = 0;
    oam_ext_mode_t ext_mode = OAM_EXT_MODE_CTC;

    memset(&g_oam_ctrl, 0, sizeof(g_oam_ctrl));

    g_oam_ctrl.keep_alive_rate = OAM_KEEP_ALIVE_RATE_DFT;
    g_oam_ctrl.packet_max_rate = OAM_PACKET_MAX_RATE_DFT;

    if (sizeof(oam_msg_hdr_t) > OAM_MQ_MSG_HDR_LEN) {
        oam_printf("oam_msg_hdr_t is larger than %d\r\n", OAM_MQ_MSG_HDR_LEN);
        return OAM_E_ERROR;
    }

    ret = ca_mutex_init(&g_oam_ctrl.mutex, "oam", 0);

    if (ret) {
        oam_printf("Init oam mutex failed!\r\n");
        return OAM_E_ERROR;
    }

    ret = ca_queue_create(&g_oam_ctrl.mq_id,
                          OAM_MQ_NAME,
                          OAM_MQ_MSG_MAX_NUM,
                          OAM_MQ_MSG_LEN,
                          0);

    if (ret != 0) {
        oam_printf("Create ONU message queue failed!\r\n");
        return OAM_E_ERROR;
    }

    oam_onu_info_init_adapt();
    
    oam_core_init();

    oam_control_handler_init(oam_link_complete, oam_link_lost,
                             (oam_build_oampdu_t)oam_build_pdu_info);

    oam_vendor_register_handler_init();

    oam_timer_init();
    oam_event_init();

    ext_mode = oam_ext_mode_get_adapt();

    oam_epon_mac_mode_get_adapt( (ca_uint8*)&g_epon_mac_mod );

    oam_evt_reg_adapt();
    oam_pkt_trap_adapt();

    switch (ext_mode) {
#ifdef HAVE_CTC_OAM
        case OAM_EXT_MODE_CTC: 
        {
            oam_ctc_init();
            /* only permit base llid register */
            //oam_llid_admin_set_adapt(0, OAM_TRUE);
            break;
        }
#endif

#ifdef HAVE_DPOE_OAM
        case OAM_EXT_MODE_DPOE: 
        {
            oam_dpoe_init();
            break;
        }
#endif

#ifdef HAVE_KT_OAM
        case OAM_EXT_MODE_KT: 
        {       
            oam_kt_init();           
            break;
        }
#endif 
        default:
            break;
    }

    oam_port_stats_init();

    oam_llid_enable_init_adapt();
    
    oam_printf("OAM init done \n");

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_finish                                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_finish()
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* oam end process                                                           */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    ca_mutex_destroy(g_oam_ctrl.mutex);

    ca_queue_delete(g_oam_ctrl.mq_id);
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_entry                                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void *oam_entry(void *arg)
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* oam entry process                                                           */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_int8 buffer[OAM_MQ_MSG_LEN];
    oam_uint32 len = 0;
    oam_int32 ret = 0;

#ifndef __X86_DEMO__
    {
        unsigned int *sem = (unsigned int *) arg;
        ca_sem_post(*sem);
    }
#endif

    memset(buffer, 0, OAM_MQ_MSG_LEN);

    while (1) {
        ret = ca_queue_get(g_oam_ctrl.mq_id, buffer, OAM_MQ_MSG_LEN, &len,
                           OSAL_WAIT_FOREVER);

        if (0 == ret && len > 0) {
            ca_mutex_lock(g_oam_ctrl.mutex);

            oam_msg_proc(buffer, len);

            ca_mutex_unlock(g_oam_ctrl.mutex);
        }
    }
}


/*****************************************************************************/
/* $rtn_hdr_start  oam_orgspec_handle                                        */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_orgspec_handle(oam_uint8 *frame, oam_uint32 len)
/*                                                                           */
/* INPUTS  : o frame - oam packet                                            */
/*           o len - oam length                                              */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* process the org special oam                                               */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_pdu_orgspec_t *pdu = (oam_pdu_orgspec_t *) frame;
    OAM_ASSERT(frame != NULL);

    if (!oam_vendor_handler(pdu->oui,
                            OAM_VENDOR_HANDLE_ORG,
                            0, (oam_uint8 *)pdu, len)) {
        OAM_LOG_MAJ("received unknown OUI 0x%x 0x%x 0x%x\n", pdu->oui[0],
                    pdu->oui[1], pdu->oui[2]);
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_pkt_parser                                            */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_pkt_parser(oam_uint8 *pkt)
/*                                                                           */
/* INPUTS  : o pkt - oam packet                                              */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* process any oam packet                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_pdu_hdr_t *hdr = NULL;

    hdr = (oam_pdu_hdr_t *)pkt;

    if (ntohs(hdr->eth_type) == EPON_ETHERTYPE_SLOW) {
        return OAM_E_OK;
    }

    return OAM_E_OK;
    //return OAM_E_NOT_SUPPORT;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_pkt_proc                                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
static void oam_pkt_proc(oam_msg_t *oam_msg)
/*                                                                           */
/* INPUTS  : o oam_msg - oam message                                         */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* process any oam packet                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 *frame_ptr;
    oam_uint32 len;
    oam_port_id_t port;
    oam_uint8 llid_idx = oam_msg->hdr.llid;
    oam_llid_t llid;// = oam_msg->hdr.llid;

    if (oam_llid_find(llid_idx, &llid) == -1) {
        OAM_LOG_MAJ("llid not found src_sub_port_id:0x%x\n", llid_idx);
        return OAM_E_ERROR;
    }

    frame_ptr = oam_msg->data;
    len  = oam_msg->hdr.len;
    port = oam_msg->hdr.port;

    oam_dump_pkt("Rcv OAM", port, llid, frame_ptr, len);

    if (OAM_E_OK != oam_pkt_parser(frame_ptr))
        return;

    /* This is for reply the oam*/
    g_oam_port = port;
    g_oam_llid = llid;

    /* if the frame was not came from pon port
     * it maybe oob message, all oob message must be
     * follow the org oam format*/
    if (port == OAM_PON_PORT_ID)
    {
        if (llid == BROADCAST_LLID)
        {
            oam_laser_tx_ctrl_set(frame_ptr);
        }
        else
        {
            oam_ind_oami(llid, (oam_pdu_hdr_t *)frame_ptr, len);
        }
    }
    else
    {
        oam_orgspec_handle(frame_ptr, len);
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_laser_tx_ctrl_set                                     */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
static oam_status oam_laser_tx_ctrl_set(
    oam_uint8 *pRecv)
/*                                                                           */
/* INPUTS  : o pRecv - oam pdu buffer                                        */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* OAM laser tx control set                                                  */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
#ifdef HAVE_CTC_OAM
    oam_pdu_ctc_t *pdu = (oam_pdu_ctc_t *)pRecv;
    oam_ctc_onu_laser_control_t *pInData = (oam_ctc_onu_laser_control_t *)
                                           pdu->data;

    oam_ctc_onu_laser_control_old_t *p21Data = (oam_ctc_onu_laser_control_old_t *)
            pdu->data;


    if ((pdu->hdr.code == OAM_PDU_CODE_ORG_SPEC)
        && oam_is_oui_equal(oam_oui_ctc, pdu->oui)
        && (pdu->opcode == OAM_PDU_CTC_SET_REQ)
       ) {

        if ((pInData->hdr.branch == OAM_CTC_ATTRIB_BRANCH)
            && (ntohs(pInData->hdr.leaf) == OAM_CTC_LEAF_CODE_ONU_LASER_CONTROL)
           ) {
            /*CTC 2.1 oam format */
            if (pInData->hdr.width == (sizeof(oam_ctc_onu_laser_control_old_t) - sizeof(
                                           oam_var_cont_t))) {
                ctc_oam_onu_laser_control_set_adapt(ntohl(p21Data->action),
                                                    p21Data->mac_address,
                                                    ntohl(p21Data->transceiver_id));
            }
            /* CTC 3.0 format */
            else if (pInData->hdr.width == (sizeof(oam_ctc_onu_laser_control_t) - sizeof(
                                                oam_var_cont_t)) ) {
                ctc_oam_onu_laser_control_set_adapt(ntohs(pInData->action),
                                                    pInData->mac_address,
                                                    ntohl(pInData->transceiver_id));
            }
        }
    }

#endif

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_msg_proc                                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
static void oam_msg_proc(oam_int8 *msg, oam_uint32 len)
/*                                                                           */
/* INPUTS  : o msg - message buffer                                          */
/*           o len - message length                                          */
/* OUTPUTS : void                                                            */
/* DESCRIPTION:                                                              */
/* OAM message process                                                       */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status ret = OAM_E_OK;
    oam_msg_t *oam_msg = (oam_msg_t *)msg;
    oam_uint8 index;
    oam_timer_msg_t *timer_msg = (oam_timer_msg_t *)msg;
#ifdef HAVE_KT_OAM
    //ca_event_onu_fault_t event = {1, 0};
#endif

    OAM_ASSERT(msg != NULL);

    switch (oam_msg->hdr.type) {
        case OAM_PACKET:
            oam_pkt_proc(oam_msg);
            break;

        case OAM_LLID_REG_EVT: 
        {
            oam_onu_reg_evt_t *reg_evt = (oam_onu_reg_evt_t *)oam_msg->data;
            oam_llid_reg_event_handler(oam_msg->hdr.llid, reg_evt);
            break;
        }
        
        case OAM_KEEP_ALIVE_TIMER_EXPIRED:
            oam_time_msg_proc();
            break;

        case OAM_LINK_EVENT_TIMER_EXPIRED:
            oam_event_process_polling(NULL);
            break;

#ifdef HAVE_DPOE_OAM
        case OAM_KEY_EXCH_TIMER_EXPIRED:
        {
            index = timer_msg->data;
            oam_key_exch_timer_expire_handle(index);
            break;
        }
            
        case OAM_DPOE_OAM_UPG_END_EVT:
            dpoe_oam_upg_done_evt_handle(oam_msg->hdr.llid);
            break;
#endif

#ifdef HAVE_CTC_OAM

        case OAM_CTC_STATS_TIMER_EXPIRED:
            ctc_onu_stats_monitor_timer_handle(timer_msg->data);
            break;

        case OAM_KEEP_SILENCE_TIMER_EXPIRE: {
            ctc_oam_keep_silence_proc(timer_msg->data);
            break;
        }

#endif
        case OAM_KT_STATS_TIMER_EXPIRED:
        {
            port_polling_period_count++;
            
#if 0  // realtek switch CRC error event
            app_onu_crc_check(& event.alarm_id);
           
            if (event.alarm_id != 0){
                ret = (ca_status_t)ca_event_send(0, CA_EVENT_SWITCH_HANG, 
                    (void *)&event, sizeof(ca_event_onu_fault_t));
            }
#endif
            /*port stats read and clear in default 30s period */
            if (port_polling_period_count >= (OAM_STATS_MIB_POLL_ACCURARY/OAM_STATS_POLL_ACCURACY)){
                port_polling_period_count= 0;
                oam_port_stats_polling();
                //app_port_crc_stats_clear(RTL_ALL_PORTS_ID);
            }
            break;
        }

#ifdef HAVE_KT_OAM
        case OAM_PORT_LINK_EVT: {
            oam_onu_port_link_evt_t *link_evt = (oam_onu_port_link_evt_t *)oam_msg->data;
            extern oam_status oam_uni_link_event_handler(oam_uint16 uni_port, ca_uint8 status);
            oam_uni_link_event_handler(link_evt->port, link_evt->status);
            extern void oam_stats_uni_link_event_handle(oam_port_id_t port, oam_uint8 status);
            oam_stats_uni_link_event_handle(link_evt->port, link_evt->status);
            break;
        }

        
        case OAM_KT_IMG_UPG_EVT:
        {
            oam_kt_upg_evt_handler(msg);
            break;
        }

        case OAM_TK_UPG_DONE_EVT:
        {
            oam_tk_upg_done_evt_handle(msg);
            break;
        }
#endif
        default:
            OAM_ERROR("Invalid oam message type %d\r\n", oam_msg->hdr.type);
            break;
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_pkt_msg_send                                          */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_pkt_msg_send(
    oam_port_id_t   port,
    oam_uint8      llid_idx,
    oam_uint8       *pkt,
    oam_uint32      len)
/*                                                                           */
/* INPUTS  : o port - oam port                                               */
/*           o llid - llid                                                   */
/*           o pkt  - packet buffer                                          */
/*           o len  - packet length                                          */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* OAM packet message send                                                   */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_msg_t oam_msg;

    OAM_ASSERT_RET(pkt != NULL, OAM_E_ERROR);

    memset(&oam_msg, 0, sizeof(oam_msg));

    if (len > OAM_MQ_MSG_MAX_LEN)
        len = OAM_MQ_MSG_MAX_LEN;

    oam_msg.hdr.type = OAM_PACKET;
    oam_msg.hdr.llid = llid_idx;
    oam_msg.hdr.port = port;
    oam_msg.hdr.len  = len;

    if (len > 0)
        memcpy(&oam_msg.data[0], pkt, len);
    else {
        OAM_LOG_MAJ("Invalid len %d\r\n", len);
        return OAM_E_ERROR;
    }

    if (ca_queue_put(g_oam_ctrl.mq_id, &oam_msg, sizeof(oam_msg_hdr_t) + len, OSAL_NO_WAIT, 0) < 0)
    {
        OAM_LOG_MAJ("Send message failed.\r\n");
        return OAM_E_ERROR;
    }

    return OAM_E_OK;

}

/*****************************************************************************/
/* $rtn_hdr_start  oam_llid_reg_evt_msg_send                                 */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_llid_reg_evt_msg_send(
    oam_onu_reg_evt_t   *reg_evt)
/*                                                                           */
/* INPUTS  : o reg_evt - register event information                          */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* LLID register/deregister event message send                               */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_onu_reg_evt_msg_t reg_evt_msg;

    OAM_ASSERT_RET(reg_evt != NULL, OAM_E_ERROR);

    memset(&reg_evt_msg, 0, sizeof(reg_evt_msg));

    reg_evt_msg.hdr.type = OAM_LLID_REG_EVT;
    reg_evt_msg.hdr.llid = reg_evt->llid;
    reg_evt_msg.reg_evt  = *reg_evt;
    reg_evt_msg.hdr.len  = sizeof(oam_onu_reg_evt_t);;

    if (ca_queue_put(g_oam_ctrl.mq_id, &reg_evt_msg, sizeof(oam_onu_reg_evt_msg_t),
                     OSAL_NO_WAIT, 0) < 0) {
        OAM_LOG_MAJ("Send reg status message failed.\r\n");
        return OAM_E_ERROR;
    }

    return OAM_E_OK;

}

/*****************************************************************************/
/* $rtn_hdr_start  oam_port_link_evt_msg_send                                 */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_port_link_evt_msg_send(
    oam_onu_port_link_evt_t   *link_evt)
/*                                                                           */
/* INPUTS  : o reg_evt - register event information                          */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* LLID register/deregister event message send                               */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_onu_port_link_evt_msg_t link_evt_msg;

    OAM_ASSERT_RET(link_evt != NULL, OAM_E_ERROR);

    memset(&link_evt_msg, 0, sizeof(link_evt_msg));

    link_evt_msg.hdr.type = OAM_PORT_LINK_EVT;
    link_evt_msg.hdr.llid = 0;
    link_evt_msg.link_evt  = *link_evt;
    link_evt_msg.hdr.len  = sizeof(oam_onu_port_link_evt_t);;

    if (ca_queue_put(g_oam_ctrl.mq_id, &link_evt_msg, sizeof(oam_onu_port_link_evt_msg_t),
                     OSAL_NO_WAIT, 0) < 0) {
        OAM_LOG_MAJ("Send port link message failed.\r\n");
        return OAM_E_ERROR;
    }

    return OAM_E_OK;

}

/*****************************************************************************/
/* $rtn_hdr_start  oam_timer_msg_send                                        */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_timer_msg_send(oam_uint32 type, void *data)
/*                                                                           */
/* INPUTS  : o type - timer type                                             */
/*             data - timer data                                             */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* send oam timer message                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_timer_msg_t msg;

    memset(&msg, 0, sizeof(msg));

    msg.hdr.type = type;
    msg.data     = data;

    if (ca_queue_put          (g_oam_ctrl.mq_id, &msg, sizeof(msg), OSAL_NO_WAIT, 0) < 0) {
        OAM_LOG_MAJ("Send message failed.\r\n");
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status oam_img_upg_msg_send(oam_uint32 type, oam_uint32 data)
{
    oam_img_upg_msg_t msg;

    memset(&msg, 0, sizeof(msg));

    msg.hdr.type = type;
    msg.data     = data;

    if (ca_queue_put          (g_oam_ctrl.mq_id, &msg, sizeof(msg), OSAL_NO_WAIT, 0) < 0) {
        OAM_LOG_MAJ("Send message failed.\r\n");
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status oam_upg_done_evt_msg_send(
    oam_llid_t          llid)
{
    oam_msg_hdr_t hdr;

    memset(&hdr, 0, sizeof(oam_msg_hdr_t));

    hdr.type = OAM_DPOE_OAM_UPG_END_EVT;
    hdr.llid = llid;
    hdr.len  = 0;

    if (ca_queue_put(g_oam_ctrl.mq_id, &hdr, sizeof(oam_msg_hdr_t),
                     OSAL_NO_WAIT, 0) < 0) {
        OAM_LOG_MAJ("Send message failed.\r\n");
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

#ifdef HAVE_KT_OAM
oam_status oam_tk_upg_done_evt_send()
{
    oam_img_upg_msg_t upg_msg;

    memset(&upg_msg, 0, sizeof(oam_img_upg_msg_t));

    upg_msg.hdr.type = OAM_TK_UPG_DONE_EVT;

    if(ca_queue_put(g_oam_ctrl.mq_id, &upg_msg, sizeof(oam_msg_hdr_t), OSAL_NO_WAIT, 0) < 0) 
    {
        OAM_LOG_MAJ("Send message failed.\r\n");
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}
#endif    

/*****************************************************************************/
/* $rtn_hdr_start  oam_port_id_get                                           */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_port_id_t oam_port_id_get()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : o - oam port                                                    */
/* DESCRIPTION:                                                              */
/* get the current processing oam packet port                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    return g_oam_port;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_llid_get                                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_llid_t oam_llid_get()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : o - llid                                                        */
/* DESCRIPTION:                                                              */
/* get the current processing oam packet llid                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    return g_oam_llid;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_keep_alive_rate_set                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_keep_alive_rate_set(oam_uint8 rate)
/*                                                                           */
/* INPUTS  : o rate - oam keep alive rate(number of 100ms)                   */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* oam keep alive timer rate set                                             */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    if (rate > OAM_KEEP_ALIVE_RATE_MAX)
        return OAM_E_ERROR;

    /* update keep alive rate */

    g_oam_ctrl.keep_alive_rate = rate;

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_keep_alive_rate_get                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint8 oam_keep_alive_rate_get()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : keep alive rate                                                 */
/* DESCRIPTION:                                                              */
/* oam keep alive timer rate get(number of 100ms)                            */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    return g_oam_ctrl.keep_alive_rate;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_packet_max_rate_set                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_packet_max_rate_set(oam_uint8 rate)
/*                                                                           */
/* INPUTS  : o rate - oam packet max rate(number per 100ms)                  */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* oam packet max rate set                                                   */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    if (rate > OAM_PACKET_MAX_RATE_MAX)
        return OAM_E_ERROR;

    g_oam_ctrl.packet_max_rate = rate;

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_packet_max_rate_get                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint8 oam_packet_max_rate_get()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : packet max rate                                                 */
/* DESCRIPTION:                                                              */
/* oam packet max rate get(number per 100ms)                                 */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    return g_oam_ctrl.packet_max_rate;
}

