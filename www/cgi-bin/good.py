#!/usr/bin/env python3

import sys

print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n")

print("<html><head><title>Webserv</title><style>.glass-panel{    color: #fff;  margin: 40px auto;    background-color: rgba(255,255,255,0.06); border: 1px solid rgba(255,255,255,0.1);  width: 100%;  border-radius: 15px;  padding: 32px;    backdrop-filter: blur(10px);}.glass-button-user {   display: inline-block;    padding: 24px 32px;   border: 0;    position: absolute;   top: 50px;    right: 30px;  text-decoration: none;    border-radius: 15px;  background-color: rgba(255,255,255,0.1);  border: 1px solid rgba(255,255,255,0.1);  backdrop-filter: blur(30px);  color: rgba(255,255,255,0.8); font-size: 14px;  letter-spacing: 2px;  cursor: pointer;  text-transform: uppercase;    transition: 1s;}.glass-button-default { display: inline-block;    padding: 24px 32px;   border: 0;    text-decoration: none;    border-radius: 15px;  background-color: rgba(255,255,255,0.1);  border: 1px solid rgba(255,255,255,0.1);  backdrop-filter: blur(30px);  color: rgba(255,255,255,0.8); font-size: 14px;  letter-spacing: 2px;  cursor: pointer;  text-transform: uppercase;    transition: 1s;}.glass-button-default:hover {   transform: scale(1.1);    background-color: rgba(255,255,255,0.3);}.glass-button-user:hover { transform: scale(1.1);    background-color: rgba(255,255,255,0.3);}.glass-button-return { display: inline-block;    padding: 24px 32px;   border: 0;    position: absolute;   top: 150px;   right: 30px;  text-decoration: none;    border-radius: 15px;  background-color: rgba(255,255,255,0.1);  border: 1px solid rgba(255,255,255,0.1);  backdrop-filter: blur(30px);  color: rgba(255,255,255,0.8); font-size: 14px;  letter-spacing: 2px;  cursor: pointer;  text-transform: uppercase;    transition: 1s;}.glass-button-return:hover {    transform: scale(1.1);    background-color: rgba(255,255,255,0.3);}@media (max-width: 800px) {    .glass-button-user {        transition: 0s;       margin: 20%;}   .glass-button-return {      transition: 0s;       margin: 20%;}}.glass-button-folder {  display: inline-block;    padding: 24px 40px;   margin: 13px; border: 0;    text-decoration: none;    border-radius: 15px;  background-color: rgba(255,255,255,0.3);  border: 1px solid rgba(255,255,255,0.3);  backdrop-filter: blur(30px);  color: rgba(255,255,255,0.8); font-size: 18px;  letter-spacing: 2px;  cursor: pointer;  text-transform: uppercase;    transition: 1s;}.glass-button-folder:hover {    transform: scale(1.1);    background-color: rgba(255,255,255,0.4);}.glass-button {    display: inline-block;    padding: 24px 32px;   border: 0;    text-decoration: none;    border-radius: 15px;  background-color: rgba(255,255,255,0.1);  border: 1px solid rgba(255,255,255,0.1);  backdrop-filter: blur(30px);  color: rgba(255,255,255,0.8); font-size: 14px;  letter-spacing: 2px;  cursor: pointer;  text-transform: uppercase;    transition: 1s;}.glass-button:hover {   transform: scale(1.1);    background-color: rgba(255,255,255,0.3);}html, body {   margin: 0;    height: 100%;}body {    background: linear-gradient(45deg, #ee7752, #e73c7e, #23a6d5, #23d5ab);   background-size: 400% 400%;   animation: gradient 10s ease infinite;}@keyframes gradient {    0% {    background-position: 0% 50%;}   50% {       background-position: 100% 50%;} 100% {      background-position: 0% 50%;}}.glass-panel {  max-width: 600px;}.glass-button {   margin: 15px; margin-top: 40px;}h1, h1 a {    min-height: 120px;    width: 90%;   max-width: 700px; vertical-align: middle;   text-align: center;   margin: 0 auto;   text-decoration: none;    color: #fff;  padding-top: 60px;    color: rgba(255,255,255,0.8);}p {   width: 80%;   margin: 0 auto;   padding-bottom: 32px; color: rgba(255,255,255,0.6);}ul {  list-style-type: none;}</style></head><body><h1>WEBSERV</h1><a href =\"http://localhost:8080/html?disconnect=true\" target=\"_self\" class=\"glass-button-user\"> Disconnect", end = " ")

print(sys.argv[1], "</a>")

print("<a href =\"http://localhost:8080/user\" target=\"_self\" class=\"glass-button-return\"> Return</a>\n<div class=\"glass-panel\"><div class=\"glass-toolbar\">")

print("<button class=\"glass-button-folder\">.</button>")
print("<button class=\"glass-button-folder\">..</button>")

print("</div><div class=\"glass-toolbar\">")

n = len(sys.argv)

for i in range(2, n):
    print("<a href=\"" + sys.argv[1] + "/" + sys.argv[i] + "\"", "download=\"" + sys.argv[i] + "\"", "class=\"glass-button\">" + sys.argv[i] + "</a>")

print("</div></div></body></html>")
