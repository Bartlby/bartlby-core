<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("agentsyncer_editcfg");
$layout= new Layout();
$layout->set_menu("Agent Sync");
$layout->setTitle("Edit agent default Config");
$layout->Form("fm1", "extensions_wrap.php","GET");
$layout->Table("100%");


$ecfg="extensions/AgentSyncer/bartlby.cfg.default";
$cur_text=$_GET[cfg_file];

$fp = @fopen($ecfg, "w");
if($fp) {
	@fwrite($fp, $cur_text);
	@fclose($fp);
	$save_status="<font color=green>OK</font>";	
} else {
	$save_status="<font color=red>failed</font>";	
}

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Saved:",
				1=>$save_status
			)
		)

);


$layout->TableEnd();

$layout->FormEnd();
$layout->display();