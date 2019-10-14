
#ifdef HAVE_ZTE_BATCH_UPGRADE
#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include <network.h>
#include <netinet/in.h>
#include "zte_manage_firmware_upgrade.h"
#include <fcntl.h>
#include "onuAalIf.h"
#include "onu_zte_api.h"
#include "cs_module.h"
#include "mif.h"


static oam_uint8                     zte_upgrade_buffer[ZTE_MAX_BUFFER + 1];
zte_upgrade_firmware_node_t         zte_manage_upgrade;


#ifdef HAVE_EXT_SW_DRIVER
extern char tw_onu_sw_ver[ ];
extern char tw_onu_hw_ver[] ;
#endif

extern int write_global_to_flash();
extern int get_ip_config(cyg_uint32 *inaddr_local, cyg_uint32 *inaddr_gateway,
                         cyg_uint32 *inaddr_mask);
extern  oam_int32    zte_flash_cfg_get( );

extern int mif_upgrade (char module, char *input_buff, int length,
                        mif_upgrade_state_t *cur_state, bool end_of_file);
extern int prepare_mif_upgrade (char module, bool need_confirm_from_server);

static oam_int32  zte_proc_listen_state(   );
static oam_int32  zte_proc_upgrade_firmware_state(    );
static oam_int32  zte_build_information_packet(    );
static oam_int32  zte_build_request_download_firmware(     );
static oam_int32  zte_build_response_download_firmware_result ( oam_int8 result);
static oam_int32  zte_save_tlv_value( oam_int32 command_id, oam_int8 *data,
                                     oam_int32 data_length);
static oam_int32    zte_onu_device_infor_get( zte_onu_device_infor_t
        *infor  );
oam_int32 zte_write_flash_control_blob(oam_uint32   image_len ,
                                      blob_info_desc_t *blob_info,  oam_int8 *flash_des , int part_index  );



static oam_int32  zte_build_upgrade_firmware_success ( );
static oam_int32    zte_flash_cfg_set(  );

static oam_int32    zte_reset_onu ( );
static oam_int32     zte_parse_new_version(oam_int8   *version_path);
static oam_int32   zte_send_message_to_server(oam_int32 socket_id,
        oam_int32  server_ip, oam_int32  l4_port
        , oam_int8 *data, oam_int32 data_len,  oam_int32     *output_socket_id  );

static  oam_int32 zte_upgrade_proc_tick(  );
static  void zte_upgrade_tmfunc_match_event(epon_timer_t *timer);
static  oam_int32   zte_recover_init_state( oam_int8  close_policy_socket_flag,
        oam_int8  close_version_socket_flag  );


oam_int32
zte_manage_init( )
{

    memset (&zte_manage_upgrade, 0 , sizeof(zte_manage_upgrade ));

    /*read flash */
    zte_flash_cfg_get( );

    zte_manage_upgrade.state = INIT_STATE;

    return 0;

}



void
zte_manage_remote_upgrade(cyg_addrword_t p)
{
    oam_int32  udp_source = 0;
    struct sockaddr_in udp_local;
    oam_int32    state = 0;
    oam_int32    send_data_len = 0;
    oam_int32    output_socket_id = 0;
    oam_uint8    registerStatus = 0;
    oam_int32    i = 0;
    oam_int32    ret = 0;

    IROS_LOG_MAJ(IROS_MID_UPGRADE, " upgrade firmware version:%s %s\r\n ",
                 __DATE__, __TIME__);

    cyg_thread_delay(1000);
    zte_manage_init( );

    if ( zte_manage_upgrade.flash_cfg.updatestatus == 1) {

        for ( i = 0; i < 3; i++) {
            epon_onu_register_status_read(&registerStatus);

            if ( registerStatus != 1 ) {
                cyg_thread_delay(1000);
                continue;

            }
            else {
                send_data_len = zte_build_upgrade_firmware_success( );
                ret = zte_send_message_to_server(0,
                                                 zte_manage_upgrade.flash_cfg.policy_server_ip
                                                 , zte_manage_upgrade.flash_cfg.policyserverPort, zte_upgrade_buffer,
                                                 send_data_len, &output_socket_id );

                if (ret == 0) {
                    close( output_socket_id);
                }

                break;

            }
        }

        memset( &zte_manage_upgrade.flash_cfg, 0 , sizeof(zte_flash_upgrade_cfg_t ));
        zte_flash_cfg_set();


    }


    udp_source = socket(AF_INET, SOCK_DGRAM, 0);

    if (udp_source < 0) {
        IROS_LOG_MAJ(IROS_MID_UPGRADE, "manage upgrade:create udp socket error\r\n " );
        goto END;
    }

    memset(&udp_local, 0, sizeof(udp_local));
    udp_local.sin_family = AF_INET;
    udp_local.sin_len = sizeof(udp_local);
    udp_local.sin_port = htons(ZTE_UDP_DST_PORT);
    udp_local.sin_addr.s_addr = htonl(INADDR_ANY);  //accepts everyone...

    if (bind(udp_source, (struct sockaddr *) &udp_local, sizeof(udp_local)) < 0) {
        IROS_LOG_MAJ(IROS_MID_UPGRADE, "manage upgrade:bind /source/ error \r\n");
        close( udp_source);
        goto END;
    }


    zte_manage_upgrade.policy_server_udp_socket = udp_source;


    for (; ; ) {
        state = zte_manage_upgrade.state;

        switch ( state) {

            case    INIT_STATE:
            case    RECEIVED_SCAN_ONU_CMD:
                zte_proc_listen_state(  );
                break;

            case    RECEIVED_ENABLE_UPGRADE_CMD:
                zte_proc_upgrade_firmware_state(  );
                break;

            default:
                IROS_LOG_MAJ(IROS_MID_UPGRADE, " default manage state=%d " , state);
                break;


        }



    }


END:

    IROS_LOG_MAJ(IROS_MID_UPGRADE, " run zte manage firmware upgrade failed \r\n ");
    return ;

}






static  oam_int32  zte_proc_listen_state(         )
{
    struct sockaddr_in server_addr;
    oam_int32    server_len = 0;
    oam_int32    len = 0;
    oam_int32    policy_server_udp_socket = 0;
    oam_int32    state = 0;
    zte_udp_scan_or_start_cmd_packet_t  *pdu = NULL;
    oam_uint8    message_version = 0;
    oam_uint8    return_value = 0;
    oam_uint16  ext_flag = 0;
    oam_uint16  command_id = 0;
    oam_uint16  arguments = 0;
    oam_uint16  data_length = 0;
    oam_uint32  checksum = 0;
    oam_uint32   reserved = 0;
    struct sockaddr_in local;
    oam_int32    send_data_len = 0;
    struct sockaddr_in c_addr;
    socklen_t c_len = 0;
    oam_int32 one = 1;

    policy_server_udp_socket = zte_manage_upgrade.policy_server_udp_socket;
    state = zte_manage_upgrade.state;
    server_len = sizeof(server_addr);

    c_len = sizeof(c_addr);

    if ((len = recvfrom(policy_server_udp_socket, zte_upgrade_buffer,
                        sizeof(zte_upgrade_buffer), 0 , (struct sockaddr *)&c_addr, &c_len)) < 0  ) {
        goto END;
    }

    if (len == 0) {
        goto END;
    }

    pdu = (zte_udp_scan_or_start_cmd_packet_t *)zte_upgrade_buffer;

    if ( memcmp(pdu->check_signal, ZTE_SIGNAL, 6 ) ) {
        IROS_LOG_MAJ(IROS_MID_UPGRADE, "check zte sigal error\r\n " );
        goto END;
    }


    message_version = pdu->message_version;
    return_value = pdu->return_value;
    ext_flag = ntohs(pdu->ext_flag);
    command_id = ntohs(pdu->command_id);
    arguments = ntohs(pdu->arguments);
    data_length = ntohs(pdu->data_length);
    checksum = ntohl(pdu->checksum);
    reserved = ntohl(pdu->reserved);

    if ( len - sizeof (zte_udp_scan_or_start_cmd_packet_t) + 1 < data_length  ) {
        /*tlv length is error, it's server bug, it must make a round*/
        data_length = len - sizeof (zte_udp_scan_or_start_cmd_packet_t) + 1;
    }

    zte_save_tlv_value(command_id, pdu->data, data_length);


    /*deal with data*/
    switch (  command_id ) {
        case    ZTE_UDP_SCAN_CMD:
            IROS_LOG_MAJ(IROS_MID_UPGRADE, " receive ZTE_UDP_SCAN_CMD message \r\n " );

            if (  zte_manage_upgrade.state == RECEIVED_SCAN_ONU_CMD ) {
                goto END;
            }

            if (zte_manage_upgrade.policy_server_tcp_socket == 0 ) {
                zte_manage_upgrade.policy_server_tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

                if (zte_manage_upgrade.policy_server_tcp_socket < 0) {
                    zte_manage_upgrade.policy_server_tcp_socket = 0;
                    IROS_LOG_MAJ(IROS_MID_UPGRADE,
                                 "manage upgrade:create policy socket failed \r\n" );
                    goto END;
                }

                if (setsockopt(zte_manage_upgrade.policy_server_tcp_socket, SOL_SOCKET,
                               SO_REUSEADDR, &one, sizeof(one))) {
                    IROS_LOG_MAJ(IROS_MID_UPGRADE,
                                 "set policy server socket with SO_REUSEADDR error \r\n");
                    zte_recover_init_state(1, 0 );
                    goto END;
                }

                if (setsockopt(zte_manage_upgrade.policy_server_tcp_socket, SOL_SOCKET,
                               SO_REUSEPORT, &one, sizeof(one))) {
                    zte_recover_init_state(1, 0 );
                    IROS_LOG_MAJ(IROS_MID_UPGRADE,
                                 "set policy server socket with SO_REUSEPORT error \r\n");
                    goto END;
                }
            }

            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_len = sizeof(local);
            local.sin_port = htons(zte_manage_upgrade.policy_server_l4_port);
            local.sin_addr.s_addr = htonl( zte_manage_upgrade.policy_server_ip);

            if (connect(zte_manage_upgrade.policy_server_tcp_socket,
                        (struct sockaddr *)&local, sizeof(local)) < 0) {
                IROS_LOG_MAJ(IROS_MID_UPGRADE,
                             "mange upgrade: Can't connect to policy server target \r\n");
                perror("connect");
                zte_recover_init_state(1, 0 );
                goto END;
            }

            /* zte_manage_upgrade.policy_server_tcp_socket=policy_server_tcp_socket; */
            send_data_len = zte_build_information_packet(   );

            /*send a TCP response*/
            if ((len = write(zte_manage_upgrade.policy_server_tcp_socket ,
                             zte_upgrade_buffer, send_data_len)) < 0) {
                IROS_LOG_MAJ(IROS_MID_UPGRADE,
                             "send reponse failed after received scan ONU message\r\n ");
            }
            else {
                IROS_LOG_MAJ(IROS_MID_UPGRADE, "send TCP response for scan ONU \r\n " );

                if ((len = read(zte_manage_upgrade.policy_server_tcp_socket ,
                                zte_upgrade_buffer , ZTE_MAX_BUFFER)) <= 0) {
                    IROS_LOG_MAJ(IROS_MID_UPGRADE,
                                 "receive policy sever TCP reponse failed \r\n " );
                    zte_recover_init_state( 1, 0);
                }
                else {
                    zte_manage_upgrade.state = RECEIVED_SCAN_ONU_CMD;
                    zte_manage_upgrade.state_record.valid = 1;
                    zte_manage_upgrade.state_record.state = RECEIVED_SCAN_ONU_CMD;
                    zte_manage_upgrade.state_record.age = 120;

                    ca_timer_add((epon_timer_t *)&zte_manage_upgrade.zte_monitor_state_timer,
                                 zte_upgrade_tmfunc_match_event, 10);
                }
            }


            break;

        case    ZTE_UDP_START_CMD:
            IROS_LOG_MAJ(IROS_MID_UPGRADE, " receive ZTE_UDP_START_CMD message \r\n " );

            zte_parse_new_version(zte_manage_upgrade.version_server_file_path);

            if (zte_manage_upgrade.version_server_tcp_socket == 0 ) {
                zte_manage_upgrade.version_server_tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

                if (zte_manage_upgrade.version_server_tcp_socket < 0) {
                    IROS_LOG_MAJ(IROS_MID_UPGRADE, "create version socket failed \r\n" );
                    zte_recover_init_state(1, 0);
                    goto END;
                }

                if (setsockopt(zte_manage_upgrade.version_server_tcp_socket, SOL_SOCKET,
                               SO_REUSEADDR, &one, sizeof(one))) {
                    IROS_LOG_MAJ(IROS_MID_UPGRADE,
                                 "set version server socket with SO_REUSEADDR error \r\n");
                    zte_recover_init_state(1, 1 );
                    goto END;
                }

                if (setsockopt(zte_manage_upgrade.version_server_tcp_socket, SOL_SOCKET,
                               SO_REUSEPORT, &one, sizeof(one))) {
                    IROS_LOG_MAJ(IROS_MID_UPGRADE,
                                 "set version server socket with SO_REUSEPORT error \r\n");
                    zte_recover_init_state(1, 1 );
                    goto END;
                }
            }

            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_len = sizeof(local);
            local.sin_port = htons(zte_manage_upgrade.version_server_l4_port);
            local.sin_addr.s_addr = htonl( zte_manage_upgrade.version_server_ip);

            if (connect(zte_manage_upgrade.version_server_tcp_socket,
                        (struct sockaddr *)&local, sizeof(local)) < 0) {
                IROS_LOG_MAJ(IROS_MID_UPGRADE, "Can't connect to version sever target\r\n " );
                zte_recover_init_state(1, 1 );

                goto END;
            }

            send_data_len = zte_build_request_download_firmware( );

            /*send a TCP reponse*/
            if ((len = write(zte_manage_upgrade.version_server_tcp_socket ,
                             zte_upgrade_buffer, send_data_len)) < 0) {
                IROS_LOG_MAJ(IROS_MID_UPGRADE,
                             "send tcp reponse failed for start upgrade message \r\n ");
            }
            else {
                zte_manage_upgrade.state = RECEIVED_ENABLE_UPGRADE_CMD;
            }

            break;

    }

END:

    return 0;
}

static oam_int32  zte_proc_upgrade_firmware_state(      )
{
    oam_int32  version_server_tcp_socket = 0;
    int cur_blob_idx = current_blob_desc.blob_idx;
    char *flash_des = NULL;
    int blob_idx = 0x1 & (cur_blob_idx + 1);;
    blob_info_desc_t *blob_info = blob_info_tab[blob_idx];
    int len = 0;
    oam_int8      *p_str = NULL;
    int part_index = 0;
    oam_int32    ret = 0;
    oam_uint32   send_data_len = 0;
    struct timeval    tv;
    oam_int32   receive_frame_offset_len = 0;
    oam_int32    i = 0;
    mif_upgrade_state_t  flash_state;


    part_index = blob_part_tab[blob_idx];
    flash_des = (char *)((unsigned int )blob_info + sizeof(blob_info_desc_t));

    version_server_tcp_socket = zte_manage_upgrade.version_server_tcp_socket;

    if ((len = read(version_server_tcp_socket , zte_upgrade_buffer ,
                    ZTE_MAX_BUFFER)) < 0) {
        zte_recover_init_state( 1, 1);
        IROS_LOG_MAJ(IROS_MID_UPGRADE, "receive version server failed\r\n" );

    }
    else {
        if ( len == 0) {
            /*send download  message to policy server*/
            if (zte_manage_upgrade.image_len == 0) {
                zte_recover_init_state( 1, 1 );
                goto END;
            }


            /*write the new version*/
            epon_onu_set_onu_sw_version(zte_manage_upgrade.onu.new_version_no
                                        , strlen(zte_manage_upgrade.onu.new_version_no));

            /* write control block of this blob */
            diag_printf ("\r\n download success, ready to save flash ,  image length=%d\r\n",
                         zte_manage_upgrade.image_len);
            //zte_write_flash_control_blob(zte_manage_upgrade.image_len, blob_info, flash_des, part_index);

            ret = mif_upgrade (3, zte_upgrade_buffer, 0, &flash_state, true);

            if (ret == 0 && flash_state.tlv_update_successed & 0x1) {

                /*save flash*/
                zte_manage_upgrade.flash_cfg.policy_server_ip =
                    zte_manage_upgrade.policy_server_ip;
                zte_manage_upgrade.flash_cfg.policyserverPort =
                    zte_manage_upgrade.policy_server_l4_port;
                zte_manage_upgrade.flash_cfg.updatestatus = 1;
                strcpy( zte_manage_upgrade.flash_cfg.old_version_no,
                        zte_manage_upgrade.onu.new_version_no);
                zte_flash_cfg_set(  );

                send_data_len = zte_build_response_download_firmware_result(0);

                if ((len = write(zte_manage_upgrade.policy_server_tcp_socket ,
                                 zte_upgrade_buffer, send_data_len)) < 0) {
                    IROS_LOG_MAJ(IROS_MID_UPGRADE,
                                 "warning:send response download firmware message failed\r\n");
                }

                /*reset ONU*/
                zte_reset_onu( );
            }
            else {

                send_data_len = zte_build_response_download_firmware_result(1);

                if ((len = write(zte_manage_upgrade.policy_server_tcp_socket ,
                                 zte_upgrade_buffer, send_data_len)) < 0) {
                    IROS_LOG_MAJ(IROS_MID_UPGRADE,
                                 "warning:send response download firmware message failed\r\n");
                }

                zte_recover_init_state( 1, 1 );

            }

        }
        else {
            receive_frame_offset_len = 0;

            if (zte_manage_upgrade.strart_receive_image_flag == 0) {
                epon_timer_del((epon_timer_t *)&zte_manage_upgrade.zte_monitor_state_timer);
                zte_upgrade_buffer[len] = '\0';
                p_str = strstr(zte_upgrade_buffer , "200 OK");

                if ( p_str != NULL) {
                    p_str = strstr(zte_upgrade_buffer , "\r\n\r\n");

                    if (p_str != NULL) {
                        zte_manage_upgrade.strart_receive_image_flag = 1;
                        /*set receive frame timeout*/
                        tv.tv_sec = 60;
                        tv.tv_usec = 0;
                        setsockopt(version_server_tcp_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

                        for ( i = 0; i < len - 3; i++) {
                            if ((zte_upgrade_buffer[i] == '\r')
                                && (zte_upgrade_buffer[i + 1] == '\n')
                                && (zte_upgrade_buffer[i + 2] == '\r')
                                && (zte_upgrade_buffer[i + 3] == '\n' )) {
                                receive_frame_offset_len = i + 4;
                                break;
                            }
                        }

                        if ( receive_frame_offset_len < len) {
                            len = len - receive_frame_offset_len;
                            /*have firmware data */

                            IROS_LOG_MAJ(IROS_MID_UPGRADE,
                                         "\r\n strart data: %02x %02x %02x %02x %02x %02x \r\n"
                                         , zte_upgrade_buffer[receive_frame_offset_len]
                                         , zte_upgrade_buffer[receive_frame_offset_len + 1]
                                         , zte_upgrade_buffer[receive_frame_offset_len + 2]
                                         , zte_upgrade_buffer[receive_frame_offset_len + 3]
                                         , zte_upgrade_buffer[receive_frame_offset_len + 4]
                                         , zte_upgrade_buffer[receive_frame_offset_len + 5]);
                            diag_printf ("ready download image, offset=%d, len=%d :\r\n",
                                         receive_frame_offset_len, len);

                        }
                        else {
                            diag_printf ("it is server response, omit it !!!\r\n");
                            goto END;
                        }
                    }
                    else {


                    }

                }
                else {
                    diag_printf ("it is server response, omit it \r\n");
                    goto END;
                }
            }

            diag_printf(".....");

            /*write data block to flash*/
            if (zte_manage_upgrade.received_blocks == 0) {
                //Geesun check and remove the signature
                char sigChar[MAX_SIGNATURE_LENGTH];
                memset(sigChar, 0, sizeof(sigChar));
                memcpy(sigChar, zte_upgrade_buffer + receive_frame_offset_len,
                       SIGNATURE_STR_LEN);

                if (strcmp(SIGNATURE_STR2, sigChar)) {
                    //Bad signature
                    send_data_len = zte_build_response_download_firmware_result(1);

                    if ((len = write(zte_manage_upgrade.policy_server_tcp_socket ,
                                     zte_upgrade_buffer, send_data_len)) < 0) {
                        IROS_LOG_MAJ(IROS_MID_UPGRADE, "warning:send message Failed  \r\n");
                    }

                    IROS_LOG_MAJ(IROS_MID_UPGRADE, "warning:The image is not ZTE onu Image\r\n");
                    zte_recover_init_state( 1, 1 );
                    goto END;
                }
                else {
                    receive_frame_offset_len += SIGNATURE_LEN;
                    len -= SIGNATURE_LEN;
                }

                ret = prepare_mif_upgrade(3, false);

                if (ret != 0) {
                    zte_recover_init_state( 1, 1 );
                    goto END;
                }

                zte_manage_upgrade.received_blocks = 1;

            }

            //printf("wirte a block to flash \n");
            ret = mif_upgrade (3, zte_upgrade_buffer + receive_frame_offset_len, len,
                               &flash_state, false);

            if (ret != 0 ) {
                send_data_len = zte_build_response_download_firmware_result(1);

                if ((len = write(zte_manage_upgrade.policy_server_tcp_socket ,
                                 zte_upgrade_buffer, send_data_len)) < 0) {
                    IROS_LOG_MAJ(IROS_MID_UPGRADE,
                                 "warning:send response download firmware message failed\r\n");
                }

                zte_recover_init_state( 1, 1 );
            }

            //flash_write(flash_des + zte_manage_upgrade.image_len, zte_upgrade_buffer+receive_frame_offset_len, len);

            zte_manage_upgrade.image_len += len;

        }


    }


END:

    return 0;


}


static oam_int32  zte_build_information_packet(     )
{
    oam_int32    count = 0;

    /*send a TCP reponse*/
    /*
    ¡G"GET /ReportVersion.servlet?status=SendVersion&deviceType=d42x&productMAC=00:d0:d0:9a:47:29&deviceSN=00000000000000000000000000
    000000&deviceID=00000000000000000000000000000000&deviceIP=192.168.1.1&oldVersionNo=V2.0.0T6 HTTP/1.1..Host: 192.168.1.100:5678..
    Content-Type: text/xml; charset="utf-8"..Keep-Alive: ..Connection: TE, Keep-Alive..TE: trailers...¡±
    */


    zte_onu_device_infor_get(&zte_manage_upgrade.onu);

    count += sprintf(zte_upgrade_buffer + count, "%s" ,
                     "GET /ReportVersion.servlet?status=SendVersion");
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&deviceType=",
                     zte_manage_upgrade.onu.device_type);
    count += sprintf(zte_upgrade_buffer + count, "%s%02x:%02x:%02x:%02x:%02x:%02x" ,
                     "&productMAC="
                     , zte_manage_upgrade.onu.product_mac[0]
                     , zte_manage_upgrade.onu.product_mac[1]
                     , zte_manage_upgrade.onu.product_mac[2]
                     , zte_manage_upgrade.onu.product_mac[3]
                     , zte_manage_upgrade.onu.product_mac[4]
                     , zte_manage_upgrade.onu.product_mac[5]);

    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&deviceSN=",
                     zte_manage_upgrade.onu.device_sn);
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&deviceID=",
                     zte_manage_upgrade.onu.device_id);
    count += sprintf(zte_upgrade_buffer + count, "%s%d.%d.%d.%d" , "&deviceIP="
                     , (zte_manage_upgrade.onu.device_ip >> 24) & 0xff
                     , (zte_manage_upgrade.onu.device_ip >> 16) & 0xff
                     , (zte_manage_upgrade.onu.device_ip >> 8) & 0xff
                     , (zte_manage_upgrade.onu.device_ip) & 0xff );

    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&hardwareVersionNo=",
                     zte_manage_upgrade.onu.hardware_version_no);
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&moduleName=",
                     "firmware");
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&oldVersionNo=",
                     zte_manage_upgrade.onu.old_version_no);


    count += sprintf(zte_upgrade_buffer + count, "%s" ,  " HTTP/1.0\r\n");
    count += sprintf(zte_upgrade_buffer + count, "Host: %d.%d.%d.%d:%d\r\n" ,
                     (zte_manage_upgrade.policy_server_ip >> 24) & 0xff
                     , (zte_manage_upgrade.policy_server_ip >> 16) & 0xff
                     , (zte_manage_upgrade.policy_server_ip >> 8) & 0xff
                     , zte_manage_upgrade.policy_server_ip  & 0xff
                     , zte_manage_upgrade.policy_server_l4_port);

    count += sprintf(zte_upgrade_buffer + count, "%s%s" ,
                     "Content-Type: text/xml; ", "charset=\"utf-8\"\r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" , "Keep-Alive: \r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" ,
                     "Connection: TE, Keep-Alive\r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" , "TE: trailers\r\n\r\n");

    return count;
}

static oam_int32  zte_build_request_download_firmware(     )
{
    oam_int32    count = 0;

    /*
    GET /DownloadVersion.servlet?status=DownloadFile&deviceID=00000000000000000000000000000000
    &deviceIP=192.168.1.1&versionFilePath=d42x/V2.0.0T4/d42xver HTTP/1.1..Host: 192.168.1.100:45678..
    Content-Type: text/xml; charset="utf-8"..Keep-Alive: ..Connection: TE, Keep-Alive..TE: trailers....
    */


    count += sprintf(zte_upgrade_buffer + count, "%s" ,
                     "GET /DownloadVersion.servlet?status=DownloadFile");
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&deviceID=",
                     zte_manage_upgrade.onu.device_id);
    count += sprintf(zte_upgrade_buffer + count, "%s%d.%d.%d.%d" , "&deviceIP="
                     , (zte_manage_upgrade.onu.device_ip >> 24) & 0xff
                     , (zte_manage_upgrade.onu.device_ip >> 16) & 0xff
                     , (zte_manage_upgrade.onu.device_ip >> 8) & 0xff
                     , (zte_manage_upgrade.onu.device_ip) & 0xff );

    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&versionFilePath=",
                     zte_manage_upgrade.version_server_file_path);
    count += sprintf(zte_upgrade_buffer + count, "%s" , " HTTP/1.0\r\n");
    count += sprintf(zte_upgrade_buffer + count, "Host: %d.%d.%d.%d:%d\r\n" ,
                     (zte_manage_upgrade.version_server_ip >> 24) & 0xff
                     , (zte_manage_upgrade.version_server_ip >> 16) & 0xff
                     , (zte_manage_upgrade.version_server_ip >> 8) & 0xff
                     , zte_manage_upgrade.version_server_ip  & 0xff
                     , zte_manage_upgrade.version_server_l4_port);
    count += sprintf(zte_upgrade_buffer + count, "%s%s" ,
                     "Content-Type: text/xml; ", "charset=\"utf-8\"\r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" , "Keep-Alive: \r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" ,
                     "Connection: TE, Keep-Alive\r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" , "TE: trailers\r\n\r\n");


    return count;


}


static oam_int32  zte_build_response_download_firmware_result ( oam_int8 result)
{
    oam_int32    count = 0;

    /*
    GET /ReportVersion.servlet?status=DownloadSuccess&deviceType=d42x&productMAC=00:d0:d0:9a:47:29
    &deviceSN=00000000000000000000000000000000&deviceID=00000000000000000000000000000000
    &deviceIP=192.168.1.1&hardwareVersionNo=D420V2&oldVersionNo=V2.0.0T6&newVersionNo=V2.0.0T4 HTTP/1.1..
    Host: 192.168.1.100:5678..Content-Type: text/xml; charset="utf-8"..Keep-Alive: ..Connection: TE, Keep-Alive..
    TE: trailers....
    */

    count += sprintf(zte_upgrade_buffer + count, "%s%s"
                     , "GET /ReportVersion.servlet?status="
                     , result == 0 ? "DownloadSuccess" : "DownloadFail");
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&deviceType=",
                     zte_manage_upgrade.onu.device_type);
    count += sprintf(zte_upgrade_buffer + count, "%s%02x:%02x:%02x:%02x:%02x:%02x" ,
                     "&productMAC="
                     , zte_manage_upgrade.onu.product_mac[0]
                     , zte_manage_upgrade.onu.product_mac[1]
                     , zte_manage_upgrade.onu.product_mac[2]
                     , zte_manage_upgrade.onu.product_mac[3]
                     , zte_manage_upgrade.onu.product_mac[4]
                     , zte_manage_upgrade.onu.product_mac[5]);

    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&deviceSN=",
                     zte_manage_upgrade.onu.device_sn);
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&deviceID=",
                     zte_manage_upgrade.onu.device_id);
    count += sprintf(zte_upgrade_buffer + count, "%s%d.%d.%d.%d" , "&deviceIP="
                     , (zte_manage_upgrade.onu.device_ip >> 24) & 0xff
                     , (zte_manage_upgrade.onu.device_ip >> 16) & 0xff
                     , (zte_manage_upgrade.onu.device_ip >> 8) & 0xff
                     , (zte_manage_upgrade.onu.device_ip) & 0xff );
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&hardwareVersionNo=",
                     zte_manage_upgrade.onu.hardware_version_no);
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&oldVersionNo=",
                     zte_manage_upgrade.onu.old_version_no);
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&newVersionNo=",
                     zte_manage_upgrade.onu.new_version_no);
    count += sprintf(zte_upgrade_buffer + count, "%s" ,  " HTTP/1.0\r\n");
    count += sprintf(zte_upgrade_buffer + count, "Host: %d.%d.%d.%d:%d\r\n" ,
                     (zte_manage_upgrade.policy_server_ip >> 24) & 0xff
                     , (zte_manage_upgrade.policy_server_ip >> 16) & 0xff
                     , (zte_manage_upgrade.policy_server_ip >> 8) & 0xff
                     , zte_manage_upgrade.policy_server_ip  & 0xff
                     , zte_manage_upgrade.policy_server_l4_port);
    count += sprintf(zte_upgrade_buffer + count, "%s%s" ,
                     "Content-Type: text/xml; ", "charset=\"utf-8\"\r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" , "Keep-Alive: \r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" ,
                     "Connection: TE, Keep-Alive\r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" , "TE: trailers\r\n\r\n");



    return count;

}

static oam_int32  zte_build_upgrade_firmware_success ( )
{
    /*
    GET /ReportVersion.servlet?status=UpdateSuccess&deviceType=d42x&deviceMAC=00:d0:d0:9a:47:29
    &deviceSN=00000000000000000000000000000000&deviceID=00000000000000000000000000000000
    &deviceIP=192.168.1.1&oldVersionNo=V2.0.0T6&newVersionNo=V2.0.0T4 HTTP/1.1..
    Host: 192.168.1.100:5678..Content-Type: text/xml; charset="utf-8"..Keep-Alive: ..
    Connection: TE, Keep-Alive..TE: trailers....
    */

    oam_int32    count = 0;
    zte_onu_device_infor_get(&zte_manage_upgrade.onu);

#ifdef HAVE_EXT_SW_DRIVER
    strcpy(zte_manage_upgrade.onu.new_version_no,  tw_onu_sw_ver );
#else
    strcpy( zte_manage_upgrade.onu.new_version_no,
            ZTE_1PORT_ONU_SOFTWARE_VERSION );
#endif


    count += sprintf(zte_upgrade_buffer + count, "%s" ,
                     "GET /ReportVersion.servlet?status=UpdateSuccess");
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&deviceType=",
                     zte_manage_upgrade.onu.device_type);
    count += sprintf(zte_upgrade_buffer + count, "%s%02x:%02x:%02x:%02x:%02x:%02x" ,
                     "&productMAC="
                     , zte_manage_upgrade.onu.product_mac[0]
                     , zte_manage_upgrade.onu.product_mac[1]
                     , zte_manage_upgrade.onu.product_mac[2]
                     , zte_manage_upgrade.onu.product_mac[3]
                     , zte_manage_upgrade.onu.product_mac[4]
                     , zte_manage_upgrade.onu.product_mac[5]);
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&deviceSN=",
                     zte_manage_upgrade.onu.device_sn);

    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&deviceID=",
                     zte_manage_upgrade.onu.device_id);
    count += sprintf(zte_upgrade_buffer + count, "%s%d.%d.%d.%d" , "&deviceIP="
                     , (zte_manage_upgrade.onu.device_ip >> 24) & 0xff
                     , (zte_manage_upgrade.onu.device_ip >> 16) & 0xff
                     , (zte_manage_upgrade.onu.device_ip >> 8) & 0xff
                     , (zte_manage_upgrade.onu.device_ip) & 0xff );
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&oldVersionNo=",
                     zte_manage_upgrade.flash_cfg.old_version_no);
    count += sprintf(zte_upgrade_buffer + count, "%s%s" , "&newVersionNo=",
                     zte_manage_upgrade.onu.new_version_no);
    count += sprintf(zte_upgrade_buffer + count, "%s" ,  " HTTP/1.0\r\n");
    count += sprintf(zte_upgrade_buffer + count, "Host: %d.%d.%d.%d:%d\r\n" ,
                     (zte_manage_upgrade.flash_cfg.policy_server_ip >> 24) & 0xff
                     , (zte_manage_upgrade.flash_cfg.policy_server_ip >> 16) & 0xff
                     , (zte_manage_upgrade.flash_cfg.policy_server_ip >> 8) & 0xff
                     , zte_manage_upgrade.flash_cfg.policy_server_ip  & 0xff
                     , zte_manage_upgrade.flash_cfg.policyserverPort);

    count += sprintf(zte_upgrade_buffer + count, "%s%s" ,
                     "Content-Type: text/xml; ", "charset=\"utf-8\"\r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" , "Keep-Alive: \r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" ,
                     "Connection: TE, Keep-Alive\r\n");
    count += sprintf(zte_upgrade_buffer + count, "%s" , "TE: trailers\r\n\r\n");


    return count;

}

static oam_int32  zte_save_tlv_value( oam_int32 command_id, oam_int8 *data,
                                     oam_int32 data_length)
{
    oam_int32    i = 0;
    zte_server_tlv_t     *entry = NULL;
    oam_uint16  type = 0;
    oam_uint16  length = 0;
    oam_int32    value = 0;
    oam_int32    j = 0;

    for (i = 0; i < data_length; i = i + sizeof (zte_server_tlv_t) - 1 + length) {
        entry = ( zte_server_tlv_t *) (data + i);
        type = ntohs(entry->type);
        length = ntohs(entry->length);
        value = 0;

        if ( length < 5) {
            for ( j = 1; j < length + 1; j ++) {
                value += (entry->value[j - 1] << ((length - j) * 8) ) & (0xff << ((
                             length - j) * 8));
            }
        }

        if ( command_id == ZTE_UDP_SCAN_CMD) {
            switch ( type )  {
                case        POLICY_SERVER_IP_TYPE:
                    zte_manage_upgrade.policy_server_ip = value;
                    break;

                case        POLICY_SERVER_L4_PORT_TYPE:
                    zte_manage_upgrade.policy_server_l4_port = value;
                    break;

                case        POLICY_SERVER_TIMEOUT_TYPE:
                    zte_manage_upgrade.policy_server_timeout = value;
                    break;

                case        MC_OR_BC_START_IP_TYPE:
                    zte_manage_upgrade.mc_or_bc_start_ip = value;
                    break;

                case        MC_OR_BC_END_IP_TYPE:
                    zte_manage_upgrade.mc_or_bc_end_ip = value;
                    break;

                default:
                    IROS_LOG_MAJ(IROS_MID_UPGRADE, " unkown type=%d in %s %d\r\n ", type,
                                 __FUNCTION__, __LINE__);
                    break;

            }
        }

        if ( command_id == ZTE_UDP_START_CMD) {
            switch ( type )  {
                case        VERSION_SERVER_IP_TYPE:
                    zte_manage_upgrade.version_server_ip = value;
                    break;

                case        VERSION_SERVER_L4_PORT_TYPE:
                    zte_manage_upgrade.version_server_l4_port = value;
                    break;

                case        VERSION_SERVER_FILE_PATH_TYPE:
                    strcpy( zte_manage_upgrade.version_server_file_path , entry->value);

                    break;

                case        VERSION_SERVER_FILE_LENGTH:
                    zte_manage_upgrade.version_server_file_len = value;
                    break;



                default:
                    IROS_LOG_MAJ(IROS_MID_UPGRADE, " unkown type=%d in %s %d\r\n ", type,
                                 __FUNCTION__, __LINE__);
                    break;

            }
        }





    }

    return 0;

}

oam_int32    zte_onu_device_infor_get( zte_onu_device_infor_t     *infor  )
{
    cyg_uint32 inaddr_local = 0;
    cyg_uint32 inaddr_gateway = 0;
    cyg_uint32 inaddr_mask = 0;
    oam_int8      *p_str = NULL;
    oam_int8     software_version[32];


    strcpy( infor->status, ZTE_STATE);
    strcpy(  infor->device_type, ZTE_DEVICE_TYPE);
    //lynxd clean onu_aal_get_local_mac( infor->product_mac);


    sprintf(infor->device_sn, "%02x%02x%02x%02x%02x%02x"
            , infor->product_mac[0],  infor->product_mac[1],  infor->product_mac[2]
            , infor->product_mac[3], infor->product_mac[4], infor->product_mac[5]);

#ifdef HAVE_EXT_SW_DRIVER
    strcpy( software_version,  tw_onu_sw_ver );
#else
    strcpy(software_version,  ZTE_1PORT_ONU_SOFTWARE_VERSION);
#endif

    p_str = strstr(software_version, "_" );

    if (p_str != NULL) {
        *p_str = '\0';
    }

    sprintf (zte_manage_upgrade.onu.device_id, "%s_%s"
             , zte_manage_upgrade.onu.device_sn , software_version);


    get_ip_config(&inaddr_local, &inaddr_gateway,  & inaddr_mask);
    infor->device_ip = inaddr_local ;
#ifdef HAVE_EXT_SW_DRIVER
    strcpy( infor->hardware_version_no, tw_onu_hw_ver );
    strcpy( infor->old_version_no,  tw_onu_sw_ver );
#else
    strcpy( infor->hardware_version_no, ZTE_1PORT_ONU_HARDWARE_VERSION );
    strcpy( infor->old_version_no,  ZTE_1PORT_ONU_SOFTWARE_VERSION );
#endif


    return 0;

}

oam_int32 zte_write_flash_control_blob(oam_uint32   image_len ,
                                      blob_info_desc_t *blob_info,  oam_int8 *flash_des , int part_index  )
{
    blob_info_desc_t info;
    int cur_blob_idx = current_blob_desc.blob_idx;

    info.head_signature = BLOB_INFO_HEAD_SIGNATURE;
    info.len = image_len;
    info.blob_start_ptr = (struct image_blob_header_t *) flash_des;
    info.tail_signature = BLOB_INFO_TAIL_SIGNATURE;
    info.upgrade_stamp = blob_info_tab[cur_blob_idx]->upgrade_stamp + 1;
    info.is_not_confirmed = 0;
    info.is_not_executed = 0;

    flash_write((char *)&blob_info->head_signature, (char *)&info.head_signature,
                sizeof(info.head_signature));
    flash_write((char *)&blob_info->upgrade_stamp, (char *)&info.upgrade_stamp,
                sizeof(info.upgrade_stamp));
    flash_write((char *)&blob_info->len, (char *)&info.len, sizeof(info.len));
    flash_write((char *)&blob_info->blob_start_ptr, (char *)&info.blob_start_ptr,
                sizeof(info.blob_start_ptr));
    flash_write((char *)&blob_info->tail_signature, (char *)&info.tail_signature,
                sizeof(info.tail_signature));
    flash_write((char *)&blob_info->is_not_confirmed,
                (char *)&info.is_not_confirmed, sizeof(info.is_not_confirmed));
    flash_write((char *)&blob_info->is_not_executed, (char *)&info.is_not_executed,
                sizeof(info.is_not_executed));

    flash_part_write_done(part_index);


    return 0;


}

static oam_int32    zte_flash_cfg_set( )
{

    oam_int32    ret = 0;



    write_global_to_flash();


    return ret;

}



static oam_int32   zte_send_message_to_server(oam_int32 socket_id,
        oam_int32  server_ip, oam_int32  l4_port
        , oam_int8 *data, oam_int32 data_len,  oam_int32     *output_socket_id  )
{
    oam_int32    ret = 1;
    struct sockaddr_in local;
    oam_int32   len = 0;
    oam_int32 one = 1;

    if (socket_id == 0) {
        socket_id = socket(AF_INET, SOCK_STREAM, 0);

        if (socket_id < 0) {
            IROS_LOG_MAJ(IROS_MID_UPGRADE, "create stream socket failed\r\n");
            goto END;
        }

        * output_socket_id = socket_id;

        if (setsockopt(socket_id , SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
            IROS_LOG_MAJ(IROS_MID_UPGRADE, "setsockopt /source/ SO_REUSEADDR error");
            goto END;
        }

        if (setsockopt(socket_id , SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one))) {
            IROS_LOG_MAJ(IROS_MID_UPGRADE, "setsockopt /source/ SO_REUSEPORT error");
            goto END;
        }


        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_len = sizeof(local);
        local.sin_port = htons(l4_port);
        local.sin_addr.s_addr = htonl( server_ip);

        if (connect(socket_id, (struct sockaddr *)&local, sizeof(local)) < 0) {
            IROS_LOG_MAJ(IROS_MID_UPGRADE, "Can't connect to target\r\n");
            perror("connect");
            close(socket_id );
            goto END;
            /*it must do a loop*/
        }

    }

    /*send a TCP reponse*/
    if ((len = write(socket_id , data, data_len)) < 0) {
        IROS_LOG_MAJ(IROS_MID_UPGRADE, "send policy server TCP message failed");
    }
    else {

        ret = 0;
    }

END:

    return ret;

}

static oam_int32    zte_reset_onu ( )
{
    cyg_thread_delay(1000);
    IROS_LOG_MAJ(IROS_MID_UPGRADE, "reboot ONU\n");
    hal_delay_us(10000);
    HAL_PLATFORM_RESET();

    return 0;
}

static oam_int32     zte_parse_new_version(oam_int8   *version_path)
{

    oam_int8        *old_point = NULL;
    oam_int8        *new_point = NULL;
    oam_int8        *temp_point = NULL;
    oam_int32    str_len = 0;

    str_len = strlen(version_path);

    old_point = version_path;
    new_point = version_path;
    temp_point = version_path;

    zte_manage_upgrade.onu.new_version_no[0] = '\0';

    for ( ; ;  ) {
        temp_point = strstr(new_point, "/");

        if (temp_point != NULL) {
            old_point = new_point;
            new_point = temp_point + 1;

            if ( *new_point == '\0') {
                break;
            }
        }
        else {
            str_len = (oam_int32 )(new_point - old_point) ;

            if (str_len > 2) {
                memcpy(zte_manage_upgrade.onu.new_version_no, old_point, str_len - 1 );
                zte_manage_upgrade.onu.new_version_no[str_len - 1] = '\0';
            }
            else {

            }

            break;
        }


    }

    return 0;

}

static oam_int32  zte_upgrade_proc_tick(  )
{
    zte_upgrade_state_record_t  *p_record = NULL;
    oam_int32    ret = 1;

    p_record = & zte_manage_upgrade.state_record;

    if (p_record->valid ) {
        if (p_record->age) {
            p_record->age--;

            if (p_record->age == 0) {
                if (  zte_manage_upgrade.state == RECEIVED_SCAN_ONU_CMD) {
                    IROS_LOG_MAJ(IROS_MID_UPGRADE, "\r\n state recover to init for timeout \r\n");
                    zte_recover_init_state( 1, 1 );
                    ret = 0;
                }
            }
        }

    }

    return ret;

}

static void zte_upgrade_tmfunc_match_event(void *timer)
{
    oam_int32    ret = 0;

    ret = zte_upgrade_proc_tick( );

    if (ret != 0) {
        ca_timer_add((epon_timer_t *)&zte_manage_upgrade.zte_monitor_state_timer,
                     zte_upgrade_tmfunc_match_event, 10);

    }

}

static oam_int32   zte_recover_init_state( oam_int8  close_policy_socket_flag,
        oam_int8  close_version_socket_flag  )
{

    zte_manage_upgrade.state = INIT_STATE;

    if ( close_policy_socket_flag) {
        if ( zte_manage_upgrade.policy_server_tcp_socket != 0 ) {
            close( zte_manage_upgrade.policy_server_tcp_socket);
            zte_manage_upgrade.policy_server_tcp_socket = 0;
            zte_manage_upgrade.policy_server_l4_port = 0;
        }
    }

    if ( close_version_socket_flag ) {
        if ( zte_manage_upgrade.version_server_tcp_socket != 0 ) {
            close( zte_manage_upgrade.version_server_tcp_socket);
            zte_manage_upgrade.version_server_tcp_socket = 0;
            zte_manage_upgrade.version_server_l4_port = 0;
        }
    }

    zte_manage_upgrade.strart_receive_image_flag = 0;
    zte_manage_upgrade.received_blocks = 0;
    zte_manage_upgrade.recevied_frame_count = 0;
    zte_manage_upgrade.image_len = 0;


    return 0;

}

#endif /* #ifdef HAVE_ZTE_API */

