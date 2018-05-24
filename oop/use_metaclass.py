class Hello(object):
	def hello(self, name='world'):
		print('Hello, %s.' % name)

from hello import Hello

h = Hello()

print(h.hello())

print(type(Hello))

print(type(h))


