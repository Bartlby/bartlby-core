<?php
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/OcL/OcL.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("bsp_manage");
	$sg = new OcL();
	
	
	$layout= new Layout();
	$layout->setTitle("OcL: BsP");
	$layout->set_menu("OcL");
	$layout->OUT .= '<script src="extensions/OcL/bsp.js" type="text/javascript"></script>';
	$layout->OUT .= '<link rel="stylesheet" id="CSS" type="text/css" href="extensions/OcL/bsp.css"></link>';
	$layout->Form("fm1", "extensions_wrap.php");
	$layout->Table("100%");
	$layout->set_menu("OcL");
	
	$servs=$btl->GetWorker();
	$wstr = "<table><tr>";
	$x=0;
	while(list($k, $v) = @each($servs)) {
		if($x%6 == 0) $wstr .= "</tr><tr>";
		$x++;
		
		$v1=bartlby_get_worker_by_id($btl->CFG, $v[worker_id]);
		
		$wstr .= "<td><input type=checkbox name=wrk[] value='" . $v[worker_id] . "'>" . $v[name] . "</td>";
				
		$optind++;
	}
	$wstr .= "</table>";
	
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>"Workers:",
				1=>$wstr
			)
		)

	);
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>"Actions:",
				1=>"<div id='bsp_action'></div>"
			)
		)

	);
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>"Plan:",
				1=>'<div id="bsp_main">loading...<script>window.setTimeout(\'bspDate(' . date("1,(m-1),Y") . ')\', 200);</script></div>'
			)
		)

	);
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>"",
				1=>'<div  style="visibility:hidden;" id="debug" style="height:200px; overflow:auto;"></div>'
			)
		)

	);
	$layout->TableEnd();
	$layout->FormEnd();
	$layout->display();
	
	
/*	
<script>
	
		
	
</script>


BSP init
</div>



	debug:<br>
	<a href="#" onClick="bspDate(1,1,2008);">aaa</A>
</div>';
	$layout->set_menu("OcL");
	$layout->display();


*/