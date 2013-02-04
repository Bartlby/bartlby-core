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


static sig_atomic_t connection_timed_out=0;


static void bartlby_conn_timeout(int signo) {
 	connection_timed_out = 1;
}

void bartlby_check_active(struct service * svc, char * cfgfile) {
	int client_socket;
	
	int return_bytes;
	
	
	char return_buffer[1024];
	char * client_request;
	
	
	int result;
	
	
	
	
	
	/*
	NEW
	*/
	char * rmessage;
	int sum_rmessage;
	
	struct sigaction act1, oact1;
	
	
	act1.sa_handler = bartlby_conn_timeout;


	if(sigemptyset(&act1.sa_mask) < 0) {

		sprintf(svc->new_server_text, "%s", ALARM_ERROR);
		svc->current_state=STATE_CRITICAL;

		return;


	}	

	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif

	if(sigaction(SIGALRM, &act1, &oact1) < 0) {

		sprintf(svc->new_server_text, "%s", ALARM_ERROR);
		svc->current_state=STATE_CRITICAL;

		return;


	}
	
	//sleep(40);
	connection_timed_out=0;
	alarm(svc->service_check_timeout);
	result = bartlby_agent_tcp_connect(svc->srv->client_ip,svc->srv->client_port,&client_socket, svc);
	
	if(connection_timed_out == 1) {
		sprintf(svc->new_server_text, "%s", "timed out");
		svc->current_state=STATE_CRITICAL;	
		return;
	}
	if(result != STATE_OK) {
		svc->current_state=STATE_CRITICAL;
		return;
	}
	/*
		SEND Request
	*/
	connection_timed_out=0;
	asprintf(&client_request, "%s| %s|", svc->plugin, svc->plugin_arguments);
	
	
	//Send it
	alarm(svc->service_check_timeout);
	send(client_socket, client_request, (strlen(svc->plugin)+strlen(svc->plugin_arguments)+3),0);
	//Check it
	if(connection_timed_out == 1) {
		
		
		sprintf(svc->new_server_text, "%s", CONN_ERROR);
		svc->current_state=STATE_CRITICAL;
		free(client_request);
		return;
	}
	
	free(client_request);
	
	//Recieve Reply
	rmessage=malloc(sizeof(char)*(1024*4));
	memset(rmessage, '\0', sizeof(char)*(1024*4));
	alarm(svc->service_check_timeout);
	connection_timed_out=0;
	sum_rmessage=0;
	while((return_bytes=recv(client_socket, return_buffer, 1024, 0)) > 0 &&  sum_rmessage < 1024*4 && connection_timed_out == 0) {
		return_buffer[return_bytes] = '\0';
		strcat(rmessage, return_buffer);		
		sum_rmessage += return_bytes;
		
	}
	

	
	//Check if timed out or Receive error
	if(connection_timed_out == 1 || sum_rmessage <= 0) {
		
		if(connection_timed_out == 1) {
			sprintf(svc->new_server_text, "%s", TIMEOUT_ERROR);
		} else if(sum_rmessage <= 0) {
			_log("%d recv error() '%s'", return_bytes, rmessage);
			sprintf(svc->new_server_text, "%s %d", RECV_ERROR, return_bytes);
		} 
		svc->current_state=STATE_CRITICAL;
		free(rmessage);
		return;
	}
	
	alarm(0);
	close(client_socket);
	
	
	bartlby_action_handle_reply(svc, rmessage, cfgfile);
	
	free(rmessage);
	
        return;
		
}

void bartlby_action_handle_reply(struct service * svc, char * rmessage, char * cfgfile) {
	int char_idx=0, cur_char_idx=0;
	
	char * curr_line;
	int data_is_ok;
	
	cur_char_idx=0;
	char_idx=0;
	curr_line=malloc(sizeof(char) * (strlen(rmessage)+20));
	
	 
	
	data_is_ok=0;
   	while(char_idx < strlen(rmessage)) {
   		//_log("%c", rmessage[cur_char_idx]);
   		
   		curr_line[cur_char_idx]=rmessage[char_idx];
   		
   		if(rmessage[char_idx] == '\n' || char_idx == strlen(rmessage) || char_idx == 1023) { //END of rmessage buffer :)
   			curr_line[cur_char_idx]='\0';
   			
   			if(strlen(curr_line) > 0) {

   				data_is_ok=bartlby_action_handle_reply_line(svc, curr_line, cfgfile);
   				if(data_is_ok == 1) {
   					break;
   				}
   			}
   			
   			cur_char_idx=0;	
   			char_idx++;
   			continue;	
   		}
   		cur_char_idx++;
   		char_idx++;
   	}
   	
	free(curr_line);
	
	if(data_is_ok != 1) {
		//Maybe we did'nt receive any data like 0|Result\n
		sprintf(svc->new_server_text, "%s (1)", PROTOCOL_ERROR);
		svc->current_state=STATE_CRITICAL;	
	}
	
}
int bartlby_action_handle_reply_line(struct service * svc, char * line, char * cfgfile) {
	char * return_token;
	
	if(strlen(line) == 0) {
		return 2;	
	}
	if(strncmp(line, "OS:", 3) == 0) {
		//_log("OS: '%s'", line);
		return 0;	
	}	
	if(strncmp(line, "PERF: ", 6) == 0) {
		//_log("PERF: '%s'", line);
		bartlby_perf_track(svc,line, strlen(line), cfgfile);	
		return 0;
	}
	//_log("DATA: '%s'", line);
	return_token = strtok(line, "|");
        if(return_token != NULL) {
        	//Verfiy result code to be 0-2 :-) 
        	if(return_token[0] != '0' && return_token[0] != '1' && return_token[0] != '2' && return_token[0] != '4' && return_token[0] != '3') {
        		svc->current_state=STATE_UNKOWN;	
        		return 0;
        	} else {
        		svc->current_state=atoi(return_token);
        	}
        	
        	return_token = strtok(NULL, "|");
        	if(return_token != NULL) {
        		sprintf(svc->new_server_text, "%s", return_token);
        		//see if we have nagios plugin perf-data
        		return_token = strtok(NULL, "|");
        		if(return_token != NULL) {
        				//append the nagiosperfdata after the normal output 
        				strcat(svc->new_server_text, " - |");
        				strcat(svc->new_server_text, return_token);
        		}
        		
        	} else {
        		
        		sprintf(svc->new_server_text, "(empty output)");
        		
        	}	
        	return 1;
        } else {
        	
        	sprintf(svc->new_server_text, PROTOCOL_ERROR);
        	svc->current_state=STATE_CRITICAL;
        	return 1;
        }
	
	
	
}
