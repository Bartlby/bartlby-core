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
	$btl->hasRight("ocl_view");
	$ocl = new OcL();
	
	
	
	$layout= new Layout();
	$layout->setTitle("OcL: Logbook");
	
	$layout->set_menu("OcL");
	
	$layout->Table("100%");
	
	$edate=time();
	if($_GET[edate]) $edate=$_GET[edate];
	
	
	$nday=$edate+(86400*30);
	$pday=$edate-(86400*30);
	
	
	$nlink="<a href='extensions_wrap.php?script=OcL/index.php&edate=" . $nday . "'>" . date("m.Y", $nday)  . "</A>";
	$plink="<a href='extensions_wrap.php?script=OcL/index.php&edate=" . $pday . "'>" . date("m.Y", $pday)  . "</A>";
	$csv="<a href='extensions_wrap.php?script=OcL/csv.php&edate=" . $edate . "'>CSV</A>";
	
	

	$layout->Tr(
		$layout->Td(
				Array(
					array("colspan" => 2, "show" => "<b>Entrys for:</b>  " . $plink . " &lt;&lt;" .  date("m.Y", $edate) . "&gt;&gt;" . $nlink . " $csv ") 
				)
			)
	
	);	
	
	
	//get all entrys via identifier
	$identifier = date("m.Y",$edate);
	$v=unserialize($ocl->storage->load_key($identifier));
	$v=@array_reverse($v);
	for($x=0; $x<count($v); $x++) {
			
		$cur_box_content = "<table class='service_table' cellpadding=2>";
		$cur_box_content .=$layout->Tr(
			$layout->Td(
					Array(
						0=>Array(
							"width" => "900",
							'colspan'=> 3,
							"align"=>"left",
							"show"=>  nl2br($v[$x][ocl_error_long])
							)
					)
				)
		
		,true);
		//images/diabled.gif
		$del_icon="<a href='#' onClick='xajax_ExtensionAjax(\"OcL\", \"xajax_ocl_del_entry\",\"" . $identifier . "\",\""  . $v[$x][ocl_id] .  "\" )'><img border=0 alt='delete this entry' src='images/diabled.gif'></A>";
		$mod_icon="<a href='extensions_wrap.php?script=OcL/modify.php&identifier=" . $identifier . "&ocl_id=" . $v[$x][ocl_id] ."'><img border=0 alt='modify this entry' src='images/modify.gif'></A>";
		$grp_str=$ocl->resolveGroupString($v[$x][ocl_service_var]);
		
		$cur_box_content .=$layout->Tr(
			$layout->Td(
					Array(
						0=>Array(
							
							'colspan'=> 3,
							"align"=>"left",
							"show"=>  "<hr noshade>"
							)
					)
				)
		
		,true);
		
		$cur_box_content .=$layout->Tr(
			$layout->Td(
					Array(
						0=>Array(
							'colspan'=> 1,
							"align"=>"right",
							"show"=>  "Type:" . $v[$x][ocl_type] . " Took: " . $v[$x][ocl_duration].  " mins. Activated by: <i>" . $v[$x][ocl_caller] . "</i>"
							)
						,	
						1=> $grp_str,
						2=> $del_icon . " " . $mod_icon
						
					)
				)
		
		,true);
		$cur_box_content .= "</table>";
		
		$layout->push_outside($layout->create_box($v[$x][ocl_subject] . " by <b>" . $v[$x][ocl_poster]  . "</b> posted on <i>" . $v[$x][ocl_date] . "</i>", $cur_box_content));

			
			
	}
	
	$layout->TableEnd();
	$layout->display();
	
	
	
?>