# $language = "python"
# $interface = "1.0"
#此方法表示你必须先登录一台ssh服务器然后再去ssh到另外一台服务器
#ssh主机的ip
host = '192.168.40.100'
#ssh主机的用户名
user = 'pchen'
#ssh主机的密码
passwd = '1q2w3e4R'

def main():
    #向屏幕光标后发送以下文字内容，\r表示回车执行
    crt.Screen.Send('ssh '+user+'@'+host+'  \r')
    #在5秒内等待屏幕出现password:或者yes/no文字进行下方操作，超过5秒表示超时
    result = crt.Screen.WaitForStrings(['password:','(yes/no)'],5)
    #当屏幕出现password:字符
    if result == 1:
        crt.Screen.Send(passwd+"\r")
    #屏幕出现(yes/no)等相关字符
    if result == 2:
        crt.Screen.Send('yes \r')
        crt.Screen.WaitForString('password:', 2)
        crt.Screen.Send(passwd+"\r")
    #等待屏幕出现']$'字符
    crt.Screen.WaitForString(']$',5)
    #使用默认弹窗提示信息
    crt.Dialog.MessageBox('登录成功!')

main()