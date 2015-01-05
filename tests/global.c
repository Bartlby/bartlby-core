#include "bartlby.h"

#include "tinytest.h"
#include "tinytest_macros.h"
#include "bartlby_test.h"

const char * dlmsg;
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
