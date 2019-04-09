#-*- coding:utf-8 -*-
import numpy as np

A = np.array([[1,-1,2],[3,2,0]])
print(A)

v = np.array([[2],[1],[3]])
print(v)

v = np.transpose(np.array([[2,1,3]]))
print(v)

print(A[1,2])
col = A[:,1:2]
print(col)

w = np.dot(A,v)
print(w)
#####################################################################
A = np.array([[2,1,-2],[3,0,1],[1,1,-1]])
b = np.transpose(np.array([[-3,5,-2]]))
x = np.linalg.solve(A,b)
print(x)

####################################################################
import csv
import numpy as np

def readData():
    X = []
    y = []
    with open('Housing.csv') as f:
        rdr = csv.reader(f)
        # Skip the header row
        next(rdr)
        # Read X and y
        for line in rdr:
            xline = [1.0]
            for s in line[:-1]:
                xline.append(float(s))
            X.append(xline)
            y.append(float(line[-1]))
    return (X,y)

X0,y0 = readData()
# Convert all but the last 10 rows of the raw data to numpy arrays
d = len(X0)-10
X = np.array(X0[:d])
y = np.transpose(np.array([y0[:d]]))

# Compute beta
Xt = np.transpose(X)
XtX = np.dot(Xt,X)
Xty = np.dot(Xt,y)
beta = np.linalg.solve(XtX,Xty)
print(beta)

# Make predictions for the last 10 rows in the data set
for data,actual in zip(X0[d:],y0[d:]):
    x = np.array([data])
    prediction = np.dot(x,beta)
    print('prediction = '+str(prediction[0,0])+' actual = '+str(actual))


#####################################################################
#
#
# https://www.numpy.org.cn/article/basics/python_numpy_tutorial.html
#