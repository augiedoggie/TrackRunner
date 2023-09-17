#!/bin/env bash

if (($# == 0)); then
	alert --stop "Nothing selected"
	exit
fi

tarCommand=tar
# prefer bsdtar if available
if type -p bsdtar > /dev/null;then
	tarCommand=bsdtar
fi

# loop through the files passed on the command line
for ((i = 1; i <= $#; i++)); do
	if [ ! -f "${!i}" ];then
		alert --warning "Skipping non-file ${!i}"
		continue
	fi

	mimeset "${!i}"

	mimeType=`catattr -d BEOS:TYPE "${!i}"`

	outputDir=.extract_$$

	if [[ "$mimeType" == application/zip ]];then
		mkdir $outputDir
		cd $outputDir
		notify --timeout 2 "Extracting: ${!i}"
		unzip "${!i}"
	elif [[ "$mimeType" == application/x-vnd.haiku-package ]];then
		mkdir $outputDir
		cd $outputDir
		notify --timeout 2 "Extracting: ${!i}"
		package extract "${!i}"
	elif [[ "$mimeType" == application/* ]];then
		mkdir $outputDir
		cd $outputDir
		notify --timeout 2 "Extracting: ${!i}"
		$tarCommand xvf "${!i}"
	else
		alert --warning "Skipping non-archive mimetype($mimeType) for ${!i}"
		continue
	fi

	fileCount=`ls -1 | wc -l`
	cd ..
	if (($fileCount == 0));then
		# empty archive?
		rm -rf $outputDir
		continue
	elif (($fileCount == 1));then
		file=`ls -1 $outputDir`
		# check if the file/folder already exists in the current directory
		# and give the new one a unique name
		if [ -e "$file" ];then
			mv "$outputDir/$file" "$file-`date -Iseconds`"
		else
			mv "$outputDir/$file" .
		fi
		rm -rf $outputDir
	else
		# archive has multiple top-level files/folders
		# rename the folder to match the archive name and add uniqueness
		#TODO strip file extension
		mv $outputDir "${!i}-`date -Iseconds`"
	fi
done
