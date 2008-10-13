<?
function dnl($i) {
	return sprintf("%02d", $i);
}
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("main.service_detail");
$layout= new Layout();
$layout->set_menu("main");
$layout->setTitle("Actions");

if($_GET[service_id]) {
	$_GET[service_place] = $btl->findSHMPlace($_GET[service_id]);	
}

$defaults=bartlby_get_service($btl->CFG, $_GET[service_place]);
$btl->hasServerorServiceRight($defaults[service_id]);

if(!$defaults) {
	$btl->redirectError("BARTLBY::OBJECT::MISSING");
	exit(1);	
}

if(!$btl->hasRight("view_service_output", false)) {
	$defaults[new_server_text] = "you are missing: view_service_output right";	
} 

$map=$btl->getSVCMap($btl->CFG, NULL, NULL);


$svc_color=$btl->getColor($defaults[current_state]);
$svc_state=$btl->getState($defaults[current_state]);

switch($defaults[service_ack]) {
	case 0:
		$needs_ack="no";
	break;
	case 1:
		$needs_ack="yes";
	break;		
	case 2:
		$needs_ack="outstanding <input type=button value='Acknowledge this problem' onClick=\"document.location.href='ack_service.php?service_id=" . $defaults[service_id]  . "';\">";
	break;
}


if($defaults[service_type] == 1) {
	$svc_type="Active";
}

if($defaults[service_type] == 2) {
	$svc_type="Passive";
}

if($defaults[service_type] == 3) {
	$svc_type="Group";
}
if($defaults[service_type] == 4) {
	$svc_type="Local";
}
if($defaults[service_type] == 5) {
	$svc_type="SNMP";
}
if($defaults[service_type] == 6) {
	$svc_type="NRPE";
}
if($defaults[service_type] == 7) {
	$svc_type="NRPE(ssl)";
}

if($defaults[service_type] == 8) {
	$svc_type="AgentV2";
}
if($defaults[service_type] == 9) {
	$svc_type="AgentV2(no-SSL)";
}

if($defaults["notify_enabled"]==1) {
	$noti_en="true";
} else {
	$noti_en="false";
}
if($defaults["service_active"]==1) {
	$serv_en="true";
} else {
	$serv_en="false";
}
//echo $defaults[last_notify_send] . "<br>";

if( $defaults[service_delay_sum] > 0 && $defaults[service_delay_count] > 0) {
	$svcDEL=round($defaults[service_delay_sum] / $defaults[service_delay_count], 2);
} else {
	$svcDEL=0;	
}



if( $defaults[service_time_sum] > 0 && $defaults[service_time_count] > 0) {
	$svcMS=round($defaults[service_time_sum] / $defaults[service_time_count], 2);
} else {
	$svcMS=0;	
}


$server_enabled="";

if($defaults[server_enabled] != 1) {
	$server_enabled=";<i>server disabled</i>";	
}

$server_noti_enabled="";

if($defaults[server_notify] != 1) {
	$server_noti_enabled=";<i>disabled via server</i>";	
}

if($defaults[check_starttime] != 0) {
	$currun=date("d.m.Y H:i:s", $defaults[check_starttime]) . " (PID: $defaults[check_is_running] )";
} else {
	$currun="<i>Currently not running</i>";	
}

$plan_box = $btl->resolveServicePlan($defaults[exec_plan]);


if($defaults[renotify_interval] != 0) {
		$renot_en="every " . $defaults[renotify_interval] . " runs ( " . $btl->intervall(($defaults[check_interval]*$defaults[renotify_interval])) . ") ";
} else {
		$renot_en="not enabled";
}
if($defaults[escalate_divisor] != 0) {
		$escal_en="every " . $defaults[escalate_divisor] . " runs (" . $btl->intervall(($defaults[check_interval]*$defaults[escalate_divisor])) . ")";
} else {
		$escal_en="not enabled";
}



$info_box_title='Service Info';  
$layout->create_box($info_box_title, $core_content, "service_detail_service_info", array(
											"service" => $defaults,
											"service_type" => $svc_type,
											"map" => $map,
											"server_enabled" => $server_enabled,
											"currently_running" => $currun,
											"renotify" => $renot_en,
											"escalate" => $escal_en,
											"server_notifications" => $server_noti_enabled,
											"server_enabled" => $server_enabled,
											"service_ms" => $svcMS,
											"service_delay" => $svcDEL,
											"service_enabled" => $serv_en,
											"notify_enabled" => $noti_en,
											"needs_ack" => $needs_ack,
											"color" => $svc_color,
											"state" => $svc_state,
											"check_plan" => $plan_box
											)
											
		, "service_detail_service_info");

if($defaults[is_downtime] == 1) {
	$info_box_title='Downtime';  
	$core_content = "";
	$layout->create_box($info_box_title, $core_content, "service_detail_downtime_notice", array("service" => $defaults), "service_detail_downtime_notice");
	
}

$info_box_title='Last Output';  
$layout->create_box($info_box_title, $core_content, "service_detail_status_text", array("service" => $defaults), "service_detail_status_text");

if($defaults[service_type] == 2 || $defaults[service_type] == 1 || $defaults[service_type] == 4 || $defaults[service_type] == 6 || $defaults[service_type] == 7|| $defaults[service_type] == 8  || $defaults[service_type] == 9){
	$info_box_title='Plugin settings';  
	$core_content = "";
	
	$layout->create_box($info_box_title, $core_content, "service_detail_plugin_info", array("service" => $defaults), "service_detail_plugin_info");
}


if($defaults[service_type] == 5){
	if($defaults[service_snmp_type]  == 1) {
		$snmp_type = "Lower";
	} 
	if($defaults[service_snmp_type]  == 2) {
		$snmp_type = "Greater";
	} 
	$info_box_title='SNMP Service';  
	$layout->create_box($info_box_title, $core_content, "service_detail_snmp", array("service"=>$defaults, "snmp_type"=>$snmp_type), "service_detail_snmp");
}


if($defaults[service_type] == 2){
	
		
		$ibox[0][c]="green";
		$ibox[0][v]=0;	
		$ibox[0][k]="OK";
		$ibox[1][c]="orange";        
		$ibox[1][v]=1;	  
		$ibox[1][k]="Warning";
		$ibox[2][c]="red";        
		$ibox[2][v]=2;	  
		$ibox[2][k]="Critical";
		
		$ibox[$defaults[current_state]][s]=1;
		$state_dropdown=$layout->DropDown("passive_state", $ibox);
		
		$info_box_title='Passive Service';  
		$layout->create_box($info_box_title, $core_content, "service_detail_passive", array("service" => $defaults, "state_dropdown" => $state_dropdown), "service_detail_passive");
}
if($defaults[service_type] == 3){
	$info_box_title='Group Service';  
	$layout->create_box($info_box_title, $core_content, "service_detail_group_check", array("service" => $defaults), "service_detail_group_check");
}


$r=$btl->getExtensionsReturn("_serviceDetail", $layout);

$layout->OUT .= $btl->getserviceOptions($defaults, $layout);


$layout->display("service_detail");