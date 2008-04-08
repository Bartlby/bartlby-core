<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("core.edit_cfg");
$layout= new Layout();
$layout->set_menu("core");
$layout->setTitle("Edit Config");
$layout->Form("fm1", "bartlby_action.php","POST");
$layout->Table("100%");

switch($_GET[ecfg]) {
	case 'bartlby.cfg':
		$cfgfile=$btl->CFG;
	break;	
	case 'ui-extra.conf':
		$cfgfile="ui-extra.conf";
	break;
}
$ecfg=$_GET[ecfg];
$cur_conf=implode(file($cfgfile), "");

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Server:",
				1=>$layout->TextArea("cfg_file", $cur_conf, 35, 80) . $layout->Field("action", "hidden", "edit_cfg") . $layout->Field("ecfg", "hidden", $ecfg)
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