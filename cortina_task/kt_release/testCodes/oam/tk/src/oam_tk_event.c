
#ifdef HAVE_TK_OAM

#include "oam_cmn.h"
#include "oam_core.h"
#ifdef HAVE_LOOP_DETECT
#include "loop_detect.h"
#endif
#include "scfg.h"

#ifdef HAVE_KT_OAM

#include "oam_tk.h"
#include "oam_tk_event.h"

void
oam_build_tk_org_event(oam_uint8  alarm_code, oam_uint8 raised,
            oam_uint16 port, oam_uint8 context)
{
        oam_uint32 len = 0;
        oam_uint8 pdu[EPON_MAX_OAMPDU_EVENT_LEN];
        tk_oam_onu_loop_detect_event_t  *p_tk_event_oam = NULL;

        oam_if_t * intf = oam_intf_find_valid();

        if(!intf)
                return;

        oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_EVENT);
        p_tk_event_oam = (tk_oam_onu_loop_detect_event_t *)(pdu + sizeof(oam_pdu_hdr_t));
        p_tk_event_oam->seq_num = ++intf->seq_num_next_tx;
        p_tk_event_oam->tlv_type = 0xFE;
        p_tk_event_oam->len = sizeof(tk_oam_onu_loop_detect_event_t)-sizeof(p_tk_event_oam->seq_num);
        p_tk_event_oam->oui[0] = 0;
        p_tk_event_oam->oui[1] = 0xd;
        p_tk_event_oam->oui[2] = 0xb6;
        p_tk_event_oam->alarm_code = alarm_code;
        p_tk_event_oam->raised = raised;
        p_tk_event_oam->port = htons(port);
        p_tk_event_oam->link = 0;
        p_tk_event_oam->queue = 0;
        p_tk_event_oam->context = context;
        len = sizeof(oam_pdu_hdr_t) + sizeof(tk_oam_onu_loop_detect_event_t);

        intf->unique_event_tx++;

        oam_tk_send(OAM_PON_PORT_ID, intf->llid, pdu, len);
}


#ifdef HAVE_LOOP_DETECT
ca_status_t kt_oam_loop_detect_handle(ca_port_id_t port, loop_detect_event_t type)
{
    oam_uint8 curr_vendor[] = {'C','O','R','T','\0'};
    oam_uint8 tk_vendor[4] = {0x37, 0x21, 0, 1};
    oam_uint8 tk_vendor_1[4] = {0x37, 0x23, 0, 1};
    oam_uint8 tk_vendor_2[4] = {0x37, 0, 0, 1};
    oam_uint8 tk_vendor_3[4] = {0x11, 0x00, 0, 1};
    oam_uint8 tk_vendor_4[4] = {0x23, 0x00, 0, 1};
    
    oam_status result = OAM_E_OK;
    ca_device_config_tlv_t scfg_tlv;
    ca_status_t ret = CA_E_OK;

    oam_if_t * intf = oam_intf_find_valid();

    if(intf == NULL){
        return result;
    }
    
    memset(&scfg_tlv, 0, sizeof(scfg_tlv));
    scfg_tlv.type = CA_CFG_ID_PON_VENDOR_ID;
    scfg_tlv.len = CFG_ID_PON_VENDOR_ID_LEN;

    ret = ca_device_config_tlv_get(0, &scfg_tlv);
    if (CA_E_OK == ret) {
        memcpy(curr_vendor, scfg_tlv.value, CFG_ID_PON_VENDOR_ID_LEN);
    }

    OAM_ORG_DEBUG("vendor id:%s\r\n", curr_vendor);

    switch(type)
    {
        case LOOP_DETECTED_EVENT:
            if (!memcmp(curr_vendor, tk_vendor, 2) || !memcmp(curr_vendor, tk_vendor_1, 2) ||
                !memcmp(curr_vendor, tk_vendor_2, 2) || !memcmp(curr_vendor, tk_vendor_3, 2) ||
                !memcmp(curr_vendor, tk_vendor_4, 2))
            {
                oam_build_tk_org_event(0x21, 1, 1, 1);
            }
            break;

        case UNBLOCK_TIMER_EXPIRED_EVENT:
            if(!memcmp(curr_vendor, tk_vendor, 2) || !memcmp(curr_vendor, tk_vendor_1, 2) ||
                !memcmp(curr_vendor, tk_vendor_2, 2) || !memcmp(curr_vendor, tk_vendor_3, 2) ||
                !memcmp(curr_vendor, tk_vendor_4, 2))
            {
                oam_build_tk_org_event(0x21, 0, 1, 1);
            }
            break;


        case LOOPDETECT_TIMER_EXPIRED_EVENT:
            OAM_ORG_DEBUG("Do not handle\r\n");
            break;

        case LOOPDETECT_ENABLE_EVENT:
            OAM_ORG_DEBUG("Do not handle\r\n");
            break;

        default:
            OAM_ORG_LOG("Unknown\r\n");
            break;
    }

    return result;
}
#endif

#endif

#endif

