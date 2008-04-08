#!/bin/sh
#2559 PERF: RAM SWAP last
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

RRDFILE="${RRD_HTDOCS}/${1}_bartlby_swap.sh.rrd";

PNGFILE="${RRD_HTDOCS}/${1}_bartlby_swap.sh.png";
PNGFILE_SEVEN="${RRD_HTDOCS}/${1}_bartlby_swap.sh7.png";
PNGFILE_MONTH="${RRD_HTDOCS}/${1}_bartlby_swap.sh31.png";

PNGFILE_YEAR="${RRD_HTDOCS}/${1}_bartlby_swap.sh365.png";

if [ ! -f $RRDFILE ];
then
	if [ "x${BARTLBY_HOME}" != "x" ];
	then
		ORIGFILE="${BARTLBY_HOME}/perf/defaults/bartlby_swap.sh.rrd.xml";
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






if [ "$1" = "graph" ];
then
	RRDFILE="${RRD_HTDOCS}/${2}_bartlby_swap.sh.rrd";

	PNGFILE="${RRD_HTDOCS}/${2}_bartlby_swap.sh.png";
	PNGFILE_SEVEN="${RRD_HTDOCS}/${2}_bartlby_swap.sh7.png";
	PNGFILE_MONTH="${RRD_HTDOCS}/${2}_bartlby_swap.sh31.png";

	PNGFILE_YEAR="${RRD_HTDOCS}/${2}_bartlby_swap.sh365.png";
	PNGFILE_HOUR="${RRD_HTDOCS}/${2}_bartlby_swap.sh24h.png";


	SWAPT=$(cat ${RRDFILE}.info|awk '{print $2}');
	MEMT=$(cat ${RRDFILE}.info|awk '{print $1}');
	
	MEMTOTAL=$MEMT;
	SWAPTOTAL=$SWAPT;
	
	
	
	nice -n19 $RRDTOOL graph $PNGFILE_HOUR \
        -b 1024 --start -1hour -a PNG -t "RAM and SWAP ( $BARTLBY_CURR_HOST / $BARTLBY_CURR_SERVICE ) " --vertical-label "Bytes" -w 600 -h 300 \
        DEF:fram=${RRDFILE}:fram:AVERAGE \
        DEF:fswap=${RRDFILE}:fswap:AVERAGE \
        CDEF:framb=fram,1024,* \
        CDEF:fswapb=fswap,1024,* \
        CDEF:bram=$MEMTOTAL,framb,- \
        CDEF:bswap=$SWAPTOTAL,fswapb,- \
        CDEF:brammb=bram,1048576,/ \
        CDEF:frammb=framb,1048576,/ \
        CDEF:bswapmb=bswap,1048576,/ \
        CDEF:fswapmb=fswapb,1048576,/ \
        VDEF:brammb1=brammb,LAST \
        VDEF:frammb1=frammb,LAST \
        VDEF:bswapmb1=bswapmb,LAST \
        VDEF:fswapmb1=fswapmb,LAST \
        AREA:bram#99ffff:"used RAM,  last\: " GPRINT:brammb1:"%7.3lf MB " \
        LINE1:framb#ff0000:"free RAM,  last\: " GPRINT:frammb1:"%7.3lf MB     generated @\n" \
        LINE1:bswap#000000:"used SWAP, last\: " GPRINT:bswapmb1:"%7.3lf MB " \
        LINE1:fswapb#006600:"free SWAP, last\: " GPRINT:fswapmb1:"%7.3lf MB    $(/bin/date "+%d.%m.%Y %H\:%M\:%S")" \
        > /dev/null

	
	nice -n19 $RRDTOOL graph $PNGFILE \
	-b 1024 --start -86400 -a PNG -t "RAM and SWAP ( $BARTLBY_CURR_HOST / $BARTLBY_CURR_SERVICE ) " --vertical-label "Bytes" -w 600 -h 300 \
	DEF:fram=${RRDFILE}:fram:AVERAGE \
	DEF:fswap=${RRDFILE}:fswap:AVERAGE \
	CDEF:framb=fram,1024,* \
	CDEF:fswapb=fswap,1024,* \
	CDEF:bram=$MEMTOTAL,framb,- \
	CDEF:bswap=$SWAPTOTAL,fswapb,- \
	CDEF:brammb=bram,1048576,/ \
	CDEF:frammb=framb,1048576,/ \
	CDEF:bswapmb=bswap,1048576,/ \
	CDEF:fswapmb=fswapb,1048576,/ \
	VDEF:brammb1=brammb,LAST \
	VDEF:frammb1=frammb,LAST \
	VDEF:bswapmb1=bswapmb,LAST \
	VDEF:fswapmb1=fswapmb,LAST \
	AREA:bram#99ffff:"used RAM,  last\: " GPRINT:brammb1:"%7.3lf MB " \
	LINE1:framb#ff0000:"free RAM,  last\: " GPRINT:frammb1:"%7.3lf MB     generated @\n" \
	LINE1:bswap#000000:"used SWAP, last\: " GPRINT:bswapmb1:"%7.3lf MB " \
	LINE1:fswapb#006600:"free SWAP, last\: " GPRINT:fswapmb1:"%7.3lf MB    $(/bin/date "+%d.%m.%Y %H\:%M\:%S")" \
	> /dev/null
	echo "generated 24 hour graph";
	# 7 Tage - RAM und Swap in einen
	nice -n19 $RRDTOOL graph $PNGFILE_SEVEN \
	-b 1024 --start -604800 -a PNG -t "RAM und SWAP ( $BARTLBY_CURR_HOST / $BARTLBY_CURR_SERVICE )" --vertical-label "Bytes" -w 600 -h 300 \
	DEF:fram=${RRDFILE}:fram:AVERAGE \
	DEF:fswap=${RRDFILE}:fswap:AVERAGE \
	CDEF:framb=fram,1024,* \
	CDEF:fswapb=fswap,1024,* \
	CDEF:bram=$MEMTOTAL,framb,- \
	CDEF:bswap=$SWAPTOTAL,fswapb,- \
	AREA:bram#99ffff:"used  RAM" \
	LINE1:framb#ff0000:"free RAM" \
	LINE1:bswap#000000:"used SWAP" \
	LINE1:fswapb#006600:"free SWAP" > /dev/null
	echo "generated 7 day graph";
	
	
	# 1 Monat  - RAM und Swap in einen
	nice -n19 $RRDTOOL graph $PNGFILE_MONTH \
	-b 1024 --start -1month -a PNG -t "RAM und SWAP ( $BARTLBY_CURR_HOST / $BARTLBY_CURR_SERVICE )" --vertical-label "Bytes" -w 600 -h 300 \
	DEF:fram=${RRDFILE}:fram:AVERAGE \
	DEF:fswap=${RRDFILE}:fswap:AVERAGE \
	CDEF:framb=fram,1024,* \
	CDEF:fswapb=fswap,1024,* \
	CDEF:bram=$MEMTOTAL,framb,- \
	CDEF:bswap=$SWAPTOTAL,fswapb,- \
	AREA:bram#99ffff:"used  RAM" \
	LINE1:framb#ff0000:"free RAM" \
	LINE1:bswap#000000:"used SWAP" \
	LINE1:fswapb#006600:"free SWAP" > /dev/null
	echo "generated 1 month graph $PNGFILE_MONTH";
	
	
	
	# 7 Tage - RAM und Swap in einen
	nice -n19 $RRDTOOL graph $PNGFILE_YEAR \
	-b 1024 --start -1year -a PNG -t "RAM und SWAP ( $BARTLBY_CURR_HOST / $BARTLBY_CURR_SERVICE )" --vertical-label "Bytes" -w 600 -h 300 \
	DEF:fram=${RRDFILE}:fram:AVERAGE \
	DEF:fswap=${RRDFILE}:fswap:AVERAGE \
	CDEF:framb=fram,1024,* \
	CDEF:fswapb=fswap,1024,* \
	CDEF:bram=$MEMTOTAL,framb,- \
	CDEF:bswap=$SWAPTOTAL,fswapb,- \
	AREA:bram#99ffff:"used  RAM" \
	LINE1:framb#ff0000:"free RAM" \
	LINE1:bswap#000000:"used SWAP" \
	LINE1:fswapb#006600:"free SWAP" > /dev/null
	echo "generated 1 year graph $PNGFILE_YEAR";
	
	
else
	nice -n19 $RRDTOOL update $RRDFILE N:$3:$4
	echo "$5 $6" > ${RRDFILE}.info
fi;


