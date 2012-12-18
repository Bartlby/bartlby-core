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
	int x;
		//Check if service group is enabled to run checks
		if(svc->servicegroup_counter == 0) {
				///service is not member of a group
				return 0;
		}
		//Loop Threw service Groups
		for(x=0; x<svc->servicegroup_counter; x++) {
			if(svc->servicegroups[x]->servicegroup_id == servicegroup_id) {
				return 1;	
			}
		}
		
		
	
	return 0;	
}

int bartlby_svc_is_in_srvgroup(void * bartlby_address,struct service * svc, int servergroup_id) {

	int x;
		//Check if server group is enabled to run checks
		if(svc->srv->servergroup_counter == 0) {
				///Server is not member of a group
				return 0;
		}
		//Loop Threw Server Groups
		for(x=0; x<svc->srv->servergroup_counter; x++) {
			if(svc->srv->servergroups[x]->servergroup_id == servergroup_id) {
				
				return 1;
			}
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
						is_down=4;
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

