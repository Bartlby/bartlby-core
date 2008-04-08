<?
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("main.overview");
	$layout= new Layout();
	$layout->set_menu("main");
	$layout->MetaRefresh(30);
	$layout->Table("100%");
	$lib=bartlby_lib_info($btl->CFG);
	$info=$btl->info;
	
	$reload_status="data is up-to-date";
	
	if ($info[sirene_mode] == 0) {
		$sir="<a href='#' title='Enable Sirene' onClick=\"document.location.href='bartlby_action.php?action=sirene_enable';\"><img border=0 title='Enable Sirene' src='images/Disable-Sirene.gif'></A>";	
	} else {
		$sirene_intv=bartlby_config($btl->CFG, "sirene_interval");
		if(!$sirene_intv) {
			$sirene_intv=600;
		}
		$sir="Notify Every: $sirene_intv Seconds <a title='Disable Sirene' href='#' onClick=\"document.location.href='bartlby_action.php?action=sirene_disable';\"><img border=0 title='Disable Sirene' src='images/Enable-Sirene.gif'></A>";	
	}
	
	
	
	$quickview_disabled=bartlby_config("ui-extra.conf", "quickview_enabled");
	/*
	$mode=bartlby_config($btl->CFG, "i_am_a_slave");
	if(!$mode) {
		$vmode="MASTER";	
	} else {
		$vmode="SLAVE<br>dont change anything";	
	}
	*/
	if($info[round_time_count] > 0 &&  $info[round_time_sum] > 0 ) {
		$rndMS=round($info[round_time_sum] / $info[round_time_count], 2);
	} else {
		$rndMS=0;	
	}
	
		
	
	/*
	$is_repl_on=bartlby_config($btl->CFG, "replication");
	if(!$is_repl_on) {
		$is_repl_on="false";	
	}
	$repl = "Replication enabled:<font class='font2'> <b>$is_repl_on</b><br>";
	if($is_repl_on == "true") {
			$repl_cnt=bartlby_config($btl->CFG, "replicate_cnt");
			$repl .="Replicating to $repl_cnt Servers every " . bartlby_config($btl->CFG, "replication_intervall") . "<br>";
			for($x=1; $x<=$repl_cnt; $x++) {
				$repl .= str_repeat("&nbsp;", 20) . " Server:" . bartlby_config($btl->CFG, "replicate[" . $x . "]") . "<br>";	
			}
			$repl .= "Last Replication was on:" . date("d.m.Y H:i:s", $btl->info[last_replication]) . "<br></font>";
	}
	*/
	$servers=$btl->GetSVCMap();
	
	$mode=bartlby_config("ui-extra.conf", "xml_remote_count");
	if($mode) {
		$remote_xml=array();
		for($xml_X=1; $xml_X<=$mode; $xml_X++) {
				$xml_cur=bartlby_config("ui-extra.conf", "xml_remote[" . $xml_X . "]");
				$xml_cur_alias=bartlby_config("ui-extra.conf", "xml_alias[" . $xml_X . "]");
				if($xml_cur) {
					$xml_return = $btl->getRemoteStatus($xml_cur, $xml_cur_alias);
					if($xml_return) {
						array_push($remote_xml, $xml_return);	
					}
				}
		}
		
		
			
	}
	
	
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

	/*
	for($x=0; $x<$service_sum; $x++) {
		
		$svc=bartlby_get_service($btl->CFG, $x);
		
		$qck[$svc[server_name]][$svc[last_state]]++;	
		$qck[$svc[server_name]][10]=$svc[server_id];
	}
	
	*/
	
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
	$info_box_title='Core Information<div class="clock" nowrap>Time: ' . date("d.m.Y H:i:s") . '</div>';  
	// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
	$core_content = "<table class='nopad' width='100%'>
		<tr>
			<td class='font1'>(Logged in as: <font class='font2'>" . $btl->user . "</font>)</td>
			<td align=right class='font1'>Uptime:<font class='font2'>" . $btl->intervall(time()-$btl->info[startup_time]) . "</font></td>
		</tr>
		<tr>
			<td class='font1'>Services: <font class='font2'>" . $info[services] . "&nbsp;&nbsp;&nbsp;&nbsp;Workers: " . $info[workers] . "&nbsp;&nbsp;&nbsp;&nbsp;Downtimes: " . $info[downtimes]. "&nbsp;&nbsp;&nbsp;&nbsp;</td>
			<td align=right class='font1'>Datalib:<font class='font2'>" . $lib[Name] . "-" . $lib[Version] . "</font></td>
		</tr>
		<tr>
			<td class='font1' colspan=1>
			Running: $load_bar
			</td>
			<td align=right class='font1'>Avg Round Time:<font class='font2'>" . $rndMS . " ms / avg service delay: " . $avgDEL . " sec.</font></td>
		</tr>
		
		<tr>
			<td colspan=2 class='font1'>Version: <font class='font2'>" . $btl->getRelease() . "</font></td>
			
		</tr>
		<tr>
			<td colspan=2 class='font1'>Reload: <font class='font2'>" . $reload_status . "</font></td>
			
		</tr>
		
		
		
		<tr>
			<td class='font1'>&nbsp;</td>
			<td align=right class='font1'><font class='font2'>$sir</font></td>
		</tr>
	</table>";
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	
	
	
	
	
	$quick_view .= "</table>";
	$tac_title='Tactical Overview<div class="clock"></div>';  
	// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
	$tac_content = "<table class='nopad' width='100%'>
		<tr>
			<td colspan=2 class='font1'>Hosts:<font class='font2'>" . $hosts_sum . "</font></td>
			<td colspan=5 align=left class='font1'>Services:<font class='font2'>" . $service_sum . "</font></td>
		</tr>
		<tr>
			<td class='font1'>Up:<font class='font2'>" . $hosts_up. "</font></td>
			<td class='font1'>Down:<font class='font2'>" . $hosts_down. "</font></td>
			<td class='font1'><a href='services.php?expect_state=0'><font color=green>OK</font></A>:<font class='font2'>" . $services_ok. "</font></td>
			<td class='font1'><a href='services.php?expect_state=1'><font color=orange>Warning</font></A>:<font class='font2'>" . $services_warning. "</font></td>
			<td class='font1'><a href='services.php?expect_state=2'><font color=red>Critical</font></A>:<font class='font2'>" . $services_critical. "</font></td>
			<td class='font1'><a href='services.php?downtime=true'>Downtime</A>:<font class='font2'>" . $services_downtime. "</font></td>
			<td class='font1'><a href='services.php?acks=yes'>Acks</A> outstanding:<font class='font2'>" . $acks_outstanding. "</font></td>
			<td class='font1'><a href='services.php?expect_state=0&invert=true'>all failures</A></td>
		</tr>
		<tr>
			<td class='font1' colspan=7>$repl</td>
			
		</tr>
	</table>";
	$layout->push_outside($layout->create_box($tac_title, $tac_content));
	
	$health_title='System Health<div class="clock"></div>';  
	$silverbar = "<table class='nopad'>
		<tr>
			<td class='bar_left_silver'>&nbsp;</td>
			<td class='bar_middle_silver' style='width:" . 100*7.3 . "'></td>
			<td class='bar_right_silver'>&nbsp;</td>
			<td class='font2'>" . $prozent_float . "% OK</td>
			
		</tr>
		
	</table>";
	
	// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
	$health_content = "<div style='position:relative; z-index:2; '> <table class='nopad'>
		<tr>
			<td  class='bar_left_" . $color . "'>&nbsp;</td>
			<td class='bar_middle_" . $color . "' style='width:" . $prozent_float*7.3 . "'></td>
			<td class='bar_right_" . $color . "'>&nbsp;</td>
			<td class='font2'>&nbsp;</td>
			
		</tr>
		
	</table></div><div style='position:relative; z-index:1; top:-40px;'>$silverbar</div>";
	$layout->push_outside($layout->create_box($health_title, $health_content));
	
	//Generate external boxes ;-)
	
	for($bx=0; $bx<count($remote_xml); $bx++) {
		$xml_base_url=substr($remote_xml[$bx][url], 0, strrpos($remote_xml[$bx][url], "/")+1);
		
		$xml_box_content=$btl->XMLBoxHealth($remote_xml[$bx][info],$remote_xml[$bx][services]);
		$remote_xml[$bx][qck]=$xml_box_content[qck];
		$xml_box_title=$remote_xml[$bx][alias];
		
		$silverbar = "<table class='nopad'>
		<tr>
			<td class='bar_left_silver'>&nbsp;</td>
			<td class='bar_middle_silver' style='width:" . 100*7.3 . "'></td>
			<td class='bar_right_silver'>&nbsp;</td>
			<td class='font2'>" . $xml_box_content[prozent_float] . "% OK</td>
			
		</tr>
		
		</table>";
		
		$xml_box_output = "<div style='position:relative; z-index:2; '> <table class='nopad'>
		<tr>
			<td  class='bar_left_" . $xml_box_content[color] . "'>&nbsp;</td>
			<td class='bar_middle_" . $xml_box_content[color] . "' style='width:" . $xml_box_content[prozent_float]*7.3 . "'></td>
			<td class='bar_right_" . $xml_box_content[color] . "'>&nbsp;</td>
			<td class='font2'>&nbsp;</td>
			
		</tr>
		
		
	</table></div><div style='position:relative; z-index:1; top:-40px;'>$silverbar</div>
	<table class='nopad' width='100%'>
		<tr>
			
			<td colspan=6 class='font1'>Uptime: <font class='font2'>" . $btl->intervall(time()-$remote_xml[$bx][info][0][startup_time]) . "</font></td>
			
		</tr>
		<tr>
			<td colspan=6 class='font1'>Home: <font class='font2'><a target='_blank' href='$xml_base_url'>" . $xml_base_url . "</A></font></td>
			
		</tr>
		<tr>
			<td colspan=6 class='font1'>Version:<font class='font2'>" . $remote_xml[$bx][info][0][version] . "</font></td>
			
		</tr>
		<tr>
			<td colspan=2 class='font1'>Hosts:<font class='font2'>" . $xml_box_content[hosts_sum] . "</font></td>
			<td colspan=4 align=left class='font1'>Services:<font class='font2'>" . $xml_box_content[service_sum] . "</font></td>
		</tr>
		<tr>
			<td class='font1'>Up:<font class='font2'>" . $xml_box_content[hosts_up]. "</font></td>
			<td class='font1'>Down:<font class='font2'>" . $xml_box_content[hosts_down] . "</font></td>
			<td class='font1'>OK:<font class='font2'>" . $xml_box_content[services_ok]. "</font></td>
			<td class='font1'>Warning:<font class='font2'>" . $xml_box_content[services_warning]. "</font></td>
			<td class='font1'>Critical:<font class='font2'>" . $xml_box_content[services_critical]. "</font></td>
			<td class='font1'>Downtime:<font class='font2'>" . $xml_box_content[services_downtime]. "</font></td>
			<td class='font1'>Acks outstanding:<font class='font2'>" . $xml_box_content[acks_outstanding]. "</font></td>
		</tr>
		
		
	</table>
	
	";
	$layout->push_outside($layout->create_box($xml_box_title, $xml_box_output));
		
		
		
		
	}
	//Generate external quick views ;-)
	
	for($bx=0; $bx<count($remote_xml); $bx++) {
		
		$xml_box_content=$btl->XMLQuickView($remote_xml[$bx][info],$remote_xml[$bx][qck], $bx+1);
		$xml_box_title=$remote_xml[$bx][alias] . " --> QuickView";
		
		
		$layout->push_outside($layout->create_box($xml_box_title, $xml_box_content));
		
		
		
		
	}
	
	
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
	$layout->display();
	
	
function my_sys_getloadavg() {
	$con = file_get_contents("/proc/loadavg");
	$r = explode(" ", $con);
	return $r;
	
	
}
?>
