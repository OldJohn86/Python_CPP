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

Copyright (c) 2011 by Cortina Systems Incorporated
****************************************************************************/

#ifndef __OAM_TK_PDU_H__
#define __OAM_TK_PDU_H__

#ifdef HAVE_TK_OAM
#include "oam_std_pdu.h"

#define TK_OPCODE_EXT_INFO                  0x00000000
#define TK_OPCODE_GET                       0x00000001
#define TK_OPCODE_GET_RSP                   0x00000002
#define TK_OPCODE_SET                       0x00000003
#define TK_OPCODE_SET_RSP                   0x00000004
#define TK_OPCODE_MC_REQ                    0x00000005
#define TK_OPCODE_MC_REG                    0x00000006
#define TK_OPCODE_MC_REG_RSP                0x00000007

#define TK_OPCODE_FILE_READ_REQ             0x00000009
#define TK_OPCODE_FILE_WRITE_REQ            0x0000000A
#define TK_OPCODE_FILE_DATA                 0x0000000B
#define TK_OPCODE_FILE_ACK                  0x0000000C

#define TK_OAM_BRANCH_NAME                  0x06
#define TK_OAM_BRANCH_ATTRIB                0x07
#define TK_OAM_BRANCH_ACTION                0x09

#define TK_OAM_LEAF_LPORT_OBJ               0x0001
#define TK_OAM_LEAF_PORT_OBJ                0x0003
#define TK_OAM_LEAF_QUEUE_OBJ               0x0004
#define TK_OAM_LEAF_BRIDGE_OBJ              0x0082
#define TK_OAM_LEAF_BRIDGE_PORT_OBJ         0x0083

#define TK_VAR_RSP_CODE_OK                  0x80
#define TK_VAR_RSP_CODE_TOO_LONG            0x81
#define TK_VAR_RSP_CODE_UNDETERM            0xA0
#define TK_VAR_RSP_CODE_UNSUPPORTED         0xA1
#define TK_VAR_RSP_CODE_CORRUPTED           0xA2
#define TK_VAR_RSP_CODE_HW_FAILED           0xA3
#define TK_VAR_RSP_CODE_OVERFLOW            0xA4
#define TK_VAR_RSP_CODE_BAD_PARAM           0xA8
#define TK_VAR_RSP_CODE_NO_RES              0xA9
#define TK_VAR_RSP_CODE_TO_BE_CONT          0xAA

/******************************************************************************************/
/************ TK Info TLV, IOP with AllRadio OLT ******************************************/
/******************************************************************************************/

#define OAM_TK_TYPE                 0x03
#define OAM_TK_VERSION              0x00

#define OAM_TK_REPORT_MODE_TK       0x01
#define OAM_TK_REPORT_MODE_CTC      0x02
#define OAM_TK_REPORT_MODE_NTT      0x04
#define OAM_TK_REPORT_MODE_MANUAL   0x08

typedef struct {
    oam_uint8           type;
    oam_uint8           length;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           vendor_type;
    oam_uint8           version;
    oam_uint8           report_mode;
    //oam_uint8           prefer_report_mode;
} __attribute__((packed)) oam_tk_info_tlv_t;

/******************************************************************************************/
/************ TK ORG OAMPDU ******************************************/
/******************************************************************************************/
typedef struct {
    oam_uint8 branch;
    oam_uint16 leaf;
    oam_uint8 len;
    oam_uint8 data[1];
}__attribute__((packed)) tk_oam_tlv_t;

typedef struct {
    oam_pdu_hdr_t hdr;
    oam_uint8 oui[OAM_OUI_LEN];
    oam_uint8 opcode;
    oam_uint8 data[1];
} __attribute__((packed)) tk_oam_pdu_t;

typedef struct {
    oam_uint8   extBranch;
    oam_uint16  leaf;
    oam_uint8   len;
} __attribute__((packed)) tk_oam_var_cont_t;

/* KT variable descriptor */
typedef struct {
    oam_uint8   extBranch;
    oam_uint16  leaf;
} __attribute__((packed)) tk_oam_var_desc_t;

/* KT Set container */
typedef struct {
    oam_uint8   extBranch;
    oam_uint16  leaf;
    oam_uint8   len;
    oam_uint8   data[1];
} __attribute__((packed)) tk_oam_var_cont_val_t;

/* TK_OAM_LEAF_PORT_OBJ: PON, UNI port */
typedef struct {
    oam_var_cont_t hdr;
    oam_uint16    port;
} __attribute__((packed)) oam_tk_port_obj_t;

#ifdef HAVE_DPOE_OAM
typedef struct {
    oam_var_cont_t hdr;
    oam_uint8    port;      
} __attribute__((packed)) oam_tk_dpoe_port_obj_t; /* in DPOE project, port size is 1byte */
#endif
/* TK_OAM_LEAF_LPORT_OBJ */
typedef struct {
    oam_var_cont_t hdr;
    oam_uint16    port;
    oam_uint16    link;
} __attribute__((packed)) oam_tk_link_obj_t;

/* TK_OAM_LEAF_PORT_QUEUE_OBJ: KT port queue stats */
typedef struct {
    oam_var_cont_t hdr;
    oam_uint16    port;
    oam_uint16    reserved;
    oam_uint16    queue;
} __attribute__((packed)) oam_tk_queue_obj_t;

#endif
#endif
