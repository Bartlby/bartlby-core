#!/bin/sh

echo "###### UNIT TESTS ##########################"
CFG_DIR=$PWD/installation;
cd build/
make test
EE=$?
if [ $EE != 0 ];
then
	echo "TEST COMPILE FAILED";
	exit $EE;
fi;
echo "###### UNIT TESTS ##########################"


installation/etc/bartlby.startup stop
echo "Version:" 
installation/bin/bartlby -v

grepcheck=$(grep "Pid file removed" installation/var/log/bartlby*)

if [ "x$grepcheck" = "x" ];
then
	echo "ERROR SEE LOG";
	cat installation/var/log/bartlby*
	exit 1;
fi;
exit $EX;
