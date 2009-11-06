<?php

//$put_a_standard_box_around_me=false;
for($s_element=0; $s_element<count($plcs[services]); $s_element++) {
?>

<table class='service_table' cellpadding=2>
	<tr>
		<td   align='center'  valign=top width='58'  class='<?php echo $plcs[services][$s_element][color]?>_img'>
				<b><a href='services.php?expect_state=<?php echo $plcs[services][$s_element][current_state]?>'><font style='font-size:9px;'><?php echo $plcs[services][$s_element][state_readable]?></A></b>
		</td>
		<td   align=left  valign=top width='80'  class='<?php echo $plcs[services][$s_element]["class"]?>'>
			<font size=1><?php echo date("d.m.y H:i:s", $plcs[services][$s_element][last_check])?>
		</td>
		<td   align=left  valign=top width='80'  class='<?php echo $plcs[services][$s_element]["class"]?>'>
			<font size=1><?php echo date("d.m.y H:i:s", $plcs[services][$s_element][last_check]+$plcs[services][$s_element][check_interval])?>

		</td>
		<td   align=left  valign=top width='150'  class='<?php echo $plcs[services][$s_element]["class"]?>'>
			<a href='service_detail.php?service_place=<?php echo $plcs[services][$s_element][shm_place]?>'><b><?php echo $plcs[services][$s_element][service_name]?></A>
			<br>
			<?php echo $btl->getserviceOptions($plcs[services][$s_element], $layout)?>
		</td>
		<td   align=left  valign=top width='450'  class='<?php echo $plcs[services][$s_element]["class"]?>'>
			<?php echo str_replace( "\\dbr","<br>", nl2br($plcs[services][$s_element][new_server_text]))?>

		</td>

	</tr>
</table>

<?php
}
?>