#!/usr/bin/python3
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
    if_allIndex = []
    endif_allIndex = []
    for i in range(len(lines)):
        if lines[i].strip(' ').startswith('#if'):
            if_allIndex.append(i)
#            print("["+str(i)+":]" + lines[i])
        if lines[i].strip(' ').startswith('#endif'):
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
#            print(i, endif_allIndex[i])
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
#        print(parse_outer)
        ifLine_list = lines[if_outer[i]].lstrip(' ').strip('\n').split(' ')
#        print(ifLine_list)
        if macro == ifLine_list[1]:# matched
            this_array.append(parse_outer)
        else:# unmatched
            next_ab += parse_outer[1]
            next_ab += parse_outer[3]
    next_lines = next_ab.split('\n')
#    print(len(next_lines), next_lines)
    for i in range(len(next_lines)):
        next_array.append(next_lines[i] + '\n')
#    print(this_array)
#    print(next_array)
    return (this_array, next_array)

glb_macro_array = []
def iter_parse_ifend(lines, macro):
    ifdef_cnt = 0
    endif_cnt = 0
    global glb_macro_array
    for line in lines:
        if line.lstrip(' ').startswith('#if'):
            ifdef_cnt += 1
        if line.lstrip(' ').startswith('#endif'):
            endif_cnt += 1
    print(ifdef_cnt, endif_cnt)
    if ifdef_cnt == endif_cnt == 0:
        return glb_macro_array
    (macro_array, data_array) = parse_outer_ifend(lines, macro)
    glb_macro_array.extend(macro_array)
    return iter_parse_ifend(data_array, macro)

def ifdef_deal(lines, macro, opt):
#    print(lines)
    data = ''
    for line in lines:
        data += line
    macro_array = iter_parse_ifend(lines, macro)
    print(macro_array)
    print(len(macro_array))
    old_data = ['' for i in range(len(macro_array))]
    new_data = ['' for i in range(len(macro_array))]
    for i in range(len(macro_array)):
#        print(len(macro_array[i]))
        for j in range(len(macro_array[i])):
            old_data[i] += macro_array[i][j]# A+B
        if opt == "remove":
            if macro_array[i][0].lstrip(' ').startswith("#ifdef "):
#                print(macro_array[i][0])
                new_data[i] += macro_array[i][3]# B: index is 3 or -2
            elif macro_array[i][0].lstrip(' ').startswith("#ifndef "):
                print(macro_array[i][0])
                new_data[i] += macro_array[i][1]# A: index is 1
        elif opt == "keep":
            if macro_array[i][0].lstrip(' ').startswith("#ifdef "):
#                print(macro_array[i][0])
                new_data[i] += macro_array[i][1]# A: index is 1
            elif macro_array[i][0].lstrip(' ').startswith("#ifndef "):
                print(macro_array[i][0])
                new_data[i] += macro_array[i][3]# B: index is 3 or -2
        else:
            print("which input opt is neither remove nor keep!!!")
        if data.find(old_data[i]) != -1:
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
def parse_else_from_ifend(lines):
    parse_str = ['' for i in range(5)]
    data_a = ''
    data_else = ''
    data_b = ''
    else_index= []
    for i in range(len(lines)):
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
                if lines[i].lstrip(' ').startswith('#ifdef '):
                    if_cnt += 1
                if lines[i].lstrip(' ').startswith('#endif'):
                    end_cnt += 1
            if if_cnt == end_cnt:
                for i in range(1, else_index[index]):
                    data_a += lines[i]
                for i in range(else_index[index]+1, len(lines)-1):
                    data_b += lines[i]
                data_else += lines[else_index[index]]
                break
            else:
                 pass
    parse_str[0] += lines[0]
    parse_str[1] += data_a
    parse_str[2] += data_else
    parse_str[3] += data_b
    parse_str[4] += lines[-1]
    return parse_str

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

def demo_test(f_name):
    print(f_name)
    data = ''
    if_cnt = 0
    ifdef_cnt = 0
    ifndef_cnt = 0
    ifzero_cnt = 0
    ifone_cnt = 0
    elif_cnt = 0
    else_cnt = 0
    endif_cnt = 0
    with open(f_name, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        for line in lines:
            if line.lstrip(' ').startswith('#if'):
                if_cnt += 1
            if line.lstrip(' ').startswith('#ifdef '):
                ifdef_cnt += 1
            if line.lstrip(' ').startswith('#ifndef '):
                ifndef_cnt += 1
            if line.lstrip(' ').startswith('#if 0'):
                ifzero_cnt += 1
            if line.lstrip(' ').startswith('#if 1'):
                ifone_cnt += 1
            if line.lstrip(' ').startswith('#endif'):
                endif_cnt += 1
            if line.lstrip(' ').startswith('#else'):
                else_cnt += 1
            if line.lstrip(' ').startswith('#elif '):
                elif_cnt += 1
        print("[total of #if :] " + str(if_cnt))
        print(" #ifdef : " + str(ifdef_cnt))
        print(" #ifndef: " + str(ifndef_cnt))
        print(" #if 0  : " + str(ifzero_cnt))
        print(" #if 1  : " + str(ifone_cnt))
        print("[#elif        :] " + str(elif_cnt))
        print("[#else        :] " + str(else_cnt))
        print("[#endif       :] " + str(endif_cnt))
#        data = ifdef_deal(lines, 'AAAA', 'remove')
        data = ifdef_deal(lines, 'AAAA', 'keep')
    with open(f_name.replace('input','output'), 'w', encoding='utf-8', errors='ignore') as f:
        f.writelines(data)

if __name__ == '__main__':
    path='./testCodes/'
    
    demo_test("./testCodes/ifend_input.c")
