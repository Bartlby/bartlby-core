<?php
	include "config.php";

        include "layout.class.php";
        include "bartlby-ui.class.php";

        $btl=new BartlbyUi($Bartlby_CONF, false, false);
        $layout= new Layout();

?>
<body bgcolor="C2CBCF">
<center><img src='themes/<?=$layout->theme?>/images/btl-logo.gif'></center>
<?php
	$dhl=opendir("themes/" . $layout->theme . "/images/");
	while($f = readdir($dhl)) {
		if($f == "." || $f == ".." || is_dir("themes/" . $layout->theme . "/images/" . $f) || !preg_match("/.*\.[png|gif|jpg]/", $f)) {
			continue;	
		}	
		$str .= "'themes/" .  $layout->theme . "/images/" . $f . "',";
	}
	
	closedir($dhl);
	$str{strlen($str)-1} = ' ';
	
	$https_check="";
	if($_SERVER[SERVER_PROTOCOL] == "HTTP/1.1") {
		$https_check = "<font color=red>You are using a unsecure http connection!!!</font>";	
	}
	
?>

<link rel="shortcut icon" href="favicon.ico" > 
<link rel="stylesheet" id="CSS" type="text/css" href="themes/<?=$layout->theme?>/btl.css"></link>
<script type="text/javascript" src="js/btl.js"></script>

<script language="JavaScript">
<!-- begin hiding

/*
Preload Image With Update Bar Script (By Marcin Wojtowicz [one_spook@hotmail.com])
Submitted to and permission granted to Dynamicdrive.com to feature script in it's archive
For full source code to this script and 100's more, visit http://dynamicdrive.com
*/

// You may modify the following:
	var locationAfterPreload = "overview.php" // URL of the page after preload finishes
	var lengthOfPreloadBar = 200 // Length of preload bar (in pixels)
	var heightOfPreloadBar = 20 // Height of preload bar (in pixels)
	// Put the URLs of images that you want to preload below (as many as you want)
	var yourImages = new Array(<?=$str?>)

// Do not modify anything beyond this point!
if (document.images) {
	var dots = new Array() 
	dots[0] = new Image(1,1)
	dots[0].src = "themes/<?=$layout->theme?>/images/black.gif" // default preloadbar color (note: You can substitute it with your image, but it has to be 1x1 size)
	dots[1] = new Image(1,1)
	dots[1].src = "themes/<?=$layout->theme?>/images/blue.gif" // color of bar as preloading progresses (same note as above)
	var preImages = new Array(),coverage = Math.floor(lengthOfPreloadBar/yourImages.length),currCount = 0
	var loaded = new Array(),i,covered,timerID
	var leftOverWidth = lengthOfPreloadBar%coverage
}
function loadImages() { 
	for (i = 0; i < yourImages.length; i++) { 
		preImages[i] = new Image()
		preImages[i].src = yourImages[i]
	}
	for (i = 0; i < preImages.length; i++) { 
		loaded[i] = false
	}
	checkLoad()
}
function showLoginMask() {
		
}
function checkLoad() {
	if (currCount == preImages.length-4) { 
		
		showLoginMask();
		return
	}
	for (i = 0; i <= preImages.length; i++) {
		if (loaded[i] == false && preImages[i].complete) {
			loaded[i] = true
			eval("document.img" + currCount + ".src=dots[1].src")
			currCount++
		}
	}
	timerID = setTimeout("checkLoad()",10) 
}
window.setTimeout("showLoginMask()", 2000);
// end hiding -->

</script>

</head>



<center>
<font size="2" face="tahoma">Please be patient while some images<br>
are being preloaded...</font><p>
<script language="JavaScript1.1">
<!-- begin hiding
// It is recommended that you put a link to the target URL just in case if the visitor wants to skip preloading
// for some reason, or his browser doesn't support JavaScript image object.
if (document.images) {
	
	var preloadBar = ''
	for (i = 0; i < yourImages.length-1; i++) {
		preloadBar += '<img src="' + dots[0].src + '" width="' + coverage + '" height="' + heightOfPreloadBar + '" name="img' + i + '" align="absmiddle">'
	}
	preloadBar += '<img src="' + dots[0].src + '" width="' + (leftOverWidth+coverage) + '" height="' + heightOfPreloadBar + '" name="img' + (yourImages.length-1) + '" align="absmiddle">'
	document.write(preloadBar)
	loadImages()
}
document.write('<font face="tahoma" size=1><p><small>&nbsp;</p>')
// end hiding -->
</script>


			<table class="nopad">
				<tr>
					<td class="box_left_corner_top">&nbsp;</td>
					<td class="box_top_run">Login</td>
					<td class="box_right_corner_top">&nbsp;</td>
				</tr>
				<tr>
					<td class="box_left_run">&nbsp;</td>
					<td align=center class="box_content"><form method="POST" action="login.php">
						<table width=100%>
							<tr>
								<td>Username:</td>
								<td><input type=text name=login_name></td>
							</tr>
							<tr>
								<td>Password:</td>
								<td><input type=password name=login_password></td>
							</tr>
							<tr>
								<td colspan=2 align=center><input type=submit value="login"><br><?=$https_check?></td>
							</tr>
							<tr>
								<td colspan=2 align=center><font size=1>to use browser based auth. leave the fields blank and click login</font></td>
							</tr>
						</table>
						
						</Form></td>
					<td class="box_right_run">&nbsp;</td>
				</tr>
				<tr>
					<td class="box_left_corner_bottom">&nbsp;</td>
					<td class="box_bottom_run">&nbsp;</td>
					<td class="box_right_corner_bottom">&nbsp;</td>
				</tr>
			</table>
			
			<center>
										<table>
											<tr>
												<td>
													
													<font style='font-family:tahoma; font-size:8pt; color:#ffffff'><a href='http://www.bartlby.org/' target='_blank'>bartlby</A> is a GPLv2 product of <a href='http://www.januschka.com/' target='_blank'>januschka.com</A></font>
												</td>
											</tr>
										</table>
										</center>
										
