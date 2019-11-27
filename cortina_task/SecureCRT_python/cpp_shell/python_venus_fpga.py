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



# tftp 0x100080000 Image;
# tftp 0x101000000 Image-ca8277-fpga.dtb;
# tftp 0x102000000 basic-image-venus-fpga.cpio.xz;
# booti 0x100080000 - 0x101000000

# For G3 sanity test dailybuild
def main():
	#crt.Screen.Send("\r\nreboot\r\n")
	#crt.Screen.Send("\r\nreset\r\n")

	#if crt.Screen.WaitForStrings("Hit any key to stop autoboot: ") == 1:
		#crt.Screen.Send("\r\n")
	#else:
		#crt.Dialog.MessageBox("Waiting <Hit any key> Timed out!")

	# Setenv
	#crt.Screen.Send("setenv active_port 1;\r\n")
    #crt.Screen.Send("print\r\n")
    #crt.Screen.Send("setenv serverip 192.168.1.128;\r\n")
	#crt.Screen.Send("setenv ipaddr 192.168.1.2;\r\n")
	#crt.Screen.Send("dcache on;\r\n")
    #crt.Screen.Send("print\r\n")

    # if crt.Screen.WaitForStrings("VENUS# ") == 1:
        # crt.Screen.Send("setenv serverip 192.168.1.128;\r\n")
    # else :
        # crt.Dialog.MessageBox("Upgrade uboot-env Timed out!")

	if crt.Screen.WaitForStrings("VENUS# ") == 1:
		crt.Screen.Send("tftp 0x100080000 Image;\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade gpt Timed out!")

	if crt.Screen.WaitForStrings("VENUS# ") == 1:
		crt.Screen.Send("tftp 0x101000000 Image-ca8277-fpga.dtb;\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade uboot-env Timed out!")

	if crt.Screen.WaitForStrings("VENUS# ") == 1:
		crt.Screen.Send("tftp 0x102000000 basic-image-venus-fpga.cpio.xz;\r\n")
	else:
		crt.Dialog.MessageBox("Upgrade kernel Timed out!")

    # if crt.Screen.WaitForStrings("VENUS# ") == 1:
		# crt.Screen.Send("booti 0x100080000 - 0x101000000\r\n")
	# else:
		# crt.Dialog.MessageBox("Upgrade kernel Timed out!")


main()