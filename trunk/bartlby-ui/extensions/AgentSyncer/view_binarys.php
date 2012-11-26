<?
	
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
	
	


$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<b>Agent</b>")
			)
		)

);	
agentsyncer_output_bin("bartlby_agent");



$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<b>Passive</b>")
			)
		)

);	

agentsyncer_output_bin("bartlby_cmd");

$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<br><br><br><br><font color=red>for additional ARCH'es put the binary to extensions/AgentSyncer/</font>")
			)
		)

);	
	
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