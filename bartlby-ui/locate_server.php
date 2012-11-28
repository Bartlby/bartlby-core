<?php
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	$btl=new BartlbyUi($Bartlby_CONF);
	$info=$btl->getInfo();
	$layout= new Layout();
	$layout->Form("fm1", "locate_server.php");
	$layout->setTemplate("nonav.html");
	$layout->OUT .= "<script>
		function applyIP(ip, port) {
			window.opener.document.forms[\"fm1\"].server_ip.value=ip;
			window.opener.document.forms[\"fm1\"].server_port.value=port;
			window.close();
			
		}
	</script>
	";
	$layout->Table("100%");
	//Get available ip addresses
	if(!$_GET[scanip][0]) {
		$ips=Array();
		$bcasts=Array();
		$mask=Array();
		
		$layout->Tr(
				$layout->Td(
					array(
						0=>array(
							"show" => "Ip/Ranges to scan",
							"colspan" => 2,
							"class" => "header"
							)
						
						
						
					)
				)
			);	
		
		
		
		$fp=popen("ifconfig|grep 'inet addr'", "r");
		while(!feof($fp)) {
			$str=fread($fp, 1024);
			$e1=explode(":",$str);
			
			for($x=0; $x<count($e1); $x++) {
				if(preg_match("/(.*) Bcast/i", $e1[$x],$matches)) {
					array_push($ips, trim($matches[1]));
				}	
				if(preg_match("/(.*) Mask/i", $e1[$x],$matches)) {
					array_push($bcasts, trim($matches[1]));
					array_push($mask, trim($e1[$x+1]));
				}	
			}
			//echo $e1[1];
		}
		pclose($fp);
		for($x=0; $x<count($ips);$x++) {
			$layout->Tr(
				$layout->Td(
					array(
						0=>array(
							"show" => $ips[$x],
							"class" => "header1"
							),
						1=>"<input type='checkbox' checked name='scanip[]' value='" . $ips[$x] ."'>"
						
						
					)
				)
			);	
		}
		$layout->Tr(
				$layout->Td(
					array(
						0=>array(
							"show" => "127.0.0.1",
							"class" => "header1"
							),
						1=>"<input type='checkbox' checked name='scanip[]' value='127.0.0.1'>"
						
						
					)
				)
			);	
		$layout->Tr(
				$layout->Td(
					array(
						0=>array(
							"show" => $layout->Field("submit", "submit", "scan->"),
							"colspan" => 2,
							"class" => "header"
							)
						
						
						
					)
				)
			);
	} else {
			$layout->Tr(
				$layout->Td(
					array(
						0=>array(
							"show" => "Found Agents",
							"colspan" => 2,
							"class" => "header"
							)
						
						
						
					)
				)
			);	
		for($x=0; $x<count($_GET[scanip]); $x++) {
			
				if($_GET[scanip][$x] != "1127.0.0.1") {
					$results=scanPorts($_GET[scanip][$x]);
					for($y=0; $y<count($results); $y++) {
							$layout->Tr(
								$layout->Td(
									array(
										0=>array(
											"show" => "<a href=\"javascript:applyIP('" . $results[$y][ip] . "','" . $results[$y][port] . "');\">" . $results[$y][ip] . ":" . $results[$y][port] . "</A>",
											"colspan" => 2,
											"class" => "header1"
											)
										
										
										
									)
								)
							);	
					}
				} 
		}
	}		
	
	$layout->TableEnd();
	$layout->FormEnd();
	$layout->display("no");
	
	
function scanRange($ip) {
	$ret=array();
	$bret=array();
	$e=explode(".", $ip);
	for($z=0; $z<count($e)-1; $z++) {
		$zero_ip .=$e[$z] . ".";	
	}	
	
	for($x=0; $x<=50; $x++) {
		$ret=scanPorts($zero_ip . $x);
		array_push($bret, $ret);
	}
	return $ret;
}
	
function scanPorts($ip) {
	$ret = array();
	$fp=@fsockopen($ip, 9030, $errno, $errstr, 1);
	if($fp) {
		$ver=fgets($fp, 1024);
		if(strncmp($ver, "OS: ", 4) == 0) {
			array_push($ret, array("ip"=>$ip, "port"=>9030));
		}
	}
		
	return $ret;
}
?>