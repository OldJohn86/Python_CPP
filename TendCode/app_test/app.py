import requests
from lxml import etree
from threading import Thread, Lock
from queue import Queue
import re
import csv
import time


class Productor(Thread):
    def __init__(self, q, w):
        Thread.__init__(self)
        self.q = q
        self.w = w
        self.s = requests.Session()
        # 这个地方很重要，不设置这个请求状态的话后续请求会报错
        self.s.keep_alive = False
        self.headers = {
            'user-agent': 'Mozilla/5.0 (Windows NT 5.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2634.33 Safari/537.36',
        }

    def run(self):
        while not self.q.empty():
            key = self.q.get()
            url = key[0]
            if key[1] < 3:
                try:
                    req = self.s.get(url, headers=self.headers, timeout=2)
                    time.sleep(0.1)
                except Exception as e:
                    self.q.put((url, key[1] + 1))
                    print(url, e)
                else:
                    # 设置网站的编码格式为utf-8
                    req.encoding = 'utf-8'
                    self.get_info(req.text)
            else:
                # 每个链接最多请求3次，如果3次还失败就放弃请求，打印链接
                # 不这样设置的话，如果遇到有的链接请求一直超时就造成了死循环
                print(key)
        print('{} 线程结束'.format(self.getName()))

    def get_info(self, html):
        tree = etree.HTML(html)
        sites = tree.xpath('//ul[@class="listCentent"]/li')
        for site in sites:
            info = dict()
            name = site.xpath('.//h3[@class="rightTxtHead"]/a/text()')
            # 有一个网站没有名字，所以要判断查找是否为空
            name = name[0] if name else ''
            url = site.xpath('.//h3[@class="rightTxtHead"]/span/text()')[0]
            alexa = site.xpath('.//p[@class="RtCData"][1]/a/text()')[0]
            baidu_s = site.xpath('.//p[@class="RtCData"][2]/a/img/@src')[0]
            baidu_pr = site.xpath('.//p[@class="RtCData"][3]/a/img/@src')[0]
            baidu_fl = site.xpath('.//p[@class="RtCData"][4]/a/text()')[0]
            rank = site.xpath('.//div[@class="RtCRateCent"]/strong/text()')[0]
            score = site.xpath('.//div[@class="RtCRateCent"]/span/text()')[0]
            # 名称里面有的有特殊字符，所以可以过滤掉特殊的字符
            info['name'] = name.encode('gbk', 'ignore').decode('gbk')
            info['url'] = url
            info['alexa'] = alexa
            info['baidu_s'] = self.get_s(baidu_s)
            info['baidu_pr'] = self.get_s(baidu_pr)
            info['baidu_fl'] = baidu_fl
            info['rank'] = rank
            info['score'] = score.replace('得分:', '')
            self.w.put(info)

    def get_s(self, url):
        '''从百度权重或PR的图片中提取权重值'''
        s = re.findall(r'(\d+)\.gif', url)[0]
        return s


class Worker(Thread):
    def __init__(self, w, file, l):
        Thread.__init__(self)
        self.w = w
        self.file = file
        self.l = l

    def run(self):
        while True:
            info = self.w.get()
            if info is None:
                break
            try:
                # 因为共享资源，所以要加锁
                self.l.acquire()
                self.writeinfo(info)
                self.l.release()
            except Exception as e:
                print(info, e)
            self.w.task_done()
        print('{} worker is done!'.format(self.getName()))

    def writeinfo(self, data):
        headers = ['name', 'url', 'alexa', 'baidu_s', 'baidu_pr', 'baidu_fl', 'rank', 'score']
        with open(self.file, 'a', newline='', encoding='gbk') as f:
            writer = csv.DictWriter(f, fieldnames=headers)
            writer.writerow(data)


def get_csv(filename):
    '''创建一个新的csv表格，并且设置标题'''
    headers = ['name', 'url', 'alexa', 'baidu_s', 'baidu_pr', 'baidu_fl', 'rank', 'score']
    with open(filename, 'w', newline='', encoding='gbk') as f:
        writer = csv.DictWriter(f, fieldnames=headers)
        writer.writeheader()


def main(file):
    get_csv(file)
    l = Lock()
    q = Queue()
    work = Queue()
    # 插入的信息是链接和一个基础的请求次数0构成的元组，为了后续判断链接请求了几次
    q.put(('http://top.chinaz.com/all/index.html', 0))
    baseurl = 'http://top.chinaz.com/all/index_{}.html'
    for i in range(2, 1892):
        q.put((baseurl.format(i), 0))
    pl = [Productor(q, work) for i in range(10)]
    for each in pl:
        each.start()
    wl = [Worker(work, file, l) for i in range(5)]
    for each in wl:
        each.start()
    for each in pl:
        each.join()
    work.join()
    for each in wl:
        work.put(None)
    for each in wl:
        each.join()
    print('game over!')


if __name__ == '__main__':
    main('info.csv')

