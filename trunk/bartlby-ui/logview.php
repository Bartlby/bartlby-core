<?php
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasright("log.logview");
	$info=$btl->getInfo();
	$layout= new Layout();
	$layout->set_menu("report");
	$layout->Form("fm1", "logview.php");
	$layout->Table("100%");
	$map=$btl->GetSVCMap();
	
	$ch_time=time();
	if($_GET[l]) {
		$tt=explode(".",$_GET[l]);
		//var_dump($tt);
		$ch_time=mktime(0,0,0,$tt[1],$tt[2],$tt[0]);	
	}
	
	$logf=bartlby_config($btl->CFG, "logfile") . date(".Y.m.d", $ch_time);
	$svcid=$_GET[service_id];
	$srvid=$_GET[server_id];
	
	$shm_id=$btl->findSHMPlace($svcid);
	
	if($shm_id >= 0) {
		$def=bartlby_get_service($btl->CFG, $shm_id);
		$svcM=$btl->getServiceOptions($def, $layout) . "<a href='service_detail.php?service_place=" . $shm_id . "'>Detail</A>";
	}
	if($_GET[bartlby_filter]) {
		$svcM .= "&nbsp; <a href='logview.php?bartlby_filter=&server_id=$srvid&service_id=$svcid&l=" . date("Y.m.d", $ch_time) . "'>remove filter</A>";
	}
	
	
	
	$layout->Tr(
		$layout->Td(
				Array(
					0=>Array(
						'colspan'=> 3,
						'class'=>'header',
						'show'=>"<a href='logview.php?bartlby_filter=" . $_GET["bartlby_filter"] . "&server_id=$srvid&service_id=$svcid&l=" . date("Y.m.d", $ch_time-86400)  . "'>&laquo;" . date("Y.m.d", $ch_time-86400) . "</A> Logfile ($logf) <a href='logview.php?bartlby_filter=" . $_GET["bartlby_filter"] . "&server_id=$srvid&service_id=$svcid&l=" . date("Y.m.d", $ch_time+86400)  . "'>&raquo;" . date("Y.m.d", $ch_time+86400) . "</A>  "  
						)
				)
			)

	);
	$layout->Tr(
		$layout->Td(
				Array(
					0=>array("colspan" => 3, 
						"show" => "Filter:" . $layout->Field("bartlby_filter", "text", $_GET["bartlby_filter"]) . $layout->Field("bartlby_sub", "submit", "Filter") . $svcM
					)
				)
			)

		);
	$fla=@file($logf);
	$fl=@array_reverse($fla);
	$lcounter=0;
	
	
	$curp = $_GET["site"] > 0 ? $_GET["site"] : 1;
	$perp=100;
	$forward_link=$btl->create_pagelinks("logview.php?bartlby_filter=" . $_GET["bartlby_filter"] . "&server_id=$srvid&service_id=$svcid&l=" . date("Y.m.d", $ch_time), count($fl)-1, $perp, $curp,"site");
	$skip_em=($curp*$perp)-$perp;
	$skipped=0;
	
	$layout->setTitle($forward_link);	
	
	while(list($k, $v)=@each($fl)) {
		
		
		$info_array=explode(";",$v);
		
		$log_detail_o=explode("@", $info_array[2]);
		
		if($log_detail_o[1] == "PERF") {
			$tmp=explode("|", $log_detail_o[2]);
			
			if($_GET[server_id] && !cmpServiceIDHasServer($tmp[0], $_GET[server_id])) {
				continue;	
			}
			if($_GET[service_id] && $tmp[0] != $_GET[service_id]) {
				
				continue;	
			}
			if(!$btl->hasServerorServiceRight($tmp[0], false)) {
				continue;	
			}
			
			$outline = "" . $tmp[1]  . "(" . $tmp[0] . ")";
			$stcheck=6;
		} else if($log_detail_o[1] == "LOG") {
			$tmp=explode("|", $log_detail_o[2]);
			
			if($_GET[server_id] && !cmpServiceIDHasServer($tmp[0], $_GET[server_id])) {
				continue;	
			}
			
			if($_GET[service_id] && $tmp[0] != $_GET[service_id]) {
				
				continue;	
			}
			if(!$btl->hasServerorServiceRight($tmp[0], false)) {
				continue;	
			}
			$clean = htmlentities($tmp[3]);
			$outline = "<a href='logview.php?bartlby_filter=" . $_GET["bartlby_filter"] . "&server_id=$srvid&service_id=" . $tmp[0] . "&l=" . date("Y.m.d", $ch_time)  . "'>" . $tmp[2] . "</A> changed to " . $btl->getState($tmp[1]) . "<br>" . $clean . "<br>";
			$stcheck=$tmp[1];
		}else if($log_detail_o[1] == "KILL") {
			$tmp=explode("|", $log_detail_o[2]);
			
			if($_GET[server_id] && !cmpServiceIDHasServer($tmp[0], $_GET[server_id])) {
				continue;	
			}
			
			if($_GET[service_id] && $tmp[0] != $_GET[service_id]) {
				
				continue;	
			}
			if(!$btl->hasServerorServiceRight($tmp[0], false)) {
				continue;	
			}
			$clean = htmlentities($tmp[3]);
			$outline = "<a href='logview.php?bartlby_filter=" . $_GET["bartlby_filter"] . "&server_id=$srvid&service_id=" . $tmp[0] . "&l=" . date("Y.m.d", $ch_time)  . "'>" . $tmp[2] . "</A><br>" . $clean . "<br>";
			$stcheck=8;
			
		} else if($log_detail_o[1] == "NOT") {
			$tmp=explode("|", $log_detail_o[2]);
			
			if($_GET[server_id] && !cmpServiceIDHasServer($tmp[0], $_GET[server_id])) {
				continue;	
			}
			if($_GET[service_id] && $tmp[0] != $_GET[service_id]) {
				
				continue;	
			}
			if(!$btl->hasServerorServiceRight($tmp[0], false)) {
				continue;	
			}
			$outline =  "Done " . $tmp[3] . " for " . $tmp[4] . " Service:<a href='logview.php?bartlby_filter=" . $_GET["bartlby_filter"] . "&server_id=$srvid&service_id=" . $tmp[0] . "&l=" . date("Y.m.d", $ch_time)  . "'>" .  $tmp[5] . "</A> " . $btl->getState($tmp[2]);
			$stcheck=5;	
			
		} else if($log_detail_o[1] == "NOT-EXT") {
			$tmp=explode("|", $log_detail_o[2]);
			
			if($_GET[server_id] && !cmpServiceIDHasServer($tmp[0], $_GET[server_id])) {
				continue;	
			}
			
			if($_GET[service_id] && $tmp[0] != $_GET[service_id]) {
				
				continue;	
			}
			if(!$btl->hasServerorServiceRight($tmp[0], false)) {
				continue;	
			}
			$outline =  $tmp[3] . " for " . $tmp[4] . " Service:<a href='logview.php?bartlby_filter=" . $_GET["bartlby_filter"] . "&server_id=$srvid&service_id=" . $tmp[0] . "&l=" . date("Y.m.d", $ch_time)  . "'>" .  $tmp[5] . "</A> " . $tmp[6];
			$stcheck=7;	
		}else if($log_detail_o[1] == "FORCE") {
			$tmp=explode("|", $log_detail_o[2]);
			
			if($_GET[server_id] && !cmpServiceIDHasServer($tmp[0], $_GET[server_id])) {
				continue;	
			}
			
			if($_GET[service_id] && $tmp[0] != $_GET[service_id]) {
				
				continue;	
			}
			if(!$btl->hasServerorServiceRight($tmp[0], false)) {
				continue;	
			}
			$outline = "Force Service:<a href='logview.php?bartlby_filter=" . $_GET["bartlby_filter"] . "&server_id=$srvid&service_id=" . $tmp[0] . "&l=" . date("Y.m.d", $ch_time)  . "'>" .  $tmp[5] . "</A> " . $tmp[6];
			$stcheck=3;	
		} elseif(!$_GET[service_id] && !$_GET[server_id]) {
			if(!$btl->hasRight("sysmessages", false)) {
				continue;	
			}
			
			$outline = $info_array[2];
			$stcheck=3;
				
		} else {
			continue;	
		}
		
		
		
		$date=$info_array[0];
		switch($stcheck) {
			case 0: $img="ok.gif"; break;
			case 1: $img="warning.gif"; break;
			case 2: $img="critical.gif"; break;
			case 3: $img="info.gif"; break;	
			case 4: $img="info.gif"; break;
			case 5: $img="trigger.gif"; break;
			case 6: $img="icon_stat.gif"; break;
			case 7: $img="icon_work.gif"; break;
			case 8: $img="icon_work.gif"; break;
		}
		if($_GET["bartlby_filter"]) {
			if(!preg_match("/" . $_GET["bartlby_filter"] . "/i", $v)) {
				continue;
			}
		}
		if($skipped < $skip_em) {
			$skipped++;
			continue;	
		}
		if($lcounter > 100) {
			break;	
		}
		$lcounter++;
				
		
			
		if(preg_match("/^AgentSyncer.*/i", $outline)) {
			$img = "agent_syncer.gif";	
		}
		$layout->Tr(
		$layout->Td(
				Array(
					0=>Array(
						'class'=>'header1',
						
						'show'=>"<font size=1>$date</font>"
						),
					1=>Array(
						'class'=>'header1',
						'width'=>25,						
						'show'=>"<img src='themes/" . $layout->theme . "/images/$img'>"
						),
					2=>Array(
						'width'=>720,
						'class'=>'header1',
						'show'=>str_replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", str_replace( "\\dbr", "<br>",$outline))
						),
				)
			)

		);
	}
	$layout->TableEnd();
	$layout->FormEnd();
	$r=$btl->getExtensionsReturn("_logView", $layout);
	$layout->display("no");
	
	
function cmpServiceIDHasServer($svc_id, $server_id) {
	global $map;
	reset($map);
	while(list($k, $v) = @each($map)) {
		for($x=0; $x<count($v); $x++) {
			if($v[$x][server_id] == $server_id && $v[$x][service_id] == $svc_id) {
				return true;	
			}
		}
	}
	
	return false;	
}



?>
