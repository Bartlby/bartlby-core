<?
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/ServerGroups/ServerGroups.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	if($_GET[group_name]) {
		$btl->hasRight("sg_edit");
	} else {
		$btl->hasRight("sg_add");
	}
	
	$sg = new ServerGroups();
	
	$layout= new Layout();
	$layout->set_menu("Server Groups");
	$layout->Table("100%");
	
	
	
	$servers=$btl->GetServers();
	$leftservers=array();
	$rightservers=array();
	while(list($k,$v) = @each($servers)) {
		if(!@in_array($k, $members[servers])) {
			@array_push($leftservers, array("v"=>$k, "k"=>$v));	
		} else {
			@array_push($rightservers, array("v"=>$k, "k"=>$v));	
		}
	}
	reset($servers);	
	
	$members = $sg->load($_GET[group_name]);
	$tac_title=$members[name];  
	
	$leftservers=array();
	$rightservers=array();
	
	while(list($k,$v) = @each($servers)) {
		
		if(!@in_array($k, $members[servers])) {
			@array_push($leftservers, array("v"=>$k, "k"=>$v,"s"=>0));	
		} else {
			@array_push($leftservers, array("v"=>$k, "k"=>$v, "s"=>1));	
		}
	}
	
	
	$tac_content = "<form name='fm1' action='extensions_wrap.php'><input type=hidden name=script value='ServerGroups/save.php'><table class='nopad' width='100%'>
		<tr>
			<td colspan=1>Name</td>
			<td colspan=1><input type=text name=grpname value='" . $members[name] . "'></td>
			
		</tr>
		<tr>
			<td  colspan= 2 align=left valign=top>" . $layout->DropDown("all_servers[]", $leftservers, 'multiple') . "</td>
			
		</tr>
		<tr>
			<td colspan=2><input type=submit onClick='markALL(this.form)' value='store group'></td>
		</tr>
		
		
		
		
	</table></form>";
	$layout->push_outside($layout->create_box($tac_title, $tac_content));
	reset($servers);
			
	$layout->setTitle("Actions");
	$del_icon = "<a href='extensions_wrap.php?script=ServerGroups/delete.php&grpname=" . $members[name] . "'><img src='extensions/ServerGroups/delete_group.gif' border=0></A>";
				
	$layout->Tr(
	$layout->Td(
			Array(
				0=>$del_icon
			)
		)

	);
	
	
	$layout->TableEnd();
	$layout->display();

?>