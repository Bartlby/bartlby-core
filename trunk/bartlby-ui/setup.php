<?
	$dirs = array("pkgs", "extensions", "rights", "comments", "store", "store/SMS");
	$cfgs = array($Bartlby_CONF, "ui-extra.conf");
	$rights = array("rights/admin.dat", "rights/template.dat");
?>
<html>
<head>
</head>
<title>bartlby first run</title>
<body bgcolor="#ffffff">

<h1>This looks like your first run of the bartlby-ui<h1>
	
<h2>Checking permissions</h2>
<pre>
<?
$bad_error_found=false;
for($x=0; $x<count($dirs); $x++) {
	echo $dirs[$x] . ".....";
	if(is_writeable($dirs[$x])) {
		echo "<font color=green>OK</font>";
	} else {
		$bad_error_found=true;
		echo "<font color=red>CRITICAL</font> (make it writeable to the apache user)";	
		
	}
	echo "\n";
}

?>
</pre>

<h2>Checking default disabled UI extensions</h2>
<pre>
<?
$bad_error_found=false;
foreach(glob("extensions/*.disabled") as $fname) {
	echo $fname . ".....";
	
	if(is_writeable($fname)) {
		echo "<font color=green>OK</font>";
	} else {
		$bad_error_found=true;
		echo "<font color=red>CRITICAL (make it writeable to the apache user) or you cannot enable it</font>";	
		
	}
	echo "\n";
}
flush();
?>

<h2>Config Files</h2>
<pre>
<?
for($x=0; $x<count($cfgs); $x++) {
	echo $cfgs[$x] . ".....";
	if(is_writeable($cfgs[$x])) {
		echo "<font color=green>OK</font>";
	} else {
		if(file_exists($cfgs[$x])) {
			echo "<font color=orange>WARNING</font> (make it writeable to the apache user) or you cannot edit it via the ui";	
		} else {
			$bad_error_found=true;
			echo "<font color=red>CRITICAL</font> file not existing (check that this files is existing, modify your config.php)";	
		}
		
	}
	echo "\n";
}

?>

<h2>User permission</h2>
<pre>
<?
for($x=0; $x<count($rights); $x++) {
	echo $rights[$x] . ".....";
	if(is_writeable($rights[$x])) {
		echo "<font color=green>OK</font>";
	} else {
		$bad_error_found=true;
		echo "<font color=red>CRITICAL</font> either the file is not found or not writeable";	
		
	}
	echo "\n";
}

?>

</pre>
<h2>PHP module</h2>
<pre>
<?
if(!function_exists("bartlby_get_info")) {
	echo "bartlby functions not existing .... trying to load module....";
	if(!@dl("bartlby.so")) {
		$bad_error_found=true;
		echo "<font color=red>CRITICAL</font>(have you installed bartlby-php?)";
			
	} else {
		echo "<font color=green>OK</font>";	
	}
	
	echo "\n";
}

?>
<h2>checkin Core</h2>
<pre>
<?
echo "trying to retrieve Version number of core ... ";
if($bad_error_found==false) {
	$info=@bartlby_get_info($Bartlby_CONF);
	if(!$info) {
		$bad_error_found=true;
		echo "<font color=red>CRITICAL</font>(have you started the bartlby-core?)";
	} else {
		
		echo "<font color=green>OK</font> <b>" . $info[version] . "\n</b>";
		echo "Workers: $info[workers]";
		if($info[workers] <= 0) {
			echo " .. <font color=red>CRITICAL</font> no posibility to log in\n";
			$bad_error_found=true;
			
		}
		echo "\nusers: ";
		for($x=0; $x<$info[workers]; $x++) {
			$w = @bartlby_get_worker($Bartlby_CONF, $x);
			echo $w[name] . " ";	
		}
	}
	 
} else {
	echo "<font color=red>CRITICAL</font> some previous checks failed FIX THEM\n";	
}
echo "\n";
?>

<h2>check if setup.php is removeable</h2>
<?
echo "check...";
if($bad_error_found == true) {
	echo "<font color=red>CRITICAL</font> some previous checks failed FIX THEM\n";	
} else {
	
	if(@my_unlink("setup.php")) {
		echo "<font color=green>OK</font> <b>";
	} else {
		$bad_error_found=true;
		echo "<font color=red>CRITICAL</font> cannot be deleted (check permissions, or delete it your self and reload this page)\n";	
	}	
}
echo "\n";
?>
<?
	if($bad_error_found == false) {
		echo "<h1><font color=green>ALL OK</font>(Hit Reload in your browser)</h1>";	
		
	}  else {
		echo "<h1><font color=red>ERRORS OCCURED</font></h1>";		
	}
?>



</pre>



</body>
</html>

<?
function my_unlink($f) {
	global $_GET;
	if(!$_GET[dev]) {
		return unlink($f);	
	}	
	return true;
}
?>