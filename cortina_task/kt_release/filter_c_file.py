#!/usr/bin/python
# -*- coding: UTF-8 -*-
'''
'''
import re
import os
from os import walk


keywords = [
    '#if ',
    '#ifdef',
    '#ifndef',
    '#else',
    '#elif',
    '#endif'
]

macro = [
    'CONFIG_CORTINA_BOARD_FPGA',
    'CONFIG_ARCH_CORTINA_G3HGU',
    'CONFIG_ARCH_CORTINA_G3',
    'CONFIG_ARCH_CORTINA_G3Lite',
    'CONFIG_ARCH_CORTINA_SATURN_SFU',
    'CONFIG_ARCH_CORTINA_SATURN'
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

'''
[case 1]:
data_a =
[
    #ifdef AAAA
        data
    #endif
]
'''
def case1_parse(lines, macro):
    print(lines)
    #print(macro)
    data_a = []
    ifdef_index = []
    endif_index = []
    ret_endif = []
    ret_ifdef = []
    length = len(lines)
    #print(length)
    for i in range(length):
        if "#ifdef " in lines[i]:
            ifdef_index.append(i)
            #print("["+str(i)+":]" + lines[i])
        if "#endif" in lines[i]:
            endif_index.append(i)
            #print("["+str(i)+":]" + lines[i])
    #print(ifdef_index, endif_index)

    ret_ifdef.append(ifdef_index[0])
    for i in range(len(endif_index)):
        #print(i, endif_index[i])
        temp_cnt = 0
        for j in range(len(ifdef_index)):
            if ifdef_index[j] < endif_index[i]:
                #print(j, ifdef_index[j])
                temp_cnt += 1
        if i+1 == temp_cnt:
            #print(i, endif_index[i])
            ret_endif.append(endif_index[i])
    for endif in ret_endif:
        for ifdef in ifdef_index:
            if ifdef > endif:
                ret_ifdef.append(ifdef)
                break
    #print(ret_ifdef, ret_endif)

    for i in range(len(ret_ifdef)):
        #print(ret_ifdef[i], ret_endif[i])
        macro_flag = lines[ret_ifdef[i]].lstrip(' ').rstrip('\n').split(' ')
        #print(macro, macro_flag[1])
        if macro ==  macro_flag[1]:
            for i in range(ret_ifdef[i], ret_endif[i]+1):
                data_a.append(lines[i])
    #print(data_a)
    return (data_a)

def case1_keep(lines, macro):
    data = ''
    for line in lines:
        data += line
    return data

def case1_remove(lines, macro):
    data = ''
    macro_data = []
    macro_data = case1_parse(lines, macro)
    print(macro_data)

    for line in lines:
        data += line
    #print(data)
    return data

def case1_test(f_name):
    print(f_name)
    data = ''
    with open(f_name, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        data = case1_remove(lines, 'AAAA')
        #print(data)
    with open(f_name+".bak", 'w', encoding='utf-8', errors='ignore') as f:
        f.writelines(data)

'''
[case 2]:
    #ifdef AAAA
        data_a
    #else
        data_b
    #endif
'''
def parse_case2(lines, macro):
    length = len(lines)
'''
[case 3]:
    #ifndef AAAA
        data_a
    #endif
'''
def parse_case2(lines, macro):
    length = len(lines)
'''
[case 4]:
    #ifndef AAAA
        data_a
    #else
        data_b
    #endif
'''
def parse_case4(lines, macro):
    length = len(lines)

'''
        if "#if" in lines[i]:
            templist = lines[i].lstrip(' ').rstrip('\n').split(' ')
            if templist[1] == words:
                print('[' + str(i) + ']: ' + lines[i])
                print(templist)
                #for j in range(i+1, length):
                #    if "#elif" in lines[j]:
                #        print('[' + str(j) + ']: ' + lines[j])
                #    if "#endif" in lines[j]:
                #        print('[' + str(j) + ']: ' + lines[j])
                #        break
        if "#elif" in lines[i]:
                print('[' + str(i) + ']: ' + lines[i])
        if "#else" in lines[i]:
                print('[' + str(i) + ']: ' + lines[i])
        if "#endif" in lines[i]:
                print('[' + str(i) + ']: ' + lines[i])
'''
'''
'''
def deal_file(path):
    for root, dirs, files in os.walk(path):
        for f_name in files:
            if f_name.endswith('.c') or f_name.endswith('.h'):
                ff_name = os.path.join(root, f_name)
                print(ff_name)
                data = ''
                with open(os.path.join(root, f_name), 'r', encoding='utf-8', errors='ignore') as f:
                    lines = f.readlines()
                    parse_case1(lines)
'''
'''
def remove_headtail_blockline(path):
    for root, dirs, files in os.walk(path):
        for f_name in files:
            if f_name.endswith('.c') or f_name.endswith('.h'):
                ff_name = os.path.join(root, f_name)
                print(ff_name)
                data = ''
                with open(os.path.join(root, f_name), 'r', encoding='utf-8', errors='ignore') as f:
                    lines = f.readlines()
                    #for line in lines:
                    #    print(line)
                    length = len(lines)
                    for i in range(length):
                        if (i >= 0 and i < 5) or (i >= length-5 and i < length):
                            if lines[i] == '\n':
                                print(i)
                                pass
                            else:
                                data += lines[i]
                        else:
                            data += lines[i]
                with open(os.path.join(root, f_name), 'w', encoding='utf-8', errors='ignore') as f:
                    f.writelines(data)
'''
                    #    lines.remove(lines[i])
                    #print(lines)
                    #print(lines)
                    #for i in range(0, len(lines)):
                    #    if lines[i] == '\n':
                    #        lines.remove(lines[i])
                    #print(lines)
                    #print(len(lines))
                    #for line in lines:
                        #print(line)
                    #    data += line
                    #print(data)

                #parse_ifdef(lines)

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
'''

if __name__ == '__main__':
    path='./testCodes/'
    
 #   remove_headtail_blockline(path) 
 #   deal_file(path)
    case1_test("./testCodes/demo_case1.c")
