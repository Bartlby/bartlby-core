#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include <errno.h>

#include <bartlby.h>



static struct shm_header * gHdr;
static void * gDataLoaderHandle;
static char * gCFG;
static char * distr_command;


	




static int distributive_service_state_changed(struct service * svc) {
		
	char * dcmd;
	char dist_out[1024];
	
	//_log("distributive: STATE changed: %s:%d/%s", svc->server_name, svc->client_port, svc->service_name);
	
	FILE * fp;
	
	if(distr_command != 0) {
		
		dcmd = malloc(sizeof(char) * (strlen(distr_command)+5300));
		
		snprintf(dcmd, 5299, "%s %ld %ld '%s' '%s' %d '%s'", distr_command, svc->server_id, svc->service_id, svc->srv->server_name, svc->service_name, svc->current_state, svc->new_server_text);
		
		fp = popen(dcmd, "r");
		if(fp) {
			if(fgets(dist_out, 1024, fp) != NULL) {
				_log("distributive: %s", dist_out);	
			}
			pclose(fp);
						
		} else {
			_log("distributive: %s", strerror(errno));	
		}
		
		free(dcmd);
		
	}
	
	
	
	
	
	
	return EXTENSION_OK;
}

int bartlby_extension_dispatcher(int type, void * data) {
	int rtc;
	
	switch(type) {
		
		case EXTENSION_CALLBACK_STATE_CHANGED:
			rtc=distributive_service_state_changed((struct service *)data);
			return rtc;
		break;		
		default:
		
		return EXTENSION_OK;
			
		
			
	}
	return EXTENSION_OK;
}



int bartlby_extension_startup(void * shm_addr, void * dataLoaderHandle, char * configfile) {
	_log("distributive: %s", configfile);
	
	distr_command  = getConfigValue("distributive_command", configfile);
	
	
	if(distr_command == NULL) {
		_log("distributive: configuration failed 'distributive_command'");
	}
	
	
	
	gHdr=bartlby_SHM_GetHDR(shm_addr);
	gDataLoaderHandle=dataLoaderHandle;
	gCFG=configfile;
	_log("distributive initiated");
	
	return EXTENSION_OK;
}
int bartlby_extension_shutdown(int scheduler_end_code) {
	_log("distributive: scheduler ended with %d", scheduler_end_code);
	
	return EXTENSION_OK;
}
