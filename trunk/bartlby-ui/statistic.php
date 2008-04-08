<?
set_time_limit(0);

function dnl($i) {
	return sprintf("%02d", $i);
}
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("core.process_info");

if($_GET[maxn]) {
	$maxn=$_GET[maxn];	
} else {
	$maxn=10;	
}
if($_GET[sortorder]) {
	$sorto=$_GET[sortorder];	
} else {
	$sorto="desc";	
}

if($sorto == "asc") {
	$ascc="checked";	
} else {
	$descc="checked";	
}


$layout= new Layout();
$layout->set_menu("core");
$layout->setTitle("Bartlby Core Performance");
$layout->Table("100%");

//Check if profiling is enabled
	$map = $btl->GetSVCMap();
	$info = $btl->getInfo();
	$check_max=0;
	$check_avg=0;
	$check_count=0;
	$check_sum=0;
	$check_plg_max="";
	
	$round_max=0;
	$round_avg=0;
	$round_count=0;
	$round_sum=0;
	
	
	while(list($k, $servs) = @each($map)) {
		for($x=0; $x<count($servs); $x++) {
			
			$check_sum +=$servs[$x][service_time_sum];
			$check_count +=$servs[$x][service_time_count];
			
			$delay_sum += $servs[$x][service_delay_sum];
			$delay_count += $servs[$x][service_delay_count];
			
			$plugin=$servs[$x][plugin];
			if($servs[$x][service_time_count] > 0) {
				$ms=@round($servs[$x][service_time_sum] / $servs[$x][service_time_count], 2);
			} else {
				$ms=0;	
			}
			
			if($servs[$x][service_delay_count] > 0) {
				$delay_ms=@round($servs[$x][service_delay_sum] / $servs[$x][service_delay_count], 2);
			} else {
				$delay_ms=0;	
			}
			
			$service="<img src='server_icons/" . $servs[$x][server_icon] . "'><a href='service_detail.php?service_place=" . $servs[$x][shm_place] . "'>" .  $servs[$x][server_name] . "/" . $servs[$x][service_name] . "(" .   $servs[$x][plugin] . ")</A>";
			$server="<img src='server_icons/" . $servs[$x][server_icon] . "'><a href='server_detail.php?server_id=" . $servs[$x][server_id] . "'>" . $servs[$x][server_name] . "</A>";
			
			if(!is_array($plugin_table[$plugin])) {
					$plugin_table[$plugin]=Array();
			}
			array_push($plugin_table[$plugin], $ms);
			
			//Service Table
			if(!is_array($service_table[$service])) {
				$service_table[$service]=Array();
			}
			array_push($service_table[$service], $ms);
			
			//Server Table
			if(!is_array($server_table[$server])) {
				$server_table[$server]=Array();
			}
			array_push($server_table[$server], $ms);
			
			//Delay Servcie Table
			if(!is_array($delay_service_table[$service])) {
				$delay_service_table[$service]=Array();
			}
			array_push($delay_service_table[$service], $delay_ms*1000);
			
			//Delay Table
			if(!is_array($delay_server_table[$server])) {
				$delay_server_table[$server]=Array();
			}
			array_push($delay_server_table[$server], $delay_ms*1000);
		}	
	}
	$round_sum=$info[round_time_sum];
	$round_count=$info[round_time_count];
	
	
	
	
	$check_avg=round($check_sum / $check_count,2);
	$round_avg=round($round_sum / $round_count,2);
	$delay_avg=round(($delay_sum / $delay_count)*1000, 2);
	
	
	
	//Make top 10 table plugins
	//Table K1 == AVG == K2 VALUE == K3 MAX
	$delay_service_sorted=sort_table($delay_service_table);
	$delay_server_sorted=sort_table($delay_server_table);
	$plugins_sorted=sort_table($plugin_table);
	$server_sorted=sort_table($server_table);
	$service_sorted=sort_table($service_table);
	$plugin_html=make_html($plugins_sorted);
	$service_html=make_html($service_sorted);
	$server_html=make_html($server_sorted);
	$delay_service_html=make_html($delay_service_sorted);
	$delay_server_html=make_html($delay_server_sorted);
	
	
	$info_box_title="Options:";  
	$core_content = "<table  width='100%'>
	
		<tr>
			<td width=150 valign=top class='font2'>Max Num:</td>
			<td><form name='f' action=statistic.php method=POST><input type=text name='maxn' value='$maxn'><input type='submit' value='Update..'><br>
			<input type=radio name='sortorder' value='asc' $ascc >Ascending <input type=radio name='sortorder' value='desc' $descc >Descending
			</form></td>
		</tr>
		
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	
	
	
	$info_box_title="Check Time:";  
	$core_content = "<table  width='100%'>
	
		<tr>
			<td width=150 valign=top class='font2'>Average:</td>
			<td>$check_avg ms</td>
		</tr>
		
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	
	$info_box_title="Round Time:";  
	$core_content = "<table  width='100%'>
		
		<tr>
			<td width=150 valign=top class='font2'>Average:</td>
			<td>$round_avg ms</td>
		</tr>
		
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	
	$info_box_title="Delay Time:";  
	$core_content = "<table  width='100%'>
		
		<tr>
			<td width=150 valign=top class='font2'>Average:</td>
			<td>$delay_avg ms</td>
		</tr>
		
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	
	$info_box_title="Plugins:";  
	$core_content = "<table  width='100%'>
		<tr>
			<td valign=top width=150 valign=top class='font2'>Slowest:</td>
			<td>$check_plg_max</td>
		</tr>
		<tr>
			<td width=150 valign=top class='font2'>Average slowest $maxn:</td>
			<td>
				
					$plugin_html
				
			</td>
		</tr>
		
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	
	
	$info_box_title="Services:";  
	$core_content = "<table  width='100%'>
		
		<tr>
			<td width=150 valign=top class='font2'>Average slowest $maxn:</td>
			<td>
				
					$service_html
				
			</td>
		</tr>
		
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	
	$info_box_title="Delay:";  
	$core_content = "<table  width='100%'>
		
		<tr>
			<td width=150 valign=top class='font2'>Delay:</td>
			<td>
				
					$delay_service_html
				
			</td>
		</tr>
		
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	
	
	
	
	
	
	$info_box_title="Servers:";  
	$core_content = "<table  width='100%'>
		
		<tr>
			<td width=150 valign=top class='font2'>Average slowest $maxn:</td>
			<td>
				
					$server_html
				
			</td>
		</tr>
		
		
	</table>";
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	$info_box_title="Delay:";  
	$core_content = "<table  width='100%'>
		
		<tr>
			<td width=150 valign=top class='font2'>Delay:</td>
			<td>
				
					$delay_server_html
				
			</td>
		</tr>
		
		
	</table>";
	
	$layout->push_outside($layout->create_box($info_box_title, $core_content));
	
	
	

	$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 1,
					'show'=>"Slowest plugin:"
					),
				1=>$check_plg_max
			)
		)

	);
	
	
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					'show'=>"KBytes looked at:"  . round($byte_count/1024,2)
					)
			)
		)

	);	

$r=$btl->getExtensionsReturn("_processInfo", $layout);
$layout->TableEnd();

$layout->display();

function sort_table($plugin_table) {
	global $sorto;
	
	while(list($k, $v) = @each($plugin_table)) {
		
		$max=0;
		$sum=0;
		$cnt=0;
		for($x=0; $x<count($v); $x++) {
			if($v[$x] > $max) {
				$max=$v[$x];	
			}
			$sum += $v[$x];
			$cnt++;
		}
		$avg=round($sum/$cnt, 2);
		
		$plugins_sortable[$avg][$k][$max] = 1;
	}
	
	if($sorto == "asc") {
		@ksort($plugins_sortable);
	} else {
		@krsort($plugins_sortable);	
	}
	return $plugins_sortable;
	
}

function make_html($info=array()) {
	global $maxn;
	$have=0;
	$out = "<table >";
	$out .= "<tr>";
	$out .= "<td class='font2'>&nbsp;</td>";	
	$out .= "<td class='font2'>Average</td>";
	
	$out .= "</tr>";
	while(list($average, $d) = each( $info )) {
		while(list($plugin, $d1) = each($d)) {
			while(list($max, $d2) = each($d1)) {
				$out .= "<tr>";
				$out .= "<td align=left valign=top nowrap>$plugin</td>";	
				$out .= "<td align=right valign=bottom>$average ms</td>";
				
				$out .= "</tr>";
				$have++;
				
				if($have == $maxn) {
					break 3;	
				}
			}	
		}
	}
	
	$out .= "</table>";
	return $out;
}