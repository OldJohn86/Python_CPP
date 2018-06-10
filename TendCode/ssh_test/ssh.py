# -*- coding: utf-8 -*-
import paramiko
import os
from configparser import ConfigParser


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
    cmd_get_path = 'cd daily_image_sanity_test;pwd'
    db_path = run_shell(cmd_get_path)

    # 获取指定文件夹中文件的名称，并跟上面得到的文件夹绝对地址组合起来
    cmd_get_sqls = 'cd daily_image_sanity_test;ls'
    sqls = run_shell(cmd_get_sqls)
    #print(sqls)

    # 判断是否为子目录
    cmd_get_dir ='cd daily_image_sanity_test;ls -l|grep ^d|awk \'{print$9}\'' 
    dir_name = run_shell(cmd_get_dir)
    #print(dir_name)

    # 去除子目录路径
    sqls = sqls.replace(dir_name + '\n', '')
    print(sqls)

    lis = ['{}/{}'.format(db_path, each) for each in sqls.split('\n')]
    print(lis)

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
    info = read_ini()
    h = info.get('host', None)
    p = int(info.get('port', None)) # 端口是int类型
    u = info.get('username', None)
    pw = info.get('password', None)
    files = ssh_test(h, p, u, pw)

    path = './daily_images'
    if files:
        for each in files:
            name = each.split('/')[-1]
            print(each)
            print(name)
            ss = sftp_test(h, p, u, pw, each, os.path.join(path, name))

