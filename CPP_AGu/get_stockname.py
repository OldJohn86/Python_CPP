# coding=utf-8
import tushare as ts

cpp_taken = '75176520f735d971422dbff0cdcccc686bf414d416f7859372c8f77e'

pro = ts.pro_api(cpp_taken)
data = pro.query('stock_basic', exchange='', list_status='L', fields='ts_code,symbol,name,area,industry,list_date')

print(data)

with open("./stockname.txt", 'w') as f:
	f.write(str(data))

