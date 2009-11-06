<?php


error_reporting(0);	
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";

	$uname=getenv("BARTLBY_USER");
        $pw=getenv("BARTLBY_PASSWORD");
        if(!$uname && !$pw) {
                $fp=fopen("/dev/stdin", "r");
		echo "Username:";
                $_SERVER[PHP_AUTH_USER]=trim(fgets($fp, 1024));
		echo "Password";
                $_SERVER[PHP_AUTH_PW]=trim(fgets($fp, 1024));
                fclose($fp);
        } else {
                $_SERVER[PHP_AUTH_USER]=$uname;
                $_SERVER[PHP_AUTH_PW]=$pw;
        }



	
	$btl=new BartlbyUi($Bartlby_CONF, true);
	$info=@$btl->getInfo();
	$layout= new Layout();
	$layout->DisplayHelp(array(0=>"WARN|Welcome to BartlbyUI",1=>"INFO|This is the help screen"));
	$layout->MetaRefresh(30);
	$layout->Table("100%");
	$lib=@bartlby_lib_info($btl->CFG);
	
// define some key constants.
define("ESCAPE_KEY", 27);
define("ENTER_KEY", 13);

$start_from=0;
$alerts_only=1;
$show_downtimes=0;
$selected_index=0;
$reopen_service=false;
$reopen_server=false;
$running_only=0;
$ticker="|";
$hide_warns=0;
$hide_infos=0;
$group_similar=1;

$ncurses_session = ncurses_init();
$main = ncurses_newwin(0, 0, 0, 0); // main window

ncurses_border(0,0, 0,0, 0,0, 0,0);

ncurses_start_color();
ncurses_init_pair(1,NCURSES_COLOR_BLACK,NCURSES_COLOR_RED);
ncurses_init_pair(2,NCURSES_COLOR_BLACK,NCURSES_COLOR_YELLOW);
ncurses_init_pair(3,NCURSES_COLOR_BLACK,NCURSES_COLOR_GREEN);


ncurses_init_pair(4,NCURSES_COLOR_WHITE,NCURSES_COLOR_BLUE);
ncurses_init_pair(5,NCURSES_COLOR_BLACK,NCURSES_COLOR_WHITE);




while(1){

	ncurses_getmaxyx(&$main, $lines, $columns);
	ncurses_timeout(2);
	$k = ncurses_getch();
	$info = @bartlby_get_info($btl->CFG);
	$ztx=0;
	if($info[do_reload] == 1) {
		while(1) {
			disp_reload_window();
			$info = @bartlby_get_info($btl->CFG);
			if($info[do_reload] == 0) {
				break;
			}
			$ztx++;
			usleep(500000);
			
		}
	}
	
	
	
	if($k == ESCAPE_KEY || $k == 113) {
		ncurses_resetty();
       		ncurses_end();
		exit(1);	
	}
	if($k == 104) {
		disp_help();
	}
	
	if($k == ENTER_KEY || $reopen_service == true) {
		btl_disp_service();
	}
	if($k == 115 || $reopen_server == true) {
		btl_disp_server();
	}
	if($k == 119) {
		if($hide_warns == 0)  {
			$hide_warns = 1;
		} else {
			$hide_warns = 0;
		}
	}
	if($k == 82) {
		//reset view :-)
		$hide_warns=0;
		$hide_infos=0;
		$alerts_only=1;
		$group_similar=1;
	}
	if($k == 103) { //press g
		if($group_similar == 0) {
				$group_similar=1;
		} else {
				$group_similar=0;
		}
	}
	if($k == 105) {
                if($hide_infos == 0)  {
                        $hide_infos = 1;
                } else {
                        $hide_infos = 0;
                }
        }

	if($k == 258) {
		if($selected_pos > $lines/2) {
			$start_from=$start_from+1;	
		
		} else {
			$start_from=0;	
		}
		$selected_index++;
	}
	
	if($k == 97) {
		if($alerts_only==1) {
			$alerts_only=0;
		} else {
			$alerts_only=1;
		}
		$selected_index=0;
		$start_from=0;
	}
	if($k == 100) {
		if($show_downtimes == 1)
			$show_downtimes = 0;
		else
			$show_downtimes = 1;
			
	}
	if($k == 259) {
		if($select_pos < 5) {
			//$start_from--;	
			$start_from=$start_from-1;	
		} else {
			$start_from=0;	
		}
		if($selected_index > 0) {
			$selected_index--;
		}
	}
	if($k == 114) {
		if($running_only == 0) {
			$running_only=1;
			$group_similar=0;
		} else {
			$running_only=0;
			$group_similar=1;
		}
			
	}

	unset($map);
	$map = @$btl->GetSVCMap();
	$oks=0;
	$warns=0;
	$crits=0;	
	ncurses_color_set(4);
	

 	// create a lower window which is dynamically sized...


	

ncurses_color_set(4);
for($tt=0; $tt<$lines; $tt++) {
	ncurses_addstr(str_repeat(" ", $columns));
	ncurses_move($tt, 3);	
}
       
        
        $y=2;

	$a=0;
	@reset($map);
	$per_server=false;
	$already_displayed=false;
	unset($f);
	while(list($k, $servs) = @each($map)) {
		$displayed_servers++;
		
			
			
			for($x=0; $x<count($servs); $x++) {
				
				
				$per_server[$servs[$x][server_name]][$servs[$x][current_state]]++;
				
				switch($servs[$x][current_state]) {
					case 0:
						$oks++;
						
					break;	
					case 1:
						$warns++;
						
					break;
					case 2:
						$crits++;
						
					break;
					default:
						ncurses_color_set(4);
					break;
				}
				if($hide_warns == 1) {
					if($servs[$x][current_state] == 1) {
                                                continue;
                                        }
				}
				if($hide_infos == 1) {
					if($servs[$x][current_state] == 4) {
					        continue;
					}
        }

				if($alerts_only == 1) {
					if($servs[$x][current_state] == 0) {
						continue;	
					}
					if($servs[$x][is_downtime] == 1 && $show_downtimes == 0) {
						continue;
					}
				}
				if($running_only == 1) {
					if($servs[$x][check_starttime] == 0) 
						continue;
				}
				//$out_str=sprintf("%s - %s", $servs[$x][service_name], );
				if($a == $selected_index) {
					$selected_svc=$servs[$x];
					$selected_pos=$a;
					$servs[$x][selected] = true;
				} else {
					$servs[$x][selected] = false;

				}
				$f[$a] = $servs[$x];
				$a++;
			}	
		}
			$gservice_count=count($f);
			

			
			
			for($z=0; $z<count($f); $z++) {
				
				if($z >= $start_from && $y <= $lines - 4) {
					
					$out_text="";
					$disp_service="";
					
					if($per_server[$f[$z][server_name]][$f[$z][current_state]] >= 5 && $group_similar == 1) {
							if($already_displayed[$f[$z][server_name]] == 1) {
									
									continue;
							} else {
									$already_displayed[$f[$z][server_name]]=1;
									
							}
					}
					
					////////////////////////////
					
					$out_text=$f[$z][new_server_text];
					$disp_service=$f[$z][service_name];
					
					if($per_server[$f[$z][server_name]][$f[$z][current_state]] >= 5 && $group_similar == 1) {
						$out_text=$per_server[$f[$z][server_name]][$f[$z][current_state]] . " in this state";
						$disp_service = " GROUP ";
					}
					
					$this_row_selected = $f[$z][selected];
					ncurses_move($y+1, 6);
					//ncurses_addstr($servs[$x][server_name] .  str_repeat(" ", 20-strlen($servs[$x][server_name])));
					ncurses_color_set(get_ncurses_color($f[$z][current_state]));
					

					//mark_line($this_row_selected);
					 
					
					ncurses_addstr(sprintf("%-10s", $btl->GetState($f[$z][current_state])));
					ncurses_move($y+1, 20);
					ncurses_color_set(4);

					mark_line($this_row_selected);
					
					
					
					$ostr=sprintf("%-30s  ", $f[$z][server_name] .  ":" . $disp_service);
					ncurses_addstr(substr($ostr,0,27));
					
					ncurses_addstr(substr(str_replace("dbr", "", str_replace("\n", "", $out_text)), 0, $columns-50));
					ncurses_color_set(4);

					mark_line($this_row_selected);
					
					//ncurses_addstr("\n");
					ncurses_move($y+3, 6);
					ncurses_color_set(4);
										
					$y++;
					
				
				}
			}
	
  	
	
  	for($tt=$y+1; $tt<$lines; $tt++) {
  		ncurses_addstr(str_repeat(" ", $columns));
  		ncurses_move($tt, 3);	
  	}

	ncurses_border(0,0, 0,0, 0,0, 0,0);

	// border the main window
	ncurses_attron(NCURSES_A_REVERSE);

	ncurses_mvaddstr(0,1,"($selected_svc[service_id]) bartlby -> " . $btl->getRelease() . "\t" . date("d.m.Y H:i:s", time()) . " /" . $start_from . "-" . $btl->info[services]);
	ncurses_attroff(NCURSES_A_REVERSE);

	if($alerts_only == 1)
		$only_errors="true";
	else
		$only_errors="false";

	if($show_downtimes == 1)
		$dts="true";
	else
		$dts="false";

	ncurses_color_set(4);
	ncurses_attron(NCURSES_A_REVERSE);
	ncurses_mvaddstr($lines-1,1,"Status:\t OK:$oks Criticals: $crits Warnings: $warns, only errors: $only_errors , downtimes: $dts , running: $running_only ");
	ncurses_attroff(NCURSES_A_REVERSE);

  	ncurses_refresh();
  	usleep(200000);


}//end main while
function window_td($w, $x, $y, $r1, $r2) {

	ncurses_mvwaddstr( $w, $x, $y, $r1 );
	ncurses_mvwaddstr( $w, $x, $y+30, $r2 );
}
function get_ncurses_color($s) {
	switch($s) {
          	case 0:
          		$cidx=3;
          	break;
          	case 1:
          		$cidx=2;
          	break;
          	case 2:
          		$cidx=1;
          	break;
          	default:
			$cidx=4;
          	break;
          }
	return $cidx;

}

function btl_disp_service() {
	global $selected_svc;
	global $lines, $columns, $btl;
	global $reopen_service;

	$reopen_service=false;
	
	$defaults=$selected_svc;	
	
	if($defaults[service_type] == 1) {
        	$svc_type="Active";
	}

	if($defaults[service_type] == 2) {
        	$svc_type="Passive";
	}

	if($defaults[service_type] == 3) {
        	$svc_type="Group";
	}
	if($defaults[service_type] == 4) {
        	$svc_type="Local";
	}
	if($defaults[service_type] == 5) {
        	$svc_type="SNMP";
	}	
	if($defaults[service_type] == 6) {
        	$svc_type="NRPE";
	}
	if($defaults[service_type] == 7) {
        	$svc_type="NRPE(ssl)";
	}

	$color=get_ncurses_color($defaults[current_state]);

	
	$w = ncurses_newwin($lines-8,$columns-8, 2,2);
	
        ncurses_wcolor_set($w, 4);
	for($tt=0; $tt<$lines-8; $tt++) {
                ncurses_waddstr($w, str_repeat(" ", $columns-8));
                ncurses_wmove($w, $tt, 3);
        }
	
	

	window_td($w, 1,1, "Server:", $selected_svc[server_name] . "(" . $selected_svc[client_ip] . ")");
	window_td($w, 2,1, "Name:" , $selected_svc[service_name]);

	window_td($w, 3,1, "Type:" , $svc_type);


        ncurses_wcolor_set($w, $color);
	window_td($w, 4,1, "Current State:" , $btl->getState($defaults[current_state]));
	
	ncurses_wcolor_set($w, 4);


	window_td($w, 5,1, "Last Check:" , date("d.m.Y H:i:s", $defaults[last_check]));

	window_td($w, 6,1, "approx. next Check:" , date("d.m.Y H:i:s", $defaults[last_check]+$defaults[check_interval]));

	window_td($w, 7,1, "Intervall:" , $defaults[check_interval]);

	window_td($w, 8,1, "Last Notify sent:" ,  date("d.m.Y H:i:s", $defaults[last_notify_send]));
	if($defaults["notify_enabled"]==1) {
	        $noti_en="true";
	} else {
		$noti_en="false";
	}
	if($defaults[server_notify] != 1) {
		$noti_en .= " (disabled via server)";
	}

	if($defaults["service_active"]==1) {
		$serv_en = "true";

	} else {
		$serv_en = "false";
	}
	if($defaults[server_enabled] != 1) {
		$serv_en .= "(disabled via server)";
	}
	if($defaults[check_starttime] != 0) {
		$currun=date("d.m.Y H:i:s", $defaults[check_starttime]) . " (PID: $defaults[check_is_running] )";
	} else {
		$currun="(Currently not running)";
	}
	switch($defaults[service_ack]) {
		case 0:
			$needs_ack="no";
		break;
		case 1:
			$needs_ack="yes";
		
		break;
		case 2:
			$needs_ack="outstanding";
		break;
	}
	if( $defaults[service_time_sum] > 0 && $defaults[service_time_count] > 0) {
		$svcMS=round($defaults[service_time_sum] / $defaults[service_time_count], 2);
	} else {
		$svcMS=0;
	}

	

	window_td($w, 9,1, "Notify Enabled:" ,  $noti_en);


	window_td($w, 10,1, "Check Enabled:" ,  $serv_en);


	window_td($w, 11,1, "Check from:" ,  dnl($defaults[hour_from]) . ":" . dnl($defaults[min_from]) . ':00');
	window_td($w, 12,1, "Check to:" ,  dnl($defaults[hour_to]) . ":" . dnl($defaults[min_to]) . ':00');
	window_td($w, 13,1, "flap count:" ,  $defaults[flap_count]);
	window_td($w, 14,1, "flap seconds:" ,  $defaults[flap_seconds]);
	window_td($w, 15,1, "needs ack:" ,  $needs_ack);


	window_td($w, 16,1, "Status:" ,  $defaults[service_retain_current] . "/"  . $defaults[service_retain]);

	window_td($w, 17,1, "Is running?:" ,  $currun);
	window_td($w, 18,1, "Average Check time:" ,  $svcMS . " ms");


	ncurses_wattron($w, NCURSES_A_REVERSE);
 	ncurses_mvwaddstr($w, 22,1,"Last Output:");
	ncurses_wattroff($w, NCURSES_A_REVERSE);
        ncurses_mvwaddstr($w, 23,1,str_replace("\dbr", "\n", $defaults[new_server_text]));


	


	ncurses_wborder($w, 0,0, 0,0, 0,0, 0,0);

	ncurses_wattron($w, NCURSES_A_REVERSE);
 	ncurses_mvwaddstr($w, 0,1,"Service Detail:");
	ncurses_wattroff($w, NCURSES_A_REVERSE);


	ncurses_wcolor_set($w, 4);
        ncurses_wattron($w, NCURSES_A_REVERSE);
        ncurses_mvwaddstr($w, $lines-9,1,"Keys:\t (c) disable/enable checkes, (n) enable/disable notifys, (f) force");
        ncurses_wattroff($w, NCURSES_A_REVERSE);
	

	
	
	

	ncurses_wrefresh($w);
	$k = ncurses_wgetch($w);

	if($k == 110) {
		window_disable_notification();
			
	} 
	if($k == 99) {
		window_disable_check();
	}
	if($k == 115) {
		btl_disp_server();
		btl_disp_service();
	}
	if($k == 102) {
		window_force_check();
	}
	ncurses_delwin($w);
	
	
}

function window_force_check() {
	global $selected_svc;
        global $lines, $columns, $btl;
        global $reopen_service,$reopen_server;

        $defaults=$selected_svc;

        bartlby_check_force($btl->CFG, $defaults[shm_place]);

        $reopen_service=true;

}
function window_disable_check_server() {
        global $selected_svc;
        global $lines, $columns, $btl;
        global $reopen_service, $reopen_server;

        $defaults=$selected_svc;
	$x=bartlby_get_server_by_id($btl->CFG,$defaults[server_id]);

        bartlby_toggle_server_active($btl->CFG, $x[server_shm_place], 1);

        $reopen_server=true;






}

function window_disable_notification_server() {
        global $selected_svc;
        global $lines, $columns, $btl;
        global $reopen_service, $reopen_server;

        $defaults=$selected_svc;
	$x=bartlby_get_server_by_id($btl->CFG,$defaults[server_id]);

        bartlby_toggle_server_notify($btl->CFG, $x[server_shm_place], 1);
        $reopen_server=true;






}


function window_disable_check() {
        global $selected_svc;
        global $lines, $columns, $btl;
        global $reopen_service;

        $defaults=$selected_svc;

        bartlby_toggle_service_active($btl->CFG, $defaults[shm_place], 1);

        $reopen_service=true;






}

function window_disable_notification() {
	global $selected_svc;
        global $lines, $columns, $btl;
	global $reopen_service;

        $defaults=$selected_svc;
	
	bartlby_toggle_service_notify($btl->CFG, $defaults[shm_place], 1);
	$reopen_service=true;


	
	

	
}
function mark_line($tf) {
	if($tf) 
		ncurses_color_set(5);
			
}
function dnl($i) {
        return sprintf("%02d", $i);
}

function btl_disp_server() {
	global $selected_svc;
	global $lines, $columns, $btl;
	global $reopen_service, $reopen_server;
	global $map;

	$reopen_server=false;
	
	$defaults=$selected_svc;	
	
	$color=get_ncurses_color($defaults[current_state]);

	

	$w = ncurses_newwin($lines-8,$columns-8, 2,2);

        ncurses_wcolor_set($w, 4);
	for($tt=0; $tt<$lines-8; $tt++) {
                ncurses_waddstr($w, str_repeat(" ", $columns-8));
                ncurses_wmove($w, $tt, 3);
        }
	
	

	window_td($w, 1,1, "Name:", $selected_svc[server_name]);
	
	if($btl->isServerUp($defaults[server_id], $map)) {
		

        	ncurses_wcolor_set($w, 3);
		$isup="UP";
	} else {

        	ncurses_wcolor_set($w, 1);
		$isup="DOWN";
	}
	
	window_td($w, 2,1, "Status:" , $isup);

	ncurses_wcolor_set($w, 4);
	window_td($w, 3,1, "IP:" ,  $selected_svc[client_ip] . " (" . gethostbyname( $selected_svc[client_ip]) . ")");

	if($defaults["server_notify"]==1) {
		$noti_en = "true";
	} else {
		$noti_en = "false";
	}
	if($defaults["server_enabled"]==1) {
		$server_en="true";
		
	} else {
		$server_en="false";
	}
        
	window_td($w, 4,1, "Notifications:" , $noti_en);
	
	


	window_td($w, 5,1, "Enabled:" , $server_en);
	

	window_td($w, 6,1, "Last Notify Send:" , date("d.m.Y H:i:s", $defaults[last_notify_send]));

	window_td($w, 7,1, "flap seconds:" , $defaults[server_flap_seconds]);

	window_td($w, 8,1, "Last Notify sent:" ,  date("d.m.Y H:i:s", $defaults[last_notify_send]));
	

	window_td($w, 9,1, "Location:" ,  getGeoip(gethostbyname($defaults[client_ip])));




	ncurses_wborder($w, 0,0, 0,0, 0,0, 0,0);

	ncurses_wattron($w, NCURSES_A_REVERSE);
 	ncurses_mvwaddstr($w, 0,1,"Server Detail:");
	ncurses_wattroff($w, NCURSES_A_REVERSE);


	ncurses_wcolor_set($w, 4);
        ncurses_wattron($w, NCURSES_A_REVERSE);
        ncurses_mvwaddstr($w, $lines-9,1,"Keys:\t (c) disable/enable checkes, (n) enable/disable notifys");
        ncurses_wattroff($w, NCURSES_A_REVERSE);
	

	
	
	

	ncurses_wrefresh($w);
	$k = ncurses_wgetch($w);

	if($k == 110) {
		window_disable_notification_server();
			
	} 
	if($k == 99) {
		window_disable_check_server();
	}
	ncurses_delwin($w);
	
	
}
function getGeoip($ip) {
        $fp=popen("geoiplookup $ip", "r");
        while(!feof($fp)) {
                $rmsg .= fgets($fp, 1024);
        }
        $exi=pclose($fp);
        if($exi == 127) {
                return "(maybe you dont have 'geoiplookup' not installed or it is not in your PHP path)";
        } else {
                $a=explode(":",$rmsg);
                return $a[1];
        }

}

function disp_reload_window() {
	global $ticker;
	global $selected_svc;
        global $lines, $columns, $btl;
        global $reopen_service, $reopen_server;
        global $map;
        global $help;
        
       	switch($ticker) {
       		case '|':
       			$ticker='/';
       		break;	
       		case '/':
       			$ticker='|';
       		break;
       		
       	}
        
	$help = array(

                array(1, "", "Reload in progress .... please wait"),



                );


        $color=get_ncurses_color($defaults[current_state]);


        $w = ncurses_newwin(5,$columns/2, 4,6);


        ncurses_wcolor_set($w, 4);
        for($tt=0; $tt<40; $tt++) {
                ncurses_waddstr($w, str_repeat(" ", $columns-8));
                ncurses_wmove($w, $tt, 3);
        }


        for($x=0; $x<count($help); $x++) {
                if($help[$x][0] == 1) {
                        window_td($w, $x+1,1, $help[$x][1], $help[$x][2]);
                } else {

                        ncurses_wattron($w, NCURSES_A_REVERSE);
                        ncurses_mvwaddstr($w, $x+1,1,$help[$x][1]);
                        ncurses_wattroff($w, NCURSES_A_REVERSE);
                }
        }

        ncurses_wborder($w, 0,0, 0,0, 0,0, 0,0);

        ncurses_wattron($w, NCURSES_A_REVERSE);
        ncurses_mvwaddstr($w, 0,0,"Wait " . $ticker);
        ncurses_wattroff($w, NCURSES_A_REVERSE);







        ncurses_wrefresh($w);
        
        ncurses_delwin($w);


	

	
}

function disp_help() {
	global $selected_svc;
	global $lines, $columns, $btl;
	global $reopen_service, $reopen_server;
	global $map;
	global $help;

	$help = array(
		
		array(1, "h", "Display this Help"),

		array(1, "<ENTER>", "open service detail"),
		array(1, "s", "open server detail from selected service"),
		array(1, "<UP>,<DOWN>", "scroll service list"),
		array(1, "a" , "Only disable service wich arent OK"),
		array(1, "d", "also show downtimed services"),
		array(1, "r", "only show checks wich are currently running"),
		array(1, "w", "hide services wich are in state:warning"),
		array(1, "i", "hide services wich are in state:info"),
		array(1, "g", "toggle grouping of similar error messages per server"),
		array(1, "R", "Reset filter's"),
		array(0, "Service Detail:"),
		array(1, "f", "Force Check"),
		array(1, "c", "Check enable/disable"),
		array(1, "n", "Notification enable/disable"),
	        array(1, "s", "Display server details"),
		array(0, "Server Detail"),
		array(1, "c", "Check enable/disable server"),
		array(1, "n", "Notification enable/disable server"),

		array(0, "Global"),
		array(1, "q", "close window")
		
		
		);

	
	$color=get_ncurses_color($defaults[current_state]);

		
	$w = ncurses_newwin($lines-8,$columns-8, 2,2);

        ncurses_wcolor_set($w, 4);
	for($tt=0; $tt<40; $tt++) {
                ncurses_waddstr($w, str_repeat(" ", $columns-8));
                ncurses_wmove($w, $tt, 3);
        }
	
	
	for($x=0; $x<count($help); $x++) {
		if($help[$x][0] == 1) {
			window_td($w, $x+1,1, $help[$x][1], $help[$x][2]);
		} else {
			
			ncurses_wattron($w, NCURSES_A_REVERSE);
 			ncurses_mvwaddstr($w, $x+1,1,$help[$x][1]);
			ncurses_wattroff($w, NCURSES_A_REVERSE);
			
		}
	}
	
	ncurses_wborder($w, 0,0, 0,0, 0,0, 0,0);

	ncurses_wattron($w, NCURSES_A_REVERSE);
 	ncurses_mvwaddstr($w, 0,1,"Help:");
	ncurses_wattroff($w, NCURSES_A_REVERSE);



	
	
	

	ncurses_wrefresh($w);
	$k = ncurses_wgetch($w);
	ncurses_delwin($w);
	
	
}


// the following are two helper functions for
// this ncurses example.



?>
