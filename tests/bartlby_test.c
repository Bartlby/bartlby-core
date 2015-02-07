#include "bartlby.h"

#include "tinytest.h"
#include "tinytest_macros.h"
#include "bartlby_test.h"

extern const char * dlmsg;






///////

extern struct testcase_t core_tests[];
extern struct testcase_t worker_tests[];
extern struct testcase_t service_tests[];
extern struct testcase_t server_tests[];
extern struct testcase_t downtime_tests[];
extern struct testcase_t trap_tests[];
extern struct testcase_t servicegroup_tests[];
extern struct testcase_t servergroup_tests[];
extern struct testcase_t cleanup_tests[];
extern struct testcase_t lua_tests[];
extern struct testcase_t trigger_tests[];

struct testgroup_t groups[] = {
	{ "core/", core_tests },
	{ "worker/", worker_tests },
	{ "service/", service_tests },
	{ "server/", server_tests},
	{ "downtime/", downtime_tests},
	{ "traps/", trap_tests},
	{ "servicegroups/", servicegroup_tests},
	{ "servergroups/", servergroup_tests},
	{ "lua/", lua_tests},
	{ "trigger/", trigger_tests},
	{ "cleanup/", cleanup_tests},
	END_OF_GROUPS
};



int
main(int c, const char **v)
{
	/* Finally, just call tinytest_main().	It lets you specify verbose
	   or quiet output with --verbose and --quiet.	You can list
	   specific tests:
	       tinytest-demo demo/memcpy
	   or use a ..-wildcard to select multiple tests with a common
	   prefix:
	       tinytest-demo demo/..
	   If you list no tests, you get them all by default, so that
	   "tinytest-demo" and "tinytest-demo .." mean the same thing.
	*/
	
	return tinytest_main(c, v, groups);
}	






