L = [x * x for x in range(10)]
print(L)

g = (x * x for x in range(10))
for n in g:
	print(n)

def fib(max):
	n, a, b = 0, 0, 1
	while n < max:
		print(b)
		a, b = b, a + b
		n = n +1
	return 'done'

fib(6)
fib(10)

def fib_g(max):
	n, a, b = 0, 0, 1
	while n < max:
		yield b
		a, b = b, a + b
		n = n + 1
	return 'done'

f = fib_g(6)
print(f)

def odd():
	print('step 1')
	yield 1
	print('step 2')
	yield (3)
	print('step 3')
	yield (5)

o = odd()
next(o)
next(o)
next(o)

g = fib_g(6)
while True:
	try:
		x = next(g)
		print('g:', x)
	except StopIteration as e:
		print('Generator return value:', e.value)
		break

def triangles():
	list = [1]
	while True:
		L = list[:]
		yield L
		for d in range(1, len(list)):
			if len(list) == 1:
				pass
			list[d] = L[d] + L[d - 1]
		list[0] = 1
		list.append(1)

n = 0
results = []
for t in triangles():
    print(t)
    results.append(t)
    n = n + 1
    if n == 10:
        break

if results == [
    [1],
    [1, 1],
    [1, 2, 1],
    [1, 3, 3, 1],
    [1, 4, 6, 4, 1],
    [1, 5, 10, 10, 5, 1],
    [1, 6, 15, 20, 15, 6, 1],
    [1, 7, 21, 35, 35, 21, 7, 1],
    [1, 8, 28, 56, 70, 56, 28, 8, 1],
    [1, 9, 36, 84, 126, 126, 84, 36, 9, 1]
]:
    print('测试通过!')
else:
    print('测试失败!')


