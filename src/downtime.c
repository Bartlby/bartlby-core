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
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>	
#include <unistd.h>	
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>

#include <dlfcn.h>
#include <sys/time.h>


#include <bartlby.h>


int bartlby_svc_is_in_svcgroup(void * bartlby_address, struct service * svc, int servicegroup_id) {
	struct shm_header * hdr;
	
	struct servicegroup * svcgrpmap;
	struct servergroup * srvgrpmap;
	int is_member;
	
	char * group_has_service;
	int y;
	
	is_member=0;
	hdr=bartlby_SHM_GetHDR(bartlby_address);
	
	
	srvgrpmap=bartlby_SHM_ServerGroupMap(bartlby_address);
	svcgrpmap=bartlby_SHM_ServiceGroupMap(bartlby_address);
	
	
	
	group_has_service = malloc(sizeof(char)*8);
	for(y=0; y<hdr->svcgroupcount; y++) {
		
		
		sprintf(group_has_service, "|%ld|", svc->service_id);
		if(strstr(svcgrpmap[y].servicegroup_members, group_has_service) != NULL) {
			if(svcgrpmap[y].servicegroup_id == servicegroup_id) {
				is_member=1;
				break;		
			}
			
			
		}
		
	}
	
	free(group_has_service);
	if(is_member == 1) {
			//_log("SVC: %s is membership in group %s", svc->service_name,svcgrpmap[y].servicegroup_name); 
			return 1; //svc is a member of group
	}
	
	
	return 0;	
}

int bartlby_svc_is_in_srvgroup(void * bartlby_address,struct service * svc, int servergroup_id) {

	struct shm_header * hdr;
	
	struct servicegroup * svcgrpmap;
	struct servergroup * srvgrpmap;
	int is_member;
	
	char * group_has_server;
	int y;
	
	is_member=0;
	hdr=bartlby_SHM_GetHDR(bartlby_address);
	
	
	srvgrpmap=bartlby_SHM_ServerGroupMap(bartlby_address);
	svcgrpmap=bartlby_SHM_ServiceGroupMap(bartlby_address);
	
	
	
	group_has_server = malloc(sizeof(char)*8);
	for(y=0; y<hdr->srvgroupcount; y++) {
		
		
		sprintf(group_has_server, "|%ld|", svc->srv->server_id);
		if(strstr(srvgrpmap[y].servergroup_members, group_has_server) != NULL) {
			if(srvgrpmap[y].servergroup_id == servergroup_id) {
				is_member=1;
				break;		
			}
			
			
		}
		
	}
	
	free(group_has_server);
	if(is_member == 1) {
			//_log("SVC: %s is membership in group %s", svc->service_name,svcgrpmap[y].servicegroup_name); 
			return 1; //svc is a member of group
	}
	
	
	
	return 0;	
}

int bartlby_is_in_downtime(void * bartlby_address, struct service * svc) {
	struct shm_header * shm_hdr;
	struct downtime * dtmap;
	int x;
	int current_time;
	int is_down;
	
	shm_hdr=(struct shm_header *)(void *)bartlby_address;
	dtmap=bartlby_SHM_DowntimeMap(bartlby_address);
	
	current_time=time(NULL);
	
	is_down=0;
	
	for(x=0; x<shm_hdr->dtcount; x++) {
		//_log("DT: (%d) %d - %d -> %s", current_time, dtmap[x].downtime_from, dtmap[x].downtime_to, dtmap[x].downtime_notice);		
		if(current_time >= dtmap[x].downtime_from && current_time <= dtmap[x].downtime_to) {
			switch(dtmap[x].downtime_type) {
				case DT_SERVICE:
					if(dtmap[x].service_id == svc->service_id) {
						is_down=1;	
					}
				break;
				case DT_SERVER:
					if(dtmap[x].service_id == svc->server_id) {
						is_down=2;	
					}
				break;
				case DT_SERVERGROUP:
					//Check if svc->srv is a member of servergroup supplied with dtmap.service_id
					if(bartlby_svc_is_in_srvgroup(bartlby_address, svc, dtmap[x].service_id) > 0) {
						is_down=3;
					}
				break;
				case DT_SERVICEGROUP:
					//Check if svc is a member of servicegroup supplied with dtmap.service_id
					
					if(bartlby_svc_is_in_svcgroup(bartlby_address, svc, dtmap[x].service_id) > 0) {
						is_down=3;
					}
					
				break;
								
			}
			
			if(is_down > 0) {
					//We are in downtime - act as check has been done - so re-evaluation of "is in downtime"
					//happens in X Seconds
					
					//_log("@DOWNTIME@%s:%d/%s - %s (%d)", svc->srv->server_name, svc->srv->client_port, svc->service_name, dtmap[x].downtime_notice, is_down);		
					
					
				return -1;
			}
		}
	}
	return 1;
}

