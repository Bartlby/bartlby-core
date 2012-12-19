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
#define _GNU_SOURCE

#include <errno.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>  
#include <unistd.h>
#include <sys/time.h>


#include <bartlby.h>



void catch_signal(int signum);
int do_shutdown=0;

pid_t sched_pid;

struct shm_header * gshm_hdr;
struct service * gservices;
void * gSOHandle;
void * gshm_addr;
char * gConfig;


int g_micros_before_after_check=700;
long shortest_intervall;


/*
function: catch_signal
input: int signum Signal number
catches signals, need for reload and shutdown
*/

void catch_signal(int signum) {
	pid_t sig_pid;
	if(signum == SIGINT || signum == SIGUSR1) {
		do_shutdown=1;
		sig_pid=getpid();
		if(sig_pid != sched_pid) {
			
			if(kill(sched_pid, SIGINT) < 0) {
				_log("kill() failed in catch_signal() '%s`", strerror(errno));	
			} //Notify scheduler that someone is trying to kill us
			
			
			exit(1); // Exid child
				
		}
		
		
		
		
	}
	if(signum == SIGUSR2) {
                gshm_hdr->do_reload=1;
                sig_pid=getpid();
                if(sig_pid != sched_pid) {
                	
                        if(kill(sched_pid, SIGUSR2) < 0) {
                        	_log("kill() failed in catch_signal() '%s`", strerror(errno));		
                        } //notify scheduler
                        exit(1);

                }
         }
	
}


/*
function: sched_write_back_all
input: Configfile, shm_address, SOHandle
writes all shm changes back to database 
*/

void sched_write_back_all(char * cfgfile, void * shm_addr, void * SOHandle) {
	int x;
	
	struct service * services;
	struct server * servers;
	int (*doUpdate)(struct service *,char *);
	int (*doUpdateServer)(struct server *, char *);
	
	char * dlmsg;
	
	gshm_hdr=bartlby_SHM_GetHDR(shm_addr); //just to be sure ;)
	services=bartlby_SHM_ServiceMap(shm_addr);
	servers=bartlby_SHM_ServerMap(shm_addr);
	
	
	LOAD_SYMBOL(doUpdate,SOHandle, "doUpdate");
	LOAD_SYMBOL(doUpdateServer,SOHandle, "doUpdateServer");
	
	for(x=0; x<gshm_hdr->svccount; x++) {
		if(doUpdate(&services[x], cfgfile) != 1) {
			_log("doUpdate() failed in sched_writeback_all() '%s` for service id: %d", strerror(errno), services[x].service_id);		
		}
	}	
	_log("wrote back %d services!", x);
	
	/*
	for(x=0; x<gshm_hdr->srvcount; x++) {
		doUpdateServer(&servers[x], cfgfile);
	}
	_log("wrote back %d servers!", x);
	*/
	
	
	
	
}


/*
function: sched_reaper
input: int sig
catches fork childs
*/
void sched_reaper(int sig) {
	int childstatus;
	int childpid;
	if (sig != SIGCHLD && sig != SIGCLD) {
		_log("reaper: bad signal %d\n", sig);
   	} else {

		while((childpid = waitpid(-1, &childstatus, WNOHANG | WUNTRACED)) > 0 ) {
			_log("reaped: %d", childpid);
		}
		
		
	}
	return;
}

/*
function: sched_needs_ack
input: service
returns 1 if ACK is outstanding
*/
int sched_needs_ack(struct service * svc) {
	if(svc->service_ack == ACK_OUTSTANDING) {
		return 1; //Skip it something unAcked found
	} else {
		return 0; // No doIT
	}	
}

/*
function: sched_kill_runaaway
returns 1 if ACK is outstanding
function is called when a service check has not returned in the time frame
process is killed and notifications are sent, status is set to critical
*/

void sched_kill_runaaway(void * shm_addr, struct service *  svc, char * cfg, void * SOHandle) {
	int rtc;
	int rnd_intv;
	
	//if process isnt here anymore return
	if(svc->process.pid < 2)  {
		return;
	}
		
	
	rtc=kill(svc->process.pid, 9);
	if(rtc < 0 ) {
		
		switch(errno) {
			case EINVAL:
				_log("Killing runaaway process: %d (Invalid signal)",svc->process.pid); 
			break;
			case EPERM:
				_log("Killing runaaway process: %d (permission denied)",svc->process.pid); 
			break;
			
			case ESRCH:
				_log("Killing runaaway process: %d (no such process)",svc->process.pid); 
			break;	
			default:
				_log("Killing runaaway process: %d (unkw return: %d)",svc->process.pid, rtc); 
			
		}
	} else {
		
		
		_log("@KILL@%ld|%d|%s:%d/%s|Killing process with pid: %d", svc->service_id, svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name, svc->process.pid);
	}
		
	//service timed out
	sprintf(svc->new_server_text, "%s", "in-core time out");
	svc->current_state=STATE_CRITICAL;
	
	//on failure re-calc a new interval
	rnd_intv=1+(rand() % 10);
	svc->check_interval_original += rnd_intv;
	_log("bumped intervall: %d", rnd_intv);
	
	//finish service, pulls triggers etc
	bartlby_fin_service(svc,SOHandle,shm_addr,cfg);		
	svc->process.pid=0;
	svc->process.start_time=0;
	
	
	if(gshm_hdr->current_running > 0) {
		gshm_hdr->current_running--;
	} else {
		gshm_hdr->current_running=0;	
	}
	
	
}

/*
function: sched_is_server_dead
checks if a server is dead based on the life-indicator assigned
*/
int sched_is_server_dead(struct service * svc) {
	int rt;
	rt = 1;
	struct service * dm, * d1;
	int rr;
	
	/*
	Loops threw all servers trying to indicate if a server is dead	
	*/
	
	if(svc->srv->server_dead != 0) {
		// if a  "alive-inidicator" is set
		dm = svc->srv->dead_marker;
		rr = 0;
		while(dm != NULL) {
			// Until we have deadmarker loop here 
			
			//if the alive indicator is the service it self break out!! in recursion 1 this means it will be checked
			if(dm->service_id == svc->service_id) {
					break;	
			}
			
			
			
			//if the current alive-indicator is critical mark as dead and break out
			if(dm->current_state == STATE_CRITICAL && dm->service_retain_current >= dm->service_retain) {
				rt = -1;	
				break;
			}
						
			//if the next server has a dead marker and is not itself, fetch next
			if(dm->srv->server_dead != 0 && dm->service_id != dm->srv->server_dead) {
				d1 = dm->srv->dead_marker;
			
			} else {
				d1 = NULL;	
			}
			
			dm=d1;
			rr++;
			
		}
	}
	
	return rt;	
}



int sched_servergroup_notify(struct server * srv) {
		int x;
		//Check if server group has notifications
		if(srv->servergroup_counter == 0) {
				///Server is not member of a group
				return 1;
		}
		//Loop Threw Server Groups
		for(x=0; x<srv->servergroup_counter; x++) {
			if(srv->servergroups[x]->servergroup_notify == 0) {
				return 0;
			}
		}
		
		return 1;
}
int sched_servicegroup_notify(struct service * svc) {
		//Check if servicegroup has notifications on

	
	int x;
		//Check if service group is enabled to run checks
		if(svc->servicegroup_counter == 0) {
				///service is not member of a group
				return 1;
		}
		//Loop Threw service Groups
		for(x=0; x<svc->servicegroup_counter; x++) {
			if(svc->servicegroups[x]->servicegroup_notify == 0) {
				return 0;
			}
		}
		
		return 1;	
}



int sched_servergroup_active(struct server * srv) {
		int x;
		//Check if server group is enabled to run checks
		if(srv->servergroup_counter == 0) {
				///Server is not member of a group
				return 1;
		}
		//Loop Threw Server Groups
		for(x=0; x<srv->servergroup_counter; x++) {
			if(srv->servergroups[x]->servergroup_active == 0) {
				
				return 0;
			}
		}
		
		return 1;
		
}
int sched_servicegroup_active(struct service * svc) {
	//Check if service group is enabled to run checks
	
	int x;
		//Check if service group is enabled to run checks
		if(svc->servicegroup_counter == 0) {
				///service is not member of a group
				return 1;
		}
		//Loop Threw service Groups
		for(x=0; x<svc->servicegroup_counter; x++) {
			if(svc->servicegroups[x]->servicegroup_active == 0) {
				return 0;
			}
		}
		
		return 1;	
	
}


void sched_reschedule(struct service * svc) {
	//Resets the DIFF parameter
	gettimeofday(&svc->lcheck, NULL);
}

/*
function: sched_check_waiting
checks if a service is required to check or not
*/


int sched_check_waiting(void * shm_addr, struct service * svc, char * cfg, void * SOHandle, int sched_pause) {
	int cur_time;
	long my_diff;
	int kill_diff;
	
	struct timeval cur_tv;
	
	
	//FIXME: should be removed
	
	usleep(g_micros_before_after_check);
	
	cur_time=time(NULL);
	gettimeofday(&cur_tv, NULL);
	
	
	my_diff=bartlby_milli_timediff(cur_tv, svc->lcheck);
	
	
	 

	if((svc->check_interval_original-my_diff) < shortest_intervall && svc->service_active == 1 && svc->srv->server_enabled != 0) { //FIXME review server/service group
		shortest_intervall=(svc->check_interval_original-my_diff);
		 
	}
	
	
	if(sched_pause >= 0) {
		if(svc->do_force == 1) {
			svc->do_force=0; //dont force again
			_log("@FORCE@%ld|%d|%d|||%s:%d/%s|Force check", svc->service_id, svc->last_state ,svc->current_state, svc->srv->server_name, svc->srv->client_port, svc->service_name);
			return 1;	
		}
	}
	
/*
scheduler flow:
	service intervall has to be reached
		no downtime matches
		service check has to be active
		service needs to be in time range
		service has no acks needed
		server is not dead 
		server is enabled
		servergroup is enabled
		servicegroup is enabled


if any of these chain members does not match - service lcheck (for next round difF) gets updated and check will not be done		
		
*/
	
	
		
		
			//Time Range matched ;)	
			if(my_diff >= svc->check_interval_original) {
				//diff is higher
				if(bartlby_is_in_downtime(shm_addr, svc) < 0) {
					//downtimed
					sched_reschedule(svc);
					return -1;
				}	
				if(svc->service_active != 1) {
						sched_reschedule(svc);						
						return -1; //Dont sched this	
				}
				if(service_is_in_time(svc->service_exec_plan) < 0) {
						sched_reschedule(svc);						
						return -1; //Dont sched this	
				}
				if(sched_needs_ack(svc) == 1) {
						sched_reschedule(svc);						
						return -1; //Dont sched this	
				}
	
				svc->is_server_dead=sched_is_server_dead(svc);
				if(svc->is_server_dead < 0) {
						sched_reschedule(svc);
						return -1;	
				}
		
				if(svc->srv->server_enabled == 0) {
					sched_reschedule(svc);
					return -1;	
				}
				
				if(sched_servergroup_active(svc->srv) == 0) {
						//Server Group is disabled
						sched_reschedule(svc);						
						return -1;
				}
				if(sched_servicegroup_active(svc) == 0) {
					//Service is active but not group
					sched_reschedule(svc);
				  return -1;
				}
				
		
				if(svc->process.pid == 0) {
					//No check running so DO-IT
				
					return 1;
				} 
				
			}
			
		
	
	/*
	 bug discovered on large NRPE setup where SSL_handshake did not cleanly timeout
	*/
	
	if(svc->process.pid > 2) {
			
		
		kill_diff=(svc->service_check_timeout);
		my_diff=cur_time - svc->process.start_time;
		
		if(svc->service_type != SVC_TYPE_PASSIVE) {
			//Passive's should'nt time out either
			if(my_diff > kill_diff+10) {
				//_log("@@@ %d/%d @@ ", my_diff, kill_diff);
				//A little offset
				//so this is a "so called" miss coded extension ;) with faulted timeout handlers ;)
				sched_kill_runaaway(shm_addr, svc, cfg,SOHandle);	
				return -1;
			}	
		}
	}
	
	return -1;
}

void sched_wait_open(int timeout, int fasten) {
	int x;
	int y;
	
	int olim;
	int definitiv_running=0;
	
	y=0;
	x=0;
	olim=3000;
	
	
	
	
	
	if(timeout != 0) {
		olim=timeout*1000;	
	}
	while(x<olim && do_shutdown == 0) {
		usleep(10);
		x+=10;
		olim=gshm_hdr->current_running*timeout;
		
		//definitiv_running=0
		for(y=0; y<gshm_hdr->svccount; y++) {
			sched_check_waiting(gshm_addr,&gservices[y], gConfig, gSOHandle, -1);
			if(gservices[y].process.pid > 0) {
				definitiv_running++;	
			}
		}
		
		
		if(gshm_hdr->current_running <= fasten) {
			break;
		}
			
		
		if(gshm_hdr->current_running == 0) {
			break;	
		}
		if(x > olim) {
			gshm_hdr->current_running=0;
			break;
		}
		
	}
	

}




void sched_do_now(struct service * svc, char * cfgfile , void * shm_addr, void * SOHandle)  {
	 struct timeval check_start, check_end;
	
	gshm_hdr->current_running++;
	
	gettimeofday(&check_start, NULL);
		
	svc->process.pid=getpid();
	svc->process.start_time=time(NULL);
	
	
	

	
	bartlby_check_service(svc, shm_addr, SOHandle, cfgfile);	
	
	
	gettimeofday(&check_end, NULL);
	
	
	bartlby_core_perf_track(gshm_hdr, svc, PERF_TYPE_SVC_TIME, bartlby_milli_timediff(check_end,check_start));
	
	svc->process.pid=0;
	svc->process.start_time=0;
	
	
	if(gshm_hdr->current_running > 0) {
		gshm_hdr->current_running--;
	} else {
		gshm_hdr->current_running=0;	
	}
	
	
	
	
	
	
}

void sched_run_check(struct service * svc, char * cfgfile, void * shm_addr, void * SOHandle) {
      

       int child_pid;
       
     
     
       
	child_pid=fork();
	
	if(child_pid == -1) {
		_log("FORK Error %s", strerror(errno));
		return;
	} else if(child_pid == 0) {
		
		setpgid(0,0);
		
		
		
		sched_do_now(svc, cfgfile, shm_addr, SOHandle);
		
		exit(0);
	}
		
	
	
}
static int cmpservice(const void *m1, const void *m2) {
	struct service_sort * s1 = (struct service_sort *) m1;
	struct service_sort * s2 = (struct service_sort *) m2;
	int d1, d2;
	
	d1 = s1->svc->last_check + s1->svc->check_interval_original/1000;
	d2 = s2->svc->last_check + s2->svc->check_interval_original/1000;
	
	if(d1 < d2)
		return -1;
	else if(d1 > d2) 
		return 1;
	else
		return 0;
	
}
int schedule_loop(char * cfgfile, void * shm_addr, void * SOHandle) {

	
	
	int x;
	int cfg_max_parallel=0;
	
	
	
	int round_start, round_visitors;
	char * cfg_sched_pause;
	char * cfg_g_micros_before_after_check;
	
	int sched_pause;
	
	struct timeval  stat_round_start, stat_round_end, run_c_start, run_c_end;
	
	char * i_am_a_slave;
	char * cfg_mps;
		

	struct service * services;
	struct service_sort * ssort;
	 
	 
	int cfg_max_load;
	double current_load[3];
	
	char  * cfg_load_max;
	
	
	
	int ct, expt;
	
	sched_pid=getpid();
	
	gshm_addr=shm_addr;
	gSOHandle=SOHandle;
	gConfig=cfgfile;
	
	
	int childstatus;

	
	gshm_hdr=bartlby_SHM_GetHDR(shm_addr);
	ssort  = malloc(sizeof(struct service_sort)*gshm_hdr->svccount);
	
	
	_log("Scheduler working on %ld Services", gshm_hdr->svccount);
	
	cfg_mps=getConfigValue("max_concurent_checks", cfgfile);
	if(cfg_mps == NULL) {
		_log("<Warn>Defaulting max_concurent_checks to '20'");
		cfg_max_parallel=20;
	} else {
		cfg_max_parallel=atoi(cfg_mps);
		free(cfg_mps);	
	}
	cfg_load_max=getConfigValue("max_load", cfgfile);
	
	if(cfg_load_max == NULL) {
		cfg_max_load=0;
		
	} else {
		cfg_max_load=atoi(cfg_load_max);
		free(cfg_load_max);
	}
	
	signal(SIGINT, catch_signal);
	signal(SIGUSR1, catch_signal);
	signal(SIGUSR2, catch_signal);
	
	
	services=bartlby_SHM_ServiceMap(shm_addr);
	gservices=services;
	
	gshm_hdr->do_reload=0;
	
	cfg_sched_pause = getConfigValue("sched_round_pause", cfgfile);
	if(cfg_sched_pause == NULL) {
		sched_pause=100;	
		_log("info: sched_pause defaulted to: %d milli-seconds (set sched_round_pause to modify)", sched_pause);
	} else {
		sched_pause=atoi(cfg_sched_pause);
		free(cfg_sched_pause);
		if(sched_pause <= 0) {
			sched_pause=1;
			_log("info: sched_pause really low should'nt be less than 1 milliseconds defaulting to it: %d", sched_pause);
		}
	}
	cfg_g_micros_before_after_check = getConfigValue("sched_micros_before_after_check", cfgfile);
	if(cfg_g_micros_before_after_check == NULL) {
		_log("HINT: to tune performance see 'sched_micros_before_after_check' defaults to 700");	
	} else {
		g_micros_before_after_check=atoi(cfg_g_micros_before_after_check);
		_log("micros_before_after=%d", g_micros_before_after_check);
		free(cfg_g_micros_before_after_check);
	}
	
	//Make a second sortable array
	for(x=0; x<gshm_hdr->svccount; x++) {
			ssort[x].svc=&services[x];	
	}
	
	
	while(1) {
		
		if(gshm_hdr->do_reload == 1) {
			_log("queuing Reload");	
			sched_wait_open(1, 0);
			signal(SIGCHLD, SIG_IGN);
			free(ssort);
			return -2;
		}
		if(do_shutdown == 1) {
			_log("Exit recieved");	
			sched_wait_open(1,0);
			signal(SIGCHLD, SIG_IGN);
			free(ssort);
			break;
		}
		
		if(gshm_hdr->sirene_mode == 1) {
			//We are in Sirene Mode dont check anything just notifie workers that something b ad is going on
			bartlby_check_sirene(cfgfile,shm_addr);
			continue;	
		}
		
		
		round_start=time(NULL);
		gettimeofday(&stat_round_start,NULL);
		round_visitors=0;	
		
		
		
		
		
		//Sort ascending on delay time so most delayed service will be checked rapidly ;)
		if(gshm_hdr->svccount>0) {
			qsort(ssort, gshm_hdr->svccount-1, sizeof(struct service_sort), cmpservice);
		}
		
		
		shortest_intervall=10;
		for(x=0; x<gshm_hdr->svccount; x++) {
			
			
			if(do_shutdown == 1 || gshm_hdr->do_reload == 1) {
				break;	
			}
			getloadavg(current_load, 3);
			
			
			if(gshm_hdr->current_running < cfg_max_parallel || (int)current_load[0] < cfg_max_load) { 
				if(sched_check_waiting(shm_addr, ssort[x].svc, cfgfile, SOHandle, sched_pause) == 1) {
					
					gettimeofday(&run_c_start,NULL);
					
					
					round_visitors++;
					
					
					ct = time(NULL);			
					expt = (ssort[x].svc->last_check+ssort[x].svc->check_interval);
					
					if(ct > expt && ssort[x].svc->service_type != SVC_TYPE_PASSIVE) {
						// service check has delayed
						ssort[x].svc->delay_time.sum += ct - expt;
					}

					ssort[x].svc->delay_time.counter++;
					
					
					//WTF?
					if(ssort[x].svc->service_type != SVC_TYPE_PASSIVE) {
						ssort[x].svc->last_check=time(NULL);
					} 
			 		sched_reschedule(ssort[x].svc);
			 		sched_run_check(ssort[x].svc, cfgfile, shm_addr, SOHandle);
			 		
			 		usleep(g_micros_before_after_check);
			 		
			 		gettimeofday(&run_c_end,NULL);
			 		
			 		
				}				
			} else {
				while(waitpid(-1, &childstatus, WNOHANG ) > 0 );
				sched_wait_open(60,cfg_max_parallel-1);	
			}
			
		}
		
		
		if(time(NULL)-round_start > sched_pause*3 && sched_pause > 0) {
			_log("Done %d Services in %ld Seconds", round_visitors, time(NULL)-round_start);				
		}
		
		//Log Round End
		gettimeofday(&stat_round_end,NULL);
		bartlby_core_perf_track(gshm_hdr, &services[x], PERF_TYPE_ROUND_TIME, bartlby_milli_timediff(stat_round_end,stat_round_start));
				
		while(waitpid(-1, &childstatus, WNOHANG ) > 0 );

		usleep(sched_pause);
		if(shortest_intervall > 1) {
			usleep((shortest_intervall-1)*1000);
			
		}
		
						
	
		
		
		round_start=time(NULL);
		round_visitors=0;
		
		
		
		i_am_a_slave = getConfigValue("i_am_a_slave", cfgfile);
		if(i_am_a_slave == NULL) {
			replication_go(cfgfile, shm_addr, SOHandle);
			
		} else {
			_log("Skipped repl because me is a slave");	
			free(i_am_a_slave);
			return -2;
		}
		
	}
	
	return 1;
	
	
	
}


