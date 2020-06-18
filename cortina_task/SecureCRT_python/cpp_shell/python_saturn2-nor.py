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

# tftpboot 0x85000000 major-image-saturn2-sfu-eng-nor-gpt.img
# sf erase 0x0 0x300000; sf write 0x85000000 0x0 ${filesize} 
# tftpboot 0x85000000 uboot-env.bin 
# sf erase 0x300000 0x100000; sf write 0x85000000 0x300000 ${filesize} 
# tftpboot 0x85000000 saturn2_sfu.dtb 
# sf erase 0x400000 0x100000; sf write 0x85000000  0x400000 ${filesize}
# tftpboot 0x85100000 uImage.lzma 
# sf erase 0x500000 0x300000; sf write 0x85100000 0x500000 ${filesize} 
# tftpboot 0x86000000  major-image-saturn2-sfu-eng-nor.squashfs-xz
# sf erase 0x800000 0x1700000 ;sf write 0x86000000 0x800000 ${filesize}
# sf erase 0x3a00000 0x600000;
def main():
	# For Saturn upgrade
#	crt.Screen.Send("\r\nreset\r\n")
#	crt.Screen.Send("\r\nreboot\r\n")
	
	if crt.Screen.WaitForStrings("Hit any key to stop autoboot:") == 1:
		crt.Screen.Send("\r\n")
	else:
		crt.Dialog.MessageBox("Waiting <Hit any key> Timed out!")

	# Setenv
	#crt.Screen.Send("setenv serverip 192.168.1.128;\r\n")
	#crt.Screen.Send("setenv ipaddr 192.168.1.8;\r\n")

	# Upgrade gpt
	# if crt.Screen.WaitForStrings("SATURN#") == 1:
		# crt.Screen.Send("tftpboot 0x84100000 saturn_sfu_eng_epon-gpt.img;")
		# crt.Screen.Send("spi_nand erase 0x0 0x400000; spi_nand write 0x84100000 0x0 0x300000;\r\n")
	# else:
		# crt.Dialog.MessageBox("Upgrade gpt Timed out!")

	# Upgrade uboot-env
	if crt.Screen.WaitForStrings("SATURN#") == 1:
		crt.Screen.Send("tftpboot 0x85000000 uboot-env.bin;")
		crt.Screen.Send("sf erase 0x300000 0x100000; sf write 0x85000000 0x300000 ${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade uboot-env Timed out!")

	# Upgrade kernel
	if crt.Screen.WaitForStrings("Written: OK") == 1:
		crt.Screen.Send("tftpboot 0x85000000 saturn2_sfu.dtb;")
		crt.Screen.Send("sf erase 0x400000 0x100000; sf write 0x85000000  0x400000 ${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade dtb Timed out!")

	# Upgrade kernel
	if crt.Screen.WaitForStrings("Written: OK") == 1:
		crt.Screen.Send("tftpboot 0x85100000 uImage.lzma;")
		crt.Screen.Send("sf erase 0x500000 0x300000; sf write 0x85100000 0x500000 ${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade kernel Timed out!")

	# Upgrade rootfs
	if crt.Screen.WaitForStrings("Written: OK") == 1:
		crt.Screen.Send("tftpboot 0x86000000  major-image-saturn2-sfu-eng-nor.squashfs-xz;")
		crt.Screen.Send("sf erase 0x800000 0x1700000 ;sf write 0x86000000 0x800000 ${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade rootfs Timed out!")

	# Upgrade user.ubi
	if crt.Screen.WaitForStrings("Written: OK") == 1:
		# crt.Screen.Send("tftpboot 0x87000000 major-image-saturn-sfu-eng_user.ubi;")
		crt.Screen.Send("sf erase 0x3a00000 0x600000; reset;\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade user.ubi Timed out!")

	# Reset target when upgraded success this time
	# if crt.Screen.WaitForStrings("Written: OK") == 1:
		# crt.Screen.Send("reset\r\n")
	# else:
		# crt.Dialog.MessageBox("Total Upgrade Images Timed out!")

	# if crt.Screen.WaitForStrings("saturn-sfu-eng login:") == 1:
		# crt.Screen.Send("\r\nroot\r\n")
	# else:
		# crt.Dialog.MessageBox("ROOT Login Timed out!")

	# if crt.Screen.WaitForStrings("root@saturn-sfu-eng:~#") == 1:
		# crt.Screen.Send("r\n")
		# if crt.Screen.WaitForStrings("ca-app: init done") == 1:
			# crt.Screen.Send("uname -a\r\n")
		# else:
			# crt.Dialog.MessageBox("ca-app: init done Timed out!")
	# else:
		# crt.Dialog.MessageBox("Get 'uname -a' Timed out!")

main()