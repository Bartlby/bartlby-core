<?
function dnl($i) {
	return sprintf("%02d", $i);
}
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("main.service_detail");
$layout= new Layout();
$layout->set_menu("main");
$layout->setTitle("Actions");

if($_GET[service_id]) {
	$_GET[service_place] = $btl->findSHMPlace($_GET[service_id]);	
}

if(preg_match("/^XML:(.*):(.*)$/i", $_GET[service_place], $match)) {
	$defaults=$btl->remoteServiceByID($match[1], $match[2]);
} else {
	$defaults=bartlby_get_service($btl->CFG, $_GET[service_place]);
	
	$btl->hasServerorServiceRight($defaults[service_id]);
}
if(!$defaults) {
	$btl->redirectError("BARTLBY::OBJECT::MISSING");
	exit(1);	
}

if(!$btl->hasRight("view_service_output", false)) {
	$defaults[new_server_text] = "you are missing: view_service_output right";	
} 

$map=$btl->getSVCMap($btl->CFG, NULL, NULL);
$layout->Table("100%");

$svc_color=$btl->getColor($defaults[current_state]);
$svc_state=$btl->getState($defaults[current_state]);

switch($defaults[service_ack]) {
	case 0:
		$needs_ack="no";
	break;
	case 1:
		$needs_ack="yes";
	break;		
	case 2:
		$needs_ack="outstanding <input type=button value='Acknowledge this problem' onClick=\"document.location.href='ack_service.php?service_id=" . $defaults[service_id]  . "';\">";
	break;
}


if($defaults[service_type] == 1) {
	$svc_type="Active";
}

if($defaults[service_type] == 2) {
	$svc_type="Passive";
}

if($defaults[service_type] == 3) {
	$svc_type="Group";
}
if($defaults[service_type] == 4) {
	$svc_type="Local";
}
if($defaults[service_type] == 5) {
	$svc_type="SNMP";
}
if($defaults[service_type] == 6) {
	$svc_type="NRPE";
}
if($defaults[service_type] == 7) {
	$svc_type="NRPE(ssl)";
}

if($defaults[service_type] == 8) {
	$svc_type="AgentV2";
}
if($defaults[service_type] == 9) {
	$svc_type="AgentV2(no-SSL)";
}

if($defaults["notify_enabled"]==1) {
	$noti_en="true";
} else {
	$noti_en="false";
}
if($defaults["service_active"]==1) {
	$serv_en="true";
} else {
	$serv_en="false";
}
//echo $defaults[last_notify_send] . "<br>";

if( $defaults[service_delay_sum] > 0 && $defaults[service_delay_count] > 0) {
	$svcDEL=round($defaults[service_delay_sum] / $defaults[service_delay_count], 2);
} else {
	$svcDEL=0;	
}



if( $defaults[service_time_sum] > 0 && $defaults[service_time_count] > 0) {
	$svcMS=round($defaults[service_time_sum] / $defaults[service_time_count], 2);
} else {
	$svcMS=0;	
}


$server_enabled="";

if($defaults[server_enabled] != 1) {
	$server_enabled=";<i>server disabled</i>";	
}

$server_noti_enabled="";

if($defaults[server_notify] != 1) {
	$server_noti_enabled=";<i>disabled via server</i>";	
}

if($defaults[check_starttime] != 0) {
	$currun=date("d.m.Y H:i:s", $defaults[check_starttime]) . " (PID: $defaults[check_is_running] )";
} else {
	$currun="<i>Currently not running</i>";	
}

$plan_box = $btl->resolveServicePlan($defaults[exec_plan]);


if($defaults[renotify_interval] != 0) {
		$renot_en="every " . $defaults[renotify_interval] . " runs ( " . $btl->intervall(($defaults[check_interval]*$defaults[renotify_interval])) . ") ";
} else {
		$renot_en="not enabled";
}
if($defaults[escalate_divisor] != 0) {
		$escal_en="every " . $defaults[escalate_divisor] . " runs (" . $btl->intervall(($defaults[check_interval]*$defaults[escalate_divisor])) . ")";
} else {
		$escal_en="not enabled";
}



$info_box_title='Service Info';  
// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
$core_content = "<table  width='100%'>
	<tr>
		<td width=150 class='font2'>Server:</td>
		<td align=left ><a href='server_detail.php?server_id=" . $defaults[server_id]  . "'>" . $defaults[server_name] . "</A> ( IP: " . gethostbyname($defaults[client_ip]) . " Port: " . $defaults[client_port] . " )</font> $server_enabled </td> 
		<td>&nbsp;</td>     
	</tr>
	<tr>
		<td width=150 class='font2'>Name:</td>
		<td align=left >" . $defaults[service_name] . "</font></td>  
		<td>&nbsp;</td>         
	</tr>
	<tr>
		<td width=150 class='font2'>ID:</td>
		<td align=left >" . $defaults[service_id] . "</font></td>  
		<td>&nbsp;</td>         
	</tr>
	<tr>
		<td width=150 class='font2'>Type:</td>
		<td align=left >" . $svc_type . "</font></td>  
		<td>&nbsp;</td>         
	</tr>
	<tr>
		<td width=150 class='font2'>Current State:</td>
		<td align=left><font color='$svc_color'>" . $svc_state . "</font></td> 
		<td>&nbsp;</td>          
	</tr>
	<tr>
		<td width=150 class='font2' Valign=top>Server Life Indicator:</td>
		<td align=left>" . $btl->resolveDeadMarker($defaults[server_dead], $map) . "</td> 
		<td>&nbsp;</td>          
	</tr>
	
	<tr>
		<td width=150 class='font2'>Last Check:</td>
		<td align=left >" . date("d.m.Y H:i:s", $defaults[last_check]) . "</font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>approx. next Check:</td>
		<td align=left >" . date("d.m.Y H:i:s", $defaults[last_check]+$defaults[check_interval]) . "</font></td>
		<td>&nbsp;</td>           
	</tr>
	
	<tr>
		<td width=150 class='font2'>Check intervall:</td>
		<td align=left >" . $defaults[check_interval_original] . " / " . $defaults[check_interval] .  "</font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Last Notify Send:</td>
		
		<td align=left >" . date("d.m.Y H:i:s", $defaults[last_notify_send]) . "</font></td>
		<td>&nbsp;</td>           
	</tr>
		<tr>
		<td width=150 class='font2'>Re-Notification Interval</td>
		
		<td align=left >" .  $renot_en ."</font></td>
		<td>&nbsp;</td>           
	</tr>
		<tr>
		<td width=150 class='font2'>Escalate after:</td>
		
		<td align=left >" . $escal_en  . "</font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Notify Enabled:</td>
		<td align=left >" . $noti_en . " $server_noti_enabled</font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Check Enabled:</td>
		<td align=left >" . $serv_en . "</font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2' valign=top>Check Plan:</td>
		<td align=left >" . $plan_box . "</td>
		<td>&nbsp;</td>           
	</tr>
	
	<tr>
		<td width=150 class='font2'>Flap count:</td>
		<td align=left >" . $defaults[flap_count] . "</font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Flap seconds:</td>
		<td align=left >" . $defaults[flap_seconds] . "</font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Ack settings:</td>
		<td align=left >" . $needs_ack . "</font></td>
		<td>&nbsp;</td>           
	</tr>

	<tr>
		<td width=150 class='font2'>Status:</td>
		<td align=left >" . $defaults[service_retain_current] . "/"  . $defaults[service_retain] . "</font></td>
		<td>&nbsp;</td>           
	</tr>	
	
		<tr>
		<td width=150 class='font2'>Is Running?:</td>
		<td align=left >" .  $currun . "</font></td>
		<td>&nbsp;</td>           
	</tr>	
	<tr>
		<td width=150 class='font2'>Average Check Time:</td>
		<td align=left >" .  $svcMS . " ms</font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Average delay Time:</td>
		<td align=left >" .  $svcDEL . " Seconds</font></td>
		<td>&nbsp;</td>           
	</tr>
	
</table>";

$layout->push_outside($layout->create_box($info_box_title, $core_content));

if($defaults[is_downtime] == 1) {
	$info_box_title='Downtime';  
	// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
	$core_content = "<table  width='100%'>
		<tr>
			<td width=150 class='font2'>From:</td>
			<td>" . date("d.m.Y H:i", $defaults[downtime_from]) . "</td> 
		</tr>
		<tr>
			<td width=150 class='font2'>To:</td>
			<td>" . date("d.m.Y H:i", $defaults[downtime_from]) . "</td> 
		</tr>
		<tr>
			<td width=150 class='font2'>Left:</td>
			<td>" . $btl->Intervall($defaults[downtime_to]-time()) . "</td> 
		</tr>
		<tr>
			<td width=150 class='font2'>Notice/Reason:</td>
			<td>" . $defaults[downtime_notice] . "</td> 
		</tr>
		
	</table>";
	
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	
}

$info_box_title='Last Output';  
// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
$core_content = "<table  width='100%'>
	<tr>
		<td>" . str_replace( "\\dbr", "<br>",nl2br(htmlspecialchars($defaults[new_server_text]))) . "</td> 
	</tr>
	
	
</table>";


$layout->push_outside($layout->create_box($info_box_title, $core_content));

if($defaults[service_type] == 2 || $defaults[service_type] == 1 || $defaults[service_type] == 4 || $defaults[service_type] == 6 || $defaults[service_type] == 7|| $defaults[service_type] == 8  || $defaults[service_type] == 9){
	$info_box_title='Plugin settings';  
	// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
	$core_content = "<table  width='100%'>
		<tr>
			<td width=150 class='font2'>Plugin:</td>
			<td align=left >" . htmlspecialchars($defaults[plugin]) . "</font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Plugin Arguments:</td>
			<td align=left >" . htmlspecialchars($defaults[plugin_arguments]) . "</font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Plugin Timeout:</td>
			<td align=left >" . $defaults[service_check_timeout] . " Seconds</font></td>
			<td>&nbsp;</td>           
		</tr>
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
}


if($defaults[service_type] == 5){
	if($defaults[service_snmp_type]  == 1) {
		$snmp_type = "Lower";
	} 
	if($defaults[service_snmp_type]  == 2) {
		$snmp_type = "Greater";
	} 
	$info_box_title='SNMP Service';  
	// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
	$core_content = "<table  width='100%'>
		<tr>
			<td width=150 class='font2'>Community:</td>
			<td align=left >" . $defaults[service_snmp_community] . "</font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Object ID:</td>
			<td align=left >" . $defaults[service_snmp_objid] . "</font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Protocol Version:</td>
			<td align=left >" . $defaults[service_snmp_version] . "</font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Warning:</td>
			<td align=left >" . $defaults[service_snmp_warning] . "</font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Critical:</td>
			<td align=left >" . $defaults[service_snmp_critical] . "</font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Match Type:</td>
			<td align=left >" . $snmp_type . "</font></td>
			<td>&nbsp;</td>           
		</tr>
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
}


if($defaults[service_type] == 2){
	
		
		$ibox[0][c]="green";
		$ibox[0][v]=0;	
		$ibox[0][k]="OK";
		$ibox[1][c]="orange";        
		$ibox[1][v]=1;	  
		$ibox[1][k]="Warning";
		$ibox[2][c]="red";        
		$ibox[2][v]=2;	  
		$ibox[2][k]="Critical";
		
		$ibox[$defaults[current_state]][s]=1;
		$state_dropdown=$layout->DropDown("passive_state", $ibox);
		
	$info_box_title='Passive Service';  
	// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
	$core_content = "<table  width='100%'>
		<form name='pp' action='bartlby_action.php' method=POST>
		<tr>
			<td width=150 class='font2'>Timeout:</td>
			<td align=left >" . $defaults[service_passive_timeout] . "</font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Submit manually:</td>
			<td align=left >$state_dropdown<input type='hidden' name='action' value='submit_passive'><input type='hidden' name='service_id' value='" . $defaults[service_id] . "'><input type='text' name='passive_text'><input type=submit value='store'></font></td>
			<td>&nbsp;</td>           
		</tr>
		</form>
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
}
if($defaults[service_type] == 3){
	$info_box_title='Group Service';  
	// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
	$core_content = "<table  width='100%'>
		<tr>
			<td width=150 class='font2'>Check String:</td>
			<td align=left >" . $btl->resolveGroupString($defaults[service_var]) . "</font></td>
			<td>&nbsp;</td>           
		</tr>
		
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
}

//special_ui's
if(preg_match("/^XML:(.*):(.*)$/i", $_GET[service_place], $match)) {
	$special_counter=$btl->XMLRemoteConfig($match[1], "ui-extra.conf", "special_addon_ui_" . $defaults[service_id_real] . "_cnt");	
} else {
	$special_counter=bartlby_config("ui-extra.conf", "special_addon_ui_" . $defaults[service_id] . "_cnt");
	
}
if($special_counter) {
	
	for($spc=1; $spc<=$special_counter; $spc++) {
		
		if(preg_match("/^XML:(.*):(.*)$/i", $_GET[service_place], $match)) {
			$spc_name=$btl->XMLRemoteConfig($match[1], "ui-extra.conf", "special_addon_ui_" . $defaults[service_id_real] . "_[" . $spc ."]_name");
		} else {
			$spc_name=bartlby_config("ui-extra.conf", "special_addon_ui_" . $defaults[service_id] . "_[" . $spc ."]_name");
		}
		//$layout->OUT .= "menu" . $defaults[service_id] . "[" . $spc_real . "]='" . str_replace("^", "=", bartlby_config($btl->CFG, "special_addon_ui_" . $defaults[service_id] . "_[" . $spc ."]")) . "';\n";
		$info_box_title="$spc_name";  
		// (<i>Logged in as:</i><font color="#000000"><b>' . $btl->user . '</b></font>) Uptime: <font color="#000000">' . $btl->intervall(time()-$btl->info[startup_time]) . '</font>'
		
		if(preg_match("/^XML:(.*):(.*)$/i", $_GET[service_place], $match)) {
			
			$special_value=$btl->XMLRemoteConfig($match[1], 'ui-extra.conf', "special_addon_ui_" . $defaults[service_id_real] . "_[" . $spc ."]");
		} else {
			$special_value=bartlby_config('ui-extra.conf', "special_addon_ui_" . $defaults[service_id] . "_[" . $spc ."]");
		}
		
		$core_content = "<table  width='100%'>
			<tr>
				<td>" . str_replace("^", "=", $special_value) . "</td>           
			</tr>
			
			
		</table>";
		
		$layout->push_outside($layout->create_box($info_box_title, $core_content));
	}
	
}

$r=$btl->getExtensionsReturn("_serviceDetail", $layout);

$layout->OUT .= $btl->getserviceOptions($defaults, $layout);
$layout->TableEnd();

$layout->display();