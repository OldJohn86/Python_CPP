import tushare as ts
import psycopg2

class Stocks(object):#这个类表示"股票们"的整体(不是单元)
    def get_today_all(self):
        self.todayall=ts.get_today_all()

    def get_codestock_local(self):#从本地获取所有股票代号和名称
        conn = psycopg2.connect(database="wzj_quant", user=self.user, password=self.password, host="127.0.0.1",
                                port="5432")
        cur = conn.cursor()
        # 创建stocks表
        cur.execute('''
                select * from stocks;
               ''')
        rows =cur.fetchall()
        conn.commit()
        conn.close()

        return rows
        pass
    def __init__(self,user,password):
        # self.aaa = aaa
        self.todayall=[]
        self.user=user
        self.password=password

    def db_perstock_insertsql(self,stock_code,cns_name):#返回的是插入语句
        sql_temp="insert into stocks values("
        sql_temp+="\'"+stock_code+"\'"+","+"\'"+cns_name+"\'"
        sql_temp +=");"
        return sql_temp
        pass

    def db_stocks_update(self):# 根据gettodayall的情况插入原表中没的。。gettodayall中有的源表没的保留不删除#返回新增行数
        ans=0
        conn = psycopg2.connect(database="wzj_quant", user=self.user, password=self.password, host="127.0.0.1", port="5432")
        cur = conn.cursor()
        self.get_today_all()

        for i in range(0,len(self.todayall)):
            sql_temp='''select * from stocks where stock_code='''
            sql_temp+="\'"+self.todayall["code"][i]+"\';"
            cur.execute(sql_temp)
            rows=cur.fetchall()
            if(len(rows)==0):
                #如果股票代码没找到就插
                ans+=1
                cur.execute(self.db_perstock_insertsql(self.todayall["code"][i],self.todayall["name"][i]))
                pass
        conn.commit()
        conn.close()
        print("db_stocks_update finish")
        return ans

    def db_stocks_create(self):
        conn = psycopg2.connect(database="wzj_quant", user=self.user, password=self.password, host="127.0.0.1", port="5432")
        cur = conn.cursor()
        # 创建stocks表
        cur.execute('''
            drop table if exists stocks;
            create table stocks(stock_code varchar primary key,cns_name varchar);
        ''')
        conn.commit()
        conn.close()
        print("db_stocks_create finish")
        pass