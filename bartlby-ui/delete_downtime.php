<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("action.delete_downtime");

$layout= new Layout();
$layout->setTitle("Delete Downtime");
$layout->set_menu("downtimes");
$layout->Form("fm1", "bartlby_action.php");
$layout->Table("100%");




$dlmsg=$btl->finScreen("delete_downtime1");

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
					'show'=>$layout->Field("Subm", "submit", "next->") . $layout->Field("action", "hidden", "delete_downtime") . $layout->Field("downtime_id", "hidden", $_GET[downtime_id])
					)
			)
		)

);


$layout->TableEnd();
$layout->FormEnd();
$layout->display();