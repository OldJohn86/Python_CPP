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

def do_tftpboot(target, index):
    uboot_tag = b'SATURN# '
    cmdline_tag = b'root@saturn-sfu-eng:'
    written_ok = b'Written: OK'
    tftp_done = b'done'
    tftpboot_gpt = b'tftpboot 0x84100000 '+ index.encode('ascii') + b'_gpt-snand-data-20190215;'
    spi_nand_erase = b'spi_nand erase 0x0 0x400000;'
    spi_nand_write = b'spi_nand write 0x84100000 0x0 0x300000;'

    tn = telnetlib.Telnet("192.168.41.251", 2011, timeout=50)
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
    tn.write(b'setenv active_port 4;\r\n')
    tn.write(b'setenv serverip 192.168.1.129;\r\n')
    tn.write(b'setenv ipaddr 192.168.1.3;\r\n')
    tn.write(b'saveenv\r\n')
    time.sleep(1)
    
    # Upgrade gpt
    tn.read_until(uboot_tag)
    time.sleep(1)
    print(tftpboot_gpt)
    tn.write(tftpboot_gpt + b'\r\n')
    tn.read_until(uboot_tag)
    time.sleep(10)
    print(spi_nand_erase + spi_nand_write)
    tn.write(spi_nand_erase + spi_nand_write + b'\r\n')
    tn.read_until(written_ok)
    time.sleep(5)
    
    # RESET board
    tn.write(b'reset\r\n')
    time.sleep(40)
    tn.write(b'root\n')
    time.sleep(1)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'uname -a\n')
    time.sleep(1)
    #tn.write(b'exit\n')
    time.sleep(1)
    tn.write(b'cd /tmp\n')
    time.sleep(1)
    tn.write(b'tar zcvf lib.tgz /lib\n')
    time.sleep(140)
    tn.write(b'cp lib.tgz B.tgz\n')
    time.sleep(5)
    tn.write(b'\n')
    tn.write(b'cmp -l lib.tgz B.tgz\n')
    time.sleep(5)
    tn.write(b'\n')
    
    result_str = tn.read_very_eager()
    tn.close()
    return (result_str.decode('ascii', errors='ignore'))

def capture_log(current_path, target):
    log_txt = do_telnet(target)
    with open(log_file, 'w') as f:
        f.write(log_txt)

if __name__ == "__main__":
    log_file = "saturn_ssb_ddr_scan_log-20190215.txt"
    # Epon SSB DDR Scan test process
    with open(log_file, 'w') as f:
        for index in range(32):
            log_txt = do_tftpboot('saturn-sfu', str(index))
            time.sleep(10)
            f.write(log_txt)
    with open(log_file, 'r', encoding='ascii') as f:
        for line in f:
            if line.find('PLL_PI0') != -1:
                print(line)


