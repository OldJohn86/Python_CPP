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
# Method 1
index = np.where((a >= 5) & (a <= 10))
print(a[index])
# Method 2
index = np.where(np.logical_and(a>=5, a<=10))
print(a[index])
# Method 3
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

#16. 如何交换二维numpy数组中的两列？
arr = np.arange(9).reshape(3,3)
print(arr)
print(arr[:,[1,0,2]])

#17. 如何交换二维numpy数组中的两行？
arr = np.arange(9).reshape(3,3)
print(arr)
print(arr[[1,0,2],:])

#18. 如何反转二维数组的行？
arr = np.arange(9).reshape(3,3)
print(arr[::-1])

#19. 如何反转二维数组的列？
arr = np.arange(9).reshape(3,3)
print(arr[:, ::-1])

#20. 如何创建包含5到10之间随机浮动的二维数组？
arr = np.arange(9).reshape(3,3)

# Solution Method 1:
rand_arr = np.random.randint(low=5, high=10, size=(5,3)) + np.random.random((5,3))
print(rand_arr)
# Solution Method 2:
rand_arr = np.random.uniform(5,10, size=(5,3))
print(rand_arr)

#21. 如何在numpy数组中只打印小数点后三位？

# Create the random array
rand_arr = np.random.random([5,3])

# Limit to 3 decimal places
np.set_printoptions(precision=3)
print(rand_arr[:4])

#22. 如何通过e式科学记数法（如1e10）来打印一个numpy数组？

np.set_printoptions(suppress=False)
# Create the random array
np.random.seed(100)
rand_arr = np.random.random([3,3])/1e3
print(rand_arr)

np.set_printoptions(suppress=True, precision=6)  # precision is optional
print(rand_arr)

#23. 如何限制numpy数组输出中打印的项目数？
a = np.arange(15)
print(a)
np.set_printoptions(threshold=6)
a = np.arange(15)
print(a)

#24. 如何打印完整的numpy数组而不截断
np.set_printoptions(threshold=6)
a = np.arange(15)
print(a)
# Solution
np.set_printoptions(threshold=np.nan)
print(a)


#################################################################################
#
#
# https://www.numpy.org.cn/article/advanced/numpy_exercises_for_data_analysis.html
#