# $language = "python"
# $interface = "1.0"

#官方的实例可以在未连接任何服务器的情况下进行ssh连接

#你要连接的ssh服务器ip
#host = '192.168.40.100'
#ssh用户名
#user = 'pchen'
#ssh密码
#passwd = '1q2w3e4R'
crt.Screen.Clear()
crt.Screen.Synchronous = True
crt.Screen.IgnoreCase = True


# tftpboot 0x05000000 gpt-nandinfo.img
# nand erase 0x0 0x00400000
# nand write 0x05000000 0x0 0x00300000
# tftpboot 0x05000000 uboot-env.bin
# nand erase 0x00400000 0x100000
# nand write 0x05000000 0x400000 ${filesize}
# nand erase 0x00500000 0x100000 
# nand write 0x05000000 0x500000 ${filesize}
# tftpboot 0x05000000 rootfs.mubi 
# ... Bytes transferred = 72876032 (4800000 hex)
# nand erase 0x00600000 0x0D000000
# nand write 0x05000000 0x00600000 $filesize 
# nand erase 0x0D600000 0x0D000000            <--- Optional
# nand write 0x05000000 0x0D600000 $filesize  <--- Optional
# tftpboot 0x05000000 user.ubi                
# nand erase 0x1a600000 0x5800000 
# nand write 0x05000000 0x1a600000 ${filesize}

# For G3 sanity test dailybuild
def main():
	crt.Screen.Send("reboot\r\n")
	crt.Screen.Send("reset\r\n")

	if crt.Screen.WaitForStrings("Hit any key to stop autoboot:") == 1:
		crt.Screen.Send("\r\n")
	else:
		crt.Dialog.MessageBox("Waiting <Hit any key> Timed out!")

	# Setenv
	crt.Screen.Send("setenv active_port 4;\r\n")
	crt.Screen.Send("setenv serverip 192.168.1.130;\r\n")
	crt.Screen.Send("setenv ipaddr 192.168.1.5;\r\n")
	crt.Screen.Send("saveenv;\r\n")
	crt.Screen.Send("reset\r\n")

	if crt.Screen.WaitForStrings("Hit any key to stop autoboot:") == 1:
		crt.Screen.Send("\r\n")
	else:
		crt.Dialog.MessageBox("Waiting <Hit any key> Timed out!")

	# Upgrade gpt
	# if crt.Screen.WaitForStrings("VENUS#") == 1:
		# crt.Screen.Send("tftpboot 0x04100000 major-image-venus-eng-gpt-nandinfo.img;")
		# crt.Screen.Send("nand erase 0x0 0x400000; nand write 0x04100000 0x0 0x300000;\r\n")
	# else:
		# crt.Dialog.MessageBox("Upgrade gpt Timed out!")

	# Upgrade uboot-env
	# if crt.Screen.WaitForStrings("written: OK") == 1:
		# crt.Screen.Send("tftpboot 0x05000000 uboot-env.bin;")
		# crt.Screen.Send("nand erase 0x400000 0x80000; nand write 0x05000000 0x400000 0x20000;\r\n")
	# else:
		# crt.Dialog.MessageBox("Upgrade uboot-env Timed out!")

	# Upgrade kernel
	if crt.Screen.WaitForStrings("VENUS#") == 1:
		crt.Screen.Send("tftpboot 0x05000000 major-image-venus-eng.mubi;")
		crt.Screen.Send("nand erase 0x00600000 0x0D000000; nand write 0x5000000 0x00600000 0x${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade kernel Timed out!")

	if crt.Screen.WaitForStrings("written: OK") == 1:
		crt.Screen.Send("nand erase 0x0D600000 0x0D000000; nand write 0x5000000 0x0D600000 0x${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade gpt Timed out!")

	# if crt.Screen.WaitForStrings("written: OK") == 1:
		# crt.Screen.Send("tftpboot 0x05000000 major-image-g3-eng.mubi;")
		# crt.Screen.Send("nand erase 0xD500000 0xD000000; nand write 0x05000000 0xD500000 0x${filesize};\r\n")
	# else:
		# crt.Dialog.MessageBox("Upgrade kernel Timed out!")

	# Reset target when upgraded success this time
	if crt.Screen.WaitForStrings("written: OK") == 1:
		crt.Screen.Send("reset\r\n")
	else:
		crt.Dialog.MessageBox("Total Upgrade Images Timed out!")

	if crt.Screen.WaitForStrings("venus-eng login:") == 1:
		crt.Screen.Send("\r\nroot\r\n")
	else:
		crt.Dialog.MessageBox("ROOT Login Timed out!")

	if crt.Screen.WaitForStrings("root@venus-eng:~#") == 1:
		crt.Screen.Send("r\n")
		crt.Screen.Send("r\n")
		crt.Screen.Send("uname -a\r\n")
	else:
		crt.Dialog.MessageBox("Get 'uname -a' Timed out!")

main()
