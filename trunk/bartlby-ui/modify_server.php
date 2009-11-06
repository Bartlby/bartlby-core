<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";



$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();


$layout->set_menu("client");

$ov .= $layout->Form("fm1", "bartlby_action.php", "GET", true);
$layout->Table("100%");
if($_GET[server_id]) {
	$btl->hasServerRight($_GET[server_id]);	
}


$defaults=@bartlby_get_server_by_id($btl->CFG, $_GET[server_id]);

$optind=0;
if(!is_dir("pkgs")) {
	mkdir("pkgs", 0777);	
}

$dhl=opendir("pkgs");
$packages[$optind][c]="";
$packages[$optind][v]="";	
$packages[$optind][k]="--None--";
$optind++;
while($file = readdir($dhl)) {
	//$sr=bartlby_get_server_by_id($btl->CFG, $k);
	
	//$isup=$btl->isServerUp($k);
	//if($isup == 1 ) { $isup="UP"; } else { $isup="DOWN"; }
	if(!is_dir("pkgs/" . $file)) {
		$packages[$optind][c]="";
		$packages[$optind][v]=$file;	
		$packages[$optind][k]="&raquo;" . $file;
		$optind++;
	}
}
closedir($dhl);


$fm_action="modify_server";
$layout->setTitle("Modify Server");
if($_GET["copy"] == "true") {
	$fm_action="add_server";
	$btl->hasRight("action.copy_server");
	$layout->setTitle("Copy Server");
}
if($_GET["new"] == "true") {
	$fm_action="add_server";
	$btl->hasRight("action.add_server");
	$layout->setTitle("Add Server");
	
	$defaults["min_from"]="00";
	$defaults["min_to"]="59";
	$defaults["hour_from"]="00";
	$defaults["hour_to"]="24";
	$defaults["server_dead"] = "0";
	
	$defaults[server_port]=(int)bartlby_config("ui-extra.conf", "new.server.port");
	$defaults[server_icon]=bartlby_config("ui-extra.conf", "new.server.icon");
	
	
}

//Notify Enabled
$notenabled[0][c]="";
$notenabled[0][v] = 0; //No
$notenabled[0][k] = "No"; //No
$notenabled[0][s]=0;

$notenabled[1][c]="";
$notenabled[1][v] = 1; //No
$notenabled[1][k] = "Yes"; //No
$notenabled[1][s]=0;

if(is_int($defaults[server_notify]) && $defaults[server_notify] == 0) {
	$notenabled[0][s]=1;	
	
} else {
	
	$notenabled[1][s]=1;
}

//Notify Enabled
$servactive[0][c]="";
$servactive[0][v] = 0; //No
$servactive[0][k] = "No"; //No
$servactive[0][s]=0;

$servactive[1][c]="";
$servactive[1][v] = 1; //No
$servactive[1][k] = "Yes"; //No
$servactive[1][s]=0;


if(is_int($defaults[server_enabled]) && $defaults[server_enabled] == 0) {
	$servactive[0][s]=1;	
	
} else {

	$servactive[1][s]=1;
}


if($fm_action == "modify_server") {
	$btl->hasRight("action.modify_server");	
}


if($defaults == false && $_GET["new"] != "true") {
	$btl->redirectError("BARTLBY::OBJECT::MISSING");
	exit(1);	
}
$optind=0;
$dhl=opendir("server_icons");
while($file = readdir($dhl)) {
	//$sr=bartlby_get_server_by_id($btl->CFG, $k);
	
	//$isup=$btl->isServerUp($k);
	//if($isup == 1 ) { $isup="UP"; } else { $isup="DOWN"; }
	if(preg_match("/.*\.[png|gif]/", $file)) {
		
		if($defaults[server_icon] == $file) {
			
			$server_icons[$optind][s]=1;
		}
		$server_icons[$optind][c]="";
		$server_icons[$optind][v]=$file;	
		$server_icons[$optind][k]="&raquo;" . $file;
		$optind++;
	}
	
}
closedir($dhl);

$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Server Name",
			1=>$layout->Field("server_name", "text", $defaults[server_name]) . $layout->Field("action", "hidden", $fm_action) . "<a href=\"javascript:var w=window.open('locate_server.php','','width=353,height=421, scrollbar=yes, scrollbars=yes')\">Find Server Wizard!</A>"
		)
	)
,true);

$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Server IP",
			1=>$layout->Field("server_ip", "text", $defaults[server_ip])
		)
	)
,true);
$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Server Port",
			1=>$layout->Field("server_port", "text", $defaults[server_port]) . $layout->Field("server_id", "hidden", $_GET[server_id])
		)
	)
,true);

$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Server Enabled?",
			1=>$layout->DropDown("server_enabled", $servactive)
			
		)
	)
,true);

$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Server Notify?",
			1=>$layout->DropDown("server_notify", $notenabled)
			
		)
	)
,true);


$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Flap Seconds",
			1=>$layout->Field("server_flap_seconds", "text", $defaults[server_flap_seconds])
		)
	)
,true);


if(!$_GET["copy"] && !$_GET["new"]) {

	if($defaults[server_dead]) {
		$svc = bartlby_get_service_by_id($btl->CFG, $defaults[server_dead]);	
	}
$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Alive indicator",
			1=>$btl->service_selector("dead_marker", $svc[server_name] . "/" . $svc[service_name] , "service_search1", $defaults[server_dead])
		)
	)
,true);

	
}

if($fm_action == "add_server") {
	$ov .= $layout->Tr(
	$layout->Td(
			Array(
				0=>"Package:",
				1=>$layout->DropDown("package_name", $packages) 
			)
		)

	,true);	
}
$ov .= $layout->Tr(
	$layout->Td(
			Array(
				0=>"Icon:",
				1=>$layout->DropDown("server_icon", $server_icons, "onChange=\"serviceManageIconChange(this.form);\"") 
			)
		)

,true);
$ov .= $layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"left",
					'show'=>"<div id=picholder></div><script>serviceManageIconChange(document.fm1);</script>"
					)
			)
		)

,true);





$title="add server";  
$content = "<table>" . $ov . "</table>";
$layout->push_outside($layout->create_box($layout->BoxTitle, $content));
	
	
$r=$btl->getExtensionsReturn("_PRE_" . $fm_action, $layout);

$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "button", "next->", "", " onClick='xajax_AddModifyClient(xajax.getFormValues(\"fm1\"))'")
					)
			)
		)

,false);

$layout->TableEnd();
$layout->FormEnd();
$layout->display();