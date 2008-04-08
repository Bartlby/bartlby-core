<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->setTitle("Delete Worker");
$layout->set_menu("worker");
$layout->Form("fm1", "bartlby_action.php");
$layout->Table("100%");


if($btl->user_id != $_GET[worker_id]) {
	$btl->hasRight("action.delete_worker");
}
if(!$btl->isSuperUser() && $btl->user_id != $_GET[worker_id]) {
	
	$btl->hasRight("action.modify_all_workers");
}


$global_msg=bartlby_get_worker_by_id($btl->CFG, $_GET[worker_id]);

$dlmsg=$btl->finScreen("delete_worker1");

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
					'show'=>$layout->Field("Subm", "submit", "next->") . $layout->Field("action", "hidden", "delete_worker") . $layout->Field("worker_id", "hidden", $_GET[worker_id])
					)
			)
		)

);


$layout->TableEnd();
$layout->FormEnd();
$layout->display();