<?
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/ServerGroups/ServerGroups.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$sg = new ServerGroups();
	$servers=$btl->GetSVCMap();
	
	$layout= new Layout();
	$layout->setTitle("Group overview");
	
	$layout->set_menu("Server Groups");
	$layout->Table("100%");
	
	$r = $sg->_overview();
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>$r
			)
		)

	);
	
	$layout->TableEnd();
	$layout->display();