<table  width='100%'>
		<form name='pp' action='bartlby_action.php' method=POST>
		<tr>
			<td width=150 class='font2'>Timeout:</td>
			<td align=left ><?=$plcs[service][service_passive_timeout]?></font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Submit manually:</td>
			<td align=left ><?=$plcs[state_dropdown]?><input type='hidden' name='action' value='submit_passive'><input type='hidden' name='service_id' value='<?=$plcs[service][service_id]?>'><input type='text' name='passive_text'><input type=submit value='store'></font></td>
			<td>&nbsp;</td>           
		</tr>
		</form>
	</table>