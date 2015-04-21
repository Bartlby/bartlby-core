#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"



/*
cproto -I../../../include -I/usr/include/curl -I/usr/include/json  servicegroups.c
Test Library  Functions

long ServiceGroupChangeId(long from, long to, char *config); - DONE
int GetsServicegroupById(long servicegroup_id, struct servicegroup *svc, char *config); - DONE
int GetServiceGroupMap(struct servicegroup *svcs, char *config, int orch_id); - DONE
long AddServiceGroup(struct servicegroup *svc, char *config); - DONE
int DeleteServiceGroup(long servicegroup_id, char *config); - DONE
int UpdateServiceGroup(struct servicegroup *svc, char *config); - DONE

*/


extern char * dlmsg;
	//DEFINE SAMPLE DATA:
	struct servicegroup dummy_servicegroup = {

	.servicegroup_id = 1,
	.servicegroup_name="unit-test",
	.servicegroup_notify=0,
	.servicegroup_active=0,
	.servicegroup_members[0]=0,
	.servicegroup_dead =0,
	.dead_marker=NULL,	
	.enabled_triggers = "",
	.orch_id = TEST_ORCH_ID
	};



/// TEST FUNCTIONS
void test_servicegroup_lib(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;


	struct servicegroup modified_object;
	struct servicegroup returned_object;


	struct servicegroup * srvmap;
	
	srvmap=NULL;
	long (*AddServiceGroup)(struct servicegroup *, char *);
	int (*UpdateServiceGroup)(struct servicegroup *, char *);
	int (*DeleteServiceGroup)(long servicegroup_id, char *);
	int (*GetsServicegroupById)(long, struct servicegroup *, char * );
	long (*ServiceGroupChangeId)(long, long, char*);
	long (*GetServiceGroupMap)(struct servicegroup*, char*, int);
	


	int rtc=-1;
	long object_id=-1;
	long lrtc=-1;
	long NN=-1;
	long rtcn=-1;
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

	
	LOAD_SYMBOL_TEST(AddServiceGroup,SOHandle, "AddServiceGroup");
	LOAD_SYMBOL_TEST(DeleteServiceGroup,SOHandle, "DeleteServiceGroup");
	LOAD_SYMBOL_TEST(UpdateServiceGroup,SOHandle, "UpdateServiceGroup");	
	LOAD_SYMBOL_TEST(GetsServicegroupById,SOHandle, "GetsServicegroupById");	
	LOAD_SYMBOL_TEST(ServiceGroupChangeId,SOHandle, "ServiceGroupChangeId");	
	LOAD_SYMBOL_TEST(GetServiceGroupMap,SOHandle, "GetServiceGroupMap");	
	
	


	/******* ADD SERVICEGROUP ****/
	lrtc=AddServiceGroup(&dummy_servicegroup, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	TT_DECLARE("INFO",("... Added ServiceGroup id: %ld", object_id));
	/******* ADD SERVICEGROUP ****/


	/******* MODIFY SERVICEGROUP ****/
	dummy_servicegroup.servicegroup_id=object_id;
	memcpy(&modified_object,&dummy_servicegroup, sizeof(struct servicegroup));
	memcpy(&returned_object,&dummy_servicegroup, sizeof(struct servicegroup));
	snprintf(modified_object.servicegroup_name,1000, "modified-unit-test");
	rtc=UpdateServiceGroup(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Modified ServiceGroup, changed name"));
	/******* MODIFY SERVICEGROUP ****/

	/******* GETSERVICEGROUPBYID ****/
	rtc=GetsServicegroupById(object_id, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_str_op(returned_object.servicegroup_name, ==, modified_object.servicegroup_name);
	TT_DECLARE("INFO",("... get servicegroup by id  %ld", object_id));
	/******* GETSERVICEGROUPBYID ****/

	/******* SERVICEGROUPCHANGEID ****/
	NN=lrtc+999;
	object_id=ServiceGroupChangeId(lrtc, NN, CONFIG);
	tt_int_op(object_id, ==, NN);
	TT_DECLARE("INFO",("... Changed servicegroup id from %ld to %ld ",lrtc, object_id));
	/******* SERVICEGROUPCHANGEID ****/



	/*** SERVICEGROUPMAP **/
	srvmap = malloc(sizeof(struct servicegroup)*(shm_hdr->svcgroupcount+2));
	rtcn=GetServiceGroupMap(srvmap, CONFIG, TEST_ORCH_ID);
	tt_int_op(rtcn, >, 0);
	lrtc=-1;
	for(x=0; x<rtcn; x++) {
		if(srvmap[x].servicegroup_id==object_id) {
			lrtc = 1;
		}
	}
	tt_int_op(lrtc, ==, 1);
	TT_DECLARE("INFO",("... Created ServiceGroupMap and found  %ld ", object_id));
	/*** SERVICEGROUPMAP **/






	/*** DELETESERVICEGROUP **/
	rtc=DeleteServiceGroup(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Delete ServiceGroup  %ld ", object_id));
	/*** DELETESERVICEGROUP **/
	




	end:
		if(srvmap != NULL) free(srvmap);
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	;


}


void test_servicegroup_running(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct servicegroup * map;
	int x;
	int res;
	
	int (*AddServiceGroup)(struct servicegroup *, char *);
	int (*DeleteServiceGroup)(long servicegroup_id, char *);
		

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

	
	/// ADD servicegroup

	LOAD_SYMBOL_TEST(AddServiceGroup,SOHandle, "AddServiceGroup");
	LOAD_SYMBOL_TEST(DeleteServiceGroup,SOHandle, "DeleteServiceGroup");


	lrtc=AddServiceGroup(&dummy_servicegroup, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	
	TT_DECLARE("INFO",("... Added ServiceGroup id: %ld", object_id));

	dummy_servicegroup.servicegroup_id=object_id;
	
	//Reload - and detach/reattach to shm
	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
	shm_hdr->do_reload=1;
	shmdt(bartlby_address);
	sleep(2);
	

	bartlby_address=bartlby_get_shm(CONFIG);

	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);

	TT_DECLARE("INFO", ("Reloaded SHM"));
	tt_int_op(shm_hdr->do_reload, ==, 0);

	map = bartlby_SHM_ServiceGroupMap(bartlby_address);

	res = -1;
	for(x=0; x<shm_hdr->svcgroupcount; x++) {
		
		if(map[x].servicegroup_id == object_id) {
			res = 1;
			break;
		}
	}


	rtc=DeleteServiceGroup(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);

	TT_DECLARE("INFO",("... Delete ServiceGroup  ", object_id));



	tt_int_op(res, ==, 1);
	TT_DECLARE("INFO",("... SHM ServiceGroup found  ", object_id));



	end:
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	;


}
	

//TINYTEST SETUP
struct testcase_t servicegroup_tests[] = {
	{ "lib", test_servicegroup_lib, },
	{ "running", test_servicegroup_running, },
	END_OF_TESTCASES
};
