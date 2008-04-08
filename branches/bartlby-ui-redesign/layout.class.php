<?
#////////////////////////////////////////////
#//
#// MXS(erver) Project by h.januschka
#// initatated august 2003
#// -------------------------------------
#// MultiPlayerMedia Server with the main 
#// goal to have an easy interface for 
#// Macromedia FLASH© games and Guis
#//
#//
#//
#//
#//--------------------------------------------
#// @package WAI
#// @file LAYOUT.class.php
#// @author klewan <klewan@chello.at>
#// @desc centralized layout control :)
#//
#//
#//
#////////////////////////////////////////////

require_once ("xajax/xajax.inc.php");
include("xajax.common.php");

class Layout {
	var $OUT;
	var $template_file;
	
	
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
		$r = "<table class=\"nopad\">	<tr><td class=\"nav_main\" onClick=\"doToggle('$name')\"><img id='" . $name . "_plus' src='images/plus.gif' border=0> $name</td></tr><tr><td class=\"nav_place\">&nbsp;</td></tr></table><table class=\"nopad\" id='" . $name . "_sub' style='display:none;'>";
		
		
		return $r;	
	}
	function addSub($root, $name, $link) {
		$r="<tr><td class=\"nav_sub\"><a href='" . $link . "' class=\"sub\">$name</A></td></tr><tr><td class=\"nav_place\">&nbsp;</td></tr>";
		return $r;	
	}
	function endMenu() {
		return "</table>";	
	}
	
	function display($cr="") {
		//global $BARTLBY_UI_VERSION;
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
			
		$this->end_time=$this->microtime_float();
		$diff=$this->end_time-$this->start_time;
		$source_file=$_SERVER[SCRIPT_URI];
		$source_file=str_replace(".php", ".phps",$source_file);
		$bname=basename($source_file);
		
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
		
		
		
		$fp=fopen($this->template_file, "r");
		while(!feof($fp)) {
			$str=fgets($fp, 1024);
			$o = str_replace("<!--BTUICONTENT-->",$this->OUT,$str);
			$o = str_replace("<!--BTUIOUTSIDE-->",$this->OUTSIDE,$o);
			$o = str_replace("<!--BTUIBOXTITLE-->",$this->BoxTitle,$o);
			$o = str_replace("<!--BTUITIME-->",round($diff,2),$o);
			$o = str_replace("<!--BTLEXTMENU-->",$this->ext_menu,$o);
			$o = str_replace("<!--SERVERTIME-->",date("d.m.Y H:i:s") ,$o);
			$o = str_replace("<!--XAJAX-->", $xajax->getJavascript("xajax"), $o);
			$o = str_replace("<!--UIVERSION-->",BARTLBY_UI_VERSION ,$o);
			echo $o; 	
		}
		fclose($fp);
		
			
		
		
	}
	function create_box($title, $content, $id="") {
		if($id != "") {
			$oid = " id=\"$id\" ";
		} else {
			$oid = "";	
		}
		
		
		$rr .= '
			<table class="nopad" ' . $oid . '>
				<tr>
					<td class="box_left_corner_top">&nbsp;</td>
					<td class="box_top_run">' . $title . '</td>
					<td class="box_right_corner_top">&nbsp;</td>
				</tr>
				<tr>
					<td class="box_left_run">&nbsp;</td>
					<td class="box_content">' . $content . '</td>
					<td class="box_right_run">&nbsp;</td>
				</tr>
				<tr>
					<td class="box_left_corner_bottom">&nbsp;</td>
					<td class="box_bottom_run">&nbsp;</td>
					<td class="box_right_corner_bottom">&nbsp;</td>
				</tr>
			</table>
		';
		return $rr;
	}
	function push_outside($content) {
		$this->OUTSIDE .= $content;
	}

}