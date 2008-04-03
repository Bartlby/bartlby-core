#!/bin/sh
#########################################
# Bartlby Check Disk
# $Id: bartlby_process.sh,v 1.4 2006/09/09 20:10:09 hjanuschka Exp $ */
#
#
# $Revision: 1.4 $
# $Source: /cvsroot/bartlby/bartlby-plugins/bartlby_process.sh,v $
#
#
#
# $Log: bartlby_process.sh,v $
# Revision 1.4  2006/09/09 20:10:09  hjanuschka
# snmp
#
# Revision 1.3  2006/09/09 19:38:42  hjanuschka
# auto commit
#
# Revision 1.2  2005/11/16 23:52:43  hjanuschka
# PATH fixes
#
# Revision 1.1  2005/09/20 21:14:49  hjanuschka
# initial import
#
#
#
#########################################

VERSION="0.1";
MY_PATH=`echo $0 | sed -e 's,[\\/][^\\/][^\\/]*$,,'`
export PATH="$PATH:/usr/local/bin/";

if [ -f $MY_PATH/bartlby.funcs ]; then
	. $MY_PATH/bartlby.funcs
	plg_init_args $*;
	
else
	echo "your setup doesnt look fine cant locate 'bartlby.funcs'";
	exit 2;
fi;

set_my_name "bartlby_process"

has_executable "qpst" "please see http://www.januschka.com/__qpst.c/";





plg_get_arg "h" "IS_HELP";
plg_get_arg "w" "W_LEVEL";
plg_get_arg "c" "C_LEVEL";
plg_get_arg "p" "P_TO_FIND";
plg_get_arg "S" "SNMP";
plg_get_arg "C" "SNMP_COMM";
plg_get_arg "H" "SNMP_HOST";
plg_get_arg "v" "SNMP_VER";


if [ "$W_LEVEL" = "false" ];
then
	W_LEVEL="1";
fi;
if [ "$C_LEVEL" = "false" ];
then
	C_LEVEL="1";
fi;

if [ "$P_TO_FIND" = "false" ]; 
then
	echo "You should pass the -p option";
	export IS_HELP="true";
fi;

if [ "$IS_HELP" != "false" ]; 
then
	echo "Bartlby Check Swap $VERSION";
	echo "***************************";
	echo "Check Swap usage and alert if percentage of free swap is lower than X";
	echo "-h Display Help";
	echo "-p Process to find (looks at whole command line)";
	echo "-w Warn if count of found processes is lower than -w (Default: 1)";
	echo "-w Critical if count of found processes is lower than -w (Default: 1)";
	echo "-S use SNMP mode"; 
	echo "  -C Community (default: public)";
	echo "  -H Host (default: localhost)";
	echo "  -i interface index (default: 1)";
	echo "  -v version (default: 1), 1 or 2c";
	end_warning "";
fi;

function snmp_get_ps_parms {
	
	snmpwalk -Oq -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST hrSWRunParameters.$1|while read oid axf a;
	do
		echo "$axf";
	done;
	
}


function snmp_ps {
	snmpwalk -Oq -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST hrSWRunIndex|while read oid axf a;
	do
		IDX=$(echo $oid|awk -F. '{print $NF}');
		name=$(snmpget -t 3 -Oqv -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST hrSWRunPath.$IDX 2>/dev/null);
		params=$(snmp_get_ps_parms $IDX);
		echo "$name $params"|tr -d '"';
	done;
}

function check_proc {
	
	
	cnt=`qpst  -r $P_TO_FIND -i -p $$ -P $$  -c`; #exit code isnt safe as of 255 limitation ;-)
	echo -n " [ found $cnt processes for $P_TO_FIND $1/$2 ]\\dbr";
	
	if [ $cnt -lt $2 ]; 
	then
		exit $STATE_CRITICAL;
	fi;
	if [ $cnt -lt $1 ];
	then
		exit $STATE_WARNING;
	fi;
	
	exit $STATE_OK;
	
	
	
	
}


#### SNMP 
#hrSWRunIndex
#

if [ "$SNMP" = "false" ];
then

	MY_OUT_STR=`check_proc $W_LEVEL $C_LEVEL`;
	ECO=$?;
else
	has_executable "snmpget"
	has_executable "snmpwalk"
	if [ "$SNMP_HOST" = "false" ];
	then
		SNMP_HOST="localhost";
	fi;
	if [ "$SNMP_COMM" = "false" ];
	then
		SNMP_COMM="public";
	fi;
	if [ "$SNMP_VER" = "false" ];
	then
		SNMP_VER="1";
	fi;
	
	cnt=$(snmp_ps|grep $P_TO_FIND|wc -l);
	MY_OUT_STR=" [ SNMP found $cnt processes for $P_TO_FIND $W_LEVEL/$C_LEVEL ]\\dbr";
	
	
	if [ $cnt -lt $C_LEVEL ]; 
	then
		ECO=$STATE_CRITICAL;
	fi;
	if [ $cnt -lt $W_LEVEL ];
	then
		ECO=$STATE_WARNING;
	fi;
	
	ECO=$STATE_OK;
fi;

if [ $ECO = $STATE_CRITICAL ];
then
	end_critical "$MY_OUT_STR";
	
fi;
if [ $ECO = $STATE_WARNING ];
then
	
	end_warning "$MY_OUT_STR";
fi;


end_ok "$MY_OUT_STR";


#parse args
#get part list
#df each part
#produce warning/crit/ok