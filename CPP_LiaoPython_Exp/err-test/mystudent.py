#import unittest

class Student(object):

	def __init__(self, name, score):
		self.name  = name
		self.score = score
	def get_grade(self):
		if self.score > 100 or self.score < 0:
			raise ValueError('score must between 0 and 100!')
		if 80 > self.score >= 60:
			return 'B'
		if 101 > self.score >= 80:
			return 'A'
		return 'C'
