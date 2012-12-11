#!/bin/sh
echo "Generating Debian Package";

if [ "x$1" = "x" ];
then
	echo -n -e "Version:";
	read v;
else
	v=$1;
fi;



checkinstall --install=no --pkgversion=$v --nodoc --maintainer=helmut@januschka.com --pkgname=bartlby-core  -y --requires=libssl0.9.8,libmysqlclient16
