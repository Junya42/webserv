#!/bin/bash
>&2 echo "In cgi script get.sh"

replace_name="USERNAME"
replace_link="LINK"
filename=$PATH_TRANSLATED
name=$REMOTE_USER
host=$SERVER_NAME:$SERVER_PORT


#	Check if file is a directory
if [ -d "$filename" ]
then
	echo "400" >/dev/stderr
	exit 42
fi

# Check if file exist
if [ ! -f "$filename" ]
then
	echo "404" >/dev/stderr
	exit 42
fi

size=$(wc -c $filename | awk '{print $1}')

echo -ne "Content-Lenght: $size\n\n"

cat $filename


exit 0
