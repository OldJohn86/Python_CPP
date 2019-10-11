#!/usr/bin/python
# -*- coding: utf-8 -*-

'''
Author: pengpeng.chen@cortina-access.com
Date:   2019-10-10
'''


import re
import os
from os import walk
import sys

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
[case1]:
    #ifdef AAAA
        data
    #endif
'''
def get_ifend_allIndex(lines):
    ifdef_all = []
    endif_all = []
    for i in range(len(lines)):
        if "#ifdef " in lines[i]:
            ifdef_all.append(i)
#            print("["+str(i)+":]" + lines[i])
        if "#endif" in lines[i]:
            endif_all.append(i)
#            print("["+str(i)+":]" + lines[i])
#    print(len(ifdef_all), ifdef_all)
#    print(len(endif_all), endif_all)
    return (ifdef_all, endif_all)

def get_ifend_1stIndex(lines, ifdef_all, endif_all):
    ifdef_1st = []
    endif_1st = []
    ifdef_1st.append(ifdef_all[0])
    for i in range(len(endif_all)):
#        print(i, endif_all[i])
        temp_cnt = 0
        for j in range(len(ifdef_all)):
            if ifdef_all[j] < endif_all[i]:
#                print(j, ifdef_all[j])
                temp_cnt += 1
        if i+1 == temp_cnt:
#            print(i, endif_all[i])
            endif_1st.append(endif_all[i])
    for endif in endif_1st:
        for ifdef in ifdef_all:
            if ifdef > endif:
                ifdef_1st.append(ifdef)
                break
#    print(len(ifdef_1st), ifdef_1st)
#    print(len(endif_1st), endif_1st)
    return (ifdef_1st, endif_1st)

def get_ifend_3rdIndex(lines, ifdef_all, endif_all):
    endif_3rd = []
    ifdef_3rd = []
    for i in range(len(endif_all)-2):
#        print(i, endif_all[i])
        if endif_all[i] > ifdef_all[i] and endif_all[i] > ifdef_all[i+1] and endif_all[i] > ifdef_all[i+2]:
            endif_3rd.append(endif_all[i])
            ifdef_3rd.append(ifdef_all[i+2])
#    print(len(ifdef_3rd), ifdef_3rd)
#    print(len(endif_3rd), endif_3rd)
    return (ifdef_3rd, endif_3rd)

def get_ifend_2ndIndex(lines, ifdef_all, endif_all):
    endif_2nd = []
    ifdef_2nd = []
    (ifdef_1st, endif_1st) = get_ifend_1stIndex(lines, ifdef_all, endif_all)
    (ifdef_3rd, endif_3rd) = get_ifend_3rdIndex(lines, ifdef_all, endif_all)
    ifdef_2nd = [v for v in ifdef_all if v not in ifdef_1st and v not in ifdef_3rd]
    endif_2nd = [v for v in endif_all if v not in endif_1st and v not in endif_3rd]
#    print(len(ifdef_2nd), ifdef_2nd)
#    print(len(endif_2nd), endif_2nd)
    return (ifdef_2nd, endif_2nd)

def ifend_keep(lines, macro):
    data = ''
    for line in lines:
        data += line
    return data

def ifend_find_3level(lines, macro):
    macro_array = []
    macro_str = ['', '', '', '', '', '', '', '', '','']
    index = 0
    (ifdef_all, endif_all) = get_ifend_allIndex(lines)
    (ifdef_1st, endif_1st) = get_ifend_1stIndex(lines, ifdef_all, endif_all)
    (ifdef_2nd, endif_2nd) = get_ifend_2ndIndex(lines, ifdef_all, endif_all)
    (ifdef_3rd, endif_3rd) = get_ifend_3rdIndex(lines, ifdef_all, endif_all)
    # check The 1st level #ifdef
    # check The 2nd level #ifdef
    # check The 3rd level #ifdef
    for (ifdef, endif) in [(ifdef_1st, endif_1st),(ifdef_2nd, endif_2nd),(ifdef_3rd, endif_3rd)]:
        print(ifdef, endif)
        for i in range(len(ifdef)):
#            print(lines[ifdef[i]].strip(' ').strip('\n').split(' '))
            if macro == (lines[ifdef[i]].strip(' ').strip('\n').split(' '))[1]:
                print(i, ifdef[i]+1, endif[i]+1)
                for i in range(ifdef[i], endif[i]+1):
                    macro_str[index] += lines[i]
#                print(macro_str[index])
                macro_array.append(macro_str[index])
                if index < len(macro_str):
                    index += 1
                else:
                    print("ERROR: macro_str[] list index out of range!")
                    break
            else:
#                print(lines[ifdef[i]].strip(' ').strip('\n').split(' '))
#                print(i, ifdef[i]+1, endif[i]+1)
                pass
    return macro_array

def ifend_find_iter(lines, macro):
    data_a = ''
    macro_array= []
    macro_str = ['', '', '', '', '', '', '', '', '','']
    index = 0
    ifdef_cnt = 0
    endif_cnt = 0
    for line in lines:
        if "#ifdef " in line:
            ifdef_cnt += 1
        if "#endif" in line:
            endif_cnt += 1
    if ifdef_cnt == endif_cnt == 0:
        return macro_array
    return ifend_find_iter(data_a, macro)


def ifend_remove(lines, macro):
    data = ''
    macro_array = ifend_find_3level(lines, macro)
    #macro_array = ifend_find_iter(lines, macro)
    print(macro_array)
    for line in lines:
        data += line
    for i in range(len(macro_array)):
        if data.find(macro_array[i]) != -1:
            data = data.replace(macro_array[i], '')
    return data

def ifend_test(f_name):
    print(f_name)
    data = ''
    with open(f_name, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        data = ifend_remove(lines, 'aaaa')
    with open(f_name.replace('input','output'), 'w', encoding='utf-8', errors='ignore') as f:
        f.writelines(data)

'''
[case2]:
    #ifdef AAAA
        data_a
    #else
        data_b
    #endif
'''
def parse_case2(lines, macro):
    length = len(lines)

'''
[case3]:
    #ifndef AAAA
        data_a
    #endif
'''
def parse_case3(lines, macro):
    length = len(lines)

'''
[case4]:
    #ifndef AAAA
        data_a
    #else
        data_b
    #endif
'''
def parse_case4(lines, macro):
    length = len(lines)

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
                    parse_ifend_(lines)
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
    ifend_test("./testCodes/ifend_input.c")
