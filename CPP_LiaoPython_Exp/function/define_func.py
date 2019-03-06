def my_abs(x):
	if not isinstance(x, (int, float)):
		raise TypeError('bad operand type')
	if x > 0:
		return x
	else:
		return -x

print(my_abs(100))
print(my_abs(-99))

def nop():
	pass

age =18
if age >= 18:
	pass

import math

def move(x, y, step, angle=0):
	nx = x + step * math.cos(angle)
	ny = y - step * math.sin(angle)
	return nx, ny

x, y = move(100, 100, 60, math.pi / 6)
print(x, y)

r = move(100, 100, 60, math.pi / 6)
print(r)

def quadratic(a, b ,c):
	if a == 0:
		return TypeError('paramter a must not be Zero')
	if not isinstance(a, (int, float)):
		return TypeError('Bad operand type')
	if not isinstance(b, (int, float)):
		return TypeError('Bad operand type')
	if not isinstance(b, (int, float)):
		return TypeError('Bad operand type')
	return ((-b + math.sqrt(b * b - 4 * a * c)) / (2 * a), (-b - math.sqrt(b * b - 4 * a * c)) / (2 * a))



