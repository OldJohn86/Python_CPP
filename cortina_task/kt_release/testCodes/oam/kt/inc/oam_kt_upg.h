#ifndef __OAM_KT_UPG_H__
#define __OAM_KT_UPG_H__

#ifdef HAVE_KT_OAM
#include "oam_types.h"
#include "oam_kt_adapt.h"

#define MAX_IPADDR_STR_LEN       32
#define OAM_KT_UPG_DFLT_DEV_NAME "eth0"

#define OAM_KT_UPG_THREAD_CREATE(func, arg) \
    do { \
        pthread_t      pthread; \
        pthread_attr_t attr; \
        int            ret; \
        \
        pthread_attr_init(&attr); \
        ret = pthread_create(&pthread, &attr, func, arg); \
        if (ret != 0) \
        { \
            OAM_ORG_LOG_MAJ("thread create err\r\n"); \
            return -1; \
        } \
    }while(0)

#ifdef HAVE_TFTPD
#define TFTPD_START \
        tftpd_start
#define TFTPD_THREAD_START_CB_REGISTER(func) \
        tftpd_thread_start_cb_register(func)
#define TFTPD_THREAD_STOP_CB_REGISTER(func) \
        tftpd_thread_stop_cb_register(func)
#define TFTPD_RECV_MSG_ERR_CB_REGISTER(func) \
        tftpd_recv_msg_err_cb_register(func)
#define TFTPD_RECV_MSG_DONE_CB_REGISTER(func) \
        tftpd_recv_msg_done_cb_register(func)

#else
#define TFTPD_START NULL
#define TFTPD_THREAD_START_CB_REGISTER(func) 
#define TFTPD_THREAD_STOP_CB_REGISTER(func) 
#define TFTPD_RECV_MSG_ERR_CB_REGISTER(func) 
#define TFTPD_RECV_MSG_DONE_CB_REGISTER(func) 
#endif

typedef struct oam_kt_upg_tftp_addr_ctx{
    oam_uint32 ipAddr;
    oam_uint32 ipMask;
    oam_uint32 gw;
    oam_uint16 vlan;
    oam_uint8  priority;
}oam_kt_upg_tftp_addr_ctx_t;

typedef enum oam_kt_upg_tftp_state{
    OAM_KT_UPG_TFTP_IDLE_S,
    OAM_KT_UPG_TFTP_RECV_DONE_S,
    OAM_KT_UPG_TFTP_RECV_ERR_S,
    OAM_KT_UPG_TFTP_IMG_UPG_DONE_S,
    OAM_KT_UPG_TFTP_THREAD_START_S,
    OAM_KT_UPG_TFTP_THREAD_STOP_S,
    OAM_KT_UPG_TFTP_MAX_S,
}oam_kt_upg_tftp_state_e;

typedef enum oam_kt_upg_dl_state{
    OAM_KT_UPG_DL_DLING_S  = 1,
    OAM_KT_UPG_DL_WRITING_S,
    OAM_KT_UPG_CRC_ERR_S,
    OAM_KT_UPG_WR_ERR_S,
    OAM_KT_UPG_DL_ERR_S,
    OAM_KT_UPG_FINSHED_S,
}oam_kt_upg_dl_state_e;

typedef enum oam_kt_upg_auto_reboot{
    OAM_KT_UPG_AUTO_REBOOT = 1,
    OAM_KT_UPG_DL_NO_REBOOT,
}oam_kt_upg_auto_reboot_e;

typedef enum{
    OAM_KT_UPG_THREAD_STATE_STOP,
    OAM_KT_UPG_THREAD_STATE_RUNNING,
}oam_kt_upg_thread_state_e;

typedef struct oam_kt_dev_ctx{
    oam_uint8 dfltDev[KT_MAX_IFNAME_LEN];
    oam_uint8 vlanDev[KT_MAX_IFNAME_LEN];    
}oam_kt_dev_ctx_t;

typedef struct oam_kt_upg_mgr_ctx{
    oam_uint32                 upg_state;
    oam_kt_upg_tftp_addr_ctx_t tftp_addr;
    oam_uint8                  dl_state;
    oam_kt_upg_auto_reboot_e   auto_reboot;
    oam_uint32                 tftpd_thread_state;
    oam_kt_dev_ctx_t           dev_ctx;
}oam_kt_upg_mgr_ctx_t;

oam_uint32 oam_onu_upg_tftp_addr_set(
    oam_kt_upg_tftp_addr_ctx_t *addr_ctx);

oam_uint32 oam_onu_upg_tftp_auto_reboot_set(
        oam_kt_upg_auto_reboot_e auto_reboot);

oam_uint32 oam_onu_upg_tftp_dl_state_get(
    oam_uint8  *dl_state);

void oam_kt_upg_evt_handler(void *msg);
void tk_oam_onu_upg_init(void);

#endif/*HAVE_KT_OAM*/
#endif/*__OAM_KT_UPG_H__*/
