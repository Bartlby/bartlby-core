<?

include "config.php";
include_once "bartlbystorage.class.php";

class SMS {
        function SMS() {
              $this->layout = new Layout();
		$this->storage=new bartlbyStorage("SMS");
		$this->sms_queue="/var/spool/sms/outgoing/";
        }


        function _About() {
                return "SMS Version 0.1 by h.januschka";
        }
        
        function mobile_nr_field($nr) {
        	return "<table width=100%><tr><td>Mobile Nummer:</td><td><input type='text' name='SMSextension_mobile_nr' value='$nr'></td></tr></table>";	
       }
        function _PRE_add_worker() {
		global $layout, $defaults, $btl;
		
			
		return $this->mobile_nr_field("");
		
	}
	function make_user_list() {
		global $layout, $btl, $default, $_GET;
		
		
		if(!is_dir($this->sms_queue)) {
			return "if you install debian package smstools and put the queue to " . $this->sms_queue . " you would be able to send SMS";	
		}
		
		if(!$btl->hasRight("modify_all_workers",false)) {
			return "you are missing 'modify_all_workers' right";
		}
		
		$srvid = $default[service_id];
		$svcid = $default[service_id];
		if($_GET[service_id]) {
			$srvid = $_GET[service_id];
			$svcid = $_GET[service_id];
		}
		
		$servs=$btl->GetWorker();
		$optind=0;
		
		while(list($k, $v) = @each($servs)) {
		
		
			$v1=bartlby_get_worker_by_id($btl->CFG, $v[worker_id]);
		
			$servers[$optind][c]="";
			$servers[$optind][v]=$v1[name];	
			$servers[$optind][k]=$v1[name];
			$optind++;
		}
		$msg = "dear users,\n";
		$msg .= "i am registering/modify a downtime for" . $defaults[server_name] . "\n\n--------------\n\n";
		if($default[downtime_type]==2) {
			//Server
			
			$defaults=bartlby_get_server_by_id($btl->CFG, $srvid);
			$msg .= "Server:" . $defaults[server_name] . "\n";
			
			
		} else {
			//service
			
			$defaults=bartlby_get_service_by_id($btl->CFG, $svcid);
			$msg .= "Service:" . $defaults[server_name] . "/" .  $defaults[service_name] . "\n";
			
			
		}
		$msg .= "Because of: //FILL_IN//\n";
		$msg .= "\n\n--------------\n\n regards, " . $btl->user;
		
		
		$textarea = "<textarea name=SMSEXT_TXT style='width:450px; height:250px;'>$msg</textarea>";
		$dropdown = $layout->dropdown("SMSextension_susers[]", $servers, "multiple", "style=\"height:125;width:125px\"");
		
		$o = "<table width=100%>";
		$o .= "<tr><td align=left valign=top>Send Message to following users:</td><td align=left valign=top>$dropdown</td></tr>";
		$o .= "<tr><td align=left valign=top>Message:</td><td align=left valign=top>$textarea</td></tr>";
		$o .= "</table>";
		return $o;
		
	}
	/*

	*/
	function _PRE_add_downtime() {
		
		return $this->make_user_list();
		
	}
	
	function send_message() {
		global $_GET;
		//for each select user
		// get mobile NR;
		// put sms queue file into smstools queue
		if(!is_dir($this->sms_queue)) {
			return "if you install debian package smstools and put the queue to " . $this->sms_queue . " you would be able to send SMS";	
		}		
		for($x=0; $x<count($_GET[SMSextension_susers]); $x++) {
			$k = $_GET[SMSextension_susers][$x] . "_SMSextension_mobile_nr";
			$nr = $this->storage->load_key($k);
			$fp = @fopen($this->sms_queue . "/tmp." . md5($k), "w");
			if($fp) {
				$sms = "To: " . $nr . "\n\n";
				$sms .= $_GET[SMSEXT_TXT] . "\n";
				fwrite($fp, $sms);
				fclose($fp);
				$r .= "queued for: " . $_GET[SMSextension_susers][$x] . "<br>";
			} else {
				$r .= 	$_GET[SMSextension_susers][$x] . " queue fopen failed<br>";
			}
		}
		return $r;
		
		
	}
	
	function _PRE_modify_downtime() {
		return $this->make_user_list();
	}
	function _POST_add_downtime() {
		return $this->send_message();
	}
	function _POST_modify_downtime() {
		return $this->send_message();
	}
	function _POST_add_worker() {
		global $layout, $_GET, $defaults, $btl;
		$k = $btl->user . "_SMSextension_mobile_nr";
		$this->storage->save_key($k, $_GET["SMSextension_mobile_nr"]);
		return "Mobile number: <b>" . $_GET["SMSextension_mobile_nr"] . "</b> stored";
	}
	function _POST_modify_worker() {
		global $layout, $_GET, $defaults, $btl;
		$k = $_GET[worker_name] . "_SMSextension_mobile_nr";
		$this->storage->save_key($k, $_GET["SMSextension_mobile_nr"]);
		return "Mobile number: <b>" . $_GET["SMSextension_mobile_nr"] . "</b> stored";		
	}
	function _PRE_modify_worker() {
		global $layout, $_GET, $defaults, $btl;
		$k = $defaults[name] . "_SMSextension_mobile_nr";
		$v = $this->storage->load_key($k);
		
		return $this->mobile_nr_field($v);	
		
		
	}
        
        
}

?>
