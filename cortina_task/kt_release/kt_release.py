# -*- coding: utf-8 -*-
'''
Created on 2019-04-22 @CPP
'''
import paramiko
import getpass
import pexpect
import time
import os
import re

from datetime import date
from configparser import ConfigParser

#@#$%^&
datetag = date.today().strftime('%Y-%m-%d')
# print(datetag)
build_path = 'cd /space/pchen/kt_release; '
# print(build_path)
yocto_path = build_path.replace('; ', '/') +'yocto-'+ datetag+'; '
# print(yocto_path)

# Read config info
def read_ini():
    info = dict()
    cf = ConfigParser()
    cf.read('config.ini', encoding='utf-8')
    keys = cf.options('ssh')
    for each in keys:
        info[each] = cf.get('ssh', each)
    # print(info)
    return info

# @#$%^&
def ssh_expect(ip, password, cmd):
    ssh = pexpect.spawn('ssh pchen@%s "%s"' % (ip, cmd))
    try:
        i = ssh.expect(['password:', 'continue connecting (yes/no)?'], timeout=5)
        if i == 0:
            ssh.sendline(password)
        elif i == 1:
            ssh.sendline('yes\n')
            ssh.expect('password: ')
            ssh.sendline(password)
        ssh.sendline(cmd)
        r = ssh.read().decode('utf8')
        print(r)
        ret = 0
    except pexpect.EOF:
        print("EOF")
        ssh.close()
        ret = -1
    except pexpect.TIMEOUT:
        print("TIMEOUT")
        ssh.close()
        ret = -2
    return ret


# #$%@! define ssh cmd
def ssh_cmd(host, port, username, password, cmd):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    try:
        ssh.connect(host, port, username, password, timeout=10)
    except Exception as e:
        print(e)
    ssh_in, ssh_out, ssh_error = ssh.exec_command(cmd, get_pty=True)
    result = ssh_out.read() or ssh_error.read()
    return result.decode('utf8').strip()
    ssh.close()


# @#$%^ define ssh gitclone yocto
def ssh_gitclone(host, port, username, password):
    cmd_gitclone = [
        'git clone -b kt-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/yocto yocto-'+datetag,
        'git clone -b kt-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-oe-yocto-cortina',
        'git clone -b kt-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-ca-bsp',
        'git clone -b kt-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/openembedded/meta-openembedded',
        'git clone -b kt-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-oe-ca'
    ]
    for index, cmd in enumerate(cmd_gitclone):
        if index == 0:
            gitclone_path = ssh_cmd(host, port, username, password, build_path + cmd);
        else:
            gitclone_path = ssh_cmd(host, port, username, password, yocto_path + cmd);
        print(str(index) +': '+ gitclone_path)

# sftp test func
def sftp_test(host, port, username, password, from_file, to_file):
    transport = paramiko.Transport((host, port))
    try:
        transport.connect(username=username, password=password)
    except Exception as e:
        print(e)
        return
    sftp = paramiko.SFTPClient.from_transport(transport)
    sftp.get(from_file, to_file)
    transport.close()

# download files from remote server
def sftp_download(host, port, username, password, from_file, to_file):
    transport = paramiko.Transport((host, port))
    try:
        transport.connect(username=username, password=password)
    except Exception as e:
        print(e)
        return
    sftp = paramiko.SFTPClient.from_transport(transport)
    sftp.get(from_file, to_file)
    transport.close()

# upload files to remote server
def sftp_upload(host, port, username, password, from_file, to_file):
    transport = paramiko.Transport((host, port))
    try:
        transport.connect(username=username, password=password)
    except Exception as e:
        print(e)
        return
    sftp = paramiko.SFTPClient.from_transport(transport)
    sftp.put(from_file, to_file)
    transport.close()

#@#$%^&
def version_autoincrement(_file):
    key_word = "21.00.00.09."
    file_data = ""
    key_char = []
    with open(_file, "r", encoding="utf-8") as f:
        for line in f:
            if key_word in line:
                print(line)
                cur_ver = re.compile('"(.*)"').findall(line)
                key_char = cur_ver[-1].split('.')
                #print(key_char)
                key_char[-1] = str(int(key_char[-1]) + 1).zfill(2)
                #print(key_char)
                new_ver = ".".join(key_char)
                #print(new_ver)
                line = line.replace(cur_ver[-1], new_ver)
                print(line)
            file_data += line
    with open(_file, "w", encoding="utf-8") as f:
        f.write(file_data)


# @#$%^ define modify release verion func
def ssh_update_version(host, port, username, password):
    from_file = [
        'meta-ca-bsp/classes/sfu_upgrade_img.bbclass',
        'meta-oe-ca/classes/kt_upgrade_img.bbclass',
        'meta-oe-ca/recipes-boot/u-boot/u-boot_2015.04.bbappend'
    ]
    to_file = [
        'tmp/sfu_upgrade_img.bbclass',
        'tmp/kt_upgrade_img.bbclass',
        'tmp/u-boot_2015.04.bbappend'
    ]
    for index, ff in enumerate(from_file):
        tf = to_file[index]
        print(tf)
        ff = yocto_path.split()[-1].replace(';', '/') + ff
        print(ff)
        sftp_download(host, port, username, password, ff, tf)
        version_autoincrement(tf)
        sftp_upload(host, port, username, password, tf, ff)


if __name__ == '__main__':
    info = read_ini()
    h = info.get('host', None)
    p = int(info.get('port', None))
    u = info.get('username', None)
    pw = info.get('password', None)

    # STEP #1
    ssh_gitclone(h, p, u, pw)
    # STEP #2
    ssh_update_version(h, p, u, pw)
    # STEP #3
    cmd_bitbake = 'TEMPLATECONF=meta-oe-ca/conf/saturn-sfu-eng-kt; source ./oe-init-build-env; bitbake major-image &'
    ssh_expect('sh-c-21', pw, yocto_path + cmd_bitbake)

