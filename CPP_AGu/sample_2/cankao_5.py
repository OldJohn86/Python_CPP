# coding=utf-8
import tushare as ts

d = ts.get_tick_data('600519', date='2019-04-04')
print (d)
print('*************************')

e = ts.get_hist_data('600519', start='2019-01-10',end='2019-04-04')
print (e)
