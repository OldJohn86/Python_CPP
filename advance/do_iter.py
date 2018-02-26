list = ['John', 'Peter', 'Jason', 'Joe']

d = {'a': 1, 'b': 2, 'c': 3}
for key in d:
	print(key)

for ch in 'ABCD':
	print(ch)

from collections import Iterable
print(isinstance('abc', Iterable))
print(isinstance([1, 2, 3], Iterable))
print(isinstance(123, Iterable))

for i, value in enumerate(list):
	print(i, value)

for x, y in [(1, 1), (2, 4), (3, 9)]:
	print(x,y)

L1 = [1, 2, 6, 8, 4, 3, 0]
def findMinAndMax(L):
	if len(L) == 0:
		return (None, None)
	else:
		for i, value in enumerate(L):
			if i == 0:
				Max = L[i]
				Min = L[i]
			else:
				if value > Max:
					Max = L[i]
				if value < Min:
					Min = L[i]
		return (Min, Max)


print(findMinAndMax(L1))

# 测试
if findMinAndMax([]) != (None, None):
    print('测试失败!')
elif findMinAndMax([7]) != (7, 7):
    print('测试失败!')
elif findMinAndMax([7, 1]) != (1, 7):
    print('测试失败!')
elif findMinAndMax([7, 1, 3, 9, 5]) != (1, 9):
    print('测试失败!')
else:
    print('测试成功!')






