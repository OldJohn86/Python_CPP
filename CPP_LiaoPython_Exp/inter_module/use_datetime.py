import re

from datetime import datetime, timezone, timedelta
def to_timestamp(dt_str, tz_str):
	date = datetime.strptime(dt_str, '%Y-%m-%d %H:%M:%S')
	tz = tz_str.split('UTC')[1]
	tz = tz.split(':')[0]
	tz_utc = timezone(timedelta(hours=int(tz)))
	date = date.replace(tzinfo=tz_utc)
	return date.timestamp()

t1 = to_timestamp('2015-6-1 08:10:30', 'UTC+7:00')
assert t1 == 1433121030.0, t1

t2 = to_timestamp('2015-5-31 16:10:30', 'UTC-09:00')
assert t2 == 1433121030.0, t2

print('ok')




