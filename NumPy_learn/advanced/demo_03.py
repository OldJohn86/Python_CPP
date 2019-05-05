#-*- utf-8 -*-

import numpy as np

# 51. 如何在numpy中为数组生成单热编码？
# **给定：**
np.random.seed(101) 
arr = np.random.randint(1,4, size=6)
arr
# > array([2, 3, 2, 2, 2, 1])

# Solution:
def one_hot_encodings(arr):
        uniqs = np.unique(arr)
        out = np.zeros((arr.shape[0], uniqs.shape[0]))
        for i, k in enumerate(arr):
            out[i, k-1] = 1
        return out

one_hot_encodings(arr)
# > array([[ 0.,  1.,  0.],
# >        [ 0.,  0.,  1.],
# >        [ 0.,  1.,  0.],
# >        [ 0.,  1.,  0.],
# >        [ 0.,  1.,  0.],
# >        [ 1.,  0.,  0.]])

# Method 2:
(arr[:, None] == np.unique(arr)).view(np.int8)


#52. 如何创建按分类变量分组的行号？
# **给定：**
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
species = np.genfromtxt(url, delimiter=',', dtype='str', usecols=4)
np.random.seed(100)
species_small = np.sort(np.random.choice(species, size=20))
species_small
# > array(['Iris-setosa', 'Iris-setosa', 'Iris-setosa', 'Iris-setosa',
# >        'Iris-setosa', 'Iris-versicolor', 'Iris-versicolor',
# >        'Iris-versicolor', 'Iris-versicolor', 'Iris-versicolor',
# >        'Iris-versicolor', 'Iris-versicolor', 'Iris-versicolor',
# >        'Iris-versicolor', 'Iris-virginica', 'Iris-virginica',
# >        'Iris-virginica', 'Iris-virginica', 'Iris-virginica',
# >        'Iris-virginica'],
# >       dtype='<U15')
print([i for val in np.unique(species_small) for i, grp in enumerate(species_small[species_small==val])])

#53. 如何根据给定的分类变量创建组ID？
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
species = np.genfromtxt(url, delimiter=',', dtype='str', usecols=4)
species_small = np.sort(np.random.choice(species, size=20))
species_small
# > array(['Iris-setosa', 'Iris-setosa', 'Iris-setosa', 'Iris-setosa',
# >        'Iris-setosa', 'Iris-setosa', 'Iris-versicolor', 'Iris-versicolor',
# >        'Iris-versicolor', 'Iris-versicolor', 'Iris-versicolor',
# >        'Iris-versicolor', 'Iris-virginica', 'Iris-virginica',
# >        'Iris-virginica', 'Iris-virginica', 'Iris-virginica',
# >        'Iris-virginica', 'Iris-virginica', 'Iris-virginica'],
# >       dtype='<U15')

# Solution:
output = [np.argwhere(np.unique(species_small) == s).tolist()[0][0] for val in np.unique(species_small) for s in species_small[species_small==val]]

# Solution: For Loop version
output = []
uniqs = np.unique(species_small)

for val in uniqs:  # uniq values in group
    for s in species_small[species_small==val]:  # each element in group
        groupid = np.argwhere(uniqs == s).tolist()[0][0]  # groupid
        output.append(groupid)

print(output)
# > [0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2]

#54. 如何使用numpy对数组中的项进行排名？
np.random.seed(10)
a = np.random.randint(20, size=10)
print(a)
# > [ 9  4 15  0 17 16 17  8  9  0]

#55. 如何使用numpy对多维数组中的项进行排名？
np.random.seed(10)
a = np.random.randint(20, size=[2,5])
print(a)
# > [[ 9  4 15  0 17]
# >  [16 17  8  9  0]]

# Solution
print(a.ravel().argsort().argsort().reshape(a.shape))
# > [[ 9  4 15  0 17]
# >  [16 17  8  9  0]]
# > [[4 2 6 0 8]
# >  [7 9 3 5 1]]

#56. 如何在二维numpy数组的每一行中找到最大值？
# Input
np.random.seed(100)
a = np.random.randint(1,10, [5,3])
a

# Solution 1
np.amax(a, axis=1)

# Solution 2
np.apply_along_axis(np.max, arr=a, axis=1)
# > array([9, 8, 6, 3, 9])

#57. 如何计算二维numpy数组每行的最小值？
# Input
np.random.seed(100)
a = np.random.randint(1,10, [5,3])
a

# Solution
np.apply_along_axis(lambda x: np.min(x)/np.max(x), arr=a, axis=1)
# > array([ 0.44444444,  0.125     ,  0.5       ,  1.        ,  0.11111111])

#58. 如何在numpy数组中找到重复的记录？
# Input
np.random.seed(100)
a = np.random.randint(0, 5, 10)

## Solution
# There is no direct function to do this as of 1.13.3

# Create an all True array
out = np.full(a.shape[0], True)

# Find the index positions of unique elements
unique_positions = np.unique(a, return_index=True)[1]

# Mark those positions as False
out[unique_positions] = False

print(out)
# > [False  True False  True False False  True  True  True  True]

#59. 如何找出数字的分组均值？
# Input
url = 'https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data'
iris = np.genfromtxt(url, delimiter=',', dtype='object')
names = ('sepallength', 'sepalwidth', 'petallength', 'petalwidth', 'species')


# Solution
# No direct way to implement this. Just a version of a workaround.
numeric_column = iris[:, 1].astype('float')  # sepalwidth
grouping_column = iris[:, 4]  # species

# List comprehension version
[[group_val, numeric_column[grouping_column==group_val].mean()] for group_val in np.unique(grouping_column)]

# For Loop version
output = []
for group_val in np.unique(grouping_column):
    output.append([group_val, numeric_column[grouping_column==group_val].mean()])

output
# > [[b'Iris-setosa', 3.418],
# >  [b'Iris-versicolor', 2.770],
# >  [b'Iris-virginica', 2.974]]

#60. 如何将PIL图像转换为numpy数组？
from io import BytesIO
from PIL import Image
import PIL, requests

# Import image from URL
URL = 'https://upload.wikimedia.org/wikipedia/commons/8/8b/Denali_Mt_McKinley.jpg'
response = requests.get(URL)

# Read it as Image
I = Image.open(BytesIO(response.content))

# Optionally resize
I = I.resize([150,150])

# Convert to numpy array
arr = np.asarray(I)

# Optionaly Convert it back to an image and show
im = PIL.Image.fromarray(np.uint8(arr))
Image.Image.show(im)

#61. 如何删除numpy数组中所有缺少的值？
a = np.array([1,2,3,np.nan,5,6,7,np.nan])
a[~np.isnan(a)]
# > array([ 1.,  2.,  3.,  5.,  6.,  7.])

#62. 如何计算两个数组之间的欧氏距离？
# Input
a = np.array([1,2,3,4,5])
b = np.array([4,5,6,7,8])

# Solution
dist = np.linalg.norm(a-b)
dist
# > 6.7082039324993694

#63. 如何在一维数组中找到所有的局部极大值(或峰值)？
a = np.array([1, 3, 7, 1, 2, 6, 0, 1])
doublediff = np.diff(np.sign(np.diff(a)))
peak_locations = np.where(doublediff == -2)[0] + 1
peak_locations
# > array([2, 5])

#64. 如何从二维数组中减去一维数组，其中一维数组的每一项从各自的行中减去？
# Input
a_2d = np.array([[3,3,3],[4,4,4],[5,5,5]])
b_1d = np.array([1,2,3])

# Solution
print(a_2d - b_1d[:,None])
# > [[2 2 2]
# >  [2 2 2]
# >  [2 2 2]]

#65. 如何查找数组中项的第n次重复索引？
x = np.array([1, 2, 1, 1, 3, 4, 3, 1, 1, 2, 1, 1, 2])
n = 5

# Solution 1: List comprehension
[i for i, v in enumerate(x) if v == 1][n-1]

# Solution 2: Numpy version
np.where(x == 1)[0][n-1]
# > 8

#66. 如何将numpy的datetime 64对象转换为datetime的datetime对象？
# **给定：** a numpy datetime64 object
dt64 = np.datetime64('2018-02-25 22:10:10')

# Solution
from datetime import datetime
dt64.tolist()

# or

dt64.astype(datetime)
# > datetime.datetime(2018, 2, 25, 22, 10, 10)

#67. 如何计算numpy数组的移动平均值？
# Solution
# Source: https://stackoverflow.com/questions/14313510/how-to-calculate-moving-average-using-numpy
def moving_average(a, n=3) :
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n

np.random.seed(100)
Z = np.random.randint(10, size=10)
print('array: ', Z)
# Method 1
moving_average(Z, n=3).round(2)

# Method 2:  # Thanks AlanLRH!
# np.ones(3)/3 gives equal weights. Use np.ones(4)/4 for window size 4.
np.convolve(Z, np.ones(3)/3, mode='valid')


# > array:  [8 8 3 7 7 0 4 2 5 2]
# > moving average:  [ 6.33  6.    5.67  4.67  3.67  2.    3.67  3.  ]

#68. 如何在给定起始点、长度和步骤的情况下创建一个numpy数组序列？
length = 10
start = 5
step = 3

def seq(start, length, step):
    end = start + (step*length)
    return np.arange(start, end, step)

seq(start, length, step)
# > array([ 5,  8, 11, 14, 17, 20, 23, 26, 29, 32])

#69. 如何填写不规则系列的numpy日期中的缺失日期？
# Input
dates = np.arange(np.datetime64('2018-02-01'), np.datetime64('2018-02-25'), 2)
print(dates)

# Solution ---------------
filled_in = np.array([np.arange(date, (date+d)) for date, d in zip(dates, np.diff(dates))]).reshape(-1)

# add the last day
output = np.hstack([filled_in, dates[-1]])
output

# For loop version -------
out = []
for date, d in zip(dates, np.diff(dates)):
    out.append(np.arange(date, (date+d)))

filled_in = np.array(out).reshape(-1)

# add the last day
output = np.hstack([filled_in, dates[-1]])
output
# > ['2018-02-01' '2018-02-03' '2018-02-05' '2018-02-07' '2018-02-09'
# >  '2018-02-11' '2018-02-13' '2018-02-15' '2018-02-17' '2018-02-19'
# >  '2018-02-21' '2018-02-23']

# > array(['2018-02-01', '2018-02-02', '2018-02-03', '2018-02-04',
# >        '2018-02-05', '2018-02-06', '2018-02-07', '2018-02-08',
# >        '2018-02-09', '2018-02-10', '2018-02-11', '2018-02-12',
# >        '2018-02-13', '2018-02-14', '2018-02-15', '2018-02-16',
# >        '2018-02-17', '2018-02-18', '2018-02-19', '2018-02-20',
# >        '2018-02-21', '2018-02-22', '2018-02-23'], dtype='datetime64[D]')

#70. 如何从给定的一维数组创建步长？
def gen_strides(a, stride_len=5, window_len=5):
    n_strides = ((a.size-window_len)//stride_len) + 1
    # return np.array([a[s:(s+window_len)] for s in np.arange(0, a.size, stride_len)[:n_strides]])
    return np.array([a[s:(s+window_len)] for s in np.arange(0, n_strides*stride_len, stride_len)])

print(gen_strides(np.arange(15), stride_len=2, window_len=4))
# > [[ 0  1  2  3]
# >  [ 2  3  4  5]
# >  [ 4  5  6  7]
# >  [ 6  7  8  9]
# >  [ 8  9 10 11]
# >  [10 11 12 13]]

#
#https://www.numpy.org.cn/article/advanced/index.html
#




