#!/bin/bash
#exit;
###############
# Smstools compatible sms trigger for bartlby
# <hjanuschka> 2008

# smstools - http://smstools.meinemullemaus.de/
#############

BARTLBY_HTDOCS=/var/www/htdocs/bartlby.krone.at/


######
#
#  $1 == email
#  $2 == icq
#  $3 == name
#  $4 == msg
#####

PRIO=0;
if [ "$READABLE_STATE"  != "OK" ];
then
	PRIO=2;
fi;

APK=$(echo -n ${3}_PusherAPIKey|md5sum|awk '{print $1}');
TEMPAPIKEY=$(cat $BARTLBY_HTDOCS/store/Pusher/$APK);

OSS=$(echo $TEMPAPIKEY|awk '{ split($0, a, "-"); print a[1] }');
APIKEY=$(echo $TEMPAPIKEY|awk '{ split($0, a, "-"); print a[2] }');
echo $TEMPAPIKEY;



SUBJ="${BARTLBY_CURR_HOST} / ${BARTLBY_CURR_SERVICE} : ${READABLE_STATE} ";
MSG=$(echo -e $4);

INETCONN=$(/opt/bartlby/bin/bartlby_shmt  /opt/bartlby/etc/bartlby.cfg list|grep 6479|awk '{ split($0, a, ";"); print a[5] }');

if [ "$INETCONN" != "0" ];
then
	echo "DOIN SMS leitung down";

	/opt/bartlby/trigger/sms.sh "$1" "$2" "$3" "$4" 2>&1; 
	exit;

fi;

if [ "$OSS" = "IOS" ];
then

	/usr/local/bin/oper/prowl.pl -apikey="$APIKEY" -application="Bartlby" -event="$SUBJ" -notification="$MSG" -priority=$PRIO 2>&1
else
	#ANDROID
	echo "PUSH-ANDROID";
echo $APIKEY;
	/usr/local/bin/oper/send_nma.pl -k $APIKEY -s $SUBJ -m "$MSG"  -p $PRIO

fi;



