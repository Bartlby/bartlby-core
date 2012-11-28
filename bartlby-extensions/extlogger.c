#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include <mysql/mysql.h>

#include <bartlby.h>



static struct shm_header * gHdr;
static void * gDataLoaderHandle;
static char * gCFG;
static char * cType;
static char * cHost;
static char * cUser;
static char * cPassword;
static char * cDB;
static MYSQL *mysql;
static int db_connected;
	

#define EXTLOGGER_UPDATE "update log set endtime=UNIX_TIMESTAMP(), endmsg='%s', endstate=%d where service_id=%ld and endtime=0"
#define EXTLOGGER_INSERT "insert into log (starttime, service_id, startmsg, startstate) values(UNIX_TIMESTAMP(), %ld, '%s', %d)"

static void extlogger_mysql_connect(void) {
	
	
	//Establish a mysql connection
	mysql=mysql_init(NULL);
	mysql=mysql_real_connect(mysql, cHost, cUser, cPassword, NULL, 0, NULL, 0);
		if(mysql_errno(mysql) != 0) {
			db_connected=0;
			_log("extlogger: mysql error: %s", mysql_error(mysql));
		} else {
			mysql_select_db(mysql, cDB);
			if(mysql_errno(mysql) != 0) {
				_log("extlogger: mysql error: %s", mysql_error(mysql));
				db_connected=0;
			} else {
				db_connected=1;	
			}
		}
	
	
	
	
		
}

static int extlogger_service_state_changed(struct service * svc) {
	char * sqlu;
	char * sqli;	
	
	
	//_log("extlogger: STATE changed: %s:%d/%s", svc->server_name, svc->client_port, svc->service_name);
	
	
	sqlu = malloc(sizeof(char)*(strlen(EXTLOGGER_UPDATE)+strlen(svc->new_server_text)+20));
	sqli = malloc(sizeof(char)*(strlen(EXTLOGGER_INSERT)+strlen(svc->new_server_text)+20));
	
	sprintf(sqlu, EXTLOGGER_UPDATE, svc->new_server_text, svc->current_state, svc->service_id);
	sprintf(sqli, EXTLOGGER_INSERT, svc->service_id, svc->new_server_text,svc->current_state);
	
	extlogger_mysql_connect();
		
	if(db_connected == 0) {
		_log("extlogger: problem with database doin a reload");	
		gHdr->do_reload=1;
		
	} else {
		
		mysql_query(mysql, sqlu);
		if(mysql_errno(mysql) != 0) {
			
			_log("extlogger: mysql error: %s", mysql_error(mysql));
		}
		mysql_query(mysql, sqli);
		if(mysql_errno(mysql) != 0) {
			
			_log("extlogger: mysql error: %s", mysql_error(mysql));
		}
	}
	mysql_close(mysql);
	
	free(sqlu);
	free(sqli);
	
	
	
	
	
	return EXTENSION_OK;
}

int bartlby_extension_dispatcher(int type, void * data) {
	int rtc;
	
	switch(type) {
		
		case EXTENSION_CALLBACK_STATE_CHANGED:
			rtc=extlogger_service_state_changed((struct service *)data);
			return rtc;
		break;		
		default:
		
		return EXTENSION_OK;
			
		
			
	}
	return EXTENSION_OK;
}



int bartlby_extension_startup(void * shm_addr, void * dataLoaderHandle, char * configfile) {
	_log("extlogger: %s", configfile);
	/*
	extlogger_type=
	extlogger_host=
	extlogger_user=
	extlogger_password=
	extlogger_db=
	*/
	cType  = getConfigValue("extlogger_type", configfile);
	cHost = getConfigValue("extlogger_host", configfile);
	cUser = getConfigValue("extlogger_user", configfile);
	cPassword = getConfigValue("extlogger_password", configfile);
	cDB = getConfigValue("extlogger_db", configfile);
	
	if(cType == NULL || cHost == NULL || cUser  == NULL || cPassword == NULL || cDB == NULL) {
		_log("extlogger: configuration failed 'extlogger_type', 'extlogger_host', 'extlogger_user', 'extlogger_password', 'extlogger_db'");
	}
	
	
	
	gHdr=bartlby_SHM_GetHDR(shm_addr);
	gDataLoaderHandle=dataLoaderHandle;
	gCFG=configfile;
	_log("Extlogger initiated");
	
	return EXTENSION_OK;
}
int bartlby_extension_shutdown(int scheduler_end_code) {
	_log("extlogger: scheduler ended with %d", scheduler_end_code);
	
	
	return EXTENSION_OK;
}
