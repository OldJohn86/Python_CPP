#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_DPOE_OAM

#include "oam_dpoe_key_exchange.h"
#include "epon.h"
#include "port.h"
#include "oam_dpoe.h"

static ca_oam_key_exchange_mode_t g_key_exchange_mode = CA_OAM_KEY_EXCHANGE_MODE_NONE;
key_exch_llid_time_t g_key_exch_timer[OAM_MAX_LLID_NUM];
/*
*Set key exchange expiration timer
*provide by APP, called by OAM
*/
oam_uint32 oam_dpoe_key_exchange_expir_timer_set(
    oam_llid_t llid,
    oam_uint16 timer)
{
    g_key_exch_timer[llid].valid = 1;
    g_key_exch_timer[llid].time = timer;

    OAM_ORG_DEBUG("%d,set or disable key exchange timer,llid=%d,timer= %d\n",__LINE__,llid,timer);
    if (g_key_exch_timer[llid].time != 0)
        dpoe_encrypt_enable(llid, TRUE);
    else dpoe_encrypt_enable(llid, FALSE);

    return OAM_E_OK;
}

/*
*Get key exchange expiration timer
*provide by APP, called by OAM
*/
oam_uint32 oam_dpoe_key_exchange_expir_timer_get(
    oam_llid_t llid,
    oam_uint16 *timer)
{
    OAM_ASSERT_RET(timer,OAM_E_PARAM);

    if(g_key_exch_timer[llid].valid){
        *timer = g_key_exch_timer[llid].time;
    }else{
        OAM_ORG_DEBUG("%d,g_key_exch_timer[llid].valid is not valid",__LINE__);
        return OAM_E_ERROR;
    }
    return OAM_E_OK;
}

/*Set key exchange mode
*provide by APP, called by OAM
*/
oam_uint32 oam_dpoe_key_exhange_mode_set(ca_oam_key_exchange_mode_t key_exchange_mode)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t pon_port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7);

    switch(key_exchange_mode){
        case CA_OAM_KEY_EXCHANGE_MODE_NONE:
            OAM_ORG_DEBUG("%d,Now is CA_OAM_KEY_EXCHANGE_MODE_NONE\n",__LINE__);
            ret = ca_port_encryption_enable_set(0,pon_port,CA_DIRECTION_ENC_BI,FALSE);
            CA_RET_VALUE_CHECK(ret);
            g_key_exchange_mode = key_exchange_mode;
            return OAM_E_OK;
        case CA_OAM_KEY_EXCHANGE_MODE_1G_DOWN_ONLY:
            OAM_ORG_DEBUG("%d,Now is CA_OAM_KEY_EXCHANGE_MODE_1G_DOWN_ONLY\n",__LINE__);
            ret = ca_port_encryption_enable_set(0,pon_port,CA_DIRECTION_ENC_RX,TRUE);
            CA_RET_VALUE_CHECK(ret);
            break;
        case CA_OAM_KEY_EXCHANGE_MODE_10G_DOWN_ONLY:
            OAM_ORG_DEBUG("%d,Now is CA_OAM_KEY_EXCHANGE_MODE_10G_DOWN_ONLY\n",__LINE__);
            ret = ca_port_encryption_enable_set(0,pon_port,CA_DIRECTION_ENC_RX,TRUE);
            CA_RET_VALUE_CHECK(ret);
            break;
        case CA_OAM_KEY_EXCHANGE_MODE_10G_BI:
            OAM_ORG_DEBUG("%d,Now is CA_OAM_KEY_EXCHANGE_MODE_10G_BI\n",__LINE__);
            ret = ca_port_encryption_enable_set(0,pon_port,CA_DIRECTION_ENC_BI,TRUE);
            CA_RET_VALUE_CHECK(ret);
            break;
        default:
            OAM_ORG_DEBUG("Invalid key exchange mode, set to CA_OAM_KEY_EXCHANGE_MODE_NONE\n",__LINE__);
            ret = ca_port_encryption_enable_set(0,pon_port,CA_DIRECTION_ENC_BI,FALSE);
            //g_key_exchange_mode = CA_OAM_KEY_EXCHANGE_MODE_NONE;
            return OAM_E_NOT_SUPPORT;
    }

    g_key_exchange_mode = key_exchange_mode;

    return OAM_E_OK;
}

void oam_dpoe_key_exhange_mode_set_callback(void *data){

    ca_status_t ret = CA_E_OK;
    ca_uint32_t pon_port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7);
    ca_uint8_t *mode = (ca_oam_key_exchange_mode_t *)data;

    switch(*mode){
        case CA_OAM_KEY_EXCHANGE_MODE_NONE:
            OAM_ORG_DEBUG("%d,Now is CA_OAM_KEY_EXCHANGE_MODE_NONE\n",__LINE__);
            g_key_exchange_mode = CA_OAM_KEY_EXCHANGE_MODE_NONE;
            return OAM_E_OK;
        case CA_OAM_KEY_EXCHANGE_MODE_1G_DOWN_ONLY:
            OAM_ORG_DEBUG("%d,Now is CA_OAM_KEY_EXCHANGE_MODE_1G_DOWN_ONLY\n",__LINE__);
            ret = ca_port_encryption_enable_set(0,pon_port,CA_DIRECTION_ENC_RX,TRUE);
            CA_RET_VALUE_CHECK(ret);
            break;
        case CA_OAM_KEY_EXCHANGE_MODE_10G_DOWN_ONLY:
            OAM_ORG_DEBUG("%d,Now is CA_OAM_KEY_EXCHANGE_MODE_10G_DOWN_ONLY\n",__LINE__);
            ret = ca_port_encryption_enable_set(0,pon_port,CA_DIRECTION_ENC_RX,TRUE);
            CA_RET_VALUE_CHECK(ret);
            break;
        case CA_OAM_KEY_EXCHANGE_MODE_10G_BI:
            OAM_ORG_DEBUG("%d,Now is CA_OAM_KEY_EXCHANGE_MODE_10G_BI\n",__LINE__);
            ret = ca_port_encryption_enable_set(0,pon_port,CA_DIRECTION_ENC_BI,TRUE);
            CA_RET_VALUE_CHECK(ret);
            break;
        default:
            OAM_ORG_DEBUG("%d,Invalid key exchange mode\n",__LINE__);
            g_key_exchange_mode = CA_OAM_KEY_EXCHANGE_MODE_NONE;
            return OAM_E_NOT_SUPPORT;
    }

    g_key_exchange_mode = *mode;

    return OAM_E_OK;

}


/*Get key exchange mode
*provide by APP, called by OAM
*/
oam_uint32 oam_dpoe_key_exhange_mode_get(ca_oam_key_exchange_mode_t *key_exchange_mode)
{
    OAM_ASSERT_RET(key_exchange_mode,OAM_E_PARAM);

    *key_exchange_mode = g_key_exchange_mode;

    return OAM_E_OK;
}

/*Set new key send from OLT
*provide by APP, called by OAM
*/
oam_uint32 oam_dpoe_key_from_olt_set(
    oam_llid_t llid,
    oam_uint32 key_number,
    oam_uint32 key_length,
    oam_uint8 *key_value)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t pon_port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7);

    OAM_ASSERT_RET(key_value,OAM_E_PARAM);

    OAM_ORG_DEBUG("Now is %s:%d \n ",__FUNCTION__,__LINE__);

    ret = ca_epon_llid_encryption_key_set(0,pon_port,llid,key_number,key_value);
    CA_RET_VALUE_CHECK(ret);
}

void dpoe_encrypt_init()
{
    memset(g_key_exch_timer, 0, sizeof(g_key_exch_timer));
    system("echo 0 > /etc/dpoe_encrypt_mode.txt");
    system("wpa_supplicant -Dwired -ieth0 -c/etc/wpa_supplicant.conf &");
}

void dpoe_encrypt_llid_reg(oam_llid_t llid, oam_uint16 index)
{
    int i;

    for (i=0;i<OAM_MAX_LLID_NUM;i++)
    {
        if (g_key_exch_timer[i].valid == 0)
        {
            g_key_exch_timer[i].valid = 1;
            g_key_exch_timer[i].llid = index;
            return;
        }
    }
}

void dpoe_encrypt_llid_dereg(oam_llid_t llid, oam_uint16 index)
{
    int i;

    for (i=0;i<OAM_MAX_LLID_NUM;i++)
    {
        if (g_key_exch_timer[i].valid && g_key_exch_timer[i].llid == index)
        {
            g_key_exch_timer[i].valid = 0;
            if (g_key_exch_timer[i].key_exchange_handler)
            {
                ca_timer_del(g_key_exch_timer[i].key_exchange_handler);
                g_key_exch_timer[i].key_exchange_handler = 0;
            }
            if (g_key_exch_timer[i].ver_handler)
            {
                ca_timer_del(g_key_exch_timer[i].ver_handler);
                g_key_exch_timer[i].ver_handler = 0;
            }
            return;
        }
    }
}

void oam_key_exch_timer_expire_handle(oam_uint8 index)
{
    ca_uint32_t pon_port = CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7);
    oam_uint8 key[16],tmp_key[16];
    oam_uint32 r;
    oam_uint8 i = 0;
    memset(&key,0,sizeof(key));
    memset(&tmp_key,0,sizeof(tmp_key));
    if (index > 16) return;
    if (!g_key_exch_timer[index].valid) return;
    // generate random key
    r = random();
    memcpy(tmp_key, &r, 4);
    r = random();
    memcpy(&tmp_key[4], &r, 4);
    r = random();
    memcpy(&tmp_key[8], &r, 4);
    r = random();
    memcpy(&tmp_key[12], &r, 4);

    for(i=0;i<16;i++){
       key[i] = tmp_key[15-i];
       //ca_printf("\n The key after reverted the order is 0x%x\n ",key[i]);
    }
    ca_epon_llid_encryption_key_set(0,pon_port,index,g_key_exch_timer[index].number, (ca_epon_port_encryption_key_t * )tmp_key);
    ca_mdelay(1);
    oam_key_exchange_request(g_key_exch_timer[index].llid, g_key_exch_timer[index].number, key);

    g_key_exch_timer[index].number = (g_key_exch_timer[index].number == 0)? 1:0;
    // start switchover timer; optional

    if(CA_OAM_KEY_EXCHANGE_MODE_1G_DOWN_ONLY == g_key_exchange_mode
        || CA_OAM_KEY_EXCHANGE_MODE_10G_DOWN_ONLY == g_key_exchange_mode){
        //system("wpa_supplicant -Dwired -ieth0 -c/etc/wpa_supplicant.conf -dd&");
    }
}

void dpoe_key_exch_timer_expire(void *data)
/*                                                                           */
/* INPUTS  : N/A                                                             */
/* OUTPUTS : N/A                                                             */
/* DESCRIPTION:                                                              */
/* oam keep alive timer process                                              */
/* $rtn_hdr_end                                                              */
/*****************************************************************************/
{
    oam_timer_msg_send(OAM_KEY_EXCH_TIMER_EXPIRED, data);
}

void dpoe_encrypt_enable(oam_uint8 index, oam_boolean enable)
{
    if (enable)
    {
        // start key exchange timer
        if (g_key_exch_timer[index].key_exchange_handler) return;

        g_key_exch_timer[index].key_exchange_handler =
            ca_circle_timer_add(g_key_exch_timer[index].time * 1000,
                           dpoe_key_exch_timer_expire, index);
    }
    else
    {
        if (g_key_exch_timer[index].key_exchange_handler)
        {
            ca_timer_del(g_key_exch_timer[index].key_exchange_handler);
            g_key_exch_timer[index].key_exchange_handler = 0;
        }
    }
}


#endif














