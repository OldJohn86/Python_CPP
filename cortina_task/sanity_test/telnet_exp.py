#!/usr/bin/env python3

# -*- coding: utf-8 -*-


import telnetlib
import time

HOST = "192.168.41.251"
PORT = 2012

#with telnetlib.Telnet(HOST, PORT) as tn:
#    tn.write(b"ls /home/\n")
#print(tn.read_all().decode('ascii'))

def do_telnet(host):
    tn = telnetlib.Telnet(host, port=2012, timeout=50)
    
    # tn.read_until(b'login: ')
    # tn.write(username + b'\n')

    # tn.read_until(b'Password: ')
    # tn.write(password + b'\n')
    
    time.sleep(1)
    tn.write(b'root\n')
    time.sleep(1)
    tn.write(b'\r\n')
    tn.write(b'\r\n')
    tn.write(b'\r\n')
    tn.read_until(b'root@g3-eng:~# ')

    tn.write(b'reboot\n')
    time.sleep(50)
    result_str = tn.read_very_eager()
    return (result_str.decode('ascii', errors='ignore'))

    tn.write(b'exit\n')
    tn.close()
    
if __name__ == '__main__':
    host = '192.168.41.251'
    log_txt = do_telnet(host)
    print(log_txt)

    with open('./logfile/g3_santy_test.txt', 'w') as f:
        f.write(log_txt)

