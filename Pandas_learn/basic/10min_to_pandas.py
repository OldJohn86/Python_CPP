#--*-- coding: utf-8 --*--

"""
Created on 2019-05-06
@author: John Chen
"""


import numpy as np
import pandas as pd

s = pd.Series([1, 3, 5, np.nan, 6, 8])
print(s)

dates = pd.date_range('20190101', periods=6)
print(dates)


