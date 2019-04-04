# coding=utf-8
from bs4 import BeautifulSoup
import requests
import time
import random
from lxml import etree

def getpage(h1,pro,i):
	url='http://app.finance.ifeng.com/list/stock.php?t=ha&f=chg_pct&o=desc&p='+ str(i)
	try:
		html=requests.get(url,timeout=16,headers=h1,proxies=pro)
		html.encoding = "utf-8"
		html = html.text
		print ('抓到网页')
		parse(html)
	except:
		print('again')
		getpage(h1,pro,i)

def parse(html):
	try:
		soup = BeautifulSoup(html,'lxml')
		con = soup.find_all('tr')
		for item in con:
			print (item.get_text())
			L1.append(item.get_text())
	except:
		print("重新解析")
		parse(html)

print('程序开始执行,designed by luke')
h1 = {
	'User-Agent':'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.102 Safari/537.36'
}
o_g = ['101.67.63.200:80','219.145.43.211:80','27.184.57.108:8118','27.184.57.108:8118']


L1 = []

a = random.randint(0,3)
pro = {'http': o_g[a]}
i = 1

while i < 15:
	getpage(h1,pro,i)
	i = i + 1
	print('下一页')
	print('随机休眠N秒')
	time.sleep(random.randint(0,10))
    
print('totally end')
print (L1)
