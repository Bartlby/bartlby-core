<?php
/*
	$layout->create_box($info_box_title, $core_content, "core_info", array(
		'user' => $btl->user,
		'time' =>  date("d.m.Y H:i:s"),
		'uptime' =>  $btl->intervall(time()-$btl->info[startup_time]),
		'services' => $info[services],
		'workers' => $info[workers],
		'downtimes' => $info[downtimes],
		'datalib' => $lib[Name],
		'datalib_version' => $lib[Version],
		'running' => $load_bar,
		'round_ms_time' => $rndMS,
		'average_delay' => $avgDEL,
		'release_name' => $btl->getRelease(),
		'reload_state' => $reload_status,
		'sirene'  => $sir

*/
?>
<table class='nopad' width='100%'>
		<tr>
			<td class='font1'>(Logged in as: <font class='font2'><?=$plcs[user]?></font>)</td>
			<td align=right class='font1'>Uptime:<font class='font2'><?=$plcs[uptime]?></font></td>
		</tr>
		<tr>
			<td class='font1'>Services: <font class='font2'><?=$plcs[services]?>&nbsp;&nbsp;&nbsp;&nbsp;Workers: <?=$plcs[workers]?>&nbsp;&nbsp;&nbsp;&nbsp;Downtimes: <?=$plcs[downtimes]?>&nbsp;&nbsp;&nbsp;&nbsp;</td>
			<td align=right class='font1'>Datalib:<font class='font2'><?=$plcs[datalib]?>-<?=$plcs[datalib_version]?></font></td>
		</tr>
		<tr>
			<td class='font1' colspan=1>
			Running: <?=$plcs[running]?>
			</td>
			<td align=right class='font1'>Avg Round Time:<font class='font2'><?=$plcs[round_ms_time]?> ms / avg service delay: <?=$plcs[average_delay]?> sec.</font></td>
		</tr>
		
		<tr>
			<td colspan=2 class='font1'>Version: <font class='font2'><?=$plcs[release_name]?></font></td>
			
		</tr>
		<tr>
			<td colspan=2 class='font1'>Reload: <font class='font2'><?=$plcs[reload_state]?></font></td>
			
		</tr>
		
		
		
		<tr>
			<td class='font1'>&nbsp;</td>
			<td align=right class='font1'><font class='font2'><?=$plcs[sirene]?></font></td>
		</tr>
	</table>
