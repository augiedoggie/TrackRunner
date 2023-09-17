#!/bin/env bash

if (($# == 0)); then
	alert --stop "Nothing selected"
	exit
fi

for ((i = 1; i <= $#; i++)); do
	#clipString+=" ${!i}" ## space separator
	clipString+="${!i}"$'\n' ## newline seperator
done

if [ -n "$clipString" ]; then
	clipboard -c "$clipString"
fi
