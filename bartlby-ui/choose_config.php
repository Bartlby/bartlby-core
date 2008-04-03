<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("core.edit_cfg");
$layout= new Layout();
$layout->set_menu("core");
$layout->setTitle("Choose Config");
$layout->Form("fm1", "edit_config.php","GET");
$layout->Table("100%");

$optind=0;
$packages[$optind][c]="";
$packages[$optind][v]="bartlby.cfg";	
$packages[$optind][k]="Core";
$optind++;
$packages[$optind][c]="";
$packages[$optind][v]="ui-extra.conf";	
$packages[$optind][k]="User Interface";
$optind++;


$layout->Tr(
$layout->Td(
		Array(
			0=>"Config file:",
			1=>$layout->DropDown("ecfg", $packages) 
		)
	)

);

$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "submit", "next->")
					)
			)
		)

);


$layout->TableEnd();

$layout->FormEnd();
$layout->display();