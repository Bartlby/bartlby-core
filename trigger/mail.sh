#!/bin/sh
######
#
#  $1 == email
#  $2 == icq
#  $3 == name
#  $4 == msg
# setenv("READABLE_STATE", human_state, 1);
#        str_replace(str,"$READABLE_LAST", human_state_last, max);
#        setenv("PROGNAME", PROGNAME, 1);
#        str_replace(str,"$PROGNAME", PROGNAME, max);
#        setenv("VERSION", VERSION, 1);
#        str_replace(str,"$VERSION", VERSION, max);
#        setenv("SERVER", svc->server_name, 1);
#        str_replace(str,"$SERVER", svc->server_name, max);
#        setenv("SERVICE", svc->service_name, 1);
#        str_replace(str,"$SERVICE", svc->service_name, max);
#        setenv("MESSAGE",  svc->new_server_text, 1);
#        str_replace(str,"$MESSAGE", svc->new_server_text, max);
#        setenv("CLIENT_IP", svc->client_ip, 1);
#        str_replace(str,"$CLIENT_IP", svc->client_ip, max);
#       setenv("CLIENT_PORT", clport, 1);
#        str_replace(str,"$CLIENT_PORT", clport, max);

#####


SUBJ="bartlby notify: ${BARTLBY_CURR_HOST} / ${BARTLBY_CURR_SERVICE} : ${READABLE_STATE} ";
MSG=$(echo -e $4);

echo -e ${BARTLBY_CURR_HOST} $MSG|mail -s "$SUBJ" $1
echo "Mail sent to $1";
