<?php
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/ArS/ArS.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("super_user");
	$sg = new ArS();
	$servers=$btl->GetSVCMap();
	
	$layout= new Layout();
	$layout->setTitle("ArS: View Report");
	
	$layout->set_menu("ArS");
	$layout->Form("fm1", "extensions_wrap.php");
	$layout->Table("100%");
	

$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<b>Known reports</b>")
			)
		)

);


	$da = $sg->storage->load_key("reports");
	$reports=unserialize($da);
	
	for($x=0; $x<count($reports); $x++) {
	
		$defaults=bartlby_get_service_by_id($btl->CFG, $reports[$x][ars_service_id]);
		if($reports[$x][ars_daily])
			$day="daily";
		else
			$day="";
		
		if($reports[$x][ars_weekly])
			$week="weekly";
		else
			$week="";
		
		if($reports[$x][ars_monthly])
			$month="month";
		else
			$month="";
		
		
		$layout->Tr(
			$layout->Td(
					Array(
					
						0=>"<input type=radio name='ars_report' value='" . $x . "'>" .  $layout->Field("action", "hidden", "delete") . $layout->Field("script", "hidden", "ArS/action.php"),
						1=>$reports[$x][ars_to] . "(" . $defaults[server_name] . "/" . $defaults[service_name] . ") $day $week $month"
					)
				)
		
		);	
	}

$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "button", "delete", "" ," onClick='xajax_ExtensionAjax(\"ArS\", \"xajax_ars_delete_form\", xajax.getFormValues(\"fm1\"))'") . $layout->Field("server_id", "hidden", $_GET[server_id])
					)
			)
		)

);
	$layout->FormEnd();
	
	$layout->TableEnd();
	$layout->display();