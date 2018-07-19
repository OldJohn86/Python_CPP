#!/usr/bin/env python3

# -*- coding:utf-8 -*-
import paramiko
import os
import sys
import time

def ssh_scp_put(ip,port,user,password,local_file,remote_file):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(ip, port, user, password)
    a = ssh.exec_command('date')
    stdin, stdout, stderr = a
    print stdout.read()
    sftp = paramiko.SFTPClient.from_transport(ssh.get_transport())
    sftp = ssh.open_sftp()
    sftp.put(local_file, remote_file)

def ssh_scp_get(ip, port, user, password, remote_file, local_file):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(ip, port, user, password)
    a = ssh.exec_command('date')
    stdin, stdout, stderr = a
    print stdout.read()
    sftp = paramiko.SFTPClient.from_transport(ssh.get_transport())
    sftp = ssh.open_sftp()
    sftp.get(remote_file, local_file)

if __name__ == "__main__":
    port = 28683
    user = 'cortina'
    ip = '192.168.65.235'
    password = 'Cortina123'
    remote_file = ''
    local_file = ''
    ssh_scp_put(ip, port, user, password, remote_file, local_file)
