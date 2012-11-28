<?

	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/ServerGroups/ServerGroups.class.php";
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$inv = new ServerGroups();
	$btl->hasRight("sg_add");
	$inv->save($_GET);
	
	
	$layout= new Layout();
	$layout->set_menu("Server Groups");
	$layout->Table("100%");
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>"group $_GET[grpname] saved"
			)
		)

	);
	
	
	$layout->TableEnd();
	$layout->display();
	
?>