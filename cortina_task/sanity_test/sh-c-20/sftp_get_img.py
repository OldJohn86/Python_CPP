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
class Tools(object):
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

if __name__ == "__main__":
    config = './config/dailybuild_server_config.ini'
    
    ssh_info = read_ini(config, 'ssh')
    host = ssh_info.get('host', None)
    port = int(ssh_info.get('port', None)) # 端口是int类型
    username = ssh_info.get('username', None)
    password = ssh_info.get('password', None)

    obj = Tools(username, password, port, host)
#download_img(obj, config, 'saturn-sfu', 'epon')
#download_img(obj, config, 'saturn-sfu', 'gpon')
    download_img(obj, config, 'g3')

#upload_log(obj, config, 'g3')
    
