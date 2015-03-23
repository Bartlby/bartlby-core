#!/bin/bash
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


MESSAGE=$(cat)
SUBJ="bartlby notify: ${BARTLBY_TRIGGER_SVC_SRV_NAME} / ${BARTLBY_TRIGGER_SVC_NAME} : ${BARTLBY_TRIGGER_SVC_CURRENT_STATE_READABLE} ";





MOBNR=$BARTLBY_TRIGGER_WRK_MOBILENR;
FNAME=$(mktemp  -p /var/spool/sms/outgoing/);
chmod a+rw $FNAME;

echo "To: $MOBNR" > $FNAME;
echo "" >> $FNAME;
echo $MESSAGE >> $FNAME;


