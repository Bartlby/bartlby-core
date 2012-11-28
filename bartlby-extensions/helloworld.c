#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include <bartlby.h>

#define MY_CHECK_TYPE_ID 700

static struct shm_hdr * gHdr;
static void * gDataLoaderHandle;
static char * gCFG;


int helloworld_service_pre_check(struct service * svc) {
	//_log("helloworld: PRE check: %s:%d/%s", svc->server_name, svc->client_port, svc->service_name);
	return EXTENSION_OK;
}
int helloworld_service_post_check(struct service * svc) {
	//_log("helloworld: POST check: %s:%d/%s", svc->server_name, svc->client_port, svc->service_name);

	return EXTENSION_OK;
}
int helloworld_service_state_changed(struct service * svc) {
	//_log("helloworld: STATE changed: %s:%d/%s", svc->server_name, svc->client_port, svc->service_name);
	return EXTENSION_OK;
}
int helloworld_service_trigger_pre(struct service * svc) {
	//_log("helloworld: trigger_pre %s:%d/%s", svc->server_name, svc->client_port, svc->service_name);
	return EXTENSION_OK;
}
int helloworld_sched_wait(struct service * svc) {
	//_log("helloworld: SCHED_WAIT: %s:%d/%s", svc->server_name, svc->client_port, svc->service_name);
	return EXTENSION_OK;
}


int helloworld_unkw_check_type(struct service * svc) {
	//Implement your own check type
	// see svc->service_type if it is YOUR ID return EXTENSION_NOK so no one else can get your check
	
	return EXTENSION_OK;
}

int helloworld_check_time(struct service * svc) {
	//Get the svc wich has currently being perf counted
	return EXTENSION_OK;	
}
int helloworld_round_time(struct shm_hdr * hdr) {
	////Get the svc wich has currently being perf counted
	return EXTENSION_OK;	
}
int helloworld_event_pushed(struct btl_event * ev) {
	return EXTENSION_OK;	
}
int helloworld_replication(void) {
	return EXTENSION_OK;	
}

int bartlby_extension_dispatcher(int type, void * data) {
	int rtc;
	
	switch(type) {
		case EXTENSION_CALLBACK_PRE_CHECK:
			rtc=helloworld_service_pre_check((struct service *)data);
			return rtc;
		break;
		case EXTENSION_CALLBACK_POST_CHECK:
			rtc=helloworld_service_post_check((struct service *)data);
			return rtc;
			
			
		break;
		case EXTENSION_CALLBACK_STATE_CHANGED:
			rtc=helloworld_service_state_changed((struct service *)data);
			return rtc;
		break;
		
		case EXTENSION_CALLBACK_TRIGGER_PRE:
			rtc=helloworld_service_trigger_pre((struct service *)data);
			return rtc;
		break;
		case EXTENSION_CALLBACK_SCHED_WAIT:
			rtc=helloworld_sched_wait((struct service *)data);
			return rtc;
		break;
		case EXTENSION_CALLBACK_UNKOWN_CHECK_TYPE:
			rtc=helloworld_unkw_check_type((struct service *)data);
			return rtc;
		break;
		case EXTENSION_CALLBACK_ROUND_TIME:
			rtc=helloworld_round_time((struct shm_hdr *) data);
			return rtc;
		break;
		case EXTENSION_CALLBACK_CHECK_TIME:
			rtc=helloworld_check_time((struct service *) data);
			return rtc;
		break;
		case EXTENSION_CALLBACK_EVENT_PUSHED:
			rtc=helloworld_event_pushed((struct btl_event *)data);
			return rtc;
		break;
		case EXTENSION_CALLBACK_REPLICATION_GO:
			rtc=helloworld_replication();
			return rtc;
		break;
		
		default:
			_log("helloworld unkown message:%d", type);
			//Be gentle if we do not know what to do
			//do as all is OK to not block bartlby
			return EXTENSION_OK;
			
		
			
	}
	return EXTENSION_OK;
}


int bartlby_extension_startup(void * shm_addr, void * dataLoaderHandle, char * configfile) {
	_log("helloworld: %s", configfile);
	gHdr=shm_addr;
	gDataLoaderHandle=dataLoaderHandle;
	gCFG=configfile;
	return EXTENSION_OK;
}
int bartlby_extension_shutdown(int scheduler_end_code) {
	_log("helloworld: scheduler ended with %d", scheduler_end_code);
	
	return EXTENSION_OK;
}
