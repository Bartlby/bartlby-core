<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("action.add_downtime");
$layout= new Layout();

$layout->setTitle("");

$layout->set_menu("downtimes");

$ov .= $layout->Form("fm1", "bartlby_action.php", "GET", true);
$layout->Table("100%");


if($_GET[service_id]{0} == 's') {
	$dt_type="Server";
	$cl=str_replace("s", "", $_GET[service_id]);
	$rr=bartlby_get_server_by_id($btl->CFG, $cl);
	$lappend=$rr[server_name];
	$dt_hidden=2;
} else {
	$rr=bartlby_get_service_by_id($btl->CFG, $_GET[service_id]);
	
	$lappend=$rr[server_name] . "/" . $rr[service_name];
	
	$dt_type="service";
	$dt_hidden=1;
}

$map = $btl->GetSVCMap();
$optind=0;
//$res=mysql_query("select srv.server_id, srv.server_name from servers srv, rights r where r.right_value=srv.server_id and r.right_key='server' and r.right_user_id=" . $poseidon->user_id);



$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Reason",
			1=>$layout->Field("downtime_notice", "text", "") . $layout->Field("action", "hidden", "add_downtime") . $layout->Field("service_id", "hidden", $_GET[service_id])
		)
	)
,true);
$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"From",
			1=>$layout->Field("downtime_from", "text", date("d.m.Y H:i", time())) . $layout->Field("downtime_type", "hidden", $dt_hidden)
		)
	)
,true);

$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"To",
			1=>$layout->Field("downtime_to", "text", date("d.m.Y H:i", time()+1024))
		)
	)
,true);
$ov .= $layout->Tr(
	$layout->Td(
			Array(
				0=>"Type",
				1=>$dt_type . $layout->Field("lappend", "hidden", $lappend)
			)
		)

,true);

$title="add downtime";  
$content = "<table>" . $ov . "</table>";
$layout->push_outside($layout->create_box($title, $content));
	

$r=$btl->getExtensionsReturn("_PRE_add_downtime", $layout);


$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "button", "next->", "" ," onClick='xajax_AddDowntime(xajax.getFormValues(\"fm1\"))'")
					)
			)
		)

,false);


$layout->TableEnd();
$layout->FormEnd();
$layout->display();