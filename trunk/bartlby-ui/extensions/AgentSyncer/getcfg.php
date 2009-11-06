<?php
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	include "extensions/AgentSyncer/AgentSyncer.class.php";
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("agentsyncer_sync");
	$sg = new AgentSyncer();
	$servers=$btl->GetSVCMap();
	$plugin_root=bartlby_config($btl->CFG, "agent_plugin_dir");
	
	
	
	include "extensions/AgentSyncer/bartlby.cfg.default";	
?>