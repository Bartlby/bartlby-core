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
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>	
#include <unistd.h>	

#include <dlfcn.h>

#include <bartlby.h>

int main(int argc, char ** argv) {
	
	char * shmtok;
	int shm_id;
	//int * shm_elements;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct service * svcmap;
	struct server * srvmap;
	int x;
	
	
	
	if(argc < 3) {
		printf("CFGFILE option");
		exit(2);	
	}
	shmtok = getConfigValue("shm_key", argv[1]);
	if(shmtok == NULL) {
		printf("Unset variable `shm_key'\n");
		exit(2);
	}
	shm_id = shmget(ftok(shmtok, 32), 0, 0777);
	if(shm_id != -1) {
		/*
		
		struct worker * wrkmap;
		struct service * svcmap;
		
		wrkmap=bartlby_SHM_WorkerMap(bartlby_address);
		svcmap=bartlby_SHM_ServiceMap(bartlby_address);
		printf("Services:\n");
		for(x=0; x<shm_hdr->svccount; x++) 
			printf("\t%s:%d/%s %d\n", svcmap[x].server_name, svcmap[x].client_port, svcmap[x].service_name, svcmap[x].current_state);
		printf("Workers:\n");
		for(x=0; x<shm_hdr->wrkcount; x++) 
			printf("\t%s\n", wrkmap[x].mail);
		
		printf("Current running checks: %d\n", shm_hdr->current_running);
		shm_hdr->do_reload=1;
		
		
		free(shmtok);
		*/
		
		if(strcmp(argv[2], "status") == 0) {
			bartlby_address=shmat(shm_id,NULL,0);
			shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
			
			printf("%d\t%ld\t%ld\t%ld\t%ld\n", shm_id, shm_hdr->svccount, shm_hdr->wrkcount, shm_hdr->dtcount, shm_hdr->current_running);
			
			shmdt(bartlby_address); 
			exit(1);
		} else if ( strcmp(argv[2], "list") == 0 ) {
			bartlby_address=shmat(shm_id,NULL,0);
			svcmap=bartlby_SHM_ServiceMap(bartlby_address);
			shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
			srvmap=bartlby_SHM_ServerMap(bartlby_address);
			
			for(x=0; x<shm_hdr->svccount; x++) {
				printf("%ld;%s;%d;%s;%d;\n", svcmap[x].service_id, srvmap[svcmap[x].srv_place].server_name, srvmap[svcmap[x].srv_place].client_port, svcmap[x].service_name, svcmap[x].current_state);
			}
			exit(1);
		}
		printf("Unknown option: status|remove|list");
		
		
	} else {
		printf("SHM doesnt exist is %s running\n", PROGNAME);
		exit(2);
	}	
		
		
	
	return 1;
}
