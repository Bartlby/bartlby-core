<?php

	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	include "Mail.php";
	include "Mail/mime.php";
	include "extensions/ArS/ArS.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("super_user");
	$sg = new ArS();
	$servers=$btl->GetSVCMap();
	
	
	$layout= new Layout();
	$layout->setTitle("ArS: action");
	
	$layout->set_menu("ArS");
	$layout->Form("fm1", "extensions_wrap.php");
	$layout->Table("100%");
	
	if($_GET[action] == "add") {
		$action="Adding report";	
		
		$da = $sg->storage->load_key("reports");
		$reports=unserialize($da);
		if(!is_array($reports)) {
			$reports=array();	
		}
		
		array_push($reports, $_GET);
		
		$da = serialize($reports);
		$da = $sg->storage->save_key("reports", $da);
		
		$output = "report added!!!";
		
		
		
	} else if($_GET[action] == "delete") {
		$action = "delete report";
		
		$da = $sg->storage->load_key("reports");
		$reports=unserialize($da);
		$y=0;
		
		for($x=0; $x<count($reports); $x++) {
			if($x != $_GET[ars_report]) {
				$new[$y]=$reports[$x];
				$y++;	
			}
		}
		
		$da = serialize($new);
		$sg->storage->save_key("reports", $da);
		
		$output = "report deleted";	
	} else if($_GET[action] == "sendout") {
		
		$action = "sending out the reports";
		
		
		$da = $sg->storage->load_key("reports");
		$reports=unserialize($da);
		$y=0;
		
		
		for($x=0; $x<count($reports); $x++) {
			$rf=false;
			$defaults=bartlby_get_service_by_id($btl->CFG, $reports[$x][ars_service_id]);
			$rap = "Report for: " . $defaults[server_name] . "/" . $defaults[service_name] . "\n";
			$btl_subj = "Bartlby report";
			
			@reset($servers);
			while(@list($k, $v) = @each($servers)) {
				for($z=0; $z<count($v); $z++) {
					if($v[$z][service_id]==$reports[$x][ars_service_id]) {
						$lstate=$v[$z][current_state];
					}
				}	
			}
			
			
			if($_GET[wich] == "daily") {
				
				if($reports[$x][ars_daily]) {
					$rap .= "DAILY REPORT:\n\n";
					$rep = $btl->do_report(date("d.m.Y", time()-86400), date("d.m.Y", time()-86400), $lstate, $reports[$x][ars_service_id]);
					$rap .= "FROM: " . date("d.m.Y", time()-86400) . " TO: " . date("d.m.Y", time()-86400) . "\n";
					$file =  $sg->format_report($rep, $reports[$x][ars_type], $rap);
					$rf=true;
				
				
				}
			}
			if($_GET[wich] == "weekly") {
				if($reports[$x][ars_weekly]) {
					$rap .= "WEEKLY REPORT:\n\n";
					$rep = $btl->do_report(date("d.m.Y", time()-(86400*7)), date("d.m.Y"), $lstate, $reports[$x][ars_service_id]);
					$rap .= "FROM: " . date("d.m.Y", time()-(86400*7)) . " TO: " . date("d.m.Y", time()) . "\n";
					$file =  $sg->format_report($rep, $reports[$x][ars_type], $rap);
					$rf=true;
					
					
				}		
			}
			
			if($_GET[wich] == "monthly") {
				if($reports[$x][ars_monthly]) {
					$rap .= "MONTHLY REPORT:\n\n";
					$rep = $btl->do_report(date("d.m.Y", time()-(86400*31)), date("d.m.Y"), $lstate, $reports[$x][ars_service_id]);
					$rap .= "FROM: " . date("d.m.Y", time()-(86400*31)) . " TO: " . date("d.m.Y", time()) . "\n";
					$file =  $sg->format_report($rep, $reports[$x][ars_type], $rap);

					$rf=true;
					
				}	
			}
			
					
			
			
			
			if($rf == true) {
				$headers = array('From' =>  $sg->storage->load_key("ars_smtp_from") , 'To' => $reports[$x][ars_to],
						   'Subject' => $btl_subj);
					   
					   
				$smtp = Mail::factory('smtp',
		                	array ('host' =>  $sg->storage->load_key("ars_smtp_host"),
              		          		'auth' => false,
                        		   	'timeout' => 10,
                        			'debug' => false
                			));
			
                	
                	
	                	$tmpfname = tempnam ("/tmp", "ArS");
			
				$fp = fopen($tmpfname, "w");
				fwrite($fp, $file);
				fclose($fp);
			
				copy($tmpfname, $tmpfname . ".html");
				unlink($tmpfname);
			
				$mime = new Mail_Mime();
				$mime->setTxtBody("see the attachment for details");
				
				$mime->addAttachment($tmpfname . ".html", "text/html", "report.html");
			
			
				//update perf handler ;)
				$btl->updatePerfHandler($defaults[server_id], $defaults[service_id]);
        	        	$path=bartlby_config($btl->CFG, "performance_rrd_htdocs");
	                	foreach(glob($path . "/" . $reports[$x][ars_service_id] . "_*.png") as $fn) {
                		     	//$mime->addAttachment($fn, "image/png", basename($fn), true, 'base64', 'inline');
		                     	$mime->addHTMLImage($fn, "image/png", basename($fn), true);
		                     	$file .= "<img src='" . basename($fn) . "'><br>";
	                	}
	                	$mime->setHTMLBody($file);
	                	$body=$mime->get();
				$hdrs=$mime->headers($headers);
                	
                	
                		$mail = $smtp->send($reports[$x][ars_to], $hdrs, $body);

			}

			
			
			
		}
		
		
		$output = "done";
			
	}
	
	
	
	
	
	
	

$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<b>$action</b>")
			)
		)

);	
$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "$output")
			)
		)

);	


	$layout->FormEnd();
	
	$layout->TableEnd();
	$layout->display();
