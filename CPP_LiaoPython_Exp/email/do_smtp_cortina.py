from email import encoders
from email.header import Header
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email.utils import parseaddr, formataddr

import smtplib

def _format_addr(s):
	name, addr = parseaddr(s)
	return formataddr((Header(name, 'utf-8').encode(), addr))

from_addr = input('From: ')
password = input('Password: ')
to_addr = input('To: ')
smtp_server = input('SMTP server: ')

msg = MIMEMultipart()
msg['From'] = _format_addr('Python爱好者 <%s>' % from_addr)
msg['To'] = _format_addr('管理员 <%s>' % to_addr)
msg['Subject'] = Header('来自SMTP的问候......', 'utf-8').encode()

msg.attach(MIMEText('send with file...', 'plain', 'utf-8'))

with open('/Users/pchen/github/Python_CPP/inter_module/test1.jpg', 'rb') as f:
	mime = MIMEBase('image', 'jpg', filename='test1.jpg')
	mime.add_header('Content-Disposition', 'attachment', filename='test1.jpg')
	mime.add_header('Content-ID', '<0>')
	mime.add_header('X-Attachment-Id', '0')
	mime.set_payload(f.read())
	encoders.encode_base64(mime)
	msg.attach(mime)

try:
    mailserver = smtplib.SMTP("smtp.office365.com", 587)
    mailserver.ehlo()
    mailserver.starttls()
    mailserver.ehlo()
    mailserver.set_debuglevel(1)
    mailserver.login(from_addr, password)
    mailserver.sendmail(from_addr, [to_addr], msg.as_string())
    mailserver.quit()
    print("Email send sucess")
except smtplib.SMTPException as e:
    print("Email send failed", e)

