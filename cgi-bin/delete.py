#!/usr/bin/env python3

import sys
import os

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

start = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Webserv</title><style>.glass-panel{\tcolor: #fff;\tmargin: 40px auto;\tbackground-color: rgba(255,255,255,0.06);\tborder: 1px solid rgba(255,255,255,0.1);\twidth: 100%;\tborder-radius: 15px;\tpadding: 32px;\tbackdrop-filter: blur(10px);}.glass-button-user {\tdisplay: inline-block;\tpadding: 24px 32px;\tborder: 0;\tposition: absolute;\ttop: 50px;\tright: 30px;\ttext-decoration: none;\tborder-radius: 15px;\tbackground-color: rgba(255,255,255,0.1);\tborder: 1px solid rgba(255,255,255,0.1);\tbackdrop-filter: blur(30px);\tcolor: rgba(255,255,255,0.8); font-size: 14px;\tletter-spacing: 2px;\tcursor: pointer;\ttext-transform: uppercase;\ttransition: 1s;}.glass-button-default { display: inline-block;\tpadding: 24px 32px;\t border: 0;\ttext-decoration: none;\tborder-radius: 15px;\tbackground-color: rgba(255,255,255,0.1);\tborder: 1px solid rgba(255,255,255,0.1);\tbackdrop-filter: blur(30px);\tcolor: rgba(255,255,255,0.8); font-size: 14px;\tletter-spacing: 2px;\tcursor: pointer;\ttext-transform: uppercase;\ttransition: 1s;}.glass-button-default:hover {\t transform: scale(1.1);\tbackground-color: rgba(255,255,255,0.3);}.glass-button-user:hover {\ttransform: scale(1.1);\tbackground-color: rgba(255,255,255,0.3);}.glass-button-return {\tdisplay: inline-block;\tpadding: 24px 32px;\tborder: 0;\tposition: absolute;\t top: 150px;\t right: 30px;\ttext-decoration: none;\tborder-radius: 15px;\tbackground-color: rgba(255,255,255,0.1);\tborder: 1px solid rgba(255,255,255,0.1);\tbackdrop-filter: blur(30px);\tcolor: rgba(255,255,255,0.8);\tfont-size: 14px;\tletter-spacing: 2px;\tcursor: pointer;\ttext-transform: uppercase;\ttransition: 1s;}.glass-button-return:hover {\ttransform: scale(1.1);\tbackground-color: rgba(255,255,255,0.3);}@media (max-width: 800px) {\t.glass-button-user {\ttransition: 0s;\tmargin: 20%;}\t.glass-button-return {\ttransition: 0s;\t margin: 20%;}}.glass-button-folder {\tdisplay: inline-block;\tpadding: 24px 40px;\tmargin: 13px; border: 0;\ttext-decoration: none;\tborder-radius: 15px;\tbackground-color: rgba(255,255,255,0.3);\tborder: 1px solid rgba(255,255,255,0.3);\tbackdrop-filter: blur(30px);\tcolor: rgba(255,255,255,0.8);\tfont-size: 18px;\tletter-spacing: 2px;\tcursor: pointer;\ttext-transform: uppercase;\ttransition: 1s;}.glass-button-folder:hover {\ttransform: scale(1.1);\tbackground-color: rgba(255,255,255,0.4);}.glass-button {\tdisplay: inline-block;\tpadding: 24px 32px;\tborder: 0;\ttext-decoration: none;\tborder-radius: 15px;\tbackground-color: rgba(255,255,255,0.1);\tborder: 1px solid rgba(255,255,255,0.1);\tbackdrop-filter: blur(30px);\tcolor: rgba(255,255,255,0.8);\tfont-size: 14px;\tletter-spacing: 2px;\tcursor: pointer;\ttext-transform: uppercase;\t\ttransition: 1s;}.glass-button:hover {\ttransform: scale(1.1);\tbackground-color: rgba(255,255,255,0.3);}html, body {\t margin: 0;\theight: 100%;}body {\tbackground: linear-gradient(45deg, #ee7752, #e73c7e, #23a6d5, #23d5ab);\tbackground-size: 400% 400%;\tanimation: gradient 10s ease infinite;}@keyframes gradient {\t0%{\tbackground-position: 0% 50%;}\t50% {\tbackground-position: 100% 50%;} 100% {\tbackground-position: 0% 50%;}}.glass-panel {\tmax-width: 600px;}.glass-button {\tmargin: 15px;\tmargin-top: 40px;}h1, h1 a {\tmin-height: 120px;\twidth: 90%;\tmax-width: 700px; vertical-align: middle;\ttext-align: center;\tmargin: 0 auto;\ttext-decoration: none;\tcolor: #fff;\tpadding-top: 60px;\tcolor: rgba(255,255,255,0.8);}p {\twidth: 80%;\tmargin: 0 auto;\tpadding-bottom: 32px; color: rgba(255,255,255,0.6);}ul {\tlist-style-type: none;}</style></head><body><h1>WEBSERV</h1><a href =\"http://localhost:8080/html?disconnect=true\" target=\"_self\" class=\"glass-button-user\"> Disconnect "

user = str(os.environ.get('REMOTE_USER'))
server = str(os.environ.get('SERVER_NAME'))
port = str(os.environ.get('SERVER_PORT'))

return_button = "<a href = \"http://" + server + ":" + port + "/cgi-bin/get.sh/user\" target=\"_self\" class=\"glass-button-return\"> Return</a>\n"

toolbar = "<div class=\"glass-panel\"><div class=\"glass-toolbar\">\n"

folder_back = "<a href =\"" + str(os.environ.get('HTTP_REFERER')) + "\" target=\"_self\" class=\"glass-button-return\"> .. </a>\n"


path = "/tmp/private_webserv/" + server + "/" + user

exist = os.path.exists(path)

x = start + user + "</a>" + return_button + toolbar + folder_back
if exist == 1:
    listing = os.listdir(path)
    path += "/"
    y = ""
    for val in listing:
        isdir = os.path.isdir(path + val)
        if isdir == 1:
            x += "<a href=\"http://" + server + ":" + port + "/cgi-bin/delete.py" + path + val + "\" target=\"_self\" class=\"glass-button-folder\">" + val + "</a>\n"
        else:
            y += "<div class=\"glass-button\">" + "http://" + server + ":" + port + path + val + "</div>\n"
    x += "</div><div class=\"glass-toolbar\">" + y + "</div></div></body></html>"
else:
    x += "</div></div></body></html>"

print("Content-Length:", len(x))
print()

print(x)