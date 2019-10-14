#ifdef HAVE_DPOE_OAM

#include "oam_types.h"
#include "oam_cmn.h"
#include "oam_main.h"
#include "oam_adapt.h"
#include "oam_supp.h"
#include "oam_core.h"

typedef enum{
	CA_OAM_KEY_EXCHANGE_MODE_NONE 		   = 0,	// none
    CA_OAM_KEY_EXCHANGE_MODE_1G_DOWN_ONLY  = 1, // 1G Downstream only
    CA_OAM_KEY_EXCHANGE_MODE_10G_DOWN_ONLY = 2, // 10G Downstream only
    CA_OAM_KEY_EXCHANGE_MODE_10G_BI        = 3  // 10G Bidirectional
}ca_oam_key_exchange_mode_t;

oam_uint32 oam_dpoe_key_exchange_expir_timer_set(oam_llid_t llid,oam_uint16 timer);
oam_uint32 oam_dpoe_key_exchange_expir_timer_get(oam_llid_t llid,oam_uint16 *timer);
oam_uint32 oam_dpoe_key_exhange_mode_set(ca_oam_key_exchange_mode_t key_exchange_mode);
oam_uint32 oam_dpoe_key_exhange_mode_get(ca_oam_key_exchange_mode_t *key_exchange_mode);
oam_uint32 oam_dpoe_key_from_olt_set(oam_llid_t llid,oam_uint32 key_number,oam_uint32 key_length,oam_uint8 *key_value);
void dpoe_encrypt_init();
void dpoe_encrypt_llid_reg(oam_llid_t llid, oam_uint16 index);
void dpoe_encrypt_llid_dereg(oam_llid_t llid, oam_uint16 index);
void oam_key_exch_timer_expire_handle(oam_uint8 index);
void dpoe_key_exch_timer_expire(void *data);
void dpoe_encrypt_enable(oam_uint8 index, oam_boolean enable);
void oam_dpoe_key_exhange_mode_set_callback(void *data);

#endif











