<?
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/BnR/BnR.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("super_user");
	$sg = new BnR();
	$servers=$btl->GetSVCMap();
	
	$layout= new Layout();
	$layout->setTitle("BnR: Backup!!");
	
	$layout->set_menu("BnR");
	$layout->Form("fm1", "extensions_wrap.php");
	$layout->Table("100%");
	

$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<b>Create backup of system</b>")
			)
		)

);	
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Comment:",
				1=>$layout->Field("package_with_comment", "textbox", "")
			)
		)

);	
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Include Plugins:" .$layout->Field("script", "hidden", "BnR/do_backup.php"),
				1=>$layout->Field("package_with_plugins", "checkbox", "checked")
			)
		)

);
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Include Perf Handlers:",
				1=>$layout->Field("package_with_perf", "checkbox", "checked")
			)
		)

);



$layout->Tr(
	$layout->Td(
			Array(
				0=>"Include config files:",
				1=>$layout->Field("package_with_config", "checkbox", "checked")
			)
		)

);

$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "button", "next->", "" ," onClick='xajax_ExtensionAjax(\"BnR\", \"xajax_backup_form\", xajax.getFormValues(\"fm1\"))'") . $layout->Field("server_id", "hidden", $_GET[server_id])
					)
			)
		)

);
	$layout->FormEnd();
	
	$layout->TableEnd();
	$layout->display();