from collections import Iterable
print(isinstance([], Iterable))
print(isinstance({}, Iterable))
print(isinstance('abc', Iterable))
print(isinstance((x for x in range(10)), Iterable))
print(isinstance(100, Iterable))


from collections import Iterator
print(isinstance((x for x in range(10)), Iterator))
print(isinstance([], Iterator))
print(isinstance({}, Iterator))
print(isinstance('abc', Iterator))


print(isinstance(iter([]), Iterator))
print(isinstance(iter({}), Iterator))
print(isinstance(iter('abc'), Iterator))


for x in [1, 2, 3, 4, 5]:
	pass

it = iter([1, 2, 3, 4, 5])
while True:
	try:
		x = next(it)
		print(x)
	except StopIteration:
		break




