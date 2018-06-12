# -*- coding:utf-8 -*-
#使用aiohttp实现的简单的异步IO服务器
#添加main路由以及修改content_type='text/html'

import asyncio
from aiohttp import web

async def index(request):
    await asyncio.sleep(1)
    return web.Response(body='<h1>Index</h1>',content_type='text/html')

async def hello(request):
    await asyncio.sleep(0.5)
    text = '<h1>hello, %s!</h1>' % request.match_info['name']
    print(text)
    return web.Response(body=text.encode('utf-8'),content_type='text/html')

async def mainresponse(request):
    text = '<h1>hello,main</h>'
    print(text)
    return web.Response(body=text.encode('utf-8'),content_type='text/html')

async def init(loop):
    app = web.Application(loop=loop)
    app.router.add_route('GET', '/', index)
    app.router.add_route('GET', '/hello/{name}', hello)
    app.router.add_route('GET', '/main', mainresponse)
    srv = await loop.create_server(app.make_handler(), '127.0.0.1', 8000)
    print('Server started at http://127.0.0.1:8000...')
    return srv

loop = asyncio.get_event_loop()
loop.run_until_complete(init(loop))
loop.run_forever()
