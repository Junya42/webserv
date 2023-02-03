#!/bin/bash

echo $#
#	Check if enough arguments
if [ $# -ne 4 ]
then
	echo -n "Server Error"
	exit 42
fi

replace_name="USERNAME"
replace_link="LINK"
filename=$1
name=$2
host=$3
cmd=$4

check=0


echo filename = $filename
#	Check if file is a directory
if [ -d $filename ]
then
	echo -n "Bad Request"
	exit 42
fi

# Check if file exist
if [ ! -f $filename ]
then
	echo -n "Not Found"
	exit 42
fi

#	Check if we need to modify the file
if [ $cmd = 'replace' ]
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

echo -ne "\n\n"

if [ $check -eq 1 ]
then
	rm $filename
fi

exit 0
