def fact1(n):
	if n==1:
		return 1
	return n * fact(n - 1)

def fact(n):
	return fact_iter(n, 1)

def fact_iter(num, product):
	if num == 1:
		return product
	return fact_iter(num - 1, num * product)

print(fact(1))
print(fact(5))
print(fact(50))
print(fact(100))
#print(fact(1000))

def hanoi(n, x, y, z):
	if n == 1:
		print(x, '-->', z)
	else:
		hanoi(n-1, x, z, y)
		hanoi(1, x, y, z)
		hanoi(n-1, y, x, z)
hanoi(3, 'A', 'B', 'C')






