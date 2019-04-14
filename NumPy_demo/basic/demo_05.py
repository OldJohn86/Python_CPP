#-*- coding:utf-8 -*-

import numpy as np

array = np.arange(20)
print(array)
print(array.shape)
print(array[3])

array[3] = 100
print(array)

array_2 = np.arange(20).reshape(4,5)
print(array_2)
print(array_2.shape)
print(array_2[3][4])


array_3 = np.arange(27).reshape(3,3,3)
print(array_3)
print(array_3.shape)
print(np.arange(10, 35, 3))

zero_array = np.zeros((2,4))
print(zero_array)
one_array = np.ones((3,4))
print(one_array)
empty_array = np.empty((2,3))
print(empty_array)
full_array = np.full((2,2), 3)
print(full_array)
eye_array = np.eye(3,3)
print(eye_array)
lin_array = np.linspace(0, 10, num=4)
print(lin_array)

array = np.array([4, 5, 6])
print(array)

list = [4, 5, 6]
print(list)
array = np.array(list)
print(array)
print(type(list))

array = np.array([(1,2,3), (4,5,6)])
print(array)
print(array.shape)
print(np.random.random((2,2)))


#####################################################################
#
#
# https://www.numpy.org.cn/article/basics/python_numpy_tutorial.html
#