<?php

include "config.php";

class PluginArgumentQuickLook {
        function PluginArgumentQuickLook() {
                $this->layout = new Layout();

        }


        function _About() {
                return "lets you search the plugin arguments via quick look Version 0.1 by h.januschka";
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
        /*
        function _serverDetail() {
                return "";
        }
        */
	function _quickLook() {
		global $_GET, $rq, $btl;
		
		$servers=$_GET["servers"];
		
		$rq .= "<tr>";
		$rq .= "<td colspan=2>";
		$rq .= "<center><b>Plugin Argument</b></center>";
		$rq .= "</td></tr>";
		
		
		$svcfound=false;
		@reset($servers);
		
		while(list($k, $v) = @each($servers)) {
			for($x=0; $x<count($v); $x++) {
				
				if(@preg_match("/" . $_GET[search] . "/i", $v[$x][plugin_arguments])) {
				
				
					$rq .= "<tr><td><a href='service_detail.php?service_place=" . $v[$x][shm_place] . "'><font size=1>" . $v[$x][server_name] . "/" . $v[$x][service_name] . "</A></font></td><td>" . $btl->getServiceOptions($v[$x], $layout) . "</td>";	
					$svcfound=true;
				}
			}
		}
		
		@reset($servers);	
		if($svcfound == false) {
			$rq .= "<tr><td colspan=2><i>no argument line matched</i></td></tr>";
		}
		$_GET[rq] = $rq; //damn return to extensCaller
		return "";
	}

        
}

?>
