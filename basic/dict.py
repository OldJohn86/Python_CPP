d = {'Michael': 95, 'Bob': 75, 'Tracy':85}
print(d)
print(d['Michael'])

d['Adam'] = 67
print(d['Adam'])

d['Jack'] = 90
print(d['Jack'])

d['Jack'] = 88
print(d['Jack'])

print('Thomas' in d)

d.get('Thomas')
d.get('Thomas', -1)

print(d)
d.pop('Bob')
print(d)

s = set([1, 2, 3])
print(s)

s = set([1, 2, 2, 3, 3, 4, 4])
print(s)

s.add(4)
print(s)
s.add(4)
print(s)

s.remove(4)
print(s)

s1 = set([1, 2, 3])
s2 = set([2, 3, 4])
print(s1 & s2)
print(s1 | s2)

a = ['c', 'b', 'a']
print(a)
a.sort()
print(a)

a = 'abc'
a.replace('a', 'A')
print(a)

a = 'abc'
b = a.replace('a', 'A')
print(b)

