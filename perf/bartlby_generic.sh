#!/bin/bash

MY_NAME=$(basename $0);
MY_NAME=${MY_NAME/./}


function getConfigValue {
	if [ ! -f $BARTLBY_CONFIG ];
	then
		echo "Config $BARTLBY_CONFIG doesnt look like a file"  3
		exit;
	fi;

	r=`cat $BARTLBY_CONFIG |grep $1|awk -F"=" '{print \$2}'`
	echo $r;

}


RRD_HTDOCS=$(getConfigValue performance_rrd_htdocs);

RRD_TOOLCFG=$(getConfigValue rrd_tool|tr -d [:blank:]);
RRDTOOL=${RRD_TOOLCFG:-'/usr/bin/rrdtool'}

RRDFILE="${RRD_HTDOCS}/${1}_${MY_NAME}.rrd";





DS_STR="";
UPD_STR="";
GRAPH_STR="";
cnt=1;
LEGEND_STR="";

cl[1]="#54EC48";
cl[2]="#ECD748";
cl[3]="#DE48EC";


function rrd_getDS {
			
			rrdtool info $2  |grep "ds\["|awk -F"." '{gsub(/ds\[/, "", $1); gsub(/\]/, "", $1); DS[$1]=1;} END {for(x in DS) { print x}}'|while read k; 
			do
				
				#graph each DS
				
				RRDFILE="$2";
				PNGFILE="${RRD_HTDOCS}/${1}_${MY_NAME}${k}.png";
				PNGFILE_SEVEN="${RRD_HTDOCS}/${1}_${MY_NAME}${k}7.png";
				PNGFILE_MONTH="${RRD_HTDOCS}/${1}_${MY_NAME}${k}31.png";
				PNGFILE_YEAR="${RRD_HTDOCS}/${1}_${MY_NAME}${k}365.png";
				PNGFILE_HOUR="${RRD_HTDOCS}/${1}_${MY_NAME}${k}60min.png";
				
				
				
				
				rrdtool graph $PNGFILE_HOUR  --start -1hour \
		-a PNG \
		--vertical-label "Generic PerfGraph" -t "Last Hour ( $BARTLBY_CURR_HOST / $BARTLBY_CURR_SERVICE )" \
		 -w 600 -h 300 \
				DEF:report${cnt}=${RRDFILE}:${k}:AVERAGE \
				AREA:report${cnt}${cl[$cnt]}:${k}  \
				VDEF:report${cnt}l=report${cnt},LAST \
				VDEF:report${cnt}mi=report${cnt},MINIMUM \
				VDEF:report${cnt}ma=report${cnt},MAXIMUM  \
				VDEF:report${cnt}avg=report${cnt},AVERAGE \
				COMMENT:"Generated @ $(/bin/date "+%d.%m.%Y %H\:%M\:%S")\\n" \
				GPRINT:report${cnt}l:"Last %5.2lf" \
				GPRINT:report${cnt}mi:"Min %5.2lf" \
				GPRINT:report${cnt}ma:"Max %5.2lf" \
				GPRINT:report${cnt}avg:"AVG %5.2lf" \
				LINE1:report${cnt}#000000 \
		COMMENT:"\\n"  
		
		
		rrdtool graph $PNGFILE  --start -86400 \
		-a PNG \
		--vertical-label "Generic PerfGraph" -t "Last 24Hours ( $BARTLBY_CURR_HOST / $BARTLBY_CURR_SERVICE )" \
		 -w 600 -h 300 \
				DEF:report${cnt}=${RRDFILE}:${k}:AVERAGE \
				AREA:report${cnt}${cl[$cnt]}:${k}  \
				VDEF:report${cnt}l=report${cnt},LAST \
				VDEF:report${cnt}mi=report${cnt},MINIMUM \
				VDEF:report${cnt}ma=report${cnt},MAXIMUM  \
				VDEF:report${cnt}avg=report${cnt},AVERAGE \
				COMMENT:"Generated @ $(/bin/date "+%d.%m.%Y %H\:%M\:%S")\\n" \
				GPRINT:report${cnt}l:"Last %5.2lf" \
				GPRINT:report${cnt}mi:"Min %5.2lf" \
				GPRINT:report${cnt}ma:"Max %5.2lf" \
				GPRINT:report${cnt}avg:"AVG %5.2lf" \
				LINE1:report${cnt}#000000 \
		COMMENT:"\\n"  
		
		rrdtool graph $PNGFILE_SEVEN  --start -1week -t "Last Week ( $BARTLBY_CURR_HOST / $BARTLBY_CURR_SERVICE )" \
		-a PNG \
		--vertical-label "Generic PerfGraph" \
		 -w 600 -h 300 \
		 		DEF:report${cnt}=${RRDFILE}:${k}:AVERAGE \
				AREA:report${cnt}${cl[$cnt]}:${k}  \
				VDEF:report${cnt}l=report${cnt},LAST \
				VDEF:report${cnt}mi=report${cnt},MINIMUM \
				VDEF:report${cnt}ma=report${cnt},MAXIMUM  \
				VDEF:report${cnt}avg=report${cnt},AVERAGE \
				COMMENT:"Generated @ $(/bin/date "+%d.%m.%Y %H\:%M\:%S")\\n" \
				GPRINT:report${cnt}l:"Last %5.2lf" \
				GPRINT:report${cnt}mi:"Min %5.2lf" \
				GPRINT:report${cnt}ma:"Max %5.2lf" \
				GPRINT:report${cnt}avg:"AVG %5.2lf" \
				LINE1:report${cnt}#000000 \
		COMMENT:"\\n"  
		
		
		rrdtool graph $PNGFILE_MONTH  --start -1month \
		-a PNG \
		--vertical-label "Generic PerfGraph" -t "Last Month ( $BARTLBY_CURR_HOST / $BARTLBY_CURR_SERVICE )" \
		 -w 600 -h 300 \
				DEF:report${cnt}=${RRDFILE}:${k}:AVERAGE \
				AREA:report${cnt}${cl[$cnt]}:${k}  \
				VDEF:report${cnt}l=report${cnt},LAST \
				VDEF:report${cnt}mi=report${cnt},MINIMUM \
				VDEF:report${cnt}ma=report${cnt},MAXIMUM  \
				VDEF:report${cnt}avg=report${cnt},AVERAGE \
				COMMENT:"Generated @ $(/bin/date "+%d.%m.%Y %H\:%M\:%S")\\n" \
				GPRINT:report${cnt}l:"Last %5.2lf" \
				GPRINT:report${cnt}mi:"Min %5.2lf" \
				GPRINT:report${cnt}ma:"Max %5.2lf" \
				GPRINT:report${cnt}avg:"AVG %5.2lf" \
				LINE1:report${cnt}#000000 \
		COMMENT:"\\n"  
		
		
		rrdtool graph $PNGFILE_YEAR  --start -1year -t "Last YEAR ( $BARTLBY_CURR_HOST / $BARTLBY_CURR_SERVICE )" \
		-a PNG \
		--vertical-label "Generic PerfGraph" \
		 -w 600 -h 300 \
				DEF:report${cnt}=${RRDFILE}:${k}:AVERAGE \
				AREA:report${cnt}${cl[$cnt]}:${k}  \
				VDEF:report${cnt}l=report${cnt},LAST \
				VDEF:report${cnt}mi=report${cnt},MINIMUM \
				VDEF:report${cnt}ma=report${cnt},MAXIMUM  \
				VDEF:report${cnt}avg=report${cnt},AVERAGE \
				COMMENT:"Generated @ $(/bin/date "+%d.%m.%Y %H\:%M\:%S")\\n" \
				GPRINT:report${cnt}l:"Last %5.2lf" \
				GPRINT:report${cnt}mi:"Min %5.2lf" \
				GPRINT:report${cnt}ma:"Max %5.2lf" \
				GPRINT:report${cnt}avg:"AVG %5.2lf" \
				LINE1:report${cnt}#000000 \
		COMMENT:"\\n"  
		
		
		echo "done $PNGFILE_HOUR";
		
				
				
				cnt=$[cnt+1];
				
				
				
				
				
			done;

}

if [ "x$1" = "xgraph" ];
then
	
		RRDFILE="${RRD_HTDOCS}/${2}_${MY_NAME}.rrd";
		

		if [ ! -f $RRDFILE ];
		then
			echo "NO RRD DB exists";
			exit;
		fi;
		
	
		rrd_getDS $2 $RRDFILE;		
		
	
		
		exit;
				
fi;

cnt=1;
for x in $*;
do
	if [ "$x" = "last" ];
	then
		continue;
	fi;
	if [ "$x" = "PERF:" ];
	then
		continue;
	fi;
	if [ "$x" = "$1" ];
	then
		continue;
	fi;
	
	k=$(echo $x| awk -F"=" '{print $1}');
	v=$(echo $x| awk -F"=" '{print $2}');
	DS_STR="${DS_STR} DS:${k}:GAUGE:120:U:U";
	UPD_STR="${UPD_STR}${v}:";
	cnt=$[cnt+1];

done

if [ ! -f $RRDFILE  ];
then 
rrdtool create $RRDFILE --step 60 $DS_STR \
RRA:AVERAGE:0.5:1:2880 \
RRA:AVERAGE:0.5:5:2880 \
RRA:AVERAGE:0.5:30:5840 \
RRA:AVERAGE:0.5:300:7840 \
RRA:MAX:0.5:1:2880 \
RRA:MAX:0.5:5:2880 \
RRA:MAX:0.5:30:5840 \
RRA:MAX:0.5:300:7840 \
RRA:MIN:0.5:1:2880 \
RRA:MIN:0.5:5:2880 \
RRA:MIN:0.5:30:5840 \
RRA:MIN:0.5:300:7840 \
RRA:LAST:0.5:1:2880 \
RRA:LAST:0.5:5:2880 \
RRA:LAST:0.5:30:5840 \
RRA:LAST:0.5:300:7840




fi;
rrdtool update $RRDFILE N:${UPD_STR%?};
echo "UPDATED ${UPD_STR%?}";



