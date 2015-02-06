#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"

extern char * dlmsg;
	//DEFINE SAMPLE DATA:

extern struct service dummy_service;
extern struct server dummy_server;


/// TEST FUNCTIONS
void test_core_sql(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;


	

	
	
	int (*TestSQL)(char*, char *);
	int (*UpdateWorker)(struct worker *, char *);
	int (*DeleteWorker)(long worker_id, char *);
	int (*GetWorkerById)(int, struct worker *, char * );

	int rtc=-1;
	
	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);
	

	
	/// ADD worker

	LOAD_SYMBOL_TEST(TestSQL,SOHandle, "TestSQL");
	
	rtc=TestSQL(CONFIG, "aass''asas");
	if(rtc != 0) {
		exit(1);
	}

	
	
	

	
	

	end:
		dlclose(SOHandle);
		shmdt(bartlby_address);
	;


}
void test_structs(void *data) {
	(void)data;
	/*
	tt_int_op(sizeof(struct service), ==, sizeof(struct service_old));
	tt_int_op(sizeof(struct shm_counter), ==, sizeof(struct shm_counter_old));
	tt_int_op(sizeof(struct perf_statistic), ==, sizeof(struct perf_statistic_old));
	tt_int_op(sizeof(struct snmpi), ==, sizeof(struct snmpi_old));
	tt_int_op(sizeof(struct sprocess), ==, sizeof(struct sprocess_old));
	tt_int_op(sizeof(struct sched_worker), ==, sizeof(struct sched_worker_old));
	tt_int_op(sizeof(struct notification_log_entry), ==, sizeof(struct notification_log_entry_old));
	tt_int_op(sizeof(struct shm_header), ==, sizeof(struct shm_header_old));
	tt_int_op(sizeof(struct server), ==, sizeof(struct server_old));
	tt_int_op(sizeof(struct trap), ==, sizeof(struct trap_old));
	tt_int_op(sizeof(struct servicegroup), ==, sizeof(struct servicegroup_old));
	tt_int_op(sizeof(struct servergroup), ==, sizeof(struct servergroup_old));

	tt_int_op(sizeof(struct worker), ==, sizeof(struct worker_old));
	tt_int_op(sizeof(struct downtime), ==, sizeof(struct downtime_old));
	tt_int_op(sizeof(struct btl_event), ==, sizeof(struct btl_event_old));
	*/

	//dummy_service.srv=&dummy_server;
	//dump_svc(&dummy_service);
	end:
	;
}
void test_shm_so(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;


	

	
	
	
	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);
	
	tt_ptr_op(SOHandle, !=, NULL);
	tt_ptr_op(bartlby_address, !=, NULL);
	
	TT_DECLARE("INFO",("... Lib and SHM work  "));


	end:
		dlclose(SOHandle);
		shmdt(bartlby_address);
	;


}	

//TINYTEST SETUP
struct testcase_t core_tests[] = {
	{ "shm_and_lib", test_shm_so, },
	{ "structs", test_structs, },
	END_OF_TESTCASES
};