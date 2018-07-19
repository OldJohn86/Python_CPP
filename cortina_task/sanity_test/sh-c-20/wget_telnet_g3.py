#!/usr/bin/env python3

# -*- coding: utf-8 -*-
import telnetlib
import time
import os

from datetime import date

sanity_test_path = 'cd /home/pchen/daily_image_sanity_test;'
server_url       = 'http://192.168.65.235:18683/'
ubootenv         = 'uboot-env.bin'

activeport_set     = b'setenv active_port 1;\r\n'
serverip_set       = b'setenv serverip 192.168.1.128;\r\n'
ipaddr_set         = b'setenv ipaddr 192.168.1.2;\r\n'
saveenv_set        = b'saveenv;\r\n'
reset_set          = b'reset\r\n'

tftpboot_gpt       = b'tftpboot 0x84100000 major-image-g3-eng-gpt-nandinfo.img; nand erase 0x0 0x400000; nand write 0x84100000 0x0 0x300000;\r\n'
tftpboot_ubootenv  = b'tftpboot 0x84000000 g3-uboot-env.bin; nand erase 0x400000 0x100000; nand write 0x84000000 0x400000 0x20000;\r\n'
tftpboot_image     = b'tftpboot 0x85000000 major-image-g3-eng.mubi; nand erase 0x500000 0xD000000; nand write 0x85000000 0x500000 0x${filesize};\r\n'


# day = date.today()
y_m_d = date.today().strftime('%Y-%m-%d')
y_m = date.today().strftime('%Y-%m')
print(y_m_d)
print(y_m)

def do_wget(target, child=''):
    cmd_rm_img = sanity_test_path +' rm *'+target+'*;'+' rm *.dtb'
    print(cmd_rm_img)
    os.system(cmd_rm_img)

    if target == 'g3':
        target_img_type = 'g3-eng-major-image'
        target_gpt = 'major-image-g3-eng-gpt-nandinfo.img'
        target_img = 'major-image-g3-eng.mubi'
    elif target == 'saturn-sfu':
        if child == 'epon':
            target_img_type = 'saturn-sfu-eng_epon-major-image'
        elif child == 'gpon':
            target_img_type = 'saturn-sfu-eng_gpon-major-image'
        else:
            pass

        target_gpt = 'major-image-saturn-sfu-eng-gpt.img'
        target_img = 'uImage'
        dtb = 'uImage-saturn_sfu.dtb'
        rootfs = 'major-image-saturn-sfu-eng.rootfs.squashfs.ubi'
        ubi = 'major-image-saturn-sfu-eng_user.ubi'
    else:
        pass
    
    cmd_wget_gpt = sanity_test_path+' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+target_gpt
    print(cmd_wget_gpt)
    os.system(cmd_wget_gpt)

    cmd_wget_ubootenv = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+ubootenv
    print(cmd_wget_ubootenv)
    os.system(cmd_wget_ubootenv)
    
    cmd_rename_ubootenv = sanity_test_path + ' mv '+ubootenv+' '+target+'-'+ubootenv
    print(cmd_rename_ubootenv)
    os.system(cmd_rename_ubootenv)
    
    cmd_wget_img = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+target_img
    print(cmd_wget_img)
    os.system(cmd_wget_img)

    if target == 'saturn-sfu':
        cmd_wget_img = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+dtb
        print(cmd_wget_img)
        os.system(cmd_wget_img)

        cmd_wget_img = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+rootfs
        print(cmd_wget_img)
        os.system(cmd_wget_img)

        cmd_wget_img = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+ubi
        print(cmd_wget_img)
        os.system(cmd_wget_img)
    else:
        pass

def do_telnet(host):
    tn = telnetlib.Telnet(host, port=2012, timeout=50)
    
    # tn.read_until(b'login: ')
    # tn.write(username + b'\n')

    # tn.read_until(b'Password: ')
    # tn.write(password + b'\n')
    
    tn.write(b'root\n')
    time.sleep(1)
    tn.write(b'\r\n')
    time.sleep(1)

    tn.read_until(b'root@g3-eng:~# ')
    tn.write(b'reboot\n')
    time.sleep(1)

    tn.read_until(b'Hit any key to stop autoboot: ')
    tn.write(b'\r\n')
    time.sleep(1)

    tn.write(activeport_set)
    tn.write(serverip_set)
    tn.write(ipaddr_set)
    tn.write(saveenv_set)
    tn.write(reset_set)

    tn.read_until(b'Hit any key to stop autoboot: ')
    tn.write(b'\r\n')
    time.sleep(1)

    # Upgrade gpt
    tn.read_until(b'G3# ')
    tn.write(tftpboot_gpt)
    time.sleep(1)
    # Upgrade uboot_env
    tn.read_until(b'written: OK')
    tn.write(tftpboot_ubootenv)
    time.sleep(1)
    # Upgrade image
    tn.read_until(b'written: OK')
    tn.write(tftpboot_image)
    time.sleep(1)
    # RESET board
    tn.read_until(b'written: OK')
    tn.write(reset_set)

    time.sleep(50)
    tn.write(b'root\n')
    time.sleep(1)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'uname -a\n')
    time.sleep(1)
    tn.write(b'exit\n')
    time.sleep(1)

    # result_str = tn.read_all()
    result_str = tn.read_very_eager()
    tn.close()
    return (result_str.decode('ascii', errors='ignore'))


if __name__ == '__main__':
	# download images from server ip
    do_wget('g3')
    #do_wget('saturn-sfu', 'epon')

    log_txt = do_telnet('192.168.41.251')
    #print(log_txt)
    if os.path.exists('./logfile'):
        pass
    else:
        os.makedirs('./logfile')

    with open('./logfile/g3_santy_testlog'+y_m_d+'.txt', 'w') as f:
        f.write(log_txt)


