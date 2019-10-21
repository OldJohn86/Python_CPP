#ifndef __OAM_ADAPT_H__
#define __OAM_ADAPT_H__

#include "oam_types.h"
#include "oam_std_pdu.h"
#include "oam_client.h"
#include "ca_version.h"

#define OAM_DYING_GASP_NUM  1 /* Default value */

#ifdef HAVE_MPORTS
#define OAM_UNI_PORT_NUM_MAX     4
#else
#define OAM_UNI_PORT_NUM_MAX     1
#endif

#define OAM_PORT_QUEUE_NUM_MAX  8

typedef struct{
    oam_uint32 uni_num;
    oam_uint32 oam_uni_bmp;
    oam_uint32 ca_xge_bmp;
    oam_uint32 ca_ge_bmp;
    oam_uint16 user_port_id[OAM_UNI_PORT_NUM_MAX];
    ca_port_id_t  ca_port_id[OAM_UNI_PORT_NUM_MAX];
}oam_onu_port_info_t;

typedef struct{
    oam_uint32 fwd_frames;
    oam_uint32 drop_frames;
    oam_uint32 fwd_bytes;
    oam_uint32 drop_bytes;
}oam_queue_stats_t;

extern oam_onu_port_info_t oam_onu_port_info;

#define OAM_UNI_PORT_NUM    oam_onu_port_info.uni_num

/* UNI ports */
#define OAM_UNI_PORT_ID_CHECK(port) \
        if((port) < OAM_UNI_PORT_ID1 || (port) >= (OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM)) { \
            OAM_ERROR("port id invalid. %d?port num = %d\r\n",  port, OAM_UNI_PORT_NUM); \
            return OAM_E_ERROR; \
        }

/* PON + UNI ports */
#define OAM_PORT_ID_CHECK(port)\
        if((port) < OAM_PON_PORT_ID || (port) >= (OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM)) { \
            OAM_ERROR("port id invalid. %d\r\n",  (port)); \
            return OAM_E_ERROR; \
        }

#define OAM_QUEUE_ID_CHECK(queue)\
        if(queue >= OAM_PORT_QUEUE_NUM_MAX) { \
            OAM_ERROR("queue id invalid. %d\r\n",  (queue)); \
            return OAM_E_ERROR; \
        }

#define OAM_PON_PORT_ID_TO_CA_PORT()        oam_port_to_ca_port_adapt(OAM_PON_PORT_ID)
#define OAM_UNI_PORT_ID_TO_CA_PORT(port)    oam_port_to_ca_port_adapt(port)
#define OAM_PORT_ID_TO_CA_PORT(port)        oam_port_to_ca_port_adapt(port)
#define OAM_CA_PORT_TO_OAM_PORT(ca_port)    oam_ca_port_to_oam_port_adapt(ca_port)

#define DPOE_ETH3_BIT  0x8
#define DPOE_ETH2_BIT  0x4

ca_port_id_t oam_port_to_ca_port_adapt(oam_port_id_t port);
oam_port_id_t oam_ca_port_to_oam_port_adapt(ca_port_id_t port);

oam_boolean oam_d2_compatible_get_adapt();

oam_boolean oam_is_image_updating_adapt();

oam_uint8   oam_dying_gasp_num_get_adapt();

oam_status  oam_dying_gasp_send_adapt(oam_pdu_info_t *pdu, oam_uint32 len);

oam_uint16   oam_uni_port_num_get_adapt();

oam_status oam_uni_bmp_get_adapt ( oam_uint32* oam_uni_bmp );

oam_status oam_pkt_send_adapt(oam_port_id_t port, oam_llid_t llid,
                              oam_uint8 *buf, oam_uint32 len);

oam_status oam_llid_mac_get_adapt(oam_uint16 index, oam_uint8 *mac);

oam_status oam_llid_admin_set_adapt(oam_uint16 index, oam_boolean enable);

oam_status oam_port_lpbk_set_adapt(
    oam_llid_t llid,
    oam_uint8 mux_action,
    oam_uint8 par_action);

oam_status oam_buf_int_status_get_adapt(oam_boolean *status);

oam_status oam_pon_stats_get_adapt(oam_boolean read_clear, oam_pon_stats_t *stats);

oam_status oam_pon_stats_summary_get_adapt(oam_boolean read_clear, oam_uint64 *frames,
        oam_uint64 *errors, oam_uint64 *err_symb);

oam_status oam_uni_stats_get_adapt(oam_port_id_t port, oam_boolean read_clear, oam_uni_stats64_t *stats);

oam_status oam_pon_stats_clear_adapt();

oam_status oam_uni_stats_clear_adapt(oam_port_id_t port);

oam_status oam_queue_stats_get_adapt(
    oam_port_id_t port, 
    oam_uint8 queue, 
    oam_boolean read_clear, 
    oam_queue_stats_t *stats);

oam_status oam_queue_stats_clear_adapt(
    oam_port_id_t port, 
    oam_uint8 queue);

oam_status oam_evt_reg_adapt();

oam_status oam_pkt_trap_adapt();

oam_ext_mode_t oam_ext_mode_get_adapt();

oam_status oam_ca_xge_bmp_get_adapt ( oam_uint32* bmp );

oam_status oam_ca_ge_bmp_get_adapt ( oam_uint32* bmp );

oam_status oam_epon_mac_mode_get_adapt(ca_uint8_t* epon_mac_mod);


oam_status oam_pon_admin_set_adapt(oam_port_id_t port, oam_boolean enable);

oam_status oam_pon_led_set_adapt(oam_uint8 action);

oam_status oam_dying_gasp_init_adapt(oam_if_t *intf);

oam_status oam_traffic_set_adapt(oam_uint8 link_status);

void oam_pon_mac_reset_adapt();

oam_status oam_traffic_enable_adapt(oam_uint8 enable);
#ifdef HAVE_LOOP_DETECT
oam_status oam_enable_loop_detect_adapt(oam_boolean enable);
#endif
void oam_onu_port_info_dump(void);
void oam_system_reboot(void);
void oam_eth_enable(oam_uint8 eth ,oam_boolean enable);
oam_boolean oam_dpoe_ubq_check(void);
oam_status oam_port_control_set_adapt(oam_uint8 enable);
oam_status oam_onu_info_init_adapt();
oam_status oam_llid_enable_init_adapt();

ca_uint32_t oam_reg_status_event_handler
(
    ca_device_id_t         device_id,
    ca_uint32_t         event_id,
    ca_void_t         *event,
    ca_uint32_t         len,
    ca_void_t        *cookie
);

ca_uint32_t oam_port_link_event_handler
(
    ca_device_id_t         device_id,
    ca_uint32_t         event_id,
    ca_void_t         *event,
    ca_uint32_t         len,
    ca_void_t        *cookie
);


#define OAM_UPG_IMG_NAME      "/tmp/oam.img"
void oam_img_upgrade_adapt(void);

oam_int32 oam_img_verify_adapt(void);

oam_int32 oam_img_commit_adapt(void);

#endif

