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
#include "oam_std_pdu.h"
#include "oam_zte_pdu.h"
#include "oam_ctc_client.h"
#include "oam_zte.h"
#include "oam_zte_stats.h"
#include "oam_zte_adapt.h"
#include "oam_ctc_adapt.h"
#ifdef HAVE_PPPOE
#include "pppoe_api.h"
#include "pppoe_session.h"
#endif

#define TAG2VID(tag)    ((tag)&0x0fff)
#define TAG2PRI(tag)    (((tag)&0xe000)>>13)

oam_status zte_port_mac_filter_syn_db(
    oam_port_id_t port
);
oam_status zte_port_mac_bind_syn_db(
    oam_port_id_t port
);

oam_status zte_port_static_mac_syn_db(
    oam_port_id_t port
);



oam_boolean isZTE_F500_board()
{
    oam_uint8 vendorId[OAM_CTC_VENDER_ID_LEN];
    oam_uint8 model_name[OAM_CTC_MODEL_ID_LEN];

    if ( (strncmp((char *)model_name, "F500", 4) == 0) &&
         (strcmp((char *)vendorId, "ZTE") == 0))
        return TRUE;
    else
        return FALSE;
}

void
oam_zte_handler_port_link_status(
    oam_uint16   evt_dev,
    oam_uint32   evt_type,
    void        *data)
{

}

oam_status zte_oam_onu_id_get_adapt(oam_uint8 *onu_id)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_vendor_id_get_adapt(oam_uint8 *vendor_id)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_model_id_get_adapt(oam_int8 *model_id)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

extern void onu_hw_version_get(oam_uint8 *hw_vstr, oam_int32 len);
oam_status zte_oam_hw_ver_get_adapt(oam_uint8 *hw_ver)
{
    oam_status ret = OAM_E_OK;

    if (!hw_ver) {
        return OAM_E_ERROR;
    }

#if 0
    ret = onu_hw_version_get(hw_ver, OAM_CTC_ONU_HW_LEN);
#endif

    return ret;
}

oam_status zte_oam_sw_ver_get_adapt(
    oam_uint8   *swVer)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_fw_ver_get_adapt(oam_int8 *fw_ver, oam_uint8 *len)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_chip_id_get_adapt(oam_ctc_onu_chip_id_t *chip_id)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_onu_stack_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_onu_mdutrunk_vlan_set_adapt(
    oam_port_id_t port,
    oam_uint16 num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_onu_mdutrunk_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *trunk_num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_mac_limit_set_adapt(
    oam_port_id_t port,
    oam_uint16  num)
{

    oam_status rt = OAM_E_OK;

    rt = ctc_oam_onu_mac_limit_set_adapt ( port,  num);

    return rt;
}

oam_status zte_oam_mac_limit_get_adapt(
    oam_port_id_t port,
    oam_uint16  *num)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_mac_aging_set_adapt(
    oam_uint32  time)
{
    oam_status rt = OAM_E_OK;

    rt = ctc_oam_eth_mac_aging_time_set_adapt (time);

    return rt;
}

oam_status zte_oam_mac_aging_get_adapt(
    oam_uint32  *time)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_filter_add_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status zte_oam_port_mac_filter_del_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_filter_clr_adapt(
    oam_port_id_t port)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_filter_get_adapt(
    oam_port_id_t port,
    oam_uint8 *num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_bind_add_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_bind_del_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_bind_clr_adapt(
    oam_port_id_t port)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_bind_get_adapt(
    oam_port_id_t port,
    oam_uint8 *num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_static_add_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_static_del_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_static_clr_adapt(
    oam_port_id_t port)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_mac_static_get_adapt(
    oam_port_id_t port,
    oam_uint8 *num,
    zte_oam_onu_mac_entry_t  *filter)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_pon_dn_shaping_set_adapt(
    zte_oam_onu_rate_t *cfg)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_pon_dn_shaping_get_adapt(
    zte_oam_onu_rate_t *cfg)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_port_statistics_get(
    oam_port_id_t port,
    oam_zte_onu_port_stats *stats)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_statistics_get_adapt(
    oam_port_id_t port,
    oam_zte_onu_port_stats *stats)
{
    return zte_port_statistics_get(port, stats);
}

oam_status zte_flux_stat_stop_adapt(
    oam_port_id_t port)
{
    return zte_flux_stat_stop(port);
}
oam_status zte_flux_stat_start_adapt(
    oam_port_id_t port,
    oam_uint32  interval,
    oam_uint32  duration)
{
    return zte_flux_stat_start(port, interval, duration);
}

oam_status zte_oam_port_stat_reset_adapt(oam_port_id_t port)
{
    return zte_port_stat_reset(port);
}

oam_status zte_oam_flux_stat_ctrl_get_adapt(
    oam_port_id_t port,
    oam_uint8 *action,
    oam_uint32 *interval,
    oam_uint32 *duration)
{
    return zte_flux_stat_ctrl_get(port, action, interval, duration);
}

oam_status zte_pon_port_stat_get(
    oam_zte_onu_llid_stats1 *stats1,
    oam_zte_onu_llid_stats2 *stats2)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_pon_port_stat_get_adapt(
    oam_zte_onu_llid_stats1 *stats1,
    oam_zte_onu_llid_stats2 *stats2)
{
    return zte_pon_port_stat_get(stats1, stats2);
}

oam_status zte_oam_stats_reset_uni_adapt(oam_port_id_t port)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_isolation_set_adapt(
    oam_uint8  enable)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_isolation_get_adapt(
    oam_uint8  *enable)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_flux_statistics_get_adapt(
    oam_port_id_t port,
    zte_oam_onu_port_flux_stats_t *stats)
{
    return zte_flux_statistics_get(port, stats);
}

#if 0
oam_status zte_oam_mac_addr_pool_get_adapt(
    oam_port_id_t port,
    oam_uint32 *num,
    cs_sdl_fdb_entry_t  *entry)
{
    oam_status rt = CS_E_OK;

    cs_callback_context_t  context;
    cs_sdl_fdb_entry_get_mode_t mode = SDL_FDB_ENTRY_GET_MODE_ALL;
    oam_uint16 offset = 0;
    oam_uint16 index = 0;
    oam_uint32 number = 0;

    while (1) {
        rt = epon_request_onu_fdb_entry_getnext_per_port(context, 0,  0,  port, mode,
                offset, &index,  entry);

        if (rt)
            break;

        offset = index;
        number++;
        entry++;
    }

    *num = number;

    return rt;
}
#endif

oam_status zte_oam_voq_buffer_set_adapt(
    oam_uint8 direction,
    oam_uint8  queue,
    oam_uint32 threshold)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_buf_mgmt_cap_get_adapt(
    zte_oam_onu_queue_buffer_t  *qbuf)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_tx_laser_ctrl_set_adapt(
    oam_uint8 match,
    oam_uint8 *pon_mac,
    oam_uint8 action,
    oam_uint16  duration)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_rougue_onu_excl_ability_set_adapt(
    oam_uint8 enable)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_rougue_onu_excl_ability_get_adapt(
    oam_uint8  *enable)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_hg_mac_pool_add(
    oam_uint8 entries,
    oam_uint8  *mac)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_hg_mac_pool_del(
    oam_uint8 entries,
    oam_uint8  *mac)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_hg_mac_pool_clr( )
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_hg_mac_pool_get_adapt(
    oam_uint8 *entries,
    oam_uint8  *mac)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_hg_mgmt_vlan_set_adapt(
    oam_uint16  vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_hg_mgmt_vlan_get_adapt(
    oam_uint16  *vlan)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_hg_report_state_set_adapt(
    oam_port_id_t portId,
    oam_uint8 *hg_mac,
    oam_uint8 state)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_hg_report_state_get_adapt(
    oam_port_id_t portId,
    oam_uint8 *hg_mac,
    oam_uint8 *state)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


oam_status zte_oam_port_attr_set_adapt(
    oam_port_id_t portId,
    zte_oam_port_mode_e  mode)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_attr_get_adapt(
    oam_port_id_t portId,
    zte_oam_port_mode_e  *mode)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_port_work_attr_get_adapt(
    oam_port_id_t portId,
    zte_oam_port_mode_e  *mode)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}
oam_status zte_oam_remote_conn_set_adapt(
    oam_zte_onu_remote_conn_t  *ipConfig)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_remote_conn_get_adapt(
    oam_zte_onu_remote_conn_t  *ipConfig)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_onu_config_save_adapt()
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_onu_config_clear_adapt()
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_factory_setting_restore_adapt()
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_prv_oam_onu_sn_get_adapt(
    oam_uint8  *onu_sn,
    oam_uint8  *sn_len)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_prv_oam_typec_config_adapt(
    oam_uint8  k1,
    oam_uint8  k2)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_prv_oam_typec_status_get_adapt(
    oam_uint8  *k1,
    oam_uint8  *k2)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

/******pppoe discovery*********/
oam_status zte_oam_pppoe_simul_test_start_adapt(
    oam_port_id_t port,
    oam_uint16 vlan,
    oam_uint8 mode,
    oam_int8 *name,
    oam_int8 *pwd)
{
#ifdef HAVE_PPPOE
    cs_callback_context_t     context;
    oam_status ret = CS_E_OK;

    ret = epon_request_onu_pppoe_test_start(context, 0, 0, port, vlan);

    if (!ret) {
        ret = epon_request_onu_pppoe_auth_set(context, 0, 0, mode, name, pwd);
    }

    return ret;

#else

    return OAM_E_OK;

#endif
}

oam_status zte_oam_pppoe_simul_test_stop_adapt(
    oam_port_id_t port)
{
#ifdef HAVE_PPPOE
    cs_callback_context_t     context;
    return epon_request_onu_pppoe_test_stop(context, 0, 0, port);

#else

    return OAM_E_OK;

#endif
}
oam_status zte_oam_pppoe_simul_result_get_adapt(
    oam_port_id_t port,
    oam_uint8 *state,
    oam_uint8 *reason)
{
#ifdef HAVE_PPPOE
    cs_callback_context_t     context;
    pppoe_test_result_t test_rslt;

    memset(&test_rslt, 0, sizeof(pppoe_test_result_t));

    if (CS_E_OK != epon_request_onu_pppoe_test_result_get(context, 0, 0, port,
            &test_rslt)) {
        return CS_E_ERROR;
    }

    *state = test_rslt.simul_rslt;
    *reason = test_rslt.reason;
#endif

    return OAM_E_OK;
}

/******ipoe start*********/
oam_status zte_oam_ipoe_simul_test_start_adapt(
    oam_port_id_t port,
    oam_uint16 vlan,
    oam_uint8  mothed,
    oam_int8  *name,
    oam_int8  *pwd,
    oam_int8   *mac,
    oam_uint8 *option60,
    oam_uint8  timeout)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_ipoe_simul_test_stop_adapt(
    oam_port_id_t port)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}
oam_status zte_oam_ipoe_simul_result_get_adapt(
    oam_port_id_t port,
    oam_zte_onu_ipoe_state *state)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}
/******ipoe end*********/

oam_uint32 zte_oam_mac_pool_query_adapt(
    oam_zte_onu_port_mac_pool_hdr_t *inHeader,
    oam_zte_onu_port_mac_pool_hdr_t *outHeader,
    oam_zte_onu_port_mac_pool_block_t *block)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_port_mac_filter_syn_db(
    oam_port_id_t port
)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_port_mac_bind_syn_db(
    oam_port_id_t port
)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_port_static_mac_syn_db(
    oam_port_id_t port
)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_onu_pon_mac_get_adapt(oam_uint8 *mac)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_dry_contact_a_alarm_info_get_adapt(oam_uint64 *alarm_info)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_dry_contact_b_alarm_info_get_adapt(oam_uint64 *alarm_info)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_check_ip_address_adapt(oam_uint32 ip, oam_uint32 mask,
        oam_uint32 gw)
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_db_alarm_restore_adapt()
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

oam_status zte_oam_db_port_alarm_restore_adapt()
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

#include <sys/time.h>
oam_uint32 system_tick = 100;
oam_uint32 tick_nsec = 10000;
oam_uint64 cs_current_time(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);

    /* This calculation is to get ticks on linux system 100 ticks - 1 sec */

    oam_uint32 cur_ticks = (now.tv_sec - 1185252800) * system_tick +
                           (now.tv_usec / tick_nsec);

    return (oam_uint64) cur_ticks;
}

