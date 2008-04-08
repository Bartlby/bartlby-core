<?

include "config.php";
include_once "bartlbystorage.class.php";

class Basket {
        function Basket() {
                $this->layout = new Layout();
                $this->storage=new bartlbyStorage("Basket");

        }


        function _About() {
                return "Basket Version 0.1 by h.januschka";
        }
  
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
       function loadBasket() {
       	global $btl;
       	
       	$stored = $this->storage->load_key($btl->user . "_favorites");
       	$sto = @unserialize($stored);
       	
       	$r = "<table class='nopad'>
			<tr>                                                                      
				<td class='nav_main' onClick=\"doToggle('BasketFavorites')\"><img id='BasketFavorites_plus' src='images/minus.gif' border=0> Fav. Services</td>                      
			</tr>
			<tr>
				<td class=\"nav_place\">&nbsp;</td>
			</tr>
		</table>
		<table class=\"nopad\" id='BasketFavorites_sub' style='display:block;'>";
		
		
		while(@list($k, $v) = @each($sto[services])) {
			$idx=$btl->findSHMPlace($k);
			$svc = bartlby_get_service($btl->CFG, $idx);
			$beauty_state = $btl->getState($svc[current_state]);
			$label=$svc[server_name] . "/" . $svc[service_name];
			$label=substr($label, 0,20);
			$r .= "<tr><td style='font-size:10px; font-family:tahoma'><a href='service_detail.php?service_place=" . $idx . "'><font size=1>" .  $label . "</A> ($beauty_state) <a href='javascript:void(0)' onClick=\"xajax_ExtensionAjax('Basket', 'DelService', '" . $svc[service_id] . "')\">X</A></td></tr>";	
			
		}

		$r .= "</table>";
       	
       	return $r;	
       }
       function DelService() {
       	global $_GET, $btl;
       	$stored = $this->storage->load_key($btl->user . "_favorites");
       	$sto = @unserialize($stored);
       
       	$re = new XajaxResponse();
       
       	unset($sto[services][$_GET[xajaxargs][2]]);
       	
       	$stored = @serialize($sto);
       	$stored = $this->storage->save_key($btl->user . "_favorites", $stored);
       	
       	
       	$re->addScript("xajax_ExtensionAjax('Basket', 'updateBasket');");
       	
       	$re->addAssign("basket_add_remove", "innerHTML", "<a href='javascript:void(0)' onClick=\"xajax_ExtensionAjax('Basket', 'AddService', '" . $_GET[xajaxargs][2] . "')\">Add to Favorites</A>");
       
       	return $re;	
       }
       function AddService() {
       	global $_GET, $btl;
       	$stored = $this->storage->load_key($btl->user . "_favorites");
       	$sto = @unserialize($stored);
       	
       	
       	$re = new XajaxResponse();
       	if(!@is_array($sto[services])) $sto[services] = array();
       	if(!@array_key_exists($_GET[xajaxargs][2], $sto[services])) {
       		$sto[services][$_GET[xajaxargs][2]] = true;	
       	}
       	
       	$stored = @serialize($sto);
       	$stored = $this->storage->save_key($btl->user . "_favorites", $stored);
       	$re->addScript("xajax_ExtensionAjax('Basket', 'updateBasket');");
       	$re->addAssign("basket_add_remove", "innerHTML", "<a href='javascript:void(0)' onClick=\"xajax_ExtensionAjax('Basket', 'DelService', '" . $_GET[xajaxargs][2] . "')\">Remove from Favorites</A>");
       	
       	return $re;	
       }
       function _serviceDetail() {
       	global $defaults;
       	global $_GET, $btl;
       	
       	$stored = $this->storage->load_key($btl->user . "_favorites");
       	$sto = @unserialize($stored);
       	
       	if(!@array_key_exists($defaults[service_id], $sto[services])) {
       		$r = "<a href='javascript:void(0)' onClick=\"xajax_ExtensionAjax('Basket', 'AddService', '$defaults[service_id]')\">Add to Favorites</A>";
       	} else {
       		$r = "<a href='javascript:void(0)' onClick=\"xajax_ExtensionAjax('Basket', 'DelService', '$defaults[service_id]')\">Remove from Favorites</A>";
       	}
       	
       	$this->_overview();
       	return "<div id='basket_add_remove'>" . $r . "</div>";
       	
       		
       }
       function _logView() {
       	return $this->_overview();	
       }
       function _services() {
       	return $this->_overview();	
       }
       function _serverDetail() {
       	return $this->_overview();	
       }
       function updateBasket() {
       	$re = new xajaxResponse();
       	$content = $this->loadBasket();
       	$re->addAssign("bartlby_basket", "innerHTML", $content);
       	return $re;	
       }
       
       function _overview() {
       	global $layout;
       	$layout->OUT .= "<script>xajax_ExtensionAjax('Basket', 'updateBasket');</script>";
       }
       
}

?>
