<?
error_reporting(0);
//php bartlby_portier_spool_bulk.php  -i localhost -p 9031 -d /tmp/btl_cmds/
$shortopts  = "";
$shortopts .= "i:";  // Required value
$shortopts .= "p:"; // Optional value
$shortopts .= "d:"; // Optional value
$shortopts .= "b"; // These options do not accept values

$longopts  = array();
$options = getopt($shortopts, $longopts);

if(!$options[i] || !$options[p]  || !$options[d]) {
	echo "Specify -i IP_ADDRESS -p port of portier instance -d SPOOL_DIR\n";
	exit(2);
}

$dh = opendir($options[d]);
if(!$dh) {
		echo "could not opendir() " . $options[d] . "\n";
		exit(2);
}

while($f = readdir($dh)) {
	if($f == "." || $f == "..") continue;
	if(!is_file($options[d] . "/" . $f)) continue;
	$files[filemtime($options[d] . "/" . $f) . "-" . $f]=$options[d] . "/" . $f;
}

//Connect
$sock = fsockopen($options[i], $options[p]);
if(!$sock) {
	echo "Connect to Portier failed\n";
}

$svcs_uniq=array();
krsort($files);
while(list($k, $v) = @each($files)) {
	echo "DOING: $v \n";	
	$cnt = file_get_contents($v);
	$parsed = json_decode($cnt);
	if($parsed->method == "orch_service_status") {
		//Only send last one
		if($svcs_uniq[$parsed->service_id] == 1) {
			unlink($v);
			continue;
		}
		$svcs_uniq[$parsed->service_id]=1;

	}
	
	fwrite($sock, $cnt . "\n");
	$ret = fgets($sock, 1024);
	$ret_parsed = json_decode($ret);
	if($ret_parsed->error_code != 0) {
		echo "FAIL: " . $ret . "\n";
	} else {
		unlink($v);
	}





}


fclose($sock);

?>
