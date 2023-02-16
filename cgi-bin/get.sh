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

#modifying the file
cp $filename $filename.cpy
filename=$filename.cpy
sed -i "s/$replace_name/$name/g" $filename
sed -i "s/$replace_link/$host/g" $filename

size=$(wc -c $filename | awk '{print $1}')

echo -ne "Content-Lenght: $size\n\n"

#cat $filename | sed -z '$ s/\n$//'
cat $filename

rm $filename


exit 0