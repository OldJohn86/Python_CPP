#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "oam_types.h"
#include "oam_adapt.h"
#include "oam_cmn.h"
#include "oam_main.h"



oam_boolean oam_d2_compatible_get_adapt()
{
    return FALSE;
}

oam_uint8  oam_dying_gasp_num_get_adapt()
{
    /*default dying gasp num is 1*/
    return OAM_DYING_GASP_NUM;
}

oam_status oam_dying_gasp_send_adapt(oam_pdu_info_t *pdu, oam_uint32 len)
{
    return OAM_E_OK;
}

oam_uint8 oam_uni_port_num_get_adapt()
{
    return OAM_DEFAULT_PORT_NUM;
}

oam_boolean oam_is_image_updating_adapt()
{
    return 0;
}

oam_status oam_iop_vender_code_get_adapt (oam_uint8 *vcode, oam_uint32 len)
{
    OAM_ASSERT_RET(vcode != NULL, OAM_E_PARAM);

    return OAM_E_OK;
}

oam_status oam_iop_vender_code_set_adapt (const oam_uint8 *vcode,
        oam_uint32 len)
{
    OAM_ASSERT_RET(vcode != NULL, OAM_E_PARAM);

    return OAM_E_OK;
}

void oam_system_reset_adapt()
{

}

oam_status oam_pkt_send_adapt(
    oam_port_id_t port,
    oam_llid_t llid,
    oam_uint8 *buf,
    oam_uint32 len)
{
    oam_status ret = OAM_E_OK;

    OAM_ASSERT_RET(buf != NULL, OAM_E_PARAM);


    return ret;
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
/* OUTPUTS : SDL status                                                      */
/* DESCRIPTION:                                                              */
/* get the mac address for the llid                                          */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_status ret = OAM_E_OK;

    OAM_ASSERT_RET(mac != NULL, OAM_E_PARAM);

    return ret;

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
/* OUTPUTS : SDL status                                                      */
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
/* OUTPUTS : SDL status                                                      */
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
/* OUTPUTS : SDL status                                                      */
/* DESCRIPTION:                                                              */
/* set the onu encrypt mode from the sdl                                     */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    return OAM_E_OK;
}

oam_status oam_pon_stats_get_adapt(oam_pon_stats_t *stats)
{
    return OAM_E_OK;
}

oam_status oam_pon_stats_summary_get_adapt(oam_uint64 *frames,
        oam_uint64 *errors, oam_uint64 *err_symb)
{
    return OAM_E_OK;
}

oam_status oam_evt_reg_adapt()
{
    return OAM_E_OK;
}

oam_status oam_pkt_trap_adapt()
{
    return OAM_E_OK;
}

oam_ext_mode_t oam_ext_mode_get_adapt()
{
    return OAM_EXT_MODE_DPOE;
}

oam_status oam_pon_port_enable_set_adapt(oam_boolean enable)
{
    return OAM_E_OK;
}

oam_status oam_pon_led_set_adapt(oam_uint8 action)
{
    return OAM_E_OK;
}

