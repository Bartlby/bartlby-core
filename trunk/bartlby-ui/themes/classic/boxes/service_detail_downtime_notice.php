<table  width='100%'>
		<tr>
			<td width=150 class='font2'>From:</td>
			<td><?php echo date("d.m.Y H:i", $plcs[service][downtime_from])?></td> 
		</tr>
		<tr>
			<td width=150 class='font2'>To:</td>
			<td><?php echo date("d.m.Y H:i", $plcs[service][downtime_from])?></td> 
		</tr>
		<tr>
			<td width=150 class='font2'>Left:</td>
			<td><?php echo $btl->Intervall($plcs[service][downtime_to]-time())?></td> 
		</tr>
		<tr>
			<td width=150 class='font2'>Notice/Reason:</td>
			<td><?php echo $plcs[service][downtime_notice]?></td> 
		</tr>
		
	</table>