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

Copyright (c) 2013 by Cortina Systems Incorporated
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oam_types.h"
#include "oam_cmn.h"
#include "oam_std_pdu.h"

#include "cs_scfg.h"
#include "cs_aal_mpcp.h"

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_onu_id_get_adapt                                 */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status dpoe_oam_onu_id_get_adapt(oam_uint8 *onu_id)
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : o onu_id - ONU ID                                               */
/* DESCRIPTION:                                                              */
/* get ONU ID                                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    cs_status  ret = CS_E_OK;
    cs_aal_mpcp_llid_cfg_t llid_cfg;

    OAM_ASSERT_RET(onu_id != NULL, OAM_E_PARAM);

    memset(&llid_cfg, 0, sizeof(llid_cfg));

    ret = cs_aal_mpcp_llid_cfg_get(OAM_DEV_ID, OAM_PORT_ID, OAM_BASE_LLID_INDEX,
                                   &llid_cfg);

    if (CS_E_OK != ret) {
        OAM_LOG_MAJ("cs_aal_mpcp_llid_cfg_get 0 failed. ret = %d\r\n",
                    ret);
        return OAM_E_ERROR;
    }

    memcpy(onu_id, &llid_cfg.mac_addr, OAM_MACADDR_LEN);

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_max_llid_get_adapt                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status dpoe_oam_max_llid_get_adapt(
    oam_uint16 *bidir,
    oam_uint16 *ds_only)
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : o bidir   - bidirection LLID number                             */
/*           o ds_only - downstream only LLID number                         */
/* DESCRIPTION:                                                              */
/* get ONU ID                                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 mode = 0;

    OAM_ASSERT_RET(bidir   != NULL, OAM_E_PARAM);
    OAM_ASSERT_RET(ds_only != NULL, OAM_E_PARAM);

    if (cs_startup_config_read(OAM_DEV_ID, CFG_ID_PON_LLID_MODE, sizeof(mode),
                               &mode) != CS_E_OK) {
        OAM_LOG_MAJ("FAILED to read CFG_ID_PON_LLID_MODE from scfg \n");
        mode = 0;
    }

    if (0 == mode) {
        *bidir = 1;
    }
    else {
        *bidir = 8;
    }

    *ds_only = 0;

    return OAM_E_OK;
}

#define DPOE_T4_QUEUESET_NUM_MAX    4
#define DPOE_T4_VALUE_PER_SET_MAX   1

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_report_thresh_get_adapt                          */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status dpoe_oam_report_thresh_get_adapt(
    oam_uint8 index,
    oam_uint8 *queueset_num,
    oam_uint8 *value_per_set,
    oam_uint16 *report_thresh)
/*                                                                           */
/* INPUTS  : o index         - 0 based llid index                            */
/* OUTPUTS : o queueset_num  - queue set number                              */
/*           o value_per_set - queue number per set                          */
/*           o report_thresh - report threshold                              */
/* DESCRIPTION:                                                              */
/* get ONU ID                                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    cs_status ret = CS_E_OK;
    cs_uint16 threshold = 0;
    cs_uint16 i = 0;
    cs_uint16 *pos = report_thresh;

    OAM_ASSERT_RET(queueset_num  != NULL, OAM_E_PARAM);
    OAM_ASSERT_RET(value_per_set != NULL, OAM_E_PARAM);
    OAM_ASSERT_RET(report_thresh != NULL, OAM_E_PARAM);

    *queueset_num  = DPOE_T4_QUEUESET_NUM_MAX;
    *value_per_set = DPOE_T4_VALUE_PER_SET_MAX;

    for (i = 0; i < DPOE_T4_QUEUESET_NUM_MAX; i++) {
        ret = cs_aal_mpcp_report_threshold_get(OAM_DEV_ID, OAM_PORT_ID, i, index,
                                               &threshold);

        if (ret != CS_E_OK) {
            OAM_LOG_MAJ("cs_aal_mpcp_report_threshold_get queueset %d index %d failed. ret = %d\r\n",
                        i, index, ret);
            return OAM_E_ERROR;
        }

        *pos = threshold;
        pos++;
    }

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  dpoe_oam_report_thresh_set_adapt                          */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status dpoe_oam_report_thresh_set_adapt(
    oam_uint8 index,
    oam_uint8 queueset_num,
    oam_uint8 value_per_set,
    oam_uint16 *report_thresh)
/*                                                                           */
/* INPUTS  : o index         - 0 based llid index                            */
/*           o queueset_num  - queue set number                              */
/*           o value_per_set - queue number per set                          */
/*           o report_thresh - report threshold                              */
/* OUTPUTS :                                                                 */
/* DESCRIPTION:                                                              */
/* get ONU ID                                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    cs_status ret = CS_E_OK;
    cs_uint16 i = 0;
    cs_uint16 *pos = report_thresh;

    OAM_ASSERT_RET(report_thresh != NULL, OAM_E_PARAM);

    /* Parameters check */
    if (queueset_num > DPOE_T4_QUEUESET_NUM_MAX)
        return OAM_E_NOT_SUPPORT;

    if (value_per_set > DPOE_T4_VALUE_PER_SET_MAX)
        return OAM_E_NOT_SUPPORT;

    for (i = 0; i < queueset_num; i++) {
        ret = cs_aal_mpcp_report_threshold_set(OAM_DEV_ID, OAM_PORT_ID, i, index, *pos);

        if (ret != CS_E_OK) {
            OAM_LOG_MAJ("cs_aal_mpcp_report_threshold_get queueset %d index %d failed. ret = %d\r\n",
                        i, index, ret);
            return OAM_E_ERROR;
        }

        pos++;
    }

    return OAM_E_OK;
}

