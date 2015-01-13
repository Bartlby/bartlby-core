#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"



/*
cproto -I../../../include -I/usr/include/curl -I/usr/include/json  downtime.c
Test Library  Functions

int UpdateDowntime(struct downtime *svc, char *config); - DONE
int DeleteDowntime(long downtime_id, char *config); - DONE
long DowntimeChangeId(long from, long to, char *config); - DONE
long AddDowntime(struct downtime *svc, char *config); - DONE
int GetDowntimeById(long downtime_id, struct downtime *svcs, char *config); - DONE
int GetDowntimeMap(struct downtime *svcs, char *config, int orch_id); - DONE

*/


extern char * dlmsg;
	//DEFINE SAMPLE DATA:
	struct downtime dummy_downtime = {
				.downtime_id=1,
				.downtime_type = 1,
				.downtime_from=0,
				.downtime_to=0,
				.downtime_notice="notice",
				.service_id=-1,				
				.is_gone=0,
				.orch_id=TEST_ORCH_ID
	};



/// TEST FUNCTIONS
void test_downtime_lib(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;


	struct downtime modified_object;
	struct downtime returned_object;


	struct downtime * srvmap;
	
	srvmap=NULL;
	long (*AddDowntime)(struct downtime *, char *);
	int (*UpdateDowntime)(struct downtime *, char *);
	int (*DeleteDowntime)(long downtime_id, char *);
	int (*GetDowntimeById)(long, struct downtime *, char * );
	long (*DowntimeChangeId)(long, long, char*);
	int (*GetDowntimeMap)(struct downtime*, char*, int);
	


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

	
	LOAD_SYMBOL_TEST(AddDowntime,SOHandle, "AddDowntime");
	LOAD_SYMBOL_TEST(DeleteDowntime,SOHandle, "DeleteDowntime");
	LOAD_SYMBOL_TEST(UpdateDowntime,SOHandle, "UpdateDowntime");	
	LOAD_SYMBOL_TEST(GetDowntimeById,SOHandle, "GetDowntimeById");	
	LOAD_SYMBOL_TEST(DowntimeChangeId,SOHandle, "DowntimeChangeId");	
	LOAD_SYMBOL_TEST(GetDowntimeMap,SOHandle, "GetDowntimeMap");	
	


	/******* ADD DOWNTIME ****/
	lrtc=AddDowntime(&dummy_downtime, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	TT_DECLARE("INFO",("... Added Downtime id: %ld", object_id));
	/******* ADD DOWNTIME ****/


	/******* MODIFY DOWNTIME ****/
	dummy_downtime.downtime_id=object_id;
	memcpy(&modified_object,&dummy_downtime, sizeof(struct downtime));
	memcpy(&returned_object,&dummy_downtime, sizeof(struct downtime));
	snprintf(modified_object.downtime_notice,2048, "modified-unit-test");
	rtc=UpdateDowntime(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Modified Downtime, changed name"));
	/******* MODIFY DOWNTIME ****/

	/******* GETDOWNTIMEBYID ****/
	rtc=GetDowntimeById(object_id, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_str_op(returned_object.downtime_notice, ==, modified_object.downtime_notice);
	TT_DECLARE("INFO",("... get downtime by id  %ld", object_id));
	/******* GETDOWNTIMEBYID ****/

	/******* DOWNTIMECHANGEID ****/
	NN=lrtc+999;
	object_id=DowntimeChangeId(lrtc, NN, CONFIG);
	tt_int_op(object_id, ==, NN);
	TT_DECLARE("INFO",("... Changed downtime id from %ld to %ld ",lrtc, object_id));
	/******* DOWNTIMECHANGEID ****/





	/*** DOWNTIMEMAP **/
	srvmap = malloc(sizeof(struct downtime)*(shm_hdr->dtcount+2));
	rtc=GetDowntimeMap(srvmap, CONFIG, TEST_ORCH_ID);
	tt_int_op(rtc, >, 0);
	lrtc=-1;
	for(x=0; x<rtc; x++) {
		if(srvmap[x].downtime_id==object_id) {
			lrtc = 1;
		}
	}
	tt_int_op(lrtc, ==, 1);
	TT_DECLARE("INFO",("... Created DowntimeMap and found  %ld ", object_id));
	/*** DOWNTIMEMAP **/


	

	/*** DELETEDOWNTIME **/
	rtc=DeleteDowntime(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Delete Downtime  %ld ", object_id));
	/*** DELETEDOWNTIME **/
	




	end:
		if(srvmap != NULL) free(srvmap);
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	;


}


void test_downtime_running(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct downtime * map;
	int x;
	int res;
	
	int (*AddDowntime)(struct downtime *, char *);
	int (*DeleteDowntime)(long downtime_id, char *);
		

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

	
	/// ADD downtime

	LOAD_SYMBOL_TEST(AddDowntime,SOHandle, "AddDowntime");
	LOAD_SYMBOL_TEST(DeleteDowntime,SOHandle, "DeleteDowntime");


	lrtc=AddDowntime(&dummy_downtime, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	
	TT_DECLARE("INFO",("... Added Downtime id: %ld", object_id));

	dummy_downtime.downtime_id=object_id;
	
	/****** RELOAD ***/
	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
	shm_hdr->do_reload=1;
	shmdt(bartlby_address);
	sleep(2);
	

	bartlby_address=bartlby_get_shm(CONFIG);
	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);

	TT_DECLARE("INFO", ("Reloaded SHM"));
	tt_int_op(shm_hdr->do_reload, ==, 0);
	/*** RELOAD ***/

	map = bartlby_SHM_DowntimeMap(bartlby_address);

	res = -1;
	for(x=0; x<shm_hdr->dtcount; x++) {
		
		if(map[x].downtime_id == object_id) {
			res = 1;
			break;
		}
	}


	rtc=DeleteDowntime(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);

	TT_DECLARE("INFO",("... Delete Downtime  ", object_id));



	tt_int_op(res, ==, 1);
	TT_DECLARE("INFO",("... SHM Downtime found  ", object_id));



	end:
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	;


}
	

//TINYTEST SETUP
struct testcase_t downtime_tests[] = {
	{ "lib", test_downtime_lib, },
	{ "running", test_downtime_running, },
	END_OF_TESTCASES
};
