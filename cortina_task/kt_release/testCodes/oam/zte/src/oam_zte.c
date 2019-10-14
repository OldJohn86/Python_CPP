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

#ifdef ZTE_SPECIFIC
#include "oam_cmn.h"
#include "oam_control.h"
#include "oam_supp.h"
#include "oam_ctc.h"
#include "oam_zte.h"
#include "oam_ctc_pdu.h"
#include "oam_zte_pdu.h"
#include "oam_client.h"
#include "oam_ctc_client.h"
#include "oam_mux_par.h"
#include "oam_ctc_adapt.h"
#include "oam_zte_adapt.h"

#ifdef HAVE_LOOP_DETECT
#include "loop_detect_api.h"
#endif
#include "oam_ctc_alarm.h"
#include "oam_zte_stats.h"
#include "oam_adapt.h"
#include "oam_main.h"

/*Global variables*/
oam_oui_t  oam_oui_zte = {0x00, 0x0f, 0x3e};
static oam_uint8 g_zte_ext_alarm_cfg = ZTE_OAM_ALARM_CONFIG_ENABLE ;

void zte_oam_send(
    oam_llid_t llid,
    oam_uint8 *buf,
    oam_uint32 len)
{
    oam_port_id_t port = oam_port_id_get();
    oam_if_t *intf = oam_intf_find(llid);

    if (intf == NULL) {
        OAM_LOG_MAJ("intf not found \n");
        return;
    }

    if (len < OAM_MIN_OAMPDU_SIZE)
        len = OAM_MIN_OAMPDU_SIZE;

    if (port == OAM_UNI_PORT_ID1 || port == OAM_MGMT_PORT_ID) {
        zte_oam_onu_pon_mac_get_adapt((buf + 6));
    }

    if (intf->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_COMPLETED) {
        oam_send(llid, OAM_PON_PORT_ID, buf, len);
        intf->org_specific_tx++;
    }
}

oam_uint32 oam_zte_extend_alarm_admin_status_set( oam_uint8 status  )
{
    g_zte_ext_alarm_cfg = status ;
    ctc_oam_set_alarm_config(CTC_ONU_ALARM, 0, CTC_ALARM_ONU_DRY_CONTACTA,
                             status + 1);
    ctc_oam_set_alarm_config(CTC_ONU_ALARM, 0, CTC_ALARM_ONU_DRY_CONTACTB,
                             status + 1);

    return OAM_E_OK ;
}

oam_uint32 oam_zte_extend_alarm_admin_status_get( oam_uint8 *status  )
{
    *status = g_zte_ext_alarm_cfg ;
    return OAM_E_OK ;
}

void oam_build_zte_lao_report(oam_uint8 status)
{
    oam_if_t *oamif = oam_intf_find_valid();
    oam_uint8 pdu[OAM_MAX_OAMPDU_SIZE];
    oam_uint32 len = 0;
    oam_pdu_ctc_t *pOutPkt = NULL;
    oam_zte_pon_lao_rpt_containe_struct *ctn = NULL;

    if (!oamif) {
        return;
    }

    OAM_ORG_DEBUG( "status 0x%x \n", status);

    memset(pdu, 0x00, OAM_MAX_OAMPDU_SIZE);

    oam_build_pdu_hdr(oamif, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);

    pOutPkt = (oam_pdu_ctc_t *)pdu;
    memcpy(pOutPkt->oui, oam_oui_ctc, OAM_OUI_LEN);
    pOutPkt->opcode = OAM_PDU_CTC_VAR_RESP;

    len += sizeof(oam_pdu_ctc_t);

    ctn = (oam_zte_pon_lao_rpt_containe_struct *)pOutPkt->data;
    ctn->hdr.branch = OAM_CTC_ATTRIB_BRANCH;
    ctn->hdr.leaf = htons(ZTE_OAM_LEAF_ONU_LAO_RPT);
    ctn->hdr.width = 3;

    ctn->port_num = 1;
    ctn->active_pon_status = 1 - status;
    ctn->standby_pon_status = 0;

    len += sizeof(oam_zte_pon_lao_rpt_containe_struct) - 1;

    zte_oam_send(oamif->llid, pdu, len);

}

void oam_build_zte_dry_contactA_alarm(oam_uint8 status)
{
    oam_uint8 enable = 0;
    oam_uint64 alarm_info = 0;

    oam_zte_extend_alarm_admin_status_get(&enable) ;

    if (enable == ZTE_OAM_ALARM_CONFIG_DISABLE)
        return ;

    zte_oam_dry_contact_a_alarm_info_get_adapt(&alarm_info);

    OAM_ORG_DEBUG( "ContactA status 0x%x alarm info %d \n", status, alarm_info);

    ctc_oam_build_alarm_event(CTC_ONU_ALARM, 0, CTC_ALARM_ONU_DRY_CONTACTA, status,
                              alarm_info);
}

void oam_build_zte_dry_contactB_alarm(oam_uint8 status)
{
    oam_uint8 enable = 0;
    oam_uint64 alarm_info = 0;

    oam_zte_extend_alarm_admin_status_get(&enable) ;

    if (enable == ZTE_OAM_ALARM_CONFIG_DISABLE)
        return ;

    zte_oam_dry_contact_b_alarm_info_get_adapt(&alarm_info);

    OAM_ORG_DEBUG( "ContactB status 0x%x alarm info %d \n", status, alarm_info);

    ctc_oam_build_alarm_event(CTC_ONU_ALARM, 0, CTC_ALARM_ONU_DRY_CONTACTB, status,
                              alarm_info);
}

void oam_build_zte_extend_alarm_raised(oam_uint8 port, oam_uint16 alarm_id )
{
    oam_uint32 len = 0;
    oam_uint8 pdu[OAM_MAX_OAMPDU_SIZE];
    oam_zte_onu_alarm_info_struct *pAlarm = NULL ;
    oam_if_t *oamif =  NULL ;
    oam_uint8 status = 0 ;
    oam_uint16 portId = port;
    oam_pdu_ctc_index_t   *index ;
    oam_zte_onu_alarm_report *container ;

    oam_zte_extend_alarm_admin_status_get(&status) ;

    if (status == ZTE_OAM_ALARM_CONFIG_DISABLE) {
        OAM_ORG_LOG("ZTE extend alarm is disabled\n");
        return ;
    }

    oamif = oam_intf_find_valid();

    if (!oamif) {
        OAM_ORG_LOG("oamif is NULL\n");
        return;
    }

    memset(pdu, 0, sizeof(pdu));
    oam_build_pdu_hdr(oamif, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    len += sizeof(oam_pdu_hdr_t);

    pAlarm = (oam_zte_onu_alarm_info_struct *)(pdu + len);
    memcpy(pAlarm->oui, oam_oui_ctc, OAM_OUI_LEN);
    pAlarm->opCode = OAM_PDU_ZTE_VAR_RESP ;
    /*oam_printf("(oamif->ctc_OAM_Ext_version %d\n",oamif->ctc_OAM_Ext_version);*/
    /*Only support CTC 2.1..zte oam still use old management object style*/
    len += sizeof(oam_zte_onu_alarm_info_struct);

    index = (oam_pdu_ctc_index_t *) (pdu + len);
    index->hdr.branch = OAM_CTC_INDEX_BRANCH;
    index->hdr.leaf = htons(OAM_CTC_LEAF_CODE_ONU_PORT);
    index->hdr.width = sizeof(oam_uint8);
    index->value = portId;
    len += sizeof(oam_pdu_ctc_index_t);

    container = (oam_zte_onu_alarm_report *)(pdu + len);
    container->hdr.branch = OAM_CTC_ATTRIB_BRANCH ; /*0xC7*/
    container->hdr.leaf = htons(ZTE_OAM_LEAF_ALARM_REPORT) ; /* 0x8000*/
    container->hdr.width = 0x05 ;
    container->alarm_id = htons(alarm_id) ;
    len += sizeof(oam_zte_onu_alarm_report);

    oamif->unique_event_tx++;
    zte_oam_send(oamif->llid, pdu, len);
}

void oam_build_hg_extend_alarm_raised(
    oam_uint8 port ,
    oam_uint16 alarm_id )
{
    oam_int32 len = 0;
    oam_uint8 pdu[OAM_MAX_OAMPDU_SIZE ];
    oam_zte_onu_alarm_info_struct   *pAlarm = NULL ;
    oam_if_t *oamif =  oam_intf_find_valid() ;
    oam_uint8 status = 0 ;
    oam_uint16 portId = port;
    oam_pdu_ctc_index_t  *index ;
    oam_zte_onu_alarm_report *container ;

    oam_zte_extend_alarm_admin_status_get(&status) ;

    if (status == ZTE_OAM_ALARM_CONFIG_DISABLE)
        return ;

    if (!oamif)
        return;

    memset(pdu, 0, sizeof(pdu));
    oam_build_pdu_hdr(oamif, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    len += sizeof(oam_pdu_hdr_t);

    pAlarm = (oam_zte_onu_alarm_info_struct *)(pdu + len);
    memcpy(pAlarm->oui, oam_oui_ctc, OAM_OUI_LEN);
    pAlarm->opCode = OAM_PDU_ZTE_VAR_RESP ;

    OAM_ORG_LOG("(oamif->ctc_OAM_Ext_version %d\n", oamif->ctc_OAM_Ext_version);
    len += sizeof(oam_zte_onu_alarm_info_struct);

    index = (oam_pdu_ctc_index_t *) (pdu + len);
    index->hdr.branch = OAM_CTC_INDEX_BRANCH;
    index->hdr.leaf = htons(OAM_CTC_LEAF_CODE_ONU_PORT);
    index->hdr.width = sizeof(oam_uint8);
    index->value = portId;
    len += sizeof(oam_pdu_ctc_index_t);

    container = (oam_zte_onu_alarm_report *)(pdu + len);
    container->hdr.branch = OAM_CTC_ATTRIB_BRANCH ; /*0xC7*/
    container->hdr.leaf = htons(ZTE_OAM_LEAF_ALARM_REPORT) ; /* 0x8000*/
    container->hdr.width = 0x05 ;
    container->alarm_id = htons(alarm_id) ;

    len = sizeof(oam_zte_onu_alarm_report);

    oamif->unique_event_tx++;
    zte_oam_send(oamif->llid, pdu, len);
}

void oam_zte_handler_auto_neg_status_event(
    oam_uint32 port,
    oam_boolean status)
{
    if (status == OAM_FALSE)
        oam_build_zte_extend_alarm_raised(port, ZTE_OAM_AUTO_NEG_FAIL);
}

oam_uint32 zte_oam_sn_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_sn_t *pData =
        (oam_ctc_onu_sn_t *)pOut;
    oam_uint8 mac [OAM_MACADDR_LEN];
    oam_uint8 vendorId[OAM_CTC_VENDER_ID_LEN+1] = {0};
    oam_uint8 model_name[OAM_CTC_MODEL_ID_LEN+1] = {0};
    oam_uint8 hwver[OAM_CTC_ONU_HW_LEN] = {0};
    oam_uint8 swVer[OAM_CTC_ONU_SW_LEN + 1] = {0};
    oam_uint8 ctc_version = 0;
    oam_uint32 pdu_len = 0;


    OAM_CTC_FUNCTION_ENTER(index);

    oam_ctc_version_get(oam_llid_get(), &ctc_version);

    zte_oam_onu_id_get_adapt(mac);
    zte_oam_vendor_id_get_adapt(vendorId);
    zte_oam_model_id_get_adapt(model_name);
    zte_oam_hw_ver_get_adapt(hwver);

   /* memcpy(pData->venderId,  vendorId, OAM_CTC_VENDER_ID_LEN);
    memcpy(pData->model,  model_name, OAM_CTC_MODEL_ID_LEN);
    memcpy(pData->onuId, mac, OAM_MACADDR_LEN);
    memcpy(pData->hwVer, hwver, OAM_CTC_ONU_HW_LEN);*/

    memcpy(pData->venderId + OAM_CTC_VENDER_ID_LEN - strlen((char *)vendorId),  vendorId, strlen((char *)vendorId));
    memcpy(pData->model + OAM_CTC_MODEL_ID_LEN - strlen((char *)model_name),  model_name, strlen((char *)model_name));
    memcpy(pData->hwVer + OAM_CTC_ONU_HW_LEN - strlen((char *)hwver),hwver,strlen((char *)hwver));
    memcpy(pData->onuId, mac, OAM_MACADDR_LEN);
    
    zte_oam_sw_ver_get_adapt(swVer);

    memcpy(pData->swVer + OAM_CTC_ONU_SW_LEN - strlen((char *)swVer), swVer, strlen((char *)swVer));

    if ( ctc_version == 0x30) {
        pData->hdr.width = sizeof(oam_ctc_onu_sn_t) - sizeof(oam_var_cont_t);
        memcpy(pData->extend_model + OAM_CTC_EXTEND_MODEL_ID - strlen((
                    char *)model_name),  model_name, strlen((char *)model_name));
        pdu_len = sizeof(oam_ctc_onu_sn_t);
    }
    else {
        pData->hdr.width = sizeof(oam_ctc_onu_sn_t) - sizeof(oam_var_cont_t) -
                           OAM_CTC_EXTEND_MODEL_ID;
        pdu_len = sizeof(oam_ctc_onu_sn_t) - OAM_CTC_EXTEND_MODEL_ID;
    }


    return pdu_len;
}

oam_uint32 zte_oam_fw_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_fw_ver_t *pData =
        (oam_ctc_onu_fw_ver_t *)pOut;
    oam_uint8           fw_ver[OAM_CTC_ONU_FW_VERSION_LEN];

    OAM_CTC_FUNCTION_ENTER(index);

    memset(pData->fwVersion, 0x00, sizeof(pData->fwVersion));

    zte_oam_fw_ver_get_adapt(fw_ver, &pData->hdr.width);
    
    memcpy(pData->fwVersion, fw_ver, OAM_CTC_ONU_FW_VERSION_LEN);

    return (sizeof(oam_var_cont_t) + pData->hdr.width);
}

oam_uint32 zte_oam_chipid_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecV,
    oam_uint32   *procRecvLen,
    oam_uint8    *pOut)
{
    oam_ctc_onu_chip_id_t *pData =
        (oam_ctc_onu_chip_id_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    zte_oam_chip_id_get_adapt(pData);
    pData->hdr.width = 8;

    return (sizeof(oam_ctc_onu_chip_id_t));
}

oam_uint32 zte_oam_alarm_config_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_zte_onu_alarm_config *pData = (oam_zte_onu_alarm_config *)pRecv;

    OAM_CTC_FUNCTION_ENTER(index);

    if (pData->hdr.width != 1) {
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG( "enable %d\n", pData->enable);

    if (ZTE_OAM_ALARM_CONFIG_DISABLE == pData->enable ||
        ZTE_OAM_ALARM_CONFIG_ENABLE == pData->enable) {
        oam_zte_extend_alarm_admin_status_set(pData->enable);
        pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    }
    else {
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_alarm_config_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_alarm_config *pData = (oam_zte_onu_alarm_config *)pOut;
    oam_uint8 status = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    pData->hdr.width = sizeof(oam_uint8);
    oam_zte_extend_alarm_admin_status_get(&status);
    OAM_ORG_DEBUG( "enable %d\n", status);
    pData->enable = status;

    return sizeof(oam_zte_onu_alarm_config);
}

oam_status zte_oam_onu_mdutrunk_vlan_set(
    oam_port_id_t port,
    oam_uint8 *data,
    oam_uint32 len)
{
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_vlan_tag_t vlan[ZTE_VLAN_MODE_TRUNK_MAX];
    oam_ctc_onu_vlan_tag_t *pData =
        (oam_ctc_onu_vlan_tag_t *) data;
    oam_uint8 i = 0;
    oam_uint16 trunk_num = len / sizeof(oam_ctc_onu_vlan_tag_t);

    OAM_ORG_DEBUG("mdu trunk num = %d \n", trunk_num);

    if (trunk_num > ZTE_VLAN_MODE_TRUNK_MAX) {
        OAM_ORG_LOG("mdu trunk number %d is more the max %d ",
                    trunk_num, ZTE_VLAN_MODE_TRUNK_MAX);
        return OAM_E_PARAM;
    }

    for (i = 0; i < trunk_num; i++) {
        vlan[i].tpid = ntohs(pData->tpid);
        vlan[i].tag = ntohs(pData->tag);
        pData ++;
        OAM_ORG_DEBUG("mdu trunk [%d]: tpid %#x , tag %#x \n",
                      i, vlan[i].tpid, vlan[i].tag);
    }

    ret = zte_oam_onu_mdutrunk_vlan_set_adapt(port, trunk_num, vlan);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set adapter failed, ret = %d \n", ret);
    }

    return ret;
}

oam_status zte_oam_onu_mdutrunk_vlan_get(
    oam_port_id_t port,
    oam_uint32 *num,
    oam_uint8 *data
)
{
    oam_status ret = OAM_E_OK;
    oam_ctc_onu_vlan_tag_t vlan[ZTE_VLAN_MODE_TRUNK_MAX];
    oam_ctc_onu_vlan_tag_t *pData =
        (oam_ctc_onu_vlan_tag_t *) data;
    oam_uint8 i = 0;
    oam_uint16 trunk_num = 0;

    ret = zte_oam_onu_mdutrunk_vlan_get_adapt(port, &trunk_num, vlan);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        return ret;
    }

    OAM_ORG_DEBUG("mdu trunk num = %d \n", trunk_num);

    if (trunk_num > ZTE_VLAN_MODE_TRUNK_MAX) {
        OAM_ORG_LOG("mdu trunk number %d is more the max %d ",
                    trunk_num, ZTE_VLAN_MODE_TRUNK_MAX);
        return OAM_E_PARAM;
    }

    for (i = 0; i < trunk_num; i++) {
        OAM_ORG_DEBUG("mdu trunk [%d]: tpid %#x , tag %#x \n",
                      i, vlan[i].tpid, vlan[i].tag);
        pData->tpid = htons(vlan[i].tpid);
        pData->tag = htons(vlan[i].tag);
        pData ++;
    }

    *num = trunk_num;
    return OAM_E_OK;
}

oam_uint32 zte_oam_onu_vlan_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecV,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_ctc_onu_vlan_t *pData =
        (oam_ctc_onu_vlan_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 vlanMode = CTC_VLAN_MODE_TRANSPARENT;
    oam_uint32 num = 0, len = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = ctc_oam_onu_vlan_mode_get_adapt(index.idxValue, &vlanMode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get vlan mode adapt return failed, ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    OAM_ORG_DEBUG("vlan mode = %d \n", vlanMode);

    pData->hdr.width = 0;

    if ((vlanMode != CTC_VLAN_MODE_TRANSPARENT) &&
        (vlanMode != ZTE_VLAN_MODE_MDUTRUNK)) {
        ret = ctc_oam_onu_default_vlan_tag_get(index.idxValue, pData->data);

        if (ret != OAM_E_OK) {
            OAM_ORG_LOG("ctc_oam_onu_default_vlan_tag_get failed,ret = %d \n", ret);
            goto ERROR_EXIT;
        }

        pData->hdr.width += sizeof(oam_ctc_onu_vlan_tag_t);
    }

    ret = OAM_E_OK;

    switch (vlanMode) {
        case CTC_VLAN_MODE_TRANSPARENT:
        case CTC_VLAN_MODE_TAG:
        case ZTE_VLAN_MODE_STACK:
            break;

        case CTC_VLAN_MODE_TRANSLATE:
            ret = ctc_oam_onu_translate_vlan_get(index.idxValue, &num,
                                                 pData->data + pData->hdr.width);

            if (ret != OAM_E_OK) {
                OAM_ORG_LOG("ctc_oam_onu_translate_vlan_get failed,ret = %d \n", ret);
                goto ERROR_EXIT;
            }

            pData->hdr.width += num * sizeof(oam_ctc_onu_vlan_xlate_entry);
            break;

        case CTC_VLAN_MODE_AGG:
            ret = ctc_oam_onu_agg_vlan_get(index.idxValue, &len,
                                           pData->data + pData->hdr.width);

            if (ret != OAM_E_OK) {
                OAM_ORG_LOG("ctc_oam_onu_agg_vlan_get failed,ret = %d \n", ret);
                goto ERROR_EXIT;
            }

            pData->hdr.width  += len;
            break;

        case CTC_VLAN_MODE_TRUNK:
        case ZTE_VLAN_MODE_HYBRID:
            ret = ctc_oam_onu_trunk_vlan_get(index.idxValue, &num,
                                             pData->data + pData->hdr.width);

            if (ret != OAM_E_OK) {
                OAM_ORG_LOG("ctc_oam_onu_trunk_vlan_get failed,ret = %d \n", ret);
                goto ERROR_EXIT;
            }

            pData->hdr.width += ( num * sizeof(oam_ctc_onu_vlan_tag_t));
            break;

        case ZTE_VLAN_MODE_MDUTRUNK:
            ret = zte_oam_onu_mdutrunk_vlan_get(index.idxValue, &num,
                                                pData->data + pData->hdr.width);

            if (ret != OAM_E_OK) {
                OAM_ORG_LOG("zte_oam_onu_mdutrunk_vlan_get failed,ret = %d \n", ret);
                goto ERROR_EXIT;
            }

            pData->hdr.width += ( num * sizeof(oam_ctc_onu_vlan_tag_t));
            break;

        default:
            OAM_ORG_LOG("unknow vlan mode  vlan mode = %d \n", vlanMode);
            goto ERROR_EXIT;
    }

    pData->vlanMode = vlanMode;
    pData->hdr.width +=  sizeof(pData->vlanMode);

    return  sizeof(oam_var_cont_t) + pData->hdr.width;

ERROR_EXIT:
    pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    return sizeof(oam_var_cont_t);

}

oam_uint32 zte_oam_onu_vlan_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_var_cont_t *pOutData
        = (oam_var_cont_t *)pOut;
    oam_ctc_onu_vlan_t *pInData  =
        (oam_ctc_onu_vlan_t *)pRecv;
    oam_uint8 vlanMode = CTC_VLAN_MODE_TRANSPARENT;
    oam_uint32 len = 0;
    oam_ctc_onu_vlan_tag_t defVlan;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutData->width  = OAM_CTC_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    memset(&defVlan, 0x00, sizeof(oam_ctc_onu_vlan_tag_t));
    
    vlanMode = pInData->vlanMode;

    OAM_ORG_DEBUG("vlan mode = %d \n", vlanMode);

    Recvlen -= sizeof(pInData->vlanMode);

    if ((vlanMode != CTC_VLAN_MODE_TRANSPARENT) &&
        (vlanMode != ZTE_VLAN_MODE_MDUTRUNK)) {
        oam_ctc_onu_vlan_tag_t *vlan  =
            (oam_ctc_onu_vlan_tag_t *) pInData->data;
        defVlan.tpid = ntohs(vlan->tpid);
        defVlan.tag = ntohs(vlan->tag);
        OAM_ORG_DEBUG("default vlan tpid:%#x, tag:%#x \n", defVlan.tpid, defVlan.tag);
        len = sizeof(oam_ctc_onu_vlan_tag_t);
        Recvlen -= len;
    }

    switch (vlanMode) {
        case CTC_VLAN_MODE_TRANSPARENT:
            ret = ctc_oam_onu_transparent_vlan_set_adapt(index.idxValue);
            break;

        case CTC_VLAN_MODE_TAG:
            ret = ctc_oam_onu_tag_vlan_set_adapt(index.idxValue, defVlan);
            break;

        case CTC_VLAN_MODE_TRANSLATE:
            ret = ctc_oam_onu_translate_vlan_set(index.idxValue, defVlan,
                                                 pInData->data + len, Recvlen);
            break;

        case CTC_VLAN_MODE_AGG:
            ret = ctc_oam_onu_agg_vlan_set(index.idxValue, defVlan, pInData->data + len,
                                           Recvlen);
            break;

        case CTC_VLAN_MODE_TRUNK:
        case ZTE_VLAN_MODE_HYBRID:
            ret = ctc_oam_onu_trunk_vlan_set(index.idxValue, defVlan, pInData->data + len,
                                             Recvlen);
            break;

        case ZTE_VLAN_MODE_MDUTRUNK:
            ret = zte_oam_onu_mdutrunk_vlan_set(index.idxValue, pInData->data + len,
                                                Recvlen);
            break;

        case ZTE_VLAN_MODE_STACK:
            ret = zte_oam_onu_stack_vlan_set_adapt(index.idxValue, defVlan);
            break;

        default:
            OAM_ORG_LOG("unknow vlan mode  vlan mode = %d \n", vlanMode);
            goto ERROR_EXIT;
    }

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("set_adapt failed,ret = %d \n", ret);
        goto ERROR_EXIT;
    }

    pOutData->width = OAM_CTC_VAR_SET_OK;
    return sizeof(oam_var_cont_t);

ERROR_EXIT:
    pOutData->width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
    return sizeof(oam_var_cont_t);
}


oam_uint32 zte_oam_mac_limit_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_zte_onu_mac_limit *pData = (oam_zte_onu_mac_limit *)pRecv;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutHdr->width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pData->number = ntohs(pData->number);

    OAM_ORG_DEBUG( "number 0x%x \n", pData->number);

    ret = zte_oam_mac_limit_set_adapt(index.idxValue, pData->number);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_mac_limit_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_mac_limit *pData = (oam_zte_onu_mac_limit *)pOut;
    oam_uint16 number = 0;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pData->hdr.width = sizeof(oam_uint16);
    ret = zte_oam_mac_limit_get_adapt(index.idxValue, &number);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("Number %d\n", number);
    pData->number = htons(number);

    return sizeof(oam_zte_onu_mac_limit);
}

oam_uint32 zte_oam_mac_aging_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    zte_oam_onu_mac_aging *pData = (zte_oam_onu_mac_aging *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_uint32 time = 0;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    time = ntohl(pData->time);
    OAM_ORG_DEBUG("Aging time 0x%x\n", time);

    if ((time < ZTE_OAM_MAC_AGING_MIN) ||
        (time > ZTE_OAM_MAC_AGING_MAX) ||
        (Recvlen != 4)) {
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = zte_oam_mac_aging_set_adapt(time);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_mac_aging_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    zte_oam_onu_mac_aging *pData = (zte_oam_onu_mac_aging *)pOut;
    oam_uint32 time = 0;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = zte_oam_mac_aging_get_adapt(&time);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("Aging time = 0x%x\n", time);
    pData->hdr.width = sizeof(oam_uint32);
    pData->time = htonl(time);
    return sizeof(zte_oam_onu_mac_aging);
}

oam_uint32 zte_oam_port_mac_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_desc_t *pInHdr = (oam_var_desc_t *)pRecv;
    oam_zte_onu_mac_entry *pData = (oam_zte_onu_mac_entry *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_uint32  i = 0;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutHdr->width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;

    switch (pData->action) {
        case ZTE_MAC_ENTRY_ACTION_DEL:
        case ZTE_MAC_ENTRY_ACTION_ADD:

            if (Recvlen != 2 + sizeof(zte_oam_onu_mac_entry_t)*pData->num) {
                OAM_ORG_LOG("Invalid width: %d \n", Recvlen);
                goto End;
                break;
            }

            OAM_ORG_DEBUG("Action = %x\n", pData->action);

            for (i = 0; i < pData->num; i++) {
                pData->entry[i].vlan = ntohs(pData->entry[i].vlan);
                OAM_ORG_DEBUG("vlanId = %d , Mac = %#x:%#x:%#x:%#x:%#x:%#x \n",
                              pData->entry[i].vlan,
                              pData->entry[i].mac[0], pData->entry[i].mac[1], pData->entry[i].mac[2],
                              pData->entry[i].mac[3], pData->entry[i].mac[4], pData->entry[i].mac[5]);

                if (((pData->entry[i].mac[0] == 0xff)
                     && (pData->entry[i].mac[1] == 0xff)
                     && (pData->entry[i].mac[2] == 0xff)
                     && (pData->entry[i].mac[3] == 0xff)
                     && (pData->entry[i].mac[4] == 0xff)
                     && (pData->entry[i].mac[5] == 0xff))
                    || (pData->entry[i].mac[0] & 0x01)) {
                    OAM_ORG_LOG("Mac addr is invalid. \n");
                    goto End;
                }
            }

            if (pData->action == ZTE_MAC_ENTRY_ACTION_ADD) {
                switch (ntohs(pInHdr->leaf)) {
                    case ZTE_OAM_LEAF_PORT_MAC_FILTER:
                        OAM_ORG_DEBUG("Add mac filter\n");
                        ret = zte_oam_port_mac_filter_add_adapt(index.idxValue, pData->num,
                                                                pData->entry);
                        break;

                    case ZTE_OAM_LEAF_PORT_MAC_BIND:
                        OAM_ORG_DEBUG("Add mac bind\n");
                        ret = zte_oam_port_mac_bind_add_adapt(index.idxValue, pData->num, pData->entry);
                        break;

                    case ZTE_OAM_LEAF_PORT_MAC_STATIC:
                        OAM_ORG_DEBUG("Add mac static\n");
                        ret = zte_oam_port_mac_static_add_adapt(index.idxValue, pData->num,
                                                                pData->entry);
                        break;

                    default:
                        break;
                }

                if (ret == OAM_E_RESOURCE) {
                    OAM_ORG_LOG("No resource, ret = %d \n", ret);
                    pOutHdr->width = OAM_CTC_VAR_SET_NO_RES;
                    return sizeof(oam_var_cont_t);
                }
            }
            else {
                switch (ntohs(pInHdr->leaf)) {
                    case ZTE_OAM_LEAF_PORT_MAC_FILTER:
                        OAM_ORG_DEBUG("Del mac filter\n");
                        ret = zte_oam_port_mac_filter_del_adapt(index.idxValue, pData->num,
                                                                pData->entry);
                        break;

                    case ZTE_OAM_LEAF_PORT_MAC_BIND:
                        OAM_ORG_DEBUG("Del mac bind\n");
                        ret = zte_oam_port_mac_bind_del_adapt(index.idxValue, pData->num, pData->entry);
                        break;

                    case ZTE_OAM_LEAF_PORT_MAC_STATIC:
                        OAM_ORG_DEBUG("Del static mac\n");
                        ret = zte_oam_port_mac_static_del_adapt(index.idxValue, pData->num,
                                                                pData->entry);
                        break;

                    default:
                        break;
                }

            }

            if (ret != OAM_E_OK) {
                OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
                goto End;
            }

            break;

        case ZTE_MAC_ENTRY_ACTION_CLR:

            switch (ntohs(pInHdr->leaf)) {
                case ZTE_OAM_LEAF_PORT_MAC_FILTER:
                    OAM_ORG_DEBUG("Clear mac filter\n");
                    ret = zte_oam_port_mac_filter_clr_adapt(index.idxValue);
                    break;

                case ZTE_OAM_LEAF_PORT_MAC_BIND:
                    OAM_ORG_DEBUG("Clear mac bind\n");
                    ret = zte_oam_port_mac_bind_clr_adapt(index.idxValue);
                    break;

                case ZTE_OAM_LEAF_PORT_MAC_STATIC:
                    OAM_ORG_DEBUG("Clear mac static\n");
                    ret = zte_oam_port_mac_static_clr_adapt(index.idxValue);
                    break;

                default:
                    break;
            }

            if (ret != OAM_E_OK) {
                OAM_ORG_DEBUG("Clear adapt return failed, ret = %d \n", ret);
                goto End;
            }

            break;

        default:
            goto End;
            break;
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    return sizeof(oam_var_cont_t);

End:
    pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_port_mac_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_var_desc_t *pInHdr = (oam_var_desc_t *)pRecv;
    oam_zte_onu_mac_entry *pData = (oam_zte_onu_mac_entry *)pOut;
    oam_uint8 i, number = 0;
    zte_oam_onu_mac_entry_t entry[ZTE_MAX_ENTRIES_PER_CONTAINER];
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    switch (ntohs(pInHdr->leaf)) {
        case ZTE_OAM_LEAF_PORT_MAC_FILTER:
            ret = zte_oam_port_mac_filter_get_adapt(index.idxValue, &number, entry);
            break;

        case ZTE_OAM_LEAF_PORT_MAC_BIND:
            ret = zte_oam_port_mac_bind_get_adapt(index.idxValue, &number, entry);
            break;

        case ZTE_OAM_LEAF_PORT_MAC_STATIC:
            ret = zte_oam_port_mac_static_get_adapt(index.idxValue, &number, entry);
            break;

        default:
            break;
    }

    if (ret != OAM_E_OK) {
        pData->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        *procRecvLen = sizeof(oam_var_cont_t) + sizeof(oam_uint8);
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("Total mac Num = %d \n", number);
    pData->num = number;
    pData->hdr.width = sizeof(oam_uint8) + sizeof(oam_uint8)
                       + pData->num * sizeof(zte_oam_onu_mac_entry_t);
    pData->action = ZTE_MAC_ENTRY_ACTION_LST;

    for (i = 0; i < pData->num; i++) {
        pData->entry[i].vlan = htons(entry[i].vlan);
        memcpy(pData->entry[i].mac, entry[i].mac, OAM_MACADDR_LEN);
    }

    *procRecvLen = sizeof(oam_var_cont_t) + sizeof(oam_uint8);
    return sizeof(oam_var_cont_t) + pData->hdr.width;
}


oam_uint32 zte_oam_port_isolation_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_onu_isolation_config *pData = (oam_zte_onu_isolation_config *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (pData->hdr.width != 1) {
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("Enable %d\n", pData->enable);
    ret = zte_oam_port_isolation_set_adapt(pData->enable);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;

    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_port_isolation_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_isolation_config *pData = (oam_zte_onu_isolation_config *)pOut;
    oam_uint8 enable;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = zte_oam_port_isolation_get_adapt(&enable);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("Enable = %d\n", enable);
    pData->hdr.width = sizeof(oam_uint8);
    pData->enable = enable;
    return sizeof(oam_zte_onu_isolation_config);
}

oam_uint32 zte_oam_pon_dn_shaping_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
#ifdef HAVE_MPORTS
    oam_zte_pon_dn_shaping_t *pData = (oam_zte_pon_dn_shaping_t *)pRecv;
    zte_oam_onu_rate_t cfg;
    oam_status ret = OAM_E_OK;
    OAM_CTC_FUNCTION_ENTER(index);
    memset(&cfg, 0, sizeof(zte_oam_onu_rate_t));
    cfg.enable = pData->op;

    if (cfg.enable) {
        cfg.cir = (pData->rate[0] << 16) | (pData->rate[1] << 8) | pData->rate[2];
        cfg.cbs = (pData->burst[0] << 16) | (pData->burst[1] << 8) | pData->burst[2];
    }

    OAM_ORG_DEBUG("Enable %d cir 0x%x, cbs 0x%x \n", cfg.enable, cfg.cir, cfg.cbs);
    ret = zte_oam_pon_dn_shaping_set_adapt(&cfg);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
#else
    pOutHdr->width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
#endif
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_pon_dn_shaping_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_pon_dn_shaping_t *pData = (oam_zte_pon_dn_shaping_t *)pOut;
#if HAVE_MPORTS
    zte_oam_onu_rate_t cfg;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = zte_oam_pon_dn_shaping_get_adapt(&cfg);

    if (ret != OAM_E_OK) {
        pData->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("Enable %d cir 0x%x, cbs 0x%x \n", cfg.enable, cfg.cir, cfg.cbs);
    pData->op = cfg.enable;

    if (cfg.enable) {
        pData->hdr.width = 7;

        pData->rate[0] = (cfg.cir >> 16) & 0xff;
        pData->rate[1] = (cfg.cir >> 8) & 0xff;
        pData->rate[2] = cfg.cir & 0xff;

        pData->burst[0] = (cfg.cbs >> 16) & 0xff;
        pData->burst[1] = (cfg.cbs >> 8) & 0xff;
        pData->burst[2] = cfg.cbs & 0xff;
    }
    else {
        pData->hdr.width = 1;
    }

    return sizeof(oam_var_cont_t) + pData->hdr.width;
#else
    pData->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
    return sizeof(oam_var_cont_t);
#endif
}

oam_uint32 zte_oam_port_stat_reset(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_onu_port_stats_reset *pReset = (oam_zte_onu_port_stats_reset *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutHdr->width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (!pReset->reset) {
        zte_oam_port_stat_reset_adapt(index.idxValue);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;

    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_pppoe_simul_test(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_onu_pppoe_test_rsp *pTest = (oam_zte_onu_pppoe_test_rsp *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint16 vlan = 0;
    oam_uint8 mode = 0;
    oam_int8 name[PPPOE_USER_MAX_LEN + 1], pwd[PPPOE_USER_MAX_LEN + 1];
    oam_uint8 i = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    memset(name, 0, sizeof(name));
    memset(pwd, 0, sizeof(pwd));

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutHdr->width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;

    if (pTest->config.indication == 1) {
        vlan = ntohs(pTest->config.vlan);
        mode = pTest->config.mode;

        for (i = 0; i < PPPOE_USER_MAX_LEN; i++ ) {
            if (pTest->config.name[i] == 0)
                continue;
            else
                break;
        }

        memcpy(name, &pTest->config.name[i], (PPPOE_USER_MAX_LEN - i));

        for (i = 0; i < PPPOE_PWD_MAX_LEN; i++) {
            if (pTest->config.pwd[i] == 0)
                continue;
            else
                break;
        }

        memcpy(pwd, &pTest->config.pwd[i], (PPPOE_PWD_MAX_LEN - i));

        ret = zte_oam_pppoe_simul_test_start_adapt(index.idxValue, vlan, mode, name,
                pwd);
    }
    else if (pTest->config.indication == 0) {
        ret = zte_oam_pppoe_simul_test_stop_adapt(index.idxValue);
    }
    else {
        ret = OAM_E_PARAM;
    }

    if (OAM_E_OK != ret) {
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_pppoe_simul_result_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_pppoe_state *pState = (oam_zte_onu_pppoe_state *)pOut;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pState->hdr.width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = zte_oam_pppoe_simul_result_get_adapt(index.idxValue, &(pState->state),
            &(pState->reason));

    if (ret != OAM_E_OK) {
        pState->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("port %d state %d reason %d \n", index.idxValue, pState->state,
                  pState->reason);
    pState->hdr.width = sizeof(oam_zte_onu_pppoe_state) - sizeof(oam_var_cont_t);

    return sizeof(oam_zte_onu_pppoe_state);
}

oam_uint32 zte_oam_ipoe_simul_test(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_onu_ipoe_test_rsp *pTest = (oam_zte_onu_ipoe_test_rsp *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint16 vlan = 0;
    oam_uint8 emulation_mothed = 0, timeout = 30;
    oam_int8  user_name[IPOE_USER_MAX_LEN + 1], pwd[IPOE_PWD_MAX_LEN + 1],
              option60[IPOE_OPTION60_MAX_LEN];
    oam_int8  mac[6];
    oam_uint8 i = 0, j = 0;
    oam_uint8 user_len = 0, pwd_len = 0;

    OAM_CTC_FUNCTION_ENTER(index);

    memset(user_name, 0, sizeof(user_name));
    memset(pwd, 0, sizeof(pwd));
#if 0

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutHdr->width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

#endif
    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    OAM_ORG_DEBUG("action %d\n", pTest->config.action);

    if (pTest->config.action == 0) {
        /*start*/
        vlan = ntohs(pTest->config.vlan);
        timeout = pTest->config.timeout;
        emulation_mothed = pTest->config.emulation_method;

        for (i = 0; i < IPOE_USER_MAX_LEN; i++ ) {
            if (pTest->config.user_name[i] == 0)
                continue;
            else
                break;
        }

        memcpy(user_name, &pTest->config.user_name[i], (IPOE_USER_MAX_LEN - i));
        user_len = (IPOE_USER_MAX_LEN - i);

        for (j = 0; j < IPOE_PWD_MAX_LEN; j++) {
            if (pTest->config.pwd[j] == 0)
                continue;
            else
                break;
        }

        memcpy(pwd, &pTest->config.pwd[j], (IPOE_PWD_MAX_LEN - j));
        pwd_len = (IPOE_PWD_MAX_LEN - j);

        if (pTest->config.emulation_method == 0) {
            memset(option60, 0, IPOE_OPTION60_MAX_LEN);
            option60[0] = 60;
            option60[1] = user_len + pwd_len;

            memcpy(&option60[2], user_name, user_len);
            memcpy(&option60[2 + user_len], pwd, pwd_len);
        }
        else {
            for (i = 0; i < IPOE_OPTION60_MAX_LEN; i++) {
                if (pTest->config.option60[i] == 0)
                    continue;
                else
                    break;
            }

            memcpy(option60, &pTest->config.option60[i], (IPOE_OPTION60_MAX_LEN - i));
        }

        memcpy(mac, &pTest->config.mac, 6);

        OAM_ORG_DEBUG("method[%d] name[%s] pwd[%s] mac[%02x.%02x.%02x.%02x.%02x.%02x] option60[%s] vlan[%d] timeout[%d]",
                      emulation_mothed, user_name, pwd, mac[0], mac[1], mac[2], mac[3], mac[4],
                      mac[5], option60, vlan, timeout);
        ret = zte_oam_ipoe_simul_test_start_adapt(index.idxValue, vlan,
                emulation_mothed, user_name, pwd, mac, (oam_uint8 *)option60, timeout);
    }
    else if (pTest->config.action == 1) {
        /*terminate test*/
        ret = zte_oam_ipoe_simul_test_stop_adapt(index.idxValue);
    }
    else {
        ret = OAM_E_PARAM;
    }

    if (OAM_E_OK != ret) {
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_ipoe_simul_result_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_ipoe_state *pState = (oam_zte_onu_ipoe_state *)pOut;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);
#if 0

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pState->hdr.width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

#endif
    ret = zte_oam_ipoe_simul_result_get_adapt(index.idxValue, pState);

    if (ret != OAM_E_OK) {
        pState->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("port [%d] state [%d] conclusion [%d] reason [%d]\n",
                  index.idxValue, pState->state, pState->conclusion, pState->reason);

    OAM_ORG_DEBUG("ip[%08x] gw[0x%08x] vlan[%d] ping time[%d] ack[%d] min[%d] max[%d] avg[%d]\n",
                  pState->ip_addr, pState->gw_addr, pState->vlan_used,
                  pState->ping_times, pState->ping_ack_timers, pState->min_delay,
                  pState->max_delay, pState->avg_delay);
    pState->ip_addr     = cs_htonl(pState->ip_addr);
    pState->gw_addr     = cs_htonl(pState->gw_addr);
    pState->vlan_used   = cs_htonl(pState->vlan_used);
    pState->vci         = cs_htons(pState->vci);
    pState->ping_times  = cs_htons(pState->ping_times);
    pState->ping_ack_timers = cs_htons(pState->ping_ack_timers);
    pState->min_delay       = cs_htons(pState->min_delay);
    pState->max_delay       = cs_htons(pState->max_delay);
    pState->avg_delay       = cs_htons(pState->avg_delay);

    pState->hdr.width = sizeof(oam_zte_onu_ipoe_state) - sizeof(oam_var_cont_t);

    return sizeof(oam_zte_onu_ipoe_state);
}

oam_uint32 zte_oam_flux_stat_ctrl_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_onu_port_flux_stats_ctrl *pData = (oam_zte_onu_port_flux_stats_ctrl *)
            pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutHdr->width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;

    if (pData->action) {
        /* start flux timer */
        oam_uint32 interval = pData->interval << 8;
        oam_uint32 duration = pData->duration;

        interval = ntohl(interval);
        duration = ntohl(duration);

        OAM_ORG_DEBUG("action=%d, interval=%d, duration=%d", pData->action, interval, duration);

        if (interval && (duration > interval)) {
            zte_flux_stat_start_adapt(index.idxValue, interval, duration);
        }
        else {
            pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        }
    }
    else {
        /* stop flux timer */
        zte_flux_stat_stop_adapt(index.idxValue);
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_flux_stat_ctrl_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_port_flux_stats_ctrl *pData = (oam_zte_onu_port_flux_stats_ctrl *)
            pOut;
    oam_uint8 action = 0;
    oam_uint32 interval = 0, duration = 0;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = zte_oam_flux_stat_ctrl_get_adapt(index.idxValue, &action, &interval,
                                           &duration);

    if (ret != OAM_E_OK) {
        pData->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("port %d action %d interval %d duration %d\n", index.idxValue,
                  action, interval, duration);
    pData->hdr.width = sizeof(oam_zte_onu_port_flux_stats_ctrl) - sizeof(
                           oam_var_cont_t);
    pData->action = action;
    pData->interval = htonl(interval) >> 8;
    pData->duration = htonl(duration);

    return sizeof(oam_zte_onu_port_flux_stats_ctrl);
}

oam_uint32 zte_oam_port_statistics_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_zte_onu_port_stats_rsp *pData =
        (oam_zte_onu_port_stats_rsp *) pOut;
    oam_zte_onu_port_stats *pStats = &(pData->stats);

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = zte_oam_port_statistics_get_adapt(index.idxValue, pStats);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_CTC_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    pStats->in_octets = htonll(pStats->in_octets);
    pStats->in_ucast_pkt = htonll(pStats->in_ucast_pkt);
    pStats->in_nucast_pkt = htonll(pStats->in_nucast_pkt);
    pStats->in_discard = htonll(pStats->in_discard);
    pStats->in_error = htonll(pStats->in_error);
    pStats->out_octets = htonll(pStats->out_octets);
    pStats->out_ucast_pkt = htonll(pStats->out_ucast_pkt);
    pStats->out_nucast_pkt = htonll(pStats->out_nucast_pkt);
    pStats->out_discard = htonll(pStats->out_discard);
    pStats->out_error = htonll(pStats->out_error);

    pData->hdr.width = sizeof(oam_zte_onu_port_stats);

    return sizeof(oam_zte_onu_port_stats_rsp);
}

oam_uint32 zte_oam_pon_port_stat_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_var_desc_t *pInDesc = (oam_var_desc_t *)pRecv;
    oam_var_cont_t *pOutCont = (oam_var_cont_t *)pOut;

    OAM_CTC_FUNCTION_ENTER(index);

    if (index.valid) {
        if (index.idxValue != 0) {
            OAM_ORG_LOG("port index is invalid \n");
            pOutCont->width  = OAM_ZTE_VAR_SET_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
    }

    if (ntohs(pInDesc->leaf) == ZTE_OAM_LEAF_PON_PORT_STAT1) {
        oam_zte_onu_llid_stats1 *stats1 = (oam_zte_onu_llid_stats1 *)pOut;
        zte_oam_pon_port_stat_get_adapt(stats1, NULL);
        stats1->out_pkts = htonll(stats1->out_pkts);
        stats1->out_octets = htonll(stats1->out_octets);
        stats1->out_mc_pkts = htonll(stats1->out_mc_pkts);
        stats1->out_bc_pkts = htonll(stats1->out_bc_pkts);
        stats1->in_pkts = htonll(stats1->in_pkts);
        stats1->in_octets = htonll(stats1->in_octets);
        stats1->in_mc_pkts = htonll(stats1->in_mc_pkts);
        stats1->in_bc_pkts = htonll(stats1->in_bc_pkts);

        stats1->hdr.width = sizeof(oam_zte_onu_llid_stats1) - sizeof(oam_var_cont_t);

        return sizeof(oam_zte_onu_llid_stats1);
    }
    else {
        oam_zte_onu_llid_stats2 *stats2 = (oam_zte_onu_llid_stats2 *)pOut;
        zte_oam_pon_port_stat_get_adapt(NULL, stats2);
        stats2->in_crc8_err_pkts = htonll(stats2->in_crc8_err_pkts);
        stats2->in_fec_crct_blks = htonll(stats2->in_fec_crct_blks);
        stats2->in_fec_uncrct_blks = htonll(stats2->in_fec_uncrct_blks);
        stats2->out_mpcp_pkts = htonll(stats2->out_mpcp_pkts);
        stats2->in_mpcp_pkts = htonll(stats2->in_mpcp_pkts);
        stats2->out_mpcp_reg_pkts = htonll(stats2->out_mpcp_reg_pkts);
        stats2->out_mpcp_rreq_pkts = htonll(stats2->out_mpcp_rreq_pkts);
        stats2->out_mpcp_rpt_pkts = htonll(stats2->out_mpcp_rpt_pkts);
        stats2->in_mpcp_gate_pkts = htonll(stats2->in_mpcp_gate_pkts);
        stats2->in_mpcp_reg_pkts = htonll(stats2->in_mpcp_reg_pkts);

        stats2->hdr.width = sizeof(oam_zte_onu_llid_stats2) - sizeof(oam_var_cont_t);
        return sizeof(oam_zte_onu_llid_stats2);
    }

}

oam_uint32 zte_oam_port_flux_stat_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_port_flux_stats *pData = (oam_zte_onu_port_flux_stats *)pOut;
    zte_oam_onu_port_flux_stats_t statis;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    memset(&statis, 0, sizeof(zte_oam_onu_port_flux_stats_t));

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = zte_oam_port_flux_statistics_get_adapt(index.idxValue, &statis);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("ctrl_ind = %x\n", statis.ctrl_ind);
    pData->hdr.width = sizeof(oam_zte_onu_port_flux_stats) - sizeof(oam_var_cont_t);
    pData->ctrl_ind = statis.ctrl_ind;
    pData->in_octets = htonll(statis.in_octets);
    pData->in_uc_pkts = htonll(statis.in_uc_pkts);
    pData->in_nuc_pkts = htonll(statis.in_nuc_pkts);
    pData->in_discard_pkts = htonll(statis.in_discard_pkts);
    pData->in_error_pkts = htonll(statis.in_error_pkts);
    pData->out_octets = htonll(statis.out_octets);
    pData->out_uc_pkts = htonll(statis.out_uc_pkts);
    pData->out_nuc_pkts = htonll(statis.out_nuc_pkts);
    pData->out_discard_pkts = htonll(statis.out_discard_pkts);
    pData->out_error_pkts = htonll(statis.out_error_pkts);

    return sizeof(oam_zte_onu_port_flux_stats);
}

oam_uint32 zte_oam_mac_pool_query(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_port_mac_pool_hdr_t *inHeader = (oam_zte_onu_port_mac_pool_hdr_t *)
            pRecv;
    oam_zte_onu_port_mac_pool_hdr_t *outHeader = (oam_zte_onu_port_mac_pool_hdr_t *)
            pOut;
    oam_zte_onu_port_mac_pool_block_t *block = (oam_zte_onu_port_mac_pool_block_t *)
            (pOut
             + sizeof(oam_zte_onu_port_mac_pool_hdr_t));

    return zte_oam_mac_pool_query_adapt(index.idxValue, inHeader, outHeader, block);
}

oam_uint32 zte_oam_voq_buffer_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_onu_queue_buffer_set *pQBuffer = (oam_zte_onu_queue_buffer_set *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_uint8 setType = 0;
    oam_uint32 usBuf = ZTE_OAM_Q0_UP_BUFFER_MAX_SIZE;
    oam_uint32 dnBuf = ZTE_OAM_Q0_DN_BUFFER_MAX_SIZE;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (pQBuffer->hdr.width != 0x05) {
        goto End;
    }

    switch (pQBuffer->direction) {
        case ZTE_OAM_BUFFER_UP_STREAM:
        case ZTE_OAM_BUFFER_DOWN_STREAM:
        case ZTE_OAM_BUFFER_BOTH_STREAM:
            setType = pQBuffer->direction + 1;
            break;

        default:
            goto End;
    }

    if (pQBuffer->enable == ZTE_OAM_BUFFER_MANAGEMENT_ENABLE) {
        usBuf = pQBuffer->buffer_size[2] | (pQBuffer->buffer_size[1] << 8) |
                (pQBuffer->buffer_size[0] << 16);
        dnBuf = usBuf;
    }
    else if (pQBuffer->enable) {
        goto End;
    }

    if (setType & 0x01) { /*UP Stream */
        OAM_ORG_DEBUG("Set Up stream buffer = %#x \n", usBuf);
        ret = zte_oam_voq_buffer_set_adapt(ZTE_OAM_BUFFER_UP_STREAM, 0, usBuf);
    }

    if (setType & 0x02) { /*DN STREAM*/
        OAM_ORG_DEBUG("Set Down stream buffer = %#x\n", dnBuf);
        ret = zte_oam_voq_buffer_set_adapt(ZTE_OAM_BUFFER_DOWN_STREAM, 0, dnBuf);
    }

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
    }
    else
        pOutHdr->width = OAM_ZTE_VAR_SET_OK;

    return sizeof(oam_var_cont_t);

End:
    pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_buff_mgt_cap_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_queue_buffer_get *pData = (oam_zte_onu_queue_buffer_get *)pOut;
    oam_status ret = OAM_E_OK;
    zte_oam_onu_queue_buffer_t qbuf;

    OAM_CTC_FUNCTION_ENTER(index);

    memset(&qbuf, 0, sizeof(zte_oam_onu_queue_buffer_t));
    ret = zte_oam_buf_mgmt_cap_get_adapt(&qbuf);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG( "capability %d min_ds 0x%x max_ds 0x%x min_us 0x%x max_us 0x%x\n",
                   qbuf.capability, qbuf.min_ds, qbuf.max_ds, qbuf.min_us, qbuf.max_us);

    pData->hdr.width = sizeof(oam_zte_onu_queue_buffer_get) -
                       sizeof(oam_var_cont_t);

    pData->capability =  qbuf.capability;
    pData->min_ds_buff_size[0] = (qbuf.min_ds >> 16) & 0xff;
    pData->min_ds_buff_size[1] = (qbuf.min_ds >> 8 ) & 0xff;
    pData->min_ds_buff_size[2] = (qbuf.min_ds    ) & 0xff;

    pData->max_ds_buff_size[0] = (qbuf.max_ds >> 16) & 0xff;
    pData->max_ds_buff_size[1] = (qbuf.max_ds >> 8 ) & 0xff;
    pData->max_ds_buff_size[2] = (qbuf.max_ds    ) & 0xff;

    pData->min_us_buff_size[0] = (qbuf.min_us >> 16) & 0xff;
    pData->min_us_buff_size[1] = (qbuf.min_us >> 8 ) & 0xff;
    pData->min_us_buff_size[2] = (qbuf.min_us    ) & 0xff;

    pData->max_us_buff_size[0] = (qbuf.max_us >> 16) & 0xff;
    pData->max_us_buff_size[1] = (qbuf.max_us >> 8 ) & 0xff;
    pData->max_us_buff_size[2] = (qbuf.max_us    ) & 0xff;

    return  sizeof(oam_zte_onu_queue_buffer_get);
}

oam_uint32 zte_oam_tx_laser_ctrl_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_pon_tx_ctrl *pData = (oam_zte_pon_tx_ctrl *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_uint8 action = pData->action;
    oam_uint16 duration = ntohs(pData->duration);
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    OAM_ORG_DEBUG( "match_mac %d\n", pData->match_mac);
    OAM_ORG_DEBUG( "action %d duration %d\n", action, duration);

    ret = zte_oam_tx_laser_ctrl_set_adapt(pData->match_mac, pData->mac, action,
                                          duration);

    if (ret == OAM_E_NOT_FOUND) {
        return 0;
    }
    else if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_rougue_onu_excl_ability_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_rougue_onu_excl_ability_t *pData = (oam_zte_rougue_onu_excl_ability_t *)
            pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;

    ret = zte_oam_rougue_onu_excl_ability_set_adapt(pData->enable);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_rougue_onu_excl_ability_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_rougue_onu_excl_ability_t *pData = (oam_zte_rougue_onu_excl_ability_t *)
            pOut;
    oam_uint8 enable = 0;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = zte_oam_rougue_onu_excl_ability_get_adapt(&enable);

    if (ret != OAM_E_OK) {
        pData->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("Enable %d \n", enable);

    pData->hdr.width = 1;
    pData->enable = enable;
    return sizeof(oam_zte_rougue_onu_excl_ability_t);
}

oam_uint32 hg_mac_is_zero(oam_uint8 *mac)
{
    oam_uint32    i = 0;
    oam_uint32    ret = 0;

    for (i = 0; i < OAM_MACADDR_LEN; i++ ) {
        if ( mac[i] != 0) {
            ret = 1;
            break;
        }
    }

    return ret;
}

oam_uint32 zte_oam_hg_mac_pool_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_onu_mc_address_pool *pData = (oam_zte_onu_mc_address_pool *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_uint8 action = pData->action;
    oam_uint8 i = 0;
    oam_uint8 *tmplen = NULL;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if ( pData->entries >  HG_PORT_MAX_MAC_POOL_NUM ) {
        OAM_ORG_LOG("Invalid entries %d\n", pData->entries);
        goto End;
    }

    OAM_ORG_DEBUG("action = %d\n", action);

    if ( (action == ZTE_MAC_ENTRY_ACTION_DEL)
         || (action == ZTE_MAC_ENTRY_ACTION_ADD)) {

        OAM_ORG_DEBUG("entries = %d\n", pData->entries);

        for (i = 0; i < pData->entries; i++ ) {
            ret = hg_mac_is_zero(pData->mac + i * OAM_MACADDR_LEN);

            if ( ret == 0 ) {
                OAM_ORG_LOG("Zero mac!\n");
                goto End;
            }

            tmplen = pData->mac + i * OAM_MACADDR_LEN;

            OAM_ORG_DEBUG("Mac = %#x:%#x:%#x:%#x:%#x:%#x \n",
                          *(tmplen), *(tmplen + 1), *(tmplen + 2),
                          *(tmplen + 3), *(tmplen + 4), *(tmplen + 5));
        }

        if ( pData->hdr.width <= 2) {
            OAM_ORG_LOG("Invalid len %d\n", pData->hdr.width);
            goto End;
        }

    }

    if ( action == ZTE_MAC_ENTRY_ACTION_CLR) {
        if ( pData->hdr.width != 1) {
            OAM_ORG_LOG("Invalid len %d\n", pData->hdr.width);
            goto End;
        }
    }

    switch ( action ) {
        case ZTE_MAC_ENTRY_ACTION_DEL:
            OAM_ORG_DEBUG("Del adapt called\n");
            ret = zte_oam_hg_mac_pool_del(pData->entries, pData->mac);
            break;

        case ZTE_MAC_ENTRY_ACTION_ADD:
            OAM_ORG_DEBUG("Add adapt called\n");
            ret = zte_oam_hg_mac_pool_add(pData->entries, pData->mac);
            break;

        case ZTE_MAC_ENTRY_ACTION_CLR:
            OAM_ORG_DEBUG("Clr adapt called\n");
            ret = zte_oam_hg_mac_pool_clr( );
            break;

        default:
            OAM_ORG_LOG ("unknow action=%d", action);
            goto End;
    }

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    return sizeof(oam_var_cont_t);

End:
    pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_hg_mac_pool_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_mc_address_pool *pMacpool = (oam_zte_onu_mc_address_pool *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 i = 0;
    oam_uint8 *tmplen = NULL;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = zte_oam_hg_mac_pool_get_adapt(&pMacpool->entries, pMacpool->mac);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        pMacpool->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("entries = %d\n", pMacpool->entries);

    for (i = 0; i < pMacpool->entries; i ++) {
        tmplen = pMacpool->mac + i * OAM_MACADDR_LEN;
        OAM_ORG_DEBUG("Mac = %#x:%#x:%#x:%#x:%#x:%#x \n",
                      *(tmplen), *(tmplen + 1), *(tmplen + 2),
                      *(tmplen + 3), *(tmplen + 4), *(tmplen + 5));
    }

    pMacpool->hdr.width = 2 + 6 * pMacpool->entries;
    pMacpool->action = ZTE_MAC_ENTRY_ACTION_LST;
    return sizeof(oam_zte_onu_mc_address_pool);
}

oam_uint32 zte_oam_hg_mgmt_vlan_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_onu_manage_vlan *pData = (oam_zte_onu_manage_vlan *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    OAM_ORG_DEBUG("Manage vlan = %d\n", ntohs(pData->manage_vlan));
    ret = zte_oam_hg_mgmt_vlan_set_adapt(ntohs(pData->manage_vlan));

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_hg_mgmt_vlan_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_onu_manage_vlan *pData = (oam_zte_onu_manage_vlan *)pOut;
    oam_uint16 mvlan = 0;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    ret = zte_oam_hg_mgmt_vlan_get_adapt(&mvlan);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("manage vlan = %d\n", mvlan);
    pData->hdr.width = 2;
    pData->manage_vlan = htons(mvlan);
    return sizeof(oam_zte_onu_manage_vlan);
}

oam_uint32 zte_oam_hg_report_state_set(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8    *pRecv,
    oam_uint32    Recvlen,
    oam_uint8    *pOut)
{
    oam_zte_hg_report_state *pData = (oam_zte_hg_report_state *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pOutHdr->width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("state = %d , Mac = %#x:%#x:%#x:%#x:%#x:%#x \n",
                  pData->hg_report_state, pData->home_gateway[0], pData->home_gateway[1],
                  pData->home_gateway[2],
                  pData->home_gateway[3], pData->home_gateway[4], pData->home_gateway[5]);

    ret = zte_oam_hg_report_state_set_adapt(index.idxValue , pData->home_gateway,
                                            pData->hg_report_state);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_oam_hg_report_state_get(
    ctc_oam_mgmt_obj_idx_t index,
    oam_uint8 *pRecv,
    oam_uint32 *procRecvLen,
    oam_uint8 *pOut)
{
    oam_zte_hg_report_state *pData = (oam_zte_hg_report_state *)pOut;
    oam_uint8 report_state = 0;
    oam_uint8 hg[OAM_MACADDR_LEN] = {0};
    oam_status ret = OAM_E_OK;

    OAM_CTC_FUNCTION_ENTER(index);

    if (!index.valid || index.idxType != OAM_CTC_ETH_PORT ) {
        OAM_ORG_LOG("port index is invalid \n");
        pData->hdr.width  = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    ret = zte_oam_hg_report_state_get_adapt(index.idxValue , hg, &report_state);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("state = %d , Mac = %#x:%#x:%#x:%#x:%#x:%#x \n",
                  report_state, hg[0], hg[1], hg[2], hg[3], hg[4], hg[5]);

    pData->hdr.width = 7;
    memcpy( pData->home_gateway, hg, OAM_MACADDR_LEN );
    pData->hg_report_state = report_state;
    return sizeof(oam_var_cont_t) + pData->hdr.width;
}

void hg_send_match_event(oam_uint8 port,  oam_uint8 *message_infor )
{
    oam_uint32 len = 0;
    oam_uint8 pdu[OAM_MAX_OAMPDU_SIZE];
    oam_zte_hg_report_match_mac_event_t *pEvent = NULL ;
    oam_if_t *oamif =  NULL ;
    oam_uint16 portId = port;

    oamif = oam_intf_find_valid();

    if (!oamif)
        return;

    oam_build_pdu_hdr(oamif, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);

    pEvent = (oam_zte_hg_report_match_mac_event_t *)(pdu + sizeof(oam_pdu_hdr_t));
    memcpy(pEvent->oui, oam_oui_ctc, OAM_OUI_LEN);
    pEvent->opCode = OAM_PDU_ZTE_VAR_RESP ;

    pEvent->index.hdr.branch = OAM_CTC_INDEX_BRANCH ; /* 0x36 */
    pEvent->index.hdr.leaf = htons(OAM_CTC_LEAF_CODE_ONU_PORT); /* 0x1 */
    pEvent->index.hdr.width = 0x01;
    pEvent->index.value = portId ;
    pEvent->container.hdr.branch = OAM_CTC_ATTRIB_BRANCH ; /* 0xC7 */
    pEvent->container.hdr.leaf = htons(ZTE_OAM_REPORT_MATCHED_MAC) ;
    pEvent->container.hdr.width = 2 + strlen((char *)message_infor) + 1 ;
    pEvent->container.message_id = htons(0x1006);
    strcpy((char *)pEvent->container.message_info, (char *)message_infor);

    len = sizeof(oam_pdu_hdr_t) + sizeof(oam_zte_hg_report_match_mac_event_t) +
          strlen((char *)message_infor)  ;

    zte_oam_send(oamif->llid, pdu, len);
}


ctc_oam_handle_t g_ctc_zte_oam_xlat_table[] = {
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC)   { BRANCH, LEAF, GET_FUNC, SET_FUNC },
    CTC_ZTE_OAM_BUILD_RESP
};

oam_uint32 zte_prv_oam_cap_get(
    oam_uint8 *pOut)
{
    oam_pdu_zte_oam_cap *cap = (oam_pdu_zte_oam_cap *)pOut;
    cap->hdr.width = 1;
    cap->cap = ZTE_OAM_SUPPORT;

    OAM_ORG_DEBUG("Cap = %d\n", cap->cap);
    return sizeof(oam_pdu_zte_oam_cap);
}

oam_uint32 zte_prv_oam_onu_sn_get(
    oam_uint8 *pOut)
{
    oam_pdu_tw_onu_sn *pData = (oam_pdu_tw_onu_sn *)pOut;
    oam_status ret = OAM_E_OK;
    oam_uint8 tw_onu_sn[129] = "ONU";
    oam_uint8  tw_sn_len = 3;

    ret = zte_prv_oam_onu_sn_get_adapt(tw_onu_sn, &tw_sn_len);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        pData->hdr.width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("onu_sn_len = %d\n", tw_sn_len);
    pData->hdr.width = tw_sn_len;
    memcpy(pData->sn, tw_onu_sn, tw_sn_len);
    return sizeof(oam_pdu_tw_onu_sn) + (tw_sn_len - 1);
}

oam_uint32 zte_prv_oam_remote_conn_set(
    oam_uint8 portId,
    oam_uint8 *pRecv,
    oam_uint8 *pOut)
{
    oam_pdu_tw_onu_remote_connect *pData = (oam_pdu_tw_onu_remote_connect *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_zte_onu_remote_conn_t   cfg;
    oam_status ret = OAM_E_OK;

    memset(&cfg, 0, sizeof(oam_zte_onu_remote_conn_t));

    if (pData->len != 24) {
        OAM_ORG_LOG("Invalid len = %d\n", pData->len);
        pOutHdr->width = TW_OAM_RESULT_WRONG_LENGTH;
        return sizeof(oam_var_cont_t);
    }
    else if ((pData->priority > 7) ||
             (ntohs(pData->vlan) > 4094) ||
             (pData->enable < 1) ||
             (pData->enable > 2) ||
             ((pData->enable == 1) &&
              zte_oam_check_ip_address_adapt(ntohl(pData->remote_ip), ntohl(pData->mask),
                      ntohl(pData->gateway)))) {
        /* remote_ip/mask/gw will be applied to IP stack, have to check first
         do not check host_ip/host_mask, that will be a subnet ip*/
        OAM_ORG_LOG("Invalid param\n");
        pOutHdr->width = TW_OAM_RESULT_WRONG_VALUE;
        return sizeof(oam_var_cont_t);
    }

    cfg.remote_ip =  ntohl(pData->remote_ip);
    cfg.mask =  ntohl(pData->mask);
    cfg.priority =  pData->priority;
    cfg.vlan =  ntohs(pData->vlan);
    cfg.enable =  pData->enable;
    cfg.host_ip =  ntohl(pData->host_ip);
    cfg.host_mask =  ntohl(pData->host_mask);
    cfg.gateway =  ntohl(pData->gateway);

    OAM_ORG_DEBUG("remote_ip 0x%x, mask 0x%x,pri %d,vlan 0x%x\n", cfg.remote_ip,
                  cfg.mask,
                  cfg.priority, cfg.vlan);
    OAM_ORG_DEBUG("enable %d, h_ip 0x%x,h_mask 0x%x,gateway 0x%x\n", cfg.enable,
                  cfg.host_ip,
                  cfg.host_mask, cfg.gateway);

    ret = zte_oam_remote_conn_set_adapt(&cfg);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = TW_OAM_RESULT_WRONG_VALUE;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = TW_OAM_RESULT_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_prv_oam_remote_conn_get(
    oam_uint8 *pOut)
{
    oam_pdu_tw_onu_remote_connect *remote_cnt = (oam_pdu_tw_onu_remote_connect *)
            pOut;
    oam_zte_onu_remote_conn_t  ipConfig;
    oam_status ret = OAM_E_OK;

    ret = zte_oam_remote_conn_get_adapt(&ipConfig);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        remote_cnt->len = TW_OAM_RESULT_BAD_VALUE;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG("remote_ip 0x%x, mask 0x%x,pri 0x%x,vlan 0x%x\n",
                  remote_cnt->remote_ip,
                  remote_cnt->mask, remote_cnt->priority, remote_cnt->vlan);
    OAM_ORG_DEBUG("enable 0x%x, host_ip 0x%x,host_mask 0x%x,gateway 0x%x,\n",
                  remote_cnt->enable,
                  remote_cnt->host_ip, remote_cnt->host_mask, remote_cnt->gateway);

    remote_cnt->len = 24;
    remote_cnt->remote_ip = htonl(ipConfig.remote_ip);
    remote_cnt->mask = htonl(ipConfig.mask);
    remote_cnt->priority = ipConfig.priority;
    remote_cnt->vlan = htons(ipConfig.vlan);
    remote_cnt->enable = ipConfig.enable;
    remote_cnt->host_ip = htonl(ipConfig.host_ip);
    remote_cnt->host_mask = htonl(ipConfig.host_mask);
    remote_cnt->gateway = htonl(ipConfig.gateway);
    return sizeof(oam_pdu_tw_onu_remote_connect);
}

oam_uint32 zte_prv_oam_onu_config(
    oam_uint8 portId,
    oam_uint8 *pRecv,
    oam_uint8 *pOut)
{
    oam_pdu_tw_onu_configuration *pData =
        (oam_pdu_tw_onu_configuration *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;

    OAM_ORG_DEBUG( "Action %d \n", pData->action);

    switch (pData->action) {
        case 1:
            /* SAVE CONFIG */
            OAM_ORG_DEBUG( "Save config\n");
            zte_oam_onu_config_save_adapt();
            break;

        case 2:
            /* CLEAR CURRENT CONFIG */
            OAM_ORG_DEBUG( "Clear current config\n");
            zte_oam_onu_config_clear_adapt();
            break;

        case 3:
            /* RESET TO FACTORY */
            OAM_ORG_DEBUG( "Restore to factory\n");
            zte_oam_factory_setting_restore_adapt();
            break;

        default:
            OAM_ORG_LOG("Unknown action.\n");
            pOutHdr->width = OAM_ZTE_VAR_SET_NO_RES;
            return sizeof(oam_var_cont_t);
            break;
    }

    pOutHdr->width = TW_OAM_RESULT_OK;
    return sizeof(oam_var_cont_t);
}


oam_uint32 zte_prv_oam_port_attr_set(
    oam_uint8 portId,
    oam_uint8 *pRecv,
    oam_uint8 *pOut)
{
    oam_zte_onu_port_attr *pCfg = (oam_zte_onu_port_attr *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_status ret = OAM_E_OK;

    OAM_ORG_DEBUG( "mode 0x%x \n", pCfg->mode);
    ret = zte_oam_port_attr_set_adapt(portId, pCfg->mode);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_prv_oam_port_attr_get(
    oam_uint8 portId,
    oam_uint8 *pOut)
{
    oam_zte_onu_port_attr *pCfg = (oam_zte_onu_port_attr *)pOut;
    zte_oam_port_mode_e mode;
    oam_status ret = OAM_E_OK;

    pCfg->hdr.width = 0x01;

    ret = zte_oam_port_attr_get_adapt(portId, &mode);
    OAM_ORG_DEBUG( "mode 0x%x \n", mode);
    pCfg->mode = mode;

    if (ret != OAM_E_OK) {
        pCfg->hdr.width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    return sizeof(oam_zte_onu_port_attr);
}

oam_uint32 zte_prv_oam_port_work_attr_get(
    oam_uint8 portId,
    oam_uint8 *pOut)
{
    oam_zte_onu_port_attr *pCfg = (oam_zte_onu_port_attr *)pOut;
    zte_oam_port_mode_e mode;
    oam_status ret = OAM_E_OK;

    pCfg->hdr.width = 0x01;

    ret = zte_oam_port_work_attr_get_adapt(portId, &mode);
    OAM_ORG_DEBUG( "mode 0x%x \n", mode);
    pCfg->mode = mode;

    if (ret != OAM_E_OK) {
        pCfg->hdr.width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    return sizeof(oam_zte_onu_port_attr);
}

oam_uint32 zte_prv_oam_typec_config(
    oam_uint8 portId,
    oam_uint8 *pRecv,
    oam_uint8 *pOut)
{
    oam_zte_onu_typec_t *pCfg = (oam_zte_onu_typec_t *)pRecv;
    oam_var_cont_t *pOutHdr = (oam_var_cont_t *)pOut;
    oam_uint8 k1 = 0, k2 = 0;
    oam_status ret = OAM_E_OK;

    k1 = pCfg->k1;
    k2 = pCfg->k2;
    OAM_ORG_DEBUG( "k1 0x%x, k2 0x%x\n", k1, k2);
    ret = zte_prv_oam_typec_config_adapt(k1, k2);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Set adapt return failed, ret = %d \n", ret);
        pOutHdr->width = OAM_ZTE_VAR_SET_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    pOutHdr->width = OAM_ZTE_VAR_SET_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 zte_prv_oam_typec_status_get(
    oam_uint8 *pOut)
{
    oam_zte_onu_typec_t *pRsp = (oam_zte_onu_typec_t *)pOut;
    oam_uint8 k1 = 0, k2 = 0;
    oam_status ret = OAM_E_OK;

    ret = zte_prv_oam_typec_status_get_adapt(&k1, &k2);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("Get adapt return failed, ret = %d \n", ret);
        pRsp->hdr.width = TW_OAM_RESULT_BAD_VALUE;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_DEBUG( "k1 0x%x, k2 0x%x\n", k1, k2);

    pRsp->hdr.width = 2;
    pRsp->k1 = k1;
    pRsp->k2 = k2;
    return sizeof(oam_zte_onu_typec_t);
}

oam_int32 oam_build_zte_var_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_int32 length)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_pdu_zte_t *pOutPkt = (oam_pdu_zte_t *)pdu;
    oam_pdu_zte_t *pRecvPkt = NULL;
    oam_uint32 len = 0, outPktLen = 0;
    oam_uint32 totalContentLen = 0;
    oam_var_desc_t *pHdrContainer = NULL;
    oam_var_cont_t *pOutContainer = NULL;

    oam_uint32 portId;
    oam_uint8 *pOutBuf;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    if (!frame ||
        length <= sizeof(oam_pdu_zte_t) + sizeof(oam_var_cont_t) - 1)
        return 0;

    if (intf == NULL) {
        return 0;
    }

    pRecvPkt = (oam_pdu_zte_t *)frame;
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
    pOutPkt->opcode = OAM_PDU_ZTE_VAR_RESP;
    pOutPkt->type = pRecvPkt->type;
    pOutPkt->port = pRecvPkt->port;
    outPktLen =  sizeof(oam_pdu_zte_t) - 1;
    totalContentLen = length - sizeof(oam_pdu_zte_t) + 1;

    portId = pRecvPkt->port;

    pHdrContainer = (oam_var_desc_t *)pRecvPkt->data;
    pOutContainer = (oam_var_cont_t *)pOutPkt->data;

    while (totalContentLen) {
        pOutBuf = (oam_uint8 *)pOutContainer;
        pOutContainer->branch = pHdrContainer->branch;
        pOutContainer->leaf = pHdrContainer->leaf;

        switch (pHdrContainer->branch) {
            case OAM_ATTRIB_BRANCH_NULL:
                goto send_resp;

            case ZTE_OAM_EXT_BRANCH_ONU: {
                switch (htons(pHdrContainer->leaf)) {
                    case ZTE_OAM_EXT_LEAF_ZTE_OAM_CAPABILITY:
                        len = zte_prv_oam_cap_get(pOutBuf);
                        break;

                    case ZTE_OAM_EXT_LEAF_ONU_SERIAL:
                        len = zte_prv_oam_onu_sn_get(pOutBuf);
                        break;

                    case ZTE_OAM_EXT_LEAF_REMOTE_CONNECT:
                        len = zte_prv_oam_remote_conn_get(pOutBuf);
                        break;

                    case ZTE_OAM_EXT_LEAF_TYPEC_PROTECT_STATUS:
                        len = zte_prv_oam_typec_status_get(pOutBuf);
                        break;

                    default:
                        pOutContainer->width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
                        outPktLen += sizeof(oam_var_cont_t);
                        goto send_resp;

                }

                outPktLen += len;
                pOutContainer = (oam_var_cont_t *)(pOutBuf + len);
                break;
            }

            case ZTE_OAM_EXT_BRANCH_PORT: {
                switch (htons(pHdrContainer->leaf)) {
                    case ZTE_OAM_EXT_LEAF_PORT_ATTR:
                        len = zte_prv_oam_port_attr_get(portId, pOutBuf);
                        break;

                    case ZTE_OAM_EXT_LEAF_GET_PORT_WORKING_ATTR:
                        len = zte_prv_oam_port_work_attr_get(portId, pOutBuf);
                        break;

                    default:
                        pOutContainer->width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
                        outPktLen += sizeof(oam_var_cont_t);
                        goto send_resp;
                }

                outPktLen += len;
                pOutContainer = (oam_var_cont_t *)(pOutBuf + len);
                break;
            }

            default:
                pOutContainer->width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
                outPktLen += sizeof(oam_var_cont_t);
                goto send_resp;

        }

        totalContentLen -= sizeof(oam_var_desc_t);

        if (totalContentLen < sizeof(oam_var_desc_t))
            break;

        if (outPktLen > OAM_MAX_OAMPDU_SIZE - 2)
            break;

        pHdrContainer++;
    }

send_resp:
    zte_oam_send(llid, pdu, outPktLen);

    return outPktLen;
}

oam_int32 oam_build_zte_set_resp(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_int32 length)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    oam_pdu_zte_t *pOutPkt = (oam_pdu_zte_t *)pdu;
    oam_pdu_zte_t *pRecvPkt = NULL;
    oam_uint32 outPktLen = 0;
    oam_uint32 totalContentLen = 0;
    oam_uint32 recvContainerLen = 0;
    oam_ctc_onu_var_set_hdr_t *pHdrContainer = NULL;
    oam_var_cont_t *pOutHdr = NULL;

    oam_uint8 *pOutBuf = NULL;

    oam_uint32 portId = 0;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);

    if (!frame ||
        (length <= (sizeof(oam_pdu_zte_t) + sizeof(oam_var_cont_t) - 1)))
        return 0;

    if (intf == NULL) {
        return 0;
    }

    pRecvPkt = (oam_pdu_zte_t *)frame;
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
    pOutPkt->opcode = OAM_PDU_ZTE_SET_RESP;
    pOutPkt->type = pRecvPkt->type;
    portId = pOutPkt->port = pRecvPkt->port;
    outPktLen =  sizeof(oam_pdu_zte_t) - 1;
    totalContentLen = length - sizeof(oam_pdu_zte_t) + 1;

    pHdrContainer = (oam_ctc_onu_var_set_hdr_t *)pRecvPkt->data;
    pOutHdr = (oam_var_cont_t *)pOutPkt->data;

    while (totalContentLen) {
        pOutBuf = (oam_uint8 *)pOutHdr;
        pOutHdr->branch = pHdrContainer->branch;
        pOutHdr->leaf = pHdrContainer->leaf;

        switch ( pHdrContainer->branch) {
            case OAM_ATTRIB_BRANCH_NULL:
                goto send_resp;

            case ZTE_OAM_EXT_BRANCH_ONU:
                switch (htons(pHdrContainer->leaf)) {
                    case ZTE_OAM_EXT_LEAF_REMOTE_CONNECT:
                        zte_prv_oam_remote_conn_set(portId, (oam_uint8 *)pHdrContainer, pOutBuf);
                        break;

                    case ZTE_OAM_EXT_LEAF_ONU_CONFIGURATION:
                        zte_prv_oam_onu_config(portId, (oam_uint8 *)pHdrContainer, pOutBuf);
                        break;

                    case ZTE_OAM_EXT_LEAF_TYPEC_PROTECT_CONFIG:
                        zte_prv_oam_typec_config(portId, (oam_uint8 *)pHdrContainer, pOutBuf);
                        break;

                    default:
                        pOutHdr->width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
                        outPktLen += sizeof(oam_var_cont_t);
                        goto send_resp;
                }

                break;

            case ZTE_OAM_EXT_BRANCH_PORT:
                switch (htons(pHdrContainer->leaf)) {
                    case ZTE_OAM_EXT_LEAF_PORT_ATTR:
                        zte_prv_oam_port_attr_set(portId, (oam_uint8 *)pHdrContainer, pOutBuf);
                        break;

                    default:
                        pOutHdr->width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
                        outPktLen += sizeof(oam_var_cont_t);
                        goto send_resp;
                }

                break;

            default:
                pOutHdr->width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
                outPktLen += sizeof(oam_var_cont_t);
                goto send_resp;

        }

        outPktLen += sizeof(oam_var_cont_t);

        recvContainerLen = sizeof(oam_var_cont_t) + pHdrContainer->width;

        pOutBuf = (oam_uint8 *)pHdrContainer;
        pHdrContainer = (oam_ctc_onu_var_set_hdr_t *)(pOutBuf + sizeof(
                            oam_var_cont_t) + pHdrContainer->width);
        totalContentLen -= recvContainerLen;
        pOutHdr++;

        if (totalContentLen <= sizeof(oam_var_desc_t))
            break;

        if (outPktLen > OAM_MAX_OAMPDU_SIZE - 2)
            break;
    }

send_resp:
    zte_oam_send(llid, pdu, outPktLen);

    return outPktLen;
}

void
handle_zte_oams(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
{
    oam_pdu_zte_t *pOamFrame = (oam_pdu_zte_t *)frame;

    if (!frame || !len)
        return;

    switch (pOamFrame->opcode) {
        case OAM_PDU_ZTE_VAR_REQ:
            OAM_ORG_DEBUG("Handle Var req from LLID %d\r\n", llid);
            oam_build_zte_var_resp(llid, frame, len);
            break;

        case OAM_PDU_ZTE_SET_REQ:
            OAM_ORG_DEBUG("Handle set req from LLID %d\r\n", llid);
            oam_build_zte_set_resp(llid, frame, len);
            break;

        default:
            break;
    }

    return;

}

void
handle_unreco_oui_oams(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_pdu_ctc_t *pCTCOamFrame = (oam_pdu_ctc_t *)frame;
    oam_pdu_ctc_t *pOutPkt = (oam_pdu_ctc_t *)pdu;
    oam_if_t *intf = NULL;
    oam_uint8 opcode;
    oam_uint32 totalContentLen = 0;
    oam_uint32 outPktLen = 0;
    oam_uint32 tmplen = 0;
    oam_uint8 *pOutVarCont = pOutPkt->data;
    oam_var_desc_t *pRecvVarCont = NULL;

    if (!frame || !len)
        return;

    if (len < sizeof(oam_pdu_ctc_t) + sizeof(oam_var_desc_t) - 1)
        return;

    OAM_LOG_INFO("handle unrecognized oui frame.\n");

    opcode = pCTCOamFrame->opcode;

    if (OAM_PDU_CTC_VAR_REQ != opcode && OAM_PDU_CTC_SET_REQ != opcode) {
        OAM_LOG_INFO("Unknown opcode.\n");
        return;
    }

    intf = oam_intf_find(llid);

    if (!intf) {
        oam_printf("handle_unreco_oui_oams at unknown port 0x%x\n", llid);
        return;
    }

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);

    memcpy(pOutPkt->oui, pCTCOamFrame->oui, OAM_OUI_LEN);

    if (OAM_PDU_CTC_VAR_REQ == opcode) {
        pOutPkt->opcode = OAM_PDU_CTC_VAR_RESP;
    }
    else if (OAM_PDU_CTC_SET_REQ == opcode) {
        pOutPkt->opcode = OAM_PDU_CTC_SET_RESP;
    }

    outPktLen =  sizeof(oam_pdu_ctc_t) - 1;
    pRecvVarCont = (oam_var_desc_t *)pCTCOamFrame->data;
    totalContentLen = len - sizeof(oam_pdu_ctc_t) + 1;

    tmplen = 0;

    if (OAM_CTC_INDEX_BRANCH == pRecvVarCont->branch) {
        tmplen = sizeof(oam_pdu_ctc_index_t);
    }
    else if (OAM_CTC_INDEX_BRANCH_E == pRecvVarCont->branch) {
        tmplen = sizeof(oam_pdu_ctc_port_index_t);
    }

    if (0 != tmplen) {
        memcpy(pOutVarCont, (oam_uint8 *)pRecvVarCont, tmplen);
        outPktLen += tmplen;
        pOutVarCont += tmplen;
        pRecvVarCont = (oam_var_desc_t *)((oam_uint8 *)pRecvVarCont + tmplen);
        totalContentLen -= tmplen;
    }

    if (totalContentLen < sizeof(oam_var_desc_t))
        return;

    memcpy(pOutVarCont, (oam_uint8 *)pRecvVarCont, sizeof(oam_var_desc_t));
    ((oam_var_cont_t *)pOutVarCont)->width = OAM_ZTE_ATTRIB_NOT_SUPPORTED;
    outPktLen += sizeof(oam_var_cont_t);
    pOutVarCont += sizeof(oam_var_cont_t);
    *pOutVarCont++ = 0;
    *pOutVarCont++ = 0;
    outPktLen += 2;

    zte_oam_send(llid, pdu, outPktLen);

}

#ifdef HAVE_LOOP_DETECT
void zte_oam_loop_detect_handle(oam_port_id_t port, oam_boolean status,
                                oam_uint32 info)
{
    oam_uint64 alarm_info = 0;
    oam_uint8 alarm_op = CTC_ALARM_RAISE;
    oam_uint16 alarm_id = ZTE_OAM_LOOP_DETECT;

    /*build CTC event*/
    if (oam_is_cuc_oam()) {
        alarm_info = info ? CUC_OUT_LOOP_VALUE : CUC_INNER_LOOP_VALUE;
    }

    if (status) {
        alarm_op = CTC_ALARM_RAISE;
        alarm_id = ZTE_OAM_LOOP_DETECT;

    }
    else {
        alarm_op = CTC_ALARM_CLEAR;
        alarm_id = ZTE_OAM_LOOP_RECOVER;
    }

    ctc_oam_build_alarm_event(CTC_PORT_ALARM, port, CTC_ALARM_PORT_LB,
                              alarm_op, alarm_info);
    /*build ZTE event*/
    oam_build_zte_extend_alarm_raised(port, alarm_id);

}
#endif

void zte_oam_init()
{
    oam_vendor_handlers_t zte_handle;

    oam_vendor_handlers_init(&zte_handle);
    zte_handle.org_handler = handle_zte_oams;

    oam_vendor_handler_register(oam_oui_zte, zte_handle);

    zte_flux_stat_init();
#ifdef HAVE_LOOP_DETECT
    cs_ld_alarm_func_reg(zte_oam_loop_detect_handle);
    /*by default, enable all uni */
    cs_ld_enable_set(OAM_DEV_ID, OAM_ALL_UNI_PORT_ID, TRUE);
#endif

    ctc_oam_vendor_std_handler_reg(oam_oui_ctc, g_ctc_zte_oam_xlat_table,
                                   sizeof(g_ctc_zte_oam_xlat_table) / sizeof(ctc_oam_handle_t));

    zte_oam_port_link_status_reg();
    zte_oam_gpio_lao_report_reg_adapt();
    
    db_restore_register();

}
#endif/*END_ZTE_SPECIFIC*/

