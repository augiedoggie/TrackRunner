#!/usr/bin/env bash

if (($# == 0)); then
	echo "Nothing selected"
	exit
fi

rm -f files.md5

for ((i = 1; i <= $#; i++)); do
	md5sum `basename "${!i}"` >> files.md5
done
