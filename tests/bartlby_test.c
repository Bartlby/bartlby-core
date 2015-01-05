#include "bartlby.h"

#include "tinytest.h"
#include "tinytest_macros.h"
#include "bartlby_test.h"

extern const char * dlmsg;






///////

extern struct testcase_t worker_tests[];

struct testgroup_t groups[] = {
	{ "worker/", worker_tests },
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






