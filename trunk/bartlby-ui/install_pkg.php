<?php
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasright("action.install_package");
$layout= new Layout();

$layout->setTitle("Select a package");
$layout->Form("fm1", "bartlby_action.php");
$layout->Table("100%");
$layout->set_menu("packages");

$optind=0;
//$res=mysql_query("select srv.server_id, srv.server_name from servers srv, rights r where r.right_value=srv.server_id and r.right_key='server' and r.right_user_id=" . $poseidon->user_id);
$dhl=opendir("pkgs");

while($file = readdir($dhl)) {
	//$sr=bartlby_get_server_by_id($btl->CFG, $k);
	
	//$isup=$btl->isServerUp($k);
	//if($isup == 1 ) { $isup="UP"; } else { $isup="DOWN"; }
	if(!is_dir("pkgs/" . $file)) {
		$packages[$optind][c]="";
		$packages[$optind][v]=$file;	
		$packages[$optind][k]="&raquo;" . $file;
		$optind++;
	}
}
closedir($dhl);



$layout->Tr(
	$layout->Td(
			Array(
				0=>"Package:",
				1=>$layout->DropDown("package_name", $packages) . $layout->Field("action", "hidden", "install_package")
			)
		)

);


$layout->Tr(
	$layout->Td(
			Array(
				0=>"Force plugins update:",
				1=>$layout->Field("force_plugins", "checkbox", "checked")
			)
		)

);
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Force perf Handlers update:",
				1=>$layout->Field("force_perf", "checkbox", "checked")
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


$layout->TableEnd();

$layout->FormEnd();
$layout->display();