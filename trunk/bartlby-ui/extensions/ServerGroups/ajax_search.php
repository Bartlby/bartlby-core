<?
chdir("../../");
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$optind=0;
$y=0;


$optind=0;
$dh=opendir("extensions/ServerGroups/data/");
while ($file = readdir ($dh)) { 
   if ($file != "." && $file != "..") { 
   	clearstatcache();
   	$ta = explode(".",$file);
   	$unser = base64_decode($ta[0]);
   	
   	
   	if(preg_match("/" . $_GET[search] . "/i", $unser)) {
     	echo "<a href=\"javascript:setSearch('" . $file . "');\">$unser</a>\n";
     	$y++;
     }
       	
   }
   if($y>20) {
		break 2;	
	} 
}
closedir($dh); 



/*
echo "<a href=\"javascript:setSearch('63');\">test1</a>\n";
*/	
?>
