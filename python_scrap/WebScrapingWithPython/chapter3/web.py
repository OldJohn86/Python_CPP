from urllib.request import urlopen
from bs4 import BeautifulSoup
import re
import datetime
import random

pages = set()
random.seed(datetime.datetime.now())

#get all the Interlink List of pages
def getInternalLinks(bsObj, includeUrl):
    internalLinks = []
    # find out all links which starts with tag of "/"
    for link in bsObj.findAll("a", href=re.compile("^(/|.*"+includeUrl+")")):
        if link.attrs['href'] is not None:
            if link.attrs['href'] not in internalLinks:
                internalLinks.append(link.attrs['href'])
    return internalLinks

#get all the Exterlink List of pages
def getExternalLinks(bsObj, excludeUrl):
    externalLinks = []
    # find out all links which start with "http"/"www", but not include current URL
    for link in bsObj.findAll("a", href=re.compile("^(http|www)((?!"+excludeUrl+").)*$")):
        if link.attrs['href'] is not None:
            externalLinks.append(link.attrs['href'])
    return externalLinks

def splitAddress(address):
    addressParts = address.replace("http://", "").split("/")
    return addressParts

def getRandomExternalLink(startingPage):
    html = urlopen(startingPage)
    bsObj = BeautifulSoup(html, features="lxml")
    externalLinks = getExternalLinks(bsObj, splitAddress(startingPage)[0])
    if len(externalLinks) == 0:
        internalLinks = getInternalLinks(startingPage)
        return getNextExternalLink(internalLinks[random.randint(0, len(internalLinks)-1)])
    else:
        return externalLinks[random.randint(0, len(externalLinks)-1)]

target = "http://oreilly.com"
def followExternalOnly(startingSite):
    externalLink = getRandomExternalLink(target)
    print("Random Links is:"+ externalLink)
    followExternalOnly(externalLink)

#followExternalOnly("http://www.oreilly.com")
#followExternalOnly(target)

allExtLinks = set()
allIntLinks = set()
def getAllExternalLinks(siteUrl):
    html = urlopen(siteUrl)
    bsObj = BeautifulSoup(html, features="lxml")
    internalLinks = getInternalLinks(bsObj, splitAddress(siteUrl)[0])
    externalLinks = getExternalLinks(bsObj, splitAddress(siteUrl)[0])
    for link in externalLinks:
        if link not in allExtLinks:
            allExtLinks.add(link)
            print(link)
    for link in internalLinks:
        if link not in allIntLinks:
            print("Going to get link URL is :"+link)
            allIntLinks.add(link)
            getAllExternalLinks(link)

#getAllExternalLinks("http://oreilly.com")
getAllExternalLinks(target)
