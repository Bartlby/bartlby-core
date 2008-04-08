<?
	include "layout.class.php";
	include "config.php";
	include "bartlby-ui.class.php";

	$layout= new Layout();
	$layout->MetaRefresh(240);
	$layout->Table("100%");
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("main.services");
	$map = $btl->GetSVCMap($_GET[service_state]);	


	
	$mode=bartlby_config("ui-extra.conf", "xml_remote_count");
	$do_xml=true;
	if($_GET[server_id]) {
		if(!preg_match("/^XML.*/i", $_GET[server_id])) {
			$do_xml=false;	
		}	
	}
	if($do_xml==true) {
		if($mode) {
			$remote_xml=array();
			for($xml_X=1; $xml_X<=$mode; $xml_X++) {
					$xml_cur=bartlby_config("ui-extra.conf", "xml_remote[" . $xml_X . "]");
					$xml_cur_alias=bartlby_config("ui-extra.conf", "xml_alias[" . $xml_X . "]");
					if($xml_cur) {
						$xml_return = $btl->getRemoteStatus($xml_cur, $xml_cur_alias);
						$btl->appendXML_to_svc_map($xml_return[services], $xml_cur_alias, $map, $xml_X);	
					}
			}
		
		
			
		}
	}
	
	
	
	$layout->set_menu("main");
	
	$layout->setTitle("Services");
	
	$display_serv=$_GET[server_id];
	if(!$display_serv) {
		while(list($k,$v) = @each($map)) {
		//	$display_serv=$k;
			
			break;	
		}
		@reset($map);	
	}
	
		
		while(list($k, $servs) = @each($map)) {
			
			
			if($display_serv && $display_serv != $k) {
				continue;	
			}
			
			$curp = $_GET[$k ."site"] > 0 ? $_GET[$k ."site"] : 1;
			$perp=bartlby_config("ui-extra.conf", "services_per_page");
			$forward_link=$btl->create_pagelinks("services.php?expect_state=" . $_GET[expect_state] . "&server_id=" . $_GET[server_id], count($servs)-1, $perp, $curp,$k ."site");
			
			
			$cur_box_title="<a href='server_detail.php?server_id=" . $servs[0][server_id] . "'>" . $servs[0][server_name] . "</A> ( " . $servs[0][client_ip] . ":" . $servs[0][client_port] . " ) $forward_link"; //. "<a href='package_create.php?action=create_package&server_id="  . $servs[0][server_id] . "'><font size=1><img src='images/icon_work1.gif' border=0></a>";
			$cur_box_content = "<table class='service_table' cellpadding=2>";
			
			$d=0;
			$skip_em=($curp*$perp)-$perp;
			$f=false;
			for($x=$skip_em; $x<count($servs); $x++) {
				
				
				if($d >= $perp) {
					break;	
				}
				if($_GET[downtime] == "" && $_GET[invert] == "" && $_GET[expect_state] != "" && $servs[$x][current_state] != $_GET[expect_state]) {
					
					continue;	
				}
				if($_GET[downtime] == "" &&  $_GET[invert] && $_GET[expect_state] != "" && $servs[$x][current_state] == $_GET[expect_state]) {
					
					continue;
				}
				if($_GET[downtime] && $servs[$x][is_downtime] != 1) {
					
					continue;	
				}
				if($_GET[expect_state] != "" && $servs[$x][is_downtime] == 1) {
					continue;
				}
				
				
				
				if($_GET[acks] == "yes" && $servs[$x][service_ack] != 2) {
					continue;	
				}
				$d++;
						
				$displayed_services++;
				$svc_color=$btl->getColor($servs[$x][current_state]);
				$svc_state=$btl->getState($servs[$x][current_state]);
				if($servs[$x][service_ack] == 2) {
					
					$svc_state .= "<br>(ACKW)";	
				}
				$server_color="black";
				$SERVER="&nbsp;";
				$class="header1";
				if($x % 2 == 1) {
					$class="header1";	
				}
								
				if($servs[$x][is_downtime] == 1) {
					$svc_state="Downtime";
					$svc_color="silver";	
				}
				$cur_box_content .= $layout->Tr(
					$layout->Td(
						Array(
							
							0=>array(
								"width"=>58,
								"align"=>"center",
								"show"=>"<b><a href='services.php?expect_state=" . $servs[$x][current_state] . "'><font style='font-size:9px;'>" . $svc_state . "</A></b>",
								'class'=>$svc_color . "_img"
							   ),
							1=>array(
								"width"=>80,
								"class"=>$class,
								"show"=>"<font size=1>" . date("d.m.y H:i:s", $servs[$x][last_check])
							   ),
							2=>array(
								"width"=>80,
								"class"=>$class,
								"show"=>"<font size=1>" .  date("d.m.y H:i:s", $servs[$x][last_check]+$servs[$x][check_interval])
							   ),						
							3=>array(
								"width"=>150,
								"class"=>"header1",
								"show"=>"<a href='service_detail.php?service_place=" . $servs[$x][shm_place] . "'><b>" . $servs[$x][service_name]  . "$working_on $flap_pic</b><br>" . "<br> " . $btl->getserviceOptions($servs[$x],$layout) . "</font>"
							   ),
							4=>array(
								"width"=>450,
								"class"=>$class,
								"show"=>str_replace( "\\dbr", "<br>",nl2br($servs[$x][new_server_text]))
							   )
						)
					)
				, true);
				$f=true;
			}
			$cur_box_content .= "</table>";
			if($f == true) {
				$layout->push_outside($layout->create_box($cur_box_title, $cur_box_content));
				$displayed_servers++;
			}
	}
	$legend_content="<table class='nopad' width='100%'>
		<tr>
			
			<td width=15><img src='images/diabled.gif'></td>
			<td align=left class='font2'>Service Check is disabled</td>
			
			
			<td width=15><img src='images/notrigger.gif'></td>
			<td align=left class='font2'>Notifications are disabled</td>
			
			<td width=15><img src='images/icon_view.gif'></td>
			<td align=left class='font2'>View Logfile</td>
			
			
			
			
		</tr>
		<tr>
			<td width=15><img src='images/enabled.gif'></td>
			<td align=left class='font2'>Service Check is enabled</td>
			
			<td width=15><img src='images/trigger.gif'></td>
			<td align=left class='font2'>Notifications are enabled</td>
			
			<td width=15><img src='images/icon_comments.gif'></td>
			<td align=left class='font2'>Comments</td>
			
			
			
		</tr>

	
		<tr>
                        <td width=15><img src='images/force.gif'></td>
                        <td align=left class='font2'>Force Check</td>

                        <td width=15><img src='images/edit-copy.gif'></td>
                        <td align=left class='font2'>Copy service</td>

                        <td width=15><img src='images/create_report.gif'></td>
                        <td align=left class='font2'>Create Report</td>



                </tr>	
		<tr>
			<td width=15><img src='images/icon_stat.gif'></td>
			<td align=left class='font2'>Update graph</td>
			
			<td width=15><img src='images/modify.gif'></td>
                        <td  colspan=3 align=left class='font2'>Modify Service</td>

		</tr>
		
	</table>";
	
	$layout->push_outside($layout->create_box("Legend", $legend_content));
	$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 6,
					'class'=>'header1',
					'show'=>"Matching Servers: $displayed_servers Matching Services: $displayed_services"
					)
			)
		)

	);	

	$r=$btl->getExtensionsReturn("_services", $layout);
	

	$layout->TableEnd();
	$layout->display();
	
?>
