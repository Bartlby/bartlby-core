#!/bin/sh

cd tests/
make CFLAGS=-DTRAVIS
./bartlby_test --verbose
EX=$?;

installation/etc/bartlby.startup stop
cat installation/var/log/bartlby*
installation/bin/bartlby -v

grepcheck=$(grep "Pid file removed" installation/var/log/bartlby*)

if [ "x$grepcheck" = "x" ];
then
	echo "ERROR SEE LOG";
	exit 1;
fi;
exit $EX;
