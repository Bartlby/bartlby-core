<?
/* $Id: ack.c 16 2008-04-07 19:20:34Z hjanuschka $ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2008 Helmut Januschka - All Rights Reserved
 *   Contact: <helmut@januschka.com>, <contact@bartlby.org>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 *   visit: www.bartlby.org for support
 * ----------------------------------------------------------------------- */
/*
$Revision: 16 $
$HeadURL: http://bartlby.svn.sourceforge.net/svnroot/bartlby/trunk/bartlby-core/src/ack.c $
$Date: 2008-04-07 21:20:34 +0200 (Mo, 07 Apr 2008) $
$Author: hjanuschka $ 
*/

	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$layout= new Layout();


	$btl->hasRight("main.overview");

	$layout->set_menu("main");
	$layout->MetaRefresh(30);
	$layout->Table("100%");
	$lib=bartlby_lib_info($btl->CFG);
	$info=$btl->info;
	
	$reload_status="data is up-to-date";
	
	if ($info[sirene_mode] == 0) {
		$sir="<a href='#' title='Enable Sirene' onClick=\"document.location.href='bartlby_action.php?action=sirene_enable';\"><img border=0 title='Enable Sirene' src='themes/" . $layout->theme . "/images/Disable-Sirene.gif'></A>";	
	} else {
		$sirene_intv=bartlby_config($btl->CFG, "sirene_interval");
		if(!$sirene_intv) {
			$sirene_intv=600;
		}
		$sir="Notify Every: $sirene_intv Seconds <a title='Disable Sirene' href='#' onClick=\"document.location.href='bartlby_action.php?action=sirene_disable';\"><img border=0 title='Disable Sirene' src='" . $layout->theme . "images/Enable-Sirene.gif'></A>";	
	}
	
	
	
	$quickview_disabled=bartlby_config("ui-extra.conf", "quickview_enabled");
	
	if($info[round_time_count] > 0 &&  $info[round_time_sum] > 0 ) {
		$rndMS=round($info[round_time_sum] / $info[round_time_count], 2);
	} else {
		$rndMS=0;	
	}
	
		
	
	
	$servers=$btl->GetSVCMap();
	
	
	
	$hosts_sum=count($servers);
	$hosts_up=0;
	$hosts_down=0;
	$services_critical=0;
	$services_ok=0;
	$services_warning=0;
	$services_unkown=0;
	$services_downtime=0;
	$all_services=0;
	$acks_outstanding=0;
	$gdelay_count = 0;
	$gdelay_sum = 0;
	
	while(list($k,$v)=@each($servers)) {
		$x=$k;
		if($btl->isServerUp($x, $servers)) {
			$hosts_up++;	
		} else {
			$hosts_down++;	
			$hosts_a_down[$k]=1;
			
		}
		
		for($y=0; $y<count($v); $y++) {
			//service_delay_sum
			$gdelay_sum += $v[$y][service_delay_sum];
			$gdelay_count += $v[$y][service_delay_count];
			
			if($v[$y][is_gone]) $reload_status="<font color=red>Reload needed</font>";		
			
			$qck[$v[$y][server_id]][$v[$y][current_state]]++;	
			$qck[$v[$y][server_id]][10]=$v[$y][server_id];
			$qck[$v[$y][server_id]][server_icon]=$v[$y][server_icon];
			$qck[$v[$y][server_id]][server_name]=$v[$y][server_name];
			if($v[$y][is_downtime] == 1) {
				$qck[$v[$y][server_id]][$v[$y][current_state]]--;
				$qck[$v[$y][server_id]][downtime]++;
				
			}
			if($v[$y][service_ack] == 2) {
				$qck[$v[$y][server_id]][acks]++;	
				$acks_outstanding++;
				
			}
			
			
			$all_services++;
			switch($v[$y][current_state]) {

				case 0:
					$services_ok++;
					if($v[$y][is_downtime] == 1) {
						$services_ok--;
						$services_downtime++;	
					}
				break;
				case 1:
					$services_warning++;
					if($v[$y][is_downtime] == 1) {
						$services_warning--;
						$services_downtime++;	
					}
				break;
				case 2:
					$services_critical++;
					if($v[$y][is_downtime] == 1) {
						$services_critical--;
						$services_downtime++;	
					}
				break;
				
				default:
					$services_unkown++;
					if($v[$y][is_downtime] == 1) {
						$services_ok--;
						$services_downtime++;	
					}
				
				
			}	
		}
		
		
	}
	
	$service_sum=$all_services-$services_downtime;
	
	
	
	

	if($service_sum == 0) {
		$criticals=100;
	} else {
		$criticals=(($service_sum-$services_ok) * 100 / $service_sum);
	}

	$proz=100-$criticals;
	
	
	
	
	$prozent_zahl = floor($proz);
	$prozent_float = number_format($proz, 1); 
	$prozent_crit_zahl = floor($criticals);
	$prozent_crit_float = number_format($criticals, 1); 
	
	$color="green";
	
	if($prozent_float <= 60) {
		$color="red";	
	} else if($prozent_float <= 90) {
		$color="yellow";	
	} else if($prozent_float <= 80) {
		$color="red";	
	} else {
		$color="green";
	}

	$bar=$prozent_float . "% Ok - $prozent_crit_float % Critical";

	
	
	$quick_view = "<table class='nopad' width=100%>";
	while(list($k, $v)=@each($qck)) {
		
		if($k != $last_qck) {
			$cl="";
			$STATE="UP";
			if ($hosts_a_down[$qck[$k][10]] == 1) {
				$cl="";
				$STATE="DOWN";
			}
			$quick_view .= "<tr>";
			$quick_view .= "<td class=$cl><img src='server_icons/" . $qck[$k][server_icon] . "'><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "'>" . $qck[$k][server_name] . "</A></td>";
			$quick_view .= "<td class=$cl><font size=1>$STATE</td>";
			$quick_view .= "<td class=$cl><table width=100>";
			
			$sf=false;
			if($qck[$k][0]) {
				$sf=true;
				$qo="<tr><td class=green_box><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=0'>" . $qck[$k][0] . " OK's</A></td></tr>";
			}
			if($qck[$k][1]) {
				$sf=true;
				$qw="<tr><td class=orange_box><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=1'>" . $qck[$k][1] . " Warnings</A></td></tr>";
			}
			
			if($qck[$k][2]) {
				$sf=true;
				$qc="<tr><td class=red_box><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=2'>" . $qck[$k][2] . " Criticals</A></td></tr>";
			}
			
			if($qck[$k][3]) {
				$sf=true;
				$qk="<tr><td class=silver_box><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=3'>" . $qck[$k][3] . " Unkown</A></td></tr>";
			}
			if($qck[$k][4]) {
				$sf=true;
				$qk="<tr><td class=silver_box><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=4'>" . $qck[$k][4] . " Info</A></td></tr>";
			}
			if($qck[$k][downtime]) {
				//$qk="<tr><td class=silver_box><font size=1>" . $qck[$k][downtime] . " Downtime</td></tr>";
				$qk="<tr><td class=silver_box><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&downtime=true'>" . $qck[$k][downtime] . " Downtime</A></td></tr>";
			}
			if($qck[$k][acks]) {
				$qk="<tr><td class=silver_box><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=2&acks=yes'>" . $qck[$k][acks] . " Ack Wait</A></td></tr>";
			}
					
				$quick_view .= "$qo";
				$quick_view .= "$qw";
				$quick_view .= "$qc";
				$quick_view .= "$qk";
			$quick_view .= "</table></td>";
			$quick_view .= "</tr>";
			$quick_view .= "<tr><td colspan=3><hr noshade></td></tr>";
		}
		
		$last_qck=$k;	
		$qo="";
		$qw="";
		$qc="";
		$qk="";
	}
	
	
	if($gdelay_count>0 && $gdelay_sum > 0) {
		
		$avgDEL = round($gdelay_sum/$gdelay_count,2);
	} else {
		$avgDEL = 0;	
	}
	
	$max_running = bartlby_config($btl->CFG, "max_concurent_checks");
	$max_load = bartlby_config($btl->CFG, "max_load");
	$curr_load = my_sys_getloadavg();
		
	if($curr_load[0] > $max_load) {
			
		if($info[current_running] >= $max_running) {
			$load_bar = "<font color=red>" . $info[current_running]  . " / " . $max_running  . " </font> Load: <font color=red> " . $curr_load[0] . " / " . $max_load . " </font>";
	
		} else if ($info[current_running] >= $max_running-2) {
			$load_bar = "<font color=orange>" . $info[current_running]  . " / " . $max_running  . " </font> Load: <font color=orange> " . $curr_load[0] . " / " . $max_load . " </font>";			

	
		} else {
			$load_bar = "<font color=green>" . $info[current_running]  . " / " . $max_running  . " </font> Load: <font color=green>" . $curr_load[0] . " / " . $max_load . " </font>";
	
		}
	} else {
		$load_bar = "<font color=green>" . $info[current_running]  . "</font> Load: <font color=green>" . $curr_load[0] . " / " . $max_load . " </font>";	
	}
	$info_box_title='Core Information';  
	$core_content = "";
	$layout->create_box($info_box_title, $core_content, "core_info", array(
		'user' => $btl->user,
		'time' =>  date("d.m.Y H:i:s"),
		'uptime' =>  $btl->intervall(time()-$btl->info[startup_time]),
		'services' => $info[services],
		'workers' => $info[workers],
		'downtimes' => $info[downtimes],
		'datalib' => $lib[Name],
		'datalib_version' => $lib[Version],
		'running' => $load_bar,
		'round_ms_time' => $rndMS,
		'average_delay' => $avgDEL,
		'release_name' => $btl->getRelease(),
		'reload_state' => $reload_status,
		'sirene'  => $sir
		
		), "core_info");
	
	
	
	
	
	$quick_view .= "</table>";
	$tac_title='Tactical Overview';  
	$layout->create_box($tac_title, $tac_content, "tactical_overview",array(
		'host_sum' => $hosts_sum,
		'hosts_up' => $hosts_up,
		'hosts_down' => $hosts_down,
		'services_ok' => $services_ok,
		'services_warning' => $services_warning,
		'services_critical' => $services_critical,
		'services_downtime' => $services_downtime,
		'acks_outstanding' => $acks_outstanding,
		'services_sum' => $service_sum
	
	), "tactical_overview");
	
	$health_title='System Health';  
	$layout->create_box($health_title, $health_content,"system_health", array(
			'prozent_float' => $prozent_float,
			'color' => $color
		), "system_health");
	
	
	$layout->setTitle("QuickView");
	$r=$btl->getExtensionsReturn("_overview", $layout);
	
	if($quickview_disabled != "false") {
		$layout->Tr(
		$layout->Td(
				array(0=>$quick_view)
			)

		);
		
	} else {
		$layout->Tr(
		$layout->Td(
				array(0=>"<i>Disabled</i>")
			)

		);	
	}
	
	
	
	$layout->TableEnd();
	$layout->display("overview");
	
	
function my_sys_getloadavg() {
	$con = file_get_contents("/proc/loadavg");
	$r = explode(" ", $con);
	return $r;
	
	
}
?>
