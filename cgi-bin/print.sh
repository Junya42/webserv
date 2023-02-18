#!/bin/bash
filename=$QUERY_STRING

#	Check if file is a directory
if [ -d "$filename" ]
then
	#echo "400" >/dev/stderr
	exit 42
fi

# Check if file exist
if [ ! -f "$filename" ]
then
	#echo "404" >/dev/stderr
	exit 42
fi
html_start="Content-Type: text/html\n\n"
html_start+="
   <html>
	<head>
		<title>Webserv</title>
		<link rel=\"icon\" type=\"image/x-icon\" href=\"/favicon.ico\">
		<style>
.glass-panel {
	color: #fff;
	margin: 40px auto;
	background-color: rgba(255,255,255,0.06);
	border: 1px solid rgba(255,255,255,0.1);
	width: 100%;
	border-radius: 15px;
	padding: 32px;
	backdrop-filter: blur(10px);
}
.glass-button {
	display: inline-block;
	padding: 24px 32px;
	border: 0;
	text-decoration: none;
	border-radius: 15px;
	background-color: rgba(255,255,255,0.1);
	border: 1px solid rgba(255,255,255,0.1);
	backdrop-filter: blur(30px);
	color: rgba(255,255,255,0.8);
	font-size: 14px;
	letter-spacing: 2px;
	cursor: pointer;
	text-transform: uppercase;
	transition: 1s;
}
.glass-button:hover {
	transform: scale(1.1);
	background-color: rgba(255,255,255,0.3);
}
		html, body {
			margin: 0;
			height: 100%;
		}
		body {
			background: linear-gradient(45deg, #ee7752, #e73c7e, #23a6d5, #23d5ab);
			background-size: 400% 400%;
			animation: gradient 10s ease infinite;
		}
		@keyframes gradient {
			0% {
				background-position: 0% 50%;
			}
			50% {
				background-position: 100% 50%;
			}
			100% {
				background-position: 0% 50%;
			}
		}
		.glass-panel {  
			max-width: 600px;
		}
		.glass-button {
			margin: 200px;
			margin-top: 40px;
		}
		h1, h1 a {
			min-height: 120px;
			width: 90%;
			max-width: 700px;
			vertical-align: middle;
			text-align: center;
			margin: 0 auto;
			text-decoration: none;
			color: #fff;
			padding-top: 60px;
			color: rgba(255,255,255,0.8);
		}
		p {
			width: 80%;
			margin: 0 auto;
			padding-bottom: 32px;
			color: rgba(255,255,255,0.6);
		}
		</style>
	</head>
	<body>
		<h1>POST</h1>
		<div class=\"glass-panel\">"

html_start+="<div class=\"glass-toolbar\"><p>"

html_end="</p></div></div></html></body>\n"

size=$(wc -c $filename | awk '{print $1}')
size=$(expr $size + ${#html_start})
size=$(expr $size + ${#html_end})
#size+=${#html_start}
#size+=${#html_end}

echo -ne "Content-Lenght: $size\n\n"

echo $html_start
cat $filename
echo $html_end


exit 0
