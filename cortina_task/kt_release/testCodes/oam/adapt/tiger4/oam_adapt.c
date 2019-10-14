#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* osal api header include */
#include "osal_common.h"

/* oam internal header include */
#include "oam_types.h"
#include "oam_cmn.h"
#include "oam_adapt.h"
#include "oam_sm.h"
#include "oam_main.h"

/* driver & platfrom header include */
#include "cs_scfg.h"

#include "cs_aal_evt.h"
#include "cs_aal_pkt.h"
#include "cs_aal_pmac.h"
#include "cs_aal_mpcp.h"


/*****************************************************************************/
/* $rtn_hdr_start  oam_d2_compatible_get_adapt                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_boolean oam_d2_compatible_get_adapt()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : oam boolean                                                     */
/* DESCRIPTION:                                                              */
/* get d2 compatible support or not                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 oam_version = 0;

    if (cs_startup_config_read(OAM_DEV_ID, CFG_ID_OAM_VERSION, sizeof(oam_version),
                               &oam_version) != CS_E_OK) {
        OAM_LOG_MAJ("FAILED to read CFG_ID_OAM_VERSION from scfg \n");
        return OAM_FALSE;
    }

    if (0 == oam_version)
        return OAM_FALSE;
    else
        return OAM_TRUE;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_dying_gasp_num_get_adapt                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint8  oam_dying_gasp_num_get_adapt()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : dying gasp number                                               */
/* DESCRIPTION:                                                              */
/* get the dying gasp number                                                 */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    return OAM_DYING_GASP_NUM;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_dying_gasp_send_adapt                                 */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_dying_gasp_send_adapt(oam_pdu_info_t *pdu, oam_uint32 len)
/*                                                                           */
/* INPUTS  : o pdu - oam info pdu                                            */
/*           o len - oam pdu length                                          */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* send dying gasp oam packet                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_switch_port_num_get_adapt                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint8 oam_switch_port_num_get_adapt()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : port number                                                     */
/* DESCRIPTION:                                                              */
/* get the switch port number                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 num = 1;

    if (cs_startup_config_read(OAM_DEV_ID, CFG_ID_SWITCH_PORT_NUM, sizeof(num),
                               &num) != CS_E_OK) {
        OAM_LOG_MAJ("FAILED to read CFG_ID_SWITCH_PORT_NUM from scfg \n");
        num = 1;
    }

    return num;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_is_image_updating_adapt                               */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_boolean oam_is_image_updating_adapt()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : oam boolean                                                     */
/* DESCRIPTION:                                                              */
/* get image updating status                                                 */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    /* should call l2ftp api later */
    return 0;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_iop_vender_code_get_adapt                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_iop_vender_code_get_adapt (
    oam_uint8 *vcode,
    oam_uint32 len)
/*                                                                           */
/* INPUTS  : o vcode - vendor code                                           */
/*           o len - vendor code length                                      */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* get iop vendor code                                                       */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_iop_vender_code_set_adapt                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_iop_vender_code_set_adapt (
    const oam_uint8 *vcode,
    oam_uint32 len)
/*                                                                           */
/* INPUTS  : o vcode - vendor code                                           */
/*           o len   - vendor code length                                    */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* set iop vendor code                                                       */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_system_reset_adapt                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
void oam_system_reset_adapt()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS :                                                                 */
/* DESCRIPTION:                                                              */
/* reset system                                                              */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{

}

/*****************************************************************************/
/* $rtn_hdr_start  oam_llid_mac_get_adapt                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_llid_mac_get_adapt(
    oam_uint16 index,
    oam_uint8 *mac)
/*                                                                           */
/* INPUTS  : o index - llid index                                            */
/*           o mac - llid mac address                                        */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* get the mac address for the llid                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    cs_status  ret = CS_E_OK;
    cs_aal_mpcp_llid_cfg_t llid_cfg;

    memset(&llid_cfg, 0, sizeof(llid_cfg));

    ret = cs_aal_mpcp_llid_cfg_get(OAM_DEV_ID, OAM_PORT_ID, index, &llid_cfg);

    if (CS_E_OK != ret) {
        OAM_LOG_MAJ("cs_aal_mpcp_llid_cfg_get index %d failed. ret = %d\r\n",
                    index, ret);
        return OAM_E_ERROR;
    }

    memcpy(mac, &llid_cfg.mac_addr, OAM_MACADDR_LEN);

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_llid_admin_set_adapt                                  */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_llid_admin_set_adapt(
    oam_uint16 index,
    oam_boolean enable)
/*                                                                           */
/* INPUTS  : o index - onu llid index                                        */
/*           o enable - llid register enable                                 */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* set register permit for the llid                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    cs_status  ret = CS_E_OK;
    cs_uint32  mask = 0;
    cs_aal_mpcp_llid_cfg_t llid_cfg;

    memset(&llid_cfg, 0, sizeof(llid_cfg));

    mask = CS_AAL_MPCP_LLID_CFG_MASK_REG_EN;
    llid_cfg.register_permit = 1;

    ret = cs_aal_mpcp_llid_cfg_set(OAM_DEV_ID, OAM_PORT_ID, index, mask, &llid_cfg);

    if (CS_E_OK != ret) {
        OAM_LOG_MAJ("cs_aal_mpcp_llid_cfg_set index %d failed. ret = %d\r\n",
                    index, ret);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_port_lpbk_set_adapt                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_port_lpbk_set_adapt(
    oam_llid_t llid,
    oam_uint8 mux_action,
    oam_uint8 par_action)
/*                                                                           */
/* INPUTS  : o llid - onu llid                                               */
/*           o mux_action - mux action                                       */
/*           o par_action - par action                                       */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* set the loopback to the sdl                                               */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_buf_int_status_get_adapt                              */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_buf_int_status_get_adapt(oam_boolean *status)
/*                                                                           */
/* INPUTS  : o status - onu buf int status                                   */
/* OUTPUTS : status                                                          */
/* DESCRIPTION:                                                              */
/* get the bm status                                                         */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_enc_mode_get_adapt                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_enc_mode_get_adapt(oam_uint8 *mode)
/*                                                                           */
/* INPUTS  : o mode - onu encrypt mode                                       */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* get the onu encrypt mode from the sdl                                     */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_enc_mode_set_adapt                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_enc_mode_set_adapt(oam_uint8 mode)
/*                                                                           */
/* INPUTS  : o mode - onu encrypt mode                                       */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* set the onu encrypt mode from the sdl                                     */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status  ret = OAM_E_OK;

    return ret;
}


/*****************************************************************************/
/* $rtn_hdr_start  oam_pon_stats_get_adapt                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_pon_stats_get_adapt(oam_pon_stats_t *stats)
/*                                                                           */
/* INPUTS  : o stats - pon statistics                                        */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* get the onu pon statistics                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_pon_stats_get_adapt                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_pon_stats_summary_get_adapt(
    oam_uint64 *frames,
    oam_uint64 *errors,
    oam_uint64 *err_symb)
/*                                                                           */
/* INPUTS  : o frames - total frames                                         */
/*           o errors - error frames                                         */
/*           o err_symb - error symbols                                      */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* get the onu pon statistics summary                                        */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

static void oam_evt_handler(cs_dev_id_t dev_id, cs_int32 event_type, void *msg,
                            cs_int32 len)
{
    OAM_ASSERT(msg != NULL);

    OAM_LOG_DEBUG("dev %d evt %d\r\n", dev_id, event_type);

    switch (event_type) {
        case CS_AAL_EVT_REG_CHANGE: {
            oam_onu_reg_evt_t reg_evt;
            cs_aal_evt_reg_t *event_reg = (cs_aal_evt_reg_t *)msg;

            memset(&reg_evt, 0, sizeof(reg_evt));

            reg_evt.index = event_reg->index;
            reg_evt.status = event_reg->reg ? OAM_TRUE : OAM_FALSE;

            oam_llid_reg_evt_msg_send(event_reg->llid, &reg_evt);
            break;
        }

        default:
            OAM_LOG_MAJ("Invalid event %d\r\n", event_type);
            break;
    }

}

static void oam_pkt_handler(cs_dev_id_t dev_id, cs_aal_pkt_t *pPkt)
{
    OAM_ASSERT(pPkt != NULL);

    oam_pkt_msg_send(pPkt->port, pPkt->llid, pPkt->data + pPkt->offset, pPkt->len);
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_evt_reg_adapt                                         */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_evt_reg_adapt()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* register event                                                            */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    cs_status  ret = CS_E_OK;

    ret = cs_evt_reg(OAM_DEV_ID, CS_AAL_EVT_REG_CHANGE, oam_evt_handler);

    if (ret != CS_E_OK) {
        OAM_LOG_MAJ("Register CS_AAL_EVT_REG_CHANGE failed\r\n");
        return OAM_E_ERROR;
    }

    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_pkt_trap_adapt                                        */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_pkt_trap_adapt()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* register oam packet trap                                                  */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    cs_status  ret = CS_E_OK;

    ret = cs_aal_pkt_rx_reg(OAM_DEV_ID, CS_PKT_TYPE_EFM_OAM, oam_pkt_handler);

    if (ret != CS_E_OK) {
        OAM_LOG_MAJ("Register pkt rx CS_PKT_TYPE_EFM_OAM failed\r\n");
        return OAM_E_ERROR;
    }

    return ret;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_pkt_send_adapt                                        */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_pkt_send_adapt(
    oam_port_id_t port,
    oam_llid_t llid,
    oam_uint8 *buf,
    oam_uint32 len)
/*                                                                           */
/* INPUTS  : o port - port id                                                */
/*           o llid - llid                                                   */
/*           o buf  - packet buffer                                          */
/*           o len  - packet length                                          */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* send oam packet                                                           */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    cs_status ret = CS_E_OK;
    cs_aal_pkt_t *pkt = NULL;
    oam_uint32 total_len = 0;
    oam_uint32 pkt_len = 0;
    oam_uint8 repeat = 0;

    /* change len to uint16 */
    len &= 0xFFFF;

    /* + 4 should be removed later accroding to driver's change */
    if (len < EPON_TX_MIN_FRM_LEN + 4)
        pkt_len = EPON_TX_MIN_FRM_LEN + 4;
    else
        pkt_len = len;

    total_len = sizeof(cs_aal_pkt_t) + CS_PKT_OFFSET + pkt_len;

    pkt = OAM_MALLOC(total_len);

    if (NULL == pkt) {
        OAM_LOG_CRI("malloc memory for oam pkt failed.\r\n");
        return OAM_E_RESOURCE;
    }

    OAM_LOG_DEBUG("OAM pkt send for llid %d\r\n", llid);

    memset(pkt, 0, total_len);

    pkt->llid = llid;
    pkt->port = OAM_PON_PORT_ID;
    pkt->offset = CS_PKT_OFFSET;
    pkt->len = pkt_len;

    /* workaround for single cos fifo full */
    {
        oam_if_t *intf = oam_intf_find(llid);

        if (intf != NULL && intf->valid) {
            pkt->cos = intf->index % 8;
            OAM_LOG_DEBUG("OAM pkt send for llid %d cos %d\r\n", llid, pkt->cos);
        }
    }

    /* copy the significative bytes from buff */
    memcpy(pkt->data + pkt->offset, buf, len);

    do {
        ret = cs_aal_pkt_tx(OAM_DEV_ID, pkt);

        if (ret != CS_E_OK) {
            OAM_LOG_MAJ("OAM packet tx failed. ret=%d\r\n", ret);
            OAM_FREE(pkt);
            return OAM_E_ERROR;
        }

        repeat++;

        usleep(1);

        if (repeat >= 5)
            break;
    }
    while (CS_E_CONFLICT == ret);

    OAM_FREE(pkt);

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_ext_mode_get_adapt                                    */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_ext_mode_t oam_ext_mode_get_adapt()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : externed oam mode                                               */
/* DESCRIPTION:                                                              */
/* get externed oam mode                                                     */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_uint8 mode = 0;
    oam_ext_mode_t ext_mode;

    if (cs_startup_config_read(OAM_DEV_ID, CFG_ID_EXT_OAM_MODE, sizeof(mode),
                               &mode) != CS_E_OK) {
        OAM_LOG_MAJ("FAILED to read CFG_ID_EXT_OAM_MODE from scfg \n");
        mode = 0;
    }

    switch (mode) {
        case 0:
            ext_mode = OAM_EXT_MODE_CTC;
            break;

        case 1:
            ext_mode = OAM_EXT_MODE_DPOE;
            break;

        default:
            ext_mode = OAM_EXT_MODE_CTC;
            break;
    }

    return ext_mode;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_pon_admin_set_adapt                                   */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_pon_admin_set_adapt(
    oam_port_id_t port,
    oam_boolean enable)
/*                                                                           */
/* INPUTS  : o port   - port id                                              */
/*           o enable - admin status                                         */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* set pon port admin status                                                 */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    cs_status ret = CS_E_OK;
    cs_aal_pmac_laser_cfg_t laser_cfg;
    cs_uint32 mask = 0;

    memset(&laser_cfg, 0, sizeof(laser_cfg));

    if (enable)
        laser_cfg.tx_power = 1;
    else
        laser_cfg.tx_power = 0;

    mask = 0x02;
    ret = cs_aal_pmac_laser_set(OAM_DEV_ID, port, mask, &laser_cfg);

    if (ret != 0) {
        OAM_LOG_MAJ("cs_aal_pmac_laser_set return error. ret(%d)\n", ret);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_pon_led_set_adapt                                     */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_pon_led_set_adapt(oam_uint8 action)
/*                                                                           */
/* INPUTS  : o action - led action                                           */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* set the onu pon led status                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status  ret = OAM_E_OK;

    return ret;
}

