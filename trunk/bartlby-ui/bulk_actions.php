<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->set_menu("main");
$layout->setTitle("");


$servers=$btl->GetSVCMap();

$servcs = array();
while(list($k, $v) = @each($servers)) {
	for($y=0; $y<count($v); $y++) {
		array_push($servcs, $v[$y]);	
	}
}



$title="Services";  
$content = "asdf";

$layout->create_box($title, $content, "core_info", array(
	'services' => $servcs,
), "bulk_services");



$layout->display("bulk_actions");
