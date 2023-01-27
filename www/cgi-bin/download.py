#!/usr/bin/env python3

import sys

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print();

print("<html><head><title>Webserv</title><style>\n.glass-panel\n{\n    color: #fff;\n  margin: 40px auto;\n    background-color: rgba(255,255,255,0.06);\n border: 1px solid rgba(255,255,255,0.1);\n  width: 100%;\n  border-radius: 15px;\n  padding: 32px;\n    backdrop-filter: blur(10px);\n}\n.glass-button-user \n{\n   display: inline-block;\n    padding: 24px 32px;\n   border: 0;\n    position: absolute;\n   top: 50px;\n    right: 30px;\n  text-decoration: none;\n    border-radius: 15px;\n  background-color: rgba(255,255,255,0.1);\n  border: 1px solid rgba(255,255,255,0.1);\n  backdrop-filter: blur(30px);\n  color: rgba(255,255,255,0.8);\n font-size: 14px;\n  letter-spacing: 2px;\n  cursor: pointer;\n  text-transform: uppercase;\n    transition: 1s;\n}\n.glass-button-default \n{\n display: inline-block;\n    padding: 24px 32px;\n   border: 0;\n    text-decoration: none;\n    border-radius: 15px;\n  background-color: rgba(255,255,255,0.1);\n  border: 1px solid rgba(255,255,255,0.1);\n  backdrop-filter: blur(30px);\n  color: rgba(255,255,255,0.8);\n font-size: 14px;\n  letter-spacing: 2px;\n  cursor: pointer;\n  text-transform: uppercase;\n    transition: 1s;\n}\n.glass-button-default:hover \n{\n   transform: scale(1.1);\n    background-color: rgba(255,255,255,0.3);\n}\n.glass-button-user:hover \n{\n transform: scale(1.1);\n    background-color: rgba(255,255,255,0.3);\n}\n.glass-button-return \n{\n display: inline-block;\n    padding: 24px 32px;\n   border: 0;\n    position: absolute;\n   top: 150px;\n   right: 30px;\n  text-decoration: none;\n    border-radius: 15px;\n  background-color: rgba(255,255,255,0.1);\n  border: 1px solid rgba(255,255,255,0.1);\n  backdrop-filter: blur(30px);\n  color: rgba(255,255,255,0.8);\n font-size: 14px;\n  letter-spacing: 2px;\n  cursor: pointer;\n  text-transform: uppercase;\n    transition: 1s;\n}\n.glass-button-return:hover \n{\n    transform: scale(1.1);\n    background-color: rgba(255,255,255,0.3);\n}\n@media (max-width: 800px) \n{\n    .glass-button-user \n{\n        transition: 0s;\n       margin: 20%;\n}\n   .glass-button-return \n{\n      transition: 0s;\n       margin: 20%;\n}\n}\n.glass-button-folder \n{\n  display: inline-block;\n    padding: 24px 40px;\n   margin: 13px;\n border: 0;\n    text-decoration: none;\n    border-radius: 15px;\n  background-color: rgba(255,255,255,0.3);\n  border: 1px solid rgba(255,255,255,0.3);\n  backdrop-filter: blur(30px);\n  color: rgba(255,255,255,0.8);\n font-size: 18px;\n  letter-spacing: 2px;\n  cursor: pointer;\n  text-transform: uppercase;\n    transition: 1s;\n}\n.glass-button-folder:hover \n{\n    transform: scale(1.1);\n    background-color: rgba(255,255,255,0.4);\n}\n.glass-button \n{\n    display: inline-block;\n    padding: 24px 32px;\n   border: 0;\n    text-decoration: none;\n    border-radius: 15px;\n  background-color: rgba(255,255,255,0.1);\n  border: 1px solid rgba(255,255,255,0.1);\n  backdrop-filter: blur(30px);\n  color: rgba(255,255,255,0.8);\n font-size: 14px;\n  letter-spacing: 2px;\n  cursor: pointer;\n  text-transform: uppercase;\n    transition: 1s;\n}\n.glass-button:hover \n{\n   transform: scale(1.1);\n    background-color: rgba(255,255,255,0.3);\n}\nhtml, body \n{\n   margin: 0;\n    height: 100%;\n}\nbody \n{\n    background: linear-gradient(45deg, #ee7752, #e73c7e, #23a6d5, #23d5ab);\n   background-size: 400% 400%;\n   animation: gradient 10s ease infinite;\n}\n@keyframes gradient \n{\n    0% \n{\n    background-position: 0% 50%;\n}\n   50% \n{\n       background-position: 100% 50%;\n}\n 100% \n{\n      background-position: 0% 50%;\n}\n}\n.glass-panel \n{\n  max-width: 600px;\n}\n.glass-button \n{\n   margin: 15px;\n margin-top: 40px;\n}\nh1, h1 a \n{\n    min-height: 120px;\n    width: 90%;\n   max-width: 700px;\n vertical-align: middle;\n   text-align: center;\n   margin: 0 auto;\n   text-decoration: none;\n    color: #fff;\n  padding-top: 60px;\n    color: rgba(255,255,255,0.8);\n}\np \n{\n   width: 80%;\n   margin: 0 auto;\n   padding-bottom: 32px;\n color: rgba(255,255,255,0.6);\n}\nul \n{\n  list-style-type: none;\n}\n</style>\n</head>\n<body>\n<h1>WEBSERV</h1>\n<a href =\"http://localhost:8080/html?disconnect=true\" target=\"_self\" class=\"glass-button-user\"> Disconnect", end = " ")

print(sys.argv[1], "</a>")

print("<a href =\"http://localhost:8080/user\" target=\"_self\" class=\"glass-button-return\"> Return</a>\n<div class=\"glass-panel\"><div class=\"glass-toolbar\">")

print("<button class=\"glass-button-folder\">.</button>")
print("<button class=\"glass-button-folder\">..</button>")

print("</div><div class=\"glass-toolbar\">")

n = len(sys.argv)

for i in range(2, n):
    print("<a href=\"" + sys.argv[1] + "/" + sys.argv[i] + "\"", "download=\"" + sys.argv[i] + "\"", "class=\"glass-button\">" + sys.argv[i] + "</a>")

print("</div></div></body></html>")
