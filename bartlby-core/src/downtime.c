/* $Id: downtime.c,v 1.1 2006/01/19 23:30:22 hjanuschka Exp $ */
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
$Revision: 1.1 $
$Source: /cvsroot/bartlby/bartlby-core/src/downtime.c,v $


$Log: downtime.c,v $
Revision 1.1  2006/01/19 23:30:22  hjanuschka
introducing downtime's

Revision 1.2  2006/01/16 20:51:41  hjanuschka
performance stuff moved to perf.c
timeing information on perf handler

Revision 1.1  2005/12/29 20:05:55  hjanuschka
core statistic (should be used in debug mode only produces a biiiig file)

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



#include <bartlby.h>

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
			}
			if(is_down > 0) {
				//_log("@DOWNTIME@%s:%d/%s - %s (%d)", svc->server_name, svc->client_port, svc->service_name, dtmap[x].downtime_notice, is_down);		
				return -1;
			}
		}
	}
	return 1;
}

