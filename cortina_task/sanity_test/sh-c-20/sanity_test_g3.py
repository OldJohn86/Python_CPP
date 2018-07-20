#!/usr/bin/env python3

# -*- coding: utf-8 -*-
import os
import sys
import time
import paramiko
import telnetlib
import getpass

from datetime import date
from configparser import ConfigParser


# Get Timestamp
#y_m_d = '2018-07-18'
y_m_d = date.today().strftime('%Y-%m-%d')
#y_m ='2018-07'
y_m = date.today().strftime('%Y-%m')
print(y_m_d)
print(y_m)

# 读取配置文件获取服务器的登录信息
def read_ini(config, option):
    info = dict()
    cf = ConfigParser()
    cf.read(config, encoding='utf-8')
    keys = cf.options(option)
    for each in keys:
        info[each] = cf.get(option, each)
    print(info)
    return info

# 定义SSH SFTP Tool 工具类
class SftpTool(object):
    def __init__(self, username, password, port, host):
        self.user = username
        self.password = password
        self.port = port
        self.ip = host
    def connect(self):
        try:
            self.ssh = paramiko.SSHClient()
            self.ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            self.ssh.connect(self.ip, self.port, self.user, self.password)
            print("Connect created!!")
        except Exception as e:
            print("ERROR: Failed to connect to Host[%s]!!" % self.ip)
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
        print("Connect closed")

def download_img(obj, config, target, child=''):
    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    local_path_abs = os.path.join(os.getcwd(), local_path)
    print(local_path_abs)
    if not os.path.exists(local_path_abs):
        os.makedirs(local_path_abs)
    else:
        oldfile_list = os.listdir(local_path_abs)
        for item in oldfile_list:
            os.remove(local_path_abs +'/' + item)
    remote_path = path_info.get('remote_path', None)

    getattr(obj, "connect")()
    target_info = read_ini(config, target)
    for item in target_info.values():
        if target == 'g3':
            target_path = path_info.get('g3_path', None)
        elif target == 'saturn-sfu':
            if child == 'epon':
                target_path = path_info.get('epon_path', None)
            elif child == 'gpon':
                target_path = path_info.get('gpon_path', None)
            else:
                print("Target child[%s] is invalid!!" % target)
                pass
        else:
            print("Target[%s] is invalid!!" % target)
            pass

        if '-sanitytest-log.txt' in item:
            pass
        else:
            remote_file = remote_path+target+'/'+ y_m +'/'+ y_m_d + target_path + item
            print(remote_file)
            # local_file = local_path_abs + item
            local_file = os.path.join(local_path_abs, item)
            print(local_file)
            getattr(obj, "input")(local_file, remote_file)
            getattr(obj, "get")()
            time.sleep(1)
    getattr(obj, "close")()

def upload_log(obj, config, target, child=''):
    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    local_path_abs = os.path.join(os.getcwd(), local_path)
    print(local_path_abs)
    if not os.path.exists(local_path_abs):
        print("ERROR: local_path[%s] NOT exists!!" % local_path_abs)
    remote_path = path_info.get('remote_path', None)

    getattr(obj, "connect")()
    target_info = read_ini(config, target)
    for item in target_info.values():
        if target == 'g3':
            target_path = path_info.get('g3_path', None)
        elif target == 'saturn-sfu':
            if child == 'epon':
                target_path = path_info.get('epon_path', None)
            elif child == 'gpon':
                target_path = path_info.get('gpon_path', None)
            else:
                print("ERROR: Input saturn-sfu child[%s] is invalid!!" % child)
                pass
        else:
            print("ERROR: Input target[%s] is invalid!!" % target)
            pass
        if '-sanitytest-log.txt' in item:
            remote_file = remote_path + target +'/'+ y_m +'/'+ y_m_d + target_path+ y_m_d +'-'+ item
            print(remote_file)
            local_file = os.path.join(local_path_abs, y_m_d +'-'+ item)
            print(local_file)
            if not os.path.exists(local_file):
                print("ERROR: local_file[%s] NOT exists!!" % local_file)
            getattr(obj, "input")(local_file, remote_file)
            getattr(obj, "put")()
            time.sleep(1)
        else:
            pass
    getattr(obj, "close")()

def do_telnet(config, target):
    telnet_info = read_ini(config, 'telnet')
    host = telnet_info.get('host', None)
    if target == 'g3':
        port = int(telnet_info.get('g3_port', None))
    elif target == 'saturn-sfu':
        port = int(telnet_info.get('saturn_port', None))
    else:
        print("ERROR: Input target[%s] is invalid!!" % target)

    tftpboot_info = read_ini(config, 'tftpboot')
    saveenv_set = tftpboot_info.get('saveenv_set', None).encode('ascii')
    reset_set = tftpboot_info.get('reset_set', None).encode('ascii')
    if target == "g3":
        activeport_set = tftpboot_info.get('g3_activeport_set', None).encode('ascii')
        ipaddr_set = tftpboot_info.get('g3_ipaddr_set', None).encode('ascii')
        serverip_set = tftpboot_info.get('g3_serverip_set', None).encode('ascii')
        tftpboot_gpt = tftpboot_info.get('g3_tftpboot_gpt', None).encode('ascii')
        tftpboot_ubootenv = tftpboot_info.get('g3_tftpboot_ubootenv', None).encode('ascii')
        tftpboot_image = tftpboot_info.get('g3_tftpboot_image', None).encode('ascii')
    elif target == "saturn-sfu":
        activeport_set = tftpboot_info.get('saturn_activeport_set', None).encode('ascii')
        ipaddr_set = tftpboot_info.get('saturn_ipaddr_set', None).encode('ascii')
        serverip_set = tftpboot_info.get('saturn_serverip_set', None).encode('ascii')
        tftpboot_gpt = tftpboot_info.get('saturn_tftpboot_gpt', None).encode('ascii')
        tftpboot_ubootenv = tftpboot_info.get('saturn_tftpboot_ubootenv', None).encode('ascii')
        tftpboot_dtb = tftpboot_info.get('saturn_tftpboot_dtb', None).encode('ascii')
        tftpboot_image = tftpboot_info.get('saturn_tftpboot_image', None).encode('ascii')
        tftpboot_rootfs = tftpboot_info.get('saturn_tftpboot_rootfs', None).encode('ascii')
        tftpboot_userubi = tftpboot_info.get('saturn_tftpboot_userubi', None).encode('ascii')
    
    tn = telnetlib.Telnet(host, port, timeout=50)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'root\n')
    time.sleep(1)
    tn.write(b'\r\n')
    time.sleep(1)

    if target == 'g3':
        tn.read_until(b'root@g3-eng:~# ')
    elif target == 'saturn-sfu':
        tn.read_until(b'root@saturn-sfu-eng:~# ')
    else:
        pass
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
    if target == 'g3':
        written_ok = b'written: OK'
        target_tag = b'G3# '
    elif target == 'saturn-sfu':
        written_ok = b'Written: OK'
        target_tag = b'SATURN# '
    else:
        print("ERROR: Input target[%s] invalid!" % target)
        pass
    tn.read_until(target_tag)
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

    time.sleep(50)
    tn.write(b'root\n')
    time.sleep(1)
    tn.write(b'\r\n')
    time.sleep(1)
    tn.write(b'uname -a\n')
    time.sleep(1)
    tn.write(b'exit\n')
    time.sleep(1)

    result_str = tn.read_very_eager()
    tn.close()
    return (result_str.decode('ascii', errors='ignore'))

def capture_log(config, target, child=''):
    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    local_path_abs = os.path.join(os.getcwd(), local_path)

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
    with open(local_file_path, 'w') as f:
        f.write(log_txt)

if __name__ == "__main__":
    config = './config/dailybuild_server_config.ini'
    ssh_info = read_ini(config, 'ssh')
    host = ssh_info.get('host', None)
    port = int(ssh_info.get('port', None)) # 端口是int类型
    username = ssh_info.get('username', None)
    password = ssh_info.get('password', None)
    obj = SftpTool(username, password, port, host)

    # G3 sanity test process
    download_img(obj, config, 'g3')
    capture_log(config, 'g3')
    upload_log(obj, config, 'g3')
 
    # Epon sanity test process
    download_img(obj, config, 'saturn-sfu', 'epon')
    capture_log(config, 'saturn-sfu', 'epon')
    upload_log(obj, config, 'saturn-sfu', 'epon')

    # Gpon sanity test process
    download_img(obj, config, 'saturn-sfu', 'gpon')
    capture_log(config, 'saturn-sfu', 'gpon')
    upload_log(obj, config, 'saturn-sfu', 'gpon')
