# $language = "python"
# $interface = "1.0"

# tftpboot 0x85000000 major-image-saturn2-sfu-eng-gpt.img
# mtd erase spi-nand0 0 0x400000; mtd write spi-nand0 0x85000000 0x0 ${filesize};
# tftpboot 0x85000000 uboot-env.bin
# mtd erase spi-nand0 0x400000 0x100000; mtd write spi-nand0 0x85000000 0x400000 ${filesize};
# mtd erase spi-nand0 0x500000 0x100000; mtd write spi-nand0 0x85000000 0x500000 ${filesize};

# tftpboot 0x85000000 saturn2_sfu_c0.dtb
# mtd erase spi-nand0 0x600000 0x80000; mtd write spi-nand0 0x85000000  0x600000 ${filesize};
# tftpboot 0x85100000 uImage.lzma
# mtd erase spi-nand0 0x700000 0x600000; mtd write spi-nand0 0x85100000 0x700000 ${filesize};
# tftpboot 0x86000000 major-image-saturn2-sfu-c0.rootfs
# mtd erase spi-nand0 0xD00000 0x2800000 ;mtd write spi-nand0 0x86000000 0xD00000 ${filesize};

# tftpboot 0x85000000 major-image-saturn2-sfu-eng_user.ubi
# mtd erase spi-nand0 0x6400000 0x1400000 ;mtd write spi-nand0 0x85000000 0x6400000 ${filesize}

crt.Screen.Clear()
crt.Screen.Synchronous = True
crt.Screen.IgnoreCase = True


def main():
	# For Saturn upgrade
	crt.Screen.Send("reset\r\n")
	# crt.Screen.Send("\r\nreboot\r\n")
	
	if crt.Screen.WaitForStrings("Hit any key to stop autoboot:") == 1:
		crt.Screen.Send("\r\n")
	else:
		crt.Dialog.MessageBox("Waiting <Hit any key> Timed out!")

	# Setenv
	# crt.Screen.Send("setenv active_port 3;\r\n")
	# crt.Screen.Send("setenv serverip 192.168.1.128;\r\n")
	# crt.Screen.Send("setenv ipaddr 192.168.1.3;\r\n")
		
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
	if crt.Screen.WaitForStrings("SATURN2# ") == 1:
		crt.Screen.Send("tftpboot 0x85000000 saturn2_sfu_c0.dtb; ")
		crt.Screen.Send("mtd erase spi-nand0 0x600000 0x80000; mtd write spi-nand0 0x85000000  0x600000 ${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade dtb Timed out!")

	# Upgrade kernel
	if crt.Screen.WaitForStrings("SATURN2# ") == 1:
		crt.Screen.Send("tftpboot 0x85100000 uImage.lzma; ")
		crt.Screen.Send("mtd erase spi-nand0 0x700000 0x600000; mtd write spi-nand0 0x85100000 0x700000 ${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade kernel Timed out!")

	# Upgrade rootfs
	if crt.Screen.WaitForStrings("SATURN2# ") == 1:
		crt.Screen.Send("tftpboot 0x86000000 major-image-saturn2-sfu-c0.rootfs; ")
		crt.Screen.Send("mtd erase spi-nand0 0xD00000 0x2800000; mtd write spi-nand0 0x86000000 0xD00000 ${filesize};\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade rootfs Timed out!")

	# Upgrade user.ubi
	# if crt.Screen.WaitForStrings("SATURN2#") == 1:
		# crt.Screen.Send("tftpboot 0x87000000 major-image-saturn2-sfu-c0_user.ubi; ")
		# crt.Screen.Send("mtd erase spi-nand0 0x6400000 0x1400000 ;mtd write spi-nand0 0x87000000 0x6400000 ${filesize};\r\n")
	# else:
		# crt.Dialog.MessageBox("Upgrade user.ubi Timed out!")

	# Reset target when upgraded success this time
	if crt.Screen.WaitForStrings("SATURN2", 180) == 1:
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