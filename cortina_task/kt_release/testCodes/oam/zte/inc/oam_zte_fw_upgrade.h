/****************************************************************************
            Software License for Customer Use of Cortina Software
                          Grant Terms and Conditions

IMPORTANT NOTICE - READ CAREFULLY: This Software License for Customer Use
of Cortina Software ("LICENSE") is the agreement which governs use of
software of Cortina Systems, Inc. and its subsidiaries ("CORTINA"),
including computer software (source code and object code) and associated
printed materials ("SOFTWARE").  The SOFTWARE is protected by copyright laws
and international copyright treaties, as well as other intellectual property
laws and treaties.  The SOFTWARE is not sold, and instead is only licensed
for use, strictly in accordance with this document.  Any hardware sold by
CORTINA is protected by various patents, and is sold but this LICENSE does
not cover that sale, since it may not necessarily be sold as a package with
the SOFTWARE.  This LICENSE sets forth the terms and conditions of the
SOFTWARE LICENSE only.  By downloading, installing, copying, or otherwise
using the SOFTWARE, you agree to be bound by the terms of this LICENSE.
If you do not agree to the terms of this LICENSE, then do not download the
SOFTWARE.

DEFINITIONS:  "DEVICE" means the Cortina Systems? Daytona SDK product.
"You" or "CUSTOMER" means the entity or individual that uses the SOFTWARE.
"SOFTWARE" means the Cortina Systems? SDK software.

GRANT OF LICENSE:  Subject to the restrictions below, CORTINA hereby grants
CUSTOMER a non-exclusive, non-assignable, non-transferable, royalty-free,
perpetual copyright license to (1) install and use the SOFTWARE for
reference only with the DEVICE; and (2) copy the SOFTWARE for your internal
use only for use with the DEVICE.

RESTRICTIONS:  The SOFTWARE must be used solely in conjunction with the
DEVICE and solely with Your own products that incorporate the DEVICE.  You
may not distribute the SOFTWARE to any third party.  You may not modify
the SOFTWARE or make derivatives of the SOFTWARE without assigning any and
all rights in such modifications and derivatives to CORTINA.  You shall not
through incorporation, modification or distribution of the SOFTWARE cause
it to become subject to any open source licenses.  You may not
reverse-assemble, reverse-compile, or otherwise reverse-engineer any
SOFTWARE provided in binary or machine readable form.  You may not
distribute the SOFTWARE to your customers without written permission
from CORTINA.

OWNERSHIP OF SOFTWARE AND COPYRIGHTS. All title and copyrights in and the
SOFTWARE and any accompanying printed materials, and copies of the SOFTWARE,
are owned by CORTINA. The SOFTWARE protected by the copyright laws of the
United States and other countries, and international treaty provisions.
You may not remove any copyright notices from the SOFTWARE.  Except as
otherwise expressly provided, CORTINA grants no express or implied right
under CORTINA patents, copyrights, trademarks, or other intellectual
property rights.

DISCLAIMER OF WARRANTIES. THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING ANY IMPLIED WARRANTIES
OF MERCHANTABILITY, NONINFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE,
TITLE, AND NON-INFRINGEMENT.  CORTINA does not warrant or assume
responsibility for the accuracy or completeness of any information, text,
graphics, links or other items contained within the SOFTWARE.  Without
limiting the foregoing, you are solely responsible for determining and
verifying that the SOFTWARE that you obtain and install is the appropriate
version for your purpose.

LIMITATION OF LIABILITY. IN NO EVENT SHALL CORTINA OR ITS SUPPLIERS BE
LIABLE FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, LOST
PROFITS, BUSINESS INTERRUPTION, OR LOST INFORMATION) OR ANY LOSS ARISING OUT
OF THE USE OF OR INABILITY TO USE OF OR INABILITY TO USE THE SOFTWARE, EVEN
IF CORTINA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
TERMINATION OF THIS LICENSE. This LICENSE will automatically terminate if
You fail to comply with any of the terms and conditions hereof. Upon
termination, You will immediately cease use of the SOFTWARE and destroy all
copies of the SOFTWARE or return all copies of the SOFTWARE in your control
to CORTINA.  IF you commence or participate in any legal proceeding against
CORTINA, then CORTINA may, in its sole discretion, suspend or terminate all
license grants and any other rights provided under this LICENSE during the
pendency of such legal proceedings.
APPLICABLE LAWS. Claims arising under this LICENSE shall be governed by the
laws of the State of California, excluding its principles of conflict of
laws.  The United Nations Convention on Contracts for the International Sale
of Goods is specifically disclaimed.  You shall not export the SOFTWARE
without first obtaining any required export license or other approval from
the applicable governmental entity, if required.  This is the entire
agreement and understanding between You and CORTINA relating to this subject
matter.
GOVERNMENT RESTRICTED RIGHTS. The SOFTWARE is provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to
restrictions as set forth in FAR52.227-14 and DFAR252.227-7013 et seq. or
its successor. Use of the SOFTWARE by the Government constitutes
acknowledgment of CORTINA's proprietary rights therein. Contractor or
Manufacturer is CORTINA.

Copyright (c) 2009 by Cortina Systems Incorporated
****************************************************************************/
#ifndef OAM_ZTE_FW_UPGRADE_H_
#define OAM_ZTE_FW_UPGRADE_H_

#ifdef HAVE_ZTE_BATCH_UPGRADE

#define POLICY_SERVER_IP_TYPE                     1
#define POLICY_SERVER_L4_PORT_TYPE           2
#define POLICY_SERVER_TIMEOUT_TYPE           3
#define MC_OR_BC_START_IP_TYPE                   4
#define MC_OR_BC_END_IP_TYPE                       5

#define MAX_SIGNATURE_LENGTH 64
#define SIGNATURE_STR2 "F401ZL7"    /* no more than 64 byte */
#define SIGNATURE_STR_LEN 7         /* relate to SIGNATURE_STR2 */
#define SIGNATURE_LEN  (SIGNATURE_STR_LEN+4)    /* the four is the length of the package */

#define VERSION_SERVER_IP_TYPE                     1
#define VERSION_SERVER_L4_PORT_TYPE           2
#define VERSION_SERVER_FILE_PATH_TYPE       3
#define VERSION_SERVER_FILE_LENGTH             4

#define ZTE_MAX_BUFFER                                    4000
#define ZTE_UDP_DST_PORT                                33333
#define ZTE_SIGNAL                                             "ZTENET"

#define ZTE_STATE                                               "SendVersion"
#define ZTE_DEVICE_TYPE                                   "F401"
#define ZTE_MODULE_NAME                                "firmware"

#define ZTE_UDP_SCAN_CMD                                0x0302
#define ZTE_UDP_START_CMD                              0x0301

#define ZTE_STRING_LEN                                      32


typedef enum {
    INIT_STATE    = 0,
    RECEIVED_SCAN_ONU_CMD,
    RECEIVED_ENABLE_UPGRADE_CMD
} ZTE_UPGRADE_FIRMWARE_STATUS;

/*save sever information in manage remote upgrade*/
typedef struct {
    oam_int32    policy_server_ip;
    oam_int32    policyserverPort;

    oam_uint8    updatestatus;
    oam_uint8    old_version_no[32];

}  zte_flash_upgrade_cfg_t;


typedef struct {
    oam_uint16  type;
    oam_uint16  length;
    oam_uint8    value[1];
} __attribute__((packed)) zte_server_tlv_t;

typedef struct {
    oam_uint8            valid;
    oam_int32            state;
    oam_uint16           age;
} zte_upgrade_state_record_t;



typedef struct {
    oam_uint8    check_signal[6];    /*it is 'ZTENET'*/
    oam_uint8    message_version;
    oam_uint8    return_value;
    oam_uint16  ext_flag;
    oam_uint16  command_id;
    oam_uint16  arguments;
    oam_uint16  data_length;
    oam_uint32  checksum;
    oam_uint32   reserved;
    oam_uint8    data[1];
} __attribute__((packed)) zte_udp_scan_or_start_cmd_packet_t;


typedef struct {
    oam_uint8
    status[ZTE_STRING_LEN];                                 /*it is 'SendVersion'*/
    oam_uint8    device_type[ZTE_STRING_LEN];                        /*ZXA10F400*/
    oam_uint8    product_mac[CS_MACADDR_LEN];
    oam_uint8    device_sn[ZTE_STRING_LEN];
    oam_uint8    device_id[ZTE_STRING_LEN];
    oam_uint32   device_ip;
    oam_uint8    hardware_version_no[ZTE_STRING_LEN];
    oam_uint8    old_version_no[ZTE_STRING_LEN];
    oam_uint8    new_version_no[ZTE_STRING_LEN];
} zte_onu_device_infor_t;



typedef struct {
    oam_uint32       state;
    oam_int32        policy_server_udp_socket;
    oam_int32        policy_server_tcp_socket;
    oam_int32        policy_server_ip;
    oam_int32        policy_server_l4_port;
    oam_int32        policy_server_timeout;
    oam_int32        mc_or_bc_start_ip;
    oam_int32        mc_or_bc_end_ip;


    oam_int32        version_server_tcp_socket;
    oam_int32        version_server_ip;
    oam_int32        version_server_l4_port;
    oam_int8         version_server_file_path[128];
    oam_uint32       version_server_file_len;


    oam_uint8        strart_receive_image_flag;
    oam_int8         received_blocks;
    oam_uint32       recevied_frame_count;
    oam_uint32       image_len;


    zte_onu_device_infor_t      onu;
    zte_flash_upgrade_cfg_t     flash_cfg;
    epon_timer_t                zte_monitor_state_timer;
    zte_upgrade_state_record_t  state_record;

} zte_upgrade_firmware_node_t;

#endif
#endif
