<?php
include "config.php";
include "bartlby-ui.class.php";
include "layout.class.php";


$layout= new Layout();
$layout->MetaRefresh();
$layout->Table("100%");

for($x=0; $x<count($_GET[msg])-1;$x++) {
	
	$inf=explode("|", $_GET[msg][$x]);
	
	switch($inf[0]) {
		case 'OK': $img="ok.png"; break;
		case 'WARN': $img="warning.png"; break;
		case 'CRIT': $img="critical.png"; break;
		case 'INFO': $img="info.png"; break;	
		case 'TRIG': $img="trigger.png"; break;
		
	}

	
	
	$tr .=	$layout->Td(
				array(
					0=>Array(
						"show"=>"<img src='images/$img'>",
						"width"=>20
					   ),
					1=>$inf[1]
				)
		);
	
	
}

$layout->Tr(
	$tr

);


$layout->TableEnd();
$layout->display("1");


?>