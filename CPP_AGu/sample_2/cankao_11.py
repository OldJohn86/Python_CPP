# -*- coding: utf-8 -*-
'''
'''

import pandas as pd
import requests
import random
from bs4 import BeautifulSoup
#import pymysql
# 导入MySQL驱动
import mysql.connector
from sqlalchemy import create_engine



def get_one_page(i):
    # 使用ip代理,随机代理ip
    # 设置headers 这些都是为了避免访问过于频繁被禁止
    try:
        IPs = [{'HTTP': 'HTTP://118.190.95.35:9001'},
               {'HTTP': 'HTTP://61.135.217.7:80'},
               {'HTTP': 'HTTP://116.1.11.19:80'}]
        reportTime = '2017-12-31'
        headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36',
            'Referer': 'http://s.askci.com/stock/a/?reportTime=%s' % reportTime + '&pageNum=178'
        }
        url = 'http://s.askci.com/stock/a/?reportTime=%s' % reportTime + '&pageNum=%d' % i
        response = requests.get(url, headers=headers, proxies=random.choice(IPs))
        if response.status_code == 200:
            return response.text
        return None
    except requests.RequestException:
        print("爬取出错")


def parse_one_page(html):
    soup = BeautifulSoup(html, 'lxml')
    # 选择id为myTable04的控件，代码中有解释
    content = soup.select("#myTable04")[0]

    # 第一行设置为column,并且选第一个(因为是一个list), 其实也只有一个
    tbl = pd.read_html(content.prettify(), header=0)[0]
    # 强制修改列名，中文列名不好写入数据库
    tbl.columns = ['serial_number', 'stock_code', 'stock_abbre', 'company_name', 'province', 'city',
                   'main_business_income',
                   'net_profit', 'employee', 'listing_date', 'zhaogushu', 'financial_report',
                   'industry_classification', 'industry_type', 'main_business']
    return tbl


def generate_mysql(db, user, password, port):
    # 创立连接
    # db就是需要数据库中的schema 选一个schema就好了
    conn = mysql.connector.connect(
        host='localhost',
        user=user,
        password=password,
        port=port,
        charset='utf8',
        db=db
    )
    cursor = conn.cursor()
    # 创建数据库
    sql = 'CREATE DATABASE if not exists %s' % db
    # 执行命令
    cursor.execute(sql)

    # 删除旧表
    sql = 'DROP TABLE if exists listed_company'
    # 执行命令
    cursor.execute(sql)

    # 创建新表
    sql = 'CREATE TABLE if not exists listed_company (serial_number INT(20) NOT NULL, stock_code VARCHAR(20), stock_abbre VARCHAR(20), company_name VARCHAR (20),province VARCHAR (20), city VARCHAR (20), main_business_income VARCHAR(20), net_profit VARCHAR (20), employee INT (20), listing_date DATE, zhaogushu VARCHAR(20), financial_report VARCHAR (20), industry_classification VARCHAR(20), industry_type VARCHAR(100), main_business VARCHAR(200), PRIMARY KEY (serial_number))'
    # 执行命令
    cursor.execute(sql)

    # 断开链接
    conn.close()


def write_to_sql(tbl, db, user, password, port):
    engine = create_engine('mysql+mysqlconnector://%s:%s@localhost:%d/%s?charset=utf8' % (user, password, port, db))
    tbl.to_sql('listed_company', con=engine, if_exists='append', index=False)


def main(page):
    # 设置预想database
    db = 'test'
    user = 'root'
    password = '1q2w3e4R'
    port = 3306
    # 生成数据库
    generate_mysql(db, user, password, port)
    # 选page这么多页
    for i in range(1, page + 1):
        # 读取网页html
        html = get_one_page(i)
        # 分析得到dataframe
        tbl = parse_one_page(html)
        # 将dataframe写到sql中
        write_to_sql(tbl, db, user, password, port)


if __name__ == '__main__':
    reportTime = '2017-12-31'
    url = 'http://s.askci.com/stock/a/?reportTime=%s' % reportTime + '&pageNum=%d'
    # 最大设置为178,输入页数就好了
    main(2)
