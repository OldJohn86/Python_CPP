#!/usr/bin/env python3

#-*- coding:utf-8 -*-
import requests
from bs4 import BeautifulSoup
import traceback
import re
import xlwt

#获取html页面text信息
def getHTMLText(url, code="utf-8"):
    try:
        r = requests.get(url)
        r.raise_for_status()
        r.encoding = code
        return r.text
    except:
        traceback.print_exc()
        return ""

#获取股票列表
def getStockList(lst, stockURL):
    html = getHTMLText(stockURL,"GB2312")
    soup = BeautifulSoup(html, 'html.parser') 
    a = soup.find_all('a')
    for i in a:
        try:
            href = i.attrs['href']
            lst.append(re.findall(r"[s][hz]\d{6}", href)[0])
        except:
            continue

#根据股票的代号查找股票的交易信息，并将结果存储到相关文件
def getStockInfo(lst, stockURL):
    #创建EXCEL文件
    book=xlwt.Workbook(encoding='utf-8')
    sheet1=book.add_sheet('sheet1', cell_overwrite_ok=True)
    heads=['股票名称','最高','最低','今开','昨收','成交额','成交量','净值','折价率']
    num=0
    for head in heads:
        sheet1.write(0,num,head)
        num=num+1
    book.save('gupiao.xls')
    count = 1
    length=len(lst)
    #每次将一个查询的数据输出到EXCEl表中
    for stock in lst:
        url = stockURL + stock + ".html"
        html = getHTMLText(url)
        try:
            if html=="":
                continue
            infoDict = {}
            soup = BeautifulSoup(html, 'html.parser')
            stockInfo = soup.find('div',attrs={'class':'stock-bets'})
            #查找股票名称
            if stockInfo:
                name = stockInfo.find_all(attrs={'class':'bets-name'})[0]
                infoDict.update({'股票名称': name.text.split()[0]})
                #寻找所有键和值（最高、最低）
                keyList = stockInfo.find_all('dt')
                valueList = stockInfo.find_all('dd')
                for i in range(len(keyList)):
                    key = keyList[i].text
                    val = valueList[i].text
                    infoDict[key] = val
                j=0
                for i in heads:
                    if i in infoDict:
                        sheet1.write(count,j,infoDict[i])
                        j=j+1
                    else:
                        pass
                book.save('gupiao.xls')
                print("\r当前进度: {:.2f}%".format(count*100/length),end="")
                count=count+1
        except:
            print("\r当前进度: {:.2f}%".format(count * 100 / length), end="")
            count = count + 1
            continue

if __name__=='__main__':
    #找寻将数据静态写在html页面的网页
    #stock_list_url = 'http://quote.eastmoney.com/stocklist.html'
    stock_list_url = 'http://quote.eastmoney.com/center/gridlist.html#hs_a_board'
    stock_info_url = 'https://gupiao.baidu.com/stock/'
    slist=[]
    getStockList(slist, stock_list_url)
    print (slist)
    #getStockInfo(slist, stock_info_url)
