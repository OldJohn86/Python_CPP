#!/usr/bin/python3
 
import pymysql
 
# 打开数据库连接
db = pymysql.connect("localhost","root","1q2w3e4R","scraping" )
 
# 使用 cursor() 方法创建一个游标对象 cursor
cursor = db.cursor()
 
# 使用 execute()  方法执行 SQL 查询 
#cursor.execute("SELECT VERSION()")
cursor.execute("USE scraping")
cursor.execute("SELECT * FROM pages WHERE id=2")

# 使用 fetchone() 方法获取单条数据.
data = cursor.fetchone()
 
#print ("Database version : %s " % data)
print (data)

# 关闭数据库连接
cursor.close()
db.close()
