#ifndef _KT_LOG_H
#define _KT_LOG_H

#include "oam_cmn.h"

extern void kt_log_optical_power_alarm(oam_uint8 status);
extern void kt_log_voltage_alarm(oam_uint8 status, oam_uint8 high_low_flag);
extern void kt_log_lao_alarm(oam_uint8 status);
extern void kt_log_sen_exceed_raised_alarm(void);
extern void kt_log_optical_power_released_alarm(void);

#endif
