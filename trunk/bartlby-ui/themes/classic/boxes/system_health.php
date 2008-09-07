<?
/*
$layout->create_box($health_title, $health_content,"system_health", array(
			'prozent_float' => $prozent_float,
			'color' => $color
		), "system_health");
*/
?>
<?
$silverbar = "<table class='nopad'>
		<tr>
			<td class='bar_left_silver'>&nbsp;</td>
			<td class='bar_middle_silver' style='width:" . 100*7.3 . "'></td>
			<td class='bar_right_silver'>&nbsp;</td>
			<td class='font2'>" . $plcs[prozent_float] . "% OK</td>
			
		</tr>
		
	</table>";
?>
	
<div style='position:relative; z-index:2; '> <table class='nopad'>
		<tr>
			<td  class='bar_left_<?=$plcs[color]?>'>&nbsp;</td>
			<td class='bar_middle_<?=$plcs[color]?>' style='width:<?=($plcs[prozent_float]*7.3)?>'></td>
			<td class='bar_right_<?=$plcs[color]?>'>&nbsp;</td>
			<td class='font2'>&nbsp;</td>
			
		</tr>
		
	</table></div><div style='position:relative; z-index:1; top:-40px;'><?=$silverbar?></div>
