<?php
	include "config.php";
	
	$e=explode("?", $_GET[script]);
	$s = explode("&" , $e[1]);
	for($x=0; $x<count($s); $x++) {
		$a=explode("=", $s[$x]);
		$_GET[$a[0]]=$a[1];	
	}
	include("extensions/"  . $e[0]);
?>