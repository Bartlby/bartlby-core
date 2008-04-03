<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->setTitle("Select Downtime");
$layout->set_menu("downtimes");
$layout->Form("fm1", $_GET[script]);
$layout->Table("100%");

$r=bartlby_downtime_map($btl->CFG);
$optind=0;
for($x=0; $x<count($r); $x++) {
	if($r[$x][downtime_type] == 1) {
		$dttype="Service";	
	} else if($r[$x][downtime_type] == 2) {
		$dttype="Server&nbsp;";
	}
	$dts[$optind][c]="";
	$dts[$optind][v]=$r[$x][downtime_id];	
	$dts[$optind][k]=date("d.m.Y H:i", $r[$x][downtime_from]) . "&nbsp;&nbsp;-&nbsp;&nbsp;" . date("d.m.Y H:i", $r[$x][downtime_to]) . " " . $r[$x][downtime_notice];
	$optind++;	
}

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Downtime:",
				1=>$layout->DropDown("downtime_id", $dts)
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

$layout->TableEnd();
$layout->FormEnd();
$layout->display();
?>