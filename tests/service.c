#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"



/*
cproto -I../../../include -I/usr/include/curl -I/usr/include/json  service.c
Test Library  Functions

int UpdateServiceInterval(struct service *svc, char *config); -  DONE
int doUpdate(struct service *svc, char *config); -  DONE
long ServiceChangeId(long from, long to, char *config); - DONE
int GetServiceById(long service_id, struct service *svc, char *config); - DONE
int UpdateService(struct service *svc, char *config); - DONE
int DeleteService(long service_id, char *config); - DONE
long AddService(struct service *svc, char *config); - DONE
int GetServiceMap(struct service *svcs, char *config, int orch_id); - DONE

*/


extern char * dlmsg;

//DEFINE SAMPLE DATA:
struct service dummy_service = {
	.service_id = 999,
	.server_id = 1,
	.last_state = 0,
	.current_state = 0,
	.current_output = "Unit Output",
	.service_name = "Unit-Service",
	.plugin = "bartlby_load",
	.plugin_arguments = "-w 10 -c 5 -p",
	.check_interval = 60,
	.check_interval_original = 60,
	.last_check = 9999999,
	.lcheck  = 0,
	.service_exec_plan = "",
	.notify_enabled = 1,
	.last_notify_send = 9999,
	.last_state_change = 9999,
	.flap_count = 0,
	.service_active = 1,
	.service_var = "",
	.service_type = SVC_TYPE_ACTIVE,
	.service_passive_timeout = 120,
	.notify_last_state = 0,
	.service_check_timeout = 20,
	.service_ack_enabled = 0,
	.service_ack_current = 0,
	.service_retain = 2,
	.service_retain_current = 0,
	.pstat = {
			.sum=0,
			.counter=0
	},
	.delay_time = {
			.sum=0,
			.counter=0
	},
	.do_force = 0,
	.snmp_info =  {
		.community="public",
		.version = 2,
		.objid="11111",
		.warn=10,
		.crit=20,
		.type = SNMP_CHECK_TYPE_GREATER,
		.textmatch=""
	},
	.recovery_outstanding = 0,
	.process = {
		.start_time = 999,
		.pid = 0
	},
	.flap_seconds=122,
	.srv = NULL,
	.srv_place = 0,
	.is_server_dead = 0,
	.renotify_interval = 0, // interval to renotify
	.escalate_divisor = 0, //
	.is_gone = 0,
	.servicegroups=NULL,
	.servicegroup_counter=0,
	.servicegroup_place=0,
	.fires_events = 0,
	.enabled_triggers ="",
	.handled = 0,
	.orch_id = TEST_ORCH_ID,
	.last_orch_sync = 9999,
	.usid = "USID",
	.prio = 100,
	.notify_super_users = 0	
};






/// TEST FUNCTIONS
void test_service_lib(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;

	struct shm_header * shm_hdr;
	struct service modified_object;
	struct service returned_object;


	struct service * svcmap;
	
	svcmap=NULL;
	long (*AddService)(struct service *, char *);
	int (*UpdateService)(struct service *, char *);
	int (*DeleteService)(long service_id, char *);
	int (*GetServiceById)(long, struct service *, char * );
	long (*ServiceChangeId)(long, long, char*);
	int (*GetServiceMap)(struct service*, char*, int);
	int (*doUpdate)(struct service*, char*);
	int (*UpdateServiceInterval)(struct service *, char *);


	int rtc=-1;
	long object_id=-1;
	long lrtc=-1;
	int x;

	long NN=999;

	bartlby_address=NULL;
	SOHandle=NULL;
	SKIP_IF_NOT_RUNNING(CONFIG);

	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);

	tt_ptr_op(bartlby_address, !=, NULL);
	tt_ptr_op(SOHandle, !=, NULL);
	
	
	shm_hdr = bartlby_SHM_GetHDR(bartlby_address);
	

	
	LOAD_SYMBOL_TEST(AddService,SOHandle, "AddService");
	LOAD_SYMBOL_TEST(DeleteService,SOHandle, "DeleteService");
	LOAD_SYMBOL_TEST(UpdateService,SOHandle, "UpdateService");	
	LOAD_SYMBOL_TEST(GetServiceById,SOHandle, "GetServiceById");	
	LOAD_SYMBOL_TEST(ServiceChangeId,SOHandle, "ServiceChangeId");	
	LOAD_SYMBOL_TEST(GetServiceMap,SOHandle, "GetServiceMap");	
	LOAD_SYMBOL_TEST(doUpdate, SOHandle, "doUpdate");
	LOAD_SYMBOL_TEST(UpdateServiceInterval, SOHandle, "UpdateServiceInterval");


	/******* ADD Service ****/
	lrtc=AddService(&dummy_service, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	TT_DECLARE("INFO",("... Added Service id: %ld", object_id));
	/******* ADD service ****/

	

	/******* MODIFY Service ****/
	dummy_service.service_id=object_id;
	memcpy(&modified_object,&dummy_service, sizeof(struct service));
	memcpy(&returned_object,&dummy_service, sizeof(struct service));
	snprintf(modified_object.service_name,2048, "modified-unit-test");
	rtc=UpdateService(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Modified Service, changed name"));
	/******* MODIFY Service ****/

	
	/******* GETSERVICEBYID ****/
	rtc=GetServiceById(object_id, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_str_op(returned_object.service_name, ==, modified_object.service_name);
	TT_DECLARE("INFO",("... get service by id %ld", object_id));
	/******* GETSERVICEBYID ****/

	/******* SERVICECHANGEID ****/
	NN=lrtc+999;
	object_id=ServiceChangeId(lrtc, NN, CONFIG);
	tt_int_op(object_id, ==, NN);
	TT_DECLARE("INFO",("... Changed service id from %ld to %ld ",lrtc, object_id));
	/******* serviceCHANGEID ****/


	

	/*** serviceMAP **/
	svcmap = malloc(sizeof(struct service)*(shm_hdr->svccount+2));
	rtc=GetServiceMap(svcmap, CONFIG, TEST_ORCH_ID);
	tt_int_op(rtc, >, 0);
	
	lrtc=-1;
	for(x=0; x<=rtc; x++) {
		if(svcmap[x].service_id==object_id) {
			lrtc = 1;
		}
	}
	tt_int_op(lrtc, ==, 1);
	TT_DECLARE("INFO",("... Created ServiceMap and found  %ld ", object_id));
	/*** serviceMAP **/

	/***** doUpdate **/
	modified_object.current_state=27;
	modified_object.service_id=NN;
	rtc=doUpdate(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);
	rtc=GetServiceById(NN, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_int_op(returned_object.current_state, ==, 27);
	
	TT_DECLARE("INFO",("... Called doUpdate() on  %ld ", object_id));

	/***** doUpdate **/

	/**** UpdateServiceInterval ***/
	returned_object.check_interval=90;
	rtc=UpdateServiceInterval(&returned_object, CONFIG);
	tt_int_op(rtc, ==, 1);

	rtc=GetServiceById(NN, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_int_op(returned_object.check_interval, ==, 90);

	TT_DECLARE("INFO",("... UpdateServiceInterval  %ld ", object_id));
	/**** UpdateServiceInterval ***/

	
	

	/*** DELETEservice **/
	rtc=DeleteService(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Delete Service  %ld ", object_id));
	/*** DELETEservice **/
	




	end:
		if(svcmap != NULL) free(svcmap);
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	;


}


void test_service_running(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct service * map;
	int x;
	int res;
	
	int (*AddService)(struct service *, char *);
	int (*DeleteService)(long service_id, char *);
		

	int rtc=-1;
	long lrtc=-1;
	long object_id=-1;

	bartlby_address=NULL;
	SOHandle=NULL;
	SKIP_IF_NOT_RUNNING(CONFIG);

	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);

	
	tt_ptr_op(bartlby_address, !=, NULL);
	tt_ptr_op(SOHandle, !=, NULL);
	
	/// ADD service

	LOAD_SYMBOL_TEST(AddService,SOHandle, "AddService");
	LOAD_SYMBOL_TEST(DeleteService,SOHandle, "DeleteService");


	lrtc=AddService(&dummy_service, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	
	TT_DECLARE("INFO",("... Added Service id: %ld", object_id));

	dummy_service.service_id=object_id;
	
	/****** RELOAD ***/
	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
	shm_hdr->do_reload=1;
	shmdt(bartlby_address);
	sleep(2);
	

	bartlby_address=bartlby_get_shm(CONFIG);
	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);

	TT_DECLARE("INFO", ("Reloaded SHM"));
	tt_int_op(shm_hdr->do_reload, ==, 0);
	/*** RELOAD ***/

	map = bartlby_SHM_ServiceMap(bartlby_address);

	res = -1;
	for(x=0; x<shm_hdr->svccount; x++) {
		
		if(map[x].service_id == object_id) {
			res = 1;
			break;
		}
	}


	rtc=DeleteService(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);

	TT_DECLARE("INFO",("... Delete Service  %ld ", object_id));



	tt_int_op(res, ==, 1);
	TT_DECLARE("INFO",("... SHM Service found  %ld ", object_id));



	end:
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	;


}
	

//TINYTEST SETUP
struct testcase_t service_tests[] = {
	{ "lib", test_service_lib, },
	{ "running", test_service_running, },
	END_OF_TESTCASES
};
