#!/usr/bin/env python3

# -*- coding: utf-8 -*-
import paramiko
import os
from datetime import date
from configparser import ConfigParser

g3_image_url     = 'http://192.168.65.235:18683/g3/'
g3_image_type    = 'g3-eng-major-image'
g3_gpt           = 'major-image-g3-eng-gpt-nandinfo.img'
ubootenv         = 'uboot-env.bin'
g3_ubootenv      = 'g3-uboot-env.bin'
g3_image         = 'major-image-g3-eng.mubi'
sanity_test_path = 'cd /home/pchen/daily_image_sanity_test;'

# day = date.today()
y_m_d = date.today().strftime('%Y-%m-%d')
y_m = date.today().strftime('%Y-%m')
print(y_m_d)
print(y_m)

# 读取配置文件获取服务器的登录信息
def read_ini():
    info = dict()
    cf = ConfigParser()
    cf.read('config.ini', encoding='utf-8')
    keys = cf.options('ssh')
    for each in keys:
        info[each] = cf.get('ssh', each)
    print(info)
    return info

# 连接服务区并执行shell命令返回输出结果
def ssh_test(host, port, username, password):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    # 连接服务器
    try:
        ssh.connect(hostname=host, port=port, username=username, password=password)
    except Exception as e:
        print(e)
        return

    # 设置一个内部函数，执行shell命令并返回输出结果
    def run_shell(cmd):
        ssh_in, ssh_out, ssh_error = ssh.exec_command(cmd)
        result = ssh_out.read() or ssh_error.read()
        return result.decode().strip()

    # 获取指定文件夹的绝对地址
    cmd_get_path = sanity_test_path +' pwd'
    db_path = run_shell(cmd_get_path)

    # Removed old image files
    cmd_rm_image = sanity_test_path +' rm *g3*'
    rm_ret = run_shell(cmd_rm_image)

    # Get the lastest image files from Server IP
    cmd_wget_g3gpt = sanity_test_path +' wget '+g3_image_url+'/'+y_m+'/'+y_m_d+'/'+g3_image_type+'/'+g3_gpt
    print(cmd_wget_g3gpt)
    g3gpt_ret = run_shell(cmd_wget_g3gpt)

    cmd_wget_ubootenv = sanity_test_path +' wget '+g3_image_url+'/'+y_m+'/'+y_m_d+'/'+g3_image_type+'/'+ubootenv
    print(cmd_wget_ubootenv)
    ubootenv_ret = run_shell(cmd_wget_ubootenv)

    cmd_rename_g3ubootenv = sanity_test_path + ' mv '+ubootenv+' '+g3_ubootenv
    print(cmd_rename_g3ubootenv)
    g3ubootenv_ret = run_shell(cmd_rename_g3ubootenv)

    cmd_wget_g3image = sanity_test_path +' wget '+g3_image_url+'/'+y_m+'/'+y_m_d+'/'+g3_image_type+'/'+g3_image
    print(cmd_wget_g3image)
    g3image_ret = run_shell(cmd_wget_g3image)

    # 获取指定文件夹中文件的名称，并跟上面得到的文件夹绝对地址组合起来
    cmd_get_sqls = sanity_test_path +' ls'
    sqls = run_shell(cmd_get_sqls)
    # print(sqls)

    lis = ['{}/{}'.format(db_path, each) for each in sqls.split('\n')]
    # print(lis)

    # 关闭连接
    ssh.close()
    return lis

# 链接服务器进行文件传输
def sftp_test(host, port, username, password, from_file, to_file):
    transport = paramiko.Transport((host, port))
    try:
        transport.connect(username=username, password=password)
    except Exception as e:
        print(e)
        return
    sftp = paramiko.SFTPClient.from_transport(transport)

    # 将文件下载到本地，如果是上传使用 put
    sftp.get(from_file, to_file)
    transport.close()

if __name__ == '__main__':
    # Removed old images first.
    cmd_rm_image = sanity_test_path +' rm *g3*'
    print(cmd_rm_image)
    os.system(cmd_rm_image)

    # Get the lastest image files from Server IP
    cmd_wget_g3gpt = sanity_test_path +' wget '+g3_image_url+'/'+y_m+'/'+y_m_d+'/'+g3_image_type+'/'+g3_gpt
    print(cmd_wget_g3gpt)
    os.system(cmd_wget_g3gpt)

    cmd_wget_ubootenv = sanity_test_path +' wget '+g3_image_url+'/'+y_m+'/'+y_m_d+'/'+g3_image_type+'/'+ubootenv
    print(cmd_wget_ubootenv)
    os.system(cmd_wget_ubootenv)

    cmd_rename_g3ubootenv = sanity_test_path + ' mv '+ubootenv+' '+g3_ubootenv
    print(cmd_rename_g3ubootenv)
    os.system(cmd_rename_g3ubootenv)

    cmd_wget_g3image = sanity_test_path +' wget '+g3_image_url+'/'+y_m+'/'+y_m_d+'/'+g3_image_type+'/'+g3_image
    print(cmd_wget_g3image)
    os.system(cmd_wget_g3image)



