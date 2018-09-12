import urllib
import urllib2

values = {"username":"cp_china@163.com","password":"Chen5372"}
data = urllib.urlencode(values)
url = "https://passport.csdn.net/account/login"
geturl = url + "?" + data
print geturl
request = urllib2.Request(geturl)
response = urllib2.urlopen(request)
print response.read()

