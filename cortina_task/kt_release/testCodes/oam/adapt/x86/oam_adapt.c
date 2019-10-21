#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "osal_common.h"
#include "oam_types.h"
#include "oam_adapt.h"
#include "oam_cmn.h"
#include "oam_main.h"


oam_status oam_pkt_msg_send_2_olt(oam_dev_id_t dev, oam_uint8 pon,
                                  oam_llid_t llid, oam_uint8 *pkt, oam_uint32 len);


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
    return OAM_FALSE;
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
    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_switch_port_num_get_adapt                             */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_uint16 oam_switch_port_num_get_adapt()
/*                                                                           */
/* INPUTS  :                                                                 */
/* OUTPUTS : port number                                                     */
/* DESCRIPTION:                                                              */
/* get the switch port number                                                */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    return OAM_SWITCH_PORT_NUM;
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
    OAM_ASSERT_RET(vcode != NULL, OAM_E_PARAM);

    return OAM_E_OK;
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
    OAM_ASSERT_RET(vcode != NULL, OAM_E_PARAM);

    return OAM_E_OK;
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
    oam_llid_t llid,
    oam_uint8 *mac)
/*                                                                           */
/* INPUTS  : o llid - onu llid                                               */
/*           o mac - llid mac address                                        */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* get the mac address for the llid                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status ret = OAM_E_OK;

    OAM_ASSERT_RET(mac != NULL, OAM_E_PARAM);

    mac[0] = 0x00;
    mac[1] = 0x13;
    mac[2] = 0x25;
    mac[5] = llid & 0xFF;

    return ret;
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
    return OAM_E_OK;
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
    //TODO:Geesun remove the MACRO after the SDL complete

    return OAM_E_OK;
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
    return OAM_E_OK;
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
    return OAM_E_OK;
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
    return OAM_E_OK;
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
    return OAM_E_OK;
}

#define OAM_MQ_OLT_NAME        "olt_oam_queue"
static unsigned int g_olt_mq_id = 0;

oam_status oam_pkt_msg_send_2_olt(oam_dev_id_t dev, oam_uint8 pon,
                                  oam_llid_t llid, oam_uint8 *pkt, oam_uint32 len)
{
    oam_msg_t oam_msg;

    memset(&oam_msg, 0, sizeof(oam_msg));

    if (len > OAM_MQ_MSG_MAX_LEN)
        len = OAM_MQ_MSG_MAX_LEN;

    oam_msg.hdr.type = OAM_PACKET;
    oam_msg.hdr.dev  = dev;
    oam_msg.hdr.pon  = pon;
    oam_msg.hdr.llid = llid;
    oam_msg.hdr.len  = len;

    if (len > 0 && pkt != NULL)
        memcpy(&oam_msg.data[0], pkt, len);

    if (cs_queue_put(g_olt_mq_id, &oam_msg, len + sizeof(oam_msg_hdr_t),
                     OSAL_NO_WAIT, 0) < 0) {
        OAM_LOG_DEBUG("\r\n[%s:%d]Send message failed.\r\n", __FUNCTION__, __LINE__);
        return OAM_E_ERROR;
    }

    return OAM_E_OK;

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
    int ret = 0;

    ret = cs_queue_create(&g_olt_mq_id,
                          OAM_MQ_OLT_NAME,
                          OAM_MQ_MSG_MAX_NUM,
                          OAM_MQ_MSG_LEN,
                          0);

    if (ret) {
        OAM_LOG_DEBUG("\r\nCreate ONU message queue failed!\r\n");
        return OAM_E_ERROR;
    }

    return OAM_E_OK;
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
    return OAM_E_OK;
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
    oam_status ret = OAM_E_OK;

    OAM_ASSERT_RET(buf != NULL, OAM_E_PARAM);

    oam_pkt_msg_send_2_olt(0, 0, llid, buf, len);

    return ret;
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
    return OAM_EXT_MODE_CTC;
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
    return OAM_E_OK;
}

/*****************************************************************************/
/* $rtn_hdr_start  oam_dying_gasp_init_adapt                                 */
/* CATEGORY   : APP                                                          */
/* ACCESS     : public                                                       */
/* BLOCK      : OAM                                                          */
/* CHIP       : ----                                                         */
/*                                                                           */
oam_status oam_dying_gasp_init_adapt(oam_if_t *intf)
/*                                                                           */
/* INPUTS  : o: index - llid index                                           */
/*              llid  - llid allocated by olt                                */
/* OUTPUTS : OAM status                                                      */
/* DESCRIPTION:                                                              */
/* set dying gasp packet buffer to kernal for linux solution                 */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    return OAM_E_OK;
}
