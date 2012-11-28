#!/bin/sh
# SVCID PERF: '192.168.2.228' 'public' 'hrProcessorLoad.3' 'INTEGER: 5' '20' '30'
# Load Plugin Functions

function getConfigValue {
	if [ ! -f $BARTLBY_CONFIG ];
	then
		echo "Config $BARTLBY_CONFIG doesnt look like a file"  3
		exit;
	fi;
	
	r=`cat $BARTLBY_CONFIG |grep $1|awk -F"=" '{print \$2}'`
	echo $r;

}


RRD_HTDOCS=$(getConfigValue performance_rrd_htdocs|tr -d [:blank:]);

RRD_TOOLCFG=$(getConfigValue rrd_tool|tr -d [:blank:]);
RRDTOOL=${RRD_TOOLCFG:-'/usr/bin/rrdtool'}

#3188_bartlby_snmp.sh_192.168.2.228_hrProcessorLoad.3.rrd
RRDFILE="${RRD_HTDOCS}/${1}_bartlby_snmp.sh_${3}_${5}.rrd";

PNGFILE="${RRDFILE}.png";
PNGFILE_SEVEN="${RRDFILE}7.png";
PNGFILE_MONTH="${RRDFILE}31.png";
PNGFILE_YEAR="${RRDFILE}365.png";


RRD_VALUE=$(echo $6|sed 's/INTEGER: //g');

if [ ! -f $RRDFILE ];
then
	if [ "x${BARTLBY_HOME}" != "x" ];
	then
		ORIGFILE="${BARTLBY_HOME}/perf/defaults/bartlby_snmp.sh.rrd.xml";
		if [ -f $ORIGFILE ];
		then
			$RRDTOOL restore $ORIGFILE $RRDFILE;
			chmod a+rw $RRDFILE;
		fi;
	else
		echo "BARTLBY_HOME not set maybe you are running from command line";
		exit;
	fi;
fi;


$RRDTOOL update $RRDFILE N:$RRD_VALUE


echo "'${RRD_VALUE}'";


$RRDTOOL graph $PNGFILE --start -86400 -a PNG -t "SNMP ( $3 / $5 ) @ $(/bin/date "+%d.%m.%Y %H:%M:%S")" --vertical-label "Average Load" -w 600 -h 300 -M DEF:load1=${RRDFILE}:load1:AVERAGE VDEF:load1l=load1,LAST AREA:load1#ff0000:"Load Last\:" GPRINT:load1l:"%5.2lf\n" LINE1:load1#ff0000:""
$RRDTOOL graph $PNGFILE_SEVEN --start -604800 -a PNG -t "SNMP ( $3 / $5 ) @ $(/bin/date "+%d.%m.%Y %H:%M:%S")" --vertical-label "Average Load" -w 600 -h 300 -M DEF:load1=${RRDFILE}:load1:AVERAGE VDEF:load1l=load1,LAST AREA:load1#ff0000:"Load Last\:" GPRINT:load1l:"%5.2lf\n" LINE1:load1#ff0000:""
$RRDTOOL graph $PNGFILE_SEVEN --start -1month -a PNG -t "SNMP ( $3 / $5 ) @ $(/bin/date "+%d.%m.%Y %H:%M:%S")" --vertical-label "Average Load" -w 600 -h 300 -M DEF:load1=${RRDFILE}:load1:AVERAGE VDEF:load1l=load1,LAST AREA:load1#ff0000:"Load Last\:" GPRINT:load1l:"%5.2lf\n" LINE1:load1#ff0000:""
$RRDTOOL graph $PNGFILE_SEVEN --start -1year -a PNG -t "SNMP ( $3 / $5 ) @ $(/bin/date "+%d.%m.%Y %H:%M:%S")" --vertical-label "Average Load" -w 600 -h 300 -M DEF:load1=${RRDFILE}:load1:AVERAGE VDEF:load1l=load1,LAST AREA:load1#ff0000:"Load Last\:" GPRINT:load1l:"%5.2lf\n" LINE1:load1#ff0000:""



echo -n "$RRDFILE done";
