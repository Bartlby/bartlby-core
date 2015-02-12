<?

//var_dump(ini_get("force_audit"));

echo "migrating old triggers....";
	$trigger_dummy = array(
		"trigger_name" => "trigger_name",
		"trigger_enabled" => 1,
		"trigger_type" => 1,
		"trigger_data" => "mail.sh",
		"orch_id" => 0,
		"trigger_execplan" => ""
	);

$r = bartlby_new("/opt/bartlby/etc/bartlby.cfg");
$dir = opendir("/opt/bartlby/trigger/");
while($file = readdir($dir)) {
	if($file == "." || $file == "..") continue;
	//Add Trigger :D
	$trigger_dummy["trigger_name"]=$file;
	$trigger_dummy["trigger_data"]=$file;
	$new_id=bartlby_add_trigger($r, $trigger_dummy);
	
	$triggers_old[]="|" . $file . "|";
	$triggers_new[]="|" . $new_id . "|";

	echo "Created new Trigger with ID: " . $new_id . " going to replace ->" . $file . " with the new one " . $new_id . "\n";
	array("new_id"=>$new_id, "old_name"=>$file);
}

closedir($dir);


	$i=bartlby_get_info($r);
	for($y=0; $y<$i["services"]; $y++) {
		 $obj = bartlby_get_service($r, $y);
		 $obj["enabled_triggers"] = str_replace($triggers_old,  $triggers_new , $obj["enabled_triggers"]);
		 bartlby_modify_service($r, $obj["service_id"], $obj);
		 

	}
	for($y=0; $y<$i["server"]; $y++) {
		 $obj = bartlby_get_server($r, $y);
		 $obj["enabled_triggers"] = str_replace($triggers_old,  $triggers_new , $obj["enabled_triggers"]);
		 bartlby_modify_server($r, $obj["server_id"], $obj);
	}

	for($y=0; $y<$i["workers"]; $y++) {
		 $obj = bartlby_get_worker($r, $y);
		 $obj["enabled_triggers"] = str_replace($triggers_old,  $triggers_new , $obj["enabled_triggers"]);
		 bartlby_modify_worker($r, $obj["worker_id"], $obj);
	}
	for($y=0; $y<$i["servergroups"]; $y++) {
		 $obj = bartlby_get_servergroup($r, $y);
		 $obj["enabled_triggers"] = str_replace($triggers_old,  $triggers_new , $obj["enabled_triggers"]);
		 bartlby_modify_servergroup($r, $obj["servergroup_id"], $obj);
	}
	for($y=0; $y<$i["servicegroups"]; $y++) {
		 $obj = bartlby_get_servicegroup($r, $y);
		 $obj["enabled_triggers"] = str_replace($triggers_old,  $triggers_new , $obj["enabled_triggers"]);
		 bartlby_modify_servicegroup($r, $obj["servicegroup_id"], $obj);
	}	

	function bartlby_generic_audit($res, $type,  $id, $line) {
		return true;
	}
	function bartlby_object_audit($res, $type, $id, $action) {
		return true;
	}
?>