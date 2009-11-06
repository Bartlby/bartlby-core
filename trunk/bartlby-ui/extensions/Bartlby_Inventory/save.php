<?php
	chdir("../../");
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/Bartlby_Inventory/Bartlby_Inventory.class.php";
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$inv = new Bartlby_Inventory();
	
	$inv->storeServer($_GET[id], $_GET[serial], $_GET[Warranty], $_GET[add_info]);
	
?>