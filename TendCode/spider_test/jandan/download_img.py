# -*- coding: utf-8 -*-
import hashlib
import base64
import requests
from bs4 import BeautifulSoup
import re
import threading
import multiprocessing
import os

def _md5(value):
    '''md5加密'''
    m = hashlib.md5()
    m.update(value.encode('utf-8'))
    return m.hexdigest()


def _base64_decode(data):
    '''bash64解码，要注意原字符串长度报错问题'''
    missing_padding = 4 - len(data) % 4
    if missing_padding:
        data += '=' * missing_padding
    return base64.b64decode(data)


def get_imgurl(m, r='', d=0):
    '''解密获取图片链接'''
    e = "DECODE"
    q = 4
    r = _md5(r)
    o = _md5(r[0:0 + 16])
    n = _md5(r[16:16 + 16])
    l = m[0:q]
    c = o + _md5(o + l)
    m = m[q:]
    k = _base64_decode(m)
    url = ''
    url = k.decode('utf-8', errors='ignore')
    url = '//w' + url
#    print(url)
#    h = list(range(256))
#    b = [ord(c[g % len(c)]) for g in range(256)]

#    f = 0
#    for g in range(0, 256):
#        f = (f + h[g] + b[g]) % 256
#        tmp = h[g]
#        h[g] = h[f]
#        h[f] = tmp

    t = ""
#    p, f = 0, 0
#    for g in range(0, len(k)):
#        p = (p + 1) % 256
#        f = (f + h[p]) % 256
#        tmp = h[p]
#        h[p] = h[f]
#        h[f] = tmp
#        t += chr(k[g] ^ (h[(h[p] + h[f]) % 256]))
#    t = t[26:]
    t = url
    return t


def get_r(js_url):
    '''获取关键字符串'''
    js = requests.get(js_url).text
    # 之前用的下面注释掉的这个，后来煎蛋改了函数名称，少个f_
    # _r = re.findall('c=f_[\w\d]+\(e,"(.*?)"\)', js)[0]
    _r = re.findall('c=[\w\d]+\(e,"(.*?)"\)', js)[0]
    return _r


def load_img(imgurl, file):
    '''下载单张图片到制定的文件夹下'''
    name = imgurl.split('/')[-1]
#    print(name)
#    print(file)
    file = "{}/{}".format(file,name)
#    print(file)
    item = requests.get(imgurl).content
    with open(file,'wb') as f:
        f.write(item)
#    print('{} is loaded'.format(name))



def load_imgs(url,file):
    '''多线程下载单页的所有图片'''
    threads = []
    headers = {
        'user-agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10.8; rv:49.0) Gecko/20100101 Firefox/49.0',
        'Host': 'jandan.net'
    }
    html = requests.get(url, headers=headers).text
    soup = BeautifulSoup(html, 'lxml')
    # 这个地方必须使用[-1]来提取js地址，因为有的页面有两个js地址，其中第一个是被注释了不用的
    js_url = re.findall('<script src="(//cdn.jandan.net/static/min/[\w\d]+\.\d+\.js)"></script>', html)[-1]
    _r = get_r('http:{}'.format(js_url))
    tags = soup.select('.img-hash')
    for each in tags:
        hash = each.text
        img_url = 'http:' + get_imgurl(hash, _r)
        t = threading.Thread(target=load_img,args=(img_url,file))
        threads.append(t)
    for i in threads:
        i.start()
    for i in threads:
        i.join()
    print(url,'is ok')

def get_dir():
    '''判断文件夹是否存在，如果不存在就创建一个'''
    filename = "pics"
    if not os.path.isdir(filename):
        os.makedirs(filename)
    return filename

def main(start,offset,file):
    '''多进程下载多页的图片,传入参数是开始页码数，结束页码，图片保存文件夹地址'''
    end = start + offset
    pool = multiprocessing.Pool(processes=4)
    base_url = 'http://jandan.net/ooxx/page-{}'
    for i in range(start,end+1):
        url = base_url.format(i)
        pool.apply_async(func=load_imgs,args=(url,file))
    pool.close()
    pool.join()


if __name__ == '__main__':
    import time
    t = time.time()
    get_dir()
    main(50689384,100,r'./pics')
#    time.sleep(60)
#    main(30,35,r'./download2')
#    time.sleep(60)
#    main(40,45,r'./download3')
#    time.sleep(60)
#    main(50,55,r'./download4')
#    time.sleep(60)
    print(time.time()-t)
