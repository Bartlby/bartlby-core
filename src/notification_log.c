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
#define CONN_TIMEOUT 20

/*
Config options
	notification_aggregate = true|false (enables aggregation)
	notification_aggregation_interval = 120 (Seconds between aggregation runs, this is the delay time between the notifications - if <= the notification is ignored and its going to be delay until the notification is aged >=)
			the higher this value the harder the aggregation - less notifications will be sent out.
	notification_aggration_hardcore_limit = 10
			if matched services count >= a short variant is going to be used

TODO:

	bartlby_trigger()
		call bartlby_notification_log_last_notification_state before sending notification and see if it is a non-sense-same-state notification

	PORTIER:
		check for non local_user upstream aggregate there too
IDEA:
	rework - flapping detection?
*/

struct trigger_aggregate {
	char trigger_name[512];
	int notification_count;
	struct trigger_aggregate * next;
	struct trigger_aggregate * prev;
	struct service_aggregate * service_list_head;


};

struct service_aggregate {
	int service_id;
	struct service * svc;
	int states[4];
	int state_changes;
	struct service_aggregate * next;
	struct service_aggregate * prev;

};

struct worker_aggregate {
	int worker_id;
	struct trigger_aggregate * trigger_list_head;
	struct worker_aggregate * next;
	struct worker_aggregate * prev;
};




//PRAGMA HELPERS
void bartlby_notification_log_aggregate_destroy(struct worker_aggregate * current) {
	struct worker_aggregate  * wa, *w;
	struct trigger_aggregate * ta, *t;
	struct service_aggregate * sa, *s;

	while(current != NULL) {
			t=current->trigger_list_head;
			while(t != NULL) {
				s=t->service_list_head;
				if(t->notification_count == 0) s=NULL;
				while(s != NULL) {
					sa=s->next;
					free(s);
					s=sa;					
				}
				ta=t->next;
				free(t);
				t=ta;				

			}
			wa=current->next;
			free(current);
			current=wa;
	}
	

}

void * bartlby_notification_log_worker_exists(struct worker_aggregate * head, int worker_id) {
	struct worker_aggregate * cur;
	cur=head;
	while(cur != NULL) {
		if(cur->worker_id == worker_id) {
			return cur;
		}
		cur=cur->next;
	}
	return NULL;
}

void * bartlby_notification_log_trigger_exists(struct trigger_aggregate * head, char * trigger_name) {
	struct trigger_aggregate * cur;
	cur=head;
	while(cur != NULL) {
		if(strcmp(cur->trigger_name, trigger_name) == 0 ) {
			return cur;
		}
		cur=cur->next;
	}
	return NULL;
}



void * bartlby_notification_log_service_exists(struct service_aggregate * head, int service_id) {
	struct service_aggregate * cur;
	cur=head;
	while(cur != NULL) {
		if(cur->service_id == service_id) {
			return cur;
		}
		cur=cur->next;
	}
	return NULL;
}


struct trigger_aggregate * bartlby_notification_log_add_trigger(struct trigger_aggregate * head, char * trigger_name) {
	struct trigger_aggregate * current;
	current = head;
	 while (current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(struct trigger_aggregate));
    
    sprintf(current->next->trigger_name, "%s", trigger_name);
    current->next->next = NULL;
    current->next->prev=current;

    current->next->notification_count=0;

   	struct service_aggregate * service_list;
	service_list = malloc(sizeof(struct service_aggregate));
	service_list->next=NULL;
	service_list->prev=NULL;

	service_list->states[0]=0;
    service_list->states[1]=0;
    service_list->states[2]=0;
    service_list->states[3]=0;
    service_list->state_changes=0;
	service_list->service_id=-1;
	current->next->service_list_head=service_list;



    return current->next;

}

struct service_aggregate * bartlby_notification_log_add_service(struct service_aggregate * head, int val) {
	struct service_aggregate * current;
	current = head;
	 while (current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(struct service_aggregate));
    current->next->service_id = val;
    current->next->next = NULL;
    current->next->prev=current;
	
		current->next->state_changes=0;
    current->next->states[0]=0;
    current->next->states[1]=0;
    current->next->states[2]=0;
    current->next->states[3]=0;

    return current->next;

}
struct worker_aggregate * bartlby_notification_log_add_worker(struct worker_aggregate * head, int val) {
	struct worker_aggregate * current;
	current = head;
	 while (current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(struct worker_aggregate));
    current->next->worker_id = val;
    current->next->next = NULL;
    current->next->prev=current;


    //ADD Trigger List
    struct trigger_aggregate * trigger_list;
    trigger_list=malloc(sizeof(struct trigger_aggregate));
	trigger_list->next=NULL;
	trigger_list->prev=NULL;
	trigger_list->notification_count=0;
	sprintf(trigger_list->trigger_name, "null");
	

	//svc_list->trigger_list_head=trigger_list;
	trigger_list->service_list_head=NULL;
	current->next->trigger_list_head=trigger_list;

	return current->next;

}
//HELPERS



int bartlby_notification_log_last_notification_state(struct shm_header * shmhdr,char * cfgfile,  long svc_id, long worker_id, char * trigger_name) {
	/*
		input svc_id
		get last notification sent to svc_id - by walking threw array starting at current_top
		return int state


		this FIXES BUG with warning(no notify)->critical(notify)->warning(no-notify)->critical(again a notify) - bug

	*/
	int x;
	time_t max_ts, max_state;
	max_ts=0;
	max_state=-1;
	for(x=0; x<NOTIFICATION_LOG_MAX; x++) {
		if(shmhdr->notification_log[x].notification_valid != -1 && shmhdr->notification_log[x].service_id == svc_id && shmhdr->notification_log[x].worker_id == worker_id && shmhdr->notification_log[x].type == 0 && strcmp(shmhdr->notification_log[x].trigger_name, trigger_name) == 0) {
			if(shmhdr->notification_log[x].time > max_ts) {
				_log(LH_NOTIFYLOG, B_LOG_DEBUG,"FOUND x:%d WORKER_ID: %ld SERVICE_ID: %ld - state %d",x, shmhdr->notification_log[x].worker_id, shmhdr->notification_log[x].service_id, shmhdr->notification_log[x].state);
				max_ts=shmhdr->notification_log[x].time;
				max_state=shmhdr->notification_log[x].state;
			}
		}
	}
	return max_state;


}
void * bartlby_notification_log_set_hardcopy(struct shm_header * shmhdr, void * hardcopy, long notification_log_current_top, time_t notification_log_last_run) {
	/*
		memcpy hardcopy into shmhdr->notification_log
		set shmhdr->notification_log_current_top to notification_log_current_top
		free() - *hardcopy
	*/


	long notification_log_size;


	notification_log_size=sizeof(struct notification_log_entry)*NOTIFICATION_LOG_MAX;
	_log(LH_NOTIFYLOG, B_LOG_DEBUG,"NOTIFICATION_LOG: SET HARDCOPY: %d", notification_log_size);
	memcpy(shmhdr->notification_log, hardcopy, notification_log_size);
	free(hardcopy);
	shmhdr->notification_log_current_top=notification_log_current_top;
	shmhdr->notification_log_aggregate_last_run = notification_log_last_run;
	return NULL;
}


void * bartlby_notification_log_get_hardcopy(struct shm_header * shmhdr) {
	/*
	malloc
	return pointer to memcpy'd	binary data of notification log
	called before reload - so after reload - bartlby_notification_log_set_hardcopy() can be used to repopulate the notification log
	//AKA - reload survive!!
	*/
	void * return_hardcopy;
	long notification_log_size;


	notification_log_size=sizeof(struct notification_log_entry)*NOTIFICATION_LOG_MAX;
	_log(LH_NOTIFYLOG, B_LOG_DEBUG,"NOTIFICATION_LOG: GET HARDCOPY: %d", notification_log_size);
	return_hardcopy  = malloc(notification_log_size);
	memcpy(return_hardcopy, shmhdr->notification_log, notification_log_size);

	return return_hardcopy;


}

void bartlby_notification_log_finish(struct shm_header * shmhdr) {
	int x;
	shmhdr->notification_log_current_top=0;
	for(x=0; x<NOTIFICATION_LOG_MAX; x++) {
		shmhdr->notification_log[x].notification_valid=-1;
	}
	_log(LH_NOTIFYLOG, B_LOG_DEBUG,"Notification Log Finished!!");

}
void bartlby_notification_log_init(struct shm_header * shmhdr) {
	int x;
	shmhdr->notification_log_current_top=0;
	shmhdr->notification_log_aggregate_last_run=time(NULL);
	for(x=0; x<NOTIFICATION_LOG_MAX; x++) {
		shmhdr->notification_log[x].notification_valid=-1;
	}
	_log(LH_NOTIFYLOG, B_LOG_INFO,"Initialized Empty Notification Log");
}

void bartlby_notification_log_add(struct shm_header * shmhdr,char * cfgfile,  long worker_id, long service_id, int state, int type, int aggregation_interval, char * trigger_name) {

	//Add One entry
	//Aquire Semaphore
	//Increment hdr->notification_log_current_top || if +1>NOTIFICATION_LOG_MAX - start at 0
	//Add Notification to the array
	//release semaphore
	char * sem_name;


	sem_name=getConfigValue("notification_log_sem_name", cfgfile);
	if(sem_name == NULL) {
		sem_name=strdup("bartlby_notification_log_generic");
		//_log("notification_log_sem_name unset using default value");
	}


	sem_t * sem_id;
	int x;
	sem_id=sem_open(sem_name, O_CREAT, 0755, 1);

	sem_wait(sem_id);

	
	
	x=shmhdr->notification_log_current_top;
	shmhdr->notification_log[x].notification_valid=0;
	shmhdr->notification_log[x].worker_id=worker_id;
	shmhdr->notification_log[x].service_id=service_id;
	shmhdr->notification_log[x].state=state;
	shmhdr->notification_log[x].aggregated=0;
	sprintf(shmhdr->notification_log[x].trigger_name, "%s", trigger_name);
	shmhdr->notification_log[x].time = time(NULL);
	shmhdr->notification_log[x].type=type;
	shmhdr->notification_log[x].aggregation_interval=aggregation_interval;
	//bartlby_notification_B_LOG_DEBUG(shmhdr);
	

	if(shmhdr->notification_log_current_top+1 == NOTIFICATION_LOG_MAX) {
		shmhdr->notification_log_current_top=0;
	} else {
		shmhdr->notification_log_current_top++;
	}
	sem_post(sem_id);
	free(sem_name);
	
	

}
struct service *  bartlby_notification_log_get_service(struct shm_header * shmhdr, long service_id) {
	int x;
	struct service * svcmap;

	svcmap=bartlby_SHM_ServiceMap(shmhdr);

	for(x=0; x<shmhdr->svccount; x++) {
		if(svcmap[x].service_id == service_id) {
			return &svcmap[x];
		}		
	}
	return NULL;
}

struct worker *  bartlby_notification_log_get_worker(struct shm_header * shmhdr, int worker_id) {
	int x;
	struct worker * wrkmap;

	wrkmap=bartlby_SHM_WorkerMap(shmhdr);

	for(x=0; x<shmhdr->wrkcount; x++) {
		if(wrkmap[x].worker_id == worker_id) {
			return &wrkmap[x];
		}		
	}
	return NULL;
}
void bartlby_notification_log_aggregate(struct shm_header *shmhdr, char * cfgfile) {
	// Looop threw msg log starting from notification_log_current_top
	// if aggregate <= 0 - do aggregation
	//Group user, svc, trigger name - and call bartlby_trigger() with a custom msg e.g.:
	// 12 Services Changed State in the last 5 Minutes
	// Service1 (3xOK, 2xWARNING, 3xCRITICAL) - current state = OK (SVC_OUTPUT)
	// Service3 (3xOK, 2xWARNING, 3xCRITICAL) - current state = OK (SVC_OUTPUT)

	//Optionally "HARDCORE aggregate" - aggregated services count > X
	//Group by user, svc, trigger,name 
	// 122 Services changed state in the last 5 minutes
	// 120 are OK now, 1 is warning, 1 is CRITICAL

	int x;
	int found=0;
	struct worker_aggregate * head;
	struct worker_aggregate * current;
	struct worker_aggregate * w;
	struct service_aggregate * s;
	struct trigger_aggregate * t;

	char * exec_str;
	char * trigger_dir;

	char  notify_msg[5000];
	int connection_timed_out;
	FILE * ptrigger;

	char trigger_return[1024];
	struct service * svc;
	char * tmpstr;

	struct worker * wrkmap;

	long okcount=0;
	long warncount=0;
	long critcount=0;
	long othercount=0;

	int still_broken_count=0;

	struct service * csvc;


	//UPSTREAM NOTIFICATIONS:
	char * cfg_upstream_enabled;
	char * cfg_upstream_has_local_users;
	

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


	
	trigger_dir=getConfigValue("trigger_dir", cfgfile);
	head = NULL;

	for(x=0; x<NOTIFICATION_LOG_MAX; x++) {
		if(shmhdr->notification_log[x].type != 3 && shmhdr->notification_log[x].aggregation_interval > 0 && shmhdr->notification_log[x].aggregated == 0  && shmhdr->notification_log[x].notification_valid != -1 ) {
			if(head == NULL ) {
				head = malloc(sizeof(struct worker_aggregate));
				head->next=NULL;
				head->prev=NULL;
				head->worker_id=-1;
				head->trigger_list_head=NULL;

			}	
			w = bartlby_notification_log_worker_exists(head, shmhdr->notification_log[x].worker_id);
			if(w == NULL) {
				w = bartlby_notification_log_add_worker(head, shmhdr->notification_log[x].worker_id);	
				
			}
			t = bartlby_notification_log_trigger_exists(w->trigger_list_head, shmhdr->notification_log[x].trigger_name);
			if(t == NULL) {
				t = bartlby_notification_log_add_trigger(w->trigger_list_head, shmhdr->notification_log[x].trigger_name);
				
			}
			
			s = bartlby_notification_log_service_exists(t->service_list_head, shmhdr->notification_log[x].service_id);
			if(s == NULL) {
				s = bartlby_notification_log_add_service(t->service_list_head, shmhdr->notification_log[x].service_id);
				
			}





			switch(shmhdr->notification_log[x].state) {
				case 0:
				case 1:
				case 2:
					s->states[shmhdr->notification_log[x].state]++;
				break;
				default:
					s->states[3]++;
				break;
			}
			s->state_changes++;
			t->notification_count++;
			shmhdr->notification_log[x].aggregated=1;
			
			
			found++;
		} 

	}


	current = head;
	if(found > 0) {
		while(current != NULL) {
				//_log("DOING WORKER: %d", current->worker_id);
				t=current->trigger_list_head;
				while(t != NULL) {
					if(strcmp(t->trigger_name, "null") != 0) {
						//_log("\t Trigger: %s Count: %d", t->trigger_name, t->notification_count);
						

						//Send Aggregated Notification
						wrkmap=bartlby_notification_log_get_worker(shmhdr, current->worker_id);
						///////////
						okcount=0;
						critcount=0;
						warncount=0;
						othercount=0;
						//BUILD MSG:
						sprintf(notify_msg, "%d Notifications happened in the aggregation window \\n", t->notification_count);
						s=t->service_list_head;
						while(s != NULL) {
							if(s->service_id != -1) {
								//_log("\t\t Service: State Changes: %d -  id: %d OK: %d WARN: %d CRIT:%d OTHER: %d", s->state_changes, s->service_id, s->states[0], s->states[1], s->states[2], s->states[3]);
								okcount += s->states[0];
								warncount += s->states[1];
								critcount += s->states[2];
								othercount += s->states[3];
							}
							s=s->next;					
						}
						CHECKED_ASPRINTF(&tmpstr, "%ld OK, %ld Warning, %ld Critical, %ld Other \\n", okcount, warncount, critcount, othercount);
						strncat(notify_msg, tmpstr, sizeof(notify_msg) - strlen(notify_msg) - 1);

						free(tmpstr);
						strncat(notify_msg, " Still Broken: ", sizeof(notify_msg) - strlen(notify_msg) - 1);

						still_broken_count=0;
						s=t->service_list_head;
						while(s != NULL) {
							if(s->service_id != -1) {
								//_log("\t\t Service: State Changes: %d -  id: %d OK: %d WARN: %d CRIT:%d OTHER: %d", s->state_changes, s->service_id, s->states[0], s->states[1], s->states[2], s->states[3]);
								csvc=bartlby_notification_log_get_service(shmhdr, s->service_id);
								if(csvc->current_state != 0) {
									CHECKED_ASPRINTF(&tmpstr, "%s/%s (%d)\\n", csvc->srv->server_name, csvc->service_name, csvc->current_state);
									strncat(notify_msg, tmpstr, sizeof(notify_msg) - strlen(notify_msg) - 1);
									free(tmpstr);
									still_broken_count++;
								}
								
							}
							s=s->next;					
						}
						if(still_broken_count == 0) {
							//FIXME LENGTH
							
							strncat(notify_msg, "EVERYTHING Recovered", sizeof(notify_msg) - strlen(notify_msg) - 1);


						}






						CHECKED_ASPRINTF(&exec_str, "%s/%s \"%s\" \"%s\" \"%s\" \"%s\"", trigger_dir, t->trigger_name, wrkmap->mail,wrkmap->icq,wrkmap->name, notify_msg);


						_log(LH_NOTIFYLOG, B_LOG_DEBUG,"@NOT-AGGREGATE@: %s for Worker '%s' Notifications original: %d - %s",t->trigger_name,  wrkmap->name, t->notification_count, notify_msg);
						
						if(upstream_enabled == 1) {
							_log(LH_NOTIFYLOG, B_LOG_DEBUG,"@AGG-UPSTREAM-NOT-USER@ - TRIGGER: %s  local_users: %d  to-standbys:%d cmdline `%s'", t->trigger_name,  1, 0, exec_str);
							bartlby_trigger_upstream(cfgfile, 1, 0, t->trigger_name, exec_str, NULL);
							free(exec_str);
							t=t->next;
							continue;
						}
						




						ptrigger=popen(exec_str, "r");
						if(ptrigger != NULL) {
							connection_timed_out=0;
							alarm(CONN_TIMEOUT);
							if(fgets(trigger_return, 1024, ptrigger) != NULL) {
								trigger_return[strlen(trigger_return)-1]='\0';
								//_log("@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'%s'", svc->service_id, svc->last_state ,svc->current_state,t->trigger_name,wrkmap[x].name, svc->srv->server_name, svc->srv->client_port, svc->service_name, trigger_return);
								
    						} else {
    							//_log("@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'(empty output)'", svc->service_id, svc->last_state ,svc->current_state,entry->d_name,wrkmap[x].name, svc->srv->server_name, svc->srv->client_port, svc->service_name);
    						}
      								
    						if(connection_timed_out == 1) {
    							//_log("@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'(timed out)'", svc->service_id, svc->last_state ,svc->current_state,entry->d_name,wrkmap[x].name, svc->srv->server_name, svc->srv->client_port, svc->service_name);
    						}
    						connection_timed_out=0;
							alarm(0);
							if(ptrigger != NULL) {
      							pclose(ptrigger);
      						}
      					} else {
      						//_log("@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'(failed %s)'", svc->service_id, svc->last_state ,svc->current_state,entry->d_name,wrkmap[x].name, svc->srv->server_name, svc->srv->client_port, svc->service_name, full_path);	
      					}
						free(exec_str);
						
						/////




						
					}
					t=t->next;				

				}
				current=current->next;
		}
	}

	free(trigger_dir);



	current = head;
	//bartlby_notification_log_aggregate_destroy(current);
	bartlby_notification_log_aggregate_destroy(head);
	shmhdr->notification_log_aggregate_last_run=time(NULL);


}
void bartlby_notification_B_LOG_DEBUG(struct shm_header * shmhdr) {
	//Print out a list
	//Possible Loop:
	int done=0;
	int x;
	//First get current - to limit
	for(x=0; x<NOTIFICATION_LOG_MAX; x++) {
		if(shmhdr->notification_log[x].notification_valid >= 0) {
			_log(LH_NOTIFYLOG, B_LOG_DEBUG,"NOTIFICATION_LOG[%d] / %d - %d", x, shmhdr->notification_log_current_top, time(NULL)-shmhdr->notification_log[x].time);
		}					

	}
	
	
}
