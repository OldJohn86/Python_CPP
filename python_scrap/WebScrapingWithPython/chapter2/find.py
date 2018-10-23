from urllib.request import urlopen
from bs4 import BeautifulSoup

html = urlopen("http://www.pythonscraping.com/pages/page3.html")
bsObj = BeautifulSoup(html, features="lxml")

#for child in bsObj.find("table", {"id":"giftList"}).children:
#for child in bsObj.find("table", {"id":"giftList"}).descendants:
#print(child)

#for sibling in bsObj.find("table", {"id":"giftList"}).tr.next_siblings:
#print(sibling)
#for sibling in bsObj.find("table", {"id":"giftList"}).tr.previous_siblings:
#print(sibling)

print(bsObj.find("img", {"src":"../img/gifts/img1.jpg"}).parent.previous_sibling.get_text())
print(bsObj.find("img", {"src":"../img/gifts/img2.jpg"}).parent.previous_sibling.get_text())
