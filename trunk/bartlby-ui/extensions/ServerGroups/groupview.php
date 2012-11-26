<?
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/ServerGroups/ServerGroups.class.php";
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$inv = new ServerGroups();
	
	$servers=$btl->GetSVCMap();
	$defaults=$inv->load($_GET[grpname]);
	
	
	$layout= new Layout();
	$layout->set_menu("Server Groups");
	$layout->Table("100%");
	$layout->setTitle($defaults[name]);
	
		$sidx=0;
	while(list($k,$v)=@each($servers)) {
		$x=$k;
		if(!@in_array($k, $defaults[servers]) && is_array($defaults[servers])) {
			continue;
		}
		if($btl->isServerUp($x, $servers)) {
			$hosts_up++;	
		} else {
			$hosts_down++;	
			$hosts_a_down[$k]=1;
			
		}
	
		for($y=0; $y<count($v); $y++) {
			$qck[$v[$y][server_id]][$v[$y][current_state]]++;	
			$qck[$v[$y][server_id]][10]=$v[$y][server_id];
			$qck[$v[$y][server_id]][server_icon]=$v[$y][server_icon];
			$qck[$v[$y][server_id]][server_name]=$v[$y][server_name];
			if($v[$y][is_downtime] == 1) {
				$qck[$v[$y][server_id]][$v[$y][current_state]]--;
				$qck[$v[$y][server_id]][downtime]++;
				
			}
			if($v[$y][service_ack] == 2) {
				$qck[$v[$y][server_id]][acks]++;	
				$acks_outstanding++;
				
			}
			
			$services_coll[$sidx]=$v[$y];
			$sidx++;
			$all_services++;
			switch($v[$y][current_state]) {

				case 0:
					$services_ok++;
					if($v[$y][is_downtime] == 1) {
						$services_ok--;
						$services_downtime++;	
					}
				break;
				case 1:
					$services_warning++;
					if($v[$y][is_downtime] == 1) {
						$services_warning--;
						$services_downtime++;	
					}
				break;
				case 2:
					$services_critical++;
					if($v[$y][is_downtime] == 1) {
						$services_critical--;
						$services_downtime++;	
					}
				break;
				
				default:
					$services_unkown++;
					if($v[$y][is_downtime] == 1) {
						$services_ok--;
						$services_downtime++;	
					}
				
				
			}	
		}
		
		
	}
	
	
	
	$quick_view = "<table class='nopad' width=100%>";
	while(list($k, $v)=@each($qck)) {
		
		if($k != $last_qck) {
			$cl="";
			$STATE="UP";
			if ($hosts_a_down[$qck[$k][10]] == 1) {
				$cl="";
				$STATE="DOWN";
			}
			$quick_view .= "<tr>";
			$quick_view .= "<td class=$cl><img src='server_icons/" . $qck[$k][server_icon] . "'><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "'>" . $qck[$k][server_name] . "</A></td>";
			$quick_view .= "<td class=$cl><font size=1>$STATE</td>";
			$quick_view .= "<td class=$cl><table width=100>";
			
			$sf=false;
			if($qck[$k][0]) {
				$sf=true;
				$qo="<tr><td ><font size=1><span class='label label-success'><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=0'>" . $qck[$k][0] . " OK's</A></span></td></tr>";
			}
			if($qck[$k][1]) {
				$sf=true;
				$qw="<tr><td ><span class='label label-warning'><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=1'>" . $qck[$k][1] . " Warnings</A></span></td></tr>";
			}
			
			if($qck[$k][2]) {
				$sf=true;
				$qc="<tr><td><span class='label label-important'><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=2'>" . $qck[$k][2] . " Criticals</A></span></td></tr>";
			}
			
			if($qck[$k][3]) {
				$sf=true;
				$qk="<tr><td ><span class='label label-default'><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=3'>" . $qck[$k][3] . " Unkown</A></span></td></tr>";
			}
			if($qck[$k][4]) {
				$sf=true;
				$qk="<tr><td><span class='label label-default'><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=4'>" . $qck[$k][4] . " Info</A></span></td></tr>";
			}
			if($qck[$k][downtime]) {
				$qk="<tr><td ><span class='label label-default'><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&downtime=true'>" . $qck[$k][downtime] . " Downtime</A></span></td></tr>";
			}
			if($qck[$k][acks]) {
				$qk="<tr><td><span class='label label-default'><font size=1><a href='services.php?server_id=" . $qck[$k][10] . "&expect_state=2&acks=yes'>" . $qck[$k][acks] . " Ack Wait</A></td></td></tr>";
			}
					
				$quick_view .= "$qo";
				$quick_view .= "$qw";
				$quick_view .= "$qc";
				$quick_view .= "$qk";
			$quick_view .= "</table></td>";
			$quick_view .= "</tr>";
			$quick_view .= "<tr><td colspan=3><hr noshade></td></tr>";
		}
		
		$last_qck=$k;	
		$qo="";
		$qw="";
		$qc="";
		$qk="";
	}
	
	$quick_view .= "</table>";
	
	
	$quick_view .= '<table class="table table-striped table-bordered " id="services_table">
						  <thead>
							  <tr>
							  	<th>Server</th>
								  <th>State</th>
								  <th>LastCheck</th>
								  <th>NextCheck</th>
								  <th>Service</th>
								  <th>Output</th>
								  <th>Options</th>
							  </tr>
						  </thead>
						    <tbody>';
						    
for($x=0;$x<count($services_coll); $x++) {

		$lbl = "label-default";
		$services_coll[$x][reada]="UNKOWN";
	if($services_coll[$x][current_state] == 0) {
			$lbl = "label-success";
			$services_coll[$x][reada]="OK";
	}

	if($services_coll[$x][current_state] == 1) {
			$lbl = "label-warning";
			$services_coll[$x][reada]="WARINING";
	}
		if($services_coll[$x][current_state] == 2) {
			$lbl = "label-important";
			$services_coll[$x][reada]="CRITICAL";
			
	}
	
	
	
	$quick_view .= "<tr>";
		$quick_view .= "<td   align=left  valign=top width='1'>
			<a href='services.php?server_id=". $services_coll[$x][server_id] . "'><b>" . $services_coll[$x][server_name] . "</A>
			
			
		</td>

		<td   align='center'  valign=top width=1>
						<span class='label " . $lbl . "'><a href='services.php?expect_state=" . $services_coll[$x][current_state] . "'>" . $services_coll[$x][reada] . "</A></span>
					
		</td>
		<td   align=left  valign=top width='80'  >
			<font size=1>" . date("d.m.y H:i:s", $services_coll[$x][last_check]) . "
		</td>
		<td   align=left  valign=top width='80'  >
			<font size=1>" . date("d.m.y H:i:s", $services_coll[$x][last_check]+$services_coll[$x][check_interval]) . "

		</td>
		<td   align=left  valign=top width='100'  >
			<a href='service_detail.php?service_place=" . $services_coll[$x][shm_place] . "'><b>" . $services_coll[$x][service_name] . "</A>
			
			
		</td>
		<td   align=left  valign=top width='450' >
			" . str_replace( "\\dbr","<br>", nl2br($services_coll[$x][new_server_text])) . "
			
		</td>
		<td width=320>
			
			" . $btl->getserviceOptions($services_coll[$x], $layout) . "
		</td>
	</tr>";
	
	
}

$quick_view .= '</tbody>
</table>';
	
	
	$layout->Tr(
		$layout->Td(
				array(0=>$quick_view)
			)

	);
	
	
	$layout->TableEnd();
	$layout->display();

	
?>