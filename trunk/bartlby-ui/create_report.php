<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasright("log.report");

$ibox[0][c]="green";
$ibox[0][v]=0;
$ibox[0][s]=1;	
$ibox[0][k]="OK";
$ibox[1][c]="orange";        
$ibox[1][v]=1;	  
$ibox[1][k]="Warning";
$ibox[2][c]="red";        
$ibox[2][v]=2;	  
$ibox[2][k]="Critical";

$map = $btl->GetSVCMap();
$optind=0;
//$res=mysql_query("select srv.server_id, srv.server_name from servers srv, rights r where r.right_value=srv.server_id and r.right_key='server' and r.right_user_id=" . $poseidon->user_id);

while(list($k, $servs) = @each($map)) {
	$displayed_servers++;
	
	for($x=0; $x<count($servs); $x++) {
		//$v1=bartlby_get_service_by_id($btl->CFG, $servs[$x][service_id]);
		
		if($x == 0) {
			//$isup=$btl->isServerUp($v1[server_id]);
			//if($isup == 1 ) { $isup="UP"; } else { $isup="DOWN"; }
			$servers[$optind][c]="";
			$servers[$optind][v]="";	
			$servers[$optind][k]="[&nbsp;&nbsp;]&raquo;" . $servs[$x][server_name] . "&laquo;";
			$optind++;
		} else {
			
		}
		$state=$btl->getState($servs[$x][current_state]);
		$servers[$optind][c]="";
		$servers[$optind][v]=$servs[$x][service_id];	
		$servers[$optind][k]="&nbsp;[ $state ]&nbsp;" .  $servs[$x][service_name];
		if($servs[$x][service_id] == $_GET[service_id]) {
			$servers[$optind][s]=1;
		}
		
		$optind++;
	}
}


$layout= new Layout();

$layout->set_menu("report");
$layout->setTitle("Report");
$layout->Form("fm1", "report.php");
$layout->Table("100%");






$layout->Tr(
	$layout->Td(
			Array(
				0=>"Start Date:",
				1=>$layout->Field("report_start", "text", date("d.m.Y",time()-86800)) 
			)
		)

);


$layout->Tr(
	$layout->Td(
			Array(
				0=>"End Date:",
				1=>$layout->Field("report_end", "text", date("d.m.Y",time())) 
			)
		)

);

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Assume Initial State:",
				1=>$layout->DropDown("report_init", $ibox)
			)
		)

);
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Service:",
				1=>$layout->DropDown("report_service", $servers)
			)
		)

);
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Email RCPT:",
				1=>$layout->Field("report_rcpt", "text", "") 
			)
		)

);

$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "button", "next->", "" ," onClick='xajax_CreateReport(xajax.getFormValues(\"fm1\"))'") . $layout->Field("server_id", "hidden", $_GET[server_id])
					)
			)
		)

);


$layout->TableEnd();

$layout->FormEnd();
$layout->display();