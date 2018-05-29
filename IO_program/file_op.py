#f = open('/Users/pchen/github/Python_CPP/err-test/test.txt', 'aw')
#f.write('\nHello, World!!')
#f.read()
#f.close()

#try:
#	file = open('/Users/pchen/github/Python_CPP/err-test/test.txt', 'r')
#	print(file.read())
#finally:
#	if file:
#		file.close()

#with open('/Users/pchen/github/Python_CPP/err-test/test.txt', 'r') as f2:
#	print(f2.read())

#with open('/Users/pchen/Desktop/hosts', 'r') as f:
#	for line in f.readlines():
#		print(line.strip())

#f1 = open('/Users/pchen/g3-ddr3.jpg', 'rb')
#f1.read()

#f = open('/Users/pchen/github/Python_CPP/err-test/test.txt', 'r', encoding='gbk')
#f.read()

with open('/Users/pchen/github/Python_CPP/err-test/test.txt', 'w') as f:
	f.write('Hello World!')
