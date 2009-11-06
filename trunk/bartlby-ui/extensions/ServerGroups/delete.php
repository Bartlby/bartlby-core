<?php
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/ServerGroups/ServerGroups.class.php";
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("sg_delete");
	$inv = new ServerGroups();
	
	$inv->delete($_GET[grpname]);
	
	
		$layout= new Layout();
	$layout->set_menu("Server Groups");
	$layout->Table("100%");
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>"group $_GET[grpname] deleted"
			)
		)

	);
	
	
	$layout->TableEnd();
	$layout->display();

	
?>