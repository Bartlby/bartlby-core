
#include <bartlby.h>

static sig_atomic_t portier_connection_timed_out=0;
#define CONN_TIMEOUT 15



static void bartlby_portier_conn_timeout(int signo) {
 	portier_connection_timed_out = 1;
}

//Caller has to free
char * bartlby_portier_fetch_reply(int sock, int spool) {
		char buffer[2048];
		int rbytes;
		portier_connection_timed_out=0;

		if(spool == 1) {
			return strdup("{ 'error_code' : 0, 'error_msg': 'localfile'}");
		}

		alarm(5);
		rbytes=read(sock, buffer, 2047);
		if(rbytes < 0) {
			_log(LH_TRIGGER, B_LOG_CRIT, "Portier: fetching reply failed\n");
			return NULL;
		}
		if(portier_connection_timed_out == 1) {
			return NULL;
		} else {
			buffer[rbytes]=0;
			return strdup(buffer);
		}
		
}

void bartlby_portier_disconnect(int sock,int spool) {
	json_object * jso_out;

	if(spool == 1) {
		close(sock);
		return;
	}

	jso_out = json_object_new_object();
	json_object_object_add(jso_out, "method", json_object_new_string("exit"));
	bartlby_portier_send(jso_out, sock, spool);
	close(sock);

	json_object_put(jso_out);
}

int bartlby_portier_send_no_result(json_object * obj, int sock, int spool) {
	char * reply;
	json_object * json_error, * json_errormsg, *rjso;
	if(bartlby_portier_send(obj, sock, spool) >0) {
    	reply=bartlby_portier_fetch_reply(sock, spool);
    	rjso=json_tokener_parse(reply);
    	if(rjso && 
    		json_object_object_get_ex(rjso, "error_code", &json_error) && 
    		json_object_object_get_ex(rjso, "error_msg", &json_errormsg)) {
    		
    		if(json_object_get_int(json_error) < 0) {
				_log(LH_MAIN, B_LOG_CRIT, "Remote JSON COMMAND failed '%s'  with '%s'",json_object_to_json_string(obj), json_object_get_string(json_errormsg));
			}    else { 			
    			_log(LH_MAIN, B_LOG_DEBUG, "Remote JSON COMMAND '%s' returned with error_code '%ld'", json_object_to_json_string(obj), json_object_get_int(json_error));
    		}
    		json_object_put(rjso);
    	} else {
    		_log(LH_MAIN, B_LOG_CRIT, "JSON PARSE ERROR on '%s'", reply);
    	}
    	free(reply);
    }	
    //bartlby_portier_disconnect(sock, spool);
    return 0;
}
int bartlby_portier_send(json_object * obj, int sock,int spool) {

	const char * json_package = json_object_to_json_string(obj);

	if(spool == 1) {
		if(write(sock, json_package, strlen(json_package)) < 0) {
			return -1;
		}
		return 1; 
	}

	portier_connection_timed_out=0;
	alarm(5);
	if(write(sock, json_package, strlen(json_package)) < 0) {
			return -1;
	}
	alarm(0);
	if(portier_connection_timed_out == 1) {
		_log(LH_TRIGGER, B_LOG_DEBUG, "PORTIER: sending of json package timed out");
		return -1;
	}
	portier_connection_timed_out=0;
	return 1;
}

int bartlby_portier_connect(char *host_name,int port, int spool, char * cfgfile){
	int result;


	struct addrinfo hints, *res, *ressave;
	char ipvservice[20];
	int sockfd;
	char * spool_dir_cfg;
	char * spool_dir_tmpl;
	struct stat sb;
	mode_t mask;
	
	if(spool == 1) {
		spool_dir_cfg=getConfigValue("portier_spool_dir", cfgfile);
		if(spool_dir_cfg != NULL) {

			if (stat(spool_dir_cfg, &sb) == 0 && S_ISDIR(sb.st_mode)) {
				CHECKED_ASPRINTF(&spool_dir_tmpl, "%s/cmd_XXXXXX", spool_dir_cfg);
				
				//* SET UMASK - @@@coverity
				mask=umask(S_IXUSR | S_IRWXG | S_IRWXO);
				sockfd=mkstemp(spool_dir_tmpl);
				umask(mask);

				free(spool_dir_tmpl);
				free(spool_dir_cfg);
				return sockfd;
			} else {
				_log(LH_MAIN, B_LOG_CRIT, "portier_spool_dir does not exist (%s) - portier command is lost!", spool_dir_cfg);
				free(spool_dir_cfg);
				return -1;
			}
			
		}
	}



	sprintf(ipvservice, "%d",port);
	
	 memset(&hints, 0, sizeof(struct addrinfo));

   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
	
	 portier_connection_timed_out=0;
	
	
	 result = getaddrinfo(host_name, ipvservice, &hints, &res);
	 if(result < 0 || portier_connection_timed_out != 0) {
	 		return -7;
	}
	ressave = res;
	sockfd=-1;
	
	while (res) {
		
        sockfd = socket(res->ai_family,
                        res->ai_socktype,
                        res->ai_protocol);
        
        if (!(sockfd < 0)) {
            if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
                break;
            }
        	    
            close(sockfd);
            sockfd=-1;
            
        }
    res=res->ai_next;
  }
  freeaddrinfo(ressave);
 
	return sockfd;
	
	
}



int bartlby_portier_send_log(char * passive_host, int passive_port, char * passwd, char  * log_line, char * cfgfile) {

	int res;
	
	int rc;
	
	int client_socket;
	int client_connect_retval=-1;
	struct sigaction act1, oact1;
	

	json_object * jso_out, *jso_in, *jso_err_code, *jso_errmsg;

	
	portier_connection_timed_out=0;
	

	
	int do_spool=0;
	char * portier_spool=NULL;
	portier_spool=getConfigValue("portier_spool_commands", cfgfile);

	if(portier_spool != NULL && strcmp(portier_spool, "true") == 0) {
		do_spool=1;
	}
	if(portier_spool != NULL)  free(portier_spool);



	
	act1.sa_handler = bartlby_portier_conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		
		return -3; //timeout handler
	
		
	}
	
	alarm(5);
	client_socket = bartlby_portier_connect(passive_host, passive_port, do_spool, cfgfile);
	alarm(0);
	if(portier_connection_timed_out == 1 || client_socket == -1) {
		return -4; //connect
	} 
	portier_connection_timed_out=0;



	//#define SERVICE_UPDATE_TEXT "update services set , , ervice_last_notify_send=FROM_UNIXTIME(%d), service_last_state_change=FROM_UNIXTIME(%d), 
	jso_out = json_object_new_object();
	json_object_object_add(jso_out, "method", json_object_new_string("orch_log"));
	json_object_object_add(jso_out, "log_line", json_object_new_string(log_line));
	json_object_object_add(jso_out, "time_stamp", json_object_new_string(log_line));
	
	
	json_object_object_add(jso_out, "passwd", json_object_new_string(passwd));
	
	bartlby_portier_send_no_result(jso_out, client_socket, do_spool);


	bartlby_portier_disconnect(client_socket, do_spool);
	
	json_object_put(jso_out);

	
	//INIT JSON OBJECT and send "svc_status"


	return 0;
}

int bartlby_portier_send_svc_status(char * passive_host, int passive_port, char * passwd, struct service * svc, char * cfgfile) {

	int res;
	char verstr[2048];
	char cmdstr[2048];
	char result[2048];
	int rc;
	
	int client_socket;
	int client_connect_retval=-1;
	struct sigaction act1, oact1;
	

	json_object * jso_out, *jso_in, *jso_err_code, *jso_errmsg;


	int do_spool=0;
	char * portier_spool=NULL;
	portier_spool=getConfigValue("portier_spool_commands", cfgfile);

	if(portier_spool != NULL && strcmp(portier_spool, "true") == 0) {
		do_spool=1;
	}
	if(portier_spool != NULL)  free(portier_spool);


	
	portier_connection_timed_out=0;
	

	
	
	
	act1.sa_handler = bartlby_portier_conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		
		return -3; //timeout handler
	
		
	}
	
	alarm(5);
	client_socket = bartlby_portier_connect(passive_host, passive_port,do_spool, cfgfile);
	alarm(0);
	if(portier_connection_timed_out == 1 || client_socket == -1) {
		return -4; //connect
	} 
	portier_connection_timed_out=0;



	//#define SERVICE_UPDATE_TEXT "update services set , , ervice_last_notify_send=FROM_UNIXTIME(%d), service_last_state_change=FROM_UNIXTIME(%d), 
	jso_out = json_object_new_object();
	json_object_object_add(jso_out, "method", json_object_new_string("orch_service_status"));
	json_object_object_add(jso_out, "service_id", json_object_new_int64(svc->service_id));
	
	json_object_object_add(jso_out, "handled", json_object_new_int(svc->handled));
	json_object_object_add(jso_out, "service_retain_current", json_object_new_int(svc->service_retain_current));
	json_object_object_add(jso_out, "service_ack_current", json_object_new_int(svc->service_ack_current));
	json_object_object_add(jso_out, "last_check", json_object_new_int(svc->last_check));
	json_object_object_add(jso_out, "new_server_text", json_object_new_string(svc->new_server_text));
	json_object_object_add(jso_out, "current_state", json_object_new_int(svc->current_state));
	json_object_object_add(jso_out, "last_notify_send", json_object_new_int(svc->last_notify_send));
	json_object_object_add(jso_out, "last_state_change", json_object_new_int(svc->last_state_change));


	json_object_object_add(jso_out, "passwd", json_object_new_string(passwd));
	
	bartlby_portier_send_no_result(jso_out, client_socket, do_spool);


	bartlby_portier_disconnect(client_socket,do_spool);
	
	json_object_put(jso_out);

	


	return 0;
}

int bartlby_portier_send_trigger(char * passive_host, int passive_port, int to_standbys,char * trigger_name, char * execline, struct service * svc, int node_id, char * portier_passwd, char * cfgfile) {
	int res;
	char verstr[2048];
	char cmdstr[2048];
	char result[2048];
	int rc;
	
	int client_socket;
	int client_connect_retval=-1;
	struct sigaction act1, oact1;
	

	json_object * jso_out, *jso_in, *jso_err_code, *jso_errmsg;

	
	int do_spool=0;
	char * portier_spool=NULL;
	portier_spool=getConfigValue("portier_spool_commands", cfgfile);

	if(portier_spool != NULL && strcmp(portier_spool, "true") == 0) {
		do_spool=1;
	}
	if(portier_spool != NULL)  free(portier_spool);


	portier_connection_timed_out=0;
	

	
	
	
	act1.sa_handler = bartlby_portier_conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		
		return -3; //timeout handler
	
		
	}

	alarm(5);
	client_socket = bartlby_portier_connect(passive_host, passive_port, do_spool, cfgfile);
	alarm(0);
	if(portier_connection_timed_out == 1 || client_socket == -1) {
		return -4; //connect
	} 
	portier_connection_timed_out=0;

	

		if(svc != NULL) {
			jso_out = json_object_new_object();
			json_object_object_add(jso_out, "method", json_object_new_string("exec_trigger"));
			json_object_object_add(jso_out, "standby_workers_only", json_object_new_int(to_standbys));
			json_object_object_add(jso_out, "execline", json_object_new_string(execline));
			json_object_object_add(jso_out, "trigger_name", json_object_new_string(trigger_name));
			json_object_object_add(jso_out, "service_id", json_object_new_int64(svc->service_id));
			json_object_object_add(jso_out, "server_id", json_object_new_int64(svc->server_id));
			json_object_object_add(jso_out, "notifiy_last_state", json_object_new_int(svc->notify_last_state));
			json_object_object_add(jso_out, "current_state", json_object_new_int(svc->current_state));
			json_object_object_add(jso_out, "recovery_outstanding", json_object_new_int(svc->recovery_outstanding));
			json_object_object_add(jso_out, "node_id", json_object_new_int(node_id));
			json_object_object_add(jso_out, "passwd", json_object_new_string(portier_passwd));
			json_object_object_add(jso_out, "service_name", json_object_new_string(svc->service_name));
			
			bartlby_portier_send_no_result(jso_out, client_socket, do_spool);

			json_object_put(jso_out);			
		} else {
			jso_out = json_object_new_object();
			json_object_object_add(jso_out, "method", json_object_new_string("exec_trigger_line"));
			json_object_object_add(jso_out, "execline", json_object_new_string(execline));
			json_object_object_add(jso_out, "passwd", json_object_new_string(portier_passwd));
			
			bartlby_portier_send_no_result(jso_out, client_socket, do_spool);

			json_object_put(jso_out);	



		}

	bartlby_portier_disconnect(client_socket,do_spool);

	return 0;
}









