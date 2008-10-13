<?
/*
	"service" => $defaults,
											"service_type" => $svc_type,
											"map" => $map,
											"server_enabled" => $server_enabled,
											"currently_running" => $currun,
											"renotify" => $renot_en,
											"escalate" => $escal_en,
											"server_notifications" => $server_noti_enabled,
											"server_enabled" => $server_enabled,
											"service_ms" => $svcMS,
											"service_delay" => $svcDEL,
											"service_enabled" => $serv_en,
											"notify_enabled" => $noti_en,
											"needs_ack" => $needs_ack,
											"color" => $svc_color,
											"state" => $svc_state

*/

?>
<table  width='100%'>
	<tr>
		<td width=150 class='font2'>Server:</td>
		<td align=left ><a href='server_detail.php?server_id=<?=$plcs[service][server_id] ?>'><?=$plcs[service][server_name]?></A> ( IP: <?=gethostbyname($plcs[service][client_ip])?> Port: <?=$plcs[service][client_port]?> )</font> <?=$plcs[server_enabled]?> </td> 
		<td>&nbsp;</td>     
	</tr>
	<tr>
		<td width=150 class='font2'>Name:</td>
		<td align=left ><?=$plcs[service][service_name]?></font></td>  
		<td>&nbsp;</td>         
	</tr>
	<tr>
		<td width=150 class='font2'>ID:</td>
		<td align=left ><?=$plcs[service][service_id]?></font></td>  
		<td>&nbsp;</td>         
	</tr>
	<tr>
		<td width=150 class='font2'>Type:</td>
		<td align=left ><?=$plcs[service_type]?></font></td>  
		<td>&nbsp;</td>         
	</tr>
	<tr>
		<td width=150 class='font2'>Current State:</td>
		<td align=left><font color='<?=$plcs[color]?>'><?=$plcs[state]?></font></td> 
		<td>&nbsp;</td>          
	</tr>
	<tr>
		<td width=150 class='font2' Valign=top>Server Life Indicator:</td>
		<td align=left><?=$btl->resolveDeadMarker($plcs[service][server_dead], $plcs[map])?></td> 
		<td>&nbsp;</td>          
	</tr>
	
	<tr>
		<td width=150 class='font2'>Last Check:</td>
		<td align=left ><?=date("d.m.Y H:i:s", $plcs[service][last_check])?></font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>approx. next Check:</td>
		<td align=left ><?=date("d.m.Y H:i:s", $plcs[service][last_check]+$plcs[service][check_interval])?></font></td>
		<td>&nbsp;</td>           
	</tr>
	
	<tr>
		<td width=150 class='font2'>Check intervall:</td>
		<td align=left ><?=$plcs[service][check_interval_original]?> / <?=$plcs[service][check_interval]?></font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Last Notify Send:</td>
		
		<td align=left ><?=date("d.m.Y H:i:s", $plcs[service][last_notify_send])?></font></td>
		<td>&nbsp;</td>           
	</tr>
		<tr>
		<td width=150 class='font2'>Re-Notification Interval</td>
		
		<td align=left ><?=$plcs[renotify]?></font></td>
		<td>&nbsp;</td>           
	</tr>
		<tr>
		<td width=150 class='font2'>Escalate after:</td>
		
		<td align=left ><?=$plcs[escalate]?></font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Notify Enabled:</td>
		<td align=left ><?=$plcs[notify_enabled]?> <?=$plcs[server_notifications]?></font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Check Enabled:</td>
		<td align=left ><?=$plcs[service_enabled]?></font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2' valign=top>Check Plan:</td>
		<td align=left ><?=$plcs[check_plan]?></td>
		<td>&nbsp;</td>           
	</tr>
	
	<tr>
		<td width=150 class='font2'>Flap count:</td>
		<td align=left ><?=$plcs[service][flap_count]?></font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Flap seconds:</td>
		<td align=left ><?=$plcs[service][flap_seconds]?></font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Ack settings:</td>
		<td align=left ><?=$plcs[needs_ack]?></font></td>
		<td>&nbsp;</td>           
	</tr>

	<tr>
		<td width=150 class='font2'>Status:</td>
		<td align=left ><?=$plcs[service][service_retain_current]?> / <?=$plcs[service][service_retain]?></font></td>
		<td>&nbsp;</td>           
	</tr>	
	
		<tr>
		<td width=150 class='font2'>Is Running?:</td>
		<td align=left ><?= $plcs[currently_running]?></font></td>
		<td>&nbsp;</td>           
	</tr>	
	<tr>
		<td width=150 class='font2'>Average Check Time:</td>
		<td align=left ><?= $plcs[service_ms]?> ms</font></td>
		<td>&nbsp;</td>           
	</tr>
	<tr>
		<td width=150 class='font2'>Average delay Time:</td>
		<td align=left ><?= $plcs[service_delay]?> Seconds</font></td>
		<td>&nbsp;</td>           
	</tr>
	
</table>