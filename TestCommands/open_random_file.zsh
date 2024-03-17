#!/bin/env zsh -f

print_usage() {
	print "$ZSH_ARGZERO [-r] <path>"
	print "	-r	Recurse into subdirectories"
	print "	-h	Show this help"
	print
	print "	If no <path> is given then the current directory will be used."
	print "	If <path> is a file then its parent directory will be used."
	exit 1
}

zparseopts -D -E -F -K -- \
	r=recurseDirs \
	h=help \
	|| print_usage

(( $#help )) && print_usage

[[ "$1" = -- || "$1" = - ]] && shift

# use the current folder(.) by default in case nothing was selected
local searchDir="."
if (( $# > 0 ));then
	if test -d "$1";then
		searchDir="$1"
	else
		# if $1 is a file then use its parent dir
		searchDir="$1:h"
	fi
fi

#TODO make sure files are not executables or scripts that will be run by Tracker
local -a files
if (( $#recurseDirs ));then
	files=( $searchDir/**/*(N.) )
else
	files=( $searchDir/*(N.) )
fi

if (( $#files == 0 ));then
	alert --stop 'No files found!'
	exit 1
fi

open "$files[$(( RANDOM%$#files + 1 ))]"
