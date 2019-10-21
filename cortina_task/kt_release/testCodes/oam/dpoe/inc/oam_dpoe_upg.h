#ifndef __OAM_DPOE_UPG_H__
#define __OAM_DPOE_UPG_H__

#include "oam_types.h"
#include "oam_std_pdu.h"

#define DPOE_OAM_UPG_DFLT_RECV_TIMEOUT (1*1000)
#define DPOE_OAM_UPG_DFLT_RETRY_LIMIT  3

#define DPOE_OAM_UPG_MAX_BLOCK_LEN     1400
#define DPOE_OAM_UPG_FILE_NAME_MAX_LEN 64

#define DPOE_OAM_UPG_FILE_TRANS_OPECODE_RESERVED 0x0
#define DPOE_OAM_UPG_FILE_TRANS_OPECODE_WR       0x01
#define DPOE_OAM_UPG_FILE_TRANS_OPECODE_FTD      0x02
#define DPOE_OAM_UPG_FILE_TRANS_OPECODE_FTA      0x03

#define DPOE_OAM_UPG_ENTER() OAM_ORG_LOG("enter\r\n")
#define DPOE_OAM_UPG_LEAVE() OAM_ORG_LOG("leave\r\n")

#define DPOE_OAM_UPG_IMG_CLOSE(img_ctrl) \
    do { \
        dpoe_oam_upg_img_close(img_ctrl->fd); \
        img_ctrl->fd = -1; \
    } while(0)

typedef oam_int32 (*dpoe_oam_img_upg_fsm_handler_t) (void *, void *, void *);

typedef enum{
    DPOE_OAM_FILE_TRANS_RESP_OK,
    DPOE_OAM_FILE_TRANS_RESP_UNDEFINED,
    DPOE_OAM_FILE_TRANS_RESP_NOT_FOUND,
    DPOE_OAM_FILE_TRANS_RESP_NO_ACCESS,
    DPOE_OAM_FILE_TRANS_RESP_FULL,
    DPOE_OAM_FILE_TRANS_RESP_ILLEGAL_OPERATION,
    DPOE_OAM_FILE_TRANS_RESP_UNKNOWN_ID,
    DPOE_OAM_FILE_TRANS_RESP_BAD_BLOCK,
    DPOE_OAM_FILE_TRANS_RESP_TIMEOUT,
    DPOE_OAM_FILE_TRANS_RESP_BUSY,
    DPOE_OAM_FILE_TRANS_RESP_INCOMPATIBLE_FILE,
    DPOE_OAM_FILE_TRANS_RESP_CORRUPTED_FILE,
}dpoe_oam_file_trans_resp_code_e;

typedef enum{
    DPOE_OAM_UPG_WAIT_WRITE_REQUEST_STATE,
    DPOE_OAM_UPG_WRITE_REQUEST_STATE,
    DPOE_OAM_UPG_ACK_ILLEGAL_STATE,
    DPOE_OAM_UPG_ACK_REQUEST_STATE,
    DPOE_OAM_UPG_WAIT_DATA_TRANSFER_STATE,
    DPOE_OAM_UPG_ILLEGAL_FWR_STATE,
    DPOE_OAM_UPG_ACK_DISCARD_STATE,
    DPOE_OAM_UPG_TIMEOUT_STATE,
    DPOE_OAM_UPG_CHECK_NEXT_BLOCK_STATE,
    DPOE_OAM_UPG_WRITE_BLOCK_STATE,
    DPOE_OAM_UPG_SEND_ACK_STATE,
    DPOE_OAM_UPG_VERIFY_IMAGE_STATE,
    DPOE_OAM_UPG_COMMIT_IMAGE_STATE,
    DPOE_OAM_UPG_ACK_BUSY_STATE,
    DPOE_OAM_UPG_CONFIRM_IMAGE_STATE,
    DPOE_OAM_UPG_MAX_STATE,
}dpoe_oam_upg_state_e;

typedef enum{
    DPOE_OAM_UPG_EOAMI_WR_EVENT,
    DPOE_OAM_UPG_EOAMI_FTD_EVENT,
    DPOE_OAM_UPG_EOAMI_FTA_END_EVENT,
    DPOE_OAM_UPG_EOAMI_FTA_ERROR_EVENT,
    DPOE_OAM_UPG_EOAMI_FTA_TIMEOUT_EVENT,
    DPOE_OAM_UPG_STORAGEDONE_EVENT,
    DPOE_OAM_UPG_COMMMIT_DONE_EVENT,
    DPOE_OAM_UPG_RETRY_TIMER_DONE_EVENT,
    DPOE_OAM_UPG_UTC_EVENT,
    DPOE_OAM_UPG_RESULTCODE_OK_EVENT,
    DPOE_OAM_UPG_MAX_EVENT,
}dpoe_oam_upg_event_e;

typedef struct{
    oam_int32 upg_state;
    oam_int32 pre_upg_state;
    oam_int32 event;	
}dpoe_oam_upg_fsm_ctx_t;

typedef struct{
    oam_int32  fd;
    oam_uint16 block_num;
    oam_uint16 block_size;
    oam_uint16 block_width;
    oam_uint16 next_block_num;
    oam_uint32 commit_len;
    oam_uint32 pre_commit_len;
    oam_uint8  retry_count;
    oam_uint32 timerId;
    oam_uint32 threadId;
    oam_uint8  oui[OAM_OUI_LEN];
    oam_uint8  block_data[DPOE_OAM_UPG_MAX_BLOCK_LEN];
    oam_uint8  file_name[DPOE_OAM_UPG_FILE_NAME_MAX_LEN];
}dpoe_oam_img_ctrl_ctx_t;

typedef struct{
    oam_int32 recv_timeout;
    oam_int32 retry_limit;
}dpoe_oam_upg_cfg_ctx_t;

typedef struct{
    oam_llid_t llid;    
}dpoe_oam_upg_trans_ctx_t;

typedef struct{
    dpoe_oam_upg_fsm_ctx_t         dpoe_oam_upg_fsm_ctx;
    dpoe_oam_img_ctrl_ctx_t        dpoe_oam_img_ctrl_ctx;
    dpoe_oam_upg_cfg_ctx_t         dpoe_oam_upg_cfg_ctx;
    dpoe_oam_upg_trans_ctx_t       dpoe_oam_upg_trans_ctx;
    dpoe_oam_img_upg_fsm_handler_t img_upg_fsm_handler[DPOE_OAM_UPG_MAX_STATE][DPOE_OAM_UPG_MAX_EVENT];
}dpoe_oam_upg_mgr_ctx_t;

typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           opcode;
    oam_uint8           fileTransOpcode;
    oam_uint16          blockNum;
    oam_uint8           respCode;
    oam_uint8           pad[1];
} __attribute__((packed)) oam_pdu_dpoe_file_trans_ack_t;

typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           opcode;
    oam_uint8           fileTransOpcode;
    oam_uint16          blockNum;
    oam_uint16          blockWidth;
    oam_uint8           data[1];
} __attribute__((packed)) oam_pdu_dpoe_file_trans_data_t;

typedef struct {
    oam_var_cont_t     hdr;
    oam_uint8          value[1];
} __attribute__((packed)) oam_pdu_varies_t;

typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           opcode;
    oam_uint8           fileTransOpcode;
    oam_uint8           data[1];
} __attribute__((packed)) oam_pdu_dpoe_file_trans_t;

oam_int32 dpoe_oam_upg_retry_timer_start(void);
oam_int32 dpoe_oam_upg_done_evt_handle(oam_llid_t llid);
oam_int32 dpoe_oam_upg_init(void);


#endif/*__OAM_DPOE_UPG_H__*/
