#!/usr/bin/env python3

# -*- coding: utf-8 -*-
import os
from datetime import date

sanity_test_path = 'cd /home/pchen/daily_image_sanity_test;'
server_url       = 'http://192.168.65.235:18683/'
ubootenv         = 'uboot-env.bin'

# day = date.today()
y_m_d = date.today().strftime('%Y-%m-%d')
y_m = date.today().strftime('%Y-%m')
print(y_m_d)
print(y_m)

def do_wget(target, child=''):
    cmd_rm_img = sanity_test_path +' rm *'+target+'*;'+' rm *.dtb'
    print(cmd_rm_img)
    os.system(cmd_rm_img)

    if target == 'g3':
        target_img_type = 'g3-eng-major-image'
        target_gpt = 'major-image-g3-eng-gpt-nandinfo.img'
        target_img = 'major-image-g3-eng.mubi'
    elif target == 'saturn-sfu':
        if child == 'epon':
            target_img_type = 'saturn-sfu-eng_epon-major-image'
        elif child == 'gpon':
            target_img_type = 'saturn-sfu-eng_gpon-major-image'
        else:
            pass

        target_gpt = 'major-image-saturn-sfu-eng-gpt.img'
        target_img = 'uImage'
        dtb = 'uImage-saturn_sfu.dtb'
        rootfs = 'major-image-saturn-sfu-eng.rootfs.squashfs.ubi'
        ubi = 'major-image-saturn-sfu-eng_user.ubi'
    else:
        pass
    
    cmd_wget_gpt = sanity_test_path+' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+target_gpt
    print(cmd_wget_gpt)
    os.system(cmd_wget_gpt)

    cmd_wget_ubootenv = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+ubootenv
    print(cmd_wget_ubootenv)
    os.system(cmd_wget_ubootenv)
    
    cmd_rename_ubootenv = sanity_test_path + ' mv '+ubootenv+' '+target+'-'+ubootenv
    print(cmd_rename_ubootenv)
    os.system(cmd_rename_ubootenv)
    
    cmd_wget_img = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+target_img
    print(cmd_wget_img)
    os.system(cmd_wget_img)

    if target == 'saturn-sfu':
        cmd_wget_img = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+dtb
        print(cmd_wget_img)
        os.system(cmd_wget_img)

        cmd_wget_img = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+rootfs
        print(cmd_wget_img)
        os.system(cmd_wget_img)

        cmd_wget_img = sanity_test_path +' wget '+server_url+target+'/'+y_m+'/'+y_m_d+'/'+target_img_type+'/'+ubi
        print(cmd_wget_img)
        os.system(cmd_wget_img)
    else:
        pass

if __name__ == '__main__':
    do_wget('g3')
    do_wget('saturn-sfu', 'epon')

