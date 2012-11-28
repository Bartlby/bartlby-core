#!/bin/sh
######
#
#  $1 == email
#  $2 == icq
#  $3 == name
#  $4 == msg
#####

ulimit -c 0 #for jabblib


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


CFGKEY="jabber_${3}";

JABBERUSER=`getConfigValue $CFGKEY|tr -d [:blank:]`
JABBERHOST=`getConfigValue jabber_host|tr -d [:blank:]`
JABBERPORT=`getConfigValue jabber_port|tr -d [:blank:]`
JABBERLOGIN=`getConfigValue jabber_from|tr -d [:blank:]`
JABBERPW=`getConfigValue jabber_pass|tr -d [:blank:]`

SENDTO=${JABBERUSER:-"${3}@$JABBERHOST"}
USEPORT=${JABBERPORT:-5222}

has_executable "jabblib" "see jabblib @ http://steve.org.uk/Software/jabber/"
jabblib --server=$JABBERHOST --port=$USEPORT --username=$JABBERLOGIN --password=$JABBERPW --to=$SENDTO --send="$4"
echo "Message Send for: $SENDTO to host $JABBERHOST : $USEPORT";
