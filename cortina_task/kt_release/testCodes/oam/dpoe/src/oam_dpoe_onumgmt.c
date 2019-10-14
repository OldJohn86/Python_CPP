#ifdef HAVE_DPOE_OAM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_dpoe.h"
#include "oam_dpoe_pdu.h"
#include "oam_dpoe_client.h"
#include "oam_dpoe_adapt.h"
#include "oam_mux_par.h"
#include "oam_client.h"
#include "oam_timer.h"
#include "iniparser.h"
#include "ca_version.h"
#include "scfg.h"

extern oam_llid_t g_curr_llid;

void dpoe_creat_onu_manu_model_ini_file(){
    FILE    *ini ;
    ini = fopen("/etc/onu_manu_modle.ini","w");
    fprintf(ini,
        "[onu]\n"
        "manu_info = CA8271-ODM-vendor;\n"
        "model = CA8271-SFU-EVB;\n");
    fclose(ini);
    return;
}

oam_uint32 dpoe_oam_onu_id_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_onu_id_t *data = (oam_dpoe_onu_id_t *)out;
    oam_status ret = OAM_E_OK;
    oam_uint32 pdu_len = 0;
    oam_uint8  macaddr[OAM_MACADDR_LEN];
    //oam_if_t *intf = oam_intf_find(g_curr_llid);

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    ret = oam_llid_mac_get_adapt(0, &macaddr[0]);
    if(ret == OAM_E_OK){
        memcpy(data->onu_id, macaddr, OAM_DPOE_ONU_ID_LEN);

    }else {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        data->hdr.width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);
    }
#if 0
    if (intf)
    {
        memcpy(data->onu_id, intf->macaddr, OAM_DPOE_ONU_ID_LEN);
    }
    else {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        data->hdr.width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);
    }
#endif
    data->hdr.width = sizeof(oam_dpoe_onu_id_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_onu_id_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_fw_info_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_fw_info_t *data = (oam_dpoe_fw_info_t *)out;
    fw_info_t info;
    oam_uint32 pdu_len = 0;
    OAM_ORG_LOG("dpoe_oam_fw_info_get\n");
    oam_uint16 bootver = 0;
    oam_uint8 tmp_bootver[8],get_bootver[4];
    ca_status_t ret = CA_E_OK;
    //dpoe_oam_fw_info_get_adapt(&info);

    memset(tmp_bootver,0,sizeof(tmp_bootver));
    memset(get_bootver,0,sizeof(get_bootver));
    ret= ca_loader_info_get(0,tmp_bootver);
    if(CA_E_OK != ret){
        bootver = 0;
    }else{
        strncpy(get_bootver,(tmp_bootver+2),4);
        bootver = atoi(get_bootver);
    }

    info.bootver = bootver;
    info.bootver = htons(info.bootver);
    info.bootcrc = 0x826a8558;
    info.bootcrc = htonl(info.bootcrc);
    info.fwver = 0x0327;
    info.fwver = htons(info.fwver);
    info.fwcrc = 0x83075517;
    info.fwcrc = htonl(info.fwcrc);
    memcpy(&data->info, &info, sizeof(fw_info_t));
    data->hdr.width = sizeof(fw_info_t);
    pdu_len = sizeof(oam_dpoe_fw_info_t);
    return pdu_len;
}

oam_uint32 dpoe_oam_chip_info_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_chip_info_t *data = (oam_dpoe_chip_info_t *)out;
    chip_info_t info;
    oam_uint32 pdu_len = 0;
    oam_boolean is_ubq = FALSE;
    ca_device_config_tlv_t cfg_model_name;
    ca_device_config_tlv_t cfg_hw_ver;
    ca_status_t ret=CA_E_OK;
    ca_uint8_t hw_model_name[16];
    ca_uint32_t chip_modle = 0;
    ca_uint32_t tmp_val= 0;

    memset(&cfg_model_name,0,sizeof(ca_device_config_tlv_t));
    memset(&cfg_hw_ver,0,sizeof(ca_device_config_tlv_t));
    memset(hw_model_name, 0, sizeof(hw_model_name));
    memset(&info,0,sizeof(chip_info_t));
    OAM_ORG_LOG("dpoe_oam_chip_info_get\n");
    //dpoe_oam_chip_info_get_adapt(&info);

    is_ubq = oam_dpoe_ubq_check();
    info.jedecid = 0x0879;
    info.jedecid = htons(info.jedecid);

    cfg_model_name.type = CA_CFG_ID_MODEL_NAME ;
    cfg_model_name.len = CFG_ID_MODEL_NAME_LEN;
    cfg_hw_ver.type = CA_CFG_ID_HW_VERSION;
    cfg_hw_ver.len = CFG_ID_HW_VERSION_LEN;
    ret = ca_device_config_tlv_get(0,&cfg_model_name);
    if(ret != CA_E_OK){
        info.chip_model = 0x8271;
    }else{
        strcpy(hw_model_name,cfg_model_name.value);
        OAM_ORG_LOG("cfg_model_name.value =%s\n\r",cfg_model_name.value);
        chip_modle = atoi(hw_model_name);
        tmp_val = chip_modle%10;
        info.chip_model|=tmp_val;
        tmp_val  =(chip_modle/10)%10;
        info.chip_model|=tmp_val<<4;
        tmp_val  =(chip_modle/100)%10;
        info.chip_model|=tmp_val<<8;
        tmp_val  =(chip_modle/1000)%10;
        info.chip_model|=tmp_val<<12;
        OAM_ORG_LOG("info.chip_model = 0x%08x\n\r",info.chip_model);
    }

    ret = ca_device_config_tlv_get(0,&cfg_hw_ver);
    if(ret != CA_E_OK){
        info.chip_ver = 0x10000;
        info.chip_ver = htonl(info.chip_ver);
    }else{
        memcpy(&(info.chip_ver),cfg_hw_ver.value,4);
    }

    if(is_ubq){
        info.chip_model = 0x8271;
    }
    info.chip_model = htonl(info.chip_model);
    memcpy(&data->info, &info, sizeof(chip_info_t));
    data->hdr.width =  sizeof(chip_info_t);
    pdu_len = sizeof(oam_dpoe_chip_info_t);
    return pdu_len;
}

oam_uint32 dpoe_oam_data_manuf_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_manuday_t *data = (oam_dpoe_manuday_t *)out;
    manuday_t info;
    oam_uint32 pdu_len = 0;

    OAM_ORG_LOG("dpoe_oam_data_manuf_get\n");
    //dpoe_oam_manuday_get_adapt(&info);
    info.year = 0x2016;
    info.year = htons(info.year);
    info.month = 1;
    info.day = 1;
    memcpy(&data->info, &info, sizeof(manuday_t));
    data->hdr.width = sizeof(manuday_t);
    pdu_len = sizeof(oam_dpoe_manuday_t);
    return pdu_len;
}

static char manu_info[64] = {
0x38, 0x32, 0x30, 0x30, 0x30, 0x42,
0x41, 0x32, 0x46, 0x54, 0x45, 0x37, 0x35, 0x30, 0x32, 0x30, 0x42, 0x43, 0x4d, 0x35, 0x35, 0x30,
0x33, 0x30, 0x53, 0x75, 0x6d, 0x69, 0x74, 0x6f, 0x6d, 0x6f, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
0x30, 0x30, 0x55, 0x41, 0x30, 0x41, 0x30, 0x30, 0x52, 0x32, 0x31, 0x31, 0x30, 0x30, 0x4c, 0x30,
0x31, 0x30, 0x31, 0x36, 0x30, 0x36, 0x32, 0x32, 0x62, 0x36
};
oam_uint32 dpoe_oam_manuf_info_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint32 pdu_len = 0;
    dictionary *ini;
    char default_manu_info[] = "CA8271-ODM-vendor";
    char tmp_manu_info[64];
    char *tmp_manu_info_get;

    memset(tmp_manu_info,0,64);

    OAM_ORG_LOG("dpoe_oam_manuf_info_get\n");
    ini = iniparser_load("/etc/onu_manu_modle.ini");

    if(NULL== ini){
        dpoe_creat_onu_manu_model_ini_file();
        OAM_ORG_LOG("dpoe_oam_manuf_info_get 1: default_manu_info is %s,length is %d\n\r",default_manu_info,strlen(default_manu_info));
        memcpy(tmp_manu_info,default_manu_info,strlen(default_manu_info));
    }else{
        iniparser_dump(ini,stderr);
        tmp_manu_info_get = iniparser_getstring(ini,"onu:manu_info","null");
        OAM_ORG_LOG("dpoe_oam_manuf_info_get 2:tmp_manu_info_get is %s, length is %d\n\r",tmp_manu_info_get,strlen(tmp_manu_info_get));
        memcpy(tmp_manu_info,tmp_manu_info_get,strlen(tmp_manu_info_get));
    }
    memcpy(((oam_uint8 *)data + 4), tmp_manu_info, strlen(tmp_manu_info));
    data->width = 64;
    pdu_len = sizeof(oam_var_cont_t) + 64;
    return pdu_len;
}

oam_uint32 dpoe_oam_vn_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    char vn[31];
    oam_uint32 pdu_len = 0;
    OAM_ORG_LOG("dpoe_oam_vendorname_get\n");
    memset(vn, 0, 31);
    strcpy(vn, "cortina_access");
    memcpy((ca_uint8_t *)data + sizeof(oam_var_cont_t), vn, 31);
    data->width = 31;
    pdu_len = sizeof(oam_var_cont_t) + 31;
    return pdu_len;
}

oam_uint32 dpoe_oam_max_llid_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_max_llid_t *data = (oam_dpoe_max_llid_t *)out;
    oam_status ret = OAM_E_OK;
    oam_uint16 bidir = 8, ds_only = 1;
    oam_uint32 pdu_len = 0;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
    OAM_ORG_LOG("dpoe_oam_max_llid_get\n");

    ret = dpoe_oam_max_llid_get_adapt(&bidir, &ds_only);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("get adapt return failed, ret = %d \n", ret);
        data->hdr.width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);
    }

    OAM_ORG_LOG("bidir %d, ds_only %d\r\n", bidir, ds_only);

    data->bidir   = htons(bidir);
    data->ds_only = htons(ds_only);

    data->hdr.width = sizeof(oam_dpoe_max_llid_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_max_llid_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_uni_num_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_port_nmb_t *data = (oam_dpoe_port_nmb_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 num = 0;

    OAM_ORG_LOG("dpoe_oam_uni_num_get\n");
    //num = 2;
    dpoe_oam_uni_num_get_adapt(&num);
    //num = 4;
    data->nmb = htons(num);
    data->hdr.width = sizeof(oam_dpoe_port_nmb_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_port_nmb_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_pon_num_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_port_nmb_t *data = (oam_dpoe_port_nmb_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint16 num = 0;

    OAM_ORG_LOG("dpoe_oam_pon_num_get\n");
    num = 1;
    data->nmb = htons(num);

    data->hdr.width = sizeof(oam_dpoe_port_nmb_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_port_nmb_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_model_name_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_model_name_t *data = (oam_dpoe_model_name_t *)out;
    oam_uint8 model_num[OAM_DPOE_MODEL_NAME_LEN] = {0x46,0x54,0x45,0x37,0x35,0x30,0x32,0x2d,0x55,0x41,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    oam_status ret = OAM_E_OK;
    oam_uint32 pdu_len = 0;
    char default_onu_model[] = "CA8271-SFU-EVB";
    dictionary *ini;
    char *tmp_model_num_get;
    char tmp_model_num[OAM_DPOE_MODEL_NAME_LEN];

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    memset(tmp_model_num,0,OAM_DPOE_MODEL_NAME_LEN);
    ini = iniparser_load("/etc/onu_manu_modle.ini");

    if(NULL== ini){
        dpoe_creat_onu_manu_model_ini_file();
        OAM_ORG_LOG("dpoe_oam_model_name_get 1:default_onu_model is %s,length is %d\n\r",default_onu_model,strlen(default_onu_model));
        memcpy(tmp_model_num,default_onu_model,strlen(default_onu_model));
    }else{
        iniparser_dump(ini,stderr);
        tmp_model_num_get = iniparser_getstring(ini,"onu:model","null");
        OAM_ORG_LOG("dpoe_oam_model_name_get 2:tmp_model_num_get is %s,length is %d\n\r",tmp_model_num_get,strlen(tmp_model_num_get));
        memcpy(tmp_model_num,tmp_model_num_get,strlen(tmp_model_num_get));
    }
    memcpy(data->model_name, tmp_model_num, strlen(tmp_model_num));

    //data->hdr.width = sizeof(oam_dpoe_model_name_t) - sizeof(oam_var_cont_t);
    data->hdr.width = 0x1f;
    pdu_len = sizeof(oam_dpoe_model_name_t);

    return pdu_len;
}


oam_uint32 dpoe_oam_hw_ver_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    char vn[31];
    oam_uint32 pdu_len = 0;
    ca_printf("dpoe_oam_hw_ver_get\n");
    memset(vn, 0, 31);
    strcpy(vn, "v1.0");
    memcpy((oam_uint8 *)data + sizeof(oam_var_cont_t), vn, 31);
    data->width = 31;
    pdu_len = sizeof(oam_var_cont_t) + 31;
    return pdu_len;
}

oam_uint32 dpoe_oam_pkt_buff_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_pkt_buf_t *data = (oam_dpoe_pkt_buf_t *)out;
    oam_uint32 pdu_len = 0;

    OAM_ORG_LOG("dpoe_oam_pkt_buff_get\n");
#if 0
    data->up_queue = 64;
    data->up_queue_link = 8;
    data->up_queue_increment = 64;
    data->ds_queue = 8;
    data->ds_queue_link = 8;
    data->ds_queue_increment = 64;
    data->total_buf = 64000;
    data->total_buf = htons(data->total_buf);
    data->up_buf = 32000;
    data->up_buf = htons(data->up_buf);
    data->ds_buf = 32000;
    data->ds_buf = htons(data->ds_buf);

#endif
	data->up_queue = 0x1f;
    data->up_queue_link = 0x1f;
    data->up_queue_increment = 0x04;
    data->ds_queue = 0x10;
    data->ds_queue_link = 0x10;
    data->ds_queue_increment = 0x04;
    data->total_buf = 2048;
    data->total_buf = htons(data->total_buf);
    data->up_buf = 2048;
    data->up_buf = htons(data->up_buf);
    data->ds_buf = 2048;
    data->ds_buf = htons(data->ds_buf);

    data->hdr.width = sizeof(oam_dpoe_pkt_buf_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_pkt_buf_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_report_thresh_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_report_thresh_t *data = (oam_dpoe_report_thresh_t *)out;
    oam_uint8 queueset_num = 0, value_per_set = 0;
    oam_uint16 report_thresh[OAM_DPOE_QUEUESET_NUM_MAX * OAM_DPOE_VALUE_PER_SET_MAX];
    oam_status ret = OAM_E_OK;
    oam_uint32 pdu_len = 0;
    oam_uint8 index = 0;
    oam_uint16 i = 0;

    OAM_ORG_LOG("dpoe_oam_report_thresh_get\n");

    memset(report_thresh,0,OAM_DPOE_QUEUESET_NUM_MAX * OAM_DPOE_VALUE_PER_SET_MAX*sizeof(oam_uint16));
    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
#if 0
    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    index = mgmt_obj.obj.llid.value;

    ret = dpoe_oam_report_thresh_get_adapt(index, &queueset_num, &value_per_set,
                                           report_thresh);

    if (ret == OAM_E_NOT_SUPPORT) {
        data->hdr.width = OAM_DPOE_RSP_UNSUPPORT;
        return sizeof(oam_var_cont_t);
    }
    else if (ret != OAM_E_OK) {
        data->hdr.width = OAM_DPOE_RSP_HW_FAILURE;
        return sizeof(oam_var_cont_t);
    }

    data->queueset_num = queueset_num;
    data->value_per_set = value_per_set;

    for (i = 0; i < queueset_num * value_per_set; i += value_per_set) {
        data->report_thresh[i] = htons(report_thresh[i]);
        OAM_ORG_LOG("Get report thresh[%d] = 0x%04x\n\r",i,data->report_thresh[i]);
    }

    data->hdr.width= value_per_set*queueset_num*sizeof(oam_uint16)+2;
    pdu_len = data->hdr.width + sizeof(oam_var_cont_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_report_thresh_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32    proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *out_data = (oam_var_cont_t *)out;
    oam_dpoe_report_thresh_t *in_data =
        (oam_dpoe_report_thresh_t *)recv;
    oam_uint8 queueset_num = 0, value_per_set = 0;
    oam_uint16 report_thresh[OAM_DPOE_QUEUESET_NUM_MAX * OAM_DPOE_VALUE_PER_SET_MAX];
    oam_status ret = OAM_E_OK;
    oam_uint8 index = 0;
    oam_uint16 i = 0;

    OAM_ORG_LOG("dpoe_oam_report_thresh_set\n");

    memset(report_thresh, 0,sizeof(oam_uint16) * OAM_DPOE_QUEUESET_NUM_MAX * OAM_DPOE_VALUE_PER_SET_MAX);

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
#if 0
    if (!mgmt_obj.valid) {
        out_data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        out_data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    index = mgmt_obj.obj.llid.value;
    queueset_num  = in_data->queueset_num;
    value_per_set = in_data->value_per_set;

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        out_data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif

    index = mgmt_obj.obj.llid.value;
    queueset_num  = in_data->queueset_num;
    value_per_set = in_data->value_per_set;

    OAM_ORG_LOG("LLID=%d, queueset_num = %d, value_per_set = %d\r\n",
                  index, queueset_num, value_per_set);

    if (queueset_num > OAM_DPOE_QUEUESET_NUM_MAX) {
        out_data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (value_per_set > OAM_DPOE_VALUE_PER_SET_MAX) {
        out_data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    memcpy(report_thresh, in_data->report_thresh,
           sizeof(oam_uint16) * queueset_num * value_per_set);

    for (i = 0; i < queueset_num * value_per_set; i++) {
        report_thresh[i] = ntohs(report_thresh[i]);
    }

    for (i = 0; i < queueset_num * value_per_set; i += value_per_set) {
        OAM_ORG_LOG("Set report thresh[%d] = 0x%x\n\r",i,report_thresh[i]);
    }

    ret = dpoe_oam_report_thresh_set_adapt(index, queueset_num, value_per_set,
                                           report_thresh);

    if (OAM_E_NOT_SUPPORT == ret) {
        OAM_ORG_DEBUG("dpoe_oam_report_thresh_set_adapt failed, ret = %d \n", ret);
        out_data->width = OAM_DPOE_RSP_BAD_PARAM;
    }
    else if (OAM_E_OK != ret) {
        OAM_ORG_DEBUG("dpoe_oam_report_thresh_set_adapt failed, ret = %d \n", ret);
        out_data->width = OAM_DPOE_RSP_HW_FAILURE;
    }
    else {
        out_data->width = OAM_DPOE_RSP_OK;
    }

    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_llid_fwd_state_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_uint8_t *data = (oam_dpoe_uint8_t *)out;
    oam_uint32 pdu_len = 0;

    OAM_ORG_LOG("dpoe_oam_llid_fwd_state_get\n");
    if (!mgmt_obj.valid) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.llid.hdr.leaf != OAM_DPOE_LEAF_OBJ_LLID) {
        data->hdr.width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    data->hdr.width = 1;
    data->value = dpoe_oam_llid_fwd_state_get_adapt(mgmt_obj.obj.llid.value);
    pdu_len = sizeof(oam_dpoe_uint8_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_frame_rate_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_frame_rate_t *data = (oam_dpoe_frame_rate_t *)out;
    oam_uint8 max_rate = 0, min_rate = 0;
    oam_uint32 pdu_len = 0;

    OAM_ORG_LOG("dpoe_oam_frame_rate_get\n");

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    max_rate = oam_packet_max_rate_get();
    min_rate = oam_keep_alive_rate_get();

    OAM_ORG_DEBUG("max_rate = %d, min_rate = %d\r\n",
                  max_rate, min_rate);

    data->max_rate = max_rate;
    data->min_rate = min_rate;

    data->hdr.width = sizeof(oam_dpoe_frame_rate_t) - sizeof(oam_var_cont_t);
    pdu_len = sizeof(oam_dpoe_frame_rate_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_frame_rate_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32    proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *out_data = (oam_var_cont_t *)out;
    oam_dpoe_frame_rate_t *in_data =
        (oam_dpoe_frame_rate_t *)recv;
    oam_uint8 max_rate = 0, min_rate = 0;
    oam_status ret = OAM_E_OK;

    OAM_ORG_LOG("dpoe_oam_frame_rate_set\n");

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);

    max_rate = in_data->max_rate;
    min_rate = in_data->min_rate;

    OAM_ORG_DEBUG("max_rate = %d, min_rate = %d\r\n",
                  max_rate, min_rate);

    if (max_rate > OAM_PACKET_MAX_RATE_MAX) {
        out_data->width = OAM_DPOE_RSP_BAD_PARAM;
        goto RETURN;
    }

    if (min_rate > OAM_KEEP_ALIVE_RATE_MAX) {
        out_data->width = OAM_DPOE_RSP_BAD_PARAM;
        goto RETURN;
    }

    ret = oam_packet_max_rate_set(max_rate);
    ret += oam_keep_alive_rate_set(min_rate);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG_MIN("oam_packet_max_rate_set failed, ret = %d \n", ret);
        out_data->width = OAM_DPOE_RSP_BAD_PARAM;
    }
    else {
        out_data->width = OAM_DPOE_RSP_OK;
    }

RETURN:
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_manuf_org_name_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 org_name[256];

    OAM_ORG_LOG("dpoe_oam_manuf_org_name_get\n");

    memset(org_name,0,256);
    dpoe_oam_manuf_org_name_get_adapt(org_name);
    memcpy(((oam_uint8 *)data + 4),org_name,strlen(org_name));

    data->width = strlen(org_name);
    pdu_len = sizeof(oam_var_cont_t) + strlen(org_name);
    return pdu_len;
}

oam_uint32 dpoe_oam_fw_mfg_time_vary_set(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32    proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}


oam_uint32 dpoe_oam_fw_mfg_time_vary_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_dpoe_fw_mfg_time_t *data = (oam_dpoe_fw_mfg_time_t *)out;
    fw_mfg_time_t time;
    oam_uint32 pdu_len = 0;

    OAM_ORG_LOG("dpoe_oam_fw_mfg_time_vary_get\n");
    dpoe_oam_fw_mfg_get_adapt(&time);
    memcpy(&data->time, &time, sizeof(fw_mfg_time_t));
    data->hdr.width = sizeof(fw_mfg_time_t);
    pdu_len = sizeof(oam_dpoe_fw_mfg_time_t);
    return pdu_len;
}

oam_uint32 dpoe_oam_port_type_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   *proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint8 *ptr = out;
    oam_uint32 pdu_len = 0;
    oam_uint8 i;
    oam_uint8 value[64];

    memset(value, 0, 64);
#if 0
    if (!mgmt_obj.valid) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }

    if (mgmt_obj.obj.onu.hdr.leaf != OAM_DPOE_LEAF_OBJ_ONU) {
        data->width = OAM_DPOE_RSP_BAD_PARAM;
        return sizeof(oam_var_cont_t);
    }
#endif
    dpoe_oam_port_type_get_adapt(&i, value);
    ptr += 4;
    memcpy(ptr, value, i);
    data->width = i;
    pdu_len = sizeof(oam_var_cont_t) + i;

    return pdu_len;
}

oam_uint32 dpoe_oam_reset_onu(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
    dpoe_oam_reset_onu_adapt();

    data->width = OAM_DPOE_RSP_OK;
    return sizeof(oam_var_cont_t);
}

oam_uint32 dpoe_oam_epon_mode_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_uint32 pdu_len = 0;
    oam_dpoe_uint16_t *data = (oam_dpoe_uint16_t *)out;
    oam_uint16 epon_mode = 0;

    OAM_ORG_LOG("Now Onu Receive epon mode get Oam\n");
    //dpoe_oam_epon_mode_get_adapt(&epon_mode);
    //10G up ;10 down
    epon_mode = 0x408;

    data->value = htons(epon_mode);
    data->hdr.width = sizeof(oam_uint16);

    pdu_len = sizeof(oam_dpoe_uint16_t);

    return pdu_len;
}

oam_uint32 dpoe_oam_software_bundle_get(
    dpoe_oam_mgmt_obj_t mgmt_obj,
    oam_uint8    *recv,
    oam_uint32   proc_recv_len,
    oam_uint8    *out)
{
    oam_var_cont_t *data = (oam_var_cont_t *)out;
    oam_uint32 pdu_len = 0;
    oam_uint8 value[64] ;

    OAM_DPOE_FUNCTION_ENTER(mgmt_obj);
    memset(value,0,64);

    dpoe_oam_software_bundle_get_adapt(value);
    memset((oam_uint8 *)data+sizeof(oam_var_cont_t),0,64);
    memcpy((oam_uint8 *)data+sizeof(oam_var_cont_t),value,64);

    data->width = 64;
    pdu_len = sizeof(oam_var_cont_t)+64;

    return pdu_len;
}

#endif

