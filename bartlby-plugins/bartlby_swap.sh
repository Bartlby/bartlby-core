#!/bin/sh
#########################################
# Bartlby Check Disk
# $Id: bartlby_swap.sh,v 1.6 2006/12/25 02:08:30 hjanuschka Exp $ */
#
#
# $Revision: 1.6 $
# $Source: /cvsroot/bartlby/bartlby-plugins/bartlby_swap.sh,v $
#
#
#
# $Log: bartlby_swap.sh,v $
# Revision 1.6  2006/12/25 02:08:30  hjanuschka
# auto commit
#
# Revision 1.5  2006/09/09 19:38:42  hjanuschka
# auto commit
#
# Revision 1.4  2006/08/27 02:12:00  hjanuschka
# auto commit
#
# Revision 1.3  2006/05/06 23:54:45  hjanuschka
# check_load define,include errors
#
# Revision 1.2  2005/12/27 20:47:38  hjanuschka
# added bartlby_if (check if net interface is up, and be able to track bandwith with RRD)
# added bartlby_snmp (check and track snmp values)
#
# Revision 1.1  2005/09/20 18:53:56  hjanuschka
# initial import
#
#
#
#########################################

#sleep 120

VERSION="0.1";
MY_PATH=`echo $0 | sed -e 's,[\\/][^\\/][^\\/]*$,,'`
if [ -f $MY_PATH/bartlby.funcs ]; then
	. $MY_PATH/bartlby.funcs
	plg_init_args $*;
	
else
	echo "your setup doesnt look fine cant locate 'bartlby.funcs'";
	exit 2;
fi;

#end_critical "dummy";

has_executable "df";
has_executable "awk";
has_executable "grep";
has_executable "cat";


set_my_name "bartlby_swap"



plg_get_arg "h" "IS_HELP";
plg_get_arg "w" "W_LEVEL";
plg_get_arg "c" "C_LEVEL";
plg_get_arg "p" "PERFIT";

plg_get_arg "S" "SNMP";
plg_get_arg "C" "SNMP_COMM";
plg_get_arg "H" "SNMP_HOST";
plg_get_arg "v" "SNMP_VER";
plg_get_arg "x" "SNMP_WINDOWS";

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



	
if [ "$W_LEVEL" = "false" ];
then
	W_LEVEL="15";
fi;
if [ "$C_LEVEL" = "false" ];
then
	C_LEVEL="10";
fi;


if [ "$IS_HELP" != "false" ]; 
then
	echo "Bartlby Check Swap $VERSION";
	echo "***************************";
	echo "Check Swap usage and alert if percentage of free swap is lower than X";
	
	echo "-w Warning level if free swap is lower than -w produce a warning (Default: 15)";
	echo "-c Critical level if free swap is lower than -c produce a critical (Default: 10)";
	echo "-S use SNMP mode"; 
	echo "  -x windows target host = windows";
	echo "  -C Community (default: public)";
	echo "  -H Host (default: localhost)";
	echo "  -v version (default: 1), 1 or 2c";	
	echo "-h Display Help";
	end_warning "";
fi;

function check_snmp_get_sum {

	snmpwalk -Oq -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST hrStorageType|while read oid otype;
	do
		if [ "$otype" = "$2" ];
		then
			IDX=$(echo $oid|awk -F. '{print $NF}');
			ISIZE=$(snmpget -t 3 -Oqv -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST $1.$IDX 2>/dev/null);		
			#IUSED=$(snmpget -t 3 -Oqv -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST HOST-RESOURCES-MIB::hrStorageUsed.$IDX 2>/dev/null);
			if [ $? != 0 ];
			then
				echo "snmpquery failed";
				exit $STATE_CRITICAL;
			fi;
			echo -ne "${ISIZE}+";
		fi;
	done;


}

function check_snmp_swap {
	SNMP_SWAP_HUN=0;
	SNMP_SWAP_USED=0;
	
	HUN=$(check_snmp_get_sum  HOST-RESOURCES-MIB::hrStorageSize HOST-RESOURCES-TYPES::hrStorageVirtualMemory);
	HUN="${HUN}0";
	EHUN=$[HUN];
	
	USED=$(check_snmp_get_sum  HOST-RESOURCES-MIB::hrStorageUsed HOST-RESOURCES-TYPES::hrStorageVirtualMemory);
	USED="${USED}0";
	EUSED=$[USED];
	
	hundred=$EHUN;
	used=$EUSED;
	
	p_s=`expr $hundred \- $used`;
 	p_a=`expr $p_s \* 100`;
 	p_end=`expr $p_a / $hundred`;
 	
 	echo -n " [ Free Swap (SNMP): $p_end% ] ";
 	
 	if [ $p_end -lt $2 ]; 
 	then
 		exit $STATE_CRITICAL;
 	fi;
 	if [ $p_end -lt $1 ]; 
 	then
 		exit $STATE_WARNING;
 	fi;
 	
 	
 	exit $STATE_OK;
		
	
	
	
}
function check_swap {
 	if [ ! -f /proc/swaps ];
 	then
 		echo -n " Swap maybe not enabled on your system";
 		exit $STATE_WARNING; 	
 	fi;
 		
 	hundred=`cat /proc/swaps |grep -v "Filename"|awk '{ ges += \$3; us += \$4; } END {print ges}' 2>&1`
 	used=`cat /proc/swaps |grep -v "Filename"|awk '{ ges += \$3; us += \$4; } END {print us}' 2>&1`
 	
	
 	p_s=`expr $hundred \- $used`;
 	p_a=`expr $p_s \* 100`;
 	p_end=`expr $p_a / $hundred`;
 	
 	echo -n " [ Free Swap: $p_end% ] ";
 	
 	if [ $p_end -lt $2 ]; 
 	then
 		exit $STATE_CRITICAL;
 	fi;
 	if [ $p_end -lt $1 ]; 
 	then
 		exit $STATE_WARNING;
 	fi;
 	
 	
 	exit $STATE_OK;
 	
 	
}


if [ "$SNMP" = "false" ];
then
	MY_OUT_STR=`check_swap $W_LEVEL $C_LEVEL`;
	ECO=$?;
	if [ "$PERFIT" != "false" ];
	then
		FRAM=$(grep MemFree: /proc/meminfo|awk '{print $2}')
		FSWAP=$(grep SwapFree: /proc/meminfo|awk '{print $2}')
		
		SWAPT=`grep SwapTotal: /proc/meminfo|awk '{print $2}'`
	       MEMT=`grep MemTotal: /proc/meminfo|awk '{print $2}'`
	
		MEMTOTAL=$(expr $MEMT \* 1024)
	       SWAPTOTAL=$(expr $SWAPT \* 1024)
	
		echo "PERF: $FRAM $FSWAP $MEMTOTAL $SWAPTOTAL last"|sed 's/\r//';
	fi;
else
	has_executable "snmpget"
	has_executable "snmpwalk"
	
	MY_OUT_STR=`check_snmp_swap $W_LEVEL $C_LEVEL`;
	ECO=$?;
	if [ "$PERFIT" != "false" ];
	then                        
		HUN=$(check_snmp_get_sum  HOST-RESOURCES-MIB::hrStorageSize HOST-RESOURCES-TYPES::hrStorageRam);
		HUN="${HUN}0";
		EHUN=$[HUN];
	
		USED=$(check_snmp_get_sum  HOST-RESOURCES-MIB::hrStorageUsed HOST-RESOURCES-TYPES::hrStorageRam);
		USED="${USED}0";
		EUSED=$[USED];
		
		
	
		FRAM=$[EHUN-EUSED];
		MEMT=$EHUN;
		
		HUN=$(check_snmp_get_sum  HOST-RESOURCES-MIB::hrStorageSize HOST-RESOURCES-TYPES::hrStorageVirtualMemory);
		HUN="${HUN}0";
		EHUN=$[HUN];
		USED=$(check_snmp_get_sum  HOST-RESOURCES-MIB::hrStorageUsed HOST-RESOURCES-TYPES::hrStorageVirtualMemory);
		USED="${USED}0";
		EUSED=$[USED];
		
		FSWAP=$[EHUN-EUSED];
		SWAPT=$EHUN;
		
			       
	       SWAPTOTAL=$(expr $SWAPT \* 1024)
		MEMTOTAL=$(expr $MEMT \* 1024)
		
		if [ "$SNMP_WINDOWS" != "false" ];
		then
			FRAM=$[FRAM*1024];
			FSWAP=$[FSWAP*1024];
			#MEMTOTAL=$[MEMTOTAL*1024];
			#SWAPTOTAL=$[SWAPTOTAL*1024];
		fi;
		
		echo "PERF: $FRAM $FSWAP $MEMTOTAL $SWAPTOTAL last"|sed 's/\r//';
	fi;
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
