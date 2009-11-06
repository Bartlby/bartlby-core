<?php
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/AgentSyncer/AgentSyncer.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("agentsyncer_editcfg");
	$sg = new AgentSyncer();
	$servers=$btl->GetSVCMap();
	
	$layout= new Layout();
	$layout->setTitle("AgentSyncer: Binarys");
	
	$layout->set_menu("Agent Sync");
	$layout->Table("100%");
	
	




while(list($k, $v) = each($servers)) {
	$dd="not yet synced";
	$fn = "extensions/AgentSyncer/store/" . md5($v[0][server_name]);
	if(file_exists($fn)) {
		$dd=date("d.m.Y H:i:s", file_get_contents($fn));
	}
	
	$layout->Tr(
		$layout->Td(
				Array(
					"<b><img src='server_icons/" . $v[0][server_icon] . "'><a href='server_detail.php?server_id=" . $v[0][server_id] . "'>" . $v[0][server_name] . "</A></b>",
					$dd
				)
			)
	
	);	
}
	
	$layout->TableEnd();
	$layout->display();
	
	
function agentsyncer_output_bin($b) {
	global $layout;
	$dd = "extensions/AgentSyncer/";
	
	$dhl=opendir($dd);
	
	
	while ( $f = readdir($dhl)) {
		if(preg_match("/^" . $b . ".*/i", $f)) {
			$layout->Tr(
			$layout->Td(
					Array(
						$f,
						md5_file($dd . "/" . $f)
					)
				)
       	
			);		
		}
	}
}
