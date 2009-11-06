<?php
include "config.php";

class Bartlby_Inventory {
	function Bartlby_Inventory() {
		$this->layout = new Layout();
		
	}
	function _restore() {
		global $orig_servers, $o, $bdir;	
	
		foreach(glob("extensions/Bartlby_Inventory/*.ser") as $fname) {
			@unlink($fname);
		}
		foreach(glob($bdir . "/*.Bartlby_Inventory") as $fname) {
			$mn = str_replace("Bartlby_Inventory", "ser", $fname);
			@copy($fname, $mn);			
		}
		
	}
	function _backup() {
		global $o, $bdir;	
		$o .= "Bartlby_Inventory saving to " . $bdir . "<br>";
		foreach(glob("extensions/Bartlby_Inventory/data/*.ser") as $fname) {
			clearstatcache();
   			$mn = str_replace("ser", "Bartlby_Inventory", $fname);
   			$o .= $mn . ":::" . basename($mn) . "<br>";
			@copy($fname, $bdir . "/" . basename($mn));
						
		}
	}
	function storeServer($id, $serial, $war,$info) {
		$s[id] = $id;
		$s[serial] = $serial;
		$s[war] = $war;
		$s[info]=$info;
		
		$store=serialize($s);
		
		$fp = fopen("extensions/Bartlby_Inventory/data/" . $id . ".ser", "w");
		fwrite($fp, $store);
		fclose($fp);
		
		header("Location: ../../extensions_wrap.php?script=Bartlby_Inventory/index.php");
	}
	function getDefaults($id) {
		
		$fp = @fopen("extensions/Bartlby_Inventory/data/" . $id . ".ser", "r");
		if($fp) {
			while(!feof($fp)) {
				$bf .= fgets($fp, 1024);	
			}
			fclose($fp);
		}
		$r=unserialize($bf);
		
		return $r;
		
	}
	
	function _About() {
		return "BartlbyInventory Version 0.1 by h.januschka";	
	}
	function _Menu() {
		$r =  $this->layout->beginMenu();
		$r .= $this->layout->addRoot("Inventory");
		$r .= $this->layout->addSub("Inventory", "Overview","extensions_wrap.php?script=Bartlby_Inventory/index.php");
		
		$r .= $this->layout->endMenu();
		return $r;
	}
	/*
	function _overview() {
		return "_overview";	
	}
	function _services() {
		return "_services";	
	}
	function _processInfo() {
		return "_processInfo";	
	}
	*/
	function _serverDetail() {
		global $defaults;
		$d=$this->getDefaults($_GET[server_id]);
		$r="<a href='extensions_wrap.php?script=Bartlby_Inventory/index.php&server_id=" . $_GET[server_id] . "'><img src='images/barcode.gif' border=0> Inventory Details</A><br>";
		if($d[serial] != "") {
			$r .= "<br><b>Serial: </b> " . $d[serial] . "<br>";
		}
		if($d[war] != "") {
			$r .= "<b>Warranty End: </b> " . $d[war] . "<br>";
		}
		if($d[info] != "") {
			$r .= "<b>Aditional Info: </b> " . nl2br($d[info]) . "<br>";
		}
		
		if(function_exists("snmpget")) {
			snmp_set_valueretrieval(SNMP_VALUE_LIBRARY);
			$r .= "<br><br><table cellpadding=0 cellspacing=0 width=100%>";
			$r .= "<tr><td colspan=2><b>SNMP Info</b></td></tr>";
			$descr = @snmpget($defaults[server_ip], "public", "sysDescr.0",1);
			$uptime = @snmpget($defaults[server_ip], "public", "sysUpTime.0",1);
			$contact = @snmpget($defaults[server_ip], "public", "sysContact.0",1);
			$sysname = @snmpget($defaults[server_ip], "public", "sysName.0",1);
			$syslocation = @snmpget($defaults[server_ip], "public", "sysLocation.0",1);
			
			$descr = preg_replace("/(^.*: )/", "", $descr);
			$uptime = preg_replace("/(^.*: )/", "", $uptime);
			$contact = preg_replace("/(^.*: )/", "", $contact);
			$sysname = preg_replace("/(^.*: )/", "", $sysname);
			$syslocation = preg_replace("/(^.*: )/", "", $syslocation);
			
			
			
			$r .= "<tr><td align=left valign=top>Description:</td><td> " . $descr . "</td></tr>";	
			$r .= "<tr><td align=left valign=top>Uptime:</td><td> " . $uptime . "</td></tr>";	
			$r .= "<tr><td align=left valign=top>Contact:</td><td> " . $contact . "</td></tr>";	
			$r .= "<tr><td align=left valign=top>Systemname:</td><td> " . $sysname . "</td></tr>";	
			$r .= "<tr><td align=left valign=top>Location:</td><td> " . $syslocation . "</td></tr>";	
			$r .= "<tr><td align=left valign=top>Uptime:</td><td> " . $uptime . "</td></tr>";	
			
			$r .= "</table>";
		} else {
			$r .= "<br><br><table cellpadding=0 cellspacing=0 width=100%>";
			$r .= "<tr><td colspan=2><i>you should re-compile your php with --with-snmp</i></td></tr>";	
			$r .= "</table>";
		}
		
		return $r;		
	}
	/*
	function _serviceDetail() {
		global $defaults;
		
		
		return "<a href='extensions_wrap.php?script=Bartlby_Inventory/index.php&server_id=" . $defaults[server_id] . "'>Modify/View Inventory Details</A>";		
	}*/
}

?>
