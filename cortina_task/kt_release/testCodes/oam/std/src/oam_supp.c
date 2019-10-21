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

DEFINITIONS:  "DEVICE" means the Cortina Systems(TM) SDK product.
"You" or "CUSTOMER" means the entity or individual that uses the SOFTWARE.
"SOFTWARE" means the Cortina Systems(TM) SDK software.

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

#include <string.h>
#include "oam_cmn.h"
#include "oam_core.h"
#include "oam_mux_par.h"
#include "oam_adapt.h"
#include "oam_supp.h"

/*
 * OAM handler for discovery and extend org OAM
 */
static oam_vendor_register_handler_t  oam_oui_handlers[OAM_MAX_OUI];

/*
 * OAM handler for standard oam for special code process
 */
static oam_std_oam_handler_t     g_oam_handlers[OAM_MAX_STD_HDL_NUM];

/*
 * Dump all org oam packet switch
 */
static oam_dump_pkt_type_e g_oam_dump_pkt_type = OAM_DUMP_TYPE_NONE;

void oam_buf_print(oam_uint8 *pbuf , oam_int32 len)
{
    oam_int32 i , j = 0 , k;
    oam_uint8 strbuf[16];
    oam_int32 remain_len = 0;

    oam_printf("\r\n==============================================================================");

    for (i = 0 ; i < len ; i++) {
        if (i % 16 == 0) {
            j = 0;
            //oam_printf("\r\n%p: ", pbuf + i);
            oam_printf("\r\n%04x: ", i);
            memset(strbuf , 0 , sizeof(strbuf));
            remain_len = len - i;
        }

        strbuf[i % 16] =  *((oam_uint8 *)(pbuf + i));
        oam_printf("%02x ", *((oam_uint8 *)(pbuf + i)));

        if (++j == 8)
            oam_printf(" ");

        if (remain_len == j && (remain_len % 16) != 0) {
            if (j < 8) oam_printf(" ");

            for ( ; j < 16 ; j++) {
                oam_printf("   ");
            }
        }

        if (j == 16) {
            remain_len = 0;
            oam_printf("; ");

            for (k = 0 ; k < 16 ; k++) {
                if (strbuf[k] >= ' ' && strbuf[k] <= '~')
                    oam_printf("%c", strbuf[k]);
                else
                    oam_printf(".");
            }
        }
    }

    oam_printf("\r\n\n");
    return;
}

/******************************************************************************/
/* $rtn_hdr_start            OAM DUMP PACKET                                  */
/* CATEGORY   : App                                                           */
/* ACCESS     : Public                                                        */
/* BLOCK      : OAM                                                           */
/* CHIP       : ----                                                          */
void oam_dump_pkt(char *comment,
                  oam_port_id_t port, oam_llid_t llid,
                  oam_uint8 *buffer, oam_uint32 len)
/*                                                                            */
/* INPUTS     : o char *comment                                               */
/*              o oam_port_id_t port                                          */
/*              o oam_llid_t llid                                             */
/*              o oam_uint8 *buffer                                           */
/*              o oam_uint32 len                                              */
/* OUTPUTS    : ----                                                          */
/* RETURNS    : ----                                                          */
/*                                                                            */
/* DESCRIPTION:                                                               */
/* API to dump OAM packet                                                     */
/*                                                                            */
/* $rtn_hdr_end                                                               */
/******************************************************************************/
{
    oam_pdu_hdr_t *pdu = (oam_pdu_hdr_t *) buffer;
    oam_boolean dump = FALSE;
    OAM_ASSERT(comment != NULL
               && buffer != NULL);

    if (g_oam_dump_pkt_type == OAM_DUMP_TYPE_NONE) {
        return;
    }

    /*always dump too short pkt*/
    if (len < sizeof(oam_pdu_hdr_t)) {
        oam_printf("\n\n%s:pkt len = %d, port = %d, llid = %d ",
                   comment, len, port, llid);
        oam_buf_print(buffer, len);
        return;
    }

    if ((pdu->code == OAM_PDU_CODE_ORG_SPEC
         && g_oam_dump_pkt_type == OAM_DUMP_TYPE_ORG)
        || (pdu->code == OAM_PDU_CODE_EVENT
            && g_oam_dump_pkt_type == OAM_DUMP_TYPE_EVT)
        || g_oam_dump_pkt_type == OAM_DUMP_TYPE_ALL ) {
        dump = TRUE;
    }

    if (dump) {
        oam_printf("\n\n%s:pkt len = %d, port = %d, llid = %d ",
                   comment, len, port, llid);
        oam_buf_print(buffer, len);
        oam_printf("\n");
    }
}

/******************************************************************************/
/* $rtn_hdr_start            OAM SET PACKET DUMP                              */
/* CATEGORY   : App                                                           */
/* ACCESS     : Public                                                        */
/* BLOCK      : OAM                                                           */
/* CHIP       : ----                                                          */
void oam_set_pkt_dump(oam_dump_pkt_type_e type)
/*                                                                            */
/* INPUTS     : o oam_dump_pkt_type_e type                                    */
/* OUTPUTS    : ----                                                          */
/* RETURNS    : ----                                                          */
/*                                                                            */
/* DESCRIPTION:                                                               */
/* API to set OAM packet dump                                                 */
/*                                                                            */
/* $rtn_hdr_end                                                               */
/******************************************************************************/
{
    g_oam_dump_pkt_type = type;
}

/******************************************************************************/
/* $rtn_hdr_start            OAM OUI COMPARE                                  */
/* CATEGORY   : App                                                           */
/* ACCESS     : Public                                                        */
/* BLOCK      : OAM                                                           */
/* CHIP       : ----                                                          */
oam_boolean oam_is_oui_equal(oam_oui_t oui1, oam_oui_t oui2)
/*                                                                            */
/* INPUTS     : o oam_oui_t oui1                                              */
/*              o oam_oui_t oui2                                              */
/* OUTPUTS    : ----                                                          */
/* RETURNS    : o oam_boolean ret                                             */
/*                                                                            */
/* DESCRIPTION:                                                               */
/* API to compare the two OUIs are equal or not                               */
/*                                                                            */
/* $rtn_hdr_end                                                               */
/******************************************************************************/
{
    OAM_ASSERT_RET(oui1 != NULL, FALSE);
    OAM_ASSERT_RET(oui2 != NULL, FALSE);

    if (memcmp(oui1, oui2, OAM_OUI_LEN))
        return FALSE;
    else
        return TRUE;
}

/******************************************************************************/
/* $rtn_hdr_start            OAM HANDLER REGISTER                             */
/* CATEGORY   : App                                                           */
/* ACCESS     : Public                                                        */
/* BLOCK      : OAM                                                           */
/* CHIP       : ----                                                          */
oam_status oam_std_handler_register(
    oam_uint8 code,
    oam_vendor_handler_t handler)
/*                                                                            */
/* INPUTS     : o oam_uint8 code                                              */
/*              o oam_vendor_handler_t handler                                */
/* OUTPUTS    : ----                                                          */
/* RETURNS    : o oam_status ret                                              */
/*                                                                            */
/* DESCRIPTION:                                                               */
/* APi to register the std handlers to the system                             */
/*                                                                            */
/* $rtn_hdr_end                                                               */
/******************************************************************************/
{
    oam_uint32 i = 0;
    OAM_ASSERT_RET(code != 0, FALSE);

    for (i = 0; i < OAM_MAX_STD_HDL_NUM; i++) {
        if (g_oam_handlers[i].code  == code) {
            OAM_LOG_MAJ("The code is already register \n");
            return OAM_E_PARAM;
        }
    }

    /*oui doesn't exist in table, register the handler */
    for (i = 0; i < OAM_MAX_STD_HDL_NUM; i++) {
        if (0 == g_oam_handlers[i].code) {
            g_oam_handlers[i].code = code;
            g_oam_handlers[i].code_handler = handler;
            return OAM_E_OK;
        }
    }

    return OAM_E_RESOURCE;
}

/******************************************************************************/
/* $rtn_hdr_start            OAM STANDARD HANDLER                             */
/* CATEGORY   : App                                                           */
/* ACCESS     : Public                                                        */
/* BLOCK      : OAM                                                           */
/* CHIP       : ----                                                          */
oam_boolean  oam_std_handler(
    oam_uint8 code,
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
/*                                                                            */
/* INPUTS     : o oam_uint8 code                                              */
/*              o oam_llid_t llid                                             */
/*              o oam_uint8 * frame                                           */
/*              o oam_uint32 len                                              */
/* OUTPUTS    : ----                                                          */
/* RETURNS    : o oam_status ret                                              */
/*                                                                            */
/* DESCRIPTION:                                                               */
/* call the std oam handler according to the code                             */
/* TRUE  - call the handle success                                            */
/* FALSE - No handle register                                                 */
/*                                                                            */
/* $rtn_hdr_end                                                               */
/******************************************************************************/
{
    oam_uint8 i = 0;
    oam_vendor_handler_t handle = NULL;

    OAM_ASSERT_RET(frame != NULL, FALSE);
    OAM_ASSERT_RET(code != 0 , FALSE);

    for (i = 0; i < OAM_MAX_STD_HDL_NUM; i++) {
        if (g_oam_handlers[i].code  == code ) {
            handle = g_oam_handlers[i].code_handler;
        }
    }

    if (handle == NULL) {
        return FALSE;
    }

    handle(llid, frame, len);

    return TRUE;
}

/******************************************************************************/
/* $rtn_hdr_start            OAM HANDLER INIT                                 */
/* CATEGORY   : App                                                           */
/* ACCESS     : Public                                                        */
/* BLOCK      : OAM                                                           */
/* CHIP       : ----                                                          */
void oam_vendor_handlers_init(oam_vendor_handlers_t *handlers)
/*                                                                            */
/* INPUTS     : o oam_vendor_handlers_t * handlers                            */
/* OUTPUTS    : ----                                                          */
/* RETURNS    : ----                                                          */
/*                                                                            */
/* DESCRIPTION:                                                               */
/* Init the vendor oam handles to NULL                                        */
/*                                                                            */
/* $rtn_hdr_end                                                               */
/******************************************************************************/
{
    OAM_ASSERT(handlers != NULL);

    memset(handlers, 0x00, sizeof(oam_vendor_handlers_t));
}

/******************************************************************************/
/* $rtn_hdr_start            OAM REGISTER HANDLER INIT                        */
/* CATEGORY   : App                                                           */
/* ACCESS     : Public                                                        */
/* BLOCK      : OAM                                                           */
/* CHIP       : ----                                                          */
void oam_vendor_register_handler_init()
/*                                                                            */
/* INPUTS     : ----                                                          */
/* OUTPUTS    : ----                                                          */
/* RETURNS    : ----                                                          */
/*                                                                            */
/* DESCRIPTION:                                                               */
/* Init all vendors oam handles to NULL                                       */
/*                                                                            */
/* $rtn_hdr_end                                                               */
/******************************************************************************/
{
    oam_uint8 i = 0 ;

    memset(oam_oui_handlers, 0x00,
           sizeof(oam_vendor_register_handler_t) * OAM_MAX_OUI);
    memset(g_oam_handlers, 0x00,
           sizeof(oam_std_oam_handler_t)*OAM_MAX_STD_HDL_NUM);

    for (i = 0; i < OAM_MAX_OUI; i++) {
        oam_oui_handlers[i].valid = OAM_FALSE;
        oam_vendor_handlers_init(&oam_oui_handlers[i].handlers);
    }
}

/******************************************************************************/
/* $rtn_hdr_start  oam_vendor_handler_register                               */
/* CATEGORY   : App                                                           */
/* ACCESS     : Public                                                        */
/* BLOCK      : OAM                                                           */
/* CHIP       : ----                                                          */
oam_status oam_vendor_handler_register(
    oam_oui_t   oui,
    oam_vendor_handlers_t   handlers)
/*                                                                            */
/* INPUTS     : o oam_oui_t oui                                               */
/*              o oam_vendor_handlers_t handlers                              */
/* OUTPUTS    : ----                                                          */
/* RETURNS    : ----                                                          */
/*                                                                            */
/* DESCRIPTION:                                                               */
/* Register the vendor handlers to the system                                 */
/*                                                                            */
/* $rtn_hdr_end                                                               */
/******************************************************************************/
{
    oam_uint8 i = 0;

    if (oui[0] == 0 && oui[1] == 0 && oui[2] == 0)
        return OAM_E_PARAM;

    /*if oui already registered, just overwrite the callback function */
    for (i = 0; i < OAM_MAX_OUI; i++) {
        if (!oam_oui_handlers[i].valid)
            continue;

        if (oam_is_oui_equal(oui, oam_oui_handlers[i].oui)) {
            oam_oui_handlers[i].handlers = handlers;
            return OAM_E_OK;
        }
    }

    /*oui doesn't exist in table, register the handler */
    for (i = 0; i < OAM_MAX_OUI; i++) {
        if (!oam_oui_handlers[i].valid) {
            memcpy(oam_oui_handlers[i].oui, oui, OAM_OUI_LEN);
            oam_oui_handlers[i].handlers = handlers;
            oam_oui_handlers[i].valid = OAM_TRUE;
            return OAM_E_OK;
        }
    }

    return OAM_E_RESOURCE;
}

/******************************************************************************/
/* $rtn_hdr_start  oam_vendor_handler                                         */
/* CATEGORY   : App                                                           */
/* ACCESS     : Public                                                        */
/* BLOCK      : OAM                                                           */
/* CHIP       : ----                                                          */
/*                                                                            */
oam_boolean  oam_vendor_handler(
    oam_oui_t oui,
    oam_vendor_handler_type_e type,
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
/*                                                                           */
/* INPUTS  : o oui - register vendor oui                                     */
/*           o type - hanlder type                                           */
/*           o llid - llid port id                                           */
/*           o frame - the oam frame to process                              */
/*           o len - the length of frame                                     */
/* OUTPUTS : TRUE - call the handle success                                  */
/*           FALSE - No handle register                                      */
/* DESCRIPTION:                                                              */
/* call the vendor org oam hanlder                                           */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 i = 0;
    oam_vendor_handler_t handle = NULL;

    OAM_ASSERT_RET(frame != NULL, FALSE);

    switch (type) {
        case OAM_VENDOR_HANDLE_ORG:
            for (i = 0; i < OAM_MAX_OUI; i++) {
                if (oam_oui_handlers[i].valid
                    && oam_is_oui_equal(oui, oam_oui_handlers[i].oui)) {
                    handle = oam_oui_handlers[i].handlers.org_handler;
                    break;
                }
            }

            break;

        default:
            handle = NULL;
    }

    if (handle == NULL) {
        OAM_LOG_INFO("No handler found ,oui = %#x:%#x:%#x, handle type = %d\n",
                     oui[0], oui[1], oui[2], type);
        return FALSE;
    }

    handle(llid, frame, len);

    return TRUE;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_vendor_build_handler                                  */
/* CATEGORY   : App                                                          */
/* ACCESS     : Public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint32  oam_vendor_build_handler(
    oam_vendor_handler_type_e type,
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
/*                                                                           */
/* INPUTS  : o type - hanlder type                                           */
/*           o llid - llid port id                                           */
/*           o frame - the oam frame to process                              */
/*           o len - the length of frame                                     */
/* OUTPUTS : TRUE - call the handle success                                  */
/*           FALSE - No handle register                                      */
/* DESCRIPTION:                                                              */
/* call the vendor oam info build hanlder                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 i = 0;
    oam_vendor_build_handler_t handle = NULL;

    OAM_ASSERT_RET(frame != NULL, -1);

    switch (type) {
        case OAM_VENDOR_HANDLE_EXT_INFO_BUILD:
            for (i = 0; i < OAM_MAX_OUI; i++) {
                if (oam_oui_handlers[i].valid) {
                    handle = oam_oui_handlers[i].handlers.ext_info_build_handler;

                    if (handle != NULL) {
                        break;
                    }
                }
            }

            break;

        case OAM_VENDOR_HANDLE_INFO_BUILD:
            for (i = 0; i < OAM_MAX_OUI; i++) {
                if (oam_oui_handlers[i].valid) {
                    handle = oam_oui_handlers[i].handlers.info_build_handler;

                    if (handle != NULL) {
                        break;
                    }
                }
            }

            break;

        default:
            handle = NULL;
    }

    if (handle == NULL) {
        return -1;
    }

    return handle(llid, frame, len);
}


/*****************************************************************************/
/* $rtn_hdr_start  oam_vendor_proc_handler                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_boolean  oam_vendor_proc_handler(
    oam_vendor_handler_type_e type,
    oam_llid_t llid,
    oam_uint8 *frame,
    oam_uint32 len)
/*                                                                           */
/* INPUTS  : o type - hanlder type                                           */
/*           o llid - llid port id                                           */
/*           o frame - the oam frame to process                              */
/*           o len - the length of frame                                     */
/* OUTPUTS : TRUE - call the handle success                                  */
/*           FALSE - No handle register                                      */
/* DESCRIPTION:                                                              */
/* call the vendor oam info process hanlder                                  */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 i = 0;
    oam_vendor_handler_t handle = NULL;

    OAM_ASSERT_RET(frame != NULL, FALSE);

    switch (type) {
        case OAM_VENDOR_HANDLE_EXT_INFO_PROC:
            for (i = 0; i < OAM_MAX_OUI; i++) {
                if (oam_oui_handlers[i].valid) {
                    handle = oam_oui_handlers[i].handlers.ext_info_proc_handler;

                    if (handle != NULL) {
                        break;
                    }
                }
            }

            break;

        case OAM_VENDOR_HANDLE_INFO_PROC:
            for (i = 0; i < OAM_MAX_OUI; i++) {
                if (oam_oui_handlers[i].valid) {
                    handle = oam_oui_handlers[i].handlers.info_proc_handler;

                    if (handle != NULL) {
                        break;
                    }
                }
            }

            break;

        default:
            handle = NULL;
    }

    if (handle == NULL) {
        return FALSE;
    }

    handle(llid, frame, len);

    return TRUE;
}

