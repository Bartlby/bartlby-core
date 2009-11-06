<?php
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
if($ajaxed == "true") {
	$info_box_title="Extended Search";  
	$core_content = "
	<table  width='100%'>
		
		<tr>
			<td width=150 valign=top class='font2'>Search:</td>
			<td>
			<input type='text' onkeyup=\"buffer_suggest.modified('user_search', 'xajax_UserSearch', '" . $_GET[script] . "');\" id=user_search autocomplete='off' /> (PREG syntax)
				
				<div style='background-color:#ffffff; position:absolute' id='user_search_suggest'>
				</div>
			
			
			</td>
		</tr>
		
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
}

$dropdownded = bartlby_config("ui-extra.conf", "disable_dropdown_search");

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