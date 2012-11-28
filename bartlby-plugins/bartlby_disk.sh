#!/bin/sh
#########################################
# Bartlby Check Disk
# $Id: bartlby_disk.sh,v 1.4 2006/09/09 19:38:42 hjanuschka Exp $ */
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


function get_parts {
	df -P|grep -v "Mounted"|while read a b c d e pp; do
		echo "${pp},";
	done;
}

function check_part {
	
	if [ "$2" = "false" ];
	then
		echo "$1 critical level missing; ";
		exit $STATE_CRITICAL;
	fi;
	if [ "$3" = "false" ];
	then
		echo "$1 warning level missing; ";
		exit $STATE_CRITICAL;
	fi;
	prozt=`(df -P  $1|grep -v "Mounted"|while read a b c d  pr a; do echo "$d $pr"; done;)|sed 's/[\n|%]//g'`;
	
	
	proz=$(echo $prozt|awk '{ print $2}');
	freemb=$(echo $prozt|awk '{ print $1}');
	freemb=$[freemb/1024];
	echo -n " [disk: $1 reached $proz% $freemb free ";

	if [ "$USE_MEGA" != "false" ];
	then
		proz=$freemb;
	fi;


	#check for CRIT
	if [ "$proz" -ge "$3" ];
	then
		echo -n " CRITICAL ]\\dbr";
		exit $STATE_CRITICAL;
	fi;
	if [ "$proz" -ge "$2" ];
	then
		echo -n " WARNING ]\\dbr";
		exit $STATE_WARNING;
	fi;
	echo -n " OK ]\\dbr";
	exit $STATE_OK;
	
	
}

set_my_name "bartlby_disk"
OFS=$IFS;
has_executable "df"

plg_get_arg "h" "IS_HELP";
plg_get_arg "p" "PARTITIONS";
plg_get_arg "x" "EXCLUDE";
plg_get_arg "w" "W_LEVEL";
plg_get_arg "c" "C_LEVEL";


plg_get_arg "S" "SNMP";
plg_get_arg "C" "SNMP_COMM";
plg_get_arg "H" "SNMP_HOST";
plg_get_arg "v" "SNMP_VER";
plg_get_arg "x" "SNMP_WINDOWS";

plg_get_arg "m" "USE_MEGA"

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
	W_LEVEL="90";
fi;
if [ "$C_LEVEL" = "false" ];
then
	C_LEVEL="98";
fi;

if [ "$IS_HELP" != "false" ]; 
then
	echo "Bartlby Check Disk $VERSION";
	echo "***************************";
	echo "-h Display Help";
	echo "-p Comma Seperated list of partitions to check (Default all partitions are used)";
	echo "-x comma separated list of partitions wich should get excluded from check (Default: none is excluded)";
	echo "-w Warning level if usage is higher than -w produce a warning";
	echo "-c Critical level if usage is higher than -c produce a critical";
	echo "-m treat -c/-w as Megabytes free instead of percent values (for large disks)";
	echo "-S use SNMP mode"; 
	echo "  -C Community (default: public)";
	echo "  -H Host (default: localhost)";
	echo "  -i interface index (default: 1)";
	echo "  -v version (default: 1), 1 or 2c";
	end_warning "";
fi;


function snmp_do_it {
	EE=$1;
	OFS=$IFS;
	IFS=",";
	
	for x in $EE;
	do
		hun=$(snmpget -t 3 -Oqv -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST hrStorageSize.$x);
		used=$(snmpget -t 3 -Oqv -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST hrStorageUsed.$x);
		descr=$(snmpget -t 3 -Oqv -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST hrStorageDescr.$x);						
		
								
		p_e=$[hun-used];
		p_p=$[p_e*100];
		if [ $hun -gt 0 ];
		then
			p_ed=$[p_p/hun];
		fi;
		p_end=$[100-p_ed];
		
		
	
	
		proz=$p_end;
		freemb=$[p_e/1024/1024];
		
		echo -n " [disk: $descr reached $proz% $freemb free ";

		if [ "$USE_MEGA" != "false" ];
		then
			proz=$freemb;
		fi;
		#check for CRIT
		if [ "$proz" -ge "$C_LEVEL" ];
		then
			echo -n " CRITICAL ]\\dbr";
			IS_C="true";
		fi;
		if [ "$proz" -ge "$W_LEVEL" ];
		then
			echo -n " WARNING ]\\dbr";
			IS_W="true";
			
		fi;
		echo -n " OK ]\\dbr";
		
		
		
	done;
	
	IFS=$OFS;
}

function snmp_exclude_disks {
	OFS=$IFS;
	
	IFS=",";
	
	for x in $1;
	do
		EX="false";
		for y in $EXCLUDE;
		do
			o=$(snmpget -t 3 -Oqv -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST hrStorageDescr.$x 2>/dev/null);
			if [ "$y" = "$o" ];
			then
				EX="true";
			fi;			
		done;
		if [ "$EX" = "false" ];
		then
			echo -ne "$x,";
		fi;
	done;
	IFS=$OFS;

}
function snmp_find_indexes {
	OFS=$IFS;
	
	IFS=",";
	for x in $1;
	do
		IFS=$OFS;
		snmpwalk -Oq -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST hrStorageDescr|while read oid axf a;
		do
			
			if [ "$axf" = "$x" ];
			then
				IDX=$(echo $oid|awk -F. '{print $NF}');
				echo -ne "$IDX,";
			fi;
		done;
		IFS=",";
	done;
	IFS=$OFS;
	
}
function snmp_get_indexes {
	
	snmpwalk -Oq -v $SNMP_VER -c $SNMP_COMM $SNMP_HOST hrStorageType|while read oid axf a;
	do
		
		if [ "$axf" = "HOST-RESOURCES-TYPES::hrStorageFixedDisk" ];
		then
				
			IDX=$(echo $oid|awk -F. '{print $NF}');
			echo -ne "$IDX, ";
		fi;
	done;
}


if [ "$SNMP" = "false" ];
then
	if [ "$PARTITIONS" = "false" ];
	then
		PARTITIONS=`get_parts`;	
	fi;
	
	PARTITIONS=`echo $PARTITIONS|sed 's/\n//g'|sed 's/ //g'`;
	
	#get partitions as specified with -p :-)
	
	MY_OUT_STR="";
	OIFS=$IFS;
	IFS=",";
	
	IS_W="false";
	IS_C="false";
	
	EX="false";
	
	for p in $PARTITIONS; do
		
		EX="false";
		for excl  in $EXCLUDE; do
			if [ "$excl" = "$p" ];
			then
				
				EX="true";
			fi;
			
		done;
		if [ "$EX" = "false" ];
		then
			IFS=" "; # for check_part;
			MY_OUT_STR="${MY_OUT_STR}`check_part $p $W_LEVEL $C_LEVEL`";	
			
			ECO=$?;
			if [ $ECO = $STATE_CRITICAL ];
			then
				IS_C="true";
				
			fi;
			if [ $ECO = $STATE_WARNING ];
			then
				
				IS_W="true";
			fi;
		fi;
		IFS=","; #restor check_part broken IFS
		
	done;
	
	
	IFS=$OIFS;

else
	IS_W="false";
	IS_C="false";
	
	if [ "$PARTITIONS" = "false" ];
	then
		PARTITIONS=`snmp_get_indexes`;
		PARTITIONS=$(echo $PARTITIONS|tr -d [:blank:]);
		#snmp_get_indexes;
		#exit;	
	else
		PARTITIONS=$(echo $PARTITIONS|tr -d [:blank:]);
		PARTITIONS=$(snmp_find_indexes $PARTITIONS);
		PARTITIONS=$(echo $PARTITIONS|tr -d [:blank:]);
	fi;
	
	
	if  [ "$EXCLUDE" != "false" ];
	then
		PARTITIONS=$(echo $PARTITIONS|tr -d [:blank:]);
		PARTITIONS=$(snmp_exclude_disks $PARTITIONS);
		PARTITIONS=$(echo $PARTITIONS|tr -d [:blank:]);
	fi;
	
	IFS=$OFS;
	
	
	snmp_do_it "$PARTITIONS";
	#echo $MY_OUT_STR;
	
	
	
fi;



if [ "$IS_C" = "true" ];
then
	end_critical "$MY_OUT_STR";
fi;

if [ "$IS_W" = "true" ];
then
	
	end_warning "$MY_OUT_STR";
fi;

#still alive so its okee

end_ok "$MY_OUT_STR";


#parse args
#get part list
#df each part
#produce warning/crit/ok
