<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->set_menu("main");
$layout->setTitle("Acknowledge Problem");
$layout->Form("fm1", "bartlby_action.php");
$layout->Table("100%");

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Comment:",
				1=>$layout->TextArea("comment", "", 10, 50) . $layout->Field("service_id","hidden", $_GET[service_id]) . $layout->Field("action","hidden", "ack_problem")
			)
		));
		
	$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"center",
					'show'=>$layout->Field("Subm", "submit", "submit->")
					)
			)
		));


$layout->TableEnd();
$layout->display();
?>


