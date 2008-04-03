#!/bin/sh
######
#
#  $1 == email
#  $2 == icq
#  $3 == name
#  $4 == msg
#####




function has_executable {
	saout=`type -p $1`
	if [ $? = 0 ];
	then
		return 0;
	fi;
	echo "executable $1 not found on your system wich is required by $MY_NAME"
	if [ "x$2" != "x" ];
	then
		#we have a notice where to get the binary :-)
		echo "Additional notices: $2";
	fi;
	exit 1;
	
}

function getConfigValue {
	if [ ! -f $BARTLBY_HOME/etc/bartlby.cfg ];
	then
		raiseErr "Config $BARTLBY_HOME/etc/bartlby.cfg doesnt look like a file"  3
		
	fi;
	
	r=`cat $BARTLBY_HOME/etc/bartlby.cfg |grep $1|awk -F"=" '{print \$2}'`
	echo $r;

}


######
# how to embedd a notification
# jabber user_name is per default bartlby username
# if jabber_$USERNAME="" is set in cfg file use this



LICQFIFO=`getConfigValue licq_fifo|tr -d [:blank:]`


if [ "x$LICQFIFO" = "x" ];
then
	echo "licq_fifo not set in cfg file, you need licq , and it should be running"
fi;

echo "message $2 \"$4\"" > $LICQFIFO;
echo "message sent to $2";