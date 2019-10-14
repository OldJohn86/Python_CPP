
#include "oam_kt_event.h"
#include "oam_core.h"
#include "oam_client.h"

#include "opm_api.h"
#include "ca_event.h"
#include "oam_tk.h"

extern oam_uint8 g_opm_status;
extern oam_uint8 g_highvolt_status;

void oam_build_opm_event(oam_uint8 status, oam_uint8 flag)
{
    oam_uint32 len = 0;
    oam_uint8 pdu[EPON_MAX_OAMPDU_EVENT_LEN];
    kt_oam_onu_opm_event_t *p_opm_event_oam = NULL;

    oam_if_t *intf = oam_intf_find_valid();

    OAM_ORG_LOG_CRI("Optical power alarm detected %s\n",(status==1)?"raised":"released");
    if(!intf){
        return;
    }

    memset(pdu, 0, EPON_MAX_OAMPDU_EVENT_LEN);
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    p_opm_event_oam = (kt_oam_onu_opm_event_t *)(pdu + sizeof(oam_pdu_hdr_t));
    p_opm_event_oam->oui[0] = 0xaa;
    p_opm_event_oam->oui[1] = 0xaa;
    p_opm_event_oam->oui[2] = 0xaa;
    p_opm_event_oam->exopcode = KT_OAM_EVENT_EXT_OP_CODE;
    p_opm_event_oam->type = KT_OAM_EVENT_TYPE_OPM;
    p_opm_event_oam->len = 3;
    p_opm_event_oam->info = status;
    len = sizeof(oam_pdu_hdr_t) + sizeof(kt_oam_onu_opm_event_t);
    //g_opm_flag = status - 1;
    g_opm_status = status;
    intf->org_specific_tx++;

    kt_oam_send(OAM_PON_PORT_ID, intf->llid, pdu, len);

    cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_EMERGENCY,0, "광파워알람%s\n",(status==1)?"발생":"해제");;
}

void oam_build_ddm_event(oam_uint8 id, oam_uint16 state)
{
    oam_uint32 len = 0;
    oam_uint8 pdu[EPON_MAX_OAMPDU_EVENT_LEN];
    kt_oam_onu_ddm_event_t *p_opm_event_oam = NULL;

    oam_if_t *intf = oam_intf_find_valid();

    OAM_ORG_LOG_INFO("ddm id:%d, alarm detected %s\r\n", id, (state==0)?"Normal":((state==1)?"low" :"high"));
    if(!intf){
        return;
    }

    memset(pdu, 0, EPON_MAX_OAMPDU_EVENT_LEN);
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    p_opm_event_oam = (kt_oam_onu_opm_event_t *)(pdu + sizeof(oam_pdu_hdr_t));
    p_opm_event_oam->oui[0] = 0xaa;
    p_opm_event_oam->oui[1] = 0xaa;
    p_opm_event_oam->oui[2] = 0xaa;
    p_opm_event_oam->exopcode = KT_OAM_EVENT_EXT_OP_CODE;
    p_opm_event_oam->type = KT_OAM_EVENT_TYPE_DDM;
    p_opm_event_oam->len = 4;
    p_opm_event_oam->id = id;
    p_opm_event_oam->state = state;

    len = sizeof(oam_pdu_hdr_t) + sizeof(kt_oam_onu_ddm_event_t);
    //g_opm_flag = status - 1;
    if ((id - 1) == OPM_RX_PWR){
        if (OPM_ALARM_NORMAL == state)
            g_opm_status = 2;
        else
            g_opm_status = 1;
        OAM_ORG_LOG_CRI("opm status:%d\r\n", g_opm_status);
    }
    intf->org_specific_tx++;

    kt_oam_send(OAM_PON_PORT_ID, intf->llid, pdu, len);
    cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_EMERGENCY,0,
        "%sddm alarm%s\n",(1)?"과":"저",(state==1)?"발생":"해제");
}

void oam_build_voltage_event(oam_uint32 status, oam_uint8 high_low_flag)
{
    oam_uint32 len = 0;
    oam_uint8 pdu[EPON_MAX_OAMPDU_EVENT_LEN];
    kt_oam_event_t *p_vol_event_oam = NULL;
    oam_if_t *intf = oam_intf_find_valid();

    cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_EMERGENCY,0,
        "%s전압알람%s\n",(high_low_flag)?"과":"저",(status==1)?"발생":"해제");

    if(!intf){
        OAM_ORG_LOG_ERROR("OAM LLID interface not found\r\n");
        return;
    }

    memset(pdu, 0, EPON_MAX_OAMPDU_EVENT_LEN);
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    p_vol_event_oam = (kt_oam_event_t *)(pdu + sizeof(oam_pdu_hdr_t));
    p_vol_event_oam->oui[0] = 0xaa;
    p_vol_event_oam->oui[1] = 0xaa;
    p_vol_event_oam->oui[2] = 0xaa;
    p_vol_event_oam->exopcode = KT_OAM_EVENT_EXT_OP_CODE;
    p_vol_event_oam->type = KT_OAM_EVENT_TYPE_VOLTAGE;
    p_vol_event_oam->len = 3;
    p_vol_event_oam->info = status;
    len = sizeof(oam_pdu_hdr_t) + sizeof(kt_oam_event_t);

    intf->org_specific_tx++;

    /*Currently, Only Over voltage send the OAM.*/

    if(high_low_flag){
        kt_oam_send(OAM_PON_PORT_ID, intf->llid, pdu, len);
        g_highvolt_status = status;
    }


}

/* check weather need to send over voltage event when llid registered and oam discovery complete.
 *  1. adjust to over voltage, then reboot, gpio will not trigger over voltage event
 *  2. adjust over voltage or normal voltage during link un-registered, OAM can not be sent out
 *   */
void oam_build_voltag_event_check()
{
    oam_uint32 value = 0;
    oam_uint32 status = 0;

    oam_kt_adapt_over_voltage_status_get(&status);

    if(status != g_highvolt_status){
        oam_build_voltage_event(status, 1);
    }
    return;
}


void oam_build_lao_event(oam_uint8 status)
{
    oam_uint32 len = 0;
    oam_uint8 pdu[EPON_MAX_OAMPDU_EVENT_LEN];
    kt_oam_onu_lao_event_t *p_lao_event_oam = NULL;

    oam_if_t *intf = oam_intf_find_valid();

    OAM_ORG_LOG_CRI("Laser always on detected %s\n",(status==1)?"raised":"released");

    if(!intf){
            return;
        }

    memset(pdu, 0, EPON_MAX_OAMPDU_EVENT_LEN);
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    p_lao_event_oam = (kt_oam_onu_lao_event_t *)(pdu + sizeof(oam_pdu_hdr_t));
    p_lao_event_oam->oui[0] = 0xaa;
    p_lao_event_oam->oui[1] = 0xaa;
    p_lao_event_oam->oui[2] = 0xaa;
    p_lao_event_oam->exopcode = KT_OAM_EVENT_EXT_OP_CODE;
    p_lao_event_oam->type = KT_OAM_EVENT_TYPE_LAO;
    p_lao_event_oam->len = 3;
    p_lao_event_oam->info = status;
    len = sizeof(oam_pdu_hdr_t) + sizeof(kt_oam_onu_lao_event_t);

    intf->org_specific_tx++;

    kt_oam_send(OAM_PON_PORT_ID, intf->llid, pdu, len);

    cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_EMERGENCY,0,
        "역광장애%s\n",(status==1)?"발생":"해제");

}

void
oam_build_mac_limit_3_event(
	ca_device_id_t 		device_id,
	ca_uint32_t 		event_id,
	ca_void_t 			*event,
	ca_uint32_t 		len,
	ca_void_t           *cookie
)
{
    oam_uint32 length = 0;
    oam_uint8 pdu[EPON_MAX_OAMPDU_EVENT_LEN];
    kt_oam_event_t *event_oam_p = NULL;
    ca_uint32_t value = *(ca_uint32_t *)event;

    oam_if_t *intf = oam_intf_find_valid();

    OAM_ORG_LOG_INFO("mac limit + 3 detected.\r\n");

    if(!intf){
        return;
    }

    memset(pdu, 0, EPON_MAX_OAMPDU_EVENT_LEN);
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    event_oam_p = (kt_oam_onu_lao_event_t *)(pdu + sizeof(oam_pdu_hdr_t));
    event_oam_p->oui[0] = 0xaa;
    event_oam_p->oui[1] = 0xaa;
    event_oam_p->oui[2] = 0xaa;
    event_oam_p->exopcode = KT_OAM_EVENT_EXT_OP_CODE;
    event_oam_p->type = KT_OAM_EVENT_TYPE_MAC_LIMIT3;
    event_oam_p->len = 1;
    event_oam_p->info = value;
    length = sizeof(oam_pdu_hdr_t) + sizeof(kt_oam_event_t);

    intf->org_specific_tx++;

    kt_oam_send(OAM_PON_PORT_ID, intf->llid, pdu, length);
    //ca_printf("<%s, %d>OAM:: cnt:%d\r\n", __FUNCTION__, __LINE__, value);

}

/* report the root cause why WatchDog Timer will reset ONU.
   currently only support l2 switch related Event as below:
 - CRC Error
 - L2 Switch Hanged Error
 - LED Mode Change Error
*/
void oam_build_switch_error_event(oam_uint8 module, oam_uint8 alarmId)
{

    oam_uint32 len = 0;
    oam_uint8 pdu[EPON_MAX_OAMPDU_EVENT_LEN];
    kt_oam_event_switch_error_t *p_sw_event_oam = NULL;

    oam_if_t *intf = oam_intf_find_valid();

    OAM_ORG_LOG("watchdog timer out, caused by module %d, alarm id %d\r\n", module, alarmId);
    if(!intf){
        return;
    }

    memset(pdu, 0, EPON_MAX_OAMPDU_EVENT_LEN);
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    p_sw_event_oam = (kt_oam_event_switch_error_t *)(pdu + sizeof(oam_pdu_hdr_t));
    p_sw_event_oam->oui[0] = 0xaa;
    p_sw_event_oam->oui[1] = 0xaa;
    p_sw_event_oam->oui[2] = 0xaa;
    p_sw_event_oam->exopcode = KT_OAM_EVENT_EXT_OP_CODE;
    p_sw_event_oam->type = KT_OAM_EVENT_TYPE_SWITCH_ERROR;
    p_sw_event_oam->len = 2;
    p_sw_event_oam->module = module;
    p_sw_event_oam->alarm_id = alarmId;

    len = sizeof(oam_pdu_hdr_t) + sizeof(kt_oam_event_switch_error_t);

    intf->org_specific_tx++;

    kt_oam_send(OAM_PON_PORT_ID, intf->llid, pdu, len);

    if(alarmId == KT_OAM_EVENT_SWITCH_ERROR_CRC_ERROR)
    {
        cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_EMERGENCY,0,"스위치 포트 crc 오류\r\n");  /* switch port crc error */
    }
    else if(alarmId == KT_OAM_EVENT_SWITCH_ERROR_SWITHH_HANGED)
    {
        cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_EMERGENCY,0,"스위치 멈춤\r\n"); /* switch hang */
    }
    else if(alarmId == KT_OAM_EVENT_SWITCH_ERROR_LED_MODE_CHANGE)
    {
        cap_log_msg(0, CA_MOD_OAM_APP, CAP_LOG_EMERGENCY,0,"스위치 LED 모드가 변경됨 \r\n"); /* switch led mode changed */
    }
	return;
}

ca_uint32_t oam_switch_event_handler(
    ca_device_id_t      device_id,
    ca_uint32_t         event_id,
    ca_void_t           *event,
    ca_uint32_t         len,
    ca_void_t           *cookie)
{
    ca_event_onu_fault_t *p_event;

    if(event == NULL)
    {
        return CA_E_ERROR;
    }
    p_event = (ca_event_onu_fault_t *)event;

    OAM_ORG_LOG("receive event_id %d, module %d, alarm id %d \r\n", event_id, p_event->module, p_event->alarm_id);

    oam_build_switch_error_event(p_event->module, p_event->alarm_id);
    return 0;
}

