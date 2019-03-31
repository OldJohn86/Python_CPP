import psycopg2 #使用的是PostgreSQL数据库
import tushare as ts
from Stocks import*
from HData import*
import  datetime

stocks=Stocks("postgres","123456")
hdata=HData("postgres","123456")

# stocks.db_stocks_create()#如果还没有表则需要创建
#print(stocks.db_stocks_update())#根据todayall的情况更新stocks表

#hdata.db_hdata_date_create()

nowdate=datetime.datetime.now().date()

codestock_local=stocks.get_codestock_local()

hdata.db_connect()#由于每次连接数据库都要耗时0.0几秒，故获取历史数据时统一连接
for i in range(0,len(codestock_local)):
    nowcode=codestock_local[i][0]

    #print(hdata.get_all_hdata_of_stock(nowcode))

    print(i,nowcode,codestock_local[i][1])

    maxdate=hdata.db_get_maxdate_of_stock(nowcode)
    print(maxdate, nowdate)
    if(maxdate):
        if(maxdate>=nowdate):#maxdate小的时候说明还有最新的数据没放进去
            continue
        hist_data=ts.get_hist_data(nowcode, str(maxdate+datetime.timedelta(1)),str(nowdate), 'D', 3, 0.001)
        hdata.insert_perstock_hdatadate(nowcode, hist_data)
    else:#说明从未获取过这只股票的历史数据
        hist_data = ts.get_hist_data(nowcode, None, str(nowdate), 'D', 3, 0.001)
        hdata.insert_perstock_hdatadate(nowcode, hist_data)

hdata.db_disconnect()