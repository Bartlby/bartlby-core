#include "bartlby.h"


#define CONFIG "/opt/bartlby/etc/bartlby.cfg"


#define LOAD_SYMBOL_TEST(x,y,z) 	x=dlsym(y, z); \
    	if((dlmsg=dlerror()) != NULL) { \
        	printf("dl error: %s\n", dlmsg);	\
			return 1;	\
    	}


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

int main(int argc, char ** argv) {
	void * SOHandle;
	void * bartlby_address;
	
	int (*TestSQL)(char*,char*);

	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);
	

	LOAD_SYMBOL_TEST(TestSQL,SOHandle, "TestSQL");

	TestSQL(CONFIG,"asdasd'das'");

	dlclose(SOHandle);



}



