# -*- coding:utf-8 -*- 
'''
Created on 2019年1月24日
@author: Jimmy

@modify on 2019/04/16 BY CPP
'''
import time
import datetime
import tushare as ts
import pandas as pd
import numpy as np
import pymysql
pymysql.install_as_MySQLdb()
from sqlalchemy import create_engine 

#symbols = ['600519', '000651']
symbols = ['600519']
#symbols = ['600000', '600008', '600848', '600023', '300199', '300001', '002252']
mysql_engine = create_engine('mysql://root:1q2w3e4R@localhost:3306/test?charset=utf8&use_unicode=1')

"""
获取实时行情
"""
def get_realtime_price():
    for _ in range(3):
        try:
            df = ts.get_realtime_quotes(symbols)
            df = df[['code', 'price', 'volume', 'amount']]
            df = df.set_index('code')
            for col in df.columns:
                df[col] = df[col].astype(float)
        except:
            time.sleep(3)
        else:
            #print(df)
            return df

    
def run():
    today  = datetime.datetime.today().date()
    today = str(today).replace('-', '')
    last = None
    while True:
        hour = datetime.datetime.today().hour
        minute = datetime.datetime.today().minute
        second = datetime.datetime.today().second
        if (hour < 9 and hour >= 0) or (hour == 9 and minute < 30):
            print('%s 当天（上午）未开盘...%s:%s:%s' % (today, str(hour).zfill(2), str(minute).zfill(2), str(second).zfill(2)))
            #break
        elif (hour == 11 and minute > 30):
            print('%s 当天（上午）已收市...%s:%s:%s' % (today, str(hour).zfill(2), str(minute).zfill(2), str(second).zfill(2)))
            #break
        elif (hour >= 12 and hour < 13):
            print('%s 当天（下午）未开盘...%s:%s:%s' % (today, str(hour).zfill(2), str(minute).zfill(2), str(second).zfill(2)))
            #break
        elif (hour >= 13 and hour <= 23):
            print('%s 当天（下午）已收市...%s:%s:%s' % (today, str(hour).zfill(2), str(minute).zfill(2), str(second).zfill(2)))
            #break
        else:
            this = get_realtime_price()
            #print(this)
            if last is None:
                last = this
            else:
                try:
                    df = this - last
                except Exception as e:
                    if this in None:
                        print('this is None')
                    if last is None:
                        print('last is None')
                    print('%s' % e)
                    df = pd.DataFrame()
                #print(df)
                if df is not None:
                    last = this
                    #print(this)
                    df = df[(df.volume > 0) & (df.amount > 0) ]
                    df.columns = ['change', 'volume', 'amount']
                    this = this[['price']]
                    df = df.merge(this, left_index=True, right_index=True, how='left')
                    df['volume'] = df['volume'] / 100
                    df['volume'] = df['volume'].astype(int)
                    #print(df.shape[0])
                    if df.shape[0] > 0:
                        df['date'] = today
                        df['time'] = '%s:%s:%s'%(str(hour).zfill(2), str(minute).zfill(2), str(second).zfill(2))
                        df = df.reset_index()
                        df['type'] = df['change'].map(lambda x: np.where(x>=0, '买入', '卖出'))
                        df.to_sql('STOCK_TICK_cpp', mysql_engine, index=False, if_exists='append')
                        print(df)
        time.sleep(3)
        

def tick_test():
    df = ts.get_tick_data('300199', date='2019-01-22', src='tt')
    df['code'] = '300199'
    df['date'] = '20190122'
    df.to_sql('STOCK_TICK', mysql_engine, index=False, if_exists='append')
    print(df)

if __name__ == '__main__':
    run()
#    tick_test()    
 
