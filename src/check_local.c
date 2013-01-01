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

void bartlby_check_local(struct service * svc, char * cfgfile) {
	struct sigaction act1, oact1;
	char * file_request;
	char c;
	char * plugin_dir;
	FILE * fp;
	int plugin_rtc;
	
	int round;
	char * rmessage, *rmessage_temp;
	struct stat plg_stat;
	
	act1.sa_handler = bartlby_conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		
		sprintf(svc->new_server_text, "%s", ALARM_ERROR);
		svc->current_state=STATE_CRITICAL;
				
		return;
	
		
	}
	plugin_dir=getConfigValue("agent_plugin_dir", cfgfile);
        if(plugin_dir == NULL) {
        	
        	sprintf(svc->new_server_text, "Plugin dir failed 'agent_plugin_dir' not set");
		svc->current_state=STATE_CRITICAL;		  	
		return;
        }
	connection_timed_out=0;
	
	asprintf(&file_request, "%s/%s",plugin_dir, svc->plugin);
	
	if(stat(file_request, &plg_stat) < 0) {
		//oops file is not here
		sprintf(svc->new_server_text, "Plugin does not exist in %s",file_request );
		svc->current_state=STATE_CRITICAL;		  	
		free(plugin_dir);
		free(file_request);	
		return;
	}
	
	free(file_request);
	asprintf(&file_request, "%s/%s %s 2>/dev/null",plugin_dir, svc->plugin, svc->plugin_arguments);
	
	
	signal(SIGPIPE,SIG_DFL);
	signal(SIGCHLD,SIG_DFL);
		
	fp=popen(file_request, "r");
	if(fp != NULL) {
		connection_timed_out=0;
		alarm(svc->service_check_timeout);
		
		
		rmessage_temp=malloc(sizeof(char)*(1024*4));
		rmessage=malloc(sizeof(char)*(1024*4));
		memset(rmessage, '\0', sizeof(char)*(1024*4));
		memset(rmessage_temp, '\0', sizeof(char)*(1024*4));
		round=0;
		while((c=fgetc(fp)) != EOF && connection_timed_out != 1 && round < 1024*4){
			rmessage_temp[round]=c;	
			round++;
		}
		plugin_rtc=pclose(fp);
		
		rmessage_temp[round]='\0';
		
		
		alarm(0);
		
		
		
		if(connection_timed_out == 1) {
			//alarm has reached
			
			sprintf(svc->new_server_text, "Timed out");
			svc->current_state=STATE_CRITICAL;
			return;
				
		} 
		
		
		bartlby_check_grep_perf_line(rmessage_temp, svc, cfgfile);
		connection_timed_out = 0;
		sprintf(rmessage, "%d|%s", WEXITSTATUS(plugin_rtc), rmessage_temp); 
		
		bartlby_action_handle_reply(svc, rmessage, cfgfile);
		
		free(rmessage_temp);
		free(rmessage);
		
	} else {
		sprintf(svc->new_server_text, "popen failed on (%s)", file_request);
		svc->current_state=STATE_CRITICAL;
	}	
	free(file_request);
	free(plugin_dir);
	
	return;	
}
