#ifndef __OAM_KT_EVENT_H__
#define __OAM_KT_EVENT_H__

#include "ca_types.h"
#include "oam_types.h"

/* KT private event */
#define KT_OAM_EVENT_EXT_OP_CODE        0x0F

#define KT_OAM_EVENT_TYPE_OPM           0x02   /* optical power alarm */
#define KT_OAM_EVENT_TYPE_VOLTAGE       0x03   /* over voltage */
#define KT_OAM_EVENT_TYPE_LAO           0x04   /* laser always on */
#define KT_OAM_EVENT_TYPE_DDM           0x05   /* DDM threshold */
#define KT_OAM_EVENT_TYPE_SWITCH_ERROR  0x07   /* rootcause why watchdog timer triggered */
#define KT_OAM_EVENT_TYPE_MAC_LIMIT3    0x08   /* mac number over max number+3 */

#define KT_OAM_EVENT_STATE_RAISE        0x01
#define KT_OAM_EVENT_STATE_RELEASE      0x02

#define KT_OAM_EVENT_SWITCH_ERROR_MODULE      0x01   /* watchdog timer triggered caused by L2 switch */

#define KT_OAM_EVENT_SWITCH_ERROR_CRC_ERROR       0x01
#define KT_OAM_EVENT_SWITCH_ERROR_SWITHH_HANGED   0x02
#define KT_OAM_EVENT_SWITCH_ERROR_LED_MODE_CHANGE 0x03

typedef struct {
    oam_uint8        oui[3];
    oam_uint8        exopcode;
    oam_uint8        type;
    oam_uint8        len;
    oam_uint8        info;
} __attribute__((packed)) kt_oam_onu_opm_event_t;

typedef struct {
    oam_uint8        oui[3];
    oam_uint8        exopcode;
    oam_uint8        type;
    oam_uint8        len;
    oam_uint8        id;
    oam_uint8        state;
} __attribute__((packed)) kt_oam_onu_ddm_event_t;

typedef struct {
    oam_uint8        oui[3];
    oam_uint8        exopcode;
    oam_uint8        type;
    oam_uint8        len;
    oam_uint8        info;
} __attribute__((packed)) kt_oam_onu_lao_event_t;

typedef struct {
    oam_uint8        oui[3];
    oam_uint8        exopcode;
    oam_uint8        type;
    oam_uint8        len;
    oam_uint8        info;
} __attribute__((packed)) kt_oam_event_t;

typedef struct {
    oam_uint8        oui[3];
    oam_uint8        exopcode;
    oam_uint8        type;
    oam_uint8        len;
    oam_uint8        module;
    oam_uint8        alarm_id;
} __attribute__((packed)) kt_oam_event_switch_error_t;

void oam_build_voltag_event_check();

void oam_build_opm_event(oam_uint8 status, oam_uint8 flag);
void oam_build_ddm_event(oam_uint8 id, oam_uint16 state);
void oam_build_opm_event(oam_uint8 status, oam_uint8 flag);
void oam_build_voltage_event(oam_uint32 status, oam_uint8 high_low_flag);
void oam_build_lao_event(oam_uint8 status);
void oam_build_mac_limit_3_event(
    ca_device_id_t      device_id,
    ca_uint32_t         event_id,
    ca_void_t           *event,
    ca_uint32_t         len,
    ca_void_t           *cookie);

void oam_build_switch_error_event(oam_uint8 module, oam_uint8 alarmId);

ca_uint32_t oam_switch_event_handler(
    ca_device_id_t      device_id,
    ca_uint32_t         event_id,
    ca_void_t           *event,
    ca_uint32_t         len,
    ca_void_t           *cookie);

#endif
