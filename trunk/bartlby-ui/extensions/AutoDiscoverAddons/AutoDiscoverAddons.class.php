<?php

include "config.php";

class AutoDiscoverAddons {
        function AutoDiscoverAddons() {
                $this->layout = new Layout();

        }


        function _About() {
                return "AutoDiscoverAddons Version 0.1 by h.januschka";
        }
        function _permissions() {
        	global $worker_rights;
        	$checked="";
        	if($worker_rights[ada_allowed][0] && $worker_rights[ada_allowed][0] != "false") {
        		$checked="checked";
        	}
        	
        	$r = "<input type=checkbox name='ada_allowed' $checked>allowed<br>";
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
        /*
        function _serverDetail() {
                return "";
        }
        */
        function xajax_update() {
        	global $_GET, $btl;
        	$re = new XajaxResponse();
        	$rrd_dir=bartlby_config($btl->CFG, "performance_rrd_htdocs");
        	$svc_counter=bartlby_config("ui-extra.conf", "special_addon_ui_" . $svcid . "_cnt");
		if(!$svc_counter) {
			$r = $this->_globExt($_GET[xajaxargs][2], $rrd_dir);
			$re->addAssign("autodiscoveraddons_layer", "innerHTML", $r);
			$re->addAssign("AutoDiscoverAddonsHide", "style.display", "none");
		}
        	
        	
        	return $re;	
       }
       function getJavascripts() {
       	global $defaults;
       	$r = "<script language='JavaScript'>
        	  	
        	  	function updatePerfhandlerExt(cnt) {
        	  		as = document.getElementById('AutoDiscoverAddonsHide');
        	  		if(as.style.display == 'block') {
        	  			return;
        	  		}
				as.style.display = 'block';
				xajax_updatePerfHandler(\"xajax_ExtensionAjax('AutoDiscoverAddons', 'xajax_update', '" . $defaults[service_id] . "')\", '" . $defaults[server_id] . "','" . $defaults[service_id] . "');
        	  		
        	  	}
        	  	
        	  	
        	  </script>
        	  <div id=AutoDiscoverAddonsHide style='display:none'><font color='red'><img src='extensions/AutoDiscoverAddons/ajax-loader.gif'> reload in progress....</font></div><a href='javascript:updatePerfhandlerExt();'>Update Perfhandler data</A><br>
        	  <div id='autodiscoveraddons_layer'>";
        	  return $r;
       }
       function endScripts() {
       	
       	$r = "</div>";
       	
       	
       	return $r;
       }
        function _globExt($svcid, $path) {
        	  global $defaults, $xajax;
        	  $x = 0;        	  
                foreach(glob($path . "/" . $svcid . "_*.png") as $fn) {
                        $r .= "<img onClick='updatePerfhandlerExt();' id='perfh" . $x . "' src='rrd/" . basename($fn) . "?" . time() . "'><br>";
                        $x++;
                }
               return $r;
        }

        function _serviceDetail() {
                global $defaults, $btl;
                if($btl->hasRight("ada_allowed", false)) {
                	$rrd_dir=bartlby_config($btl->CFG, "performance_rrd_htdocs");
                	if($rrd_dir) {
                     	   $svcid=$defaults[service_id];
                        	//see if someone has hardcoded some special_addon_stuff in ui config
                        	$svc_counter=bartlby_config("ui-extra.conf", "special_addon_ui_" . $svcid . "_cnt");
                        	if(!$svc_counter) {
                        		    $re = $this->getJavascripts();
                        		    $re .= $this->_globExt($svcid, $rrd_dir);
                        		    $re .= $this->endScripts();
                            	    return $re;
                        	}


	                } else {
       	                 return "";
              	  }
		}


        }
}

?>
