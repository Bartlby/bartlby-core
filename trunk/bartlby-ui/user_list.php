<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->set_menu("worker");
$layout->setTitle("Select a  Worker");
$layout->Form("fm1", $_GET[script]);
$layout->Table("100%");


$servs=$btl->GetWorker();
$optind=0;


$ajaxed = bartlby_config("ui-extra.conf", "ajaxed");

if($dropdownded != "true")  {

	while(list($k, $v) = @each($servs)) {
		
		
		$v1=bartlby_get_worker_by_id($btl->CFG, $v[worker_id]);
		
		$servers[$optind][c]="";
		$servers[$optind][v]=$v1[worker_id];	
		$servers[$optind][k]=$v1[name];
		$optind++;
	}
	
	
	
	$layout->Tr(
		$layout->Td(
				Array(
					0=>"Worker:",
					1=>$layout->DropDown("worker_id", $servers)
				)
			)
	
	);
	
	$layout->Tr(
		$layout->Td(
				Array(
					0=>Array(
						'colspan'=> 2,
						"align"=>"right",
						'show'=>$layout->Field("Subm", "submit", "next->")
						)
				)
			)
	
	);
} else {
	$layout->Tr(
		$layout->Td(
				Array(
					0=>Array(
						'colspan'=> 2,
						"align"=>"left",
						'show'=>"Dropdown searches disabled in ui-extra config"
						)
				)
			)
	
	);	
}

$layout->TableEnd();

$layout->FormEnd();
$layout->display();