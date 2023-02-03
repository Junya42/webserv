#!/bin/bash
>&2 echo "IN CGI SCRIPT"

replace_name="USERNAME"
replace_link="LINK"
filename=$PATH_TRANSLATED
name=$REMOTE_USER
host=$SERVER_NAME:$SERVER_PORT

check=0

#	Check if file is a directory
if [ -d "$filename" ]
then
	echo -n "Bad Request"
	exit 42
fi

# Check if file exist
if [ ! -f "$filename" ]
then
	echo -n "Not Found"
	exit 42
fi

#	Check if we need to modify the file
if [ $# -eq 1 ] && [ $1 = 'replace' ]
then
	check=1
	cp $filename $filename.cpy
	filename=$filename.cpy
	sed -i "s/$replace_name/$name/g" $filename
	sed -i "s/$replace_link/$host/g" $filename
fi

size=$(wc -c $filename | awk '{print $1}')

echo -ne "Content-Lenght: $size\n\n"

cat $filename

if [ $check -eq 1 ]
then
	rm $filename
fi

exit 0
