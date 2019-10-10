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
def case1_parse(lines):
    # print(lines)
    ifdef_all = []
    endif_all = []
    endif_1st = []
    ifdef_1st = []
    endif_2nd = []
    ifdef_2nd = []
    # print(len(lines))
    for i in range(len(lines)):
        if "#ifdef " in lines[i]:
            ifdef_all.append(i)
            # print("["+str(i)+":]" + lines[i])
        if "#endif" in lines[i]:
            endif_all.append(i)
            # print("["+str(i)+":]" + lines[i])
    # print(len(ifdef_all), ifdef_all)
    # print(len(endif_all), endif_all)

    ifdef_1st.append(ifdef_all[0])
    for i in range(len(endif_all)):
        # print(i, endif_all[i])
        temp_cnt = 0
        for j in range(len(ifdef_all)):
            if ifdef_all[j] < endif_all[i]:
                # print(j, ifdef_all[j])
                temp_cnt += 1
        if i+1 == temp_cnt:
            # print(i, endif_all[i])
            endif_1st.append(endif_all[i])
    for endif in endif_1st:
        for ifdef in ifdef_all:
            if ifdef > endif:
                ifdef_1st.append(ifdef)
                break
    # print(len(ifdef_1st), ifdef_1st)
    # print(len(endif_1st), endif_1st)
    ifdef_2nd = [v for v in ifdef_all if v not in ifdef_1st]
    # print(len(ifdef_2nd), ifdef_2nd)
    endif_2nd = [v for v in endif_all if v not in endif_1st]
    # print(len(endif_2nd), endif_2nd)
    return (ifdef_1st, endif_1st, ifdef_2nd, endif_2nd)

def case1_keep(lines, macro):
    data = ''
    for line in lines:
        data += line
    return data

def find_macro(lines, macro):
    # print(lines)
    macro_array = []
    macro_str = ['', '', '', '', '']
    ifdef_1st = []
    endif_1st = []
    ifdef_2nd = []
    endif_2nd = []
    (ifdef_1st, endif_1st, ifdef_2nd, endif_2nd) = case1_parse(lines)
    print(len(ifdef_1st), ifdef_1st)
    print(len(endif_1st), endif_1st)
    print(len(ifdef_2nd), ifdef_2nd)
    print(len(endif_2nd), endif_2nd)
    index = 0
    # check The first level #ifdef
    for i in range(len(ifdef_1st)):
        # print(lines[ifdef_1st[i]].strip(' ').strip('\n').split(' '))
        if macro == (lines[ifdef_1st[i]].strip(' ').strip('\n').split(' '))[1]:
            print(i, ifdef_1st[i]+1, endif_1st[i]+1)
            for i in range(ifdef_1st[i], endif_1st[i]+1):
                macro_str[index] += lines[i]
            # print(str(index) +"*************************************************************")
            # print(macro_str[index])
            macro_array.append(macro_str[index])
            if index < len(macro_str):
                index += 1
            else:
                print("ERROR: macro_str[] list index out of range!")
                break
        else:
            # print(lines[ifdef_1st[i]].strip(' ').strip('\n').split(' '))
            # print(i, ifdef_1st[i]+1, endif_1st[i]+1)
            pass
    # check The second level #ifdef
    for i in range(len(ifdef_2nd)):
        # print(lines[ifdef_2nd[i]].strip(' ').strip('\n').split(' '))
        if macro == (lines[ifdef_2nd[i]].strip(' ').strip('\n').split(' '))[1]:
            print(i, ifdef_2nd[i]+1, endif_2nd[i]+1)
            for i in range(ifdef_2nd[i], endif_2nd[i]+1):
                macro_str[index] += lines[i]
            # print(str(index) +"*************************************************************")
            # print(macro_str[index])
            macro_array.append(macro_str[index])
            if index < len(macro_str):
                index += 1
            else:
                print("ERROR: macro_str[] list index out of range!")
                break
        else:
            # print(lines[ifdef_2nd[i]].strip(' ').strip('\n').split(' '))
            # print(i, ifdef_2nd[i]+1, endif_2nd[i]+1)
             pass
    return (macro_array)

def case1_remove(lines, macro):
    # print(lines)
    data = ''
    macro_array = []
    macro_array = find_macro(lines, macro)
    print(macro_array)
    for line in lines:
        data += line
    for i in range(len(macro_array)):
        if data.find(macro_array[i]) != -1:
            data = data.replace(macro_array[i], ' ')
    return data

def case1_test(f_name):
    print(f_name)
    data = ''
    with open(f_name, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        data = case1_remove(lines, 'CCCC')
        # print(data)
    with open(f_name.replace('.c','.bak.c'), 'w', encoding='utf-8', errors='ignore') as f:
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
