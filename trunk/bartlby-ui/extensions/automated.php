<?php
//Run UI-Extensions from shell 
//Sample Call:
// php automated.php \
//   	username=hjanuschka \
//   	password=gizmo1 \
//   	script=BnR/do_backup.php \
//   	package_with_comment=Automated_Backup
//   	package_with_plugins=checked \
//   	package_with_perf=checked \
//   	package_with_config=checked 2>&1>/dev/null;
//   	
// Will produce a full backup

chdir("../");
for($x=0; $x<$argc; $x++) {
	$t=explode("=", $argv[$x]);
	$_GET[$t[0]]=$t[1];
}
$_SERVER[PHP_AUTH_USER]=$_GET["username"];
$_SERVER[PHP_AUTH_PW]=$_GET["password"];

include("extensions/"  . $_GET[script]);



?>