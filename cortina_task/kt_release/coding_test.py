
import os

f = open("./testCodes/epon.c","rb")#二进制格式读文件
while True:
    line = f.readline()
    if not line:
        break
    else:
        try:
        #print(line.decode('utf8'))
            line.decode('utf8')
        except:
            print(str(line))
