/* $Id: shm.c,v 1.11 2007/07/27 22:54:04 hjanuschka Exp $ */
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
$Source: /cvsroot/bartlby/bartlby-core/src/shm.c,v $


$Log: shm.c,v $
Revision 1.11  2007/07/27 22:54:04  hjanuschka
int to long changing

Revision 1.10  2007/02/15 20:46:38  hjanuschka
auto commit

Revision 1.9  2007/01/29 04:04:04  hjanuschka
auto commit

Revision 1.8  2007/01/05 01:49:00  hjanuschka
auto commit

Revision 1.6  2006/10/05 23:19:37  hjanuschka
auto commit

Revision 1.5  2006/04/24 22:20:00  hjanuschka
core: event queue

Revision 1.4  2006/01/19 23:30:22  hjanuschka
introducing downtime's

Revision 1.3  2005/09/28 21:46:30  hjanuschka
converted files to unix
jabber.sh -> disabled core dumps -> jabblibs segfaults
                                    will try to patch it later

Revision 1.2  2005/08/28 16:02:59  hjanuschka
CVS Header


*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include <bartlby.h>



struct shm_header * bartlby_SHM_GetHDR(void * shm_addr) {
	return (struct shm_header *)(void *)shm_addr;
}

void bartlby_SHM_link_services_servers(void * shm_addr, char * cfgfile) {
	struct shm_header * hdr;
	struct server * srvmap;
	struct service * svcmap;
	int x;
	int y;
	int marker_found;
	
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	srvmap=bartlby_SHM_ServerMap(shm_addr);
	svcmap=bartlby_SHM_ServiceMap(shm_addr);
	
	for(x=0; x<hdr->svccount; x++) {
		for(y=0; y<hdr->srvcount; y++) {
			if(svcmap[x].server_id == srvmap[y].server_id) {
				//_log("linking: %s -> %s", svcmap[x].service_name, srvmap[y].server_name);
				svcmap[x].srv=&srvmap[y];
				svcmap[x].srv_place=y;
				svcmap[x].check_interval_original += 1+(int) (500.0*rand()/(RAND_MAX+1.0));
				
				
			}
				
		}	
	}
	for(y=0; y<hdr->srvcount; y++) {
		marker_found = 0;
		srvmap[y].dead_marker = NULL;
		if(srvmap[y].server_dead != 0) {
			
			for(x=0; x<hdr->svccount; x++) {
				if(svcmap[x].service_id == srvmap[y].server_dead) {
					srvmap[y].dead_marker = &svcmap[x];
					marker_found = 1;	
				}
			}
			
		}
		if(marker_found == 0) {
			
			if(srvmap[y].server_dead != 0) {
				_log("Service assigned as a alive-marker not found service_id: %d", srvmap[y].server_dead);	
			}
		}
	}
	
	_log("linked services with servers!");
	
}

struct btl_event * bartlby_SHM_EventMap(void * shm_addr) {
	//Is beyond the 3 integers :-)
	struct shm_header * hdr;
	struct server * srvmap;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	srvmap=bartlby_SHM_ServerMap(shm_addr);
	
	
	return (struct btl_event *)(void *)&srvmap[hdr->srvcount]+20;
}

struct server * bartlby_SHM_ServerMap(void * shm_addr) {
	//Is beyond the 3 integers :-)
	struct shm_header * hdr;
	struct service * svcmap;
	struct downtime * dtmap;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	
	svcmap=bartlby_SHM_ServiceMap(shm_addr);
	//wrkmap=(struct worker *)(void*)&svcmap[hdr->svccount]+20;
	dtmap=bartlby_SHM_DowntimeMap(shm_addr);
	
	return (struct server *)(void *)&dtmap[hdr->dtcount]+20;
}

struct downtime * bartlby_SHM_DowntimeMap(void * shm_addr) {
	//Is beyond the 3 integers :-)
	struct shm_header * hdr;
	struct service * svcmap;
	struct worker * wrkmap;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	
	svcmap=bartlby_SHM_ServiceMap(shm_addr);
	//wrkmap=(struct worker *)(void*)&svcmap[hdr->svccount]+20;
	wrkmap=bartlby_SHM_WorkerMap(shm_addr);
	
	return (struct downtime *)(void *)&wrkmap[hdr->wrkcount]+20;
}

struct worker * bartlby_SHM_WorkerMap(void * shm_addr) {
	//Is beyond the 3 integers :-)
	struct shm_header * hdr;
	struct service * svcmap;
	hdr=bartlby_SHM_GetHDR(shm_addr);
	
	svcmap=bartlby_SHM_ServiceMap(shm_addr);
	
	return (struct worker *)(void*)&svcmap[hdr->svccount]+20;
}

struct service * bartlby_SHM_ServiceMap(void * shm_addr) {
	//Is beyond the 3 integers :-)
	return (struct service *)(void *)shm_addr+sizeof(struct shm_header);
}
