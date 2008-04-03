<?
	$Bartlby_CONF="/opt/bartlby-dev/etc/bartlby.cfg";
	if(file_exists("setup.php")) {
		include("setup.php");
		exit(1);	
	}
	if($do_not_merge_post_get != true) {
		$_GET=array_merge($_GET, $_POST);
	}
?>
