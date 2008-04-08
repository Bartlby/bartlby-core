<?
function dnl($i) {
	return sprintf("%02d", $i);
}
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);


$layout= new Layout();
$layout->set_menu("core");
$layout->setTitle("Core Extensions");
$layout->Table("100%");

$core_extensions="";
$cnt = (int)bartlby_config($btl->CFG, "extension_count");

if($cnt) {
	for($x=1; $x<=$cnt; $x++) {
		$path = bartlby_config($btl->CFG, "extension[" . $x . "]");
		$core_extensions .= $btl->wikiLink("core_extensions:" . basename($path), basename($path)) . "<br>";
	}
}

	
$layout->Tr(
		$layout->Td(
				array(0=>$core_extensions)
			)

		);		


$r=$btl->getExtensionsReturn("_About", $layout, true);
$layout->TableEnd();

$layout->display();

