#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"




extern char * dlmsg;

void test_cleanup_db(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;
	int (*CleanupTestData)(char *);

	int rtc=-1;
	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);

	
	tt_ptr_op(bartlby_address, !=, NULL);
	tt_ptr_op(SOHandle, !=, NULL);

	
	/// ADD server

	LOAD_SYMBOL_TEST(CleanupTestData,SOHandle, "CleanupTestData");
	rtc=CleanupTestData(CONFIG);
	tt_int_op(rtc, ==, 1);


	end:
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);


	
}

struct testcase_t cleanup_tests[] = {
	{ "db", test_cleanup_db, },
	END_OF_TESTCASES
};