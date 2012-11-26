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
	$layout->setTitle("BnR: Restore!!");
	
	$layout->set_menu("BnR");
	$layout->Form("fm1", "extensions_wrap.php");
	$layout->Table("100%");
	
	
$optind=0;
$bstore="extensions/BnR/store/";
$dhl=opendir($bstore);

while($file = readdir($dhl)) {
	if($file == "." || $file == ".." || $file == "CVS") continue;
	
	if(is_dir($bstore . $file)) {
		
		$info=file_get_contents($bstore . $file . "/info.txt");
		
		$packages[$optind][c]="";
		$packages[$optind][v]=$file;	
		$packages[$optind][k]="&raquo;" . $file . "($info)";
		$optind++;
	}
}
closedir($dhl);

$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<b>Restore Backup</b>")
			)
		)

);	

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Backup:",
				1=>$layout->DropDown("backup", $packages)
			)
		)

);


$layout->Tr(
	$layout->Td(
			Array(
				0=>"Force plugins overwrite:",
				1=>$layout->Field("force_plugins", "checkbox", "checked") . $layout->Field("script", "hidden", "BnR/do_restore.php")
			)
		)

);
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Force perf Handlers overwrite:",
				1=>$layout->Field("force_perf", "checkbox", "checked")
			)
		)

);

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Restore config files:",
				1=>$layout->Field("force_config", "checkbox", "checked")
			)
		)

);


$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "submit", "next->") . $layout->Field("server_id", "hidden", $_GET[server_id])
					)
			)
		)

);
	$layout->FormEnd();
	
	$layout->TableEnd();
	$layout->display();