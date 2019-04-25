# -*- coding: utf-8 -*-
import os
import sys
import time
import datetime
import paramiko
#import telnetlib
import getpass
import smtplib

from datetime import date
from configparser import ConfigParser
from email import encoders
from email.header import Header
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

import tushare as ts

def read_ini(config, option):
    info = dict()
    cf = ConfigParser()
    cf.read(config, encoding='utf-8')
    keys = cf.options(option)
    for each in keys:
        info[each] = cf.get(option, each)
    # print(info)
    return info

def send_mail(config, text_msg):
    mail_info = read_ini(config, 'mail')
    mail_user = str(mail_info.get('user', None))
    mail_postfix = str(mail_info.get('postfix', None))
    mail_pwd = str(mail_info.get('pwd', None))
    mail_host = str(mail_info.get('host', None))
    mail_port = str(mail_info.get('port', None))
    to_list = str(mail_info.get('to_list', None))
    list_mailaddr = to_list.split()
    # print(list_mailaddr)
    mailto_list = [x +'@'+ mail_postfix for x in list_mailaddr]
    # print(mailto_list)

    my_mail = mail_user +"@" + mail_postfix

    msg = MIMEMultipart()
    msg['Subject'] = date.today().strftime('%Y-%m-%d') + " AGu Daily Report..."
    context_msg = 'AAAAAAAAAAAAAAAAAAAAAA'
    # print(context_msg)
    
    msg['From'] = my_mail
    msg['To'] = ";".join(mailto_list)
    # msg.attach(MIMEText('send with sanity test log file...', 'plain', 'utf-8'))
    # msg.attach(MIMEText('Test Image: \r\n' + context_msg, 'plain', 'utf-8'))

    #text_msg = '[Results]: \r\n'+ context_msg + '\r\n\r\n [Rev Info]: \r\n'
    msg.attach(MIMEText(text_msg, 'plain', 'utf-8'))
 
    #att1 = MIMEText(open(glb_log_file, 'rb').read(), 'base64', 'utf-8')
    #att1["Content-Type"] = 'application/octet-stream'
    #att1["Content-Disposition"] = 'attachment; filename="sanity-test-log.txt"'
    #msg.attach(att1)
    try:
        smtpObj = smtplib.SMTP(mail_host, mail_port)
        # smtpObj.ehlo()
        smtpObj.starttls()
        # smtpObj.ehlo()
        # smtpObj.set_debuglevel(1)
        smtpObj.login(my_mail, mail_pwd)
        smtpObj.sendmail(my_mail, mailto_list, msg.as_string())
        smtpObj.quit()
        print("Email send success")
    except smtplib.SMTPException as e:
        print("Email send failed", e)


if __name__ == "__main__":
    #y_m_d = date.today().strftime('%Y%m%d')
    #print(y_m_d)

    current_path = sys.argv[0].rstrip('/tstest_cpp.py')
    #print(current_path)
    config = os.path.join(current_path, 'ts_config.ini')
    #print(config)

    ts_info = read_ini(config, 'tushare')
    ts_token = str(ts_info.get('cpp_token', None))
    #print(ts_token)
    pro = ts.pro_api(ts_token)

    stock_info = read_ini(config, 'stock')
    stock_pool = str(stock_info.get('stock_pool', None)).split()
    print(stock_pool)

    total = len(stock_pool)
    msg = ''
    for i in range(len(stock_pool)):
        try:
            df = pro.daily(ts_code=stock_pool[i], start_date=date.today().strftime('%Y%m%d'), end_date=date.today().strftime('%Y%m%d'))
            print('Seq: ' + str(i+1) + ' of ' + str(total) + '   Code: ' + str(stock_pool[i]))
            msg += str(df) + '\n'
        except Exception as aa:
            print(aa)
    
    send_mail(config, msg)