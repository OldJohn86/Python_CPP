
#include "osal_common.h"
#include "oam_cmn.h"
#include "oam_adapt.h"
#include "oam_port_stats.h"
#include "oam_tk_upg.h"
/*
KT introduce this feature  inder to drop deployed CS8032 VOC problem,  OLT will regularly send the special reset OAM 
to reset ONU, if CS8022 detect system is idle status it will  reboot itself automatically. 
   For example: 
Anytime, KT OLT will send the special reset OAM with the traffic rate in the morning, and CS8032 will monitor the 
traffic for upstream and downstream below to the traffic ratio for a long time. CS8032 will detect Up and Down stream 
below the designated traffic, and it will reboot itself.  It will drop the accumulated customer complain in their 
perspective. 
It means that CS8032 will automatically search the empty traffic time slot ,and the time slot can’t make any impact 
to the customer’s network work, and the automatic reboot will renew CS8032 from the strange symptoms. 

checking mechanism:
polling uni ports counters every 30s, the counter of all the minutes during the period (start_hour - end_hour) 
are greater than threshold (include upstream and downstream), then reboot ONU, otherwise, do nothing.
*/

//#define OAM_PORT_STATS_POLLING_ACCURACY 30000 /* 30s */
#define OAM_PORT_AVG_STATS_MONITOR_ACCURACY 30000 /* 30s */

typedef struct {
    oam_uint64 rx_byte; //1 minute rx byte
    oam_uint64 tx_byte; //1 minute tx byte
    oam_uint64 rx_avg; 
    oam_uint64 tx_avg; 
    oam_uint32 cycle_timer_id;
    oam_uint32 polling_cnt;  //twice per second
    oam_uint16 start_hour;
    oam_uint16 start_min;
    oam_uint16 end_hour;
    oam_uint16 end_min;
    oam_uint16 up_threshold;
    oam_uint16 dn_threshold;
    oam_uint16 enable;
    oam_uint16 detecting_status; 
    oam_uint16 reset_flag_cnt;
}oam_port_avg_stats_t;

static oam_port_avg_stats_t g_oam_port_avg_stats; 

void oam_local_time_get(oam_uint16 *local_hour, oam_uint16 *local_min, oam_uint16 *local_sec)
{  
    time_t rawtime;  
    struct tm * timeinfo;  
    time (&rawtime );  
    timeinfo = localtime ( &rawtime );  
    *local_hour = timeinfo->tm_hour; 
    *local_min = timeinfo->tm_min; 
    *local_sec = timeinfo->tm_sec; 
    return NULL;
}

void oam_local_time_display()
{
    oam_uint16 local_hour, local_min, local_sec;
    oam_local_time_get(&local_hour, &local_min, &local_sec);
    
    oam_printf( "\r\nLocal Time %02d:%02d:%02d \r\n", local_hour, local_min, local_sec ); 
}   

void oam_guarddog_port_traffic_polling()
{
    oam_boolean upgrade_flag;
    oam_uint16 local_hour, local_min, local_sec;
    oam_uint16 port = 0;
    oam_uint64 txDelta = 0; 
    oam_uint64 rxDelta = 0;
    oam_uint64 rx_avg = 0; 
    oam_uint64 tx_avg = 0;
    
    oam_uint64 txBytes = 0;
    oam_uint64 rxBytes = 0;
    oam_uint64 txPkt = 0;
    oam_uint64 rxPkt = 0;
    oam_uni_stats64_t stats;
    
    for(port = OAM_UNI_PORT_ID1; port < OAM_UNI_PORT_ID1 + OAM_UNI_PORT_NUM; port++){
        oam_onu_uni_stats_sw_get(port, &stats);
        txBytes += stats.txbyte_cnt;
        rxBytes += stats.rxbyte_cnt;
        txPkt += stats.txfrm_cnt;
        rxPkt += stats.rxfrm_cnt;
    }

    upgrade_flag = oam_tk_upg_upgrading();
    if(upgrade_flag)
        return ;

    /* add packet header: 20bytes */
    txBytes += txPkt * 20;
    rxBytes += rxPkt * 20;
    
    if(!g_oam_port_avg_stats.rx_byte)
    {
        g_oam_port_avg_stats.rx_byte = rxBytes;
        rxDelta = rxBytes; 
    }
    else
    {
        if(g_oam_port_avg_stats.rx_byte <= rxBytes)
        {
            rxDelta = rxBytes - g_oam_port_avg_stats.rx_byte;
        }
        else
        {
            rxDelta = (0xffffffffffffffffULL -  g_oam_port_avg_stats.rx_byte) +  rxBytes;
        }
        
        g_oam_port_avg_stats.rx_byte = rxBytes;
    }
        
    if(!g_oam_port_avg_stats.tx_byte)
    {
        g_oam_port_avg_stats.tx_byte = txBytes;
        txDelta = txBytes;
    }
    else
    {
        if(g_oam_port_avg_stats.tx_byte <= txBytes)
            txDelta = txBytes - g_oam_port_avg_stats.tx_byte;
        else
        {
            txDelta = (0xffffffffffffffffULL -  g_oam_port_avg_stats.tx_byte) + txBytes;
        }
        
        g_oam_port_avg_stats.tx_byte = txBytes; 
    }

    /* convert Bytes to Kbps */
    tx_avg = txDelta;
    rx_avg = rxDelta;
    rx_avg *=8;     //bytes to bits
    tx_avg *=8;     //bytes to bits
    rx_avg /= (OAM_STATS_MIB_POLL_ACCURARY/1000);   // per second rate, unit is bit
    tx_avg /= (OAM_STATS_MIB_POLL_ACCURARY/1000);   // per second rate, unit is bit
    rx_avg /= 1000; //bits to kpps
    tx_avg /= 1000; //bits to kpps  
            
    g_oam_port_avg_stats.tx_avg = tx_avg;
    g_oam_port_avg_stats.rx_avg = rx_avg;
        
    if(g_oam_port_avg_stats.detecting_status)
    {
        oam_local_time_get(&local_hour, &local_min, &local_sec);
        oam_printf("polling %02d:%02d:%02d-> dn %lldkbps, up %lldkbps\r\n",local_hour, local_min, local_sec,
            g_oam_port_avg_stats.tx_avg, g_oam_port_avg_stats.rx_avg);
    }
}

void oam_guarddog_port_avg_stats_monitor()
{
    oam_uint16 local_hour, local_min, local_sec;
    oam_uint16 lcl_v = 0;
    oam_uint16 start_v = 0;
    oam_uint16 end_v = 0;

    if(!g_oam_port_avg_stats.enable)
    {
        oam_printf("Invalid average traffic check status: %d\r\n", g_oam_port_avg_stats.enable);
        g_oam_port_avg_stats.detecting_status = 0;
        return;
    }
        
        
    //to check detect interval, unit is minute
    oam_local_time_get(&local_hour, &local_min, &local_sec);
    //oam_printf("\n local time %d:%d duration[%d~%d]\r\n", local_hour,local_min, g_oam_port_avg_stats.start_time, g_oam_port_avg_stats.end_time);

    //startup traffic detection procedure
    lcl_v = local_hour * 60 + local_min;
    
    start_v = g_oam_port_avg_stats.start_hour * 60 + g_oam_port_avg_stats.start_min;
    if(lcl_v < start_v)     /* not start traffic monitor yet */
    {   
        g_oam_port_avg_stats.detecting_status = 0;
        return;
    }
            
    if(lcl_v == start_v)    /* start traffic monitor */
    {
        if(!g_oam_port_avg_stats.detecting_status)
            oam_printf("Start traffic detection, local time [%d:%d], detect time range[%d,%d] ~ [%d,%d] \r\n",local_hour,local_min,g_oam_port_avg_stats.start_hour,g_oam_port_avg_stats.start_min, \
            g_oam_port_avg_stats.end_hour,g_oam_port_avg_stats.end_min);
        
        if(!g_oam_port_avg_stats.detecting_status)
            g_oam_port_avg_stats.detecting_status = 1; 
    }

    end_v = g_oam_port_avg_stats.end_hour * 60 + g_oam_port_avg_stats.end_min;
    
    if(lcl_v > end_v )      /* end traffic monitor */
    {
        g_oam_port_avg_stats.detecting_status = 0;
        return; 
    }

    if(g_oam_port_avg_stats.detecting_status)
    {
        oam_printf("detect %02d:%02d:%02d-> dn %lldkbps, up %lldkbps\r\n",local_hour, local_min, local_sec,
            g_oam_port_avg_stats.tx_avg, g_oam_port_avg_stats.rx_avg);
        
        if(lcl_v < end_v)   /* traffic monitor */
        {   
            if(g_oam_port_avg_stats.tx_avg > g_oam_port_avg_stats.dn_threshold ||  g_oam_port_avg_stats.rx_avg > g_oam_port_avg_stats.up_threshold)
            {
                oam_printf("%02d:%02d Not hit, [Tx:%lld, threshold:%d] [Rx:%lld, threshold:%d]\r\n",local_hour, local_min, 
                    g_oam_port_avg_stats.tx_avg,g_oam_port_avg_stats.dn_threshold,g_oam_port_avg_stats.rx_avg,g_oam_port_avg_stats.up_threshold);

                g_oam_port_avg_stats.detecting_status = 0; 
                g_oam_port_avg_stats.reset_flag_cnt = 0;
                return ;
            }
        }       
        if(lcl_v == end_v)  /* the last minute */
        {
            oam_printf("\n %02d:%02d Hit it, reboot CS8032 [Tx:%lld, threshold:%d] [Rx:%lld, threshold:%d]\r\r\n",local_hour, local_min, 
                g_oam_port_avg_stats.tx_avg,g_oam_port_avg_stats.dn_threshold,g_oam_port_avg_stats.rx_avg,g_oam_port_avg_stats.up_threshold);
            ca_timer_add(100, oam_system_reboot, NULL);

        }
    }
}

oam_status oam_guarddog_reboot_set(oam_uint16 up_rate, oam_uint16 dn_rate, oam_uint16 start_hour, oam_uint16 start_min, oam_uint16 end_hour, oam_uint16 end_min)
{ 
    if((start_hour > end_hour) || ((start_hour == end_hour) && (start_min >= end_min)))
    {
        oam_printf("Invalid time range: [%d, %d] ~ [%d, %d]\r\n", start_hour,start_min, end_hour, end_min);
        g_oam_port_avg_stats.enable = 0; 
        g_oam_port_avg_stats.detecting_status = 0;
        return OAM_E_PARAM;
    }
    
    g_oam_port_avg_stats.start_hour = start_hour;
    g_oam_port_avg_stats.start_min = start_min;
    g_oam_port_avg_stats.end_hour = end_hour;
    g_oam_port_avg_stats.end_min = end_min;
    g_oam_port_avg_stats.up_threshold = up_rate; 
    g_oam_port_avg_stats.dn_threshold = dn_rate; 
    g_oam_port_avg_stats.rx_avg = 0;
    g_oam_port_avg_stats.tx_avg = 0;
    g_oam_port_avg_stats.enable= 1;
    g_oam_port_avg_stats.reset_flag_cnt = 0;

    return OAM_E_OK;
}

oam_status oam_guarddog_reboot_enable(
    oam_uint16 up_rate, oam_uint16 down_rate, 
    oam_uint16 start_hour, oam_uint16 start_min, 
    oam_uint16 end_hour, oam_uint16 end_min)
{

    if(oam_guarddog_reboot_set(up_rate, down_rate, start_hour, start_min, end_hour, end_min))
        return OAM_E_ERROR;
    
    if(!g_oam_port_avg_stats.cycle_timer_id)
        g_oam_port_avg_stats.cycle_timer_id = ca_circle_timer_add(OAM_PORT_AVG_STATS_MONITOR_ACCURACY, 
            oam_guarddog_port_avg_stats_monitor, NULL);
    return OAM_E_OK;
}

void oam_guarddog_info_show()
{
    oam_local_time_display();
    
    oam_printf("\r\nenable %d\r\n", g_oam_port_avg_stats.enable);
    oam_printf("up_threshold %d\r\n", g_oam_port_avg_stats.up_threshold);
    oam_printf("dn_threshold %d\r\n", g_oam_port_avg_stats.dn_threshold);
    oam_printf("start_hour %d\r\n", g_oam_port_avg_stats.start_hour);
    oam_printf("start_min %d\r\n", g_oam_port_avg_stats.start_min);
    oam_printf("end_hour %d\r\n", g_oam_port_avg_stats.end_hour);
    oam_printf("end_min %d\r\n", g_oam_port_avg_stats.end_min);

    oam_printf("detecting_status %d\r\n", g_oam_port_avg_stats.detecting_status);
    oam_printf("reset_flag_cnt %d\r\n", g_oam_port_avg_stats.reset_flag_cnt);
    oam_printf("rx_byte %lld\r\n", g_oam_port_avg_stats.rx_byte);
    oam_printf("tx_byte %lld\r\n", g_oam_port_avg_stats.tx_byte);
    oam_printf("rx_avg %lld\r\n", g_oam_port_avg_stats.rx_avg);
    oam_printf("tx_avg %lld\r\n", g_oam_port_avg_stats.tx_avg);
    oam_printf("cycle_timer_id %d\r\n", g_oam_port_avg_stats.cycle_timer_id);
    
}

void oam_guarddog_reboot_init()
{
    memset(&g_oam_port_avg_stats, 0, sizeof(oam_port_avg_stats_t));  
}


