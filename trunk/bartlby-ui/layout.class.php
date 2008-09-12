<?
/* $Id: ack.c 16 2008-04-07 19:20:34Z hjanuschka $ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2008 Helmut Januschka - All Rights Reserved
 *   Contact: <helmut@januschka.com>, <contact@bartlby.org>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 *   visit: www.bartlby.org for support
 * ----------------------------------------------------------------------- */
/*
$Revision: 16 $
$HeadURL: http://bartlby.svn.sourceforge.net/svnroot/bartlby/trunk/bartlby-core/src/ack.c $
$Date: 2008-04-07 21:20:34 +0200 (Mo, 07 Apr 2008) $
$Author: hjanuschka $ 
*/


require_once ("xajax/xajax.inc.php");
include("xajax.common.php");

class Layout {
	var $OUT;
	var $template_file;
	var $box_count;
	

	function setTheme($name="classic") {
		if($name=="") $name="classic";
		$this->theme=$name;	
	}
	
	function set_menu($men) {
		$this->OUT .= "<script>doToggle('" . $men . "');</script>";	
		$this->menu_set=true;
	}
	function microtime_float()
	{
   		list($usec, $sec) = explode(" ", microtime());
   		return ((float)$usec + (float)$sec);
	}
	function setTemplate($file) {
		$this->template_file=$file;
	}
	function Layout($scr='') {
		$this->box_count=1;
		if(bartlby_config("ui-extra.conf", "theme") != "") {
			$this->theme=bartlby_config("ui-extra.conf", "theme");
		} else {
			$this->theme="classic";
		}

		$this->template_file="template.html";
		$this->start_time=$this->microtime_float();
		$this->menu_set=false;
	}

	function Table($proz="100%", $border=0) {
		$this->OUT .= "<table border=$border width='$proz' cellpadding=0 cellspacing=0 border=0>";
	}
	function MetaRefresh($time=20) {
		$this->OUT .= "<script>function ReloadME() {
			if(global_reload == 1) {
				document.location.reload();
			}
			window.setTimeout('ReloadME()', " . $time . "000);
		}
		window.setTimeout('ReloadME()', " . $time . "000);</script>";	
	}
	function TableEnd() {
		$this->OUT .= "</table>";	
	}
	function DisplayHelp($msg=array()) {
		for($x=0; $x<=count($msg);$x++) {
			$fin .= "msg[]=" . $msg[$x] . "&";
		}
		//$this->OUT .= "<script>parent.unten.location.href='help.php?" . $fin . "';</script>";
	}

	function Td($data=array()) {
		for($x=0;$x<count($data);$x++) {
			$width="";
			$height="";
			$class="";
			$colspan="";
			$disp=$data[$x];
			$align="align=left";
			if (is_array($data[$x])) {
				if ($data[$x]["width"]) $width="width='" . $data[$x]["width"] .  "'";
				if ($data[$x]["height"]) $height="height='" . $data[$x]["width"] .  "'";
				if ($data[$x]["class"]) $class="class='" . $data[$x]["class"] .  "'";
				if ($data[$x]["colspan"]) $colspan="colspan='" . $data[$x]["colspan"] .  "'";
				if ($data[$x]["align"]) $align="align='" . $data[$x]["align"] .  "'";
			}
			
			if (is_array($data[$x])) $disp=$data[$x]["show"];
			
			
			//$disp=htmlspecialchars($disp);			
			$r .= "<td $colspan  $align  valign=top $width $height $class>\n" . $disp . "\n</td>\n";	
		}
		return $r;
	}
	
	function Tr($td, $return = false) {
		$data="<tr>\n$td\n</tr>\n";
		if($return == true) {
			return  $data;
		} else {
			$this->OUT .= $data;
		}
		
	}
	function Form($name,$action, $method='GET', $r = false) {
		
		$rr = "<form id='$name' name='$name' action='$action' method='POST'>\n";	
		if($r) {
			return $rr;	
		} else {
			$this->OUT .= $rr;	
		}
		
	}
	function FormEnd($r=false) {
		$rr .= "</form>\n";	
		if($r) {
			return $rr;	
		} else {
			$this->OUT .= $rr;	
		}
	}
	
	function TextArea($name, $def, $height=7, $width=100) {
		return "<textarea name='$name' cols=$width rows=$height>$def</textarea>\n";
	}
	
	function Field($name, $type='text', $value='',$L='', $chkBox='', $help = array()) {
		$n="name='$name' id='$name'";
		$value=htmlspecialchars($value);
		if($help) {
			$hIcon="<a href='help.php?msg[0]=$help&msg[1]=NULL' target='unten'><img src='info.gif' border=0></A>";
		}
		$r="<input type='$type' value='$value' $n $chkBox>$hIcon<div style='color:#ff0000' id='error_" . $name . "'></div>\n";
		if ($L) {
			$this->OUT .= $r;
		} else {
			return $r;
		}
		
	}
	function DropDown($name,$options=array(), $type='', $style='') {
		$r = "<select name='$name' $type $style>\n";
		for ($x=0;$x<count($options); $x++) {
			$sel="";
			if ($options[$x][s] == 1) $sel="selected";
			$r .= "<option style='background-color: " .  $options[$x][c] . "' value='" . $options[$x][v] . "' $sel>" . $options[$x][k] . "\n";	
		}		
		$r .= "</select><div style='color:#ff0000' id='error_" . $name . "'></div>\n";
		return $r;
	}
	function setTitle($str) {
		$this->BoxTitle=$str;
	}
	function beginMenu() {
		return "";	
	}
	function addRoot($name) {

		$r = "<table class=\"nopad\">	<tr><td class=\"nav_main\" onClick=\"doToggle('$name')\"><img id='" . $name . "_plus' src='themes/" . $this->theme . "/images/plus.gif' border=0> $name</td></tr><tr><td class=\"nav_place\">&nbsp;</td></tr></table><table class=\"nopad\" id='" . $name . "_sub' style='display:none;'>";
		
		
		return $r;	
	}
	function addSub($root, $name, $link) {
		$r="<tr><td class=\"nav_sub\"><a href='" . $link . "' class=\"sub\">$name</A></td></tr><tr><td class=\"nav_place\">&nbsp;</td></tr>";
		return $r;	
	}
	function endMenu() {
		return "</table>";	
	}
	
	function display($lineup_file="") {
		if($lineup_file=="no") $lineup_file="";
		global $xajax;
		
		if($this->menu_set == false) {
			$this->set_menu("core");	
			$this->set_menu("main");
			$this->set_menu("report");
			$this->set_menu("services");
			$this->set_menu("session");
			$this->set_menu("client");
			$this->set_menu("worker");
			$this->set_menu("downtimes");
			
		}
		$this->template_file="themes/" . $this->theme . "/theme.php";
			
		$this->end_time=$this->microtime_float();
		$diff=$this->end_time-$this->start_time;
		

		//Create Menu.
		$this->ext_menu .= $this->beginMenu();
		$this->ext_menu .= $this->addRoot("Monitoring");
                $this->ext_menu .= $this->addSub("Monitoring", "Overview","overview.php");
                $this->ext_menu .= $this->addSub("Monitoring", "Services","services.php");
                $this->ext_menu .= $this->addSub("Monitoring", "Servers","extensions_wrap.php?script=ServerGroups/groupview.php");
		$this->ext_menu .= $this->endMenu();


		$this->ext_menu .= $this->beginMenu();
		$this->ext_menu .= $this->addRoot("Reporting");
                $this->ext_menu .= $this->addSub("Reporting", "Report/s","create_report.php");
                $this->ext_menu .= $this->addSub("Reporting", "Logfile","logview.php");
                $this->ext_menu .= $this->addSub("Reporting", "Notifications","logview.php?bartlby_filter=@NOT@");
		$this->ext_menu .= $this->endMenu();

		$this->ext_menu .= $this->beginMenu();
		$this->ext_menu .= $this->addRoot("Server/s");
                $this->ext_menu .= $this->addSub("Server/s", "Add","add_server.php");
                $this->ext_menu .= $this->addSub("Server/s", "Modify","server_list.php?script=modify_server.php");
                $this->ext_menu .= $this->addSub("Server/s", "Delete","server_list.php?script=delete_server.php");
		$this->ext_menu .= $this->EndMenu();

		$this->ext_menu .= $this->beginMenu();
		$this->ext_menu .= $this->addRoot("Packages");
                $this->ext_menu .= $this->addSub("Packages", "Install","server_list.php?script=install_pkg.php");
                $this->ext_menu .= $this->addSub("Packages", "Uninstall","server_list.php?script=uninstall_pkg.php");
                $this->ext_menu .= $this->addSub("Packages", "Create","package_create.php");
                $this->ext_menu .= $this->addSub("Packages", "Delete","package_delete.php");
		$this->ext_menu .= $this->endMenu();

		$this->ext_menu .= $this->beginMenu();
		$this->ext_menu .= $this->addRoot("Service/s");
                $this->ext_menu .= $this->addSub("Service/s", "Add","add_service.php");
                $this->ext_menu .= $this->addSub("Service/s", "Modify","service_list.php?script=modify_service.php");
                $this->ext_menu .= $this->addSub("Service/s", "Delete","service_list.php?script=delete_service.php");
                $this->ext_menu .= $this->addSub("Service/s", "Bulk","bulk_actions.php");
		$this->ext_menu .= $this->endMenu();

		$this->ext_menu .= $this->beginMenu();
		$this->ext_menu .= $this->addRoot("Downtime/s");
                $this->ext_menu .= $this->addSub("Downtime/s", "Add","service_list.php?script=add_downtime.php");
                $this->ext_menu .= $this->addSub("Downtime/s", "Modify","downtime_list.php?script=modify_downtime.php");
                $this->ext_menu .= $this->addSub("Downtime/s", "Delete","downtime_list.php?script=delete_downtime.php");
		$this->ext_menu .= $this->endMenu();


		$this->ext_menu .= $this->beginMenu();
		$this->ext_menu .= $this->addRoot("Worker/s");
                $this->ext_menu .= $this->addSub("Worker/s", "Add","add_worker.php");
                $this->ext_menu .= $this->addSub("Worker/s", "Modify","user_list.php?script=modify_worker.php");
                $this->ext_menu .= $this->addSub("Worker/s", "Delete","user_list.php?script=delete_worker.php");
		$this->ext_menu .= $this->addSub("Worker/s", "Permissions","user_list.php?script=permission_worker.php");
		$this->ext_menu .= $this->endMenu();


		



		$dhl = opendir("extensions");
		while($file = readdir($dhl)) {
			if($file != "." && $file != "..") {
				if(!file_exists("extensions/" .  $file . ".disabled")) {
					@include_once("extensions/" . $file . "/" . $file . ".class.php");
					if (class_exists($file)) {
						eval("\$clh = new " . $file . "();");
						if(method_exists($clh, "_menu")) {
							$this->ext_menu .= $clh->_menu();
						}
					}
				}
			}
		}
		closedir();
		

		$this->ext_menu .= $this->beginMenu();
		$this->ext_menu .= $this->addRoot("Core");
                $this->ext_menu .= $this->addSub("Core", "Reload","bartlby_action.php?action=reload");
                $this->ext_menu .= $this->addSub("Core", "Config","choose_config.php");
                $this->ext_menu .= $this->addSub("Core", "Statistic","statistic.php");
		$this->ext_menu .= $this->addSub("Core", "Event Queue","event_queue.php");
		$this->ext_menu .= $this->addSub("Core", "Extensions","extensions.php");
		$this->ext_menu .= $this->addSub("Core", "About","version.php");
		$this->ext_menu .= $this->endMenu();

		
		$this->BTUICONTENT=$this->OUT;
		$this->BTUIOUTSIDE=$this->OUTSIDE;
		$this->BTUIBOXTITLE=$this->BoxTitle;
		$this->BTUITIME=round($diff,2);
		$this->BTLEXTMENU=$this->ext_menu;
		$this->SERVERTIME=date("d.m.Y H:i:s");
		$this->XAJAX=$xajax->getJavascript("xajax");			
		$this->UIVERSION=BARTLBY_UI_VERSION;
		$this->RELNOT=BARTLBY_RELNOT;
		
		$this->create_box($this->BoxTitle, $this->OUT, "MAIN");


		//Default LineUp
		if($lineup_file == "") {
			$lineup_file="default";
		}

		$lineup_path="themes/" . $this->theme . "/lineups/" . $lineup_file . ".php";
		if(!file_exists($lineup_path)) {
			$lineup_path="themes/classic/lineups/default.php";
		}
		ob_start();
			include($lineup_path);
		
		$this->BTUIOUTSIDE = ob_get_contents();		
		ob_end_clean();
		

		ob_start();
			include($this->template_file);
		
		$o = ob_get_contents();
		ob_end_clean();

		echo $o;


			
		
		
	}

	function disp_box($name) {
		if($name != "UNPLACED") {
			$this->boxes_placed[$name]=true;
			return $this->boxes[$name];
		} else {
			@ksort($this->boxes);
			while(list($k, $v) = @each($this->boxes)) {
				if($this->boxes_placed[$k] != true) {
					$r .= $v;
				}			
			}
			return $r;
		}
	}
	function create_box($title, $content, $id="", $plcs="", $box_file="") {
		if($id != "") {
			$oid = $id;
		} else {
			//$oid = rand(100,2);	
			$oid=$this->box_count;
			$this->box_count++;
		}
		if(!is_array($plcs)) {
			$box_file="default_box.php";
		} else {
			$box_file .= ".php";
		}
		$boxes_path="themes/" . $this->theme . "/boxes/" . $box_file;
		if(!file_exists($boxes_path)) {
			
			$boxes_path="themes/classic/boxes/" . $box_file;
		}
		ob_start();
			include($boxes_path);
		
		$o = ob_get_contents();	
			
		ob_end_clean();		
		$this->boxes[$oid]=$o;
		if($box_file != "default_box.php") { //pack into a standard box
			$this->create_box($title, $o, $oid);
		}


		return $oid;
	}
	function push_outside($content) {
		//echo "HELLP!!!";
	}

}
