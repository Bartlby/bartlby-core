<?php
set_time_limit(0);

include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasright("log.report");
$btl->hasServerorServiceRight($_GET[report_service]);


echo "<form action=print_report.php method=get>";

while(list($k, $v) = @each($_GET)) {
	if($k != "sec_filter") {
		echo "<input type=hidden name='$k' value='$v'>";	
	}	
}
echo "<input type=text name='sec_filter' value=''>";
echo "<input type=submit value='Apply Second Filter'>";




echo "</form>";

$defaults=bartlby_get_service_by_id($btl->CFG, $_GET[report_service]);
$rap = "Second filter: " . $_GET[sec_filter] . "<br><a href='javascript:window.print();'>Print it</A><br>Report for: " . $defaults[server_name] . "/" . $defaults[service_name] . "\n";

$rep=$btl->do_report($_GET[report_start], $_GET[report_end], $_GET[report_init], $_GET[report_service]);
$rap .= "FROM: " . $_GET[report_start] . " TO: " . $_GET[report_end] . "\n";

$file .= $out;




$file .=  $btl->format_report($rep, "html", $rap, true);

$svc=$rep[svc];
$state_array=$rep[state_array];
$notify=$rep[notify];
$files_scanned=$rep[files_scanned];
$hun=$svc[0]+$svc[1]+$svc[2];
$flash[0]="0";
$flash[1]="0";
$flash[2]="0";

//$img_file=$btl->create_report_img($state_array, $time_start, $time_end);





while(list($state, $time) = @each($svc)) {
				
	
	$perc =   (($hun-$time) * 100 / $hun);
	$perc =100-$perc;
	
	
	$flash[$state]=$perc;
	
	
}

for($x=0; $x<3; $x++) {
	$nstate= $x+1;
	$rstr .= "&text_" . $nstate . "=" . $btl->getState($x) . "&value_" . $nstate . "=" . $flash[$x];	
}

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
<!--http://actionscript.org/showMovie.php?id=483-->';




echo $file;
					