<?php
	
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
	
	$layout->OUT .= "<script>
		function moveRight(f) {
			
			cnt=f[\"all_servers[]\"].options.length;
			for(x=0; x<cnt; x++) {
				opt = f[\"all_servers[]\"].options[x];
				if(opt == null) {
					continue;	
				}
				if(opt.selected == true) {
					val = opt.value;
					text = opt.text;
					newOpt = new Option(text, val);
					cr = f[\"choosen_servers[]\"].options.length;
					f[\"choosen_servers[]\"].options[cr] = newOpt;
					f[\"all_servers[]\"].options[x] = null;
				}
			}
		}
		function moveLeft(f) {
			cnt=f[\"choosen_servers[]\"].options.length;
			for(x=0; x<cnt; x++) {
				opt = f[\"choosen_servers[]\"].options[x];
				if(opt == null) {
					continue;	
				}
				if(opt.selected == true) {
					val = opt.value;
					text = opt.text;
					newOpt = new Option(text, val);
					cr = f[\"all_servers[]\"].options.length;
					f[\"all_servers[]\"].options[cr] = newOpt;
					f[\"choosen_servers[]\"].options[x] = null;
				}
			}	
		}
		function markALL(f) {
			cnt=f[\"choosen_servers[]\"].options.length;
			for(x=0; x<cnt; x++) {
				opt = f[\"choosen_servers[]\"].options[x];
				if(opt == null) {
					continue;	
				}
				opt.selected=true;
			}
		}
	
	</script>";
	
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
			@array_push($leftservers, array("v"=>$k, "k"=>$v));	
		} else {
			@array_push($rightservers, array("v"=>$k, "k"=>$v));	
		}
	}
	
	
	$tac_content = "<form name='fm1' action='extensions_wrap.php'><input type=hidden name=script value='ServerGroups/save.php'><table class='nopad' width='100%'>
		<tr>
			<td colspan=1>Name</td>
			<td colspan=1><input type=text name=grpname value='" . $members[name] . "'></td>
			
		</tr>
		<tr>
			<td align=left valign=top>" . $layout->DropDown("all_servers[]", $leftservers, 'multiple', "onDblClick='moveRight(this.form)' style=\"height:250;width:250px\"") . "</td>
			<td align=center valign=middle><input type=button value=\"&gt;&gt\" onClick='moveRight(this.form)'><br><input onClick='moveLeft(this.form)' type=button value=\"&lt;&lt\"></td>
			<td align=right valign=top>". $layout->DropDown("choosen_servers[]", $rightservers, 'multiple',  "onDblClick='moveLeft(this.form)' style=\"height:250;width:250px\"") . "</td>
		</tr>
		<tr>
			<td colspan=4><input type=submit onClick='markALL(this.form)' value='store group'></td>
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