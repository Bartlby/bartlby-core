<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("action.create_package");
$layout= new Layout();

$layout= new Layout();
$layout->Form("fm1", "bartlby_action.php");
$layout->Table("100%");
$layout->set_menu("packages");
$layout->setTitle("Package Name");

$map = $btl->GetSVCMap();
$optind=0;
while(list($k, $servs) = @each($map)) {

	for($x=0; $x<count($servs); $x++) {
		//$v1=bartlby_get_service_by_id($btl->CFG, $servs[$x][service_id]);
		
		if($x == 0) {
			//$isup=$btl->isServerUp($v1[server_id]);
			//if($isup == 1 ) { $isup="UP"; } else { $isup="DOWN"; }
			$servers[$optind][c]="";
			$servers[$optind][v]="";	
			$servers[$optind][k]="" . $servs[$x][server_name] . "";
			$servers[$optind][is_group]=1;
			$optind++;
		} else {
			
		}
		$state=$btl->getState($servs[$x][current_state]);
		$servers[$optind][c]="";
		$servers[$optind][v]=$servs[$x][service_id];	
		$servers[$optind][k]="" .  $servs[$x][service_name];
		
		
		if(strstr((string)$defaults[services],"|" . $servs[$x][service_id] . "|")) {
			$servers[$optind][s]=1;	
		}
		
		$optind++;
	}
}

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Name:",
				1=>$layout->Field("package_name", "test", "") . $layout->Field("action", "hidden", "create_package")
			)
		)

);
$layout->Tr(
	$layout->Td(
		array(
			0=>"Services:",
			1=>$layout->DropDown("services[]", $servers, "multiple")
		)
	)
);

$layout->Tr(
	$layout->Td(
			Array(
				0=>"Include Plugins:",
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
				0=>"Overwrite existing package:",
				1=>$layout->Field("package_overwrite", "checkbox", "checked")
			)
		)

);

$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "button", "next->", "" ," onClick='xajax_CreatePackage(xajax.getFormValues(\"fm1\"))'") . $layout->Field("server_id", "hidden", $_GET[server_id])
					)
			)
		)

);


$layout->TableEnd();

$layout->FormEnd();
$layout->display();