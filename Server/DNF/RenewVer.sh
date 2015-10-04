#!/bin/bash
NewVer=`echo $1 | awk -F "." '{print $1$2$3$4$5}'`
cat DNFVer | while read line; do
	NowVer=`echo $line| awk -F "." '{print $1$2$3$4$5}'`
	if [ "$NowVer" != "" ];then
		if [ $NowVer -lt $NewVer ];then
			echo "$1" >DNFVer
			echo "New=$1->$NewVer"
			echo "Now=$line->$NowVer"
		fi
	fi
done

