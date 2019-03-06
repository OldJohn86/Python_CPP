#!/usr/bin/env python3
# -*- coding: utf-8 -*-

class MyObject(object):
	def __init__(self):
		self.x = 9

	def power(self):
		return self.x * self.x

obj = MyObject()

print('hasattr(obj, \'x\') =', hasattr(obj, 'x'))
print('hasattr(obj, \'y\') =', hasattr(obj, 'y'))
setattr(obj, 'y', 19)

print('hasattr(obj, \'y\') =', hasattr(obj, 'y'))
print('getattr(obj, \'y\') =', getattr(obj, 'y'))
print('obj.y =', obj.y)

print('genattr(obj, \'z\') =', getattr(obj, 'z', 404))

f = getattr(obj, 'power')
print(f)
print(f())


