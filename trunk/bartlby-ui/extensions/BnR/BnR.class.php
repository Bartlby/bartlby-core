<?php

include "config.php";

define(GROUPS_PER_ROW, 4);

class BnR {
	function BnR() {
		$this->layout = new Layout();
		
	}
	
	
	function _About() {
		return "Backup and restore";
			
	}
	function xajax_backup_form() {
		global $xajax;
		$res = new xajaxResponse();
		$values = $xajax->_xmlToArray("xjxquery", $_GET[xajaxargs][2]);
		
		if($values[package_with_comment] == "") {
			$res->addAssign("error_package_with_comment", "innerHTML", "required field");			
		} else {
			$res->AddScript("document.fm1.submit()");	
		}
		
		return $res;
	}
	function _permissions() {
		global $worker_rights;
		//bnr.backup
		//bnr.restore
		/*
		$bchecked="";
		$rchecked="";
		if($worker_rights["bnr.backup"][0] && $worker_rights["bnr.backup"][0] != "false") {
			$bchecked="checked";
		}
		if($worker_rights["bnr.restore"][0] && $worker_rights["bnr.restore"][0] != "false") {
			$rchecked="checked";
		}
		
		$r = "<input type=checkbox name='bnr.backup' $bchecked> Backup<br>";	
		$r .= "<input type=checkbox name='bnr.restore' $rchecked> Restore (also requires super user)<br>";
		*/
		$r = "BnR requires 'super user' right";
		return $r;
	}
	
	function _Menu() {
		$r =  $this->layout->beginMenu();
		$r .= $this->layout->addRoot("BnR");
		$r .= $this->layout->addSub("BnR", "Backup","extensions_wrap.php?script=BnR/backup.php");
		$r .= $this->layout->addSub("BnR", "Restore","extensions_wrap.php?script=BnR/restore.php");
		
		$r .= $this->layout->endMenu();
		return $r;
	}
	
	
	/*
	function _services() {
		return "_services";	
	}
	function _processInfo() {
		return "_processInfo";	
	}
	*/
	
	/* function _quickLook() {
		global $_GET, $rq;
		$rq .= "<tr>";
		$rq .= "<td colspan=2>";
		$rq .= "<center><b>Groups</b></center>";
		$rq .= "</td></tr>";
		
		
		$svcfound=false;
		$dhl = opendir("extensions/BnR/data/");
		
		while($file = readdir($dhl)) {
			
			if($file == "." || $file == "..") {
				continue;	
			}	
			
			$defaults=$this->load($file);
			if(!$defaults[name]) {
				continue;	
			}
			if(@preg_match("/" . $_GET[search] . "/i", $defaults[name])) {
				$svcfound=true;
				$rq .= "<tr><td colspan=1><a href='extensions_wrap.php?script=BnR/groupview.php&grpname=$file'>" . $defaults[name] . "</A></td><td><img src='extensions/BnR/group_member.png'></td></tr>";
			}
			
		}	
			
		if($svcfound == false) {
			$rq .= "<tr><td colspan=2><i>no group matched</i></td></tr>";
		}
		return "";
	}
	*/
	
	/*
	function _serviceDetail() {
		global $defaults;
		
		
		return "<a href='extensions_wrap.php?script=SSH/index.php&server_id=" . $defaults[server_id] . "'>Modify/View Inventory Details</A>";		
	}
	*/
	
	
}

?>
