/* $Id: check_group.c,v 1.3 2008/01/24 14:10:16 hjanuschka Exp $ */
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
$Revision: 1.3 $
$Source: /cvsroot/bartlby/bartlby-core/src/check_group.c,v $


$Log: check_group.c,v $
Revision 1.3  2008/01/24 14:10:16  hjanuschka
auto commit

Revision 1.2  2007/02/15 16:25:32  hjanuschka
auto commit

Revision 1.1  2006/11/25 01:16:00  hjanuschka
auto commit


*/
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <bartlby.h>




void bartlby_check_group(struct service * svc, void * shm_addr) {
	struct shm_header * hdr;
	struct service * svcmap;


	char del[]="|";
	char tmp_svcvar[2048];	
	char * tok;
	int svc_id;
	int state;
	struct service * svg;
	int x;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	svcmap=bartlby_SHM_ServiceMap(shm_addr);	
	
	if(svc->service_var[0] == '\0')  {
		
		
		sprintf(svc->new_server_text, "%s", GROUP_WITHOUT_PARMS);
		svc->current_state=STATE_CRITICAL;	
	} else {
		strcpy(tmp_svcvar, svc->service_var);
		
		tok=strtok(tmp_svcvar, del);
		while(tok != NULL) {
			
			sscanf(tok, "%d=%d", &svc_id, &state);
			
			
			
			svg=NULL;
			for(x=0; x<hdr->svccount; x++) {
				
				if(svcmap[x].service_id == svc_id) {
					svg=&svcmap[x];
				}
			}	
			if(svg == NULL) { 
				
				sprintf(svc->new_server_text, "%s (Service: %d not found)", GROUP_CRITICAL, svc_id);
				svc->current_state=STATE_CRITICAL;
				return;
			}
			
			if(bartlby_is_in_downtime(shm_addr, svg) > 0 && svg->current_state == state) {
				//_log("Service: is not %d\n", svg->current_state );
				sprintf(svc->new_server_text, "%s %s:%d/%s - %d", GROUP_CRITICAL, svg->srv->server_name, svg->srv->client_port, svg->service_name, state);
				svc->current_state=STATE_CRITICAL;
				
				return;	
			}
			
			tok=strtok(NULL, del);
			
		}
		
		sprintf(svc->new_server_text, "%s", GROUP_OK);
		svc->current_state=STATE_OK;
	}
		
		
}

