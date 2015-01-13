cd ../src/libs/mysql/
cproto -I../../../include -I/usr/include/curl -I/usr/include/json  *.c|grep -v '/'|awk -F"(" '{ print $1 }'|awk '{ gsub(/\*/, "", $0); print $NF}'|sort|uniq|while read a; do 
	grep $a /usr/local/src/bartlby-core/tests/*.c 2>&1 >> /dev/null; 
	EX=$?;
	if [ "$EX" != 0 ]; then  
		echo $a; 
fi;
 done
