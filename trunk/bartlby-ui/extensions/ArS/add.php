<?
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/ArS/ArS.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("super_user");
	$sg = new ArS();
	$servers=$btl->GetSVCMap();
	
	$layout= new Layout();
	$layout->setTitle("ArS: Add Report");
	
	$layout->set_menu("ArS");
	$layout->Form("fm1", "extensions_wrap.php");
	$layout->Table("100%");
	
	$defaults=bartlby_get_service_by_id($btl->CFG, $_GET[service_id]);

$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<b>Add a automated report</b>")
			)
		)

);	
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Service:" . $layout->Field("ars_service_id", "hidden", $_GET[service_id]) . $layout->Field("action", "hidden", "add"),
				1=>$defaults[server_name] . "/" . $defaults[service_name]
			)
		)

);	
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Recipient:",
				1=>$layout->Field("ars_to", "textbox", "")
			)
		)

);	
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Daily:" .$layout->Field("script", "hidden", "ArS/action.php"),
				1=>$layout->Field("ars_daily", "checkbox", "checked")
			)
		)

);
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Weekly:",
				1=>$layout->Field("ars_weekly", "checkbox", "checked")
			)
		)

);



$layout->Tr(
	$layout->Td(
			Array(
				0=>"Monthly:",
				1=>$layout->Field("ars_monthly", "checkbox", "checked")
			)
		)

);
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Include Performance Charts:",
				1=>$layout->Field("ars_perf", "checkbox", "checked")
			)
		)

);

$typos[0][s]=0;
$typos[0][k]="html";
$typos[0][v]="html";

$typos[1][s]=0;
$typos[1][k]="csv";
$typos[1][v]="csv";


$typos[2][s]=0;
$typos[2][k]="xls";
$typos[2][v]="xls";


$layout->Tr(
	$layout->Td(
			Array(
				0=>"Format:",
				1=>$layout->DropDown("ars_type", $typos)
			)
		)

);


$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "button", "next->", "" ," onClick='xajax_ExtensionAjax(\"ArS\", \"xajax_ars_add_form\", xajax.getFormValues(\"fm1\"))'") . $layout->Field("server_id", "hidden", $_GET[server_id])
					)
			)
		)

);
	$layout->FormEnd();
	
	$layout->TableEnd();
	$layout->display();