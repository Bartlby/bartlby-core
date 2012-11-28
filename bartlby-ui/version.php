<?php
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	
	$servers=$btl->GetSVCMap();
	
	$layout= new Layout();
	$layout->setTitle("Version");
	
	$layout->set_menu("main");
	$layout->Table("100%");
	
	
	
	
	$layout->Tr($layout->Td(array(
			0=>"Core:",
			1=>"<b>" . $btl->info[version] . "</b>"
			
			)
		)
	);
	$layout->Tr($layout->Td(array(
			0=>"PHP:",
			1=>"<b>" . bartlby_version() . "</b>"
			
			)
		)
	);
	
	$layout->Tr($layout->Td(array(
			0=>"UI:",
			1=>"<b>" . BARTLBY_UI_VERSION . "</b>"
			
			)
		)
	);
	
	
	
	$layout->TableEnd();
	$layout->display();	



