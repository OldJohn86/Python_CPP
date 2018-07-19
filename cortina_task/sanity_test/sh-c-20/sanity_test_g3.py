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


#print("\033[32;1m****开始配置目标机器信息*****\033[0m")
#ips = input("主机IP:")
#user = input("主机账号:")
#password = getpass.getpass("主机密码:")
#port = 22

server_url       = 'http://192.168.65.235:18683/'
ubootenv         = 'uboot-env.bin'

activeport_set     = b'setenv active_port 1;\r\n'
serverip_set       = b'setenv serverip 192.168.1.128;\r\n'
ipaddr_set         = b'setenv ipaddr 192.168.1.2;\r\n'
saveenv_set        = b'saveenv;\r\n'
reset_set          = b'reset\r\n'

tftpboot_gpt       = b'tftpboot 0x84100000 major-image-g3-eng-gpt-nandinfo.img; nand erase 0x0 0x400000; nand write 0x84100000 0x0 0x300000;\r\n'
tftpboot_ubootenv  = b'tftpboot 0x84000000 uboot-env.bin; nand erase 0x400000 0x100000; nand write 0x84000000 0x400000 0x20000;\r\n'
tftpboot_image     = b'tftpboot 0x85000000 major-image-g3-eng.mubi; nand erase 0x500000 0xD000000; nand write 0x85000000 0x500000 0x${filesize};\r\n'


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
class SFTPTools(object):
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
            print("Failed to connect to Host")
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
    if os.path.exists(local_path):
        pass
    else:
        os.makedirs(local_path)
    remote_path = path_info.get('remote_path', None)

    # Get Timestamp
    y_m_d = '2018-07-18'
#date.today().strftime('%Y-%m-%d')
    y_m ='2018-07'
#date.today().strftime('%Y-%m')
    print(y_m_d)
    print(y_m)

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
            local_file = local_path + item
            print(remote_file)
            print(local_file)
            getattr(obj, "input")(local_file, remote_file)
            getattr(obj, "get")()
    getattr(obj, "close")()

def upload_log(obj, config, target, child=''):
    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    if os.path.exists(local_path):
        pass
    else:
        os.makedirs(local_path)
    remote_path = path_info.get('remote_path', None)

    # Get Timestamp
    y_m_d = '2018-07-18'
#date.today().strftime('%Y-%m-%d')
    y_m ='2018-07'
#date.today().strftime('%Y-%m')
    print(y_m_d)
    print(y_m)

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
            remote_file = remote_path+target+'/'+ y_m +'/'+ y_m_d + target_path + y_m_d+ '-' + item
            local_file = local_path + item
            print(remote_file)
            print(local_file)
            getattr(obj, "input")(local_file, remote_file)
            getattr(obj, "put")()
        else:
            pass
    getattr(obj, "close")()


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


if __name__ == "__main__":
    config = './config/dailybuild_server_config.ini'
    
    ssh_info = read_ini(config, 'ssh')
    host = ssh_info.get('host', None)
    port = int(ssh_info.get('port', None)) # 端口是int类型
    username = ssh_info.get('username', None)
    password = ssh_info.get('password', None)

    obj = SFTPTools(username, password, port, host)
#download_img(obj, config, 'saturn-sfu', 'epon')
#download_img(obj, config, 'saturn-sfu', 'gpon')
    
    download_img(obj, config, 'g3')
    log_txt = do_telnet('192.168.41.251')
    #print(log_txt)
    if os.path.exists('./daily_image_sanity_test'):
        pass
    else:
        os.makedirs('./daily_image_sanity_test')
    with open('./daily_image_sanity_test/g3-sanitytest-log.txt', 'w') as f:
        f.write(log_txt)
    upload_log(obj, config, 'g3')
    
