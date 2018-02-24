L = ['Michael', 'Sarah', 'Tracy', 'Bob', 'Jack']
print(L)
print(L[:3])
print(L[-2:-1])

L1 = list(range(100))
print(L1)
print(L1[:10])
print(L1[-10:])
print(L1[10:20])
print(L1[:10:2])
print(L1[::5])
print(L1[:])

def trim(s):
	while s[:1] == ' ':
		s = s[1:]
		trim(s)
	while s[-1:] == ' ':
		s = s[0:-1]
		trim(s)
	return s

def test_space():
	if trim('hello  ') != 'hello':
		print('测试失败!')
	elif trim('  hello') != 'hello':
		print('测试失败!')
	elif trim('  hello  ') != 'hello':
		print('测试失败!')
	elif trim('  hello  world  ') != 'hello  world':
		print('测试失败!')
	elif trim('') != '':
		print('测试失败!')
	elif trim('    ') != '':
		print('测试失败!')
	else:
		print('测试成功!')

test_space()

