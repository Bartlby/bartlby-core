<?php
set_time_limit(0);

include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasright("log.report");
$btl->hasServerorServiceRight($_GET[report_service]);

$defaults=bartlby_get_service_by_id($btl->CFG, $_GET[report_service]);

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


$layout= new Layout();

$layout->set_menu("report");
$layout->setTitle("Report");
$layout->Form("fm1", "report.php");
$layout->Table("100%");

$state_array=array();

$log_mask=bartlby_config($btl->CFG, "logfile");

if(!$_GET[report_service] || !$log_mask) {
	$out ="You v choosen a server? or log file is not set";	
} else {
	
	if($_POST[report_rcpt]) {
		
		$out .= $btl->send_custom_report($_POST[report_rcpt], $_GET[report_service], $_GET[report_start], $_GET[report_end]);
	}
	$out .= "creating report for service: $_GET[report_service] From: $_GET[report_start] To: $_GET[report_end]<br>";	
	$ra=$btl->do_report($_GET[report_start], $_GET[report_end], $_GET[report_init], $_GET[report_service]);
	
	$svc=$ra[svc];
	$state_array=$ra[state_array];
	$notify=$ra[notify];
	$files_scanned=$ra[files_scanned];
		
	
	
	
		$out .= "<table width=100%>";
		$out .= "<td colspan=3 class=header>Service Time</td>";
		
		$hun=$svc[0]+$svc[1]+$svc[2];
		$flash[0]="0";
		$flash[1]="0";
		$flash[2]="0";
		
		//$img_file=$btl->create_report_img($state_array, $time_start, $time_end);
		
		
		
		while(list($state, $time) = @each($svc)) {
						
			
			$perc =   (($hun-$time) * 100 / $hun);
			$perc =100-$perc;
			
			$out .= "<tr>";
			$out .= "<td width=200 class='" . $btl->getColor($state) . "'>State:  " . $btl->getState($state) . "<a href='#' onClick=\"return dropdownmenu(this, event, menu_state" . $state . ", '400px')\" onMouseout=\"delayhidemenu()\">X</A><br>";
			
			$out .= "</td>";
			$out .= "<td>Time:  " . $btl->intervall($time) . " seconds</td>";
			$out .= "<td><b>" . round($perc,2) . "%</b>   </td>";
			
			$flash[$state]=$perc;
			
			
		}
		
		for($x=0; $x<3; $x++) {
			$nstate= $x+1;
			$rstr .= "&text_" . $nstate . "=" . $btl->getState($x) . "&value_" . $nstate . "=" . $flash[$x];	
		}
		$idx=$btl->findSHMPlace($defaults[service_id]);
		$svc_option_line="<a href='service_detail.php?service_place=$idx'>" . $defaults[server_name] . ":" . $defaults[client_port] . "/" . $defaults[service_name] . "</A>" . $btl->getServiceOptions($defaults, $layout) . "<a href='print_report.php?report_start=" . $_GET[report_start] . "&report_end=" .  $_GET[report_end] . "&report_init=" . $_GET[report_init] . "&report_service=" . $_GET[report_service] . "' target='_blank'>Print</A>";
		
		$out .= "<tr>";
		
			$out .= '<td colspan=2 align=center>
			
				<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" 
					codebase="http://fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=7,0,0,0" 
					width="350" 
					height="250" 
					id="pie" 
					align="middle">
				<param name="wmode" value="transparent">
				<param name="allowScriptAccess" value="sameDomain" />
				<param name="movie" value="flash/pie.swf?a=' . $rstr . '" />
				<param name="quality" value="high" />
				<embed src="flash/pie.swf?a=' . $rstr . '" quality="high" width="350" height="250" name="pie" align="middle" allowScriptAccess="sameDomain" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" wmode="transparent"/>
				</object>
				<br>
				<!--http://actionscript.org/showMovie.php?id=483-->
				' . $svc_option_line	 . '
			</td>';
				$out .= "</tr>";
		$out .= "</table>";
		
		$out .= "<table width=100%>";
		$out .= "<td colspan=2 class=header>Notifications:</td>";
		$hun=$daycnt;
		while(list($worker, $dd) = @each($notify)) {
			
			
			
			$out .= "<tr>";
			$out .= "<td valign=top width=200><b>$worker</b></td>";
			
			$out .= "<td>";
			
			
			while(list($trigger, $dd1) = @each($dd)) {
				$out .=	"<i>" . $trigger . "</i><br>";
				while(list($k, $ts) = @each($dd1)) {
					$out .= "&nbsp;	&nbsp;&nbsp;&nbsp;&nbsp; "  . date("d.m.Y H:i:s", $ts[0]) . " (<font color='" . $btl->getColor($ts[1]) . "'>" . $btl->getState($ts[1]) . "</font>)<br>";
				}
			}
			
			$out .= "</td>";
			
			
			$out .= "</tr>";
		}
		
		$out .= "</table>";
		
		
		$o1 .= "<table width=100%>";
		$o1 .= "<td colspan=3 class=header>Output:</td>";
		for($xy=0; $xy<count($state_array);$xy++) {
					$o1 .= "<tr>";
					$o1 .= "<td>" . date("d.m.Y H:i:s", $state_array[$xy][end]) . "</td>";
					$o1 .= "<td valign=top width=200><b><font color='" . $btl->getColor($state_array[$xy][lstate]) . "'>" . $btl->getState($state_array[$xy][lstate]) . "</font></b></td>";
			
					$o1 .= "<td>" . $state_array[$xy][msg] . "</td></tr>";
						
		}
		$o1 .= "</table>";
		
	
}


$layout->Tr(
	$layout->Td(
			Array(
				0=>$out
			)
		)

);
$layout->Tr(
	$layout->Td(
			Array(
				0=>$o1
			)
		)

);
for($x=0; $x<count($files_scanned); $x++)  {
	$worked_on_files++;
	$worked_on_lines += $files_scanned[$x][1]; 	
}

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Looked @ $worked_on_files files and $worked_on_lines Lines"
			)
		)

);





$layout->TableEnd();

$layout->FormEnd();
$layout->display();

