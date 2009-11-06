<?php
include "config.php";

class OcL {
	function OcL() {
		$this->layout = new Layout();
		$this->storage = new BartlbyStorage("OcL");
		
	}
	
	function resolveGroupString($str) {
		global $btl;
		$aa=explode("|", $str);
		for($aax=0; $aax<count($aa); $aax++) {
			$bb = explode("=", $aa[$aax]);
			if($aa[$aax]) {
				//$svc = @bartlby_get_service_by_id($this->CFG, $aa[$aax]);
				$idx=$btl->findSHMPlace($aa[$aax]);
				$svc=bartlby_get_service($btl->CFG, $idx);
				$dtemp="";
				if($svc[is_downtime] == 1) {
					$dtemp="<i>DOWNTIME</i>";
				}
				$r .= "Service: <a href='service_detail.php?service_place=" . $idx . "'>$svc[server_name]:$svc[client_port]/$svc[service_name]</A> (Current: <font color=" . $btl->getColor($svc[current_state]) . ">" . $btl->getState($svc[current_state]) . "</font>) $dtemp<br>";
			}
		}	
		return $r;
	}
	function _About() {
		$snotice="enables you to 'blog' about system events";
		return $snotice;
			
	}
	
	function _Menu() {
		$r =  $this->layout->beginMenu();
		$r .= $this->layout->addRoot("OcL");
		$r .= $this->layout->addSub("OcL", "Logs","extensions_wrap.php?script=OcL/index.php");
		$r .= $this->layout->addSub("OcL", "Add","extensions_wrap.php?script=OcL/add.php");
		$r .= $this->layout->addSub("OcL", "Schedule","extensions_wrap.php?script=OcL/bsp.php");
		
		$r .= $this->layout->endMenu();
		return $r;
	}
	
	function xajax_get_bsp_entrys() {
		global $xajax, $btl;
		$res = new xajaxResponse();
		//$res->AddScript("alert('loaded');");
		return $res;
	}
	function xajax_ocl_add_standby() {
		global $xajax, $btl;
		$btl->hasRight("bsp_manage");
		$this->storage = new BartlbyStorage("OcL");
		$res = new xajaxResponse();
		$values = $xajax->_xmlToArray("xjxquery", $_GET[xajaxargs][2]);
		
		/* Add to activ, each checked hour */
		$worker_string="";
		$key=$_GET[xajaxargs][3] . "-" . $_GET[xajaxargs][4] . "-" . $_GET[xajaxargs][5] . "-standby";
		$stored=unserialize($this->storage->load_key($key));
		
		if($stored==false) {
			//// FRESH
			$stored[0]="";	
		}
		
		
		for($x=0; $x<count($values[wrk]); $x++) {
			
			$cw = @bartlby_get_worker_by_id($btl->CFG, $values[wrk][$x]);
			$worker_string  .= "<li class=bsp>" . $cw[name];
			
		}
		
		
		
		for($x=0; $x<count($values[chkb]); $x++) {
			//echo $values[chkb][$x] . "\n";
			
			$res->AddAssign("standbys_" . $values[chkb][$x], "innerHTML", "<b>Active</b>:" . strtolower($worker_string));	
			
			//Store Map
			$stored[$values[chkb][$x]]=$values[wrk];
		}
		
		
		$rs=$this->storage->save_key($key, serialize($stored));
		return $res;	
	}
	function xajax_ocl_add_active() {
		global $xajax, $btl;
		$btl->hasRight("bsp_manage");
		$this->storage = new BartlbyStorage("OcL");
		$res = new xajaxResponse();
		$values = $xajax->_xmlToArray("xjxquery", $_GET[xajaxargs][2]);
		
		/* Add to activ, each checked hour */
		$worker_string="";
		$key=$_GET[xajaxargs][3] . "-" . $_GET[xajaxargs][4] . "-" . $_GET[xajaxargs][5] . "-active";
		$stored=unserialize($this->storage->load_key($key));
		
		if($stored==false) {
			//// FRESH
			$stored[0]="";	
		}
		
		
		for($x=0; $x<count($values[wrk]); $x++) {
			
			$cw = @bartlby_get_worker_by_id($btl->CFG, $values[wrk][$x]);
			$worker_string  .= "<li class=bsp>" . $cw[name];
			
		}
		
		
		
		for($x=0; $x<count($values[chkb]); $x++) {
			//echo $values[chkb][$x] . "\n";
			
			$res->AddAssign("actives_" . $values[chkb][$x], "innerHTML", "<b>Active</b>:" . strtolower($worker_string));	
			
			//Store Map
			$stored[$values[chkb][$x]]=$values[wrk];
		}
		
		
		$rs=$this->storage->save_key($key, serialize($stored));
		return $res;	
	}
	function xajax_get_bsp_hours() {
		
		global $xajax, $btl;
		$btl->hasRight("bsp_manage");
		$res = new xajaxResponse();
		$servs=$btl->GetWorker();
		
		
		while(list($k, $v) = @each($servs)) {
			$linked_worker[$v[worker_id]]=$v[name];	
		}
		
		
		
		$key1=$_GET[xajaxargs][2] . "-" . $_GET[xajaxargs][3] . "-" . $_GET[xajaxargs][4] . "-active";
		$key2=$_GET[xajaxargs][2] . "-" . $_GET[xajaxargs][3] . "-" . $_GET[xajaxargs][4] . "-standby";
		
		$stored1=unserialize($this->storage->load_key($key1));
		$stored2=unserialize($this->storage->load_key($key2));
		
		
		while(list($k,$v) = @each($stored1)) {
			$worker_string="";
			for($x=0; $x<count($v); $x++) {
				$worker_string .= "<li>" . $linked_worker[$v[$x]];	
			}
			$res->AddAssign("actives_" . $k, "innerHTML", "<b>Active</b>:" . strtolower($worker_string));
		}
		
		while(list($k,$v) = @each($stored2)) {
			$worker_string="";
			for($x=0; $x<count($v); $x++) {
				$worker_string .= "<li>" . $linked_worker[$v[$x]];	
			}
			$res->AddAssign("standbys_" . $k, "innerHTML", "<b>Standby</b>:" . strtolower($worker_string));
		}
		
		//$res->AddScript("alert('loaded');");
		return $res;	
	}
	function xajax_ocl_del_entry() {
		global $xajax, $btl;
		$res = new xajaxResponse();
		$btl->hasRight("ocl_delete");
		$identifier=$_GET[xajaxargs][2];
		$ocl_id=$_GET[xajaxargs][3];
		
		$v=unserialize($this->storage->load_key($identifier));
		
		
		$new_x=0;;
		for($x=0; $x<count($v); $x++) {
			if($v[$x][ocl_id] != $ocl_id) {
				$n[$new_x]=$v[$x];
				$new_x++;
			}
		}
		
		
		$this->storage->save_key($identifier,serialize($n));
		$res->AddScript("document.location.reload()");
		
		return $res;
	}
	function xajax_ocl_add_form() {
		global $xajax;
		$res = new xajaxResponse();
		$values = $xajax->_xmlToArray("xjxquery", $_GET[xajaxargs][2]);
		$e=0;
		$res->addAssign("error_ocl_date", "innerHTML", "");
		$res->addAssign("error_ocl_subject", "innerHTML", "");
		$res->addAssign("error_ocl_duration", "innerHTML", "");
		$res->addAssign("error_ocl_caller", "innerHTML", "");
		$res->addAssign("error_ocl_error_long", "innerHTML", "");
		
		if($values[ocl_date] == "") {
			$res->addAssign("error_ocl_date", "innerHTML", "required field");			
			$e++;
		} 
		if($values[ocl_subject] == "") {
			$res->addAssign("error_ocl_subject", "innerHTML", "required field");			
			$e++;
		} 
		
		if($values[ocl_duration] == "") {
			$res->addAssign("error_ocl_duration", "innerHTML", "required field");			
			$e++;
		} 
		if($values[ocl_caller] == "") {
			$res->addAssign("error_ocl_caller", "innerHTML", "required field");			
			$e++;
		} 
		if($values[ocl_error_long] == "") {
			$res->addAssign("error_ocl_error_long", "innerHTML", "required field");			
			$e++;
		} 
		if($e == 0) {
			$res->AddScript("document.fm1.submit()");	
		}
		return $res;
	}
	
	function _permissions() {
		global $worker_rights;
		
		$ky["ocl_add"]="allowed to add ocl-entrys";	
		$ky["ocl_csv"]="can view csv reports";
		$ky["ocl_view"]="can view logbook";
		$ky["ocl_edit"]="can edit entrys";
		$ky["ocl_delete"]="can delete";
		$ky["bsp_manage"]="allowed to manage/update BsP's";
		
		while(list($k, $v) = each($ky)) {
			$kc="";
			if($worker_rights[$k][0] && $worker_rights[$k][0] != "false") {
				$kc="checked";	
			}
			$r .= "<input type=checkbox name='$k' $kc> " . $ky[$k] . "<br>";
				
		}
		return $r;
	}
	
	
	function bsp_me_is_in($map, $state) {
		global $btl;
		$last_swaped=0;
		$gx=0;
		while(list($k, $v) = @each($map)) {
			
			for($x=0; $x<count($v); $x++) {
				
				if($v[$x] == $btl->user_id) {
					
					$r .= sprintf("%02d:00", $k) . " ";	
					
				}
				
				
				
			}	
			
		}
		return $r;
	}
	
	function _overview() {
		global $btl;
		$identifier = date("m.Y",time());
		$v=unserialize($this->storage->load_key($identifier));
		$v=@array_reverse($v);
		
		
		$lm = "Latest 3 OcL entry's:";
		
		for($x=0; $x<3; $x++) {
			if($v[$x][ocl_subject]) {
				$lm .= "<li> <a href='extensions_wrap.php?script=OcL/index.php'>" . $v[$x][ocl_subject] . "</b></A> by <i>" . $v[$x][ocl_poster] . "</i> on " . $v[$x][ocl_date];	
			}
		}
		$bsp_today="NO";
		$bsp_tomorrow="NO";
		
		
		
		
		$act1=date("j-n-Y") . "-active";
		$stb1=date("j-n-Y") . "-standby";
		$act2=date("j-n-Y",time()+86400) . "-active";
		$stb2=date("j-n-Y",time()+86400) . "-standby";
		
		$a1=unserialize($this->storage->load_key($act1));
		$s1=unserialize($this->storage->load_key($stdb1));
		$a2=unserialize($this->storage->load_key($act2));
		$s2=unserialize($this->storage->load_key($stb2));
		
		$today_temp_a =$this->bsp_me_is_in($a1, "activ");
		$today_temp_s =$this->bsp_me_is_in($s1,"standby");
		
		$tomorrow_temp_a =$this->bsp_me_is_in($a2, "activ");
		$tomorrow_temp_s =$this->bsp_me_is_in($s2,"standby");
		
		
		
		if($today_temp_a  != "" || $today_temp_s != "") {
			$bsp_today="Activ: $today_temp_a , Standby: $today_temp_s";
			
		}
		if($tomorrow_temp_a  != "" || $tomorrow_temp_s != "") {
			$bsp_tomorrow="Activ: $tomorrow_temp_a , Standby: $tomorrow_temp_s";
			
		}
		
		
		$lm .= "<br>BsP for you!:<br>";
		$lm .= "<b>Today</b>: " . $bsp_today . "<br>"; 
		$lm .= "<b>Tomorrow</b>:" . $bsp_tomorrow;
		
		return $lm;
		
		
	}
	/*
	function _services() {
		return "_services";	
	}
	function _processInfo() {
		return "_processInfo";	
	}
	*/
	/*
	function _serverDetail() {
			
	}
	*/
	/*
	function _serviceDetail() {
		global $defaults;
		
		
		return "<a href='extensions_wrap.php?script=OcL/index.php&server_id=" . $defaults[server_id] . "'>Modify/View Inventory Details</A>";		
	}
	*/
	
	
	
	
}

?>
