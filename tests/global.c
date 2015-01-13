#include "bartlby.h"

#include "tinytest.h"
#include "tinytest_macros.h"
#include "bartlby_test.h"

const char * dlmsg;

int bartlby_is_running(char * cfgfile) {
	char * pid_dir, * pidfile;
	char PID[50];
	FILE * fp;

	pid_dir = getConfigValue("pidfile_dir", cfgfile);

	if (asprintf( &pidfile, "%s/bartlby.pid", pid_dir ) == -1) {                             \
       printf("ASPRINTF FAILED");
       free(pid_dir);
       return -1;
    }

    free(pid_dir);
    
    fp = fopen(pidfile, "r");
    if(fp == NULL) {
    	
    	free(pidfile);
    	return -1;
    }
    
    if(fgets(PID, 49, fp) == NULL) {
    	free(pidfile);
    	return -1;
    }
    fclose(fp);
    free(pidfile);
    if(kill(atoi(PID), 0) < 0 ) {
    	return -1;
    }
    return 1;



}

void * bartlby_get_shm(char * cfgfile) {
	char * shmtok;
	void * bartlby_address;
	int shm_id;
	
	shmtok = getConfigValue("shm_key", cfgfile);
	if(shmtok == NULL) {
		return NULL;	
	}		
	
	
	shm_id = shmget(ftok(shmtok, 32), 0,  0777);
	free(shmtok);
	if(shm_id != -1) {
		bartlby_address=shmat(shm_id,NULL,0);
		return bartlby_address;
	} else {
		printf("cannot attach to SHM\n");	
		return NULL;
	}
}


void * bartlby_get_sohandle(char * cfgfile) {
	char * data_lib;
	char * dlmsg;
	void * SOHandle;
	
	data_lib=getConfigValue("data_library", cfgfile);
	if(data_lib == NULL) {
		printf("cannot find data_lib key in %s\n", cfgfile);	
		return NULL;
	}
	SOHandle=dlopen(data_lib, RTLD_LAZY);
	if((dlmsg=dlerror()) != NULL) {
		printf( "DL Error: %s\n", dlmsg);
        	return NULL;
    	}	
    	free(data_lib);
    	return SOHandle;
} 
