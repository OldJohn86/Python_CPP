# -*- coding: utf-8 -*-
"""
Created on Fri Apr 27 19:58:48 2018
@author: gmn
"""
 
#导入requests module  
import requests  
#导入random module  
import random  
#导入BeautifulSoup module   
from bs4 import BeautifulSoup  
#导入pandas module
import pandas as pd
  
#User-Agent列表，这个可以自己在网上搜到  
USER_AGENTS = [  
    "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; AcooBrowser; .NET CLR 1.1.4322; .NET CLR 2.0.50727)",  
    "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0; Acoo Browser; SLCC1; .NET CLR 2.0.50727; Media Center PC 5.0; .NET CLR 3.0.04506)",  
    "Mozilla/4.0 (compatible; MSIE 7.0; AOL 9.5; AOLBuild 4337.35; Windows NT 5.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)",  
    "Mozilla/5.0 (Windows; U; MSIE 9.0; Windows NT 9.0; en-US)",  
    "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Win64; x64; Trident/5.0; .NET CLR 3.5.30729; .NET CLR 3.0.30729; .NET CLR 2.0.50727; Media Center PC 6.0)",  
    "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0; WOW64; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; .NET CLR 1.0.3705; .NET CLR 1.1.4322)",  
    "Mozilla/4.0 (compatible; MSIE 7.0b; Windows NT 5.2; .NET CLR 1.1.4322; .NET CLR 2.0.50727; InfoPath.2; .NET CLR 3.0.04506.30)",  
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN) AppleWebKit/523.15 (KHTML, like Gecko, Safari/419.3) Arora/0.3 (Change: 287 c9dfb30)",  
    "Mozilla/5.0 (X11; U; Linux; en-US) AppleWebKit/527+ (KHTML, like Gecko, Safari/419.3) Arora/0.6",  
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.2pre) Gecko/20070215 K-Ninja/2.1.1",  
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9) Gecko/20080705 Firefox/3.0 Kapiko/3.0",  
    "Mozilla/5.0 (X11; Linux i686; U;) Gecko/20070322 Kazehakase/0.4.5",  
    "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.0.8) Gecko Fedora/1.9.0.8-1.fc10 Kazehakase/0.5.6",  
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/535.11 (KHTML, like Gecko) Chrome/17.0.963.56 Safari/535.11",  
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_7_3) AppleWebKit/535.20 (KHTML, like Gecko) Chrome/19.0.1036.7 Safari/535.20",  
    "Opera/9.80 (Macintosh; Intel Mac OS X 10.6.8; U; fr) Presto/2.9.168 Version/11.52",  
]  
  
#设置IP代理  
proxies={"https":"http://219.135.99.185:8088"}  

#设置要爬取的url  
url='http://quote.eastmoney.com/stocklist.html'  

#设置get请求的User-Agent，用于伪装浏览器UA  
headers={'User-Agent':random.choice(USER_AGENTS)}  

#requests get请求  
req=requests.get(url,headers=headers,proxies=proxies)

#页面结构化
#bs=BeautifulSoup(req.content,'html.parser')
bs=BeautifulSoup(req.text,'lxml')

quotesearch=bs.find('div',attrs={'id':'quotesearch'})
stock_list=quotesearch.find_all('ul')
market_List=quotesearch.find_all('div',attrs={'class':'sltit'})
#设置各个字段的名称
col_name=['MARKET_ID','STOCK_NAME','STOCK_CODE']
#变量初始化
list_row=[]
df_all=pd.DataFrame()

#设置保存的路径
dirPath='./homeWork02'
dirPath=dirPath+''
#设置文件保存的文件名
fileName='./assignment01.csv'
     
for stock, market in zip(stock_list,market_List):
    #获取A股类型：上股或者深股
    market_id=market.get_text().encode('latin').decode('gb18030')  #编码转换
    #print(market_id)
    #获取所有的证券信息
    stock_info=stock.find_all('li')
    for jj in range(len(stock_info)):
        #获取其中一条证券信息
        detail=stock_info[jj].get_text().encode('latin').decode('gb18030') #编码转换
        #提取证券名称和代码
        arr=detail.split('(')
        stock_name=arr[0] #获取证券名称
        stock_code=arr[1].replace(')','') #获取证券代码
        list_row=[]  #在此一定要重新把list_row列表清空，不然list_row在后面就会超过3列了
        #组合一条字段信息
        list_row.append(market_id)
        list_row.append(stock_name)
        list_row.append(stock_code)

        df_row=pd.DataFrame([list_row],columns=col_name) #字段名称和字段信息的组合
        df_all=df_all.append(df_row) #把该条证券信息到df_all变量中

df_all.to_csv(dirPath+fileName,encoding='gb18030',index=False) #把内容写进csv文件中

