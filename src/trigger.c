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

#define DEFAULT_NOTIFY_MSG "State Change ($READABLE_STATE$)\n*********** $PROGNAME$ $VERSION$ ********************\n[  Server: $SERVER$, Service: $SERVICE$, State: $READABLE_STATE$]\n$MESSAGE$"
#define FL 0
#define TR 1

//#define TRIGGER_DEBUG
//#define TRIGGER_DEBUG_LOG

#ifdef TRIGGER_DEBUG
#define trigger_debug(format, ...) fprintf(stderr,format, ##__VA_ARGS__);
	#ifdef TRIGGER_DEBUG_LOG
		#undef trigger_debug
		#define trigger_debug(format, ...) _debug(format, ##__VA_ARGS__); 
	#endif
#else
#define trigger_debug(format, ...) ;
#endif


void bartlby_trigger_setup_env(struct service * svc, struct worker * wrk) {
	char * current_state;
	char * svc_id;
	char * srv_id;

	char * human_state;
	char * human_state_last;
	

        if(svc != NULL) {
	human_state=bartlby_beauty_state(svc->current_state);
	human_state_last=bartlby_beauty_state(svc->last_state);


	CHECKED_ASPRINTF(&current_state, "%d", svc->current_state);
	CHECKED_ASPRINTF(&svc_id, "%ld", svc->service_id);
	CHECKED_ASPRINTF(&srv_id, "%ld", svc->server_id);


	setenv("BARTLBY_TRIGGER_SVC_NAME", svc->service_name,1);
	setenv("BARTLBY_TRIGGER_SVC_SRV_NAME", svc->srv->server_name,1);
	setenv("BARTLBY_TRIGGER_SVC_OUTPUT", svc->current_output,1);

	setenv("BARTLBY_TRIGGER_SVC_CURRENT_STATE", current_state,1);
	setenv("BARTLBY_TRIGGER_SVC_ID", svc_id,1);
	setenv("BARTLBY_TRIGGER_SRV_ID", srv_id,1);
	
	setenv("BARTLBY_TRIGGER_SVC_CURRENT_STATE_READABLE", human_state, 1);
	setenv("BARTLBY_TRIGGER_SVC_CURRENT_LAST_STATE_READABLE", human_state_last, 1);

	}
	
	//AT LEAST WE CAN SET THE WORKER INFO
	setenv("BARTLBY_TRIGGER_WRK_EMAIL", wrk->mail,1);
	setenv("BARTLBY_TRIGGER_WRK_NAME", wrk->name,1);
	setenv("BARTLBY_TRIGGER_WRK_MOBILENR", wrk->mobilenr,1);
	


	
	if(svc != NULL) {
		free(current_state);
		free(svc_id);
		free(srv_id);
		free(human_state_last);
		free(human_state);
	}


}
char * bartlby_trigger_get_message(char * cfgfile, struct service * svc, struct worker * wrk, struct trigger * trig) {
	char * cfg_trigger_msg;
	char * notify_msg;

	if(svc == NULL) return strdup("SVC EMPTY");
	cfg_trigger_msg=getConfigValue("trigger_msg", cfgfile);

	if(cfg_trigger_msg == NULL) {
		cfg_trigger_msg=strdup(DEFAULT_NOTIFY_MSG);	
	}


	notify_msg=malloc(2048);
	snprintf(notify_msg,1024, "%s", cfg_trigger_msg);
	bartlby_replace_svc_in_str(notify_msg, svc, 2047);
	
	
	
	free(cfg_trigger_msg);

	trigger_debug("%s\n", notify_msg);

	return notify_msg;
}

void bartlby_trigger_wrap(char * cfgfile, struct service * svc, struct worker * wrk, struct trigger * trig, char * prebuilt_message) {

	int has_to_free_message = 0;
	if(prebuilt_message == NULL) {
		prebuilt_message=bartlby_trigger_get_message(cfgfile, svc, wrk, trig);
		has_to_free_message=1;
	}
	bartlby_trigger_setup_env(svc, wrk);
	trigger_debug("MSG: %s", prebuilt_message);
	switch(trig->trigger_type) {
		case TRIGGER_TYPE_LOCAL:
			bartlby_trigger_local(cfgfile, svc, wrk, trig, prebuilt_message);
		break;	
		case TRIGGER_TYPE_WEBHOOKS:
			bartlby_trigger_webhooks(cfgfile,svc,wrk,trig,prebuilt_message);
		break;
		case TRIGGER_TYPE_SMTP:
			bartlby_trigger_smtp(cfgfile,svc,wrk,trig,prebuilt_message);
		break;
		case TRIGGER_TYPE_LUA:
			bartlby_trigger_lua(cfgfile,svc,wrk,trig,prebuilt_message);
		break;
		default:
			_debug("Trigger type: %d is unkown -> Name: %s, ID:%ld", trig->trigger_type, trig->trigger_name, trig->trigger_id);
			

	}
	if(has_to_free_message == 1) {
		free(prebuilt_message);
	}
	



}

int bartlby_trigger_per_worker(char * cfgfile,
									void * shm_addr,
									struct worker * wrk,
									struct trigger * trig,
									struct service * svc,
									int type_of_notification,
									int do_check,
									int received_via,
									int upstream_enabled,
									int upstream_has_local_users,
									char * find_trigger,
									char * prebuilt_message) {
	struct shm_header * hdr;
	struct server * srvmap;
	

	char * srv_name=NULL;
	int srv_port;

	
	
	
	

	int notification_log_last_state;

	
	switch(type_of_notification) {
						case NOTIFICATION_TYPE_SIRENE:
							trigger_debug("\t>>SIRENE\n");

						break;
						case NOTIFICATION_TYPE_NORMAL:
							trigger_debug("\t>>NORMAL\n");
						break;
						case NOTIFICATION_TYPE_RENOTIFY:
							trigger_debug("\t>>RENOTIFY\n");
						break;

						case NOTIFICATION_TYPE_ESCALATION:
							trigger_debug("\t>>ESCALATION\n");
						break;

	}


	hdr = bartlby_SHM_GetHDR(shm_addr);
	srvmap = bartlby_SHM_ServerMap(shm_addr);
	
	
	trigger_debug("WORKER: %s plan: %s find_trigger: %s\n", wrk->name, wrk->notify_plan, find_trigger);
	if(service_is_in_time(wrk->notify_plan) > 0) {
		if(bartlby_worker_has_service(wrk, svc, cfgfile, 0) != 0 || do_check == 0) {
			if(strstr(wrk->enabled_triggers, find_trigger) != NULL || strlen(wrk->enabled_triggers) == 0) {
					if((bartlby_trigger_escalation(wrk, svc, type_of_notification, 0)) == FL) { trigger_debug("\t-ESCALATION REACHED\n");  return -2; }
					if((bartlby_trigger_worker_level(wrk, svc, 0)) == FL) { trigger_debug("\t-TRIGGER_DEBUG WORKER MISSING LEVEL\n");   return -2; }
					/* if re-notify - and user is not active continue */
					if(type_of_notification == NOTIFICATION_TYPE_RENOTIFY && wrk->active != 1) { trigger_debug("\t-RENOTIFY BUT USER INACTIVE\n");   return -2; }
					/* if standby escalation message check if worker is in standby mode either skip him/her*/
					if(type_of_notification == NOTIFICATION_TYPE_ESCALATION && wrk->active != 2) { trigger_debug("\t-STANDBY-ESCALATION but worker is not standby\n")  return -2;}
					if(type_of_notification == NOTIFICATION_TYPE_NORMAL) { //if it is not a renotify, either standby escalation or normal notification
						//Check what was the last state that got send to THIS user
					
						if(svc != NULL) {
							notification_log_last_state=bartlby_notification_log_last_notification_state(hdr,cfgfile,  svc->service_id, wrk->worker_id, trig);
							if(notification_log_last_state != -1) { //If no log entry found be nice and send it out
								if(notification_log_last_state == svc->current_state) {
									_log(LH_TRIGGER, B_LOG_DEBUG,"FIX  FLAPPING BUG - %d - %d (%d) - svc_id: %d", notification_log_last_state, svc->current_state, type_of_notification, svc->service_id);
									trigger_debug("\t-FLAPPING BUG\n");
									return -2;
								}
							}
						}


					}

					if(svc != NULL) {
						svc->last_notify_send=time(NULL);
						if(svc->srv_place >= 0) {
							srvmap[svc->srv_place].last_notify_send=time(NULL);
							srv_name=(char*)&srvmap[svc->srv_place].server_name;
							srv_port=srvmap[svc->srv_place].client_port;
						} else {
							//We are upstreamed - no server info
							srv_name=strdup("Upstreamed Host");
							srv_port=9030;
						}

						wrk->escalation_time=time(NULL);
					}

					

					if(svc != NULL && type_of_notification != NOTIFICATION_TYPE_AGGREGATE) {					
						_log(LH_TRIGGER, B_LOG_HASTO,"@NOT@%ld|%d|%d|%s|%s|%s:%d/%s (%d)", svc->service_id, svc->last_state ,svc->current_state,trig->trigger_name,wrk->name, srv_name ,srv_port, svc->service_name, wrk->notification_aggregation_interval);
					} else {
						_log(LH_TRIGGER, B_LOG_HASTO,"@AGG@%s|%s|%s",trig->trigger_name, wrk->name,  prebuilt_message);
					}
					

					if(svc != NULL) {
						if(svc->srv_place < 0) {
							//if it was upstreamed
							
							free(srv_name);
						}

						bartlby_notification_log_add(hdr, cfgfile, wrk->worker_id, svc->service_id, svc->current_state, type_of_notification, wrk->notification_aggregation_interval,  trig, received_via);
						if(wrk->notification_aggregation_interval > 0 && type_of_notification != NOTIFICATION_TYPE_AGGREGATE ) { // 3 == THE AGGREGATION MESSAGE ITSELF
							//As we aggregate the notifications - skip the execution of the trigger
							trigger_debug("\t@@@AGG\n");
							return -2;
						}
						
					}
													
					

					
					/*
					//// DO IT WITH A JSON TRIGGER - if you have local users
					if(upstream_enabled == 1 && upstream_has_local_users == 1) {
						
						_log(LH_TRIGGER, B_LOG_HASTO,"@UPSTREAM-NOT-USER@ - TRIGGER: %s  local_users: %d  type_of_notification:%d cmdline `%s'", trig->trigger_name,  upstream_has_local_users, type_of_notification, NULL);
						bartlby_trigger_upstream(cfgfile, upstream_has_local_users, type_of_notification, trig, svc);
						fprintf(stderr, "UPSTR\n");
						return -2;
					}*/

					
					bartlby_trigger_wrap(cfgfile, svc, wrk, trig, prebuilt_message);
					trigger_debug("\t+NOTIFICATION SENT!\n");
						
			} else {
				trigger_debug("\t-worker does not have trigger\n");
			}
		} else {
			trigger_debug("\t-worker does not have service\n");
		}
	} else {
		trigger_debug("\t-worker timeplan missmatch\n");
	}
	return 0;


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

int bartlby_trigger_worker_level(struct worker * w,  struct service * svc, int node_id) {
	char * find_level, * last_level;
	char * blevel;
	int rt;
	int level;
	int last;
	
	if(svc == NULL) return TR;

	level = svc->current_state;
	last  = svc->notify_last_state;
	
	blevel=bartlby_beauty_state(level);
	
	CHECKED_ASPRINTF(&last_level, "|%d|", last);
	CHECKED_ASPRINTF(&find_level, "|%d|",level);
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
int bartlby_trigger_escalation(struct worker *w, struct service * svc, int standby_workers_only, int node_id) {
	
	if(svc == NULL) return TR;
	if(standby_workers_only == NOTIFICATION_TYPE_NORMAL && w->active != 1) {
		//_log("Worker: %s is inactive", w->mail);
		return FL;	
	}
	if((time(NULL) - w->escalation_time) >= (w->escalation_minutes*60)) {
		w->escalation_count=0;
		return TR;	
	} else {
		if(w->escalation_count > w->escalation_limit) {
			if(node_id <= 0) {
				_log(LH_TRIGGER, B_LOG_DEBUG,"@NOT-EXT@%ld|%d|%d|%s||%s:%d/%s|'(escalation %d/%ld)'", svc->service_id, svc->last_state ,svc->current_state,w->name, svc->srv->server_name, svc->srv->client_port, svc->service_name,w->escalation_count, w->escalation_limit);
			} else {
				_log(LH_TRIGGER, B_LOG_DEBUG,"@NOT-EXT@ UPSTREAM Escalation on worker");
			}
			return FL;
		} else {
			w->escalation_count++;
			return TR;	
		}	
	}
}

int bartlby_trigger_enabled(struct service *svc) {
	if(svc == NULL) return TR;
	if(sched_servergroup_notify(svc->srv) == 0) {
		_log(LH_TRIGGER, B_LOG_DEBUG,"@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Notifications disabled on this servergroup)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name);
		return FL;
	}
	if(sched_servicegroup_notify(svc) == 0) {
		_log(LH_TRIGGER, B_LOG_DEBUG,"@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Notifications disabled on this servicegroup)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name);
		return FL;
	}
	
	
	
	if(svc->srv->server_notify == 0) {
		_log(LH_TRIGGER, B_LOG_DEBUG,"@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Notifications disabled on this server)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name);
		return FL;
	}
	if(svc->notify_enabled == 0) {
		//_log("Suppressed notify: Notifications disabled %s:%d/%s",svc->client_ip, svc->srv->client_port, svc->service_name);
		_log(LH_TRIGGER, B_LOG_DEBUG,"@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Notifications disabled)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name);
		return FL;
	} else {
		if((time(NULL) - svc->srv->last_notify_send) > svc->srv->server_flap_seconds) {
			svc->srv->flap_count=0;	
		} else {
			if(svc->srv->flap_count > 2) {
				_log(LH_TRIGGER, B_LOG_DEBUG,"@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Server lazy %ld)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name, svc->flap_count);
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
				_log(LH_TRIGGER, B_LOG_DEBUG,"@NOT-EXT@%ld|%d|%d|||%s:%d/%s|'(Service flapping %ld)'", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name, svc->flap_count);
				return FL;
			} else {
				//Log("trigger", "Service %s:%d/%s Sent", svc->client_ip, svc->srv->client_port, svc->service_name);	
				svc->flap_count++;
				return TR;
			}
			
		}
	}
					
  

	
}

int bartlby_worker_has_service(struct worker * w, struct service * svc, char * cfgfile, int node_id) {


	char * visible_servers, * visible_services;
	char * selected_servers, * selected_services;
	int is_super_user;
	char * find_server;
	char * find_service;
	
	int the_state;
	
	
	the_state = 0; //default zero
	
	if(svc == NULL) return 1;	
	
	//_log("user_right_file: %s", user_dat);
	
	visible_servers = w->visible_servers;
	visible_services = w->visible_services;
	
	is_super_user=w->is_super_user;
	selected_servers = w->selected_servers;
	selected_services = w->selected_services;
	
	
	

	
	if(node_id <= 0) {
		CHECKED_ASPRINTF(&find_server, ",%ld,", svc->server_id);
		CHECKED_ASPRINTF(&find_service, ",%ld,", svc->service_id);
	} else {
		CHECKED_ASPRINTF(&find_server, ",%d-%ld,",node_id, svc->server_id);
		CHECKED_ASPRINTF(&find_service, ",%d-%ld,",node_id, svc->service_id);
	}
	
	
	//_log("@TRIG@ visible_servers: %s; visible_services:%s; super_user: %s;", visible_servers, visible_services, is_super_user);
	
	
	if(strstr(visible_servers, find_server) == NULL) {
		the_state=0;	
		if(is_super_user == 1 && svc->notify_super_users == 1) {
			the_state=6;
		}
	} else {
		the_state=1; //temp its ok	
	}
	
	if(the_state == 0 && strstr(visible_services, find_service) == NULL) {
		the_state = 0;
		if(is_super_user == 1 && svc->notify_super_users == 1) {
			the_state=6;
		}		
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
	
	
	

	
	
	
	free(find_server);
	free(find_service);
		

	return the_state;
}





void bartlby_trigger_upstream(char * cfgfile, int has_local_users, int type_of_notification, struct trigger * trig, struct service * svc) {
	//GET CONFIG
	//UPSTREAM_HOST:
	//UPSTREAM_PORT:
	//UPSTREAM_NODE ID

	char * cfg_upstream_host;
	char * cfg_upstream_port;
	char * portier_passwd;
	char * cfg_upstream_my_node_id;
	int upstream_port;
	int node_id=0;
	int rtc;

	cfg_upstream_host = getConfigValue("upstream_host", cfgfile);
	cfg_upstream_port = getConfigValue("upstream_port", cfgfile);
	cfg_upstream_my_node_id = getConfigValue("upstream_my_node_id", cfgfile);
	
	portier_passwd = getConfigValue("portier_password", cfgfile);
	
	
	if(cfg_upstream_port == NULL) {
		//cfg_upstream_port=strdup("false");	
		upstream_port=9031;
	} else {
		upstream_port=atoi(cfg_upstream_port);
	}
	if(cfg_upstream_my_node_id == NULL) {
		//cfg_upstream_port=strdup("false");	
		node_id=-1;
	} else {
		node_id=atoi(cfg_upstream_my_node_id);
	}

	

	if(cfg_upstream_host == NULL || node_id < 0 || portier_passwd == NULL) {
		_log(LH_TRIGGER, B_LOG_CRIT,"Misconfigured upstream either upstream_host or upstream_my_node_id or portier_password is not set");

		if(portier_passwd != NULL) free(portier_passwd);
		if(cfg_upstream_host != NULL) free(cfg_upstream_host);
		if(cfg_upstream_port != NULL) free(cfg_upstream_port);
		if(cfg_upstream_my_node_id != NULL) free(cfg_upstream_my_node_id);

		return;
	}

	

	rtc=-1;
	if(has_local_users == 1) {
		//rtc=bartlby_portier_send_trigger(cfg_upstream_host, upstream_port, type_of_notification,trig, NULL, node_id, portier_passwd, cfgfile);
	} else {
		rtc=bartlby_portier_send_trigger(cfg_upstream_host, upstream_port, type_of_notification, trig, svc, node_id, portier_passwd,cfgfile);
	}
	if(rtc < 0 ) {
		_log(LH_TRIGGER, B_LOG_CRIT,"Notification Upstream failed");
	}
	
	free(portier_passwd);
	free(cfg_upstream_port);
	free(cfg_upstream_host);
	free(cfg_upstream_my_node_id);
}

void bartlby_trigger( struct service * svc,
						  char * cfgfile,
						  void * shm_addr,
						  int do_check,
						  int type_of_notification,
						  struct worker * specific_worker,
						  struct trigger * specific_trigger,
						  char * prebuilt_message
						  ) {
	/*
		loop threw trigger map
			exclude on parameters
				loop threw workermap
					call trigger per worker
						switch type
							call real_trigger


	following calls
		##### SIRENE
		do_check = 0
		type_of_notification = NOTIFICATION_TYPE_SIRENE

		#NORMAL (first) notification
		do_check = 1
		type_of_notification = NOTIFICATION_TYPE_NORMAL

		# RE-NOTIFICATION (2nd)
		do_check = 1
		type_of_notification = NOTIFICATION_TYPE_RENOTIFY 

		# escalation message (sends notify to standby workers)
		do_check = 1
		type_of_notification =  NOTIFICATION_TYPE_ESCALATION




	*/
	long x,y;
	struct trigger * triggermap;
	struct shm_header * shmhdr;
	struct worker * workermap;
	char * find_trigger;

	//UPSTREAM NOTIFICATIONS:
	char * cfg_upstream_enabled;
	char * cfg_upstream_has_local_users;
	
	int lua_return;
	int upstream_enabled;
	int upstream_has_local_users;
	

	cfg_upstream_enabled = getConfigValue("upstream_enabled", cfgfile);
	cfg_upstream_has_local_users = getConfigValue("upstream_has_local_users", cfgfile);
	

	
	if(cfg_upstream_enabled == NULL) {
		cfg_upstream_enabled=strdup("false");	
	}
	if(strcmp(cfg_upstream_enabled, "true") == 0) {
		upstream_enabled=1; 
	} else {
		upstream_enabled=0;
	}
	if(cfg_upstream_has_local_users == NULL) {
		cfg_upstream_has_local_users=strdup("false");	
	}
	if(strcmp(cfg_upstream_has_local_users, "true") == 0) {
		upstream_has_local_users=1;
	} else {
		upstream_has_local_users=0;
	}
	free(cfg_upstream_enabled);
	free(cfg_upstream_has_local_users);
	///UPSTEAM NOTIFICATIONS

	

	shmhdr = bartlby_SHM_GetHDR(shm_addr);
	triggermap = bartlby_SHM_TriggerMap(shm_addr);
	workermap = bartlby_SHM_WorkerMap(shm_addr);


	trigger_debug("##SITUATION: %d/%d\n", do_check, type_of_notification);
	if(svc != NULL) {
		if(svc->handled == SERVICE_HANDLED) {
			trigger_debug("\t--SERVICE_HANDLED SKIP NOTIFICATION\n");
			return;
		}
		lua_return=0;
		if(svc->script_enabled == 1 && strlen(svc->script) > 3) {
			lua_return=bartlby_trigger_script(svc, svc->script);
			if(lua_return == -1) {
				trigger_debug("\t-- LUA Script canceld notification\n");
				return;

			}
			if(lua_return == -2) {
				//Passthrough this trigger msg
				type_of_notification=NOTIFICATION_TYPE_AGGREGATE;
			}
		}
	}


	if(do_check == 1 && svc != NULL) {
		if(bartlby_trigger_enabled(svc) == FL) {
			trigger_debug("\t-- SKIP - do check\n");
			return;	
		}
	}
	
	//LOOP
	for(x=0; x<shmhdr->triggercount; x++) {
		if(specific_trigger != NULL) {
			if(triggermap[x].trigger_id != specific_trigger->trigger_id) {
				continue;
			}
		}
		trigger_debug("\t @@ Triggername: %s\n", triggermap[x].trigger_name);
		if(triggermap[x].trigger_enabled == 0) {
			trigger_debug("\t-- TRIGGER IS DISABLED\n");
			continue;
		}

		
		CHECKED_ASPRINTF(&find_trigger, "|%ld|" , triggermap[x].trigger_id);
		
		if(do_check == 1 && svc != NULL) {
			if(bartlby_servergroup_has_trigger(svc->srv, find_trigger) != 1) {
				free(find_trigger);
				trigger_debug("\t-- SERVERGROUP MISSING TRIGGER\n");
				continue;
			}
			if(bartlby_servicegroup_has_trigger(svc, find_trigger) != 1) {
				trigger_debug("\t-- SERVICEGROUP MISSING TRIGGER\n");
				free(find_trigger);
				continue;
			}
				
			if(strlen(svc->srv->enabled_triggers) > 2 && strstr(svc->srv->enabled_triggers, find_trigger) == NULL) {
				trigger_debug("\t-- SERVER MISSING TRIGGER\n");
				free(find_trigger);
				continue;
			}
		}			

		
		if(upstream_enabled == 1 && upstream_has_local_users == 0 && svc != NULL) {
				_debug("@UPSTREAM-NOT-TOP@ - TRIGGER: %s  local_users: %d  type_of_notification:%d", triggermap[x].trigger_name,  upstream_has_local_users, type_of_notification);
				svc->last_notify_send=time(NULL);
				svc->srv->last_notify_send=time(NULL);
				bartlby_trigger_upstream(cfgfile,upstream_has_local_users, type_of_notification, &triggermap[x], svc);
				continue;
		}
		


		for(y=0; y<shmhdr->wrkcount; y++) {
			if(specific_worker != NULL) {
				if(workermap[y].worker_id != specific_worker->worker_id) {
					continue;
				}
			}
			bartlby_trigger_per_worker(cfgfile, shm_addr, &workermap[y], &triggermap[x], svc, type_of_notification, do_check, NOTIFICATION_VIA_LOCAL, upstream_enabled, upstream_has_local_users, find_trigger, prebuilt_message);
		}	


		trigger_debug("\t -- GO FOR %s (%s)\n", triggermap[x].trigger_name, triggermap[x].trigger_data);
		free(find_trigger);
	}

	
 

}




















