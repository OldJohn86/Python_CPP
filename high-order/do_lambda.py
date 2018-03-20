print(list(map(lambda x: x * x, [1, 2, 3, 4, 5, 6, 7, 8, 9])))


def f(x):
	return x * x

print(list(map(f, [1, 2, 3, 4 ,5, 6, 7, 8, 9])))

fun = lambda x: x * x
print(f(5))

def build(x, y):
	return lambda: x * x + y * y



def is_odd(n):
	return n % 2 == 1

L = list(filter(is_odd, range(1, 20)))
print(L)

L1 = list(filter(lambda x: x % 2 == 1, range(1, 20)))
print(L1)

