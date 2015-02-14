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
void test_notifications(void *data) {
	(void)data;
		void * SOHandle;
		void * bartlby_address;
		struct shm_header *hdr;

		SOHandle = bartlby_get_sohandle(CONFIG);
		bartlby_address=bartlby_get_shm(CONFIG);
		hdr = bartlby_SHM_GetHDR(bartlby_address);

/*

##### SIRENE
		do_check = 0
		type_of_notification = -1

		#NORMAL (first) notification
		do_check = 1
		type_of_notification = 0

		# RE-NOTIFICATION (2nd)
		do_check = 1
		type_of_notification = 2 

		# escalation message (sends notify to standby workers)
		do_check = 1
		type_of_notification =  1

*/	
		int x;
		set_cfg(CONFIG);
		dummy_service.srv = &dummy_server;
		dummy_service.notify_enabled = 1;
		dummy_server.server_notify = 1;
		dummy_service.notify_super_users=1;
		dummy_service.service_id=4;
		bartlby_trigger( NULL, CONFIG, bartlby_address, 0, NOTIFICATION_TYPE_SIRENE, NULL, NULL, "SIRENE");
		bartlby_trigger( &dummy_service, CONFIG, bartlby_address, 1, NOTIFICATION_TYPE_NORMAL,NULL, NULL, NULL);
		bartlby_trigger( &dummy_service, CONFIG, bartlby_address, 1, NOTIFICATION_TYPE_ESCALATION,NULL, NULL, NULL);
		bartlby_trigger( &dummy_service, CONFIG, bartlby_address, 1, NOTIFICATION_TYPE_RENOTIFY,NULL, NULL, NULL);
	

		bartlby_SHM_link_services_servers(bartlby_address, CONFIG, SOHandle);

		bartlby_notification_log_aggregate(bartlby_address, hdr, CONFIG);


		fprintf(stderr, "DUMPING NOTIFY LOG:\n");
		for(x=0; x<NOTIFICATION_LOG_MAX; x++) {
			if(hdr->notification_log[x].notification_valid != -1) {
				fprintf(stderr, "%ld;%ld;%d;%d\n", hdr->notification_log[x].worker_id, hdr->notification_log[x].trigger_id,hdr->notification_log[x].type, hdr->notification_log[x].aggregated);
				//hdr->notification_log[x].notification_valid=-1;
			}
		}
	
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
	{ "trigger", test_notifications,TT_OFF_BY_DEFAULT },
	END_OF_TESTCASES
};