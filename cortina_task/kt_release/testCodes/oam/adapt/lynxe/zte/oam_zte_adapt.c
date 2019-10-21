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
#include <sys/time.h>

#ifdef ZTE_SPECIFIC

#include "oam_types.h"
#include "oam_cmn.h"
#include "oam_std_pdu.h"
#include "oam_zte_pdu.h"
#include "oam_ctc_client.h"
#include "oam_zte.h"
#include "oam_zte_stats.h"
#include "oam_zte_intf.h"
#include "oam_zte_adapt.h"
#include "oam_ctc_adapt.h"
#ifdef HAVE_PPPOE
#include "pppoe_api.h"
#endif
#include "dhcp_api.h"
#include "sal_epon_fdb.h"
#include "sal_l2.h"
#include "sal_port.h"
#include "sal_epon_vlan.h"
#include "sal_epon.h"
#include "sal_epon_qos.h"
#include "sdl_ext_port.h"
#include "db_zte.h"
#include "cs_ip_api.h"
#include "mc_api.h"

#include "cs_vlan_api.h"
#include "stats_api.h"
#include "swdlapp.h"
#include "cs_version.h"
#include "gpio_api.h"
#include "led.h"

/*Static global variables definition*/
static oam_uint32 system_tick = 100;
static oam_uint32 tick_nsec = 10000;
static oam_uint32 g_laser_tx_power_timer = 0;

static oam_uint16 g_mac_pool_session_id[4] = {0xffff, 0xffff, 0xffff, 0xffff};
static oam_uint16 g_mac_pool_seq_no[4] = {0, 0, 0, 0};
static oam_uint16 g_mac_pool_next_start_index[4] = {1, 1, 1, 1};

static void __laser_tx_power_on(void *data)
{
    cs_sal_epon_onu_pon_trans_tx_pwr_set(0, 0, 1);
    cs_sal_epon_onu_pon_trans_rx_pwr_set(0, 0, 1);
    
    led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_ON);
    g_laser_tx_power_timer = 0;
}

oam_uint64 cs_current_time(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);

    /* This calculation is to get ticks on linux system 100 ticks - 1 sec */

    oam_uint32 cur_ticks = (now.tv_sec - 1185252800) * system_tick +
                           (now.tv_usec / tick_nsec);

    return (oam_uint64) cur_ticks;
}

static oam_status zte_port_mac_filter_syn_db(
    oam_port_id_t port
)
{
    oam_uint32                 i = 0;
    db_zte_mac_bind_t   mac_filter;
    cs_sal_mac_entry_t    macs[DB_ZTE_MAX_MAC_BIND_NUM];
    oam_uint8                   number = 0;

    if (cs_sal_mac_entry_get(0, __SAL_ETH_PORT(port), &number, macs)) {
        OAM_ORG_LOG_INFO("cs_sal_mac_entry_get failed.\n");
        return OAM_E_ERROR;
    }

    OAM_ORG_LOG_INFO("mac filter number %d.\n", number);

    memset(&mac_filter, 0x00, sizeof(db_zte_mac_bind_t));

    number = number > DB_ZTE_MAX_MAC_BIND_NUM ? DB_ZTE_MAX_MAC_BIND_NUM : number;
    mac_filter.port_id = port;
    mac_filter.num = number;

    for (i = 0; i < number; i++) {
        memcpy(mac_filter.mac[i].addr, macs[i].mac, CS_MACADDR_LEN);
    }

    cs_db_entry_mod(TB_ZTE_MAC_FILTER, (void *) &mac_filter,
                    sizeof(db_zte_mac_bind_t), column_bit(TB_ZTE_MAC_FILTER, "ALL"));

    return OAM_E_OK;

}

static oam_status zte_port_mac_bind_syn_db(
    oam_port_id_t port
)
{
    oam_uint32                 i = 0;
    db_zte_mac_bind_t   mac_bind;
    cs_sal_mac_entry_t    macs[DB_ZTE_MAX_MAC_BIND_NUM];
    oam_uint8                   number = 0;
    cs_status                  ret = 0;


    if (cs_sal_mac_entry_get(0, __SAL_ETH_PORT(port), &number, macs)) {
        OAM_ORG_LOG_INFO("cs_sal_mac_entry_get failed.\n");
        return OAM_E_ERROR;
    }

    OAM_ORG_LOG_INFO("mac bind number %d.\n", number);

    memset(&mac_bind, 0x00, sizeof(mac_bind));
    number = number > DB_ZTE_MAX_MAC_BIND_NUM ? DB_ZTE_MAX_MAC_BIND_NUM : number;
    mac_bind.port_id = port;
    mac_bind.num = number;

    for (i = 0; i < number; i++) {
        memcpy(mac_bind.mac[i].addr, macs[i].mac, CS_MACADDR_LEN);
    }

    cs_db_entry_mod(TB_ZTE_MAC_BIND, (oam_uint8 *) &mac_bind, sizeof(mac_bind),
                    column_bit(TB_ZTE_MAC_BIND, "ALL"));

    return ret;

}


static oam_status zte_port_static_mac_syn_db(
    oam_port_id_t port
)
{
    oam_uint32                 i = 0;
    oam_uint16   offset = 0;
    oam_uint16   next = 0;
    oam_uint16   number = 0;
    db_zte_static_mac_t  mac_table;
    cs_sal_entry_fdb_t    entry[ZTE_MAX_ENTRIES_PER_CONTAINER];
    cs_status                  ret = 0;

    memset(entry, 0, sizeof(entry));

    while (1) {
        if (number >= ZTE_MAX_ENTRIES_PER_CONTAINER) {
            OAM_ORG_LOG_INFO("reach max static mac number: %d.\n", number);
            break;
        }

        if (cs_sal_fdb_entry_getnext(0, SAL_FDB_OP_MODE_STATIC, offset, &next,
                                     &entry[number])) {
            OAM_ORG_LOG_INFO("reach the last entry.\n");
            break;
        }

        offset = next;
        number++;
    }

    OAM_ORG_LOG_INFO("static mac number %d.\n", number);

    memset(&mac_table, 0x00, sizeof(db_zte_static_mac_t));
    number = number > DB_ZTE_MAX_MAC_BIND_NUM ? DB_ZTE_MAX_MAC_BIND_NUM : number;
    mac_table.port_id = port;
    mac_table.num = number;

    for (i = 0; i < number; i++) {
        memcpy(mac_table.mac[i].addr, entry[i].mac, CS_MACADDR_LEN);
        mac_table.vlan[i] = entry[i].vlan_id;
    }

    cs_db_entry_mod(TB_ZTE_STATIC_MAC , (oam_uint8 *) &mac_table,
                    sizeof(db_zte_static_mac_t), column_bit(TB_ZTE_STATIC_MAC, "ALL"));
    return ret;

}

oam_status zte_oam_onu_id_get_adapt(oam_uint8 *onu_id)
{
    if (!onu_id) {
        return OAM_E_ERROR;
    }

    return startup_cfg_read(CS_SCFG_ID_8A, CS_MACADDR_LEN, onu_id);
}

oam_status zte_oam_vendor_id_get_adapt(oam_uint8 *vendor_id)
{
    if (!vendor_id) {
        return OAM_E_ERROR;
    }

    strcpy((oam_int8 *)vendor_id, ONU_VENDOR_ID);
    
    return OAM_E_OK;
}

oam_status zte_oam_model_id_get_adapt(oam_uint8 *model_id)
{
    if (!model_id) {
        return OAM_E_ERROR;
    }

    strcpy((oam_int8 *)model_id, ONU_CLIENT_PRODUCT_MODEL);

    return OAM_E_OK;
}

oam_status zte_oam_hw_ver_get_adapt(oam_uint8 *hw_ver)
{
    if (!hw_ver) {
        return OAM_E_ERROR;
    }

    onu_hw_version_get(hw_ver);

    return OAM_E_OK;
}

oam_status zte_oam_sw_ver_get_adapt(
    oam_uint8   *swVer)
{
    tSwImgCfg    img_info;

    memset(&img_info, 0x00, sizeof(tSwImgCfg));
    cs_swdl_getswimginfo(CS_SWIMG_ID_0, &img_info);

    if (!img_info.isActiveB) {
        memset(&img_info, 0x00, sizeof(tSwImgCfg));
        cs_swdl_getswimginfo(CS_SWIMG_ID_1, &img_info);
    }

    memcpy(swVer, img_info.epon_version, OAM_CTC_ONU_SW_LEN);

    return OAM_E_OK;
}


oam_status zte_oam_fw_ver_get_adapt(oam_uint8 *fw_ver, oam_uint8 *len)
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

oam_status zte_oam_chip_id_get_adapt(oam_ctc_onu_chip_id_t *chip_id)
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


oam_status zte_oam_onu_stack_vlan_set_adapt(
    oam_port_id_t port,
    oam_ctc_onu_vlan_tag_t vlan)
{
    cs_sal_vlan_tag_t    def_vlan;
    app_vlan_tag_t app_dft_vlan;
    cs_sal_vlan_mode_t   vlan_mode = SAL_VLAN_MODE_STACKING;
#ifdef NEED_SAVE
    db_zte_vlan_t vlan_cfg;
#endif

    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);
    OAM_ORG_DEBUG("In [%s, %d] VLAN tag = %d\r\n",  __FUNCTION__, __LINE__,
                  vlan.tag);

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
#ifdef NEED_SAVE

    if (port <= DB_MAX_PORT
        && port > 0) {
        memset(&vlan_cfg, 0x00, sizeof(vlan_cfg));
        vlan_cfg.port_id = port;
        vlan_cfg.vlan_mode = vlan_mode;
        vlan_cfg.def_vlan.tpid = vlan.tpid;
        vlan_cfg.def_vlan.tag = vlan.tag;
        cs_db_entry_mod(TB_ZTE_VLAN, (oam_uint8 *) &vlan_cfg, sizeof(vlan_cfg),
                        column_bit(TB_ZTE_VLAN, "vlan_mode") |
                        column_bit(TB_ZTE_VLAN, "def_vlan"));
    }

#endif

    return OAM_E_OK;
}

oam_status zte_oam_onu_mdutrunk_vlan_set_adapt(
    oam_port_id_t port,
    oam_uint16 num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    return OAM_E_OK;
}

oam_status zte_oam_onu_mdutrunk_vlan_get_adapt(
    oam_port_id_t port,
    oam_uint16 *trunk_num,
    oam_ctc_onu_vlan_tag_t *vlan)
{
    return OAM_E_OK;
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
    oam_status rt = OAM_E_OK;

    rt = ctc_oam_onu_mac_limit_get_adapt( port,  num);

    return rt;
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
    oam_status rt = OAM_E_OK;

    rt = ctc_oam_eth_mac_aging_time_get_adapt (time);

    return rt;
}

oam_status zte_oam_port_mac_filter_add_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_uint16                 i;
    cs_sal_mac_entry_t        mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];

    if ( (entry == NULL) || (num > ZTE_MAX_ENTRIES_PER_CONTAINER)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_LOG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    for (i = 0; i < num; i++) {
        OAM_ORG_LOG("MAC Addr[%d]: %02x:%02x:%02x:%02x:%02x:%02x\r\n",  i,
                      entry[i].mac[0],
                      entry[i].mac[1],
                      entry[i].mac[2],
                      entry[i].mac[3],
                      entry[i].mac[4],
                      entry[i].mac[5]);
    }

    __PORT_CHECK(port);

    memset(mac_entry, 0, ZTE_MAX_ENTRIES_PER_CONTAINER * sizeof(
               cs_sal_mac_entry_t));

    for (i = 0; i < num; i++) {
        mac_entry[i].mode  = CS_SAL_MAC_ENTRY_MODE_FILTER;
        //mac_entry[i].vlanid   = entry->vlan;
        mac_entry[i].type = CS_SAL_MAC_TYPE_SRC;
        memcpy(mac_entry[i].mac, entry->mac, sizeof(cs_mac_addr_t));
        entry++;
    }

    if (cs_sal_mac_entry_add(0, __SAL_ETH_PORT(port), num, mac_entry)) {
        OAM_ORG_LOG("cs_sal_mac_entry_add return failed. \r\n");
        return OAM_E_ERROR;
    }

#ifdef NEED_SAVE
    zte_port_mac_filter_syn_db ( port);
#endif

    return OAM_E_OK;
}


oam_status zte_oam_port_mac_filter_del_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_uint16             i;
    cs_sal_mac_entry_t    mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];


    if ((entry == NULL) || (num > ZTE_MAX_ENTRIES_PER_CONTAINER)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("MAC Addr[%d]: %02x:%02x:%02x:%02x:%02x:%02x\r\n",  i,
                      entry[i].mac[0],
                      entry[i].mac[1],
                      entry[i].mac[2],
                      entry[i].mac[3],
                      entry[i].mac[4],
                      entry[i].mac[5]);
    }

    __PORT_CHECK(port);

    memset(mac_entry, 0, ZTE_MAX_ENTRIES_PER_CONTAINER * sizeof(
               cs_sal_mac_entry_t));

    for (i = 0; i < num; i++) {
        mac_entry[i].mode  = CS_SAL_MAC_ENTRY_MODE_FILTER;
        //mac_entry[i].vlanid   = entry->vlan;
        mac_entry[i].type = CS_SAL_MAC_TYPE_SRC;
        memcpy(&mac_entry[i].mac[0], entry->mac, sizeof(cs_mac_addr_t));

        entry++;
    }

    if (cs_sal_mac_entry_del(0, __SAL_ETH_PORT(port), num, mac_entry)) {
        return OAM_E_ERROR;
    }

#ifdef NEED_SAVE
    zte_port_mac_filter_syn_db ( port);
#endif

    return OAM_E_OK;
}

oam_status zte_oam_port_mac_filter_clr_adapt(
    oam_port_id_t port)
{
    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    __PORT_CHECK(port);

    if (cs_sal_mac_entry_clear(0, __SAL_ETH_PORT(port), CS_SAL_MAC_ENTRY_MODE_FILTER)) {
        return OAM_E_ERROR;
    }

#ifdef NEED_SAVE
    zte_port_mac_filter_syn_db ( port);
#endif

    return OAM_E_OK;
}

oam_status zte_oam_port_mac_filter_get_adapt(
    oam_port_id_t port,
    oam_uint8 *num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_uint16                 i;
    oam_uint8                  entry_count = 0;
    cs_sal_mac_entry_t        mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];

    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    if ((num == NULL) || (entry == NULL)) {
        return OAM_E_PARAM;
    }

    __PORT_CHECK(port);

    if (cs_sal_mac_entry_get(0, __SAL_ETH_PORT(port), &entry_count,
                             &mac_entry[0])) {
        return OAM_E_ERROR;
    }

    for (i = 0; i < entry_count; i++) {
        //entry->vlan = mac_entry[i].vlanid;
        //entry->mode = mac_entry[i].mode;
        memcpy(entry->mac, mac_entry[i].mac, sizeof(cs_mac_addr_t));

        entry++;
    }

    *num = entry_count;

    return OAM_E_OK;
}

oam_status zte_oam_port_mac_bind_add_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_uint16                 i;
    cs_sal_mac_entry_t        mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];

    if ( (entry == NULL) || (num > ZTE_MAX_ENTRIES_PER_CONTAINER)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("MAC Addr[%d]: %02x:%02x:%02x:%02x:%02x:%02x\r\n",  i,
                      entry[i].mac[0],
                      entry[i].mac[1],
                      entry[i].mac[2],
                      entry[i].mac[3],
                      entry[i].mac[4],
                      entry[i].mac[5]);
    }

    __PORT_CHECK(port);

    memset(mac_entry, 0, ZTE_MAX_ENTRIES_PER_CONTAINER * sizeof(
               cs_sal_mac_entry_t));

    for (i = 0; i < num; i++) {
        mac_entry[i].mode  = CS_SAL_MAC_ENTRY_MODE_FWD;
        //mac_entry[i].vlanid   = entry->vlan;
        mac_entry[i].type = CS_SAL_MAC_TYPE_SRC;
        memcpy(mac_entry[i].mac, entry->mac, sizeof(cs_mac_addr_t));
        entry++;
    }

    if (cs_sal_mac_entry_add(0, __SAL_ETH_PORT(port), num, mac_entry)) {
        return OAM_E_ERROR;
    }

#ifdef NEED_SAVE
    zte_port_mac_bind_syn_db ( port);
#endif

    return OAM_E_OK;

}

oam_status zte_oam_port_mac_bind_del_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_uint16             i;
    cs_sal_mac_entry_t    mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];


    if ((entry == NULL) || (num > ZTE_MAX_ENTRIES_PER_CONTAINER)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("MAC Addr[%d]: %02x:%02x:%02x:%02x:%02x:%02x\r\n",  i,
                      entry[i].mac[0],
                      entry[i].mac[1],
                      entry[i].mac[2],
                      entry[i].mac[3],
                      entry[i].mac[4],
                      entry[i].mac[5]);
    }

    __PORT_CHECK(port);

    memset(mac_entry, 0, ZTE_MAX_ENTRIES_PER_CONTAINER * sizeof(
               cs_sal_mac_entry_t));

    for (i = 0; i < num; i++) {
        mac_entry[i].mode  = CS_SAL_MAC_ENTRY_MODE_FWD;
        //mac_entry[i].vlanid   = entry->vlan;
        mac_entry[i].type = CS_SAL_MAC_TYPE_SRC;
        memcpy(mac_entry[i].mac, entry->mac, sizeof(cs_mac_addr_t));

        entry++;
    }

    if (cs_sal_mac_entry_del(0, __SAL_ETH_PORT(port), num, mac_entry)) {
        return OAM_E_ERROR;
    }

#ifdef NEED_SAVE
    zte_port_mac_bind_syn_db ( port);
#endif
    return OAM_E_OK;

}

oam_status zte_oam_port_mac_bind_clr_adapt(
    oam_port_id_t port)
{
    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    __PORT_CHECK(port);

    if (cs_sal_mac_entry_clear(0, __SAL_ETH_PORT(port), CS_SAL_MAC_ENTRY_MODE_FWD)) {
        return OAM_E_ERROR;
    }

#ifdef NEED_SAVE
    zte_port_mac_bind_syn_db ( port);
#endif

    return OAM_E_OK;
}

oam_status zte_oam_port_mac_bind_get_adapt(
    oam_port_id_t port,
    oam_uint8 *num,
    zte_oam_onu_mac_entry_t  *entry)
{
    oam_uint16                 i;
    oam_uint8                  entry_count = 0;
    cs_sal_mac_entry_t        mac_entry[ZTE_MAX_ENTRIES_PER_CONTAINER];


    if ((num == NULL) || (entry == NULL)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    __PORT_CHECK(port);

    if (cs_sal_mac_entry_get(0, __SAL_ETH_PORT(port), &entry_count,
                             &mac_entry[0])) {
        return OAM_E_ERROR;
    }

    for (i = 0; i < entry_count; i++) {
        //entry->vlan = mac_entry[i].vlanid;
        //entry->mode = mac_entry[i].mode;
        memcpy(entry->mac, mac_entry[i].mac, sizeof(cs_mac_addr_t));

        entry++;
    }

    *num = entry_count;

    return OAM_E_OK;
}

oam_status zte_oam_port_mac_static_add_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    cs_sal_entry_fdb_t    sdl_entry;
    oam_uint8 i;

    if (entry == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("MAC Addr[%d]: %02x:%02x:%02x:%02x:%02x:%02x\r\n",  i,
                      entry[i].mac[0],
                      entry[i].mac[1],
                      entry[i].mac[2],
                      entry[i].mac[3],
                      entry[i].mac[4],
                      entry[i].mac[5]);
        OAM_ORG_DEBUG("vlanid[%d]: %d\r\n",  i, entry->vlan);
    }

    __PORT_CHECK(port);

    for (i = 0; i < num; i++) {
        memset(&sdl_entry, 0, sizeof(cs_sal_entry_fdb_t));
        memcpy(sdl_entry.mac, entry->mac, sizeof(cs_mac_addr_t));
        sdl_entry.port = __SAL_ETH_PORT(port);
        sdl_entry.type = SAL_FDB_ENTRY_STATIC; /*static entry*/
        sdl_entry.vlan_id = entry->vlan;

        if (cs_sal_fdb_entry_add(0, &sdl_entry)) {
            return OAM_E_ERROR;
        }

        entry++;
    }

#ifdef NEED_SAVE
    zte_port_static_mac_syn_db ( port);
#endif

    return OAM_E_OK;

}

oam_status zte_oam_port_mac_static_del_adapt(
    oam_port_id_t port,
    oam_uint8 num,
    zte_oam_onu_mac_entry_t  *entry)
{
    cs_mac_addr_t    mac;
    oam_uint8 i;

    if (entry == NULL) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    for (i = 0; i < num; i++) {
        OAM_ORG_DEBUG("MAC Addr[%d]: %02x:%02x:%02x:%02x:%02x:%02x\r\n",  i,
                      entry[i].mac[0],
                      entry[i].mac[1],
                      entry[i].mac[2],
                      entry[i].mac[3],
                      entry[i].mac[4],
                      entry[i].mac[5]);
        OAM_ORG_DEBUG("vlanid[%d]: %d\r\n",  i, entry->vlan);
    }

    __PORT_CHECK(port);

    for (i = 0; i < num; i++) {
        memset(mac, 0, sizeof(cs_mac_addr_t));
        memcpy(mac, entry->mac, sizeof(cs_mac_addr_t));

        if (cs_sal_fdb_entry_del(0, &mac, entry->vlan)) {
            return OAM_E_ERROR;
        }

        entry++;
    }

#ifdef NEED_SAVE
    zte_port_static_mac_syn_db ( port);
#endif

    return OAM_E_OK;
}

oam_status zte_oam_port_mac_static_clr_adapt(
    oam_port_id_t port)
{
    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    __PORT_CHECK(port);

    if (cs_sal_fdb_entry_clr(0, SAL_FDB_OP_MODE_STATIC)) {
        return OAM_E_ERROR;
    }

#ifdef NEED_SAVE
    zte_port_static_mac_syn_db ( port);
#endif

    return OAM_E_OK;
}

oam_status zte_oam_port_mac_static_get_adapt(
    oam_port_id_t port,
    oam_uint8 *num,
    zte_oam_onu_mac_entry_t  *filter)
{
    oam_uint16   offset = 0;
    oam_uint16   next = 0;
    oam_uint16   number = 0;
    cs_sal_entry_fdb_t    entry;


    if ((filter == NULL) || (num == NULL)) {
        return OAM_E_PARAM;
    }

    OAM_ORG_DEBUG("In [%s, %d] port = %d\r\n",  __FUNCTION__, __LINE__, port);

    __PORT_CHECK(port);

    while (1) {
        if (number >= ZTE_MAX_ENTRIES_PER_CONTAINER)
            break;

        memset(&entry, 0, sizeof(cs_sal_entry_fdb_t));

        if (cs_sal_fdb_entry_getnext(0, SAL_FDB_OP_MODE_STATIC, offset, &next,
                                     &entry)) {
            break;
        }

        filter->vlan = entry.vlan_id;
        memcpy(filter->mac, entry.mac, sizeof(cs_mac_addr_t));

        offset = next;
        number++;
        filter++;
    }

    *num = number;

    return OAM_E_OK;
}

oam_status zte_oam_pon_dn_shaping_set_adapt(
    zte_oam_onu_rate_t *cfg)
{
    cs_port_id_t portid = CS_PORT_ETH0;
    cs_sdl_policy_t rate;
#ifdef NEED_SAVE
    cs_db_eth_port_t   eth_port;
#endif
    cs_status_e rt = CS_OK;

    rate.enable = cfg->enable;
    rate.cbs = cfg->cbs;
    rate.ebs = 0;
    rate.rate = cfg->cir;
    rt = cs_sal_epon_onu_port_ds_rate_limit_set(0, 0, portid, &rate);

    if (rt)
        return OAM_E_ERROR;

#ifdef NEED_SAVE
    memset(&eth_port, 0, sizeof(eth_port));
    eth_port.ds_rate_limit_enable = cfg->enable;
    eth_port.ds_rate_limit = cfg->cir;
    eth_port.ds_cbs = cfg->cbs ;
    cs_db_entry_mod(TB_ETH_PORT, (void *) &eth_port, sizeof(eth_port),
                    column_bit(TB_ETH_PORT, "ds_rate_limit_enable") |
                    column_bit(TB_ETH_PORT, "ds_rate_limit") |
                    column_bit(TB_ETH_PORT, "ds_cbs"));
#endif

    return OAM_E_OK;
}

oam_status zte_oam_pon_dn_shaping_get_adapt(
    zte_oam_onu_rate_t *cfg)
{
    cs_port_id_t portid = CS_PORT_ETH0;
    cs_sdl_policy_t rate;
    cs_status_e rt;

    rt = cs_sal_epon_onu_port_ds_rate_limit_get(0, 0, portid, &rate);

    if (rt)
        return OAM_E_ERROR;

    cfg->enable = rate.enable;
    cfg->cir = rate.rate;
    cfg->cbs = rate.cbs;


    return OAM_E_OK;
}

oam_status zte_port_statistics_get(
    oam_port_id_t port,
    oam_zte_onu_port_stats *stats)
{
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
        cs_sal_epon_onu_fec_cnt_get(0, 0, &fec_cnt);
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
    cs_stats_uni_reset(port);

    return OAM_E_OK;
}

oam_status zte_oam_port_isolation_set_adapt(
    oam_uint8  enable)
{
    cs_status_e ret;
#ifdef NEED_SAVE
    db_zte_onu_svc_t   svc;
#endif

    ret = cs_l2_port_isolation_set(0, enable);

    if (ret)
        return OAM_E_ERROR;

#ifdef NEED_SAVE
    memset(&svc, 0x00, sizeof(svc));
    svc.port_iso = enable;
    cs_db_entry_mod(TB_ZTE_ONU_SVC, (void *) &svc, sizeof(svc),
                    column_bit(TB_ZTE_ONU_SVC, "port_iso"));
#endif

    return OAM_E_OK;
}


oam_status zte_oam_port_isolation_get_adapt(
    oam_uint8  *enable)
{
    oam_status ret;

    ret = cs_l2_port_isolation_get(0, enable);

    if (ret)
        return OAM_E_ERROR;

    return OAM_E_OK;
}

oam_status zte_oam_port_flux_statistics_get_adapt(
    oam_port_id_t port,
    zte_oam_onu_port_flux_stats_t *stats)
{
    return zte_flux_statistics_get(port, stats);
}


oam_status zte_oam_voq_buffer_set_adapt(
    oam_uint8 direction,
    oam_uint8  queue,
    oam_uint32 threshold)
{
    cs_port_id_t port_id;
    cs_status_e ret = CS_OK;
    oam_uint32 que_thre;
#ifdef NEED_SAVE
    db_zte_voq_t  db_voq;
    cs_db_pon_port_t pon;
#endif

    switch (direction) {
        case ZTE_OAM_BUFFER_UP_STREAM:
            port_id = CS_PORT_SNI0;
            break;

        case ZTE_OAM_BUFFER_DOWN_STREAM:
            port_id = CS_PORT_ETH0;
            break;

        default:
            ret = CS_ERR_GLBL_INVALID_PARAMS;
            goto end;
    }

    que_thre = threshold / 1024;

    ret = cs_sal_epon_onu_que_size_set(0, 0, port_id, queue, que_thre);

    if (ret)
        return OAM_E_ERROR;

#ifdef NEED_SAVE
    if(direction == ZTE_OAM_BUFFER_UP_STREAM){
        memset(&pon, 0x00, sizeof(cs_db_pon_port_t));
        cs_db_entry_get(TB_PON_PORT, (void *) &db_voq, sizeof(db_voq));
        pon.voq_cfg[queue] = que_thre;
        cs_db_entry_mod(TB_PON_PORT, &pon, sizeof(pon),
                        column_bit(TB_PON_PORT, "voq_cfg"));
    }
    else{
        memset(&db_voq, 0x00, sizeof(db_voq));
        db_voq.port_id = direction;
        cs_db_entry_get(TB_ZTE_QOS_VOQ, (void *) &db_voq, sizeof(db_voq));
        db_voq.voq_cfg[queue] = que_thre;
        cs_db_entry_mod(TB_ZTE_QOS_VOQ, (void *) &db_voq, sizeof(db_voq),
                        column_bit(TB_ZTE_QOS_VOQ, "voq_cfg"));
    }
#endif

    return OAM_E_OK;

end:
    return ret;
}

oam_status zte_oam_buf_mgmt_cap_get_adapt(
    zte_oam_onu_queue_buffer_t  *qbuf)
{
    oam_status ret = OAM_E_OK;
    cs_buf_abi_t  buf_cap;

    if (NULL == qbuf) {
        ret = OAM_E_PARAM;
        goto end;
    }

    qbuf->capability = 1;

    ret = cs_sal_epon_onu_port_buf_ability_get(0, 0, CS_PORT_SNI0, &buf_cap);

    if (ret != CS_OK) {
        ret = OAM_E_PARAM;
        goto end;
    }

    qbuf->max_us = buf_cap.max_buf_abi;
    qbuf->min_us = buf_cap.min_buf_abi;

    ret = cs_sal_epon_onu_port_buf_ability_get(0, 0, CS_PORT_ETH0, &buf_cap);

    if (ret != CS_OK) {
        ret = OAM_E_PARAM;
        goto end;
    }

    qbuf->max_ds = buf_cap.max_buf_abi;
    qbuf->min_ds = buf_cap.min_buf_abi;

    return OAM_E_OK;

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
    cs_status_e ret = CS_OK;
    
    if (match > 1) {
        return OAM_E_PARAM;
    }

    if ( (1 == match) && (NULL == pon_mac) ) {
        return OAM_E_PARAM;
    }

    cs_sal_epon_onu_pon_mac_addr_get(0, 0, &onu_mac);

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
            ret = cs_sal_epon_onu_pon_trans_tx_pwr_set(0, 0, 1);
            led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_ON);
        }
        else if (1 == action) {
            ret = cs_sal_epon_onu_pon_trans_tx_pwr_set(0, 0, 0);
            led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_OFF);
        }
        else if (2 == action) {
            ret = cs_sal_epon_onu_pon_trans_tx_pwr_set(0, 0, 0);
            ret += cs_sal_epon_onu_pon_trans_tx_pwr_set(0, 0, 0);
            led_send_message(0, LED_MSG_UPDATE, LED_EVENT_TX_OFF);
        }
        else {
            return OAM_E_PARAM;
        }

        if (duration > 0) {
            g_laser_tx_power_timer = cs_timer_add(duration * 1000, __laser_tx_power_on,
                                                  NULL);
        }
    }
    else {
        return OAM_E_NOT_FOUND;
    }

    if(ret){
        return OAM_E_PARAM;
    }
    
    return OAM_E_OK;
}

oam_status zte_oam_rougue_onu_excl_ability_set_adapt(
    oam_uint8 enable)
{
    return OAM_E_OK;
}

oam_status zte_oam_rougue_onu_excl_ability_get_adapt(
    oam_uint8  *enable)
{
    return OAM_E_OK;
}

oam_status zte_oam_hg_mac_pool_add(
    oam_uint8 entries,
    oam_uint8  *mac)
{
    return OAM_E_OK;
}

oam_status zte_oam_hg_mac_pool_del(
    oam_uint8 entries,
    oam_uint8  *mac)
{

    return OAM_E_OK;
}

oam_status zte_oam_hg_mac_pool_clr( )
{
    return OAM_E_OK;
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
    return OAM_E_OK;
}

oam_status zte_oam_hg_mgmt_vlan_set_adapt(
    oam_uint16  vlan)
{
    return OAM_E_OK;
}

oam_status zte_oam_hg_mgmt_vlan_get_adapt(
    oam_uint16  *vlan)
{
    return OAM_E_OK;
}

oam_status zte_oam_hg_report_state_set_adapt(
    oam_port_id_t portId,
    oam_uint8 *hg_mac,
    oam_uint8 state)
{
    return OAM_E_OK;
}

oam_status zte_oam_hg_report_state_get_adapt(
    oam_port_id_t portId,
    oam_uint8 *hg_mac,
    oam_uint8 *state)
{
    return OAM_E_OK;
}


oam_status zte_oam_port_attr_set_adapt(
    oam_port_id_t portId,
    zte_oam_port_mode_e  mode)
{
    cs_sal_eth_port_mode_t cfg = 0;
    cs_status_e ret = 0;
#ifdef NEED_SAVE
    cs_db_eth_port_t   port_info;
#endif

    switch (mode) {
        case ZTE_OAM_PORT_MODE_AUTO:
            cfg = CS_SAL_ETH_PORT_MODE_AUTO_RATE_AUTO_DUPLEX_AUTO;
            break;

        case ZTE_OAM_PORT_MODE_10M_HALF:
            cfg = CS_SAL_ETH_PORT_MODE_FORCE_RATE_10_DUPLEX_HALF;
            break;

        case ZTE_OAM_PORT_MODE_10M_FULL:
            cfg = CS_SAL_ETH_PORT_MODE_FORCE_RATE_10_DUPLEX_FULL;
            break;

        case ZTE_OAM_PORT_MODE_100M_HALF:
            cfg = CS_SAL_ETH_PORT_MODE_FORCE_RATE_100_DUPLEX_HALF;
            break;

        case ZTE_OAM_PORT_MODE_100M_FULL:
            cfg = CS_SAL_ETH_PORT_MODE_FORCE_RATE_100_DUPLEX_FULL;
            break;

        case ZTE_OAM_PORT_MODE_1000M_FULL:
            cfg = CS_SAL_ETH_PORT_MODE_FORCE_RATE_1000_DUPLEX_FULL;
            break;

        default:
            return OAM_E_PARAM;
    }

    ret = cs_sal_eth_port_mode_set(0, __SAL_ETH_PORT(portId), cfg);

    if (ret) {
        return OAM_E_ERROR;
    }

#ifdef NEED_SAVE
    if (portId <= DB_MAX_PORT && portId > 0) {
        memset(&port_info, 0x00, sizeof(port_info));
        port_info.port_id = portId;
        port_info.work_mode = cfg;
        cs_db_entry_mod(TB_ETH_PORT, (void *) &port_info, sizeof(port_info),
                        column_bit(TB_ETH_PORT, "work_mode"));
    }
#endif

    return OAM_E_OK;
}

oam_status zte_oam_port_attr_get_adapt(
    oam_port_id_t portId,
    zte_oam_port_mode_e  *mode)
{
    cs_sal_eth_port_mode_t cfg = 0;

    if (cs_sal_eth_port_mode_get(0, __SAL_ETH_PORT(portId), &cfg)) {
        return OAM_E_ERROR;
    }

    switch (cfg) {
        case CS_SAL_ETH_PORT_MODE_AUTO_RATE_AUTO_DUPLEX_AUTO:
        case CS_SAL_ETH_PORT_MODE_AUTO_RATE_10_100_DUPLEX_AUTO:
            *mode = ZTE_OAM_PORT_MODE_AUTO;
            break;

        case CS_SAL_ETH_PORT_MODE_FORCE_RATE_10_DUPLEX_HALF:
            *mode = ZTE_OAM_PORT_MODE_10M_HALF;
            break;

        case CS_SAL_ETH_PORT_MODE_FORCE_RATE_10_DUPLEX_FULL:
            *mode = ZTE_OAM_PORT_MODE_10M_FULL;
            break;

        case CS_SAL_ETH_PORT_MODE_FORCE_RATE_100_DUPLEX_HALF:
            *mode = ZTE_OAM_PORT_MODE_100M_HALF;
            break;

        case CS_SAL_ETH_PORT_MODE_FORCE_RATE_100_DUPLEX_FULL:
            *mode = ZTE_OAM_PORT_MODE_100M_FULL;
            break;

        case CS_SAL_ETH_PORT_MODE_FORCE_RATE_1000_DUPLEX_FULL:
            *mode = ZTE_OAM_PORT_MODE_1000M_FULL;
            break;

        default:
            break;
    }

    return OAM_E_OK;
}

oam_status zte_oam_port_work_attr_get_adapt(
    oam_port_id_t portId,
    zte_oam_port_mode_e  *mode)
{
    oam_status rc = OAM_E_OK;

    return rc;
}

oam_status zte_oam_remote_conn_set_adapt(
    oam_zte_onu_remote_conn_t  *ipConfig)
{
    cs_int32 ret = 0;
    oam_uint16 vlan = 0;
    oam_uint16 enable = 0;
    cs_ip_host_if_cfg_t cfg;
#ifdef NEED_SAVE
    db_zte_mgmt_t  mgmt_info;
#endif

    memset(&cfg, 0, sizeof(cs_ip_host_if_cfg_t));
    cfg.ip_addr.ip_ver =  CS_IPV4;
    cfg.ip_mask.ip_ver =  CS_IPV4;
    cfg.gw_ip.ip_ver =  CS_IPV4;

    cs_db_entry_get(TB_ZTE_MGMT, (void *) &mgmt_info, sizeof(mgmt_info));

    enable = (ipConfig->enable == 1) ? 1 : 0;

    if (enable) {
        cfg.ip_addr.addr.v4.s_addr = ipConfig->remote_ip;
        cfg.ip_mask.addr.v4.s_addr = ipConfig->mask;
        cfg.gw_ip.addr.v4.s_addr = ipConfig->gateway;
    }
    else {
        cfg.ip_addr.addr.v4.s_addr = 0xc0a80101;
        cfg.ip_mask.addr.v4.s_addr = 0xFFFFFF00;
        cfg.gw_ip.addr.v4.s_addr = 0xc0a80101;
    }

    ret = cs_l3_intf_set(0, 0, &cfg);

    if (ret != 0) {
        return ret;
    }

    vlan |= (((ipConfig->priority) << 13) & 0xE000);
    vlan |= (ipConfig->vlan & 0xFFF);
    cs_inband_mgnt_set(0, 0, vlan, enable);

#ifdef NEED_SAVE
    mgmt_info.inband_ip = ipConfig->remote_ip;
    mgmt_info.inband_mask = ipConfig->mask;
    mgmt_info.gw = ipConfig->gateway;
    mgmt_info.mgmt_net = ipConfig->host_ip;
    mgmt_info.mgmt_mask = ipConfig->host_mask;
    mgmt_info.inband_vlan = ipConfig->vlan;
    mgmt_info.inband_pri = ipConfig->priority;
    mgmt_info.inband_enable = ipConfig->enable;

    cs_db_entry_mod(TB_ZTE_MGMT, (void *) &mgmt_info, sizeof(mgmt_info),
                    column_bit(TB_ZTE_MGMT, "ALL"));
#endif

    return ret;
}

oam_status zte_oam_remote_conn_get_adapt(
    oam_zte_onu_remote_conn_t  *ipConfig)
{
    cs_int32 ret = 0;
    oam_uint16 pri, enable;
    cs_ip_host_if_cfg_t cfg;
    db_zte_mgmt_t  mgmt_info;

    cs_db_entry_get(TB_ZTE_MGMT, (void *) &mgmt_info, sizeof(mgmt_info));

    ipConfig->remote_ip =  mgmt_info.inband_ip;
    ipConfig->mask = mgmt_info.inband_mask;
    ipConfig->gateway = mgmt_info.gw ;

    cs_inband_mgnt_get(0, 0, &pri, &enable);
    ipConfig->priority = pri;
    ipConfig->enable = enable ? 1 : 2;
    memset(&cfg, 0, sizeof(cs_ip_host_if_cfg_t));
    ret = cs_l3_intf_get(0, 0, &cfg);
    ipConfig->host_ip = cfg.ip_addr.addr.v4.s_addr;
    ipConfig->host_mask = cfg.ip_mask.addr.v4.s_addr;
    //ipConfig->gateway= cfg.gw_ip.addr.v4.s_addr;
    return ret;
}

oam_status zte_oam_onu_config_save_adapt()
{
    cs_db_lynxe_save();
    return OAM_E_OK;
}

oam_status zte_oam_onu_config_clear_adapt()
{
    cs_db_load(0);
    return OAM_E_OK;
}

oam_status zte_oam_factory_setting_restore_adapt()
{
    cs_db_factory_default();
    cs_reset(0, CS_RESET_CHIP);
    
    return OAM_E_OK;
}

oam_status zte_prv_oam_onu_sn_get_adapt(
    oam_uint8  *onu_sn,
    oam_uint8  *sn_len)
{
    db_zte_sn_t sn;

    memset(&sn, 0x00, sizeof(sn));
    cs_db_entry_get(TB_ZTE_SN, (void *) &sn, sizeof(sn));
    memcpy(onu_sn, sn.sn, strlen((char *)sn.sn));
    *sn_len = strlen((char *)sn.sn);

    return OAM_E_OK;
}

oam_status zte_prv_oam_typec_config_adapt(
    oam_uint8  k1,
    oam_uint8  k2)
{
    return OAM_E_OK;
}

oam_status zte_prv_oam_typec_status_get_adapt(
    oam_uint8  *k1,
    oam_uint8  *k2)
{
    return OAM_E_OK;
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
    cs_pppoe_auth_set(0, mode, name, pwd);
    cs_pppoe_test_set(0, TRUE, vlan);
#endif    
    return OAM_E_OK;
}

oam_status zte_oam_pppoe_simul_test_stop_adapt(
    oam_port_id_t port)
{
#ifdef HAVE_PPPOE
    cs_pppoe_test_set(0, FALSE, 0);
#endif

    return OAM_E_OK;
}
oam_status zte_oam_pppoe_simul_result_get_adapt(
    oam_port_id_t port,
    oam_uint8 *state,
    oam_uint8 *reason)
{
#ifdef HAVE_PPPOE
    cs_pppoe_test_get(0, state, reason);
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
    oam_status ret = OAM_E_OK;

    cs_dhcp_test_set(0, TRUE, vlan, option60 + 2, timeout);
    return ret;
}

oam_status zte_oam_ipoe_simul_test_stop_adapt(
    oam_port_id_t port)
{
    cs_dhcp_test_set(0, FALSE, 0, NULL, 0);
    return OAM_E_OK;
}
oam_status zte_oam_ipoe_simul_result_get_adapt(
    oam_port_id_t port,
    oam_zte_onu_ipoe_state *state)
{
    cs_dhcp_test_get(0, &state->state, &state->conclusion, &state->reason,
                     &state->ip_addr, &state->gw_addr, &state->vlan_used);
    return OAM_E_OK;
}
/******ipoe end*********/

oam_uint32 zte_oam_mac_pool_query_adapt(
    oam_port_id_t portId,
    oam_zte_onu_port_mac_pool_hdr_t *inHeader,
    oam_zte_onu_port_mac_pool_hdr_t *outHeader,
    oam_zte_onu_port_mac_pool_block_t *block)
{
    oam_uint32 len = 0;
    oam_uint32 i = 0;
    oam_uint32 total = 0, tmpLen, start_index;
    oam_uint8 blockData[100][CS_MACADDR_LEN];
    oam_uint32 blockNum = 0;
    oam_uint16 attr;
    oam_uint8 mac[CS_MACADDR_LEN];
    oam_uint16 sessionId = ntohs(inHeader->sessionId);
    cs_sal_entry_fdb_t fdb_entry;
    oam_uint16 offset = 0;
    oam_uint16 out_index = 0;
    oam_uint16  vlan_id = 0;
    cs_boolean  unicast_search_flag = TRUE;
    cs_status_e ret = 0;
    cs_sal_fdb_op_mode_t mode = SAL_FDB_OP_MODE_BOTH;

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

    memset(&fdb_entry, 0x00, sizeof(cs_sal_fdb_entry_t) );

    i = 0;
    start_index = 0;

    do {
        if (unicast_search_flag == TRUE) {
#ifdef HAVE_MPORTS
            /*ret = cs_sal_fdb_entry_getnext (context, 0,  0,  index.idxValue, mode, offset,
                &out_index,  &fdb_entry);*/
#else
            ret = cs_sal_fdb_entry_getnext(0, mode,  offset, &out_index, &fdb_entry);
#endif

            if (ret) {
                OAM_ORG_DEBUG("cs_sal_fdb_entry_getnext fails. ret: 0x%x \n", ret);
                unicast_search_flag = FALSE;
                offset = 0;

            }
        }

        fdb_entry.port = __OAM_ETH_PORT(fdb_entry.port);

        if (unicast_search_flag == FALSE) {
            /*unicast FDB has searched ,then search MC FDB*/
            ret = mc_port_fdb_get ( 0, portId,  offset, &out_index, mac, &vlan_id);

            if (ret) {

                OAM_ORG_DEBUG("mc_port_fdb_get fails. ret: 0x%x \n", ret);
                /*init again */
                memset (g_mac_pool_session_id, 0xff, sizeof(g_mac_pool_session_id ) );
                memset (g_mac_pool_seq_no, 0x0, sizeof (g_mac_pool_seq_no ) );
                memset (g_mac_pool_next_start_index, 1, sizeof (g_mac_pool_next_start_index ) );
                break;
            }

            fdb_entry.port = portId;
            fdb_entry.type = SAL_FDB_ENTRY_MC_TYPE;
            fdb_entry.vlan_id = vlan_id;
            memcpy (&fdb_entry.mac, mac, 6);
        }

        offset = out_index;

        if (fdb_entry.port != portId) {
            OAM_ORG_LOG_INFO("not this port \n");
            continue;
        }

        i ++;
        memcpy(mac, &fdb_entry.mac, CS_MACADDR_LEN);
        attr = 0;
        OAM_ORG_DEBUG("status = %d ,mac = %#x:%#x:%#x:%#x:%#x:%#x\n", fdb_entry.type,
                      mac[0], mac[1], mac[2],
                      mac[3], mac[4], mac[5]);

        switch (fdb_entry.type) {
            case SAL_FDB_ENTRY_DYNAMIC:
                attr |= ZTE_FDB_TYPE_DYNAMIC;
                break;

            case SAL_FDB_ENTRY_STATIC:
                attr |= ZTE_FDB_TYPE_STATIC;
                break;

            case  SAL_FDB_ENTRY_MC_TYPE:
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
    if ( i == ZTE_MAX_ENTRY_TABLE || blockNum == 0 || ret) {
        OAM_ORG_LOG_INFO("reach the last entry.\n");
        OAM_ORG_LOG_INFO("i= %d, blockNum = %d, ret = 0x%x\n", i, blockNum, ret);
        outHeader->seqNo = htons(g_mac_pool_seq_no[ portId - 1] | 0x8000);
    }

    outHeader->totalEntryNum = htons(total);
    outHeader->len = htons(len - 4);
    outHeader->result = OAM_ZTE_VAR_SET_OK;

    return len;
}




oam_status zte_oam_onu_pon_mac_get_adapt(oam_uint8 *mac)
{
    cs_status_e ret = CS_SUCCESS;
    cs_mac_t onu_mac;

    OAM_ASSERT_RET(mac != NULL, OAM_E_PARAM);

    ret = cs_sal_epon_onu_pon_mac_addr_get(0, 0, &onu_mac);

    if (ret != CS_SUCCESS) {
        return OAM_E_ERROR;
    }

    memcpy(mac, (oam_uint8 *)&onu_mac, sizeof(cs_mac_t));

    return ret;
}

oam_status zte_oam_dry_contact_a_alarm_info_get_adapt(oam_uint64 *alarm_info)
{
    db_zte_backup_battery_t battery;

    memset(&battery, 0x00, sizeof(battery));
    battery.cfg_id = BATTERY_SWITCH_A_INPUT;
    cs_db_entry_get(TB_ZTE_BAK_BATTERY, &battery, sizeof(battery));
    memcpy((oam_uint8 *)alarm_info, &battery.alarm_info,
           sizeof(battery.alarm_info));

    return OAM_E_OK;
}

oam_status zte_oam_dry_contact_b_alarm_info_get_adapt(oam_uint64 *alarm_info)
{
    db_zte_backup_battery_t battery;

    memset(&battery, 0x00, sizeof(battery));
    battery.cfg_id = BATTERY_SWITCH_B_INPUT;
    cs_db_entry_get(TB_ZTE_BAK_BATTERY, &battery, sizeof(battery));
    memcpy((oam_uint8 *)alarm_info, &battery.alarm_info,
           sizeof(battery.alarm_info));

    return OAM_E_OK;
}

oam_status zte_oam_check_ip_address_adapt(oam_uint32 ip, oam_uint32 mask,
        oam_uint32 gw)
{
    cs_int32 ret = 0;
    ret = cs_l3_ip_addr_valid_check(ip, mask, gw);
    return ret;
}

oam_status zte_oam_db_alarm_restore_adapt()
{
    return OAM_E_OK;
}

oam_status zte_oam_db_port_alarm_restore_adapt()
{
    return OAM_E_OK;
}


static void zte_oam_port_link_status_handle(const sdl_event_info_s *pinfo)
{
    oam_port_id_t port = 0;
    oam_boolean   opr;

    OAM_ASSERT(pinfo != NULL);

    port = pinfo->port_num;
    opr = pinfo->event_data.link_chng_data.link_up;

    OAM_ORG_DEBUG("port %d, link %d\n", port, opr);

    port = __OAM_ETH_PORT(port);

    if ((port == OAM_PON_PORT_ID) || ( port > CTC_MAX_ONU_UNI_PORTS)) {
        OAM_ORG_LOG("oam_handler_port_link_status port %d error!\n", port);
        return;
    }

    if (opr == OAM_TRUE) {
        oam_build_zte_extend_alarm_raised(port, ZTE_OAM_LINK_UP);
    }
    else {
        oam_build_zte_extend_alarm_raised(port, ZTE_OAM_LINK_DOWN);
    }

}

void zte_oam_port_link_status_reg ()
{
    cs_event_cfg_s   evt_cfg;
    cs_status_e       status;

    memset(&evt_cfg, 0, sizeof(cs_event_cfg_s));
    evt_cfg.callback   = zte_oam_port_link_status_handle;
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
        return;
    }
}

oam_status zte_oam_gpio_lao_report_reg_adapt()
{
    cs_gpio_lao_report_reg(oam_build_zte_lao_report);

    return OAM_E_OK;
}

#endif/*END_ZTE_SPECIFIC*/

