# -*- coding: utf-8 -*-
import requests
from bs4 import BeautifulSoup
import hashlib
import re
import base64


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
#print(url)
#h = list(range(256))
#b = [ord(c[g % len(c)]) for g in range(256)]

#f = 0
#for g in range(0, 256):
#f = (f + h[g] + b[g]) % 256
#tmp = h[g]
#h[g] = h[f]
#h[f] = tmp

    t = ""
#p, f = 0, 0
#for g in range(0, len(k)):
#p = (p + 1) % 256
#f = (f + h[p]) % 256
#tmp = h[p]
#h[p] = h[f]
#h[f] = tmp
#t += chr(k[g] ^ (h[(h[p] + h[f]) % 256]))
#t = t[26:]
    t = url
    return t

m = 'ece8ozWUT/VGGxW1hlbITPgE0XMZ9Y/yWpCi5Rz5F/h2uSWgxwV6IQl6DAeuFiT9mH2ep3CETLlpwyD+kU0YHpsHPLnY6LMHyIQo6sTu9/UdY5k+Vjt3EQ'
r = 'HpRB2OSft5RhlSyZaXV8xYpvEAgDThcA'
m1 = 'Ly93eDMuc2luYWltZy5jbi9tdzYwMC85ZGFkMWIwY2x5MWZzNmV5NGRicjRqMjFhbzByOGRxeS5qcGc='
r1 = 'NQDkkbfuL1erELUdjzjtMhi8WEPQkoV7'
#print(get_imgurl(m,r))
#print(get_imgurl(m1,r1))

def get_r(js_url):
    '''获取关键字符串'''
    js = requests.get(js_url).text
    _r = re.findall('c=[\w\d]+\(e,"(.*?)"\)', js)[0]
    return _r


def get_urls(url):
    '''获取一个页面的所有图片的链接'''
    headers = {
        'user-agent': 'Mozilla/5.0 (Windows NT 5.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2634.33 Safari/537.36',
        'Host': 'jandan.net'
    }
    html = requests.get(url, headers=headers).text
    js_url = 'http:' + re.findall('<script src="(//cdn.jandan.net/static/min/[\w\d]+\.\d+\.js)"></script>', html)[-1]
    _r = get_r(js_url)
    #print(_r)
    soup = BeautifulSoup(html, 'lxml')
    #print(soup)
    tags = soup.select('.img-hash')
    #print(tags)
    for tag in tags:
        #print(tag)
        img_hash = tag.text
        #print(img_hash)
        img_url = get_imgurl(img_hash,_r)
        print(img_url)


if __name__ == '__main__':
    get_urls('http://jandan.net/ooxx/page-44')
