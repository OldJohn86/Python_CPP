from urllib.request import urlopen
from bs4 import BeautifulSoup
import re
import pymysql

conn = pymysql.connect("localhost", "root", "1q2w3e4R", "wikipedia")

cur = conn.cursor()
cur.execute("USE wikipedia")

def insertPageIfNotExists(url):
    cur.execute("SELECT * FROM pages WHERE url = \"%s\"" % (url))
    if cur.rowcount == 0:
        cur.execute("INSERT INTO pages (url) VALUES (\"%s\")" % (url))
        conn.commit()
        return cur.lastrowid
    else:
        return cur.fetchone()[0]

def insertLink(fromPageId, toPageId):
    cur.execute("SELECT * FROM links WHERE fromPageId = \"%s\" AND toPageId = \"%s\"" % (int(fromPageId), int(toPageId)))
    if cur.rowcount == 0:
        cur.execute("INSERT INTO links (fromPageId, toPageId) VALUES (\"%s\", \"%s\")" % (int(fromPageId), int(toPageId)))
        conn.commit()

pages = set()
def getLinks(pageUrl, recursionLevel):
    global pages
    if recursionLevel > 4:
        return;
    pageId = insertPageIfNotExists(pageUrl)
    html = urlopen("http://en.wikipedia.org"+pageUrl)
    bsObj = BeautifulSoup(html, features='lxml')
    for link in bsObj.findAll("a", href=re.compile("^(/wiki/)((?!:).)*$")):
        insertLink(pageId, insertPageIfNotExists(link.attrs['href']))
        if link.attrs['href'] not in pages:
            #
            newPage = link.attrs['href']
            pages.add(newPage)
            getLinks(newPage, recursionLevel+1)

getLinks("/wiki/Kevin_Bacon", 0)
cur.close()
conn.close()
