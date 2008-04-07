/* $Id$ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2008 Helmut Januschka - All Rights Reserved
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
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <bartlby.h>



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
	struct worker * wrkmap;
	char * cfg_instant_wb;
	
	struct shm_header * hdr;
	
	int (*doUpdate)(struct service *,char *);
	
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	wrkmap=bartlby_SHM_WorkerMap(shm_addr);
	
		
	if(svc->current_state != svc->last_state) {
		svc->service_retain_current=0;
		svc->last_state=svc->current_state;
		
		_log("@LOG@%ld|%d|%s:%d/%s|%s", svc->service_id, svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name, svc->new_server_text);
		bartlby_push_event(EVENT_STATUS_CHANGED, "Service-Changed;%d;%s:%d/%s;%d;%s", svc->service_id, svc->srv->server_name, svc->srv->client_port, svc->service_name, svc->current_state, svc->new_server_text);
		bartlby_callback(EXTENSION_CALLBACK_STATE_CHANGED, svc);
	}	
	if(svc->service_retain_current == svc->service_retain && svc->current_state != svc->notify_last_state) {
	
		bartlby_push_event(EVENT_TRIGGER_PUSHED, "Service-Changed;%d;%s:%d/%s;%d;%s", svc->service_id, svc->srv->server_name, svc->srv->client_port, svc->service_name, svc->current_state, svc->new_server_text);
				
		if(svc->current_state == STATE_CRITICAL && svc->recovery_outstanding == RECOVERY_DONE) {
			svc->recovery_outstanding = RECOVERY_OUTSTANDING;	
		}
		
		bartlby_trigger(svc, cfgfile, shm_addr, 1, 0);
		svc->notify_last_state=svc->current_state;
		
		if(svc->current_state == STATE_OK && svc->recovery_outstanding == RECOVERY_OUTSTANDING) {
			svc->recovery_outstanding=RECOVERY_DONE;
			
		}
		
		if(svc->service_ack == ACK_NEEDED && svc->current_state == STATE_CRITICAL) {
			svc->service_ack=ACK_OUTSTANDING;	
		}
		
		
	
	}
	/* Check if we need to re-notify */
	/* when retain in reached */
	/* current state is critical*/
	/* re-notify interval reached  (seen from last_notify) */
	
	//if ( (svc->service_retain_current >= svc->service_retain) && ( svc->current_state == STATE_CRITICAL )  && ( svc->renotify_interval > 0 ) && ( (time(NULL)-svc->last_notify_send) >= svc->renotify_interval ) ) {
	if ( (svc->service_retain_current >= svc->service_retain) && ( svc->current_state == STATE_CRITICAL )  && ( svc->renotify_interval > 0 ) && ( svc->service_retain_current % svc->renotify_interval == 0 ) ) {
		_log("re-notify	 for %s:%d/%s", svc->srv->server_name,svc->srv->client_port, svc->service_name);
		bartlby_trigger(svc, cfgfile, shm_addr, 1, 0);
		svc->notify_last_state=svc->current_state;
		
	}
	/* Check if we need to escalate and enable standby workers */
	/* when retain in reached */
	/* current state is critical*/
	/* retain_current >= escalate_value */ 
	
	if ( (svc->service_retain_current >= svc->service_retain) && ( svc->current_state == STATE_CRITICAL )  && ( svc->escalate_divisor > 0 ) && ( svc->service_retain_current % svc->escalate_divisor == 0 ) ) {
		_log("escalate to standby workers	 for %s:%d/%s", svc->srv->server_name,svc->srv->client_port, svc->service_name);
		bartlby_trigger(svc, cfgfile, shm_addr, 1, 1);
		
		
	}
	
	
	
	
	
	bartlby_callback(EXTENSION_CALLBACK_POST_CHECK, svc);
	
	svc->service_retain_current++;
	
	
	
	cfg_instant_wb = getConfigValue("instant_write_back", cfgfile);
	if(cfg_instant_wb == NULL) {
		cfg_instant_wb=strdup("true");	
	}
	
	if(strcmp(cfg_instant_wb, "true") == 0) {
		LOAD_SYMBOL(doUpdate,SOHandle, "doUpdate");
		doUpdate(svc,cfgfile);
	}
	
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
	if(svc->service_type == SVC_TYPE_PASSIVE) {
		ctime=time(NULL);
		pdiff=ctime-svc->last_check;
		
		if(svc->service_passive_timeout > 0 && pdiff >= svc->service_passive_timeout) {
			
			sprintf(svc->new_server_text, "%s", PASSIVE_TIMEOUT);
			if(pdiff >= svc->service_passive_timeout * 2) {
				svc->current_state=STATE_CRITICAL;
			} else {
				svc->current_state=STATE_WARNING;
			}
			
			
		}
		//_log("PASSIVE_CHECK %d->%d", svc->service_passive_timeout, svc->service_id);
		bartlby_check_grep_perf_line(svc->new_server_text, svc, cfgfile);
		
		bartlby_fin_service(svc, SOHandle,shm_addr,cfgfile);
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
		_log("Undefined service check type: %d", svc->service_type);
		sprintf(svc->new_server_text, "undefined service type (%d)", svc->service_type);
		svc->current_state=STATE_CRITICAL;
	}
	
	return;
}
