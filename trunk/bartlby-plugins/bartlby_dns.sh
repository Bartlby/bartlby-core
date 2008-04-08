#!/bin/sh
#########################################
# Bartlby Check Disk
# $Id: bartlby_dns.sh,v 1.1 2005/10/13 23:08:28 hjanuschka Exp $ */
#
#
# $Revision: 1.1 $
# $Source: /cvsroot/bartlby/bartlby-plugins/bartlby_dns.sh,v $
#
#
#
# $Log: bartlby_dns.sh,v $
# Revision 1.1  2005/10/13 23:08:28  hjanuschka
# dns plugin added
#
#
#########################################

VERSION="0.1";
MY_PATH=`echo $0 | sed -e 's,[\\/][^\\/][^\\/]*$,,'`
if [ -f $MY_PATH/bartlby.funcs ]; then
	. $MY_PATH/bartlby.funcs
	plg_init_args $*;
	
else
	echo "your setup doesnt look fine cant locate 'bartlby.funcs'";
	exit 2;
fi;






set_my_name "bartlby_dns"

has_executable "nslookup"

plg_get_arg "h" "IS_HELP";
plg_get_arg "s" "SERVER";
plg_get_arg "d" "DOMAIN";
plg_get_arg "t" "TIMEOUT";



if [ "$IS_HELP" != "false" ]; 
then
	echo "Bartlby Check DNS $VERSION";
	echo "***************************";
	echo "-h Display Help";
	echo "-d Domain to check";
	echo "-s DNS server to use";
	echo "-t Timeout";
	end_warning "";
fi;



if [ "$SERVER" = "false" ];
then
	end_warning "no server specified";
fi;
if [ "$DOMAIN" = "false" ];
then
	end_warning "no domain specified";
fi;
if [ "$TIMEOUT" = "false" ];
then
	TIMEOUT=2;
fi;

RC=`nslookup -timeout=$TIMEOUT $DOMAIN $SERVER 2>&1|grep "Address"|wc -l`

if [ $RC -lt 2 ];
then
	end_critical "DNS Errror $SERVER/$DOMAIN";
else
	end_ok "DNS OK $SERVER/$DOMAIN";
fi;