# $language = "python"
# $interface = "1.0"

#官方的实例可以在未连接任何服务器的情况下进行ssh连接

#你要连接的ssh服务器ip
host = '192.168.44.20'
#ssh用户名
user = 'pchen'
#ssh密码
passwd = '1q2w3e4R'

def main():

    #进行cmd操作连接创建新的session连接
    cmd = "/SSH2 /L %s /PASSWORD %s /C 3DES /M MD5 %s" % (user, passwd, host)
    crt.Session.Connect(cmd)
    #使用默认弹窗提示信息
    crt.Dialog.MessageBox('登录成功!')

main()