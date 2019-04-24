# -*- utf-8 -*-
'''
Created on 2019-04-24 @CORTINA CPP
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
glb_kt_ver = '1.0.0.9.'
key_word = "21.00.00.09."

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

class SSHConnection(object):
	def __init__(self, host, port, username, password):
		self._host = host
		self._port = port
		self._username = username
		self._password = password
		self._transport = None
		self._sftp = None
		self._client = None
		self._connect()  # 建立连接

	def _connect(self):
		if self._transport is None:
			transport = paramiko.Transport((self._host, self._port))
			try:
				transport.connect(username=self._username, password=self._password)
				self._transport = transport
			except Exception as e:
				print(e)

	#下载
	def download(self, remotepath, localpath):
		if self._sftp is None:
			self._sftp = paramiko.SFTPClient.from_transport(self._transport)
		self._sftp.get(remotepath, localpath)

	#上传
	def put(self, localpath, remotepath):
		if self._sftp is None:
			self._sftp = paramiko.SFTPClient.from_transport(self._transport)
		self._sftp.put(localpath, remotepath)

	#执行命令
	def ssh_command(self, command):
		if self._client is None:
			self._client = paramiko.SSHClient()
			self._client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
			try:
				self._client.connect(self._host, self._port, self._username, self._password, timeout=10)
			except Exception as e:
				print(e)
		stdin, stdout, stderr = self._client.exec_command(command, get_pty=True)
		data = stdout.read()
		if len(data) > 0:
			#print(data.decode('utf8').strip()) #打印正确结果
			return data.decode('utf8').strip()
		err = stderr.read()
		if len(err) > 0:
			#print(err.decode('utf8').strip())  #输出错误结果
			return err.decode('utf8').strip()

	def invoke_shell_command(self, command):
		if self._client is None:
			self._client = paramiko.SSHClient()
			self._client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
			try:
				self._client.connect(self._host, self._port, self._username, self._password, timeout=10)
			except Exception as e:
				print(e)
		chan = self._client.invoke_shell()
		chan.sendall(command +'\n')
		lines = []
		while True:
			line = str(chan.recv(1024))
			if "Summary: " in line:
				lines.append(line)
				break
			lines.append(line)
		result = ' '.join(lines)
		#return result
		return line.decode('utf8').strip()

	def close(self):
		if self._transport:
			self._transport.close()
		if self._client:
			self._client.close()

# @#$%^ define ssh gitclone yocto
def yocto_gitclone(SSHConnection):
    cmd_gitclone = [
        'git clone -b kt-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/yocto yocto-'+datetag,
        'git clone -b kt-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-oe-yocto-cortina',
        'git clone -b kt-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-ca-bsp',
        'git clone -b kt-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/openembedded/meta-openembedded',
        'git clone -b kt-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-oe-ca'
	]
    for index, cmd in enumerate(cmd_gitclone):
        if index == 0:
            gitclone_path = SSHConnection.ssh_command(build_path + cmd)
        else:
            gitclone_path = SSHConnection.ssh_command(yocto_path + cmd)
        print(str(index) +': '+ gitclone_path)

#@#$%^&
def version_autoincrement(_file):
    global glb_kt_ver
    file_data = ""
    key_char = []
    with open(_file, "r", encoding="utf-8") as f:
        for line in f:
            if key_word in line:
                print("OLD_VER: " + line.strip('\n'))
                cur_ver = re.compile('"(.*)"').findall(line)
                key_char = cur_ver[-1].split('.')
                # print(key_char)
                key_char[-1] = str(int(key_char[-1]) + 1)
                glb_kt_ver = '1.0.0.9.' + key_char[-1]
                key_char[-1] = key_char[-1].zfill(2)
                # print(key_char)
                new_ver = ".".join(key_char)
                # print(new_ver)
                line = line.replace(cur_ver[-1], new_ver)
                print("NEW_VER: " + line.strip('\n'))
                print("KT_VER: " + glb_kt_ver)
            file_data += line
    with open(_file, "w", encoding="utf-8") as f:
        f.write(file_data)

# @#$%^ define modify release verion func
def update_version(SSHConnection):
    if not os.path.exists('./tmp'):
        os.makedirs('./tmp')
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
        localfile = to_file[index]
        # print(localfile)
        remotefile = yocto_path.split()[-1].replace(';', '/') + ff
        SSHConnection.download(remotefile, localfile)
        version_autoincrement(localfile)
        SSHConnection.put(localfile, remotefile)
        print(remotefile)
		
def build_image(SSHConnection):
	cmd_bitbake = 'TEMPLATECONF=meta-oe-ca/conf/saturn-sfu-eng-kt; source ./oe-init-build-env; bitbake major-image'
	build_path = SSHConnection.invoke_shell_command(yocto_path + cmd_bitbake)
	print(build_path)

def backup_image(SSHConnection):
	if not os.path.exists('./tmp/' + glb_kt_ver):
		os.makedirs('./tmp/' + glb_kt_ver)
	remote_image_path = yocto_path.split()[-1].replace(';', '/') + 'build/tmp/deploy/images/saturn-sfu-eng-kt/'
	print(remote_image_path)
	image_file = [
	'uImage',
	'saturn_sfu.dtb',
	'major-image-saturn-sfu-eng-kt-upgrade.kt.img',
	'major-image-saturn-sfu-eng-kt_user.ubi',
	'major-image-saturn-sfu-eng-kt.rootfs.squashfs.ubi'
	]
	local_image_path = './tmp/' + glb_kt_ver +'/'
	for index, img in enumerate(image_file):
		local_image = local_image_path + img
		print(local_image)
		remote_image = remote_image_path + img
		print(remote_image)
		SSHConnection.download(remote_image, local_image)

if __name__ == "__main__":
	info = read_ini()
	h = info.get('host', None)
	p = int(info.get('port', None))
	u = info.get('username', None)
	pw = info.get('password', None)
	# START connection
	conn = SSHConnection(h, p, u, pw)
	
	# STEP 1. 
	yocto_gitclone(conn) 
	# STEP 2.
	update_version(conn)
	# STEP 3.
	build_image(conn)
	# STEP 4.
	backup_image(conn)

	# CLOSE connection
	conn.close()
