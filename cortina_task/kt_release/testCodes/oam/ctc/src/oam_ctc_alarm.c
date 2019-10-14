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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CTC_OAM
#include "oam_cmn.h"
#include "oam_adapt.h"
#include "oam_core.h"
#include "oam_client.h"
#include "oam_ctc_pdu.h"
#include "oam_ctc_client.h"
#include "oam_ctc_alarm.h"
#include "oam_ctc_adapt.h"
#ifndef __X86_DEMO__
#ifdef ZTE_SPECIFIC
#include "db_zte.h"
#endif/*END_ZTE_SPECIFIC*/
#endif

/*Global variables*/
ctc_oam_alarm_info_t g_onu_alarm_cfg[] = {
    /* alarm_code                   threshold_type                  info_need,      polling_need */
    {CTC_ALARM_EQUIPMENT,           CTC_THRESH_UNSUPPORT ,          TRUE,           FALSE  },
    {CTC_ALARM_POWER,               CTC_THRESH_UNSUPPORT ,          TRUE,           FALSE  },
    {CTC_ALARM_BATT_MISSING,        CTC_THRESH_UNSUPPORT,           FALSE,          FALSE  },
    {CTC_ALARM_BATT_FAILURE,        CTC_THRESH_UNSUPPORT ,          FALSE,          FALSE  },
    {CTC_ALARM_BATT_VOLTLOW,        CTC_THRESH_SIGNED_LOW ,         TRUE,           FALSE  },
    {CTC_ALARM_PHY_INTRUSION,       CTC_THRESH_UNSUPPORT,           FALSE,          FALSE  },
    {CTC_ALARM_SELF_TEST,           CTC_THRESH_UNSUPPORT ,          TRUE,           FALSE  },
    {CTC_ALARM_ONU_TEMP_HIGH,       CTC_THRESH_SIGNED_HIGH,         TRUE,           FALSE  },
    {CTC_ALARM_ONU_TEMP_LOW,        CTC_THRESH_SIGNED_LOW,          TRUE,           FALSE  },
    {CTC_ALARM_ONU_IAD_CONN,        CTC_THRESH_UNSUPPORT,           TRUE,           FALSE  },
    {CTC_ALARM_ONU_PONIF_SWITCH,    CTC_THRESH_UNSUPPORT,           TRUE,           FALSE  },
    {CTC_ALARM_ONU_SLP_STATUS_UPDATE, CTC_THRESH_UNSUPPORT,         TRUE,           FALSE  },
    {CTC_ALARM_ONU_DRY_CONTACTA,    CTC_THRESH_UNSUPPORT,           TRUE,           FALSE  },
    {CTC_ALARM_ONU_DRY_CONTACTB,    CTC_THRESH_UNSUPPORT,           TRUE,           FALSE  }
};

ctc_oam_alarm_info_t g_pon_alarm_cfg[] = {
    /* alarm_code               threshold_type              info_need,      polling_need */
    {CTC_ALARM_RX_POW_HIGH,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_ALARM_RX_POW_LOW,      CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_ALARM_TX_POW_HIGH,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_ALARM_TX_POW_LOW,      CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_ALARM_TX_BIAS_HIGH,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_ALARM_TX_BIAS_LOW,     CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_ALARM_VCC_HIGH,        CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_ALARM_VCC_LOW,         CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_ALARM_PON_TEMP_HIGH,   CTC_THRESH_SIGNED_HIGH,     TRUE,           TRUE},
    {CTC_ALARM_PON_TEMP_LOW,    CTC_THRESH_SIGNED_LOW,      TRUE,           TRUE},
    {CTC_WARN_RX_POW_HIGH,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_WARN_RX_POW_LOW,       CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_WARN_TX_POW_HIGH,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_WARN_TX_POW_LOW,       CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_WARN_TX_BIAS_HIGH,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_WARN_TX_BIAS_LOW,      CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_WARN_VCC_HIGH,         CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_WARN_VCC_LOW,          CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_WARN_PON_TEMP_HIGH,    CTC_THRESH_SIGNED_HIGH,     TRUE,           TRUE},
    {CTC_WARN_PON_TEMP_LOW,     CTC_THRESH_SIGNED_LOW,      TRUE,           TRUE},
    {CTC_ALARM_PON_RX_DROP,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_TX_DROP,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_RX_CRC,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_TX_CRC,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_RX_UNDER,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_TX_UNDER,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_RX_OVER,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_TX_OVER,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_RX_FRAG,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_TX_FRAG,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_RX_JABBER,   CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_TX_JABBER,   CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_RX_DISCA,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_TX_DISCA,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_RX_ERROR,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_ALARM_PON_TX_ERROR,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_RX_DROP,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_TX_DROP,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_RX_CRC,       CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_TX_CRC,       CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_RX_UNDER,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_TX_UNDER,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_RX_OVER,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_TX_OVER,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_RX_FRAG,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_TX_FRAG,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_RX_JABBER,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_TX_JABBER,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_RX_DISCA,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_TX_DISCA,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_RX_ERROR,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE},
    {CTC_WARN_PON_TX_ERROR,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           FALSE}
};

ctc_oam_alarm_info_t g_cuc_pon_alarm_cfg[] = {
    /* alarm_code               threshold_type              info_need,      polling_need */
    {CTC_ALARM_RX_POW_HIGH,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_ALARM_RX_POW_LOW,      CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_ALARM_TX_POW_HIGH,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_ALARM_TX_POW_LOW,      CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_ALARM_TX_BIAS_HIGH,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_ALARM_TX_BIAS_LOW,     CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_ALARM_VCC_HIGH,        CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_ALARM_VCC_LOW,         CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_ALARM_PON_TEMP_HIGH,   CTC_THRESH_SIGNED_HIGH,     TRUE,           TRUE},
    {CTC_ALARM_PON_TEMP_LOW,    CTC_THRESH_SIGNED_LOW,      TRUE,           TRUE},
    {CTC_WARN_RX_POW_HIGH,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_WARN_RX_POW_LOW,       CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_WARN_TX_POW_HIGH,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_WARN_TX_POW_LOW,       CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_WARN_TX_BIAS_HIGH,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_WARN_TX_BIAS_LOW,      CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_WARN_VCC_HIGH,         CTC_THRESH_UNSIGNED_HIGH,   TRUE,           TRUE},
    {CTC_WARN_VCC_LOW,          CTC_THRESH_UNSIGNED_LOW,    TRUE,           TRUE},
    {CTC_WARN_PON_TEMP_HIGH,    CTC_THRESH_SIGNED_HIGH,     TRUE,           TRUE},
    {CTC_WARN_PON_TEMP_LOW,     CTC_THRESH_SIGNED_LOW,      TRUE,           TRUE},
    {CUC_OUTOFTIME_TX_SLOT,     CTC_THRESH_UNSUPPORT,       TRUE,           TRUE},
};

/*port alarm cfg table*/
/*ctc*/
ctc_oam_alarm_info_t **g_port_alarm_cfg = NULL;
oam_uint32 g_port_alarm_num = 0;
/*cuc*/
ctc_oam_alarm_info_t **g_cuc_port_alarm_cfg = NULL;
oam_uint32 g_cuc_port_alarm_num = 0;

/*polling timer*/
ctc_oam_alarm_polling_cb_t g_alarm_polling_cb;


extern void ctc_oam_send(
    oam_llid_t llid,
    oam_uint8 *buf,
    oam_int32 len
);

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_init_port_alarm_info                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void ctc_oam_init_alarm_item_info(ctc_oam_alarm_info_t *item)
/*                                                                          */
/* INPUTS  :                                                                */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    item->alarm_state = CTC_ALARM_CLEAR;
    item->enable = CTC_ALARM_OFF;

    if (item->threshold_type == CTC_THRESH_SIGNED_HIGH) {
        item->raise_alarm_threshold.signValue = SIGNED_RAISED_DEF_THRESH;
        item->clear_alarm_threshold.signValue = SIGNED_CLEARED_DEF_THRESH;
    }
    else if (item->threshold_type == CTC_THRESH_SIGNED_LOW) {
        item->raise_alarm_threshold.signValue = SIGNED_CLEARED_DEF_THRESH;
        item->clear_alarm_threshold.signValue = SIGNED_RAISED_DEF_THRESH;
    }
    else if (item->threshold_type == CTC_THRESH_UNSIGNED_HIGH) {
        item->raise_alarm_threshold.unsignValue = UNSIGNED_RAISED_DEF_THRESH;
        item->clear_alarm_threshold.unsignValue = UNSIGNED_CLEARED_DEF_THRESH;
    }
    else if (item->threshold_type == CTC_THRESH_UNSIGNED_LOW) {
        item->raise_alarm_threshold.unsignValue = UNSIGNED_CLEARED_DEF_THRESH;
        item->clear_alarm_threshold.unsignValue = UNSIGNED_RAISED_DEF_THRESH;
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_init_pon_alarm_info                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void ctc_oam_init_pon_alarm_info()
/*                                                                          */
/* INPUTS  :                                                                */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 i = 0;
    oam_uint8 num = 0;
    ctc_oam_alarm_info_t *pInfo = NULL;
#if 0

    if (!oam_is_cuc_oam()) {
        num = sizeof(g_pon_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);
        pInfo = &g_pon_alarm_cfg[0];
    }
    else {
        num = sizeof(g_cuc_pon_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);
        pInfo = &g_cuc_pon_alarm_cfg[0];
    }

#endif

    /*init both table*/
    /*init ctc table*/
    num = sizeof(g_pon_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);
    pInfo = &g_pon_alarm_cfg[0];

    for (i = 0; i < num; i++) {
        ctc_oam_init_alarm_item_info(pInfo);
        pInfo++;
    }

    /*init cuc table*/
    num = sizeof(g_cuc_pon_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);
    pInfo = &g_cuc_pon_alarm_cfg[0];

    for (i = 0; i < num; i++) {
        ctc_oam_init_alarm_item_info(pInfo);
        pInfo++;
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_init_onu_alarm_info                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void ctc_oam_init_onu_alarm_info()
/*                                                                          */
/* INPUTS  :                                                                */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 i = 0;
    oam_uint8 num = sizeof(g_onu_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);

    for (i = 0; i < num; i++) {
        ctc_oam_init_alarm_item_info(&g_onu_alarm_cfg[i]);
    }
}


/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_init_port_alarm_info                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void ctc_oam_init_port_alarm_info()
/*                                                                          */
/* INPUTS  :                                                                */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 i = 0;
    oam_uint8 port = 0;
    ctc_oam_alarm_info_t item_cfg[] = {
        /* alarm_code               threshold_type              info_need,  polling_need  */
        {CTC_ALARM_AUTO_NEG_FAIL,   CTC_THRESH_UNSUPPORT ,      FALSE,      FALSE  },
        {CTC_ALARM_PORT_LOS,        CTC_THRESH_UNSUPPORT ,      FALSE,      FALSE  },
        {CTC_ALARM_PORT_FAIL,       CTC_THRESH_UNSUPPORT,       FALSE,      FALSE  },
        {CTC_ALARM_PORT_LB,         CTC_THRESH_UNSUPPORT ,      FALSE,      FALSE  },
        {CTC_ALARM_PORT_CONGEST,    CTC_THRESH_UNSUPPORT,       FALSE,      TRUE  },
        {CTC_ALARM_PORT_RX_DROP,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_TX_DROP,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_RX_CRC,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_TX_CRC,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_RX_UNDER,   CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_TX_UNDER,   CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_RX_OVER,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_TX_OVER,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_RX_FRAG,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_TX_FRAG,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },

        {CTC_ALARM_PORT_RX_JABBER,  CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_TX_JABBER,  CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_RX_DISCA,   CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_TX_DISCA,   CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_RX_ERROR,   CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_TX_ERROR,   CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },

        {CTC_ALARM_PORT_STATUS,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_RX_DROP,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_TX_DROP,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_RX_CRC,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_TX_CRC,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_RX_UNDER,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_TX_UNDER,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_RX_OVER,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_TX_OVER,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_RX_FRAG,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_TX_FRAG,     CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_RX_JABBER,   CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_TX_JABBER,   CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_RX_DISCA,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_TX_DISCA,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_RX_ERROR,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_TX_ERROR,    CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_WARN_PORT_STATUS,      CTC_THRESH_UNSIGNED_HIGH,   TRUE,       FALSE  },
        {CTC_ALARM_PORT_LOOPDETECT, CTC_THRESH_UNSUPPORT,       FALSE,      FALSE  }
    };
    ctc_oam_alarm_info_t cuc_item_cfg[] = {
        /* alarm_code               threshold_type              info_need,  polling_need  */
        {CTC_ALARM_AUTO_NEG_FAIL,   CTC_THRESH_UNSUPPORT ,      FALSE,      FALSE  },
        {CTC_ALARM_PORT_LOS,        CTC_THRESH_UNSUPPORT ,      FALSE,      FALSE  },
        {CTC_ALARM_PORT_FAIL,       CTC_THRESH_UNSUPPORT,       FALSE,      FALSE  },
        {CTC_ALARM_PORT_LB,         CTC_THRESH_UNSUPPORT ,      TRUE,       FALSE  },
        {CTC_ALARM_PORT_CONGEST,    CTC_THRESH_UNSUPPORT,       FALSE,      FALSE  }
    };

    g_port_alarm_num = sizeof(item_cfg) / sizeof(ctc_oam_alarm_info_t);
    g_cuc_port_alarm_num = sizeof(cuc_item_cfg) / sizeof(ctc_oam_alarm_info_t);

    g_port_alarm_cfg = OAM_MALLOC(sizeof(ctc_oam_alarm_info_t *)
                                  *CTC_MAX_ONU_UNI_PORTS);
    g_cuc_port_alarm_cfg = OAM_MALLOC(sizeof(ctc_oam_alarm_info_t *)
                                      *CTC_MAX_ONU_UNI_PORTS);

    if (g_port_alarm_cfg == NULL || g_cuc_port_alarm_cfg == NULL) {
        OAM_ERROR("alarm malloc fail: fatal error\n");
        return;
    }

    memset(g_port_alarm_cfg, 0,
           sizeof(ctc_oam_alarm_info_t *)*CTC_MAX_ONU_UNI_PORTS);
    memset(g_cuc_port_alarm_cfg, 0,
           sizeof(ctc_oam_alarm_info_t *)*CTC_MAX_ONU_UNI_PORTS);

    for (port = 0; port < CTC_MAX_ONU_UNI_PORTS; port++) {
        g_port_alarm_cfg[port] = OAM_MALLOC(sizeof(item_cfg));
        g_cuc_port_alarm_cfg[port] = OAM_MALLOC(sizeof(cuc_item_cfg));

        if (g_port_alarm_cfg[port] == NULL || g_cuc_port_alarm_cfg[port] == NULL) {
            OAM_ERROR("alarm malloc fail: fatal error\n");
            return;
        }

        memcpy(g_port_alarm_cfg[port], item_cfg, sizeof(item_cfg));
        memcpy(g_cuc_port_alarm_cfg[port], cuc_item_cfg, sizeof(cuc_item_cfg));

        for (i = 0; i < g_port_alarm_num; i++) {
            ctc_oam_init_alarm_item_info(&g_port_alarm_cfg[port][i]);
        }

        for (i = 0; i < g_cuc_port_alarm_num; i++) {
            ctc_oam_init_alarm_item_info(&g_cuc_port_alarm_cfg[port][i]);
        }
    }

#if 0

    if (!oam_is_cuc_oam()) {
        g_port_alarm_num = sizeof(item_cfg) / sizeof(ctc_oam_alarm_info_t);
    }
    else {
        g_port_alarm_num = sizeof(cuc_item_cfg) / sizeof(ctc_oam_alarm_info_t);
    }

    g_port_alarm_cfg = iros_malloc(IROS_MID_OAM,
                                   sizeof(ctc_oam_alarm_info_t *)*CTC_MAX_ONU_UNI_PORTS);

    if (!g_port_alarm_cfg)
        return;

    memset(g_port_alarm_cfg, 0,
           sizeof(ctc_oam_alarm_info_t *)*CTC_MAX_ONU_UNI_PORTS);

    for (port = 0; port < CTC_MAX_ONU_UNI_PORTS; port++) {
        if (!oam_is_cuc_oam()) {
            g_port_alarm_cfg[port] = iros_malloc(IROS_MID_OAM, sizeof(item_cfg));
        }
        else {
            g_port_alarm_cfg[port] = iros_malloc(IROS_MID_OAM, sizeof(cuc_item_cfg));
        }

        if (!g_port_alarm_cfg[port])
            return;

        if (!oam_is_cuc_oam()) {
            memcpy(g_port_alarm_cfg[port], item_cfg, sizeof(item_cfg));
        }
        else {
            memcpy(g_port_alarm_cfg[port], cuc_item_cfg, sizeof(cuc_item_cfg));
        }

        for (i = 0; i < g_port_alarm_num; i++) {
            ctc_oam_init_alarm_item_info(&g_port_alarm_cfg[port][i]);
        }
    }

#endif
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_alarm_start_polling_timer                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void ctc_alarm_stop_polling_timer()
/*                                                                          */
/* INPUTS  :                                                                */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    if (g_alarm_polling_cb.timer_handler != 0) {
        ca_timer_del(g_alarm_polling_cb.timer_handler );
        g_alarm_polling_cb.timer_handler = 0;
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_alarm_start_polling_timer                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void ctc_alarm_start_polling_timer()
/*                                                                          */
/* INPUTS  :                                                                */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    g_alarm_polling_cb.timer_handler = ca_circle_timer_add(
                                           g_alarm_polling_cb.timer_value,
                                           ctc_oam_alarm_polling,
                                           NULL);
}

/* find at least one llid ext oam complete */
oam_boolean ctc_oam_discovery_result_get()
{
    oam_uint32 i = 0;
    oam_if_t *oamif = NULL;

    for (i = 0; i < OAM_MAX_IF; i++) {
        oamif = oam_intf_get(i);

        if (OAM_FALSE == oamif->valid)
            continue;

        if (oamif->ctc_OAM_Ext_Status == OAM_CTC_EXT_ONU_COMPLETED) {
            return TRUE;
        }
    }

    return FALSE;
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_alarm_init                                        */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void ctc_oam_alarm_init()
/*                                                                          */
/* INPUTS  :                                                                */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    memset(&g_alarm_polling_cb, 0, sizeof(g_alarm_polling_cb));
    g_alarm_polling_cb.timer_value = ALARM_POLLING_INTERVAL;

    /*init alarm info table*/
    ctc_oam_init_pon_alarm_info();
    ctc_oam_init_onu_alarm_info();
    ctc_oam_init_port_alarm_info();

    ctc_oam_eth_port_alarm_reg_adapt();
    ctc_oam_gpio_alarm_reg_adapt();
    
    ctc_oam_ld_alarm_reg_adapt();
    ctc_oam_opm_alarm_reg_adapt();
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_get_alarm_info                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
static oam_status ctc_oam_get_alarm_info_table(
    ctc_oam_alarm_class_e type,
    oam_port_id_t port,
    oam_uint32 *num,
    ctc_oam_alarm_info_t **info)
/*                                                                           */
/* INPUTS  : o alarm_code---alarm id                                         */
/*           o enable---                                                     */
/*           o portId---port id                                              */
/* OUTPUTS : -----                                                           */
/* RETURNS : o EPON_RETURN_XXX(see section 3.7 for return code definitions)  */
/* DESCRIPTION:                                                              */
/* API to set alarm on/off                                                   */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    if (!ctc_oam_discovery_result_get()) {
        OAM_ORG_LOG_INFO("ctc oam discovery not finish, you may get wrong info\n");
    }

    if (type == CTC_PON_ALARM) {
        if (!oam_is_cuc_oam()) {
            *info = g_pon_alarm_cfg;
            *num = sizeof(g_pon_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);
        }
        else {
            *info = g_cuc_pon_alarm_cfg;
            *num = sizeof(g_cuc_pon_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);
        }
    }
    else if (type == CTC_ONU_ALARM) {
        *info = g_onu_alarm_cfg;
        *num = sizeof(g_onu_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);
    }
    else if (type == CTC_PORT_ALARM) {
        if (port > CTC_MAX_ONU_UNI_PORTS || port < OAM_UNI_PORT_ID1) {
            return OAM_E_PARAM;
        }

        if (!oam_is_cuc_oam()) {
            *info = g_port_alarm_cfg[port - 1];
            *num = g_port_alarm_num;
        }
        else {
            *info = g_cuc_port_alarm_cfg[port - 1];
            *num = g_cuc_port_alarm_num;
        }
    }
    else {
        return OAM_E_PARAM;
    }

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_get_alarm_info                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
static ctc_oam_alarm_info_t *ctc_oam_get_alarm_info(
    ctc_oam_alarm_class_e type,
    oam_port_id_t port,
    oam_uint16  alarm_code)
/*                                                                           */
/* INPUTS  : o alarm_code---alarm id                                         */
/*           o enable---                                                     */
/*           o portId---port id                                              */
/* OUTPUTS : -----                                                           */
/* RETURNS : o EPON_RETURN_XXX(see section 3.7 for return code definitions)  */
/* DESCRIPTION:                                                              */
/* API to set alarm on/off                                                   */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    ctc_oam_alarm_info_t *p_info_table = NULL;
    oam_uint32 num = 0;
    oam_uint32 i = 0;

    if (OAM_E_OK != ctc_oam_get_alarm_info_table(type, port, &num, &p_info_table)) {
        OAM_ORG_LOG("can not search alarm table, type %d, port %d\n", type, port);
        return NULL;
    }

    if (!p_info_table) {
        OAM_ORG_LOG("alarm table is null\n");
        return NULL;
    }

    for (i = 0; i < num; i++) {
        if (p_info_table[i].alarm_code == alarm_code) {
            return &p_info_table[i];
        }
    }

    /*Not search it*/
    OAM_ORG_LOG("can not search table item, type %d, port %d\n", type, port);
    return NULL;

}

void ctc_oam_clear_alarm_event(
    ctc_oam_alarm_class_e type,
    oam_port_id_t port,
    oam_uint16  alarm_code)
{
    ctc_oam_alarm_info_t *p_info = NULL;
    p_info = ctc_oam_get_alarm_info(type, port, alarm_code);

    if (p_info == NULL) {
        return;
    }

    p_info->alarm_state = CTC_ALARM_CLEAR;
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_build_alarm_event                                 */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void ctc_oam_build_alarm_event(
    ctc_oam_alarm_class_e type,
    oam_port_id_t port,
    oam_uint16  alarm_code,
    oam_uint8 alarm_op,
    oam_uint64 alarm_info)
/*                                                                           */
/* INPUTS  : o  type - alarm type                                            */
/*           o  port - ONU uni or pon port, onu self                         */
/*           o  alarm_code  - alarm code                                     */
/*           o  alarm_op  - 0/raise 1/clear                                  */
/*           o  alarm_info  - info                                           */
/* OUTPUTS : ----                                                            */
/* RETURNS :                                                                 */
/* DESCRIPTION:                                                              */
/* API to compose CTC alram event OAM frame                                  */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_int32 len = 0;
    oam_ctc_tlv_org_spec_t *p_tlv_org_event_oam = NULL ;
    oam_uint8   *p_seq = NULL;
    oam_uint16 seq = 0;
    oam_uint8 pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *oamif = oam_intf_find_valid();
    ctc_oam_alarm_info_t *p_info = NULL;
    oam_uint32   alarm_info_len = 0;
    oam_uint32   alarm_infor_uint32 = 0;
    oam_uint8    alarm_infor_buffer[8];

    /* 0x0115 --- DownstreamDropEventsAlarm
       0x0134 --- UpstreamErrorsWarning
       0x0306 --- DownstreamDropEventsAlarm
       0x0327 --- StatusChangeTimesWarning
    */
    if ( ( (alarm_code >= CTC_ALARM_PON_RX_DROP) && (alarm_code <= CTC_WARN_PON_TX_ERROR) )
         || (( alarm_code >= CTC_ALARM_PORT_RX_DROP)  && ( alarm_code <= CTC_WARN_PORT_STATUS)) ) {
        alarm_info_len = 8;
    }
    else {
        alarm_info_len = 4;
    }

    if (oamif == NULL) {
        OAM_ORG_LOG("oamif is NULL.\n");
        return;
    }

    p_info = ctc_oam_get_alarm_info(type, port, alarm_code);

    if (p_info == NULL) {
        OAM_ORG_LOG("Alarm info is NULL.\n");
        return;
    }

    /*check alarm is opened*/
    if (p_info->enable == CTC_ALARM_OFF) {
        return;
    }

    /*check whether redundant alarm*/
    if (p_info->alarm_state == alarm_op) {
        return;
    }

    p_info->alarm_state = alarm_op;

    memset(pdu, 0x00, OAM_MAX_OAMPDU_SIZE);

    oam_build_pdu_hdr(oamif, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_EVENT);

    p_seq = pdu + sizeof(oam_pdu_hdr_t);
    seq = htons(++oamif->seq_num_next_tx);
    memcpy(p_seq, &seq, sizeof(seq));
    len = sizeof(oam_pdu_hdr_t) + sizeof(oam_uint16);

    p_tlv_org_event_oam = (oam_ctc_tlv_org_spec_t *)(pdu + len);

    len +=  sizeof(oam_ctc_tlv_org_spec_t) - 1; /*remove seq_num field*/
    p_tlv_org_event_oam ->type = OAM_INFO_TLV_ORG_SPEC;
    p_tlv_org_event_oam ->length = sizeof(oam_ctc_tlv_org_spec_t) -
                                   1;/*remove seq_num field*/
    memcpy(p_tlv_org_event_oam->oui, oam_oui_ctc, OAM_OUI_LEN);

    p_tlv_org_event_oam ->org_spec_value.alarm_id = htons(alarm_code);
    p_tlv_org_event_oam ->org_spec_value.time_stamp = 0;
    p_tlv_org_event_oam ->org_spec_value.alarm_state = alarm_op;

    if (oam_is_cuc_oam()) {
        if (alarm_op == CTC_ALARM_RAISE) {
            p_tlv_org_event_oam ->org_spec_value.alarm_state = 1;
        }
        else if (alarm_op == CTC_ALARM_CLEAR) {
            p_tlv_org_event_oam ->org_spec_value.alarm_state = 0;
        }
    }

    if (type == CTC_ONU_ALARM) {
        p_tlv_org_event_oam ->org_spec_value.object_type = htons(
                    OAM_CTC_ALARM_TYPE_ONU);
        p_tlv_org_event_oam ->org_spec_value.instance_num = htonl(0xffffffff);
    }
    else if (type == CTC_PON_ALARM) {
        p_tlv_org_event_oam ->org_spec_value.object_type = htons(
                    OAM_CTC_ALARM_TYPE_PON);
        p_tlv_org_event_oam ->org_spec_value.instance_num = htonl(0x00000001);
    }
    else if (type == CTC_PORT_ALARM) {
        p_tlv_org_event_oam ->org_spec_value.object_type = htons(
                    OAM_CTC_ALARM_TYPE_UNI);
        p_tlv_org_event_oam ->org_spec_value.instance_num = htonl(0x01000000 | port);
    }

    if (p_info->info_need) {
        if ( alarm_info_len == 4) {
            alarm_infor_uint32 = alarm_info;
            alarm_infor_uint32 = htonl (alarm_infor_uint32);
            memcpy( alarm_infor_buffer, &alarm_infor_uint32, 4 );
        }
        else {
            alarm_info = htonll(alarm_info);
            memcpy( alarm_infor_buffer, &alarm_info, 8 );
        }

        memcpy(p_tlv_org_event_oam->org_spec_value.alarm_info, alarm_infor_buffer,
               alarm_info_len);
        len += alarm_info_len;
        p_tlv_org_event_oam ->length += alarm_info_len;
    }


    ctc_oam_send(oamif->llid, pdu, len);

}


/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_set_alarm_config                                  */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
int ctc_oam_set_alarm_config(
    ctc_oam_alarm_class_e   type,
    unsigned int              portId,
    unsigned short              alarm_code,
    unsigned char               enable)
/*                                                                           */
/* INPUTS  : o type---alarm type                                             */
/*           o portid--- port id                                             */
/*           o alarm_code---code                                             */
/*           o enable---CTC_ALARM_ON/CTC_ALARM_OFF                           */
/* OUTPUTS : -----                                                           */
/* RETURNS : o EPON_RETURN_XXX(see section 3.7 for return code definitions)  */
/* DESCRIPTION:                                                              */
/* API to set alarm on/off                                                   */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    ctc_oam_alarm_info_t *p_info = NULL;


    if (enable != CTC_ALARM_ON && enable != CTC_ALARM_OFF) {
        return OAM_E_PARAM;
    }

    p_info = ctc_oam_get_alarm_info(type, portId, alarm_code);

    if (!p_info) {
        return OAM_E_PARAM;
    }

    if (enable == p_info->enable) {
        /*Do nothing*/
        return OAM_E_OK;
    }

    p_info->enable = enable;

    /*check the alarm status when the first set it to on or last set it off*/
    if (enable == CTC_ALARM_ON) {
        if (p_info->polling_need) {
            g_alarm_polling_cb.polling_enable_counter++;

            if (g_alarm_polling_cb.polling_enable_counter == 1) {
                /*Start the polling time to check the alarm status period*/
                ctc_alarm_start_polling_timer();
            }
        }
    }
    else {
        if (p_info->polling_need) {
            if (g_alarm_polling_cb.polling_enable_counter > 0) {
                g_alarm_polling_cb.polling_enable_counter--;

                if (g_alarm_polling_cb.polling_enable_counter == 0) {
                    ctc_alarm_stop_polling_timer();
                }
            }
        }
    }

    return OAM_E_OK;

}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_get_alarm_config                                  */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
int ctc_oam_get_alarm_config(
    ctc_oam_alarm_class_e   type,
    unsigned int              portId,
    unsigned short              alarm_code,
    unsigned int             *alarm_config)
/*                                                                           */
/* INPUTS  : o alarm_code---alarm id                                         */
/*           o portId---port id                                              */
/* OUTPUTS : o raiseValue                                                    */
/* OUTPUTS : o clearValue                                                    */
/* RETURNS : o CS_E_XXX(see section 3.7 for return code definitions)         */
/* DESCRIPTION:                                                              */
/* API to get alarm threshold                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    ctc_oam_alarm_info_t *p_info = NULL;

    p_info = ctc_oam_get_alarm_info(type, portId, alarm_code);

    if (!p_info) {
        return OAM_E_PARAM;
    }

    *alarm_config = p_info->enable;
    return OAM_E_OK;

}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_set_alarm_threshold                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status ctc_oam_set_alarm_threshold(
    ctc_oam_alarm_class_e type,
    oam_uint32 portId,
    oam_uint16  alarm_code,
    oam_uint32 raiseValue,
    oam_uint32 clearValue)
/*                                                                           */
/* INPUTS  : o alarm_code---alarm id                                         */
/*           o raiseValue---raise value                                      */
/*           o clearValue---clear value                                      */
/*           o portId---port id                                              */
/* OUTPUTS : o ------                                                        */
/* RETURNS : o EPON_RETURN_XXX(see section 3.7 for return code definitions)  */
/* DESCRIPTION:                                                              */
/* API to set alarm threshold                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    ctc_oam_alarm_info_t *p_info = NULL;

    p_info = ctc_oam_get_alarm_info(type, portId, alarm_code);

    if (!p_info) {
        return OAM_E_PARAM;
    }


    if (p_info->threshold_type == CTC_THRESH_SIGNED_LOW ||
        p_info->threshold_type == CTC_THRESH_SIGNED_HIGH) {
        p_info->raise_alarm_threshold.signValue = raiseValue;
        p_info->clear_alarm_threshold.signValue = clearValue;
        return OAM_E_OK;
    }
    else if (p_info->threshold_type == CTC_THRESH_UNSIGNED_HIGH ||
             p_info->threshold_type == CTC_THRESH_UNSIGNED_LOW) {
        p_info->raise_alarm_threshold.unsignValue = raiseValue;
        p_info->clear_alarm_threshold.unsignValue = clearValue;
        return OAM_E_OK;
    }
    else {
        return OAM_E_ERROR;
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_get_alarm_threshold                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status ctc_oam_get_alarm_threshold(
    ctc_oam_alarm_class_e type,
    oam_uint32 portId,
    oam_uint16  alarm_code,
    oam_uint32 *raiseValue,
    oam_uint32 *clearValue)
/*                                                                           */
/* INPUTS  : o alarm_code---alarm id                                         */
/*           o portId---port id                                              */
/* OUTPUTS : o raiseValue                                                    */
/* OUTPUTS : o clearValue                                                    */
/* RETURNS : o EPON_RETURN_XXX(see section 3.7 for return code definitions)  */
/* DESCRIPTION:                                                              */
/* API to get alarm threshold                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    ctc_oam_alarm_info_t *p_info = NULL;

    p_info = ctc_oam_get_alarm_info(type, portId, alarm_code);

    if (!p_info) {
        return OAM_E_PARAM;
    }


    if (p_info->threshold_type == CTC_THRESH_SIGNED_LOW ||
        p_info->threshold_type == CTC_THRESH_SIGNED_HIGH) {
        * raiseValue = p_info->raise_alarm_threshold.signValue ;
        *clearValue = p_info->clear_alarm_threshold.signValue;
        return OAM_E_OK;

    }
    else if (p_info->threshold_type == CTC_THRESH_UNSIGNED_HIGH ||
             p_info->threshold_type == CTC_THRESH_UNSIGNED_LOW) {
        *raiseValue = p_info->raise_alarm_threshold.unsignValue;
        *clearValue = p_info->clear_alarm_threshold.unsignValue;
        return OAM_E_OK;
    }
    else {
        return OAM_E_ERROR;
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_alarm_threshold_get                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status ctc_oam_alarm_status_get(
    ctc_oam_alarm_class_e type,
    oam_uint32 portId,
    oam_uint16  alarm_code,
    oam_uint8 *status,
    oam_uint32 *info)
/*                                                                          */
/* INPUTS  :                                                                */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status ret = OAM_E_OK;

    /*add code here for alarm status get*/
    /* if alarm raise, return status=1, else return status=0 */
    switch (alarm_code) {
        case CUC_OUTOFTIME_TX_SLOT:
            ctc_oam_laser_alarm_status_get(status);

            *info = 1;
            break;

        case CTC_ALARM_PORT_CONGEST:
            ctc_oam_up_congest_status_get(portId, status);
            OAM_LOG_MAJ("port %d, status %d\n", portId, status);
            
            break;

        default:
            ret = OAM_E_ERROR;
    }

    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_alarm_current_value_get                           */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status ctc_oam_alarm_current_value_get(
    ctc_oam_alarm_class_e type,
    oam_uint32 portId,
    oam_uint16  alarm_code,
    oam_uint32 *value)
/*                                                                          */
/* INPUTS  :                                                                */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status ret = OAM_E_OK;
    oam_int16 tem = 0;
    oam_uint16 svcc = 0, tx_bias = 0, tx_power = 0, rx_power = 0;
    oam_uint32 tmp = 0;

    ret = ctc_oam_onu_opmdiag_get_adapt(&tem, &svcc, &tx_bias, &tx_power,
                                        &rx_power);
    OAM_ORG_DEBUG("temp=%d,svcc=%d,bias=%d,tx_pow=%d, rx_pow=%d\n",
                     tem, svcc, tx_bias, tx_power, rx_power);

    switch (alarm_code) {
        case CTC_ALARM_PON_TEMP_HIGH:
        case CTC_ALARM_PON_TEMP_LOW:
        case CTC_WARN_PON_TEMP_HIGH:
        case CTC_WARN_PON_TEMP_LOW:
            tmp = (oam_uint32)tem;
            break;

        case CTC_ALARM_RX_POW_HIGH:
        case CTC_ALARM_RX_POW_LOW:
        case CTC_WARN_RX_POW_HIGH:
        case CTC_WARN_RX_POW_LOW:
            tmp = (oam_uint32)rx_power;
            break;

        case CTC_ALARM_TX_POW_HIGH:
        case CTC_ALARM_TX_POW_LOW:
        case CTC_WARN_TX_POW_HIGH:
        case CTC_WARN_TX_POW_LOW:
            tmp = (oam_uint32)tx_power;
            break;

        case CTC_ALARM_TX_BIAS_HIGH:
        case CTC_ALARM_TX_BIAS_LOW:
        case CTC_WARN_TX_BIAS_HIGH:
        case CTC_WARN_TX_BIAS_LOW:
            tmp = (oam_uint32)tx_bias;
            break;

        case CTC_ALARM_VCC_HIGH:
        case CTC_ALARM_VCC_LOW:
        case CTC_WARN_VCC_HIGH:
        case CTC_WARN_VCC_LOW:
            tmp = (oam_uint32)svcc;
            break;

        default:
            ret = OAM_E_ERROR;
            break;
    }

    *value = tmp;
    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_alarm_polling_handle                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void ctc_oam_alarm_polling_handle(
    ctc_oam_alarm_class_e type,
    oam_port_id_t port,
    oam_uint32 num,
    ctc_oam_alarm_info_t *p_info)
/*                                                                           */
/* INPUTS  :                                                                 */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint32 i = 0;
    oam_uint32 unsign_value = 0;
    oam_int32 sign_value = 0;
    oam_uint8 status = 0;

    for (i = 0; i < num; i++) {
        if (p_info[i].enable == CTC_ALARM_ON && p_info[i].polling_need) {
            /*need check*/
            if (p_info[i].threshold_type != CTC_THRESH_UNSUPPORT) {
                /*get current value to compare with threshold*/
                if (OAM_E_OK != ctc_oam_alarm_current_value_get(type, port,
                        p_info[i].alarm_code, &unsign_value)) {
                    continue;
                }

                sign_value = unsign_value;

                switch (p_info[i].threshold_type) {
                    case CTC_THRESH_SIGNED_HIGH:
                        if (sign_value >= p_info[i].raise_alarm_threshold.signValue) {
                            ctc_oam_build_alarm_event(type, port, p_info[i].alarm_code, CTC_ALARM_RAISE,
                                                      (oam_uint64)sign_value);
                        }
                        else if (sign_value <  p_info[i].clear_alarm_threshold.signValue) {
                            ctc_oam_build_alarm_event(type, port, p_info[i].alarm_code, CTC_ALARM_CLEAR,
                                                      (oam_uint64)sign_value);
                        }

                        break;

                    case CTC_THRESH_UNSIGNED_HIGH:
                        if (unsign_value >= p_info[i].raise_alarm_threshold.unsignValue) {
                            ctc_oam_build_alarm_event(type, port, p_info[i].alarm_code, CTC_ALARM_RAISE,
                                                      (oam_uint64)unsign_value);
                        }
                        else if (unsign_value <  p_info[i].clear_alarm_threshold.unsignValue) {
                            ctc_oam_build_alarm_event(type, port, p_info[i].alarm_code, CTC_ALARM_CLEAR,
                                                      (oam_uint64)unsign_value);
                        }

                        break;

                    case CTC_THRESH_SIGNED_LOW:
                        if (sign_value <= p_info[i].raise_alarm_threshold.signValue) {
                            ctc_oam_build_alarm_event(type, port, p_info[i].alarm_code, CTC_ALARM_RAISE,
                                                      (oam_uint64)sign_value);
                        }
                        else if (sign_value > p_info[i].clear_alarm_threshold.signValue) {
                            ctc_oam_build_alarm_event(type, port, p_info[i].alarm_code, CTC_ALARM_CLEAR,
                                                      (oam_uint64)sign_value);
                        }

                        break;

                    case CTC_THRESH_UNSIGNED_LOW:
                        if (unsign_value <= p_info[i].raise_alarm_threshold.unsignValue) {
                            ctc_oam_build_alarm_event(type, port, p_info[i].alarm_code, CTC_ALARM_RAISE,
                                                      (oam_uint64)unsign_value);
                        }
                        else if (unsign_value > p_info[i].clear_alarm_threshold.unsignValue) {
                            ctc_oam_build_alarm_event(type, port, p_info[i].alarm_code, CTC_ALARM_CLEAR,
                                                      (oam_uint64) unsign_value);
                        }

                        break;

                    default:
                        break;
                }
            }
            else {
                /*not threshold type, then get status and info*/
                if (OAM_E_OK == ctc_oam_alarm_status_get(type, port, p_info[i].alarm_code,
                        &status, &unsign_value)) {
                    if (status == 1) {
                        ctc_oam_build_alarm_event(type, port, p_info[i].alarm_code, CTC_ALARM_RAISE,
                                                  (oam_uint64)unsign_value);
                    }
                    else {
                        ctc_oam_build_alarm_event(type, port, p_info[i].alarm_code, CTC_ALARM_CLEAR,
                                                  (oam_uint64)unsign_value);
                    }
                }
                else
                    continue;
            }
        }
    }
}

/*****************************************************************************/
/* $rtn_hdr_start  ctc_oam_alarm_polling                                     */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void ctc_oam_alarm_polling()
/*                                                                          */
/* INPUTS  :                                                                */
/*     none                                                                  */
/* OUTPUTS : ----                                                            */
/* DESCRIPTION:                                                              */
/* API to init alarm info                                                    */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint32 num = 0;
    oam_uint8 i = 0;
    ctc_oam_alarm_info_t *p_info = NULL;

    if (!ctc_oam_discovery_result_get()) {
        OAM_ORG_LOG_INFO("alarm polling wait for discovery finished\n");
        return;
    }

    /*polling pon alarm*/
    ctc_oam_get_alarm_info_table(CTC_PON_ALARM, 0, &num, &p_info);
    ctc_oam_alarm_polling_handle(CTC_PON_ALARM, 0, num, p_info);

    /*polling onu alarm*/
    ctc_oam_get_alarm_info_table(CTC_ONU_ALARM, 0, &num, &p_info);
    ctc_oam_alarm_polling_handle(CTC_ONU_ALARM, 0, num, p_info);

    /*Polling port alarm*/
    for (i = 0; i < CTC_MAX_ONU_UNI_PORTS; i++) {
        ctc_oam_get_alarm_info_table(CTC_PORT_ALARM, i + 1, &num, &p_info);
        ctc_oam_alarm_polling_handle(CTC_PORT_ALARM, i + 1, num, p_info);
    }

}

static void ctc_alarm_show(oam_uint32 num, ctc_oam_alarm_info_t *info, oam_print func, void *fd)
{
    oam_uint8 i = 0;

    OAM_PRINTF("\r\n");
    OAM_PRINTF("  alarm id     polling need    state    enable    raise thresh    clear thresh\r\n");
    OAM_PRINTF("  ------    ------------    -----    ------    ------------    ------------\r\n");

    for (i = 0; i < num; i++) {
        OAM_PRINTF("  0x%04x    %-12d    %-5d    %-6d    0x%-10x    0x%-10x\r\n",
                   info[i].alarm_code, info[i].polling_need, info[i].alarm_state,
                   info[i].enable, info[i].raise_alarm_threshold.unsignValue,
                   info[i].clear_alarm_threshold.unsignValue);
    }
}

void ctc_oam_onu_alarm_show(oam_print func, void *fd)
{
    oam_uint32 num = 0;
    ctc_oam_alarm_info_t *p_info = NULL;

    if (!ctc_oam_discovery_result_get()) {
        oam_printf("alarm polling wait for discovery finished\n");
        return;
    }

    ctc_oam_get_alarm_info_table(CTC_ONU_ALARM, 0, &num, &p_info);

    ctc_alarm_show(num, p_info, func, fd);
}

void ctc_oam_pon_alarm_show(oam_print func, void *fd)
{
    oam_uint32 num = 0;
    ctc_oam_alarm_info_t *p_info = NULL;

    if (!ctc_oam_discovery_result_get()) {
        oam_printf("alarm polling wait for discovery finished\n");
        return;
    }

    ctc_oam_get_alarm_info_table(CTC_PON_ALARM, 0, &num, &p_info);

    ctc_alarm_show(num, p_info, func, fd);
}

void ctc_oam_port_alarm_show(oam_uint32 port, oam_print func, void *fd)
{
    oam_uint32 num = 0;
    ctc_oam_alarm_info_t *p_info = NULL;

    if (port > CTC_MAX_ONU_UNI_PORTS) {
        oam_printf("Invalid port\r\n");
        return;
    }

    if (!ctc_oam_discovery_result_get()) {
        oam_printf("alarm polling wait for discovery finished\n");
        return;
    }

    ctc_oam_get_alarm_info_table(CTC_PORT_ALARM, port, &num, &p_info);

    ctc_alarm_show(num, p_info, func, fd);
}

#ifndef __X86_DEMO__
#ifdef ZTE_SPECIFIC
oam_status ctc_oam_onu_alarm_syn_db (  )
{
    oam_uint32                i = 0;
    oam_uint32                   number = 0;
    db_zte_onu_alarm_t   *db_onu_alarm = NULL;
    db_zte_alarm_entry_t     *entry = NULL;
    oam_uint32                 zte_onu_alarm_len = 0;
    oam_status                  ret = 0;

    zte_onu_alarm_len = sizeof(db_zte_onu_alarm_t);
    db_onu_alarm = (db_zte_onu_alarm_t *) OAM_MALLOC(zte_onu_alarm_len);

    if ( db_onu_alarm == NULL ) {
        ret = OAM_E_ERROR;
        goto END;
    }

    memset( db_onu_alarm, 0x00, zte_onu_alarm_len);

    number = sizeof(g_onu_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);

    for (i = 0; i < number ; i++) {
        entry = &db_onu_alarm->entry[i];
        entry->alarm_code = g_onu_alarm_cfg[i].alarm_code;
        entry->alarm_type = CTC_ONU_ALARM;
        entry->enable = g_onu_alarm_cfg[i].enable;
        entry->threshold_type = g_onu_alarm_cfg[i].threshold_type;

        if ( entry->threshold_type == CTC_THRESH_SIGNED_LOW
             || entry->threshold_type == CTC_THRESH_SIGNED_HIGH ) {
            entry->raise_threshold = g_onu_alarm_cfg[i].raise_alarm_threshold.signValue;
            entry->clear_threshold = g_onu_alarm_cfg[i].clear_alarm_threshold.signValue;
        }

        if ( entry->threshold_type == CTC_THRESH_UNSIGNED_LOW
             || entry->threshold_type == CTC_THRESH_UNSIGNED_HIGH ) {
            entry->raise_threshold = g_onu_alarm_cfg[i].raise_alarm_threshold.unsignValue;
            entry->clear_threshold = g_onu_alarm_cfg[i].clear_alarm_threshold.unsignValue;
        }
    }

    db_onu_alarm->num += number;

    if (oam_is_cuc_oam() == TRUE) {
        number = sizeof(g_cuc_pon_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);

        for (i = 0; i < number ; i++) {
            entry = &db_onu_alarm->entry[i + db_onu_alarm->num];
            entry->alarm_code = g_cuc_pon_alarm_cfg[i].alarm_code;
            entry->alarm_type = CTC_PON_ALARM;
            entry->enable = g_cuc_pon_alarm_cfg[i].enable;
            entry->threshold_type = g_cuc_pon_alarm_cfg[i].threshold_type;

            if ( entry->threshold_type == CTC_THRESH_SIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_SIGNED_HIGH ) {
                entry->raise_threshold = g_cuc_pon_alarm_cfg[i].raise_alarm_threshold.signValue;
                entry->clear_threshold = g_cuc_pon_alarm_cfg[i].clear_alarm_threshold.signValue;
            }

            if ( entry->threshold_type == CTC_THRESH_UNSIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_UNSIGNED_HIGH ) {
                entry->raise_threshold =
                    g_cuc_pon_alarm_cfg[i].raise_alarm_threshold.unsignValue;
                entry->clear_threshold =
                    g_cuc_pon_alarm_cfg[i].clear_alarm_threshold.unsignValue;
            }

        }

        db_onu_alarm->num += number;
    }
    else {

        number = sizeof(g_pon_alarm_cfg) / sizeof(ctc_oam_alarm_info_t);

        for (i = 0; i < number ; i++) {
            entry = &db_onu_alarm->entry[i + db_onu_alarm->num];
            entry->alarm_code = g_pon_alarm_cfg[i].alarm_code;
            entry->alarm_type = CTC_PON_ALARM;
            entry->enable = g_pon_alarm_cfg[i].enable;
            entry->threshold_type = g_pon_alarm_cfg[i].threshold_type;

            if ( entry->threshold_type == CTC_THRESH_SIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_SIGNED_HIGH ) {
                entry->raise_threshold = g_pon_alarm_cfg[i].raise_alarm_threshold.signValue;
                entry->clear_threshold = g_pon_alarm_cfg[i].clear_alarm_threshold.signValue;
            }

            if ( entry->threshold_type == CTC_THRESH_UNSIGNED_LOW
                 || entry->threshold_type == CTC_THRESH_UNSIGNED_HIGH ) {
                entry->raise_threshold = g_pon_alarm_cfg[i].raise_alarm_threshold.unsignValue;
                entry->clear_threshold = g_pon_alarm_cfg[i].clear_alarm_threshold.unsignValue;
            }

        }

        db_onu_alarm->num += number;
    }



    cs_db_entry_mod(TB_ZTE_ONU_ALARM , (void *) db_onu_alarm, zte_onu_alarm_len,
                    column_bit(TB_ZTE_ONU_ALARM, "ALL"));

    if (db_onu_alarm != NULL) {
        OAM_FREE(db_onu_alarm);
        db_onu_alarm = NULL;
    }
    
END:
    return ret;
}


oam_status ctc_oam_port_alarm_syn_db ( oam_port_id_t port  )
{
    oam_uint32                i = 0;
    db_zte_port_alarm_t   db_port_alarm;
    oam_uint32                 zte_port_alarm_len = 0;
    oam_boolean               cuc_flag = TRUE;
    ctc_oam_alarm_info_t *item = NULL;
    db_zte_alarm_entry_t    entry;

    zte_port_alarm_len = sizeof(db_zte_port_alarm_t);

    memset (&db_port_alarm, 0x00, zte_port_alarm_len );

    cuc_flag = oam_is_cuc_oam();
    db_port_alarm.port_id = DB_PORT_IDX;

    if ( cuc_flag == TRUE) {
        db_port_alarm.onu_entry.num = g_cuc_port_alarm_num;

        for (i = 0; i < g_cuc_port_alarm_num; i++) {
            memset(&entry, 0x00, sizeof(entry));
            //entry = &db_port_alarm->onu_entry.entry[i];
            item = &g_cuc_port_alarm_cfg[port - 1][i];
            entry.alarm_code = item->alarm_code;
            entry.alarm_type = CTC_PORT_ALARM;
            entry.enable = item->enable;
            entry.threshold_type = item->threshold_type;

            if ( entry.threshold_type == CTC_THRESH_SIGNED_LOW
                 || entry.threshold_type == CTC_THRESH_SIGNED_HIGH ) {
                entry.raise_threshold = item->raise_alarm_threshold.signValue;
                entry.clear_threshold = item->clear_alarm_threshold.signValue;
            }

            if ( entry.threshold_type == CTC_THRESH_UNSIGNED_LOW
                 || entry.threshold_type == CTC_THRESH_UNSIGNED_HIGH ) {
                entry.raise_threshold = item->raise_alarm_threshold.unsignValue;
                entry.clear_threshold = item->clear_alarm_threshold.unsignValue;
            }

            memcpy(&db_port_alarm.onu_entry.entry[i], &entry, sizeof(entry));
        }

    }
    else {

        db_port_alarm.onu_entry.num = g_port_alarm_num;

        for (i = 0; i < g_port_alarm_num; i++) {
            memset(&entry, 0x00, sizeof(entry));
            //entry = &db_port_alarm->onu_entry.entry[i];
            item = &g_port_alarm_cfg[port - 1][i];
            entry.alarm_code = item->alarm_code;
            entry.alarm_type = CTC_PORT_ALARM;
            entry.enable = item->enable;
            entry.threshold_type = item->threshold_type;

            if ( entry.threshold_type == CTC_THRESH_SIGNED_LOW
                 || entry.threshold_type == CTC_THRESH_SIGNED_HIGH ) {
                entry.raise_threshold = item->raise_alarm_threshold.signValue;
                entry.clear_threshold = item->clear_alarm_threshold.signValue;
            }

            if ( entry.threshold_type == CTC_THRESH_UNSIGNED_LOW
                 || entry.threshold_type == CTC_THRESH_UNSIGNED_HIGH ) {
                entry.raise_threshold = item->raise_alarm_threshold.unsignValue;
                entry.clear_threshold = item->clear_alarm_threshold.unsignValue;
            }

            memcpy(&db_port_alarm.onu_entry.entry[i], &entry, sizeof(entry));
        }

    }

    cs_db_entry_mod(TB_ZTE_PORT_ALARM , (void *)&db_port_alarm, zte_port_alarm_len,
                    column_bit(TB_ZTE_PORT_ALARM, "ALL"));

    return OAM_E_OK;

}
#endif/*END_ZTE_SPECIFIC*/
#endif

#endif


