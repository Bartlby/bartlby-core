<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->setTitle("Delete Service");
$layout->set_menu("services");
$layout->Form("fm1", "bartlby_action.php");
$layout->Table("100%");


if($_GET[service_id] == "" || $_GET[service_id]{0} == 's') {
	$layout->Form("fm1", "bartlby_action.php");
	$layout->Table("100%");

	
	$layout->Tr(
		$layout->Td(
			array(
				0=>'you have choosen a server',
				1=>'<input type=button value=back onClick="javascript:history.back();">'
				
			)
		)
	);
	
	$layout->TableEnd();
	$layout->FormEnd();
	$layout->display();
	exit(1);
	
}

if($_GET[service_id]) {
	$btl->hasServerorServiceRight($_GET[service_id]);	
}
$btl->hasRight("action.delete_service");

$global_msg=bartlby_get_service_by_id($btl->CFG, $_GET[service_id]);
if($global_msg == false) {
	$btl->redirectError("BARTLBY::OBJECT::MISSING");
	exit(1);	
}

$dlmsg=$btl->finScreen("delete_service1");

$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					'show'=>$dlmsg
					)
			)
		)

);




$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "submit", "next->") . $layout->Field("action", "hidden", "delete_service") . $layout->Field("service_id", "hidden", $_GET[service_id])
					)
			)
		)

);


$layout->TableEnd();
$layout->FormEnd();
$layout->display();