#-*- coding: utf-8 -*-

import numpy as np

#25. 如何导入数字和文本的数据集保持文本在numpy数组中完好无损？

# Solution
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')
names = ('sepallength', 'sepalwidth', 'petallength', 'petalwidth', 'species')
# Print the first 3 rows
print(iris[:3])

#26. 如何从1维元组数组中提取特定列？

# **给定：**
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris_1d = np.genfromtxt(url, delimiter=',', dtype=None)
print(iris_1d.shape)
# Solution:
species = np.array([row[4] for row in iris_1d])
print(species[:5])

#27. 如何将1维元组数组转换为2维numpy数组？

# **给定：**
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris_1d = np.genfromtxt(url, delimiter=',', dtype=None)

# Solution:
# Method 1: Convert each row to a list and get the first 4 items
iris_2d = np.array([row.tolist()[:4] for row in iris_1d])
print(iris_2d[:4])

# Alt Method 2: Import only the first 4 columns from source url
iris_2d = np.genfromtxt(url, delimiter=',', dtype='float', usecols=[0,1,2,3])
print(iris_2d[:4])


#28. 如何计算numpy数组的均值，中位数，标准差？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')
sepallength = np.genfromtxt(url, delimiter=',', dtype='float', usecols=[0])

# Solution
mu, med, sd = np.mean(sepallength), np.median(sepallength), np.std(sepallength)
print(mu, med, sd)

#29. 如何规范化数组，使数组的值正好介于0和1之间？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
sepallength = np.genfromtxt(url, delimiter=',', dtype='float', usecols=[0])

# Solution
Smax, Smin = sepallength.max(), sepallength.min()
S = (sepallength - Smin)/(Smax - Smin)
#print(S)
# or 
S = (sepallength - Smin)/sepallength.ptp()  # Thanks, David Ojeda!
#print(S)

#30. 如何计算Softmax得分？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')
sepallength = np.array([float(row[0]) for row in iris])

# Solution
def softmax(x):
    """Compute softmax values for each sets of scores in x.
    https://stackoverflow.com/questions/34968722/how-to-implement-the-softmax-function-in-python"""
    e_x = np.exp(x - np.max(x))
    return e_x / e_x.sum(axis=0)

#print(softmax(sepallength))

#31. 如何找到numpy数组的百分位数？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
sepallength = np.genfromtxt(url, delimiter=',', dtype='float', usecols=[0])

# Solution
print(np.percentile(sepallength, q=[5, 95]))
# > array([ 4.6  ,  7.255])

#32. 如何在数组中的随机位置插入值？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris_2d = np.genfromtxt(url, delimiter=',', dtype='object')

# Method 1
i, j = np.where(iris_2d)

# i, j contain the row numbers and column numbers of 600 elements of iris_x
np.random.seed(100)
iris_2d[np.random.choice((i), 20), np.random.choice((j), 20)] = np.nan

# Method 2
np.random.seed(100)
iris_2d[np.random.randint(150, size=20), np.random.randint(4, size=20)] = np.nan

# Print first 10 rows
print(iris_2d[:10])

#33. 如何在numpy数组中找到缺失值的位置？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris_2d = np.genfromtxt(url, delimiter=',', dtype='float', usecols=[0,1,2,3])
iris_2d[np.random.randint(150, size=20), np.random.randint(4, size=20)] = np.nan

# Solution
print("Number of missing values: \n", np.isnan(iris_2d[:, 0]).sum())
print("Position of missing values: \n", np.where(np.isnan(iris_2d[:, 0])))
# > Number of missing values: 
# >  5
# > Position of missing values: 
# >  (array([ 39,  88,  99, 130, 147]),)

#34. 如何根据两个或多个条件过滤numpy数组？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris_2d = np.genfromtxt(url, delimiter=',', dtype='float', usecols=[0,1,2,3])

# Solution
condition = (iris_2d[:, 2] > 1.5) & (iris_2d[:, 0] < 5.0)
iris_2d[condition]
# > array([[ 4.8,  3.4,  1.6,  0.2],
# >        [ 4.8,  3.4,  1.9,  0.2],
# >        [ 4.7,  3.2,  1.6,  0.2],
# >        [ 4.8,  3.1,  1.6,  0.2],
# >        [ 4.9,  2.4,  3.3,  1. ],
# >        [ 4.9,  2.5,  4.5,  1.7]])

#35. 如何从numpy数组中删除包含缺失值的行？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris_2d = np.genfromtxt(url, delimiter=',', dtype='float', usecols=[0,1,2,3])
iris_2d[np.random.randint(150, size=20), np.random.randint(4, size=20)] = np.nan

# Solution
# No direct numpy function for this.
# Method 1:
any_nan_in_row = np.array([~np.any(np.isnan(row)) for row in iris_2d])
iris_2d[any_nan_in_row][:5]

# Method 2: (By Rong)
iris_2d[np.sum(np.isnan(iris_2d), axis = 1) == 0][:5]
# > array([[ 4.9,  3. ,  1.4,  0.2],
# >        [ 4.7,  3.2,  1.3,  0.2],
# >        [ 4.6,  3.1,  1.5,  0.2],
# >        [ 5. ,  3.6,  1.4,  0.2],
# >        [ 5.4,  3.9,  1.7,  0.4]])

#36. 如何找到numpy数组的两列之间的相关性？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='float', usecols=[0,1,2,3])

# Solution 1
np.corrcoef(iris[:, 0], iris[:, 2])[0, 1]

# Solution 2
from scipy.stats.stats import pearsonr  
corr, p_value = pearsonr(iris[:, 0], iris[:, 2])
print(corr)

#37. 如何查找给定数组是否具有任何空值？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris_2d = np.genfromtxt(url, delimiter=',', dtype='float', usecols=[0,1,2,3])

np.isnan(iris_2d).any()
# > False

#38. 如何在numpy数组中用0替换所有缺失值？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris_2d = np.genfromtxt(url, delimiter=',', dtype='float', usecols=[0,1,2,3])
iris_2d[np.random.randint(150, size=20), np.random.randint(4, size=20)] = np.nan

# Solution
iris_2d[np.isnan(iris_2d)] = 0
iris_2d[:4]
# > array([[ 5.1,  3.5,  1.4,  0. ],
# >        [ 4.9,  3. ,  1.4,  0.2],
# >        [ 4.7,  3.2,  1.3,  0.2],
# >        [ 4.6,  3.1,  1.5,  0.2]])

#39. 如何在numpy数组中查找唯一值的计数？
# Import iris keeping the text column intact
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')
names = ('sepallength', 'sepalwidth', 'petallength', 'petalwidth', 'species')

# Solution
# Extract the species column as an array
species = np.array([row.tolist()[4] for row in iris])

# Get the unique values and the counts
np.unique(species, return_counts=True)
# > (array([b'Iris-setosa', b'Iris-versicolor', b'Iris-virginica'],
# >        dtype='|S15'), array([50, 50, 50]))

#40. 如何将数字转换为分类（文本）数组？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')
names = ('sepallength', 'sepalwidth', 'petallength', 'petalwidth', 'species')

# Bin petallength 
petal_length_bin = np.digitize(iris[:, 2].astype('float'), [0, 3, 5, 10])

# Map it to respective category
label_map = {1: 'small', 2: 'medium', 3: 'large', 4: np.nan}
petal_length_cat = [label_map[x] for x in petal_length_bin]

# View
petal_length_cat[:4]
# > ['small', 'small', 'small', 'small']

#41. 如何从numpy数组的现有列创建新列？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris_2d = np.genfromtxt(url, delimiter=',', dtype='object')

# Solution
# Compute volume
sepallength = iris_2d[:, 0].astype('float')
petallength = iris_2d[:, 2].astype('float')
volume = (np.pi * petallength * (sepallength**2))/3

# Introduce new dimension to match iris_2d's
volume = volume[:, np.newaxis]

# Add the new column
out = np.hstack([iris_2d, volume])

# View
out[:4]
# > array([[b'5.1', b'3.5', b'1.4', b'0.2', b'Iris-setosa', 38.13265162927291],
# >        [b'4.9', b'3.0', b'1.4', b'0.2', b'Iris-setosa', 35.200498485922445],
# >        [b'4.7', b'3.2', b'1.3', b'0.2', b'Iris-setosa', 30.0723720777127],
# >        [b'4.6', b'3.1', b'1.5', b'0.2', b'Iris-setosa', 33.238050274980004]], dtype=object)

#42. 如何在numpy中进行概率抽样？
# Import iris keeping the text column intact
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')

# Solution
# Get the species column
species = iris[:, 4]

# Approach 1: Generate Probablistically
np.random.seed(100)
a = np.array(['Iris-setosa', 'Iris-versicolor', 'Iris-virginica'])
species_out = np.random.choice(a, 150, p=[0.5, 0.25, 0.25])

# Approach 2: Probablistic Sampling (preferred)
np.random.seed(100)
probs = np.r_[np.linspace(0, 0.500, num=50), np.linspace(0.501, .750, num=50), np.linspace(.751, 1.0, num=50)]
index = np.searchsorted(probs, np.random.random(150))
species_out = species[index]
print(np.unique(species_out, return_counts=True))

# > (array([b'Iris-setosa', b'Iris-versicolor', b'Iris-virginica'], dtype=object), array([77, 37, 36]))

#43. 如何在按另一个数组分组时获取数组的第二大值？

# Import iris keeping the text column intact
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')

# Solution
# Get the species and petal length columns
petal_len_setosa = iris[iris[:, 4] == b'Iris-setosa', [2]].astype('float')

# Get the second last value
np.unique(np.sort(petal_len_setosa))[-2]
# > 1.7

#44. 如何按列对2D数组进行排序
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')
names = ('sepallength', 'sepalwidth', 'petallength', 'petalwidth', 'species')

# Sort by column position 0: SepalLength
print(iris[iris[:,0].argsort()][:20])
# > [[b'4.3' b'3.0' b'1.1' b'0.1' b'Iris-setosa']
# >  [b'4.4' b'3.2' b'1.3' b'0.2' b'Iris-setosa']
# >  [b'4.4' b'3.0' b'1.3' b'0.2' b'Iris-setosa']
# >  [b'4.4' b'2.9' b'1.4' b'0.2' b'Iris-setosa']
# >  [b'4.5' b'2.3' b'1.3' b'0.3' b'Iris-setosa']
# >  [b'4.6' b'3.6' b'1.0' b'0.2' b'Iris-setosa']
# >  [b'4.6' b'3.1' b'1.5' b'0.2' b'Iris-setosa']
# >  [b'4.6' b'3.4' b'1.4' b'0.3' b'Iris-setosa']
# >  [b'4.6' b'3.2' b'1.4' b'0.2' b'Iris-setosa']
# >  [b'4.7' b'3.2' b'1.3' b'0.2' b'Iris-setosa']
# >  [b'4.7' b'3.2' b'1.6' b'0.2' b'Iris-setosa']
# >  [b'4.8' b'3.0' b'1.4' b'0.1' b'Iris-setosa']
# >  [b'4.8' b'3.0' b'1.4' b'0.3' b'Iris-setosa']
# >  [b'4.8' b'3.4' b'1.9' b'0.2' b'Iris-setosa']
# >  [b'4.8' b'3.4' b'1.6' b'0.2' b'Iris-setosa']
# >  [b'4.8' b'3.1' b'1.6' b'0.2' b'Iris-setosa']
# >  [b'4.9' b'2.4' b'3.3' b'1.0' b'Iris-versicolor']
# >  [b'4.9' b'2.5' b'4.5' b'1.7' b'Iris-virginica']
# >  [b'4.9' b'3.1' b'1.5' b'0.1' b'Iris-setosa']
# >  [b'4.9' b'3.1' b'1.5' b'0.1' b'Iris-setosa']]

#45. 如何在numpy数组中找到最常见的值？
# **给定：**
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')

# Solution:
vals, counts = np.unique(iris[:, 2], return_counts=True)
print(vals[np.argmax(counts)])
# > b'1.5'

#46. 如何找到第一次出现的值大于给定值的位置？
# **给定：**
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')

# Solution: (edit: changed argmax to argwhere. Thanks Rong!)
np.argwhere(iris[:, 3].astype(float) > 1.0)[0]
# > 50

#47. 如何将大于给定值的所有值替换为给定的截止值？
# Input
np.set_printoptions(precision=2)
np.random.seed(100)
a = np.random.uniform(1,50, 20)

# Solution 1: Using np.clip
np.clip(a, a_min=10, a_max=30)

# Solution 2: Using np.where
print(np.where(a < 10, 10, np.where(a > 30, 30, a)))
# > [ 27.63  14.64  21.8   30.    10.    10.    30.    30.    10.    29.18  30.
# >   11.25  10.08  10.    11.77  30.    30.    10.    30.    14.43]

#48. 如何从numpy数组中获取最大n值的位置？
# Input
np.random.seed(100)
a = np.random.uniform(1,50, 20)

# Solution:
print(a.argsort())
# > [18 7 3 10 15]

# Solution 2:
np.argpartition(-a, 5)[:5]
# > [15 10  3  7 18]

# Below methods will get you the values.
# Method 1:
a[a.argsort()][-5:]

# Method 2:
np.sort(a)[-5:]

# Method 3:
np.partition(a, kth=-5)[-5:]

# Method 4:
a[np.argpartition(-a, 5)][:5]

#49. 如何计算数组中所有可能值的行数？
# **给定：**
np.random.seed(100)
arr = np.random.randint(1,11,size=(6, 10))
arr
# > array([[ 9,  9,  4,  8,  8,  1,  5,  3,  6,  3],
# >        [ 3,  3,  2,  1,  9,  5,  1, 10,  7,  3],
# >        [ 5,  2,  6,  4,  5,  5,  4,  8,  2,  2],
# >        [ 8,  8,  1,  3, 10, 10,  4,  3,  6,  9],
# >        [ 2,  1,  8,  7,  3,  1,  9,  3,  6,  2],
# >        [ 9,  2,  6,  5,  3,  9,  4,  6,  1, 10]])

# Solution
def counts_of_all_values_rowwise(arr2d):
    # Unique values and its counts row wise
    num_counts_array = [np.unique(row, return_counts=True) for row in arr2d]

    # Counts of all values row wise
    return([[int(b[a==i]) if i in a else 0 for i in np.unique(arr2d)] for a, b in num_counts_array])

# Print
print(np.arange(1,11))
counts_of_all_values_rowwise(arr)
# > [ 1  2  3  4  5  6  7  8  9 10]

# > [[1, 0, 2, 1, 1, 1, 0, 2, 2, 0],
# >  [2, 1, 3, 0, 1, 0, 1, 0, 1, 1],
# >  [0, 3, 0, 2, 3, 1, 0, 1, 0, 0],
# >  [1, 0, 2, 1, 0, 1, 0, 2, 1, 2],
# >  [2, 2, 2, 0, 0, 1, 1, 1, 1, 0],
# >  [1, 1, 1, 1, 1, 2, 0, 0, 2, 1]]

# Example 2:
arr = np.array([np.array(list('bill clinton')), np.array(list('narendramodi')), np.array(list('jjayalalitha'))])
print(np.unique(arr))
counts_of_all_values_rowwise(arr)
# > [' ' 'a' 'b' 'c' 'd' 'e' 'h' 'i' 'j' 'l' 'm' 'n' 'o' 'r' 't' 'y']

# > [[1, 0, 1, 1, 0, 0, 0, 2, 0, 3, 0, 2, 1, 0, 1, 0],
# >  [0, 2, 0, 0, 2, 1, 0, 1, 0, 0, 1, 2, 1, 2, 0, 0],
# >  [0, 4, 0, 0, 0, 0, 1, 1, 2, 2, 0, 0, 0, 0, 1, 1]]

#50. 如何将数组转换为平面一维数组？
 # **给定：**
arr1 = np.arange(3)
arr2 = np.arange(3,7)
arr3 = np.arange(7,10)

array_of_arrays = np.array([arr1, arr2, arr3])
print('array_of_arrays: ', array_of_arrays)

# Solution 1
arr_2d = np.array([a for arr in array_of_arrays for a in arr])

# Solution 2:
arr_2d = np.concatenate(array_of_arrays)
print(arr_2d)
# > array_of_arrays:  [array([0, 1, 2]) array([3, 4, 5, 6]) array([7, 8, 9])]
# > [0 1 2 3 4 5 6 7 8 9]


#
#
# https://www.numpy.org.cn/article/advanced/numpy_exercises_for_data_analysis.html
#
