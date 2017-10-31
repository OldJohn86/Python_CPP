#!/usr/local/bin/python
# -*- coding: utf-8 -*-
import requests
import urllib
import re
import random
from time import sleep
def main():
	url='https://www.zhihu.com/?next=%2Fquestion%2F22591304%2Ffollowers'
	#感觉这个话题人很多
	headers={
	    'User-Agent':'',
	    'Host':'www.zhihu.com',
	    'Accept-Encoding':'gzip,deflate,br',
	    'Accept-Language':'zh-CN,en-US;q=0.8,en;q=0.6',
	    'Origin':'https://www.zhihu.com/',
	    'Connection':'keep-alive',
	    'Referer':'https://www.zhihu.com/?next=%2Fquestion%2F22591304%2Ffollowers',
	    'Content-Type':'application/x-www-form-urlencoded; charset=UTF-8',
	    'X-Requested-With':'XMLHttpRequest',
	    'X-Xsrftoken':'',
	    'Cookie':''
	}
	i=1
	print ('1111111')
	for x in xrange(20,3600,20):
		print('2222222')
		data={'start':'0', 'offset':str(x), '_xsrf':'a128464ef225a69348cef94c38f4e428'}
		#知乎用offset控制加载的个数，每次相应加载20
		content=requests.post(url,headers=headers,data=data,timeout=10).text
		#用post提交from data
		imgs=re.findall('<img src=\\\\\"(.*?)_m.jpg',content)
		#在爬下来的json上用正则提取图片地址，去掉_m为大图
		for img in imgs:
			print ('3333333')
			try:
				img=img.replace('\\','')
				#去掉\字符这个干扰成分
				pic=img+'.jpg'
				path='/Users/pchen/github/Python_CPP/spiderweb/zhihu/jpg/'+str(i)+'.jpg'
				#声明存储地址及图片名称
				urllib.urlretrieve(pic,path)
				#下载图片
				print u'下载了第'+str(i)+u'张图片'
				i+=1
				sleep(random.uniform(0.5,1))
				#睡眠函数用于防止爬取过快被封IP
			except:
				print u'抓漏1张'
				pass
		sleep(random.uniform(0.5,1))

if __name__=='__main__':
	main()