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



struct shm_header * bartlby_SHM_GetHDR(void * shm_addr) {
	return (struct shm_header *)(void *)shm_addr;
}

void bartlby_SHM_link_services_servers(void * shm_addr, char * cfgfile, void * SOHandle) {
	struct shm_header * hdr;
	struct server * srvmap;
	struct service * svcmap;
	struct servicegroup * svcgrpmap;
	struct servergroup * srvgrpmap;
	int x;
	int y;
	int marker_found;
	int default_server_group_index;
	int default_service_group_index;
	
	const char * dlmsg;
	char * group_has_server;
	char * group_has_service;
	char * autodelete_orphaned_services;
	
	int (*DeleteService)(int, char *); 
	
	default_server_group_index=-1;
	default_service_group_index=-1;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	srvmap=bartlby_SHM_ServerMap(shm_addr);
	svcmap=bartlby_SHM_ServiceMap(shm_addr);
	
	srvgrpmap=bartlby_SHM_ServerGroupMap(shm_addr);
	svcgrpmap=bartlby_SHM_ServiceGroupMap(shm_addr);
	
	
	
	LOAD_SYMBOL(DeleteService, SOHandle, "DeleteService");
	
	autodelete_orphaned_services=getConfigValue("autodelete_orphaned_services", cfgfile);
	if(autodelete_orphaned_services == NULL) {
		autodelete_orphaned_services=strdup("true");	
	}
	
	for(x=0; x<hdr->svccount; x++) {
		svcmap[x].srv_place=-1;
		for(y=0; y<hdr->srvcount; y++) {
			if(svcmap[x].server_id == srvmap[y].server_id) {
				
				svcmap[x].srv=&srvmap[y];
				svcmap[x].srv_place=y;
				svcmap[x].check_interval_original += 1+(int) (500.0*rand()/(RAND_MAX+1.0));
				
			
				
				
			}
				
		}
		if(	svcmap[x].srv_place == -1) {
			_log("Service-ID: %d is orphaned named %s", svcmap[x].service_id, svcmap[x].service_name);
			svcmap[x].service_active = 0;
			if(strcmp(autodelete_orphaned_services,"true") == 0) {	
				DeleteService(svcmap[x].service_id, cfgfile);
				hdr->do_reload=1;
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
		if(strcmp(srvgrpmap[y].servergroup_name, "DEFAULT") == 0) {
			default_server_group_index=y;
			sprintf(srvgrpmap[y].servergroup_members, "%s", "");
			continue;
		}
		for(x=0; x<hdr->srvcount; x++) {
			
			asprintf(&group_has_server, "|%ld|", srvmap[x].server_id);
			
			if(strstr(srvgrpmap[y].servergroup_members, group_has_server) != NULL) {
				
				srvmap[x].servergroups[srvmap[x].servergroup_counter] = &srvgrpmap[y];
				srvmap[x].servergroup_place[srvmap[x].servergroup_counter]=y;
				srvmap[x].servergroup_counter++;
				
				//_log("\t\thas server: %s", srvmap[x].server_name);

			}
						
			free(group_has_server);
		}
	}
	
	
						
		

	
	
	
	for(y=0; y<hdr->svcgroupcount; y++) {
		//_log("SVC-GROUP NAME: %s", svcgrpmap[y].servicegroup_name);
		if(strcmp(svcgrpmap[y].servicegroup_name, "DEFAULT") == 0) {
			default_service_group_index=y;
			sprintf(svcgrpmap[y].servicegroup_members, "%s", "");
			continue;
		}
		for(x=0; x<hdr->svccount; x++) {
			
			asprintf(&group_has_service, "|%ld|", svcmap[x].service_id);
			
			if(strstr(svcgrpmap[y].servicegroup_members, group_has_service) != NULL) {
				
				svcmap[x].servicegroups[svcmap[x].servicegroup_counter] = &svcgrpmap[y];
				svcmap[x].servicegroup_place[svcmap[x].servicegroup_counter]=y;
				svcmap[x].servicegroup_counter++;
				//_log("\t\thas service: %s", svcmap[x].service_name);
				//_log("LINKED SERVICE ON PLACE: %d to group on place %d", x, y);
			}
						
			free(group_has_service);
		}
	}
	
	
	//Link Group DeadMarkers
	for(y=0; y<hdr->svcgroupcount; y++) {
		marker_found=0;
		svcgrpmap[y].dead_marker=NULL;
		for(x=0; x<hdr->svccount; x++) {
			if(svcmap[x].service_id == svcgrpmap[y].servicegroup_dead) {
				marker_found=1;
				svcgrpmap[y].dead_marker=&svcmap[x];
			}
		}
		if(marker_found == 0 && svcgrpmap[y].servicegroup_dead != 0) {
			_log("Service assigned as a alive-marker for service-group  %d not found service_id: %d", svcgrpmap[y].servicegroup_id, svcgrpmap[y].servicegroup_dead);	
		}
		
	}
	
	for(y=0; y<hdr->srvgroupcount; y++) {
		marker_found=0;
		srvgrpmap[y].dead_marker=NULL;
		for(x=0; x<hdr->svccount; x++) {
			if(svcmap[x].service_id == srvgrpmap[y].servergroup_dead) {
				marker_found=1;
				srvgrpmap[y].dead_marker=&svcmap[x];
			}
		}
		if(marker_found == 0 && srvgrpmap[y].servergroup_dead != 0) {
			_log("Service assigned as a alive-marker for server-group  %d not found service_id: %d", srvgrpmap[y].servergroup_id, srvgrpmap[y].servergroup_dead);	
		}
		
	}
	
	
	//If we have a "DEFAULT" GROUP add all unassigned servers to it
	if (default_server_group_index>-1) {
	
		for(x=0; x<hdr->srvcount; x++) {
							
					if(srvmap[x].servergroup_counter == 0) {
						asprintf(&group_has_server, "|%d|", srvmap[x].server_id);
						
						srvmap[x].servergroups[srvmap[x].servergroup_counter] = &srvgrpmap[default_server_group_index];
						srvmap[x].servergroup_place[srvmap[x].servergroup_counter]=default_server_group_index;
						srvmap[x].servergroup_counter++;
						//FIXME enlarge membership possibility
						if(strlen(srvgrpmap[default_server_group_index].servergroup_members)+strlen(group_has_server) < GROUP_MEMBER_STR_LENGTH) {
							strcat(srvgrpmap[default_server_group_index].servergroup_members, group_has_server);
						} 
						
						
						free(group_has_server);
					}
								
	
		}
		
	}	
	
	//If we have a "DEFAULT" GROUP add all unassigned services to it
	if (default_service_group_index>-1) {
			for(x=0; x<hdr->svccount; x++) {
				
				if(svcmap[x].servicegroup_counter == 0) {
					
					
					asprintf(&group_has_service, "|%ld|", svcmap[x].service_id);
				
					
					svcmap[x].servicegroups[svcmap[x].servicegroup_counter] = &svcgrpmap[default_service_group_index];
					svcmap[x].servicegroup_place[svcmap[x].servicegroup_counter]=default_service_group_index;
					svcmap[x].servicegroup_counter++;
					
					//FIXME enlarge membership possibility
					if(strlen(svcgrpmap[default_service_group_index].servicegroup_members)+strlen(group_has_service) < GROUP_MEMBER_STR_LENGTH) {
						strcat(svcgrpmap[default_service_group_index].servicegroup_members, group_has_service);
					}
					
					
					free(group_has_service);
			}	
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
	
	
	return (struct servicegroup *)(void *)&srvgrpmap[hdr->srvgroupcount+1];
}

struct servergroup * bartlby_SHM_ServerGroupMap(void * shm_addr) {
	//Is beyond the events
	struct shm_header * hdr;
	struct btl_event * evmap;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	evmap=bartlby_SHM_EventMap(shm_addr);
	
	
	return (struct servergroup *)(void *)&evmap[EVENT_QUEUE_MAX+1];
}

struct btl_event * bartlby_SHM_EventMap(void * shm_addr) {
	//Is beyond the 3 integers :-)
	struct shm_header * hdr;
	struct server * srvmap;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	srvmap=bartlby_SHM_ServerMap(shm_addr);
	
	
	return (struct btl_event *)(void *)&srvmap[hdr->srvcount+1];
}

struct server * bartlby_SHM_ServerMap(void * shm_addr) {
	//Is beyond the 3 integers :-)
	struct shm_header * hdr;
	struct service * svcmap;
	struct downtime * dtmap;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	
	svcmap=bartlby_SHM_ServiceMap(shm_addr);
	//wrkmap=(struct worker *)(void*)&svcmap[hdr->svccount];
	dtmap=bartlby_SHM_DowntimeMap(shm_addr);
	
	return (struct server *)(void *)&dtmap[hdr->dtcount+1];
}

struct downtime * bartlby_SHM_DowntimeMap(void * shm_addr) {
	//Is beyond the 3 integers :-)
	struct shm_header * hdr;
	struct service * svcmap;
	struct worker * wrkmap;
	
	hdr=bartlby_SHM_GetHDR(shm_addr);
	
	svcmap=bartlby_SHM_ServiceMap(shm_addr);
	//wrkmap=(struct worker *)(void*)&svcmap[hdr->svccount];
	wrkmap=bartlby_SHM_WorkerMap(shm_addr);
	
	return (struct downtime *)(void *)&wrkmap[hdr->wrkcount+1];
}

struct worker * bartlby_SHM_WorkerMap(void * shm_addr) {
	//Is beyond the 3 integers :-)
	struct shm_header * hdr;
	struct service * svcmap;
	hdr=bartlby_SHM_GetHDR(shm_addr);
	
	svcmap=bartlby_SHM_ServiceMap(shm_addr);
	
	return (struct worker *)(void*)&svcmap[hdr->svccount+1];
}

struct service * bartlby_SHM_ServiceMap(void * shm_addr) {
	//Is beyond the 3 integers :-)
	return (struct service *)(void *)(shm_addr+sizeof(struct shm_header));
}
