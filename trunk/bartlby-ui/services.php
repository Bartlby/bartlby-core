<?
	include "layout.class.php";
	include "config.php";
	include "bartlby-ui.class.php";

	$btl=new BartlbyUi($Bartlby_CONF);
	$layout= new Layout();
	$layout->MetaRefresh(240);
	$layout->Table("100%");
	
	
	$btl->hasRight("main.services");
	$map = $btl->GetSVCMap($_GET[service_state]);	


	
	
	$layout->set_menu("main");
	$layout->setTitle("Services");
	
	$display_serv=$_GET[server_id];
	
	
		
		while(list($k, $servs) = @each($map)) {
			
			
			if($display_serv && $display_serv != $k) {
				continue;	
			}
			
			$curp = $_GET[$k ."site"] > 0 ? $_GET[$k ."site"] : 1;
			$perp=bartlby_config("ui-extra.conf", "services_per_page");
			$forward_link=$btl->create_pagelinks("services.php?expect_state=" . $_GET[expect_state] . "&server_id=" . $_GET[server_id], count($servs)-1, $perp, $curp,$k ."site");
			
			
			$cur_box_title="<a href='server_detail.php?server_id=" . $servs[0][server_id] . "'>" . $servs[0][server_name] . "</A> ( " . $servs[0][client_ip] . ":" . $servs[0][client_port] . " ) $forward_link"; //. "<a href='package_create.php?action=create_package&server_id="  . $servs[0][server_id] . "'><font size=1><img src='themes/" . $layout->theme . "/images/icon_work1.gif' border=0></a>";
			
			
			$d=0;
			$skip_em=($curp*$perp)-$perp;
			$f=false;
			$services_found=array();
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
				$servs[$x][color]=$svc_color;
				$servs[$x][state_readable]=$svc_state;
				$servs[$x]["class"]=$class;
				array_push($services_found, $servs[$x]);			
				$f=true;
			}
			
			if($f == true) {
				
				$displayed_servers++;
				$layout->create_box($cur_box_title, $cur_box_content, "server_box_" . $x,
											array(
												"services" => $services_found,
												"state" => $svc_state,
												"color" => $svc_color,
												
												
											)
				
				,"service_list_element");
				
				
			}
	}
	$legend_content="";
	
	$layout->create_box("Legend", $legend_content, "legend", array("a"=>"b"), "legend");
	
	
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
	
	$layout->boxes_placed[MAIN]=true;
	$layout->TableEnd();
	$layout->display("services");
	
?>
