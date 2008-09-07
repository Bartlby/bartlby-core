<?
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	include "extensions/AgentSyncer/AgentSyncer.class.php";
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("agentsyncer_sync");
	$sg = new AgentSyncer();
	$servers=$btl->GetSVCMap();
	$plugin_root=bartlby_config($btl->CFG, "agent_plugin_dir");
	//echo "PLUGIN " . $_SERVER["REMOTE_ADDR"] . " " .  $_GET[clientname] . "\n";
	
	//response with required plugins
	$client_found = 0;
	while(list($k, $v) = each($servers)) {
		//echo $v[0][server_name] . "-" .   $_SERVER["REMOTE_ADDR"] .  "-" . gethostbyname($v[0][client_ip]) . "-\n";
		if($v[0][server_name] == $_GET[clientname]) {
			$client_found=1;
			for($x=0; $x<count($v); $x++) {
				
				$distinct[$v[$x][plugin]]=1;
				
			}
		}
	}
	if($client_found == 0) {
		//Auto register?
		$auto_reg = bartlby_config("ui-extra.conf", "sync_auto_register");
		$exclude = bartlby_config("ui-extra.conf", "sync_exclude_clients");
		$default_pkg = bartlby_config("ui-extra.conf", "sync_default_pkg");
		$exclude_list=explode(",", $exclude);
		
		
		if($auto_reg == "true") {
			if(!in_array($_GET[clientname], $exclude_list)) {
				$add_server=bartlby_add_server($btl->CFG, $_GET[clientname],$_SERVER["REMOTE_ADDR"], 9030, "01generic.gif", 1, 1, 2000, 0);
				echo "ADDSERVER $add_server\n";
				flush();
				if(!$default_pkg) {
					$add_service=bartlby_add_service($btl->CFG, $add_server, "INIT", "Initial Check", "-h", 0, "" ,2000,1,"",200, 20, 0, 3, "", "", "", "", "", "", 1, 120);	
					echo "SERVICEADD INIT\n";
					flush();
				} else {
					$btl->installPackage($default_pkg, $add_server, NULL, NULL);
					echo "INSTPKG $default_pkg \n";
					flush();
				}
				$btl->_log("AgentSyncer: $_GET[clientname] / " . $_SERVER["REMOTE_ADDR"]  . " registered");
			} else {
				echo "INFO clientname_excluded \n";		
				flush();
			}	
		} else {
			echo "INFO auto_register_off \n";	
			flush();
		}
	}
	
	
	while(@list($k, $v) = @each($distinct)) {
		$fn = $plugin_root . "/" . $k;
		
		echo "PLUGIN extensions_wrap.php?script=AgentSyncer/getplugin.php&plugin=$k $k";
		if(file_exists($fn)) {
			$xy=@md5_file($fn);	
		} else {
			$xy="-";	
		}
		echo " " . $xy . "\n";
		flush();
	}

	$fn = $plugin_root . "/bartlby.funcs";
        echo "PLUGIN extensions_wrap.php?script=AgentSyncer/getplugin.php&plugin=$k $k";
        if(file_exists($fn)) {
	       $xy=@md5_file($fn);
        } else {
                 $xy="-";
        }
        echo " " . $xy . "\n";
        flush();

	
	$fn="extensions/AgentSyncer/bartlby_agent." . $_GET[arch];
	echo "AB extensions_wrap.php?script=AgentSyncer/getab.php?bin=$fn $fn";
	if(file_exists($fn)) {
		$xy=@md5_file($fn);	
	} else {
		$xy="-";	
	}
	echo " " . $xy . "\n";
	flush();
	$fn="extensions/AgentSyncer/bartlby_cmd." . $_GET[arch];
	echo "CB extensions_wrap.php?script=AgentSyncer/getab.php?bin=$fn $fn";
	if(file_exists($fn)) {
		$xy=@md5_file($fn);	
	} else {
		$xy="-";	
	}
	echo " " . $xy . "\n";
	flush();
	$btl->_log("AgentSyncer: $_GET[clientname] / " . $_SERVER["REMOTE_ADDR"] . " synced");

	$fp=@fopen("extensions/AgentSyncer/store/" . md5($_GET[clientname]), "w");
	@fwrite($fp, time());
	@fclose($fp);
	



?>
