#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"

extern char * dlmsg;
	//DEFINE SAMPLE DATA:


extern struct service dummy_service;
extern struct server dummy_server;


#define TEST_LUA_SCRIPT " \
	function bartlby_service_finish_hook(svc_obj, svc_table)  \n \
		r = bartlby_service_set_status(svc_obj, 2) \n \
		r = bartlby_service_set_output(svc_obj, 'new output') \n \
		return 17 \n \
	end \n \
	return 20 \n"

void test_lua_finish_hook(void *data) {
	(void)data;

	int rtc;
	
	dummy_service.srv=&dummy_server;
	dummy_service.current_state=1;
	rtc=bartlby_finish_script(&dummy_service, TEST_LUA_SCRIPT);
	TT_DECLARE("INFO",(".. LUA SCRIPT RETURN %d ", rtc));

	tt_int_op(rtc, ==, 17);
	tt_str_op(dummy_service.current_output, ==, "new output");
	tt_int_op(dummy_service.current_state, ==, 2);
	

	end:
		
	;


}	

//TINYTEST SETUP
struct testcase_t lua_tests[] = {
	{ "service_finish_hook", test_lua_finish_hook, },
	END_OF_TESTCASES
};