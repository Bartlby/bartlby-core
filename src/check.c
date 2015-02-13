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

void bartlby_check_eventhandler(struct service * svc, char * cfgfile) {
	//Copy service to new service - run check based on type with a replace plugin <EVENT>_PLUGIN PLUGIN_ARGS STATE RETAIN COUNT
	struct service event_service;
	int eventhandler_called;
	
	int is_hard;
	char state_level[20];
	
	is_hard=(svc->service_retain_current+1) % (svc->service_retain+1);
	
	if( is_hard == 0) {
		sprintf(state_level, "HARD");
		if(svc->fires_events == 2) { //only soft enabled
			return;
		}
	} else {
		sprintf(state_level, "SOFT");
		if(svc->fires_events == 1) { //only hard enabled
			return;
		}
	}
	
	memcpy(&event_service, svc, sizeof(struct service));
	
	//change plugin name - and args
	sprintf(event_service.plugin, "event_%s", svc->plugin);
	sprintf(event_service.plugin_arguments, "%s %s %d %ld", state_level, svc->plugin_arguments, svc->current_state, svc->service_retain_current);
	
	eventhandler_called=0;
	if(svc->service_type == SVC_TYPE_ACTIVE) {
		bartlby_check_active(&event_service,cfgfile);
		eventhandler_called=1;
	}
	if(svc->service_type == SVC_TYPE_LOCAL) {
		bartlby_check_local(&event_service,cfgfile);
		eventhandler_called=1;
	}
	
	if(svc->service_type == SVC_TYPE_NRPE) {
		bartlby_check_nrpe(&event_service, cfgfile, 0);	
		eventhandler_called=1;
	}    
	if(svc->service_type == SVC_TYPE_SSH) {
		bartlby_check_ssh(&event_service, cfgfile);	
		eventhandler_called=1;
	}                                                      
	if(svc->service_type == SVC_TYPE_NRPE_SSL) {
		bartlby_check_nrpe(&event_service, cfgfile, 1);
		eventhandler_called=1;

	}
	if(svc->service_type == SVC_TYPE_V2) {
		bartlby_check_v2(&event_service, cfgfile, 1);
		eventhandler_called=1;
	}
	if(svc->service_type == SVC_TYPE_V2_NO_SSL) {
		bartlby_check_v2(&event_service, cfgfile, 0);
		eventhandler_called=1;
	}
	if(eventhandler_called == 1) {
		
		_log(LH_CHECK, B_LOG_HASTO,"@EV-HANDLER@%ld|%d|%s:%d/%s|%s|(%s)-%s", svc->service_id, svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name, state_level, event_service.plugin, event_service.current_output);
		//
	}
	
}

void bartlby_check_grep_perf_line(char * l, struct service * svc, char * cfgfile) {
	char * tmp_line;
	int x;
	int lf_found=0;
	int n_orig=0;
	int p_line=0;
	char * pl;
	
	
	
	
	if(strncmp(l, "PERF: ", 6) == 0) {
		tmp_line = strdup(l);
		pl=strdup(l);
		
		for(x=0; x<strlen(tmp_line); x++) {
			if(lf_found == 0) {
				if(tmp_line[x] == '\n') {
					lf_found=1;
					continue;	
				}
				pl[p_line] = tmp_line[x];
				p_line++;
				
			} else {
				if(tmp_line[x] != '\n') {
					l[n_orig] = tmp_line[x];
					n_orig++;
						
				}	
			}
		}
		
		pl[p_line]='\0';
		l[n_orig]='\n';
		l[n_orig+1]='\0';
		
		bartlby_perf_track(svc,pl, strlen(pl), cfgfile);
				
		free(pl);
		free(tmp_line);		
	} else {
		return;	
	}
}






void bartlby_fin_service(struct service * svc, void * SOHandle, void * shm_addr,char * cfgfile) {
	char * dlmsg;
	char * cfg_instant_wb;
	
	
	json_object * jso;
	
	int do_log;
	
	
	int (*doUpdate)(struct service *,char *);
	
	do_log=0;
	

	if(svc->script_enabled == 1 && strlen(svc->script) > 3) {
		bartlby_finish_script(svc, svc->script);
		bartlby_callback(EXTENSION_CALLBACK_SCRIPT_FIRED, svc);
	}



	if(svc->current_state != svc->last_state) {
		svc->service_retain_current=0;
		svc->last_state=svc->current_state;
		svc->last_state_change=time(NULL);
		do_log=1;
		

		/******* PUSH SVC TO EVENT QUEUE *******/
		jso = json_object_new_object();
		json_object_object_add(jso,"type", json_object_new_string("Service-Changed"));
		json_object_object_add(jso,"service", bartlby_service_to_json(svc));
		bartlby_push_event(cfgfile, shm_addr, EVENT_STATUS_CHANGED, json_object_to_json_string(jso));
		json_object_put(jso);

		/******* PUSH SVC TO EVENT QUEUE *******/

		bartlby_callback(EXTENSION_CALLBACK_STATE_CHANGED, svc);

	}
	//LOG HARD STATE	
	//rename notify_last_state to last_hard_state !? would make much more sense :D
	
	if(svc->service_retain_current == svc->service_retain && svc->current_state != svc->notify_last_state) {
		do_log=2;

		/******* PUSH SVC TO EVENT QUEUE *******/
		jso = json_object_new_object();
		json_object_object_add(jso,"type", json_object_new_string("Trigger-Fired"));
		json_object_object_add(jso,"service", bartlby_service_to_json(svc));
		bartlby_push_event(cfgfile,shm_addr, EVENT_TRIGGER_PUSHED, json_object_to_json_string(jso));
		json_object_put(jso);
		/******* PUSH SVC TO EVENT QUEUE *******/

		
					
		if(svc->current_state == STATE_CRITICAL && svc->recovery_outstanding == RECOVERY_DONE) {
			svc->recovery_outstanding = RECOVERY_OUTSTANDING;	
		}
		
		bartlby_trigger(svc, cfgfile, shm_addr, 1, NOTIFICATION_TYPE_NORMAL, NULL, NULL, NULL);
		svc->notify_last_state=svc->current_state;
		
		if(svc->current_state == STATE_OK && svc->recovery_outstanding == RECOVERY_OUTSTANDING) {
			svc->recovery_outstanding=RECOVERY_DONE;
			
		}
		
		if(svc->service_ack_enabled == 1 && svc->current_state == STATE_CRITICAL) {
			svc->service_ack_current=ACK_OUTSTANDING;	
		}
		
		// if service is dead / and retain is reached (see above if) - call event handler
		
		
		
		
	
	}
	if(do_log > 0) {
		
		switch(do_log) {
			case 1:
				_log(LH_CHECK, B_LOG_HASTO,"@LOG@%ld|%d|%s:%d/%s|%s|SOFT", svc->service_id, svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name, svc->current_output);
				
			break;
			case 2:
				_log(LH_CHECK, B_LOG_HASTO,"@LOG@%ld|%d|%s:%d/%s|%s|HARD", svc->service_id, svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name, svc->current_output);
				
			break;
		}
		
		

		//FIRE Web Hooks?
		if(svc->srv->web_hooks_level == 3) {
			//HOOKS ON BOTH
			bartlby_call_webhooks(cfgfile, svc,svc->service_check_timeout, svc->srv->web_hooks, NULL);
		} else {
			if(svc->srv->web_hooks_level == 1) {
				//ONLY HARD
				if(do_log == 2) {
					bartlby_call_webhooks(cfgfile, svc,svc->service_check_timeout, svc->srv->web_hooks, NULL);	
				}
			}
			if(svc->srv->web_hooks_level == 2) {
				if(do_log == 1) {
					//ONLY SOFT
					bartlby_call_webhooks(cfgfile, svc,svc->service_check_timeout, svc->srv->web_hooks, NULL);
				}
			}

		}
		bartlby_callback(EXTENSION_CALLBACK_WEBHOOKS_FIRED, svc);
		
	}
	
	if(svc->current_state == STATE_CRITICAL && svc->fires_events > 0) {
				bartlby_check_eventhandler(svc, cfgfile);
	}
	if(svc->current_state == STATE_OK) {
		svc->handled=SERVICE_UNHANDLED;
	}
	/* Check if we need to re-notify */
	/* when retain in reached */
	/* current state is critical*/
	/* re-notify interval reached  (seen from last_notify) */
	
	//if ( (svc->service_retain_current >= svc->service_retain) && ( svc->current_state == STATE_CRITICAL )  && ( svc->renotify_interval > 0 ) && ( (time(NULL)-svc->last_notify_send) >= svc->renotify_interval ) ) {
	if ( (svc->service_retain_current >= svc->service_retain) && ( svc->current_state == STATE_CRITICAL )  && ( svc->renotify_interval > 0 ) && ( svc->service_retain_current % svc->renotify_interval == 0 ) ) {
		_log(LH_CHECK, B_LOG_CRIT,"re-notify	 for %s:%d/%s", svc->srv->server_name,svc->srv->client_port, svc->service_name);
		bartlby_trigger(svc, cfgfile, shm_addr, 1, NOTIFICATION_TYPE_RENOTIFY, NULL, NULL, NULL);
		svc->notify_last_state=svc->current_state;
		
	}
	/* Check if we need to escalate and enable standby workers */
	/* when retain in reached */
	/* current state is critical*/
	/* retain_current >= escalate_value */ 
	
	if ( (svc->service_retain_current >= svc->service_retain) && ( svc->current_state == STATE_CRITICAL )  && ( svc->escalate_divisor > 0 ) && ( svc->service_retain_current % svc->escalate_divisor == 0 ) ) {
		_log(LH_CHECK, B_LOG_CRIT,"escalate to standby workers	 for %s:%d/%s", svc->srv->server_name,svc->srv->client_port, svc->service_name);
		bartlby_trigger(svc, cfgfile, shm_addr, 1,  NOTIFICATION_TYPE_ESCALATION, NULL, NULL, NULL);
		
		
	}
	
	
	
	
	
	bartlby_callback(EXTENSION_CALLBACK_POST_CHECK, svc);
	
	if(svc->service_retain_current == LONG_MAX) {
		svc->service_retain_current=svc->service_retain;
	}
	svc->service_retain_current++;

	
	
	cfg_instant_wb = getConfigValue("instant_write_back", cfgfile);
	if(cfg_instant_wb == NULL) {
		cfg_instant_wb=strdup("true");	
	}
	
	if(strcmp(cfg_instant_wb, "true") == 0) {
		LOAD_SYMBOL(doUpdate,SOHandle, "doUpdate");
		doUpdate(svc,cfgfile);
	}
	
	bartlby_orchestra_send_svc(cfgfile,svc); 

	free(cfg_instant_wb);
	
	
	
}

void bartlby_check_service(struct service * svc, void * shm_addr, void * SOHandle, char * cfgfile) {
	//_log("check service");
	int ctime, pdiff;
	//_log("<%d/%d -- CHECK >: %s",svc->current_state,svc->last_state, svc->service_name);
	
	setenv("BARTLBY_CONFIG", cfgfile,1);
	setenv("BARTLBY_CURR_PLUGIN", svc->plugin,1);
	setenv("BARTLBY_CURR_HOST", svc->srv->server_name,1);
	setenv("BARTLBY_CURR_SERVICE", svc->service_name,1);
	
	if(bartlby_callback(EXTENSION_CALLBACK_PRE_CHECK, svc) != EXTENSION_OK) {
			/*
			a extension canceld the check
			*/
			return;	
	}	
	if(svc->service_type == SVC_TYPE_GROUP) {
		bartlby_check_group(svc, shm_addr);
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;	
	}
	if(svc->service_type == SVC_TYPE_TRAP) {
		//Flags are changed inside the portier part
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;	
	}
	if(svc->service_type == SVC_TYPE_PASSIVE) {
		ctime=time(NULL);
		pdiff=ctime-svc->last_check;
		
		if(svc->service_passive_timeout > 0 && pdiff >= svc->service_passive_timeout) {
			
			sprintf(svc->current_output, "%s", PASSIVE_TIMEOUT);
			if(pdiff >= svc->service_passive_timeout * 2) {
				svc->current_state=STATE_CRITICAL;
			} else {
				svc->current_state=STATE_WARNING;
			}
			
			
		}
		//_log("PASSIVE_CHECK %d->%d", svc->service_passive_timeout, svc->service_id);
		bartlby_check_grep_perf_line(svc->current_output, svc, cfgfile);
		
		bartlby_fin_service(svc, SOHandle,shm_addr,cfgfile);
		return;	
	}
	if(svc->service_type == SVC_TYPE_JSON) {
		bartlby_check_json(svc,cfgfile);
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		
		return;		
	}	
	if(svc->service_type == SVC_TYPE_ACTIVE) {
		bartlby_check_active(svc,cfgfile);
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;		
	}
	if(svc->service_type == SVC_TYPE_LOCAL) {
		bartlby_check_local(svc,cfgfile);
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;		
	}
	if(svc->service_type == SVC_TYPE_SNMP) {
		
		bartlby_check_snmp(svc,cfgfile);
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;
	}
	if(svc->service_type == SVC_TYPE_NRPE) {
		bartlby_check_nrpe(svc, cfgfile, 0);	
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;
	}                                                       
	if(svc->service_type == SVC_TYPE_NRPE_SSL) {
		bartlby_check_nrpe(svc, cfgfile, 1);	           
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;
	
	}
	if(svc->service_type == SVC_TYPE_SSH) {
		bartlby_check_ssh(svc, cfgfile);	           
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;
	
	}
	if(svc->service_type == SVC_TYPE_V2) {
		bartlby_check_v2(svc, cfgfile, 1);	           
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;	
	}
	if(svc->service_type == SVC_TYPE_V2_NO_SSL) {
		bartlby_check_v2(svc, cfgfile, 0);	           
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;	
	}
	//
	if(bartlby_callback(EXTENSION_CALLBACK_UNKOWN_CHECK_TYPE, svc) != EXTENSION_OK) {
		bartlby_fin_service(svc,SOHandle,shm_addr,cfgfile);
		return;
	} else {
		_log(LH_CHECK, B_LOG_CRIT,"Undefined service check type: %d", svc->service_type);
		sprintf(svc->current_output, "undefined service type (%d)", svc->service_type);
		svc->current_state=STATE_CRITICAL;
	}
	
	return;
}
