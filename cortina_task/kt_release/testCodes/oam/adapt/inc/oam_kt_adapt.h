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

Copyright (c) 2009 by Cortina Systems Incorporated
****************************************************************************/
#ifndef __OAM_KT_ADAPT_H_
#define __OAM_KT_ADAPT_H_

#ifdef HAVE_KT_OAM
#include "oam_kt_pdu.h"
#ifdef HAVE_CA_LED
#include "ca_led.h"
#endif
#include "ca_version.h"
#include "oam_tk.h"

typedef struct {
        oam_uint16          year;
        oam_uint8           month;
        oam_uint8           day;
        oam_uint8           hour;
        oam_uint8           minute;
        oam_uint8           second;
} __attribute__((packed)) kt_prv_system_time_t;

oam_status kt_prv_oam_onu_block_set_adapt (
                            oam_uint8 action
);

oam_status kt_prv_oam_onu_block_get_adapt (
                            oam_uint8 *action
);

oam_status kt_prv_oam_eth_mac_limit_set_adapt (
                            oam_uint8 oper,
                            oam_uint32 num
);

oam_status kt_prv_oam_eth_mac_limit_get_adapt (
                            oam_uint8 *oper,
                            oam_uint8 *num
);

oam_status kt_prv_oam_onu_mpcp_counter_get_adapt (
    oam_kt_onu_mpcp_counter_container_struct   *counter
);

oam_status kt_prv_oam_onu_queue_drop_counter_get_adapt (
    oam_kt_onu_queue_drop_counter_container_struct *counter
);

oam_status kt_prv_oam_port_counter_get_adapt (
                            oam_uint8  port_id,
                            oam_kt_onu_port_counter_container_struct *counter
);

oam_status kt_prv_oam_diagnostics_get_adapt (
                            oam_uint16 *temp,
                            oam_uint16 *tx_power,
                            oam_uint16 *rx_power
);

oam_status kt_prv_oam_loop_detect_set_adapt (
                            oam_uint8 oper,
                            oam_uint8 interval,
                            oam_uint8 block_time
);

oam_status kt_prv_oam_loop_detect_get_adapt (
                            oam_uint8 *oper,
                            oam_uint8 *interval,
                            oam_uint8 *block_time
);

oam_status kt_prv_oam_dhcp_option_cfg_get_adapt (
                            oam_uint8 *cfg);

oam_status kt_prv_oam_dhcp_option_cfg_set_adapt (
                            oam_uint8 cfg);

oam_status kt_prv_oam_tx_power_off_set_adapt (
                            oam_uint8 action,
                            oam_uint8 duration

);

oam_status kt_prv_oam_ds_shaping_set_adapt (
                            oam_uint8 port_id,
                            oam_uint8 oper,
                            oam_uint32 rate
);

oam_status kt_prv_oam_ds_shaping_get_adapt (
                            oam_uint8 port_id,
                            oam_uint8 *oper,
                            oam_uint8 *rate
);

oam_status kt_prv_oam_eth_rstp_set_adapt (
                            oam_uint8 oper
);

oam_status kt_prv_oam_eth_rstp_get_adapt (
                            oam_uint8 *oper
);

oam_status kt_prv_oam_static_mac_get_adapt (
                            oam_uint8 *mac,
                            oam_uint8 *num
);

oam_status kt_prv_oam_all_static_fdb_entries_del_adapt ();

oam_status kt_prv_oam_static_fdb_entries_add_adapt (
                            oam_uint8 *mac,
                            oam_uint8 num
);
oam_status kt_prv_oam_all_dynamic_fdb_entries_clr_adapt ();

oam_status kt_prv_oam_default_restore_adapt ();

oam_status kt_prv_oam_power_reset_adapt ();

oam_status kt_prv_oam_onu_mac_table_get_adapt (
                            oam_uint8 *mac,
                            oam_uint32 *num);

oam_status kt_prv_oam_opm_ddm_get_adapt(
    oam_int16    *temperature,
    oam_uint16   *voltage,
    oam_uint16   *tx_bias,
    oam_uint16   *tx_power,
    oam_uint16   *rx_power
);
oam_boolean kt_prv_oam_is_new_board_get_adapt();

oam_status kt_prv_oam_onu_time_set_adapt (
    kt_prv_system_time_t *sys_time);
oam_status kt_oam_onu_ipv6_opt254_set_adapt(oam_uint8 enable);

oam_status kt_oam_onu_ipv6_opt254_get_adapt(oam_uint8 *enable);
oam_status kt_prv_oam_ddm_threshold_set_adapt(ddm_threshold_t *data);
oam_status kt_prv_oam_ddm_threshold_get_adapt(ddm_threshold_t *data);
oam_status kt_prv_oam_onu_per_port_mac_entries_get_adapt (ca_uint8 port, oam_kt_onu_per_port_mac_entries_t *pFdbEntries, ca_uint8 
*num);


#define KT_MAX_VLANID                                   4095

#define KT_NORMAL_RESET_DELAY_ADAPT             200         /* 200ms */

#define KT_OAM_MAX_RULE_RECORD                 24
#define KT_OAM_MAX_CLS                                  (16-2)

#define KT_MAX_IFNAME_LEN                   16
#define SIOCSIFVLAN                         0x8983  /* Set 802.1Q VLAN options */

#ifdef HAVE_CA_LED
#define CA_LED_PON_REGISTER_SET_ON() \
    do { \
        ca_led_pon_register_set(CA_LED_ON_EVT); \
    }while(0)

#define CA_LED_PON_REGISTER_SET_OFF() \
    do { \
        ca_led_pon_register_set(CA_LED_OFF_EVT); \
    }while(0)
#define CA_LED_PON_REGISTER_SET_BLINK() \
    do { \
        ca_led_pon_register_set(CA_LED_BLINK_EVT); \
    }while(0)
#else
#define CA_LED_PON_REGISTER_SET_ON()
#define CA_LED_PON_REGISTER_SET_OFF()
#define CA_LED_PON_REGISTER_SET_BLINK()
#endif

#ifdef HAVE_CA_GPIO
#define CA_GPIO_SET_VALUE(name, value) \
    do{ \
        ca_gpio_set_value(name, value); \
    }while(0)
#else
#define CA_GPIO_SET_VALUE(name, value)
#endif

typedef struct {
        oam_uint32       timer;
        oam_uint8     mode;
} kt_oam_reset_set_timer_adapt_t;

enum vlan_ioctl_cmds {
    ADD_VLAN_CMD,
    DEL_VLAN_CMD,
    SET_VLAN_INGRESS_PRIORITY_CMD,
    SET_VLAN_EGRESS_PRIORITY_CMD,
    GET_VLAN_INGRESS_PRIORITY_CMD,
    GET_VLAN_EGRESS_PRIORITY_CMD,
    SET_VLAN_NAME_TYPE_CMD,
    SET_VLAN_FLAG_CMD
};

struct vlan_ioctl_args {
    int cmd; /* Should be one of the vlan_ioctl_cmds enum above. */
    char device1[24];

    union {
        char device2[24];
        int VID;
        unsigned int skb_priority;
        unsigned int name_type;
        unsigned int bind_type;
        unsigned int flag; /* Matches vlan_dev_info flags */
    } u;

    short vlan_qos;
};

oam_status kt_oam_action_reset_adapt (
                            oam_uint8  force
);

void stp_send_oam_tk_org_event(ca_uint8_t raised, ca_uint16_t portid);
oam_status oam_kt_adapt_set_ipAddr(oam_uint8 *dev, oam_uint32 ipAddr);
oam_status oam_kt_adapt_set_ipMask(oam_uint8 *dev, oam_uint32 mask);
oam_status oam_kt_adapt_set_dev_up(oam_uint8 *dev);
oam_status oam_kt_adapt_dev_vlan_add(oam_uint8 *dev, oam_uint16 vlanId);
oam_status oam_kt_adapt_dev_vlan_del(oam_uint8 *dev);
oam_int32 oam_kt_adapt_vlan_port_set(oam_uint32 vid);
oam_int32 oam_kt_adapt_vlan_port_del(oam_uint32 vid);
oam_int32 oam_kt_adapt_build_over_voltage_event(void *arg, void *data);
oam_int32 oam_kt_adapt_build_lao_detect_event(void *arg, void *data);
void oam_kt_pon_act_handle(void *arg, void *data);


#define KT_DASAN_UNI_PORT_FIRST 0
#define KT_DASAN_PON_PORT       10

#define KT_TK_UNI_PORT_FIRST 1
#define KT_TK_PON_PORT       0

#define KT_TK_MAX_MAC_PER_PORT 128

oam_status oam_kt_pon_led_set_adapt(oam_uint8 action);

#endif

#endif
