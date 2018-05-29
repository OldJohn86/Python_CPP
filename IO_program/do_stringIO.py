from io import StringIO

f = StringIO()
f.write('hello')
f.write(' ')
f.write('world!')
print(f.getvalue())



from io import StringIO

file = StringIO('Hello!\nHi!\nGoodBye!')

while True:
	s = file.readline()
	if s == '':
		break
	print(s.strip())

