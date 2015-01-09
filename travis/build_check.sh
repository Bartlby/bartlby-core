#!/bin/sh

echo "###### UNIT TESTS ##########################"
CFG_DIR=$PWD;
cd tests/
make CFLAGS=-DCONFIG=\\\"$CFG_DIR/etc/bartlby.cfg\\\"
./bartlby_test
EX=$?;
cd ..
echo "###### UNIT TESTS ##########################"

installation/etc/bartlby.startup stop
echo "Version:" 
installation/bin/bartlby -v

grepcheck=$(grep "Pid file removed" installation/var/log/bartlby*)

if [ "x$grepcheck" = "x" ];
then
	echo "ERROR SEE LOG";
	exit 1;
fi;
exit $EX;
