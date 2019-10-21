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
#include <time.h>

#include "oam_types.h"
#include "oam_cmn.h"
#include "oam_std_pdu.h"
#include "oam_dpoe_pdu.h"
#include "oam_adapt.h"
#include "oam_dpoe_adapt.h"
#include "oam_port_stats.h"
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
#include "l2.h"
#include "eth_port.h"
#include "rate.h"
#include "port.h"
#include "mcast.h"
#endif

#include "epon.h"
#include "oam_dpoe_frame.h"
#include "opm_api.h"
#include "ca_version.h"
#include "scfg.h"
#include "queue.h"
#include "ca_version.h"
#include "qos.h"

//#define UNI_PORT_XX(x) ca_uint32 eid = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, 5+x);

oam_dpoe_llid_queue_int_t g_llid_queue_config;

static oam_uint32 g_laser_tx_power_timer = 0;

extern dpoe_rule_configured g_dpoe_rule[16];
extern oam_uint8 g_dpoe_cls_port_map_counter;
extern dpoe_cls_port_map g_dpoe_cls_port_map[16];
extern void _dpoe_find_mc_llid_index(oam_uint16 mc_llid,ca_llid_t *llid_index);
extern oam_status __dpoe_oam_traffic_cfg(oam_boolean en,ca_llid_t llid_index, oam_llid_t llid);
extern void __dpoe_oam_default_traffic_cfg_init();
static ca_status_t __dpoe_cls_rule_check_for_mc(ca_uint32_t cls_index,
    ca_uint32_t                 new_priority,
    ca_classifier_key_t         *new_key,
    ca_classifier_key_mask_t    *new_key_mask,
    ca_classifier_action_t      *new_action,
    ca_uint32_t                 *new_cls_index);

void dpoe_llid_queue_adapt_init()
{
    int i=0, j=0;
    ca_device_config_tlv_t scfg_tlv;
    memset(&scfg_tlv,0,sizeof(ca_device_config_tlv_t));
    memset(&g_llid_queue_config, 0, sizeof(oam_dpoe_llid_queue_int_t));
    scfg_tlv.type = CA_CFG_ID_PON_LLID_NUM;
    scfg_tlv.len =  CFG_ID_PON_LLID_NUM;
    ca_status_t ret = CA_E_OK;
    ca_boolean_t register_enable = FALSE;
    oam_uint8 llid_num = 0;

    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if ( CA_E_OK != ret )
    {
        OAM_LOG_DEBUG("read CA_CFG_ID_PON_LLID_NUM fail. ret = %d \r\n", ret);
        return ;
    }
    llid_num = scfg_tlv.value[0];
    g_llid_queue_config.n_port = OAM_UNI_PORT_NUM;

    for(i=0;i<llid_num;i++){
       ret = ca_epon_mpcp_registration_get(0,CA_PORT_ID(CA_PORT_TYPE_EPON,0x07),i,&register_enable);
       if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_mpcp_registration_get failed\n\r");
            continue;
       }
       if(register_enable){
            g_llid_queue_config.n_llid++;
       }
    }

    if(0 == g_llid_queue_config.n_llid){
        g_llid_queue_config.n_llid = 1;
    }

    for (i = 0; i < g_llid_queue_config.n_llid; i++)
    {
        g_llid_queue_config.llid[i].n_queue = 8;
        for (j = 0; j < 8; j++){
            g_llid_queue_config.llid[i].q[j] = 0x64;
        }
    }
    for (i = 0; i < OAM_UNI_PORT_NUM; i++)
    {
        g_llid_queue_config.port[i].n_queue = 8;
        for (j = 0; j < 8; j++)
            g_llid_queue_config.port[i].q[j] = 0x64;
    }
}


void _dpoe_save_queue_cfg(oam_uint8 type, oam_uint16 port, oam_uint8 count, oam_uint32 *size)
{
    int i;
    oam_uint32 *ptr = size;
    if (type == OAM_DPOE_LEAF_OBJ_UNI)
    {
        if ((port == 0) || (port > OAM_UNI_PORT_NUM))
            return;
        g_llid_queue_config.port[port - 1].n_queue = count;
        for (i = 0; i < count; i++)
        {
            g_llid_queue_config.port[port - 1].q[i] = *ptr;
            ptr++;
        }
    }
    else
    {
        if ((port == 0) || (port > 8))
            return;
        g_llid_queue_config.llid[port - 1].n_queue = 1;
        g_llid_queue_config.llid[port - 1].q[0] = *ptr;
    }
}


void _dpoe_get_queue_cfg(oam_uint8 type, oam_uint16 port, oam_uint8 *count, oam_uint32 *size)
{
    int i;
    oam_uint32 *ptr = size;
    if (type == OAM_DPOE_LEAF_OBJ_UNI)
    {
        if ((port == 0) || (port > 4))
            return;
        *count = g_llid_queue_config.port[port - 1].n_queue;
        for (i = 0; i < *count; i++)
        {
            *ptr = g_llid_queue_config.port[port - 1].q[i];
            ptr++;
        }
    }
    else
    {
        if ((port == 0) || (port > 8))
            return;
        *count = 1;
        *ptr = g_llid_queue_config.llid[port - 1].q[0];
    }
}


/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_onu_id_get_adapt                                 */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status dpoe_oam_onu_id_get_adapt(oam_uint8 *onu_id)
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : o onu_id - ONU ID                                               */
/* DESCRIPTION:                                                              */
/* get ONU ID                                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status ret = OAM_E_OK;

    if (!onu_id) {
        return OAM_E_ERROR;
    }
    oam_llid_mac_get_adapt(0, onu_id);
    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_max_llid_get_adapt                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status dpoe_oam_max_llid_get_adapt(
    oam_uint16 *bidir,
    oam_uint16 *ds_only)
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : o bidir - bidirection LLID number                               */
/*           o ds_only - downstream only LLID number                         */
/* DESCRIPTION:                                                              */
/* get ONU ID                                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    //OAM_ASSERT_RET(bidir   != NULL, OAM_E_PARAM);
    //OAM_ASSERT_RET(ds_only != NULL, OAM_E_PARAM);
    ca_device_config_tlv_t scfg_tlv;
    ca_status_t ret = CA_E_OK;
    oam_uint8 llid_num = 0;

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_PON_LLID_NUM;
    scfg_tlv.len = sizeof(ca_uint8_t);

    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if (CA_E_OK != ret) {
        OAM_ORG_LOG("read CA_CFG_ID_PON_LLID_NUM fail\r\n");
        return OAM_E_ERROR;
    }

    memcpy(&llid_num,scfg_tlv.value,sizeof(ca_uint8_t));

    *bidir = llid_num;
    *ds_only = llid_num;

    return OAM_E_OK;
}

static ca_uint16_t    g_oam_threshold1 = 0x0800;
static ca_uint16_t    g_oam_threshold2 = 0x0800;
static ca_uint16_t    g_oam_threshold3 = 0x0800;
static ca_uint16_t    g_oam_threshold4 = 0x0800;
static oam_uint8      g_oam_queueset_num = 0x04;
static oam_uint8      g_oam_value_per_set = 0x01;
/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_report_thresh_get_adapt                          */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status dpoe_oam_report_thresh_get_adapt(
    oam_uint8 index,
    oam_uint8 *queueset_num,
    oam_uint8 *value_per_set,
    oam_uint16 *report_thresh)
/*                                                                           */
/* INPUTS  : o index         - 0 based llid index                            */
/* OUTPUTS : o queueset_num  - queue set number                              */
/*           o value_per_set - queue number per set                          */
/*           o report_thresh - report threshold                              */
/* DESCRIPTION:                                                              */
/* get ONU ID                                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    OAM_ASSERT_RET(queueset_num  != NULL, OAM_E_PARAM);
    OAM_ASSERT_RET(value_per_set != NULL, OAM_E_PARAM);
    OAM_ASSERT_RET(report_thresh != NULL, OAM_E_PARAM);

    *queueset_num = g_oam_queueset_num;
    *value_per_set = g_oam_value_per_set;
    *report_thresh = g_oam_threshold1;
    *(report_thresh+1) = g_oam_threshold2;
    *(report_thresh+2) = g_oam_threshold3;
    *(report_thresh+3) = g_oam_threshold4;

    OAM_ORG_LOG("dpoe_oam_report_thresh_get_adapt:g_oam_threshold1=0x%04x,g_oam_threshold2=0x%04x,g_oam_threshold3=0x%04x,g_oam_threshold4=0x%04x\n\r",
        g_oam_threshold1,
        g_oam_threshold2,
        g_oam_threshold3,
        g_oam_threshold4);
    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_report_thresh_set_adapt                          */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status dpoe_oam_report_thresh_set_adapt(
    oam_uint8 index,
    oam_uint8 queueset_num,
    oam_uint8 value_per_set,
    oam_uint16 *report_thresh)
/*                                                                           */
/* INPUTS  : o index         - 0 based llid index                            */
/*           o queueset_num  - queue set number                              */
/*           o value_per_set - queue number per set                          */
/*           o report_thresh - report threshold                              */
/* OUTPUTS :                                                                 */
/* DESCRIPTION:                                                              */
/* get ONU ID                                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    ca_uint16_t    threshold1 = 0;
    ca_uint16_t    threshold2 = 0;
    ca_uint16_t    threshold3 = 0;
    ca_uint16_t    threshold4 = 0;
    ca_device_config_tlv_t  scfg_cfg;
    ca_status_t ret = CA_E_OK;
    oam_uint8 report_mode = 0;

    memset(&scfg_cfg,0,sizeof(ca_device_config_tlv_t));
    OAM_ASSERT_RET(report_thresh != NULL, OAM_E_PARAM);

    ca_port_id_t ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();

#if  defined(CONFIG_ARCH_CORTINA_SATURN_SFU)  || defined(CONFIG_ARCH_CORTINA_G3HGU)
    scfg_cfg.type = CA_CFG_ID_PON_REPORT_MODE;
    scfg_cfg.len = 1;
    ret = ca_device_config_tlv_get(0,&scfg_cfg);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_report_thresh_set_adapt: read CA_CFG_ID_PON_REPORT_MODE\n\r");
        return ret;
    }
    report_mode = scfg_cfg.value[0];
    if(4 == report_mode){
        threshold1 = 0x3C00;
        threshold2 = 0x7800;
        threshold3 = 0xB400;
        threshold4 = 0xF000;
    }else{
        threshold1 = *report_thresh;
        threshold2 = *(report_thresh+1);
        threshold3 = *(report_thresh+2);
        threshold4 = *(report_thresh+3);
    }
#else
    threshold1 = *report_thresh;
    threshold2 = *(report_thresh+1);
    threshold3 = *(report_thresh+2);
    threshold4 = *(report_thresh+3);

#endif
    OAM_ORG_LOG("dpoe_oam_report_thresh_set_adapt:threshold1 = 0x%04x,threshold2 =0x%04x,threshold3 = 0x%04x,threshold4 =0x%04x ,queueset_num = %d,value_per_set =%d\n\r",
                threshold1,threshold2,threshold3,threshold4,queueset_num, value_per_set);
    ret = ca_epon_queue_set_thresholds_set(0,ca_port,index,threshold1,threshold2,threshold3,threshold4);
    if(ret!=CA_E_OK){
        OAM_ORG_LOG("ca_epon_mpcp_thresholds_set failed\n\r");
        return ret;
    }
    g_oam_threshold1 = *report_thresh;
    g_oam_threshold2 = *(report_thresh+1);
    g_oam_threshold3 = *(report_thresh+2);
    g_oam_threshold4 = *(report_thresh+3);
    g_oam_queueset_num  = queueset_num;
    g_oam_value_per_set = value_per_set ;
    return OAM_E_OK;
}


oam_uint8 dpoe_oam_llid_fwd_state_get_adapt(oam_uint8 llid)
{
    ca_boolean_t upstream = FALSE,downstream = FALSE ;
    ca_status_t ret = CA_E_OK;

    ret = ca_epon_llid_traffic_enable_get(0,CA_PORT_ID(CA_PORT_TYPE_EPON,0x07),llid,&upstream,&downstream);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_llid_fwd_state_get_adapt:ca_epon_llid_traffic_enable_get failed\n\r");
        return 0;
    }

    OAM_ORG_LOG("ca_epon_llid_traffic_enable_get:upstream =%d\n\r",upstream);
    if(upstream){
        return 0;
    }else{
        return 1;
    }
}

void dpoe_oam_fw_info_get_adapt(fw_info_t *info)
{
    //ca_uint8 tmp[12] = {0x03, 0x27, 0x82, 0x6a, 0x85, 0x58, 0x03, 0x29, 0x83, 0x07, 0x55, 0x17};
    memset(info, 0, sizeof(fw_info_t));
    info->bootver = 0;
    info->bootcrc = 0x826a8558;
    info->fwver = 0x0327;
    info->fwcrc = 0x83075517;

}

void dpoe_oam_chip_info_get_adapt(chip_info_t *info)
{
    //ca_uint8 tmp[10] = {0x02, 0x03, 0x00, 0x00, 0x47, 0x01, 0xb2, 0x11, 0x08, 0x16};
    memset(info, 0, sizeof(chip_info_t));
    info->jedecid = 0x0203;
    info->chip_model = 0x4701;
    info->chip_ver = 0x1b21108;
}

void dpoe_oam_manuday_get_adapt(manuday_t *info)
{
    info->year = 0x2016;
    info->month = 1;
    info->day = 1;
}


void dpoe_oam_manuinfo_get_adapt(oam_uint8 *info)
{

    ca_uint8_t manu_info[64] = {0x38,0x32,0x30,0x30,0x30,0x42,0x41,0x32,0x46,0x54,0x45,0x37,0x35,0x30,0x32,0x30,
                        0x42,0x43,0x4d,0x35,0x35,0x30,0x33,0x30,0x53,0x75,0x6d,0x69,0x74,0x6f,0x6d,0x6f,
                        0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x55,0x41,0x30,0x41,0x30,0x30,0x52,0x32,
                        0x31,0x31,0x30,0x30,0x4c,0x30,0x31,0x30,0x31,0x36,0x30,0x36,0x32,0x32,0x62,0x36};
    memcpy(info, manu_info, 64);
}

void dpoe_oam_manuf_org_name_get_adapt(oam_uint8 *org_name){
    ca_cert_info_t cert_data;
    ca_status_t ret;
    oam_uint8 *tmp_org_name = NULL,*tmp_sub_org_name = NULL;
    oam_uint8 i =0;
    oam_uint8 *default_org_name = "CommScope, Inc. of North Carolina";
    memset(&cert_data,0,sizeof(ca_cert_info_t));

    ret = ca_cert_info_get(0,&cert_data);
    if(CA_E_OK != ret){
        memcpy(org_name,default_org_name,strlen(default_org_name));
        return;
    }
    //subject= /C=US/O=CommScope, Inc. of North Carolina/OU=US06/CN=00:24:45:A0:11:08
    tmp_org_name = strstr(cert_data.org_name,"O=");
    if(tmp_org_name){
        tmp_sub_org_name = tmp_org_name;
        for(i=0;i<strlen(tmp_org_name);i++){
            if('/' == *(tmp_sub_org_name+i))
                break;
        }
        memcpy(org_name,(tmp_org_name+2),(i-2));
    }else{
        memcpy(org_name,default_org_name,strlen(default_org_name));
    }
    return;
}

void dpoe_oam_fw_mfg_get_adapt(fw_mfg_time_t *time)
{
    ca_cert_info_t cert_data;
    ca_status_t ret;
    ca_uint8_t default_code[13]= {0x31,0x38,0x30,0x31,0x30,0x31,0x31,0x32,0x30,0x30,0x30,0x30,0x5a};
    ca_uint8_t tmp_cvc_code[13];
    ca_uint8_t tmp_code[13];
    memset(&cert_data,0,sizeof(ca_cert_info_t));
    memset(tmp_cvc_code,0,13);
    memset(tmp_code,0,13);

    ret = ca_cert_info_get(0,&cert_data);
    if(CA_E_OK != ret){
        memcpy(time->code,default_code,13);
        memcpy(time->cvc,default_code,13);
        return;
    }
    tmp_cvc_code[0] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].year/10%10+'0';
    tmp_cvc_code[1] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].year%10+'0';
    tmp_cvc_code[2] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].month/10+'0';
    tmp_cvc_code[3] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].month%10+'0';
    tmp_cvc_code[4] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].day/10+'0';
    tmp_cvc_code[5] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].day%10+'0';
    tmp_cvc_code[6] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].hour/10+'0';
    tmp_cvc_code[7] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].hour%10+'0';
    tmp_cvc_code[8] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].minute/10+'0';
    tmp_cvc_code[9] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].minute%10+'0';
    tmp_cvc_code[10] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].second/10+'0';
    tmp_cvc_code[11] = cert_data.date[CA_CERT_CVC_BEFORE_DATE].second%10+'0';
    tmp_cvc_code[12] = 0x5A;

    tmp_code[0] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].year/10%10+'0';
    tmp_code[1] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].year%10+'0';
    tmp_code[2] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].month/10+'0';
    tmp_code[3] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].month%10+'0';
    tmp_code[4] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].day/10+'0';
    tmp_code[5] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].day%10+'0';
    tmp_code[6] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].hour/10+'0';
    tmp_code[7] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].hour%10+'0';
    tmp_code[8] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].minute/10+'0';
    tmp_code[9] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].minute%10+'0';
    tmp_code[10] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].second/10+'0';
    tmp_code[11] = cert_data.date[CA_CERT_CODE_BEFORE_DATE].second%10+'0';
    tmp_code[12] = 0x5A;

    memcpy(time->code,tmp_code,13);
    memcpy(time->cvc,tmp_cvc_code,13);
    return;
}

void dpoe_oam_port_type_get_adapt(oam_uint8 *total_port, oam_uint8 *value)
{
    int i;
    oam_uint8 *ptr;
    oam_boolean is_ubq = FALSE;
    oam_uint8 ubq_type[4]={DPOE_OAM_CMCI,DPOE_OAM_CMCI,DPOE_OAM_EDVA,0x0D};

    is_ubq = oam_dpoe_ubq_check();
    *total_port = 4;
    ptr = value;

    if(is_ubq){
        memcpy(ptr,ubq_type,4);
    }else{
        for (i = 0; i < 4; i++){
                *ptr ++ = DPOE_OAM_CMCI;
        }
    }
}


void dpoe_oam_reset_onu_adapt()
{
    //ca_reset(0, CA_SYS_RESET_MODE_COLD);
    ca_printf("\r\nstart to reset ONU\r\n");
    ca_timer_add(1000, oam_system_reboot, NULL);
}

oam_uint32 dpoe_oam_dynmac_size_get_adapt()
{
    return 4096;
}

static oam_uint16 g_aging_time = 30000;
oam_uint16 dpoe_oam_aging_get_adapt()
{
    return g_aging_time;
}

#if defined(CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN)

void dpoe_oam_aging_set_adapt(oam_uint16 time)
{
    //ca_l2_aging_time_set(0, time/100);
}

oam_uint32 dpoe_oam_dynmac_table_get_adapt(oam_uint16 port, oam_uint16 *mac_number, oam_uint8 *buf)
{
    return 0;
}


oam_uint32 dpoe_oam_stcmac_table_get_adapt(oam_uint16 port, oam_uint16 *mac_number, oam_uint8 *buf)
{

    return 0;
}

void dpoe_oam_autonet_get_adapt(oam_uint16 port, oam_uint16 *max_cap, oam_uint16 *curr_cap)
{

}

void dpoe_oam_autonet_set_adapt(oam_uint16 port, oam_uint16 cap)
{


}


void dpoe_oam_uni_adm_ctrl_get_adapt(oam_uint16 port, oam_uint8 *enabled)
{

}


void dpoe_oam_uni_adm_ctrl_set_adapt(oam_uint16 port, oam_uint8 enable)
{

}


void dpoe_oam_uni_minmac_get_adapt(oam_uint16 port, oam_uint16 *nmb)
{
}

void dpoe_oam_uni_minmac_set_adapt(oam_uint16 port, oam_uint16 nmb)
{
}

void dpoe_oam_uni_maxmac_get_adapt(oam_uint16 port, oam_uint32 *nmb)
{
}

void dpoe_oam_uni_maxmac_set_adapt(oam_uint16 port, oam_uint32 nmb)
{
}

void dpoe_oam_onu_maxmac_get_adapt(oam_uint16 *nmb)
{
}

void dpoe_oam_onu_maxmac_set_adapt(oam_uint16 nmb)
{
}

void dpoe_oam_unkn_flooding_get_adapt(oam_uint8 *enable)
{
}

void dpoe_oam_unkn_flooding_set_adapt(oam_uint32 enable)
{
}

#else
void dpoe_oam_aging_set_adapt(oam_uint16 time)
{
    ca_l2_aging_time_set(0, time/100);
    g_aging_time = time;
}

oam_uint32 dpoe_oam_dynmac_table_get_adapt(oam_uint16 port, oam_uint16 *mac_number, oam_uint8 *buf)
{
    ca_l2_addr_entry_t *entry;
    ca_iterator_t return_entry;
    ca_status_t status;
    ca_uint8 *ptr;
    ca_port_id_t ca_port = 0;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    memset(&return_entry, 0, sizeof(ca_iterator_t));
    *mac_number = 0;
    ptr = buf;
    return_entry.entry_data = (ca_l2_addr_entry_t *)malloc(sizeof(ca_l2_addr_entry_t));
    if (!return_entry.entry_data)
        return -1;
    memset(return_entry.entry_data,0,sizeof(ca_l2_addr_entry_t));
    return_entry.entry_count = 1;
    return_entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * return_entry.entry_count;
    status = ca_l2_addr_iterate(0, &return_entry);

    while (CA_E_OK == status)
    {
        if (return_entry.entry_count == 1)
        {
            entry = (ca_l2_addr_entry_t *)return_entry.entry_data;
            if (ca_port == entry->port_id && !entry->static_flag)
            {
                memcpy(ptr+(*mac_number)*sizeof(ca_mac_addr_t), entry->mac_addr, sizeof(ca_mac_addr_t));
                (*mac_number)++;
            }
        }
        status = ca_l2_addr_iterate(0, &return_entry);
    }
    free(return_entry.entry_data);
    return 0;
}


oam_uint32 dpoe_oam_stcmac_table_get_adapt(oam_uint16 port, oam_uint16 *mac_number, oam_uint8 *buf)
{
    ca_l2_addr_entry_t *entry;
    ca_iterator_t return_entry;
    ca_status_t status;
    ca_uint8 *ptr;
    ca_port_id_t ca_port = 0;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    memset(&return_entry, 0, sizeof(ca_iterator_t));
    *mac_number = 0;
    ptr = buf;
    return_entry.entry_data = (ca_l2_addr_entry_t *)malloc(sizeof(ca_l2_addr_entry_t));
    if (!return_entry.entry_data)
        return -1;
    memset(return_entry.entry_data,0,sizeof(ca_l2_addr_entry_t));
    return_entry.entry_count = 1;
    return_entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * return_entry.entry_count;
    status = ca_l2_addr_iterate(0, &return_entry);

    while (CA_E_OK == status)
    {
        if (return_entry.entry_count == 1)
        {
            entry = (ca_l2_addr_entry_t *)return_entry.entry_data;
            if (ca_port == entry->port_id && entry->static_flag)
            {
                memcpy(ptr+(*mac_number)*sizeof(ca_mac_addr_t), entry->mac_addr, sizeof(ca_mac_addr_t));
                (*mac_number)++;
            }
        }
        status = ca_l2_addr_iterate(0, &return_entry);
    }
    return 0;
}

#define PRESENT_MASK(b)               (1<<(b))
#define BITON(m, b)                   (((m) & PRESENT_MASK(b)) != 0)
#define SETBIT(m, b)                  ((m) |= PRESENT_MASK(b))
void dpoe_oam_autonet_get_adapt(oam_uint16 port, oam_uint16 *max_cap, oam_uint16 *curr_cap)
{
    ca_eth_port_ability_t abi;
    ca_eth_port_mdix_t mode;
    ca_status_t ret = CA_E_OK;
    ca_port_id_t ca_port = 0;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    memset(&abi,0,sizeof(ca_eth_port_ability_t));
    ret = ca_eth_port_phy_advert_get(0,ca_port,&abi);
    if(ret!= CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_autonet_get_adapt:ca_eth_port_phy_advert_get failed\n\r");
        return;
    }
    OAM_ORG_LOG("dpoe_oam_autonet_get_adapt:ca_eth_port_phy_advert_get = 0x%08x \n\r",abi);
    *max_cap = 0xff;
    *curr_cap= 0;
    if (abi.half_10 || abi.half_100){
        SETBIT(*curr_cap, 0);
     }
    if(abi.full_10||abi.full_100||abi.full_1g||abi.full_10g||abi.full_2d5g||abi.full_5g)
    {
        SETBIT(*curr_cap, 1);
    }
    if (abi.full_10 || abi.half_10){
        SETBIT(*curr_cap, 2);
    }
    if (abi.full_100 || abi.half_100){
        SETBIT(*curr_cap, 3);
    }
    if (abi.full_1g){
        SETBIT(*curr_cap, 4);
    }
    if (abi.full_10g){
        SETBIT(*curr_cap, 5);
    }
    if (abi.flow_ctrl){
        SETBIT(*curr_cap, 6);
    }

    ca_eth_port_phy_mdix_get(0,ca_port, &mode);
    if (mode == CA_PORT_MDIX_AUTO){
        SETBIT(*curr_cap, 7);
    }
}

void dpoe_oam_autonet_set_adapt(oam_uint16 port, oam_uint16 cap)
{
    ca_eth_port_ability_t abi;
    ca_eth_port_mdix_t mode;
    ca_port_id_t ca_port = 0;
    ca_status_t ret = CA_E_OK;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    memset(&abi,0,sizeof(ca_eth_port_ability_t));
    ret = ca_eth_port_phy_advert_get(0,ca_port,&abi);
    if(ret!=CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_autonet_set_adapt:ca_eth_port_phy_advert_get failed\n\r");
        return;
    }
    abi.flow_ctrl = 0;
    abi.half_10 =0;
    abi.full_10 =0 ;
    abi.half_100 = 0;
    abi.full_100 =0;
    abi.full_1g = 0;
    abi.full_10g = 0;
    abi.full_5g = 0;
    abi.full_2d5g = 0;

    if (BITON(cap, 0)) // half
    {
        if (BITON(cap, 2))
        {
            abi.half_10 = 1;
        }
        if (BITON(cap, 3)){
            abi.half_100 = 1;
        }
    }

    if (BITON(cap, 1)) // full
    {
        if (BITON(cap, 2)){
            abi.full_10 = 1;
         }
        if (BITON(cap, 3)){
            abi.full_100 = 1;
         }
        if (BITON(cap, 4)){
            abi.full_1g = 1;
         }
        if (BITON(cap, 5)){
            abi.full_10g = 1;
        }
    }
    if (BITON(cap, 6)){
            abi.flow_ctrl = 1;
        }

    ret = ca_eth_port_phy_advert_set(0,ca_port,abi);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_autonet_set_adapt:ca_eth_port_phy_advert_set failed\n\r");
        return;
    }
    if (BITON(cap, 7))
    {
        mode = CA_PORT_MDIX_AUTO;
        ca_eth_port_phy_mdix_set(0,ca_port, mode);
    }

}

ca_boolean_t oam_uni_adm_ctrl_drop_flag = FALSE;
void dpoe_oam_uni_adm_ctrl_get_adapt(oam_uint16 port, oam_uint8 *enabled)
{
    ca_port_id_t ca_port = 0;
    ca_l2_learning_control_t learn_ctl;

    memset(&learn_ctl,0,sizeof(ca_l2_learning_control_t));

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ca_l2_learning_control_get(0,ca_port,&learn_ctl);

    *enabled = learn_ctl.sa_mac_table_full_policy;
}


void dpoe_oam_uni_adm_ctrl_set_adapt(oam_uint16 port, oam_uint8 enable)
{
    ca_port_id_t ca_port = 0;
    ca_l2_learning_control_t learn_ctl;

    memset(&learn_ctl,0,sizeof(ca_l2_learning_control_t));

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);


    ca_l2_learning_control_get(0,ca_port,&learn_ctl);
    if(enable){
        learn_ctl.sa_mac_table_full_policy = CA_L2_MAC_FULL_DROP;
    }else{
        learn_ctl.sa_mac_table_full_policy = CA_L2_MAC_FULL_FWD_NO_LEARN;
    }
    ca_l2_learning_control_set(0,ca_port,&learn_ctl);
}

oam_uint16 g_uni_minmac[4] = {0,0,0,0};

void dpoe_oam_uni_minmac_get_adapt(oam_uint16 port, oam_uint16 *nmb)
{
    ca_port_id_t ca_port;
    ca_uint32 tmp_num = 0;

    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_l2_mac_limit_get(0,CA_L2_MAC_LIMIT_DISABLE,ca_port,&tmp_num);
    *nmb = (oam_uint16)tmp_num;
}

void dpoe_oam_uni_minmac_set_adapt(oam_uint16 port, oam_uint16 nmb)
{
    ca_port_id_t ca_port;
    ca_status_t ret = CA_E_OK;

    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    OAM_ORG_LOG("dpoe_oam_uni_minmac_set_adapt: ca_port=0x%x\n\r",ca_port);

    ret = ca_l2_mac_limit_set(0,CA_L2_MAC_LIMIT_PORT,ca_port,nmb);
    if(CA_E_OK!= ret)
        OAM_ORG_LOG("dpoe_oam_uni_minmac_set_adapt:ca_l2_mac_limit_set() failed \n\r");
}

void dpoe_oam_uni_maxmac_get_adapt(oam_uint16 port, oam_uint16 *nmb)
{
    ca_port_id_t ca_port = 0;
    ca_status_t ret = CA_E_OK;
    ca_uint32 number = 0;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ret = ca_l2_mac_limit_get(0,CA_L2_MAC_LIMIT_PORT,ca_port,&number);
    OAM_ORG_LOG("ca_port is 0x%x ,number is %d\r\n",ca_port,number);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("ca_l2_mac_limit_get() failed\n\r");
        return;
    }
    *nmb = (oam_uint16)number;
    return;
}

void dpoe_oam_uni_maxmac_set_adapt(oam_uint16 port, oam_uint16 nmb)
{
    ca_port_id_t ca_port = 0;
    ca_status_t ret = CA_E_OK;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    OAM_ORG_LOG("ca_port is 0x%x ,number is %d\r\n",ca_port,nmb);
    ret = ca_l2_mac_limit_set(0,CA_L2_MAC_LIMIT_PORT,ca_port,nmb);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("ca_l2_mac_limit_set() failed\n\r");
        return;
    }
    return;
}

static ca_l2_learning_mode_t g_dpoe_learn_mode = 0;

void dpoe_oam_onu_maxmac_get_adapt(oam_uint16 *nmb)
{
    ca_l2_learning_mode_t learn_mode = 0;
    ca_status_t ret = CA_E_OK;
    memset(&learn_mode,0,sizeof(ca_l2_learning_mode_t));

    ca_l2_learning_mode_get(0,&learn_mode);

    if(learn_mode == CA_L2_LEARNING_DISABLE){
        *nmb = 0;
    }else{
        ret= ca_l2_mac_limit_get(0,CA_L2_MAC_LIMIT_SYSTEM,0,nmb);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("dpoe_oam_onu_maxmac_get_adapt:ca_l2_mac_limit_get() failed\n\r");
        }
        if(*nmb>4096){
            *nmb = 4096;
        }
    }

    OAM_ORG_LOG("Get MAC Learning Aggregate Limit = %d\n\r",*nmb);
}

void dpoe_oam_onu_maxmac_set_adapt(oam_uint16 nmb)
{
    ca_l2_learning_mode_t learn_mode = 0;
    ca_status_t ret = CA_E_OK;

    OAM_ORG_LOG("Set MAC Learning Aggregate Limit = %d\n\r",nmb);
    memset(&learn_mode,0,sizeof(ca_l2_learning_mode_t));
    ca_l2_learning_mode_get(0,&learn_mode);

    if(nmb==0){
        g_dpoe_learn_mode = learn_mode;
        learn_mode = CA_L2_LEARNING_DISABLE;
        ca_l2_learning_mode_set(0,learn_mode);
    }else {
        if(learn_mode == CA_L2_LEARNING_DISABLE){
            learn_mode = g_dpoe_learn_mode;
            ca_l2_learning_mode_set(0,learn_mode);
        }
        ret = ca_l2_mac_limit_set(0,CA_L2_MAC_LIMIT_SYSTEM,0,nmb);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("dpoe_oam_onu_maxmac_set_adapt:ca_l2_mac_limit_set() failed\n\r");
        }
    }
}

void dpoe_oam_unkn_flooding_get_adapt(oam_uint8 *enable)
{
    ca_l2_learning_control_t control;

    ca_port_id_t ca_port = 0;

    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(1);

    ca_l2_learning_control_get(0,ca_port, &control);
    *enable = (control.sa_mac_table_full_policy == 1) ? 0:1;
}

void dpoe_oam_unkn_flooding_set_adapt(oam_uint32 enable)
{
    ca_l2_learning_control_t control;
    ca_port_id_t ca_port = 0;
    ca_uint32 rate = 0;

    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(1);

    ca_l2_learning_control_get(0,ca_port, &control);
    if (!enable){
        control.sa_mac_table_full_policy = CA_L2_MAC_FULL_DROP;
        ca_l2_flooding_rate_set(0,ca_port,CA_L2_FLOODING_TYPE_UUC,0,rate);
    }
    else{
        control.sa_mac_table_full_policy = CA_L2_MAC_FULL_FWD_NO_LEARN;
        //work-around for bug#50718,maybe need modfiy the rate
        rate = 1638400;
        ca_l2_flooding_rate_set(0,ca_port,CA_L2_FLOODING_TYPE_UUC,0,rate);
    }
    ca_l2_learning_control_set(0,ca_port, &control);
}
#endif

oam_uint8 g_local_switch[4] = {0,0,0,0};
void dpoe_oam_local_switch_get_adapt(oam_uint16 port, oam_uint8 *enable)
{
    *enable = g_local_switch[port];
}

void dpoe_oam_local_switch_set_adapt(oam_uint16 port, oam_uint32 enable)
{
    g_local_switch[port] = enable;
}

void dpoe_oam_llid_queue_get_adapt(oam_dpoe_llid_queue_int_t *llid_queue)
{
    memcpy(llid_queue, &g_llid_queue_config, sizeof(oam_dpoe_llid_queue_int_t));
}

/* the delay between llid_x and llid_y registration has been done in SDK */
void dpoe_oam_mllid_reg_enable_adapt(oam_uint8 llid_num)
{
    ca_uint32_t i = 0;
    ca_port_id_t pon_port = OAM_PON_PORT_ID_TO_CA_PORT();

    for(i = 1; i < llid_num; i++){
        ca_epon_mpcp_registration_set(0, pon_port, i, 1);
    }
}

void dpoe_oam_llid_queue_set_adapt(oam_dpoe_llid_queue_int_t *llid_queue)
{
    int i, j;
    ca_port_id_t ca_port = 0;
    ca_status_t ret = CA_E_OK;

    memcpy(&g_llid_queue_config,llid_queue,sizeof(oam_dpoe_llid_queue_int_t));
    OAM_ORG_LOG("configure llid number %d, UNI port number %d \r\n", llid_queue->n_llid, llid_queue->n_port);
    if (llid_queue->n_llid)
    {
        for(j=0;j<llid_queue->n_llid;j++){

            for (i = 0; i < llid_queue->llid[j].n_queue; i++)
            {
                OAM_ORG_LOG("\t configure PON queue %d, size %d\r\n", i, llid_queue->llid[j].q[i]);
                ret = ca_queue_length_set(0, CA_PORT_ID(CA_PORT_TYPE_ETHERNET, 7), i, llid_queue->llid[j].q[i]);
                if(ret != CA_E_OK){
                    OAM_ORG_LOG("ca_queue_length_set for llid failed\n\r");
                    continue;
                }
            }
        }
    }
    if (llid_queue->n_port)
    {
        for (i = 0; i < llid_queue->n_port; i++)
        {
            for (j = 0; j < llid_queue->port[i].n_queue; j ++)
            {
                OAM_ORG_LOG("\t configure UNI %d queue %d, size %d\r\n", i, j, llid_queue->port[i].q[i]);
                ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(i);
                ret = ca_queue_length_set(0, ca_port, j, llid_queue->port[i].q[i]);
                if(ret != CA_E_OK){
                    OAM_ORG_LOG("ca_queue_length_set for port failed\n\r");
                    continue;
                }
            }
        }
    }

    if(llid_queue->n_llid > 1){
        dpoe_oam_mllid_reg_enable_adapt(llid_queue->n_llid);
    }
}

void dpoe_oam_mac_full_get_adapt(oam_uint16 port, oam_uint8 *beh)
{
    ca_l2_learning_control_t control;
    ca_port_id_t ca_port = 0;

    memset(&control,0,sizeof(ca_l2_learning_control_t));

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ca_l2_learning_control_get(0,ca_port, &control);
    OAM_ORG_LOG("control.sa_mac_table_full_policy = %d\n\r",control.sa_mac_table_full_policy);
    if (control.sa_mac_table_full_policy == CA_L2_MAC_FULL_DROP){
        *beh = 0;
    }
    else if((control.sa_mac_table_full_policy == CA_L2_MAC_FULL_FWD_LRU)
            ||(control.sa_mac_table_full_policy == CA_L2_MAC_FULL_FWD_NO_LEARN)){
        *beh = 1;
    }

}

void dpoe_oam_mac_full_set_adapt(oam_uint16 port, oam_uint8 beh)
{
    ca_l2_learning_control_t control;
    ca_port_id_t ca_port = 0;

    memset(&control,0,sizeof(ca_l2_learning_control_t));
    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_l2_learning_control_get(0,ca_port, &control);
    if (beh)
        control.sa_mac_table_full_policy = CA_L2_MAC_FULL_FWD_LRU;
    else
        control.sa_mac_table_full_policy = CA_L2_MAC_FULL_DROP;
    ca_l2_learning_control_set(0,ca_port, &control);
}

void dpoe_oam_clr_dyn_mac_adapt(oam_uint16 port)
{
    ca_port_id_t ca_port = 0;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_l2_addr_delete_by_port(0, ca_port, CA_L2_ADDR_OPERATION_DYNAMIC);
}

void dpoe_oam_add_dyn_mac_adapt(oam_uint16 port, oam_uint8 nmb, oam_uint8 *mac)
{
    ca_l2_addr_entry_t entry;
    int i;
    oam_uint8 *ptr = mac;
    ca_port_id_t ca_port = 0;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    memset(&entry, 0, sizeof(ca_l2_addr_entry_t));
    for (i = 0;i < nmb; i++)
    {
        memcpy(&entry.mac_addr, ptr, 6);
        entry.da_permit = 1;
        entry.sa_permit = 1;
        entry.port_id = ca_port;
        entry.aging_timer = g_aging_time / 10;
        ptr += 6;
        ca_l2_addr_add(0, &entry);
    }
}

void dpoe_oam_del_dyn_mac_adapt(oam_uint16 port, oam_uint8 nmb, oam_uint8 *mac)
{
    ca_l2_addr_entry_t entry;
    int i;
    oam_uint8 *ptr = mac;
    ca_port_id_t ca_port = 0;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    memset(&entry, 0, sizeof(ca_l2_addr_entry_t));
    for (i = 0;i < nmb; i++)
    {
        memcpy(&entry.mac_addr, ptr, 6);
        entry.da_permit = 1;
        entry.sa_permit = 1;
        entry.port_id = ca_port;
        ptr += 6;
        ca_l2_addr_delete(0, &entry);
    }
}

void dpoe_oam_clr_sta_mac_adapt(oam_uint16 port)
{
    ca_port_id_t ca_port = 0;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_l2_addr_delete_by_port(0, ca_port, CA_L2_ADDR_OPERATION_STATIC);
}

void dpoe_oam_add_sta_mac_adapt(oam_uint16 port, oam_uint8 nmb, oam_uint8 *mac)
{
    ca_l2_addr_entry_t addr;
    ca_port_id_t ca_port = 0;
    oam_uint8 i =0;
    ca_status_t ret = CA_E_OK;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    OAM_ORG_LOG("dpoe_oam_add_sta_mac_adapt:nmb=%d\n\r",nmb);
    if(nmb>21){
        nmb = 21;
    }

    for(i=0;i<nmb;i++){
        memset(&addr, 0, sizeof(ca_l2_addr_entry_t));
        memcpy(&addr.mac_addr, mac+6*i, 6);
        addr.da_permit = 1;
        addr.sa_permit = 1;
        addr.static_flag = 1;
        addr.port_id = ca_port;
        ret = ca_l2_addr_add(0, &addr);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("dpoe_oam_add_sta_mac_adapt:ca_l2_addr_add failed\n\r");
            continue;
        }
    }

}

void dpoe_oam_del_sta_mac_adapt(oam_uint16 port, oam_uint8 nmb, oam_uint8 *mac)
{
    ca_l2_addr_entry_t addr;
    ca_port_id_t ca_port = 0;
    oam_uint8 i =0;
    ca_status_t ret = CA_E_OK;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    OAM_ORG_LOG("dpoe_oam_del_sta_mac_adapt:nmb=%d\n\r",nmb);
    if(nmb>21){
        nmb = 21;
    }
    for(i=0;i<nmb;i++){
        memset(&addr, 0, sizeof(ca_l2_addr_entry_t));
        memcpy(&addr.mac_addr, mac+6*i, 6);
        addr.da_permit = 1;
        addr.sa_permit = 1;
        addr.static_flag = 1;
        addr.port_id = ca_port;
        ret = ca_l2_addr_delete(0, &addr);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("dpoe_oam_del_sta_mac_adapt:ca_l2_addr_add failed\n\r");
            continue;
        }
    }
}

oam_uint8 g_queue_thr[64];
oam_uint64 g_queue_val[64];
oam_uint8 dpoe_oam_queue_delay_thr_get_adapt(oam_uint8 queue)
{
    if (queue > 63) return 0;
    return g_queue_thr[queue];
}

void dpoe_oam_queue_delay_thr_set_adapt(oam_uint8 queue, oam_uint8 value)
{
    if (queue > 63) return;
    g_queue_thr[queue] = value;
}


oam_uint64 dpoe_oam_queue_delay_val_get_adapt(oam_uint8 queue)
{
    if (queue > 63) return 0;
    return g_queue_val[queue];
}

void dpoe_oam_queue_delay_val_reset_adapt(oam_uint8 queue)
{
    if (queue > 63) return;
    g_queue_val[queue] = 0;
}

oam_uint64 dpoe_oam_frm_dropped_get_adapt(oam_uint8 queue)
{
    if (queue > 63) return 0;
    return 0;
}

void dpoe_oam_frm_dropped_reset_adapt(oam_uint8 queue)
{
    if (queue > 63) return;
}

oam_uint64 dpoe_oam_oct_dropped_get_adapt(oam_uint8 queue)
{
    if (queue > 63) return 0;
    return 0;
}

void dpoe_oam_oct_dropped_reset_adapt(oam_uint8 queue)
{
    if (queue > 63) return;
}

oam_uint64 dpoe_oam_oct_delayed_get_adapt(oam_uint8 queue)
{
    if (queue > 63) return 0;
    return 0;
}

void dpoe_oam_oct_delayed_reset_adapt(oam_uint8 queue)
{
    if (queue > 63) return;
}

oam_uint64 dpoe_oam_usoct_unused_get_adapt(oam_uint8 queue)
{
    if (queue > 63) return 0;
    return 0;
}

void dpoe_oam_usoct_unused_reset_adapt(oam_uint8 queue)
{
    if (queue > 63) return;
}

oam_uint16 dpoe_oam_opt_mon_temp_get_adapt()
{
    ca_status_t ret = CA_E_OK;

    opm_diag_info_t diag_info;

    memset(&diag_info, 0, sizeof(opm_diag_info_t));

    ret = opm_ddm_raw_data_get(0, 0, &diag_info);

    if (ret == CA_E_OK)
        return diag_info.temp;

    return 0;
}

void dpoe_oam_opt_mon_temp_reset_adapt()
{
}

oam_uint16 dpoe_oam_opt_mon_vcc_get_adapt()
{
    ca_status_t ret = CA_E_OK;

    opm_diag_info_t diag_info;

    memset(&diag_info, 0, sizeof(opm_diag_info_t));

    ret = opm_ddm_raw_data_get(0, 0, &diag_info);

    if (ret == CA_E_OK)
        return diag_info.volt;

    return 0;
}

void dpoe_oam_opt_mon_vcc_reset_adapt()
{
}

oam_uint16 dpoe_oam_opt_mon_bias_get_adapt()
{
    ca_status_t ret = CA_E_OK;

    opm_diag_info_t diag_info;

    memset(&diag_info, 0, sizeof(opm_diag_info_t));

    ret = opm_ddm_raw_data_get(0, 0, &diag_info);

    if (ret == CA_E_OK)
        return diag_info.tx_bias;

    return 0;
}

void dpoe_oam_opt_mon_bias_reset_adapt()
{
}

oam_uint16 dpoe_oam_opt_mon_txpwr_get_adapt()
{
    ca_status_t ret = CA_E_OK;

    opm_diag_info_t diag_info;

    memset(&diag_info, 0, sizeof(opm_diag_info_t));

    ret = opm_ddm_raw_data_get(0, 0, &diag_info);

    if (ret == CA_E_OK)
        return diag_info.tx_pwr;

    return 0;
}

void dpoe_oam_opt_mon_txpwr_reset_adapt()
{
}

oam_uint16 dpoe_oam_opt_mon_rxpwr_get_adapt()
{
    ca_status_t ret = CA_E_OK;

    opm_diag_info_t diag_info;

    memset(&diag_info, 0, sizeof(opm_diag_info_t));

    ret = opm_ddm_raw_data_get(0, 0, &diag_info);

    if (ret == CA_E_OK)
        return diag_info.rx_pwr;

    return 0;
}

void dpoe_oam_opt_mon_rxpwr_reset_adapt()
{
}

oam_uint64 dpoe_oam_rx_frm_y_get_adapt(oam_uint8 type, oam_uint8 value)
{
    ca_port_id_t ca_port = 0;
    ca_policer_stats_t p_sts;
    oam_uint16 port = value;
    ca_status_t ret = CA_E_OK;

    memset(&p_sts,0,sizeof(ca_policer_stats_t));
    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ret = ca_port_policer_stats_get(0,ca_port,FALSE,&p_sts);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_rx_frm_y_get_adapt:ca_port_policer_stats_get failed\n\r");
        return CA_E_UNAVAIL;
    }
    return p_sts.yellow_packets;
}

void dpoe_oam_rx_frm_y_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_tx_frm_y_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_tx_frm_y_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_tx_oct_g_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, FALSE, &data);
    return data.tx_good_octets;
#else
    return 0;
#endif
}

void dpoe_oam_tx_oct_g_reset_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, FALSE, &data);
#endif
}

oam_uint64 dpoe_oam_rx_oct_y_get_adapt(oam_uint8 type, oam_uint8 value)
{
    ca_port_id_t ca_port = 0;
    ca_policer_stats_t p_sts;
    oam_uint16 port = value;
    ca_status_t ret = CA_E_OK;

    memset(&p_sts,0,sizeof(ca_policer_stats_t));
    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ret = ca_port_policer_stats_get(0,ca_port,FALSE,&p_sts);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_rx_oct_y_get_adapt:ca_port_policer_stats_get failed\n\r");
        return CA_E_UNAVAIL;
    }
    return p_sts.yellow_bytes;
}

void dpoe_oam_rx_oct_y_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_rx_oct_g_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;
    ca_policer_stats_t p_sts;
    ca_status_t ret = CA_E_OK;

    memset(&p_sts,0,sizeof(ca_policer_stats_t));

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ret = ca_port_policer_stats_get(0,ca_port,FALSE,&p_sts);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_rx_oct_g_get_adapt:ca_port_policer_stats_get failed\n\r");
        return CA_E_UNAVAIL;
    }

    return p_sts.green_bytes;
#else
    return 0;
#endif
}

void dpoe_oam_rx_oct_g_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_tx_oct_y_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_tx_oct_y_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_rx_frm_l2cp_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_rx_frm_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_rx_oct_l2cp_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_rx_oct_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_tx_frm_l2cp_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_tx_frm_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_tx_oct_l2cp_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_tx_oct_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_disc_frm_l2cp_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_disc_frm_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_disc_oct_l2cp_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_disc_oct_l2cp_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_tx_err_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_tx_err_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}

oam_uint64 dpoe_oam_rx_err_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_rx_err_reset_adapt(oam_uint8 type, oam_uint8 value)
{
}


/* service level aggrement */
oam_status dpoe_oam_bcast_limit_get_adapt(oam_uint16 port, oam_uint32 *value)
{
    ca_status_t ret = CA_E_OK;
    ca_l2_flooding_type_t ptype;
    ca_port_id_t ca_port;
    ca_boolean_t pps = 0;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ptype = CA_L2_FLOODING_TYPE_BC;

    ret = ca_l2_flooding_rate_get(0, ca_port, ptype, &pps, value);
    if(ret != CA_E_OK){
        OAM_ORG_LOG_CRI("failed, ret = %d, port_id = 0x%x\r\n", ret, ca_port);
        return OAM_E_ERROR;
    }
    OAM_ORG_DEBUG("ret = %d, port_id = 0x%x, ptype = %d, rate = %d\r\n", ret, ca_port, ptype, *value);
    return OAM_E_OK;
}

oam_status dpoe_oam_bcast_limit_set_adapt(oam_uint16 port, oam_uint32 value)
{
    ca_status_t ret = CA_E_OK;
    ca_l2_flooding_type_t ptype;
    ca_port_id_t ca_port;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ptype = CA_L2_FLOODING_TYPE_BC;

    OAM_ORG_DEBUG("port_id = 0x%x, ptype = %d, rate = %d\r\n", ca_port, ptype, value);
    ret = ca_l2_flooding_rate_set(0, ca_port, ptype, TRUE, value);
    if(ret != CA_E_OK){
        OAM_ORG_LOG_CRI("failed, ret = %d, port_id = 0x%x\r\n", ret, ca_port);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;

}

static oam_uint16  g_dpoe_cbs = 0;
oam_status dpoe_oam_queue_cir_get_adapt(oam_uint16 port, oam_uint8 queue, oam_uint16 *cbs, oam_uint32 *cir)
{
    ca_status_t ret = CA_E_OK;
    ca_shaper_t shaper;
    ca_port_id_t ca_port;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ret = ca_queue_shaper_get(0, ca_port, queue, &shaper);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_queue_cir_get_adapt:ca_queue_shaper_get failed, ret = %d, port_id = 0x%x\r\n", ret, ca_port);
        return OAM_E_ERROR;
    }
    OAM_ORG_DEBUG("ret = %d, port_id = 0x%x, queue_id = %d, enable = %d, pps = %d, burst_size = %d, rate = %d\r\n",
        ret, ca_port, queue, shaper.enable, shaper.pps, shaper.burst_size, shaper.rate);

    if(shaper.enable == 0){
        *cbs = 0;
        *cir = 0;
    }else{
        *cbs = g_dpoe_cbs;
        *cir = shaper.rate;
    }
    return OAM_E_OK;
}

oam_status dpoe_oam_queue_cir_set_adapt(oam_uint16 port, oam_uint8 queue, oam_uint16 cbs, oam_uint32 cir)
{
    ca_status_t ret = CA_E_OK;
    oam_port_id_t port_id;
    ca_shaper_t shaper;
    ca_port_id_t ca_port;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    memset(&shaper, 0, sizeof(ca_shaper_t));
    ret = ca_queue_shaper_get(0, ca_port, queue, &shaper);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_queue_cir_set_adapt:ca_queue_shaper_get failed, ret = %d, port_id = 0x%x\r\n", ret, ca_port);
        return OAM_E_ERROR;
    }
    if(cbs == 0){
        shaper.enable = 0;
    }else{
        shaper.enable = 1;
        shaper.pps = FALSE;
        shaper.burst_size = cbs;
        shaper.rate = cir;
        g_dpoe_cbs = cbs;
    }

    OAM_ORG_DEBUG("port_id = 0x%x, queue_id = %d, enable = %d, pps = %d, burst_size = %d, rate = %d\r\n",
        port_id, queue, shaper.enable, shaper.pps, shaper.burst_size, shaper.rate);
    ret = ca_queue_shaper_set(0, ca_port, queue, &shaper);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("failed, ret = %d, port_id = 0x%x\r\n", ret, ca_port);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status dpoe_oam_fec_get_adapt(oam_uint16 port, oam_uint32 *ds, oam_uint32 *us)
{
    ca_status_t ret = CA_E_OK;
    ca_port_id_t ca_port;

    ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();

    ret = ca_epon_port_fec_enable_get(0, ca_port, us, ds);
    if(ret != CA_E_OK){
        OAM_ORG_LOG_CRI("failed, ret = %d, port_id = 0x%x\r\n", ret, ca_port);
        return OAM_E_ERROR;
    }
    OAM_ORG_DEBUG("ret = %d, port_id = 0x%x, tx_enable = %d, rx_enable = %d\r\n", ret, ca_port, *us, *ds);
    return OAM_E_OK;
}

oam_status dpoe_oam_fec_set_adapt(oam_uint16 port, oam_uint32 ds, oam_uint32 us)
{
    ca_status_t ret = CA_E_OK;
    ca_port_id_t ca_port;

    ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();

    OAM_ORG_DEBUG("port_id = 0x%x, tx_enable = %d, rx_enable = %d\r\n", ca_port, us, ds);
    ret = ca_epon_port_fec_enable_set(0, ca_port, us, ds);
    if(ret != CA_E_OK){
        OAM_ORG_LOG_CRI("failed, ret = %d, port_id = 0x%x\r\n", ret, ca_port);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status dpoe_oam_queue_eir_get_adapt(oam_uint16 port, oam_uint8 queue, oam_uint16 *ebs, oam_uint32 *eir)
{
    return OAM_E_OK;
}

oam_status dpoe_oam_queue_eir_set_adapt(oam_uint16 port, oam_uint8 queue, oam_uint16 ebs, oam_uint32 eir)
{
    return OAM_E_OK;
}

oam_status dpoe_oam_queue_mark_get_adapt(oam_uint16 port, oam_uint8 queue, oam_dpoe_queue_mark_value_t *data)
{
    return OAM_E_OK;
}

oam_status dpoe_oam_queue_mark_set_adapt(oam_uint16 port, oam_uint8 queue, oam_dpoe_queue_mark_value_t data)
{
    return OAM_E_OK;
}

oam_status dpoe_oam_queue_limiter_cap_get_adapt(oam_uint16 port, oam_uint8 queue, oam_dpoe_queue_limiter_cap_value_t *data)
{
    /* hard code */
    data->num_limiter = 1;      /* Number of Rate Limiters */
    data->cbs_increment = 1;    /* CBS Min Increment */
    data->cir_increment  = 1;   /* CIR Min Increment */
    data->ebs_increment = 1;    /* EBS Min Increment */
    data->eir_increment = 1;    /* EIR Min Increment */
    data->color_aware= TRUE;        /* Clolor Aware? */
    data->coupling_config = TRUE;    /* Coupling Configurable? */
    data->coupling_behavior = TRUE;  /* Coupling Behavior Default */
    data->color_marking = TRUE;      /* Color Marking Support? */
    data->color_drop = TRUE;         /* Smart Color Drop? */

    return OAM_E_OK;
}

oam_status dpoe_oam_queue_limiter_cap_set_adapt(oam_uint16 port, oam_uint8 queue, oam_dpoe_queue_limiter_cap_value_t data)
{
    return OAM_E_OK;
}

oam_status dpoe_oam_coupling_flag_get_adapt(oam_uint16 port, oam_uint8 queue, oam_uint8 *value)
{
    ca_port_id_t ca_port;
    ca_policer_t policer;

    memset(&policer,0,sizeof(ca_policer_t));

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ca_port_policer_get(0,ca_port,&policer);

    if(policer.mode == CA_POLICER_MODE_TRTCMDS_COUPLED){
        *value = 0x01;
    }else if(policer.mode == CA_POLICER_MODE_TRTCMDS ){
        *value = 0x00;
    }else{
        *value = 0x00;
    }
    return OAM_E_OK;
}

oam_status dpoe_oam_coupling_flag_set_adapt(oam_uint16 port, oam_uint8 queue, oam_uint8 value)
{
    ca_port_id_t ca_port;
    ca_policer_t policer;

    memset(&policer,0,sizeof(ca_policer_t));

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ca_port_policer_get(0,ca_port,&policer);
    if(value){
        policer.mode = CA_POLICER_MODE_TRTCMDS_COUPLED;
    }else{
        policer.mode = CA_POLICER_MODE_TRTCMDS;
    }
    ca_port_policer_set(0,ca_port,&policer);
    return OAM_E_OK;
}

static dpoe_cls_traffic  g_dpoe_traffic_cls_index_for_onu[16];
static oam_uint8 g_dpoe_traffic_cls_index_for_onu_is_first = 0;
static oam_uint8 g_dpoe_traffic_cls_index_for_onu_cnt = 0;

static dpoe_cls_traffic  g_dpoe_traffic_cls_index_for_llid[16];
static oam_uint8 g_dpoe_traffic_cls_index_for_llid_is_first = 0;
static oam_uint8 g_dpoe_traffic_cls_index_for_llid_cnt = 0;

static dpoe_cls_traffic g_dpoe_traffic_default_cls_index[300];
static oam_uint8 g_dpoe_traffic_default_cls_index_cnt = 0;

void __dpoe_oam_default_traffic_cfg_init(){
    memset(g_dpoe_traffic_default_cls_index,0,300*sizeof(dpoe_cls_traffic));
    g_dpoe_traffic_default_cls_index_cnt = 0;
}
oam_status __dpoe_oam_traffic_cfg(oam_boolean en,ca_llid_t llid_index, oam_llid_t llid){  
    ca_device_config_tlv_t scfg_tlv;
    ca_status_t ret = CA_E_OK;
    oam_uint8 oam_mode = 0;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t cls_action;
    ca_uint32_t cls_index = 0;

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&cls_action,0,sizeof(ca_classifier_action_t));

    scfg_tlv.type = CA_CFG_ID_OAM_MODE;
    scfg_tlv.len =  sizeof(ca_uint8_t);

    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if ( CA_E_OK != ret )
    {
        OAM_LOG_DEBUG("read CA_CFG_ID_OAM_MODE failed. ret = %d \r\n", ret);
        return OAM_E_ERROR;
    }
    oam_mode = scfg_tlv.value[0];
    
    if(1 == oam_mode){
        if(en){

        }else{
            OAM_ORG_LOG("disable downstream traffic \n\r");
            key.src_port = CA_PORT_ID(CA_PORT_TYPE_EPON,0x07);
            key.key_handle.llid_cos_index = llid_index<<11;
            key_mask.key_handle = 1;
            key_mask.src_port = 1;
            cls_action.forward = CA_CLASSIFIER_FORWARD_DENY;
            ret = ca_classifier_rule_add(0,7,&key,&key_mask,&cls_action,&cls_index);
            OAM_ORG_LOG("add cls rule ,cls_index=%d, ret= %d\n\r",cls_index,ret);
            if(ret == CA_E_OK){
                g_dpoe_traffic_default_cls_index[g_dpoe_traffic_default_cls_index_cnt].valid = TRUE;
                g_dpoe_traffic_default_cls_index[g_dpoe_traffic_default_cls_index_cnt].cls_index = cls_index;
                OAM_ORG_LOG("g_dpoe_traffic_default_cls_index_cnt %d store cls_index = %d\n\r",g_dpoe_traffic_default_cls_index_cnt,cls_index);
                g_dpoe_traffic_default_cls_index_cnt++;
            }
            
            OAM_ORG_LOG("disable upstream traffic \n\r");
            ret = ca_epon_llid_traffic_enable_set(0,CA_PORT_ID(CA_PORT_TYPE_EPON,0x07),llid_index,en,1);
            if(ret != CA_E_OK){
                OAM_ORG_LOG("ca_epon_llid_traffic_enable_set failed ,ret=%d\n\r",ret);
            }
        }  
    }
    return CA_E_OK;
}

oam_status dpoe_oam_onu_traffic_set_adapt(oam_boolean en)
{
    oam_uint8 i = 0,j=0;
    oam_llid_t llid = 0xffff;
    ca_status_t ret = CA_E_OK;
    oam_uint8 llid_num = OAM_MAX_IF;;
    ca_boolean_t register_enable = FALSE;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t cls_action;
    ca_uint32_t cls_index = 0;
    ca_device_config_tlv_t scfg_tlv;
    oam_uint8 tmp_default_cls_index_cnt = g_dpoe_traffic_default_cls_index_cnt;

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&cls_action,0,sizeof(ca_classifier_action_t));

    scfg_tlv.type = CA_CFG_ID_PON_LLID_NUM;
    scfg_tlv.len =  CFG_ID_PON_LLID_NUM;

    if(g_dpoe_traffic_cls_index_for_onu_is_first == 0){
        memset(g_dpoe_traffic_cls_index_for_onu,0,16*sizeof(dpoe_cls_traffic));
        g_dpoe_traffic_cls_index_for_onu_is_first = 1;
        g_dpoe_traffic_cls_index_for_onu_cnt = 0;
    }

    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if ( CA_E_OK != ret )
    {
        OAM_LOG_DEBUG("read CA_CFG_ID_PON_LLID_NUM fail. ret = %d \r\n", ret);
        return OAM_E_ERROR;
    }

    llid_num = scfg_tlv.value[0];

    OAM_ORG_LOG("dpoe_oam_onu_traffic_set_adapt: llid_num = %d\n\r",llid_num);

    for(i=0;i<llid_num;i++){
        ret = ca_epon_mpcp_registration_get(0,CA_PORT_ID(CA_PORT_TYPE_EPON,0x07),i,&register_enable);
        if ( CA_E_OK != ret )
        {
            OAM_ORG_LOG("ca_epon_mpcp_registration_get fail. ret = %d \r\n", ret);
            return OAM_E_ERROR;
        }
        if(register_enable){
             OAM_ORG_LOG("llid= %d has been found\n\r",i);
             ret = ca_epon_llid_traffic_enable_set(0,CA_PORT_ID(CA_PORT_TYPE_EPON,0x07),i,en,1);
             if(CA_E_OK != ret){
                OAM_ORG_LOG("dpoe_oam_onu_traffic_set_adapt: ca_epon_llid_traffic_enable_set failed\n\r");
                continue;
             }
            if(en == FALSE){
                OAM_ORG_LOG("dpoe_oam_onu_traffic_set_adapt:disable downstream traffic \n\r");
                if(16 == g_dpoe_traffic_cls_index_for_onu_cnt){
                    OAM_ORG_LOG("g_dpoe_traffic_cls_index_for_onu_cnt is 16, so reset it \n\r");
                    g_dpoe_traffic_cls_index_for_onu_cnt = 0;
                }
                key.src_port = CA_PORT_ID(CA_PORT_TYPE_EPON,0x07);
                key.key_handle.llid_cos_index = i<<11;
                key_mask.key_handle = 1;
                key_mask.src_port = 1;
                cls_action.forward = CA_CLASSIFIER_FORWARD_DENY;
                ret = ca_classifier_rule_add(0,7,&key,&key_mask,&cls_action,&cls_index);
                OAM_ORG_LOG("dpoe_oam_onu_traffic_set_adapt:add cls rule ,cls_index=%d, ret= %d\n\r",cls_index,ret);
                if(ret == CA_E_OK){
                    OAM_ORG_LOG("dpoe_oam_onu_traffic_set_adapt:store cls_index = %d\n\r",cls_index);
                    g_dpoe_traffic_cls_index_for_onu[g_dpoe_traffic_cls_index_for_onu_cnt].cls_index = cls_index;
                    g_dpoe_traffic_cls_index_for_onu[g_dpoe_traffic_cls_index_for_onu_cnt].valid = TRUE;
                    g_dpoe_traffic_cls_index_for_onu_cnt++;
                }
            }
        }
    }

     if(en){
        OAM_ORG_LOG("dpoe_oam_onu_traffic_set_adapt:enable downstream traffic \n\r");
        for(j=0;j<g_dpoe_traffic_cls_index_for_onu_cnt;j++){
            if(g_dpoe_traffic_cls_index_for_onu[j].valid){
                OAM_ORG_LOG("Found the cls_index=%d, j=%d to delete\n\r",g_dpoe_traffic_cls_index_for_onu[j],j);
                ret = ca_classifier_rule_delete(0,g_dpoe_traffic_cls_index_for_onu[j].cls_index);
                if(CA_E_OK == ret){
                    OAM_ORG_LOG("dpoe_oam_onu_traffic_set_adapt:ca_classifier_rule_delete() successfully \n\r");
                    memset(&g_dpoe_traffic_cls_index_for_onu[j],0,sizeof(dpoe_cls_traffic));
                }else{
                    OAM_ORG_LOG("dpoe_oam_onu_traffic_set_adapt:ca_classifier_rule_delete() failed \n\r");
                    continue;
                }
            }
        }

        for(j=0;j<g_dpoe_traffic_default_cls_index_cnt;j++){
            if(g_dpoe_traffic_default_cls_index[j].valid){
                OAM_ORG_LOG("Found the default cls_index=%d, j=%d to delete\n\r",g_dpoe_traffic_default_cls_index[j],j);
                ret = ca_classifier_rule_delete(0,g_dpoe_traffic_default_cls_index[j].cls_index);
                if(CA_E_OK == ret){
                    OAM_ORG_LOG("ca_classifier_rule_delete() for default rule successfully \n\r");
                    memset(&g_dpoe_traffic_default_cls_index[j],0,sizeof(dpoe_cls_traffic));
                    tmp_default_cls_index_cnt--;
                }else{
                    OAM_ORG_LOG("ca_classifier_rule_delete() for default rule failed \n\r");
                    continue;
                }
            }
        }
        if(tmp_default_cls_index_cnt==0){
            g_dpoe_traffic_default_cls_index_cnt = 0;
            OAM_ORG_LOG("reset g_dpoe_traffic_default_cls_index_cnt to 0 \n\r");
        }

        
    }
    return OAM_E_OK;
}

oam_status dpoe_oam_llid_traffic_set_adapt(oam_uint16 llid, oam_boolean en)
{
    ca_status_t ret = CA_E_OK;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t cls_action;
    ca_uint32_t cls_index = 0;
    oam_uint8 j=0,i=0;
    oam_llid_t llid_value = 0;
    oam_uint8 llid_index = 0;
    oam_if_t *intf = NULL;
    dpoe_cls_traffic *tmp_cls_traffic = NULL;
    oam_uint8 tmp_default_cls_index_cnt = g_dpoe_traffic_default_cls_index_cnt;
    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&cls_action,0,sizeof(ca_classifier_action_t));

    if( g_dpoe_traffic_cls_index_for_llid_is_first==0){
        memset(g_dpoe_traffic_cls_index_for_llid,0,16*sizeof(dpoe_cls_traffic));
        g_dpoe_traffic_cls_index_for_llid_is_first = 1;
        g_dpoe_traffic_cls_index_for_llid_cnt=0;
    }

    OAM_ORG_LOG("Start to enable/disable upstream traffic ,en=%d\n\r",en);
    llid_value = oam_llid_get();
    if(llid_value == 0xffff){
        OAM_ORG_LOG("Not found llid,, llid_value %d\r\n", llid_value);
        return CA_E_ERROR;
    }
    intf = oam_intf_find(llid_value);
    if (intf == NULL) {
        OAM_ORG_LOG("intf is NULL, llid id = %d \n", llid_value);
        return CA_E_ERROR;
    }
    llid_index  = intf->index;
    ret =  ca_epon_llid_traffic_enable_set(0,CA_PORT_ID(CA_PORT_TYPE_EPON,0x07),llid_index,en,1);
    if(CA_E_OK != ret)
    {
        OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt : ca_epon_llid_traffic_enable_set failed\n\r");
        return ret;
    }
    if(en){
        OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt:enable downstream traffic \n\r");
         for(j=0;j<g_dpoe_traffic_cls_index_for_llid_cnt;j++){
            //OAM_ORG_LOG("g_dpoe_traffic_cls_index_for_llid[%d].valid = %d\n\r",j,g_dpoe_traffic_cls_index_for_llid[j].valid);
            //OAM_ORG_LOG("g_dpoe_traffic_cls_index_for_llid[%d].cls_index = %d\n\r",j,g_dpoe_traffic_cls_index_for_llid[j].cls_index);
            if(g_dpoe_traffic_cls_index_for_llid[j].valid){
                OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt:delete cls_index = %d\n\r",g_dpoe_traffic_cls_index_for_llid[j].cls_index);
                ret = ca_classifier_rule_delete(0,g_dpoe_traffic_cls_index_for_llid[j].cls_index);
                if(CA_E_OK == ret){
                    OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt:ca_classifier_rule_delete() successfully \n\r");
                    memset(&g_dpoe_traffic_cls_index_for_llid[j],0,sizeof(dpoe_cls_traffic));
                }else{
                    OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt:ca_classifier_rule_delete() failed\n\r");
                    continue;
                }
            }
        }
         
        for(j=0;j<g_dpoe_traffic_default_cls_index_cnt;j++){
            if(g_dpoe_traffic_default_cls_index[j].valid){
                OAM_ORG_LOG("Found the default cls_index=%d, j=%d to delete\n\r",g_dpoe_traffic_default_cls_index[j],j);
                ret = ca_classifier_rule_delete(0,g_dpoe_traffic_default_cls_index[j].cls_index);
                if(CA_E_OK == ret){
                    OAM_ORG_LOG("ca_classifier_rule_delete() for default rule successfully \n\r");
                    memset(&g_dpoe_traffic_default_cls_index[j],0,sizeof(dpoe_cls_traffic));
                    tmp_default_cls_index_cnt--;
                }else{
                    OAM_ORG_LOG("ca_classifier_rule_delete() for default rule failed \n\r");
                    continue;
                }
            }
        }
        if(tmp_default_cls_index_cnt==0){
            g_dpoe_traffic_default_cls_index_cnt = 0;
            OAM_ORG_LOG("reset g_dpoe_traffic_default_cls_index_cnt to 0 \n\r");
        }
    }else{
        OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt:disable downstream traffic \n\r");
        j=0;
        if(16 == g_dpoe_traffic_cls_index_for_llid_cnt){
            OAM_ORG_LOG("g_dpoe_traffic_cls_index_for_llid_cnt is 16 ,so reset it \n\r");
            g_dpoe_traffic_cls_index_for_llid_cnt = 0;
        }
        key.src_port = CA_PORT_ID(CA_PORT_TYPE_EPON,0x07);
        llid_value = oam_llid_get();
        if(llid_value == 0xffff){
            OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt:not found llid,, llid_value %d\r\n", llid_value);
            return CA_E_ERROR;
        }
        intf = oam_intf_find(llid_value);
        if (intf == NULL) {
            OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt:intf is NULL, llid id = %d \n", llid_value);
            return CA_E_ERROR;
        }
        llid_index  = intf->index;
        OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt:llid=%d,llid_index =%d\n\r",llid_value,llid_index);
        key.key_handle.llid_cos_index = llid_index<<11;
        key_mask.key_handle = 1;
        key_mask.src_port = 1;
        cls_action.forward = CA_CLASSIFIER_FORWARD_DENY;
        ret = ca_classifier_rule_add(0,7,&key,&key_mask,&cls_action,&cls_index);
        OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt:add cls rule ,cls_index=%d, ret= %d\n\r",cls_index,ret);
        if(CA_E_OK == ret){
            g_dpoe_traffic_cls_index_for_llid[g_dpoe_traffic_cls_index_for_llid_cnt].valid = TRUE;
            g_dpoe_traffic_cls_index_for_llid[g_dpoe_traffic_cls_index_for_llid_cnt].cls_index = cls_index;
            OAM_ORG_LOG("dpoe_oam_llid_traffic_set_adapt:cls_index_for_llid_cnt %d store cls_index = %d\n\r",g_dpoe_traffic_cls_index_for_llid_cnt,cls_index);
            g_dpoe_traffic_cls_index_for_llid_cnt++;
        }
    }
    return OAM_E_OK;
}

oam_status dpoe_oam_llid_loopabck_set_adapt(oam_uint16 llid, oam_boolean en)
{
    ca_status_t ret = CA_E_OK;
    ca_mpcp_llid_t mpcp_llid = 0;

    mpcp_llid = oam_llid_get();
    ret =  ca_epon_llid_loopback_enable_set(0,CA_PORT_ID(CA_PORT_TYPE_EPON,0x07),mpcp_llid,en);
    if(CA_E_OK != ret){
        OAM_ORG_LOG("dpoe_oam_llid_loopabck_set_adapt: ca_epon_llid_loopback_enable_set failed\n\r");
        return ret;
    }

    return OAM_E_OK;
}

oam_status dpoe_oam_loopback_set_adapt(oam_uint16 port, oam_boolean en, oam_uint8 value)
{
    ca_status_t ret = CA_E_OK;
    ca_port_id_t ca_port;
    ca_port_loopback_mode_t mode = CA_PORT_LOOPBACK_NONE;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    if( 0x00 == value){
        mode = CA_PORT_LOOPBACK_PHY_LOCAL;
    }else if(0x01 == value){
        mode = CA_PORT_LOOPBACK_MAC_LOCAL;
    }else if(0x02 == value){
        mode  = CA_PORT_LOOPBACK_MAC_REMOTE;
    }

    OAM_ORG_DEBUG("port_id = 0x%x, mode = %d\r\n", ca_port, mode);
    ret = ca_port_loopback_set(0, ca_port, mode);
    if(ret != CA_E_OK){
        OAM_ORG_LOG_CRI("failed, ret = %d, port_id = 0x%x\r\n", ret, ca_port);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

oam_status dpoe_oam_laser_tx_off_get_adapt(oam_uint16 *value)
{
    /* None */
    return OAM_E_OK;
}

static void __laser_tx_power_on(void *data)
{
    system("echo out> /sys/class/gpio/gpio9/direction");
    system("echo 1  > /sys/class/gpio/gpio9/value");
    g_laser_tx_power_timer = 0;
}

oam_status dpoe_oam_laser_tx_off_set_adapt(oam_uint16 value)
{
     ca_port_id_t pon_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();

    if (g_laser_tx_power_timer != 0) { /*stop timer incase called before previous timer expired */
        ca_timer_del(g_laser_tx_power_timer);
        g_laser_tx_power_timer = 0;
    }

    if(0 == value){
        //enable
        system("echo out> /sys/class/gpio/gpio9/direction");
        system("echo 1  > /sys/class/gpio/gpio9/value");
    }else if (0xFFFF == value){
        system("echo 0  > /sys/class/gpio/gpio9/value");
    }else
    {
       system("echo 0  > /sys/class/gpio/gpio9/value");
        g_laser_tx_power_timer  = ca_timer_add(value , __laser_tx_power_on, NULL);
    }
    return OAM_E_OK;
}

oam_status dpoe_oam_mc_ctrl_set_adapt(oam_dpoe_onu_mc_ctrl_t *cfg)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_l2_mcast_entry_t entry;

    memset(&entry, 0, sizeof(ca_l2_mcast_entry_t));

    ca_l2_mcast_group_add(0, &entry);
#endif
    return OAM_E_OK;
}

oam_status dpoe_oam_mc_reg_set_adapt(oam_dpoe_onu_mc_reg_t *cfg)
{
    /* None */
    return OAM_E_OK;
}


oam_uint32 dpoe_oam_transok_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.tx_uc_frames + data.tx_mc_frames + data.tx_bc_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_scoll_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.single_collision_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_mcoll_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.multiple_collision_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_frm_recv_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.rx_uc_frames + data.rx_mc_frames + data.rx_bc_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_fcserr_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.rx_fcs_error_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_alignerr_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.alignmentErrors);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_octtranok_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.tx_good_octets);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_frmdef_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.frames_with_deferredXmissions);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_latecoll_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.late_collisions);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_frmabort_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.frames_aborted_dueto_XS_colls);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_frmlost1_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.frames_lost_duetoint_mac_rcv_error);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_octok_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.rx_good_octets);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_frmlost2_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.frames_lost_duetoint_mac_xmit_error);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_mcxmit_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.tx_mc_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_bcxmit_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.tx_bc_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_frmexc_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.frames_with_excessive_deferral);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_mcrecv_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.rx_mc_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_bcrecv_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.rx_bc_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_inlenerr_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.in_rangeLength_errors);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_outlenerr_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.outof_range_length_field);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_giantlenerr_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.rx_oversize_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_macstat_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    oam_uint32 data = 0;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    if((ca_port == CA_PORT_ID(CA_PORT_TYPE_CPU,0x12))||(ca_port == CA_PORT_ID(CA_PORT_TYPE_CPU,0x13))){
        return 1;
    }else{
        ca_port_enable_get(0, ca_port, &data);
        return data;
    }
#else
    return 0;
#endif
}

void dpoe_oam_macstat_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 state)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    oam_uint32 data = state;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_port_enable_set(0, ca_port, data);
#endif
}

oam_uint32 dpoe_oam_collfrm_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;
    memset(&data,0,sizeof(ca_eth_port_stats_t));

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_eth_port_stats_get(0, ca_port, TRUE, &data);
    return (data.phy_stats.collision_frames);
#else
    return 0;
#endif
}

oam_uint32 dpoe_oam_symerr_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port ;
    ca_status_t ret = CA_E_OK;

    memset(&data,0,sizeof(ca_eth_port_stats_t));

    if(OAM_DPOE_LEAF_OBJ_UNI == type){
        port = value;
        DPOE_OAM_UNI_PORT_ID_CHECK(port);
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    }else{
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
    }
    ret = ca_eth_port_stats_get(0, ca_port, FALSE, &data);
    if(ret != CA_E_OK){
        OAM_ORG_LOG(" dpoe_oam_symerr_get_adapt:ca_eth_port_stats_get() failed\n\r");
        return 0;
    }
    return (data.phy_stats.symbol_error_during_carrier);
#else
    return 0;
#endif
}

void dpoe_oam_mac_addr_get_adapt(oam_uint8 *mac)
{

}

void dpoe_oam_mac_addr_set_adapt(oam_uint8 *mac)
{

}

oam_uint8 dpoe_oam_mdavail_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_eth_port_stats_t data;
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;
    ca_eth_port_link_status_t link_status;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    OAM_ORG_LOG("ca_port in dpoe_oam_mdavail_get_adapt is 0x%x\n\r" ,ca_port);

    if((ca_port == CA_PORT_ID(CA_PORT_TYPE_CPU,0x12))||(ca_port == CA_PORT_ID(CA_PORT_TYPE_CPU,0x13))){
         return 0x03;
    }else{
        ca_eth_port_link_status_get(0,ca_port,&link_status);

        if(CA_PORT_LINK_UP == link_status)
        {
            return 0x03;
        }
        else
        {
            return 0x04;
        }
    }
#else
    return 0x04;
#endif
}

oam_uint8 dpoe_oam_autonegadmin_get_adapt(oam_uint8 type, oam_uint8 value)
{
    ca_port_id_t ca_port = 0;
    oam_uint16 port = value;
    ca_boolean AutoNegAdminState = TRUE;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    if((ca_port == CA_PORT_ID(CA_PORT_TYPE_CPU,0x12))||(ca_port == CA_PORT_ID(CA_PORT_TYPE_CPU,0x13))){
        return 0x2;
    }else{
    ca_eth_port_autoneg_get(0,ca_port,&AutoNegAdminState);
        if(AutoNegAdminState){
            return 0x2;
        }else{
            return 0x1;
        }
    }
}

oam_uint8 dpoe_oam_autonegsig_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

oam_uint8 dpoe_oam_autonegcfg_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 3;
}

void dpoe_oam_autonegcfg_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 set)
{

}

oam_uint8 dpoe_oam_autoneglctab_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 16;
}

oam_uint8 dpoe_oam_autonegadvtab_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 16;
}

void dpoe_oam_autonegadvtab_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 set)
{

}

oam_uint8 dpoe_oam_autonegrcvtab_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 16;
}

oam_uint8 dpoe_oam_autoneglcsab_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 16;
}

oam_uint8 dpoe_oam_autonegadvsab_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 16;
}

void dpoe_oam_autonegadvsab_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 set)
{
}

oam_uint8 dpoe_oam_autonegrecvsab_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 16;
}

oam_uint8 dpoe_oam_duplex_get_adapt(oam_uint8 type, oam_uint8 value)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN

    oam_uint16 port = value;
    ca_port_id_t ca_port = 0;
    ca_eth_port_duplex_t port_duplex;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    if((ca_port == CA_PORT_ID(CA_PORT_TYPE_CPU,0x12))||(ca_port == CA_PORT_ID(CA_PORT_TYPE_CPU,0x13))){
        return 0x02;
    }else{
        ca_eth_port_duplex_get(0, ca_port,&port_duplex);

        if(CA_PORT_DUPLEX_HALF==port_duplex){
            return 0x01;
        }else{
            return 0x02;
        }
    }
#else
    return 0x2;
#endif
}

void dpoe_oam_duplex_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 set)
{
    oam_uint16 port = value;
    ca_port_id_t ca_port = 0;
    ca_eth_port_duplex_t port_duplex =CA_PORT_DUPLEX_AUTO ;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    if(0x01 == set){
        port_duplex = CA_PORT_DUPLEX_HALF;
    }else if(0x02 == set){
        port_duplex = CA_PORT_DUPLEX_FULL;
    }

    ca_eth_port_duplex_set(0, ca_port, port_duplex);
}

oam_uint32 dpoe_oam_macxmit_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

oam_uint32 dpoe_oam_macrecv_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

oam_uint32 dpoe_oam_notspop_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

oam_uint32 dpoe_oam_pauseallow_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

void dpoe_oam_pauseallow_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint32 set)
{

}

oam_uint64 dpoe_oam_pausexmit_get_adapt(oam_uint8 type, oam_uint8 value)
{
    ca_port_id_t ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(value);
    ca_eth_port_stats_t port_status;
    ca_status_t ret = CA_E_OK;
    memset(&port_status,0,sizeof(ca_eth_port_stats_t));

    ret = ca_eth_port_stats_get(0,ca_port,0,&port_status);
    if(ret!= CA_E_OK){
        OAM_ORG_LOG("ca_eth_port_stats_get() failed\n\r");
        return ret;
    }

    return (port_status.tx_pause_frames);
}

oam_uint64 dpoe_oam_pauserecv_get_adapt(oam_uint8 type, oam_uint8 value)
{
    ca_port_id_t ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(value);
    ca_eth_port_stats_t port_status;
    ca_status_t ret = CA_E_OK;
    memset(&port_status,0,sizeof(ca_eth_port_stats_t));

    ret = ca_eth_port_stats_get(0,ca_port,0,&port_status);
    if(ret!= CA_E_OK){
        OAM_ORG_LOG("ca_eth_port_stats_get() failed\n\r");
        return ret;
    }

    return (port_status.rx_pause_frames);
}

oam_uint32 dpoe_oam_mpcpxmit_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

oam_uint32 dpoe_oam_mpcprecv_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 0;
}

oam_uint32 dpoe_oam_feccb_get_adapt(oam_uint8 type, oam_uint32 *value)
{
    ca_epon_port_fec_stats_t fec_stats;
    memset(&fec_stats,0,sizeof(ca_epon_port_fec_stats_t));

    ca_port_id_t ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
    ca_epon_port_fec_stats_get(0,ca_port,FALSE,&fec_stats);
    *value = fec_stats.corrected_codewords;
    return 0;
}

oam_uint32 dpoe_oam_fecucb_get_adapt(oam_uint8 type, oam_uint32 *value)
{
    ca_epon_port_fec_stats_t fec_stats;
    memset(&fec_stats,0,sizeof(ca_epon_port_fec_stats_t));

    ca_port_id_t ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
    ca_epon_port_fec_stats_get(0,ca_port,FALSE,&fec_stats);
    *value = fec_stats.uncorrectable_codewords;

    return 0;
}

oam_uint8 dpoe_oam_fecmode_get_adapt(oam_uint8 type, oam_uint8 value)
{
    return 3;
}

void dpoe_oam_fecmode_set_adapt(oam_uint8 type, oam_uint8 value, oam_uint8 set)
{

}


void change_precedence(oam_uint8 *prec)
{
    if((0==*prec)||(1==*prec)){
       *prec = 7;
    }else if((*prec>=2)&&(*prec<=6)){
       *prec = 6;
    }else if((*prec>=7)&&(*prec<=8)){
       *prec = 5;
    }else if((*prec>=9)&&(*prec<=10)){
       *prec = 4;
    }else if((*prec>=11)&&(*prec<=12)){
       *prec = 3;
    }else if((*prec>=13)&&(*prec<=14)){
       *prec = 2;
    }else if(15==*prec){
       *prec = 1;
    }else{
       *prec = 0;
    }
}

void dpoe_get_src_mac_for_ubq(ca_mac_addr_t *mac2,ca_mac_addr_t *mac3)
{
    ca_device_config_tlv_t scfg_tlv;
    ca_status_t ret = CA_E_OK;

    CA_ASSERT_RET(mac2,CA_E_PARAM);
    CA_ASSERT_RET(mac3,CA_E_PARAM);

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_NE_MAC_ADDR2;
    scfg_tlv.len = sizeof(ca_mac_addr_t);
    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if (CA_E_OK != ret) {
        OAM_LOG_DEBUG("read CA_CFG_ID_NE_MAC_ADDR2 fail. ret = %d \r\n", ret);
        return OAM_E_ERROR;
    }
    memcpy(mac2,scfg_tlv.value,sizeof(ca_mac_addr_t));

    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_NE_MAC_ADDR3;
    scfg_tlv.len = sizeof(ca_mac_addr_t);
    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if (CA_E_OK != ret) {
        OAM_LOG_DEBUG("read CA_CFG_ID_NE_MAC_ADDR3 fail. ret = %d \r\n", ret);
        return OAM_E_ERROR;
    }
    memcpy(mac3,scfg_tlv.value,sizeof(ca_mac_addr_t));
}

void _add_vlan_member(ca_uint16_t vid, ca_port_id_t member_port, ca_port_id_t untagged_port)
{
    ca_uint8_t member_count = 0;
    ca_port_id_t member_ports[8];
    ca_uint8_t untagged_count = 0;
    ca_port_id_t untagged_ports[8];
    ca_status_t rc;

    if (!member_port && !untagged_port)
        return;

    rc = ca_l2_vlan_create(0 , vid);
    if (rc == CA_E_OK || CA_E_EXISTS)
    {
        ca_l2_vlan_port_get(0, vid, &member_count, member_ports, &untagged_count,untagged_ports);
        if (member_port)
        {
            member_ports[member_count] =  member_port;
            member_count ++;
        }
        if (untagged_port)
        {
            untagged_ports[untagged_count] = untagged_port;
            untagged_count++;
        }
        ca_l2_vlan_port_set(0, vid,member_count,member_ports,untagged_count,untagged_ports);
    }
}

void _set_vlan_default_tag(ca_port_id_t ca_port){
    ca_vlan_port_control_t vlan_port_ctl;
    memset(&vlan_port_ctl,0,sizeof(ca_vlan_port_control_t));

    ca_l2_vlan_port_control_get(0,ca_port,&vlan_port_ctl);
    vlan_port_ctl.default_tag = 0;
    ca_l2_vlan_port_control_set(0,ca_port,&vlan_port_ctl);
    OAM_ORG_LOG("\t set vlan_port_config.default_tag to 0\n\r");

}

void dpoe_cls_rule_merge(dpoe_oam_frm_process_t *rule)
{
    int j=0;
    OAM_ORG_LOG("Start to rule merge\n\r");
    if((rule->cvid_flag)&&(0==rule->llid_flag))
    {
        for(j=0;j<16;j++)
        {
            if(g_dpoe_rule[j].frm_prc.cvid_flag
                && g_dpoe_rule[j].frm_prc.llid_flag
                &&(rule->cvid == g_dpoe_rule[j].frm_prc.cvid))
                {
                    rule->llid = g_dpoe_rule[j].frm_prc.llid;
                    rule->llid_flag = 1;
                    OAM_ORG_LOG("Find the match vid and llid,vid=%d,llid=%d\n\r",rule->cvid,rule->llid);
                    break;
                }
        }
    }
}

static ca_status_t __dpoe_cls_rule_check_for_mc(ca_uint32_t cls_index,
    ca_uint32_t                 new_priority,
    ca_classifier_key_t         *new_key,
    ca_classifier_key_mask_t    *new_key_mask,
    ca_classifier_action_t      *new_action,
    ca_uint32_t                 *new_cls_index)
 {
    ca_uint32_t              cls_priority =0;
    ca_classifier_key_t      cls_key;
    ca_classifier_key_mask_t cls_key_mask;
    ca_classifier_action_t   cls_action;
    ca_status_t ret = CA_E_OK;
    
    memset(&cls_key,0,sizeof(ca_classifier_key_t));
    memset(&cls_key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&cls_action,0,sizeof(ca_classifier_action_t));

    ret = ca_classifier_rule_get(0,cls_index,&cls_priority,&cls_key,&cls_key_mask,&cls_action);
    if(ret!=CA_E_OK){
        OAM_ORG_LOG("MC:ca_classifier_rule_get failed\n\r");
        return ret;
    }
    ret = ca_classifier_rule_delete(0,cls_index);
    if(ret!=CA_E_OK){
        OAM_ORG_LOG("MC:ca_classifier_rule_delete failed\n\r");
        return ret;
    }

    if(new_priority >= cls_priority){
        ret = ca_classifier_rule_add(0,new_priority,new_key,new_key_mask,new_action,new_cls_index);
        if(ret!=CA_E_OK){
            OAM_ORG_LOG("MC:ca_classifier_rule_add() with new priority failed\n\r");
            return ret;
        }
    }else{
        ret = ca_classifier_rule_add(0,cls_priority,new_key,new_key_mask,&cls_action,new_cls_index);
        if(ret!=CA_E_OK){
            OAM_ORG_LOG("MC:ca_classifier_rule_add() with old priority failed\n\r");
            return ret;
        }        
    }
    OAM_ORG_LOG("MC:rule cls index = %d\n\r",*new_cls_index);
    return ret;
}


ca_uint32_t dpoe_config_rule(oam_uint8 type, oam_uint16 port, dpoe_oam_frm_process_t *rule)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_port_id_t ca_port;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    ca_uint32_t index = 0,new_cls_index=0;
    ca_port_id_t llid;
    oam_uint8 llid_index = 0;
    oam_uint8 vlan_config = 0;
    ca_vlan_port_control_t vlan_port_config;
    ca_status_t rc;
    oam_boolean is_ubq = FALSE;
    ca_mac_addr_t mac2;
    ca_mac_addr_t mac3;
    oam_uint8 i =0,j=0,k=0,n=0;
    ca_uint32_t inner_tpid[5];
    ca_uint32_t inner_num = 0;
    ca_uint32_t outer_tpid[5];
    ca_uint32_t outer_num = 0;
    ca_uint32_t mc_llid_num = 64;
    ca_port_id_t  mc_llids[64];
    ca_status_t ret = CA_E_OK;
    ca_l2_mcast_group_members_t dpoe_mcast_group_mem;
    oam_llid_t llid_sw_index = 0;
    ca_uint32_t mc_llid_nums = 64;
    ca_mpcp_llid_t tmp_llid[64];
    ca_l2_mcast_entry_t dpoe_l2_mcast_entry;

    ca_port_id_t uni_port;
    ca_port_id_t pon_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
    ca_port_id_t dest_port = 0;
    memset(&key, 0, sizeof(ca_classifier_key_t));
    memset(&key_mask, 0, sizeof(ca_classifier_key_mask_t));
    memset(&action, 0, sizeof(ca_classifier_action_t));
    memset(mac2,0,sizeof(ca_mac_addr_t));
    memset(mac3,0,sizeof(ca_mac_addr_t));
    memset(inner_tpid,0,sizeof(inner_tpid));
    memset(outer_tpid,0,sizeof(outer_tpid));
    memset(mc_llids,0,sizeof(mc_llids));
    memset(&dpoe_mcast_group_mem,0,sizeof(ca_l2_mcast_group_members_t));
    memset(&dpoe_l2_mcast_entry,0,sizeof(ca_l2_mcast_entry_t));
    memset(tmp_llid,0,64*sizeof(ca_mpcp_llid_t));

    if (type == OAM_DPOE_LEAF_OBJ_UNI){
        DPOE_OAM_UNI_PORT_ID_CHECK(port);
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    }
    else
    {
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        key.key_handle.llid_cos_index = (rule->llid << 11) + (7 << 8);
        key_mask.key_handle = 1;
    }

    is_ubq = oam_dpoe_ubq_check();

    change_precedence(&rule->precedence);
    OAM_ORG_LOG("Start to add cls rule\n");
    OAM_ORG_LOG("add rule, precedence %d\r\n", rule->precedence);

    if (rule->llid_flag)
    {
        OAM_ORG_LOG("\t key.key_handle.llid_cos_index 0x%x\r\n", rule->llid);
        j=0;
        ret= ca_siepon_mc_llid_list(0, &mc_llid_nums,tmp_llid);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("dpoe_config_rule:ca_siepon_mc_llid_list failed ,ret = %d\n\r",ret);
            return ret;
        }
        for(i=0;i<mc_llid_nums;i++){
            if(rule->llid == tmp_llid[i]){
                OAM_ORG_LOG("\t MC: tmp_llid[%d]=%d\n\r",i,tmp_llid[i]);
                _dpoe_find_mc_llid_index(tmp_llid[i],&llid_sw_index);
                key.handle_type =  CA_KEY_HANDLE_TYPE_LLID_COS;
        		key.key_handle.flow_id = llid_sw_index;
        		key_mask.key_handle = 1;

                OAM_ORG_LOG("MC:Handle_type:%d\n\r",key.handle_type);
                OAM_ORG_LOG("MC:llid_sw_index=%d\n\r",llid_sw_index);

                key.src_port = ca_port;
                key_mask.src_port = 1;
                OAM_ORG_LOG("MC:source port 0x%x\r\n", ca_port);
                srand((unsigned)time(NULL)); 
                dpoe_l2_mcast_entry.replication_id = rand();;
                OAM_ORG_LOG("MC:replication_id=%d\n\r",dpoe_l2_mcast_entry.replication_id);

                if (rule->l2_da_flag){
                    if(rule->l2_da_max_flag){
                       memcpy(key.l2.mac_da.mac_max, rule->l2_da_max, 6);
                       memcpy(dpoe_l2_mcast_entry.group_mac_addr,rule->l2_da_max, 6);
                    }else if(rule->l2_da_min_flag){
                       memcpy(key.l2.mac_da.mac_min, rule->l2_da_min, 6);
                       memcpy(dpoe_l2_mcast_entry.group_mac_addr,rule->l2_da_min, 6);
                    }else{
                       memcpy(key.l2.mac_da.mac_min, rule->l2_da, 6);
                       memcpy(key.l2.mac_da.mac_max, rule->l2_da, 6);
                       memcpy(dpoe_l2_mcast_entry.group_mac_addr,rule->l2_da, 6);
                    }
                    OAM_ORG_LOG("MC:group_mac_addr = 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n\r",
                        dpoe_l2_mcast_entry.group_mac_addr[0],
                        dpoe_l2_mcast_entry.group_mac_addr[1],
                        dpoe_l2_mcast_entry.group_mac_addr[2],
                        dpoe_l2_mcast_entry.group_mac_addr[3],
                        dpoe_l2_mcast_entry.group_mac_addr[4],
                        dpoe_l2_mcast_entry.group_mac_addr[5]);
                    key_mask.l2 = 1;
                    key_mask.l2_mask.mac_da = 0x3f;
                }

                if (rule->ipv4_da_flag){
                    key.ip.ip_da.afi = CA_IPV4;
                    key.ip.ip_da.ip_addr.ipv4_addr = (rule->ipv4_da)<<(rule->lsb_mask);
                    key.ip.ip_da.addr_len = 32;
                    key.ip.ip_da_max.ip_addr.ipv4_addr = key.ip.ip_da.ip_addr.ipv4_addr;
                    for(k=0;k<(rule->lsb_mask);k++){
                        key.ip.ip_da_max.ip_addr.ipv4_addr |=(1<<k);
                    }
                    key.ip.ip_da_max.addr_len = 32;
                    key_mask.ip = 1;
                    key_mask.ip_mask.ip_da = 1;
                    //key_mask.ip_mask.ip_da_max = 1;
                    memcpy(&(dpoe_l2_mcast_entry.group_ip_addr),&(key.ip.ip_da),sizeof(ca_ip_address_t));
                    OAM_ORG_LOG("MC:group_ip_addr&& ip_da: %d.%d.%d.%d\n\r",
                        key.ip.ip_da.ip_addr.ipv4_addr>>24&0xFF,
                        key.ip.ip_da.ip_addr.ipv4_addr>>16&0xFF,
                        key.ip.ip_da.ip_addr.ipv4_addr>>8&0xFF,
                        key.ip.ip_da.ip_addr.ipv4_addr&0xFF);
                }

                 if (rule->ipv4_sa_flag){
                    key.ip.ip_sa.afi = CA_IPV4;
                    key.ip.ip_sa.ip_addr.ipv4_addr = (rule->ipv4_sa)<<(rule->lsb_mask);
                    key.ip.ip_sa.addr_len = 32;
                    key.ip.ip_sa_max.ip_addr.ipv4_addr = key.ip.ip_sa.ip_addr.ipv4_addr;
                    for(k=0;k<(rule->lsb_mask);k++){
                        key.ip.ip_sa_max.ip_addr.ipv4_addr |=(1<<k);
                    }
                    key.ip.ip_sa_max.addr_len = 32;
                    key_mask.ip = 1;
                    key_mask.ip_mask.ip_sa = 1;
                    //key_mask.ip_mask.ip_sa_max = 1;
                    //memcpy(&(dpoe_l2_mcast_entry.src_ip_address),&(key.ip.ip_sa),sizeof(ca_ip_address_t));
                    OAM_ORG_LOG("MC:src_ip_address &&ip_sa:  %d.%d.%d.%d \n\r",
                        key.ip.ip_sa.ip_addr.ipv4_addr>>24&0XFF,
                        key.ip.ip_sa.ip_addr.ipv4_addr>>16&0xFF,
                        key.ip.ip_sa.ip_addr.ipv4_addr>>8&0xFF,
                        key.ip.ip_sa.ip_addr.ipv4_addr&0xFF);
                 }
                 
                if (rule->ipv6_sa_flag)
                {
                    key.ip.ip_sa.afi = CA_IPV6;
                    key.ip.ip_sa_max.afi = CA_IPV6;
                    memcpy(key.ip.ip_sa.ip_addr.ipv6_addr, rule->ipv6_sa, 4*sizeof(oam_uint32));
                    if(rule->ipv6_sa_max_flag){
                        memcpy(key.ip.ip_sa_max.ip_addr.ipv6_addr, rule->ipv6_sa_max, 4*sizeof(oam_uint32));
                    }else{
                        memcpy(key.ip.ip_sa_max.ip_addr.ipv6_addr, rule->ipv6_sa, 4*sizeof(oam_uint32));
                    }
                    key.ip.ip_sa_max.addr_len = 128;
                    key.ip.ip_sa.addr_len = 128;
                    key_mask.ip = 1;
                    key_mask.ip_mask.ip_sa = 1;
                    //key_mask.ip_mask.ip_sa_max = 1;
                }
                
                if (rule->ipv6_da_flag)
                {
                    key.ip.ip_da.afi = CA_IPV6;
                    key.ip.ip_da_max.afi = CA_IPV6;
                    memcpy(key.ip.ip_da.ip_addr.ipv6_addr, rule->ipv6_da, 4*sizeof(oam_uint32));
                    if(rule->ipv6_da_max_flag){
                        memcpy(key.ip.ip_da_max.ip_addr.ipv6_addr, rule->ipv6_da_max, 4*sizeof(oam_uint32));
                    }else{
                        memcpy(key.ip.ip_da_max.ip_addr.ipv6_addr, rule->ipv6_da, 4*sizeof(oam_uint32));
                    }
                    key.ip.ip_da.addr_len = 128;
                    key.ip.ip_da_max.addr_len = 128;
                    key_mask.ip = 1;
                    key_mask.ip_mask.ip_da = 1;
                    //key_mask.ip_mask.ip_da_max = 1;
                }

                if (rule->cvlan_not_exist)
                {
                    key.l2.vlan_count = 0;
                    key_mask.l2 = 1;
                    key_mask.l2_mask.vlan_count = 1;
                    dpoe_l2_mcast_entry.mcast_vlan = 0;
                    OAM_ORG_LOG("MC:cast_vlan = %d\n\r",dpoe_l2_mcast_entry.mcast_vlan);
                    OAM_ORG_LOG("\t cvlan not exist , vlan_count=0\r\n");
                }
                if (rule->cvid_flag)
                {
                    key.l2.vlan_otag.vlan_min.vid = rule->cvid;
                    key.l2.vlan_otag.vlan_max.vid = rule->cvid;
                    key.l2.vlan_count = 1;
                    key_mask.l2 = 1;
                    key_mask.l2_mask.vlan_count = 1;
                    key_mask.l2_mask.vlan_otag_mask.vid = 1;
                    key_mask.l2_mask.vlan_otag = 1;
                    dpoe_l2_mcast_entry.mcast_vlan = rule->cvid;
                    OAM_ORG_LOG("MC:cast_vlan = %d\n\r",dpoe_l2_mcast_entry.mcast_vlan);
                    OAM_ORG_LOG("\t cvlan 0x%x, vlan_count=1\r\n", rule->cvid);
                }

                if(0x86DD == rule->l2_type){
                    rule->l2_type_flag = 1;
                    rule->ip_version_flag = 0;
                }

                if (rule->l2_type_flag)
                {
                    key.l2.ethertype = rule->l2_type;
                    key_mask.l2 = 1;
                    key_mask.l2_mask.ethertype = 1;
                    OAM_ORG_LOG("\t MC:ethernet type 0x%x\n\r", rule->l2_type);
                }

                if(rule->ip_version_flag){
                    key.ip.ip_version = rule->ip_version;
                    key.ip.ip_valid = 1;
                    //key_mask.ip = 1;
                    //key_mask.ip_mask.ip_version = 1;
                }
                
                if(rule->result.result_action_flag){
                     if (rule->result.result_action == RULE_RESULT_DISC)
                    {
                        action.forward = CA_CLASSIFIER_FORWARD_DENY;
                        OAM_ORG_LOG("MC: result DISCARD \r\n");
                    }
                    else if(rule->result.result_action == RULE_RESULT_FWD)
                    {   OAM_ORG_LOG("MC:result action Forward\n\r");
                        action.forward = CA_CLASSIFIER_FORWARD_FE;
                    }
                }
                
                if (rule->result.result_queue_flag){
                    if (2 == rule->result.result_queue_type)
                    {   // LLID
                        action.options.masks.action_handle = 1;
                        action.options.action_handle.flow_id = llid_sw_index;
                        OAM_ORG_LOG("\t MC: flow id %d\r\n", action.options.action_handle.flow_id);
                        if (rule->cvid_flag)
                        {
                            _add_vlan_member(rule->cvid, ca_port, 0);
                            _add_vlan_member(rule->cvid, pon_port, 0);
                            OAM_ORG_LOG("\t MC:add CVLAN member 0x%x\r\n", ca_port);
                            OAM_ORG_LOG("\t MC:add CVLAN member 0x%x\r\n", pon_port);
                        }
                        if (rule->svid_flag)
                        {
                            _add_vlan_member(rule->svid, ca_port, 0);
                            _add_vlan_member(rule->svid, pon_port, 0);
                            OAM_ORG_LOG("\t MC:add SVLAN member 0x%x\r\n", ca_port);
                            OAM_ORG_LOG("\t MC:add SVLAN member 0x%x\r\n", pon_port);
                        }
                    }
                     else { // UNI
                        OAM_ORG_LOG("MC:rule->result.result_queue_number=%d\n\r",rule->result.result_queue_number);
                        for(n=0;n<rule->result.result_queue_number;n++){
                            OAM_ORG_LOG("MC:rule->result.result_queue_instance[%d]=%d",n,rule->result.result_queue_instance[n]);
                            uni_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(rule->result.result_queue_instance[n]);     
                            dpoe_mcast_group_mem.member[j++].member_port = uni_port;
                            dpoe_mcast_group_mem.member_count = j;
                            action.forward = CA_CLASSIFIER_FORWARD_PORT;
                            OAM_ORG_LOG("MC:add uni port to member,uni_port = 0x%8x,member_count = %d\n\r",uni_port,dpoe_mcast_group_mem.member_count);
                            if (rule->cvid_flag)
                            {
                                _add_vlan_member(rule->cvid, ca_port, 0);
                                _add_vlan_member(rule->cvid, uni_port, 0);
                                OAM_ORG_LOG("\tMC: add CVLAN member 0x%x\r\n", ca_port);
                                OAM_ORG_LOG("\tMC: add CVLAN member 0x%x\r\n", uni_port);
                            }
                            if (rule->svid_flag)
                            {
                                _add_vlan_member(rule->svid, ca_port, 0);
                                _add_vlan_member(rule->svid, uni_port, 0);
                                OAM_ORG_LOG("\t MC:add SVLAN member 0x%x\r\n", uni_port);
                            }
                        }
                    }
                }

                ret = ca_l2_mcast_group_add(0,&dpoe_l2_mcast_entry);
                if(ret!= CA_E_OK)
                {
                   OAM_ORG_LOG(" ca_l2_mcast_group_add() failed ,ret =%d \n\r",ret);
                   return ret;
                }

                action.options.mcg_id = dpoe_l2_mcast_entry.mcg_id;
                action.options.masks.mcg_id = 1;
                dpoe_mcast_group_mem.mcg_id = dpoe_l2_mcast_entry.mcg_id;

                ret = ca_l2_mcast_member_add(0,&dpoe_mcast_group_mem);
                if(ret != CA_E_OK){
                	OAM_ORG_LOG("ca_l2_mcast_member_add failed ,ret = %d\n\r",ret);
                	return ret;
                }

                OAM_ORG_LOG("MC:action.forward = %d,action.options.mcg_id = %d , flow_id = %d\n\r",
                        action.forward,action.options.mcg_id,key.key_handle.flow_id );

                rc = ca_classifier_rule_add(0,rule->precedence,&key,&key_mask,&action, &index);
                OAM_ORG_LOG("MC:add cls index %d, result %d\r\n", index, rc);
                if(rc == CA_E_EXISTS){
                    OAM_ORG_LOG("MC:add rule failed withi result CA_E_EXISTS, Try to merge the rule\n\r");
                    ret =__dpoe_cls_rule_check_for_mc(index,rule->precedence,&key,&key_mask,&action,&new_cls_index);
                    OAM_ORG_LOG("MC:Second add rule cls index = %d,result = %d\n\r",new_cls_index,ret);
                    return new_cls_index;
                }else{
                    return index;
                }            
            }
        }

        key.key_handle.llid_cos_index = (rule->llid << 11) + (7 << 8);
        key_mask.key_handle = 1;
    }

    key.src_port = ca_port;
    key_mask.src_port = 1;

    OAM_ORG_LOG("\t source port 0x%x\r\n", ca_port);

    if (rule->l2_da_flag)
    {
        if(rule->l2_da_max_flag){
           memcpy(key.l2.mac_da.mac_max, rule->l2_da_max, 6);
           OAM_ORG_LOG("\t dst mac max 0x%x:%x:%x:%x:%x:%x \r\n", rule->l2_da_max[0], rule->l2_da_max[1], rule->l2_da_max[2],
                rule->l2_da_max[3], rule->l2_da_max[4], rule->l2_da_max[5]);
        }

        if(rule->l2_da_min_flag){
           memcpy(key.l2.mac_da.mac_min, rule->l2_da_min, 6);
           OAM_ORG_LOG("\t dst mac min 0x%x:%x:%x:%x:%x:%x \r\n", rule->l2_da_min[0], rule->l2_da_min[1], rule->l2_da_min[2],
                rule->l2_da_min[3], rule->l2_da_min[4], rule->l2_da_min[5]);
        }

        if(!rule->l2_da_max_flag&&!rule->l2_da_min_flag){
           memcpy(key.l2.mac_da.mac_max, rule->l2_da, 6);
           memcpy(key.l2.mac_da.mac_min, rule->l2_da, 6);
           OAM_ORG_LOG("\t dst mac 0x%x:%x:%x:%x:%x:%x \r\n", rule->l2_da[0], rule->l2_da[1], rule->l2_da[2],
                rule->l2_da[3], rule->l2_da[4], rule->l2_da[5]);
        }
        key_mask.l2 = 1;
        key_mask.l2_mask.mac_da = 0x3f;
    }
    if (rule->l2_sa_flag)
    {
        if(rule->l2_sa_max_flag){
            memcpy(key.l2.mac_sa.mac_max, rule->l2_sa_max, 6);
            OAM_ORG_LOG("\t src mac max 0x%x:%x:%x:%x:%x:%x \r\n", rule->l2_sa_max[0], rule->l2_sa_max[1], rule->l2_sa_max[2],
                rule->l2_sa_max[3], rule->l2_sa_max[4], rule->l2_sa_max[5]);
        }

        if(rule->l2_sa_min_flag){
            memcpy(key.l2.mac_sa.mac_min, rule->l2_sa_min, 6);
            OAM_ORG_LOG("\t src mac min 0x%x:%x:%x:%x:%x:%x \r\n", rule->l2_sa_min[0], rule->l2_sa_min[1], rule->l2_sa_min[2],
                rule->l2_sa_min[3], rule->l2_sa_min[4], rule->l2_sa_min[5]);
        }
        if(!rule->l2_sa_max_flag&&!rule->l2_sa_min_flag){
            memcpy(key.l2.mac_sa.mac_max, rule->l2_sa, 6);
            memcpy(key.l2.mac_sa.mac_min, rule->l2_sa, 6);
            OAM_ORG_LOG("\t src mac 0x%x:%x:%x:%x:%x:%x \r\n", rule->l2_sa[0], rule->l2_sa[1], rule->l2_sa[2],
                rule->l2_sa[3], rule->l2_sa[4], rule->l2_sa[5]);
        }
        key_mask.l2 = 1;
        key_mask.l2_mask.mac_sa = 0x3f;
    }

    if (rule->l2_type_flag)
    {
        key.l2.ethertype = rule->l2_type;
        key_mask.l2 = 1;
        key_mask.l2_mask.ethertype = 1;
        OAM_ORG_LOG("\t ethernet type 0x%x\n\r", rule->l2_type);
    }

    if(rule->ip_version_flag){
        key.ip.ip_version = rule->ip_version;
        key.ip.ip_valid = 1;
        key_mask.ip = 1;
        key_mask.ip_mask.ip_version = 1;
    }

    if (rule->stpid_flag)
    {
    #if 0
        key.l2.vlan_otag.vlan_min.tpid = rule->stpid;
        key_mask.l2 = 1;
        key_mask.l2_mask.vlan_otag_mask.tpid = 1;
    #endif
    }
    #if 0
    if (rule->spri_flag)
    {
        key.l2.vlan_otag.pri = rule->spri;
        key_mask.l2 = 1;
        key_mask.l2_mask.vlan_otag_mask.pri = 1;
    }
    #endif
    if ((rule->svid_flag)&&(!rule->cvid_flag))
    {
        if(rule->svid_max_flag){
            key.l2.vlan_otag.vlan_max.vid = rule->svid_max;
            OAM_ORG_LOG("\t only svlan:svlan max 0x%x\r\n", rule->svid_max);
        }

        if(rule->svid_min_flag){
            key.l2.vlan_otag.vlan_min.vid = rule->svid_min;
            OAM_ORG_LOG("\t only svlan :svlan min 0x%x\r\n", rule->svid_min);
        }

        if(!rule->svid_max_flag&&!rule->svid_min_flag){
            key.l2.vlan_otag.vlan_min.vid = rule->svid;
            key.l2.vlan_otag.vlan_max.vid = rule->svid;
            OAM_ORG_LOG("\t only svlan: svlan  0x%x\r\n", rule->svid);
        }
        key_mask.l2 = 1;
        key_mask.l2_mask.vlan_otag_mask.vid = 1;
        key_mask.l2_mask.vlan_otag = 1;
        vlan_config = 1;

        ca_l2_vlan_tpid_get(0,inner_tpid,&inner_num,outer_tpid,&outer_num);

        if(outer_num>1){
            for(i=0;i<outer_num;i++){
                if(outer_tpid[i] == 0x8100){
                    outer_tpid[i] = 0x88a8;
                    j++;
                    break;
                }
            }
            if(0==j){
                outer_tpid[outer_num-1] = 0x88a8;
            }
        }else{
            outer_num= outer_num+1;
            outer_tpid[outer_num-1]=0x88a8;
        }

        ca_l2_vlan_tpid_set(0,inner_tpid,inner_num,outer_tpid,outer_num);
        ca_l2_vlan_outer_tpid_add(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x06),outer_num-1);

        ca_l2_vlan_tpid_get(0,inner_tpid,&inner_num,outer_tpid,&outer_num);

        for(i=0;i<outer_num;i++){
            if(outer_tpid[i] == 0x88A8){
                j=i;
                OAM_ORG_LOG("Found 0x88A8 ,index = %d\n\r",j);
            }
            if(outer_tpid[i] == 0x8100){
                OAM_ORG_LOG("Found 0x8100 ,index = %d\n\r", i);
                ca_l2_vlan_outer_tpid_delete(0,ca_port,i);
                //ca_l2_vlan_outer_tpid_add(0,ca_port,j);

            }
        }
    }

    if((rule->svid_flag)&&(rule->cvid_flag)){
        if(rule->svid_max_flag){
            key.l2.vlan_otag.vlan_max.vid = rule->svid_max;
            OAM_ORG_LOG("Both svlan and cvlan: svid_max 0x%x\n\r",key.l2.vlan_otag.vlan_max.vid);
        }

        if(rule->svid_min_flag){
            key.l2.vlan_otag.vlan_min.vid = rule->svid_min;
            OAM_ORG_LOG("Both svlan and cvlan: svid_min 0x%x\n\r",key.l2.vlan_otag.vlan_min.vid);
        }

        if(!rule->svid_max_flag&&!rule->svid_min_flag){
            key.l2.vlan_otag.vlan_min.vid = rule->svid;
            key.l2.vlan_otag.vlan_max.vid = rule->svid;
            OAM_ORG_LOG("Both svlan and cvlan: svid_min 0x%x ,svid_max 0x%x\n\r",rule->svid,rule->svid);
        }

        key_mask.l2 = 1;
        key_mask.l2_mask.vlan_otag_mask.vid = 1;
        key_mask.l2_mask.vlan_otag = 1;

        if(rule->cvid_max_flag){
             key.l2.vlan_itag.vlan_max.vid = rule->cvid_max;
             OAM_ORG_LOG("Both svlan and cvlan: cvid_max 0x%x\n\r",rule->cvid_max);
        }

        if(rule->cvid_min_flag){
            key.l2.vlan_itag.vlan_min.vid = rule->cvid_min;
            OAM_ORG_LOG("Both svlan and cvlan: cvid_min 0x%x\n\r",rule->cvid_min);
        }

        if(!rule->cvid_max_flag&&!rule->cvid_min_flag){
            key.l2.vlan_itag.vlan_max.vid = rule->cvid;
            key.l2.vlan_itag.vlan_min.vid = rule->cvid;
            OAM_ORG_LOG("Both svlan and cvlan: cvid_min 0x%x ,cvid_max 0x%x\n\r",rule->cvid,rule->cvid);
        }

        key_mask.l2_mask.vlan_itag_mask.vid = 1;
        key_mask.l2_mask.vlan_itag = 1;

        OAM_ORG_LOG("Outer vlan = 0x%x ,Inner vlan = 0x%x\n\r",rule->svid,rule->cvid);
        memset(inner_tpid,0,sizeof(inner_tpid));
        memset(outer_tpid,0,sizeof(outer_tpid));
        inner_num = 0;
        outer_num = 0;
        j= 0;
        ca_l2_vlan_tpid_get(0,inner_tpid,&inner_num,outer_tpid,&outer_num);

        if(outer_num>1){
            for(i=0;i<outer_num;i++){
                if(outer_tpid[i] == 0x8100){
                    outer_tpid[i] = 0x88a8;
                    j++;
                    break;
                }
            }
            if(0==j){
                outer_tpid[outer_num-1] = 0x88a8;
            }
        }else{
            outer_num= outer_num+1;
            outer_tpid[outer_num-1]=0x88a8;
        }

        ca_l2_vlan_tpid_set(0,inner_tpid,inner_num,outer_tpid,outer_num);
        ca_l2_vlan_outer_tpid_add(0,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x06),outer_num-1);

        ca_l2_vlan_tpid_get(0,inner_tpid,&inner_num,outer_tpid,&outer_num);

        for(i=0;i<outer_num;i++){
            if(outer_tpid[i] == 0x88A8){
                j=i;
                OAM_ORG_LOG("Found 0x88A8 ,index = %d ,skip it\n\r",j);
            }
            if(outer_tpid[i] == 0x8100){
                OAM_ORG_LOG("Found 0x8100 ,index = %d ,so delete it\n\r", i);
                ca_l2_vlan_outer_tpid_delete(0,ca_port,i);
                //ca_l2_vlan_outer_tpid_add(0,ca_port,j);
            }
        }
    }

    if (rule->ctpid_flag)
    {
    #if 0
        key.l2.vlan_otag.vlan_min.tpid = rule->ctpid;
        key_mask.l2 = 1;
        key_mask.l2_mask.vlan_otag_mask.tpid = 1;
    #endif
    }
    #if 0
    if (rule->cpri_flag)
    {
        key.l2.vlan_itag.pri = rule->cpri;
        key_mask.l2 = 1;
        key_mask.l2_mask.vlan_itag_mask.pri = 1;
    }
    #endif
    if (rule->cvlan_not_exist)
    {
        key.l2.vlan_count = 0;
        key_mask.l2 = 1;
        key_mask.l2_mask.vlan_count = 1;
        OAM_ORG_LOG("\t cvlan not exist\r\n");
    }
    if ((rule->cvid_flag)&&(!rule->svid_flag))
    {
        if(rule->cvid_max_flag){
            key.l2.vlan_otag.vlan_max.vid = rule->cvid_max;
        }

        if(rule->cvid_min_flag){
            key.l2.vlan_otag.vlan_min.vid = rule->cvid_min;
        }

        if(!rule->cvid_max_flag&&!rule->cvid_min_flag){
            key.l2.vlan_otag.vlan_min.vid = rule->cvid;
            key.l2.vlan_otag.vlan_max.vid = rule->cvid;
        }

        key.l2.vlan_count = 1;
        key_mask.l2 = 1;
        key_mask.l2_mask.vlan_otag_mask.vid = 1;
        key_mask.l2_mask.vlan_otag = 1;
        vlan_config = 1;
        OAM_ORG_LOG("\t cvlan 0x%x\r\n", rule->cvid);
    }
    if (rule->ipv4_sa_flag)
    {
        key.ip.ip_sa.ip_addr.ipv4_addr = (rule->ipv4_sa)<<(rule->lsb_mask);
        if(rule->ipv4_sa_max_flag){
            key.ip.ip_sa_max.ip_addr.ipv4_addr = (rule->ipv4_sa_max)<<(rule->lsb_mask);
        }else{
            key.ip.ip_sa_max.ip_addr.ipv4_addr = key.ip.ip_sa.ip_addr.ipv4_addr;
        }
        key.ip.ip_sa.afi = CA_IPV4;
        key.ip.ip_sa.addr_len = 32;
        key.ip.ip_sa_max.afi = CA_IPV4;
        key.ip.ip_sa_max.addr_len = 32;
        for(i=0;i<(rule->lsb_mask);i++){
            key.ip.ip_sa_max.ip_addr.ipv4_addr |=(1<<i);
        }

        key_mask.ip = 1;
        key_mask.ip_mask.ip_sa = 1;
        if(rule->ipv4_sa_flag&&rule->ipv4_da_flag){
            key_mask.ip_mask.ip_sa_max = 0;
        }else{
            key_mask.ip_mask.ip_sa_max = 1;
        }

    }
    if (rule->ipv4_da_flag)
    {
        key.ip.ip_da.afi = CA_IPV4;
        key.ip.ip_da.ip_addr.ipv4_addr = (rule->ipv4_da)<<(rule->lsb_mask);
        key.ip.ip_da.addr_len = 32;
        if(rule->ipv4_da_max_flag){
            key.ip.ip_da_max.ip_addr.ipv4_addr = (rule->ipv4_da_max)<<(rule->lsb_mask);
        }else{
            key.ip.ip_da_max.ip_addr.ipv4_addr = key.ip.ip_da.ip_addr.ipv4_addr;
        }
        for(i=0;i<(rule->lsb_mask);i++){
            key.ip.ip_da_max.ip_addr.ipv4_addr |=(1<<i);
        }
        key.ip.ip_da_max.afi = CA_IPV4;
        key.ip.ip_da_max.addr_len = 32;
        key_mask.ip = 1;
        key_mask.ip_mask.ip_da = 1;
        if(rule->ipv4_sa_flag&&rule->ipv4_da_flag){
            key_mask.ip_mask.ip_da_max = 0;
        }else{
            key_mask.ip_mask.ip_da_max = 1;
        }
    }
    if (rule->tos_flag)
    {
        key.ip.dscp = ((rule->tos)<<(rule->lsb_mask))>>2;
        key_mask.ip = 1;
        key_mask.ip_mask.dscp = 1;
    }

    if(rule->ecn_flag){
        key.ip.ecn = ((rule->tos)<<(rule->lsb_mask))&0x3;
        key_mask.ip = 1;
        key_mask.ip_mask.ecn = 1;
    }

    if (rule->protocol_flag)
    {
        key.ip.ip_protocol = rule->protocol;
        key_mask.ip = 1;
        key_mask.ip_mask.ip_protocol = 1;
    }
    if (rule->ipv6_sa_flag)
    {
        key.ip.ip_sa.afi = CA_IPV6;
        key.ip.ip_sa_max.afi = CA_IPV6;
        memcpy(key.ip.ip_sa.ip_addr.ipv6_addr, rule->ipv6_sa, 4*sizeof(oam_uint32));
        if(rule->ipv6_sa_max_flag){
            memcpy(key.ip.ip_sa_max.ip_addr.ipv6_addr, rule->ipv6_sa_max, 4*sizeof(oam_uint32));
        }else{
            memcpy(key.ip.ip_sa_max.ip_addr.ipv6_addr, rule->ipv6_sa, 4*sizeof(oam_uint32));
        }
        key.ip.ip_sa_max.addr_len = 128;
        key.ip.ip_sa.addr_len = 128;
        key_mask.ip = 1;
        key_mask.ip_mask.ip_sa = 1;
        key_mask.ip_mask.ip_sa_max = 1;
    }
    if (rule->ipv6_da_flag)
    {
        key.ip.ip_da.afi = CA_IPV6;
        key.ip.ip_da_max.afi = CA_IPV6;
        memcpy(key.ip.ip_da.ip_addr.ipv6_addr, rule->ipv6_da, 4*sizeof(oam_uint32));
        if(rule->ipv6_da_max_flag){
            memcpy(key.ip.ip_da_max.ip_addr.ipv6_addr, rule->ipv6_da_max, 4*sizeof(oam_uint32));
        }else{
            memcpy(key.ip.ip_da_max.ip_addr.ipv6_addr, rule->ipv6_da, 4*sizeof(oam_uint32));
        }
        key.ip.ip_da.addr_len = 128;
        key.ip.ip_da_max.addr_len = 128;
        key_mask.ip = 1;
        key_mask.ip_mask.ip_da = 1;
        key_mask.ip_mask.ip_da_max = 1;
    }
    if (rule->flow_lable_flag)
    {
        key.ip.flow_label = rule->flow_lable;
        key_mask.ip = 1;
        key_mask.ip_mask.flow_label = 1;
    }
    if (rule->l4_sport_flag)
    {
        if(rule->l4_sport_max_flag){
            key.l4.src_port.max =  rule->l4_sport_max;
        }
        if(rule->l4_sport_min_flag){
            key.l4.src_port.min = rule->l4_sport_min;
            key.l4.src_port.max = 0xFFFF;
        }
        if((0 == rule->l4_sport_max_flag)&&(0 == rule->l4_sport_min_flag)){
            key.l4.src_port.min = key.l4.src_port.max = rule->l4_sport;
        }
        key_mask.l4 = 1;
        key_mask.l4_mask.src_port = 1;
    }
    if (rule->l4_dport_flag)
    {
        if(rule->l4_dport_max_flag){
            key.l4.dst_port.max = rule->l4_dport_max;
        }

        if(rule->l4_dport_min_flag){
            key.l4.dst_port.min = rule->l4_dport_min;
            key.l4.dst_port.max = 0xFFFF;
        }

        if((0 == rule->l4_dport_min_flag)&&(0 == rule->l4_dport_max_flag)){
            key.l4.dst_port.min = key.l4.dst_port.max = rule->l4_dport;
        }
        key_mask.l4 = 1;
        key_mask.l4_mask.dst_port = 1;
    }

    if(rule->ipv6_next_header_flag){
        key.ip.ip_protocol = rule->ipv6_next_header;
        key.ip.ip_version = 6;
        key_mask.ip = 1;
        key_mask.ip_mask.ip_protocol = 1;
        key_mask.ip_mask.ip_version = 1;
    }

    if(rule->flow_lable_flag){
        key.ip.flow_label = rule->flow_lable;
        key_mask.ip =1;
        key_mask.ip_mask.flow_label=1;
    }

    if(rule->result.result_action_flag){
         if (rule->result.result_action == RULE_RESULT_DISC)
        {
            action.forward = CA_CLASSIFIER_FORWARD_DENY;
            ca_classifier_rule_add(0,rule->precedence,&key,&key_mask,&action, &index);
            OAM_ORG_LOG("\t result DISCARD index %d \r\n", index);
            return index;
        }
        else if(rule->result.result_action == RULE_RESULT_FWD)
        {   OAM_ORG_LOG("result action Forward\n\r");
            action.forward = CA_CLASSIFIER_FORWARD_FE;
        }
    }

    if((!rule->result.result_action_flag)&&(!rule->result.result_queue_flag)){
        for(i=0;i<16;i++){
            if((rule->cvid_flag)
                &&(g_dpoe_rule[i].frm_prc.cvid == rule->cvid)){
                    if(g_dpoe_rule[i].frm_prc.result.result_action_flag){
                        if(g_dpoe_rule[i].frm_prc.result.result_action == RULE_RESULT_DISC){
                            action.forward = CA_CLASSIFIER_FORWARD_DENY;
                        }else if(g_dpoe_rule[i].frm_prc.result.result_action ==  RULE_RESULT_FWD ){
                            action.forward = CA_CLASSIFIER_FORWARD_FE;
                        }
                        OAM_ORG_LOG("\t copy action.forward=%d \r\n",action.forward);
                    }else{
                        OAM_ORG_LOG("result_action_flag is false,do not need to copy action.forward\n\r");
                    }
                    if(g_dpoe_rule[i].frm_prc.result.result_queue_flag){
                          action.forward = CA_CLASSIFIER_FORWARD_PORT;
                          if(2 == g_dpoe_rule[i].frm_prc.result.result_queue_type){
                                action.dest.port = pon_port;
                          }else{
                                action.dest.port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(g_dpoe_rule[i].frm_prc.result.result_queue_instance[0]);
                          }   
                          OAM_ORG_LOG("\t copy action.forward=%d ,copy action.dest.port =0x%x \r\n",action.forward,action.dest.port);  
                    }else {
                        OAM_ORG_LOG("result.result_queue_flag is false,do not need to copy result_queue_instance\n\r ");
                    }
                    
                    OAM_ORG_LOG("\t copy action from the front rule\r\n");
                    break;
            }
        }
    }

    if (rule->result.result_queue_flag)
    {
        action.forward = CA_CLASSIFIER_FORWARD_FE;
        action.options.priority = 7-rule->result.result_queue_number;
        action.options.masks.priority = 1;
        rule->result.result_action_flag = 0;

        if (2 == rule->result.result_queue_type)
        {   // LLID
            action.dest.port = pon_port;
            action.forward = CA_CLASSIFIER_FORWARD_PORT;
            action.options.masks.action_handle = 1;
            llid_index = rule->result.result_queue_instance[0];
            action.options.action_handle.flow_id = (llid_index << 11) + (7 << 8);
            OAM_ORG_LOG("\t flow id %d\r\n", action.options.action_handle.flow_id);
            if (rule->cvid_flag)
            {
                _add_vlan_member(rule->cvid, ca_port, 0);
                _add_vlan_member(rule->cvid, pon_port, 0);
                OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", ca_port);
                OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", pon_port);
            }
            if (rule->svid_flag)
            {
                _add_vlan_member(rule->svid, ca_port, 0);
                _add_vlan_member(rule->svid, pon_port, 0);
                OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", ca_port);
                OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", pon_port);
            }
        }
        else { // UNI
            uni_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(rule->result.result_queue_instance[0]);
            action.dest.port = uni_port;
            action.forward = CA_CLASSIFIER_FORWARD_PORT;
            action.options.priority = (7-rule->result.result_queue_number);
            action.options.masks.priority = 1;
            OAM_ORG_LOG("\t dest port %x\r\n", action.dest.port);
            if (rule->cvid_flag)
            {
                _add_vlan_member(rule->cvid, ca_port, 0);
                _add_vlan_member(rule->cvid, uni_port, 0);
                OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", ca_port);
                OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", uni_port);
            }
            if (rule->svid_flag)
            {
                _add_vlan_member(rule->svid, ca_port, 0);
                _add_vlan_member(rule->svid, uni_port, 0);
                OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", uni_port);
            }
        }

        OAM_ORG_LOG("\t result QUEUE\r\n");
    }
    if (rule->result.result_vlan_act == RULE_VLAN_OPR_PUSH)
    {
        if (rule->result.result_cvlan_flag)
        {
            action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_PUSH;
            action.options.outer_tpid = (rule->result.result_cvlan & 0xffff0000) >> 16;
            action.options.outer_vid = rule->result.result_cvlan & 0xffff;
            action.options.masks.outer_vlan_act = 1;
            action.options.masks.outer_tpid = 1;

            OAM_ORG_LOG("\t result add CVLAN 0x%x 0x%x\r\n", action.options.outer_tpid, action.options.outer_vid);
            _add_vlan_member(action.options.outer_vid, ca_port, 0);
            ca_port_pvid_vlan_set(0, ca_port, action.options.outer_vid);
            OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", ca_port);
            if (rule->result.result_queue_flag)
            {
                if (2 == rule->result.result_queue_type)
                {   // LLID
                    _add_vlan_member(action.options.outer_vid, pon_port, 0);
                    OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", pon_port);
                }
                else { // UNI
                    uni_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
                    _add_vlan_member(action.options.outer_vid, uni_port, 0);
                    OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", uni_port);
                }
            }
        }
        if (rule->result.result_svlan_flag)
        {
            action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_PUSH;
            action.options.outer_tpid = (rule->result.result_svlan & 0xffff0000) >> 16;
            action.options.outer_vid = rule->result.result_svlan & 0xffff;
            action.options.masks.outer_vlan_act = 1;
            action.options.masks.outer_tpid = 1;
            OAM_ORG_LOG("\t result add SVLAN 0x%x 0x%x\r\n", action.options.outer_tpid, action.options.outer_vid);
            _add_vlan_member(action.options.outer_vid, ca_port, 0);
            ca_port_pvid_vlan_set(0, ca_port, action.options.outer_vid);
            OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", ca_port);
            if (rule->result.result_queue_flag)
            {
                if (2 == rule->result.result_queue_type)
                {   // LLID
                    _add_vlan_member(action.options.outer_vid, pon_port, 0);
                    OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", pon_port);
                }
                else { // UNI
                    uni_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
                    _add_vlan_member(action.options.outer_vid, uni_port, 0);
                    OAM_ORG_LOG("\t add VLAN member 0x%x\r\n", uni_port);
                }
            }
        }
    }
    else if (rule->result.result_vlan_act == RULE_VLAN_OPR_POP)
    {
        action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_POP;
        action.options.masks.outer_vlan_act = 1;
        OAM_ORG_LOG("\t result pop VLAN\r\n");
        //uni_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        //_add_vlan_member(action.options.outer_vid, 0, uni_port);
        //OAM_ORG_LOG("\t add VLAN untagged port 0x%x\r\n", uni_port);
    }

    dpoe_get_src_mac_for_ubq(mac2,mac3);

    if(is_ubq){
        if((CA_PORT_ID(CA_PORT_TYPE_CPU, 0x12) == key.src_port))
        {
            key.src_port = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x11);
            memcpy(&key.l2.mac_sa.mac_max, mac2, 6);
            memcpy(&key.l2.mac_sa.mac_min, mac2, 6);
            key_mask.l2 = 1;
            key_mask.l2_mask.mac_sa = 1;
            OAM_ORG_LOG("\t orignal src port is 0x40012,so real src port is 0x%x, src mac 0x%x:%x:%x:%x:%x:%x \r\n",key.src_port, mac2[0], mac2[1], mac2[2],
                mac2[3], mac2[4], mac2[5]);
        }

        if((CA_PORT_ID(CA_PORT_TYPE_CPU, 0x13) == key.src_port))
        {
            key.src_port = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x11);
            memcpy(&key.l2.mac_sa.mac_max, mac3, 6);
            memcpy(&key.l2.mac_sa.mac_min, mac3, 6);
            key_mask.l2 = 1;
            key_mask.l2_mask.mac_sa = 1;
            OAM_ORG_LOG("\t orignal src port is 0x40013,so real src port is 0x%x,src mac 0x%x:%x:%x:%x:%x:%x \r\n",key.src_port, mac3[0], mac3[1], mac3[2],
                mac3[3], mac3[4], mac3[5]);
        }
    }

    if(rule->result.result_tos_flag){
        dpoe_oam_config_qos_adapt(ca_port);
        action.options.dscp = (rule->result.result_tos)>>2;
        action.options.masks.dscp = 1;
    }
    _set_vlan_default_tag(ca_port);

    rc = ca_classifier_rule_add(0,rule->precedence,&key,&key_mask,&action, &index);
    OAM_ORG_LOG("\t add cls index %d, result %d\r\n", index, rc);

#endif
    return index;
}


void __dpoe_mcgrp_delete_for_cls(ca_uint32 cls_index){
    ca_status_t ret = CA_E_OK;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t cls_action;
    ca_uint32 pri = 0;
    OAM_ORG_LOG("Start to delete mc group\n\r");
    ret = ca_classifier_rule_get(0,cls_index,&pri,&key,&key_mask,&cls_action);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("ca_classifier_rule_get failed ,ret= %d\n\r",ret);
        return;
    }

    if(cls_action.options.masks.mcg_id){
       ret = ca_l2_mcast_group_delete(0,cls_action.options.mcg_id);
       OAM_ORG_LOG("ca_l2_mcast_group_delete failed ,ret= %d\n\r",ret);
       return;
    }
    return;
}

void __dpoe_vlan_delete_for_cls(ca_uint8 is_uni,ca_port_id_t ca_port,ca_uint32 cls_index){
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t cls_action;
    ca_uint8_t member_cnt=0xFF,new_member_cnt=0xFF;
    ca_port_id_t member_port[8];
    ca_uint8_t untag_cnt=0;
    ca_port_id_t untag_port[8];
    oam_uint8 i=0,j=0,k=0;
    ca_status_t ret = CA_E_OK;
    ca_uint32 pri = 0;
    ca_uint32 vlan_id[5];

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&cls_action,0,sizeof(ca_classifier_action_t));
    memset(member_port,0,8*sizeof(ca_port_id_t));
    memset(untag_port,0,8*sizeof(ca_port_id_t));
    memset(vlan_id,4096,5*sizeof(ca_uint32));

    OAM_ORG_LOG("\t Start to iterate the cls list\n\r");
    ret = ca_classifier_rule_get(0,cls_index,&pri,&key,&key_mask,&cls_action);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("__dpoe_vlan_delete_for_cls:ca_classifier_rule_get failed ,ret= %d\n\r",ret);
        return;
    }

    if(key_mask.src_port && (ca_port==key.src_port)){
        if(CA_CLASSIFIER_VLAN_ACTION_PUSH == cls_action.options.outer_vlan_act
            ||CA_CLASSIFIER_VLAN_ACTION_POP == cls_action.options.outer_vlan_act
            ||CA_CLASSIFIER_VLAN_ACTION_SWAP == cls_action.options.outer_vlan_act
            ||key.l2.vlan_otag.vlan_min.vid
            ||key.l2.vlan_otag.vlan_max.vid
            ||key.l2.vlan_itag.vlan_min.vid
            ||key.l2.vlan_itag.vlan_max.vid
            ||cls_action.options.outer_vid){
            OAM_ORG_LOG("\t Found the cls included vlan information \n\r ");
            if(is_uni){
                if(key.l2.vlan_otag.vlan_max.vid){
                    vlan_id[j++] = key.l2.vlan_otag.vlan_max.vid;
                }

                if(cls_action.options.outer_vid){
                    vlan_id[j++] = cls_action.options.outer_vid;
                }
            }else{
                if(key.l2.vlan_otag.vlan_max.vid){
                    vlan_id[j++] = key.l2.vlan_otag.vlan_max.vid;
                }

                if(cls_action.options.outer_vid){
                    vlan_id[j++] = cls_action.options.outer_vid;
                }
            }
            for(k=0;k<j;k++){
                OAM_ORG_LOG("vlan_id[%d] =%d\n\r",k,vlan_id[k]);
                memset(member_port,0,8*sizeof(ca_port_id_t));
                memset(untag_port,0,8*sizeof(ca_port_id_t));
                member_cnt=0xFF;
                ret = ca_l2_vlan_port_get(0,vlan_id[k],&member_cnt,member_port,&untag_cnt,untag_port);
                if(CA_E_OK!=ret){
                    OAM_ORG_LOG("__dpoe_vlan_delete_for_cls:ca_l2_vlan_port_get() failed ,ret =%d\n\r",ret);
                    continue;
                }
                OAM_ORG_LOG("member cont is %d\n\r",member_cnt);
                for(i=0;i<member_cnt;i++){
                    OAM_ORG_LOG("member_port[%d]=0x%08x\n\r",i,member_port[i]);
                }
                new_member_cnt = member_cnt;
                for(i=0;i<member_cnt;i++){
                    if(member_port[i]==key.src_port
                        ||member_port[i]==cls_action.dest.port){
                        memset(&member_port[i],0,sizeof(ca_port_id_t));
                        new_member_cnt--;
                        OAM_ORG_LOG("Found the right member port ,so delete it in membership\n\r");
                    }
                }
                if(0 == new_member_cnt){
                    OAM_ORG_LOG("member ship cont is 0 ,so delete the vlan\n\r");
                    ret = ca_l2_vlan_delete(0,vlan_id);
                    if(CA_E_OK!=ret){
                        OAM_ORG_LOG("__dpoe_vlan_delete_for_cls:ca_l2_vlan_delete() failed ,ret =%d\n\r",ret);
                        return;
                    }
                }    
            }
        }
     }
}
void dpoe_oam_clr_rule_adapt( dpoe_oam_mgmt_obj_t mgmt_obj)
{
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    oam_uint8 i = 0;
    oam_uint8 uni_port = 0;
    ca_port_id_t ca_port;
    ca_status_t ret = CA_E_OK;

    if(mgmt_obj.obj.uni.hdr.leaf == OAM_DPOE_LEAF_OBJ_UNI ){
        uni_port = mgmt_obj.obj.uni.value;
        DPOE_OAM_UNI_PORT_ID_CHECK(uni_port);
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(uni_port);
        OAM_ORG_LOG("dpoe_oam_clr_rule_adapt:uni_port = %d -->0x%08x\n\r ",uni_port,ca_port);
        for (i = 0; i < 16; i++)
        {
            OAM_ORG_LOG("Start to found uni port cls\n\r");
            if(g_dpoe_cls_port_map[i].port_id == uni_port
                &&g_dpoe_cls_port_map[i].port_type == OAM_DPOE_LEAF_OBJ_UNI){
                OAM_ORG_LOG("Found uni port cls ,index= %d\n\r",g_dpoe_cls_port_map[i].cls_index);
                __dpoe_vlan_delete_for_cls(TRUE,ca_port,g_dpoe_cls_port_map[i].cls_index);
                __dpoe_mcgrp_delete_for_cls(g_dpoe_cls_port_map[i].cls_index);
                ca_classifier_rule_delete(0,g_dpoe_cls_port_map[i].cls_index);
                --g_dpoe_cls_port_map_counter;
                memset(&g_dpoe_cls_port_map[i],0,sizeof(dpoe_cls_port_map));
           }
        }
    }

    if(mgmt_obj.obj.pon.hdr.leaf == OAM_DPOE_LEAF_OBJ_PON){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        for (i = 0; i < 16; i++)
        {
            OAM_ORG_LOG("Start to found pon port cls\n\r");
            if(g_dpoe_cls_port_map[i].port_type == OAM_DPOE_LEAF_OBJ_PON){
                OAM_ORG_LOG("Found pon port cls ,index= %d\n\r",g_dpoe_cls_port_map[i].cls_index);
                __dpoe_vlan_delete_for_cls(FALSE,ca_port,g_dpoe_cls_port_map[i].cls_index);
                __dpoe_mcgrp_delete_for_cls(g_dpoe_cls_port_map[i].cls_index);
                ca_classifier_rule_delete(0,g_dpoe_cls_port_map[i].cls_index);
                --g_dpoe_cls_port_map_counter;
                memset(&g_dpoe_cls_port_map[i],0,sizeof(dpoe_cls_port_map));
            }
        }
    }

#endif
}

void dpoe_oam_del_rule_adapt(oam_uint32 cls_index){
#ifndef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
    ca_classifier_rule_delete(0,cls_index);
#endif
}

void dpoe_oam_queue_cfg_set_adapt(oam_uint8 type, oam_uint16 port, oam_uint8 count, oam_uint32 *size)
{
    ca_port_id_t ca_port;
    int i;
    oam_uint32 *ptr = size;

    if (type == OAM_DPOE_LEAF_OBJ_UNI){
        DPOE_OAM_UNI_PORT_ID_CHECK(port);
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    }
    else
    {
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
    }

    for (i = 0; i < count; i++)
    {
        ca_queue_length_set(0 ,ca_port, i, *ptr);
        ptr++;
    }

    _dpoe_save_queue_cfg(type, port, count, size);
}

void dpoe_oam_queue_cfg_get_adapt(oam_uint8 type, oam_uint16 port, oam_uint8 *count, oam_uint32 *size)
{
    _dpoe_get_queue_cfg(type, port, count, size);
}

oam_status dpoe_oam_uni_eth_stats_get_adapt(oam_uint16 port, dpoe_oam_eth_stats_t *stats)
{
    oam_status ret;
    oam_uni_stats64_t uni_stats;
    oam_port_id_t oam_port;

    oam_port = DPOE_OAM_UNI_PORT_ID_TO_OAM_PORT(port);

    OAM_ORG_LOG("oam_port %d \r\n", oam_port);
    ret = oam_onu_uni_stats_get(oam_port, &uni_stats);
    stats->rx_frames_ok = uni_stats.rxfrm_cnt;
    stats->tx_frames_ok = uni_stats.txfrm_cnt;

    stats->rx_bytes_ok = uni_stats.rxbyte_cnt;
    stats->tx_bytes_ok = uni_stats.txbyte_cnt;

    stats->rx_frames_green = uni_stats.rxfrmgreen_cnt;
    stats->tx_frames_green = uni_stats.txfrmgreen_cnt;

    stats->rx_uc_frames = uni_stats.rxucfrm_cnt;
    stats->rx_mc_frames = uni_stats.rxmcfrm_cnt;
    stats->rx_bc_frames = uni_stats.rxbcfrm_cnt;

    stats->tx_uc_frames = uni_stats.txucfrm_cnt;
    stats->tx_mc_frames = uni_stats.txmcfrm_cnt;
    stats->tx_bc_frames = uni_stats.txbcfrm_cnt;

    stats->rx_pause_frames = uni_stats.rxpausefrm_cnt;
    stats->tx_pause_frames = uni_stats.txpausefrm_cnt;

    stats->rx_frames_short = uni_stats.rxundersizefrm_cnt;
    stats->rx_frames_64 = uni_stats.rxstatsfrm64_cnt;
    stats->rx_frames_65_127 = uni_stats.rxstatsfrm65_127_cnt;
    stats->rx_frames_128_255 = uni_stats.rxstatsfrm128_255_cnt;
    stats->rx_frames_256_511 = uni_stats.rxstatsfrm256_511_cnt;
    stats->rx_frames_512_1023 = uni_stats.rxstatsfrm512_1023_cnt;
    stats->rx_frames_1024_1518 = uni_stats.rxstatsfrm1024_1518_cnt;
    stats->rx_frames_1519 = uni_stats.rxstatsfrm1519_Max_cnt;
    stats->rx_frames_oversize = uni_stats.rxoversizefrm_cnt;

    stats->tx_frames_64 = uni_stats.txstatsfrm64_cnt;
    stats->tx_frames_65_127 = uni_stats.txstatsfrm65_127_cnt;
    stats->tx_frames_128_255 = uni_stats.txstatsfrm128_255_cnt;
    stats->tx_frames_256_511 = uni_stats.txstatsfrm256_511_cnt;
    stats->tx_frames_512_1023 = uni_stats.txstatsfrm512_1023_cnt;
    stats->tx_frames_1024_1518 = uni_stats.txstatsfrm1024_1518_cnt;
    stats->tx_frames_1519 = uni_stats.txstatsfrm1519_Max_cnt;

    stats->rx_fcs_error = uni_stats.rxcrcerrfrm_cnt;
    stats->rx_error = uni_stats.rxerror_cnt;
    stats->tx_error = uni_stats.txerror_cnt;
    stats->overlimit_drop = uni_stats.rxoversizefrm_cnt;
}

oam_status dpoe_oam_pon_eth_stats_get_adapt(dpoe_oam_eth_stats_t *stats)
{
    oam_status ret;
    oam_pon_stats64_t pon_stats;

    OAM_ORG_LOG("\r\n");
    ret = oam_onu_pon_stats_get(&pon_stats);
    stats->rx_frames_ok = pon_stats.pon_frame_cnt;
    stats->tx_frames_ok = pon_stats.pon_txframe_cnt;

    stats->rx_bytes_ok = pon_stats.pon_byte_cnt;
    stats->tx_bytes_ok = pon_stats.pon_tx_byte_cnt;

    stats->rx_uc_frames = pon_stats.pon_ucframe_cnt;
    stats->rx_mc_frames = pon_stats.pon_mcframe_cnt;
    stats->rx_bc_frames = pon_stats.pon_bcframe_cnt;

    stats->tx_uc_frames = pon_stats.pon_txucframe_cnt;
    stats->tx_mc_frames = pon_stats.pon_txmcframe_cnt;
    stats->tx_bc_frames = pon_stats.pon_txbcframe_cnt;

    stats->rx_pause_frames = pon_stats.pon_pauseframe_cnt;
    stats->tx_pause_frames = pon_stats.pon_txpausecnt;

    stats->rx_frames_short = pon_stats.pon_undersizecnt;
    stats->rx_frames_64 = pon_stats.pon_rmon64_cnt;
    stats->rx_frames_65_127 = pon_stats.pon_rmon65_127_cnt;
    stats->rx_frames_128_255 = pon_stats.pon_rmon128_255_cnt;
    stats->rx_frames_256_511 = pon_stats.pon_rmon256_511_cnt;
    stats->rx_frames_512_1023 = pon_stats.pon_rmon512_1023_cnt;
    stats->rx_frames_1024_1518 = pon_stats.pon_rmon1024_1518_cnt;
    stats->rx_frames_1519 = pon_stats.pon_rx1519_maxcnt;
    stats->rx_frames_oversize = pon_stats.pon_oversize_cnt;

    stats->tx_frames_64 = pon_stats.pon_txrmon64_cnt;
    stats->tx_frames_65_127 = pon_stats.pon_txrmon65_127_cnt;
    stats->tx_frames_128_255 = pon_stats.pon_txrmon128_255_cnt;
    stats->tx_frames_256_511 = pon_stats.pon_txrmon256_511_cnt;
    stats->tx_frames_512_1023 = pon_stats.pon_txrmon512_1023_cnt;
    stats->tx_frames_1024_1518 = pon_stats.pon_txrmon1024_1518_cnt;
    stats->tx_frames_1519 = pon_stats.pon_tx1519_maxcnt;

    stats->rx_fcs_error = pon_stats.pon_rx_fcs_err;
    stats->rx_error = pon_stats.pon_crcerr_cnt + pon_stats.pon_crc8_err  + pon_stats.pon_rx_fcs_err +
        pon_stats.pon_oversize_cnt + pon_stats.pon_undersizecnt + pon_stats.pon_runt_cnt;
    stats->tx_error = pon_stats.pon_tx_crc_err + pon_stats.pon_txoversizecnt;
    stats->overlimit_drop = pon_stats.pon_oversize_cnt;

    return OAM_E_OK;
}


oam_status dpoe_oam_llid_eth_stats_get_adapt(oam_llid_t llid, dpoe_oam_eth_stats_t *stats)
{
    oam_status ret;
    oam_pon_stats64_t pon_stats;
    ca_epon_llid_stats_t  llid_stats;
    ca_port_id_t pon_port;

    pon_port = OAM_PON_PORT_ID_TO_CA_PORT();
    memset(&llid_stats,0,sizeof(ca_epon_llid_stats_t));

    ret = ca_epon_llid_stats_get(0,pon_port,llid,FALSE,&llid_stats);
    if(ret != OAM_E_OK){
        OAM_ERROR("ca_epon_llid_stats_get return failed, ret = %d \n",ret);
        return ret;
    }

    OAM_ORG_LOG("llid %d \r\n", llid);
    ret = oam_onu_pon_stats_get(&pon_stats);
    stats->rx_frames_ok =  llid_stats.rx_total_count;
    stats->tx_frames_ok =  llid_stats.tx_total_count;

    stats->rx_bytes_ok = llid_stats.rx_good_octets;
    stats->tx_bytes_ok = llid_stats.tx_good_octets;

    stats->rx_uc_frames = llid_stats.rx_uc_frames;
    stats->rx_mc_frames = llid_stats.rx_mc_frames;
    stats->rx_bc_frames = llid_stats.rx_bc_frames;

    stats->tx_uc_frames = llid_stats.tx_uc_frames;
    stats->tx_mc_frames = llid_stats.tx_mc_frames;
    stats->tx_bc_frames = llid_stats.tx_bc_frames;
    stats->rx_pause_frames = pon_stats.pon_pauseframe_cnt;
    stats->tx_pause_frames = pon_stats.pon_txpausecnt;

    stats->rx_frames_short =  pon_stats.pon_undersizecnt;
    stats->rx_frames_64 = llid_stats.rx_64_frames;
    stats->rx_frames_65_127 = llid_stats.rx_65_127_frames;
    stats->rx_frames_128_255 = llid_stats.rx_128_255_frames;
    stats->rx_frames_256_511 = llid_stats.rx_256_511_frames;
    stats->rx_frames_512_1023 = llid_stats.rx_512_1023_frames;
    stats->rx_frames_1024_1518 = llid_stats.rx_1024_1518_frames;
    stats->rx_frames_1519 = llid_stats.rx_1519_max_frames;
    stats->rx_frames_oversize = pon_stats.pon_oversize_cnt;

    stats->tx_frames_64 = llid_stats.tx_64_frames;
    stats->tx_frames_65_127 = llid_stats.tx_65_127_frames;
    stats->tx_frames_128_255 = llid_stats.tx_128_255_frames;
    stats->tx_frames_256_511 = llid_stats.tx_256_511_frames;
    stats->tx_frames_512_1023 = llid_stats.tx_512_1023_frames;
    stats->tx_frames_1024_1518 = llid_stats.tx_1024_1518_frames;
    stats->tx_frames_1519 = llid_stats.rx_1519_max_frames;

    stats->rx_fcs_error = pon_stats.pon_rx_fcs_err;
    stats->rx_error = pon_stats.pon_crcerr_cnt + pon_stats.pon_crc8_err  + pon_stats.pon_rx_fcs_err +
        pon_stats.pon_oversize_cnt + pon_stats.pon_undersizecnt + pon_stats.pon_runt_cnt;
    stats->tx_error = pon_stats.pon_tx_crc_err + pon_stats.pon_txoversizecnt;
    stats->overlimit_drop = pon_stats.pon_oversize_cnt;

    return OAM_E_OK;
}


oam_status dpoe_oam_queue_eth_stats_get_adapt(oam_uint16 port,ca_uint32_t queue,ca_queue_stats_t *queue_stats){
    ca_port_id_t ca_port;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ca_queue_stats_get(0,ca_port,queue,TRUE,queue_stats);
}


oam_status dpoe_oam_uni_eth_stats_reset_adapt(oam_uint16 port)
{
    oam_port_id_t oam_port;

    oam_port = DPOE_OAM_UNI_PORT_ID_TO_OAM_PORT(port);

    OAM_ORG_LOG("oam_port %d \r\n", oam_port);
    return oam_uni_stats_reset(oam_port);
}

oam_status dpoe_oam_pon_eth_stats_reset_adapt()
{
    oam_pon_stats64_t pon_stats;

    OAM_ORG_LOG("\r\n");
    return oam_pon_stats_reset(&pon_stats);
}

oam_status dpoe_oam_llid_eth_stats_reset_adapt(oam_llid_t llid)
{
    OAM_ORG_LOG("llid %d \r\n", llid);
    /* need change to per llid counter */
    return oam_pon_stats_reset();

}

oam_uint32 dpoe_oam_pon_mpcp_stats_get_adapt(dpoe_oam_mpcp_stats_t *stats)
{
    oam_status ret;
    oam_pon_stats64_t pon_stats;

    OAM_ORG_LOG("\r\n");
    ret = oam_onu_pon_stats_get(&pon_stats);

    stats->tx_mac_ctrl = 0;
    stats->tx_report = pon_stats.pon_mpcp_tx_rpt;
    stats->tx_reg_request = pon_stats.pon_mpcp_tx_req_reg;
    stats->tx_reg_ack = pon_stats.pon_mpcp_tx_ack_ack;

    stats->rx_mac_ctrl = 0;
    stats->rx_gate = pon_stats.pon_mpcp_uc_gat_rev;
    stats->rx_register  = pon_stats.pon_mpcp_bc_reg_reg_rev;


    return OAM_E_OK;
}


oam_uint32 dpoe_oam_llid_mpcp_stats_get_adapt(oam_llid_t llid, dpoe_oam_mpcp_stats_t *stats)
{
    oam_status ret;
    oam_pon_stats64_t pon_stats;

    OAM_ORG_LOG("llid %d \r\n", llid);
    ret = oam_onu_pon_stats_get(&pon_stats);
    stats->tx_mac_ctrl = 0;
    stats->tx_report = pon_stats.pon_mpcp_tx_rpt;
    stats->tx_reg_request = pon_stats.pon_mpcp_tx_req_reg;
    stats->tx_reg_ack = pon_stats.pon_mpcp_tx_ack_ack;

    stats->rx_mac_ctrl = 0;
    stats->rx_gate = pon_stats.pon_mpcp_uc_gat_rev;
    stats->rx_register  = pon_stats.pon_mpcp_bc_reg_reg_rev;

    return OAM_E_OK;
}

oam_uint32 dpoe_oam_media_type_get_adapt(oam_uint8 *media_type){

    *media_type = DPOE_OAM_BASE_T_MEDIA;
    return OAM_E_OK;
}

oam_uint32 dpoe_oam_epon_mode_get_adapt(oam_uint16 *epon_mode)
{
    SETBIT(*epon_mode,3);
    SETBIT(*epon_mode,10);

    return OAM_E_OK;
}

#define DPOE_OAM_IMG_FILE "/tmp/oam_saturn.img"

oam_int32 dpoe_oam_upg_img_open(void)
{
    int fd;

    fd =  open(DPOE_OAM_IMG_FILE, O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IXUSR);
    if(fd < 0)
    {
        OAM_ORG_LOG("open file %s failed", DPOE_OAM_IMG_FILE);
        return -1;
    }

    return fd;
}

oam_int32 dpoe_oam_upg_img_write(oam_int32 fd, void *data, oam_uint32 data_len)
{
    volatile oam_uint32 write_len = 0;
    oam_uint32          len       = 0;

    for(;;)
    {
        if((len = write(fd, data+write_len,  data_len-write_len)) <=0 )
        {
            //perror("write");
            //OAM_ORG_LOG("write len %d", len);
            break;
        }
        write_len += len;
    }

    return 0;
}

oam_int32 dpoe_oam_upg_img_close(oam_int32 fd)
{
    close(fd);

    return 0;
}

void dpoe_oam_upg_img_remove(void)
{
    oam_int32 ret = 0;

    if(access(DPOE_OAM_IMG_FILE, F_OK) != 0)
    {
        OAM_ORG_LOG("file %s is not exist", DPOE_OAM_IMG_FILE);
        return;
    }

    // need to call functions for clean the image in flash
    ret = remove(DPOE_OAM_IMG_FILE);
    if(ret < 0)
    {
        OAM_ORG_LOG("remove file err");
        perror("remove");
    }

    return;
}

void dpoe_oam_upg_img_upgrade(void)
{
    char upgrade_cmd[64]={0};
    system("echo 0 > /tmp/upgrade_status");
    snprintf(upgrade_cmd, 63, "ca.upgrade -f %s ", DPOE_OAM_IMG_FILE);
    system(upgrade_cmd);

    return 0;
}

oam_int32 dpoe_oam_upg_img_info_get(
    ca_device_id_t device_id,
    ca_uint8_t     img_idx,
    ca_img_info_t  *img_info)
{
    return ca_img_info_get(device_id, img_idx, img_info);
}


oam_int32 dpoe_oam_upg_img_active_set(
    ca_device_id_t device_id,
    ca_uint8_t img_idx,
    ca_uint8_t flag)
{
    return ca_img_active_set(device_id, img_idx, flag);
}

oam_int32 dpoe_oam_upg_img_commit_set(
    ca_device_id_t device_id,
    ca_uint8_t img_idx,
    ca_uint8_t flag)
{
    return ca_img_commit_set(device_id, img_idx, flag);
}

oam_int32 dpoe_oam_upg_img_executed_set(
    ca_device_id_t device_id,
    ca_uint8_t img_idx,
    ca_uint8_t flag)
{
    return ca_img_executed_set(device_id, img_idx, flag);
}

void dpoe_oam_software_bundle_get_adapt(ca_uint8  *value)
{

    return ca_fw_ver_info_get(0,value);
}

oam_uint32 dpoe_oam_uni_mac_learn_set_adapt(oam_uint16 port,oam_uint8 *buf){

    ca_l2_addr_entry_t entry;
    ca_port_id_t ca_port;
    memset(&entry,0,sizeof(ca_l2_addr_entry_t));
    ca_status_t ret =CA_E_OK;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    memcpy(entry.mac_addr,buf,6);
    entry.da_permit = 1;
    entry.sa_permit = 1;
    entry.static_flag = 1;
    entry.port_id = ca_port;
    ret = ca_l2_addr_add(0,&entry);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_uni_mac_learn_set_adapt:ca_l2_addr_add() failed\n\r");
        return ret;
    }
    return CA_E_OK;
}


oam_uint32 dpoe_oam_uni_mac_learn_get_adapt(oam_uint16 port, oam_uint16 *mac_number, oam_uint8 *buf)
{
    ca_l2_addr_entry_t *entry;
    ca_iterator_t return_entry;
    ca_status_t status;
    ca_uint8 *ptr = NULL;
    ca_int8 ret = 0;
    ca_port_id_t ca_port;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    memset(&return_entry, 0, sizeof(ca_iterator_t));
    *mac_number = 0;
    ptr = buf;
    return_entry.entry_data = (ca_l2_addr_entry_t *)malloc(sizeof(ca_l2_addr_entry_t));
    if (!return_entry.entry_data)
        return -1;
    return_entry.entry_count = 1;
    return_entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * return_entry.entry_count;
    status = ca_l2_addr_iterate(0, &return_entry);

    while (CA_E_OK == status)
    {
        if (return_entry.entry_count == 1)
        {
            entry = (ca_l2_addr_entry_t *)return_entry.entry_data;
            if (ca_port == entry->port_id)
            {
                ret  = memcmp(ptr, entry->mac_addr, sizeof(ca_mac_addr_t));
                if(ret == 0){
                    (*mac_number)++;
                }
            }
        }
        status = ca_l2_addr_iterate(0, &return_entry);
    }
    free(return_entry.entry_data);
    return 0;
}

oam_uint32 dpoe_oam_uni_num_get_adapt(oam_uint16 *num){
    *num = OAM_UNI_PORT_NUM;

    return 0;
}

oam_uint32 dpoe_std_oam_phytype_get_adapt(oam_uint8 type,oam_uint16 port,oam_uint8 *phy_type){

    ca_port_id_t ca_port;
    ca_eth_port_speed_t port_speed ;
    ca_status_t ret = CA_E_OK;

    if (type == OAM_DPOE_LEAF_OBJ_UNI){
        DPOE_OAM_UNI_PORT_ID_CHECK(port);
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    }
    else
    {
        //ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
    }

    if((ca_port==CA_PORT_ID(CA_PORT_TYPE_CPU,0x12))||(ca_port==CA_PORT_ID(CA_PORT_TYPE_CPU,0x13))){
        *phy_type = 0x28;
    }else{
        ret = ca_eth_port_speed_get(0,ca_port,&port_speed);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_speed_get failed \n\r");
            return ret;
        }

        switch(port_speed){
            case CA_PORT_SPEED_10M:
                if(ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x03)
                    ||ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x04)){
                        *phy_type = 0x07;
                    }else{
                        OAM_ORG_LOG("10Mbps no support for port3 and por4\n\r");
                        *phy_type = 0x02;
                    }
                break;
            case CA_PORT_SPEED_100M:
                if(ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x03)
                    ||ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x04)){
                    *phy_type = 0x17;
                 }else{
                    *phy_type = 0x18;
                 }
                break;
            case CA_PORT_SPEED_1G:
                if(ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x03)
                    ||ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x04)){
                      *phy_type = 0x28;
                  }else{
                       *phy_type = 0x24;
                  }
                break;
            case CA_PORT_SPEED_2_5G:
                 if(ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x05)
                    ||ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x06)){
                        *phy_type = 0x7D;
                    }else{
                        OAM_ORG_LOG("2.5Gbps no support for port3 and por4\n\r");
                        *phy_type = 0x02;
                    }
                break;
            case CA_PORT_SPEED_10G:
                 if(ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x05)
                    ||ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x06)){
                        *phy_type = 0x30;
                    }else{
                        OAM_ORG_LOG("10Gbps no support for port3 and por4\n\r");
                        *phy_type = 0x02;
                    }
                break;
            case CA_PORT_SPEED_5G:
                 if(ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x05)
                    ||ca_port==CA_PORT_ID(CA_PORT_TYPE_ETHERNET,0x06)){
                        *phy_type = 0x7E;
                    }else{
                        OAM_ORG_LOG("5Gbps no support for port3 and por4\n\r");
                        *phy_type = 0x02;
                    }
                break;
            case CA_PORT_SPEED_AUTO:
                *phy_type = 0x01;
                break;
            default :
                *phy_type = 0x31;
                break;
        }
   }
    return CA_E_OK;
}

oam_uint32 dpoe_std_oam_macctrl_get_adapt(oam_uint16 port){
    ca_port_id_t ca_port;
    ca_eth_port_ability_t port_ability;
    ca_status_t ret = CA_E_OK;

    memset(&port_ability,0,sizeof(ca_eth_port_ability_t));

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    if((ca_port == CA_PORT_ID(CA_PORT_TYPE_CPU,0x12))||(ca_port == CA_PORT_ID(CA_PORT_TYPE_CPU,0x13))){
        return TRUE;
    }else{
        ret = ca_eth_port_phy_advert_get(0,ca_port,&port_ability);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_phy_advert_get failed \n\r");
            return ret;
        }
        return (port_ability.flow_ctrl);
    }
}

oam_uint8 dpoe_esafe_value = 3;
oam_uint32 dpoe_oam_esafe_cfg_set_adapt(oam_uint8 value){
    dpoe_esafe_value = value;
}

oam_uint32 dpoe_oam_esafe_cfg_get_adapt(oam_uint8 *value){
    *value = dpoe_esafe_value;
}


void dpoe_oam_config_qos_adapt(ca_port_id_t port_id){
    ca_qos_config_t qos_cfg;
    memset(&qos_cfg,0,sizeof(ca_qos_config_t));
    OAM_ORG_LOG("\n\r set dscp_remap_mode to cls\n\r");
    ca_qos_config_get(0,port_id,&qos_cfg);
    qos_cfg.dscp_remap_mode = CA_DSCP_MAP_CLASSIFIER;
    ca_qos_config_set(0,port_id,&qos_cfg);
}

static oam_uint8  g_dpoe_cvlan_insert_flag = 0;
static oam_uint16 g_dpoe_cvlan_insert_tpid = 0x8100;
oam_uint32 dpoe_oam_cvlan_tpid_set_adapt(oam_uint16 port,oam_uint16 tpid,oam_uint8 insert_flag){
    ca_uint32_t inner_tpid[5];
    ca_uint32_t inner_num = 0;
    ca_uint32_t outer_tpid[5];
    ca_uint32_t outer_num = 0;
    ca_port_id_t ca_port;
    ca_vlan_action_t default_action;
    ca_vlan_action_t untag_action;
    ca_vlan_action_t single_tag_action;
    ca_vlan_action_t double_tag_action;
    oam_uint8 tpid_index=0;
    ca_status_t ret = CA_E_OK;

    memset(inner_tpid,0,sizeof(inner_tpid));
    memset(outer_tpid,0,sizeof(outer_tpid));
    memset(&untag_action,0,sizeof(ca_vlan_action_t));
    memset(&single_tag_action,0,sizeof(ca_vlan_action_t));
    memset(&double_tag_action,0,sizeof(ca_vlan_action_t));
    memset(&default_action,0,sizeof(ca_vlan_action_t));

    g_dpoe_cvlan_insert_flag = insert_flag;
    g_dpoe_cvlan_insert_tpid = tpid;

    if(insert_flag == 0 ||tpid == 0x8100)
        return CA_E_OK;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ret = ca_l2_vlan_tpid_get(0,inner_tpid,&inner_num,outer_tpid,&outer_num);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_cvlan_tpid_set_adapt:ca_l2_vlan_tpid_get failed\n\r");
        return ret;
    }
    tpid_index = outer_num;
    outer_tpid[tpid_index]= tpid;
    outer_num++;
    ca_l2_vlan_tpid_set(0,inner_tpid,inner_num,outer_tpid,outer_num);
    ca_l2_vlan_outer_tpid_add(0,ca_port,tpid_index);

    ca_l2_vlan_egress_default_action_get(0,ca_port,&default_action,&untag_action,&single_tag_action,&double_tag_action);
    default_action.new_outer_tpid_src = CA_L2_VLAN_TPID_LOCAL ;
    default_action.new_outer_tpid_index = tpid_index;
    ca_l2_vlan_egress_default_action_set(0,ca_port,&default_action,&untag_action,&single_tag_action,&double_tag_action);
}

oam_uint32 dpoe_oam_cvlan_tpid_get_adapt(oam_uint16 port,oam_uint16 *tpid,oam_uint8 *insert_flag){
    *tpid = g_dpoe_cvlan_insert_tpid;
    *insert_flag = g_dpoe_cvlan_insert_flag;
}


void dpoe_oam_auto_neg_admin_ctrl_get_adapt(oam_uint16 port ,oam_uint8 *ctrl_flag){
    ca_port_id_t ca_port;
    ca_boolean_t is_enable = 0;
    ca_status_t ret = CA_E_OK;

    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);

    ret = ca_eth_port_autoneg_get(0,ca_port,&is_enable);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_auto_neg_admin_ctrl_get_adapt:ca_eth_port_autoneg_get failed\n\r");
        *ctrl_flag = 0x01;
        return;
    }

    if(is_enable == 1){
        *ctrl_flag = 0x02;
    }else{
        *ctrl_flag = 0x01;
    }
    return;
}

void dpoe_oam_auto_neg_admin_ctrl_set_adapt(oam_uint16 port ,oam_uint8 ctrl_flag){
    ca_port_id_t ca_port;
    ca_boolean_t is_enable = 0;
    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_status_t ret = CA_E_OK;

    if(ctrl_flag == 0x02){
        is_enable = 1;
    }else if(ctrl_flag == 0x01){
        is_enable = 0;
    }
    ret = ca_eth_port_autoneg_set(0,ca_port,is_enable);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_auto_neg_admin_ctrl_set_adapt:ca_eth_port_autoneg_set failed\n\r");
        return;
    }
    return;
}

void dpoe_oam_pause_mac_ctl_frame_rev_get_adapt(oam_uint16 port, oam_uint64 *frame){
    ca_port_id_t ca_port;
    DPOE_OAM_UNI_PORT_ID_CHECK(port);
    ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
    ca_status_t ret = CA_E_OK;
    ca_eth_port_stats_t port_stats;

    memset(&port_stats,0,sizeof(ca_eth_port_stats_t));

    ret = ca_eth_port_stats_get(0,ca_port,0,&port_stats);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("dpoe_oam_pause_mac_ctl_frame_rev_get_adapt:ca_eth_port_stats_get failed\n\r");
        return;
    }
    *frame = port_stats.rx_pause_frames;
    return;
}

static oam_uint16 g_dpoe_rule_pmc_fwr_cfg = 0;
void dpoe_oam_rule_pmc_fwr_cfg_get_adapt(oam_uint16 *value){
    *value = g_dpoe_rule_pmc_fwr_cfg;
}

void dpoe_oam_rule_pmc_fwr_cfg_set_adapt(oam_uint16 value){

    g_dpoe_rule_pmc_fwr_cfg = value;
}

void dpoe_oam_rx_frame_green_get_adapt(oam_uint64 *frame){
    ca_policer_stats_t pol_status;
    memset(&pol_status,0,sizeof(ca_policer_t));
    ca_port_id_t ca_port;
    ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
    ca_status_t ret = CA_E_OK;
    
    ret = ca_port_policer_stats_get(0,ca_port,FALSE,&pol_status);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("ca_port_policer_stats_get failed ,%d\n\r",ret);
        return;
    }
    *frame = pol_status.green_packets;
}

void dpoe_oam_multicast_llid_get_adapt(oam_uint8          *num,oam_uint16 *mc_llid ){
    ca_uint32_t mc_llid_nums = 64;
    ca_mpcp_llid_t tmp_llid[64];
    ca_status_t ret = CA_E_OK;
    
    memset(tmp_llid,0,64*sizeof(ca_mpcp_llid_t));

    ret= ca_siepon_mc_llid_list(0, &mc_llid_nums,tmp_llid);
    if(ret != CA_E_OK){
        OAM_ORG_LOG("ca_siepon_mc_llid_list() failed,ret=%d\n\r",ret);
        return;
    }
    OAM_ORG_LOG("mc_llid_nums=%d\n\r",mc_llid_nums);
    *num = (oam_uint8)mc_llid_nums;
    memcpy(mc_llid,tmp_llid,mc_llid_nums*sizeof(oam_uint16));
    return;
}

void dpoe_oam_out_of_range_length_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.rx_oversize_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.rx_oversize_frames;
    }
}

void dpoe_oam_count_tx_frame_1519_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.tx_1519_max_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.tx_1519_maxframes;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.tx_1519_max_frames;
    }
}

void dpoe_oam_count_rx_frame_1519_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.rx_1519_max_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.rx_1519_maxframes;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.rx_1519_max_frames;
    }
}

void dpoe_oam_count_tx_frame_1518_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.tx_1024_1518_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.tx_1024_1518_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.tx_1024_1518_frames;
    }
}

void dpoe_oam_count_rx_frame_1518_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.rx_1024_1518_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.rx_1024_1518_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.rx_1024_1518_frames;
    }
}

void dpoe_oam_count_tx_frame_1023_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.tx_512_1023_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.tx_512_1023_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.tx_512_1023_frames;
    }
}

void dpoe_oam_count_rx_frame_1023_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.rx_512_1023_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.rx_512_1023_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.rx_512_1023_frames;
    }
}

void dpoe_oam_count_tx_frame_511_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.tx_256_511_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.tx_256_511_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.tx_256_511_frames;
    }
}

void dpoe_oam_count_rx_frame_511_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.rx_256_511_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.rx_256_511_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.rx_256_511_frames;
    }
}

void dpoe_oam_count_tx_frame_255_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.tx_128_255_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.tx_128_255_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.tx_128_255_frames;
    }
}

void dpoe_oam_count_rx_frame_255_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.rx_128_255_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.rx_128_255_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.rx_128_255_frames;
    }
}

void dpoe_oam_count_tx_frame_127_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.tx_65_127_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.tx_65_127_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.tx_65_127_frames;
    }
}

void dpoe_oam_count_rx_frame_127_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.rx_65_127_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.rx_65_127_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.rx_65_127_frames;
    }
}

void dpoe_oam_count_tx_frame_64_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.tx_64_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.tx_64_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.tx_64_frames;
    }
}

void dpoe_oam_count_rx_frame_64_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.rx_64_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.rx_64_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.rx_64_frames;
    }
}

void dpoe_oam_count_rx_frame_short_get_adapt(oam_uint8 type,oam_uint64 *frame,oam_uint8 port,oam_uint16 llid){
    ca_eth_port_stats_t eth_port_stats;
    ca_epon_port_stats_t epon_port_stats;
    ca_epon_llid_stats_t  epon_llid_stats;
    ca_port_id_t ca_port=0;
    ca_status_t ret = CA_E_OK;
    memset(&eth_port_stats,0,sizeof(ca_eth_port_stats_t));
    memset(&epon_port_stats,0,sizeof(ca_epon_port_stats_t));
    memset(&epon_llid_stats,0,sizeof(ca_epon_llid_stats_t));
    if( OAM_DPOE_LEAF_OBJ_UNI == type){
        ca_port = DPOE_OAM_UNI_PORT_ID_TO_CA_PORT(port);
        ret = ca_eth_port_stats_get(0,ca_port,0,&eth_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_eth_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = eth_port_stats.rx_runt_frames;
    }else if(OAM_DPOE_LEAF_OBJ_PON == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_port_stats_get(0,ca_port,0,&epon_port_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_port_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_port_stats.rx_runt_frames;
    }else if(OAM_DPOE_LEAF_OBJ_LLID == type){
        ca_port = DPOE_OAM_PON_PORT_ID_TO_CA_PORT();
        ret = ca_epon_llid_stats_get(0,ca_port,llid,FALSE,&epon_llid_stats);
        if(ret != CA_E_OK){
            OAM_ORG_LOG("ca_epon_llid_stats_get() failed,ret=%d\n\r",ret);
            return;
        }
        *frame = epon_llid_stats.rx_total_count-epon_llid_stats.rx_64_frames-
                    epon_llid_stats.rx_65_127_frames-epon_llid_stats.rx_128_255_frames-epon_llid_stats.rx_256_511_frames-
                    epon_llid_stats.rx_512_1023_frames-epon_llid_stats.rx_1024_1518_frames-epon_llid_stats.rx_1519_max_frames;
    }
}

