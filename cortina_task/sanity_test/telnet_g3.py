#!/usr/bin/env python3

# -*- coding: utf-8 -*-


import telnetlib
import time

HOST = "192.168.41.251"
PORT = 2012

activeport_set     = b'setenv active_port 1;\r\n'
serverip_set       = b'setenv serverip 192.168.1.128;\r\n'
ipaddr_set         = b'setenv ipaddr 192.168.1.2;\r\n'
saveenv_set        = b'saveenv;\r\n'
reset_set          = b'reset\r\n'

tftpboot_gpt       = b'tftpboot 0x84100000 major-image-g3-eng-gpt-nandinfo.img; nand erase 0x0 0x400000; nand write 0x84100000 0x0 0x300000;\r\n'
tftpboot_ubootenv  = b'tftpboot 0x84000000 g3-uboot-env.bin; nand erase 0x400000 0x100000; nand write 0x84000000 0x400000 0x20000;\r\n'
tftpboot_image     = b'tftpboot 0x85000000 major-image-g3-eng.mubi; nand erase 0x500000 0xD000000; nand write 0x85000000 0x500000 0x${filesize};\r\n'


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

    # result_str = tn.read_all()
    result_str = tn.read_very_eager()

#tn.read_until(b'g3-eng login: ')

    tn.write(b'exit\n')
    tn.close()
    return (result_str.decode('ascii', errors='ignore'))
    
if __name__ == '__main__':
    host = '192.168.41.251'
    log_txt = do_telnet(host)
    print(log_txt)

    with open('./logfile/g3_santy_testlog.txt', 'w') as f:
        f.write(log_txt)

