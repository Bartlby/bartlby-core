
<?
$put_a_standard_box_around_me=false;
//$put_a_standard_box_around_me=false;
for($s_element=0; $s_element<count($plcs[services]); $s_element++) {
	$lbl = "label-default";
	if($plcs[services][$s_element][color] == "green") {
			$lbl = "label-success";
	}

	if($plcs[services][$s_element][color] == "orange") {
			$lbl = "label-warning";
	}
	if($plcs[services][$s_element][color] == "red") {
			$lbl = "label-important";
	}
	
?>


	<tr>
		<td   align=left  valign=top width='1'  class='<?=$plcs[services][$s_element]["class"]?>'>
			<a href='services.php?server_id=<?=$plcs[services][$s_element][server_id]?>'><b><?=$plcs[services][$s_element][server_name]?></A>
			
			
		</td>

		<td   align='center'  valign=top width=1>
						<span class="label <?=$lbl?>"><a href='services.php?expect_state=<?=$plcs[services][$s_element][current_state]?>'><?=$plcs[services][$s_element][state_readable]?></A></span>
					
		</td>
		<td   align=left  valign=top width='80'  class='<?=$plcs[services][$s_element]["class"]?>'>
			<font size=1><?=date("d.m.y H:i:s", $plcs[services][$s_element][last_check])?>
		</td>
		<td   align=left  valign=top width='80'  class='<?=$plcs[services][$s_element]["class"]?>'>
			<font size=1><?=date("d.m.y H:i:s", $plcs[services][$s_element][last_check]+$plcs[services][$s_element][check_interval])?>

		</td>
		<td   align=left  valign=top width='100'  class='<?=$plcs[services][$s_element]["class"]?>'>
			<a href='service_detail.php?service_place=<?=$plcs[services][$s_element][shm_place]?>'><b><?=$plcs[services][$s_element][service_name]?></A>
			
			
		</td>
		<td   align=left  valign=top width='450'  class='<?=$plcs[services][$s_element]["class"]?>'>
			<?=str_replace( "\\dbr","<br>", nl2br($plcs[services][$s_element][new_server_text]))?>
			
		</td>
		<td width=320>
			
			<?=$btl->getserviceOptions($plcs[services][$s_element], $layout)?>
		</td>
	</tr>

<?
}
?>
