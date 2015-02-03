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
	//tt_int_op(sizeof(struct service), ==, sizeof(struct service_old));
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