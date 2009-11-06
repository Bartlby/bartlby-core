<?php
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/ArS/ArS.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("super_user");
	$sg = new ArS();
	$servers=$btl->GetSVCMap();
	
	$layout= new Layout();
	$layout->setTitle("ArS: Settings");
	
	$layout->set_menu("ArS");
	$layout->Form("fm1", "extensions_wrap.php");
	$layout->Table("100%");
	
	

$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<b>Settings</b>")
			)
		)

);

if($_GET[ars_smtp_host]) {
	$sg->storage->save_key("ars_smtp_from", $_GET[ars_smtp_from]);	
	$sg->storage->save_key("ars_smtp_host", $_GET[ars_smtp_host]);	
	$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<b><font color=red>saved!</font></b>")
			)
		)

	);
}

	
	$layout->Tr(
		$layout->Td(
				Array(
				
					0=>"Smtp Host",
					1=>$layout->Field("ars_smtp_host", "text", $sg->storage->load_key("ars_smtp_host"))
				)
			)
	
	);	
	$layout->Tr(
		$layout->Td(
				Array(
				
					0=>"MAIL FROM:",
					1=>$layout->Field("ars_smtp_from", "text", $sg->storage->load_key("ars_smtp_from"))
				)
			)
	
	);	
	

$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("script", "hidden", "ArS/settings.php") . $layout->Field("Subm", "submit", "save", "" )
					)
			)
		)

);
	$layout->FormEnd();
	
	$layout->TableEnd();
	$layout->display();