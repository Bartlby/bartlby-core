/* $Id$ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2009 Helmut Januschka - All Rights Reserved
 *   Contact: <helmut@januschka.com>, <contact@bartlby.org>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 *   visit: www.bartlby.org for support
 * ----------------------------------------------------------------------- */
/*
$Revision$
$HeadURL$
$Date$
$Author$ 
*/


#include <bartlby.h>

#define DEFAULT_NOTIFY_MSG "State Change ($READABLE_STATE$)\\n*********** $PROGNAME$ $VERSION$ ********************\\n[  Server: $SERVER$, Service: $SERVICE$, State: $READABLE_STATE$]\\n%$MESSAGE$"
#define FL 0
#define TR 1


static sig_atomic_t connection_timed_out=0;
#define CONN_TIMEOUT 15


static void trigger_conn_timeout(int signo) {
 	connection_timed_out = 1;
}

int bartlby_servicegroup_has_trigger(struct service * svc, char * trigger) {
		//Check if servicegroup has notifications on

	
	int x;
		//Check if service group is enabled to run checks
		if(svc->servicegroup_counter == 0) {
				///service is not member of a group
				return 1;
		}
		//Loop Threw service Groups
		for(x=0; x<svc->servicegroup_counter; x++) {
			if(strlen(svc->servicegroups[x]->enabled_triggers) > 2 && strstr(svc->servicegroups[x]->enabled_triggers, trigger) == NULL) {
				return 0;
			}
		}
		
		return 1;	
}

int bartlby_servergroup_has_trigger(struct server * srv, char * trigger) {
		int x;
		//Check if server group has notifications
		if(srv->servergroup_counter == 0) {
				///Server is not member of a group
				return 1;
		}
		//Loop Threw Server Groups
		for(x=0; x<srv->servergroup_counter; x++) {
			if(strlen(srv->servergroups[x]->enabled_triggers) > 2 && strstr(srv->servergroups[x]->enabled_triggers, trigger) == NULL) {
				return 0;
			}
		}
		
		return 1;
}

int bartlby_trigger_worker_level(struct worker * w,  struct service * svc) {
	char * find_level, * last_level;
	char * blevel;
	int rt;
	int level;
	int last;
	
	level = svc->current_state;
	last  = svc->notify_last_state;
	
	blevel=bartlby_beauty_state(level);
	
	asprintf(&last_level, "|%d|", last);
	asprintf(&find_level, "|%d|",level);
	if(strstr(w->notify_levels, find_level) != NULL || strlen(w->notify_levels) == 0) {
		if(level < last) {
			//_log("@debug level:%d < last:%d", level, last);
			if(strstr(w->notify_levels, last_level) != NULL) {
				rt=TR;
				//_log("@debug has both levels: %d/%d", level, last);		
			} else {
				if(level == STATE_OK && svc->recovery_outstanding == RECOVERY_OUTSTANDING) {
					rt=TR;
					//_log("@debug services has recoverd so pull trigger %d/%d", level, last);
				} else { 
					//_log("@debug service doesn't have last_state %d/%d", level, last);	
					rt = FL;	
				}
			}
		} else {
			//_log("@debug current level is selected and higher than last so fire!! %d/%d", level, last);	
			rt=TR;
		}
	} else {
		rt=FL;	
		//_log("Worker %s doesnt have level: %s '%s'-->'%s'(%d)", w->mail, blevel, find_level, w->notify_levels, strlen(w->notify_levels));
		
	}
	
	free(blevel);
	free(last_level);
	free(find_level);	
	return rt;
}

int bartlby_trigger_escalation(struct worker *w, struct service * svc, int standby_workers_only) {
	if(standby_workers_only == 0 && w->active != 1) {
		//_log("Worker: %s is inactive", w->mail);
		return FL;	
	}
	if((time(NULL) - w->escalation_time) >= (w->escalation_minutes*60)) {
		w->escalation_count=0;
		return TR;	
	} else {
		if(w->escalation_count > w->escalation_limit) {
			_log("@NOT-EXT@%ld|%d|%d|%s||%s:%d/%s|'(escalation %d/%ld)'", svc->service_id, svc->last_state ,svc->current_state,w->name, svc->srv->server_name, svc->srv->client_port, svc->service_name,w->escalation_count, w->escalation_limit);
			return FL;
		} else {
			w->escalation_count++;
			return TR;	
		}	
	}
}

int bartlby_trigger_chk(struct service *svc) {
	
	if(sched_servergroup_notify(svc->srv) == 0) {
		_log("@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Notifications disabled on this servergroup)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name);
		return FL;
	}
	if(sched_servicegroup_notify(svc) == 0) {
		_log("@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Notifications disabled on this servicegroup)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name);
		return FL;
	}
	
	
	
	if(svc->srv->server_notify == 0) {
		_log("@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Notifications disabled on this server)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name);
		return FL;
	}
	if(svc->notify_enabled == 0) {
		//_log("Suppressed notify: Notifications disabled %s:%d/%s",svc->client_ip, svc->srv->client_port, svc->service_name);
		_log("@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Notifications disabled)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name);
		return FL;
	} else {
		if((time(NULL) - svc->srv->last_notify_send) > svc->srv->server_flap_seconds) {
			svc->srv->flap_count=0;	
		} else {
			if(svc->srv->flap_count > 2) {
				_log("@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Server lazy %ld)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name, svc->flap_count);
				return FL;
			} else {
				svc->srv->flap_count++;	
			}	
		}
		
		if((time(NULL)- svc->last_notify_send) >= svc->flap_seconds) {
			svc->flap_count=0;
			return TR;				
		} else {
			
			if(svc->flap_count > 2) {
				_log("@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Service flapping %ld)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name, svc->flap_count);
				return FL;
			} else {
				//Log("trigger", "Service %s:%d/%s Sent", svc->client_ip, svc->srv->client_port, svc->service_name);	
				svc->flap_count++;
				return TR;
			}
			
		}
	}
					
  

	
}

int bartlby_worker_has_service(struct worker * w, struct service * svc, char * cfgfile) {

// return 0 if worker doesnt have
// else return 1	
// default to false
	char * uir;
	char * user_dat;
	char * visible_servers, * visible_services;
	char * selected_servers, * selected_services;
	char * is_super_user;
	char * find_server;
	char * find_service;
	
	int the_state;
	
	
	the_state = 0; //default zero
	
	uir = getConfigValue("ui_right_dir", cfgfile);
	if(uir == NULL) {
		_log("variable 'ui_right_dir' unset wich should point to the right folder down in bartlby-ui");
		return 1; // if any right reading problem OK -> for backcomp
	}
	
	
	asprintf(&user_dat, "%s/%ld.dat", uir, w->worker_id);
	free(uir);
	
	//_log("user_right_file: %s", user_dat);
	
	visible_servers = getConfigValue_ex("servers", user_dat, 0);
	visible_services = getConfigValue_ex("services", user_dat, 0);
	is_super_user = getConfigValue_ex("super_user", user_dat, 0);
	selected_servers = getConfigValue_ex("selected_servers", user_dat, 0);
	selected_services = getConfigValue_ex("selected_services", user_dat, 0);
	
	
	if(visible_servers == NULL) 
		visible_servers=strdup("");
	
	if(visible_services == NULL) 
		visible_services=strdup("");
	
	if(is_super_user == NULL) 
		is_super_user=strdup("false");
	
	
	if(selected_services == NULL) 
		selected_services=strdup("");
		
	if(selected_servers == NULL) 
		selected_servers=strdup("");
	
	
	asprintf(&find_server, ",%ld,", svc->server_id);
	asprintf(&find_service, ",%ld,", svc->service_id);
	
	
	
	//_log("@TRIG@ visible_servers: %s; visible_services:%s; super_user: %s;", visible_servers, visible_services, is_super_user);
	
	
	if(strstr(visible_servers, find_server) == NULL && strcmp(is_super_user, "true") != 0) {
		the_state=0;	
		//_log("doesnt have server: %s", visible_servers);
	} else {
		the_state=1; //temp its ok	
	}
	
	if(the_state == 0 && strstr(visible_services, find_service) == NULL && strcmp(is_super_user, "true") != 0) {
		the_state = 0;
		//_log("doesnt have service: %s", visible_services);	
	} else {
		
		the_state = 2;	//Temp ok
	}
	
	
	if(the_state > 0) {
		//server,service is visible now check if its selected ;)
		if(strlen(selected_servers) < 2 && strlen(selected_services) < 2) {
			the_state=3;	
		} else {
			if(strstr(selected_servers, find_server) != NULL || strstr(selected_services, find_service) != NULL) {
				the_state=4;	
			} else {
				//_log("not selected: %s/%s %s/%s", selected_servers, find_server, selected_services, find_service);
				the_state=0;	
			}
		}
		
			
	}	
	
	
	

	//read in ui_right_dir cfg var
	//compare if server is visible -> if empty string -> yes -> else check if selected -> yes
	//compare if service is visible -> if empty string -> yes ->  else check if selected -> yes
	
	
	//_log("@TRIG@ return %d for worker '%s' with selected_services '%s'", the_state, w->name, selected_services);	

	free(user_dat);
	if(visible_servers != NULL)
		free(visible_servers);
	if(visible_services != NULL)
		free(visible_services);
	if(is_super_user != NULL)
		free(is_super_user);
	if(selected_servers != NULL)
		free(selected_servers);
	if(selected_services != NULL)
		free(selected_services);
	if(find_server != NULL)
		free(find_server);
	if(find_service != NULL)
		free(find_service);
		

	return the_state;
}

void bartlby_trigger(struct service * svc, char * cfgfile, void * shm_addr, int do_check, int standby_workers_only) {
	char * trigger_dir;
	struct dirent *entry;
	DIR * dtrigger;
	char * full_path;
	struct stat finfo;	
	int x;
	
	struct ext_notify en;
	
	char * notify_msg;
	char * find_str;
	struct worker * wrkmap;
	struct shm_header * hdr;
	char * exec_str;
	FILE * ptrigger;
	char * find_trigger;
	char trigger_return[1024];
	struct sigaction act1, oact1;
	char * cfg_trigger_msg;
	
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	wrkmap=bartlby_SHM_WorkerMap(shm_addr);
	
	if(do_check == 1) {
		if(bartlby_trigger_chk(svc) == FL) {
			return;	
		}
	}
	
	signal(SIGPIPE,SIG_DFL);
	signal(SIGCHLD,SIG_DFL);
	
	act1.sa_handler = trigger_conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		
		printf("ALARM SETUP ERROR");
		exit(1);
				
		
	
		
	}
	
	
	
	/*
	LAST CUR SERVICE
	PROGNAME VERSION
	SERVERN SERVICE CUR MSG
	*/
	
	asprintf(&find_str, "|%ld|", svc->service_id);
	cfg_trigger_msg=getConfigValue("trigger_msg", cfgfile);
	if(cfg_trigger_msg == NULL) {
		cfg_trigger_msg=strdup(DEFAULT_NOTIFY_MSG);	
	}
	notify_msg=malloc(2048);
	snprintf(notify_msg,1024, "%s", cfg_trigger_msg);
	//$VARS
	bartlby_replace_svc_in_str(notify_msg, svc, 2047);
	
	
	
	free(cfg_trigger_msg);
	
	
	
	
	
	trigger_dir=getConfigValue("trigger_dir", cfgfile);
	if(trigger_dir == NULL) {
		_log("bartlby_trigger() failed");
		return;	
	}
	dtrigger = opendir(trigger_dir);
	if(!dtrigger) {
		_log("opendir %s failed", trigger_dir);
		return;	
	}
	while((entry = readdir(dtrigger)) != NULL) {
		
		if(entry->d_name[0] == '.') continue;
		
		asprintf(&find_trigger, "|%s|" , entry->d_name);
		asprintf(&full_path, "%s/%s", trigger_dir, entry->d_name);
		if(lstat(full_path, &finfo) < 0) {
			free(find_trigger);
			free(full_path);
			_log("lstat() %s failed", full_path);
			return;	
		}
		
		if(bartlby_servergroup_has_trigger(svc->srv, find_trigger) != 1) {
			
			free(find_trigger);
			free(full_path);
			continue;
		}
		if(bartlby_servicegroup_has_trigger(svc, find_trigger) != 1) {
			
			free(find_trigger);
			free(full_path);
			continue;
		}
		
		if(strlen(svc->srv->enabled_triggers) > 2 && strstr(svc->srv->enabled_triggers, find_trigger) == NULL) {
			
			free(find_trigger);
			free(full_path);
			continue;
		}
		
		
		
		if(strlen(svc->enabled_triggers) > 2 && strstr(svc->enabled_triggers, find_trigger) == NULL) {
			//Service has selected wich triggers - and current trigger 'find_trigger' is not activated/selected
			free(find_trigger);
			free(full_path);
			continue;
			
		}
		
		if(S_ISREG(finfo.st_mode)) {
			
			for(x=0; x<hdr->wrkcount; x++) {
				if(service_is_in_time(wrkmap[x].notify_plan) > 0) {
					if(bartlby_worker_has_service(&wrkmap[x], svc, cfgfile) != 0 || do_check == 0) {
						if(strstr(wrkmap[x].enabled_triggers, find_trigger) != NULL || strlen(wrkmap[x].enabled_triggers) == 0) {
							
							
							if((bartlby_trigger_escalation(&wrkmap[x], svc, standby_workers_only)) == FL) continue;
							if((bartlby_trigger_worker_level(&wrkmap[x], svc)) == FL) continue;
								
							/* if standby escalation message check if worker is in standby mode either skip him/her*/
							if(standby_workers_only == 1 && wrkmap[x].active != 2) continue;
							
							en.trigger = entry->d_name;
							en.svc = svc;
							en.wrk = &wrkmap[x];
							bartlby_callback(EXTENSION_CALLBACK_TRIGGER_FIRED, &en);
							
							//_log("EXEC trigger: %s", full_path);
							_log("@NOT@%ld|%d|%d|%s|%s|%s:%d/%s", svc->service_id, svc->last_state ,svc->current_state,entry->d_name,wrkmap[x].name, svc->srv->server_name, svc->srv->client_port, svc->service_name);
							
							svc->last_notify_send=time(NULL);
							svc->srv->last_notify_send=time(NULL);
							wrkmap[x].escalation_time=time(NULL);
							asprintf(&exec_str, "%s \"%s\" \"%s\" \"%s\" \"%s\"", full_path, wrkmap[x].mail,wrkmap[x].icq,wrkmap[x].name, notify_msg);
							ptrigger=popen(exec_str, "r");
							if(ptrigger != NULL) {
								connection_timed_out=0;
								alarm(CONN_TIMEOUT);
								if(fgets(trigger_return, 1024, ptrigger) != NULL) {
									trigger_return[strlen(trigger_return)-1]='\0';
									_log("@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'%s'", svc->service_id, svc->last_state ,svc->current_state,entry->d_name,wrkmap[x].name, svc->srv->server_name, svc->srv->client_port, svc->service_name, trigger_return);
									
      								} else {
      									_log("@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'(empty output)'", svc->service_id, svc->last_state ,svc->current_state,entry->d_name,wrkmap[x].name, svc->srv->server_name, svc->srv->client_port, svc->service_name);
      								}
      								
      								if(connection_timed_out == 1) {
      									_log("@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'(timed out)'", svc->service_id, svc->last_state ,svc->current_state,entry->d_name,wrkmap[x].name, svc->srv->server_name, svc->srv->client_port, svc->service_name);
      								}
      								connection_timed_out=0;
								alarm(0);
								if(ptrigger != NULL) {
      									pclose(ptrigger);
      								}
      							} else {
      								_log("@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'(failed %s)'", svc->service_id, svc->last_state ,svc->current_state,entry->d_name,wrkmap[x].name, svc->srv->server_name, svc->srv->client_port, svc->service_name, full_path);	
      							}
							free(exec_str);
						} else {
							//_log("Worker: %s does not have trigger: %s", wrkmap[x].name, entry->d_name);
						}
						
					}
				}
			}	
		}
				
		
		
		free(full_path);
		free(find_trigger);
	}
	
	free(find_str);
	
	
	free(notify_msg);
	closedir(dtrigger);
}








