# $language = "python"
# $interface = "1.0"


# tftpboot 0x85000000  gpt.img
# spi_nand erase 0x0000000 0x00400000; spi_nand write 0x85000000  0x0000000 0x300000
# tftpboot 0x85000000  uboot-env.bin
# spi_nand erase 0x0400000 0x00100000; spi_nand write 0x85000000  0x0400000 0x20000
# tftpboot 0x85000000  saturn_sfu.dtb
# spi_nand erase 0x0500000 0x00100000; spi_nand write 0x85000000  0x0500000 0x20000
# tftpboot 0x85000000  uImage
# spi_nand erase 0x0600000 0x00600000; spi_nand write 0x85000000  0x0600000 0x500000
# tftpboot 0x85000000  rootfs.squashfs.ubi
# spi_nand erase 0x0c00000 0x02800000; spi_nand write 0x85000000  0x0c00000 ${filesize}
# tftpboot 0x85000000  user.ubi
# spi_nand erase 0x6300000 0x01400000; spi_nand write 0x85000000  0x6300000 ${filesize}

crt.Screen.Clear()
crt.Screen.Synchronous = True
crt.Screen.IgnoreCase = True


def main():
	# For Saturn upgrade
	# crt.Screen.Send("\r\nreset\r\n")
	# crt.Screen.Send("\r\nreboot\r\n")
	
	# if crt.Screen.WaitForStrings("Hit any key to stop autoboot:") == 1:
		# crt.Screen.Send("\r\n")
	# else:
		# crt.Dialog.MessageBox("Waiting <Hit any key> Timed out!")

	# Setenv
	crt.Screen.Send("setenv active_port 3;\r\n")
	crt.Screen.Send("setenv serverip 192.168.1.128;\r\n")
	crt.Screen.Send("setenv ipaddr 192.168.1.3;\r\n")
		
	# Upgrade gpt
	# if crt.Screen.WaitForStrings("SATURN#") == 1:
		# crt.Screen.Send("tftpboot 0x84100000 major-image-saturn-sfu-eng-kt-gpt.img;")
		# crt.Screen.Send("spi_nand erase 0x0 0x400000; spi_nand write 0x84100000 0x0 0x300000;\r\n")
	# else:
		# crt.Dialog.MessageBox("Upgrade gpt Timed out!")

	# Upgrade uboot-env
	# if crt.Screen.WaitForStrings("Written: OK") == 1:
		# crt.Screen.Send("tftpboot 0x84000000 uboot-env-saturn-sfu-eng-kt.bin;")
		# crt.Screen.Send("spi_nand erase 0x400000 0x100000; spi_nand write 0x84000000 0x400000 0x20000;\r\n")
	# else:
		# crt.Dialog.MessageBox("Upgrade uboot-env Timed out!")

	# Upgrade dtb
	if crt.Screen.WaitForStrings("SATURN#") == 1:
		crt.Screen.Send("tftpboot 0x85000000 uImage-saturn_sfu_c20.dtb;")
		crt.Screen.Send("spi_nand erase 0x500000 0x100000; spi_nand write 0x85000000 0x500000 0x20000;\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade dtb Timed out!")
	# Upgrade kernel
	if crt.Screen.WaitForStrings("Written: OK") == 1:
		crt.Screen.Send("tftpboot 0x85100000 uImage;")
		crt.Screen.Send("spi_nand erase 0x600000 0x600000; spi_nand write 0x85100000 0x600000 0x500000;\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade kernel Timed out!")

	# Upgrade rootfs
	if crt.Screen.WaitForStrings("Written: OK") == 1:
		crt.Screen.Send("tftpboot 0x86000000 major-image-saturn-sfu-eng-kt.rootfs.squashfs.ubi;")
		crt.Screen.Send("spi_nand erase 0xC00000 0x2800000; spi_nand write 0x86000000 0xC00000 ${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade rootfs Timed out!")

	# Upgrade user.ubi
	# if crt.Screen.WaitForStrings("Written: OK") == 1:
		# crt.Screen.Send("tftpboot 0x87000000 major-image-saturn-sfu-eng-kt_user.ubi;")
		# crt.Screen.Send("spi_nand erase 0x6300000 0x1400000; spi_nand write 0x87000000 0x6300000 ${filesize};\r\n")
	# else:
		# crt.Dialog.MessageBox("Upgrade user.ubi Timed out!")

	# Reset target when upgraded success this time
	if crt.Screen.WaitForStrings("Written: OK", 180) == 1:
		crt.Screen.Send("reset\r\n")
	else:
		crt.Dialog.MessageBox("Total Upgrade Images Timed out!")

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