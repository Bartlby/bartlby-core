<?php
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	$btl=new BartlbyUi($Bartlby_CONF);
	$info=$btl->getInfo();
	$layout= new Layout();
	
	$layout->Table("100%");
	$lib=bartlby_lib_info($btl->CFG);
	$exi=-1;
	$trigger_dir=bartlby_config($btl->CFG, "trigger_dir");
	if(!$trigger_dir) {
		$cmd_out="Trigger_dir not set";	
	} else {
		$base_dir=bartlby_config($btl->CFG, "basedir");
		
		$trs=explode("|", $_GET[trs]);
		for($x=0; $x<count($trs); $x++) {
			if($trs[$x] == "") {
				continue;	
			}	
			if(!is_executable($trigger_dir . "/" . $trs[$x])) {
				continue;
			}
			$estr="export BARTLBY_HOME=\"$base_dir\";" .  $trigger_dir . "/" . $trs[$x] . " \"" . $_GET[mail] . "\" \"" . $_GET[icq] . "\" \"" . $_GET[user] . "\" \"bartlby simulation message\"";
			$cmd_out .= "<hr><b>" . $trs[$x] . "</b><br>";
			$p=popen($estr . "2>&1", "r");
			while(!feof($p)) {
				$cmd_out .= fgets($p, 1024);
			}
			$exi=(fclose($p)>>8)&0xFF;
			
			
		}
	}
	
	
	
	$layout->Tr(
		$layout->Td(
				Array(
					0=>Array(
						'colspan'=> 1,
						'class'=>'header',
						'show'=>"Trigger"
						)
				)
			)

	);
	$layout->Tr(
	$layout->Td(
			Array(
				0=>nl2br($cmd_out) . "<br><br><hr noshade>Exited with: $exi"
				
			)
		)

	);
	$layout->TableEnd();
	$layout->display("no");
	
?>
