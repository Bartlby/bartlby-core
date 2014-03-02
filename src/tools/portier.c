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


#define CMD_PASSIVE 1
#define CMD_GET_PLG 2
#define CMD_REPL 3
#define CMD_SVCLIST 4
#define CMD_GETSERVERID 5
#define CMD_EXEC_TRIGGER_LINE 6
#define CMD_EXEC_TRIGGER 7

#define CONN_TIMEOUT 10

ssize_t	readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}
/* end readn */

ssize_t Readn(int fd, void *ptr, size_t nbytes)
{
	ssize_t		n;

	if ( (n = readn(fd, ptr, nbytes)) < 0)
		return -1;
		
	return(n);
}

static void agent_conn_timeout(int signo) {
 	connection_timed_out = 1;
}
int main(int argc, char ** argv) {
	struct sigaction act1, oact1;
	char svc_in[2048];
	char svc_out[2048];
	char * in_server_name;
	
	char * allowed_ip_list;
	int ip_ok=-1;
	struct sockaddr_storage name;
   	unsigned int namelen = sizeof(name);
	
	char * token;
	
	
	int command;
	int svc_found=0;
	
	int error;
	char namebuf[50];
	char portbuf[50];
	
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int passive_svcid;
	int standby_workers_only;
	int passive_serverid;
	int passive_state;
	char passive_text[2048];
	char * passive_beauty;
	
	char * trigger_name;
	char * notify_msg;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//SHM
	char * shmtok;
	int shm_id;
	//int * shm_elements;
	void * bartlby_address;
	
	int x;
	
	
	struct shm_header * shm_hdr;
	struct service * svcmap;
	struct server * srvmap;
	struct worker * wrkmap;
	char * find_trigger;
	char * exec_str;
	char * full_path;
	FILE * ptrigger;
	char trigger_return[1024];

	char * trigger_dir;
	struct stat finfo;	

	allowed_ip_list=getConfigValue("allowed_ips", argv[0]);
	if(allowed_ip_list == NULL) {
        	printf("-No Ip Allowed");
        	exit(1);
        	
        }
	
	token=strtok(allowed_ip_list,",");
        
        if (getpeername(0,(struct sockaddr *)&name, &namelen) < 0) {
   		//syslog(LOG_ERR, "getpeername: %m");
   		exit(1);
   	} else {
   		//syslog(LOG_INFO, "Connection from %s",	inet_ntoa(name.sin_addr));
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
	
	shmtok = getConfigValue("shm_key", argv[0]);
	
	if(shmtok == NULL) {
		_log("Unset variable `shm_key'");
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
		
	} else {
		printf("-1 Bartlby down!!!\n");	
		fflush(stdout);
		exit(1);
	}
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if(argc == 0) {
		printf("CONFIG FILE MISSING\n");
		exit(1);	
	}
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
	if(read(fileno(stdin), svc_in, 2000) < 0) {
		printf("-2 read-error on portier side\n");
		exit(1);
	}
	alarm(0);
	
	if(connection_timed_out == 1) {
		printf("Timed out!!!\n");
		exit(1);	
	}
	passive_svcid=-1;
	passive_state=-1;
	sprintf(passive_text, "failed");
	sprintf(svc_out, "-20");
	token=strtok(svc_in, "|");
	if(token != NULL) {
		command=atoi(token);
		switch(command) {
			
			case CMD_GET_PLG:
				token=strtok(NULL, "|");
				if(token != NULL) {
					passive_svcid=atoi(token);
					
					svc_found=0;
					for(x=0; x<shm_hdr->svccount; x++) {
						
						if(svcmap[x].service_id == passive_svcid) {
							svc_found = 1;
							break;
						}
					}
					if(svc_found == 1) {
						//1|413395|2|dasdsadsadsadas|
						if(svcmap[x].service_type == SVC_TYPE_PASSIVE) {
							
							sprintf(svc_out, "+PLG|%s %s|\n", svcmap[x].plugin,svcmap[x].plugin_arguments);
							
						} else {
							sprintf(svc_out, "-3 Service is not of type 'PASSIVE'");	
						}
					} else {
						sprintf(svc_out, "-4 Service not found\n");	
					}
					
					
					
				} else {
					sprintf(svc_out, "-5 SVCID missing\n");	
					
				}
			break;
			case CMD_EXEC_TRIGGER_LINE:
				//second is "standbys"
				//third is cmd line
				token=strtok(NULL, "|");
				if(token != NULL) {
					passive_svcid=atoi(token); //TO_STANDBYS
					token=strtok(NULL, "|");
					if(token != NULL) {

						exec_str=token;

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
	      						
						sprintf(svc_out, "+ EXECTRIGGERCMD: '%s' returned: %s", token, trigger_return);

						//FIXME POPEN

					}
				}


			break;
			case CMD_EXEC_TRIGGER:
				//second is "standbys"
				//third is cmd line
				token=strtok(NULL, "|");
				if(token != NULL) {
					standby_workers_only=atoi(token); //TO_STANDBYS
					token=strtok(NULL, "|");
					if(token != NULL) {
						notify_msg = token;
						token=strtok(NULL, "|");
						if(token != NULL) {
							trigger_name=token;
						}

					}
				}

				trigger_dir=getConfigValue("trigger_dir", argv[0]);
				if(trigger_dir == NULL) {
					exit(4);
				}
				asprintf(&find_trigger, "|%s|" , trigger_name);
				asprintf(&full_path, "%s/%s", trigger_dir, trigger_name);
				if(lstat(full_path, &finfo) < 0) {
					printf("STAT FAILED '%s'", full_path);
					free(find_trigger);
					free(full_path);
					exit(4);
				}
			
				for(x=0; x<shm_hdr->wrkcount; x++) {
					if(service_is_in_time(wrkmap[x].notify_plan) > 0) {
						//if(bartlby_worker_has_service(&wrkmap[x], svc, cfgfile) != 0 || do_check == 0) {
							if(strstr(wrkmap[x].enabled_triggers, find_trigger) != NULL || strlen(wrkmap[x].enabled_triggers) == 0) {
								
								
								//if((bartlby_trigger_escalation(&wrkmap[x], svc, standby_workers_only)) == FL) continue;
								//if((bartlby_trigger_worker_level(&wrkmap[x], svc)) == FL) continue;
									
								/* if standby escalation message check if worker is in standby mode either skip him/her*/
								if(standby_workers_only == 1 && wrkmap[x].active != 2) continue;
								wrkmap[x].escalation_time=time(NULL);
								asprintf(&exec_str, "%s \"%s\" \"%s\" \"%s\" \"%s\"", full_path, wrkmap[x].mail,wrkmap[x].icq,wrkmap[x].name, notify_msg);
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
							
							
						//}
					}
				}
			}
			free(find_trigger);	
			sprintf(svc_out, "+CALLTRIGGER LOCAL: '%s' msg: '%s' return: %s", trigger_name, notify_msg,trigger_return);
				
			break;

			case CMD_PASSIVE:
				//Second is SVCID
				//Third is new status
				//Fourth is new service_text
				token=strtok(NULL, "|");
				if(token != NULL) {
					passive_svcid=atoi(token);
					
					token=strtok(NULL, "|");
					if(token != NULL) {
						
						passive_state=atoi(token);
						token=strtok(NULL, "|");
						if(token != NULL) {
							
							sprintf(passive_text, "%s", token);
							
							token = strtok(NULL, "|");
							if(token != NULL) {
								strcat(passive_text, " - |");
								strcat(passive_text, token);
							}
							
						} else {
							sprintf(passive_text," ");
									
						}
						
					
						
					
						svc_found=0;
						for(x=0; x<shm_hdr->svccount; x++) {
							
							if(svcmap[x].service_id == passive_svcid) {
								svc_found = 1;
								break;
							}
						}
						if(svc_found == 1) {
							//2|413395
							if(svcmap[x].service_type == SVC_TYPE_PASSIVE) {
								svcmap[x].last_state=svcmap[x].current_state;
								svcmap[x].current_state=passive_state;
								sprintf(svcmap[x].new_server_text, "%s", passive_text);
								svcmap[x].last_check=time(NULL);
								
								
								passive_beauty=bartlby_beauty_state(svcmap[x].current_state);
								sprintf(svc_out, "+PASSIVOK (%d) %ld : %s (%s)\n", x, svcmap[x].service_id, passive_beauty, svcmap[x].new_server_text);
								free(passive_beauty);
							} else {
								sprintf(svc_out, "-3 Service is not of type 'PASSIVE'");	
							}
						} else {
							sprintf(svc_out, "-4 Service not found\n");	
						}
						
					} else {
						sprintf(svc_out, "New state missing\n");		
					}
					
				} else {
					sprintf(svc_out, "SVCID missing\n");	
					
				}
			break;
			case CMD_GETSERVERID:
				token=strtok(NULL, "|");
				if(token != NULL) {
					in_server_name=strdup(token);
					sprintf(svc_out, " ");
					
					for(x=0; x<shm_hdr->svccount; x++) {
						if(strcmp(srvmap[svcmap[x].srv_place].server_name, in_server_name) == 0) {
							printf("%ld", svcmap[x].server_id);
								
						}
					}
					printf("\n");
					fflush(stdout);
				} else {
					sprintf(svc_out, "-5 server_name missing");	
				}
			break;
			case CMD_SVCLIST:
				token=strtok(NULL, "|");
				if(token != NULL) {
					passive_serverid=atoi(token);
					sprintf(svc_out, " ");
					for(x=0; x<shm_hdr->svccount; x++) {
						if(svcmap[x].server_id == passive_serverid && svcmap[x].service_type == SVC_TYPE_PASSIVE && svcmap[x].service_passive_timeout > 0) {
							if(service_is_in_time(svcmap[x].service_exec_plan)) {
								printf("%ld", svcmap[x].service_id);
								printf(" ");	
							}
						}
					}
					printf("\n");
					fflush(stdout);
				} else {
					sprintf(svc_out, "-5 server_id missing");	
				}
			break;
			default:
				printf("-2 cmd not found\n");
				exit(1);				
		}
		
		printf("%s", svc_out);
			
	}
	shmdt(bartlby_address);
	//bartlby_encode(svc_out, strlen(svc_out));
	//printf("%s", svc_out);
	//bartlby_decode(svc_out, strlen(svc_out));
	
	
	return 1;
}
