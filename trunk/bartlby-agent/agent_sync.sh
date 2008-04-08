#!/bin/sh
MY_PATH=`echo $0 | sed -e 's,[\\/][^\\/][^\\/]*$,,'`

cd $MY_PATH;
. agent_sync.cfg

if [ ! -d "$BARTLBY_PLUGIN_DIR" ];
then
	mkdir -p $BARTLBY_PLUGIN_DIR;
fi;

function bartlby_dl_cfg {
	wget  -q --http-passwd=$BARTLBY_PW --http-user=$BARTLBY_USER -O bartlby.cfg.tmp "$BARTLBY_HTTP_HOST/extensions_wrap.php?script=AgentSyncer/getcfg.php"
	perl -i -pe "s#PLUGIN_DIR#$BARTLBY_PLUGIN_DIR#" bartlby.cfg.tmp
	if [ -s "bartlby.cfg.tmp" ];
	then
		grep -sq "allowed_ips" bartlby.cfg.tmp;
		EX=$?;
		if [ $EX = 0 ];
		then
			mv bartlby.cfg.tmp bartlby.cfg
			echo "CFG updated";
		fi;
	fi;
}

function bartlby_dl_cmd {
	wget  --http-passwd=$BARTLBY_PW --http-user=$BARTLBY_USER -O $BARTLBY_HOME/bartlby_cmd "$BARTLBY_HTTP_HOST/$1"
	chmod a+x $BARTLBY_HOME/bartlby_cmd;
}	

function bartlby_get_cmd_bin {
	if [ "$3" = "-" ];
	then
		echo "cmd binary: $2 missing on sync host";
		return;
	fi;
	if [ ! -f "$BARTLBY_HOME/bartlby_cmd" ];
	then
		echo "DL: Agent $BARTLBY_HOME/bartlby_cmd";
		bartlby_dl_agent $1 $BARTLBY_HOME/bartlby_cmd;
		return;
	fi;
	my_md=$(md5sum $BARTLBY_HOME/bartlby_cmd|awk '{print $1}');
	
	if [ "$my_md" != "$3" ];
	then
		echo "DL1 $3 / $my_md $BARTLBY_HOME/bartlby_cmd : $url";
		bartlby_dl_cmd $1 $BARTLBY_HOME/bartlby_cmd;
	else
		echo "agent $BARTLBY_HOME/bartlby_cmd is up-to-date";
	fi;
}


function bartlby_dl_agent {
	wget  --http-passwd=$BARTLBY_PW --http-user=$BARTLBY_USER -O $BARTLBY_HOME/bartlby_agent "$BARTLBY_HTTP_HOST/$1"
	chmod a+x $BARTLBY_HOME/bartlby_agent;
}	


function bartlby_dl_plg {
	wget  -q --http-passwd=$BARTLBY_PW --http-user=$BARTLBY_USER -O $BARTLBY_PLUGIN_DIR/$2 "$BARTLBY_HTTP_HOST/$1"
	chmod a+x $BARTLBY_PLUGIN_DIR/$2;
}	

function bartlby_get_agent_bin {
	if [ "$3" = "-" ];
	then
		echo "agent binary: $2 missing on sync host";
		return;
	fi;
	if [ ! -f "$BARTLBY_HOME/bartlby_agent" ];
	then
		echo "DL: Agent $BARTLBY_HOME/bartlby_agent";
		bartlby_dl_agent $1 $BARTLBY_HOME/bartlby_agent;
		return;
	fi;
	my_md=$(md5sum $BARTLBY_HOME/bartlby_agent|awk '{print $1}');
	
	if [ "$my_md" != "$3" ];
	then
		killall -9 bartlby_agent
		echo "DL1 $3 / $my_md $BARTLBY_HOME/bartlby_agent : $url";
		bartlby_dl_agent $1 $BARTLBY_HOME/bartlby_agent;
	else
		echo "agent $BARTLBY_HOME/bartlby_agent is up-to-date";
	fi;
}


function bartlby_get_plugin {
	if [ "$3" = "-" ];
	then
		echo "plugin: $2 missing on sync host";
		return;
	fi;
	if [ ! -f "$BARTLBY_PLUGIN_DIR/$2" ];
	then
		echo "DL: NEW PLG $2";
		bartlby_dl_plg $1 $2;
		return;
	fi;
	my_md=$(md5sum $BARTLBY_PLUGIN_DIR/$2|awk '{print $1}');
	
	if [ "$my_md" != "$3" ];
	then
		echo "DL: $url";
		bartlby_dl_plg $1 $2;
	else
		echo "plugin $2 is up-to-date";
	fi;
}


wget --http-passwd=$BARTLBY_PW --http-user=$BARTLBY_USER -q -O /dev/stdout "$BARTLBY_HTTP_HOST/extensions_wrap.php?script=AgentSyncer/sync.php&clientname=$BARTLBY_SYNC_CLIENTNAME&arch=$BARTLBY_SYNC_ARCH"|while read cmd url fn md;
do
	if [ "$cmd" = "PLUGIN" ];
	then
		bartlby_get_plugin $url $fn $md
	fi;
	if [ "$cmd" = "AB" ];
	then
		bartlby_get_agent_bin $url $fn $md
	fi;
	if [ "$cmd" = "CB" ];
	then
		bartlby_get_cmd_bin $url $fn $md
	fi;
	
	if [ "$cmd" = "ADDSERVER" ];
	then
		echo "client first time auto registered";
	fi;
	if [ "$cmd" = "SERVICEADD" ];
	then
		echo "$url service added";
	fi;
	if [ "$cmd" = "INSTPKG" ];
	then
		echo "Package $url installed";
	fi;
	if [ "$cmd" = "INFO" ];
	then
		echo "Info: $url";
	fi;
done;
bartlby_dl_cfg;
