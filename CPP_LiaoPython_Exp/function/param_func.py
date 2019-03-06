def power(x, n=2):
	s = 1
	while n > 0:
		n = n -1
		s = s * x
	return s

print(power(5))
print(power(5, 3))
print(power(5, 4))

def enroll(name, gender, age=6, city='Beijing'):
	print('name:', name)
	print('gender:', gender)
	print('age:', age)
	print('city:', city)

enroll('Sarah', 'F', 9)
enroll('Jason', 'F', city='Shanghai')

def add_end(L=None):
	if L is None:
		L = []
	L.append('END')
	return L

print(add_end())

def calc(*numbers):
	sum = 0
	for n in numbers:
		sum = sum + n * n
	return sum
print(calc(1, 2))
print(calc(0))

def person(name, age, **kw):
	print('name:', name, 'age:', age, 'other:', kw)

person('Michael', 30)
person('Bob', 35, city='Beijing')
person('Adam', 45, gender='M', job='Engineer')

extra = {'city': 'Beijing', 'job': 'Engineer'}
person('Jack', 24, **extra)

def person2(name, age, **kw):
	if 'city' in kw:
		pass
	if 'job' in kw:
		pass
	print('name:', name, 'age:', age, 'other:', kw)

person2('John', 31, city='Shanghai', addr='Pudong', zipcode=201210)

def person3(name, age, *, city, job):
	print(name, age, city, job)

person3('Jack', 24, city='Beijing', job='Engineer')




