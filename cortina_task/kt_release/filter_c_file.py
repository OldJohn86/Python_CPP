#!/usr/bin/python
# -*- coding: UTF-8 -*-
'''
'''

import os
from os import walk


keywords = [
    "#if "
    "#ifdef",
    "#ifndef",
    "#if 0",
    "#if defined",
    "#if undefined"
    "#if !defined"
    "#elif",
    "#endif"
]


def coding_verify(f_name):
    with open(f_name, 'rb') as f:
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

def parse_ifdef(lines):
    length = len(lines)
    print(length)
    for i in range(0, length):
        if "#if" in lines[i]:
            templist = []
            print('[' + str(i) + ']: ' + lines[i])
            #templist.append(lines[i].split(' '))
            #print(templist[0])
            print(lines[i].lstrip(' ').rstrip('\n').split(' '))

def filter_c_file(path):
    for root, dirs, files in os.walk(path):
        for f_name in files:
            if f_name.endswith('.c') or f_name.endswith('.h'):
                ff_name = os.path.join(root, f_name)
                print(ff_name)
                with open(os.path.join(root, f_name), 'r', encoding='utf-8', errors='ignore') as f:
                    lines = f.readlines()
                parse_ifdef(lines)

#                length = len(lines)
#                print(length)
#                result = []
#                i = 0
#                while i < length:
#                    for words in keywords:
#                        if words in lines[i]:
#                            print('[' + str(i) + ']: ' + lines[i])
#                    i += 1
                    #line = line.replace('\n', '').replace('\t', '').replace(' ', '').replace('\r', '')
                    #result.append(line)
                #f = open (path+filename.replace('.c', '.bak.c').replace ('.h', '.bak.h'), 'w')
                #for i in result:  # 写文件
                #    f.write (i)
                #f.close ()


if __name__ == '__main__':
    path='./testCodes/'
    filter_c_file(path)
