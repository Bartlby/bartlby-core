#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"



/*
cproto -I../../../include -I/usr/include/curl -I/usr/include/json  triggers.c
Test Library  Functions

long AddTrigger(struct trigger *svc, char *config); - DONE
int DeleteTrigger(long trigger_id, char *config); - DONE
int UpdateTrigger(struct trigger *svc, char *config); - DONE 
long TriggerChangeId(long from, long to, char *config); - DONE
int GetTriggerById(long trigger_id, struct trigger *svc, char *config); - DONE
int GetTriggerMap(struct trigger *svcs, char *config, int orch_id); - DONE 



*/


extern char * dlmsg;
	//DEFINE SAMPLE DATA:
	struct trigger dummy_trigger = {
	.trigger_id=1,
	.trigger_count=233,
	.trigger_name = "MAIL",
	.trigger_enabled = 1,
	.trigger_type = 1,
	.trigger_data = "mail.sh",
	.trigger_execplan = "",
	.trigger_full_path = "/opt/bartlby/trigger/mail.sh",
	.is_gone = 0,
	.orch_id = 999				
	};



/// TEST FUNCTIONS
void test_trigger_lib(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;


	struct trigger modified_object;
	struct trigger returned_object;

	long rtcn=-1;
	struct trigger * srvmap;
	
	srvmap=NULL;
	long (*AddTrigger)(struct trigger *, char *);
	int (*UpdateTrigger)(struct trigger *, char *);
	int (*DeleteTrigger)(long trigger_id, char *);
	int (*GetTriggerById)(long, struct trigger *, char * );
	long (*TriggerChangeId)(long, long, char*);
	long (*GetTriggerMap)(struct trigger*, char*, int);
	


	int rtc=-1;
	long object_id=-1;
	long lrtc=-1;
	long NN=-1;
	int x;
	struct shm_header * shm_hdr;

	
	bartlby_address=NULL;
	SOHandle=NULL;
	SKIP_IF_NOT_RUNNING(CONFIG);

	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);

	
	tt_ptr_op(bartlby_address, !=, NULL);
	tt_ptr_op(SOHandle, !=, NULL);
	
	shm_hdr = bartlby_SHM_GetHDR(bartlby_address);

	
	LOAD_SYMBOL_TEST(AddTrigger,SOHandle, "AddTrigger");
	LOAD_SYMBOL_TEST(DeleteTrigger,SOHandle, "DeleteTrigger");
	LOAD_SYMBOL_TEST(UpdateTrigger,SOHandle, "UpdateTrigger");	
	LOAD_SYMBOL_TEST(GetTriggerById,SOHandle, "GetTriggerById");	
	LOAD_SYMBOL_TEST(TriggerChangeId,SOHandle, "TriggerChangeId");	
	LOAD_SYMBOL_TEST(GetTriggerMap,SOHandle, "GetTriggerMap");	
	
	


	/******* ADD TRIGGER ****/
	lrtc=AddTrigger(&dummy_trigger, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	TT_DECLARE("INFO",("... Added Trigger id: %ld", object_id));
	/******* ADD TRIGGER ****/


	/******* MODIFY TRIGGER ****/
	dummy_trigger.trigger_id=object_id;
	memcpy(&modified_object,&dummy_trigger, sizeof(struct trigger));
	memcpy(&returned_object,&dummy_trigger, sizeof(struct trigger));
	snprintf(modified_object.trigger_data,1024, "modified-unit-test");
	rtc=UpdateTrigger(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Modified Trigger, changed name"));
	/******* MODIFY TRIGGER ****/

	/******* GETTRIGGERBYID ****/
	rtc=GetTriggerById(object_id, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_str_op(returned_object.trigger_data, ==, modified_object.trigger_data);
	TT_DECLARE("INFO",("... get trigger by id  %ld", object_id));
	/******* GETTRIGGERBYID ****/

	/******* TRIGGERCHANGEID ****/
	NN=lrtc+999;
	object_id=TriggerChangeId(lrtc, NN, CONFIG);
	tt_int_op(object_id, ==, NN);
	TT_DECLARE("INFO",("... Changed trigger id from %ld to %ld ",lrtc, object_id));
	/******* TRIGGERCHANGEID ****/


	


	/*** TRIGGERMAP **/
	srvmap = malloc(sizeof(struct trigger)*(shm_hdr->triggercount+2));
	rtcn=GetTriggerMap(srvmap, CONFIG, TEST_ORCH_ID);
	tt_int_op(rtcn, >, 0);
	lrtc=-1;
	for(x=0; x<rtcn; x++) {
		if(srvmap[x].trigger_id==object_id) {
			lrtc = 1;
		}
	}
	tt_int_op(lrtc, ==, 1);
	TT_DECLARE("INFO",("... Created TriggerMap and found  %ld ", object_id));
	/*** TRIGGERMAP **/






	/*** DELETETRIGGER **/
	rtc=DeleteTrigger(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Delete Trigger  %ld ", object_id));
	/*** DELETETRIGGER **/
	




	end:
		if(srvmap != NULL) free(srvmap);
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	
	;


}


void test_trigger_running(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct trigger * map;
	int x;
	int res;
	
	int (*AddTrigger)(struct trigger *, char *);
	int (*DeleteTrigger)(long trigger_id, char *);
		

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
	
	
	/// ADD trigger

	LOAD_SYMBOL_TEST(AddTrigger,SOHandle, "AddTrigger");
	LOAD_SYMBOL_TEST(DeleteTrigger,SOHandle, "DeleteTrigger");


	lrtc=AddTrigger(&dummy_trigger, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	
	TT_DECLARE("INFO",("... Added Trigger id: %ld", object_id));

	dummy_trigger.trigger_id=object_id;
	
	//Reload - and detach/reattach to shm
	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
	shm_hdr->do_reload=1;
	shmdt(bartlby_address);
	sleep(2);
	

	bartlby_address=bartlby_get_shm(CONFIG);

	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);

	TT_DECLARE("INFO", ("Reloaded SHM"));
	tt_int_op(shm_hdr->do_reload, ==, 0);

	map = bartlby_SHM_TriggerMap(bartlby_address);

	res = -1;
	for(x=0; x<shm_hdr->triggercount; x++) {
		
		if(map[x].trigger_id == object_id) {
			res = 1;
			break;
		}
	}


	rtc=DeleteTrigger(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);

	TT_DECLARE("INFO",("... Delete Trigger  ", object_id));



	tt_int_op(res, ==, 1);
	TT_DECLARE("INFO",("... SHM Trigger found  ", object_id));



	end:
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	;


}
	

//TINYTEST SETUP
struct testcase_t trigger_tests[] = {
	{ "lib", test_trigger_lib, },
	{ "running", test_trigger_running, },
	END_OF_TESTCASES
};
