<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->set_menu("worker");
$layout->setTitle("");
$defaults=@bartlby_get_worker_by_id($btl->CFG, $_GET[worker_id]);

$fm_action="save_permissions";
$btl->hasRight("super_user");

if($defaults == false) {
	$btl->redirectError("BARTLBY::OBJECT::MISSING");
	exit(1);	
}

$map = $btl->GetSVCMap();
$worker_rights = $btl->loadForeignRights($defaults[worker_id]);
$optind=0;

while(list($k, $servs) = @each($map)) {

	for($x=0; $x<count($servs); $x++) {
		//$v1=bartlby_get_service_by_id($btl->CFG, $servs[$x][service_id]);
		
		if($x == 0 ) {
			//$isup=$btl->isServerUp($v1[server_id]);
			//if($isup == 1 ) { $isup="UP"; } else { $isup="DOWN"; }
			$servers[$optind][c]="";
			$servers[$optind][v]="s" . $servs[$x][server_id];	
			$servers[$optind][k]="" . $servs[$x][server_name] . "";
			$servers[$optind][is_group]=1;
			
			if(@in_array($servs[$x][server_id], $worker_rights[servers])) {
				$servers[$optind][s]=1;	
			}
			$optind++;
		} else {
			
		}
		$state=$btl->getState($servs[$x][current_state]);
		$servers[$optind][c]="";
		$servers[$optind][v]=$servs[$x][service_id];	
		$servers[$optind][k]="" .  $servs[$x][service_name];
		if(@in_array($servs[$x][service_id], $worker_rights[services])) {
			$servers[$optind][s]=1;	
		}
		
		
		
		$optind++;
	}
}
$ov = "<form name='fm1' action='bartlby_action.php' method=POST>";
$layout->Table("100%");




$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Name",
			1=>$defaults[name]. $layout->Field("action", "hidden", $fm_action)
		)
	)
, true);

$ov .= $layout->Tr(
	$layout->Td(
		array(
			0=>"Visible services:",
			1=>$layout->DropDown("worker_services[]", $servers, "multiple")
		)
	)
,true);





$all_keys=$btl->loadForeignRights("template");
while(list($k,$v) = each($all_keys)) {
	if($k == "servers" || $k == "services" || $k == "selected_servers" || $k == "selected_services") continue;
	if($worker_rights[$k][0] && $worker_rights[$k][0] != "false") {
		$checked="checked";
	} else {
		$checked="";
	}
	
	$ov .= $layout->Tr(
		$layout->Td(
			array(
				0=>translatedKey($k),
				1=>"<input type=checkbox name='" . $k . "' $checked>"
			)
		)
	,true);	
			
	}



	




function translatedKey($k) {
	$ar["super_user"]="Super User (has full rights, can also give rights)";
	$ar["action.reload"]="allowed to reload data";
	$ar["core.edit_cfg"]="can edit config files";
	$ar["core.process_info"]="can view statistics";
	$ar["core.event_queue"]="view event queue";
	$ar["action.disable_notify"]="allowed to disable notifications";
	$ar["action.enable_service"]="allowed to enable notifications";
	$ar["action.add_comment"]="Add comments (also required for service ack)";
	$ar["action.force_check"]="can force checks";
	$ar["action.perfhandler_graph"]="perfhandler update";
	$ar["action.disable_extension"]="can disable ui-extensions";
	$ar["action.enable_extension"]="can enable ui-extensions";
	$ar["action.sirene_enable"]="can fire the sirene";
	$ar["action.sirene_disable"]="can disarm the sirene";
	$ar["modify_all_workers"]="allowed to see all workers";
	$ar["main.server_detail"]="server details";
	$ar["main.service_detail"]="can view the service list";
	$ar["main.overview"]="can view the overview";
	$ar["action.stop"]="can shutdown bartlby";
	$ar["log.report"]="can do reports?";
	$ar["log.logview"]="logfile visible?";
	$ar["action.add_server"]="add servers";
	$ar["action.delete_server"]=" delete servers";
	$ar["action.modify_server"]="modify servers";
	$ar["action.copy_server"]="copy servers";
	$ar["action.install_package"]="is allowed to install packages";
	$ar["action.uninstall_package"]="is allowed to uninstall packages";
	$ar["action.delete_package"]="delete packages";
	$ar["action.create_package"]="create packages";
	
	$ar["action.add_worker"]="add worker";
	$ar["action.delete_worker"]=" delete worker";
	$ar["action.modify_worker"]="modify worker";
	$ar["action.copy_worker"]="copy worker";
	
	$ar["action.add_service"]="add service";
	$ar["action.delete_service"]=" delete service";
	$ar["action.modify_service"]="modify service";
	$ar["action.copy_service"]="copy service";
	
	$ar["action.add_downtime"]="add downtime";
	$ar["action.delete_downtime"]=" delete downtime";
	$ar["action.modify_downtime"]="modify downtime";
	$ar["main.services"]="can view service list";
	$ar["sysmessages"]="system messages visible? in logview.";
	$ar["view_service_output"]="service output visible?";
	
	if(!$ar[$k]) {
		return "Key ('$k') not translated";	
	} else {
		return $ar[$k];
	}
	
}
$layout->TableEnd();
	
	
	$title="UI privileges";  
	$ov .= $layout->Field("worker_id", "hidden", $_GET[worker_id]) . "<input type=submit value='Save'>";
	$ov .= "</form>";
	$content = "<table>" . $ov . "</table>";
	$layout->push_outside($layout->create_box($title, $content));
	
	$r=$btl->getExtensionsReturn("_permissions", $layout);

	
	$title="";  

	//$layout->push_outside($layout->create_box($title, $content));




$layout->display();
