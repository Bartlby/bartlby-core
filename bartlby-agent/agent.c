/* $Id: agent.c,v 1.11 2008/03/28 05:08:45 hjanuschka Exp $ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005 Helmut Januschka - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */
/*
$Revision: 1.11 $
$Source: /cvsroot/bartlby/bartlby-agent/agent.c,v $


$Log: agent.c,v $
Revision 1.11  2008/03/28 05:08:45  hjanuschka
agent:	 compile warnings fixed
passive: run.sh fixes (http://wiki.bartlby.org/dokuwiki/doku.php?id=start#passiveagentsetup)
passive: HOWTO.passive and passive_run.sample added

Revision 1.10  2006/12/27 19:08:50  hjanuschka
*** empty log message ***

Revision 1.1  2006/11/25 00:54:46  hjanuschka
auto commit

Revision 1.2  2006/08/19 00:06:47  hjanuschka
agent: loadLimit protocoll error!!

Revision 1.1.1.1  2006/07/22 23:27:49  hjanuschka
initial stand alone agent

Revision 1.22  2006/05/12 23:38:02  hjanuschka
*** empty log message ***

Revision 1.21  2006/02/12 00:15:34  hjanuschka
Makefile.conf added
Local checks implemented
minor active check fixes and clean ups for re-use with local checks

Revision 1.20  2006/02/10 23:54:46  hjanuschka
SIRENE mode added

Revision 1.19  2006/02/09 00:14:50  hjanuschka
datalib: mysql/ catch failed logins
core: fixed some setuid problems with datalib
core: zero worker detected and logged
core: network code re-worked, much faster and cleaner now
core: encode/decode removed
php: encode/decode removed
ui: topology map manager added
ui: nicer menu (flap)
ui: server_detail (added)
startup sh: pre-start check if logfile is writeable

Revision 1.18  2006/01/29 15:53:05  hjanuschka
server icon

Revision 1.17  2006/01/08 23:20:41  hjanuschka
install target

Revision 1.16  2006/01/08 16:17:24  hjanuschka
mysql shema^

Revision 1.15  2005/12/31 00:29:44  hjanuschka
some more perf fixes during high load test

Revision 1.14  2005/12/25 23:01:16  hjanuschka
stress testing with RRD
perf fixes

Revision 1.13  2005/12/24 17:53:41  hjanuschka
performance interface i.e: for adding RRD tools or something like that

Revision 1.12  2005/10/09 14:44:09  hjanuschka
agent announces OS and version

Revision 1.11  2005/09/28 21:46:30  hjanuschka
converted files to unix
jabber.sh -> disabled core dumps -> jabblibs segfaults
                                    will try to patch it later

Revision 1.10  2005/09/27 19:39:01  hjanuschka
trigger timeout
agent local timeout

Revision 1.9  2005/09/22 02:55:03  hjanuschka
agent: def timeout 15
check: strreplace ' "

Revision 1.8  2005/09/18 04:04:52  hjanuschka
replication interface (currently just a try out)
one instance can now replicate itself to another using portier as a transport way
FIXME: need to sort out a binary write() problem

Revision 1.7  2005/09/14 22:01:41  hjanuschka
debug in data_lib added and removed
agent: off by two :-) *fG* malloc error producing magic char's  (fixed)

Revision 1.6  2005/09/13 22:11:52  hjanuschka
ip_list moved to .cfg
	allowed_ips
load limit moved to cfg
	agent_load_limit

portier now also uses ip list to verify ip of connector

portier: passive check without plg args fixed

Revision 1.5  2005/09/02 02:16:57  hjanuschka
some trap downs ;-)

Revision 1.4  2005/08/30 20:13:17  hjanuschka
fixed pclose() wrong exit code in agent

Revision 1.3  2005/08/28 16:02:59  hjanuschka
CVS Header


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
        	sprintf(svc_back, "2|IP Blocked \n");
        	
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
