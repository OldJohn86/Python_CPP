def now():
	print('2015-3-25')

f = now
f()
print(now.__name__)
print(f.__name__)

def log(func):
	def wrapper(*args, **kw):
		print('call %s():' % func.__name__)
		return func(*args, **kw)
	return wrapper

@log
def now():
	print('2015-3-25')
now()

def log(text):
	def decorator(func):
		def wrapper(*args, **kw):
			print('%s %s():' % (text, func.__name__))
			return func(*args, **kw)
		return wrapper
	return decorator

@log('execute')
def now():
	print('2015-3-25')
now()
print(now.__name__)

import functools

def log(func):
	@functools.wraps(func)
	def wrapper(*args, **kw):
		print('call %s():' % func.__name__)
		return func(*args, **kw)
	return wrapper

import functools

def log(text):
	def decorator(func):
		@functools.wraps(func)
		def wrapper(*args, **kw):
			print('%s %s():' % (text, func.__name__))
			return func(*args, **kw)
		return wrapper
	return decorator


@log('execute')
def now():
	print('2015-3-25')
now()
print(now.__name__)

import time, functools

def metric(fn):
	@functools.wraps(fn)
	def wrapper(*args, **kw):
		s_time = time.time()
		func = fn(*args, **kw)
		exec_time = time.time() - s_time
		print('%s executed in %s ms' % (fn.__name__, exec_time))
		return func
	return wrapper


@metric
def fast(x, y):
    time.sleep(0.0012)
    return x + y

@metric
def slow(x, y, z):
    time.sleep(0.1234)
    return x * y * z

f = fast(11, 22)
s = slow(11, 22, 33)
if f != 33:
    print('测试失败!')
elif s != 7986:
    print('测试失败!')

