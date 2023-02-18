#!/bin/bash

file_to_print=$PATH_TRANSLATED
name=$REMOTE_USER
host=$SERVER_NAME:$SERVER_PORT


#	Check if file is a directory
if [ -d "$file_to_print" ]
then
	echo "400" >/dev/stderr
	exit 42
fi

# Check if file exist
if [ ! -f "$file_to_print" ]
then
	echo "404" >/dev/stderr
	exit 42
fi

html_start="<html>"
html_start+="<head>"
html_start+="<link rel=\"stylesheet\" href=\"get_no_mod.sh/neonstyle.css\">"
html_start+="</head>"
html_start+="<body>"
html_start+="<div class=\"login-box\">"
html_start+="<h2>File data</h2>"
html_start+="<a href=#>"
html_start+="<span></span><span></span><span></span><span></span>"
html_start+="</a>"
html_start+="<p>"

html_end="</p>"
html_end+="</div>"
html_end+="</body>"
html_end+="</html>"

size=$(wc -c $file_to_print | awk '{print $1}')
#size=$(expr $size + ${#html_start})
#size=$(expr $size + ${#html_end})

echo -ne "Content-Lenght: $size\n\n"

echo $html_start
echo "$(cat $file_to_print)"
echo $html_end

exit 0