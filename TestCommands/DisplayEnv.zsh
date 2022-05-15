#!/usr/bin/env zsh

echo "PWD: $PWD"

#echo "$@"
if (($# == 0)); then
	echo "No command line arguments given"
else
	for ((i = 1; i <= $#; i++)); do
		echo "Arg $i: $argv[i]"
	done
fi

echo
read -k "?Display environment variables?[y/N]: "

if [[ "$REPLY" == "y" ]]; then
	echo "\n"
	env
fi
