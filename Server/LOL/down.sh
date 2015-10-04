#!/bin/bash
Name=$1
cat $Name | while read list; do
	PACK=`echo $list | grep 'autopatch' | awk  'BEGIN { FS="[?]" } {print $6}' `
	RMd5=`echo $list | grep 'autopatch' | awk  'BEGIN { FS="[?]" } {print $7}'`	
	if [ "$PACK" != "" ]
	then
		PackName=`echo $list | grep 'autopatch' | awk  'BEGIN { FS="[?]" } {print $5}'`
		LMd5=`md5sum ./patchs/$PackName | cut -f 1 -d ' '`
		if [ "$RMd5" != "$LMd5" ];then
			echo "$PackName:RMd5=$RMd5"
			echo "$PackName:LMd5=$LMd5"
			wget -O "./patchs/$PackName"  $PACK -c
			LMd5=`md5sum ./patchs/$PackName | cut -f 1 -d ' '`
			if [ "$RMd5" != "$LMd5" ];then
				echo "$PackName:RMd5=$RMd5"
				echo "$PackName:LMd5=$LMd5"
				wget -O "./patchs/$PackName"  $PACK
			fi
		fi
	else
		PACK0=`echo $list | grep 'autopatch' | awk  'BEGIN { FS="[?]" } {print $2}'`
		RMd5=`echo $list | grep 'autopatch' | awk  'BEGIN { FS="[?]" } {print $3}'`	
		if [ "$PACK0" != "" ];then
			PackName0=`echo $list | grep 'autopatch' | awk  'BEGIN { FS="[?]" } {print $1}'| awk  'BEGIN { FS="[=]" } {print $2}'`
			LMd5=`md5sum ./patchs/$PackName0 | cut -f 1 -d ' '`
			if [ "$RMd5" != "$LMd5" ];then
				echo "$PackName0:RMd5=$RMd5"
				echo "$PackName0:LMd5=$LMd5"
				wget -O "./patchs/$PackName0"  $PACK0 -c
				LMd5=`md5sum ./patchs/$PackName0 | cut -f 1 -d ' '`
				if [ "$RMd5" != "$LMd5" ];then
					echo "$PackName0:RMd5=$RMd5"
					echo "$PackName0:LMd5=$LMd5"
					wget -O "./patchs/$PackName0"  $PACK0
				fi
			fi
		fi
	fi
done
