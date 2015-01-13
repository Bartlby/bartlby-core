#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"



/*
cproto -I../../../include -I/usr/include/curl -I/usr/include/json  traps.c
Test Library  Functions

long AddTrap(struct trap *svc, char *config); - DONE
int DeleteTrap(long trap_id, char *config); - DONE
int UpdateTrap(struct trap *svc, char *config); - DONE 
long TrapChangeId(long from, long to, char *config); - DONE
int GetTrapById(long trap_id, struct trap *svc, char *config); - DONE
int GetTrapMap(struct trap *svcs, char *config, int orch_id); - DONE 



*/


extern char * dlmsg;
	//DEFINE SAMPLE DATA:
	struct trap dummy_trap = {
	.trap_id = 1,
    .trap_name  = ".*",
    .trap_catcher = ".*",
    .trap_status_text = ".*", 
    .trap_status_ok = ".*",
    .trap_status_warning = ".*",
    .trap_status_critical = ".*",
    .trap_service_id = -1,
    .service_shm_place=-1,
    .trap_fixed_status = -2,
    .trap_prio = 100,
    .trap_is_final = 1,
    .orch_id=TEST_ORCH_ID,
    .is_gone=0,
    .matched=0,
    .trap_last_match = 99999,
    .trap_last_data = "EMPTY"
				
	};



/// TEST FUNCTIONS
void test_trap_lib(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;


	struct trap modified_object;
	struct trap returned_object;


	struct trap * srvmap;
	
	srvmap=NULL;
	long (*AddTrap)(struct trap *, char *);
	int (*UpdateTrap)(struct trap *, char *);
	int (*DeleteTrap)(long trap_id, char *);
	int (*GetTrapById)(long, struct trap *, char * );
	long (*TrapChangeId)(long, long, char*);
	int (*GetTrapMap)(struct trap*, char*, int);
	


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

	
	LOAD_SYMBOL_TEST(AddTrap,SOHandle, "AddTrap");
	LOAD_SYMBOL_TEST(DeleteTrap,SOHandle, "DeleteTrap");
	LOAD_SYMBOL_TEST(UpdateTrap,SOHandle, "UpdateTrap");	
	LOAD_SYMBOL_TEST(GetTrapById,SOHandle, "GetTrapById");	
	LOAD_SYMBOL_TEST(TrapChangeId,SOHandle, "TrapChangeId");	
	LOAD_SYMBOL_TEST(GetTrapMap,SOHandle, "GetTrapMap");	
	
	


	/******* ADD TRAP ****/
	lrtc=AddTrap(&dummy_trap, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	TT_DECLARE("INFO",("... Added Trap id: %ld", object_id));
	/******* ADD TRAP ****/


	/******* MODIFY TRAP ****/
	dummy_trap.trap_id=object_id;
	memcpy(&modified_object,&dummy_trap, sizeof(struct trap));
	memcpy(&returned_object,&dummy_trap, sizeof(struct trap));
	snprintf(modified_object.trap_catcher,2048, "modified-unit-test");
	rtc=UpdateTrap(&modified_object, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Modified Trap, changed name"));
	/******* MODIFY TRAP ****/

	/******* GETTRAPBYID ****/
	rtc=GetTrapById(object_id, &returned_object, CONFIG);
	tt_int_op(rtc, ==, 0);
	tt_str_op(returned_object.trap_catcher, ==, modified_object.trap_catcher);
	TT_DECLARE("INFO",("... get trap by id  %ld", object_id));
	/******* GETTRAPBYID ****/

	/******* TRAPCHANGEID ****/
	NN=lrtc+999;
	object_id=TrapChangeId(lrtc, NN, CONFIG);
	tt_int_op(object_id, ==, NN);
	TT_DECLARE("INFO",("... Changed trap id from %ld to %ld ",lrtc, object_id));
	/******* TRAPCHANGEID ****/


	


	/*** TRAPMAP **/
	srvmap = malloc(sizeof(struct trap)*(shm_hdr->trapcount+2));
	rtc=GetTrapMap(srvmap, CONFIG, TEST_ORCH_ID);
	tt_int_op(rtc, >, 0);
	lrtc=-1;
	for(x=0; x<rtc; x++) {
		if(srvmap[x].trap_id==object_id) {
			lrtc = 1;
		}
	}
	tt_int_op(lrtc, ==, 1);
	TT_DECLARE("INFO",("... Created TrapMap and found  %ld ", object_id));
	/*** TRAPMAP **/






	/*** DELETETRAP **/
	rtc=DeleteTrap(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);
	TT_DECLARE("INFO",("... Delete Trap  %ld ", object_id));
	/*** DELETETRAP **/
	




	end:
		if(srvmap != NULL) free(srvmap);
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	
	;


}


void test_trap_running(void *data) {
	(void)data;
	void * SOHandle;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct trap * map;
	int x;
	int res;
	
	int (*AddTrap)(struct trap *, char *);
	int (*DeleteTrap)(long trap_id, char *);
		

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
	
	
	/// ADD trap

	LOAD_SYMBOL_TEST(AddTrap,SOHandle, "AddTrap");
	LOAD_SYMBOL_TEST(DeleteTrap,SOHandle, "DeleteTrap");


	lrtc=AddTrap(&dummy_trap, CONFIG);
	object_id=lrtc;
	tt_int_op(lrtc, >, 0);
	
	TT_DECLARE("INFO",("... Added Trap id: %ld", object_id));

	dummy_trap.trap_id=object_id;
	
	//Reload - and detach/reattach to shm
	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
	shm_hdr->do_reload=1;
	shmdt(bartlby_address);
	sleep(2);
	

	bartlby_address=bartlby_get_shm(CONFIG);

	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);

	TT_DECLARE("INFO", ("Reloaded SHM"));
	tt_int_op(shm_hdr->do_reload, ==, 0);

	map = bartlby_SHM_TrapMap(bartlby_address);

	res = -1;
	for(x=0; x<shm_hdr->trapcount; x++) {
		
		if(map[x].trap_id == object_id) {
			res = 1;
			break;
		}
	}


	rtc=DeleteTrap(object_id, CONFIG);
	tt_int_op(rtc, ==, 1);

	TT_DECLARE("INFO",("... Delete Trap  ", object_id));



	tt_int_op(res, ==, 1);
	TT_DECLARE("INFO",("... SHM Trap found  ", object_id));



	end:
		if(SOHandle != NULL) dlclose(SOHandle);
		if(bartlby_address != NULL) shmdt(bartlby_address);
	;


}
	

//TINYTEST SETUP
struct testcase_t trap_tests[] = {
	{ "lib", test_trap_lib, },
	{ "running", test_trap_running, },
	END_OF_TESTCASES
};
