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

DUMP:
		./a.out dump  /opt/bartlby  bartlby-dump.shm
	REPLAY
		./a.out replay /opt/bartlby1  bartlby-dump.shm 20000
		
		
*/



#include <bartlby.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>


int dumpMem(const char *start, const char *stop, const char *programName, char * file)
{
	const char *pos = start;
	ssize_t bytesWritten;

	/* If we have a huge memory area, we have to be careful
	 * with the size we pass to write. Otherwise the ssize_t
	 * might overflow. So we do the huge chunks first with
	 * this special loop. */
	FILE * fp = fopen(file, "w");
	while((uintptr_t)stop-(uintptr_t)pos > (uintptr_t)SSIZE_MAX)
	{
		bytesWritten = write(fileno(fp), pos, SSIZE_MAX);
		if(bytesWritten <= 0)
		{
			fprintf(stderr, "%s: Error writing to standard output: %s\n",
					programName, strerror(errno));
			fclose(fp);
			return -2;
		}
		else
			pos += bytesWritten;
	}

	/* Now dump the rest. */
	while(pos != stop)
	{
		bytesWritten = write(fileno(fp), pos, stop-pos);
		if(bytesWritten <= 0)
		{
			fprintf(stderr, "%s: Error writing to standard output: %s\n",
					programName, strerror(errno));
			fclose(fp);
			return -2;
		}
		else
			pos += bytesWritten;
	}
	fclose(fp);
	/* If we reach this, everything worked fine. */
	return 0;
}

int dumpShm(int id, int byKey, const char *param, const char *programName, char * file)
{
	struct shmid_ds infoBuffer;
	const char *shm;
	int status;

	/* Put the segment into our address space. We do this even before
	 * we query its size, to make sure the segment does not disappear
	 * (i.e. get deleted) between getting the size and attaching it. */
	shm = shmat(id, NULL, SHM_RDONLY);
	if(shm == (const char *)-1)
	{
		if(byKey)
			fprintf(stderr, "%s: Cannot attach the shared memory segment with the key \"%s\": %s\n",
					programName, param, strerror(errno));
		else
			fprintf(stderr, "%s: Cannot attach the shared memory segment with the id \"%s\": %s\n",
					programName, param, strerror(errno));
		return -1;
	}

	/* Now we need to know how big our segment is. */
	if(shmctl(id, IPC_STAT, &infoBuffer) == -1)
	{
		if(byKey)
			fprintf(stderr, "%s: Cannot determine the size of the shared memory segment with the key \"%s\": %s\n",
					programName, param, strerror(errno));
		else
			fprintf(stderr, "%s: Cannot determine the size of the shared memory segment with the id \"%s\": %s\n",
					programName, param, strerror(errno));

		shmdt(shm);
		return -1;
	}

	/* Dump memory and save status */
	status = dumpMem(shm, shm + infoBuffer.shm_segsz, programName, file);

	shmdt(shm);
	return status;
}

int replay_shm(char * k, char * f, long size) {
  int gshm_id;
	void * gBartlby_address;
	long gSHMSize = size;
	FILE * fp;
	int ch;
	char * shm_char;
	int x;
		
	printf("KEY => %s - FILE=>%s", k, f);
	gshm_id = shmget(ftok(k, 32), gSHMSize,IPC_CREAT | IPC_EXCL | 0777);
		if(gshm_id < 0) {
			//REUSE
			fprintf(stdout, "REUSED\n");
			gshm_id = shmget(ftok(k, 32), gSHMSize,IPC_CREAT | 0777);
		}
		gBartlby_address=shmat(gshm_id,NULL,0);
		fp = fopen(f, "r");
		x=0;
		while(!feof(fp)) {
				 ch = fgetc(fp);
				 shm_char = gBartlby_address;
				 shm_char[x] = ch;
				 x++; 
		}
		fclose(fp);
		return 0;
}

int dumpSHMToFile(char * keystr, char * file) {
	//Write SHM To File
	char *numEnd;
	char *programName="bartlby_shm_dump";
	intmax_t key_signed;
	uintmax_t key_unsigned;
	int id;

	
	id = shmget(ftok(keystr,32), 0, 0);
	if(id == -1)
	{
		fprintf(stderr, "%s: Cannot open a shared memory segment with the key \"%s\": %s\n",
				programName, keystr, strerror(errno));
		return -1;
	}

	return dumpShm(id, 1, keystr, programName, file);	
}


int main(int argc, char ** argv) {
	
	char * shmtok;
	int shm_id;
	//int * shm_elements;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct service * svcmap;
	struct server * srvmap;
	int x;
	key_t key;
	
	if ( strcmp(argv[1], "dump") == 0 ) {
			x = dumpSHMToFile(argv[2], argv[3]);		
			if(x == 0) {
				printf("Created File at %s - of SHM %s\n", argv[2], argv[3]);
			}
			exit(1);
		} else if ( strcmp(argv[1], "replay") == 0 ) {
			x = replay_shm(argv[2], argv[3], atol(argv[4]));		
			if(x == 0) {
				printf("FILLED SHM %s - with data from %s\n", argv[2], argv[3]);
			}
			exit(1);
	}else if ( strcmp(argv[1], "ftok") == 0 ) {
			key = ftok(argv[2], 32);
			printf("0x%jx\n", (uintmax_t)key);
			exit(1);
	}else if ( strcmp(argv[1], "expectcore") == 0 ) {
			
			printf("%d", EXPECTCORE);
			exit(1);
	}
	
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
				printf("%ld;%s;%d;%s;%d;%s\n", svcmap[x].service_id, srvmap[svcmap[x].srv_place].server_name, srvmap[svcmap[x].srv_place].client_port, svcmap[x].service_name, svcmap[x].current_state,  svcmap[x].new_server_text);
			}
			exit(1);
		} else if ( strcmp(argv[2], "reload") == 0 ) {
			bartlby_address=shmat(shm_id,NULL,0);
			shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
			shm_hdr->do_reload=1;
			printf("Reload Scheduled\n");
			exit(1);
		} 
		else if ( strcmp(argv[2], "lock_for_db") == 0 ) {
			bartlby_address=shmat(shm_id,NULL,0);
			shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
			shm_hdr->do_reload=2;
			while(shm_hdr->do_reload != 3) {
				sleep(1);
			}
			printf("DB LOCKED\n");
			exit(1);
		}
		else if ( strcmp(argv[2], "unlock_for_db") == 0 ) {
			bartlby_address=shmat(shm_id,NULL,0);
			shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
			shm_hdr->do_reload=0;
			
			printf("DB unlocked\n");
			exit(1);
		}
		printf("Unknown option: status|remove|list");
		
		
	} else {
		printf("SHM doesnt exist is %s running\n", PROGNAME);
		exit(2);
	}	
		
		
	
	return 1;
}
