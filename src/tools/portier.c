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
static int is_http;
static int http_headers_shown=0;

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

#define PORTIER_CLEANUP json_object_put(jso_in);\
						fflush(stdout); \
						if(is_http) { \
							break; \
						}\
						continue;
						


//function defs
void bartlby_show_error(int code, char * msg, int http);
void bartlby_portier_submit_passive_result(long service_id, int status, const char * message);
void bartlby_portier_find_server_id(const char * server_name);
void bartlby_portier_find_services(long server_id);
void bartlby_portier_get_plugin_info(long service_id);
void bartlby_portier_exec_trigger(char * cfgfile, int standby_workers_only, const char * execline, const char * trigger_name, int service_id, int server_id, int notify_last_state, int current_state, int recovery_outstanding, int node_id, const char * passwd);
void bartlby_portier_exec_trigger_line(char * cfgfile, const char * execline, const char * passwd);

void bartlby_portier_set_svc_state(long service_id, char * service_text, long current_state,long last_notify_send,long last_state_change,long service_ack_current,long service_retain_current,long handled, long last_check);



void bartlby_portier_set_svc_state(long service_id, char * service_text, long current_state,long last_notify_send,long last_state_change,long service_ack_current,long service_retain_current,long handled, long last_check) {
	printf("-123 not impl. '%s' \n", service_text);
}


void bartlby_portier_exec_trigger_line(char * cfgfile, const char * execline, const char * passwd) {

	
	char * portier_passwd;
	
	int x;
	FILE * ptrigger;
	char trigger_return[1024] = {0};
	json_object * jso;

	portier_passwd=getConfigValue("portier_password", cfgfile);
	if(portier_passwd == NULL) {
		bartlby_show_error(-222, "Portier Passwd unset", is_http);
		return;
	}

	if(portier_passwd != NULL && strcmp(passwd, portier_passwd) == 0) {
		ptrigger=popen(execline, "r");
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
		jso = json_object_new_object();
		json_object_object_add(jso,"error_code", json_object_new_int(0));
		json_object_object_add(jso,"execline", json_object_new_string(execline));
		json_object_object_add(jso,"output", json_object_new_string(trigger_return));
	
		printf("%s\n", json_object_to_json_string(jso));

		json_object_put(jso);
	      							
	
	} else {
		bartlby_show_error(-113, "Auth Failed", is_http);
	}
	free(portier_passwd);
	return;


}

void bartlby_portier_exec_trigger(char * cfgfile, int standby_workers_only, const char * execline, const char * trigger_name, int service_id, int server_id, int notify_last_state, int current_state, int recovery_outstanding, int node_id, const char * passwd) {

	struct service local_svc;
	int trigger_fine = 0;
	char * portier_passwd;
	char * find_trigger, * full_path, * trigger_dir;
	struct stat finfo;
	char * base_dir, *exec_str;
	int x;
	FILE * ptrigger;
	char trigger_return[1024];
	int notification_log_last_state;
	json_object * jso;

	portier_passwd=getConfigValue("portier_password", cfgfile);
	if(portier_passwd == NULL) {
		bartlby_show_error(-222, "Portier Passwd unset", is_http);
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
		bartlby_show_error(-223, "Param Error", is_http);
		return;
	}

	trigger_dir=getConfigValue("trigger_dir", cfgfile);
	if(trigger_dir == NULL) {
			bartlby_show_error(-224, "Trigger Dir unset in CFG", is_http);
			return;
	}
	asprintf(&find_trigger, "|%s|" , trigger_name);
	asprintf(&full_path, "%s/%s", trigger_dir, trigger_name);
	if(lstat(full_path, &finfo) < 0) {
		bartlby_show_error(-225, "Stat failed on trigger dir", is_http);
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
					
					/* if re-notify - and user is not active continue */
					if(standby_workers_only == 2 && wrkmap[x].active != 1) continue;
					/* if standby escalation message check if worker is in standby mode either skip him/her*/
					if(standby_workers_only == 1 && wrkmap[x].active != 2) continue;
					if(standby_workers_only == 0) { //if it is not a renotify, either standby escalation or normal notification
						//Check what was the last state that got send to THIS user
						notification_log_last_state=bartlby_notification_log_last_notification_state(shm_hdr,cfgfile,  local_svc.service_id, wrkmap[x].worker_id, (char*)trigger_name);
						if(notification_log_last_state != -1) { //If no log entry found be nice and send it out
							if(notification_log_last_state == local_svc.current_state) {
								_log(LH_TRIGGER, B_LOG_DEBUG,"FIX  FLAPPING BUG - %d - %d (%d) - svc_id: %d", notification_log_last_state, local_svc.current_state, standby_workers_only, local_svc.service_id);
								continue; //THIS SOLVES FLAPPING BUG
							}
						}
									
					}
															
						wrkmap[x].escalation_time=time(NULL);
						asprintf(&exec_str, "%s \"%s\" \"%s\" \"%s\" \"%s\" 2>&1", full_path, wrkmap[x].mail,wrkmap[x].icq,wrkmap[x].name, execline);
						_log(LH_PORTIER, B_LOG_HASTO, "@NOT@%ld|%d|%d|%s|%s|UPSTREAMED - %s", local_svc.service_id, local_svc.notify_last_state ,local_svc.current_state,trigger_name,wrkmap[x].name, execline);
						bartlby_notification_log_add(shm_hdr, cfgfile, wrkmap[x].worker_id, local_svc.service_id, local_svc.current_state, standby_workers_only, wrkmap[x].notification_aggregation_interval,  (char*)trigger_name);
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
				
	jso = json_object_new_object();
	json_object_object_add(jso,"error_code", json_object_new_int(0));
	json_object_object_add(jso,"trigger", json_object_new_string(trigger_name));
	json_object_object_add(jso,"output", json_object_new_string(trigger_return));
	
	printf("%s\n", json_object_to_json_string(jso));

	json_object_put(jso);

	


}
void bartlby_portier_get_plugin_info(long service_id) {
	int svc_found=0;
	int x;
	json_object * jso;


	for(x=0; x<shm_hdr->svccount; x++) {
		if(svcmap[x].service_id == service_id) {
			svc_found = 1;
			break;
		}
	}
	if(svc_found == 1) {
		if(svcmap[x].service_type == SVC_TYPE_PASSIVE) {
			
			jso=json_object_new_object();
			json_object_object_add(jso, "error_code", json_object_new_int(0));
			json_object_object_add(jso, "plugin", json_object_new_string(svcmap[x].plugin));
			json_object_object_add(jso, "args", json_object_new_string(svcmap[x].plugin_arguments));
			
			printf("%s\n", json_object_to_json_string(jso));
			
			json_object_put(jso);

		} else {
			bartlby_show_error(-441, "Service is not a Passive one", is_http);
		}
	} else {
		bartlby_show_error(-444, "Service not found", is_http);
	}
}


void bartlby_portier_find_services(long server_id) {
	int x;
	json_object * jso;
	json_object * jso_svc_array;

	jso = json_object_new_object();
	jso_svc_array = json_object_new_array();
	json_object_object_add(jso, "error_code", json_object_new_int(0));

	for(x=0; x<shm_hdr->svccount; x++) {
		if(svcmap[x].server_id == server_id && svcmap[x].service_type == SVC_TYPE_PASSIVE && svcmap[x].service_passive_timeout > 0) {
			if(service_is_in_time(svcmap[x].service_exec_plan)) {
				json_object_array_add(jso_svc_array,json_object_new_int64(svcmap[x].service_id));
			}
		}
	}
	json_object_object_add(jso, "services", jso_svc_array);

	printf("%s\n", json_object_to_json_string(jso));

	json_object_put(jso_svc_array);
	json_object_put(jso);

}
void bartlby_portier_find_server_id(const char * server_name) {
	int x;
	int found=0;
	json_object * jso;

	
    

	for(x=0; x<shm_hdr->svccount; x++) {
		if(strcmp(srvmap[svcmap[x].srv_place].server_name, server_name) == 0) {
			found=1;
			jso = json_object_new_object();
			json_object_object_add(jso, "error_code", json_object_new_int(0));
			json_object_object_add(jso, "server_id", json_object_new_int64(svcmap[x].server_id));
			printf("%s\n", json_object_to_json_string(jso));
			json_object_put(jso);
			break;
					
		}
	}
	if(found == 0) {
		bartlby_show_error(-333, "Server not found", is_http);
	}
	
	

}
void bartlby_output_http_headers() {
	printf("HTTP/1.1 200 OK\r\n");
	printf("Server: bartlby-portier\r\n");
	printf("Content-Type: text/json\r\n");
	printf("\r\n\r\n");
	http_headers_shown=1;
}
void bartlby_show_error(int code, char * msg, int http) {
	json_object * jso;
	if(http == 1 && http_headers_shown == 0) {
		bartlby_output_http_headers();
	}
	jso = json_object_new_object();
	json_object_object_add(jso, "error_code", json_object_new_int(code));
    json_object_object_add(jso, "error_msg", json_object_new_string(msg));

    printf("%s\n", json_object_to_json_string(jso));

    json_object_put(jso);
    fflush(stdout);
    if(http==1) {
		exit(1);    	
    }
    

}

void bartlby_portier_submit_passive_result(long service_id, int status, const char * message) {
	
	int svc_found, x;
	char * passive_beauty;
	json_object * jso;
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
			
			jso = json_object_new_object();
			json_object_object_add(jso, "error_code", json_object_new_int(0));
    		json_object_object_add(jso, "message", json_object_new_string("Passive Result submitted"));


    		printf("%s\n", json_object_to_json_string(jso));
    		json_object_put(jso);
			

			
			return;
		} else {
			bartlby_show_error(-22, "Service is not type 'Passive'", is_http);
			return;
		}
	} else {
		bartlby_show_error(-22, "Service not found", is_http);
		return;
	}


	
}




static void agent_conn_timeout(int signo) {
 	connection_timed_out = 1;
}
int main(int argc, char ** argv) {
	struct sigaction act1, oact1;
	
	char inputbuffer[4096];
	char cfgfile[2048];
	
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

//#define CMD_L_D
#ifndef CMD_L_D
#define ARGV_IDX 0
#else
#define ARGV_IDX 0
#endif


	//buffers:
	char textbuffer1[2048],textbuffer2[2048],textbuffer3[2048];
	long longbuffer1, longbuffer2, longbuffer3, longbuffer4, longbuffer5, longbuffer6, longbuffer7, longbuffer8;
	int intbuffer1, intbuffer2, intbuffer3,intbuffer4,intbuffer5,intbuffer6,intbuffer7;
	json_object * jsoo[10];
	


	snprintf(cfgfile,2047, "%s", argv[ARGV_IDX]);



	set_cfg(cfgfile);

#ifndef CMD_L_D	
	allowed_ip_list=getConfigValue("allowed_ips", cfgfile);
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
       
#endif
	
	shmtok = getConfigValue("shm_key", cfgfile);
	
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
	int loop_end=0;
	int bytes_read;
	connection_timed_out=0;

	while(loop_end == 0) {
		alarm(CONN_TIMEOUT);
		//ipmlg]ajgai]Amoowlkecvg~"/j"nmacnjmqv~
		bytes_read=read(fileno(stdin), inputbuffer, 4095);
		if(bytes_read < 0) {
			bartlby_show_error(-2, "error recv()", 0); //or actually timeout by alarm()
			exit(1);
		}
		inputbuffer[bytes_read]=0;
		alarm(0);
				
		
		is_http=0;
		if(strstr(inputbuffer, "HTTP/1.1") != NULL){
			is_http=1;
		}
		json_object *jso_in,*jso_method;
		char * json_input = strstr(inputbuffer, "{");
		if(json_input == NULL) {
			bartlby_show_error(-4, "NO JSON OBJECT", is_http);
			continue;		
		}
		jso_in=json_tokener_parse(json_input);
		if(jso_in) {

			if(json_object_object_get_ex(jso_in, "method", &jso_method)) {
				if(is_http) {
					bartlby_output_http_headers();
				}
				/*
				METHOD: "set_passive"
				PURPOSE: Submit Check Result for a Passive Services, usually invoked by a call to "bartlby_cmd"
				ON-ERROR: error_code < 0, and "error_msg" is set to a text
				<< {"method": "set_passive", "service_id": 1, "state":2, "passive_text": "asdsasddsa"} 
				>> {"error_code": 0, "message": "passive result submitted"}
				*/

				 if(strcmp(json_object_get_string(jso_method), "set_passive") == 0) {
					if(json_object_object_get_ex(jso_in, "service_id", &jsoo[0]) && 
					   json_object_object_get_ex(jso_in, "state", &jsoo[1]) &&
					   json_object_object_get_ex(jso_in, "passive_text", &jsoo[2])) {

							bartlby_portier_submit_passive_result(json_object_get_int64(jsoo[0]), json_object_get_int(jsoo[1]), json_object_get_string(jsoo[2]));
							PORTIER_CLEANUP;

					}

				 }
				 /*
				 METHOD: get_server_id
				 PURPOSE: get id of an server via the name - e.g look-up hostname on passive node and get the corresponding id's
				 ON-ERROR: error_code < 0 and "error_msg" set to a text
				 << {"method": "get_server_id", "server_name": "localhost"}
				 >> {"error_code": 0, "server_id", "27"}
				 */
				 if(strcmp(json_object_get_string(jso_method), "get_server_id") == 0) {
					if(json_object_object_get_ex(jso_in, "server_name", &jsoo[0])) {
							bartlby_portier_find_server_id(json_object_get_string(jsoo[0]));
							PORTIER_CLEANUP;

					}

				 }

				 /*
				 METHOD: exit
				 PURPOSE: closes connection
				 ON-ERROR: error_code < 0 and "error_msg" set to a text
				 << {"method": "exit"}
				 >> NONE
				 */
				 if(strcmp(json_object_get_string(jso_method), "exit") == 0) {
					exit(2);
				 }

				 /*
				 METHOD: server_checks_needed
				 PURPOSE: get a list of services from server specified by server_id, wich need to be checked - respecting there settings (e.g.: is timerange reached etc.)
				 ON-ERROR: error_code < 0 and "error_msg" set to a text
				 << {"method": "server_checks_needed", "server_id": 2}
				 >> {"error_code": 0, "services": [2,22,34,45]}
				 */
				 if(strcmp(json_object_get_string(jso_method), "server_checks_needed") == 0) {
					if(json_object_object_get_ex(jso_in, "server_id", &jsoo[0])) {
							bartlby_portier_find_services(json_object_get_int64(jsoo[0]));
							PORTIER_CLEANUP;

					}

				 }
				 /*
				 METHOD: plugin_info
				 PURPOSE: get the plugin info, executable_name and plugin_arguments of a given service_id
				 ON-ERROR: error_code < 0 and "error_msg" set to a text
				 << {"method": "plugin_info", "service_id": 9}
				 >> {"error_code": 0, "plugin": "bartlby_load.sh", "args": "-c 30" }
				 */
				 if(strcmp(json_object_get_string(jso_method), "plugin_info") == 0) {
					if(json_object_object_get_ex(jso_in, "service_id", &jsoo[0])) {
							bartlby_portier_get_plugin_info(json_object_get_int64(jsoo[0]));
							PORTIER_CLEANUP;

					}

				 }

				 /*
				 METHOD: exec_trigger
				 NOTE: THIS IS PW PROTECTED
				 PURPOSE: Execute a trigger - act as it was triggerd locally
				 ON-ERROR: error_code < 0 and "error_msg" set to a text
				 << {"method": "exec_trigger", 
				 	 "standby_workers_only": 0,
				 	 "execline": "a b c",
				 	 "trigger_name": "mail.sh",
				 	 "service_id": 9,
				 	 "server_id": 2,
				 	 "notifiy_last_state": 2,
				 	 "current_state": 0,
				 	 "recovery_outstanding": 0,
				 	 "node_id": 0,
				 	 "passwd": "password"
				 	}
				 >> {"error_code": 0, "trigger": "bartlby_load.sh", "output": "Sent SMS" }
				 */
				 if(strcmp(json_object_get_string(jso_method), "exec_trigger") == 0) {
					if( json_object_object_get_ex(jso_in, "standby_workers_only", &jsoo[0]) &&
						json_object_object_get_ex(jso_in, "execline", &jsoo[1]) &&
						json_object_object_get_ex(jso_in, "trigger_name", &jsoo[2]) &&
						json_object_object_get_ex(jso_in, "service_id", &jsoo[3]) &&
						json_object_object_get_ex(jso_in, "server_id", &jsoo[4]) &&
						json_object_object_get_ex(jso_in, "notifiy_last_state", &jsoo[5]) &&
						json_object_object_get_ex(jso_in, "current_state", &jsoo[6]) &&
						json_object_object_get_ex(jso_in, "recovery_outstanding", &jsoo[7]) &&
						json_object_object_get_ex(jso_in, "node_id", &jsoo[8]) &&
						json_object_object_get_ex(jso_in, "passwd", &jsoo[9])
						) {
							
							bartlby_portier_exec_trigger(cfgfile, 
														json_object_get_int(jsoo[0]),
														json_object_get_string(jsoo[1]),
														json_object_get_string(jsoo[2]),
														json_object_get_int64(jsoo[3]),
														json_object_get_int64(jsoo[4]),
														json_object_get_int(jsoo[5]),
														json_object_get_int(jsoo[6]),
														json_object_get_int(jsoo[7]),
														json_object_get_int(jsoo[8]),
														json_object_get_string(jsoo[9])
														);
			
							PORTIER_CLEANUP;

					}

				 }
				 /*
				 METHOD: exec_trigger_line
				 NOTE: THIS IS PW PROTECTED
				 PURPOSE: Executre a full prebuilt trigger line
				 ON-ERROR: error_code < 0 and "error_msg" set to a text
				 << {"method": "exec_trigger", 
				 	 "standby_workers_only": 0,
				 	 "execline": "a b c",
				 	 "trigger_name": "mail.sh",
				 	 "service_id": 9,
				 	 "server_id": 2,
				 	 "notifiy_last_state": 2,
				 	 "current_state": 0,
				 	 "recovery_outstanding": 0,
				 	 "node_id": 0,
				 	 "passwd": "password"
				 	}
				 >> {"error_code": 0, "execline": "/opt/bartlby/trigger/bartlby_load.sh", "output": "Sent SMS" }
				 */
				 if(strcmp(json_object_get_string(jso_method), "exec_trigger_line") == 0) {
						if(	json_object_object_get_ex(jso_in, "execline", &jsoo[0]) &&
							json_object_object_get_ex(jso_in, "passwd", &jsoo[1])
						) {
							
							bartlby_portier_exec_trigger_line(cfgfile, 
														json_object_get_string(jsoo[0]),
														json_object_get_string(jsoo[1])														
														);
			
							PORTIER_CLEANUP;

					}

				 }

				 //IF STILL ALIVE
				 // RAIS METHOD NOT IMPLEMENTED
				 bartlby_show_error(-999, "Method not implemented", is_http);

			} else {
				bartlby_show_error(-5, "NO METHOD SPECIFIED", is_http);
			}
			
		} else {
			bartlby_show_error(-6, "JSON PARSE ERROR", is_http);
			continue;
		}
		
		fflush(stdout);
		/*
		
		if(sscanf(inputbuffer, "8|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%2047[^\n|]s", &longbuffer1,  &longbuffer1, &longbuffer3, &longbuffer4,&longbuffer5,&longbuffer6,&longbuffer7, &longbuffer8, &textbuffer1) == 9) {
			//8|8|Plugin does not exist (/opt/bartlby-agent/plugins//init)|1|1409175456|1409175457|0|6|0| SVC OBJ STATE
			bartlby_portier_set_svc_state(longbuffer1, textbuffer1, longbuffer2,longbuffer3,longbuffer4,longbuffer5,longbuffer6,longbuffer7, longbuffer8);
			PORTIER_CLEANUP
			exit(1);
		}
		printf("'%s' - %d\n", textbuffer1, intbuffer1);
		//%d|%d|%s|%s|%d|%d|%d|%d|%d|%d|%s|\n
		printf("-1111 unkown command\n");
		exit(1);
		*/
	}

}



