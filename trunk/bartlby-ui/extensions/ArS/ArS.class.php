<?

include "config.php";
include_once "bartlbystorage.class.php";



class ArS {
	function ArS() {
		$this->layout = new Layout();
		$this->storage = new BartlbyStorage("ArS");
		
	}
	
	
	function _About() {
		return "Automated Reporting System (EXPERIMENTAL)!!!";
			
	}
	function format_report($rep, $type='html', $hdr) {
		global $btl;
		
		
		
		$svc=$rep[svc];
		$state_array=$rep[state_array];
		$notify=$rep[notify];
		$files_scanned=$rep[files_scanned];
		
		$hun=$svc[0]+$svc[1]+$svc[2];
		
		
		
		
		switch($type) {
			case 'html':
				$rap ="<html><head><style>td{font-size:12px; font-family:tahoma}</style></head><body>";
				$rap .= "<table width=100% border=3>";
			break;	
		}
		switch($type) {
			case 'html':
				$rap .= "<tr><td colspan=3>" . $hdr . "</td></tr>";
			break;	
		}
		
		switch($type) {
			case 'html':
				$rap .= "<tr><td colspan=3><b>Service Availability</b></td></tr>";
			break;	
		}
		
		while(list($state, $time) = @each($svc)) {
			
								
			$perc =   (($hun-$time) * 100 / $hun);
			$perc =100-$perc;
			
			
			switch($type) {
				case 'html':
					$rap .= "<tr>";
					$rap .= "<td>";
					$rap .= "<font color=" . $btl->getColor($state) . ">" . $btl->getState($state) . "</font>";
					$rap .= "</td>";
					$rap .= "<td>";
					$rap .= $btl->intervall($time);
					$rap .= "</td>";
					$rap .= "<td>";
					$rap .= round($perc,2);
					$rap .= "%</td>";
					$rap .= "</tr>";
				break;	
			}
			
			
			
			
		}
		
		switch($type) {
			case 'html':
				$rap .= "<tr><td colspan=3><b>Notifications</b></td></tr>";
			break;	
		}
		while(list($worker, $dd) = @each($notify)) {
			
			switch($type) {
				case 'html':
					$rap .= "<tr><td colspan=2>" . $worker . "</td><td>";
				break;	
			}
			
					
			while(list($trigger, $dd1) = @each($dd)) {
				$rap .= "\t" . $trigger . "<br>";
				while(list($k, $ts) = @each($dd1)) {
					
					switch($type) {
						case 'html':
							$rap .= "<li>" . date("d.m.Y H:i:s", $ts[0]) . " (" . $btl->getState($ts[1]) . ")<br>";
						break;	
					}
				}
			}
			switch($type) {
				case 'html':
					$rap .= "</td></tr>";
				break;	
			}
						
		}
		switch($type) {
			case 'html':
				$rap .= "<tr><td colspan=3><b>Output</b></td></tr>";
			break;	
		}
		for($xy=0; $xy<count($state_array);$xy++) {
				switch($type) {
					case 'html':
						$o1 .= "<tr>";
						$o1 .= "<td>" . date("d.m.Y H:i:s", $state_array[$xy][end]) . "</td>";
						$o1 .= "<td>" .  $btl->getState($state_array[$xy][lstate]) . " </td>";
						$o1 .= "<td>" . $state_array[$xy][msg] . " </td>";
						$o1 .= "</tr>";
					
					break;	
				}
								
		}
		
		$rap .= $o1;
		
		switch($type) {
			case 'html':
				$rap .= "</table></body></html>";
			break;	
		}
		
		return $rap;
	
	}
	
	function xajax_ars_delete_form() {
		global $xajax;
		$res = new xajaxResponse();
		$values = $xajax->_xmlToArray("xjxquery", $_GET[xajaxargs][2]);
		
		if($values[ars_report] == "") {
			$res->AddScript("alert('no report selected to delete')");	
		} else {
			$res->AddScript("document.fm1.submit()");	
		}
		
		return $res;	
	}
	function xajax_ars_add_form() {
		global $xajax;
		$res = new xajaxResponse();
		$values = $xajax->_xmlToArray("xjxquery", $_GET[xajaxargs][2]);
		
		if($values[ars_to] == "") {
			$res->addAssign("error_ars_to", "innerHTML", "required field");			
		} else {
			$res->AddScript("document.fm1.submit()");	
		}
		
		return $res;
	}
	function _permissions() {
		global $worker_rights;
		
		$r = "ArS requires 'super user' right";
		return $r;
	}
	
	function _Menu() {
		$r =  $this->layout->beginMenu();
		$r .= $this->layout->addRoot("ArS");
		$r .= $this->layout->addSub("ArS", "Settings","extensions_wrap.php?script=ArS/settings.php");
		$r .= $this->layout->addSub("ArS", "View/Delete","extensions_wrap.php?script=ArS/view.php");
		$r .= $this->layout->addSub("ArS", "Add","service_list.php?script=extensions_wrap.php%3fscript=ArS/add.php");
		
		
		$r .= $this->layout->endMenu();
		return $r;
	}
	
	
	
	
	
}

?>
