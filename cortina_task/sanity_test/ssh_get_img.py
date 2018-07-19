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

config = './config/dailybuild_server_config.ini'
#option = 'ssh'
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
            print("连接已建立")
        except Exception as e:
            print("未能连接到主机")

    def cmd(self):
        cmd = input("请输入要执行的命令:>>")
        stdout, stdin, stderr = self.ssh.exec_command(cmd)
        print(sys.stdout.read())

    def input(self, local_file, remote_file):
        #input("本地文件的绝对路径:>>")
        self.local_file_abs = local_file 
        #input("远程文件的绝对路径:>>")
        self.remote_file_abs = remote_file

    def put(self):
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())
        sftp = self.ssh.open_sftp()
        # self.input()
        sftp.put(self.local_file_abs, self.remote_file_abs)
        # sftp.close()

    def get(self):
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())
        sftp = self.ssh.open_sftp()
        # self.input()
        sftp.get(self.remote_file_abs, self.local_file_abs)
        # sftp.close()

    def close(self):
        self.ssh.close()
        print("连接关闭")


if __name__ == "__main__":
    msg = '''\033[32;1m
    执行命令 >>输入cmd
    上传文件 >>输入put
    下载文件 >>输入get
    退出     >>输入q\033[0m
    '''
    ssh_info = read_ini(config, 'ssh')
    host = ssh_info.get('host', None)
    port = int(ssh_info.get('port', None)) # 端口是int类型
    username = ssh_info.get('username', None)
    password = ssh_info.get('password', None)
    obj = Tools(username, password, port, host)
    getattr(obj, "connect")()
    
    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    if os.path.exists(local_path):
        pass
    else:
        os.makedirs(local_path)
    remote_path = path_info.get('remote_path', None)
    g3_path = path_info.get('g3_path', None)
    epon_path = path_info.get('epon_path', None)
    gpon_path = path_info.get('gpon_path', None)
    y_m_d = date.today().strftime('%Y-%m-%d')
    y_m = date.today().strftime('%Y-%m')
    #print(y_m_d)
    #print(y_m)

    # Download g3 img
    target = 'g3'
    g3_info = read_ini(config, 'g3')
    for item in g3_info.values():
        remote_file = remote_path +target+'/'+ y_m +'/'+ y_m_d + g3_path + item
        local_file = local_path + item
        print(remote_file)        
        print(local_file)
        #getattr(obj, "get")()

    # Download epon img
    target = 'saturn-sfu'
    saturn_info = read_ini(config, target)
    for item in saturn_info.values():
        remote_file = remote_path+target+'/'+ y_m +'/'+ y_m_d + epon_path + item
        local_file = local_path + item
        print(remote_file)        
        print(local_file)        
        getattr(obj, "input")(local_file, remote_file)
        getattr(obj, "get")()

    while True:
        print(msg)
        inp = input("action:>>")
        if hasattr(obj,inp):
            getattr(obj,inp)()
        if inp == "q":
            getattr(obj,"close")()
            exit()
        else:
            print("没有该选项，请重新输入:>>")
