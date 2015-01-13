#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"



/*
cproto -I../../../include -I/usr/include/curl -I/usr/include/json  service.c
Test Library  Functions

long WorkerChangeId(long from, long to, char *config); -> DONE
int GetWorkerById(long worker_id, struct worker *svc, char *config); - DONE
int UpdateWorker(struct worker *svc, char *config); - DONE
int DeleteWorker(long worker_id, char *config); - DONE
long AddWorker(struct worker *svc, char *config); - DONE
int GetWorkerMap(struct worker *svcs, char *config, int orch_id); - DONE
*/


extern char * dlmsg;
	//DEFINE SAMPLE DATA:
	struct worker dummy_worker = {
		.name="unit-tester",
		.mail="unit@test.com",
		.icq="000000",
		.worker_id=999,
		.active=0,
		.notify_plan=" ",
		.password="unitpassword",
		.escalation_count=0,
		.escalation_time=10000,
		.notify_levels="",
		.enabled_triggers="|mail.sh|",
		.t="",
		.escalation_limit=20,
		.escalation_minutes=10,
		.is_gone=0,
		.visible_servers="",
		.visible_services="",
		.selected_servers="",
		.selected_services="",
		.notification_aggregation_interval=60,
		.is_super_user=1,
		.orch_id=TEST_ORCH_ID,
		.api_pubkey="",
		.api_privkey="",
		.api_enabled=0
	};



/// TEST FUNCTIONS
void test_worker_lib(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;


	struct worker modified_object;
	struct worker returned_object;


	struct worker * wrkmap;
	
	wrkmap=NULL;
	long (*AddWorker)(struct worker *, char *);
	int (*UpdateWorker)(struct worker *, char *);
	int (*DeleteWorker)(long worker_id, char *);
	int (*GetWorkerById)(long, struct worker *, char * );
	long (*WorkerChangeId)(long, long, char*);
	int (*GetWorkerMap)(struct worker*, char*, int);



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

	
	LOAD_SYMBOL_TEST(AddWorker,SOHandle, "AddWorker");
	LOAD_SYMBOL_TEST(DeleteWorker,SOHandle, "DeleteWorker");
	LOAD_SYMBOL_TEST(UpdateWorker,SOHandle, "UpdateWorker");	
	LOAD_SYMBOL_TEST(GetWorkerById,SOHandle, "GetWorkerById");	
	LOAD_SYMBOL_TEST(WorkerChangeId,SOHandle, "WorkerChangeId");	
	LOAD_SYMBOL_TEST(GetWorkerMap,SOHandle, "GetWorkerMap");	


	/******* ADD WORKER ****/
	lrtc=AddWorker(&dummy_worker, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	TT_DECLARE("INFO",("... Added Worker id: %ld", object_id));
	/******* ADD WORKER ****/


	/******* MODIFY WORKER ****/
	dummy_worker.worker_id=object_id;
	memcpy(&modified_object,&dummy_worker, sizeof(struct worker));
	memcpy(&returned_object,&dummy_worker, sizeof(struct worker));
	snprintf(modified_object.name,2048, "modified-unit-test");
	rtc=UpdateWorker(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Modified Worker, changed name"));
	/******* MODIFY WORKER ****/

	/******* GETWORKERBYID ****/
	rtc=GetWorkerById(object_id, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_str_op(returned_object.name, ==, modified_object.name);
	TT_DECLARE("INFO",("... get worker by id ", object_id));
	/******* GETWORKERBYID ****/

	/******* WORKERCHANGEID ****/
	NN=lrtc+999;
	object_id=WorkerChangeId(lrtc, NN, CONFIG);
	tt_int_op(object_id, ==, NN);
	TT_DECLARE("INFO",("... Changed worker id from %ld to %ld ",lrtc, object_id));
	/******* WORKERCHANGEID ****/



	/*** WORKERMAP **/
	wrkmap = malloc(sizeof(struct worker)*(shm_hdr->wrkcount+2));
	rtc=GetWorkerMap(wrkmap, CONFIG, TEST_ORCH_ID);
	tt_int_op(rtc, >, 0);
	lrtc=-1;
	for(x=0; x<rtc; x++) {
		if(wrkmap[x].worker_id==object_id) {
			lrtc = 1;
		}
	}
	tt_int_op(lrtc, ==, 1);
	TT_DECLARE("INFO",("... Created WorkerMap and found  %ld ", object_id));
	/*** WORKERMAP **/

	/*** DELETEWORKER **/
	rtc=DeleteWorker(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Delete Worker  %ld ", object_id));
	/*** DELETEWORKER **/
	




	end:
		if(wrkmap != NULL) free(wrkmap);
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	
	;


}


void test_worker_running(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct worker * map;
	int x;
	int res;
	
	int (*AddWorker)(struct worker *, char *);
	int (*DeleteWorker)(long worker_id, char *);
		

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
	
	/// ADD worker

	LOAD_SYMBOL_TEST(AddWorker,SOHandle, "AddWorker");
	LOAD_SYMBOL_TEST(DeleteWorker,SOHandle, "DeleteWorker");


	lrtc=AddWorker(&dummy_worker, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	
	TT_DECLARE("INFO",("... Added Worker id: %ld", object_id));

	dummy_worker.worker_id=object_id;
	
	//Reload - and detach/reattach to shm
	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
	shm_hdr->do_reload=1;
	shmdt(bartlby_address);
	sleep(2);
	

	bartlby_address=bartlby_get_shm(CONFIG);

	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);

	TT_DECLARE("INFO", ("Reloaded SHM"));
	tt_int_op(shm_hdr->do_reload, ==, 0);

	map = bartlby_SHM_WorkerMap(bartlby_address);

	res = -1;
	for(x=0; x<shm_hdr->wrkcount; x++) {
		
		if(map[x].worker_id == object_id) {
			res = 1;
			break;
		}
	}


	rtc=DeleteWorker(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);

	TT_DECLARE("INFO",("... Delete Worker  ", object_id));



	tt_int_op(res, ==, 1);
	TT_DECLARE("INFO",("... SHM Worker found  ", object_id));



	end:
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	;


}
	

//TINYTEST SETUP
struct testcase_t worker_tests[] = {
	{ "lib", test_worker_lib, },
	{ "running", test_worker_running, },
	END_OF_TESTCASES
};
