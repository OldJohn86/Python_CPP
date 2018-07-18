#!/usr/bin/env python3

# -*- coding: utf-8 -*-
import os
from datetime import date

sanity_test_path = 'cd /home/pchen/daily_image_sanity_test;'
server_url       = 'http://192.168.65.235:18683/'
image_type       = '-eng-major-image'
gpt              = '-eng-gpt-nandinfo.img'
ubootenv         = 'uboot-env.bin'
major            = 'major-image-'

# day = date.today()
y_m_d = date.today().strftime('%Y-%m-%d')
y_m = date.today().strftime('%Y-%m')
print(y_m_d)
print(y_m)

def do_wget(target):
    # Removed old images first.
    cmd_rm_image = sanity_test_path +' rm *'+target+'*'
    print(cmd_rm_image)
    os.system(cmd_rm_image)

    if target == 'g3':
        cmd_wget_g3gpt = sanity_test_path+' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target+image_type+'/'+major+target+gpt
        print(cmd_wget_g3gpt)
        os.system(cmd_wget_g3gpt)

        cmd_wget_ubootenv = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target+image_type+'/'+ubootenv
        print(cmd_wget_ubootenv)
        os.system(cmd_wget_ubootenv)

        cmd_rename_g3ubootenv = sanity_test_path + ' mv '+ubootenv+' '+target+'-'+ubootenv
        print(cmd_rename_g3ubootenv)
        os.system(cmd_rename_g3ubootenv)

        cmd_wget_g3image = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target+image_type+'/'+major+target+'-eng.mubi'
        print(cmd_wget_g3image)
        os.system(cmd_wget_g3image)

    if target == 'saturn-sfu':
        pass

if __name__ == '__main__':
    do_wget('g3')
    do_wget('saturn-sfu')

