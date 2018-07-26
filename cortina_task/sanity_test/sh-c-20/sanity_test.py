#!/usr/bin/env python3

# -*- coding: utf-8 -*-
import os
import sys
import time
import datetime
import paramiko
import telnetlib
import getpass

from datetime import date
from configparser import ConfigParser


# Get Timestamp
#y_m_d = date.today().strftime('%Y-%m-%d')
#y_m = date.today().strftime('%Y-%m')
#print(y_m_d)
#print(y_m)

# 读取配置文件获取服务器的登录信息
def read_ini(config, option):
    info = dict()
    cf = ConfigParser()
    cf.read(config, encoding='utf-8')
    keys = cf.options(option)
    for each in keys:
        info[each] = cf.get(option, each)
    # print(info)
    return info

# 定义SSH SFTP Tool 工具类
class SftpTool(object):
    def __init__(self, username, password, port, host):
        self.user = username
        self.password = password
        self.port = port
        self.ip = host
    def connect(self, target_path_abs):
        try:
            self.ssh = paramiko.SSHClient()
            self.ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            self.ssh.connect(self.ip, self.port, self.user, self.password)
            print("Host[%s] connect created!!" % self.ip)
        except Exception as e:
            print("ERROR: Failed to connect to Host[%s]!!" % self.ip)
        self.stdin, self.stdout, self.stderr = self.ssh.exec_command("ls " + target_path_abs)
        stdout_str = self.stdout.read().decode('ascii')
        # print(result_str.split())
        return stdout_str.split()
    def input(self, local_file, remote_file):
        self.local_file_abs = local_file 
        self.remote_file_abs = remote_file
    def put(self):
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())
        sftp = self.ssh.open_sftp()
        # self.input()
        sftp.put(self.local_file_abs, self.remote_file_abs)
        sftp.close()
    def get(self):
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())
        sftp = self.ssh.open_sftp()
        # self.input()
        sftp.get(self.remote_file_abs, self.local_file_abs)
        sftp.close()
    def close(self):
        self.ssh.close()
        print("Host[%s] connect closed!!" % self.ip)

def download_img(obj, current_path, config, target, child=''):
    # Get Timestamp First
    y_m_d = date.today().strftime('%Y-%m-%d')
    y_m = date.today().strftime('%Y-%m')

    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    local_path_abs = os.path.join(current_path, local_path)
    print(local_path_abs)
    if not os.path.exists(local_path_abs):
        os.makedirs(local_path_abs)
    else:
        old_file_list = os.listdir(local_path_abs)
        for item in old_file_list:
            if not os.path.isdir(local_path_abs + item):
                os.remove(local_path_abs + item)
    if target == 'g3':
        target_path = path_info.get('g3_path', None)
        local_backup_path_abs = os.path.join(local_path_abs, target)
    elif target == 'g3hgu':
        local_backup_path_abs = os.path.join(local_path_abs, target +'_'+ child)
        if child == 'epon':
            target_path = path_info.get('g3hgu_epon_path', None)
        elif child == 'gpon':
            target_path = path_info.get('g3hgu_gpon_path', None)
        else:
            print("Target <%s> child[%s] is invalid!!" % target, child)
    elif target == 'saturn-sfu':
        local_backup_path_abs = os.path.join(local_path_abs, target +'_'+ child)
        if child == 'epon':
            target_path = path_info.get('epon_path', None)
        elif child == 'gpon':
             target_path = path_info.get('gpon_path', None)
        else:
            print("Target <%s> child[%s] is invalid!!" % target, child)
    else:
        print("Target[%s] is invalid!!" % target)
    if not os.path.exists(local_backup_path_abs):
        os.makedirs(local_backup_path_abs)
    else:
        old_backfile_list = os.listdir(local_backup_path_abs)
        for item in old_backfile_list:
            os.remove(local_backup_path_abs +'/' + item)
    print(local_backup_path_abs)
    remote_path = path_info.get('remote_path', None)
    remote_path_abs = remote_path + target+'/'+ y_m +'/'+ y_m_d + target_path
    return_items = getattr(obj, "connect")(remote_path_abs)
    print(return_items)
    if target == 'g3':
        log_file = y_m_d +'-'+ target + '-sanitytest-log.txt';
    elif target == 'saturn-sfu':
        log_file = y_m_d +'-'+ child + '-sanitytest-log.txt';
    if log_file in return_items:
        print("%s HAD put on the server already!!!" % log_file)
        return False
    # Download doing
    target_info = read_ini(config, target)
    ret = True
    for item in target_info.values():
            if not '-sanitytest-log.txt' in item:
                if item in return_items:
                    print("%s WAS Found on server!!!" % item)
                else:
                    print("%s WAS NOT Found on server!!!" % item)
                    ret = False
                    break
                remote_file = remote_path_abs + item
                # print(remote_file)
                if target == 'g3' and item == 'uboot-env.bin':
                    local_file = os.path.join(local_path_abs, target + item)
                else:
                    local_file = os.path.join(local_path_abs, item)
                local_backup_file = os.path.join(local_backup_path_abs, item)
                if target != 'g3hgu':
                    print(local_file)
                    getattr(obj, "input")(local_file, remote_file)
                    getattr(obj, "get")()
                    time.sleep(1)
                # print(local_backup_file + '\r\n')
                getattr(obj, "input")(local_backup_file, remote_file)
                getattr(obj, "get")()
                time.sleep(1)
    getattr(obj, "close")()
    return ret

def upload_log(obj, current_path, config, target, child=''):
    # Get Timestamp First
    y_m_d = date.today().strftime('%Y-%m-%d')
    y_m = date.today().strftime('%Y-%m')

    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    local_path_abs = os.path.join(current_path, local_path)
    print(local_path_abs)
    if not os.path.exists(local_path_abs):
        print("ERROR: local_path[%s] NOT exists!!" % local_path_abs)
    if target == 'g3':
        log_file = target + '-sanitytest-log.txt';
        target_path = path_info.get('g3_path', None)
    elif target == 'saturn-sfu':
        log_file = child + '-sanitytest-log.txt';
        if child == 'epon':
            target_path = path_info.get('epon_path', None)
        elif child == 'gpon':
            target_path = path_info.get('gpon_path', None)
        else:
            print("ERROR: Input saturn-sfu child[%s] is invalid!!" % child)
    else:
        print("ERROR: Input target[%s] is invalid!!" % target)
    remote_path = path_info.get('remote_path', None)
    remote_path_abs = remote_path + target+'/'+ y_m +'/'+ y_m_d + target_path
    return_items = getattr(obj, "connect")(remote_path_abs)
    print(return_items)
    target_info = read_ini(config, target)
    for item in target_info.values():
        if log_file == item:
            remote_file = remote_path + target +'/'+ y_m +'/'+ y_m_d + target_path + y_m_d +'-'+ item
            print(remote_file)
            local_file = os.path.join(local_path_abs, y_m_d +'-'+ item)
            print(local_file)
            if not os.path.exists(local_file):
                print("ERROR: local_file[%s] NOT exists!!" % local_file)
            getattr(obj, "input")(local_file, remote_file)
            getattr(obj, "put")()
            time.sleep(1)
    getattr(obj, "close")()

def do_telnet(config, target):
    telnet_info = read_ini(config, 'telnet')
    host = telnet_info.get('host', None)
    tftpboot_info = read_ini(config, 'tftpboot')
    saveenv_set = tftpboot_info.get('saveenv_set', None).encode('ascii')
    reset_set = tftpboot_info.get('reset_set', None).encode('ascii')
    if target == "g3":
        port = int(telnet_info.get('g3_port', None))
        activeport_set = tftpboot_info.get('g3_activeport_set', None).encode('ascii')
        ipaddr_set = tftpboot_info.get('g3_ipaddr_set', None).encode('ascii')
        serverip_set = tftpboot_info.get('g3_serverip_set', None).encode('ascii')
        tftpboot_gpt = tftpboot_info.get('g3_tftpboot_gpt', None).encode('ascii')
        tftpboot_ubootenv = tftpboot_info.get('g3_tftpboot_ubootenv', None).encode('ascii')
        tftpboot_image = tftpboot_info.get('g3_tftpboot_image', None).encode('ascii')
        uboot_tag = b'G3# '
        cmdline_tag = b'root@g3-eng:~# '
        written_ok = b'written: OK'
    elif target == "saturn-sfu":
        port = int(telnet_info.get('saturn_port', None))
        activeport_set = tftpboot_info.get('saturn_activeport_set', None).encode('ascii')
        ipaddr_set = tftpboot_info.get('saturn_ipaddr_set', None).encode('ascii')
        serverip_set = tftpboot_info.get('saturn_serverip_set', None).encode('ascii')
        tftpboot_gpt = tftpboot_info.get('saturn_tftpboot_gpt', None).encode('ascii')
        tftpboot_ubootenv = tftpboot_info.get('saturn_tftpboot_ubootenv', None).encode('ascii')
        tftpboot_dtb = tftpboot_info.get('saturn_tftpboot_dtb', None).encode('ascii')
        tftpboot_image = tftpboot_info.get('saturn_tftpboot_image', None).encode('ascii')
        tftpboot_rootfs = tftpboot_info.get('saturn_tftpboot_rootfs', None).encode('ascii')
        tftpboot_userubi = tftpboot_info.get('saturn_tftpboot_userubi', None).encode('ascii')
        uboot_tag = b'SATURN# '
        cmdline_tag = b'root@saturn-sfu-eng:~# '
        written_ok = b'Written: OK'
    else:
        print("ERROR: Input target[%s] is invalid!!" % target)

    tn = telnetlib.Telnet(host, port, timeout=50)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'root\n')
    time.sleep(1)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.read_until(cmdline_tag)
    tn.write(b'reboot\n')
    time.sleep(1)
    tn.read_until(b'Hit any key to stop autoboot: ')
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(activeport_set + b'\r\n')
    tn.write(serverip_set + b'\r\n')
    tn.write(ipaddr_set + b'\r\n')
    tn.write(saveenv_set + b'\r\n')
    tn.write(reset_set + b'\r\n')
    tn.read_until(b'Hit any key to stop autoboot: ')
    tn.write(b'\r\n')
    time.sleep(1)
    # Upgrade gpt
    tn.read_until(uboot_tag)
    tn.write(tftpboot_gpt + b'\r\n')
    time.sleep(1)
    # Upgrade uboot_env
    tn.read_until(written_ok)
    tn.write(tftpboot_ubootenv + b'\r\n')
    time.sleep(1)
    # Upgrade image
    tn.read_until(written_ok)
    tn.write(tftpboot_image + b'\r\n')
    time.sleep(1)
    if target =='saturn-sfu':
        tn.read_until(written_ok)
        tn.write(tftpboot_dtb + b'\r\n')
        time.sleep(1)
        tn.read_until(written_ok)
        tn.write(tftpboot_rootfs + b'\r\n')
        time.sleep(1)
        tn.read_until(written_ok)
        tn.write(tftpboot_userubi + b'\r\n')
        time.sleep(1)
    else:
        pass
    # RESET board
    tn.read_until(written_ok)
    tn.write(reset_set + b'\r\n')
    time.sleep(60)
    tn.write(b'root\n')
    time.sleep(1)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'uname -a\n')
    time.sleep(1)
    #tn.write(b'exit\n')
    time.sleep(1)
    result_str = tn.read_very_eager()
    tn.close()
    return (result_str.decode('ascii', errors='ignore'))

def capture_log(current_path, config, target, child=''):
    # Get Timestamp First
    y_m_d = date.today().strftime('%Y-%m-%d')
    y_m = date.today().strftime('%Y-%m')

    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    local_path_abs = os.path.join(current_path, local_path)
    if not os.path.exists(local_path_abs):
        os.makedirs(local_path_abs)
    if target == 'g3':
        log_file_path = local_path_abs + y_m_d +'-'+ target +'-sanitytest-log.txt'; 
    elif target == 'saturn-sfu':
        log_file_path = local_path_abs + y_m_d +'-'+ child +'-sanitytest-log.txt'; 
    else:
        pass
    print(log_file_path)
    log_txt = do_telnet(config, target)
    with open(log_file_path, 'w') as f:
        f.write(log_txt)

if __name__ == "__main__":
    current_path = sys.argv[0].rstrip('/sanity_test_g3.py')
    print(current_path)
    config = os.path.join(current_path, 'config/dailybuild_server_config.ini')
    print(config)
    ssh_info = read_ini(config, 'ssh')
    host = ssh_info.get('host', None)
    port = int(ssh_info.get('port', None)) # 端口是int类型
    username = ssh_info.get('username', None)
    password = ssh_info.get('password', None)
    obj = SftpTool(username, password, port, host)

    #g3hgu_img_ok = download_img(obj, current_path, config, 'g3hgu', 'epon')
    #time.sleep(2)
    #print("g3hgu img download is %s" % g3hgu_img_ok)

    print("--- --- --- G3/EPON/GPON Sanity Test Start!!! --- --- ---")
    while True:
        # G3 sanity test process
        g3_img_ok = download_img(obj, current_path, config, 'g3')
        time.sleep(2)
        print("g3 img download is %s" % g3_img_ok)
        if g3_img_ok == True:
            capture_log(current_path, config, 'g3')
            time.sleep(2)
            upload_log(obj, current_path, config, 'g3')
            time.sleep(2)
        else:
            time.sleep(20*60) # wait 1/3 hour

        # Epon sanity test process
        epon_img_ok = download_img(obj, current_path, config, 'saturn-sfu', 'epon')
        time.sleep(2)
        print("saturn-sfu_epon img download is %s" % epon_img_ok)
        if epon_img_ok == True:
            capture_log(current_path, config, 'saturn-sfu', 'epon')
            time.sleep(2)
            upload_log(obj, current_path, config, 'saturn-sfu', 'epon')
            time.sleep(2)
        else:
            time.sleep(20*60)

        # Gpon sanity test process
        gpon_img_ok = download_img(obj, current_path, config, 'saturn-sfu', 'gpon')
        time.sleep(2)
        print("saturn-sfu_epon img download is %s" % epon_img_ok)
        if gpon_img_ok == True:
            capture_log(current_path, config, 'saturn-sfu', 'gpon')
            time.sleep(2)
            upload_log(obj, current_path, config, 'saturn-sfu', 'gpon')
        else:
            time.sleep(20*60)
    print("--- --- --- G3/EPON/GPON Sanity Test End!!! --- --- ---")
