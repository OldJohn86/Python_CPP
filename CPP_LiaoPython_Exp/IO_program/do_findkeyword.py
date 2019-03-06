import os

def find_keyword(keyword, path='.'):
	for d in os.listdir(path):
		full_path = os.path.join(path, d)
		if os.path.isdir(full_path):
			find_keyword(keyword, path=full_path)
		elif os.path.isfile(full_path) and keyword in d:
			print(full_path)
	return

find_keyword('IO', '.')

