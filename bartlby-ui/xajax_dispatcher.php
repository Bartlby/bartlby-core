<?
$do_not_merge_post_get=true;
include "layout.class.php";
include "bartlby-ui.class.php";
include "config.php";


$btl=new BartlbyUi($Bartlby_CONF);
$layout = new Layout();



$xajax->processRequests();



function toggle_extension($ext) {
	$res=new xajaxResponse();
	$fn = "extensions/" . $ext . ".disabled";
	if(!file_exists($fn)) {
		@touch($fn);
		//enable	
		$res->AddAssign("extension_" . $ext, "src", "images/extension_disable.gif");
		$res->AddAssign("extension_" . $ext, "title", "enable extension");
	} else {
		@unlink($fn);
		$res->AddAssign("extension_" . $ext, "src", "images/extension_enable.gif");
		$res->AddAssign("extension_" . $ext, "title", "disable extension");
		//disable extension_disable.gif
	}
	return $res;
}
function toggle_server_check($server_id, $service_id) {
	global $btl;
	$res = new xajaxresponse();
	if(!preg_match("/^XML.*$/i", $server_id)) {
		if($btl->hasServerorServiceRight($server_id, false)) {
			$gsm=bartlby_get_server_by_id($btl->CFG, $server_id);
			
			$cur=bartlby_toggle_server_active($btl->CFG, $gsm[server_shm_place], 1);
			
			if($cur == 1) { //Active
				//$res->addAlert("Check enabled on:" . $gsm[server_name] . ":" . $gsm[client_port] . "/" . $gsm[service_name]);
				$res->AddAssign("server_" . $server_id, "src", "images/enabled.gif");
				$res->AddAssign("server_" . $server_id, "title", "Disable Checks");
			} else {
				//$res->addAlert("Check disabled on:" . $gsm[server_name] . ":" . $gsm[client_port] . "/" . $gsm[service_name]);	
				$res->AddAssign("server_" . $server_id, "src", "images/diabled.gif");
				$res->AddAssign("server_" . $server_id, "title", "Enable Checks");
			}
			
			
			
			
		} else{
			$res->addAlert("permission denied");
		}
	
	} else {
		 $res->addAlert("action not possible on xml remote instances");
	}	
	return $res;
}




function toggle_server_notify_check($server_id, $service_id) {
	global $btl;
	$res = new xajaxresponse();
	if(!preg_match("/^XML.*$/i", $server_id)) {
		if($btl->hasServerorServiceRight($server_id, false)) {
			$gsm=bartlby_get_server_by_id($btl->CFG, $server_id);
			
			$cur=bartlby_toggle_server_notify($btl->CFG, $gsm[server_shm_place], 1);
			
			if($cur == 1) { //Active
				//$res->addAlert("Check enabled on:" . $gsm[server_name] . ":" . $gsm[client_port] . "/" . $gsm[service_name]);
				$res->AddAssign("trigger_" . $server_id, "src", "images/trigger.gif");
				$res->AddAssign("trigger_" . $server_id, "title", "disable notifications");
			} else {
				//$res->addAlert("Check disabled on:" . $gsm[server_name] . ":" . $gsm[client_port] . "/" . $gsm[service_name]);	
				$res->AddAssign("trigger_" . $server_id, "src", "images/notrigger.gif");
				$res->AddAssign("trigger_" . $server_id, "title", "enable trigger");
			}
			
			
			
			
		} else{
			$res->addAlert("permission denied");
		}
	
	} else {
		 $res->addAlert("action not possible on xml remote instances");
	}	
	return $res;
}
function toggle_service_notify_check($server_id, $service_id) {
	global $btl;
	$res = new xajaxresponse();
	if(!preg_match("/^XML.*$/i", $service_id)) {
		if($btl->hasServerorServiceRight($service_id, false)) {
			$gsm=bartlby_get_service_by_id($btl->CFG, $service_id);
			$idx=$btl->findSHMPlace($service_id);
			$cur=bartlby_toggle_service_notify($btl->CFG, $idx, 1);
			
			if($cur == 1) { //Active
				//$res->addAlert("Check enabled on:" . $gsm[server_name] . ":" . $gsm[client_port] . "/" . $gsm[service_name]);
				$res->AddAssign("trigger_" . $service_id, "src", "images/trigger.gif");
				$res->AddAssign("trigger_" . $service_id, "title", "disable notifications");
			} else {
				//$res->addAlert("Check disabled on:" . $gsm[server_name] . ":" . $gsm[client_port] . "/" . $gsm[service_name]);	
				$res->AddAssign("trigger_" . $service_id, "src", "images/notrigger.gif");
				$res->AddAssign("trigger_" . $service_id, "title", "enable trigger");
			}
			
			
			
			
		} else{
			$res->addAlert("permission denied");
		}
	
	} else {
		 $res->addAlert("action not possible on xml remote instances");
	}	
	return $res;
}

function toggle_service_check($server_id, $service_id) {
	global $btl;
	$res = new xajaxresponse();
	if(!preg_match("/^XML.*$/i", $service_id)) {
		if($btl->hasServerorServiceRight($service_id, false)) {
			$gsm=bartlby_get_service_by_id($btl->CFG, $service_id);
			$idx=$btl->findSHMPlace($service_id);
			$cur=bartlby_toggle_service_active($btl->CFG, $idx, 1);
			
			if($cur == 1) { //Active
				//$res->addAlert("Check enabled on:" . $gsm[server_name] . ":" . $gsm[client_port] . "/" . $gsm[service_name]);
				$res->AddAssign("service_" . $service_id, "src", "images/enabled.gif");
				$res->AddAssign("service_" . $service_id, "title", "Disable Checks");
			} else {
				//$res->addAlert("Check disabled on:" . $gsm[server_name] . ":" . $gsm[client_port] . "/" . $gsm[service_name]);	
				$res->AddAssign("service_" . $service_id, "src", "images/diabled.gif");
				$res->AddAssign("service_" . $service_id, "title", "Enable Checks");
			}
			
			
			
			
		} else{
			$res->addAlert("permission denied");
		}
	
	} else {
		 $res->addAlert("action not possible on xml remote instances");
	}	
	return $res;
}

function updatePerfHandler($script_after, $srv_id, $svc_id) {
	global $btl;
	$res = new xajaxResponse();
	$btl->updatePerfHandler($srv_id, $svc_id);
	$res->addScript($script_after);
	return $res;
}

function ExtensionAjax($ext, $func) {
	$res = new xajaxResponse();
	@include_once("extensions/$ext/" .$ext . ".class.php");
	@eval("\$clh = new " . $ext . "();");
	if(method_exists($clh, $func)) {
		eval("\$o = \$clh->" . $func. "();");
		$ex[ex_name]=$file;
		$ex[out] = $o;	
	}
	
	return $o;
		
}

function removeDIV($div) {
	$res = new xajaxResponse();
	$res->addClear($div, 'innerHTML', '');
	return $res;
}


function forceCheck($server, $service) {
	global $btl;
	$res = new xajaxresponse();
	if(!preg_match("/^XML.*$/i", $service)) {
		if($service) {
			if($btl->hasServerorServiceRight($service, false)) {
				$gsm=bartlby_get_service_by_id($btl->CFG, $service);
				$idx=$btl->findSHMPlace($service);
				$cur=bartlby_check_force($btl->CFG, $idx);
				$res->addAlert("immediate check scheduled for:" . $gsm[server_name] . ":" . $gsm[client_port] . "/" . $gsm[service_name]);
			} else {
				$res->addAlert("permission denied to force:" . $gsm[server_name] . ":" . $gsm[client_port] . "/" . $gsm[service_name]);
			}
		} else {                                     
		 	$res->addAlert("missing service_id");
		}  
	} else {
	 	$res->addAlert("force check isnt possible on xml remote services");
	}   
	return $res;
}

function group_search($g) {
	$res = new xajaxresponse();
	$optind=0;
	$y=0;
	
	
	$optind=0;
	$dh=opendir("extensions/ServerGroups/data/");
	while ($file = readdir ($dh)) { 
	   if ($file != "." && $file != "..") { 
	   	clearstatcache();
	   	$ta = explode(".",$file);
	   	$unser = base64_decode($ta[0]);
	   	
	   	
	   	if(preg_match("/" . $g . "/i", $unser)) {
	     	$output .= "<a href=\"javascript:void(0);\" onClick=\"document.location.href='extensions_wrap.php?script=ServerGroups/edit.php&group_name=" . $file . "'\">$unser</a><br>";
	     	$y++;
	     }
	       	
	   }
	   if($y>20) {
			break 2;	
		} 
	}
	closedir($dh); 
	$output = "<a href='javascript:void(0);' onClick=\"xajax_removeDIV('group_search_suggest');\">close</A><br><br>" . $output;
	$res->AddAssign("group_search_suggest", "innerHTML", $output);
	
	return $res;
}
function jumpToUserId($id, $script) {
	$res = new xajaxResponse();
	$res->addScript("document.location.href='$script?worker_id="  . $id . "'");
	return $res;
}
function AddDowntime($av) {
	$res = new xajaxResponse();
	$al = "";
	if(!bartlbize_field($av[downtime_notice])){
		$al="1";
		$res->addAssign("error_downtime_notice", "innerHTML", "required field");
	} else {
		$res->addAssign("error_downtime_notice", "innerHTML", "");
	}
	if(!bartlbize_field($av[downtime_from])){
		$al="1";
		$res->addAssign("error_downtime_from", "innerHTML", "required field");
	} else {
		$res->addAssign("error_downtime_from", "innerHTML", "");
	}
	if(!bartlbize_field($av[downtime_to])){
		$al="1";
		$res->addAssign("error_downtime_to", "innerHTML", "required field");
	} else {
		$res->addAssign("error_downtime_to", "innerHTML", "");
	}
	if($al == "") $res->AddScript("document.fm1.submit()");
	return $res;
}
function CreatePackage($av) {
	$res = new xajaxResponse();
	
	$al="";
	
	
	if(!bartlbize_field($av[package_name])){
		$al="1";
		$res->addAssign("error_package_name", "innerHTML", "required field");
	} else {
		$res->addAssign("error_package_name", "innerHTML", "");
		if(!$av[package_overwrite]) {
			if(file_exists("pkgs/" . $av[package_name])) {
				$res->addAssign("error_package_name", "innerHTML", "Package already exists");
				$al="1";
			} else {
				$res->addAssign("error_package_name", "innerHTML", "");
			}
		}
		
	}
	
	
	if(!is_array($av[services])) {
		$al="1";
		$res->addAssign("error_services[]", "innerHTML", "select services");
	} else {
		$res->addAssign("error_services[]", "innerHTML", "");
	}	
	
	if($al=="")
		$res->AddScript("document.fm1.submit()");
	
	return $res;	
}

function UserSearch($what, $script='modify_worker.php') {
	global $btl;
	$res = new xajaxResponse();
	
	$servs=$btl->GetWorker();
	$optind=0;
	$y=0;
	while(list($k, $v) = @each($servs)) {
		
		if(preg_match("/" . $what . "/i", $v[name])) {
			
			$output .= "<a  href=\"javascript:void(0);\" onClick=\"xajax_jumpToUserId('" . $v[worker_id] . "', '$script');\">$v[name]</a><br>";
			$y++;
		}
		if($y>20) {
			break 2;	
		}
	}
	
	$output = "<a href='javascript:void(0);' onClick=\"xajax_removeDIV('user_search_suggest');\">close</A><br><br>" . $output;
	$res->addAssign("user_search_suggest", "innerHTML", $output);
	return $res;	
}


function jumpToServiceId($id, $script) {
	$res = new xajaxResponse();
	$res->addScript("document.location.href='$script?service_id="  . $id . "'");
	return $res;
}
function set_service_search_noact($d, $v) {
	global $btl;
	
	$res = new xajaxResponse();
		
	$res->addAssign("text_" . $d, "value", $v);
	
	$svc = @bartlby_get_service_by_id($btl->CFG, $v);
	
	$res->addAssign("search_" . $d, "value", $svc[server_name] . "/" .  $svc[service_name]);
	$res->addAssign($d, "innerHTML", "");
	
	return $res;
	
}
function service_noaction($what, $d) {
	global $btl;
	$res = new xajaxResponse();
	
	
	
	$map = $btl->GetSVCMap();
	$optind=0;
	$y=0;
	
	while(list($k, $servs) = @each($map)) {
		$displayed_servers++;
		
		for($x=0; $x<count($servs); $x++) {
			$ostr=$servs[$x][server_name] . "/" . $servs[$x][service_name];
			if(@preg_match("/" . $what . "/i", $ostr)) {
				$output .= "<a href=\"javascript:void(0);\" onClick=\"xajax_set_service_search_noact('" . $d . "', '" . $servs[$x][service_id] . "');\">$ostr</a><br>";
				$y++;
			}		
			if($y>20) {
				break 2;	
			}
		}
	}
	
	
	$output = "<a href='javascript:void(0);' onClick=\"xajax_removeDIV('" . $d . "');\">close</A><br><br>" . $output;
	$res->addAssign($d, "innerHTML", $output);
	return $res;	
}


function ServiceSearch($what, $script='modify_service.php') {
	global $btl;
	$res = new xajaxResponse();
	
	$map = $btl->GetSVCMap();
	$optind=0;
	$y=0;
	
	while(list($k, $servs) = @each($map)) {
		$displayed_servers++;
		
		for($x=0; $x<count($servs); $x++) {
			$ostr=$servs[$x][server_name] . "/" . $servs[$x][service_name];
			if(preg_match("/" . $what . "/i", $ostr)) {
				$output .= "<a href=\"javascript:void(0);\" onClick=\"xajax_jumpToServiceId('" . $servs[$x][service_id] . "', '$script');\">$ostr</a><br>";
				$y++;
			}		
			if($y>20) {
				break 2;	
			}
		}
	}
	
	
	$output = "<a href='javascript:void(0);' onClick=\"xajax_removeDIV('service_search_suggest');\">close</A><br><br>" . $output;
	$res->addAssign("service_search_suggest", "innerHTML", $output);
	return $res;	
}


function jumpToServerId($id, $script) {
	$res = new xajaxResponse();
	$res->addScript("document.location.href='$script?server_id="  . $id . "'");
	return $res;
}


function PluginSearch($what) {
	global $btl;
	$res = new xajaxResponse();
	
	$optind=0;
	$y=0;
	
	
	$optind=0;
	$plgs=bartlby_config($btl->CFG, "agent_plugin_dir");
	$dh=opendir($plgs);
	while ($file = readdir ($dh)) { 
	   if ($file != "." && $file != "..") { 
	   	clearstatcache();
	   	if((preg_match("/\.exe$/i", $file)) || (is_executable($plgs . "/" . $file) && !is_dir($plgs . "/" . $file))) {
	   			if(preg_match("/" . $what . "/i", $file)) {
	       			$output .= "<a href=\"javascript:void(0);\" onClick=\"document.getElementById('service_plugin').value='" . $file . "';xajax_removeDIV('plugin_search_suggest');\">$file</a><br>";
	       			$y++;
	       		}
	       	}
	   }
	   if($y>20) {
			break;	
	   } 
	}
	closedir($dh); 
	
	$output = "<a href='javascript:void(0);' onClick=\"xajax_removeDIV('plugin_search_suggest');\">close</A><br><br>" . $output;
	$res->addAssign("plugin_search_suggest", "innerHTML", $output);
	return $res;	
}

function ServerSearch($what, $script='modify_server.php') {
	global $btl;
	$res = new xajaxResponse();
	$servs=$btl->GetServers();
	$optind=0;
	//$res=mysql_query("select srv.server_id, srv.server_name from servers srv, rights r where r.right_value=srv.server_id and r.right_key='server' and r.right_user_id=" . $poseidon->user_id);
	$y=0;
	while(list($k, $v) = @each($servs)) {
		
		if(preg_match("/" . $what . "/i", $v)) {
			$output .= "<a href=\"javascript:void(0);\" onClick=\"xajax_jumpToServerId('" . $k . "', '$script');\">$v</a><br>";
			$y++;
		}
		if($y>20) {
			break 2;	
		}
	}
	$output = "<a href='javascript:void(0);' onClick=\"xajax_removeDIV('server_search_suggest');\">close</A><br><br>" . $output;
	$res->addAssign("server_search_suggest", "innerHTML", $output);
	return $res;	
}
function QuickLook($what) {
	global $btl, $layout, $rq;
	//compat for extensions
	$_GET[search] = $what;
	$res = new xajaxResponse();
	
	$servers=$btl->GetSVCMap();	
	$_GET["servers"]=$servers;
	
	//Search Servers
	$rq .= "<table width=100%>";
	$rq .= "<tr>";
	$rq .= "<td colspan=2>";
	$rq .= "<center><b>Servers</b></center>";
	$rq .= "</td></tr>";
	$sfound=false;
	while(list($k, $v) = @each($servers)) {
		
		if(@preg_match("/" . $what . "/i", $v[0][server_name])) {
			
			$rq .= "<tr><td><a href='server_detail.php?server_id=" . $v[0][server_id] . "'><font size=1>" . $v[0][server_name] . "</font></A>(<a href='services.php?server_id=" . $v[0][server_id] . "'><font size=1>Services</font></A>)</td><td>" . $btl->getserveroptions($v[0], $layout) . "</td></tr>";	
			$sfound=true;
		}
		
		
		
	}	
	if($sfound == false) {
		$rq .= "<tr><td colspan=2><i>no server matched</i></td></tr>";	
	}
	
	$rq .= "</table>";
	$output .= $layout->create_box("Server", $rq);
	$rq = "<table width=100%>";
	
	$rq .= "<tr>";
	$rq .= "<td colspan=2>";
	$rq .= "<center><b>Services</b></center>";
	$rq .= "</td></tr>";
	
	reset($servers);
	$svcfound=false;
	while(list($k, $v) = @each($servers)) {
		
		
		
		for($x=0; $x<count($v); $x++) {
			if(@preg_match("/" . $what . "/i", $v[$x][server_name] . "/" . $v[$x][service_name])) {
				$rq .= "<tr><td><a href='service_detail.php?service_place=" . $v[$x][shm_place] . "'><font size=1>" . $v[$x][server_name] . "/" . $v[$x][service_name] . "</A></font></td><td>" . $btl->getServiceOptions($v[$x], $layout) . "</td>";	
				$svcfound=true;
			}
		}
		
		
	}	
	if($svcfound == false) {
		$rq .= "<tr><td colspan=2><i>no service matched</i></td></tr>";
	}
	
	
	$rq .= "</table>";
	$output .= $layout->create_box("Services", $rq);
	$rq = "";
	
	@reset($servers);
	$btl->getExtensionsReturn("_quickLook", false);
	if($rq == "") {
		$rq = "<tr><td colspan=2><i>no extension returned results</i></td></tr>";	
	}
	$rq = "<table width=100%>" . $_GET[rq];
	$rq .= "</table>";
	//Search Services	
		
	
	//Search Workers
	//Call n get return of Extensions
	$output .=  $layout->create_box("Extensions", $rq);
	$output = "<a href='javascript:void(0);' onClick=\"xajax_removeDIV('quick_suggest');\">close</A><br>" . $output;
	$res->addAssign("quick_suggest", "innerHTML", $output);
	return $res;	
}
function CreateReport($aFormValues) {
	global $_GET, $_POST;
	
	$av = $aFormValues;
	
	$al="";
	
	$res = new xajaxResponse();
	
	if(!bartlbize_field($av[report_start])){
		$al="1";
		$res->addAssign("error_report_start", "innerHTML", "required field");
	} else {
		$res->addAssign("error_report_start", "innerHTML", "");
	}
	if(!bartlbize_field($av[report_end])){
		$al="1";
		$res->addAssign("error_report_end", "innerHTML", "required field");
	} else {
		$res->addAssign("error_report_end", "innerHTML", "");
	}
	if($av[report_service] == "") {
		$al="1";
		$res->addAssign("error_report_service", "innerHTML", "choose a service");
	} else {
		$res->addAssign("error_report_service", "innerHTML", "");
	
	}
	if($al == "")  {
		$res->addScript("document.fm1.submit()");
	}
	
	return $res;	
	
	
	
}
function AddModifyClient($aFormValues) {
	global $_GET, $_POST;
	
	$av = $aFormValues;
	
	$res = new xajaxResponse();
	
	
	
	$al="";
	
	if(!bartlbize_field($av[server_name])) {
		$res->addAssign("error_server_name", "innerHTML", "You must specify a correct server name");
		$al="1";
	} else {
		$res->addAssign("error_server_name", "innerHTML", "");
	}
		
	if(!bartlbize_field($av[server_ip])) {
		$al="1";
		$res->addAssign("error_server_ip", "innerHTML", "You must specify a correct Server IP-Address");
	}else{
		$res->addAssign("error_server_ip", "innerHTML", "");
	}
		
	if(!bartlbize_int($av[server_flap_seconds])) {
		$al="1";
		$res->addAssign("error_server_flap_seconds", "innerHTML", "required field");
	} else {
		$res->addAssign("error_server_flap_seconds", "innerHTML", "");
	}
	
	if(!bartlbize_int($av[server_port])){
		$al="1";
		$res->addAssign("error_server_port", "innerHTML", "required field");
	} else {
		$res->addAssign("error_server_port", "innerHTML", "");
	}
	if($al == "")  {
		$res->addScript("document.fm1.submit()");
	}
	
	return $res;	
}


function AddModifyWorker($aFormValues) {
	global $_GET, $_POST;
	
	$av = $aFormValues;
	
	$res = new xajaxResponse();
	
	
	
	$al="";
	
	if(!bartlbize_field($av[worker_name])){
		$res->addAssign("error_worker_name", "innerHTML", "You must specify a correct worker name");
		$al="1";
	} else {
		$res->addAssign("error_worker_name", "innerHTML", "");
	}
	
	if($av[action] == 'add_worker' || ($av[worker_password] != "" && $av[worker_password1] != "")) {	
		if(!bartlbize_field($av[worker_password])){
			$res->addAssign("error_worker_password", "innerHTML", "You must specify a correct password");
			$al="1";
		} else {
			$res->addAssign("error_worker_password", "innerHTML", "");
		}
	}
	
	if($av[worker_password] != $av[worker_password1]) {
		$res->addAssign("error_worker_password1", "innerHTML", "Passwords dont match");
		$al="1";
	} else {
		$res->addAssign("error_worker_password1", "innerHTML", "");
	}
	
	if(!bartlbize_field($av[worker_mail], true)){
		$res->addAssign("error_worker_mail", "innerHTML", "required field");
		$al="1";
	} else {
		$res->addAssign("error_worker_mail", "innerHTML", "");
	}
	
	if(!bartlbize_int($av[worker_icq], true)){
		$res->addAssign("error_worker_icq", "innerHTML", "required field");
		$al="1";
	} else {
		$res->addAssign("error_worker_icq", "innerHTML", "");
	}
	
	if(!bartlbize_int($av[escalation_limit])){
		$res->addAssign("error_escalation_limit", "innerHTML", "required field");
		$al="1";
	} else {
		$res->addAssign("error_escalation_limit", "innerHTML", "");
	}
		
		
	if(!bartlbize_int($av[escalation_minutes])){
		$res->addAssign("error_escalation_minutes", "innerHTML", "required field");
		$al="1";
	} else {
		$res->addAssign("error_escalation_minutes", "innerHTML", "");
	}
	
	
	
	if($al == "")  {
		$res->addScript("document.fm1.submit()");
	}
	
	return $res;	
}

function AddModifyService($aFormValues) {
	global $_GET, $_POST;
	
	$av = $aFormValues;
	
	$res = new xajaxResponse();
	
	
	
	$al="";
	
	if(!bartlbize_field($av[service_name])){
		$res->addAssign("error_service_name", "innerHTML", "You must specify a correct service name");
		$al="1";
	} else {
		$res->addAssign("error_service_name", "innerHTML", "");
	}
	
	if(!bartlbize_int($av[service_interval])){
		$res->addAssign("error_service_interval", "innerHTML", "required field");
		$al="1";
	} else {
		$res->addAssign("error_service_interval", "innerHTML", "");
	}
	
	if(!bartlbize_int($av[service_retain])){
		$res->addAssign("error_service_retain", "innerHTML", "required field");
		$al="1";
	} else {
		$res->addAssign("error_service_retain", "innerHTML", "");
	}

	if(!bartlbize_int($av[flap_seconds])){
		$res->addAssign("error_flap_seconds", "innerHTML", "required field");
		$al="1";
	} else {
		$res->addAssign("error_flap_seconds", "innerHTML", "");
	}
	
	if(!bartlbize_int($av[escalate_divisor])){
		$res->addAssign("error_escalate_divisor", "innerHTML", "required field");
		$al="1";
	} else {
		$res->addAssign("error_escalate_divisor", "innerHTML", "");
	}
	if(!bartlbize_int($av[renotify_interval])){
		$res->addAssign("error_renotify_interval", "innerHTML", "required field");
		$al="1";
	} else {
		$res->addAssign("error_renotify_interval", "innerHTML", "");
	}
	
	
	switch($av[service_type]) {
		case 1:
		case 6:
		case 7:
		case 8:
		case 9:
		case 4:
		case 2:
			if(!bartlbize_int($av[service_check_timeout])){
				$res->addAssign("error_service_check_timeout", "innerHTML", "required field");
				$al="1";
			} else {
				$res->addAssign("error_service_check_timeout", "innerHTML", "");
			}
	
			if(!bartlbize_field($av[service_plugin])){
				$res->addAssign("error_service_plugin", "innerHTML", "required field");
				$al="1";
			} else {
				$res->addAssign("error_service_plugin", "innerHTML", "");
			}
			
			if(!bartlbize_field($av[service_args], true)){
				$res->addAssign("error_service_args", "innerHTML", "required field");
				$al="1";
			} else {
				$res->addAssign("error_service_args", "innerHTML", "");
			}
			
			if($av[service_type] == 2) {
				if(!bartlbize_int($av[service_passive_timeout])){
					$res->addAssign("error_service_passive_timeout", "innerHTML", "required field");
					$al="1";
				} else {
					$res->addAssign("error_service_passive_timeout", "innerHTML", "");
				}
			}	
			
		
				
		break;
		case 3:
			if(!bartlbize_field($av[service_var])){
				$res->addAssign("error_service_var", "innerHTML", "required field");
				$al="1";
			} else {
				$res->addAssign("error_service_var", "innerHTML", "");
			}
		break;	
		case 5:
			if(!bartlbize_field($av[service_snmp_community])){
				$res->addAssign("error_service_snmp_community", "innerHTML", "required field");
				$al="1";
			} else {
				$res->addAssign("error_service_snmp_community", "innerHTML", "");
			}
				
			
			
			if(!bartlbize_field($av[service_snmp_objid])){
				$res->addAssign("error_service_snmp_objid", "innerHTML", "required field");
				$al="1";
			} else {
				$res->addAssign("error_service_snmp_objid", "innerHTML", "");
			}
			
			if(!bartlbize_int($av[service_snmp_warning])){
				$res->addAssign("error_service_snmp_warning", "innerHTML", "required field");
				$al="1";
			} else {
				$res->addAssign("error_service_snmp_warning", "innerHTML", "");
			}
			if(!bartlbize_int($av[service_snmp_critical])){
				$res->addAssign("error_service_snmp_critical", "innerHTML", "required field");
				$al="1";
			} else {
				$res->addAssign("error_service_snmp_critical", "innerHTML", "");
			}
					
		break;
		
	
		
	}
	
	
	if($al == "")  {
		$res->addScript("document.fm1.submit()");
	}
	
	return $res;	
}
function bartlbize_date($v) {
	if($v == "") {
		return false;
	}
	if(!preg_match("/[0-9].+:[0-9].+:[0-9].+/i", $v)) {
		
		return false;	
	}
	return true;
	
}
function bartlbize_int($v, $n = false) {
	if($v == "") {
		return $n;
	}
	
	if(!preg_match("/^[0-9]+$/i", $v)) {
		return false;	
	}
	return true;
	
}


function bartlbize_field($v, $n=false) {
	if(!$n) {
		if($v == "") {
			return false;
		}
	}
	if(preg_match("/'/i", $v)) {
		return false;	
	}
	return true;
	
}

?>
