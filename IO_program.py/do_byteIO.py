from io import BytesIO

f = BytesIO()
f.write('中文'.encode('utf-8'))
print(f.getvalue())

from io import BytesIO
file = BytesIO(b'\xe4\xb8\xad\xe6\x96\x87')
file.read()

