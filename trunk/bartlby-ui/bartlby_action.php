<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";

$act=$_GET[action];
if($_POST[action]) {
	$act=$_POST[action];	
}
$btl=new BartlbyUi($Bartlby_CONF);
$layout= new Layout();
$layout->setTitle("Bartlby Action ($act)");


$layout->Table("100%");

function dnl($i) {
	return sprintf("%02d", $i);
}


if($act != "delete_package_ask") {
	$btl->hasRight("action." . $act);
}
switch($act) {
	case 'save_permissions':
	
		while(list($k, $v) = each($_POST)) {
			if($k == "worker_id" || $k == "action") continue;
			if($k == "worker_services") {
				for($x=0; $x<count($_POST[worker_services]); $x++) {
					if($_POST[worker_services][$x]{0} == 's') {
						$cl = str_replace("s", "", $_POST[worker_services][$x]);
				
						$servers .= "," . $cl;
					} else {
						$services .= "," . $_POST[worker_services][$x];	
					}
				}
				
				$new_file .= "services=" . $services . ",\n";
				$new_file .= "servers=" . $servers . ",\n";
				continue;	
			}
			$nk = str_replace("action_", "action.", $k);
			$nk = str_replace("main_", "main.", $nk);
			$nk = str_replace("core_", "core.", $nk);
			$nk = str_replace("log_", "log.", $nk);
			$new_file .= $nk . "=true\n";	
		}
		$wk = bartlby_get_worker_by_id($btl->CFG, $_POST[worker_id]);
		$fp = @fopen("rights/" . $wk[worker_id] . ".dat", "w");
		@fwrite($fp, $new_file);
		@fclose($fp);
	break;
	
	case 'disable_extension':
		
		touch("extensions/" . $_GET[ext] . ".disabled");
	break;
	case 'enable_extension':
		@unlink("extensions/" . $_GET[ext] . ".disabled");
	break;
	case 'perfhandler_graph':
		$btl->hasServerorServiceRight($_GET[service_id]);
		$r=$btl->updatePerfHandler($_GET[server_id], $_GET[service_id]);
		$r .='<br><br><span style="font-weight: bold;">Next Steps:<br /></span><ul><li><a href="service_detail.php?service_place=' . $svc[shm_place] . '">Back to the Service</a></li></ul>';
		$global_msg["output"]=$r;
		//get perfhandler_dir
		
		
		
	break;
	
	case 'delete_downtime':
		$layout->set_menu("downtimes");
		if($_GET[downtime_id]) {
			$rdt = bartlby_delete_downtime($btl->CFG, $_GET[downtime_id]);	
			$layout->OUT .= "<script>doReloadButton();</script>";
			
		} else {
			$act="missing_param";
		}
	break;
	case 'modify_downtime':
		$layout->set_menu("downtimes");
		if($_GET[downtime_notice] && $_GET[downtime_from] && $_GET[downtime_to] && $_GET[downtime_type]) {
			sscanf($_GET[downtime_from],"%d.%d.%d %d:%d", &$d, &$m, &$y, &$h, &$s);
			$dfrom=mktime($h, $s, 0, $m, $d,$y);
			sscanf($_GET[downtime_to],"%d.%d.%d %d:%d", &$d, &$m, &$y, &$h, &$s);
			$dto=mktime($h, $s, 0, $m, $d,$y);
			$clean_service=str_replace("s", "", $_GET[service_id]);
			
			$rdt=bartlby_modify_downtime($btl->CFG, $dfrom, $dto, $_GET[downtime_type], $_GET[downtime_notice], $clean_service, $_GET[downtime_id]);
			
			$layout->OUT .= "<script>doReloadButton();</script>";
		} else {
			$act="missing_param";
		}
	break;
	case 'add_downtime':
		$layout->set_menu("downtimes");
		if($_GET[downtime_notice] && $_GET[downtime_from] && $_GET[downtime_to] && $_GET[downtime_type]) {
			$msg = $_GET[downtime_from];
			sscanf($_GET[downtime_from],"%d.%d.%d %d:%d", &$d, &$m, &$y, &$h, &$s);
			$dfrom=mktime($h, $s, 0, $m, $d,$y);
			sscanf($_GET[downtime_to],"%d.%d.%d %d:%d", &$d, &$m, &$y, &$h, &$s);
			$dto=mktime($h, $s, 0, $m, $d,$y);
			
			$clean_service=str_replace("s", "", $_GET[service_id]);
			
			$notice=$_GET[lappend] . " " . $_GET[downtime_notice];
			
			$rdt=bartlby_add_downtime($btl->CFG, $dfrom, $dto, $_GET[downtime_type], $notice, $clean_service);
			
			$layout->OUT .= "<script>doReloadButton();</script>";
			
		}  else {
			$act="missing_param";
		}
	break;
	
	case 'edit_cfg':
		$layout->set_menu("core");
		
		
		switch($_POST[ecfg]) {
			case 'bartlby.cfg':
				$cfgfile=$btl->CFG;
			break;	
			case 'ui-extra.conf':
				$cfgfile="ui-extra.conf";
			break;
		}
		
		
		if($_POST["cfg_file"]) {
			if (get_magic_quotes_gpc()) {
				$_POST["cfg_file"] = stripslashes($_POST["cfg_file"]);
			}
			$new_cfg=$_POST["cfg_file"];
			//Backup current
			$backup_name=$cfgfile . ".bak_" . date("d-m-Y_H_i_s");
			$global_msg[backup_cfg]=$backup_name;
		
			copy($cfgfile, $backup_name);
			$fp=fopen($cfgfile, "w");
			$new_cfg=str_replace("\r", "", $new_cfg);
			fwrite($fp, $new_cfg);
			fclose($fp);
		}  else {
			$act="missing_param";
		}
		
	break;
	
	case 'submit_passive':
		
		if(!preg_match("/^XML.*$/i", $_GET[service_id])) {
			$btl->hasServerorServiceRight($_GET[service_id]);
			if($_GET[passive_text]) {
				$global_msg=bartlby_get_service_by_id($btl->CFG, $_GET[service_id]);
				$idx=$btl->findSHMPlace($_GET[service_id]);
				$global_msg[readable_state]="<font color=" . $btl->getColor($_GET[passive_state]) . ">" . $btl->getState($_GET[passive_state]) . "</font>";
				bartlby_set_passive($btl->CFG, $idx, $_GET[passive_state], $_GET[passive_text]);
			} else {
				$act="missing_param";
			}
		} else {
			$act="xml_remote";	
		}
		
	break;
	
	case 'stop':
		$layout->set_menu("core");
		$base_dir=bartlby_config($btl->CFG, "basedir");
		if(!$base_dir) {
			$act="missing_param";
		} else {
			$cmd="export BARTLBY_HOME='$base_dir'; cd \$BARTLBY_HOME; ./bartlby.startup stop 2>&1";
			
			$fp=popen($cmd, "r");
			$msg=fgets($fp, 1024);
			pclose($fp);	
		}
	break;
	case 'delete_package':
		$layout->set_menu("packages");
		if($_GET[package_name]) {
			
			unlink("pkgs/" . $_GET[package_name]);
		} else {
			$act="missing_param";
		}
	break;
	case 'delete_package_ask':
		$layout->set_menu("packages");
		$global_msg[button] .= "<input type=button value='yes' onClick=\"document.location.href='bartlby_action.php?action=delete_package&package_name=" . $_GET[package_name] . "'\">";
	break;
	
	case 'ack_problem':
	case 'add_comment':
		$btl->hasServerorServiceRight($_GET[service_id]);
		$global_msg=bartlby_get_service_by_id($btl->CFG, $_GET[service_id]);
		$layout->set_menu("main");
		if($act == 'ack_problem') {
			$_GET[subject]="Acknowledge of problem";
			$_GET[notify][0]=2;
			$idx=$btl->findSHMPlace($_GET[service_id]);
			bartlby_ack_problem($btl->CFG, $idx);
				
		}
		if($_GET[subject] && $_GET[comment]) {
			
			$fp=@fopen("comments/" . (int)$_GET[service_id], "a+");
			if(!$fp) {
				$act="missing_param";	
			} else {
				fwrite($fp, $btl->user . "|" . time() . "|" . str_replace("\n", " ", nl2br($_GET[comment])) . "|" . str_replace("\n", " ", nl2br($_GET[subject])) . "|" . implode(",", $_GET[notify]). "\n");
				fclose($fp);
				
			}
		} else {
			$act="missing_param";
		}
		
	
	break;
	case 'uninstall_package':
		$btl->hasServerRight($_GET[server_id]);
		
		$layout->set_menu("packages");
		$global_msg["package"] = "Removing package '$_GET[package_name]' from Server:  $_GET[server_id]<br>";
		$fp=@fopen("pkgs/" . $_GET[package_name], "r");
		if($fp) {
			while(!feof($fp)) {
				$bf .= fgets($fp, 1024);	
			}
			$re=unserialize($bf);
			fclose($fp);
			for($y=0; $y<$btl->info[services]; $y++) {
				$svc=bartlby_get_service($btl->CFG, $y);
				if($svc[server_id] == $_GET[server_id]) {
					for($x=0; $x<count($re); $x++) {
						
						if($svc[service_name] == $re[$x][service_name] && $svc[plugin] == $re[$x][plugin] && $svc[plugin_arguments] == $re[$x][plugin_arguments] && $svc[check_interval] ==  $re[$x][check_interval] && $svc[service_type] == $re[$x][service_type] && $svc[exec_plan] == $re[$x][exec_plan]) {
							
							$global_msg["package"] .= "Removing Service: <b>" . $re[$x][service_name] . "</b><br>";	
							$global_msg["package"] .= str_repeat("&nbsp;", 20) . "Plugin:" . $re[$x][plugin] . "/'" . $re[$x][plugin_arguments] . " '<br>";	
							$global_msg["package"] .= str_repeat("&nbsp;", 20) . "Time: $tfrom - $tto / " . $re[$x][check_interval] . "<br>";	
							$global_msg["package"] .= str_repeat("&nbsp;", 20) . "Service Type: " . $re[$x][service_type] . "<br>";
							bartlby_delete_service($btl->CFG, $svc[service_id]);
							$found++;
						}
					}
				}
			}
			$layout->OUT .= "<script>doReloadButton();</script>";
		} else {
			$global_msg["package"] = "fopen failed()!!<br>";
			$act="missing_param";	
		}		
	break;
	case 'install_package':
		$btl->hasServerRight($_GET[server_id]);
		
		$layout->set_menu("packages");
		if($_GET[package_name] && $_GET[server_id]) {
			$global_msg["package"] = $btl->installPackage($_GET[package_name], $_GET[server_id], $_GET[force_plugins], $_GET[force_perf]);
			$layout->OUT .= "<script>doReloadButton();</script>";
		}  else {
			$act="missing_param";
		}
		
	break;
	case 'create_package':
		$layout->set_menu("packages");
		$global_msg[pkg_services]="";
		if($_GET[services]) {
			$global_msg[pkg_services]=$btl->create_package($_GET[package_name], $_GET[services], $_GET[package_with_plugins], $_GET[package_with_perf]);
		} else {                                     
		 	$act="missing_param";
		 }                            
	break;

	case 'force_check':
		$layout->set_menu("main");
		if(!preg_match("/^XML.*$/i", $_GET[service_id])) {
			if($_GET[service_id]) {
				$btl->hasServerorServiceRight($_GET[service_id]);
				$global_msg=bartlby_get_service_by_id($btl->CFG, $_GET[service_id]);
				$idx=$btl->findSHMPlace($_GET[service_id]);
				
				$cur=bartlby_check_force($btl->CFG, $idx);
				
			} else {                                     
			 	$act="missing_param";
			}  
		} else {
		 	$act="xml_remote";	
		 }   
		
	break;
	case 'sirene_enable':
	case 'sirene_disable':
		$layout->set_menu("main");
		$r=bartlby_toggle_sirene($btl->CFG);
		 $global_msg[intervall]=bartlby_config($btl->CFG, "sirene_interval");
		if(!$global_msg[intervall]) {
			 $global_msg[intervall]=600;
		}
	break;
	


	case 'reload':
		$layout->set_menu("core");
		$btl->doReload();
		$msg="done";
	break;
	case 'delete_worker':
		$layout->set_menu("worker");
		if($_GET[worker_id]) {
			if(!$btl->isSuperUser() && $btl->user_id != $_GET[worker_id]) {
				$btl->hasRight("modify_all_workers");
			}
			$global_msg=bartlby_get_worker_by_id($btl->CFG, $_GET[worker_id]);
			$d=bartlby_delete_worker($btl->CFG, $_GET[worker_id]);
			
			$layout->OUT .= "<script>doReloadButton();</script>";

		} else {                                     
		 	$act="missing_param";
		 }     
	break;
	case 'modify_worker':
		$layout->set_menu("worker");
		if($_GET[worker_id] >= 0 && $_GET[worker_name]) {
			
			if(!$btl->isSuperUser() && $btl->user_id != $_GET[worker_id]) {
				$btl->hasRight("modify_all_workers");
			}
			$wks = $btl->GetWorker(false);
			for($x=0; $x<count($wks); $x++) {
			
				if($wks[$x][name] == $_GET[worker_name] && $wks[$x][worker_id] != $_GET[worker_id]) {
					$act = 'worker_exists';	
					break 2;
				}	
			}
			
			$svcstr = "";
			for($x=0;$x<count($_GET[worker_services]); $x++) {
                                $svcstr .="" . $_GET[worker_services][$x] . "|";
                        }
                        if($svcstr != "") {
                                $svcstr = "|"  . $svcstr;
                        }

			
			$selected_servers="";
			$selected_services="";
			
			for($x=0;$x<count($_GET[worker_services]); $x++) {
				if($_GET[worker_services][$x]{0} == 's') {
					$cl = str_replace("s", "", $_GET[worker_services][$x]);
					$selected_servers .= "," . $cl;

				} else {
					$selected_services .= "," . $_GET[worker_services][$x];

				}	
			}
			$selected_servers .= ",";
			$selected_services .= ",";
			
			
			
			for($x=0;$x<count($_GET[notify]); $x++) {
				$notifystr .="" . $_GET[notify][$x] . "|";	
			}
			if($notifystr != "") {
				$notifystr = "|" . $notifystr;
			}
			$triggerstr="";
			for($x=0;$x<count($_GET[worker_triggers]); $x++) {
				$triggerstr .="" . $_GET[worker_triggers][$x] . "|";	
			}
			if($triggerstr != "") {
				$triggerstr = "|" . $triggerstr;
			}
			
			$exec_plan = "";
			$df=false;
			while(list($k, $v) = each($_GET[wdays_plan])) {
				if($v != "") {
					$df = true;		
				}
				$exec_plan .= $k . "=" . $v . "|";	
			}
			
			if($df == false) {
				$exec_plan="";	
			}
			$end_pw= md5($_GET[worker_password]);
			
			if(!$_GET[worker_password]) {
					$wrk1 = bartlby_get_worker_by_id($btl->CFG, $_GET[worker_id]);
					$end_pw=$wrk1[password];
			}
			
			
			$add=bartlby_modify_worker($btl->CFG,$_GET[worker_id],  $_GET[worker_mail], $_GET[worker_icq], $svcstr, $notifystr, $_GET[worker_active], $_GET[worker_name],$end_pw, $triggerstr, $_GET[escalation_limit], $_GET[escalation_minutes], $exec_plan);
			$btl->setUIRight("selected_servers", $selected_servers, $_GET[worker_name]);
			$btl->setUIRight("selected_services", $selected_services, $_GET[worker_name]);
			$layout->OUT .= "<script>doReloadButton();</script>";

		} else {                                     
		 	$act="missing_param";
		}     
	break;
	case 'add_worker':
		
		$layout->set_menu("worker");
		if($_GET[worker_name] && $_GET[worker_mail] && $_GET[worker_password]) {
			//Check if worker exists
			$wks = $btl->GetWorker(false);
			for($x=0; $x<count($wks); $x++) {
			
				if($wks[$x][name] == $_GET[worker_name]) {
					$act = 'worker_exists';	
					break 2;
				}	
			}
			
			
			$msg = "wa:" .  $_GET[worker_active] . "\n";
			
			for($x=0;$x<count($_GET[worker_services]); $x++) {
				$svcstr .="" . $_GET[worker_services][$x] . "|";	
			}
			if($svcstr != "") {
				$svcstr = "|"  . $svcstr;
			}
			
			for($x=0;$x<count($_GET[notify]); $x++) {
				$notifystr .="" . $_GET[notify][$x] . "|";	
			}
			if($notifystr != "") {
				$notifystr = "|" . $notifystr;
			}
			$triggerstr="";
			for($x=0;$x<count($_GET[worker_triggers]); $x++) {
				$triggerstr .="" . $_GET[worker_triggers][$x] . "|";	
			}
			if($triggerstr != "") {
				$triggerstr = "|" . $triggerstr;
			}
			$exec_plan = "";
			$df=false;
			while(list($k, $v) = each($_GET[wdays_plan])) {
				if($v != "") {
					$df = true;		
				}
				$exec_plan .= $k . "=" . $v . "|";	
			}
			
			if($df == false) {
				$exec_plan="";	
			}
			
			
			$add=bartlby_add_worker($btl->CFG, $_GET[worker_mail], $_GET[worker_icq], $svcstr, $notifystr, $_GET[worker_active], $_GET[worker_name], md5($_GET[worker_password]), $triggerstr, $_GET[escalation_limit], $_GET[escalation_minutes], $exec_plan);
			
			$layout->OUT .= "<script>doReloadButton();</script>";
			
		} else {                                     
		 	$act="missing_param";
		}     
	break;
	case 'delete_service':
		$layout->set_menu("services");
		if($_GET[service_id]) {
			$btl->hasServerorServiceRight($_GET[service_id]);
			$global_msg=bartlby_get_service_by_id($btl->CFG, $_GET[service_id]);
			$del = bartlby_delete_service($btl->CFG, $_GET[service_id]);
			$layout->OUT .= "<script>doReloadButton();</script>";
		} else {                                     
		 	$act="missing_param";
		 }     
	break;
	case 'modify_service':
		set_magic_quotes_runtime(0);
		$layout->set_menu("services");
		
		if($_GET[service_id] != "" && $_GET[service_id] && $_GET[service_server] && $_GET[service_type] &&  $_GET[service_name] &&  $_GET[service_interval] != "") {
			$btl->hasServerorServiceRight($_GET[service_id]);
			$df=false;
			$exec_plan = "";
			while(list($k, $v) = each($_GET[wdays_plan])) {
				if($v != "") {
					$df = true;		
				}
				if($_GET[wdays_inv][$k]) {
					$sepa = "!";
				} else {
					$sepa = "=";	
				}
				$exec_plan .= $k . $sepa . $v . "|";	
			}
			
			
			if($df == false) {
				$exec_plan="";	
			}
			
			
			$ads=bartlby_modify_service($btl->CFG, $_GET[service_id] , $_GET[service_server], $_GET[service_plugin],$_GET[service_name],$_GET[service_args],$_GET[notify_enabled], $exec_plan,$_GET[service_interval],$_GET[service_type],$_GET[service_var], $_GET[service_passive_timeout], $_GET[service_check_timeout], $_GET[service_ack], $_GET[service_retain], $_GET[service_snmp_community], $_GET[service_snmp_objid], $_GET[service_snmp_version], $_GET[service_snmp_warning], $_GET[service_snmp_critical], $_GET[service_snmp_type], $_GET[service_active], $_GET[flap_seconds], $_GET[renotify_interval], $_GET[escalate_divisor]);
			$global_msg=bartlby_get_server_by_id($btl->CFG, $_GET[service_server]);
			$global_msg[exec_plan]=$btl->resolveServicePlan($exec_plan);
			
			if(strlen($_GET["unlock"]) > 0) {
				bartlby_toggle_service_active($btl->CFG, $_GET["unlock"], 0);
			}
			$act="service_" . $_GET[service_type];
			
			if($_GET[service_type] == 3) {
				$global_msg[group_out] = $btl->resolveGroupString($_GET[service_var]);				
			}
			
			
			$layout->OUT .= "<script>doReloadButton();</script>";
		} else {                                     
		 	$act="missing_param";
		}     
	break;
	case 'add_service': 
		$layout->set_menu("services");
		if($_GET[service_server] && $_GET[service_type] &&  $_GET[service_name]  && $_GET[service_interval]) {
						
						//&min_from,
						//&min_to,&check_interva	l, &service_type,&service_var,&service_passive_timeout
			
			set_magic_quotes_runtime(0);
			
			$exec_plan = "";
			$df=false;
			while(list($k, $v) = each($_GET[wdays_plan])) {
				if($v != "") {
					$df = true;		
				}
				if($_GET[wdays_inv][$k]) {
					$sepa = "!";
				} else {
					$sepa = "=";	
				}
				$exec_plan .= $k . $sepa . $v . "|";		
			}
			
			if($df == false) {
				$exec_plan="";	
			}
			
			for($x = 0; $x<count($_GET[service_server]); $x++) {
				$server_id=$_GET[service_server][$x];
				$ads=bartlby_add_service($btl->CFG, $server_id, $_GET[service_plugin],$_GET[service_name],$_GET[service_args],$_GET[notify_enabled], $exec_plan,$_GET[service_interval],$_GET[service_type],$_GET[service_var], $_GET[service_passive_timeout], $_GET[service_check_timeout], $_GET[service_ack], $_GET[service_retain], $_GET[service_snmp_community], $_GET[service_snmp_objid], $_GET[service_snmp_version], $_GET[service_snmp_warning], $_GET[service_snmp_critical], $_GET[service_snmp_type],$_GET[service_active], $_GET[flap_seconds],$_GET[renotify_interval], $_GET[escalate_divisor]);
				$tmp=bartlby_get_server_by_id($btl->CFG, $server_id);

				$global_msg[server_name] .= $tmp[server_name] . ",";
			

			}	
			
			$global_msg[exec_plan]=$btl->resolveServicePlan($exec_plan);
			$act="service_" . $_GET[service_type];
			if($_GET[service_type] == 3) {
				$global_msg[group_out] = $btl->resolveGroupString($_GET[service_var]);				
			}
			$layout->OUT .= "<script>doReloadButton();</script>";
		} else {                                     
		 	$act="missing_param";
		}     
	break;
	
	case 'delete_server':
		$layout->set_menu("client");
		if($_GET[server_id]) {
			$btl->hasServerRight($_GET[server_id]);
			$global_msg=bartlby_get_server_by_id($btl->CFG, $_GET[server_id]);
			
			$s = bartlby_delete_server($btl->CFG, $_GET[server_id]);
			
			$layout->OUT .= "<script>doReloadButton();</script>";
		} else {                                     
		 	$act="missing_param";
		}     
	break;
	case 'modify_server':
		$layout->set_menu("client");
		if($_GET[server_id] && $_GET[server_name] && $_GET[server_port] && $_GET[server_ip] && $_GET[server_icon]) {
				$btl->hasServerRight($_GET[server_id]);
				// text_service_search1 == dead marker
				$mod_server=bartlby_modify_server($btl->CFG, $_GET[server_id], $_GET[server_name], $_GET[server_ip], $_GET[server_port], $_GET[server_icon], $_GET[server_enabled], $_GET[server_notify], $_GET[server_flap_seconds], $_GET["text_service_search1"]);
				$defaults=bartlby_get_server_by_id($btl->CFG, $_GET[server_id]);
				$layout->DisplayHelp(array(0=>"CRIT|You should restart bartlby for applieng changes "));
				$layout->OUT .= "<script>doReloadButton();</script>";
		} else {                                     
			$act="missing_param";
		}     
	break;
	case 'add_server':
		$layout->set_menu("client");
			if($_GET[server_name] && $_GET[server_port] && $_GET[server_ip] && $_GET[server_icon]) {
				
				$add_server=bartlby_add_server($btl->CFG, $_GET[server_name], $_GET[server_ip], $_GET[server_port], $_GET[server_icon],$_GET[server_enabled], $_GET[server_notify], $_GET[server_flap_seconds], 0);
				
				$global_msg["package"]="";
				$global_msg["init_service"]="";
				
				
				if($_GET[package_name] != "") {
					$global_msg["package"].= "<br>" . $btl->installPackage($_GET[package_name], $add_server, NULL, NULL);	
				} else {
					$add_service=bartlby_add_service($btl->CFG, $add_server, "INIT", "Initial Check", "-h", 0, "" ,2000,1,"",200, 20, 0, 3, "", "", "", "", "", "", 1, 120,0,0);
					$global_msg["init_service"]="<li>Init";
				}
				
				$layout->OUT .= "<script>doReloadButton();</script>";
			} else {                                     
		 		$act="missing_param";
			}     
	break;
	case 'storeMap':
		$layout->set_menu("services");
		$st=$_POST[storeString];
		$st=str_replace("\$", "\\$", $st);
		$st=str_replace("\\'", "'", $st);
		$fp=fopen("create_map.dat", "w");
		fwrite($fp, $st);
		fclose($fp);
	
	break;
	default:
		$act="unkw_action";
		$msg="Action not implemented ($act)";
		$layout->set_menu("core");
	break;
		
}

$get_line="";
$post_line="";
$global_line="";
while(list($k, $v) = @each($_GET)) {
		$get_line .= "GET:::" . $k . ":'" . $v . "' ";
}
while(list($k, $v) = @each($_POST)) {
		$post_line .= "POST:::" . $k . ":'" . $v . "' ";
}
while(list($k, $v) = @each($global_msg)) {
		$global_line .= "GM:::" . $k . ":'" . $v . "' ";
}
@reset($_GET);
@reset($_POST);
@reset($global_msg);
if($act != "edit_cfg" && @bartlby_config("ui-extra.conf", "ui_event_log") == "true") {
	$btl->_log("UI Event: (action->$act IP->" .  $_SERVER[REMOTE_ADDR] . ")\\dbr $get_line $post_line \\dbr $global_line");
}

$f=$act;

$msg=$btl->finScreen($f);
$ov .=  $layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					'show'=>$msg
					)
			)
		)

, true);
$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					'show'=>"<a href='overview.php'>Overview</A>"
					)
			)
		)

);

$content = "<table>" . $ov . "</table>";
$layout->create_box($layout->BoxTitle, $content, "Message");

$btl->getExtensionsReturn("_POST_" . $act, $layout);
$layout->BoxTitle="";


$layout->TableEnd();
$layout->display("bartlby_action");
