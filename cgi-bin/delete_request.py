import requests
import sys
import os

user = str(os.environ.get('REMOTE_USER'))
server = str(os.environ.get('SERVER_NAME'))
port = str(os.environ.get('SERVER_PORT'))
path = str(os.environ.get('PATH_TRANSLATED'))

link="http://" + server + ":" + port + path

#x = requests.delete(link)
x = requests.delete('http://localhost:8080/tmp/private_webserv/Anis/mdr.sh')
print(x.text)