#!/bin/bash

if (($# == 0)); then
	alert --stop "Nothing selected"
	exit
fi

if ! type -p ffmpegthumbnailer >/dev/null;then
	alert --stop "'ffmpegthumbnailer' command not found.  Pleast install the ffmpegthumbnailer package."
	exit
fi

if ! type -p gm >/dev/null;then
	alert --stop "'gm' command not found.  Pleast install the GraphicsMagick package."
	exit
fi

function nail_it {
	videoFile="$@"
	thumbnailFile="${videoFile%.*}Thumbnail_$$.png"

	ffmpegthumbnailer -i "$videoFile" -o "$thumbnailFile"

	if (($? != 0));then
		alert --stop "Error generating thumbnail for $videoFile"
		rm -f "$thumbnailFile"
		return
	fi

	## clear out any existing thumbnail data
	rmattr -p "Media:Thumbnail*" "$videoFile" 2>/dev/null

	## make Tracker happy with a square image
	gm convert "$thumbnailFile" -background none -gravity center -extent 128x128 "$thumbnailFile"

	if (($? == 0));then
		addattr -t time Media:Thumbnail:CreationTime `date +%s` "$videoFile"
		addattr -f "$thumbnailFile" -t raw Media:Thumbnail "$videoFile"
	else
		alert --stop "Error resizing thumbnail for $videoFile"
	fi

	rm -f "$thumbnailFile"
}


for ((i = 1; i <= $#; i++)); do
	nail_it ${!i}
done
