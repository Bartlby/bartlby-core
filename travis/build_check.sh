#!/bin/sh


CFG_DIR=$PWD/installation;
cd tests/
#make CFLAGS=-DCONFIG=\\\"$CFG_DIR/etc/bartlby.cfg\\\"
EE=$?
if [ $EE != 0 ];
then
	echo "TEST COMPILE FAILED";
	exit $EE;
fi;


echo "###### UNIT TESTS ##########################"
./bartlby_test
EX=$?;
cd ..
echo "###### UNIT TESTS ##########################"

sudo installation/etc/bartlby.startup stop
echo "Version:" 
installation/bin/bartlby -v

grepcheck=$(grep "Pid file removed" installation/var/log/bartlby*)

if [ "x$grepcheck" = "x" ];
then
	echo "ERROR SEE LOG";
	exit 1;
fi;
exit $EX;
