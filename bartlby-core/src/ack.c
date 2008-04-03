/* $Id: ack.c,v 1.4 2008/03/15 18:25:28 hjanuschka Exp $ */
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
$Revision: 1.4 $
$Source: /cvsroot/bartlby/bartlby-core/src/ack.c,v $


$Log: ack.c,v $
Revision 1.4  2008/03/15 18:25:28  hjanuschka
auto commit

Revision 1.3  2008/03/11 20:35:04  hjanuschka
auto commit

Revision 1.2  2007/02/15 16:25:32  hjanuschka
auto commit

Revision 1.1  2006/02/10 23:54:46  hjanuschka
SIRENE mode added

*/

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>  
#include <unistd.h>
#include <sys/time.h>


#include <bartlby.h>


static int last_sirene=0;

void bartlby_check_sirene(char * configfile, void * bartlby_address) {
	struct service svc;
	char * cfg_sirene_wait;
	int sirene_retry;
	int diff;
	
	
	
	cfg_sirene_wait=getConfigValue("sirene_interval", configfile);
	if(cfg_sirene_wait == NULL) {
		_log("'sirene_interval' not set defaulting to 600 seconds re-notify");
		sirene_retry=600;
	} else {
		sirene_retry=atoi(cfg_sirene_wait);
		free(cfg_sirene_wait);
	}
	
		
	diff=time(NULL)-last_sirene;
	
	if(diff <= sirene_retry) {
		
		sleep(2); //usually ok and be nice to CPU
		return;	
	}
	last_sirene=time(NULL);
	
	//sprintf(svc.client_ip, "0.0.0.0");
	//sprintf(svc.server_name, "CORE");
	svc.srv=malloc(sizeof(struct server));
	if(svc.srv == NULL) {
		_log("malloc failed in bartlby_check_sirene()");
		return;	
	}
	
	
	sprintf(svc.srv->client_ip, "0.0.0.0");
	sprintf(svc.srv->server_name, "CORE");
	
	sprintf(svc.service_name, "SIRENE");
	sprintf(svc.new_server_text, "Your system is in sirene mode all checks have been disabled please give the UI a bit attention");
	svc.current_state=STATE_SIRENE;
	//svc.client_port=0;
	svc.last_state=STATE_SIRENE;
	
	_log("!!! System is in sirene Mode !!!!");
	bartlby_trigger(&svc,configfile, bartlby_address, 0, 0);
	
	
	
}
