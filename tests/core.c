#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"

extern char * dlmsg;
	//DEFINE SAMPLE DATA:




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
	tt_int_op(rtc, ==, 0);
	
	

	


	end:
		dlclose(SOHandle);
		shmdt(bartlby_address);
	;


}
	

//TINYTEST SETUP
struct testcase_t core_tests[] = {
	{ "testsql", test_core_sql, },
	END_OF_TESTCASES
};