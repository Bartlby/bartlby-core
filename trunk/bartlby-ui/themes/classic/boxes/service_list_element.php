<?

//$put_a_standard_box_around_me=false;
for($s_element=0; $s_element<count($plcs[services]); $s_element++) {
?>

<table class='service_table' cellpadding=2>
	<tr>
		<td   align='center'  valign=top width='58'  class='<?=$plcs[services][$s_element][color]?>_img'>
				<b><a href='services.php?expect_state=<?=$plcs[services][$s_element][current_state]?>'><font style='font-size:9px;'><?=$plcs[services][$s_element][state_readable]?></A></b>
		</td>
		<td   align=left  valign=top width='80'  class='<?=$plcs[services][$s_element]["class"]?>'>
			<font size=1><?=date("d.m.y H:i:s", $plcs[services][$s_element][last_check])?>
		</td>
		<td   align=left  valign=top width='80'  class='<?=$plcs[services][$s_element]["class"]?>'>
			<font size=1><?=date("d.m.y H:i:s", $plcs[services][$s_element][last_check]+$plcs[services][$s_element][check_interval])?>

		</td>
		<td   align=left  valign=top width='150'  class='<?=$plcs[services][$s_element]["class"]?>'>
			<a href='service_detail.php?service_place=<?=$plcs[services][$s_element][shm_place]?>'><b><?=$plcs[services][$s_element][service_name]?></A>
			<br>
			<?=$btl->getserviceOptions($plcs[services][$s_element], $layout)?>
		</td>
		<td   align=left  valign=top width='450'  class='<?=$plcs[services][$s_element]["class"]?>'>
			<?=str_replace( "\\dbr","<br>", nl2br($plcs[services][$s_element][new_server_text]))?>

		</td>

	</tr>
</table>

<?
}
?>