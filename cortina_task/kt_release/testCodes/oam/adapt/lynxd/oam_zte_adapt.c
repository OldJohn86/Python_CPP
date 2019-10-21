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
#include "oam_zte_pdu.h"
#include "oam_plat_if.h"
#include "oam_ctc_client.h"
#include "oam_zte.h"
#include "oam_zte_stats.h"
#include "oam_zte_adapt.h"
#include "sdl_api.h"
#include "sys_cfg.h"
#include "sdl_vlan.h"
#include "app_ip_api.h"
#include "db_instance.h"
#include "oam_ctc_adapt.h"
#ifdef HAVE_PPPOE
#include "pppoe_api.h"
#include "pppoe_session.h"
#endif

#include "mc_api.h"

#define TAG2VID(tag)    ((tag)&0x0fff)
#define TAG2PRI(tag)    (((tag)&0xe000)>>13)

static oam_uint32 g_laser_tx_power_timer = 0;
static oam_uint16 ipoe_vlan = 0;

oam_status zte_port_mac_filter_syn_db(
    oam_port_id_t port
);
oam_status zte_port_mac_bind_syn_db(
    oam_port_id_t port
);

oam_status zte_port_static_mac_syn_db(
    oam_port_id_t port
);




static void __laser_tx_power_on(void *data)
{
    cs_callback_context_t   context;
    epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, 1);
    epon_request_onu_pon_trans_rx_pwr_set(context, 0, 0, 1);
    g_laser_tx_power_timer = 0;
}

oam_boolean isZTE_F500_board()
{
    oam_uint8 vendorId[OAM_CTC_VENDER_ID_LEN];
    oam_uint8 model_name[OAM_CTC_MODEL_ID_LEN];

    startup_config_read(CFG_ID_VENDOR_ID, OAM_CTC_VENDER_ID_LEN, vendorId);
    startup_config_read(CFG_ID_ONU_MOD_ID, OAM_CTC_MODEL_ID_LEN, model_name);

    if ( (strncmp(model_name, "F500", 4) == 0) && (strcmp(vendorId, "ZTE") == 0))
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
    sdl_event_port_link_t *msg = (sdl_event_port_link_t *)data;
    oam_port_id_t port = 0;
    cs_sdl_port_link_status_t opr = SDL_PORT_LINK_STATUS_DOWN;

    if (msg == NULL)
        return;

    port = msg->port;
    opr = msg->link;

    OAM_ORG_DEBUG("port %d, link %d\n", port, opr);

    if (port > CTC_MAX_ONU_UNI_PORTS) {
        OAM_ORG_LOG("oam_handler_port_link_status port %d error!\n", port);
        return;
    }

    if (port == CS_PON_PORT_ID)
        return;

    if (opr == SDL_PORT_LINK_STATUS_UP) {
        oam_build_zte_extend_alarm_raised(port, ZTE_OAM_LINK_UP);
    }
    else if (opr == SDL_PORT_LINK_STATUS_DOWN) {
        oam_build_zte_extend_alarm_raised(port, ZTE_OAM_LINK_DOWN);
        /*onu_loop_clear(port);*/
#ifdef HAVE_LOOP_DETECT
        loop_detect_port_oper_down_handler(port);
#endif
    }
    else {
        OAM_ORG_LOG("oam_handler_port_link_status unknown opr %d!\n", opr);
    }
}

oam_status zte_oam_onu_id_get_adapt(oam_uint8 *onu_id)
{
    cs_status ret = CS_E_OK;

    if (!onu_id) {
        return CS_E_ERROR;
    }

#if 0
    ret = startup_config_read(CFG_ID_MAC_ADDRESS, CS_MACADDR_LEN, onu_id);
#endif
    return ret;
}

oam_status zte_oam_vendor_id_get_adapt(oam_uint8 *vendor_id)
{
    cs_status ret = CS_E_OK;

    if (!vendor_id) {
        return CS_E_ERROR;
    }

#if 0
    ret = startup_config_read(CFG_ID_VENDOR_ID, OAM_CTC_VENDER_ID_LEN, vendor_id);
#endif

    return ret;
}

oam_status zte_oam_model_id_get_adapt(oam_int8 *model_id)
{
    cs_status ret = CS_E_OK;

    if (!model_id) {
        return CS_E_ERROR;
    }

#if 0
    ret = startup_config_read(CFG_ID_ONU_MOD_ID, OAM_CTC_MODEL_ID_LEN, model_id);
#endif

    return ret;
}

extern void onu_hw_version_get(oam_uint8 *hw_vstr, oam_int32 len);
oam_status zte_oam_hw_ver_get_adapt(oam_uint8 *hw_ver)
{
    cs_status ret = CS_E_OK;

    if (!hw_ver) {
        return CS_E_ERROR;
    }

#if 0
    ret = onu_hw_version_get(hw_ver, OAM_CTC_ONU_HW_LEN);
#endif

    return ret;
}

oam_status zte_oam_sw_ver_get_adapt(
    oam_uint8   *swVer)
{
    if (!swVer) {
        return CS_E_ERROR;
    }

    strcpy(swVer, ONU_SW_VERSION);
    /*#ifdef HAVE_MPORTS
        if(isZTE_F500_board())
        {
            strcpy(swVer, ONU_SW_VERSION_F500);
        }
    #endif*/

    return CS_E_OK;
}

oam_status zte_oam_fw_ver_get_adapt(oam_int8 *fw_ver, oam_uint8 *len)
{
    if (oam_is_cuc_oam()) {
        sprintf(fw_ver, "%s.%s.%s.%s",
                IROS_ONU_APP_VER_MAJOR,
                IROS_ONU_APP_VER_MINOR,
                IROS_ONU_APP_VER_REVISION,
                IROS_ONU_APP_VER_BUILD);
        *len = strlen(fw_ver);
    }
    else {
        fw_ver[0] = cs_atoi(IROS_ONU_APP_VER_MAJOR);
        fw_ver[1] = cs_atoi(IROS_ONU_APP_VER_MINOR);
        fw_ver[2] = cs_atoi(IROS_ONU_APP_VER_REVISION);
        fw_ver[3] = cs_atoi(IROS_ONU_APP_VER_BUILD);
        *len = sizeof(oam_uint8) * 4;
    }

    return CS_E_OK;
}

oam_status zte_oam_chip_id_get_adapt(oam_ctc_onu_chip_id_t *chip_id)
{
    oam_uint32 a, b, c;
    oam_uint16 version = 0;

    sscanf(ONU_CHIPSET_VENDOR_ID, "%2x%2x", &a, &b);
    chip_id->venderId[0] = a;
    chip_id->venderId[1] = b;

    /*sscanf(ONU_CHIPSET_MODE,"%2x%2x",&a,&b);
    chip_id->chipModel[0] = a;
    chip_id->chipModel[1] = b;*/

    onu_get_chip_version(&version);
    chip_id->chipModel[0] = version >> 8;
    chip_id->chipModel[1] = version & 0x00ff;

    sscanf(ONU_CHIPSET_VER, "%2x", &a);
    chip_id->Revision = a;

    sscanf(ONU_CHIPSET_MADE_DATE, "%2d", &a);
    sscanf(ONU_CHIPSET_MADE_DATE, "%*[^/]/%2d", &b);
    sscanf(ONU_CHIPSET_MADE_DATE, "%*[^/]/%*[^/]/%2d", &c);
    chip_id->Date[0] = a;
    chip_id->Date[1] = b;
    chip_id->Date[2] = c;
}

oam_status zte_oam_onu_stack_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t vlan)
{
    cs_callback_context_t context;
    cs_sdl_vlan_tag_t    def_vlan;
    cs_sdl_vlan_mode_t  vlan_mode = SDL_VLAN_MODE_STACKING;
    cs_status ret = CS_E_OK;
    db_zte_vlan_t vlan_cfg;


    def_vlan.vid = TAG2VID(vlan.tag);
    def_vlan.pri = TAG2PRI(vlan.tag);

    ret = epon_request_onu_vlan_set(context, 0, 0, port, def_vlan, vlan_mode, NULL,
                                    0);

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

    return ret;
}

oam_status zte_oam_onu_mdutrunk_vlan_set_adapt(
    oam_port_id_t port,
    oam_uint16 num,
    oam_ctc_onu_vlan_tag_t *vlan)
{

    return CS_E_OK;
}

oam_status zte_oam_onu_mdutrunk_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *trunk_num,
    oam_ctc_onu_vlan_tag_t *vlan)
{

    return CS_E_OK;
}

oam_status zte_oam_mac_limit_set_adapt(
    oam_port_id_t port,
    oam_uint16  num)
{

    cs_status rt = CS_E_OK;

    rt = ctc_oam_onu_mac_limit_set_adapt ( port,  num);

    return rt;
}

oam_status zte_oam_mac_limit_get_adapt(
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

oam_status zte_oam_mac_aging_set_adapt(
    oam_uint32  time)
{
    cs_status rt = CS_E_OK;

    rt = ctc_oam_eth_mac_aging_time_set_adapt (time);

    return rt;
}

oam_status zte_oam_mac_aging_get_adapt(
    oam_uint32  *time)
{
    cs_callback_context_t context;
    cs_status rt = CS_E_OK;

    rt = epon_request_onu_fdb_age_get(context, 0, 0, time);

    return rt;
}

oam_status zte_oam_port_mac_filter_add_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    cs_callback_context_t     context;
    oam_uint16                 i;
    cs_sdl_mac_tbl_entry_t    mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];
    cs_status                  ret = 0;

    if ((port > CS_UNI_PORT_ID4) || (port < CS_UNI_PORT_ID1) || (entry == NULL) ||
        (num > ZTE_MAX_ENTRIES_PER_CONTAINER)) {
        return CS_E_PARAM;
    }

    for (i = 0; i < num; i++) {
        mac_entry[i].vlanid   = entry->vlan;
        mac_entry[i].mac_type = SDL_SRC_MAC;
        memcpy(&(mac_entry[i].mac), entry->mac, sizeof(cs_mac_t));
        entry++;
    }

    ret = epon_request_onu_mac_filter_add_entry(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, num, mac_entry);
#ifdef NEED_SAVE

    if (ret == CS_E_OK) {
        zte_port_mac_filter_syn_db ( port);
    }

#endif
    return ret;

}


oam_status zte_oam_port_mac_filter_del_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    cs_callback_context_t     context;
    oam_uint16                        i;
    cs_sdl_mac_tbl_entry_t    mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];
    cs_status                          ret = 0;


    if ((port > CS_UNI_PORT_ID4) || (port < CS_UNI_PORT_ID1) || (entry == NULL) ||
        (num > ZTE_MAX_ENTRIES_PER_CONTAINER)) {
        return CS_E_PARAM;
    }

    for (i = 0; i < num; i++) {
        mac_entry[i].vlanid   = entry->vlan;
        mac_entry[i].mac_type = SDL_SRC_MAC;
        memcpy(&(mac_entry[i].mac), entry->mac, sizeof(cs_mac_t));

        entry++;
    }

    ret = epon_request_onu_mac_filter_del_entry(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, num, mac_entry);
#ifdef NEED_SAVE

    if (ret == CS_E_OK) {
        zte_port_mac_filter_syn_db ( port);
    }

#endif
    return ret;

}

oam_status zte_oam_port_mac_filter_clr_adapt(
    oam_port_id_t port)
{
    cs_callback_context_t     context;
    cs_status                ret = 0;


    if ((port > CS_UNI_PORT_ID4) || (port < CS_UNI_PORT_ID1)) {
        return CS_E_PARAM;
    }

    ret = epon_request_onu_mac_filter_clr_entry(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port);
#ifdef NEED_SAVE

    if (ret == CS_E_OK) {
        zte_port_mac_filter_syn_db ( port);
    }

#endif
    return ret;
}

oam_status zte_oam_port_mac_filter_get_adapt(
    oam_port_id_t port,
    oam_uint8 *num,
    zte_oam_onu_mac_entry_t  *entry)
{
    cs_callback_context_t     context;
    cs_status                 rc = CS_E_OK;
    oam_uint16                 i;
    oam_uint8                  entry_count = 0;
    cs_sdl_mac_tbl_entry_t    mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];


    if ((port > CS_UNI_PORT_ID4) || (port < CS_UNI_PORT_ID1) || (num == NULL) ||
        (entry == NULL)) {
        return CS_E_PARAM;
    }

    rc  = epon_request_onu_mac_filter_get_entry(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &entry_count, mac_entry);

    if ( rc ) {
        return rc;
    }

    for (i = 0; i < entry_count; i++) {
        entry->vlan = mac_entry[i].vlanid;

        memcpy(entry->mac, &(mac_entry[i].mac), sizeof(cs_mac_t));

        entry++;
    }

    *num = entry_count;

    return CS_E_OK;

}

oam_status zte_oam_port_mac_bind_add_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    cs_callback_context_t     context;
    oam_uint16                 i;
    cs_sdl_mac_tbl_entry_t    mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];
    cs_status                 ret = 0;


    if ((port > CS_UNI_PORT_ID4) || (port < CS_UNI_PORT_ID1) || (entry == NULL) ||
        (num > ZTE_MAX_ENTRIES_PER_CONTAINER)) {
        return CS_E_PARAM;
    }

    for (i = 0; i < num; i++) {
        mac_entry[i].vlanid   = entry->vlan;
        mac_entry[i].mac_type = SDL_SRC_MAC;
        memcpy(&(mac_entry[i].mac), entry->mac, sizeof(cs_mac_t));

        entry++;
    }

    ret = epon_request_onu_mac_bind_add_entry(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, num, mac_entry);
#ifdef NEED_SAVE

    if (ret == CS_E_OK ) {
        zte_port_mac_bind_syn_db ( port);
    }

#endif
    return  ret;

}

oam_status zte_oam_port_mac_bind_del_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    cs_callback_context_t     context;
    oam_uint16                       i;
    cs_sdl_mac_tbl_entry_t  mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];
    cs_status                      ret = 0;


    if ((port > CS_UNI_PORT_ID4) || (port < CS_UNI_PORT_ID1) || (entry == NULL) ||
        (num > ZTE_MAX_ENTRIES_PER_CONTAINER)) {
        return CS_E_PARAM;
    }

    for (i = 0; i < num; i++) {
        mac_entry[i].vlanid   = entry->vlan;
        mac_entry[i].mac_type = SDL_SRC_MAC;
        memcpy(&(mac_entry[i].mac), entry->mac, sizeof(cs_mac_t));

        entry++;
    }

    ret = epon_request_onu_mac_bind_del_entry(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, num, mac_entry);
#ifdef NEED_SAVE

    if (ret == CS_E_OK) {

        zte_port_mac_bind_syn_db ( port);

    }

#endif
    return ret;


}

oam_status zte_oam_port_mac_bind_clr_adapt(
    oam_port_id_t port)
{
    cs_callback_context_t     context;
    cs_status                 ret = 0;

    if ((port > CS_UNI_PORT_ID4) || (port < CS_UNI_PORT_ID1)) {
        return CS_E_PARAM;
    }

    ret = epon_request_onu_mac_bind_clr_entry(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port);
#ifdef NEED_SAVE

    if (ret == CS_E_OK) {
        zte_port_mac_bind_syn_db ( port);
    }

#endif
    return ret;
}

oam_status zte_oam_port_mac_bind_get_adapt(
    oam_port_id_t port,
    oam_uint8 *num,
    zte_oam_onu_mac_entry_t  *entry)
{
    cs_callback_context_t     context;
    cs_status                 rc = CS_E_OK;
    oam_uint16                 i;
    oam_uint8                  entry_count = 0;
    cs_sdl_mac_tbl_entry_t    mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];


    if ((port > CS_UNI_PORT_ID4) || (port < CS_UNI_PORT_ID1) || (num == NULL) ||
        (entry == NULL)) {
        return CS_E_PARAM;
    }

    rc  = epon_request_onu_mac_bind_get_entry(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &entry_count, mac_entry);

    if ( rc ) {
        return rc;
    }

    for (i = 0; i < entry_count; i++) {
        entry->vlan = mac_entry[i].vlanid;

        memcpy(entry->mac, &(mac_entry[i].mac), sizeof(cs_mac_t));

        entry++;
    }

    *num = entry_count;

    return CS_E_OK;

}

oam_status zte_oam_port_mac_static_add_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    cs_callback_context_t context;
    cs_sdl_fdb_entry_t    sdl_entry;
    oam_uint8 i;
    cs_status rt = CS_E_OK;

    for (i = 0; i < num; i++) {
        memset(&sdl_entry, 0, sizeof(cs_sdl_fdb_entry_t));
        memcpy(&sdl_entry.mac, entry->mac, sizeof(cs_mac_t));
        sdl_entry.port = port;
        sdl_entry.type = 1; /*static entry*/
        sdl_entry.vlan_id = entry->vlan;

        rt = epon_request_onu_fdb_entry_add(context, 0, 0, &sdl_entry);

        if (rt)
            return rt;

        entry++;
    }

#ifdef NEED_SAVE

    if ( port <= DB_ZTE_MAX_PORT
         && port > 0) {
        zte_port_static_mac_syn_db ( port);

    }

#endif

    return rt;
}

oam_status zte_oam_port_mac_static_del_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    cs_callback_context_t context;
    cs_mac_t    mac;
    oam_uint8 i;
    cs_status rt = CS_E_OK;

    for (i = 0; i < num; i++) {
        memset(&mac, 0, sizeof(cs_mac_t));
        memcpy(&mac, entry->mac, sizeof(cs_mac_t));
        rt = epon_request_onu_fdb_entry_del(context, 0, 0, &mac, entry->vlan);

        if (rt)
            return rt;

        entry++;
    }

#ifdef NEED_SAVE

    if ( port <= DB_ZTE_MAX_PORT
         && port > 0) {

        zte_port_static_mac_syn_db ( port);

    }

#endif
    return rt;
}

oam_status zte_oam_port_mac_static_clr_adapt(
    oam_port_id_t port)
{
    cs_status rt = CS_E_OK;

    cs_callback_context_t context;

#ifdef HAVE_MPORTS
    rt = epon_request_onu_fdb_entry_clr_per_port(context, 0, 0, SDL_FDB_CLR_STATIC,
            port);
#else
    rt = epon_request_onu_fdb_entry_clr(context, 0, 0, SDL_FDB_CLR_STATIC);
#endif
#ifdef NEED_SAVE

    if ( port <= DB_ZTE_MAX_PORT
         && port > 0) {

        zte_port_static_mac_syn_db ( port);

    }

#endif
    return rt;

}

oam_status zte_oam_port_mac_static_get_adapt(
    oam_port_id_t port,
    oam_uint8 *num,
    zte_oam_onu_mac_entry_t  *filter)
{
    cs_status rt = CS_E_OK;
    cs_callback_context_t context;
    cs_sdl_fdb_entry_get_mode_t   mac_type = SDL_FDB_ENTRY_GET_MODE_STATIC;
    oam_uint16   offset = 0;
    oam_uint16   next;
    oam_uint16   number = 0;
    cs_sdl_fdb_entry_t    entry;

    while (1) {
        if (number >= ZTE_MAX_ENTRIES_PER_CONTAINER)
            break;

        memset(&entry, 0, sizeof(cs_sdl_fdb_entry_t));
        rt = epon_request_onu_fdb_entry_getnext_per_port(context, 0, 0, port, mac_type,
                offset, &next, &entry);

        if (rt)
            break;

        filter->vlan = entry.vlan_id;
        memcpy(&filter->mac, &entry.mac, sizeof(cs_mac_t));

        offset = next;
        number++;
        filter++;
    }

    *num = number;

    return CS_E_OK;
}

oam_status zte_oam_pon_dn_shaping_set_adapt(
    zte_oam_onu_rate_t *cfg)
{
    cs_callback_context_t context;
    oam_port_id_t portid = CS_DOWNLINK_PORT;
    cs_sdl_policy_t rate;
#ifdef NEED_SAVE
    db_zte_onu_svc_t   svc;
#endif
    cs_status rt = CS_E_OK;

    rate.enable = cfg->enable;
    rate.cbs = cfg->cbs;
    rate.ebs = 0;
    rate.rate = cfg->cir;
    rt = epon_request_onu_port_ds_rate_limit_set(context, 0, 0, portid, &rate);
#ifdef NEED_SAVE

    if (rt == CS_E_OK ) {
        memset(&svc, 0, sizeof(svc ) );
        db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
        svc.ds_shape_en = cfg->enable;
        svc.ds_shape_rate = cfg->cir;
        svc.ds_cbs = cfg->cbs ;
        db_write(DB_ZTE_ONU_SVC_INFO_ID , (oam_uint8 *) &svc, sizeof(svc));
    }

#endif
    return rt;
}

oam_status zte_oam_pon_dn_shaping_get_adapt(
    zte_oam_onu_rate_t *cfg)
{
    cs_callback_context_t context;
    oam_port_id_t portid = CS_DOWNLINK_PORT;
    cs_sdl_policy_t rate;
    cs_status rt = CS_E_OK;

    rt = epon_request_onu_port_ds_rate_limit_get(context, 0, 0, portid, &rate);

    if (rt)
        return rt;

    cfg->enable = rate.enable;
    cfg->cir = rate.rate;
    cfg->cbs = rate.cbs;


    return rt;
}

oam_status zte_port_statistics_get(
    oam_port_id_t port,
    oam_zte_onu_port_stats *stats)
{
    oam_port_uni_stats_t uni_stats;

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

    return CS_E_OK;
}

oam_status zte_oam_pon_port_stat_get_adapt(
    oam_zte_onu_llid_stats1 *stats1,
    oam_zte_onu_llid_stats2 *stats2)
{
    return zte_pon_port_stat_get(stats1, stats2);
}

oam_status zte_oam_stats_reset_uni_adapt(oam_port_id_t port)
{
    return app_stats_reset_uni(port);
}

oam_status zte_oam_port_isolation_set_adapt(
    oam_uint8  enable)
{
    cs_status ret;
    cs_callback_context_t context;
#ifdef NEED_SAVE
    db_zte_onu_svc_t   svc;
#endif
    ret = epon_request_onu_port_isolation_set(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, enable);
#ifdef NEED_SAVE

    if ( ret == CS_E_OK ) {
        memset(&svc, 0x00, sizeof(svc));
        db_read(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
        svc.port_iso = enable;
        db_write(DB_ZTE_ONU_SVC_INFO_ID, (oam_uint8 *) &svc, sizeof(svc));
    }

#endif
    return ret;
}

oam_status zte_oam_port_isolation_get_adapt(
    oam_uint8  *enable)
{
    cs_status ret;
    cs_callback_context_t context;

    ret = epon_request_onu_port_isolation_get(context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, enable);

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
    cs_status rt = CS_E_OK;

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
    cs_callback_context_t  context;
    oam_port_id_t port_id;
    cs_status ret = CS_E_OK;
    oam_uint32 que_thre;
#ifdef NEED_SAVE
    db_zte_voq_t  db_voq;
#endif

    switch (direction) {
        case ZTE_OAM_BUFFER_UP_STREAM:
            port_id = CS_UNI_PORT_ID1;
            break;

        case ZTE_OAM_BUFFER_DOWN_STREAM:
            port_id = CS_PON_PORT_ID;
            break;

        default:
            ret = CS_E_PARAM;
            goto end;
    }

    que_thre = threshold / 1024;

    ret = epon_request_onu_que_size_set(context, 0, 0, port_id, queue, que_thre);
#ifdef NEED_SAVE

    if (ret == CS_E_OK ) {
        memset(&db_voq, 0x00, sizeof(db_voq));
        db_read(DB_ZTE_QOS_VOQ_ID, (oam_uint8 *) &db_voq, sizeof(db_voq));
        db_voq.voq_cfg[port_id][queue] = que_thre;
        db_write(DB_ZTE_QOS_VOQ_ID, (oam_uint8 *) &db_voq, sizeof(db_voq));

    }

#endif
end:
    return ret;
}

oam_status zte_oam_buf_mgmt_cap_get_adapt(
    zte_oam_onu_queue_buffer_t  *qbuf)
{
    cs_callback_context_t  context;
    cs_buf_abi_t  buf_cap;
    cs_status ret = CS_E_OK;

    if (NULL == qbuf) {
        ret = CS_E_PARAM;
        goto end;
    }

    qbuf->capability = 1;

    ret = epon_request_onu_port_buf_ability_get(context, 0, 0, CS_PON_PORT_ID,
            &buf_cap);

    if (ret != CS_E_OK) {
        goto end;
    }

    qbuf->max_us = buf_cap.max_buf_abi;
    qbuf->min_us = buf_cap.min_buf_abi;

    ret = epon_request_onu_port_buf_ability_get(context, 0, 0, CS_UNI_PORT_ID1,
            &buf_cap);

    if (ret != CS_E_OK) {
        goto end;
    }

    qbuf->max_ds = buf_cap.max_buf_abi;
    qbuf->min_ds = buf_cap.min_buf_abi;

end:
    return ret;
}

oam_status zte_oam_tx_laser_ctrl_set_adapt(
    oam_uint8 match,
    oam_uint8 *pon_mac,
    oam_uint8 action,
    oam_uint16  duration)
{
    cs_mac_t onu_mac;
    cs_callback_context_t     context;

    if (match > 1) {
        return CS_E_PARAM;
    }

    if ( (1 == match) && (NULL == pon_mac) ) {
        return CS_E_PARAM;
    }

    epon_request_onu_pon_mac_addr_get(context, 0, 0, &onu_mac);

    if ((0 == match) || ( (1 == match) && ((pon_mac[0] == onu_mac.addr[0])
                                           && (pon_mac[1] == onu_mac.addr[1])
                                           && (pon_mac[2] == onu_mac.addr[2])
                                           && (pon_mac[3] == onu_mac.addr[3])
                                           && (pon_mac[4] == onu_mac.addr[4])
                                           && (pon_mac[5] == onu_mac.addr[5])) )) {
        if (g_laser_tx_power_timer != 0) { /*stop previous timer */
            cs_timer_del(g_laser_tx_power_timer);
            g_laser_tx_power_timer = 0;
        }

        if (0 == action) { /* tx laser on */
            epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, 1);
        }
        else if (1 == action) {
            epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, 0);
        }
        else if (2 == action) {
            epon_request_onu_pon_trans_tx_pwr_set(context, 0, 0, 0);
            epon_request_onu_pon_trans_rx_pwr_set(context, 0, 0, 0);
        }
        else {
            return CS_E_PARAM;
        }

        if (duration > 0) {
            g_laser_tx_power_timer = cs_timer_add(duration * 1000, __laser_tx_power_on,
                                                  NULL);
        }
    }
    else {
        return CS_E_NOT_FOUND;
    }

    return CS_E_OK;
}

oam_status zte_oam_rougue_onu_excl_ability_set_adapt(
    oam_uint8 enable)
{
    return CS_E_OK;
}

oam_status zte_oam_rougue_onu_excl_ability_get_adapt(
    oam_uint8  *enable)
{
    return CS_E_OK;
}

oam_status zte_oam_hg_mac_pool_add(
    oam_uint8 entries,
    oam_uint8  *mac)
{
    return CS_E_OK;
}

oam_status zte_oam_hg_mac_pool_del(
    oam_uint8 entries,
    oam_uint8  *mac)
{

    return CS_E_OK;
}

oam_status zte_oam_hg_mac_pool_clr( )
{
    return CS_E_OK;
}

oam_status zte_oam_hg_mac_pool_get_adapt(
    oam_uint8 *entries,
    oam_uint8  *mac)
{
#if 0
    oam_uint32 i = 0;
    hg_port_config_t  *p_hg_config = NULL;
    oam_uint32 entry_count = 0;

    p_hg_config = &hg_node.home_gateway_config.cfg;

    *entries = 0;

    for (i = 0; i < HG_PORT_MAX_MAC_POOL_NUM; i ++) {
        if (hg_mac_is_zero( &p_hg_config->mac_pool[i][0]) != 0) {
            memcpy(mac + entry_count * CS_MACADDR_LEN, &p_hg_config->mac_pool[i][0],
                   CS_MACADDR_LEN);
            entry_count++;
        }
    }

    *entries = entry_count;
#endif
    return CS_E_OK;
}

oam_status zte_oam_hg_mgmt_vlan_set_adapt(
    oam_uint16  vlan)
{
    return CS_E_OK;
}

oam_status zte_oam_hg_mgmt_vlan_get_adapt(
    oam_uint16  *vlan)
{
    return CS_E_OK;
}

oam_status zte_oam_hg_report_state_set_adapt(
    oam_port_id_t portId,
    oam_uint8 *hg_mac,
    oam_uint8 state)
{
    return CS_E_OK;
}

oam_status zte_oam_hg_report_state_get_adapt(
    oam_port_id_t portId,
    oam_uint8 *hg_mac,
    oam_uint8 *state)
{
    return CS_E_OK;
}


oam_status zte_oam_port_attr_set_adapt(
    oam_port_id_t portId,
    zte_oam_port_mode_e  mode)
{
    cs_status ret;
    cs_sdl_port_speed_cfg_t cfg = 0;
    cs_callback_context_t context;
    db_zte_port_t   port_info;


    switch (mode) {
        case ZTE_OAM_PORT_MODE_AUTO:
            cfg = SDL_PORT_AUTO_10_100_1000;
            break;

        case ZTE_OAM_PORT_MODE_10M_HALF:
            cfg = SDL_PORT_10_HALF;
            break;

        case ZTE_OAM_PORT_MODE_10M_FULL:
            cfg = SDL_PORT_10_FULL;
            break;

        case ZTE_OAM_PORT_MODE_100M_HALF:
            cfg = SDL_PORT_100_HALF;
            break;

        case ZTE_OAM_PORT_MODE_100M_FULL:
            cfg = SDL_PORT_100_FULL;
            break;

        case ZTE_OAM_PORT_MODE_1000M_FULL:
            cfg = SDL_PORT_1000_FULL;
            break;

        default:
            return CS_E_PARAM;
    }

    ret = epon_request_onu_port_status_set(context,
                                           ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, portId, cfg);

    if (ret == CS_E_OK) {
        if (portId <= DB_ZTE_MAX_PORT && portId > 0) {
            memset(&port_info, 0x00, sizeof(port_info));
            db_read(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
            port_info.port[portId - 1 ].work_mode = cfg;
            db_write(DB_ZTE_PORT_INFO_ID, (oam_uint8 *) &port_info, sizeof(port_info));
        }
    }

    return ret;
}

oam_status zte_oam_port_attr_get_adapt(
    oam_port_id_t portId,
    zte_oam_port_mode_e  *mode)
{
    cs_status ret;
    cs_sdl_port_speed_cfg_t cfg = 0;
    cs_callback_context_t context;

    ret = epon_request_onu_port_status_get(context,
                                           ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, portId, &cfg);

    if (ret) {
        return ret;
    }

    switch (cfg) {
        case SDL_PORT_AUTO_10_100_1000:
        case SDL_PORT_AUTO_10_100:
            *mode = ZTE_OAM_PORT_MODE_AUTO;
            break;

        case SDL_PORT_10_HALF:
            *mode = ZTE_OAM_PORT_MODE_10M_HALF;
            break;

        case SDL_PORT_10_FULL:
            *mode = ZTE_OAM_PORT_MODE_10M_FULL;
            break;

        case SDL_PORT_100_HALF:
            *mode = ZTE_OAM_PORT_MODE_100M_HALF;
            break;

        case SDL_PORT_100_FULL:
            *mode = ZTE_OAM_PORT_MODE_100M_FULL;
            break;

        case SDL_PORT_1000_FULL:
            *mode = ZTE_OAM_PORT_MODE_1000M_FULL;
            break;

        default:
            break;
    }

    return ret;
}

oam_status zte_oam_port_work_attr_get_adapt(
    oam_port_id_t portId,
    zte_oam_port_mode_e  *mode)
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
        *mode = ZTE_OAM_PORT_MODE_UNKOWN;
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
                    *mode = ZTE_OAM_PORT_MODE_10M_FULL;
                }
                else {
                    *mode = ZTE_OAM_PORT_MODE_10M_HALF;
                }

                break;

            case SDL_PORT_SPEED_100:
                if (port_duplex == SDL_PORT_DUPLEX_FULL) {
                    *mode = ZTE_OAM_PORT_MODE_100M_FULL;
                }
                else {
                    *mode = ZTE_OAM_PORT_MODE_100M_HALF;
                }

                break;

            case SDL_PORT_SPEED_1000:
                *mode = ZTE_OAM_PORT_MODE_1000M_FULL;
                break;
        }

    }

end:
    return rc;
}
oam_status zte_oam_remote_conn_set_adapt(
    oam_zte_onu_remote_conn_t  *ipConfig)
{
    cs_status ret = CS_E_OK;
    db_zte_mgmt_t  mgmt_info;

    memset(&mgmt_info, 0x00, sizeof(mgmt_info));
    db_read(DB_ZTE_MGMT_INFO_ID, (oam_uint8 *) &mgmt_info, sizeof(mgmt_info));

    if (ipConfig->enable == 1) {
        ret = epon_request_onu_ip_config_set(
                  ipConfig->remote_ip,
                  ipConfig->mask,
                  ipConfig->gateway);
    }
    else {
        ret = epon_request_onu_ip_config_set(
                  mgmt_info.ip,
                  mgmt_info.mask,
                  0);
    }

    if (ret != CS_E_OK) {
        return ret;
    }

    ret = epon_request_onu_inband_ip_config_set(
              ipConfig->enable,
              ipConfig->priority,
              ipConfig->vlan,
              ipConfig->host_ip,
              ipConfig->host_mask);

    mgmt_info.inband_ip = ipConfig->remote_ip;
    mgmt_info.inband_mask = ipConfig->mask;
    mgmt_info.gw = ipConfig->gateway;
    mgmt_info.mgmt_net = ipConfig->host_ip;
    mgmt_info.mgmt_mask = ipConfig->host_mask;
    mgmt_info.inband_vlan = ipConfig->vlan;
    mgmt_info.inband_pri = ipConfig->priority;
    mgmt_info.inband_enable = ipConfig->enable;

    db_write(DB_ZTE_MGMT_INFO_ID, (oam_uint8 *) &mgmt_info, sizeof(mgmt_info));

    return ret;
}

oam_status zte_oam_remote_conn_get_adapt(
    oam_zte_onu_remote_conn_t  *ipConfig)
{
    cs_status ret = CS_E_OK;
    db_zte_mgmt_t  mgmt_info;

    memset(&mgmt_info, 0x00, sizeof(mgmt_info));
    db_read(DB_ZTE_MGMT_INFO_ID, (oam_uint8 *) &mgmt_info, sizeof(mgmt_info));

    ipConfig->remote_ip =  mgmt_info.inband_ip;
    ipConfig->mask = mgmt_info.inband_mask;
    ipConfig->gateway = mgmt_info.gw ;

    ret = epon_request_onu_inband_ip_config_get(
              &ipConfig->enable,
              &ipConfig->priority,
              &ipConfig->vlan,
              &ipConfig->host_ip,
              &ipConfig->host_mask);
    return ret;
}

oam_status zte_oam_onu_config_save_adapt()
{
    db_save_to_flash();
    return CS_E_OK;
}

oam_status zte_oam_onu_config_clear_adapt()
{
    db_load_from_flash();
    return CS_E_OK;
}

oam_status zte_oam_factory_setting_restore_adapt()
{
    db_zte_set_default();
    db_save_to_flash();
    return CS_E_OK;
}

oam_status zte_prv_oam_onu_sn_get_adapt(
    oam_uint8  *onu_sn,
    oam_uint8  *sn_len)
{
    db_zte_sn_t sn;
    memset(&sn, 0x00, sizeof(sn));
    db_read(DB_ZTE_SN_ID, (oam_uint8 *) &sn, sizeof(sn));
    memcpy(onu_sn, sn.sn, strlen(sn.sn));
    *sn_len = strlen(sn.sn);
    return CS_E_OK;
}

oam_status zte_prv_oam_typec_config_adapt(
    oam_uint8  k1,
    oam_uint8  k2)
{
    return CS_E_OK;
}

oam_status zte_prv_oam_typec_status_get_adapt(
    oam_uint8  *k1,
    oam_uint8  *k2)
{
    return CS_E_OK;
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
    cs_status ret = CS_E_OK;

    ret = epon_request_onu_pppoe_test_start(context, 0, 0, port, vlan);

    if (!ret) {
        ret = epon_request_onu_pppoe_auth_set(context, 0, 0, mode, name, pwd);
    }

    return ret;

#else

    return CS_E_OK;

#endif
}

oam_status zte_oam_pppoe_simul_test_stop_adapt(
    oam_port_id_t port)
{
#ifdef HAVE_PPPOE
    cs_callback_context_t     context;
    return epon_request_onu_pppoe_test_stop(context, 0, 0, port);

#else

    return CS_E_OK;

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

    return CS_E_OK;
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
    cs_status ret = CS_E_OK;

    ipoe_vlan = vlan;
    ret = epon_request_onu_ipoe_test_start(vlan, mothed, name, pwd, mac, option60,
                                           timeout);
    return ret;
}

oam_status zte_oam_ipoe_simul_test_stop_adapt(
    oam_port_id_t port)
{
    return epon_request_onu_ipoe_test_stop();
}
oam_status zte_oam_ipoe_simul_result_get_adapt(
    oam_port_id_t port,
    oam_zte_onu_ipoe_state *state)
{

    ipoe_state_t test_rslt;

    memset(&test_rslt, 0, sizeof(ipoe_state_t));

    if (CS_E_OK != epon_request_onu_ipoe_test_result_get(&test_rslt)) {
        return CS_E_ERROR;
    }

    state->state = test_rslt.state;
    state->conclusion = test_rslt.conclusion;
    state->reason = test_rslt.reason;
    state->ip_addr = test_rslt.ip;
    state->gw_addr = test_rslt.gw;
    state->vlan_used = htonl(ipoe_vlan);
    state->ping_times = test_rslt.ping_times;
    state->ping_ack_timers = test_rslt.ping_ack_timers;
    state->min_delay = test_rslt.min_delay;
    state->max_delay = test_rslt.max_delay;
    state->avg_delay = test_rslt.avg_delay;

    return CS_E_OK;
}
/******ipoe end*********/

static oam_uint16 g_mac_pool_session_id[4] = {0xffff, 0xffff, 0xffff, 0xffff};
static oam_uint16 g_mac_pool_seq_no[4] = {0, 0, 0, 0};
static oam_uint16 g_mac_pool_next_start_index[4] = {1, 1, 1, 1};
extern cs_status mc_port_fdb_get(
    cs_dev_id_t                 device,
    oam_port_id_t                portid,
    oam_uint32                   offset,
    oam_uint16                   *next,
    oam_uint8                     *mac,
    oam_uint16                    *vlan_id);

oam_uint32 zte_oam_mac_pool_query_adapt(
    oam_zte_onu_port_mac_pool_hdr_t *inHeader,
    oam_zte_onu_port_mac_pool_hdr_t *outHeader,
    oam_zte_onu_port_mac_pool_block_t *block)
{
    oam_uint32 i = 0;
    oam_uint32 total = 0, tmpLen, len, start_index;
    oam_uint8 blockData[100][CS_MACADDR_LEN];
    oam_uint32 blockNum = 0;
    oam_uint16 attr;
    oam_uint8 mac[CS_MACADDR_LEN];
    oam_uint16 portId = 0;
    oam_uint16 sessionId = ntohs(inHeader->sessionId);
    cs_sdl_fdb_entry_t fdb_entry;
    cs_callback_context_t  context;
    cs_sdl_fdb_entry_get_mode_t mode = SDL_FDB_ENTRY_GET_MODE_ALL;
    oam_uint16 offset = 0;
    oam_uint16 out_index = 0;
    oam_uint16  vlan_id = 0;
    oam_boolean  unicast_search_flag = TRUE;
    cs_status ret = CS_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        outHeader->result  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    portId = index.idxValue;

    if (portId > 4) {
        OAM_ORG_LOG("port index is out of range \n");
        outHeader->result  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("Port id = %d,session id = %d \n", portId, sessionId);

    len = sizeof(oam_zte_onu_port_mac_pool_hdr_t);

    if (sessionId != g_mac_pool_session_id[portId - 1] ||
        g_mac_pool_session_id[portId - 1] == 0xffff) {
        /*restart new session */
        g_mac_pool_session_id[portId - 1] = sessionId; /*save the session id */
        outHeader->sessionId = htons(sessionId);
        outHeader->seqNo = htons(0);
        g_mac_pool_seq_no[ portId - 1] = 0;
        g_mac_pool_next_start_index[portId - 1 ] = 1;
    }
    else { /*continue session*/
        outHeader->sessionId = htons(sessionId);
        g_mac_pool_seq_no[ portId - 1]++;
        outHeader->seqNo = htons(g_mac_pool_seq_no[ portId - 1]);
    }

    memset(&fdb_entry, 0x00, sizeof(cs_sdl_fdb_entry_t) );

    i = 0;
    start_index = 0;

    do {
        if (unicast_search_flag == TRUE) {
#ifdef HAVE_MPORTS
            ret = epon_request_onu_fdb_entry_getnext_per_port(context, 0,  0,
                    index.idxValue, mode, offset,
                    &out_index,  &fdb_entry);
#else
            ret = epon_request_onu_fdb_entry_getnext(context, 0,  0, mode, offset,
                    &out_index, &fdb_entry);
#endif

            if (ret != CS_E_OK) {
                unicast_search_flag = FALSE;
                offset = 0;

            }
        }

        if (unicast_search_flag == FALSE) {
            /*unicast FDB has searched ,then search MC FDB*/
            ret = mc_port_fdb_get ( 0,   index.idxValue,  offset, &out_index, mac,
                                    &vlan_id);

            if (ret != CS_E_OK) {

                /*init again */
                memset (g_mac_pool_session_id, 0xff, sizeof(g_mac_pool_session_id ) );
                memset (g_mac_pool_seq_no, 0x0, sizeof (g_mac_pool_seq_no ) );
                memset ( g_mac_pool_next_start_index, 1,
                         sizeof (g_mac_pool_next_start_index ) );
                break;
            }

            fdb_entry.port = index.idxValue;
            fdb_entry.type = SDL_FDB_ENTRY_MC_TYPE;
            fdb_entry.vlan_id = vlan_id;
            memcpy (&fdb_entry.mac, mac, 6);
        }

        offset = out_index;

        if (fdb_entry.port != portId) {
            OAM_ORG_LOG_INFO("not this port \n");
            continue;
        }

        i ++;
        memcpy(mac, fdb_entry.mac.addr, CS_MACADDR_LEN);
        attr = 0;
        OAM_ORG_DEBUG("status = %d ,mac = %#x:%#x:%#x:%#x:%#x:%#x\n", fdb_entry.type,
                      mac[0], mac[1], mac[2],
                      mac[3], mac[4], mac[5]);

        switch (fdb_entry.type) {
            case SDL_FDB_ENTRY_DYNAMIC:
                attr |= ZTE_FDB_TYPE_DYNAMIC;
                break;

            case SDL_FDB_ENTRY_STATIC:
                attr |= ZTE_FDB_TYPE_STATIC;
                break;

            case  SDL_FDB_ENTRY_MC_TYPE:
                attr = ZTE_FDB_TYPE_MULTICAST;        /*We need use the mac to get the mac is multicast or not.*/
                break;

            default:
                continue;
        }

        OAM_ORG_DEBUG("attr = %d \n", attr);

        start_index ++;

        /*Continue the session, need skip the old one*/
        if (start_index < g_mac_pool_next_start_index[portId - 1]) {
            continue;
        }

        /*first block*/
        if (blockNum == 0) {
            OAM_ORG_DEBUG("first block \n");
            block->blockEntryNum = htonl(1);
            block->attr = attr ;

            blockNum = 1;
            memcpy(blockData[0], mac, CS_MACADDR_LEN);
        }
        else {
            if ((start_index - g_mac_pool_next_start_index[portId - 1]) >=
                ZTE_MAX_MAC_PER_SESSION) {
                OAM_ORG_DEBUG("reach the max block\n");
                g_mac_pool_next_start_index[portId - 1] = start_index;
                break;
            }

            if ((block->attr == attr )
                && (0 == memcmp(blockData[blockNum - 1], mac, CS_MACADDR_LEN))) {
                /*merge it to one entry*/
                OAM_ORG_DEBUG("same vlan id and same mac \n");
                continue;
            }
            else if (block->attr == attr ) {
                /*Merge it to one block*/
                OAM_ORG_DEBUG("same vlan id ,put in one block\n");
                memcpy(blockData[blockNum], mac, CS_MACADDR_LEN);
                blockNum ++;
            }
            else {
                /*Save data in old block*/
                OAM_ORG_DEBUG("blockNum = %d \n", blockNum);
                memcpy(block->data, blockData, blockNum * CS_MACADDR_LEN);
                block->blockEntryNum = htonl(blockNum);
                block->attr = htons(block->attr);
                total += blockNum;
                tmpLen = sizeof(oam_zte_onu_port_mac_pool_block_t) + blockNum * CS_MACADDR_LEN;
                len += tmpLen;
                OAM_ORG_DEBUG("new block \n");
                /*New block*/
                block = (oam_zte_onu_port_mac_pool_block_t *)(((oam_uint8 *)block) + tmpLen);
                blockNum = 1;
                block->blockEntryNum = htonl(blockNum);
                block->attr = attr;

                memcpy(blockData[0], mac, CS_MACADDR_LEN);
            }
        }
    }
    while (1);

    if (blockNum != 0 ) {
        OAM_ORG_DEBUG("Append it into last blocked \n");
        /*need append the last one */
        memcpy(block->data, blockData, blockNum * CS_MACADDR_LEN);
        block->blockEntryNum = htonl(blockNum);
        block->attr = htons(block->attr);
        total += blockNum;
        tmpLen = sizeof(oam_zte_onu_port_mac_pool_block_t) + blockNum * CS_MACADDR_LEN;
        len += tmpLen;
    }

    /*Reach the last entry, or get return the last one ,or no entry found*/
    if ( i == ZTE_MAX_ENTRY_TABLE || blockNum == 0 || ret != CS_E_OK) {
        OAM_ORG_LOG_INFO("reach the last entry.\n");
        OAM_ORG_LOG_INFO("i= %d, blockNum = %d, ret = 0x%x\n", i, blockNum, ret);
        outHeader->seqNo = htons(g_mac_pool_seq_no[ portId - 1] | 0x8000);
    }

    outHeader->totalEntryNum = htons(total);
    outHeader->len = htons(len - 4);
    outHeader->result = OAM_ZTE_VAR_SET_OK;

    return len;
}

oam_status zte_port_mac_filter_syn_db(
    oam_port_id_t port
)
{
    cs_callback_context_t     context;
    oam_uint32                 i = 0;
    db_zte_mac_bind_t   mac_filter;
    cs_sdl_mac_tbl_entry_t    macs[DB_ZTE_MAX_MAC_BIND_NUM];
    oam_uint8                   number = 0;
    db_zte_mac_bind_entry_t *mac_entry = NULL;
    cs_status                  ret = 0;

    ret =  epon_request_onu_mac_filter_get_entry (context,
            ONU_DEVICEID_FOR_API, ONU_LLIDPORT_FOR_API, port, &number, macs);

    if (ret != CS_E_OK) {

        return CS_E_ERROR;
    }

    memset(&mac_filter, 0x00, sizeof(db_zte_mac_bind_t));
    db_read(DB_ZTE_MAC_FILTER_ID, (oam_uint8 *) &mac_filter,
            sizeof(db_zte_mac_bind_t));

    mac_entry = &mac_filter.bind[port - 1];

    number = number > DB_ZTE_MAX_MAC_BIND_NUM ? DB_ZTE_MAX_MAC_BIND_NUM : number;
    mac_entry->num = number;

    for (i = 0; i < number; i++) {
        memcpy(mac_entry->mac[i].addr, macs[i].mac.addr, CS_MACADDR_LEN);
    }

    db_write(DB_ZTE_MAC_FILTER_ID, (oam_uint8 *) &mac_filter,
             sizeof(db_zte_mac_bind_t));

    return ret;

}

oam_status zte_port_mac_bind_syn_db(
    oam_port_id_t port
)
{
    cs_callback_context_t     context;
    oam_uint32                 i = 0;
    db_zte_mac_bind_t   mac_bind;
    cs_sdl_mac_tbl_entry_t    macs[DB_ZTE_MAX_MAC_BIND_NUM];
    oam_uint8                   number = 0;
    db_zte_mac_bind_entry_t *mac_entry = NULL;
    cs_status                  ret = 0;


    ret = epon_request_onu_mac_bind_get_entry ( context,
            ONU_DEVICEID_FOR_API,  ONU_LLIDPORT_FOR_API, port, &number, macs);


    if (ret != CS_E_OK) {

        return CS_E_ERROR;
    }

    memset(&mac_bind, 0x00, sizeof(mac_bind));
    db_read(DB_ZTE_MAC_BIND_ID , (oam_uint8 *) &mac_bind, sizeof(mac_bind));

    mac_entry = &mac_bind.bind[port - 1];

    number = number > DB_ZTE_MAX_MAC_BIND_NUM ? DB_ZTE_MAX_MAC_BIND_NUM : number;
    mac_entry->num = number;

    for (i = 0; i < number; i++) {
        memcpy(mac_entry->mac[i].addr, macs[i].mac.addr, CS_MACADDR_LEN);
    }

    db_write(DB_ZTE_MAC_BIND_ID , (oam_uint8 *) &mac_bind, sizeof(mac_bind));

    return ret;

}

oam_status zte_port_static_mac_syn_db(
    oam_port_id_t port
)
{
    oam_uint32                 i = 0;

    oam_uint8                   number = 0;
    db_zte_static_mac_t  mac_table;
    db_zte_mac_entry_t *mac_entry = NULL;
    zte_oam_onu_mac_entry_t zte_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];
    cs_status                  ret = 0;

    ret = zte_oam_port_mac_static_get_adapt ( port, &number,  zte_entry);

    if (ret != CS_E_OK) {
        return CS_E_ERROR;
    }

    memset(&mac_table, 0x00, sizeof(db_zte_static_mac_t));
    db_read(DB_ZTE_STATIC_MAC_ID , (oam_uint8 *) &mac_table,
            sizeof(db_zte_static_mac_t));

    mac_entry = &mac_table.entry[port - 1];

    number = number > DB_ZTE_MAX_MAC_BIND_NUM ? DB_ZTE_MAX_MAC_BIND_NUM : number;
    mac_entry->num = number;

    for (i = 0; i < number; i++) {
        memcpy(mac_entry->mac[i].addr, zte_entry[i].mac, CS_MACADDR_LEN);
        mac_entry->vlan = zte_entry[i].vlan;
    }

    db_write(DB_ZTE_STATIC_MAC_ID , (oam_uint8 *) &mac_table,
             sizeof(db_zte_static_mac_t));
    return ret;

}

oam_status zte_oam_onu_pon_mac_get_adapt(cs_mac_t *mac)
{
    cs_callback_context_t     context;

    return epon_request_onu_pon_mac_addr_get(context, ONU_DEVICEID_FOR_API,
            ONU_LLIDPORT_FOR_API, mac);
}

oam_status zte_oam_dry_contact_a_alarm_info_get_adapt(oam_uint64 *alarm_info)
{
    db_zte_backup_battery_t battery;

    memset(&battery, 0 , sizeof(db_zte_backup_battery_t));
    db_read(DB_ZTE_BACKUP_BATTERY_ID, (oam_uint8 *)&battery, sizeof(battery));

    *alarm_info = battery.battery[3].alarm_info;

    return CS_E_OK;
}

oam_status zte_oam_dry_contact_b_alarm_info_get_adapt(oam_uint64 *alarm_info)
{
    db_zte_backup_battery_t battery;

    memset(&battery, 0 , sizeof(db_zte_backup_battery_t));
    db_read(DB_ZTE_BACKUP_BATTERY_ID, (oam_uint8 *)&battery, sizeof(battery));

    *alarm_info = battery.battery[4].alarm_info;

    return CS_E_OK;
}

oam_status zte_oam_check_ip_address_adapt(oam_uint32 ip, oam_uint32 mask,
        oam_uint32 gw)
{
    return epon_request_onu_check_ip_address(ip, mask, gw);
}

oam_status zte_oam_db_alarm_restore_adapt()
{
    return db_alarm_restore();
}

oam_status zte_oam_db_port_alarm_restore_adapt()
{
    return db_port_alarm_restore();
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

