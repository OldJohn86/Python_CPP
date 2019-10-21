#include "kt_log.h"

/*
 * This file is added to support Korean Log.
 * Reason to add this file: In KT web management feature, customer request Cortina to change the system log that is stored in
 * flash to Korean. However, sourceinsight and vi don't support Korean language. When the source file that contains Korean
 * log is opened by sourceinsight or vi, then save the file via sourceinsight or vi, then the Korean log will lost,this is
 * not we want. Since a lot of engineers modify code with sourceinsight or vi, this may cause the Korean log loss.
 * To avoid this problem, put all Korean log into a separate file, namely kt_log.c.
 *
 * Warning: please let the owner know if you want to modify this file.
 */

void kt_log_optical_power_alarm(oam_uint8 status)
{
    OAM_ORG_LOG( "광파워알람%s\n",(status==1)?"발생":"해제");
}

void kt_log_voltage_alarm(oam_uint8 status, oam_uint8 high_low_flag)
{
    OAM_ORG_LOG( "%s전압알람%s\n",(high_low_flag)?"과":"저",(status==1)?"발생":"해제");
}

void kt_log_lao_alarm(oam_uint8 status)
{
    OAM_ORG_LOG("역광장애%s\n",(status==1)?"발생":"해제");
}

void kt_log_sen_exceed_raised_alarm(void)
{
    OAM_ORG_LOG( "OLT광파워알람발생\n");
}

void kt_log_optical_power_released_alarm(void)
{
    OAM_ORG_LOG( "OLT광파워알람해제\n");
}
