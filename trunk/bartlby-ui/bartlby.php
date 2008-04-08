<?

$cfgfile="/storage/SF.NET/BARTLBY/bartlby/bartlby.cfg";
//$btl=bartlby_open();
//$info=bartlby_get_info($btl);
//var_dump($info);
//var_dump(bartlby_get_service($cfgfile, 1));
//var_dump(bartlby_get_info($cfgfile, 1));

echo "<pre>";
$info=@bartlby_get_info($cfgfile, 1);
if(!$info) {
	echo "NOT RUNN<br>";
	exit;	
}
for($x=0; $x<$info[services]; $x++) {
	$svc=bartlby_get_service($cfgfile, $x);
	var_dump($svc);
}
for($x=0; $x<$info[workers];$x++) {
	$wrk=bartlby_get_worker($cfgfile, $x);
	var_dump($wrk);
}

?>
