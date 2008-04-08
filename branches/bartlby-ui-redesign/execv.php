<?
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	$btl=new BartlbyUi($Bartlby_CONF);
	$info=$btl->getInfo();
	$layout= new Layout();
	$layout->setTemplate("nonav.html");
	$layout->MetaRefresh(10);
	$layout->Table("100%");
	$lib=bartlby_lib_info($btl->CFG);
	$exi=-1;
	$plugin_dir=bartlby_config($btl->CFG, "agent_plugin_dir");
	if(!$plugin_dir) {
		$cmd_out="agent_plugin_dir not set";	
	} else {
		if(strstr($_GET[cmd], "/") || strstr($_GET[cmd], "..")) {
			$cmd_out=".. or / detected skipped cmd";	
		} else {
			$fp=popen($plugin_dir . "/" . $_GET[cmd],"r");
			while(!feof($fp)) {
				$cmd_out .= nl2br(fgets($fp, 1024));	
			}			
			$exi=(fclose($fp)>>8)&0xFF;
		}
	}
	
	
	$layout->Tr(
		$layout->Td(
				Array(
					0=>Array(
						'colspan'=> 1,
						'class'=>'header',
						'show'=>"Executing ($_GET[cmd])"
						)
				)
			)

	);
	$layout->Tr(
	$layout->Td(
			Array(
				0=>$cmd_out . "<br>Exited with: $exi"
				
			)
		)

	);
	$layout->TableEnd();
	$layout->display("no");
	
?>
