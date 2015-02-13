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

#define trigger_debug(format, ...) fprintf(stderr, format);
#define CONN_TIMEOUT 15
static sig_atomic_t connection_timed_out=0;


static void trigger_conn_timeout(int signo) { //TIMEOUT HANDLER FOR trigger.sh
 	connection_timed_out = 1;
}

void bartlby_trigger_local(char * cfgfile, struct service *svc, struct worker *wrk, struct trigger *trig, char *msg)
{
	FILE * ptrigger;
	char trigger_return[1024];
	struct sigaction act1, oact1;
	
	act1.sa_handler = trigger_conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		printf("ALARM SETUP ERROR");
		exit(1);
	}

	
	ptrigger = popen(trig->trigger_full_path, "w");
	if (ptrigger != NULL) {
		connection_timed_out = 0;
		alarm(CONN_TIMEOUT);
		fprintf(ptrigger, "%s\n",msg);
		if (fgets(trigger_return, 1024, ptrigger) != NULL) {
			trigger_return[strlen(trigger_return) - 1] = '\0';
			//_log(LH_TRIGGER, B_LOG_DEBUG, "@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'%s'", svc->service_id, svc->last_state , svc->current_state, trig->trigger_name, wrk->name, srvmap[svc->srv_place].server_name, srvmap[svc->srv_place].client_port, svc->service_name, trigger_return);

		} else {
			//_log(LH_TRIGGER, B_LOG_DEBUG, "@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'(empty output)'", svc->service_id, svc->last_state , svc->current_state, trig->trigger_name, wrk->name, srvmap[svc->srv_place].server_name, srvmap[svc->srv_place].client_port, svc->service_name);
		}

		if (connection_timed_out == 1) {
			//_log(LH_TRIGGER, B_LOG_DEBUG, "@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'(timed out)'", svc->service_id, svc->last_state , svc->current_state, trig->trigger_name, wrk->name, srvmap[svc->srv_place].server_name, srvmap[svc->srv_place].client_port, svc->service_name);
		}
		connection_timed_out = 0;
		alarm(0);
		if (ptrigger != NULL) {
			pclose(ptrigger);
		}
	} else {
		//_log(LH_TRIGGER, B_LOG_DEBUG, "@NOT-EXT@%ld|%d|%d|%s|%s|%s:%d/%s|'(failed %s)'", svc->service_id, svc->last_state , svc->current_state, trig->trigger_name, wrk->name, srvmap[svc->srv_place].server_name, srvmap[svc->srv_place].client_port, svc->service_name, full_path);
	}
	

}