import requests
import json
import time

# https://socialsisteryi.github.io/bilibili-API-collect/docs/login/login_action/QR.html#web%E7%AB%AF%E6%89%AB%E7%A0%81%E7%99%BB%E5%BD%95

# curl -G 'https://api.bilibili.com/x/v3/fav/resource/list' --data-urlencode 'media_id=1052622027' --data-urlencode 'platform=web' --data-urlencode 'pn=1' --data-urlencode 'ps=5' -b 'SESSDATA=7c808ecc%2C1699270373%2Cd1fce%2A51'

URL='https://api.bilibili.com/x/v3/fav/resource/list'
PARA={'platform': 'web', 'media_id': '767919190', 'ps': 10, 'pn': 0}
CK={'SESSDATA': '7c808ecc%2C1699270373%2Cd1fce%2A51'} 
medias = []
data = json.loads(requests.get(URL,cookies=CK,params=PARA).text)
while data['data']['has_more'] == True :
    PARA['pn'] += 1
    data = json.loads(requests.get(URL,cookies=CK,params=PARA).text)
    medias += data['data']['medias']

file = open('zv.md', 'w')
file.write('---\ntitle: 中V收藏夹\ndate: {}\n---\n由B站API生成\n\n|序号|名称|链接|\n|:-:|:-:|:-:|\n'.format(time.strftime("%Y/%m/%d", time.localtime())))

i = 1
for media in medias:
    response = requests.get(media['cover'])
    if response.status_code == 200:
        try:
            f = open('zv/'+str(media['id'])+'.jpg', 'xb')
            f.write(response.content)
            f.close()
        except FileExistsError:
            pass
    file.write('|{}|{}|[![]({})](https://www.bilibili.com/video/{})|\n'.format(i, media['title'].replace('|', '\\|'), 'zv/'+str(media['id'])+'.jpg', media['bvid']))
    i += 1

file.close()

