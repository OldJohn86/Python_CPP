import re

test1 = '010-12345'
test2 = '010 12345'

if re.match(r'^\d{3}\-\d{3,8}$', test1):
	print('ok')
else:
	print('failed')

if re.match(r'^\d{3}\-\d{3,8}$', test2):
	print('ok')
else:
	print('failed')

print('a b   c'.split(' '))
print(re.split(r'\s+', 'a b   c'))
print(re.split(r'[\s\,]+', 'a,b, c  d'))
print(re.split(r'[\s\,\;]+', 'a,b;; c   d'))

m = re.match(r'^(\d{3})-(\d{3,8})$', '010-12345')
print(m.group(0))
print(m.group(1))
print(m.group(2))

t = '19:05:30'
m = re.match(r'^(0[0-9]|1[0-9]|2[0-3]|[0-9])\:(0[0-9]|1[0-9]|2[0-9]|3[0-9]|4[0-9]|5[0-9]|[0-9])\:(0[0-9]|1[0-9]|2[0-9]|3[0-9]|4[0-9]|5[0-9]|[0-9])$', t)
print(m.groups())
print(m.group(1))
print(m.group(2))
print(m.group(3))
print(m.group(0))

print(re.match(r'^(\d+)(0*)$', '102300').groups())
print(re.match(r'^(\d+?)(0*)$', '102300').groups())

import re

re_telephone = re.compile(r'^(\d{3})-(\d{3,8})$')
print(re_telephone.match('010-12345').groups())
print(re_telephone.match('010-8086').groups())


import re

email1 = 'someone@gmail.com'
email2 = 'bill.gates@microsoft.com'

email_match = re.compile(r'^[a-zA-Z\][0-9a-zA-Z.]+@[0-9a-zA-Z_]+.com$')

def is_valid_email(addr):
	if email_match.match(addr):
		return True
	else:
		return False

assert is_valid_email('someone@gmail.com')
assert is_valid_email('bill.gates@microsoft.com')
assert not is_valid_email('bob#example.com')
assert not is_valid_email('mr-bob@example.com')
print('ok')


import re
re_email_name = re.compile(r'^[<]?([\w\s]+)[>|@].+$')

def name_of_email(addr):
	r = re_email_name.match(addr)
	if r:
		return r.group(1)

assert name_of_email('<Tom Paris> tom@voyager.org') == 'Tom Paris'
assert name_of_email('tom@voyager.org') == 'tom'
print('ok')



