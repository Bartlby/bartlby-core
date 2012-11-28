<?php
include('IXR_Library.inc.php');
include("bartlby-ui.class.php");
include "config.php";

$UNAME="";
$PW="";
$AUT_OK=false;

$btl = new BartlbyUi($Bartlby_CONF);
$map=$btl->GetSVCMap();



function xml_bartlby_get_svc_map($args) {
	global $btl, $map;
	return $map;
}

function xml_bartlby_get_service($args) {
	global $btl;
	return bartlby_get_service($btl->CFG, $args);
}
function xml_bartlby_get_service_with_id($args) {
	global $btl;
	$idx=$btl->findSHMPlace($args);
	return bartlby_get_service($btl->CFG, $idx);
}


function xml_bartlby_get_worker($args) {
	global $btl;
	return bartlby_get_worker($btl->CFG, $args);	
}
function xml_bartlby_get_info($args) {
	global $btl;
	return bartlby_get_info($btl->CFG );
}
function xml_bartlby_version($args) {
	global $btl;
	return bartlby_version($btl->CFG);	
}

function xml_bartlby_config($args) {
	global $btl;
	
	return bartlby_config($args[0], $args[1]);	
}
function xml_bartlby_lib_info($args) {
	global $btl;
	

	
	return bartlby_lib_info($btl->CFG);
}
function xml_bartlby_add_server($args) {
	global $btl;
	
	//NAME IP PORT ICON
		
	$server=bartlby_add_server($btl->CFG, $args[0], $args[1], $args[2], $args[3]);
	$add_service=bartlby_add_service($btl->CFG, $server, "INIT", "Initial Check", "-h", 0, 0,24,0,59,2000,1,"",200, 20, 0);
	return $server;
}
function xml_bartlby_delete_server($args) {
	global $btl;
	return bartlby_delete_server($btl->CFG, $args);	
}
function xml_bartlby_modify_server($args) {
	global $btl;
	//SERVERID NAME IP PORT ICON
	
	$mod_server=bartlby_modify_server($btl->CFG, $args[0], $args[1], $args[2], $args[3], $args[4]);
	return $mod_server;	
}

function xml_bartlby_get_server_by_id($args) {
	global $btl;
	return bartlby_get_server_by_id($btl->CFG, $args);	
}
function xml_bartlby_decode($args) {
	return bartlby_decode($args);	
}
function xml_bartlby_encode($args) {
	return bartlby_encode($args);	
}
function xml_bartlby_add_service($args) {
	global $btl;
	//SERVERID, PLUGIN, SERVICE_NAME, PLUGIN ARGS, HOURFROM (00), HOURTO(00), MINFROM (00), MINTO(00), service_interval, service_type, GROUPSTR, PASSIVE_TIMEOUT, CHECK_TIMEOUT, SERVICE_ACK
	$ads=bartlby_add_service($btl->CFG, $args[0], $args[1],$args[2],$args[3],1, $args[4], $args[5], $args[6], $args[7],$args[8],$args[9],$args[10],$args[11],$args[12],$args[13],$args[14],$args[15],$args[16],$args[17],$args[18],$args[19]);	
	return $ads;
}
function xml_bartlby_delete_service($args) {
	global $btl;
	return bartlby_delete_service($btl->CFG, $args);	
}

function xml_bartlby_modify_service($args) {
	global $btl;
	// ID, SERVER, PLUGIN, NAME, PLUGIN ARGS, HOURFROM (00), HOURTO(00), MINFROM (00), MINTO(00), INTERVALL, TYPE, GROUPSTR, PASSIVE_TIMEOUT, CHECK_TIMEOUT, SERVICE_ACK
	$ads=bartlby_modify_service($btl->CFG, $args[0] , $args[1], $args[2],$args[3],$args[4],1, $args[5], $args[6], $args[7], $args[8],$args[9],$args[10],$args[11], $args[12], $args[13], $args[14]);
	return $ads;
}
function xml_bartlby_get_service_by_id($args) {
	global	$btl;
	return bartlby_get_service_by_id($btl->CFG, $args);
}
function xml_bartlby_add_worker($args) {
	global $btl;
	//MAIL, ICQ, SVC String, notify string, ACTIVE?, NAME, PASSWORD, TRIGGER string
	$add=bartlby_add_worker($btl->CFG, $args[0], $args[1], $args[2], $args[3], $args[4], $args[5], $args[6], $args[7]);
	return $add;
}

function xml_bartlby_delete_worker($args) {
	global $btl;
	return bartlby_delete_worker($btl->CFG, $args);
		
}

function xml_bartlby_modify_worker($args) {
	global $btl;	
	$mod=bartlby_add_worker($btl->CFG, $args[0], $args[1], $args[2], $args[3], $args[4], $args[5], $args[6], $args[7], $args[8]);
	return $mod;
}
function xml_bartlby_get_worker_by_id($args) {
	global $btl;
	return bartlby_get_worker_by_id($btl->CFG, $args);	
}
function xml_bartlby_add_downtime($args) {
	global $btl;
	//TIMESTAMP from, TIMESTAMP To, DT Type, NOTICE, service/server - id
	return bartlby_add_downtime($btl->CFG, $args[0], $args[1], $args[2], $args[3], $args[4]);	
}
function xml_bartlby_downtime_map($args) {
	global $btl;
	return bartlby_downtime_map($btl->CFG);	
}
function xml_bartlby_modify_downtime($args) {
	global $btl;
	//TIMESTAMP from, TIMESTAMP To, DT Type, NOTICE, service/server - id, DTid
	return bartlby_add_downtime($btl->CFG, $args[0], $args[1], $args[2], $args[3], $args[4], $args[5]);	
		
}
function xml_bartlby_delete_downtime($args) {
	global $btl;
	return bartlby_delete_downtime($btl->CFG, $args);	
}

function xml_bartlby_toggle_service_notify($args) {
	global $btl;
	$idx=$btl->findSHMPlace($args);
	$cur=bartlby_toggle_service_notify($btl->CFG, $idx);
	return $cur;
}

function xml_bartlby_toggle_service_active($args) {
	global $btl;
	$idx=$btl->findSHMPlace($args);
	$cur=bartlby_toggle_service_active($btl->CFG, $idx);	
	return $cur;
}

function xml_bartlby_toggle_sirene($args) {
	global $btl;
	return bartlby_toggle_sirene($btl->CFG);	
}
function xml_bartlby_reload($args) {
	global $btl;
	return bartlby_reload($btl->CFG);	
}
function xml_bartlby_shm_destroy($args) {
	global $btl;
	return bartlby_shm_destroy($btl->CFG);	
}
function xml_bartlby_ack_problem($args) {
	global $btl;
	$idx=$btl->findSHMPlace($args);
	$r=bartlby_ack_problem($btl->CFG, $idx);
	return $r;
}
function xml_bartlby_set_passive($args) {
	global $btl;
	$idx=$btl->findSHMPlace($args[0]);
	$r=bartlby_set_passive($btl->CFG, $idx, $args[1], $args[2]);
	return $r;
}

function xml_bartlby_log($args) {
	global $btl;
	$btl->_log($args);	
}

function xml_bartlby_find_service($args) {
	global $map;
	$ret=array(-1, $args[0], $args[1]);
	while(list($server, $services) = each($map)) {
		
		if($services[0][server_name] == $args[0]) {
			for($x=0; $x<count($services); $x++) {
				if($services[$x][service_name] == $args[1]) {
					$ret=$services[$x];
				}	
			}	
		}
	}
	reset($map);
	return $ret;
}

function xml_bartlby_login() {
	global $UNAME, $PW, $btl;
	
	$headers = apache_request_headers(); 
	$UNAME=$headers["Bartlby-User"];
	$PW=$headers["Bartlby-Pass"];
	
	$wrks=$btl->GetWorker();
		$auted=0;
		while(list($k, $v) = each($wrks)) {
			//$v1=bartlby_get_worker_by_id($this->CFG, $v[worker_id]);
			if($UNAME == $v[name] && $PW == $v[password]) {
				
				$auted=1;
			}
		}
		
		
	if ($auted==0) { 
		return -1;	    
	} else {
		$AUT_OK=true;
	}
	
	
	return "OK";
	
	//return new IXR_Error(4000, 'ha ha ha');
}
function xml_bartlby_force_service($args) {
	global $btl;
	
	$idx=$btl->findSHMPlace($args);
	$cur=bartlby_check_force($btl->CFG, $idx);	
	return $cur;
}

	$server = new IXR_Server(array(
	    'bartlby.login' => 'xml_bartlby_login',
	    'bartlby.get_service_map' => 'xml_bartlby_get_svc_map',
	    'bartlby.get_service' => 'xml_bartlby_get_service',
	    'bartlby.get_service_with_id' => 'xml_bartlby_get_service_with_id',
	    
	    'bartlby.get_worker' => 'xml_bartlby_get_worker',
	    'bartlby.get_info' => 'xml_bartlby_get_info',
	    'bartlby.version' => 'xml_bartlby_version',
	    'bartlby.config' => 'xml_bartlby_config',
	    'bartlby.lib_info' => 'xml_bartlby_lib_info',
	    'bartlby.add_server' => 'xml_bartlby_add_server',
	    'bartlby.delete_server' => 'xml_bartlby_delete_server',
	    'bartlby.modify_server' => 'xml_bartlby_modify_server',
	    'bartlby.get_server_by_id' => 'xml_bartlby_get_server_by_id',
	    'bartlby.encode' => 'xml_bartlby_encode',
	    'bartlby.decode' => 'xml_bartlby_decode',
	    'bartlby.add_service' => 'xml_bartlby_add_service',
	    'bartlby.delete_service' => 'xml_bartlby_delete_service',
	    'bartlby.modify_service' => 'xml_bartlby_modify_service',
	    'bartlby.get_service_by_id' => 'xml_bartlby_get_service_by_id',
	    'bartlby.add_worker' => 'xml_bartlby_add_worker',
	    'bartlby.delete_worker' => 'xml_bartlby_delete_worker',
	    'bartlby.modify_worker' => 'xml_bartlby_modify_worker',
	    'bartlby.get_worker_by_id' => 'xml_bartlby_get_worker_by_id',
	    'bartlby.add_downtime' => 'xml_bartlby_add_downtime',
	    'bartlby.downtime_map' => 'xml_bartlby_downtime_map',
	    'bartlby.modify_downtime' => 'xml_bartlby_modify_downtime',
	    'bartlby.delete_downtime' => 'xml_bartlby_delete_downtime',
	    'bartlby.toggle_service_notify' => 'xml_bartlby_toggle_service_notify',
	    'bartlby.toggle_service_active' => 'xml_bartlby_toggle_service_active',
	    'bartlby.toggle_sirene' => 'xml_bartlby_toggle_sirene',
	    'bartlby.reload' => 'xml_bartlby_reload',
	    'bartlby.shm_destroy' => 'xml_bartlby_shm_destroy',
	    'bartlby.ack_problem' => 'xml_bartlby_ack_problem',
	    'bartlby.log' => 'xml_bartlby_log',
	    'bartlby.set_passive' => 'xml_bartlby_set_passive',
	    'bartlby.find_service' => 'xml_bartlby_find_service',
	    'bartlby.force_service' => 'xml_bartlby_force_service'
	));



/*




 

  






  string(19) "bartlby_ack_problem"
}
*/