#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import sys
import time
#import datetime
import paramiko
import telnetlib
import getpass
import smtplib

from datetime import date
from configparser import ConfigParser
from email import encoders
from email.header import Header
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

def do_kernel_reboot(target):
    tn = telnetlib.Telnet("192.168.41.251", 2012, timeout=50)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'reboot\n')
    time.sleep(70)

    tn.write(b'\nroot\n')
    time.sleep(1)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'\nuname -a\n')
    time.sleep(1)
    #tn.write(b'exit\n')
    time.sleep(1)
    tn.write(b'\nls /dev/sd*\n')
    time.sleep(1)
    tn.write(b'\r\n')
    # Return log msg
    result_str = tn.read_very_eager()
    tn.close()
    return (result_str.decode('ascii', errors='ignore'))

def do_tftpboot(target, img_tag):
    uboot_tag = b'G3# '
    cmdline_tag = b'root@g3-eng:'
    written_ok = b'written: OK'
    tftp_done = b'done'
    tftpboot_mubi = b'tftpboot 0x85000000 major-image-g3-eng_' + img_tag.encode('ascii') + b'.mubi;'
    nand_erase = b'nand erase 0x500000 0xD000000;'
    nand_write = b'nand write 0x85000000 0x500000 0x${filesize};'

    tn = telnetlib.Telnet("192.168.41.251", 2012, timeout=50)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'root\n')
    time.sleep(1)
    i, tag, read_all = tn.expect([uboot_tag, cmdline_tag])
    # print(tag)
    # Reset from Uboot cmdline
    if i == 0:
        time.sleep(1)
        tn.write(b'reset\n')
    # Reboot from Kernel cmdline
    elif i == 1:
        time.sleep(1)
        tn.write(b'\r\n')
        time.sleep(1)
        tn.write(b'reboot\n')
    else:
        print("ERROR: do_tftpboot() capture log !")
    time.sleep(1)
    tn.read_until(b'Hit any key to stop autoboot: ')
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'setenv active_port 1;\r\n')
    tn.write(b'setenv serverip 192.168.1.128;\r\n')
    tn.write(b'setenv ipaddr 192.168.1.2;\r\n')
    tn.write(b'saveenv\r\n')
    time.sleep(1)
    
    # Upgrade mubi
    tn.read_until(uboot_tag)
    time.sleep(1)
    print(tftpboot_mubi)
    tn.write(tftpboot_mubi + b'\r\n')
    tn.read_until(tftp_done)
    time.sleep(1)
    print(nand_erase + nand_write)
    tn.write(nand_erase + nand_write + b'\r\n')
    tn.read_until(written_ok)
    time.sleep(5)
    
    # RESET board
    tn.write(b'\nreset\r\n')
    time.sleep(70)
    tn.write(b'\nroot\n')
    time.sleep(1)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'\nuname -a\n')
    time.sleep(1)
    #tn.write(b'exit\n')
    time.sleep(1)
    tn.write(b'\nls /dev/sd*\n')
    time.sleep(1)
    tn.write(b'\n')
    tn.write(b'\n')
    # Return log msg
    result_str = tn.read_very_eager()
    tn.close()
    return (result_str.decode('ascii', errors='ignore'))

def capture_log(current_path, target):
    log_txt = do_telnet(target)
    with open(log_file, 'w') as f:
        f.write(log_txt)

if __name__ == "__main__":
    log_file = "g3-usb3-phy-vbus-debug-log-20190222.txt"
    img_tag = ['unpatch', 'patched']

    #DO TEST process
    with open(log_file, 'w') as f:
        for i in range(len(img_tag)):
            log_txt= do_tftpboot('g3', img_tag[i])
            time.sleep(5)
            reboot_times = 1
            while (reboot_times <= 5):
                #print("***%s img reboot times [ %d ]***"%(img_tag[i], reboot_times))
                log_txt = do_kernel_reboot('g3')
                log_txt = ('\r\n*** '+img_tag[i]+ ' img reboot times  ['+ str(reboot_times) +'] ***\r\n') + log_txt
                reboot_times += 1
                time.sleep(10)
                f.write(log_txt)
    with open(log_file, 'r', encoding='ascii') as f:
        for line in f:
            if line.find('***') != -1:
                print(line.rstrip('\n'))
            if line.find('CPU Speed') != -1:
                print(line.rstrip('\n'))
            if line.find('GLOBAL_STRAP') != -1:
                print(line.rstrip('\n'))
            if line.find('SuperSpeed') != -1:
                print(line.rstrip('\n'))
            if line.find('Linux g3-eng') != -1:
                print(line.strip('\n'))
            if line.find('/dev/sd') != -1:
                print(line.rstrip('\n'))
