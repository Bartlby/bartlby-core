<?
function dnl($i) {
	return sprintf("%02d", $i);
}
function getGeoip($ip) {
	$fp=popen("geoiplookup $ip", "r");
	while(!feof($fp)) {
		$rmsg .= fgets($fp, 1024);	
	}
	$exi=pclose($fp);
	if($exi == 127) {
		return "(maybe you dont have 'geoiplookup' not installed or it is not in your PHP path)";
	} else {
		$a=explode(":",$rmsg);
		return $a[1];	
	}
		
}
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("main.server_detail");
$layout= new Layout();
$layout->set_menu("main");
$layout->setTitle("Services");

$defaults=bartlby_get_server_by_id($btl->CFG, $_GET[server_id]);
$btl->hasServerRight($_GET[server_id]);

if(!$defaults) {
	$btl->redirectError("BARTLBY::OBJECT::MISSING");
	exit(1);	
}

$map=$btl->getSVCMap($btl->CFG, NULL, NULL);
$server_map=$map[$_GET[server_id]];


if($btl->isServerUp($_GET[server_id], $map)) {
	$isup="<font color='green'>UP</font>";	
} else {
	$isup="<font color='red'>DOWN</font>";
}

$services_assigned="";

for($x=0; $x<count($server_map); $x++) {
	$dmp_info[$server_map[$x][current_state]] += 1;
	
}

while(list($k, $v) = each($dmp_info)) {
		$services_assigned .= "<a href='services.php?server_id=" . $_GET[server_id] . "&expect_state=" . $k . "'><font color='" . $btl->getColor($k) . "'>" . $btl->getState($k) . "</font></A>:" . $v . ",";
}

if($defaults["server_notify"]==1) {
	$noti_en="true";
} else {
	$noti_en="false";
}

if($defaults["server_enabled"]==1) {
	$server_en="true";
} else {
	$server_en="false";
}


$info_box_title='Server Info';  

$layout->create_box($info_box_title, $core_content, "server_detail_server_info", array(
										"service" => $defaults,
										"isup" => $isup,
										"notify_enabled" => $noti_en,
										"server_enabled" => $server_en,
										"map" => $map
										),
			"server_detail_server_info");


$info_box_title='Services';  
$layout->create_box($info_box_title, $core_content, "server_detail_services", array(
									"services_assigned" => $services_assigned,
									"server_map" => $server_map
									)
			, "server_detail_services");
	



$r=$btl->getExtensionsReturn("_serverDetail", $layout);

$layout->OUT .= $btl->getServerOptions($defaults, $layout);





$layout->display("server_detail");