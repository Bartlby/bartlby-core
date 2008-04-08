<?
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/OcL/OcL.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("ocl_add");
	$sg = new OcL();
	
	
	$layout= new Layout();
	$layout->setTitle("OcL: BsP");
	$layout->set_menu("OcL");
	
	$layout->Table("100%");
	$layout->set_menu("OcL");
	
	$servs=$btl->GetWorker();
	$x=0;
	while(list($k, $v) = @each($servs)) {
		$l[$v[worker_id]]=$v[shm_place];		
		$lo[$v[worker_id]]=$v;
	}
	
	//$k = date("j-n-Y") . "-active";
	
	$k1=$_GET[day] . "-active";
	$k2=$_GET[day] . "-standby";
	$stored1=unserialize($sg->storage->load_key($k1));
	$stored2=unserialize($sg->storage->load_key($k2));
	
	
	$current_hour=$_GET[hour];
	
	for($x=0; $x<count($stored2[$current_hour]); $x++) {
		$shm_id=$l[$stored2[$current_hour][$x]];
		bartlby_set_worker_state($btl->CFG, $shm_id, 2);
		$output .= "set on standby: " . $lo[$stored2[$current_hour][$x]][name] . "<br>\n";
	}
	for($x=0; $x<count($stored1[$current_hour]); $x++) {
		$shm_id=$l[$stored1[$current_hour][$x]];
		bartlby_set_worker_state($btl->CFG, $shm_id, 1);
		$output .= "set on active: " . $lo[$stored1[$current_hour][$x]][name] . "<br>\n";
	}
	
	$output .= "Performing BsP Task for $k1 " . $current_hour;
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>"Output:",
				1=>$output
			)
		)

	);
	
	$layout->TableEnd();
	$layout->FormEnd();
	$layout->display();
	
	