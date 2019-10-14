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
#include "oam_ctc_client.h"
#include "oam_ctc_adapt.h"
#include "oam_ctc_swimage.h"

#ifdef HAVE_CTC_OAM

static oam_ctc_img_ctrl_t  g_ctc_oam_img_ctrl;

static oam_int8 g_ctc_oam_img_err_msg[TFTP_ERROR_MAX + 1][64] = {
    "",
    "File not found",
    "File write error",
    "Disk is full",
    "Illegal operation",
    "Unknow transaction id",
    "File already exist",
    "No such user",
    ""
};


void oam_ctc_swimage_ctrl_reset()
{
    g_ctc_oam_img_ctrl.state = OAM_CTC_IMG_INIT;

    if (g_ctc_oam_img_ctrl.recv_timer != 0) {
        ca_timer_del(g_ctc_oam_img_ctrl.recv_timer);
        g_ctc_oam_img_ctrl.recv_timer = 0;
    }

    g_ctc_oam_img_ctrl.img_size = 0;
    g_ctc_oam_img_ctrl.exp_block = 1;
}

void oam_ctc_swimage_write_thread(void *data)
{
    oam_status ret = OAM_E_OK;

    while (1) {
        ca_sem_wait(g_ctc_oam_img_ctrl.sem_id, -1);

        ctc_oam_swimage_image_ready(g_ctc_oam_img_ctrl.img_id);
        ret = ctc_oam_swimage_close_adapt(g_ctc_oam_img_ctrl.img_id);

        ca_mutex_lock(g_ctc_oam_img_ctrl.mutex);

        if (ret != OAM_E_OK) {
            g_ctc_oam_img_ctrl.state = OAM_CTC_IMG_WRITE_FAILED;
        }
        else {
            g_ctc_oam_img_ctrl.state = OAM_CTC_IMG_WRITE_COMPLETED;
        }

        ca_mutex_unlock(g_ctc_oam_img_ctrl.mutex);
    }
}


void oam_ctc_swimage_init()
{
    oam_uint32 thread_id;

    g_ctc_oam_img_ctrl.recv_timer = 0;

    if (0 != ca_mutex_init(&g_ctc_oam_img_ctrl.mutex, "ctc_swimage", 0)) {
        OAM_ORG_LOG("create the mutex failed \n");
        return;
    }

    if (0 != ca_sem_init(&g_ctc_oam_img_ctrl.sem_id, "ctc_swimage", 0, 0)) {
        OAM_ORG_LOG("create the sem failed \n");
        return;
    }

    g_ctc_oam_img_ctrl.img_id = ctc_oam_swimage_id_get_adapt();

    g_ctc_oam_img_ctrl.flash_state = ctc_oam_swimage_flash_state_get_adapt();
    oam_ctc_swimage_ctrl_reset();

    ca_thread_create(&thread_id,
                     "ctc_swimage",
                     oam_ctc_swimage_write_thread,
                     NULL,
                     20480,
                     30,
                     0);
}

void ctc_oam_swimage_header_build(
    oam_uint8 *pdu,
    oam_uint8  type,
    oam_uint32 llid,
    oam_uint16 opcode)
{
    oam_ctc_swimg_hdr_t *p_header = (oam_ctc_swimg_hdr_t *) (pdu +  sizeof(
                                        oam_pdu_ctc_t) - 1);

    ctc_oam_build_header(oam_oui_ctc, llid, pdu, OAM_PDU_CTC_FILE_IMG);

    p_header->type = type;
    p_header->tid = htons(llid);
    p_header->opcode = htons(opcode);
    p_header->length = sizeof(oam_ctc_swimg_hdr_t);
}

void ctc_oam_swimage_recv_timeout(void *data)
{
    ca_mutex_lock(g_ctc_oam_img_ctrl.mutex);
    oam_ctc_swimage_ctrl_reset();
    g_ctc_oam_img_ctrl.recv_timer = 0;

    ctc_oam_swimage_close_adapt(g_ctc_oam_img_ctrl.img_id);
    ca_mutex_unlock(g_ctc_oam_img_ctrl.mutex);
}


void ctc_oam_swimage_err_rsp(
    oam_uint32 llid,
    oam_uint8 type,
    oam_uint16  error_code)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE] = {0};
    oam_ctc_swimg_err_t *err = (oam_ctc_swimg_err_t *)(pdu + sizeof(
                                   oam_pdu_ctc_t) - 1);
    oam_uint16 len = sizeof(oam_pdu_ctc_t) - 1;

    OAM_ASSERT(error_code < TFTP_ERROR_MAX);

    ctc_oam_swimage_header_build(pdu, type, llid, CTC_OAM_ONU_UPGRADE_ERROR);

    err->error = htons(error_code);
    strcpy((oam_int8 *)err->errmsg, (oam_int8 *)g_ctc_oam_img_err_msg[error_code]);
    err->hdr.length = sizeof(oam_ctc_swimg_err_t) - 1 + strlen((
                          oam_int8 *)g_ctc_oam_img_err_msg[error_code]) + 1;

    len += err->hdr.length;
    err->hdr.length = htons(err->hdr.length);
    ctc_oam_send(llid, pdu, len);

    cap_log_msg(0,CA_MOD_ALARM_APP, CAP_LOG_CRITICAL, 0, "104058:upgrading failed\n");
}


void ctc_oam_swimage_write_request(
    oam_uint32 llid,
    oam_ctc_swimg_cmn_t *data)
{
    oam_int8 *file = NULL;
    oam_int8 *mode = NULL;

    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE] = {0};
    oam_ctc_swimg_ack_t *ack = (oam_ctc_swimg_ack_t *)(pdu + sizeof(
                                   oam_pdu_ctc_t) - 1);
    oam_uint16 len = sizeof(oam_pdu_ctc_t) - 1;
    oam_status ret = OAM_E_OK;

    OAM_ASSERT(data != NULL);


    file = (oam_int8 *)(&data->data);

    mode = (oam_int8 *)(&data->data + 1 +  strlen(file));

    OAM_ORG_DEBUG("file name = %s mode = %s \n", file, mode);

    ca_mutex_lock(g_ctc_oam_img_ctrl.mutex);

    if (strncmp(mode, "Octet", strlen("Octet")) != 0) {
        OAM_ORG_LOG("invalid mode %s \n", mode);
        ctc_oam_swimage_err_rsp(llid, TFTP_DATA, TFTP_ERROR_ILLEGAL_OPERTION);
        goto Error_Exit;
    }

    if (g_ctc_oam_img_ctrl.state == OAM_CTC_IMG_DOWNLOADING
        || g_ctc_oam_img_ctrl.state == OAM_CTC_IMG_WRITING) {
        OAM_ORG_LOG("image uprgade was not completed, state = %d \n",
                    g_ctc_oam_img_ctrl.state);
        ctc_oam_swimage_err_rsp(llid, TFTP_DATA, TFTP_ERROR_ILLEGAL_OPERTION);
        goto Error_Exit;
    }

    if (g_ctc_oam_img_ctrl.flash_state == OAM_CTC_FLASH_IMG0_NA_C
        || g_ctc_oam_img_ctrl.flash_state == OAM_CTC_FLASH_IMG0_A_NC) {
        OAM_ORG_LOG("image uprgade failed, wrong flash state = %d \n",
                    g_ctc_oam_img_ctrl.flash_state);
        ctc_oam_swimage_err_rsp(llid, TFTP_DATA, TFTP_ERROR_ILLEGAL_OPERTION);
        goto Error_Exit;
    }

    ctc_oam_swimage_header_build(pdu, TFTP_DATA, llid,
                                 CTC_OAM_ONU_UPGRADE_FILE_TRANSFER_ACK);
    ack->block = 0;

    ret = ctc_oam_swimage_open_adapt(g_ctc_oam_img_ctrl.img_id);

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("open the image failed ret = %d \n", ret);
        ctc_oam_swimage_err_rsp(llid, TFTP_DATA, TFTP_ERROR_FILE_EXIST);
        goto Error_Exit;
    }

    oam_ctc_swimage_ctrl_reset();
    g_ctc_oam_img_ctrl.state = OAM_CTC_IMG_DOWNLOADING;

    g_ctc_oam_img_ctrl.recv_timer = ca_timer_add(
                                        OAM_CTC_IMG_RECV_TIMEOUT,
                                        ctc_oam_swimage_recv_timeout,
                                        0);

    oam_printf("Image upgrade start \n");

    ca_mutex_unlock(g_ctc_oam_img_ctrl.mutex);
    ack->hdr.length = sizeof(oam_ctc_swimg_ack_t);
    len += ack->hdr.length;
    ack->hdr.length = htons(ack->hdr.length);
    ctc_oam_send(llid, pdu, len);
    return ;
Error_Exit:
    ca_mutex_unlock(g_ctc_oam_img_ctrl.mutex);

}


void ctc_oam_swimage_write(
    oam_uint32 llid,
    oam_ctc_swimg_data_t *data)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE] = {0};
    oam_ctc_swimg_ack_t *ack = (oam_ctc_swimg_ack_t *)(pdu + sizeof(
                                   oam_pdu_ctc_t) - 1);
    oam_uint16 len = sizeof(oam_pdu_ctc_t) - 1;
    oam_status ret = OAM_E_OK;

    OAM_ASSERT(data != NULL);

    ctc_oam_swimage_header_build(pdu, TFTP_DATA, llid,
                                 CTC_OAM_ONU_UPGRADE_FILE_TRANSFER_ACK);

    ca_mutex_lock(g_ctc_oam_img_ctrl.mutex);

    if (g_ctc_oam_img_ctrl.state != OAM_CTC_IMG_DOWNLOADING) {
        OAM_ORG_LOG("image uprgade was not start download, state = %d \n",
                    g_ctc_oam_img_ctrl.state);
        ctc_oam_swimage_err_rsp(llid, TFTP_DATA, TFTP_ERROR_ILLEGAL_OPERTION);
        goto Error_Exit;
    }

    if (ntohs(data->block) != 0
        && ntohs(data->block) < g_ctc_oam_img_ctrl.exp_block) {
        /* duplicate block, send ack */
        OAM_ORG_LOG("duplicate blocked %d recv \n",
                    ntohs(data->block));
        ack->block = data->block;
        goto Exit;
    }

    if (ntohs(data->block) != g_ctc_oam_img_ctrl.exp_block) {
        OAM_ORG_LOG("wrong block %d recv,expect recv block %d \n",
                    ntohs(data->block),
                    g_ctc_oam_img_ctrl.exp_block
                   );
        ctc_oam_swimage_err_rsp(llid, TFTP_DATA, TFTP_ERROR_UNKNOWN_TRANS_ID);
        goto Error_Exit;
    }

    if (g_ctc_oam_img_ctrl.recv_timer != 0) {
        ca_timer_del(g_ctc_oam_img_ctrl.recv_timer);
        g_ctc_oam_img_ctrl.recv_timer = 0;
    }

    if (g_ctc_oam_img_ctrl.exp_block % 16 == 0) {
        oam_printf("\n");
    }

    oam_printf(".");

    ret = ctc_oam_swimage_write_adapt(
              g_ctc_oam_img_ctrl.img_id,
              data->data,
              ntohs(data->hdr.length) - sizeof(oam_ctc_swimg_data_t) + 1);

    if (OAM_E_OK != ret ) {
        OAM_ORG_LOG("write image failed, block %d ret = %d \n",
                    g_ctc_oam_img_ctrl.exp_block, ret);
        ctc_oam_swimage_err_rsp(llid, TFTP_DATA, TFTP_ERROR_ACCESS_VIOLATION);
        ctc_oam_swimage_close_adapt(g_ctc_oam_img_ctrl.img_id);
        goto Error_Exit;
    }

    g_ctc_oam_img_ctrl.img_size += ntohs(data->hdr.length) - sizeof(
                                       oam_ctc_swimg_data_t) + 1;

    g_ctc_oam_img_ctrl.exp_block ++;


    g_ctc_oam_img_ctrl.recv_timer = ca_timer_add(
                                        OAM_CTC_IMG_RECV_TIMEOUT,
                                        ctc_oam_swimage_recv_timeout,
                                        0);

    ack->block = data->block;

Exit:
    ca_mutex_unlock(g_ctc_oam_img_ctrl.mutex);
    ack->hdr.length = sizeof(oam_ctc_swimg_ack_t);
    len += ack->hdr.length;
    ack->hdr.length = htons(ack->hdr.length);

    ctc_oam_send(llid, pdu, len);
    return;
Error_Exit:
    ca_mutex_unlock(g_ctc_oam_img_ctrl.mutex);
}


void ctc_oam_swimage_end_download(
    oam_uint32 llid,
    oam_ctc_swimg_end_t *end)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE] = {0};
    oam_ctc_swimg_cmn_t *out = (oam_ctc_swimg_cmn_t *)(pdu + sizeof(
                                   oam_pdu_ctc_t) - 1);
    oam_uint16 len = sizeof(oam_pdu_ctc_t) - 1;
    oam_uint32 img_size = 0 ;

    OAM_ASSERT(end != NULL);

    img_size = ntohl(end->size);

    ctc_oam_swimage_header_build(pdu, CRC_CHECK, llid,
                                 CTC_OAM_ONU_UPGRADE_END_DOWNLOAD_RSP);

    ca_mutex_lock(g_ctc_oam_img_ctrl.mutex);

    if (img_size != g_ctc_oam_img_ctrl.img_size) {
        OAM_ORG_LOG("image size %d not match, recv size %d  \n",
                    img_size, g_ctc_oam_img_ctrl.img_size);
        out->data = END_RSP_ACK_CRC_ERROR;
        goto Exit;
    }


    switch (g_ctc_oam_img_ctrl.state) {
        case OAM_CTC_IMG_DOWNLOADING:
            if (g_ctc_oam_img_ctrl.recv_timer != 0) {
                ca_timer_del(g_ctc_oam_img_ctrl.recv_timer);
                g_ctc_oam_img_ctrl.recv_timer = 0;
            }

            OAM_ORG_LOG("starting write data to flash \n");
            g_ctc_oam_img_ctrl.state = OAM_CTC_IMG_WRITING;
            ca_sem_post(g_ctc_oam_img_ctrl.sem_id);
            out->data = END_RSP_ACK_WRITE;
            goto Exit;

        case OAM_CTC_IMG_WRITING:
            OAM_ORG_LOG("image writing\n");
            out->data = END_RSP_ACK_WRITE;
            goto Exit;

        case OAM_CTC_IMG_WRITE_FAILED:
            OAM_ORG_LOG("image write failed \n");
            out->data = END_RSP_ACK_CRC_ERROR;
            goto Exit;

        case OAM_CTC_IMG_WRITE_COMPLETED:
            out->data = END_RSP_ACK_OK;
            goto Exit;

        default:
            OAM_ORG_LOG("unkown download state %d \n", g_ctc_oam_img_ctrl.state);
            out->data = END_RSP_ACK_NOT_SUPP;
            goto Exit;
    }

Exit:
    ca_mutex_unlock(g_ctc_oam_img_ctrl.mutex);
    out->hdr.length = sizeof(oam_ctc_swimg_cmn_t);
    len += out->hdr.length;
    out->hdr.length = htons(out->hdr.length);

    ctc_oam_send(llid, pdu, len);
}


void ctc_oam_swimage_reboot_onu(void *data)
{
    ctc_oam_onu_reset_set_adapt();
}

void ctc_oam_swimage_active(
    oam_uint32 llid,
    oam_ctc_swimg_cmn_t *in)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE] = {0};
    oam_ctc_swimg_cmn_t *out = (oam_ctc_swimg_cmn_t *)(pdu + sizeof(
                                   oam_pdu_ctc_t) - 1);
    oam_uint16 len = sizeof(oam_pdu_ctc_t) - 1;
    oam_status ret = OAM_E_OK;

    OAM_ASSERT(in != NULL);

    ctc_oam_swimage_header_build(pdu, IMAGE_ACTIVE, llid,
                                 CTC_OAM_ONU_UPGRADE_ACTIVATE_IMG_RSP);

    if (g_ctc_oam_img_ctrl.state == OAM_CTC_IMG_DOWNLOADING
        || g_ctc_oam_img_ctrl.state == OAM_CTC_IMG_WRITING) {
        OAM_ORG_LOG("Image active in wronge state %d \n",
                    g_ctc_oam_img_ctrl.state);
        out->data = ACTIVE_RSP_ACK_ERROR;
        goto Exit;
    }

    if (g_ctc_oam_img_ctrl.flash_state == OAM_CTC_FLASH_IMG0_NA_C
        || g_ctc_oam_img_ctrl.flash_state == OAM_CTC_FLASH_IMG0_A_NC) {
        OAM_ORG_LOG("Image active in wronge flash state %d \n",
                    g_ctc_oam_img_ctrl.flash_state);
        out->data = ACTIVE_RSP_ACK_ERROR;
        goto Exit;
    }

    if (in->data != 0) {
        OAM_ORG_LOG("active flag %d was not 0 \n",
                    in->data);
        out->data = ACTIVE_RSP_ACK_PARA_ERROR;
        goto Exit;
    }

    ret = ctc_oam_swimage_active_adapt();

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("active failed , ret = %d \n", ret);
        out->data = ACTIVE_RSP_ACK_ERROR;
        goto Exit;
    }

    ca_timer_add(1 * 1000, ctc_oam_swimage_reboot_onu, 0);

    out->data = ACTIVE_RSP_ACK_OK;
Exit:
    out->hdr.length = sizeof(oam_ctc_swimg_cmn_t);
    len += out->hdr.length;
    out->hdr.length = htons(out->hdr.length);

    ctc_oam_send(llid, pdu, len);
}


void ctc_oam_swimage_commit(
    oam_uint32 llid,
    oam_ctc_swimg_cmn_t *in
)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE] = {0};
    oam_ctc_swimg_cmn_t *out = (oam_ctc_swimg_cmn_t *)(pdu + sizeof(
                                   oam_pdu_ctc_t) - 1);
    oam_uint16 len = sizeof(oam_pdu_ctc_t) - 1;
    oam_status ret = OAM_E_OK;

    OAM_ASSERT(in != NULL);

    ctc_oam_swimage_header_build(pdu, IMAGE_COMMIT, llid,
                                 CTC_OAM_ONU_UPGRADE_COMMIT_IMG_RSP);

    if (g_ctc_oam_img_ctrl.state == OAM_CTC_IMG_DOWNLOADING
        || g_ctc_oam_img_ctrl.state == OAM_CTC_IMG_WRITING) {
        OAM_ORG_LOG("Image commit in wronge state %d \n",
                    g_ctc_oam_img_ctrl.state);
        out->data = ACTIVE_RSP_ACK_ERROR;
        goto Exit;
    }

    if (in->data != 0) {
        OAM_ORG_LOG("commit flag %d was not 0 \n",
                    in->data);
        out->data = COMMIT_RSP_ACK_PARA_ERROR;
        goto Exit;
    }

    ret = ctc_oam_swimage_commit_adapt();

    if (ret != OAM_E_OK) {
        OAM_ORG_LOG("active failed , ret = %d \n", ret);
        out->data = COMMIT_RSP_ACK_ERROR;
        goto Exit;
    }

    g_ctc_oam_img_ctrl.flash_state = ctc_oam_swimage_flash_state_get_adapt();

    out->data = COMMIT_RSP_ACK_OK;
Exit:
    out->hdr.length = sizeof(oam_ctc_swimg_cmn_t);
    len += out->hdr.length;
    out->hdr.length = htons(out->hdr.length);

    ctc_oam_send(llid, pdu, len);
}


void ctc_oam_build_swimage(
    oam_uint32 llid,
    oam_uint8 *frame,
    oam_uint32 length)
{
    oam_pdu_ctc_t *ctc = (oam_pdu_ctc_t *)frame;
    oam_ctc_swimg_hdr_t *p_header = NULL;
    oam_ctc_swimg_data_t *data = NULL;
    oam_ctc_swimg_end_t *end = NULL;
    oam_ctc_swimg_cmn_t   *cmn = NULL;

    oam_uint32  proc_len = 0;

    OAM_ASSERT(frame != NULL
               && length > sizeof(oam_pdu_ctc_t) - 1
               + sizeof(oam_ctc_swimg_hdr_t));

    proc_len = sizeof(oam_pdu_ctc_t) - 1;

    p_header = (oam_ctc_swimg_hdr_t *) ctc->data;

    if (length < proc_len + ntohs(p_header->length)) {
        OAM_ORG_LOG("packet wrong length %d, expect length %d \n",
                    length, proc_len + ntohs(p_header->length));
        return ;
    }

    if (ntohs(p_header->tid) != llid) {
        OAM_ORG_LOG("packet tid 0x%x wrong, expect 0x%x \n",
                    ntohs(p_header->tid), llid);
        ctc_oam_swimage_err_rsp(
            llid, p_header->type,
            TFTP_ERROR_UNKNOWN_TRANS_ID);
        return;
    }


    switch (ntohs(p_header->opcode)) {
        case CTC_OAM_ONU_UPGRADE_FILE_WRITE_REQ:
            cmn = (oam_ctc_swimg_cmn_t *)ctc->data;
            ctc_oam_swimage_write_request(llid, cmn);
            break;

        case CTC_OAM_ONU_UPGRADE_FILE_TRANSFER_DATA:
            data = (oam_ctc_swimg_data_t *)ctc->data;
            ctc_oam_swimage_write(llid, data);
            break;

        case CTC_OAM_ONU_UPGRADE_END_DOWNLOAD_REQ:
            end = (oam_ctc_swimg_end_t *)ctc->data;
            ctc_oam_swimage_end_download(llid, end);
            break;

        case CTC_OAM_ONU_UPGRADE_ACTIVATE_IMG_REQ:
            cmn = (oam_ctc_swimg_cmn_t *)ctc->data;
            ctc_oam_swimage_active(llid, cmn);
            break;

        case CTC_OAM_ONU_UPGRADE_COMMIT_IMG_REQ:
            cmn = (oam_ctc_swimg_cmn_t *)ctc->data;
            ctc_oam_swimage_commit(llid, cmn);
            break;

        default:
            OAM_ORG_LOG("invalid opcode %d \n", ntohs(p_header->opcode));
            ctc_oam_swimage_err_rsp(
                llid, p_header->type,
                TFTP_ERROR_ILLEGAL_OPERTION);
            break;
    }

}

#endif
