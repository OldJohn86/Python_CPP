class Animal(object):

	def run(self):
		print('Animal is running...')

class Dog(Animal):

	def run(self):
		print('Dog is running...')

	def eat(self):
		print('Dog is eating meat...')

class Cat(Animal):

	def run(self):
		print('Cat is running...')

	def eat(self):
		print('Cat is eating meat...')


dog = Dog()
dog.run()

cat  = Cat()
cat.run()

a = list()
b = Animal()
c = Dog()
print(isinstance(a, list))
print(isinstance(b, Animal))
print(isinstance(c, Dog))
print(isinstance(c, Animal))
print(isinstance(b, Dog))

def run_twice(animal):
	animal.run()
	animal.run()

print(run_twice(Animal()))
print(run_twice(Dog()))
print(run_twice(Cat()))

class Tortoise(Animal):
	def run(self):
		print('Tortoise is running slowly...')

print(run_twice(Tortoise()))






