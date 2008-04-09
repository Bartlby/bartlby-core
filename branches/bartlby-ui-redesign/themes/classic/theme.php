<?
/* $Id: ack.c 16 2008-04-07 19:20:34Z hjanuschka $ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2008 Helmut Januschka - All Rights Reserved
 *   Contact: <helmut@januschka.com>, <contact@bartlby.org>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 *   visit: www.bartlby.org for support
 * ----------------------------------------------------------------------- */
/*
$Revision: 16 $
$HeadURL: http://bartlby.svn.sourceforge.net/svnroot/bartlby/trunk/bartlby-core/src/ack.c $
$Date: 2008-04-07 21:20:34 +0200 (Mo, 07 Apr 2008) $
$Author: hjanuschka $ 
*/
?>
<html>
<head>
<title>bartlby</title>
<link rel="shortcut icon" href="favicon.ico" > 
<link rel="stylesheet" id="CSS" type="text/css" href="themes/<?=$this->theme?>/btl.css"></link>
<script type="text/javascript" src="js/btl.js"></script>
<script>
	js_theme_name="classic";
</script>
<?=$this->XAJAX?>


<script>
	
	

</script>
</head>

<body leftmargin=0 topmargin=0>
	<table width="100%" class="nopad">
		<tr>
			<td  class="bg_header"><a href='overview.php'><img border=0 src='themes/<?=$this->theme?>/images/btl-logo.gif'></A>
			<div class="reload" id="reload"><input type=button style='width:155px' name="reload_button" value="reload" onClick="document.location.href='bartlby_action.php?action=reload';"></div>
			<div class="ui_performance" id="reload"><font size=1>UI-Version: <font size=1><?=$this->UIVERSION?></font> Page Render:<?=$this->BTUITIME?> secs &nbsp;&nbsp;&nbsp;&nbsp;<?=$this->SERVERTIME?> &nbsp;&nbsp;&nbsp;&nbsp;</div>
			<div style='position: absolute; left:1050Px;top:80px;'><input type=button value=Logout  style='width:50px; font-size:10px; padding-top:0px; height:18px' onClick='jsLogout()'></div>
			<div class="quick_look" id="quick_look" style="z-index:100"><font size=1>Auto Reload<input type='checkbox' checked onClick='toggleReload()' style='height:10px'>   Quick look<input onkeyup="buffer_suggest.modified('qlook', 'xajax_QuickLook');" id=qlook autocomplete='off' type=text name="qlook" style="border:solid black 1px;font-size:10px; height:17px"><div id='quick_suggest'></div></div>
						
			</td>
			
		</tr>
		<tr>
			<td class="bg_body">
				<table class="nopad">
					<tr>
						<td class="navi">
							
							
							<?=$this->BTLEXTMENU?>
							
							
							
							
							<table class="nopad">
								<tr>
									<td><a target='_blank' href='http://wiki.bartlby.org/dokuwiki/doku.php'><img border=0 src='themes/<?=$this->theme?>/images/bartlby1-first-aid.gif'></A></td>
								</tr>
								<tr>
									<td><a href="http://www.spreadfirefox.com/?q=affiliates&amp;id=0&amp;t=85"><img border="0" alt="Get Firefox!" title="Get Firefox!" src="http://sfx-images.mozilla.org/affiliates/Buttons/80x15/firefox_80x15.png"/></a></td>
								</tr>
								
							</table>
						</td>
						<td class="content">
							<table class="nopad">
								<tr>
									<td><?=$this->BTUIOUTSIDE?>
										<center>
										<table>
											<tr>
												<td>
													
													<font style='font-family:tahoma; font-size:8pt; color:#ffffff'><a href='http://www.bartlby.org/' target='_blank'>bartlby</A> is a GPLv2 product of <a href='http://www.januschka.com/' target='_blank'>januschka.com</A></font>
												</td>
											</tr>
										</table>
										</center>									
									</td>
								</tr>
							</table>
						</td>
					</tr>
				</table>			
			</td>
			
		</tr>
	</table>




<div id='bartlby_basket' style="position:absolute; left: 1010px; top:110px;"></div>



</body>
</html>
