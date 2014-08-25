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
static int connection_timed_out=0;
extern char config_file[255];


static struct shm_header * shm_hdr;
static struct service * svcmap;
static struct server * srvmap;
static struct worker * wrkmap;


#define CMD_PASSIVE 1
#define CMD_GET_PLG 2
#define CMD_REPL 3
#define CMD_SVCLIST 4
#define CMD_GETSERVERID 5
#define CMD_EXEC_TRIGGER_LINE 6
#define CMD_EXEC_TRIGGER 7

#define CONN_TIMEOUT 10

#define FL 0
#define TR 1

#define PORTIER_CLEANUP shmdt(bartlby_address);
						


//function defs
void bartlby_portier_submit_passive_result(long service_id, int status, char * message);
void bartlby_portier_find_server_id(char * server_name);
void bartlby_portier_find_services(long server_id);
void bartlby_portier_get_plugin_info(long service_id);
void bartlby_portier_exec_trigger(char * cfgfile, int standby_workers_only, char * execline, char * trigger_name, int service_id, int server_id, int notify_last_state, int current_state, int recovery_outstanding, int node_id, char * passwd);
void bartlby_portier_exec_trigger_line(char * cfgfile, int standby_workers_only, char * execline, char * trigger_name, int service_id, int server_id, int notify_last_state, int current_state, int recovery_outstanding, int node_id, char * passwd);







void bartlby_portier_exec_trigger_line(char * cfgfile, int standby_workers_only, char * execline, char * trigger_name, int service_id, int server_id, int notify_last_state, int current_state, int recovery_outstanding, int node_id, char * passwd) {

	char * portier_passwd;
	char * exec_str;
	int x;
	FILE * ptrigger;
	char trigger_return[1024];

	portier_passwd=getConfigValue("portier_password", cfgfile);
	if(portier_passwd == NULL) {
		printf("-2222 portier_passwd unset");
		return;
	}

	if(portier_passwd != NULL && strcmp(passwd, portier_passwd) == 0) {
		ptrigger=popen(exec_str, "r");
		if(ptrigger != NULL) {
			connection_timed_out=0;
			alarm(CONN_TIMEOUT);
			if(fgets(trigger_return, 1024, ptrigger) != NULL) {
				trigger_return[strlen(trigger_return)-1]='\0';
				connection_timed_out=0;
				alarm(0);
			}
			if(ptrigger != NULL) {
				pclose(ptrigger);
			}
			      						
		} 
			      						
		printf("+ EXECTRIGGERCMD: '%s' returned: %s", exec_str, trigger_return);
	} else {
		printf("- AUTH FAILED");
	}
	free(portier_passwd);
	return;


}

void bartlby_portier_exec_trigger(char * cfgfile, int standby_workers_only, char * execline, char * trigger_name, int service_id, int server_id, int notify_last_state, int current_state, int recovery_outstanding, int node_id, char * passwd) {

	struct service local_svc;
	int trigger_fine = 0;
	char * portier_passwd;
	char * find_trigger, * full_path, * trigger_dir;
	struct stat finfo;
	char * base_dir, *exec_str;
	int x;
	FILE * ptrigger;
	char trigger_return[1024];

	portier_passwd=getConfigValue("portier_password", cfgfile);
	if(portier_passwd == NULL) {
		printf("-2222 portier_passwd unset");
		return;
	}


	local_svc.server_id=server_id;
	local_svc.service_id=service_id;
	local_svc.notify_last_state=notify_last_state;
	local_svc.recovery_outstanding=recovery_outstanding;
	local_svc.current_state=current_state;
	
	if(node_id != 0 && portier_passwd != NULL && strcmp(passwd, portier_passwd) == 0) {
		trigger_fine=1;	
	}
	free(portier_passwd);

	if(trigger_fine != 1) {
		printf("-1 PARAM ERROR");
		return;
	}

	trigger_dir=getConfigValue("trigger_dir", cfgfile);
	if(trigger_dir == NULL) {
			printf("TRIGGER DIR UNSET IN CFG");
			return;
	}
	asprintf(&find_trigger, "|%s|" , trigger_name);
	asprintf(&full_path, "%s/%s", trigger_dir, trigger_name);
	if(lstat(full_path, &finfo) < 0) {
		printf("STAT FAILED '%s'", full_path);
		free(find_trigger);
		free(full_path);
		return;
	}
	base_dir = getConfigValue("basedir", cfgfile);
	if(base_dir == NULL) {
		base_dir=strdup("/");
	}
	if(setenv("BARTLBY_HOME", base_dir,1) == 0) {
	}
				
	for(x=0; x<shm_hdr->wrkcount; x++) {
		if(service_is_in_time(wrkmap[x].notify_plan) > 0) {
			if(bartlby_worker_has_service(&wrkmap[x], &local_svc, cfgfile, node_id) != 0 ) {
				if(strstr(wrkmap[x].enabled_triggers, find_trigger) != NULL || strlen(wrkmap[x].enabled_triggers) == 0) {
					if((bartlby_trigger_escalation(&wrkmap[x], &local_svc, standby_workers_only, node_id)) == FL) continue;
					if((bartlby_trigger_worker_level(&wrkmap[x], &local_svc, node_id)) == FL) continue;
					
					/* if standby escalation message check if worker is in standby mode either skip him/her*/
					if(standby_workers_only == 1 && wrkmap[x].active != 2) continue;
					//FIXME
						wrkmap[x].escalation_time=time(NULL);
						asprintf(&exec_str, "%s \"%s\" \"%s\" \"%s\" \"%s\" 2>&1", full_path, wrkmap[x].mail,wrkmap[x].icq,wrkmap[x].name, execline);
						_log(LH_PORTIER, B_LOG_HASTO, "@NOT@%ld|%d|%d|%s|%s|UPSTREAMED - %s", local_svc.service_id, local_svc.notify_last_state ,local_svc.current_state,trigger_name,wrkmap[x].name, execline);
						bartlby_notification_log_add(shm_hdr, cfgfile, wrkmap[x].worker_id, local_svc.service_id, local_svc.current_state, standby_workers_only, wrkmap[x].notification_aggregation_interval,  trigger_name);
						if(wrkmap[x].notification_aggregation_interval > 0) { // 3 == THE AGGREGATION MESSAGE ITSELF
							//As we aggregate the notifications - skip the execution of the trigger
							free(exec_str);
							continue;
						}

						ptrigger=popen(exec_str, "r");
						if(ptrigger != NULL) {
							connection_timed_out=0;
							alarm(CONN_TIMEOUT);
							if(fgets(trigger_return, 1024, ptrigger) != NULL) {
								trigger_return[strlen(trigger_return)-1]='\0';
   								connection_timed_out=0;
								alarm(0);
							}
							if(ptrigger != NULL) {
								pclose(ptrigger);
							}
	    						
						} 
		      						
						free(exec_str);
								
								
					} 
				}
		}
	}
	free(base_dir);
	free(find_trigger);	
	printf("+CALLTRIGGER LOCAL: '%s'  return: '%s'\n", trigger_name, trigger_return);
			



	


}
void bartlby_portier_get_plugin_info(long service_id) {
	int svc_found=0;
	int x;

	for(x=0; x<shm_hdr->svccount; x++) {
		if(svcmap[x].service_id == service_id) {
			svc_found = 1;
			break;
		}
	}
	if(svc_found == 1) {
		if(svcmap[x].service_type == SVC_TYPE_PASSIVE) {
			printf("+PLG|%s %s|\n", svcmap[x].plugin,svcmap[x].plugin_arguments);
		} else {
			printf("-3 Service is not of type 'PASSIVE'");	
		}
	} else {
		printf("-4 Service not found\n");	
	}
}


void bartlby_portier_find_services(long server_id) {
	int x;
	
	for(x=0; x<shm_hdr->svccount; x++) {
		if(svcmap[x].server_id == server_id && svcmap[x].service_type == SVC_TYPE_PASSIVE && svcmap[x].service_passive_timeout > 0) {
			if(service_is_in_time(svcmap[x].service_exec_plan)) {
				printf("%ld", svcmap[x].service_id);
				printf(" ");	
			}
		}
	}
	printf("\n");
	fflush(stdout);
}
void bartlby_portier_find_server_id(char * server_name) {
	int x;
	int found=0;
	for(x=0; x<shm_hdr->svccount; x++) {
		if(strcmp(srvmap[svcmap[x].srv_place].server_name, server_name) == 0) {
			found=1;
			printf("%ld\n", svcmap[x].server_id);
			break;
					
		}
	}
	if(found == 0) {
		printf("-1 server name not found");
	}

}
void bartlby_portier_submit_passive_result(long service_id, int status, char * message) {
	
	int svc_found, x;
	char * passive_beauty;
	
	svc_found=0;
	for(x=0; x<shm_hdr->svccount; x++) {
		if(svcmap[x].service_id == service_id) {
			svc_found = 1;
			break;
		}
	}
	if(svc_found == 1) {
		if(svcmap[x].service_type == SVC_TYPE_PASSIVE) {
			svcmap[x].last_state=svcmap[x].current_state;
			svcmap[x].current_state=status;
			sprintf(svcmap[x].new_server_text, "%s", message);
			svcmap[x].last_check=time(NULL);
			passive_beauty=bartlby_beauty_state(svcmap[x].current_state);
			printf("+PASSIVOK (%d) %ld : %s (%s)\n", x, svcmap[x].service_id, passive_beauty, svcmap[x].new_server_text);
			free(passive_beauty);
			return;
		} else {
			printf("-3 Service is not of type 'PASSIVE'");	
			return;
		}
	} else {
		printf("-4 Service not found\n");	
		return;
	}


	
}




static void agent_conn_timeout(int signo) {
 	connection_timed_out = 1;
}
int main(int argc, char ** argv) {
	struct sigaction act1, oact1;
	
	char inputbuffer[2048];
	
	
	char * allowed_ip_list;
	int ip_ok=-1;
	struct sockaddr_storage name;
   	unsigned int namelen = sizeof(name);
	
	char namebuf[50];
	char portbuf[50];
	
		
	char * shmtok;
	int shm_id;
	void * bartlby_address;

	char * token;
	int error;

	//buffers:
	char textbuffer1[2048],textbuffer2[2048],textbuffer3[2048];
	long longbuffer1, longbuffer2;
	int intbuffer1, intbuffer2, intbuffer3,intbuffer4,intbuffer5,intbuffer6,intbuffer7;

	
//#define CMD_L_D

#ifndef CMD_L_D
	#define ARGV_IDX 0
	set_cfg(argv[ARGV_IDX]);
	allowed_ip_list=getConfigValue("allowed_ips", argv[ARGV_IDX]);
	if(allowed_ip_list == NULL) {
        	printf("-No Ip Allowed");
        	exit(1);
        	
        }
		token=strtok(allowed_ip_list,",");
        
        if (getpeername(0,(struct sockaddr *)&name, &namelen) < 0) {
   		//syslog(LOG_ERR, "getpeername: %m");
   		exit(1);
   	} else {
   		//syslog(B_LOG_INFO, "Connection from %s",	inet_ntoa(name.sin_addr));
   	}
   	 error = getnameinfo((struct sockaddr *)&name, namelen,
                    namebuf, sizeof(namebuf),
                    portbuf, sizeof(portbuf),
                    NI_NUMERICHOST);
                    
        if (error) {
   					//syslog(LOG_ERR, "getnameinfo failed %s", gai_strerror(error));
   					exit(1);
   			}
        
        while(token != NULL) {
        	//printf("CHECKING: %s against %s\n", token, inet_ntoa(name.sin_addr));
        	if(strcmp(token,namebuf) == 0) {
        		ip_ok=0;	
        	}
        	token=strtok(NULL, ",");	
        }
        free(allowed_ip_list);
        if(ip_ok < 0) {
        	printf("-IP Blocked %s\n", namebuf);
		exit(1);
        }	
#else
#define ARGV_IDX 1        
#endif
	
	shmtok = getConfigValue("shm_key", argv[ARGV_IDX]);
	
	if(shmtok == NULL) {
		_log(LH_PORTIER, B_LOG_CRIT,"Unset variable `shm_key'");
		fflush(stdout);
		exit(1);
	}
	
	shm_id = shmget(ftok(shmtok, 32), 0, 0777);
	if(shm_id != -1) {
		
		bartlby_address=shmat(shm_id,NULL,0);
		shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
		svcmap=bartlby_SHM_ServiceMap(bartlby_address);
		srvmap=bartlby_SHM_ServerMap(bartlby_address);
		wrkmap=bartlby_SHM_WorkerMap(bartlby_address);

		printf("+SVCC: %ld WRKC: %ld V: %s\n", shm_hdr->svccount, shm_hdr->wrkcount, shm_hdr->version);
		fflush(stdout);
		free(shmtok);
		
	} else {
		printf("-1 Bartlby down!!!\n");	
		fflush(stdout);
		exit(1);
	}
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	act1.sa_handler = agent_conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		
		printf("ALARM SETUP ERROR");
		exit(1);
				
		return -1;
	
		
	}
	connection_timed_out=0;
	alarm(CONN_TIMEOUT);
	//ipmlg]ajgai]Amoowlkecvg~"/j"nmacnjmqv~
	if(read(fileno(stdin), inputbuffer, 2000) < 0) {
		printf("-2 read-error on portier side\n");
		exit(1);
	}
	alarm(0);
	
	if(connection_timed_out == 1) {
		printf("Timed out!!!\n");
		exit(1);	
	}
	
	if(sscanf(inputbuffer, "1|%ld|%d|%2047[^\n]s", &longbuffer1, &intbuffer1, &textbuffer1) == 3) {
		//1|svc_id|state|passive_text CMD_PASSIVE
		textbuffer1[strlen(textbuffer1)-1]=0;
		bartlby_portier_submit_passive_result(longbuffer1, intbuffer1, textbuffer1);
		PORTIER_CLEANUP
		exit(1);
	}
	if(sscanf(inputbuffer, "5|%2047[^\n]s|", &textbuffer1) == 1) {
		//5|server_name| -> GET SERVER ID
		textbuffer1[strlen(textbuffer1)-1]=0;
		bartlby_portier_find_server_id(textbuffer1);
		PORTIER_CLEANUP
		exit(1);
	}
	if(sscanf(inputbuffer, "4|%ld|", &longbuffer1) == 1) {
		//4|server_id|
		bartlby_portier_find_services(longbuffer1);
		PORTIER_CLEANUP
		exit(1);
	}
	if(sscanf(inputbuffer, "2|%ld|", &longbuffer1) == 1) {
		//4|server_id|
		bartlby_portier_get_plugin_info(longbuffer1);
		PORTIER_CLEANUP
		exit(1);
	}
	if(sscanf(inputbuffer, "7|%d|%2047[^\n]s|%2047[^\n]s|%d|%d|%d|%d|%d|%d|%2047[^\n]s|", &intbuffer1, &textbuffer1, &textbuffer2, &intbuffer2, &intbuffer3, &intbuffer4, &intbuffer5, &intbuffer6, &intbuffer7, &textbuffer3) == 1) {
		//4|server_id|
		
		//sprintf(cmdstr, "%d|%d|%s|%s|%d|%d|%d|%d|%d|%d|%s|\n", passive_cmd, to_standbys, execline, trigger_name, svc->service_id, svc->server_id, svc->notify_last_state, svc->current_state, svc->recovery_outstanding, node_id, portier_passwd);
		bartlby_portier_exec_trigger(argv[ARGV_IDX], intbuffer1, textbuffer1, textbuffer2, intbuffer2, intbuffer3, intbuffer4, intbuffer5, intbuffer6, intbuffer7, textbuffer3);
		PORTIER_CLEANUP
		exit(1);
	}
	if(sscanf(inputbuffer, "6|%d|%2047[^\n]s|%2047[^\n]s|%d|%d|%d|%d|%d|%d|%2047[^\n]s|", &intbuffer1, &textbuffer1, &textbuffer2, &intbuffer2, &intbuffer3, &intbuffer4, &intbuffer5, &intbuffer6, &intbuffer7, &textbuffer3) == 1) {
		//4|server_id|
		
		//sprintf(cmdstr, "%d|%d|%s|%s|%d|%d|%d|%d|%d|%d|%s|\n", passive_cmd, to_standbys, execline, trigger_name, svc->service_id, svc->server_id, svc->notify_last_state, svc->current_state, svc->recovery_outstanding, node_id, portier_passwd);
		bartlby_portier_exec_trigger_line(argv[ARGV_IDX], intbuffer1, textbuffer1, textbuffer2, intbuffer2, intbuffer3, intbuffer4, intbuffer5, intbuffer6, intbuffer7, textbuffer3);
		PORTIER_CLEANUP
		exit(1);
	}
	//%d|%d|%s|%s|%d|%d|%d|%d|%d|%d|%s|\n

	exit(1);



}
