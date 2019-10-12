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
def get_ifdef_allIndex(lines):
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

def get_ifdef_1stIndex(lines, ifdef_all, endif_all):
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

def parse_outer_ifdef(lines, macro):
    this_array = []
    next_array = []
    next_ab = ''
    (ifdef_all, endif_all) = get_ifdef_allIndex(lines)
    (ifdef_1st, endif_1st) = get_ifdef_1stIndex(lines, ifdef_all, endif_all)
    outer_macro = [list('') for i in range(len(ifdef_1st))]
    for i in range(len(ifdef_1st)):
        for index in range(ifdef_1st[i], endif_1st[i]+1):
            outer_macro[i].append(lines[index])
        parse_outer = parse_else_from_ifdef(outer_macro[i])
#        print(parse_outer)
        if macro == (lines[ifdef_1st[i]].strip(' ').strip('\n').split(' '))[1]:# matched
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
def ifdef_find_iter(lines, macro):
    ifdef_cnt = 0
    endif_cnt = 0
    global glb_macro_array
    for line in lines:
        if "#ifdef " in line:
            ifdef_cnt += 1
        if "#endif" in line:
            endif_cnt += 1
    print(ifdef_cnt, endif_cnt)
    if ifdef_cnt == endif_cnt == 0:
        return glb_macro_array
    (macro_array, data_array) = parse_outer_ifdef(lines, macro)
    glb_macro_array.extend(macro_array)
    return ifdef_find_iter(data_array, macro)

def ifdef_deal(lines, macro, opt):
#    print(lines)
    data = ''
    for line in lines:
        data += line
    macro_array = ifdef_find_iter(lines, macro)
#    print(macro_array)
#    print(len(macro_array))
    old_data = ['' for i in range(len(macro_array))]
    new_data = ['' for i in range(len(macro_array))]
    for i in range(len(macro_array)):
#        print(len(macro_array[i]))
        for j in range(len(macro_array[i])):
            old_data[i] += macro_array[i][j]# A+B
        if opt == "remove":
            new_data[i] += macro_array[i][3]# B: index is 3 or -2
        elif opt == "keep":
            new_data[i] += macro_array[i][1]# A: index is 1
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
def parse_else_from_ifdef(lines):
    parse_str = ['' for i in range(5)]
    data_a = ''
    data_else = ''
    data_b = ''
    else_index= []
    for i in range(len(lines)):
        if "else" in lines[i]:
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
                if "#ifdef " in lines[i]:
                    if_cnt += 1
                if "#endif" in lines[i]:
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
    ifdef_cnt = 0
    ifndef_cnt = 0
    endif_cnt = 0
    else_cnt = 0
    elif_cnt = 0
    with open(f_name, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        for line in lines:
            if "#ifdef " in line:
                ifdef_cnt += 1
            if "#ifndef " in line:
                ifndef_cnt += 1
            if "#endif" in line:
                endif_cnt += 1
            if "else" in line:
                else_cnt += 1
            if "elif " in line:
                elif_cnt += 1
        print("#ifdef : " + str(ifdef_cnt))
        print("#ifndef: " + str(ifndef_cnt))
        print("#endif : " + str(endif_cnt))
        print("#else  : " + str(else_cnt))
        print("#elif  : " + str(elif_cnt))
#        data = ifdef_deal(lines, 'AAAA', 'remove')
        data = ifdef_deal(lines, 'AAAA', 'keep')
    with open(f_name.replace('input','output'), 'w', encoding='utf-8', errors='ignore') as f:
        f.writelines(data)

if __name__ == '__main__':
    path='./testCodes/'
    
    demo_test("./testCodes/ifend_input.c")
