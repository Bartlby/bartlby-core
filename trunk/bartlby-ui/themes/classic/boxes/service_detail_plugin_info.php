<table  width='100%'>
		<tr>
			<td width=150 class='font2'>Plugin:</td>
			<td align=left ><?php echo htmlspecialchars($plcs[service][plugin])?></font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Plugin Arguments:</td>
			<td align=left ><?php echo htmlspecialchars($plcs[service][plugin_arguments])?></font></td>
			<td>&nbsp;</td>           
		</tr>
		<tr>
			<td width=150 class='font2'>Plugin Timeout:</td>
			<td align=left ><?php echo $plcs[service][service_check_timeout]?> Seconds</font></td>
			<td>&nbsp;</td>           
		</tr>
		
	</table>