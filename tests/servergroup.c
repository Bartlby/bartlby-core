#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"



/*
cproto -I../../../include -I/usr/include/curl -I/usr/include/json  servergroups.c
Test Library  Functions

long ServerGroupChangeId(long from, long to, char *config); - DONE
int GetServergroupById(long servergroup_id, struct servergroup *svc, char *config); - DONE
int GetServerGroupMap(struct servergroup *svcs, char *config, int orch_id); - DONE
long AddServerGroup(struct servergroup *svc, char *config); - DONE
int DeleteServerGroup(long servergroup_id, char *config); - DONE
int UpdateServerGroup(struct servergroup *svc, char *config); - DONE
*/


extern char * dlmsg;
	//DEFINE SAMPLE DATA:
	struct servergroup dummy_servergroup = {

	.servergroup_id = 1,
	.servergroup_name="unit-test",
	.servergroup_notify=0,
	.servergroup_active=0,
	.servergroup_members[0]=0,
	.servergroup_dead =0,
	.dead_marker=NULL,	
	.enabled_triggers = "",
	.orch_id = TEST_ORCH_ID
	};



/// TEST FUNCTIONS
void test_servergroup_lib(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;


	struct servergroup modified_object;
	struct servergroup returned_object;
	long rtcn;

	struct servergroup * srvmap;
	
	srvmap=NULL;
	long (*AddServerGroup)(struct servergroup *, char *);
	int (*UpdateServerGroup)(struct servergroup *, char *);
	int (*DeleteServerGroup)(long servergroup_id, char *);
	int (*GetServergroupById)(long, struct servergroup *, char * );
	long (*ServerGroupChangeId)(long, long, char*);
	long (*GetServerGroupMap)(struct servergroup*, char*, int);
	


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

	
	LOAD_SYMBOL_TEST(AddServerGroup,SOHandle, "AddServerGroup");
	LOAD_SYMBOL_TEST(DeleteServerGroup,SOHandle, "DeleteServerGroup");
	LOAD_SYMBOL_TEST(UpdateServerGroup,SOHandle, "UpdateServerGroup");	
	LOAD_SYMBOL_TEST(GetServergroupById,SOHandle, "GetServergroupById");	
	LOAD_SYMBOL_TEST(ServerGroupChangeId,SOHandle, "ServerGroupChangeId");	
	LOAD_SYMBOL_TEST(GetServerGroupMap,SOHandle, "GetServerGroupMap");	
	
	


	/******* ADD SERVERGROUP ****/
	lrtc=AddServerGroup(&dummy_servergroup, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	TT_DECLARE("INFO",("... Added ServerGroup id: %ld", object_id));
	/******* ADD SERVERGROUP ****/


	/******* MODIFY SERVERGROUP ****/
	dummy_servergroup.servergroup_id=object_id;
	memcpy(&modified_object,&dummy_servergroup, sizeof(struct servergroup));
	memcpy(&returned_object,&dummy_servergroup, sizeof(struct servergroup));
	snprintf(modified_object.servergroup_name,1000, "modified-unit-test");
	rtc=UpdateServerGroup(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Modified ServerGroup, changed name"));
	/******* MODIFY SERVERGROUP ****/

	/******* GETSERVERGROUPBYID ****/
	rtc=GetServergroupById(object_id, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_str_op(returned_object.servergroup_name, ==, modified_object.servergroup_name);
	TT_DECLARE("INFO",("... get servergroup by id  %ld", object_id));
	/******* GETSERVERGROUPBYID ****/

	/******* SERVERGROUPCHANGEID ****/
	NN=lrtc+999;
	object_id=ServerGroupChangeId(lrtc, NN, CONFIG);
	tt_int_op(object_id, ==, NN);
	TT_DECLARE("INFO",("... Changed servergroup id from %ld to %ld ",lrtc, object_id));
	/******* SERVERGROUPCHANGEID ****/







	/*** SERVERGROUPMAP **/
	srvmap = malloc(sizeof(struct servergroup)*(shm_hdr->srvgroupcount+2));
	rtcn=GetServerGroupMap(srvmap, CONFIG, TEST_ORCH_ID);
	tt_int_op(rtcn, >, 0);
	lrtc=-1;
	for(x=0; x<rtcn; x++) {
		if(srvmap[x].servergroup_id==object_id) {
			lrtc = 1;
		}
	}
	tt_int_op(lrtc, ==, 1);
	TT_DECLARE("INFO",("... Created ServerGroupMap and found  %ld ", object_id));
	/*** SERVERGROUPMAP **/






	/*** DELETESERVERGROUP **/
	rtc=DeleteServerGroup(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Delete ServerGroup  %ld ", object_id));
	/*** DELETESERVERGROUP **/
	




	end:
		if(srvmap != NULL) free(srvmap);
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	
	;


}


void test_servergroup_running(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct servergroup * map;
	int x;
	int res;
	
	int (*AddServerGroup)(struct servergroup *, char *);
	int (*DeleteServerGroup)(long servergroup_id, char *);
		

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

	
	/// ADD servergroup

	LOAD_SYMBOL_TEST(AddServerGroup,SOHandle, "AddServerGroup");
	LOAD_SYMBOL_TEST(DeleteServerGroup,SOHandle, "DeleteServerGroup");


	lrtc=AddServerGroup(&dummy_servergroup, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	
	TT_DECLARE("INFO",("... Added ServerGroup id: %ld", object_id));

	dummy_servergroup.servergroup_id=object_id;
	
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

	map = bartlby_SHM_ServerGroupMap(bartlby_address);

	res = -1;
	for(x=0; x<shm_hdr->srvgroupcount; x++) {
		
		if(map[x].servergroup_id == object_id) {
			res = 1;
			break;
		}
	}


	rtc=DeleteServerGroup(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);

	TT_DECLARE("INFO",("... Delete ServerGroup  ", object_id));



	tt_int_op(res, ==, 1);
	TT_DECLARE("INFO",("... SHM ServerGroup found  ", object_id));



	end:
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	;


}
	

//TINYTEST SETUP
struct testcase_t servergroup_tests[] = {
	{ "lib", test_servergroup_lib, },
	{ "running", test_servergroup_running, },
	END_OF_TESTCASES
};
