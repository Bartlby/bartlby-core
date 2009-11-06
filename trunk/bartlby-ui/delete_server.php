<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->setTitle("Delete Server");
$layout->set_menu("client");
$layout->Form("fm1", "bartlby_action.php");
$layout->Table("100%");


if($_GET[server_id]) {
	$btl->hasServerRight($_GET[server_id]);	
}
$btl->hasRight("action.delete_server");


$global_msg=bartlby_get_server_by_id($btl->CFG, $_GET[server_id]);
if($global_msg == false) {
	$btl->redirectError("BARTLBY::OBJECT::MISSING");
	exit(1);	
}
$dlmsg=$btl->finScreen("delete_server1");

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
					'show'=>$layout->Field("Subm", "submit", "next->") . $layout->Field("action", "hidden", "delete_server") . $layout->Field("server_id", "hidden", $_GET[server_id])
					)
			)
		)

);


$layout->TableEnd();
$layout->FormEnd();
$layout->display();