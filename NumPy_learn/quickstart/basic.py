#-*- utf-8 -*-

import numpy as np

#
a = np.arange(15).reshape(3,5)
print(a)
print(a.shape)
print(a.ndim)
print(a.dtype.name)
print(a.itemsize)
print(a.size)
print(type(a))

b = np.array([6, 7, 8])
print(b)
print(type(b))

#
a = np.array([2, 3, 4])
print(a)
print(a.dtype)

b = np.array([1.2, 3.5, 5.1])
print(b.dtype)

b = np.array([(1.5,2,3), (4,5,6)])
print(b)

c = np.array( [[1,2], [3,4]], dtype=complex )
print(c)

print(np.zeros((3,4)))
print(np.ones((2,3,4), dtype=np.int16))# dtype can also be specified
print(np.empty((2,3)))# uninitialized, output may vary

print(np.arange(10, 30, 5))
print(np.arange(0, 2, 0.3)) # it accepts float arguments

from numpy import pi
print(np.linspace(0, 2, 9))# 9 numbers from 0 to 2
x = np.linspace(0, 2*pi, 100)# useful to evaluate function at lots of points
f = np.sin(x)
print(f)

#
a = np.arange(6)
print(a)

b = np.arange(12).reshape(4,3)
print(b)

c = np.arange(24).reshape(2,3,4)
print(c)

print(np.arange(10000))

print(np.arange(10000).reshape(100,100))
#np.set_printoptions(threshold=np.nan)

#
a = np.array([20, 30, 40, 50])
b = np.arange(4)
print(b)

c = a - b
print(c)

print(b**2)
print(10*np.sin(a))
print(a<35)

A = np.array([[1,1], [0,1]])
B = np.array([[2,0],[3,4]])

print(A*B)
print(A.dot(B))
print(np.dot(A,B))

a = np.ones((2,3), dtype=int)
b = np.random.random((2,3))
a *= 3
print(a)
b += a
print(b)

a = np.ones(3, dtype=np.int32)
b = np.linspace(0, pi, 3)
print(b.dtype.name)

c = a + b
print(c)
print(c.dtype.name)

d = np.exp(c*1j)
print(d)
print(d.dtype.name)

a = np.random.random((2,3))
print(a)
print(a.sum())
print(a.min())
print(a.max())

b = np.arange(12).reshape(3,4)
print(b)
print(b.sum(axis=0))
print(b.min(axis=1))
print(b.cumsum(axis=1))

#
B = np.arange(3)
print(B)
print(np.exp(B))
print(np.sqrt(B))
C = np.array([2, -1, 4])
print(np.add(B,C))

#
a = np.arange(10)**3
print(a)
print(a[2])
print(a[2:5])
a[:6:2] = -1000
print(a)
print(a[::-1])

for i in a:
	print(i**(1/3))


#
def f(x,y):
	return 10*x+y

b = np.fromfunction(f, (5,4), dtype=int)
print(b)

print(b[2,3])
print(b[0:5, 1])
print(b[:,1])
print(b[1:3,:])
print(b[-1])

c = np.array([[[0, 1, 2], [10, 12, 13]],[[100, 101, 102],[110, 112, 113]]])
print(c.shape)
print(c[1,...])
print(c[...,2])

#
for row in b:
	print(row)

for element in b.flat:
	print(element)

#
#https://www.numpy.org.cn/user_guide/quickstart_tutorial/the_basics.html
#
