#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"



/*
cproto -I../../../include -I/usr/include/curl -I/usr/include/json  server.c
Test Library  Functions

long ServerChangeId(long from, long to, int sr, char *config); - DONE
int GetServerById(long server_id, struct server *svc, char *config); - DONE
int ModifyServer(struct server *svc, char *config); -  DONE
int DeleteServer(long server_id, char *config); -  DONE
long AddServer(struct server *svc, char *config); - DONE
int doUpdateServer(struct server *svc, char *config); -  DONE
int GetServerMap(struct server *srv, char *config, int orch_id); - DONE


*/


extern char * dlmsg;
	//DEFINE SAMPLE DATA:
	struct server dummy_server = {
			.server_id = 1,
			.client_ip = "127.0.0.1",
			.server_name = "unit-tester",
			.server_icon = "linux.gif",
			.server_enabled = 0,
			.client_port = 9030,
			.server_dead = 0,
			.server_notify = 0,
			.server_flap_seconds = 122,
			.flap_count = 0,
			.last_notify_send = 9999,			
			.dead_marker = NULL,
			.is_gone = 0,			
			.servergroups[0]=0,
			.servergroup_counter =0,
			.servergroup_place[0]=0,
			.server_ssh_keyfile="",
			.server_ssh_passphrase="",
			.server_ssh_username="",
			.enabled_triggers = "",
			.default_service_type = SVC_TYPE_ACTIVE,
			.orch_id = TEST_ORCH_ID,
			.exec_plan = "",
			.web_hooks = "",
			.json_endpoint = "",
			.web_hooks_level = 0
	};



/// TEST FUNCTIONS
void test_server_lib(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;


	struct server modified_object;
	struct server returned_object;


	struct server * srvmap;
	
	srvmap=NULL;
	long (*AddServer)(struct server *, char *);
	int (*ModifyServer)(struct server *, char *);
	int (*DeleteServer)(long server_id, char *);
	int (*GetServerById)(long, struct server *, char * );
	long (*ServerChangeId)(long, long, int, char*);
	int (*GetServerMap)(struct server*, char*, int);
	int (*doUpdateServer)(struct server*, char *);


	int rtc=-1;
	long object_id=-1;
	long lrtc=-1;
	long NN=-1;
	int x;
	struct shm_header * shm_hdr;

	
	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);
	
	shm_hdr = bartlby_SHM_GetHDR(bartlby_address);

	
	LOAD_SYMBOL_TEST(AddServer,SOHandle, "AddServer");
	LOAD_SYMBOL_TEST(DeleteServer,SOHandle, "DeleteServer");
	LOAD_SYMBOL_TEST(ModifyServer,SOHandle, "ModifyServer");	
	LOAD_SYMBOL_TEST(GetServerById,SOHandle, "GetServerById");	
	LOAD_SYMBOL_TEST(ServerChangeId,SOHandle, "ServerChangeId");	
	LOAD_SYMBOL_TEST(GetServerMap,SOHandle, "GetServerMap");	
	LOAD_SYMBOL_TEST(doUpdateServer, SOHandle, "doUpdateServer");


	/******* ADD SERVER ****/
	lrtc=AddServer(&dummy_server, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	TT_DECLARE("INFO",("... Added Server id: %ld", object_id));
	/******* ADD SERVER ****/


	/******* MODIFY SERVER ****/
	dummy_server.server_id=object_id;
	memcpy(&modified_object,&dummy_server, sizeof(struct server));
	memcpy(&returned_object,&dummy_server, sizeof(struct server));
	snprintf(modified_object.server_name,2048, "modified-unit-test");
	rtc=ModifyServer(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Modified Server, changed name"));
	/******* MODIFY SERVER ****/

	/******* GETSERVERBYID ****/
	rtc=GetServerById(object_id, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_str_op(returned_object.server_name, ==, modified_object.server_name);
	TT_DECLARE("INFO",("... get server by id  %ld", object_id));
	/******* GETSERVERBYID ****/

	/******* SERVERCHANGEID ****/
	NN=lrtc+999;
	object_id=ServerChangeId(lrtc, NN, 1,CONFIG);
	tt_int_op(object_id, ==, NN);
	TT_DECLARE("INFO",("... Changed server id from %ld to %ld ",lrtc, object_id));
	/******* SERVERCHANGEID ****/

	

	/*** SERVERMAP **/
	srvmap = malloc(sizeof(struct server)*(shm_hdr->srvcount+2));
	rtc=GetServerMap(srvmap, CONFIG, TEST_ORCH_ID);
	tt_int_op(rtc, >, 0);
	lrtc=-1;
	for(x=0; x<rtc; x++) {
		if(srvmap[x].server_id==object_id) {
			lrtc = 1;
		}
	}
	tt_int_op(lrtc, ==, 1);
	TT_DECLARE("INFO",("... Created ServerMap and found  %ld ", object_id));
	/*** SERVERMAP **/


	/***** doUpdate **/
	modified_object.server_enabled=27;
	modified_object.server_id=NN;
	rtc=doUpdateServer(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);
	rtc=GetServerById(NN, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_int_op(returned_object.server_enabled, ==, 27);
	
	TT_DECLARE("INFO",("... Called doUpdateServer() on  %ld ", object_id));

	/***** doUpdate **/


	/*** DELETESERVER **/
	rtc=DeleteServer(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Delete Server  %ld ", object_id));
	/*** DELETESERVER **/
	




	end:
		if(srvmap != NULL) free(srvmap);
		dlclose(SOHandle);
		shmdt(bartlby_address);
	;


}


void test_server_running(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct server * map;
	int x;
	int res;
	
	int (*AddServer)(struct server *, char *);
	int (*DeleteServer)(long server_id, char *);
		

	int rtc=-1;
	long lrtc=-1;
	long object_id=-1;

	SOHandle = bartlby_get_sohandle(CONFIG);
	bartlby_address=bartlby_get_shm(CONFIG);
	

	
	/// ADD server

	LOAD_SYMBOL_TEST(AddServer,SOHandle, "AddServer");
	LOAD_SYMBOL_TEST(DeleteServer,SOHandle, "DeleteServer");


	lrtc=AddServer(&dummy_server, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	
	TT_DECLARE("INFO",("... Added Server id: %ld", object_id));

	dummy_server.server_id=object_id;
	
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

	map = bartlby_SHM_ServerMap(bartlby_address);

	res = -1;
	for(x=0; x<shm_hdr->srvcount; x++) {
		
		if(map[x].server_id == object_id) {
			res = 1;
			break;
		}
	}


	rtc=DeleteServer(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);

	TT_DECLARE("INFO",("... Delete Server  ", object_id));



	tt_int_op(res, ==, 1);
	TT_DECLARE("INFO",("... SHM Server found  ", object_id));



	end:
		dlclose(SOHandle);
		shmdt(bartlby_address);
	;


}
	

//TINYTEST SETUP
struct testcase_t server_tests[] = {
	{ "lib", test_server_lib, },
	{ "running", test_server_running, },
	END_OF_TESTCASES
};
