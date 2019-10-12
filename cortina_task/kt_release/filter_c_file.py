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

def parse_outer_ifend(lines, macro):
    data_a = ''
    data_array = []
    macro_array = []
#    macro_str = ['' for i in range(1000)]
    lines_array = [list('') for i in range(1000)]
#    print(len(macro_str))
    index = 0
    (ifdef_all, endif_all) = get_ifend_allIndex(lines)
    (ifdef_1st, endif_1st) = get_ifend_1stIndex(lines, ifdef_all, endif_all)
    for i in range(len(ifdef_1st)):
        if macro == (lines[ifdef_1st[i]].strip(' ').strip('\n').split(' '))[1]:
            for i in range(ifdef_1st[i], endif_1st[i]+1):
#                macro_str[index] += lines[i]
                lines_array[index].append(lines[i])
            parse_str = parse_else_from_ifend(lines_array[index])
            print("begin******************")
            print(parse_str)
            for i in range(len(parse_str)):
                data_a += parse_str[i]
            print(data_a)
            print("end******************")
#           macro_array.append(macro_str[index])
#            if index < len(macro_str):
#                index += 1
#            else:
#                print("ERROR: macro_str[] list index out of range!")
#                break
            macro_array.append(data_a)
            if index < len(lines_array):
                index += 1
            else:
                print("ERROR: lines_array[] index out of range!")
                break
        else:
            for i in range(ifdef_1st[i]+1, endif_1st[i]):
                data_array.append(lines[i])
    print(macro_array)
    return (macro_array, data_array)

glb_macro_array = []
def ifend_find_iter(lines, macro):
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
    (macro_array, data_a) = parse_outer_ifend(lines, macro)
#    print(macro_array)
    glb_macro_array.extend(macro_array)
    return ifend_find_iter(data_a, macro)

def ifend_remove(lines, macro):
    data = ''
    macro_array = ifend_find_iter(lines, macro)
#    print(macro_array)
    for line in lines:
        data += line
    for i in range(len(macro_array)):
        if data.find(macro_array[i]) != -1:
            data = data.replace(macro_array[i], '')
    return data

def ifend_keep(lines, macro):
    data = ''
    macro_array = ifend_find_iter(lines, macro)
#    print(macro_array)
    for line in lines:
        data += line
    for i in range(len(macro_array)):
        if data.find(macro_array[i]) != -1:
#            print(macro_array[i].replace("#ifdef "+ macro, '')[:-7])
            data = data.replace(macro_array[i], macro_array[i].replace("#ifdef "+macro, '')[:-7])
    return data

def ifend_test(f_name):
    print(f_name)
    data = ''
    with open(f_name, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        data = ifend_remove(lines, 'AAAA')
#        data = ifend_keep(lines, 'AAAA')
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
def parse_else_from_ifend(lines):
#    print(lines)
    parse_str = ['' for i in range(5)]
    data_a = ''
    data_else = ''
    data_b = ''
    else_index= []
#    print(len(lines), lines[0], lines[-1])
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
            print(else_index[index])
            if_cnt = 0
            end_cnt = 0
            for i in range(1, else_index[index]):
                if "#ifdef " in lines[i]:
                    if_cnt += 1
                if "#endif" in lines[i]:
                    end_cnt += 1
            if if_cnt == end_cnt:
                print(if_cnt, end_cnt)
                print(lines)
                for i in range(1, else_index[index]):
                    data_a += lines[i]
                for i in range(else_index[index]+1, len(lines)-1):
                    data_b += lines[i]
                data_else += lines[else_index[index]]
                break
            else:
                 pass
#        print("++++++data_a++++++")
#        print(data_a)
#        print("++++++data_b++++++")
#        print(data_b)
    parse_str[0] += lines[0]
    parse_str[1] += data_a
    parse_str[2] += data_else
    parse_str[3] += data_b
    parse_str[4] += lines[-1]
#    print(parse_str)
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

if __name__ == '__main__':
    path='./testCodes/'
    
    ifend_test("./testCodes/ifend_input.c")
