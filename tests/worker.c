#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"

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
		.orch_id=999,
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


	
	
	int (*AddWorker)(struct worker *, char *);
	int (*UpdateWorker)(struct worker *, char *);
	int (*DeleteWorker)(long worker_id, char *);
	int (*GetWorkerById)(int, struct worker *, char * );

	int rtc=-1;
	int object_id=-1;

	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);
	

	
	/// ADD worker

	LOAD_SYMBOL_TEST(AddWorker,SOHandle, "AddWorker");
	LOAD_SYMBOL_TEST(DeleteWorker,SOHandle, "DeleteWorker");
	LOAD_SYMBOL_TEST(UpdateWorker,SOHandle, "UpdateWorker");	
	LOAD_SYMBOL_TEST(GetWorkerById,SOHandle, "GetWorkerById");	

	rtc=AddWorker(&dummy_worker, CONFIG);
	object_id=rtc;
	tt_int_op(rtc, >, 0);
	
	TT_DECLARE("INFO",("... Added Worker id: %ld", object_id));

	dummy_worker.worker_id=object_id;
	memcpy(&modified_object,&dummy_worker, sizeof(struct worker));
	memcpy(&returned_object,&dummy_worker, sizeof(struct worker));
	

	

	snprintf(modified_object.name,2048, "modified-unit-test");
	rtc=UpdateWorker(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);

	TT_DECLARE("INFO",("... Modified Worker, changed name"));

	rtc=GetWorkerById(object_id, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_str_op(returned_object.name, ==, modified_object.name);

	TT_DECLARE("INFO",("... get worker by id ", object_id));


	rtc=DeleteWorker(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);

	TT_DECLARE("INFO",("... Delete Worker  ", object_id));
	//TT_BLATHER(("Successfully added/modified/deleted a worker object"));
	
	




	end:
		dlclose(SOHandle);
		shmdt(bartlby_address);
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
	int object_id=-1;

	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);
	

	
	/// ADD worker

	LOAD_SYMBOL_TEST(AddWorker,SOHandle, "AddWorker");
	LOAD_SYMBOL_TEST(DeleteWorker,SOHandle, "DeleteWorker");


	rtc=AddWorker(&dummy_worker, CONFIG);
	object_id=rtc;
	tt_int_op(rtc, >, 0);
	
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
		dlclose(SOHandle);
		shmdt(bartlby_address);
	;


}
	

//TINYTEST SETUP
struct testcase_t worker_tests[] = {
	{ "lib", test_worker_lib, },
	{ "running", test_worker_running, },
	END_OF_TESTCASES
};