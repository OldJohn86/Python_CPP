#ifndef __OAM_TK_EVENT_H__
#define __OAM_TK_EVENT_H__

#ifdef HAVE_TK_OAM
#include "loop_detect.h"

#ifdef HAVE_KT_OAM
#define TK_ALARM_ID_LOOPBACK            0x20
#define TK_ALARM_ID_PORT_DISABLE        0x21

#define TK_ALARM_RAISE_ENABLE           1
#define TK_ALARM_RAISE_DISABLE          0
#define TK_ALARM_UNI_PORT_0             1
#define TK_ALARM_CONTEXT_ONU            0
#define TK_ALARM_CONTEXT_PORT           1
#define TK_ALARM_CONTEXT_LINK           2
#define TK_ALARM_CONTEXT_QUEUE          3

typedef struct {
    oam_uint16     seq_num;
    oam_uint8        tlv_type;
    oam_uint8        len;
    oam_uint8        oui[3];
    oam_uint8        alarm_code;
    oam_uint8        raised;
    oam_uint16      port;
    oam_uint16      link;
    oam_uint16      queue;
    oam_uint8        context;
} __attribute__((packed)) tk_oam_onu_loop_detect_event_t;


void
oam_build_tk_org_event(oam_uint8  alarm_code, oam_uint8 raised,
            oam_uint16 port, oam_uint8 context);

#ifdef HAVE_LOOP_DETECT
ca_status_t kt_oam_loop_detect_handle(ca_port_id_t port, loop_detect_event_t type);
#endif

#endif
#endif
#endif
