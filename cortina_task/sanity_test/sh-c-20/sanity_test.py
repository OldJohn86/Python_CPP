# -*- coding: utf-8 -*-
'''
Author: pengpeng.chen@cortina-access.com
Date: 2018-01-01
'''

import os
import sys
import time
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

glb_log_file = ""
glb_img_rev_file = ""
glb_cmd_file = ""
glb_local_path = ""

def read_ini(config, option):
    info = dict()
    cf = ConfigParser()
    cf.read(config, encoding='utf-8')
    keys = cf.options(option)
    for each in keys:
        info[each] = cf.get(option, each)
#    print(info)
    return info

def get_file_lines(inputfile):
    with open(inputfile, 'r') as f:
        info_msg = ""
        lines = f.readlines()
        line_sum = len(lines)
        for line in lines:
            info_msg += line
#        print(info_msg)
        return info_msg

def get_file_last_line(inputfile, lines_sum):
    filesize = os.path.getsize(inputfile)
    blocksize = 10240
    with open(inputfile, 'r', encoding='utf-8') as f:
        last_line = ""
        if filesize > blocksize:
            maxseekpoint = (filesize // blocksize)
            f.seek((maxseekpoint - 1) * blocksize)
        elif filesize:
            f.seek(0, 0)
        lines = f.readlines()
#        print(lines)
        if lines:
            lineno = 0
            for lineno in range(lines_sum):
                while last_line == "":
#                    print(lineno - lines_sum)
                    last_line = lines[lineno - lines_sum].strip()
                    lineno += 1
                last_line += lines[lineno - lines_sum]
#        print(last_line)
        else:
            pass
        return last_line

def send_email(config, target, child=''):
    global glb_log_file
    global glb_img_rev_file
    global glb_upgrade_cmds
#    print(glb_img_rev_file)
    y_m_d = date.today().strftime('%Y-%m-%d')
    y_m = date.today().strftime('%Y-%m')
    mail_info = read_ini(config, 'mail')
    mail_user = str(mail_info.get('user', None))
    mail_postfix = str(mail_info.get('postfix', None))
    mail_pwd = str(mail_info.get('pwd', None))
    mail_host = str(mail_info.get('host', None))
    mail_port = str(mail_info.get('port', None))
    to_list = str(mail_info.get('to_list', None))
    http_link = str(mail_info.get('http_link', None))
    list_mailaddr = to_list.split()
#    print(list_mailaddr)
    mailto_list = [x +'@'+ mail_postfix for x in list_mailaddr]
#    print(mailto_list)

    my_mail = mail_user +"@" + mail_postfix
    msg = MIMEMultipart()
    if child != '':
        msg['Subject'] = y_m_d +' '+ str(target) +' '+ str(child) + " Sanity Test Failed Report..."
        context_msg = http_link + target +'/'+ y_m +'/'+ y_m_d +'/'+ target +'-eng_'+ child +'-major-image/'
#        print(context_msg)
    else:
        msg['Subject'] = y_m_d +' '+ str(target) + " Sanity Test Failed Report..."
        context_msg = http_link + target +'/'+ y_m +'/'+ y_m_d +'/'+ target +'-eng-major-image/'
#        print(context_msg)
    text_msg = '[Test Image]: \r\n'+ context_msg
    msg['From'] = my_mail
    msg['To'] = ";".join(mailto_list)
#    msg.attach(MIMEText('send with sanity test log file...', 'plain', 'utf-8'))
#    msg.attach(MIMEText('Test Image: \r\n' + context_msg, 'plain', 'utf-8'))

    if not os.path.exists(glb_cmd_file):
#        print(glb_cmd_file)
        return False
    else:
#        print(glb_cmd_file)
        cmd_file_lines = get_file_lines(os.path.abspath(glb_cmd_file))
#        print(cmd_file_lines)
    text_msg += '\r\n\r\n [Upgrade Commands]: \r\n' + cmd_file_lines
    att0 = MIMEText(open(glb_cmd_file, 'rb').read(), 'base64', 'utf-8')
    att0["Content-Type"] = 'application/octet-stream'
    att0["Content-Disposition"] = 'attachment; filename="sanity-test-cmds.txt"'
    msg.attach(att0)

    if not os.path.exists(glb_img_rev_file):
#        print(glb_img_rev_file)
        return False
    else:
#        print(glb_img_rev_file)
        img_rev_lines = get_file_lines(os.path.abspath(glb_img_rev_file))
#        print(img_rev_lines)
    text_msg += '\r\n\r\n [Image Rev Info]: \r\n' + img_rev_lines
    msg.attach(MIMEText(text_msg, 'plain', 'utf-8'))
    att1 = MIMEText(open(glb_log_file, 'rb').read(), 'base64', 'utf-8')
    att1["Content-Type"] = 'application/octet-stream'
    att1["Content-Disposition"] = 'attachment; filename="sanity-test-log.txt"'
    msg.attach(att1)
    try:
        smtpObj = smtplib.SMTP(mail_host, mail_port)
        smtpObj.starttls()
        smtpObj.login(my_mail, mail_pwd)
        smtpObj.sendmail(my_mail, mailto_list, msg.as_string())
        smtpObj.quit()
        print("Email send success")
    except smtplib.SMTPException as e:
        print("Email send failed", e)

key_words = ['error', 'err', 'ERROR', 'failure', 'Unknown']
def log_no_errors(target, child=''):
    global glb_log_file
    if not os.path.exists(glb_log_file):
        return False
    else:
#        print(glb_log_file)
        last_lines = get_file_last_line(os.path.abspath(glb_log_file), 100)
#        print(last_lines)
        if target == 'g3':
            no_error_tag = 'root@g3-eng:~# '
        elif target == 'g3hgu':
            no_error_tag = 'root@g3hgu-eng:~# '
        elif target == 'saturn-sfu':
            no_error_tag = 'root@saturn-sfu-eng:~# '
        elif target == 'venus':
            no_error_tag = 'root@venus-eng:~# '
        elif target == 'saturn2-sfu':
            no_error_tag = 'root@saturn2-sfu-eng:~# '
        else:
            print("Target %s is vaild" % target)
        if no_error_tag in last_lines:
            for word in key_words:
                if word in last_lines:
#                    print(last_lines)
                    return False
            return True
        else:
#            print(last_lines)
            return False

class SftpTool(object):
    def __init__(self, username, password, port, host):
        self.user = username
        self.password = password
        self.port = port
        self.ip = host
    def connect(self):
#        try:
        self.ssh = paramiko.SSHClient()
        self.ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.ssh.connect(self.ip, self.port, self.user, self.password)
        print("Host[%s] connect created!!" % self.ip)
#        except Exception as e:
#            print("ERROR: Failed to connect to Host[%s]!!" % self.ip)
    def ls_cmd(self, target_path_abs):
        self.stdin, self.stdout, self.stderr = self.ssh.exec_command("ls " + target_path_abs)
        stdout_str = self.stdout.read().decode('ascii')
#        print(stdout_str.split())
        return stdout_str.split()
    def input(self, local_file, remote_file):
        self.local_file_abs = local_file 
        self.remote_file_abs = remote_file
    def put(self):
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())
        sftp = self.ssh.open_sftp()
        sftp.put(self.local_file_abs, self.remote_file_abs)
        sftp.close()
    def get(self):
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())
        sftp = self.ssh.open_sftp()
        sftp.get(self.remote_file_abs, self.local_file_abs)
        sftp.close()
    def close(self):
        self.ssh.close()
        print("Host[%s] connect closed!!" % self.ip)

def download_img(obj, current_path, config, target, child=''):
    global glb_img_rev_file
    global glb_local_path
    y_m_d = date.today().strftime('%Y-%m-%d')
    y_m = date.today().strftime('%Y-%m')
    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    local_path_abs = os.path.join(current_path, local_path)
    if not os.path.exists(local_path_abs):
        os.makedirs(local_path_abs)
    glb_local_path = local_path_abs
    if target == 'g3' or target == 'g3hgu' or target == 'venus' or target == 'saturn2-sfu':
        target_path = path_info.get(target + '_path', None)
        local_backup_path_abs = os.path.join(local_path_abs, target)
        if target == 'g3hgu':
            img_rev_info = target + '-eng.major-image.R1.1-Release-' + y_m_d + '-rev.txt'
        else:
            img_rev_info = target + '-eng.major-image.' + y_m_d + '-rev.txt'
        log_file = y_m_d +'-'+ target + '-sanitytest-log.txt';
    elif target == 'saturn-sfu':
        local_backup_path_abs = os.path.join(local_path_abs, target +'_'+ child)
        img_rev_info = target + '-eng_'+ child +'.major-image.' + y_m_d + '-rev.txt'
        log_file = y_m_d +'-'+ child + '-sanitytest-log.txt';
        target_path = path_info.get(child + '_path', None)
    else:
        print("Target[%s] is invalid!!" % target)
#    print(local_backup_path_abs)
    remote_path = path_info.get('remote_path', None)
    remote_path_abs = remote_path + target +'/'+ y_m +'/'+ y_m_d + target_path
#    print(remote_path_abs)
    return_items = getattr(obj, "ls_cmd")(remote_path_abs)
#    print(return_items)
    if log_file in return_items:
        print("%s HAD put on the server already!!!" % log_file)
        return False
    old_file_list = os.listdir(local_path_abs)
    for item in old_file_list:
        if not os.path.isdir(local_path_abs + item):
            os.remove(local_path_abs + item)
        else:
            pass
    if not os.path.exists(local_backup_path_abs):
        os.makedirs(local_backup_path_abs)
    else:
        old_backfile_list = os.listdir(local_backup_path_abs)
        for item in old_backfile_list:
            os.remove(local_backup_path_abs +'/' + item)
    target_info = read_ini(config, target)
    ret = True
    for item in target_info.values():
            if not '-sanitytest-log.txt' in item:
                if item  not in return_items:
                    print("%s WAS NOT Found on server!!!" % item)
                    ret = False
                    break
                else:
                    pass
                remote_file = remote_path_abs + item
#                print(remote_file)
                local_backup_file = os.path.join(local_backup_path_abs, item)
                if item == 'uboot-env.bin':
                    local_file = os.path.join(local_path_abs, target +'-'+ item)
                else:
                    local_file = os.path.join(local_path_abs, item)
                getattr(obj, "input")(local_file, remote_file)
                getattr(obj, "get")()
                time.sleep(1)
#                print(local_backup_file + '\r\n')
                getattr(obj, "input")(local_backup_file, remote_file)
                getattr(obj, "get")()
                time.sleep(1)
    local_img_rev_info = os.path.join(local_path_abs, img_rev_info)
#    print(local_img_rev_info)
    remote_img_rev_info = remote_path_abs + img_rev_info
#    print(remote_img_rev_info)
    if img_rev_info in return_items:
#        print("%s WAS Found on server!!!" % img_rev_info)
        getattr(obj, "input")(local_img_rev_info, remote_img_rev_info)
        getattr(obj, "get")()
        glb_img_rev_file = local_img_rev_info
    else:
        print("%s WAS NOT Found on server!!!" % img_rev_info)
        ret = False
    return ret

def upload_log(obj, current_path, config, target, child=''):
    global glb_log_file
    y_m_d = date.today().strftime('%Y-%m-%d')
    y_m = date.today().strftime('%Y-%m')
    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    local_path_abs = os.path.join(current_path, local_path)
#    print(local_path_abs)
    if not os.path.exists(local_path_abs):
        print("ERROR: local_path[%s] NOT exists!!" % local_path_abs)
    else:
        pass
    if target != 'saturn-sfu':
        log_file = target + '-sanitytest-log.txt';
        target_path = path_info.get(target + '_path', None)
    else:
        log_file = child + '-sanitytest-log.txt';
        if child == 'epon':
            target_path = path_info.get('epon_path', None)
        elif child == 'gpon':
            target_path = path_info.get('gpon_path', None)
        else:
            print("ERROR: Input saturn-sfu child[%s] is invalid!!" % child)
    remote_path = path_info.get('remote_path', None)
    target_info = read_ini(config, target)
    for item in target_info.values():
        if log_file == item:
            remote_file = remote_path + target +'/'+ y_m +'/'+ y_m_d + target_path + y_m_d +'-'+ item
#            print(remote_file)
            local_file = os.path.join(local_path_abs, y_m_d +'-'+ item)
#            print(local_file)
            glb_log_file = local_file
#            print(glb_log_file)
            if not os.path.exists(local_file):
                print("ERROR: local_file[%s] NOT exists!!" % local_file)
            else:
                glb_log_file = local_file
#                print(glb_log_file)
                getattr(obj, "input")(local_file, remote_file)
                getattr(obj, "put")()
                time.sleep(1)
        else:
            pass

def do_telnet(config, target):
    y_m_d = date.today().strftime('%Y-%m-%d')
    global glb_cmd_file
    telnet_info = read_ini(config, 'telnet')
    host = telnet_info.get('host', None)
    tftpboot_info = read_ini(config, 'tftpboot')
    saveenv_set = tftpboot_info.get('saveenv_set', None).encode('ascii')
    reset_set = tftpboot_info.get('reset_set', None).encode('ascii')
    if target == 'g3' or target == 'g3hgu' or target == 'venus':
        port = int(telnet_info.get(target + '_port', None))
        activeport_set = tftpboot_info.get(target + '_activeport_set', None).encode('ascii')
        ipaddr_set = tftpboot_info.get(target + '_ipaddr_set', None).encode('ascii')
        serverip_set = tftpboot_info.get(target + '_serverip_set', None).encode('ascii')
        tftpboot_gpt = tftpboot_info.get(target + '_tftpboot_gpt', None).encode('ascii')
        upgrade_gpt = tftpboot_info.get(target + '_upgrade_gpt', None).encode('ascii')
        tftpboot_ubootenv = tftpboot_info.get(target + '_tftpboot_ubootenv', None).encode('ascii')
        upgrade_ubootenv = tftpboot_info.get(target + '_upgrade_ubootenv', None).encode('ascii')
        if target == 'venus':
            upgrade_ubootenv_2 = tftpboot_info.get(target + '_upgrade_ubootenv_2', None).encode('ascii')
        tftpboot_image = tftpboot_info.get(target + '_tftpboot_image', None).encode('ascii')
        upgrade_image = tftpboot_info.get(target + '_upgrade_image', None).encode('ascii')
        if target == 'venus':
            upgrade_image_2 = tftpboot_info.get(target + '_upgrade_image_2', None).encode('ascii')
        if target == 'g3':
            uboot_tag = b"G3# "
            cmdline_tag = b"root@g3-eng:~# "
        elif target == 'g3hgu':
            uboot_tag = b"G3# "
            cmdline_tag = b"root@g3hgu-eng:~# "
        elif target == 'venus':
            uboot_tag = b"VENUS# "
            cmdline_tag = b"root@venus-eng:~# "
        else:
            pass
        written_ok = b"written: OK"
        tftp_done = b"done"
        if target != 'venus':
            cmd_list = [tftpboot_ubootenv,
                        upgrade_ubootenv,
                        tftpboot_image,
                        upgrade_image,
                        ]
        else:
            cmd_list = [tftpboot_gpt,
                        upgrade_gpt,
                        tftpboot_ubootenv,
                        upgrade_ubootenv,
                        upgrade_ubootenv_2,
                        tftpboot_image,
                        upgrade_image,
                        upgrade_image_2,
                        ]
    elif target == 'saturn-sfu' or target == 'saturn2-sfu':
        port = int(telnet_info.get(target + '_port', None))
        activeport_set = tftpboot_info.get(target + '_activeport_set', None).encode('ascii')
        ipaddr_set = tftpboot_info.get(target + '_ipaddr_set', None).encode('ascii')
        serverip_set = tftpboot_info.get(target + '_serverip_set', None).encode('ascii')
        tftpboot_gpt = tftpboot_info.get(target + '_tftpboot_gpt', None).encode('ascii')
        upgrade_gpt = tftpboot_info.get(target + '_upgrade_gpt', None).encode('ascii')
        tftpboot_ubootenv = tftpboot_info.get(target + '_tftpboot_ubootenv', None).encode('ascii')
        upgrade_ubootenv = tftpboot_info.get(target + '_upgrade_ubootenv', None).encode('ascii')
        tftpboot_dtb = tftpboot_info.get(target + '_tftpboot_dtb', None).encode('ascii')
        upgrade_dtb = tftpboot_info.get(target + '_upgrade_dtb', None).encode('ascii')
        tftpboot_image = tftpboot_info.get(target + '_tftpboot_image', None).encode('ascii')
        upgrade_image = tftpboot_info.get(target + '_upgrade_image', None).encode('ascii')
        tftpboot_rootfs = tftpboot_info.get(target + '_tftpboot_rootfs', None).encode('ascii')
        upgrade_rootfs = tftpboot_info.get(target + '_upgrade_rootfs', None).encode('ascii')
        tftpboot_userubi = tftpboot_info.get(target + '_tftpboot_userubi', None).encode('ascii')
        upgrade_userubi = tftpboot_info.get(target + '_upgrade_userubi', None).encode('ascii')
        uboot_tag = b"SATURN# "
        if target == 'saturn-sfu':
            cmdline_tag = b"root@saturn-sfu-eng:~# "
        elif target == 'saturn2-sfu':
            cmdline_tag = b"root@saturn2-sfu-eng:~# "
        else:
            print("ERROR: Input target[%s] is invalid!!" % target)
        written_ok = b"Written: OK"
        tftp_done = b"done"
        cmd_list = [tftpboot_gpt,
                    upgrade_gpt,
                    tftpboot_ubootenv,
                    upgrade_ubootenv,
                    tftpboot_dtb,
                    upgrade_dtb,
                    tftpboot_image,
                    upgrade_image,
                    tftpboot_rootfs,
                    upgrade_rootfs,
                    tftpboot_userubi,
                    upgrade_userubi
                    ]
    else:
        print("ERROR: Input target[%s] is invalid!!" % target)
#    print(cmd_list)
    glb_cmd_file = glb_local_path + y_m_d +'-'+ target + "-cmd_list.txt"
    run_cmds = [str(i.decode('ascii')) for i in cmd_list]
#    print('\r\n'.join(run_cmds))
    with open(glb_cmd_file, 'w') as f:
        f.write('\r\n'.join(run_cmds))
    serverip = serverip_set.split()[2]
    print(serverip)
    print(host, port)
    tn = telnetlib.Telnet(host, port, timeout=50)
    tn.write(b"\r\n")
    time.sleep(1)
    tn.write(b"root\n")
    time.sleep(1)
    i, tag, read_all = tn.expect([uboot_tag, cmdline_tag])
#    print(tag)
    if i == 0:
        time.sleep(1)
        tn.write(b"reset\n")
    elif i == 1:
        time.sleep(1)
        tn.write(b"\r\n")
        time.sleep(1)
        tn.write(b"reboot\n")
    else:
        print("ERROR: do_telnet() capture log !")
    time.sleep(1)
    tn.read_until(b"Hit any key to stop autoboot: ")
    tn.write(b"\r\n")
    time.sleep(1)
    tn.write(activeport_set + b"\r\n")
    tn.write(serverip_set + b"\r\n")
    tn.write(ipaddr_set + b"\r\n")
    tn.write(saveenv_set + b"\r\n")
    tn.write(reset_set + b"\r\n")
    tn.read_until(b"Hit any key to stop autoboot: ")
    tn.write(b"\n")
    time.sleep(1)
    log_str = tn.read_until(uboot_tag)
    time.sleep(1)
    if target == 'g3':
        pass
    else:
#    print(tftpboot_gpt)
        tn.write(tftpboot_gpt + b"\n")
        log_str += (tn.read_until(tftp_done))
        time.sleep(1)
#    print(upgrade_gpt)
        tn.write(upgrade_gpt + b"\n")
        log_str += (tn.read_until(written_ok))
        time.sleep(1)
#    print(tftpboot_ubootenv)
    tn.write(tftpboot_ubootenv + b"\n")
#    time.sleep(1)
    log_str += (tn.read_until(tftp_done))
    time.sleep(1)
#    print(upgrade_ubootenv)
    tn.write(upgrade_ubootenv + b"\n")
    log_str += (tn.read_until(written_ok))
    time.sleep(1)
    if target == 'venus':
        tn.write(upgrade_ubootenv_2 + b"\n")
        log_str += (tn.read_until(written_ok))
        time.sleep(1)
#    print(tftpboot_image)
    tn.write(tftpboot_image + b"\n")
    log_str += (tn.read_until(tftp_done))
    time.sleep(1)
#    print(upgrade_image)
    tn.write(upgrade_image + b"\n")
    log_str += (tn.read_until(written_ok))
    if target == 'venus':
        tn.write(upgrade_image_2 + b"\n")
        log_str += (tn.read_until(written_ok))
    else:
        pass
    if target =='saturn-sfu' or 'saturn2-sfu':
        time.sleep(1)
#        print(tftpboot_dtb)
        tn.write(tftpboot_dtb + b"\n")
        log_str += (tn.read_until(tftp_done))
        time.sleep(1)
#        print(upgrade_dtb)
        tn.write(upgrade_dtb + b"\n")
        log_str += (tn.read_until(written_ok))
        time.sleep(1)
#        print(tftpboot_rootfs)
        tn.write(tftpboot_rootfs + b"\n")
        log_str += (tn.read_until(tftp_done))
        time.sleep(1)
#        print(upgrade_rootfs)
        tn.write(upgrade_rootfs + b"\n")
        log_str += (tn.read_until(written_ok))
        time.sleep(1)
#        print(tftpboot_userubi)
        tn.write(tftpboot_userubi + b"\n")
        log_str += (tn.read_until(tftp_done))
        time.sleep(1)
#        print(upgrade_userubi)
        tn.write(upgrade_userubi + b"\n")
        log_str += (tn.read_until(written_ok))
    else:
        pass
    time.sleep(1)
    tn.write(reset_set + b"\r\n")
    time.sleep(100)
    tn.write(b"root\n")
    time.sleep(1)
    tn.write(b"\r\n")
    time.sleep(1)
    tn.write(b"uname -a\n")
    time.sleep(1)
    tn.write(b"\r\n")
    time.sleep(1)
    tn.write(b"lsmod\n")
    time.sleep(1)
    tn.write(b"\r\n")
    tn.write(b"cat /proc/mtd\n")
    time.sleep(1)
    tn.write(b"\r\n")
    tn.write(b"cat /proc/iomem\n")
    time.sleep(1)
    tn.write(b"\r\n")
    if target =='saturn-sfu':
        tn.write(b"ifconfig eth0 192.168.1.12 up\r\n")
        time.sleep(1)
        tn.write(b"\r\n")
    else:
        pass
    tn.write(b"ping -c10 " + serverip + b"\r\n")
    time.sleep(15)
    tn.write(b"\r\n")
    time.sleep(1)
    tn.write(b"app-cli\r\n")
    time.sleep(5)
    tn.write(b"\r\n")
    time.sleep(1)
    tn.write(b"enable\r\n")
    time.sleep(2)
    tn.write(b"config\r\n")
    time.sleep(3)
    tn.write(b"logout\r\n")
    time.sleep(2)
    tn.write(b"\r\n")
    log_str += (tn.read_very_eager())
    tn.close()
    return (str(log_str.decode('utf-8', errors='ignore')))

def capture_log(current_path, config, target, child=''):
    y_m_d = date.today().strftime('%Y-%m-%d')
    y_m = date.today().strftime('%Y-%m')
    path_info = read_ini(config, 'path')
    local_path = path_info.get('local_path', None)
    local_path_abs = os.path.join(current_path, local_path)
    if not os.path.exists(local_path_abs):
        os.makedirs(local_path_abs)
    else:
        pass
    if target == 'g3' or target == 'g3hgu' or target == 'venus' or target == 'saturn2-sfu':
        log_file_path = local_path_abs + y_m_d +'-'+ target +'-sanitytest-log.txt'; 
    elif target == 'saturn-sfu':
        log_file_path = local_path_abs + y_m_d +'-'+ child +'-sanitytest-log.txt'; 
    else:
        pass
#    print(log_file_path)
    log = do_telnet(config, target)
#    print(log)
    with open(log_file_path, 'w') as f:
        f.write(log)

img_ok = {'g3':False, 'g3hgu':False, 'epon':False, 'gpon':False, 'venus':False, 'saturn2-sfu':False}
log_ok = {'g3':False, 'g3hgu':False, 'epon':False, 'gpon':False, 'venus':False, 'saturn2-sfu':False}
def main():
    current_path = sys.argv[0].rstrip('/sanity_test.py')
#    print(current_path)
    config = os.path.join(current_path, 'config/dailybuild_server_config.ini')
#    print(config)
    ssh_info = read_ini(config, 'ssh')
    host = ssh_info.get('host', None)
    port = int(ssh_info.get('port', None))
    username = ssh_info.get('username', None)
    password = ssh_info.get('password', None)
    target_list = ssh_info.get('target', None)
#    print(target_list.split(' '))
    child_list = ssh_info.get('child', None)
#    print(child_list.split(' '))
    obj = SftpTool(username, password, port, host)
    getattr(obj, "connect")()

    while True:
        for target in target_list.split(' '):
            if target == 'saturn-sfu': #saturn-sfu
                for child in child_list.split(' '):
                    img_ok[child] = download_img(obj, current_path, config, target, child)
                    print("STATUS: %s_%s img download is %s" % (target, child, img_ok[child]))
#                    print(target +'_'+ child, img_ok[target + '_' + child])
                    time.sleep(2)
                    if img_ok[child] == True:
                        capture_log(current_path, config, target, child)
                        time.sleep(2)
                        upload_log(obj, current_path, config, target, child)
                        time.sleep(2)
                        log_ok[child] = log_no_errors(target, child)
                        time.sleep(2)
                        if log_ok[child] == False:
                            print("ERROR: %s_%s log file has checked errors & send emails!" % (target, child))
                            send_email(config, target, child)
                        else:
                            print("OKAY: %s_%s log file not checked errors" % (target, child))
            else: #g3/g3hgu/venus/saturn2-sfu
                img_ok[target] = download_img(obj, current_path, config, target)
                print("STATUS: %s img download is %s" % (target, img_ok[target]))
#                print(target, img_ok[target])
                time.sleep(2)
                if img_ok[target] == True:
                    capture_log(current_path, config, target)
                    time.sleep(2)
                    upload_log(obj, current_path, config, target)
                    time.sleep(2)
                    log_ok[target] = log_no_errors(target)
                    time.sleep(2)
                    if log_ok[target] == False:
                        print("ERROR: %s log file has checked errors & send emails!" % target)
                        send_email(config, target)
                    else:
                        print("OKAY: %s log file not checked errors" % target)
            result_list = []
            for value in img_ok.values():
                result_list.append(value)
#            print(result_list)
            if True in result_list:
#                print("DELAY: g3/epon/gpon/g3hgu/venus sanity test process sleep 5 minutes")
                time.sleep(1*60)
            else:
#                print("DELAY: g3/epon/gpon/g3hgu/venus sanity test process sleep 30 minutes")
                time.sleep(30*60)
    getattr(obj, "close")()

if __name__ == "__main__":
    main()
