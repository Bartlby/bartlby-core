<?php
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/Bartlby_Inventory/Bartlby_Inventory.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$inv = new Bartlby_Inventory();
	
	$layout= new Layout();
	$layout->set_menu("Inventory");
	$layout->Table("100%");
	
	$servers=$btl->getSVCMap();
	
	while(list($k, $v) = each($servers)) {
		if($_GET[server_id]) {
			if($v[0][server_id] != $_GET[server_id]) {
				continue;	
			
			}
		}
		$tac_title=$v[0][server_name];  
		$defaults=$inv->getDefaults($v[0][server_id]);
		
		
		$tac_content = "<form name='fm1' action='extensions/Bartlby_Inventory/save.php'><table class='nopad' width='100%'>
			<tr>
				<td>Serial</td>
				<td><input type=text value='" . $defaults[serial] . "' name='serial'><input type=hidden value='" . $v[0][server_id] . "' name='id'></td>
			</tr>
			<tr>
				<td>Warranty End</td>
				<td><input type=text value='" . $defaults[war] . "' name='Warranty'></td>
			</tr>
			<tr>
				<td>Additional Info</td>
				<td><textarea name='add_info'>" . $defaults[info] . "</textarea></td>
			</tr>
			<tr>
				
				<td colspan=2><input type=submit value='save'></td>
			</tr>
		</table></form>";
		$layout->push_outside($layout->create_box($tac_title, $tac_content));
	
		
		
	}
	
	$layout->TableEnd();
	$layout->display();

?>