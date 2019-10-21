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
#ifndef __APP_OAM_SIEPON_H__
#define __APP_OAM_SIEPON_H__

#ifdef HAVE_SIEPON_OAM

#include "oam_types.h"

/* SIEPON SNMP optimized eOAM Data type definitions */
/*END of TLV 0x0002*/
#define OAM_PDU_SIEPON_SNMP_END_OF_TLV_DT           0x00
#define OAM_PDU_SIEPON_SNMP_END_OF_TLV_DL           0x02

#define OAM_PDU_SIEPON_SNMP_GET_TLV_LEN             0x02
#define OAM_PDU_SIEPON_SNMP_ONU_MIB                 0x04
#define OAM_PDU_SIEPON_SNMP_SLEEP_MOD_CAP_R         0x20
#define OAM_PDU_SIEPON_SNMP_SLEEP_MOD_CAP_R_LEN     0x3
#define OAM_PDU_SIEPON_SNMP_EARLY_WAKEUP_CAP_R      0x21
#define OAM_PDU_SIEPON_SNMP_EARLY_WAKEUP_CAP_R_LEN  0x3
#define OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_R         0x22
#define OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_R_LEN     0x3
#define OAM_PDU_SIEPON_SNMP_ACKCAPABLE_R            0x23
#define OAM_PDU_SIEPON_SNMP_ACKCAPABLE_R_LEN        0x3
#define OAM_PDU_SIEPON_SNMP_POWERONTIME_R           0x24
#define OAM_PDU_SIEPON_SNMP_POWERONTIME_R_LEN       0x6
#define OAM_PDU_SIEPON_SNMP_MAX_SLEEP_PERIOD_R      0x25
#define OAM_PDU_SIEPON_SNMP_MAX_SLEEP_PERIOD_R_LEN  0x6
#define OAM_PDU_SIEPON_SNMP_MIN_AWARE_PERIOD_R      0x26
#define OAM_PDU_SIEPON_SNMP_MIN_AWARE_PERIOD_R_LEN  0x4


#define OAM_PDU_SIEPON_SNMP_EARLY_WAKEUP_DATA_LEN   0x3
#define OAM_PDU_SIEPON_SNMP_EARLY_WAKEUUP_EN_RW     0x27
#define OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_DATA_LEN  0x3
#define OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_EN_RW     0x28
#define OAM_PDU_SIEPON_SNMP_ACK_ENABLE_RW           0x29
#define OAM_PDU_SIEPON_SNMP_ACK_ENABLE_DATA_LEN     0x3
#define OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_RW       0x2A
#define OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_DATA_LEN 0x6
#define OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_RW       0x2B
#define OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_DATA_LEN 0x4
#define OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_RW        0x2C
#define OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_DATA_LEN  0x4

#define  SIEPON_OAM_BUILD_RESP \
    PKGB(004, OAM_PDU_SIEPON_SNMP_SLEEP_MOD_CAP_R, 0x20, \
               siepon_oam_pwrs_pkgb_sleep_mode_cap_get, NULL) \
    PKGB(005, OAM_PDU_SIEPON_SNMP_EARLY_WAKEUP_CAP_R, 0x21, \
               siepon_oam_pwrs_pkgb_early_wakeup_cap_get, NULL) \
    PKGB(006, OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_R, 0x22, \
               siepon_oam_pwrs_pkgb_sleep_ind_cap_get, NULL) \
    PKGB(007, OAM_PDU_SIEPON_SNMP_ACKCAPABLE_R, 0x23, \
               siepon_oam_pwrs_pkgb_ack_cap_get, NULL) \
    PKGB(008, OAM_PDU_SIEPON_SNMP_POWERONTIME_R, 0x24, \
               siepon_oam_pwrs_pkgb_poweron_time_get, NULL) \
    PKGB(009, OAM_PDU_SIEPON_SNMP_MAX_SLEEP_PERIOD_R, 0x25, \
               siepon_oam_pwrs_pkgb_max_sleep_period_get, NULL) \
    PKGB(010, OAM_PDU_SIEPON_SNMP_MIN_AWARE_PERIOD_R, 0x26, \
               siepon_oam_pwrs_pkgb_min_aware_period_get, NULL) \
    PKGB(011, OAM_PDU_SIEPON_SNMP_EARLY_WAKEUUP_EN_RW, 0x27, \
               siepon_oam_pwrs_pkgb_early_wakeup_get, siepon_oam_pwrs_pkgb_early_wakeup_set) \
    PKGB(012, OAM_PDU_SIEPON_SNMP_SLEEP_IND_CAP_EN_RW, 0x28, \
               siepon_oam_pwrs_pkgb_sleep_ind_get, siepon_oam_pwrs_pkgb_sleep_ind_set) \
    PKGB(013, OAM_PDU_SIEPON_SNMP_ACK_ENABLE_RW, 0x29, \
               siepon_oam_pwrs_pkgb_ack_enable_get, siepon_oam_pwrs_pkgb_ack_enable_set) \
    PKGB(014, OAM_PDU_SIEPON_SNMP_T_SLEEP_PERIOD_RW, 0x2a, \
               siepon_oam_pwrs_pkgb_sleep_period_get, siepon_oam_pwrs_pkgb_sleep_period_set) \
    PKGB(015, OAM_PDU_SIEPON_SNMP_T_AWARE_PERIOD_RW, 0x2b, \
               siepon_oam_pwrs_pkgb_aware_period_get, siepon_oam_pwrs_pkgb_aware_period_set) \
    PKGB(016, OAM_PDU_SIEPON_SNMP_T_HOLD_PERIOD_RW, 0x2c, \
               siepon_oam_pwrs_pkgb_hold_period_get, siepon_oam_pwrs_pkgb_hold_period_set) \
 

typedef oam_status (*siepon_oam_get_rountine_t)(oam_int8  *pRecv,
        oam_int8  *pOut,
        oam_int32 *pInLen, oam_int32 *pOutLen);
typedef oam_status (*siepon_oam_set_rountine_t)(oam_uint8  *pRecv,
        oam_int32 *pInLen);

typedef struct {
    oam_uint8                   type;
    siepon_oam_get_rountine_t   get_func;
    siepon_oam_set_rountine_t   set_func;
} siepon_oam_xlat_rountine_t;

typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           opcode;
    oam_uint8           data[0];
} __attribute__((packed)) oam_pdu_siepon_t;

typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
} __attribute__((packed)) oam_siepon_hdr_t;


typedef struct {
    oam_siepon_hdr_t    siepon_hdr;
    oam_uint8           opcode;
    oam_uint32          sleep_duration;
} __attribute__((packed)) oam_siepon_pwrs_sleep_allowed_t;


/*SIEPON return code*/
#define OAM_SIEPON_ATTRIB_NOT_SUPPORTED                 0x86
#define OAM_SIEPON_VAR_SET_OK                           0x80
#define OAM_SIEPON_VAR_DATASIZE_EXCESSIVE               0x81
#define OAM_SIEPON_VAR_SET_BAD_PARAM                    0x86
#define OAM_SIEPON_VAR_SET_NO_RES                       0x87

/* SIEPON ext opcode definitions */
#define OAM_PDU_SIEPON_RESERVED                         0x00
#define OAM_PDU_SIEPON_GET_REQ                          0x01
#define OAM_PDU_SIEPON_GET_RESP                         0x02
#define OAM_PDU_SIEPON_SET_REQ                          0x03
#define OAM_PDU_SIEPON_SET_RESP                         0x04
#define OAM_PDU_SIEPON_ONU_AUTH                         0x05
#define OAM_PDU_SIEPON_FILE_IMG                         0x06
#define OAM_PDU_SIEPON_QUEUECFG                         0x0A
#define OAM_PDU_SIEPON_EVENT                            0xFF

#define OAM_PDU_SIEPON_SLEEP_ALLOWED                    0xFE
#define OAM_PDU_SIEPON_EARLY_WAKEUP_ONU                 0xFD
#define OAM_PDU_SIEPON_EARLY_WAKEUP_OLT                 0xFC

/* SIEPON SNMP optimized eOAM opcode definitions */
#define OAM_PDU_SIEPON_SNMP_GET_REQ                     0x00
#define OAM_PDU_SIEPON_SNMP_RESERVED_01                 0x01
#define OAM_PDU_SIEPON_SNMP_GET_RESP                    0x02
#define OAM_PDU_SIEPON_SNMP_SET_REQ                     0x03
#define OAM_PDU_SIEPON_SNMP_RESERVED_04                 0x04
#define OAM_PDU_SIEPON_SNMP_SET_RESP                    0x05
#define OAM_PDU_SIEPON_SNMP_USERDATAGRAM                0x06
#define OAM_PDU_SIEPON_SNMP_QUEUECFG                    0x07

/* SIEPON SNMP optimized eOAM err status definitions */
#define OAM_PDU_SIEPON_SNMP_NOERR                       0x00
#define OAM_PDU_SIEPON_SNMP_TOOBIG                      0x01
#define OAM_PDU_SIEPON_SNMP_NOSUCHNAME                  0x02
#define OAM_PDU_SIEPON_SNMP_BADVAL                      0x03
#define OAM_PDU_SIEPON_SNMP_RESERVED_04                 0x04
#define OAM_PDU_SIEPON_SNMP_GENERR                      0x05

#define OAM_SIEPON_LEAF_NULL                            0x0000

#define OAM_SIEPON_BRANCH_S                             0x07
#define OAM_SIEPON_BRANCH_E                             0xC7
#define OAM_SIEPON_BRANCH_NULL                          0x00
#define OAM_SIEPON_BRANCH_A                             0xC9

#define OAM_SIEPON_LEAF_PWR_SAVING_CONTRL               0x0002

#define OAM_SIEPON_LEAF_PWR_SAVING_CAP_DISCOVER         0x000D
#define PWRS_CAP_DISCOVER_SLEEP_MODE_NONE               0x00
#define PWRS_CAP_DISCOVER_SLEEP_MODE_TX                 0x01
#define PWRS_CAP_DISCOVER_SLEEP_MODE_TRX                0x02
#define PWRS_CAP_DISCOVER_SLEEP_MODE_TX_TRX             0x03

#define PWRS_CAP_EARLY_WAKEUP_SUPPORTED                 0x00
#define PWRS_CAP_EARLY_WAKEUP_NOT_SUPPORTED             0x01

#define OAM_SIEPON_LEAF_PWR_SAVING_CFG                  0x000E
#define PWRS_CFG_EARLY_WARKUP_ENABLE                    0x00
#define PWRS_CFG_EARLY_WARKUP_DISABLE                   0x01
#define PWRS_CFG_EARLY_WARKUP_NOT_SUPPORT               0xFF

#define OAM_SIEPON_LEAF_PWR_SAVING_CAP_DISCOVER_DPOE    0xFFFF
#define PWRS_CAP_CFG_SLEEP_MODE_TX                      0x00
#define PWRS_CAP_CFG_SLEEP_MODE_RX                      0x01
#define PWRS_CAP_CFG_SLEEP_MODE_TRX                     0x02
#define PWRS_CAP_CFG_SLEEP_MODE_CFG                     0x03

typedef struct {
    oam_uint8       sleep_mode_cap;
    oam_uint8       early_wakeup_cap;
    oam_uint8       vendor_size;
}  __attribute__((packed)) oam_siepon_pwrs_cap_container_struct;

typedef struct {
    oam_uint8       early_wakeup;
    oam_uint8       sleep_dur_max[6];
}  __attribute__((packed)) oam_siepon_pwrs_cfg_container_struct;

typedef struct {
    oam_uint32      sleep_dur;
    oam_uint32      wait_dur;
    oam_uint8   sleep_flag;
    oam_uint8   sleep_mode;
}  __attribute__((packed)) oam_siepon_pwrs_sleep_control_container_struct;



/* SIEPON SNMP optimized eOAM val definitions */
#define OAM_PDU_SIEPON_SNMP_CFG_DISABLE                 0x00
#define OAM_PDU_SIEPON_SNMP_CFG_ENABLE                  0x01

/* SIEPON SNMP optimized eOAM pdu header */
typedef struct {
    oam_pdu_hdr_t       hdr;
    oam_uint8           oui[OAM_OUI_LEN];
    oam_uint8           opcode;
    oam_uint16          reqid;
    oam_uint8           errstatus;
    oam_uint8           data[0];
} __attribute__((packed)) oam_snmp_pdu_siepon_t;


/* SIEPON SNMP optimized eOAM var */
typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint8           data[0];
} __attribute__((packed)) oam_snmp_siepon_var_descriptor_t;

typedef struct {
    oam_uint16          shortval;
} __attribute__((packed)) oam_snmp_siepon_short_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint8           sleep_mode_cap;
} __attribute__((packed)) oam_siepon_sleep_mode_cap_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint8           early_wakeup_cap;
} __attribute__((packed)) oam_siepon_early_wakeup_cap_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint8           sleep_ind_cap;
} __attribute__((packed)) oam_siepon_sleep_ind_cap_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint8           ack_cap;
} __attribute__((packed)) oam_siepon_ack_cap_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint16          tx_wakeup;
    oam_uint16          trx_wakeup;
} __attribute__((packed)) oam_siepon_poweron_time_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint16          max_sleep_1;
    oam_uint16          max_sleep_2;
} __attribute__((packed)) oam_siepon_max_sleep_time_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint16          min_aware;;
} __attribute__((packed)) oam_siepon_min_aware_time_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint8           enable;;
} __attribute__((packed)) oam_siepon_early_wakeup_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint8           enable;;
} __attribute__((packed)) oam_siepon_sleep_ind_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint8           enable;;
} __attribute__((packed)) oam_siepon_ack_enable_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint16          sleep_1;
    oam_uint16          sleep_2;
} __attribute__((packed)) oam_siepon_sleep_time_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint16          aware;
} __attribute__((packed)) oam_siepon_aware_time_t;

typedef struct {
    oam_uint8           datatype;
    oam_uint8           datalen;
    oam_uint16          hold;
} __attribute__((packed)) oam_siepon_hold_time_t;

void
handle_siepon_oams(
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len);

#endif

#endif
