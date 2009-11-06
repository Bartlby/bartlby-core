<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->set_menu("main");

$svc=bartlby_get_service_by_id($btl->CFG, $_GET[service_id]);

if($svc[current_state] == 0) {
	$chk0="checked";	
}
if($svc[current_state] == 1) {
	$chk1="checked";	
}
if($svc[current_state] == 2) {

	$chk2="checked";	
}

$layout->setTitle("Comments on " . $svc[server_name] . ":" . $svc[client_port] . "/" . $svc[service_name]);

$layout->Form("fm1", "bartlby_action.php");
$layout->Table("100%");
$layout->DisplayHelp(array(0=>"INFO|Pick a service From the Dropdown List"));


$cur_box_content = "<table class='service_table' cellpadding=2><form action='bartlby_action.php' method=POST>";
$cur_box_content .= $layout->Tr(
	$layout->Td(
			Array(
				0=>"Subject:",
				1=>$layout->Field("subject", "text") 
			)
		)

,true);
$cur_box_content .= $layout->Tr(
	$layout->Td(
			Array(
				0=>"Comment:",
				1=>$layout->TextArea("comment", "", 10, 50) . $layout->Field("service_id","hidden", $_GET[service_id]) . $layout->Field("action","hidden", "add_comment")
			)
		)

,true);
$cur_box_content .= $layout->Tr(
	$layout->Td(
			Array(
				0=>"State:",
				1=>"<input type=checkbox value=0 name=notify[] $chk0><font color=green>OK</font><input value=1 type=checkbox name=notify[] $chk1><font color=orange>Warning</font><input value=2 type=checkbox name=notify[] $chk2><font color=red>Critical</font>"
			)
		)

,true);

$cur_box_content .= $layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"center",
					'show'=>$layout->Field("Subm", "submit", "submit->")
					)
			)
		)

,true);


			
$cur_box_content .= "</form></table>";
$layout->push_outside($layout->create_box("Add New Comment", $cur_box_content));





$file=@file("comments/" . $_GET[service_id]);
$rfile=@array_reverse($file);
while(list($k, $v) = @each($rfile)) {
	$cminfo=explode("|", $v);
	$cur_box_content = "<table class='service_table' cellpadding=2>";
	$cur_box_content .=$layout->Tr(
		$layout->Td(
				Array(
					0=>Array(
						'colspan'=> 2,
						"align"=>"left",
						"show"=>  $cminfo[2]
						)
				)
			)
	
	,true);
	$cur_box_content .= "</table>";
	$e=explode(",", $cminfo[4]);
	$state_info = " In state (";
	for($x=0; $x<count($e); $x++) {
		$state_info .= "<font color='" . $btl->getColor($e[$x]) . "'>" . $btl->getState($e[$x]) . "</font>,";	
	}
	$state_info{strlen($state_info)-1}=' ';
	$state_info .= ")";

	$layout->push_outside($layout->create_box($cminfo[3] . " by <b>" . $cminfo[0] . "</b> posted on <i>" . date("d.m.Y H:i:s", $cminfo[1]) . "</i>" . $state_info, $cur_box_content));

	
	
}


$layout->TableEnd();

$layout->FormEnd();
$layout->display();