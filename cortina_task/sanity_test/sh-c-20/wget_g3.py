#!/usr/bin/env python3

# -*- coding: utf-8 -*-
import os
from datetime import date

g3_image_url     = 'http://192.168.65.235:18683/g3/'
g3_image_type    = 'g3-eng-major-image'
g3_gpt           = 'major-image-g3-eng-gpt-nandinfo.img'
ubootenv         = 'uboot-env.bin'
g3_ubootenv      = 'g3-uboot-env.bin'
g3_image         = 'major-image-g3-eng.mubi'
sanity_test_path = 'cd /home/pchen/daily_image_sanity_test;'

# day = date.today()
y_m_d = date.today().strftime('%Y-%m-%d')
y_m = date.today().strftime('%Y-%m')
print(y_m_d)
print(y_m)

if __name__ == '__main__':
    # Removed old images first.
    cmd_rm_image = sanity_test_path +' rm *g3*'
    print(cmd_rm_image)
    os.system(cmd_rm_image)

    # Get the lastest image files from Server IP
    cmd_wget_g3gpt = sanity_test_path +' wget '+g3_image_url+'/'+y_m+'/'+y_m_d+'/'+g3_image_type+'/'+g3_gpt
    print(cmd_wget_g3gpt)
    os.system(cmd_wget_g3gpt)

    cmd_wget_ubootenv = sanity_test_path +' wget '+g3_image_url+'/'+y_m+'/'+y_m_d+'/'+g3_image_type+'/'+ubootenv
    print(cmd_wget_ubootenv)
    os.system(cmd_wget_ubootenv)

    cmd_rename_g3ubootenv = sanity_test_path + ' mv '+ubootenv+' '+g3_ubootenv
    print(cmd_rename_g3ubootenv)
    os.system(cmd_rename_g3ubootenv)

    cmd_wget_g3image = sanity_test_path +' wget '+g3_image_url+'/'+y_m+'/'+y_m_d+'/'+g3_image_type+'/'+g3_image
    print(cmd_wget_g3image)
    os.system(cmd_wget_g3image)



