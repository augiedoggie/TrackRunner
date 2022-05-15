#!/usr/bin/env bash

if (($# == 0)); then
	alert --stop "Nothing selected"
	exit
fi

attribute="MyTestAttribute"

for ((i = 1; i <= $#; i++)); do
	if catattr $attribute "${!i}" > /dev/null 2>& 1; then
		state="enabled"
		button="Disable"
	else
		state="disabled"
		button="Enable"
	fi

	if ! alert "$attribute is currently ${state} for ${!i}" "Cancel" $button > /dev/null; then
		if [[ $state == "disabled" ]]; then
			addattr -t int32 $attribute 1 "${!i}"
		else
			rmattr $attribute "${!i}"
		fi
	fi
done
