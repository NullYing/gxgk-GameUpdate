#!/bin/bash
cd /home/www/GameUpdate/LOL
./url
urlfile=line.txt
echo `date '+%Y-%m-%d %H:%M:%S'`
cat $urlfile | while read line; do
    md5=`echo $line |grep 'autopatch'| cut -f 2 -d '='|  cut -f 1 -d '_'| cut -f 7 -d '/'`
    Ver=`echo $line |grep 'autopatch'| cut -f 1 -d '='`
    url=`echo $line |grep 'autopatch'| cut -f 2 -d '='`
    Name=V"$Ver"_version.list
    md5check=`md5sum $Name| cut -f 1 -d ' '`

    if [ "$md5" != "$md5check" ];then
	 echo RMd5=$md5
	 echo LMd5=$md5check
        wget -O $Name - $url
    fi
	./down.sh $Name
	./RenewVer.sh $Ver
done 

