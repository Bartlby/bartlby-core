<?php
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	$btl=new BartlbyUi($Bartlby_CONF, true);
	$btl->hasRight("core.event_queue");
set_time_limit(0);
echo "<!---SMASH IE BUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFER--->\n";
echo "<script language='javascript'>window.external.setLoggedInName('" . $btl->user . "');</script>";
flush();

sleep(1);

$cfg=$btl->CFG;


$servers=$btl->GetSVCMap();

setVersionUptimeRoundTime();
echo "<script language='javascript'>window.external.cleartTreeview()</script>\n";
while(list($k,$v)=@each($servers)) {
	
	
	echo "<script language='javascript'>window.external.addServer('" . $k . "', '" . $v[0][server_name] . "');</script>";
	for($y=0; $y<count($v); $y++) {
		echo "<script language='javascript'>window.external.addService('" . $v[$y][server_id] . "', '" . $v[$y][service_id] . "', '" . $v[$y][service_name] . "', '" . $v[$y][current_state] . "',  '" . clean_ugly_string($v[$y][new_server_text]) . "');</script>\n";
	}	
}
echo "<script language='javascript'>window.external.FinishTree();</script>\n";

$proz = SetProz($servers, $btl);



echo "MAP loaded<br><!---SMASH IE BUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFER--->\n";
flush();

$last_event=bartlby_event_tick($cfg);
echo "curEV: $last_event <br>";
flush();
while(1) {
    $act=bartlby_event_tick($cfg);
    if($act != $last_event) {
        //output from last to act
        if($act < $last_event) {
            $last_event=0;   
        }
        echo "last event: $last_event actEV: $act <br>";
        flush();
        for($x=($last_event+1); $x<=$act; $x++) {
        	  sleep(1);
            $ev=bartlby_event_fetch($cfg, $x);
            
            unset($servers);
            $servers=$btl->GetSVCMap();
            setVersionUptimeRoundTime();
            $proz = SetProz($servers, $btl);
		  
		  output_ev($ev);
            
        }
        $last_event=$act;
        echo "<!---SMASH IE BUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFERBUFFER--->\n";
	flush();
	
		
       
    }
   	
   
    sleep(1);
    echo "<!--alive-->";
    setVersionUptimeRoundTime();
    flush();
}
function output_ev($ev = array()) {
		  $script_code="<script language='Javascript'>window.external.BaloonIT('" . $ev[id] . "', '" . clean_ugly_string($ev[message]) . "');</script>";
            echo "<hr>";
            echo clean_ugly_string($ev[message]);
          
            echo $script_code;
              echo "<hr>";
            echo "<br>";
            flush();
            flush();	
}
function clean_ugly_string($str) {
	$str = str_replace("\r", "",$str);
	$str = str_replace("'", "",$str);
	return $str;	
}
function setVersionUptimeRoundTime() {
	global $cfg, $btl;
	$info=@bartlby_get_info($cfg);
	if($info[round_time_count] > 0 &&  $info[round_time_sum] > 0 ) {
		$rndMS=round($info[round_time_sum] / $info[round_time_count], 2);
	} else {
		$rndMS=0;	
	}
	
		
	echo "<script language='Javascript'>window.external.setVersionUptimeRoundTime('" . $info[version] . "', '" . $btl->intervall(time()-$info[startup_time]) . "', '" . $rndMS . "');</script>";
	
}

function SetProz($servers = array(), $btl) {
	$hosts_up=0;
	$hosts_down=0;
	$services_critical=0;
	$services_ok=0;
	$services_warning=0;
	$services_unkown=0;
	$services_downtime=0;
	$all_services=0;
	$acks_outstanding=0;
	
	while(list($k,$v)=@each($servers)) {
	$x=$k;
	if($btl->isServerUp($x, $servers)) {
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

while(list($k, $v)=@each($qck)) {
		
		if($k != $last_qck) {
			$SRVID=$qck[$k][10];
			$SRVNAME=$k;
			$sf=false;
			echo "<script language='javascript'>window.external.setServerOWCDA('$SRVID', '$SRVNAME', '" . $qck[$k][0] . "', '" . $qck[$k][1] . "', '" . $qck[$k][2] . "', '" . $qck[$k][downtime] . "', '" .$qck[$k][acks] . "');</script>\n";
			
		
		$last_qck=$k;	
		$qo="";
		$qw="";
		$qc="";
		$qk="";
			
		
	}
}
echo "<script language='javascript'>window.external.setHostsUPDown('" . $hosts_up . "', '" . $hosts_down . "');</script>\n";
echo "<script language='javascript'>window.external.setOWCDA('" . $services_ok . "', '" . $services_warning . "', '" . $services_critical . "', '" . $services_downtime . "', '" . $acks_outstanding . "');</script>\n";
echo "<script language='javascript'>window.external.setLoggedInName('" . $btl->user . "');</script>";

echo "<script language='javascript'>window.external.bzsetBarColor('$color','$prozent_zahl');</script>\n";

			
}