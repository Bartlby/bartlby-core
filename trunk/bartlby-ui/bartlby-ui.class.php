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

include_once("bartlbystorage.class.php");

session_start();

set_time_limit(0);
if(function_exists("set_magic_quotes")) set_magic_quotes_runtime(0);
define("BARTLBY_UI_VERSION", "2.2");
define("BARTLBY_RELNOT", "");
$wdays[0]="Sunday";
$wdays[1]="Monday";
$wdays[2]="Tuesday";
$wdays[3]="Wednesday";
$wdays[4]="Thursday";
$wdays[5]="Friday";
$wdays[6]="Saturday";



if(!version_compare(phpversion(), "5.0.0", ">=")) {
	echo "you should have at least a php5 series";
	exit;	
}

class BartlbyUi {
	
	function send_custom_report($emails, $service_id, $from, $to) {
		include_once "Mail.php";
		include_once "Mail/mime.php";
		
		$storage = new BartlbyStorage("ArS");
		$defaults=bartlby_get_service_by_id($this->CFG, $service_id);
		$rap = "Report for: " . $defaults[server_name] . "/" . $defaults[service_name] . "\n";
		$btl_subj = "Bartlby Custom report";
		
		$rap .= "DAILY REPORT:\n\n";
		$rep = $this->do_report($from, $to, 0, $service_id);
		
		$rap .= "FROM: " . $from . " TO: " . $to . "\n";
		$file =  $this->format_report($rep, "html", $rap);
		
		
		
		
		
		
		$tmpfname = tempnam ("/tmp", "ArS");
		
		$fp = fopen($tmpfname, "w");
		fwrite($fp, $file);
		fclose($fp);
		
		copy($tmpfname, $tmpfname . ".html");
		unlink($tmpfname);
		
		$mime = new Mail_Mime();
		$mime->setTxtBody("see the attachment for details");
		
		$mime->addAttachment($tmpfname . ".html", "text/html", "report.html");
		
		
		//update perf handler ;)
		$this->updatePerfHandler($defaults[server_id], $defaults[service_id]);
		$path=bartlby_config($this->CFG, "performance_rrd_htdocs");
		foreach(glob($path . "/" . $service_id . "_*.png") as $fn) {
		     	//$mime->addAttachment($fn, "image/png", basename($fn), true, 'base64', 'inline');
		     	$mime->addHTMLImage($fn, "image/png", basename($fn), true);
		     	$file .= "<img src='" . basename($fn) . "'><br>";
		}
		$mime->setHTMLBody($file);
		$body=$mime->get();
		
		
					   
					   
		
		
		$dd = explode(";", $emails);
		for($x=0; $x<count($dd); $x++) {
	
			$headers = array('From' =>  $storage->load_key("ars_smtp_from") , 'To' => $dd[$x],
				   'Subject' => $btl_subj);
		
			$smtp = Mail::factory('smtp',
				array ('host' =>  $storage->load_key("ars_smtp_host"),
		  			'auth' => false,
			   		'timeout' => 10,
					'debug' => false
				));
		
				$hdrs=$mime->headers($headers);
		
		
				$mail = $smtp->send($dd[$x], $hdrs, $body);
				$r .= "Sent to: " . $dd[$x] . "<br>";
		
		}
		
		return $r;
		
		
	}
	
	function doReload() {
		bartlby_reload($this->CFG);
		while(1) {
			$x++;
			$i = @bartlby_get_info($this->CFG);
			flush();
			
			if($i[do_reload] == 0) {
				$msg = "Done";
				//$layout->OUT .= "<script>doReloadButton();</script>";
				break;	
			}
		}
	}
	function resolveDeadMarker($start_id, $map) {
		
		$rr=0;
		
		
		$cur_id=$start_id;
		$l = 0;
		
		
		
		while($cur_id != 0) {
			if($l != 0 && $l == $cur_id) {
				return $r;	
			}
			@reset($map);
			
			$f = false;
			while(@list($k, $v) = @each($map)) {
				
				
				for($x=0; $x<count($v); $x++) {
					
					if($v[$x][service_id] == $cur_id) {
						$f = true;					
						$r .= str_repeat("&nbsp;&nbsp;&nbsp;", $rr) .  "<a href='service_detail.php?service_place=" . $v[$x][shm_place] . "'>" . $v[$x][server_name] . "/" . $v[$x][service_name] . "</A> (<font color='" .  $this->getColor($v[$x][current_state])  . "'>" . $this->getState($v[$x][current_state]) . "</font>)<br>";
						
						
						$l = $cur_id;	
						$cur_id = $v[$x][server_dead];
						
						if($v[$x][current_state] == 2) {
							
							return $r;	
						}
						
						if($cur_id <= 0) {
							
							return $r;	
						}
						if($cur_id == $start_id ) {
							
							return $r;	
						}
						
						$rr++;
						
						
					}
					
					
				}	
				
			
				
				
			}
			if($f == false) {
			
				return $r . "<br>INDICATOR: " . $cur_id .  " not found";	
			}
			if($cur_id == $start_id ) {
				echo "b";
				return $r;	
			}
			
		}
		return $r;
		
				
	}
	function service_selector($f, $v, $d, $d1) {
		$mydiv="<div style='background-color:#ffffff; position:absolute' id='" . $d . "'></div>";
		$r = "<input type=hidden id='text_" . $d . "'  name='text_" . $d . "' value='" . $d1 . "'><input autocomplete=off type=text id='search_" . $d . "' name='search_" .  $d . "' value='" . $v . "' onkeyup=\"buffer_suggest.modified('search_" . $d . "', 'xajax_service_noaction', '" . $d . "');\">" . $mydiv;
		return $r;	
	}
	function format_report($rep, $type='html', $hdr, $do_perf=false) {
		global $btl;
		
		
		
		$svc=$rep[svc];
		$svc_id=$rep[service_id];
		$srv_id=$rep[server_id];
		$state_array=$rep[state_array];
		$notify=$rep[notify];
		$files_scanned=$rep[files_scanned];
		
		$hun=$svc[0]+$svc[1]+$svc[2];
		
		
		
		
		switch($type) {
			case 'html':
				$rap ="<html><head><style>td{font-size:12px; font-family:tahoma}</style></head><body>";
				$rap .= "<table width=100% border=3>";
			break;	
		}
		switch($type) {
			case 'html':
				$rap .= "<tr><td colspan=3>" . $hdr . "</td></tr>";
			break;	
		}
		
		switch($type) {
			case 'html':
				$rap .= "<tr><td colspan=3><b>Service Availability</b></td></tr>";
			break;	
		}
		
		while(list($state, $time) = @each($svc)) {
			
								
			$perc =   (($hun-$time) * 100 / $hun);
			$perc =100-$perc;
			
			
			switch($type) {
				case 'html':
					$rap .= "<tr>";
					$rap .= "<td>";
					$rap .= "<font color=" . $btl->getColor($state) . ">" . $btl->getState($state) . "</font>";
					$rap .= "</td>";
					$rap .= "<td>";
					$rap .= $btl->intervall($time);
					$rap .= "</td>";
					$rap .= "<td>";
					$rap .= round($perc,2);
					$rap .= "%</td>";
					$rap .= "</tr>";
				break;	
			}
			
			
			
			
		}
		
	if($do_perf == true) {
		$this->updatePerfHandler($srv_id, $svc_id);
		$path=bartlby_config($this->CFG, "performance_rrd_htdocs");

		foreach(glob($path . "/" . $svc_id  . "_*.png") as $fn) {
    	    //$mime->addAttachment($fn, "image/png", basename($fn), true, 'base64', 'inline');
	
				$rap .= "<tr><td colspan=3><img src='rrd/" . basename($fn) . "'><td></tr>";
		}
	}
		
		switch($type) {
			case 'html':
				$rap .= "<tr><td colspan=3><b>Notifications</b></td></tr>";
			break;	
		}
		while(list($worker, $dd) = @each($notify)) {
			
			switch($type) {
				case 'html':
					$rap .= "<tr><td colspan=2>" . $worker . "</td><td>";
				break;	
			}
			
					
			while(list($trigger, $dd1) = @each($dd)) {
				$rap .= "\t" . $trigger . "<br>";
				while(list($k, $ts) = @each($dd1)) {
					
					switch($type) {
						case 'html':
							$rap .= "<li>" . date("d.m.Y H:i:s", $ts[0]) . " (" . $btl->getState($ts[1]) . ")<br>";
						break;	
					}
				}
			}
			switch($type) {
				case 'html':
					$rap .= "</td></tr>";
				break;	
			}
						
		}
		switch($type) {
			case 'html':
				$rap .= "<tr><td colspan=3><b>Output</b></td></tr>";
			break;	
		}
		$c1="#cccccc";
		$c2="#eeeeee";
		$cl = $c1;
		$z=0;
		for($xy=count($state_array)-1; $xy>=0;$xy=$xy-2) {
			
				switch($type) {
					case 'html':
					$stay_time="";
					$stay_sec=$state_array[$xy][end]-$state_array[$xy][start];
					
						if($state_array[$xy][state] != 0) {
							
							
							//if it was not ok we display the stay in time ;)
							$stay_time="(" . $this->intervall($stay_sec) . ")";
						}
						if(!$_GET[sec_filter] ||  $stay_sec > $_GET[sec_filter]) {
						
							$o1 .= "<tr>";
							$o1 .= "<td bgcolor='$cl'>" . date("d.m.Y H:i:s", $state_array[$xy][end]) . "</td>";
							$o1 .= "<td bgcolor='$cl'>" .  $btl->getState($state_array[$xy][lstate]) . " </td>";
							$o1 .= "<td bgcolor='$cl'>" . $state_array[$xy][msg] . " </td>";
							$o1 .= "</tr>";
						
							$o1 .= "<tr>";
							$o1 .= "<td bgcolor='$cl'>" . date("d.m.Y H:i:s", $state_array[$xy-1][end]) . "</td>";
							$o1 .= "<td bgcolor='$cl'>" .  $btl->getState($state_array[$xy-1][lstate]) . " $stay_time </td>";
							$o1 .= "<td bgcolor='$cl'>" . $state_array[$xy-1][msg] . " </td>";
							$o1 .= "</tr>";
						
						
							$z++;
							$z++;
							if($z == 2) {
								if($cl == $c1 ){
									$cl = $c2;
								} else {
									$cl = $c1;	
								}	
								$z=0;
							}
						}
					break;	
				}
								
		}
		
		$rap .= $o1;
		
		switch($type) {
			case 'html':
				$rap .= "</table></body></html>";
			break;	
		}
		
		return $rap;
	
	}
	function resolveServicePlan($str) {
		global $wdays;
		
		$cur_wday = date("w");
		
		
		if(strlen($str) < 5) {
			return "24x7x365 <font size=1 color='blue'><i>currently in range</i></font>";	
		}
		$range_found = false;
		$o = explode("|", $str);
       	
       	$cur_ts=time();
       	
		for($x=0; $x<count($o); $x++) {
			$p = explode("=", $o[$x]);
			$inv=0;
			if(count($p) == 1) {
				$p = explode("!", $o[$x]);	
				$inv=1;
				$range_found = true;
			}
			$xx = explode(",", $p[1]);
			for($y=0; $y<count($xx); $y++) {
				list($h,$m, $h1, $m1) = sscanf($xx[$y], "%d:%d-%d:%d");
				$t1 = mktime($h, $m);
				$t2 = mktime($h1, $m1);
				if($inv == 1) {
					$filled[$x][disabled]=1;	
				}
				if($cur_ts >= $t1 &&  $cur_ts <= $t2 && $cur_wday == $x) {
					
					if($inv == 0) {
						$cl = 'blue';	
						$range_found = true;
					} else {
						$cl = 'grey';	
						$range_found = false;
						
						
					}
					
					$filled[$x][value] .= "<font color='$cl'>" . $xx[$y] . ",</font>";	
					
				} else {
					$filled[$x][value]	.= $xx[$y] . ",";
				}
			}
			$filled[$x]{strlen($filled[$x][value])-1} = " ";
			
		}
		$plan_box = "<table>";
		for($x=0; $x<=6; $x++) {
			$chk="";
			$vv = "<i>NO</i>";
			if($filled[$x][value]) {
				$chk="checked";
				$vv = $filled[$x][value];
			}
			$cl = "";
			if($cur_wday == $x) {
				$cl = "color=blue";	
			}
			if($filled[$x][disabled] == 1) {
				$invinfo="(inverted)";
			} else {
				$invinfo="";	
			}
			$plan_box .= "<tr><td><font size=1 " . $cl . ">" .  $wdays[$x] . $invinfo  . "</font></td><td><font size=1>" . $vv . "</font></td></tr>";
		}
		
		$plan_box .= "</table>";
	     
	       return $plan_box;
		
	}
	
	function do_report($start_in, $end_in, $state_in, $in_service) {
		$state_array=array();
		
		$log_mask=bartlby_config($this->CFG, "logfile");
		$date_start=explode(".", $start_in);
		$date_end=explode(".", $end_in);
		
		$time_start=mktime(0,0,0, $date_start[1], $date_start[0], $date_start[2]);
		$time_end=mktime(0,0,0, $date_end[1], $date_end[0], $date_end[2]);
		
		
		$daycnt = $time_end-$time_start+86400;
		
		$day_x=$daycnt/86400;
		$files_scanned=array();
		
		$work_on=$time_start;
		for($x=0; $x<$day_x; $x++) {
			$filename = $log_mask . "." . date("Y.m.d", $work_on);
			$last_mark=$work_on;
			
			$work_on += 86400;
			
			$fdata=@file($filename);
			$lines = count($fdata);
			
			array_push($files_scanned, array(0=>$filename, 1=>$lines));
			
			
			$last_state=$state_in;
			
			$dig_map[$time_start]=$last_state;
			while(list($k,$v) = @each($fdata)) {
				$info_array=explode(";",$v);
				
				$log_detail_o=explode("@", $info_array[2]);
				list($d, $m,$y, $h, $s, $i) = sscanf($info_array[0], "%d.%d.%d %d:%d:%d");
				$log_stamp=mktime($h,$s,$i,$m,$d,$y);
						
				if($log_detail_o[1] == "LOG") {
					$tmp=explode("|", $log_detail_o[2]);
					$msg="";
					for($z=3; $z<count($tmp);$z++) {
						$msg .= $tmp[$z];	
					}
					
					if($in_service && $tmp[0] != $in_service) {
						
						continue;	
					}
					
					if($last_state != $tmp[1]) {
						
						
						
						$diff = $log_stamp - $last_mark;
						//$out .= "State changed from " . $btl->getState($last_state) . " to " . $btl->getState($tmp[1]) . "<br>";	
						//echo "Where " . $diff . " in " . $btl->getState($last_state) . "<br>"; 
						array_push($state_array, array("start"=>$last_mark, "end"=>$log_stamp, "state"=>$last_state, "msg"=>$msg, "lstate"=>$tmp[1]));
						
						$svc[$last_state] += $diff;
						
						$last_state=$tmp[1];
						$last_mark=$log_stamp;
						$dig_map[$log_stamp]=$last_state;
					}
					
					//$out = $tmp[2] . " changed to " . $btl->getState($tmp[1]) . "(" . $tmp[3] . ")";
					
				} else if($log_detail_o[1] == "NOT") {
					$tmp=explode("|", $log_detail_o[2]);
					if($in_service && $tmp[0] != $in_service) {
					
						continue;	
					}
					//$out .= "Done " . $tmp[3] . " for " . $tmp[4] . " Service:" .  $tmp[5] . " " . $btl->getState($tmp[2]);
					if(!is_array($notify[$tmp[4]][$tmp[3]])) {
						$notify[$tmp[4]][$tmp[3]]=array();
					} 
					$el[0]=$log_stamp;
					$el[1]=$tmp[2];
					array_push($notify[$tmp[4]][$tmp[3]], $el);
				
				} else {
					continue;	
				} 	
			}
			if($work_on > time()) {
				$work_on=time();	
			}
			$diff = $work_on - $last_mark;
			//$out .= "EOD: " . $diff . " " . $btl->getState($last_state) . "<br>";
			$svc[$last_state] += $diff;
			
			
			
			
				
			
		}	
	
		$r[svc]=$svc;
		$r[service_id]=$in_service;
		$r[state_array]=$state_array;
		$r[notify]=$notify;
		$r[files_scanned]=$files_scanned;
		
		return $r;
		
	
		
	}
	function setUIRight($k, $v, $user) {
		if(!file_exists("rights/" . $user . ".dat")) {
			copy("rights/template.dat", "rights/" . $user . ".dat");
		}
		if(file_exists("rights/" . $user . ".dat")) {
			$orig = file_get_contents("rights/" . $user . ".dat");
			
			if(!preg_match("/" . $k . "=/", $orig)) {
				$orig .= "\n" . $k . "=\n";	
				
			}
			
			
			
			//$new=preg_replace("/" . $k . "=.*$/", $k . "="  . $v . "\n", $orig);
			$lines = preg_split("/[\n\r]/",$orig);
			
			$new ="";
			for($x=0; $x<count($lines); $x++) {
				$tt = explode("=", $lines[$x]);
				if($tt[0] == $k) {
					$new .= $k . "=" . $v . "\n";	
				} else {
					$new .= $lines[$x] . "\n";
				}
			}
			
			$fp = fopen("rights/" . $user . ".dat", "w");
			fwrite($fp, $new);
			fclose($fp);
			
			
			
			
			
		} else {
			if(!preg_match("/error.php/" , $_SERVER[SCRIPT_NAME])) {
				$this->redirectError("BARTLBY::RIGHT::FILE::NOT::FOUND::" . $user);
				exit(1);	
			}
		}	
	}
	function resolveGroupString($str) {
		$aa=explode("|", $str);
		for($aax=0; $aax<count($aa); $aax++) {
			$bb = explode("=", $aa[$aax]);
			if($aa[$aax]) {
				$idx=$this->findSHMPlace($aa[$aax]);
                                $svc=bartlby_get_service($this->CFG, $idx);
                                $dtemp="";
                                if($svc[is_downtime] == 1) {
                                        $dtemp="<i>DOWNTIME</i>";
                                }

				$r .= "Service: $svc[server_name]:$svc[client_port]/$svc[service_name] is not allowed to be in <font color=" . $this->getColor($bb[1]) . ">" . $this->getState($bb[1]) . "</font> (Current: <font color=" . $this->getColor($svc[current_state]) . ">" . $this->getState($svc[current_state]) . "</font>) $dtemp<br>";
			}
		}	
		return $r;
	}
	function isSuperUser() {
		if($this->rights[super_user][0] != "true") {
			return false;
		}else {
			return true;	
		}
		
	}
	function dnl($i) {
		return sprintf("%02d", $i);
	}
	function BartlbyUi($cfg, $auth=true, $shm_check=true) {
			
		if(!function_exists("bartlby_version")) {
			$dl_ret=@dl("bartlby.so");	
			if(!$dl_ret) {
				echo "Bartlby php module isn't either compiled in nor the shared variant was found!!!";
				exit;	
			}
		}	
	
		

		if(bartlby_config("ui-extra.conf", "theme") != "") {
			$this->theme=bartlby_config("ui-extra.conf", "theme");
		} else {
			$this->theme="classic";
		}
		
		
		$this->BASE_URL=substr($_SERVER[SCRIPT_URI], 0, strrpos($_SERVER[SCRIPT_URI], "/")+1);				
		
		$this->CFG=$cfg;
		//Check if bartlby is running :-)
		$this->info=@bartlby_get_info($this->CFG);
		
		/*
			Check if process is still here
		*/
		$pid_file=bartlby_config($this->CFG, "pidfile_dir");
			
		if((!$this->info && $auth == true) || !$pid_file) {
			$this->redirectError("BARTLBY::NOT::RUNNING");
			exit(1);
		} 
		
		if($auth == true && bartlby_check_shm_size($cfg) == false) {
			$this->redirectError("BARTLBY::MODULE::MISMATCH");
			exit(1);
						
		}
		
		$pid_ar=@file($pid_file . "/bartlby.pid");
		$pid_is=@implode($pid_ar, "");
		
		if(!preg_match("/error.php/" , $_SERVER[SCRIPT_NAME])) {
		
			if(!$pid_is || !file_exists("/proc/" . $pid_is . "/cmdline")) {
						
				$this->redirectError("BARTLBY::SHM::STALE");
				exit(1);
			}
		}
		
		$this->perform_auth($auth);
		$this->release=$this->info[version];
		$this->loadRights();
		$this->BASEDIR=@bartlby_config($this->CFG, "basedir");
		$this->PERFDIR=@bartlby_config($this->CFG, "performance_dir");
		
		
		
	}
	function getServerInfs($id, $map) {
		@reset($map);
		$re[0]=0;
		$re[1]=0;
		$re[2]=0;
		$re[downtimes]=0;
		for($x=0; $x<count($map[$id]); $x++) {
			if($map[$id][$x][is_downtime] == 1) {
				continue;	
			}
			$re[$map[$id][$x][current_state]]++;
			
		}
		
		return $re;
	}
	function finScreen($f=false) {
		global $_GET,$global_msg;
		
		if($f==false) {
			$f=$_GET[action];	
		}
		
		if(file_exists("fin/" . $f)) {
			$fp=fopen("fin/" . $f,"r");
			while(!feof($fp)) {
				$str=fgets($fp, 1024);
				while(list($k, $v)=@each($_GET)) {
					
					$str=str_replace("\$_GET[" . $k . "]", $v, $str);	
				}
				while(list($k, $v)=@each($global_msg)) {
					$str=str_replace("\$global_msg[" . $k . "]", $v, $str);	
				}
				$r .= $str;
			}
			
		} else {
			return "message file: $f not found";	
		}
		return $r;
	}
	function hasServerorServiceRight($svcid, $do_redir=true) {
		if($this->rights[super_user][0] == "true") {
			return true;	
		}
		
		$rt=false;
		$svc=bartlby_get_service_by_id($this->CFG, $svcid);
		if(!$svc) {
			$rt = false;
		}
		if($this->hasServerRight($svc[server_id], false)) {
			$rt = true;	
			
			
		}
		
		if(in_array($svcid, $this->rights[services])) {
			$rt = true;
		} 
		
		if($rt == false) {
			if($do_redir == true) {
				if(!preg_match("/error.php/" , $_SERVER[SCRIPT_NAME])) {
						$this->redirectError("BARTLBY::RIGHT::MISSING", "&right=service_" . $svcid);
						exit(1);	
				}
			} else {
					return false;	
			}
		} else {
			return $rt;
		}	
		
		
		
		
		
	}
	function hasServerRight($srvid, $do_redir=true) {
		
		if($this->rights[super_user][0] == "true") {
			return true;	
		}
		
		if(!is_array($this->rights[servers])) {
			return false;
		}
	
		settype($srvid, "integer");
		
		
		
		if(in_array($srvid, $this->rights[servers])) {
			return true;	
		}
		if($do_redir == true) {
			if(!preg_match("/error.php/" , $_SERVER[SCRIPT_NAME])) {
					$this->redirectError("BARTLBY::RIGHT::MISSING", "&right=server_" . $srvid);
					exit(1);	
			}
		} else {
				return false;	
		}
	}
	function hasRight($k,$do_redir=true) {
		if($this->rights[super_user][0] == "true") {
			
			return true;	
		}
		
		if($this->rights[$k] && $this->rights[$k][0] != "false") {
			
				return true;
		} else {
			if($do_redir == true) {
				if(!preg_match("/error.php/" , $_SERVER[SCRIPT_NAME])) {
					$this->redirectError("BARTLBY::RIGHT::MISSING", "&right=" . $k);
					exit(1);	
				}
			} else {
				return false;	
			}
		}	
	}
	function loadForeignRights($user) {
		if(!file_exists("rights/" . $user . ".dat")) {
			copy("rights/template.dat", "rights/" . $user . ".dat");
		}
		if(file_exists("rights/" . $user . ".dat")) {
			$fa=file("rights/" . $user . ".dat");
			while(list($k, $v) = each($fa)) {
				$s1=explode("=", $v);
				$r[$s1[0]]=explode(",", trim($s1[1]));
				
				
			}
			for($x=0; $x<count($r[services]); $x++) {
					if($r[services][$x] == "") {
						$r[services][$x]=-4;
						continue;
					}
						
					settype($r[services][$x], "integer");
			}
			for($x=0; $x<count($r[servers]); $x++) {
					if($r[servers][$x] == "") {
						$r[servers][$x]=-4;
						continue;
					}
					settype($r[servers][$x], "integer");
			}
			
			for($x=0; $x<count($r[selected_servers]); $x++) {
					if($r[selected_servers][$x] == "") {
						$r[selected_servers][$x]=-4;
						continue;
					}
					settype($r[selected_servers][$x], "integer");
			}
			for($x=0; $x<count($r[selected_services]); $x++) {
					if($r[selected_services][$x] == "") {
						$r[selected_services][$x]=-4;
						continue;
					}
					settype($r[selected_services][$x], "integer");
			}
			
			
		} else {
			if(!preg_match("/error.php/" , $_SERVER[SCRIPT_NAME])) {
				$this->redirectError("BARTLBY::RIGHT::FILE::NOT::FOUND");
				exit(1);	
			}
		}
		
		if($r[servers][0] == 0) {
			$r[servers]=null;
		}
		
		if($r[services][0] == 0) {
			
			$r[services]=null;
		}
		
		
		// if is super_user ALL services and servers are allowed
		return $r;
		
	}
	function loadRights() {
		if(!file_exists("rights/" . $this->user_id . ".dat")) {
			copy("rights/template.dat", "rights/" . $this->user_id . ".dat");
		}
		if(file_exists("rights/" . $this->user_id . ".dat")) {
			$fa=file("rights/" . $this->user_id . ".dat");
			while(list($k, $v) = each($fa)) {
				$s1=explode("=", $v);
				$this->rights[$s1[0]]=explode(",", trim($s1[1]));
				
			}
			for($x=0; $x<count($this->rights[services]); $x++) {
					if($this->rights[services][$x] == "") {
						$this->rights[services][$x]=-4;
						continue;
					}
					settype($this->rights[services][$x], "integer");
			}
			for($x=0; $x<count($this->rights[servers]); $x++) {
					if($this->rights[servers][$x] == "") {
						$this->rights[servers][$x]=-4;
						continue;
					}
					settype($this->rights[servers][$x], "integer");
			}
			for($x=0; $x<count($this->rights[selected_servers]); $x++) {
					if($this->rights[selected_servers][$x] == "") {
						$this->rights[selected_servers][$x]=-4;
						continue;
					}
					settype($this->rights[selected_servers][$x], "integer");
			}
			for($x=0; $x<count($this->rights[selected_services]); $x++) {
					if($this->rights[selected_services][$x] == "") {
						$this->rights[selected_services][$x]=-4;
						continue;
					}
					settype($this->rights[selected_services][$x], "integer");
			}
		} else {
			if(!preg_match("/error.php/" , $_SERVER[SCRIPT_NAME])) {
				$this->redirectError("BARTLBY::RIGHT::FILE::NOT::FOUND");
				exit(1);	
			}
		}
		if($this->rights[servers][0] == "0") {
			if($this->rights[services][0] ==  "0") {
				$this->rights[servers]=null;
			} else {
				$this->rights[servers][0]=-1;
			}
		}
		
		if($this->rights[services][0] ==  "0") {
			if($this->rights[servers][0] ==  "0") {
				$this->rights[services]=null;
			} else {
				$this->rights[services][0]=-1;	
			}
		}
		
		// if is super_user ALL services and servers are allowed
		
		if($this->user == @bartlby_config("ui-extra.conf", "super_user") || $this->rights[super_user][0] == "true") {
		
				$this->rights[services]=null;
				$this->rights[servers]=null;
				$this->rights[super_user][0]=true;
		}
		
	}
	function getRelease() {
		return $this->release;	
	}
	function getInfo() {
		return @bartlby_get_info($this->CFG);	
	}
	
	function perform_auth($a=true) {
		$wrks=$this->GetWorker(false);
		$auted=0;
		if($a==false) {
			$auted=1;
		} else {
			
			while(list($k, $v) = each($wrks)) {
				if($_SESSION[username] != "" && $_SESSION[password] != "") {
					
					$_SERVER[PHP_AUTH_USER]=$_SESSION[username];
					$_SERVER[PHP_AUTH_PW]=$_SESSION[password];
				}
				if($_SERVER[PHP_AUTH_USER] == $v[name] && (md5($_SERVER[PHP_AUTH_PW]) == $v[password] || $_SERVER[PHP_AUTH_PW] == $v[password])) {
					//FIXME: remove back. comp. to plain pass'es
					$auted=1;
					$this->user_id=$v[worker_id];
				}
			}
		}
		if($auted == 0 && $_SESSION[username] != "") {
			$this->redirectError("BARTLBY::LOGIN");
		}
		if ($auted==0) { 
			
			 session_destroy();
	      		 @header("WWW-Authenticate: Basic realm=\"Bartlby Config Admin\"");	
	      		 @Header("HTTP/1.0 401 Unauthorized");
	      		 $this->_log("Login attempt from " . $_SERVER[REMOTE_ADDR] . " User: '" . $_SERVER[PHP_AUTH_USER] . "'  Pass: '" . $_SERVER[PHP_AUTH_PW] . "'"); 
			 $this->redirectError("BARTLBY::LOGIN");
			 exit;
		} else {
			$this->user=$_SERVER[PHP_AUTH_USER];
			$this->pw=$_SERVER[PHP_AUTH_PW];
			
			
			
		}
	}
	function _log($str) {
		$logfile=bartlby_config($this->CFG, "logfile");
		if(!$logfile) {
			echo $str;	
		} else {
			$logfile = $logfile . "." . date("Y.m.d");
			$str=str_replace("\r", "", $str);
			$str=str_replace("\n", "", $str);
			$str=str_replace(";", "", $str);
			$str=str_replace(">", "", $str);
			$str=str_replace("<", "", $str);
			
			
			$logline=date("d.m.Y H:i:s") . ";" . "[" . posix_getpid() . "];" . $str . ";\n";
			
			$fp=@fopen($logfile, "a+");
			@fwrite($fp, $logline);
			@fclose($fp);
			@chmod($logfile, 0666);
			
		}
	}
	function redirectError($msg, $qs="") {
		
		//header("Location: error.php?msg=" . $msg);	
		
		if(!preg_match("/error.php/" , $_SERVER[SCRIPT_NAME])) {
			echo "<script>parent.location.href='error.php?msg=$msg" . $qs . "';</script>";
			exit;
		}
		
	}
	function findSHMPlace($svcid) {
		$map=bartlby_svc_map($this->CFG, $this->rights[services], $this->rights[servers]);
		
		
		
		for($x=0; $x<count($map); $x++) {
			if($map[$x][service_id] == $svcid) {
				
				return 	$map[$x][shm_place];
				
			}
		}
		return -1;	
	}
	function isServerUp($server_id, &$map) {
		
		if($map[$server_id][0][current_state] == -3) {
			return false;	
		}
		
		for($x=0; $x<count($map[$server_id]); $x++) {
			if($map[$server_id][$x][current_state] == 0) {
				return true;
			}
		}
		return false;
	}
	function ServiceCount() {
		return $this->info[services];	
	}
	function GetWorker($do_check=true) {
		
		$r=array();
		for($x=0; $x<$this->info[workers]; $x++) {
			$wrk=bartlby_get_worker($this->CFG, $x);
			if($wrk[name] == "") {
				$x=0;
				continue;	
			}
			$wrk[shm_place]=$x;
			if($do_check == true) {
				if($wrk[name] != $this->user && !$this->hasRight("modify_all_workers", false)) {
					continue;
				}
			}
			//$r[$wrk[worker_id]]=$wrk[name];
			array_push($r, $wrk);
		}	
		return $r;
	}
	function GetServers() {
		
		$map=bartlby_svc_map($this->CFG,$this->rights[services], $this->rights[servers]);
		
		
		
		for($x=0; $x<count($map); $x++) {
			$servers[$map[$x][server_id]] = $map[$x][server_name];
			
		
		}
			
		
		
		return $servers;
	}
	function GetServices() {
		/*
		$ar=array();
		for($x=0; $x<$this->info[services]; $x++) {	
			$svc=bartlby_get_service($this->CFG, $x);
			array_push($ar, $svc);
		}
		return $ar;
		*/
		$map=bartlby_svc_map($this->CFG, $this->rights[services], $this->rights[servers]);
		
		
		
		for($x=0; $x<count($map); $x++) {
			//$servers[$map[$x][server_id]] = $map[$x][server_name];
			array_push($ar, $map[$x]);
		}
		return $ar;
			
		
	}
	function is_gone($state) {
		switch($state) {
			case 1:
				return "<img src='themes/" . $this->theme . "/images/emblem-generic.png' alt='Object changed you should reload' border=0>";
			case 2:
				return "<img src='themes/" . $this->theme . "/images/emblem-important.png' alt='Object deleted you should reload' border=0>";
			default:
				return "";
		}
	}	
	function GetSVCMap($state=false) {
		//array(2555, 3191,2558)
		#view_service_output
		$has_right = $this->hasRight("view_service_output", false);
		
		$r=bartlby_svc_map($this->CFG, $this->rights[services], $this->rights[servers]);
        
        	
        	//Re order map ;-)
        	
        	
        	for($x=0; $x<count($r); $x++) {
        		if(!is_array($map[$r[$x][server_id]])) {
        			$map[$r[$x][server_id]] = array();
        			
        		}
        		if(!$has_right) {
        			$r[$x][new_server_text] = "you are missing: view_service_output right";	
        		}
        		
			
        		array_push($map[$r[$x][server_id]], $r[$x]);
        	}
        	@ksort($map);
        	
 		return $map; 
			
	}
	function getColor($state) {
		switch($state) {
			case 0: return "green"; break;
			case 1: return "orange"; break;
			case 2: return "red"; break;
			case -3: return "silver"; break;
			
			default:
				return "silver";
				
			
		}
	}


	function getState($state) {
		switch($state) {
			case 0: return "OK"; break;
			case 1: return "WARNING"; break;
			case 2: return "CRITICAL"; break;
			case 3: return "UNKOWN"; break;
			case 4: return "INFO"; break;
			case 5: return "TRIGGER"; break;
			case 6: return "FLAPPING"; break;
			case -3: return "S-DEAD"; break;
			default:
			return "UNKOWN($state)";
			
		}
	}
	function intervall($sek) {
		// http://faq-php.de/q/q-code-intervall.html
    		$i = sprintf('%d Day%s, %d Hour%s,'.
            		' %d Minute%s and %d Second%s',
            		$sek / 86400,
            		floor($sek / 86400) != 1 ? 's':'',
            		$sek / 3600 % 24,
            		floor($sek / 3600 % 24) != 1 ? 's':'',
            		$sek / 60 % 60,
            		floor($sek / 60 % 60) != 1 ? 's':'',
            		$sek % 60,
            		floor($sek % 60) != 1 ? 's':''
         	);
    		return $i;
	}
	function setTheme($name="classic") {
		if($name=="") $name="classic";
		$this->theme=$name;	
	}
	
	function getExtensionsReturn($method, $layout, $ign=false) {
		$r=array();
		$dhl = opendir("extensions");
		while($file = readdir($dhl)) {
			if($file != "." && $file != ".." && !preg_match("/.*\.disabled/", $file)) {
				if($ign == false && file_exists("extensions/" .  $file . ".disabled")) {
						continue;
				}
				@include_once("extensions/" . $file . "/" . $file . ".class.php");
				
				
				if (class_exists($file)) {
					eval("\$clh = new " . $file . "();");
					if(method_exists($clh, $method)) {
						eval("\$o = \$clh->" . $method . "();");
						$ex[ex_name]=$file;
						$ex[out] = $o;
						
						if($o != "") {
							array_push($r, $ex);
							
							if(!file_exists("extensions/" . $file . ".disabled")) {
								$endis="<tr><td colspan=2 align=right><a href=\"javascript:void(0);\" onClick=\"xajax_toggle_extension('$file')\" title='$file extension is enabled click to change'><img id='extension_img_$file' border=0 src='themes/" . $this->theme . "/images/extension_enable.gif'></A></td></tr>";
							} else {
								$endis="<tr><td colspan=2 align=right><a href=\"javascript:void(0);\" onClick=\"xajax_toggle_extension('$file')\" title='$file extension is disabled click to change'><img id='extension_img_$file' border=0 src='themes/" . $this->theme . "/images/extension_disable.gif'></A></td></tr>";	
							}
							
							
							$info_box_title='Extension: ' . $this->wikiLink("ui-extensions:" . $ex[ex_name], $ex[ex_name]);  
							// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
							$core_content = "<table  width='100%'>
								<tr>
									<td colspan=2>" . $ex[out] .  "</td> 
								</tr>
								$endis
								
								
								
							</table>";
							
							
							$layout->create_box($info_box_title, $core_content, "extension_" . $ex[ex_name]);
						}
								
						
						
					}
					
					
				}
			}
		}
		closedir($dhl);
		return $r;	
		
	}
	function wikiLink($page_name, $display) {
		return "<a target='_blank' href='http://wiki.bartlby.org/dokuwiki/doku.php?id=" . $page_name . "'>" . $display . "</A>";	
	}
	function installPackage($pkg, $server, $force_plugin, $force_perf, $my_path="") {
		$basedir=bartlby_config($this->CFG, "basedir");
		
		
		if($basedir) {
			$perf_dir=$basedir . "/perf/";	
		}
		
		$plugin_dir=bartlby_config($this->CFG, "agent_plugin_dir");
		
		
		
		
		$msg = "Installing package '$pkg' on Server:  $server<br>";
		if($my_path == "") {
			$fp=@fopen("pkgs/" . $pkg, "r");
		} else {
			$fp=@fopen($my_path . $pkg, "r");	
		}
		if($fp) {
			while(!feof($fp)) {
				$bf .= fgets($fp, 1024);	
			}
			$re=unserialize($bf);
			fclose($fp);
			for($x=0; $x<count($re); $x++) {
				$msg .= "Installing Service: <b>" . $re[$x][service_name] . "</b><br>";	
				
				
				
				$msg .= str_repeat("&nbsp;", 20) . "Plugin:" . $re[$x][plugin] . "/'" . $re[$x][plugin_arguments] . " '<br>";	
				$msg .= str_repeat("&nbsp;", 20) . "Check Plan: " . $this->resolveServicePlan($re[$x][exec_plan]) . "<br>";	
				$msg .= str_repeat("&nbsp;", 20) . "Service Type: " . $re[$x][service_type] . "<br>";
				
				$ads=bartlby_add_service($this->CFG, $server, $re[$x][plugin],$re[$x][service_name],$re[$x][plugin_arguments],$re[$x][notify_enabled],$re[$x][exec_plan],$re[$x][check_interval],$re[$x][service_type],$re[$x][service_var], $re[$x][service_passive_timeout], $re[$x][service_check_timeout], $re[$x][service_ack], $re[$x][service_retain],$re[$x][service_snmp_community], $re[$x][service_snmp_objid],$re[$x][service_snmp_version],$re[$x][service_snmp_warning],$re[$x][service_snmp_critical],$re[$x][service_snmp_type], $re[$x][service_active], $re[$x][flap_seconds],$re[$x][renotify_interval],$re[$x][escalate_divisor]);
				$msg .= str_repeat("&nbsp;", 20) . "New id: " . $ads . "<br>";
				
				if($re[$x][__install_plugin]) {
					$msg .= str_repeat("&nbsp;", 20) . "Installing plugin: " . $re[$x][plugin] . "<br>";	
					
					if(!file_exists($plugin_dir . "/" . $re[$x][plugin]) || $force_plugin == "checked") {
						$plugin=@fopen($plugin_dir . "/" . $re[$x][plugin], "wb");
						if($plugin){
							fwrite($plugin, $re[$x][__install_plugin]);
							fclose($plugin);
							@chmod($plugin_dir . "/" . $re[$x][plugin], 0777);
						} else {
							$msg .= str_repeat("&nbsp;", 25) . " plugin fopen( " . $plugin_dir . "/" . $re[$x][plugin] . ") failed<br>";
						}
					} else {
						$msg .= 	str_repeat("&nbsp;", 25) .  "plugin (" . $plugin_dir . "/" . $re[$x][plugin] . ") already existing<br>";
					}
					
				}
				if($re[$x][__install_perf]) {
					$msg .= str_repeat("&nbsp;", 20) . "Installing perf handler: " . $re[$x][plugin] . "<br>";	
					
					if(!file_exists($perf_dir . "/" . $re[$x][plugin]) || $force_perf == "checked") {
						$perf=@fopen($perf_dir . "/" . $re[$x][plugin], "wb");
						if($perf){
							fwrite($perf, $re[$x][__install_perf]);
							fclose($perf);
							@chmod($perf_dir . "/" . $re[$x][plugin], 0777);
						} else {
							$msg .= str_repeat("&nbsp;", 25) . " fopen( " . $perf_dir . "/" . $re[$x][plugin] . ") failed<br>";
						}
					} else {
						$msg .= 	str_repeat("&nbsp;", 25) .  "plugin (" . $re[$x][plugin] . ") already existing<br>";
					}
					
				}
				if($re[$x][__install_perf_default]) {
					$msg .= str_repeat("&nbsp;", 20) . "Installing perf handler (default): " . $re[$x][plugin] . "<br>";	
					
					if(!file_exists($perf_dir . "/defaults/" . $re[$x][plugin] . ".rrd") || $force_perf == "checked") {
						$perf=@fopen($perf_dir . "/defaults/" . $re[$x][plugin] . ".rrd", "wb");
						if($perf){
							fwrite($perf, $re[$x][__install_perf_default]);
							fclose($perf);
							@chmod($perf_dir . "/defaults/" . $re[$x][plugin] . ".rrd", 0777);
						} else {
							$msg .= str_repeat("&nbsp;", 25) . " fopen( " . $perf_dir . "/" . $re[$x][plugin] . ") failed<br>";
						}
					} else {
						$msg .= 	str_repeat("&nbsp;", 25) .  "plugin (" . $re[$x][plugin] . ") already existing<br>";
					}
					
				}
				
				
				

			}
			$layout->OUT .= "<script>doReloadButton();</script>";
		} else {
			$msg = "fopen failed()!!<br>";	
		}
		
		return $msg;	
	}
	function create_pagelinks($link, $max, $hm=20, $curp, $si) {
		
		$pages       = 1;
		
		if ( ($max % $hm) == 0 ) {
			$pages= $max / $hm;
		} else {
			$number = ($max / $hm);
			$pages= ceil( $number);
		}
		
		$currpage = $curp > 0 ? $curp : 1;
	
		if ($pages> 1) {
			$first = "<a href='$link&" . $si . "=1'>&laquo;</a>";
			for( $i = 0; $i <= $pages - 1; $i++ ) {
				$times = $i+1;
				if ($times == $curp) {
					$pageline .= "&nbsp;<b>$times</b>";
				} else {
					if ($times < ($currpage - 5) and ($currpage >= 6))  {
						$startdots = '&nbsp;...';
						continue;
					}
					$pageline .= "&nbsp;<a href='$link&" . $si . "=$times'>$times</a>";
					if ($times >= ($currpage + 5)) {
						$enddots = '...&nbsp;';
						break;
					}
				}
			}
			$last = "<a href='$link&" . $si . "=".$pages."'>&raquo;</a>";
			$ret    = $first.$startdots.$pageline.'&nbsp;'.$enddots.$last;
		} else {
			$ret    = "Pages: 1";
		}
	
		return $ret;
	}
	function XMLBoxHealth(&$status, &$remote_data) {
		
		$hosts_up=0;
		$hosts_down=0;
		$services_critical=0;
		$services_ok=0;
		$services_warning=0;
		$services_unkown=0;
		$services_downtime=0;
		$all_services=0;
		$acks_outstanding=0;
		for($rx=0; $rx<=count($remote_data);$rx++) {
			$servers=$remote_data[$rx];
			$hosts_sum +=count($servers);
			
			while(list($k,$v)=@each($servers)) {
				$x=$k;
				if($this->isServerUp($x, $servers)) {
					$hosts_up++;	
				} else {
					$hosts_down++;	
					$hosts_a_down[$k]=1;
					
				}
				
				
				for($y=0; $y<count($v); $y++) {
					
					$qck[$v[$y][server_name]][$v[$y][current_state]]++;	
					$qck[$v[$y][server_name]][10]=$v[$y][server_id];
					$qck[$v[$y][server_name]][server_icon]=$v[$y][server_icon];
					if($v[$y][is_downtime] == 1) {
						$qck[$v[$y][server_name]][$v[$y][current_state]]--;
						$qck[$v[$y][server_name]][downtime]++;
						
					}
					if($v[$y][service_ack] == 2) {
						$qck[$v[$y][server_name]][acks]++;	
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
		$r[prozent_float] = $prozent_float;
		$r[color] = $color;
		$r[qck]=$qck;
		
		
		$r[services_critical]=$services_critical;
		$r[services_warning]=$services_warning;
		$r[services_ok]=$services_ok;
		$r[services_downtime]=$services_downtime;
		$r[hosts_up]=$hosts_up;
		$r[hosts_down]=$hosts_down;
		$r[acks_outstanding]=$acks_outstanding;
		$r[service_sum] = $service_sum;
		$r[hosts_sum] = $hosts_sum;
		
		return $r;
		
	
	}
	function appendXML_to_svc_map($xml_data,$alias, &$map, $xml_id) {
		
		for($x=0; $x<count($xml_data); $x++) {
			
			while(list($k, $v) = each($xml_data[$x])) {
				for($y=0; $y<count($v); $y++) {
					if(!is_array($map["XML:" . $xml_id . ":" . $k])) {
						$map["XML:" . $xml_id . ":" . $k]=array();
					}
					$v[$y][server_name] = $alias . "-->" . $v[$y][server_name];
					$v[$y][server_id] = "XML:" . $xml_id . ":" . $v[$y][server_id];
					$v[$y][service_id] = "XML:" . $xml_id . ":" . $v[$y][service_id];
					$v[$y][shm_place] = "XML:" . $xml_id . ":" . $v[$y][shm_place];
					
					 array_push($map["XML:" . $xml_id . ":" . $k], $v[$y]);
				}
			}
		}
	}
	
	function XMLQuickView($status, $qck, $xml_id) {
		$quick_view="<table width=760>";
		while(list($k, $v)=@each($qck)) {
			
			if($k != $last_qck) {
				$cl="";
				$STATE="UP";
				if ($hosts_a_down[$qck[$k][10]] == 1) {
					$cl="";
					$STATE="DOWN";
				}
				$quick_view .= "<tr>";
				$quick_view .= "<td class=$cl><img src='server_icons/" . $qck[$k][server_icon] . "'><font size=1><a href='services.php?server_id=XML:" . $xml_id . ":" . $qck[$k][10] . "'>$k</A></td>";
				$quick_view .= "<td class=$cl><font size=1>$STATE</td>";
				$quick_view .= "<td class=$cl><table width=100>";
				
				$sf=false;
				if($qck[$k][0]) {
					$sf=true;
					$qo="<tr><td class=green_box><font size=1><a href='services.php?server_id=XML:" . $xml_id . ":" . $qck[$k][10] . "&expect_state=0'>" . $qck[$k][0] . " OK's</A></td></tr>";
				}
				if($qck[$k][1]) {
					$sf=true;
					$qw="<tr><td class=orange_box><font size=1><a href='services.php?server_id=XML:" . $xml_id . ":" . $qck[$k][10] . "&expect_state=1'>" . $qck[$k][1] . " Warnings</A></td></tr>";
				}
				
				if($qck[$k][2]) {
					$sf=true;
					$qc="<tr><td class=red_box><font size=1><a href='services.php?server_id=XML:" . $xml_id . ":" . $qck[$k][10] . "&expect_state=2'>" . $qck[$k][2] . " Criticals</A></td></tr>";
				}
				
				if($qck[$k][3]) {
					$sf=true;
					$qk="<tr><td class=silver_box><font size=1><a href='services.php?server_id=XML:" . $xml_id . ":" . $qck[$k][10] . "&expect_state=3'>" . $qck[$k][3] . " Unkown</A></td></tr>";
				}
				if($qck[$k][4]) {
					$sf=true;
					$qk="<tr><td class=silver_box><font size=1>" . $qck[$k][4] . " Info</td></tr>";
				}
				if($qck[$k][downtime]) {
					$qk="<tr><td class=silver_box><font size=1>" . $qck[$k][downtime] . " Downtime</td></tr>";
				}
				if($qck[$k][acks]) {
					$qk="<tr><td class=silver_box><font size=1><a href='services.php?server_id=XML:" . $xml_id . ":" . $qck[$k][10] . "&expect_state=2&acks=yes'>" . $qck[$k][acks] . " Ack Wait</A></td></tr>";
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
		
		$quick_view .= "</table>";
		
		return $quick_view;		
	}
	function XMLRemoteConfig($xml_id, $file, $var) {
		include_once("IXR_Library.inc.php");
		
		$url=bartlby_config("ui-extra.conf", "xml_remote[" . $xml_id. "]");
		$alias=bartlby_config("ui-extra.conf", "xml_alias[" . $xml_id. "]");
		if(preg_match("/http:\/\/(.*):(.*)@(.*):([0-9]+)\/(.*)/i", $url, $match)) {
			$uname=$match[1];
			$pw=$match[2];
			$port=$match[3];
			$e_url="http://" . $match[3] . "/" . $match[5];
			
		}
			
		$client = new IXR_ClientMulticall($e_url, false, $port, $uname, $pw);
		$client->debug=false;
		$client->addCall('bartlby.config', $file, $var);	
		$client->query();
		$response = $client->getResponse();
		
		
		return $response[0][0];
	}	
	function remoteServerByID($xml_id, $server_id) {
		include_once("IXR_Library.inc.php");
		
		$url=bartlby_config("ui-extra.conf", "xml_remote[" . $xml_id. "]");
		$alias=bartlby_config("ui-extra.conf", "xml_alias[" . $xml_id. "]");
		if(preg_match("/http:\/\/(.*):(.*)@(.*):([0-9]+)\/(.*)/i", $url, $match)) {
			$uname=$match[1];
			$pw=$match[2];
			$port=$match[3];
			$e_url="http://" . $match[3] . "/" . $match[5];
			
		}
			
		$client = new IXR_ClientMulticall($e_url, false, $port, $uname, $pw);
		$client->debug=false;
		$client->addCall('bartlby.get_server_by_id', $server_id);	
		$client->query();
		$response = $client->getResponse();
		
		$response[0][0][server_name] = $alias . "-->" . $response[0][0][server_name];
		$response[0][0][server_id] = "XML:" . $xml_id . ":" . $response[0][0][server_id];
		$response[0][0][service_id] = "XML:" . $xml_id . ":" . $response[0][0][service_id];
		$response[0][0][shm_place] = "XML:" . $xml_id . ":" . $response[0][0][shm_place];
		
		
		return $response[0][0];
	}
	function remoteServiceByID($xml_id, $service_id) {
		include_once("IXR_Library.inc.php");
		
		$url=bartlby_config("ui-extra.conf", "xml_remote[" . $xml_id. "]");
		$alias=bartlby_config("ui-extra.conf", "xml_alias[" . $xml_id. "]");
		if(preg_match("/http:\/\/(.*):(.*)@(.*):([0-9]+)\/(.*)/i", $url, $match)) {
			$uname=$match[1];
			$pw=$match[2];
			$port=$match[3];
			$e_url="http://" . $match[3] . "/" . $match[5];
			
		}
			
		$client = new IXR_ClientMulticall($e_url, false, $port, $uname, $pw);
		$client->debug=false;
		$client->addCall('bartlby.get_service', $service_id);	
		$client->query();
		$response = $client->getResponse();
		
		$response[0][0][server_name] = $alias . "-->" . $response[0][0][server_name];
		$response[0][0][server_id] = "XML:" . $xml_id . ":" . $response[0][0][server_id];
		$response[0][0][service_id_real] = $response[0][0][service_id];
		$response[0][0][service_id] = "XML:" . $xml_id . ":" . $response[0][0][service_id];
		$response[0][0][shm_place] = "XML:" . $xml_id . ":" . $response[0][0][shm_place];
		
		
		return $response[0][0];
	}
	function getRemoteStatus($url, $alias) {
		//Check for IXR
		//Call bartlby_info -> remote
		//call svc_map
		//return percentage
		//return quick_view
		//return svc_array
		
		include_once("IXR_Library.inc.php");
		
		//get uname and pw
		if(preg_match("/http:\/\/(.*):(.*)@(.*):([0-9]+)\/(.*)/i", $url, $match)) {
			$uname=$match[1];
			$pw=$match[2];
			$port=$match[3];
			$e_url="http://" . $match[3] . "/" . $match[5];
			
			$client = new IXR_ClientMulticall($e_url, false, $port, $uname, $pw);
			$client->debug=true;
			$client->addCall('bartlby.get_info');	
			$client->addCall('bartlby.get_service_map');	
			$client->query();
			$client->debug=false;
			$response = $client->getResponse();
		
			if(!$response) {
				return false;	
			}
			$r_array[info]=$response[0];
			$r_array[services]=$response[1];
			$r_array[url]=$url;
			$r_array[alias]=$alias;
			
			
			
			return $r_array;
			
		}
		
			
	}
	
function create_package($package_name, $in_services = array(), $with_plugins, $with_perf, $my_path="") {
		$pkg=array();
		$basedir=bartlby_config($this->CFG, "basedir");
		if($basedir) {
			$perf_dir=$basedir . "/perf/";	
		}
		$plugin_dir=bartlby_config($this->CFG, "agent_plugin_dir");
		
		if($in_services) {
			
			//$msg = "Creating package: " . $_GET[package_name] . "<br>";
			for($x=0; $x<$this->info[services]; $x++) {
				$svc=bartlby_get_service($this->CFG, $x);
				$svc=bartlby_get_service_by_id($this->CFG, $svc[service_id]);
				
				if(@in_array($svc[service_id], $in_services)) {
					
					$re .="<li>" . $svc[server_name] . ":" . $svc[client_port] . "/" . $svc[service_name];
					
					if($with_plugins) {
						
						if(file_exists($plugin_dir . "/" . $svc[plugin])) {
							$svc[__install_plugin]="";	
							$fp = fopen($plugin_dir . "/" . $svc[plugin], "rb");
							if($fp) {
								while(!feof($fp)) {
									$svc[__install_plugin] .= fgets($fp, 1024);
								}
								fclose($fp);
								$re .= "<li> ---> added plugin " . $svc[plugin] . " to package <br>";
							} else {
								$re .= " Plugin open failed (" . $svc[plugin] . ")<br>";
							}
							
							
						}
					}
					if($with_perf) {
						
						if(file_exists($perf_dir . "/" . $svc[plugin])) {
							$svc[__install_perf]="";	
							$fp1 = fopen($perf_dir . "/" . $svc[plugin], "rb");
							if($fp1) {
								while(!feof($fp1)) {
									$svc[__install_perf] .= fgets($fp1, 1024);
								}
								fclose($fp1);
								$re .= "<li> ---> added perf handler " . $svc[plugin] . " to package <br>";
							} else {
								$re .= " Plugin open failed (" . $svc[plugin] . ")<br>";
							}
							
							
						}
						if(file_exists($perf_dir . "/defaults/" . $svc[plugin] . ".rrd")) {
							$svc[__install_perf_default]="";	
							$fp1 = fopen($perf_dir . "/defaults/" . $svc[plugin] . ".rrd", "rb");
							if($fp1) {
									while(!feof($fp1)) {
										$svc[__install_perf_default] .= fgets($fp1, 1024);
									}
									fclose($fp1);
									$re .= "<li> ---> added perf handler (default) " . $svc[plugin] . ".rrd to package <br>";
							} else {
								$re .= " Plugin open failed (" . $svc[plugin] . ")<br>";
							}
							
							
						}						
					}
					
					
					array_push($pkg, $svc);
				}
				
			}
			$save=serialize($pkg);
			if($my_path == "") {
				$fp=@fopen("pkgs/" . $package_name, "w");
			} else {
				$fp=@fopen($my_path . $package_name, "w");	
			}
			if($fp) {
				fwrite($fp, $save);
				fclose($fp);
			} else {
				$re = "save failed";	
			}
		}
		
		
			return $re;	
	}
	function getWorkerOptions($defaults, $layout) {
		//return worker options
		$act[0][c]="";
		$act[0][v]="0";
		$act[0][k]="Inactive";
		if($defaults[active] == 0) {
			$act[0][s]=1;
		}
		
		$act[1][c]="";
		$act[1][v]="1";
		$act[1][k]="Active";
		if($defaults[active] == 1) {
			$act[1][s]=1;
		}
		$act[2][c]="";
		$act[2][v]="2";
		$act[2][k]="Standby";
		if($defaults[active] == 2) {
			$act[2][s]=1;
		}
		
		
		
		$dropdown = $layout->DropDown("worker_active", $act, " onChange='updateWorkerState(" . $defaults[worker_id] . ", this);'");
		
		return $dropdown;
		
	}
	function getserveroptions($defaults, $layout) {
		$modify = "<a href='modify_server.php?server_id=" . $defaults[server_id] . "'><img src='themes/" . $this->theme . "/images/modify.gif' title='Modify this server' border=0></A>";
		$copy = "<a href='modify_server.php?copy=true&server_id=" . $defaults[server_id] . "'><img src='themes/" . $this->theme . "/images/edit-copy.gif' title='Copy (Create a similar) this Server' border=0></A>";
		$logview= "<a href='logview.php?server_id=" . $defaults[server_id]. "' ><font size=1><img  title='View Events for this Server' src='themes/" . $this->theme . "/images/icon_view.gif' border=0></A>";
		
		if($defaults[server_enabled] == 1) {
			$check = "<a title='Disable Checks for this Server' href='javascript:void(0);' onClick=\"xajax_toggle_server_check('" . $defaults[server_id] . "', '" . $defaults[service_id] . "')\"><img id='server_" . $defaults[server_id] . "' src='themes/" . $this->theme . "/images/enabled.gif'  border=0></A>";
		} else {
			$check = "<a href='javascript:void(0);' onClick=\"xajax_toggle_server_check('" . $defaults[server_id] . "', '" . $defaults[service_id] . "')\"><img src='themes/" . $this->theme . "/images/diabled.gif' id='server_" . $defaults[server_id] . "' title='Enable  Checks for this Service' border=0></A>";
		}
		if($defaults[server_notify] == 1) {
			$notifys = "<a href='javascript:void(0);' onClick=\"xajax_toggle_server_notify_check('" . $defaults[server_id] . "', '" . $defaults[service_id] . "')\"><img src='themes/" . $this->theme . "/images/trigger.gif' id='trigger_" . $defaults[server_id] . "' title='Disable Notifications for this Service' border=0 data-rel='tooltip'></A>";
		} else {
			$notifys = "<a href='javascript:void(0);' onClick=\"xajax_toggle_server_notify_check('" . $defaults[server_id] . "', '" . $defaults[service_id] . "')\"><img id='trigger_" . $defaults[server_id] . "' src='themes/" . $this->theme . "/images/notrigger.gif' title='Enable Notifications for this Service' border=0 data-rel='tooltip'></A>";
		}
		$is_gone=$this->is_gone($defaults[server_gone]);
		
		return $is_gone . " " . $notifys . " " .  $check . " " . $modify . " " . $copy . " " . $logview;
	}

	function getserviceOptions($defaults, $layout) {
		if($defaults[service_active] == 1) {
			$check = "<a data-rel='tooltip' title='Disable Checks for this Service' href='javascript:void(0);' onClick=\"xajax_toggle_service_check('" . $defaults[server_id] . "', '" . $defaults[service_id] . "')\"><img id='service_" . $defaults[service_id] . "' src='themes/" . $this->theme . "/images/enabled.gif'  border=0 data-rel='tooltip'></A>";
		} else {
			$check = "<a data-rel='tooltip' href='javascript:void(0);' onClick=\"xajax_toggle_service_check('" . $defaults[server_id] . "', '" . $defaults[service_id] . "')\"><img src='themes/" . $this->theme . "/images/diabled.gif' id='service_" . $defaults[service_id] . "' title='Enable  Checks for this Service' border=0 data-rel='tooltip'></A>";
		}
		if($defaults[notify_enabled] == 1) {
			$notifys = "<a data-rel='tooltip' href='javascript:void(0);' onClick=\"xajax_toggle_service_notify_check('" . $defaults[server_id] . "', '" . $defaults[service_id] . "')\"><img src='themes/" . $this->theme . "/images/trigger.gif' id='trigger_" . $defaults[service_id] . "' title='Disable Notifications for this Service' border=0 data-rel='tooltip'></A>";
		} else {
			$notifys = "<a data-rel='tooltip' href='javascript:void(0);' onClick=\"xajax_toggle_service_notify_check('" . $defaults[server_id] . "', '" . $defaults[service_id] . "')\"><img id='trigger_" . $defaults[service_id] . "' src='themes/" . $this->theme . "/images/notrigger.gif' title='Enable Notifications for this Service' border=0 data-rel='tooltip'></A>";
		}
		if($defaults[is_downtime] == 1) {
			$downtime="<img src='themes/" . $this->theme . "/images/icon_work.gif' data-rel='tooltip' title='Service is in downtime (" . date("d.m.Y H:i:s", $defaults[downtime_from])  . "-" . date("d.m.Y H:i:s", $servs[$x][downtime_to]) . "): " . $defaults[downtime_notice] . "'>";	
		} else {
			$downtime="&nbsp;";
		}
				
		
		$modify = "<a href='modify_service.php?service_id=" . $defaults[service_id] . "'><img data-rel='tooltip' src='themes/" . $this->theme . "/images/modify.gif' title='Modify this Service' border=0 data-rel='tooltip'></A>";
		$force = "<a href='javascript:void(0);' onClick=\"xajax_forceCheck('" . $defaults[server_id] . "', '" . $defaults[service_id] . "')\"><img title='Force an immediate Check' src='themes/" . $this->theme . "/images/force.gif' border=0 data-rel='tooltip'></A>";
		$comments="<a href='view_comments.php?service_id=" . $defaults[service_id] . "'><img title='Comments for this Service' src='themes/" . $this->theme . "/images/icon_comments.gif' border=0 data-rel='tooltip'></A>";
		$logview= "<a href='logview.php?service_id=" . $defaults[service_id]. "' ><font size=1><img  title='View Events for this Service' src='themes/" . $this->theme . "/images/icon_view.gif' border=0 data-rel='tooltip'></A>";				
		$reports = "<a href='create_report.php?service_id=" . $defaults[service_id]. "' ><font size=1><img  title='Create Report' src='themes/" . $this->theme . "/images/create_report.gif' border=0 data-rel='tooltip'></A>";				
		if(file_exists($this->PERFDIR . "/" . $defaults[plugin])) {
			$stat = "<a href='bartlby_action.php?service_id=" . $defaults[service_id] . "&server_id=" . $defaults[server_id] . "&action=perfhandler_graph'><img title='Graph collected perf handler data' src='themes/" . $this->theme . "/images/icon_stat.gif' border=0 data-rel='tooltip'></A>";				
		} else {
			$stat = "";
		}
		$copy = "<a href='modify_service.php?copy=true&service_id=" . $defaults[service_id] . "'><img src='themes/" . $this->theme . "/images/edit-copy.gif' title='Copy (Create a similar) this Service' border=0 data-rel='tooltip'></A>";				

		$is_gone=$this->is_gone($defaults[is_gone]);
				
		$ret ="$is_gone $notifys $check $logview $comments $modify $force $downtime $copy $reports $stat";
		
		
		return $ret;
	}
	function updatePerfHandler($srvId, $svcId) {
		$perf_dir=bartlby_config($this->CFG,"performance_dir");
		$btlhome=bartlby_config($this->CFG, "basedir");
		
		if(!$perf_dir) {
			$r ="'performance_dir' not set in bartlby core config file";	
		} else {
			
			$idx=$this->findSHMPlace($svcId);
			$r=$idx;
			$svc=bartlby_get_service($this->CFG, $idx);
			$cmd=$perf_dir . "/" . $svc[plugin];
			if(!file_exists($cmd)) {
				$r="Perfhandler '$cmd' does not exists";
			} else {
				
				$exec="export BARTLBY_CURR_SERVICE=\"" . $svc[service_name] . "\"; export BARTLBY_CURR_HOST=\"" . $svc[server_name] . "\"; export BARTLBY_CURR_PLUGIN=\"" . $svc[plugin] . "\"; export BARTLBY_HOME=\"$btlhome\"; export BARTLBY_CONFIG=\"" . $this->CFG . "\"; " . $cmd . "  graph " . $svc[service_id] . " 2>&1";
				
				$fp=popen($exec, "r");
				$output="<hr><pre>";
				while(!feof($fp)) {
					$r .= nl2br(fgets($fp));	
				}	
				pclose($fp);
				$r .="</pre><hr>";
				$r .= "<br> Perf handler called (see output above)";
			}
			
			
			
		}	
		return $r;
	}	
}
?>
