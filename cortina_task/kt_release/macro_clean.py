#!/usr/bin/python3
# -*- coding: utf-8 -*-

'''
Author: pengpeng.chen@cortina-access.com
Date:   2019-10-10
'''

import re
import sys
import os
from os import walk
import argparse

keywords = [
    '#if',
    '#if ',
    '#if 0',
    '#if 1',
    '#ifdef',
    '#ifndef',
    '#elif',
    '#else',
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

'''
[case1]:
    #ifdef AAAA
        data
    #endif
'''
'''
[case3]:
    #ifndef AAAA
        data_a
    #endif
'''
def deal_startTabLine(line):
    if line.startswith('\t'):
#        print(line)
        line = line.replace('\t', '    ')
    return line

def deal_file(f_name):
#    print(f_name)
    data = ''
    with open(f_name, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        for line in lines:
            data += deal_startTabLine(line)
    with open(f_name, 'w', encoding='utf-8', errors='ignore') as f:
        f.writelines(data)

def get_ifend_allIndex(lines):
    if_allIndex = []
    endif_allIndex = []
    for i in range(len(lines)):
        lines[i] = deal_startTabLine(lines[i])
        if lines[i].lstrip(' ').startswith('#if'):
            if_allIndex.append(i)
#            print("["+str(i)+":]" + lines[i])
        if lines[i].lstrip(' ').startswith('#endif'):
            if '*/' in lines[i]:
                if '/*' in lines[i]:
                    endif_allIndex.append(i)
                else:
                    pass
            else:
                endif_allIndex.append(i)
#            print("["+str(i)+":]" + lines[i])
#    print(len(if_allIndex), if_allIndex)
#    print(len(endif_allIndex), endif_allIndex)
    return (if_allIndex, endif_allIndex)

def get_ifend_outerIndex(lines, if_allIndex, endif_allIndex):
    if_outer = []
    endif_outer = []
    if_outer.append(if_allIndex[0])
    for i in range(len(endif_allIndex)):
#        print(i, endif_allIndex[i])
        temp_cnt = 0
        for j in range(len(if_allIndex)):
            if if_allIndex[j] < endif_allIndex[i]:
#                print(j, if_allIndex[j])
                temp_cnt += 1
        if i+1 == temp_cnt:
#           print(i, endif_allIndex[i])
            endif_outer.append(endif_allIndex[i])
    for endif_index in endif_outer:
        for if_index in if_allIndex:
            if if_index > endif_index:
                if_outer.append(if_index)
                break
#    print(len(if_outer), if_outer)
#    print(len(endif_outer), endif_outer)
    return (if_outer, endif_outer)

def parse_outer_ifend(lines, macro):
    this_array = []
    next_array = []
    next_ab = ''
    (if_allIndex, endif_allIndex) = get_ifend_allIndex(lines)
    (if_outer, endif_outer) = get_ifend_outerIndex(lines, if_allIndex, endif_allIndex)
    outer_macro = [list('') for i in range(len(if_outer))]
    for i in range(len(if_outer)):
        for index in range(if_outer[i], endif_outer[i]+1):
            outer_macro[i].append(lines[index])
        parse_outer = parse_else_from_ifend(outer_macro[i])
        parse_outer[0] = deal_startTabLine(parse_outer[0])
        ifLine_list = parse_outer[0].lstrip(' ').strip('\n').split(' ')
#        print(macro, ifLine_list[1])
        if len(ifLine_list) > 2:
            print(ifLine_list)
        if macro == ifLine_list[1]:# matched
            this_array.append(parse_outer)
#            print(parse_outer)
        else:# unmatched
            next_ab += parse_outer[1] # A
            next_ab += parse_outer[3] # B
    next_lines = next_ab.split('\n')
#    print(len(next_lines))
    for i in range(len(next_lines)):
        next_array.append(next_lines[i] + '\n')
#    print(this_array)
#    print(next_array)
    return (this_array, next_array)

glb_macro_array = []
def iter_parse_ifend(lines, macro):
#    print(lines)
    ifdef_cnt = 0
    endif_cnt = 0
    global glb_macro_array
    for line in lines:
        line = deal_startTabLine(line)
        if line.lstrip(' ').startswith('#if'):
            ifdef_cnt += 1
        if line.lstrip(' ').startswith('#endif'):
            if '*/' in line:
                if '/*' in line:
                    endif_cnt += 1
                else:
                    pass
            else:
                    endif_cnt += 1
    print(ifdef_cnt, endif_cnt)
    if ifdef_cnt == endif_cnt == 0:
        return glb_macro_array
    else:
        (macro_array, data_array) = parse_outer_ifend(lines, macro)
        glb_macro_array.extend(macro_array)
#        print(glb_macro_array)
#        print(data_array)
        return iter_parse_ifend(data_array, macro)

def ifend_deal(lines, cmd, macro):
#    print(lines)
    data = ''
    for line in lines:
        data += line
    macro_array = iter_parse_ifend(lines, macro)
#    print(macro_array)
#    print(len(macro_array))
    old_data = ['' for i in range(len(macro_array))]
    new_data = ['' for i in range(len(macro_array))]
    for i in range(len(macro_array)):
#        print(macro_array[i])
#        print(len(macro_array[i]))
        for j in range(len(macro_array[i])):
            old_data[i] += macro_array[i][j]# A+B
#            print(macro_array[i][j])
#            print(len(macro_array[i][j]))
        print("OLD DATA: ")
        print(old_data[i])
        macro_array[i][0] = deal_startTabLine(macro_array[i][0])
        if cmd == "remove":
            if macro_array[i][0].lstrip(' ').startswith('#ifdef '):
                new_data[i] += macro_array[i][3]# B: index is 3 or -2
                print(macro_array[i][0])
#                print(macro_array[i][3])
            elif macro_array[i][0].lstrip(' ').startswith('#if 0'):
                new_data[i] += macro_array[i][3]# B: index is 3 or -2
                print(macro_array[i][0])
            elif macro_array[i][0].lstrip(' ').startswith('#if 1'):
                new_data[i] += macro_array[i][3]# B: index is 3 or -2
                print(macro_array[i][0])
            elif macro_array[i][0].lstrip(' ').startswith('#ifndef '):
                new_data[i] += macro_array[i][1]# A: index is 1
                print(macro_array[i][0])
            else:
                print("Cann't support!")
                break
        elif cmd == "keep":
            if macro_array[i][0].lstrip(' ').startswith('#ifdef '):
                new_data[i] += macro_array[i][1]# A: index is 1
                print(macro_array[i][0])
            elif macro_array[i][0].lstrip(' ').startswith('#if 0'):
                new_data[i] += macro_array[i][1]# A: index is 1
                print(macro_array[i][0])
            elif macro_array[i][0].lstrip(' ').startswith('#if 1'):
                new_data[i] += macro_array[i][1]# A: index is 1
                print(macro_array[i][0])
            elif macro_array[i][0].lstrip(' ').startswith('#ifndef '):
                new_data[i] += macro_array[i][3]# B: index is 3 or -2
                print(macro_array[i][0])
            else:
                print("Cann't support!")
                break
        else:
            print("ERROR: which input cmd is neither remove nor keep!!!")
            break
        print("New DATA: ")
        print(new_data[i])
        print(data.find(old_data[i]))
        if data.find(old_data[i]) != -1:
            print(data)
            data = data.replace(old_data[i], new_data[i])
    return data

'''
[case2]:
    #ifdef AAAA
        data_a
    #else
        data_b
    #endif
'''
'''
[case4]:
    #ifndef AAAA
        data_a
    #else
        data_b
    #endif
'''
def parse_else_from_ifend(lines):
    parse_str = ['' for i in range(5)]
    data_a = ''
    data_else = ''
    data_b = ''
    else_index= []
    for i in range(len(lines)):
        lines[i] = deal_startTabLine(lines[i])
        if lines[i].lstrip(' ').startswith('#else'):
#            print("["+str(i)+":]" + lines[i])
            else_index.append(i)
#    print(len(else_index), else_index)
    if len(else_index) == 0:
        for i in range(1, len(lines)-1):
            data_a += lines[i]
    else:
        for index in range(len(else_index)):
            if_cnt = 0
            end_cnt = 0
            for i in range(1, else_index[index]):
                lines[i] = deal_startTabLine(lines[i])
                if lines[i].lstrip(' ').startswith('#if'):
                    if_cnt += 1
                if lines[i].lstrip(' ').startswith('#endif'):
                    if '*/' in lines[i]:
                        if '/*' in lines[i]:
                            end_cnt += 1
                        else:
                            pass
                    else:
                        end_cnt += 1
#            print("if_cnt: "+ str(if_cnt) +"end_cnt: " + str(end_cnt))
            if if_cnt == end_cnt:
                for i in range(1, else_index[index]):
                    data_a += lines[i]
                for i in range(else_index[index]+1, len(lines)-1):
                    data_b += lines[i]
                data_else += lines[else_index[index]]
                break
            elif if_cnt > end_cnt:
                if index + 1 == len(else_index):
                    for i in range(1, len(lines)-1):
                        data_a += lines[i]
                    break
                else:
                    pass
            else:
                print("There should be error!!")
    parse_str[0] += lines[0]
    parse_str[1] += data_a
    parse_str[2] += data_else
    parse_str[3] += data_b
    parse_str[4] += lines[-1]
    return parse_str

def input_coding_verify(f_name):
#    print(f_name)
    cnt = 0
    with open(f_name, 'rb') as f:
        while True:
            line = f.readline()
            if not line:
                break
            else:
                try:
#                    print(line.decode('utf8'))
                    line.decode('utf8')
                except:
                    print(str(line))

def show_ifend_cnt(f_name):
#    print(f_name)
    if_cnt = 0
    ifdef_cnt = 0
    ifndef_cnt = 0
    ifzero_cnt = 0
    ifone_cnt = 0
    ifother_cnt = 0
    elif_cnt = 0
    else_cnt = 0
    endif_cnt = 0
    with open(f_name, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        for line in lines:
            line = deal_startTabLine(line)
            if line.lstrip(' ').startswith('#if'):
                if_cnt += 1
                if line.lstrip(' ').startswith('#ifdef '):
                    ifdef_cnt += 1
                elif line.lstrip(' ').startswith('#ifndef '):
                    ifndef_cnt += 1
                elif line.lstrip(' ').startswith('#if 0'):
                    ifzero_cnt += 1
                elif line.lstrip(' ').startswith('#if 1'):
                    ifone_cnt += 1
                else:
                    ifother_cnt += 1
            if line.lstrip(' ').startswith('#endif'):
                if '*/' in line:
                    if '/*' in line:
                        endif_cnt += 1
                    else:
                        pass
                else:
                    endif_cnt += 1
            if line.lstrip(' ').startswith('#else'):
                else_cnt += 1
            if line.lstrip(' ').startswith('#elif '):
                elif_cnt += 1
        print("length of file lines: " + str(len(lines)))
        print("[total of #if :] " + str(if_cnt))
        print(" #ifdef : " + str(ifdef_cnt))
        print(" #ifndef: " + str(ifndef_cnt))
        print(" #if 0  : " + str(ifzero_cnt))
        print(" #if 1  : " + str(ifone_cnt))
        print(" others : " + str(ifother_cnt))
        print("[#elif        :] " + str(elif_cnt))
        print("[#else        :] " + str(else_cnt))
        print("[#endif       :] " + str(endif_cnt))

def demo_test(f_name, cmd, macro):
#    print(f_name)
    data = ''
    with open(f_name, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        data = ifend_deal(lines, cmd, macro)
#    print(data)
    with open(f_name, 'w', encoding='utf-8', errors='ignore') as f:
        f.writelines(data)

def main():
    parser = argparse.ArgumentParser()
    parser.description='please enter there parameters p, c and m ...'
    parser.add_argument("-p", help="this is parameter - path", dest="path",  type=str, default="./testCodes/")
    parser.add_argument("-c", help="this is parameter - command(remove or keep)", dest="cmd",  type=str, default="remove")
    parser.add_argument("-m", help="this is parameter - macro name", dest="macro",  type=str, default="AAAA")
    args = parser.parse_args()
#    print(args)
#    print("parameter p is :", args.path)
#    print("parameter c is :", args.cmd)
#    print("parameter m is :", args.macro)
    path = args.path
    command = args.cmd
    macro_name = args.macro
    for root, dirs, files in os.walk(path):
        for f_name in files:
            if f_name.endswith('.c') or f_name.endswith('.h'):
                abs_f_name = os.path.join(root, f_name)
                print(abs_f_name)
                input_coding_verify(abs_f_name)
                show_ifend_cnt(abs_f_name)
#                deal_file(abs_f_name)
                demo_test(abs_f_name, command, macro_name)

if __name__ == '__main__':
    main()
