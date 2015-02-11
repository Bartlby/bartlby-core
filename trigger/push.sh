#!/bin/bash
#exit;
###############
# Smstools compatible sms trigger for bartlby
# <hjanuschka> 2008

# smstools - http://smstools.meinemullemaus.de/
#############


MESSAGE=$(cat)
SUBJ="bartlby notify: ${BARTLBY_TRIGGER_SVC_SRV_NAME} / ${BARTLBY_TRIGGER_SVC_NAME} : ${BARTLBY_TRIGGER_SVC_CURRENT_STATE_READABLE} ";




BARTLBY_HTDOCS=/var/www/htdocs/bartlby.krone.at/


######
#
#  $1 == email
#  $2 == icq
#  $3 == name
#  $4 == msg
#####

PRIO=0;
if [ "$BARTLBY_TRIGGER_SVC_CURRENT_STATE_READABLE"  != "OK" ];
then
	PRIO=2;
fi;

APK=$(echo -n ${BARTLBY_TRIGGER_WRK_NAME}_PusherAPIKey|md5sum|awk '{print $1}');
TEMPAPIKEY=$(cat $BARTLBY_HTDOCS/store/Pusher/$APK);

OSS=$(echo $TEMPAPIKEY|awk '{ split($0, a, "-"); print a[1] }');
APIKEY=$(echo $TEMPAPIKEY|awk '{ split($0, a, "-"); print a[2] }');
echo $TEMPAPIKEY;

MSG=$MESSAGE;



if [ "$OSS" = "IOS" ];
then

	/usr/local/bin/oper/prowl.pl -apikey="$APIKEY" -application="Bartlby" -event="$SUBJ" -notification="$MSG" -priority=$PRIO 2>&1
else
	#ANDROID
	echo "PUSH-ANDROID";
echo $APIKEY;
	/usr/local/bin/oper/send_nma.pl -k $APIKEY -s $SUBJ -m "$MSG"  -p $PRIO

fi;



