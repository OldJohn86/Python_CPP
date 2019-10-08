#!/usr/bin/python
# -*- coding: UTF-8 -*-
'''
'''

import os
from os import walk

bhasCCommentBegin = False

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
 
def filter_c_file(path):
    for root, dirs, files in os.walk(path):
        for f_name in files:
            if f_name.endswith('.c') or f_name.endswith('.h'):
                global bhasCCommentBegin
                print(os.path.join(root, f_name))
                with open(os.path.join(root, f_name), 'r', encoding='utf-8', errors='ignore') as f:
                    lines = f.readlines()
                length = len(lines)
                print(length)
                i = 0
                ret = 0
                while i < length:
                    while True:
                        if not bhasCCommentBegin:
                            index = lines[i].find("/*")
                            if index != -1:
                                bhasCCommentBegin = True
                                index2 = lines[i].find("*/", index + 2)
                                if index2 != -1:
                                    lines[i] = lines[i][0:index] + lines[i][index2 + 2:]
                                    bhasCCommentBegin = False  # continue look for comment
                                else:
                                    lines[i] = lines[i][0:index]  # only find "begin comment
                                    lines[i] += "\r\n"
                                    ret = -2
                                    break
                            else:
                                ret = 0  # not find
                                break
                        else:
                            index2 = lines[i].find("*/")
                            if index2 != -1:
                                bhasCCommentBegin = False
                                lines[i] = lines[i][index2 + 2:]  # continue look for comment
                            else:
                                ret = -1  # should delete this
                                break
                    if ret == -1:
                        if bhasCCommentBegin == False:
                            print("There must be some wrong")
                        del lines[i]
                        i -= 1
                        length -= 1
                    elif ret == 0:
                        index = lines[i].find("//")
                        if index != -1:
                            lines[i] = lines[i][0:index]
                            lines[i] += "\r\n"
                        index = lines[i].find("#include")
                        if index != -1:
                            lines[i] = lines[i][0:index]
                            lines[i] += "\r\n"
                    else:
                        pass
                    i += 1
                result = []
                for line in lines:
                    # line = re.sub (u'\t|\n| ','', line)
                    line = line.replace('\n', '').replace('\t', '').replace(' ', '').replace('\r', '')
                    # print(line,end='')
                    result.append(line)
                #f = open (path+filename.replace('.c', '.bak.c').replace ('.h', '.bak.h'), 'w')
                #for i in result:  # 写文件
                #    f.write (i)
                #f.close ()


if __name__ == '__main__':
    path='./testCodes/'
    filter_c_file(path)
