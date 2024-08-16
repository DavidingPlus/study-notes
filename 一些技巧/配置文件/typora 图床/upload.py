# -*- coding: UTF-8 -*-
'''
@文件        :csdnUpload.py
@时间        :2022/11/24 12:48:46
@作者        :tx
@版本        :2.0
@说明        :Typora利用csdn作为图床
'''
from __future__ import print_function
import random
from io import BytesIO
import sys
from PIL import Image
import os
from threading import Thread
import psutil
import re
import requests
import http.cookiejar as cookielib
import time
import requests
from requests_toolbelt import MultipartEncoder
import ctypes

os.chdir(os.path.expanduser('~'))  # 这是打包版本的代码
requests.packages.urllib3.disable_warnings()
session = requests.session()


class show_code(Thread):
    def __init__(self, url):
        Thread.__init__(self)
        self.url = url

    def run(self):
        response = requests.get(self.url)
        img = Image.open(BytesIO(response.content))  # 打开图片，返回PIL image对象
        img.show()


def IsLogin():
    '''
    文件读取cookie，检测cookie是否合法。
    '''
    try:
        session.cookies = cookielib.LWPCookieJar(filename=".cookie/csdn.txt")
        session.cookies.load()

        # 修改地方
        url = "https://imgservice.csdn.net/direct/v1.0/image/upload?type=blog&rtype=markdown&x-image-template=&x-image-app=direct_blog&x-image-dir=direct&x-image-suffix=png"
        response = session.get(url)
        if response.json()['msg'] == "success":
            return True
        else:
            return False
    except Exception as e:
        return False


def Login():
    '''
    csdn自动登录，同时保存cookie
    '''
    response = session.get('https://open.weixin.qq.com/connect/qrconnect?appid=wx0ae11b6a28b4b9fc&scope=snsapi_login&redirect_uri=https%3A%2F%2Fpassport.csdn.net%2Fv1%2Fregister%2FpcAuthCallBack%3FpcAuthType%3Dweixin&state=csdn&login_type=jssdk&self_redirect=default&style=white&href=https://csdnimg.cn/release/passport/history/css/replace-wx-style.css', verify=False)
    uuid = re.findall(
        '<img class="qrcode lightBorder js_qrcode_img" src="(.*?)"', response.text)[0]
    img_url = 'https://open.weixin.qq.com' + uuid
    t = show_code(img_url)
    t.start()
    t.join()

    uuid = uuid.split('/')[-1]
    url = 'https://lp.open.weixin.qq.com/connect/l/qrconnect?uuid='+uuid
    while True:
        response = session.get(url, verify=False)
        code = re.findall("window.wx_code='(.*?)'", response.text)
        if code != ['']:
            for proc in psutil.process_iter():  # 遍历当前process
                try:
                    if proc.name() == "Microsoft.Photos.exe":
                        proc.kill()  # 关闭该process
                except Exception as e:
                    pass
            break
        time.sleep(0.5)

    url = 'https://passport.csdn.net/v1/register/pcAuthCallBack?pcAuthType=weixin&code=%s&state=csdn' % code[0]
    session.get(url)
    session.cookies.save()
    IsLogin()


def UploadPic(picList: list):
    '''
    图片上传
    '''

    for pic in picList:
        # 获得图片授权
        url = "https://imgservice.csdn.net/direct/v1.0/image/upload?type=blog&rtype=markdown&x-image-template=&x-image-app=direct_blog&x-image-dir=direct&x-image-suffix=png"
        response = session.get(url)
        key = response.json()['data']
        upload_url = key['host']
        fields = {
            'key': key['filePath'],
            'policy': key['policy'],
            "OSSAccessKeyId": key['accessId'],
            "success_action_status": "200",
            "signature": key['signature'],
            "callback": key['callbackUrl'],
            'file': (os.path.basename(pic), open(pic, 'rb'), "image/png"),
        }
        multipart_encoder = MultipartEncoder(
            fields, boundary='-----------------------------' + str(random.randint(1e28, 1e29 - 1)))
        headers = {
            'content-Type': multipart_encoder.content_type,
        }
        res = requests.post(upload_url, headers=headers,
                            data=multipart_encoder, verify=False)
        if (res.status_code == 200):
            print(res.json()['data']['imageUrl'])


if __name__ == "__main__":

    # session.proxies = {"http": "http://127.0.0.1:8080",
    #                    "https": "http://127.0.0.1:8080"}
    if not os.path.exists(".cookie"):
        os.mkdir(".cookie")
    if not os.path.exists(os.path.join(".cookie", "csdn.txt")):
        tmep_f = open(os.path.join(".cookie", "csdn.txt"), 'w+')
        tmep_f.close()
    if not IsLogin():
        Login()
    UploadPic(sys.argv[1:])
