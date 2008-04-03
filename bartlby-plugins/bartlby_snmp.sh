#!/bin/sh
#########################################
# Bartlby Check Disk
# $Id: bartlby_snmp.sh,v 1.2 2005/12/27 21:45:08 hjanuschka Exp $ */
#
#
# $Revision: 1.2 $
# $Source: /cvsroot/bartlby/bartlby-plugins/bartlby_snmp.sh,v $
#
#
#
# $Log: bartlby_snmp.sh,v $
# Revision 1.2  2005/12/27 21:45:08  hjanuschka
# typo fixes in snmp plg
#
# Revision 1.1  2005/12/27 20:47:38  hjanuschka
# added bartlby_if (check if net interface is up, and be able to track bandwith with RRD)
# added bartlby_snmp (check and track snmp values)
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






set_my_name "bartlby_snmp"

has_executable "snmpget"

plg_get_arg "h" "IS_HELP";
plg_get_arg "s" "SERVER";
plg_get_arg "C" "COMMUNITY";
plg_get_arg "o" "OID";
plg_get_arg "w" "WARN";
plg_get_arg "c" "CRIT";
plg_get_arg "v" "SNMPV";
plg_get_arg "m" "MATCH_V";
plg_get_arg "p" "PERFIT";

if [ "$IS_HELP" != "false" ]; 
then
	echo "Bartlby Check SNMP (single Value) $VERSION";
	echo "***************************";
	echo "-p Perf it";
	echo "-s Server";
	echo "-v SNMP Version (DEf: 1)"
	echo "-C Community";
	echo "-o OID";
	echo "-w WARNING";
	echo "-c CRITICAL";
	echo "-m Mode";
	echo "       gt  If VALUE is greater than WARN/CRIT";
	echo "       lt  If VALUE is lower than WARN/CRIT";
	echo "       eq  If VALUE is equal to WARN/CRIT";
	echo "       ne  If VALUE is not equal to WARN/CRIT";
	
	
	
	end_warning "";
fi;

if [ "$SNMPV" = "false" ];
then
	export SNMPV=1;
fi;
if [ "$SERVER" = "false" ];
then
	end_warning "no server specified";
fi;
if [ "$COMMUNITY" = "false" ];
then
	end_warning "no COMMUNITY specified";
fi;
if [ "$OID" = "false" ];
then
	end_warning "no OID specified";
fi;
if [ "$MATCH_V" = "false" ];
then
	end_warning "no MATCH Mode specified";
fi;




SNMP_REPLY=`snmpget -O v -t 1 -v $SNMPV -c $COMMUNITY $SERVER $OID 2>&1`;
EX=$?;
SNMP_VAL=$(echo $SNMP_REPLY|awk 'BEGIN {FS=":";} {print $2}');
SNMP_V=`echo $SNMP_VAL|tr -d [:blank:]`
# First of all check if the get has worked ;-)


if [ $EX != 0 ];
then
	end_critical "Executen failed for host $SERVER";
fi;
if [ "$PERFIT" != "false" ];
then
	echo "PERF: '${SERVER}' '${COMMUNITY}' '${OID}' '${SNMP_REPLY}' '${WARN}' '${CRIT}'";
fi;

E_STR="Server: $SERVER OID: $OID  '${SNMP_V}' W: $WARN C: $CRIT";

case "$MATCH_V" in
	"gt")	
		if [ $SNMP_V -gt $CRIT ];
		then
			end_critical "${SNMP_V} gt $CRIT";
		fi;
		if [ "$SNMP_V" -gt $WARN ];
		then
			end_warning "${SNMP_V} gt $WARN";
		fi;
		end_ok "${E_STR}";;
	"lt")	
		if [ $SNMP_V -lt $CRIT ];
		then
			end_critical "${SNMP_V} gt $CRIT";
		fi;
		if [ $SNMP_V -lt $WARN ];
		then
			end_warning "${SNMP_V} gt $WARN";
		fi;
		end_ok "${E_STR}";;
		
	"eq") 
		if [ "$SNMP_V" != "$CRIT" ];
		then
			end_critical "${SNMP_V} matches CRIT: ${CRIT}";
		fi;
		if [ "$SNMP_V" != "$WARN" ];
		then
			end_warning "${SNMP_V} matches CRIT: ${WARN}";
		fi;
		end_ok "${E_STR}";;
	"ne") 
		if [ "$SNMP_V" = "$CRIT" ];
		then
			end_critical "${SNMP_V} matches CRIT: ${CRIT}";
		fi;
		if [ "$SNMP_V" = "$WARN" ];
		then
			end_warning "${SNMP_V} matches CRIT: ${WARN}";
		fi;
		end_ok "${E_STR}";;
esac;








end_ok "$E_STR";

