#ifndef __OAM_MAIN_H__
#define __OAM_MAIN_H__

#include "oam_types.h"
#include "oam_std_pdu.h"

typedef struct {
    unsigned int    mutex;
    unsigned int    mq_id;

    oam_uint8       keep_alive_rate;
    oam_uint8       packet_max_rate;
} oam_ctrl_t;


#define OAM_MQ_NAME             "onu_oam_queue"
#define OAM_MQ_MSG_HDR_LEN      32
#define OAM_MQ_MSG_MAX_LEN      2000
#define OAM_MQ_MSG_MAX_NUM      100

#define OAM_MQ_MSG_LEN          (OAM_MQ_MSG_HDR_LEN + OAM_MQ_MSG_MAX_LEN)

#define OAM_MAX_LLID_NUM                    OAM_MAX_IF
#define OAM_MAX_UNI_PORT_NUM			    4

#define OAM_KEEP_ALIVE_RATE_DFT             10      /* 10 * 100ms */
#define OAM_KEEP_ALIVE_RATE_MAX             10      /* 10 * 100ms */
#define OAM_PACKET_MAX_RATE_DFT             1       /* 1 packet per 100ms */
#define OAM_PACKET_MAX_RATE_MAX             25      /* 25 packet per 100ms */

#define OAM_PACKET                          1
#define OAM_LLID_REG_EVT                    2
#define OAM_PORT_LINK_EVT                   3

#define OAM_KEEP_ALIVE_TIMER_EXPIRED        11
#define OAM_LINK_EVENT_TIMER_EXPIRED        12
#define OAM_CTC_STATS_TIMER_EXPIRED         13
#define OAM_KEEP_SILENCE_TIMER_EXPIRE       14
#define OAM_KEY_EXCH_TIMER_EXPIRED          15
#define OAM_KT_STATS_TIMER_EXPIRED          16
#define OAM_KT_IMG_UPG_EVT                  17
#define OAM_TK_UPG_DONE_EVT                 18 /* upgrade done event for "image download via TK OAM" function */

#define OAM_DPOE_OAM_UPG_END_EVT            20

#define EPON_ETHERTYPE_SLOW     0x8809

/* led action */
#define OAM_LED_ON      1
#define OAM_LED_OFF     2
#define OAM_LED_BLINK   3


typedef struct {
    oam_uint16      type;   /* message type */
    oam_dev_id_t    dev;    /* device id, for x86 debug with olt oam component */
    oam_uint8       pon;    /* pon port id, for x86 debug with olt oam  component */
    oam_port_id_t   onu;    /* onu id, for x86 debug with olt oam  component */
    oam_port_id_t   port;   /* port id */
    oam_llid_t      llid;   /* llid */
    oam_uint16      len;    /* data length */
    oam_uint16      resv1;  /* resolved */
    oam_uint16      resv2;  /* resolved */
} __attribute__((packed)) oam_msg_hdr_t;

typedef struct {
    oam_msg_hdr_t       hdr;
    oam_uint8           data[OAM_MQ_MSG_MAX_LEN];
} __attribute__((packed)) oam_msg_t;

typedef enum
{
    OAM_MPCP_DEREG_CAUSE_NO_DEREGISTER  = 0,
    OAM_MPCP_DEREG_CAUSE_TIMESTAMPDRIPT = 1,
    OAM_MPCP_DEREG_CAUSE_OLT_DEREGISTER = 2,
    OAM_MPCP_DEREG_CAUSE_ONU_DEREGISTER = 3,
    OAM_MPCP_DEREG_CAUSE_RX_REREGISTER  = 4,
    OAM_MPCP_DEREG_CAUSE_MPCP_TIMEOUT   = 5,
    OAM_MPCP_DEREG_CAUSE_GATE_TIMEOUT   = 6,
    OAM_MPCP_DEREG_CAUSE_SW_DEREGISTER  = 7
} __attribute__((packed)) oam_mpcp_dereg_cause_t;

typedef struct {
    oam_uint16          index;                  /* for multiple llid */
    oam_uint16          llid;                  /* llid */
    oam_uint8           mac[OAM_MACADDR_LEN];   /* mac addr */
    oam_uint8           type;                   /* 1G or 10G */
    oam_boolean			status;                 /* TRUE: register, FALSE: deregister */
    oam_uint32			dereg_cause;
} __attribute__((packed)) oam_onu_reg_evt_t;

typedef struct {
    oam_msg_hdr_t       hdr;
    oam_onu_reg_evt_t   reg_evt;
} __attribute__((packed)) oam_onu_reg_evt_msg_t;

typedef struct {
    oam_uint16          port;
    oam_boolean         status;                 /* TRUE: link up, FALSE: link down */
} __attribute__((packed)) oam_onu_port_link_evt_t;

typedef struct {
    oam_msg_hdr_t       hdr;
    oam_onu_port_link_evt_t   link_evt;
} __attribute__((packed)) oam_onu_port_link_evt_msg_t;

typedef struct {
    oam_msg_hdr_t       hdr;
    void                *data;
} __attribute__((packed)) oam_timer_msg_t;

typedef struct {
    oam_msg_hdr_t       hdr;
    oam_uint32          data;
}__attribute__((packed)) oam_img_upg_msg_t;

typedef void (* oam_vendor_reg_event_handler_t)(
    oam_llid_t llid,
    oam_uint8  index);

/*
 * send packet to massage queue
 */
oam_status oam_pkt_msg_send(oam_port_id_t port, oam_uint8 llid, oam_uint8 *pkt,
                            oam_uint32 len);
oam_status oam_timer_msg_send(oam_uint32 type, void *data);
oam_status oam_img_upg_msg_send(oam_uint32 type, oam_uint32 data);
oam_status oam_llid_reg_evt_msg_send(oam_onu_reg_evt_t *reg_evt);
oam_port_id_t oam_port_id_get();
oam_llid_t oam_llid_get();
oam_status oam_keep_alive_rate_set(oam_uint8 rate);
oam_uint8  oam_keep_alive_rate_get();
oam_status oam_packet_max_rate_set(oam_uint8 rate);
oam_uint8  oam_packet_max_rate_get();
oam_status oam_upg_done_evt_msg_send(oam_llid_t llid);

#ifdef HAVE_KT_OAM
oam_status oam_tk_upg_done_evt_send();
#endif

#endif

