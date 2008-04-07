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
#include <malloc.h>
#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char * getConfigValue(char * key, char * fname);

static int connection_timed_out=0;

#define CONN_TIMEOUT 60
#define MYOS "Linux"
#define MYVERSION "0.9"

static void agent_conn_timeout(int signo) {
 	connection_timed_out = 1;
}
int main(int argc, char ** argv) {
	float loadavg[3];
	FILE * load;
	char svc_back[1024];
	char svc_in[1024];
	char * plugin_dir;
	char  * plugin_path;
	char plg[1024];
	char plg_args[1024];
	char * token;
	char * exec_str;
	int ip_ok=-1;
	int plugin_rtc;
	struct stat plg_stat;
	char plugin_output[1024];
	struct sockaddr_in name;
   	unsigned int namelen = sizeof(name);
	char * agent_load_limit;
	char * allowed_ip_list;
	
	
	FILE * fplg;
	
	struct sigaction act1, oact1;
	
        if(argc < 1) {
        	printf("Usage: bartlby_agent <CFGFILE>");
        	
        		
        }
        printf("OS: %s V: %s\n",MYOS, MYVERSION);
        fflush(stdout);
        agent_load_limit=getConfigValue("agent_load_limit", argv[argc-1]);
        allowed_ip_list=getConfigValue("allowed_ips", argv[argc-1]);
        
        if(agent_load_limit == NULL) {
        	agent_load_limit=strdup("10");	
        }
        if(allowed_ip_list == NULL) {
        	printf("2|No Ip Allowed\n\n");
        	fflush(stdout);	
		sleep(2);
        	exit(1);
        	
        }
        
        token=strtok(allowed_ip_list,",");
        
        if (getpeername(0,(struct sockaddr *)&name, &namelen) < 0) {
   		//syslog(LOG_ERR, "getpeername: %m");
   		exit(1);
   	} else {
   		//syslog(LOG_INFO, "Connection from %s",	inet_ntoa(name.sin_addr));
   	}
        
        while(token != NULL) {
        	//printf("CHECKING: %s against %s\n", token, inet_ntoa(name.sin_addr));
        	if(strcmp(token, inet_ntoa(name.sin_addr)) == 0) {
        		ip_ok=0;	
        	}
        	token=strtok(NULL, ",");	
        }
        free(allowed_ip_list);
        if(ip_ok < 0) {
        	//sleep(1);
        	sprintf(svc_back, "2|IP Blocked '%s'\n", inet_ntoa(name.sin_addr));
        	
		printf("%s\n", svc_back);
		fflush(stdout);	
		sleep(2);
		exit(1);
        }
        
        act1.sa_handler = agent_conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		
		printf("2|ALARM SETUP ERROR\n\n");
		fflush(stdout);	
		sleep(2);
		exit(1);
				
		return -1;
	
		
	}
	
    	sprintf(svc_back, "1|ouuutsch");
        
        plugin_dir=getConfigValue("agent_plugin_dir", argv[argc-1]);
        if(plugin_dir == NULL) {
        	printf("plugin dir failed\n");	
        	exit(1);
        }
        
        
        
        load=fopen("/proc/loadavg", "r");
        fscanf(load, "%f %f %f", &loadavg[0], &loadavg[1], &loadavg[2]);
        fclose(load);
        
        if(loadavg[0] < atof(agent_load_limit)) {
		free(agent_load_limit);
		connection_timed_out=0;
		alarm(CONN_TIMEOUT);
		//ipmlg]ajgai]Amoowlkecvg~"/j"nmacnjmqv~
		if(read(fileno(stdin), svc_in, 1024) < 0) {
			printf("2|read BAD!\n\n");
			fflush(stdout);	
			sleep(2);
			exit(1);
		}
		alarm(0);
		
		if(connection_timed_out == 1) {
			printf("2|conn Timed out!!!\n\n");
			fflush(stdout);	
			sleep(2);
			exit(1);	
		}
		
		svc_in[strlen(svc_in)-1]='\0';
		
		token=strtok(svc_in, "|");
		if(token == NULL) {
			sprintf(svc_back,"1|Protocol Error (No plugin specified");	
		} else {
			sprintf(plg, "%s", token);
			//syslog(LOG_ERR, "bartlby_agent: %s",plg);
			plugin_path=malloc(sizeof(char) * (strlen(plugin_dir)+strlen(plg)+255));
			sprintf(plugin_path, "%s/%s", plugin_dir, plg);
			if(stat(plugin_path,&plg_stat) < 0) {
				sprintf(svc_back, "1|Plugin does not exist (%s)", plugin_path);	
			} else {
				token=strtok(NULL, "|");
				if(token == NULL) {
					sprintf(plg_args, " ");	
				} else {
					sprintf(plg_args, "%s", token);
				}
				exec_str=malloc(sizeof(char) * (strlen(plugin_path)+strlen(plg_args)+255));
				sprintf(exec_str, "%s %s", plugin_path, plg_args);
				//printf("E_STR: P: '%s' A: '%s' F: '%s'\n", plugin_path, plg_args, exec_str);
				
				fplg=popen(exec_str, "r");
				if(fplg != NULL) {
					connection_timed_out=0;
					alarm(CONN_TIMEOUT);
					if(fgets(plugin_output, 1024, fplg) != NULL) {
						if(strncmp(plugin_output, "PERF: ", 6) == 0) {
							
							printf("%s\n",plugin_output);
							fflush(stdout);
							//sleep(1);
							if(fgets(plugin_output, 1024, fplg) != NULL) {
								plugin_rtc=pclose(fplg);
								plugin_output[strlen(plugin_output)-1]='\0';
								sprintf(svc_back, "%d|%s\n", WEXITSTATUS(plugin_rtc), plugin_output);		
							} else {
								plugin_rtc=pclose(fplg);
								sprintf(svc_back, "%d|No Output(Perf) - %s", WEXITSTATUS(plugin_rtc), exec_str);	
							
							}
							
						} else {
							plugin_rtc=pclose(fplg);
							plugin_output[strlen(plugin_output)-1]='\0';
							sprintf(svc_back, "%d|%s\n", WEXITSTATUS(plugin_rtc), plugin_output);		
						}
						
						
					} else {
						plugin_rtc=pclose(fplg);
						sprintf(svc_back, "%d|No Output - %s", WEXITSTATUS(plugin_rtc), exec_str);	
						
						
					}
					if(connection_timed_out == 1) {
						sprintf(svc_back, "1|Plugin timedout - %s", exec_str);
					}
					connection_timed_out=0;
					alarm(0);
					
					
				} else {
					sprintf(svc_back, "1|Plugin open failed");	
				}

				free(exec_str);
				
			}
			
			
			free(plugin_path);
			
			
		}
		
		
		
		
		        	
        } else { 
        	
        	free(agent_load_limit);
        	
        	sprintf(svc_back, "1|LoadLimit reached %.02f skipping Check!| \n", loadavg[0]);
		printf("%s\n", svc_back);
		fflush(stdout);	
		sleep(2);
		exit(1);
        	
        }
        fflush(stdout);
       
	//printf("SVC_BACK: %s\n", svc_back);
	//syslog(LOG_ERR, "bartlby_agent: %s",svc_back);
	
	printf("%s\n", svc_back);
	fflush(stdout);
	
	//printf("\n %s \n", svc_back);
	return 1;
}
