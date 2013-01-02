#!/bin/sh

sleep 5
installation/etc/bartlby.startup stop
sleep 3
cat installation/var/log/bartlby*
installation/bin/bartlby -v

grepcheck=$(grep "Pid file removed" installation/var/log/bartlby*)

if [ "x$grepcheck" = "x" ];
then
	echo "ERROR SEE LOG";
	exit 1;
fi;
exit 0;
