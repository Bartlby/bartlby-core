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
	struct servicegroup * svcgrpmap;
	struct servergroup * srvgrpmap;
	int x;
	int y;
	int marker_found;
	
	
	char * group_has_server;
	char * group_has_service;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	srvmap=bartlby_SHM_ServerMap(shm_addr);
	svcmap=bartlby_SHM_ServiceMap(shm_addr);
	
	srvgrpmap=bartlby_SHM_ServerGroupMap(shm_addr);
	svcgrpmap=bartlby_SHM_ServiceGroupMap(shm_addr);
	
	
	
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
	//Link Groups :)
		//Server
	for(y=0; y<hdr->srvgroupcount; y++) {
		//_log("SRV-GROUP NAME: %s", srvgrpmap[y].servergroup_name);
		for(x=0; x<hdr->srvcount; x++) {
			group_has_server = malloc(sizeof(char)*8);
			sprintf(group_has_server, "|%ld|", srvmap[x].server_id);
			
			if(strstr(srvgrpmap[y].servergroup_members, group_has_server) != NULL) {
				
				srvmap[x].servergroups[srvmap[x].servergroup_counter] = &srvgrpmap[y];
				srvmap[x].servergroup_counter++;
				//_log("\t\thas server: %s", srvmap[x].server_name);

			}
						
			free(group_has_server);
		}
	}
	
	
	for(y=0; y<hdr->svcgroupcount; y++) {
		//_log("SVC-GROUP NAME: %s", svcgrpmap[y].servicegroup_name);
		for(x=0; x<hdr->svccount; x++) {
			group_has_service = malloc(sizeof(char)*8);
			sprintf(group_has_server, "|%ld|", svcmap[x].service_id);
			
			if(strstr(svcgrpmap[y].servicegroup_members, group_has_service) != NULL) {
				
				svcmap[x].servicegroups[svcmap[x].servicegroup_counter] = &svcgrpmap[y];
				svcmap[x].servicegroup_counter++;
				//_log("\t\thas service: %s", svcmap[x].service_name);

			}
						
			free(group_has_service);
		}
	}
	
	
	_log("linked services with servers!");
	
}

struct servicegroup * bartlby_SHM_ServiceGroupMap(void * shm_addr) {
	//Is beyond the servergroups
	struct shm_header * hdr;
	struct servergroup * srvgrpmap;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	srvgrpmap=bartlby_SHM_ServerGroupMap(shm_addr);
	
	
	return (struct servicegroup *)(void *)&srvgrpmap[hdr->srvgroupcount]+20;
}

struct servergroup * bartlby_SHM_ServerGroupMap(void * shm_addr) {
	//Is beyond the events
	struct shm_header * hdr;
	struct btl_event * evmap;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	evmap=bartlby_SHM_EventMap(shm_addr);
	
	
	return (struct servergroup *)(void *)&evmap[EVENT_QUEUE_MAX]+20;
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
