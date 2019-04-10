#-*- coding:utf-8 -*-
import numpy as np 

#导入numpy作为np，并查看版本
print(np.__version__)

#如何创建一维数组？
arr = np.arange(10)
print(arr)

#如何创建一个布尔数组？
arr = np.array([0,1,2,3,4,5,6,7,8,9])
print(arr[arr % 2 == 1])

#如何从一维数组中提取满足指定条件的元素？
arr[arr % 2 == 1] = -1
print(arr)

#如何用numpy数组中的另一个值替换满足条件的元素项？
arr = np.arange(10)
out = np.where(arr % 2 == 1, -1, arr)
print(arr)
print(out)

#如何改变数组的形状？
arr = np.arange(10)
print(arr.reshape(2,-1))


#如何垂直叠加两个数组？
a = np.arange(10).reshape(2,-1)
print(a)
b = np.repeat(1, 10).reshape(2,-1)
print(b)

print(np.concatenate([a,b], axis=0))
print(np.vstack([a,b]))
print(np.r_[a,b])

#10. 如何水平叠加两个数组？
a = np.arange(10).reshape(2,-1)
b = np.repeat(1,10).reshape(2,-1)
print(np.concatenate([a,b], axis=1))
print(np.hstack([a,b]))
print(np.c_[a,b])

#11. 如何获取两个numpy数组之间的公共项？
a = np.array([1,2,3,2,3,4,3,4,5,6])
b = np.array([7,2,10,2,7,4,9,4,9,8])
print(np.intersect1d(a,b))

#12. 如何从一个数组中删除存在于另一个数组中的项？
a = np.array([1,2,3,4,5])
b = np.array([5,6,7,8,9])
print(np.setdiff1d(a,b))

#13. 如何得到两个数组元素匹配的位置？
a = np.array([1,2,3,2,3,4,3,4,5,6])
b = np.array([7,2,10,2,7,4,9,4,9,8])

print(np.where(a == b))

#14. 如何从numpy数组中提取给定范围内的所有数字？
a = np.array([2, 6, 1, 9, 10, 3, 27])

#a = np.arange(15)
index = np.where((a >= 5) & (a <= 10))
print(a[index])

index = np.where(np.logical_and(a>=5, a<=10))
print(a[index])

print(a[(a >= 5) & (a <= 10)])

#15. 如何创建一个python函数来处理scalars并在numpy数组上工作？
def maxx(x, y):
    """Get the maximum of two items"""
    if x >= y:
        return x
    else:
        return y

pair_max = np.vectorize(maxx, otypes=[float])

a = np.array([5, 7, 9, 8, 6, 4, 5])
b = np.array([6, 3, 4, 8, 9, 7, 1])

print(pair_max(a,b))

#################################################################################
#
#
# https://www.numpy.org.cn/article/advanced/numpy_exercises_for_data_analysis.html
#