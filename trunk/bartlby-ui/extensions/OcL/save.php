<?php
/*
storage layout

array 
	0 => array(element_info)
	2 => array(element_info)


*/
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/OcL/OcL.class.php";
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("ocl_add");
	$ocl = new OcL();
	
	
if (!$_GET[ocl_id]) {
	//add new
	$t_a=explode(".", $_GET[ocl_date]);
	$t_b=explode(" ", $t_a[2]);
	
	
	$identifier=$t_a[1] . "." . $t_b[0];
	
		
	
	$cur_entry[ocl_date]		=	$_GET[ocl_date];
	$cur_entry[ocl_subject]		=	$_GET[ocl_subject];
	$cur_entry[ocl_duration]	=	$_GET[ocl_duration];
	$cur_entry[ocl_type]		=	$_GET[ocl_type];
	$cur_entry[ocl_caller]		=	$_GET[ocl_caller];
	$cur_entry[ocl_error_long]	=	$_GET[ocl_error_long];
	$cur_entry[ocl_poster]		=	$btl->user;
	$cur_entry[ocl_service_var]	= 	$_GET[service_var];
	$cur_entry[ocl_id]		= 	uniqid(rand(), true);

	
	$value=$ocl->storage->load_key($identifier);
	$save_array=unserialize($value);
	if($save_array==false || !is_array($save_array)) {
		//complete new month	
		$save_array[0]=$cur_entry;
		$ocl->storage->save_key($identifier, serialize($save_array));
		
	} else {
		//push at the end
		
		@array_push($save_array, $cur_entry);
		$ocl->storage->save_key($identifier, serialize($save_array));
		
	}
	
	
	
	$layout= new Layout();
	$layout->setTitle("OcL: Add entry");
} else {
	$layout= new Layout();
	$layout->setTitle("OcL: Edit entry");

	$v=unserialize($ocl->storage->load_key($_GET[identifier]));
		
		
	$new_x=0;
	for($x=0; $x<count($v); $x++) {
		if($v[$x][ocl_id] == $_GET[ocl_id]) {
			$v[$x][ocl_date]		=	$_GET[ocl_date];
			$v[$x][ocl_subject]		=	$_GET[ocl_subject];
			$v[$x][ocl_duration]	=	$_GET[ocl_duration];
			$v[$x][ocl_type]		=	$_GET[ocl_type];
			$v[$x][ocl_caller]		=	$_GET[ocl_caller];
			$v[$x][ocl_error_long]	=	$_GET[ocl_error_long];
			$v[$x][ocl_poster]		=	$btl->user;
			$v[$x][ocl_service_var]	= 	$_GET[service_var];
			$v[$x][ocl_id]		= 	$v[$x][ocl_id];
			break;
			
		}
	}
	$ocl->storage->save_key($_GET[identifier], serialize($v));	
}
	$layout->set_menu("OcL");
	
	$layout->Table("100%");
	

	$layout->Tr(
		$layout->Td(
				Array(
					array("colspan" => 2, "show" => "<b>Done!</b>")
				)
			)
	
	);	
	
	$layout->TableEnd();
	$layout->display();
	
	
	
?>