#ifndef __OAM_TK_UPG_H__
#define __OAM_TK_UPG_H__

/* ONU image download via TK private OAM
   Broadcom extensions enables ONUs to download new firmware upgardes from OLT using a simple file transfer protocol.
   This protocl is a simplified form of TFTP. It has been modified to operate over IEEE 802.1ah OAM channel instead of IP.*/
#ifdef HAVE_TK_OAM
#ifdef HAVE_KT_OAM
#include "oam_types.h"

/* software download File Type Code */
#define OAM_TK_UPG_FILE_TYPE_BOOT   0
#define OAM_TK_UPG_FILE_TYPE_APP    1
#define OAM_TK_UPG_FILE_TYPE_CFG    2
#define OAM_TK_UPG_FILE_TYPE_DIAG   3

/* software download File ACK response code */
#define OAM_TK_UPG_ACK_OK           0          
#define OAM_TK_UPG_ACK_UNDEF        1
#define OAM_TK_UPG_ACK_NOT_FOUND    2
#define OAM_TK_UPG_ACK_NOT_ACCESS   3
#define OAM_TK_UPG_ACK_FULL         4
#define OAM_TK_UPG_ACK_ILLEGAL_OP   5
#define OAM_TK_UPG_ACK_UNKNOWN_ID   6
#define OAM_TK_UPG_ACK_BAD_BLOCK    7
#define OAM_TK_UPG_ACK_TIMEOUT      8
#define OAM_TK_UPG_ACK_BUSY         9

#define OAM_TK_UPG_BLOCK_SIZE_MAX   512


/* File Read/Write Request */
typedef struct {
    oam_uint8 file_type;   
} __attribute__((packed)) oam_tk_upg_file_req_t;

/* File Data */
typedef struct {
    oam_uint16 block_number;  
    oam_uint16 block_size;  
    oam_uint8  data[1];
} __attribute__((packed)) oam_tk_upg_file_data_t;

/* File Ack */
typedef struct {
    oam_uint16 block_number;  /* The next block number ONU expect to receive */
    oam_uint8  resp_code;
} __attribute__((packed)) oam_tk_upg_file_ack_t;

oam_status oam_tk_upg_done_evt_handle();

void oam_tk_upg_download_abort();

void oam_tk_upg_build(
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 length);
oam_boolean oam_tk_upg_upgrading();

void oam_tk_upg_init();
void oam_tk_upg_dump();
#endif
#endif
#endif