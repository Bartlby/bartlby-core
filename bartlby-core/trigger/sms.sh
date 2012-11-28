#!/bin/sh
###############
# Smstools compatible sms trigger for bartlby
# <hjanuschka> 2008

# smstools - http://smstools.meinemullemaus.de/
#############

BARTLBY_HTDOCS=/var/www/bartlby-ui/


######
#
#  $1 == email
#  $2 == icq
#  $3 == name
#  $4 == msg
#####


MNR=$(echo -n ${3}_SMSextension_mobile_nr|md5sum|awk '{print $1}');
MOBNR=$(cat $BARTLBY_HTDOCS/store/SMS/$MNR);
FNAME=$(mktemp  -p /var/spool/sms/outgoing/);
MSG=$(echo -e $4);
chmod a+rw $FNAME;

echo "To: $MOBNR" > $FNAME;
echo "" >> $FNAME;
echo $MSG >> $FNAME;


