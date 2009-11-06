<table  width='100%'>
	<tr>
		<td width=150 class='font2'>Number of services:</td>
		<td><?php echo count($plcs[server_map])?></td> 
	</tr>
	<tr>
		<td width=150 class='font2' valign=top>Services: </td>
		<td> <a href='services.php?server_id=<?php echo $_GET[server_id]?>'>All</A>,<?php echo $plcs[services_assigned]?></td> 
	</tr>
	
	
</table>