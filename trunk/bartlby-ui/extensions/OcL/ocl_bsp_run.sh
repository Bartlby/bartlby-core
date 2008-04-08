#!/bin/sh
BARTLBY_HTDOCS="/var/www/htdocs/dev.bartlby.org/bartlby-ui";
USERN="admin";
PASSW="password";



cd $BARTLBY_HTDOCS/extensions/



HOUR=$(date +%l);
HOUR=$[HOUR+0];


DAY=$(date +%d);
if [ $DAY -lt 10 ];
then
	DAY=$[MONTH+0];
fi;

MONTH=$(date +%m);
if [ $MONTH -lt 10 ];
then
	MONTH=$[MONTH+0];
fi;
YEAR=$(date +%Y);

DSTR=${DAY}-${MONTH}-${YEAR};


php automated.php username=$USERN password=$PASSW script=OcL/bsp_run.php hour=$HOUR day=$DSTR
#2>&1>/dev/null;
