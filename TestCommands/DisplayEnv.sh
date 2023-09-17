#!/bin/env bash

echo "PWD: $PWD"

#echo "$@"

if (($# == 0)); then
	echo "No command line arguments given"
else
	for ((i = 1; i <= $#; i++)); do
		echo "Arg $i: ${!i}"
	done
fi

echo
read -n1 -p "Display environment variables?[y/N]: "

if [[ "$REPLY" == "y" ]]; then
	echo -e "\n"
	env
fi
