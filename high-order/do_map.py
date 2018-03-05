def f(x):
	return x * x

r = map(f, [1, 2, 3, 4, 5, 6, 7, 8, 9])
print(list(r))

L =[]
for n in [1, 2, 3, 4, 5, 6, 7, 8, 9]:
	L.append(f(n))
print(L)

print(list(map(str, [1, 2, 3, 4, 5, 6, 7, 8, 9])))

from functools import reduce
def add(x, y):
	return x + y

print(reduce(add, [1, 3, 5, 7, 9]))

def fn(x, y):
	return x * 10 +y

print(reduce(fn, [1, 3, 5, 7, 9]))

def char2num(s):
	digits = {'0': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9}
	return digits[s]
print(reduce(fn, map(char2num, '13579')))


DIGITS = {'0': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9}

def str2int(s):
	def fn(x, y):
		return x * 10 + y
	def char2num(s):
		return DIGITS[s]
	return reduce(fn, map(char2num, s))

def str2int_2(s):
	return reduce(lambda x, y: x * 10 + y, map(char2num, s))

def normalize(name):
	return name[0].upper() + name[1:].lower()

# 测试:
L1 = ['adam', 'LISA', 'barT']
L2 = list(map(normalize, L1))
print(L2)

from functools import reduce
def prod(L):
	return reduce(lambda x, y: x * y, L)

print('3 * 5 * 7 * 9 =', prod([3, 5, 7, 9]))
if prod([3, 5, 7, 9]) == 945:
    print('测试成功!')
else:
    print('测试失败!')


from functools import reduce
def str2float(s):
	int_part, dec_part = s.split('.')
	return reduce(lambda x, y: 10 * x + y, map(int, int_part + dec_part)) / (10 ** len(dec_part))

print('str2float(\'123.456\') =', str2float('123.456'))
if abs(str2float('123.456') - 123.456) < 0.00001:
    print('测试成功!')
else:
    print('测试失败!')







