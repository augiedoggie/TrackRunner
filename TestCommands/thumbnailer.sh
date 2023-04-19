#!/usr/bin/env bash

## Add Tracker thumbnails for various file types
##
## PDF requires the poppler and graphicsmagick packages
## SVG requires the librsvg package
## VIDEO requres the ffmpegthumbnailer and graphicsmagick packages
##


if (($# == 0)); then
	alert --stop "Nothing selected"
	exit
fi


for ((i = 1; i <= $#; i++)); do
	if [ ! -f "${!i}" ];then
		echo "Skipping non-file ${!i}"
		continue
	fi

	mimeset "${!i}"

	mimeType=`catattr -d BEOS:TYPE "${!i}"`

	thumbnailFile="${!i%.*}Thumbnail_$$.png"

	needsResize=true

	case "$mimeType" in
		application/pdf)
			pdftoppm -singlefile -png -f 1 -l 1 -scale-to 128 "${!i}" "${thumbnailFile%.png}"
			;;
		video/*)
			ffmpegthumbnailer -i "${!i}" -o "$thumbnailFile"
			;;
		image/svg+xml)
			needsResize=false
			rsvg-convert -a -w 128 -h 128 -o "$thumbnailFile" "${!i}"
			;;
		# text/*)
		# 	needsResize=false
		# 	pygmentize -g -f png -O line_numbers=false -o "$thumbnailFile" "${!i}"
		# 	#gm convert "$thumbnailFile" -crop '160x160' -resize 128x128 -background white -gravity NorthWest -extent 128x128 "$thumbnailFile"
		# 	gm convert "$thumbnailFile" -crop '160x160' -thumbnail 128x128 -background white -gravity NorthWest -extent 128x128 "$thumbnailFile"
		# 	;;
		*)
			echo "Skipping unknown mimetype($mimeType) for ${!i}"
			continue
			;;
	esac

	if (($? != 0));then
		alert --stop "Error generating thumbnail for ${!i}"
		rm -f "$thumbnailFile"
		continue
	fi

	if [[ "$needsResize" == true ]];then
		## make Tracker happy with a square image
		gm convert "$thumbnailFile" -background none -gravity center -extent 128x128 "$thumbnailFile"
		#TODO check convert exit status
	fi

	## clear out any existing thumbnail data
	rmattr -p "Media:Thumbnail*" "${!i}" 2>/dev/null

	addattr -t time Media:Thumbnail:CreationTime `date +%s` "${!i}"
	addattr -f "$thumbnailFile" -t raw Media:Thumbnail "${!i}"

	rm -f "$thumbnailFile"

done
