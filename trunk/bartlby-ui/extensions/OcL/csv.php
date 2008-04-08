<?
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
	$btl->hasRight("ocl_csv");
	$ocl = new OcL();
	Header("Content-Type: text/plain");
	
	
	
	
	$edate=time();
	if($_GET[edate]) $edate=$_GET[edate];
	
	
	$nday=$edate+(86400*30);
	$pday=$edate-(86400*30);
	
	
		
	//get all entrys via identifier
	$identifier = date("m.Y",$edate);
	$v=unserialize($ocl->storage->load_key($identifier));
	$v=@array_reverse($v);
	for($x=0; $x<count($v); $x++) {
		echo $v[$x][ocl_date] . ";" . $v[$x][ocl_type] . ";" . $v[$x][ocl_duration] . ";" . $v[$x][ocl_caller] . ";" . $v[$x][ocl_poster] . ";" . $v[$x][ocl_subject] . ";" . str_replace("\n", " ", str_replace("\r\n", " ", $v[$x][ocl_error_long])) . ";\n";
				
	}
	
	
	
	
?>