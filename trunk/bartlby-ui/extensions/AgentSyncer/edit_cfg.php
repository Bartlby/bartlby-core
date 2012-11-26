<?
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
$cur_conf=implode(file($ecfg), "");

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Current:",
				1=>$layout->TextArea("cfg_file", $cur_conf, 35, 80) . $layout->Field("script", "hidden", "AgentSyncer/save_cfg.php")
			)
		)

);

$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "submit", "next->")
					)
			)
		)

);


$layout->TableEnd();

$layout->FormEnd();
$layout->display();