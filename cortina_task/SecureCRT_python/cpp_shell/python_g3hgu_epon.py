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
# nand erase 0x0 0x400000
# nand write 0x05000000 0x0 0x300000
# tftpboot 0x05000000 uboot-env.bin
# nand erase 0x400000  0x100000
# nand write 0x05000000 0x400000 0x20000
# tftpboot 0x05000000 rootfs.mubi 
# ... Bytes transferred = 72876032 (4800000 hex)
# nand erase 0x500000 0xD000000
# nand write 0x05000000 0x500000 $filesize 

# For G3 sanity test dailybuild
def main():
	crt.Screen.Send("\r\nreboot\r\n")
	crt.Screen.Send("\r\nreset\r\n")

	if crt.Screen.WaitForStrings("Hit any key to stop autoboot:") == 1:
		crt.Screen.Send("\r\n")
	else:
		crt.Dialog.MessageBox("Waiting <Hit any key> Timed out!")

	# Setenv
	crt.Screen.Send("setenv active_port 1;\r\n")
	crt.Screen.Send("setenv serverip 192.168.1.1;\r\n")
	crt.Screen.Send("setenv ipaddr 192.168.1.2;\r\n")
	crt.Screen.Send("saveenv;\r\n")
	crt.Screen.Send("reset\r\n")

	if crt.Screen.WaitForStrings("Hit any key to stop autoboot:") == 1:
		crt.Screen.Send("\r\n")
	else:
		crt.Dialog.MessageBox("Waiting <Hit any key> Timed out!")

	# Upgrade gpt
	if crt.Screen.WaitForStrings("G3#") == 1:
		crt.Screen.Send("tftpboot 0x84100000 g3hgu_epon-gpt-nandinfo.img;")
		crt.Screen.Send("nand erase 0x0 0x400000; nand write 0x84100000 0x0 0x300000;\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade gpt Timed out!")

	# Upgrade uboot-env
	if crt.Screen.WaitForStrings("written: OK") == 1:
		crt.Screen.Send("tftpboot 0x84000000 g3hgu_epon-uboot-env.bin;")
		crt.Screen.Send("nand erase 0x400000 0x100000; nand write 0x84000000 0x400000 0x20000;\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade uboot-env Timed out!")

	# Upgrade kernel
	if crt.Screen.WaitForStrings("written: OK") == 1:
		crt.Screen.Send("tftpboot 0x85000000 g3hgu_epon-rootfs.mubi;")
		crt.Screen.Send("nand erase 0x500000 0xD000000; nand write 0x85000000 0x500000 0x${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade kernel Timed out!")

	# Reset target when upgraded success this time
	if crt.Screen.WaitForStrings("written: OK") == 1:
		crt.Screen.Send("reset\r\n")
	else:
		crt.Dialog.MessageBox("Total Upgrade Images Timed out!")

	if crt.Screen.WaitForStrings("g3-eng login:") == 1:
		crt.Screen.Send("\r\nroot\r\n")
	else:
		crt.Dialog.MessageBox("ROOT Login Timed out!")

	if crt.Screen.WaitForStrings("root@g3-eng:~#") == 1:
		crt.Screen.Send("r\n")
		crt.Screen.Send("r\n")
		crt.Screen.Send("uname -a\r\n")

	else:
		crt.Dialog.MessageBox("Get 'uname -a' Timed out!")

main()