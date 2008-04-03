<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";




$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->setTitle("");

function dnl($i) {
	return sprintf("%02d", $i);
}
if($_GET[service_id]{0} == 's') {
	$layout->Form("fm1", "bartlby_action.php");
	$layout->Table("100%");

	
	$layout->Tr(
		$layout->Td(
			array(
				0=>'you have choosen a server',
				1=>'<input type=button value=back onClick="javascript:history.back();">'
				
			)
		)
	);
	
	$layout->TableEnd();
	$layout->FormEnd();
	$layout->display();
	exit(1);
	
	
} 
if($_GET[service_id]) {
	$btl->hasServerorServiceRight($_GET[service_id]);
}
$defaults=@bartlby_get_service_by_id($btl->CFG, $_GET[service_id]);

$fm_action="modify_service";
if($_GET["copy"] == "true") {
	$fm_action="add_service";
	$btl->hasRight("action.copy_service");
}
if($_GET["new"] == "true") {
	$fm_action="add_service";
	$btl->hasRight("action.add_service");
	
	$defaults["exec_plan"] = "0=00:00-23:59|1=00:00-23:59|2=00:00-23:59|3=00:00-23:59|4=00:00-23:59|5=00:00-23:59|6=00:00-23:59";
	$defaults["check_interval"]=bartlby_config("ui-extra.conf", "new.service.interval");
	$defaults[service_type]=(int)bartlby_config("ui-extra.conf", "new.service.type");
	$defaults[service_ack]=(int)bartlby_config("ui-extra.conf", "new.service.ack");
	$defaults[service_retain]=(int)bartlby_config("ui-extra.conf", "new.service.retain");
	
	$defaults[service_check_timeout]=(int)bartlby_config("ui-extra.conf", "new.service.active.tcptimeout");
	$defaults[plugin]=bartlby_config("ui-extra.conf", "new.service.active.plugin");
	$defaults[service_args]=bartlby_config("ui-extra.conf", "new.service.active.arguments");
	
	$defaults[service_passive_timeout]=(int)bartlby_config("ui-extra.conf", "new.service.passive.timeout");
	$defaults[flap_seconds]=(int)bartlby_config("ui-extra.conf", "new.service.flap_seconds");
	
	$defaults[escalate_divisor]=(int)bartlby_config("ui-extra.conf", "new.service.escalate_divisor");
	$defaults[renotify_interval]=(int)bartlby_config("ui-extra.conf", "new.service.renotify_interval");
	
	
}
if($fm_action == "modify_service") {
	$btl->hasRight("action.modify_service");
}


if($defaults == false && $_GET["new"] != "true") {
	$btl->redirectError("BARTLBY::OBJECT::MISSING");
	exit(1);	
}





//ACKS

//Ack's
$ack[0][c]="";
$ack[0][v] = 0; //No
$ack[0][k] = "No"; //No
$ack[0][s]=0;

$ack[1][c]="";
$ack[1][v] = 1; //No
$ack[1][k] = "Yes"; //No
$ack[1][s]=0;


//Notify Enabled
$notenabled[0][c]="";
$notenabled[0][v] = 0; //No
$notenabled[0][k] = "No"; //No
$notenabled[0][s]=0;

$notenabled[1][c]="";
$notenabled[1][v] = 1; //No
$notenabled[1][k] = "Yes"; //No
$notenabled[1][s]=0;

if(is_int($defaults[notify_enabled]) && $defaults[notify_enabled] == 0) {
	$notenabled[0][s]=1;	
	
} else {
	
	$notenabled[1][s]=1;
}

//Notify Enabled
$servactive[0][c]="";
$servactive[0][v] = 0; //No
$servactive[0][k] = "No"; //No
$servactive[0][s]=0;

$servactive[1][c]="";
$servactive[1][v] = 1; //No
$servactive[1][k] = "Yes"; //No
$servactive[1][s]=0;


if(is_int($defaults[service_active]) && $defaults[service_active] == 0) {
	$servactive[0][s]=1;	
	
} else {

	$servactive[1][s]=1;
}




if(is_int($defaults[service_ack]) && $defaults[service_ack] == 0) {
	$ack[0][s]=1;	
} else {

	$ack[1][s]=1;
}




$types[0][c]="";
$types[0][v]="1";
$types[0][k]="Active";

if($defaults[service_type] == 1) {
	$types[0][s]=1;
}

$types[1][c]="";
$types[1][v]="2";
$types[1][k]="Passive";
if($defaults[service_type] == 2) {
	$types[1][s]=1;
}

$types[2][c]="";
$types[2][v]="3";
$types[2][k]="Group";
if($defaults[service_type] == 3) {
	$types[2][s]=1;
}

$types[3][c]="";
$types[3][v]="4";
$types[3][k]="Local";
if($defaults[service_type] == 4) {
	$types[3][s]=1;
}
$types[4][c]="";
$types[4][v]="5";
$types[4][k]="SNMP";
if($defaults[service_type] == 5) {
	$types[4][s]=1;
}

$types[5][c]="";
$types[5][v]="6";
$types[5][k]="NRPE";
if($defaults[service_type] == 6) {
	$types[5][s]=1;
}

$types[6][c]="";
$types[6][v]="7";
$types[6][k]="NRPE(ssl)";
if($defaults[service_type] == 7) {
	$types[6][s]=1;
}

$types[8][c]="";
$types[8][v]="9";
$types[8][k]="AgentV2(no-SSL)";
if($defaults[service_type] == 9) {
	$types[8][s]=1;
}

$types[7][c]="";
$types[7][v]="8";
$types[7][k]="AgentV2";
if($defaults[service_type] == 8) {
	$types[7][s]=1;
}

if(!$defaults[service_type]) {
	$types[0][s]=1;	
}
//Get plugins :))
$layout->set_menu("services");

$servs=$btl->GetServers();
$optind=0;
while(list($k, $v) = each($servs)) {
	//$sr=bartlby_get_server_by_id($btl->CFG, $k);
	
	$servers[$optind][c]="";
	$servers[$optind][v]=$k;	
	$servers[$optind][k]=$v;
	if($defaults[server_id] == $k) {
		$servers[$optind][s]=1;	
	}
	$optind++;
}

$layout->OUT .= "<script>

		function modify_service_make_24() {
			for(x=0; x<=6; x++) {
				e = document.getElementById('wdays_plan[' + x + ']');
				e.value='00:00-23:59';
			}
			
		}
	
		function testPlg() {
		plugin=document.fm1.service_plugin.value;
		server=document.fm1.service_server.options[document.fm1.service_server.selectedIndex].value;
		plg_args=document.fm1.service_args.value;
		window.open('check.php?server=' + server +  '&plugin=' + plugin + '&args=' + plg_args, 'chk','width=600, height=600, scrollbars=yes'); 
		}
		function showPlgHelp() {
			plugin=document.fm1.service_plugin.value;
			window.open('execv.php?cmd='+plugin+' -h', 'plgwnd', 'width=600, height=600');
		}
		function GrpChk() {
			window.open('grpstr.php?str='+document.fm1.service_var.value, 'grp', 'width=600, height=600, scrollbars=yes');
		}
		function CheckTables() {
			va=document.fm1.service_type.options[document.fm1.service_type.selectedIndex].value;
			GenericToggleFix(\"active\", \"none\");
			GenericToggleFix(\"passive\", \"none\");
			GenericToggleFix(\"group\", \"none\");
			GenericToggleFix(\"snmp\", \"none\");
			
			if(va == 2) {
				GenericToggleFix(\"passive\", \"block\");
				GenericToggleFix(\"active\", \"block\");
			}
			if(va == 1 || va == 6 || va == 7 || va == 8 ||  va == 9) {
				GenericToggleFix(\"active\", \"block\");
			}
			if(va == 3) {
				GenericToggleFix(\"group\", \"block\");	
			}
			if(va == 4) {
				GenericToggleFix(\"active\", \"block\");	
			}
			if(va == 5) {
				GenericToggleFix(\"snmp\", \"block\");	
			}
			
		}
		CheckTables();
		</script>
";
echo "<form id='fm1' name='fm1' action='bartlby_action.php' method=POST>\n";
$layout->Table("100%");




$active_box_out = "<table>";

$active_box_out .=$layout->Tr(
	$layout->Td(
		array(
			0=>"Service Type",
			1=>$layout->DropDown("service_type", $types,"onChange=\"CheckTables()\"") 
		)
	)
, true);

$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Service Name",
			1=>$layout->Field("service_name", "text", $defaults[service_name]) . $layout->Field("action", "hidden", $fm_action)
		)
	)
,true);
$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Service Server",
			1=>$layout->DropDown("service_server", $servers)
			
		)
	)
,true);



$o = explode("|", $defaults[exec_plan]);


for($x=0; $x<count($o); $x++) {
	$p = explode("=", $o[$x]);
	
	if(count($p) == 1) {
		$p = explode("!", $o[$x]);
		$filled[$p[0]][disabled]=1;
		
		
	}
	
	
	$filled[$p[0]][value]=$p[1];
	
	
}
$plan_box = "<table>";
for($x=0; $x<=6; $x++) {
	$chk="";
	
	
	$inv_check="";
	if($filled[$x][disabled] == 1) {
		$inv_check="checked";
	}	
		
	$plan_box .= "<tr><td><font size=1>" .  $wdays[$x] . "</font></td><td><input type=text id='wdays_plan[" . $x . "]'  name='wdays_plan[" . $x . "]' value='" . $filled[$x][value] . "' style='font-size:10px; width:200px; height:20px'><input type=checkbox id='wdays_inv[" . $x . "]'  name='wdays_inv[" . $x . "]' $inv_check> invert</td></tr>";
	
}
$plan_box .= "<tr><td colspan=2><font size=1>Time ranges are seperated with ',' e.g.: 14:30-15:20,01:20-02:30 <a href='javascript:void(0);' onClick='modify_service_make_24();'>make 24h a day</a></font></td></tr>";
$plan_box .= "</table>";



$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Service Check Plan:",
			1=>$plan_box
			
		)
	)
, true);



$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Service Check intervall",
			1=>$layout->Field("service_interval", "text", $defaults[check_interval]) . " Seconds"
			
		)
	)
,true);

$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Service Enabled?",
			1=>$layout->DropDown("service_active", $servactive)
			
		)
	)
,true);

$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Notification enabled",
			1=>$layout->DropDown("notify_enabled", $notenabled)
			
		)
	)
,true);



$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Service Acknowledgement",
			1=>$layout->DropDown("service_ack", $ack)
			
		)
	)
,true);



$active_box_out .=$layout->Tr(
	$layout->Td(
		array(
			0=>"Service retain in status",
			1=>$layout->Field("service_retain", "text", $defaults[service_retain]) . " Times"
			
		)
	)
,true);
$active_box_out .=$layout->Tr(
	$layout->Td(
		array(
			0=>"Service flap time threshold",
			1=>$layout->Field("flap_seconds", "text", $defaults[flap_seconds]) . " seconds"
			
		)
	)
,true);


$active_box_out .=$layout->Tr(
	$layout->Td(
		array(
			0=>"Service Re-Notification",
			1=>$layout->Field("renotify_interval", "text", $defaults[renotify_interval]) . " runs"
			
		)
	)
,true);

$active_box_out .=$layout->Tr(
	$layout->Td(
		array(
			0=>"Service escalate",
			1=>$layout->Field("escalate_divisor", "text", $defaults[escalate_divisor]) . " runs"
			
		)
	)
,true);

$active_box_out .= "</table>";
$layout->push_outside($layout->create_box("Basic Settings", $active_box_out, "basic"));


$active_box_out = "<table>";

$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Service Timeout(either TCP or Local)",
			1=>$layout->Field("service_check_timeout", "text", $defaults[service_check_timeout])
			
		)
	)
,true);

$active_box_out .=$layout->Tr(
	$layout->Td(
		array(
			0=>"Service Plugin",
			1=>"<input type='text' value='" . $defaults[plugin] . "' id='service_plugin' onClick=\"buffer_suggest.makeRequest('service_plugin', 'xajax_PluginSearch');\" onkeyup=\"buffer_suggest.modified('service_plugin', 'xajax_PluginSearch');\"  name='service_plugin'  autocomplete='off' /> <div style='background-color:#ffffff; position:absolute' id='plugin_search_suggest'></div><a href='javascript:showPlgHelp();'>Show Help of Plugin</A>&nbsp;&nbsp;<a href='javascript:testPlg();'>Test It</A>"
		)
	)
,true);

 $defaults[plugin_arguments]=str_replace("\n", "\\n",  $defaults[plugin_arguments]);
 $defaults[plugin_arguments]=str_replace("\r", "\\r",  $defaults[plugin_arguments]);

$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Service Plugin Arguments",
			1=>$layout->Field("service_args", "text", $defaults[plugin_arguments])
			
		)
	)
,true);

$active_box_out .= "</table>";
$layout->push_outside($layout->create_box("Active/Local Settings", $active_box_out, "active"));


$active_box_out = "<table>";

$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Service Timeout",
			1=>$layout->Field("service_passive_timeout", "text", $defaults[service_passive_timeout])
			
		)
	)
,true);



$active_box_out .= "</table>";
$layout->push_outside($layout->create_box("Passive Settings", $active_box_out, "passive"));
$active_box_out = "<table>";


$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Group definition",
			
			1=>$layout->Field("service_var", "hidden", $defaults[service_var]) . "<a href='javascript:GrpChk();'>Open Group selector</A>"
			
		)
	)
,true);
$active_box_out .= "</table>";
$layout->push_outside($layout->create_box("Group Settings", $active_box_out, "group"));




$snmptypes[0][c]="";
$snmptypes[0][v]="1";
$snmptypes[0][k]="Lower";


if($defaults[service_snmp_type] == 1) {
	 $snmptypes[0][s]=1;
}

$snmptypes[1][c]="";
$snmptypes[1][v]="2";
$snmptypes[1][k]="Greater";

if($defaults[service_snmp_type] == 2) {
	 $snmptypes[1][s]=1;
}

$snmpversions[0][c]="";
$snmpversions[0][v]="1";
$snmpversions[0][k]="1";


if($defaults[service_snmp_version] == 1) {
	 $snmpversions[0][s]=1;
}

$snmpversions[1][c]="";
$snmpversions[1][v]="2";
$snmpversions[1][k]="2c";

if($defaults[service_snmp_version] == 2) {
	 $snmpversions[1][s]=1;
}

$active_box_out = "<table >";

$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Community",
			1=>$layout->Field("service_snmp_community", "text", $defaults[service_snmp_community])
			
		)
	)
,true);
$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"OBJ ID",
			1=>$layout->Field("service_snmp_objid", "text", $defaults[service_snmp_objid])
			
		)
	)
,true);
$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Version",
			1=>$layout->DropDown("service_snmp_version", $snmpversions, "") 
			
		)
	)
,true);
$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Warning",
			1=>$layout->Field("service_snmp_warning", "text", $defaults[service_snmp_warning])
			
		)
	)
,true);
$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Critical",
			1=>$layout->Field("service_snmp_critical", "text", $defaults[service_snmp_critical])
			
		)
	)
,true);
$active_box_out .= $layout->Tr(
	$layout->Td(
		array(
			0=>"MatchType",
			1=>$layout->DropDown("service_snmp_type", $snmptypes, "") 
			
		)
	)
,true);
$active_box_out .= "</table>";
$layout->push_outside($layout->create_box("SNMP Settings", $active_box_out, "snmp"));


if(!$_GET["copy"] && !$_GET["new"]) {
	$idx=$btl->findSHMPlace($_GET[service_id]);
	$ssvc=bartlby_get_service($btl->CFG, $idx);
	if($ssvc[service_active] == 1) {
		bartlby_toggle_service_active($btl->CFG, $idx, 0);
		$dounlock=$idx;
			
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"left",
					'show'=>"<input type=hidden name=unlock value='" . $dounlock . "'><font color=red>the service check has been disabled until you hit save, if you don't do this the service remains inactive</font>"
					)
			)
		)

	);
	}
}

$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "button", "next->", "", " onClick='xajax_AddModifyService(xajax.getFormValues(\"fm1\"))'") . $layout->Field("service_id", "hidden", $_GET[service_id])
					)
			)
		)

);


	
$r=$btl->getExtensionsReturn("_PRE_" . $fm_action, $layout);

$layout->TableEnd();
$layout->FormEnd();
$layout->display();