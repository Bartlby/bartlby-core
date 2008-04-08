/* $Id$ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2008 Helmut Januschka - All Rights Reserved
 *   Contact: <helmut@januschka.com>, <contact@bartlby.org>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 *   visit: www.bartlby.org for support
 * ----------------------------------------------------------------------- */
/*
$Revision$
$HeadURL$
$Date$
$Author$ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_bartlby.h"


/* If you declare any globals in php_bartlby.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(bartlby)
*/

/* True global resources - no need for thread safety here */
static int le_bartlby;



/* {{{ bartlby_functions[]
 *
 * Every user visible function must have an entry in bartlby_functions[].
 */
function_entry bartlby_functions[] = {
	PHP_FE(confirm_bartlby_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(bartlby_get_service,	NULL)		/* For testing, remove later. */
	PHP_FE(bartlby_get_worker,	NULL)		/* For testing, remove later. */
	PHP_FE(bartlby_get_info,	NULL)		/* For testing, remove later. */
	PHP_FE(bartlby_version,	NULL)		/* For testing, remove later. */
	PHP_FE(bartlby_config,	NULL)		/* For testing, remove later. */
	PHP_FE(bartlby_lib_info, NULL)
	
	PHP_FE(bartlby_add_server,NULL)
	PHP_FE(bartlby_delete_server,NULL)
	PHP_FE(bartlby_modify_server,NULL)
	PHP_FE(bartlby_get_server_by_id,NULL)
	
	
	PHP_FE(bartlby_encode,NULL)
	PHP_FE(bartlby_decode,NULL)
	
	
	PHP_FE(bartlby_add_service,NULL)
	PHP_FE(bartlby_delete_service,NULL)
	PHP_FE(bartlby_modify_service,NULL)
	PHP_FE(bartlby_get_service_by_id,NULL)
	
	PHP_FE(bartlby_add_worker, NULL)
	PHP_FE(bartlby_delete_worker, NULL)
	PHP_FE(bartlby_modify_worker, NULL)
	PHP_FE(bartlby_get_worker_by_id, NULL)
	
	
	PHP_FE(bartlby_add_downtime, NULL)
	PHP_FE(bartlby_downtime_map, NULL)
	PHP_FE(bartlby_modify_downtime, NULL)
	PHP_FE(bartlby_delete_downtime, NULL)
	
	PHP_FE(bartlby_toggle_service_notify, NULL)
	PHP_FE(bartlby_toggle_server_notify, NULL)
	
	PHP_FE(bartlby_toggle_service_active, NULL)
	PHP_FE(bartlby_toggle_server_active, NULL)
	
	
	PHP_FE(bartlby_toggle_sirene, NULL)
	
	PHP_FE(bartlby_svc_map,NULL)
	
	PHP_FE(bartlby_reload, NULL)
	PHP_FE(bartlby_shm_destroy, NULL)
	
	PHP_FE(bartlby_ack_problem, NULL)
	
	PHP_FE(bartlby_check_shm_size, NULL)
	
	PHP_FE(bartlby_check_force, NULL)
	
	
	PHP_FE(bartlby_event_tick, NULL)
	PHP_FE(bartlby_event_fetch, NULL)
	
	PHP_FE(bartlby_set_passive, NULL)
	
	PHP_FE(bartlby_set_server_id, NULL)
	PHP_FE(bartlby_set_service_id, NULL)
	PHP_FE(bartlby_set_worker_id, NULL)
	PHP_FE(bartlby_set_worker_state, NULL)
	PHP_FE(bartlby_set_downtime_id, NULL)
	
	{NULL, NULL, NULL}	/* Must be the last line in bartlby_functions[] */
};
/* }}} */

/* {{{ bartlby_module_entry
 */
zend_module_entry bartlby_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"bartlby",
	bartlby_functions,
	PHP_MINIT(bartlby),
	PHP_MSHUTDOWN(bartlby),
	PHP_RINIT(bartlby),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(bartlby),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(bartlby),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_BARTLBY
ZEND_GET_MODULE(bartlby)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("bartlby.global_value",      "42", PHP_INI_ALL, OnUpdateInt, global_value, zend_bartlby_globals, bartlby_globals)
    STD_PHP_INI_ENTRY("bartlby.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_bartlby_globals, bartlby_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_bartlby_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_bartlby_init_globals(zend_bartlby_globals *bartlby_globals)
{
	bartlby_globals->global_value = 0;
	bartlby_globals->global_string = NULL;
}
*/
/* }}} */

void xbartlby_decode(char * msg, int length) {
	int x;
		
	for(x=0; x<length; x++) {
		//msg[x]=2^msg[x];	
		
	}
	
}
void xbartlby_encode(char * msg, int length) {
	int x;
	for(x=0; x<length; x++) {
		//msg[x]=msg[x]^2;	
	}
	
}

char * getConfigValue(char * key, char * fname) {
	FILE * fp;
	char  str[1024];

	char * tok;
	
	fp=fopen(fname, "r");
	if(!fp) {
		php_error(E_WARNING, "Config file '%s' failed", fname);
		return NULL;
	}
	
	while(fgets(str,1024,fp) != NULL) {
		str[strlen(str)-1]='\0';
		tok=strtok(str, "=");
		if(tok != NULL) {
				if(strcmp(tok, key) == 0) {
						tok=strtok(NULL, "=");
						if(tok == NULL) {
								return NULL;
						}
						if(tok[strlen(tok)-1] == '\r') {
							tok[strlen(tok)-1]='\0';
						}
						fclose(fp);
						return strdup(tok);
						
				} else {
					continue;
				}
					
		}
			
	}
	
	
	fclose(fp);
	
	
	return NULL;
}

void * bartlby_get_sohandle(char * cfgfile) {
	char * data_lib;
	char * dlmsg;
	void * SOHandle;
	
	data_lib=getConfigValue("data_library", cfgfile);
	if(data_lib == NULL) {
		php_error(E_WARNING, "cannot find data_lib key in %s", cfgfile);	
		return NULL;
	}
	SOHandle=dlopen(data_lib, RTLD_LAZY);
	if((dlmsg=dlerror()) != NULL) {
		php_error(E_ERROR, "DL Error: %s", dlmsg);
        	return NULL;
    	}	
    	free(data_lib);
    	return SOHandle;
} 

void * bartlby_get_shm(char * cfgfile) {
	char * shmtok;
	void * bartlby_address;
	int shm_id;
	
	shmtok = getConfigValue("shm_key", cfgfile);
	if(shmtok == NULL) {
		return NULL;	
	}		
	
	
	shm_id = shmget(ftok(shmtok, 32), 0,  0777);
	free(shmtok);
	if(shm_id != -1) {
		bartlby_address=shmat(shm_id,NULL,0);
		return bartlby_address;
	} else {
		
		return NULL;
	}
}



void bartlby_mark_object_gone(char * cfg, long id, int type, int msg) {
	
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	int x;
	struct shm_header * shm_hdr;
	struct service * svcmap;
	struct server * srvmap;	
	struct worker * wrkmap;
	struct downtime * dtmap;
	char * tmpstr;
	
	bartlby_address=bartlby_get_shm(cfg); 
	if(bartlby_address != NULL) {
	
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		wrkmap=(struct worker *)(void*)&svcmap[shm_hdr->svccount]+20;
		dtmap=(struct downtime *)(void *)&wrkmap[shm_hdr->wrkcount]+20;
		srvmap=(struct server *)(void*)&dtmap[shm_hdr->dtcount]+20;
	} else {
		return; //SHM ERROR
	}

	switch(type) {
		case BARTLBY_SERVICE_GONE:
			for(x=0; x<shm_hdr->svccount; x++) {
				if(svcmap[x].service_id == id) {
					svcmap[x].is_gone = msg;
				}	
			}	
	
		break;
		case BARTLBY_SERVER_GONE:
			for(x=0; x<shm_hdr->srvcount; x++) {
				if(srvmap[x].server_id == id) {
					srvmap[x].is_gone=msg;
				}	
			}	
		break;
		case BARTLBY_WORKER_GONE:
			for(x=0; x<shm_hdr->wrkcount; x++) {
				if(wrkmap[x].worker_id == id) {
					wrkmap[x].is_gone = msg;
				}	
			}	
		break;
		default:
			return;
	}
}


/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(bartlby)
{
	/* If you have INI entries, uncomment these lines 
	ZEND_INIT_MODULE_GLOBALS(bartlby, php_bartlby_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	*/
	
	return SUCCESS;
	
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(bartlby)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(bartlby)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(bartlby)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(bartlby)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "bartlby support", "enabled");
	php_info_print_table_row(2, "php-ext version", BARTLBY_VERSION);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_bartlby_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_bartlby_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char string[256];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = sprintf(string, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "bartlby", arg);
	RETURN_STRINGL(string, len, 1);
}

PHP_FUNCTION(bartlby_version) {
	RETURN_STRING(BARTLBY_VERSION,1);	
	
}
PHP_FUNCTION(bartlby_set_downtime_id) {
	pval * bartlby_config;
	pval * from;
	pval * to;
	pval * mig;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*DowntimeChangeId)(int, int, char*);
	
	
	
	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &bartlby_config,&from, &to)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	convert_to_long(from);
	convert_to_long(to);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(DowntimeChangeId,SOHandle, "DowntimeChangeId");
	
	
	
	ret=DowntimeChangeId(Z_LVAL_P(from),Z_LVAL_P(to),Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	RETURN_LONG(ret);	
}
PHP_FUNCTION(bartlby_set_worker_id) {
	pval * bartlby_config;
	pval * from;
	pval * to;
	pval * mig;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*WorkerChangeId)(int, int,  char*);
	
	
	
	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &bartlby_config,&from, &to)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	convert_to_long(from);
	convert_to_long(to);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(WorkerChangeId,SOHandle, "WorkerChangeId");
	
	
	
	ret=WorkerChangeId(Z_LVAL_P(from),Z_LVAL_P(to),Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	RETURN_LONG(ret);	
}
PHP_FUNCTION(bartlby_set_service_id) {
	pval * bartlby_config;
	pval * from;
	pval * to;
	pval * mig;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*ServiceChangeId)(int, int,  char*);
	
	
	
	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &bartlby_config,&from, &to)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	convert_to_long(from);
	convert_to_long(to);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(ServiceChangeId,SOHandle, "ServiceChangeId");
	
	
	
	ret=ServiceChangeId(Z_LVAL_P(from),Z_LVAL_P(to),Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	RETURN_LONG(ret);	
}
PHP_FUNCTION(bartlby_set_server_id) {
	pval * bartlby_config;
	pval * from;
	pval * to;
	pval * mig;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*ServerChangeId)(int, int, int, char*);
	
	
	
	if (ZEND_NUM_ARGS() != 4 || getParameters(ht, 4, &bartlby_config,&from, &to, &mig)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	convert_to_long(from);
	convert_to_long(to);
	convert_to_long(mig);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(ServerChangeId,SOHandle, "ServerChangeId");
	
	
	
	ret=ServerChangeId(Z_LVAL_P(from),Z_LVAL_P(to),Z_LVAL_P(mig),Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	RETURN_LONG(ret);	
}

PHP_FUNCTION(bartlby_delete_downtime) {
	pval * bartlby_config;
	pval * downtime_id;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*DeleteDowntime)(int, char*);
	
	struct service svc;
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config,&downtime_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	
	convert_to_long(downtime_id);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(DeleteDowntime,SOHandle, "DeleteDowntime");
	
	
	
	ret=DeleteDowntime(Z_LVAL_P(downtime_id),Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	RETURN_LONG(ret);	
}
PHP_FUNCTION(bartlby_modify_downtime) {
	pval * bartlby_config;
	pval * dfrom;
	pval * dto;
	pval * dtype;
	pval * dnotice;
	pval * dservice;
	pval * mid;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*UpdateDowntime)(struct downtime *,char *);
	
	struct downtime svc;
	
	if (ZEND_NUM_ARGS() != 7 || getParameters(ht, 7, &bartlby_config,&dfrom, &dto, &dtype, &dnotice, &dservice, &mid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	convert_to_long(dfrom);
	convert_to_long(dto);
	convert_to_long(dtype);
	convert_to_long(dservice);
	convert_to_long(mid);
	
	
	convert_to_string(dnotice);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(UpdateDowntime,SOHandle, "UpdateDowntime");
	
	strcpy(svc.downtime_notice, Z_STRVAL_P(dnotice));
	svc.downtime_from=Z_LVAL_P(dfrom);
	svc.downtime_to=Z_LVAL_P(dto);
	svc.downtime_type=Z_LVAL_P(dtype);
	svc.service_id=Z_LVAL_P(dservice);
	svc.downtime_id=Z_LVAL_P(mid);
	
	ret=UpdateDowntime(&svc, Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	
	RETURN_LONG(ret);		
}
PHP_FUNCTION(bartlby_downtime_map) {
	zval * subarray;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	
	int x;
	struct service * svcmap;
	struct worker * wrkmap;
	struct downtime * dtmap;
	
	pval * bartlby_config;
	
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &bartlby_config)==FAILURE) {
		WRONG_PARAM_COUNT;
	}	
	
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config));
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		wrkmap=(struct worker *)(void*)&svcmap[shm_hdr->svccount]+20;
		dtmap=(struct downtime *)(void *)&wrkmap[shm_hdr->wrkcount]+20;
		
		for(x=0; x<shm_hdr->dtcount; x++) {
			
			ALLOC_INIT_ZVAL(subarray);
			array_init(subarray);
			
			add_assoc_long(subarray, "downtime_id", dtmap[x].downtime_id);
			add_assoc_long(subarray, "downtime_from", dtmap[x].downtime_from);
			add_assoc_long(subarray, "downtime_to", dtmap[x].downtime_to);
			add_assoc_long(subarray, "downtime_type", dtmap[x].downtime_type);
			add_assoc_long(subarray, "service_id", dtmap[x].service_id);
			
			add_assoc_long(subarray, "is_gone", dtmap[x].is_gone);
			
			add_assoc_string(subarray, "downtime_notice", dtmap[x].downtime_notice, 1);
			
			//Push SVC to map
			add_next_index_zval(return_value, subarray);
			
		}
		
		
		
		shmdt(bartlby_address);
		

	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}
		
	

		
}



PHP_FUNCTION(bartlby_add_downtime) {
	pval * bartlby_config;
	pval * dfrom;
	pval * dto;
	pval * dtype;
	pval * dnotice;
	pval * dservice;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*AddDowntime)(struct downtime *,char *);
	
	struct downtime svc;
	
	if (ZEND_NUM_ARGS() != 6 || getParameters(ht, 6, &bartlby_config,&dfrom, &dto, &dtype, &dnotice, &dservice)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	convert_to_long(dfrom);
	convert_to_long(dto);
	convert_to_long(dtype);
	convert_to_long(dservice);
	
	convert_to_string(dnotice);
		
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(AddDowntime,SOHandle, "AddDowntime");
	
	strcpy(svc.downtime_notice, Z_STRVAL_P(dnotice));
	svc.downtime_from=Z_LVAL_P(dfrom);
	svc.downtime_to=Z_LVAL_P(dto);
	svc.downtime_type=Z_LVAL_P(dtype);
	svc.service_id=Z_LVAL_P(dservice);
	
	ret=AddDowntime(&svc, Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	RETURN_LONG(ret);	
}

//size_of_structs=sizeof(struct shm_header)+sizeof(struct worker)+sizeof(struct service)+sizeof(struct downtime);

PHP_FUNCTION(bartlby_event_tick) {
	pval * bartlby_config;
	pval * bartlby_service_id;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct service * svcmap;
	struct worker * wrkmap;
	struct downtime * dtmap;
	
	int idx;
		
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &bartlby_config)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		idx=shm_hdr->cur_event_index;
		shmdt(bartlby_address);
		RETURN_LONG(idx);
		
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");
		RETURN_FALSE;
	}	
}

PHP_FUNCTION(bartlby_event_fetch) {
	pval * bartlby_config;
	pval * bartlby_service_id;
	pval * event_index;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct service * svcmap;
	struct worker * wrkmap;
	struct downtime * dtmap;
	struct btl_event * evntmap;
	struct server * srvmap;	
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config, &event_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	convert_to_long(event_index);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		wrkmap=(struct worker *)(void*)&svcmap[shm_hdr->svccount]+20;
		dtmap=(struct downtime *)(void*)&wrkmap[shm_hdr->wrkcount]+20;
		srvmap=(struct server *)(void*)&dtmap[shm_hdr->dtcount]+20;
		evntmap=(struct btl_event *)(void *)&srvmap[shm_hdr->srvcount]+20;
		
		if(Z_LVAL_P(event_index) < EVENT_QUEUE_MAX) {
			add_assoc_string(return_value, "message", evntmap[Z_LVAL_P(event_index)].evnt_message, 1);
			add_assoc_long(return_value, "id", evntmap[Z_LVAL_P(event_index)].evnt_id);
			add_assoc_long(return_value, "time", evntmap[Z_LVAL_P(event_index)].evnt_time);
			shmdt(bartlby_address);
		} else {
			shmdt(bartlby_address);
			RETURN_FALSE;
		}
		
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");
		RETURN_FALSE;
	}	
}


PHP_FUNCTION(bartlby_check_shm_size) {
	pval * bartlby_config;
	pval * bartlby_service_id;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int my_size;
	
	struct service * svcmap;	
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &bartlby_config)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		my_size=sizeof(struct shm_header)+ sizeof(struct server) + sizeof(struct worker)+sizeof(struct service)+sizeof(struct downtime);
		if(my_size != shm_hdr->size_of_structs) {	
			shmdt(bartlby_address);
			RETURN_FALSE;
			
		} else {
			shmdt(bartlby_address);
			RETURN_TRUE;
		}		
		
		
	
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");
		RETURN_FALSE;
	}	
}

PHP_FUNCTION(bartlby_toggle_sirene) {
	pval * bartlby_config;
	pval * bartlby_service_id;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int r;
	
	struct service * svcmap;	
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &bartlby_config)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
			
		if(shm_hdr->sirene_mode== 1) {
			shm_hdr->sirene_mode = 0;
			r=0;
		} else {
			shm_hdr->sirene_mode = 1;
			r=1;
		}
		shmdt(bartlby_address);
		RETURN_LONG(r);
		
	
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}	
}


PHP_FUNCTION(bartlby_set_passive) {
	pval * bartlby_config;
	pval * bartlby_new_state;
	pval * bartlby_new_output;
	pval * bartlby_service_id;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int r;
	
	struct service * svcmap;	
	
	if (ZEND_NUM_ARGS() != 4 || getParameters(ht, 4, &bartlby_config, &bartlby_service_id, &bartlby_new_state, &bartlby_new_output)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(bartlby_service_id);
	convert_to_long(bartlby_new_state);
	
	convert_to_string(bartlby_config);
	convert_to_string(bartlby_new_output);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		
		
		if(Z_LVAL_P(bartlby_service_id) > shm_hdr->svccount-1) {
			php_error(E_WARNING, "Service id out of bounds");	
			RETURN_FALSE;	
		}
		
		svcmap[Z_LVAL_P(bartlby_service_id)].last_state = svcmap[Z_LVAL_P(bartlby_service_id)].current_state;
		svcmap[Z_LVAL_P(bartlby_service_id)].current_state = Z_LVAL_P(bartlby_new_state);	
		snprintf(svcmap[Z_LVAL_P(bartlby_service_id)].new_server_text, 2040, "%s",Z_STRVAL_P(bartlby_new_output)); 
		svcmap[Z_LVAL_P(bartlby_service_id)].last_check = time(NULL);	
		
		r=1;
		shmdt(bartlby_address);
		RETURN_LONG(r);
		
	
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}	
	
}


PHP_FUNCTION(bartlby_ack_problem) {
	pval * bartlby_config;
	pval * bartlby_service_id;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int r;
	
	struct service * svcmap;	
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config, &bartlby_service_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(bartlby_service_id);
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		
		
		if(Z_LVAL_P(bartlby_service_id) > shm_hdr->svccount-1) {
			php_error(E_WARNING, "Service id out of bounds");	
			RETURN_FALSE;	
		}
		
		svcmap[Z_LVAL_P(bartlby_service_id)].service_ack = 1;	
		
		r=1;
		shmdt(bartlby_address);
		RETURN_LONG(r);
		
	
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}	
	
}
PHP_FUNCTION(bartlby_toggle_service_active) {
	pval * bartlby_config;
	pval * bartlby_service_id;
	pval * do_writeback;
	
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int r;
	void * SOHandle;
	char * dlmsg;
	
	struct service * svcmap;	
	int (*UpdateService)(struct service *, char *);
	
	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &bartlby_config, &bartlby_service_id, &do_writeback)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(bartlby_service_id);
	convert_to_long(do_writeback);
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		
		
		if(Z_LVAL_P(bartlby_service_id) > shm_hdr->svccount-1) {
			php_error(E_WARNING, "Service id out of bounds");	
			RETURN_FALSE;	
		}
		if(svcmap[Z_LVAL_P(bartlby_service_id)].service_active == 1) {
			svcmap[Z_LVAL_P(bartlby_service_id)].service_active = 0;
			r=0;
		} else {
			svcmap[Z_LVAL_P(bartlby_service_id)].service_active = 1;	
			r=1;
		}
		if(Z_LVAL_P(do_writeback) == 1) {
			LOAD_SYMBOL(UpdateService,SOHandle, "UpdateService");
			UpdateService(&svcmap[Z_LVAL_P(bartlby_service_id)], Z_STRVAL_P(bartlby_config));
		}
		
		dlclose(SOHandle);
		shmdt(bartlby_address);
		RETURN_LONG(r);
		
	
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}	
}

//bartlby_toggle_server_notify

PHP_FUNCTION(bartlby_toggle_server_notify) {
	pval * bartlby_config;
	pval * bartlby_service_id;
	pval * do_writeback;
	
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int r;
	void * SOHandle;
	char * dlmsg;
	
	struct service * svcmap;
	struct worker * wrkmap;
	struct downtime * dtmap;
	struct server * srvmap;
		
	int (*ModifyServer)(struct server *, char *);
	
	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &bartlby_config, &bartlby_service_id, &do_writeback)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(bartlby_service_id);
	convert_to_long(do_writeback);
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		wrkmap=(struct worker *)(void*)&svcmap[shm_hdr->svccount]+20;
		dtmap=(struct downtime *)(void*)&wrkmap[shm_hdr->wrkcount]+20;
		srvmap=(struct server *)(void*)&dtmap[shm_hdr->dtcount]+20;
		
		
		if(Z_LVAL_P(bartlby_service_id) > shm_hdr->srvcount-1) {
			php_error(E_WARNING, "Server id out of bounds");	
			RETURN_FALSE;	
		}
		if(srvmap[Z_LVAL_P(bartlby_service_id)].server_notify == 1) {
			srvmap[Z_LVAL_P(bartlby_service_id)].server_notify = 0;
			r=0;
		} else {
			srvmap[Z_LVAL_P(bartlby_service_id)].server_notify = 1;	
			r=1;
		}
		if(Z_LVAL_P(do_writeback) == 1) {
			LOAD_SYMBOL(ModifyServer,SOHandle, "ModifyServer");
			ModifyServer(&srvmap[Z_LVAL_P(bartlby_service_id)], Z_STRVAL_P(bartlby_config));
		}
		
		dlclose(SOHandle);
		shmdt(bartlby_address);
		RETURN_LONG(r);
		
	
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}	
}



PHP_FUNCTION(bartlby_toggle_server_active) {
	pval * bartlby_config;
	pval * bartlby_service_id;
	pval * do_writeback;
	
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int r;
	void * SOHandle;
	char * dlmsg;
	
	struct service * svcmap;
	struct worker * wrkmap;
	struct downtime * dtmap;
	struct server * srvmap;
		
	int (*ModifyServer)(struct server *, char *);
	
	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &bartlby_config, &bartlby_service_id, &do_writeback)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(bartlby_service_id);
	convert_to_long(do_writeback);
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		wrkmap=(struct worker *)(void*)&svcmap[shm_hdr->svccount]+20;
		dtmap=(struct downtime *)(void*)&wrkmap[shm_hdr->wrkcount]+20;
		srvmap=(struct server *)(void*)&dtmap[shm_hdr->dtcount]+20;
		
		
		if(Z_LVAL_P(bartlby_service_id) > shm_hdr->srvcount-1) {
			php_error(E_WARNING, "Server id out of bounds");	
			RETURN_FALSE;	
		}
		if(srvmap[Z_LVAL_P(bartlby_service_id)].server_enabled == 1) {
			srvmap[Z_LVAL_P(bartlby_service_id)].server_enabled = 0;
			r=0;
		} else {
			srvmap[Z_LVAL_P(bartlby_service_id)].server_enabled = 1;	
			r=1;
		}
		if(Z_LVAL_P(do_writeback) == 1) {
			LOAD_SYMBOL(ModifyServer,SOHandle, "ModifyServer");
			ModifyServer(&srvmap[Z_LVAL_P(bartlby_service_id)], Z_STRVAL_P(bartlby_config));
		}
		
		dlclose(SOHandle);
		shmdt(bartlby_address);
		RETURN_LONG(r);
		
	
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}	
}

PHP_FUNCTION(bartlby_set_worker_state) {
	pval * bartlby_config;
	pval * bartlby_worker_id;
	pval * new_state;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int r;
	struct worker * wrkmap;
	struct service * svcmap;
	
	void * SOHandle;
	char * dlmsg;
	int (*UpdateWorker)(struct worker *, char *);
	
	
	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &bartlby_config, &bartlby_worker_id, &new_state)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(bartlby_worker_id);
	convert_to_long(new_state);
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
 	
		
	
	
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		wrkmap=(struct worker *)(void*)&svcmap[shm_hdr->svccount]+20;
		
		
		
		if(Z_LVAL_P(bartlby_worker_id) > shm_hdr->wrkcount-1) {
			php_error(E_WARNING, "Worker id out of bounds");	
			RETURN_FALSE;	
		}
		
		
		r=1;
		wrkmap[Z_LVAL_P(bartlby_worker_id)].active=Z_LVAL_P(new_state);
		
		LOAD_SYMBOL(UpdateWorker,SOHandle, "UpdateWorker");
		UpdateWorker(&wrkmap[Z_LVAL_P(bartlby_worker_id)], Z_STRVAL_P(bartlby_config));
		
		
		dlclose(SOHandle);
		shmdt(bartlby_address);
		RETURN_LONG(r);
		
	
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}	
}

PHP_FUNCTION(bartlby_toggle_service_notify) {
	pval * bartlby_config;
	pval * bartlby_service_id;
	pval * do_writeback;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int r;
	struct service * svcmap; 
	
	void * SOHandle;
	char * dlmsg;
	int (*UpdateService)(struct service *, char *);
	
	
	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &bartlby_config, &bartlby_service_id, &do_writeback)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(bartlby_service_id);
	convert_to_long(do_writeback);
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
 	
		
	
	
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		
		
		if(Z_LVAL_P(bartlby_service_id) > shm_hdr->svccount-1) {
			php_error(E_WARNING, "Service id out of bounds");	
			RETURN_FALSE;	
		}
		if(svcmap[Z_LVAL_P(bartlby_service_id)].notify_enabled == 1) {
			svcmap[Z_LVAL_P(bartlby_service_id)].notify_enabled = 0;
			r=0;
		} else {
			svcmap[Z_LVAL_P(bartlby_service_id)].notify_enabled = 1;	
			r=1;
		}
		if(Z_LVAL_P(do_writeback) == 1) {
			LOAD_SYMBOL(UpdateService,SOHandle, "UpdateService");
			UpdateService(&svcmap[Z_LVAL_P(bartlby_service_id)], Z_STRVAL_P(bartlby_config));
			
		}
		
		dlclose(SOHandle);
		shmdt(bartlby_address);
		RETURN_LONG(r);
		
	
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}	
}


//bartlby_check_force
PHP_FUNCTION(bartlby_check_force) {
	pval * bartlby_config;
	pval * bartlby_service_id;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int r;
	struct service * svcmap; 
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config, &bartlby_service_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(bartlby_service_id);
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		
		
		if(Z_LVAL_P(bartlby_service_id) > shm_hdr->svccount-1) {
			php_error(E_WARNING, "Service id out of bounds");	
			RETURN_FALSE;	
		}
		svcmap[Z_LVAL_P(bartlby_service_id)].do_force = 1;	
		r=1;
		
		
		shmdt(bartlby_address);
		RETURN_LONG(r);
		
	
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}	
}


int btl_is_array(pval * ar, int service_id) {
	HashTable *arr_hash;
	zval **data;
    	HashPosition pointer;
    	int array_count;
    	char * string_key;
    	int str_key_len;
    	long num_key;
    	
    	if(Z_TYPE_P(ar) != IS_ARRAY) {
    		return 1;
    	}
	
	arr_hash = Z_ARRVAL_P(ar);
    	array_count = zend_hash_num_elements(arr_hash);
    	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {

            if(Z_TYPE_PP(data) == IS_STRING) {
            	//printf("String: %s\n", Z_STRVAL_PP(data));	
            } else if(Z_TYPE_PP(data) == IS_LONG) {
            	//printf("Long: %d\n", Z_LVAL_PP(data));	
            	if(Z_LVAL_PP(data) == service_id) {
        		return 1;
        	}
            }

            	
    	}
    	return -1;
}

PHP_FUNCTION(bartlby_svc_map) {
	zval * subarray;
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	
	int x;
	int y;
	int is_down;
	int current_time;
	int current_state;
	
	struct service * svcmap;
	struct worker * wrkmap;
	struct downtime * dtmap;
	struct server * srvmap;
	
	pval * bartlby_config;
	pval * svc_right_array;
	pval * server_right_array;
	
	if (ZEND_NUM_ARGS() != 3 || getParameters(ht, 3, &bartlby_config, &svc_right_array, &server_right_array)==FAILURE) {
		WRONG_PARAM_COUNT;
	}	
	
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config));
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		wrkmap=(struct worker *)(void*)&svcmap[shm_hdr->svccount]+20;
		dtmap=(struct downtime *)(void*)&wrkmap[shm_hdr->wrkcount]+20;
		srvmap=(struct server *)(void*)&dtmap[shm_hdr->dtcount]+20;
		
		current_time=time(NULL);
		for(x=0; x<shm_hdr->svccount; x++) {
			if(btl_is_array(svc_right_array, svcmap[x].service_id) == -1 &&  btl_is_array(server_right_array, svcmap[x].server_id) == -1) {
				continue;	
			}
			ALLOC_INIT_ZVAL(subarray);
			array_init(subarray);
			add_assoc_long(subarray, "service_id", svcmap[x].service_id);
			add_assoc_long(subarray, "server_id", svcmap[x].server_id);
			add_assoc_long(subarray, "last_state", svcmap[x].last_state);
			
			
			if(svcmap[x].is_server_dead < 0) {
				add_assoc_long(subarray, "current_state", -3);
				add_assoc_long(subarray, "server_is_dead", 1);
			} else {
				add_assoc_long(subarray, "current_state", svcmap[x].current_state);
			}
			
			
			add_assoc_long(subarray, "client_port", srvmap[svcmap[x].srv_place].client_port);
					
			add_assoc_string(subarray, "new_server_text", svcmap[x].new_server_text, 1);
			add_assoc_string(subarray, "service_name", svcmap[x].service_name, 1);
			add_assoc_string(subarray, "server_name", srvmap[svcmap[x].srv_place].server_name, 1);
			add_assoc_string(subarray, "client_ip", srvmap[svcmap[x].srv_place].client_ip, 1);
			add_assoc_string(subarray, "plugin", svcmap[x].plugin, 1);
			add_assoc_string(subarray, "plugin_arguments", svcmap[x].plugin_arguments, 1);
			
			add_assoc_long(subarray, "check_interval", svcmap[x].check_interval);
			add_assoc_long(subarray, "check_interval_original", svcmap[x].check_interval_original);
			add_assoc_long(subarray, "last_check", svcmap[x].last_check);
			
			add_assoc_string(subarray, "exec_plan", svcmap[x].service_exec_plan, 1);
			
			add_assoc_long(subarray, "notify_enabled", svcmap[x].notify_enabled);
			add_assoc_long(subarray, "last_notify_send", svcmap[x].last_notify_send);
			add_assoc_long(subarray, "flap_count", svcmap[x].flap_count);
			
			
			add_assoc_long(subarray, "service_active", svcmap[x].service_active);
			add_assoc_long(subarray, "service_type", svcmap[x].service_type);
			add_assoc_long(subarray, "service_passive_timeout", svcmap[x].service_passive_timeout);
			
			
			add_assoc_string(subarray, "service_var", svcmap[x].service_var, 1);
			add_assoc_string(subarray, "server_icon", srvmap[svcmap[x].srv_place].server_icon, 1);
			add_assoc_long(subarray, "service_check_timeout", svcmap[x].service_check_timeout);
			add_assoc_long(subarray, "service_ack", svcmap[x].service_ack);
			add_assoc_long(subarray, "service_retain", svcmap[x].service_retain);
			add_assoc_long(subarray, "service_retain_current", svcmap[x].service_retain_current);
			add_assoc_long(subarray, "check_is_running", svcmap[x].process.pid);
			add_assoc_long(subarray, "check_starttime", svcmap[x].process.start_time);
			
			add_assoc_long(subarray, "shm_place", x);
			
			add_assoc_long(subarray, "service_time_sum", svcmap[x].pstat.sum);
			add_assoc_long(subarray, "service_time_count",svcmap[x].pstat.counter);
		
			add_assoc_long(subarray, "service_delay_sum", svcmap[x].delay_time.sum);
			add_assoc_long(subarray, "service_delay_count",svcmap[x].delay_time.counter);
			
			//bartlby_get_service_by_id
			
			add_assoc_string(subarray, "service_snmp_community", svcmap[x].snmp_info.community, 1);
			add_assoc_string(subarray, "service_snmp_objid", svcmap[x].snmp_info.objid, 1);
			
			add_assoc_long(subarray, "service_snmp_warning",svcmap[x].snmp_info.warn);
			add_assoc_long(subarray, "service_snmp_critical",svcmap[x].snmp_info.crit);
			add_assoc_long(subarray, "service_snmp_version",svcmap[x].snmp_info.version);
			add_assoc_long(subarray, "service_snmp_type",svcmap[x].snmp_info.type);
			
			add_assoc_long(subarray, "flap_seconds",svcmap[x].flap_seconds);
			
			add_assoc_long(subarray, "server_flap_seconds",srvmap[svcmap[x].srv_place].server_flap_seconds);
			add_assoc_long(subarray, "server_last_notify_send",srvmap[svcmap[x].srv_place].last_notify_send);
			add_assoc_long(subarray, "server_notify",srvmap[svcmap[x].srv_place].server_notify);
			add_assoc_long(subarray, "server_enabled",srvmap[svcmap[x].srv_place].server_enabled);
			add_assoc_long(subarray, "server_dead",srvmap[svcmap[x].srv_place].server_dead);
			
			
			//svc.renotify_interval, svc.escalate_divisor
			add_assoc_long(subarray, "renotify_interval",svcmap[x].renotify_interval);
			add_assoc_long(subarray, "escalate_divisor",svcmap[x].escalate_divisor);
			
			add_assoc_long(subarray, "server_gone",srvmap[svcmap[x].srv_place].is_gone);
			
			//svc.is_gone
			add_assoc_long(subarray, "is_gone",svcmap[x].is_gone);
			
			
			for(y=0; y<shm_hdr->dtcount; y++) {
				is_down=0;
				if(current_time >= dtmap[y].downtime_from && current_time <= dtmap[y].downtime_to) {
					switch(dtmap[y].downtime_type) {
						case DT_SERVICE:
							if(dtmap[y].service_id == svcmap[x].service_id) {
								is_down=1;	
								
							}
						break;
						case DT_SERVER:
							if(dtmap[y].service_id == svcmap[x].server_id) {
								is_down=2;	
							}
						break;				
					}
					if(is_down > 0) {
						add_assoc_long(subarray, "is_downtime", 1);
						add_assoc_long(subarray, "downtime_from", dtmap[y].downtime_from);
						add_assoc_long(subarray, "downtime_to", dtmap[y].downtime_to);
						add_assoc_string(subarray, "downtime_notice", dtmap[y].downtime_notice, 1);
						add_assoc_long(subarray, "downtime_service", dtmap[y].service_id);
						add_assoc_long(subarray, "downtime_type", dtmap[y].downtime_type);
						break;
					}
				}	
			}
			if(is_down==0) {
				add_assoc_long(subarray, "is_downtime", 0);	
			}
			//Push SVC to map
			add_next_index_zval(return_value, subarray);
			
		}
		
		
		
		shmdt(bartlby_address);
		

	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}
		
	
	/*
	array_init(return_value);
	
	add_index_long(return_value,100, 1);
	ALLOC_INIT_ZVAL(subarray);
	array_init(subarray);
	add_next_index_string(subarray,"testsub", 1);
	add_assoc_zval(return_value, "sub", subarray);
	
	
	ALLOC_INIT_ZVAL(subarray);
	array_init(subarray);
	add_next_index_string(subarray,"testsub1", 1);
	add_assoc_zval(return_value, "sub1", subarray);
	*/
		
}
PHP_FUNCTION(bartlby_encode) {
	pval * instr;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &instr)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(instr);
	xbartlby_encode(Z_STRVAL_P(instr), strlen(Z_STRVAL_P(instr)));
	
	RETURN_STRING(Z_STRVAL_P(instr),1);
	
	
}

PHP_FUNCTION(bartlby_decode) {
	pval * instr;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &instr)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(instr);
	xbartlby_decode(Z_STRVAL_P(instr), strlen(Z_STRVAL_P(instr)));
	
	RETURN_STRING(Z_STRVAL_P(instr),1);	
}
PHP_FUNCTION(bartlby_shm_destroy) {
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	struct shmid_ds shm_desc;
	
	
	pval * bartlby_config;
	
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &bartlby_config)==FAILURE) {
		WRONG_PARAM_COUNT;
	}	
	
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	shmtok = getConfigValue("shm_key", Z_STRVAL_P(bartlby_config));
	if(shmtok == NULL) {
		RETURN_FALSE;
	}		
	
	
	shm_id = shmget(ftok(shmtok, 32), 0,  0777);
	
	shmctl(shm_id, IPC_RMID, &shm_desc);
	
	
	free(shmtok);
	RETURN_TRUE;	
}

PHP_FUNCTION(bartlby_reload) {
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	
	
	
	pval * bartlby_config;
	
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &bartlby_config)==FAILURE) {
		WRONG_PARAM_COUNT;
	}	
	
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config));
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		shm_hdr->do_reload=1;
		
		
		
		add_assoc_string(return_value, "reload", "OK", 1);
		
		shmdt(bartlby_address);
		

	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}
		
	RETURN_TRUE;	
}
PHP_FUNCTION(bartlby_add_worker) {
	//svc->mail, svc->icq, svc->services, svc->notify_levels, svc->active, svc->name
	pval * bartlby_config;
	pval * mail;
	pval * icq;
	pval * services;
	pval * notify_levels;
	pval * active;
	pval * name;
	pval * password;
	pval * enabled_triggers;
	pval * escalation_limit;
	pval * escalation_minutes;
	pval * notify_plan;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*AddWorker)(struct worker *,char *);
	
	struct worker svc;
	
	if (ZEND_NUM_ARGS() != 12 || getParameters(ht, 12, &bartlby_config,&mail, &icq, &services, &notify_levels, &active, &name, &password, &enabled_triggers, &escalation_limit, &escalation_minutes, &notify_plan)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(enabled_triggers);
	convert_to_string(bartlby_config);
	convert_to_string(mail);
	convert_to_string(icq);
	convert_to_string(password);
	convert_to_string(services);
	convert_to_string(notify_levels);
	convert_to_string(name);
	convert_to_long(active);
	convert_to_string(notify_plan);
	
	convert_to_long(escalation_limit);
	convert_to_long(escalation_minutes);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(AddWorker,SOHandle, "AddWorker");
	
	strcpy(svc.password, Z_STRVAL_P(password));
	strcpy(svc.name, Z_STRVAL_P(name));
	
	strcpy(svc.notify_plan, Z_STRVAL_P(notify_plan));
	
	strcpy(svc.mail, Z_STRVAL_P(mail));
	strcpy(svc.icq, Z_STRVAL_P(icq));
	strcpy(svc.services, Z_STRVAL_P(services));
	strcpy(svc.notify_levels, Z_STRVAL_P(notify_levels));
	strcpy(svc.enabled_triggers, Z_STRVAL_P(enabled_triggers));
	svc.active=Z_LVAL_P(active);
	svc.escalation_limit=Z_LVAL_P(escalation_limit);
	svc.escalation_minutes=Z_LVAL_P(escalation_minutes);
	
	ret=AddWorker(&svc, Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	RETURN_LONG(ret);	
}

PHP_FUNCTION(bartlby_delete_worker) {
	pval * bartlby_config;
	pval * worker_id;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*DeleteWorker)(int, char*);
	
	struct service svc;
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config,&worker_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	
	convert_to_long(worker_id);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(DeleteWorker,SOHandle, "DeleteWorker");
	
	
	
	ret=DeleteWorker(Z_LVAL_P(worker_id),Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	bartlby_mark_object_gone(Z_STRVAL_P(bartlby_config),Z_LVAL_P(worker_id), BARTLBY_WORKER_GONE, BARTLBY_OBJECT_DELETED);

	RETURN_LONG(ret);	
}

PHP_FUNCTION(bartlby_modify_worker) {
	pval * bartlby_config;
	pval * mail;
	pval * icq;
	pval * services;
	pval * notify_levels;
	pval * active;
	pval * name;
	pval * worker_id;
	pval * password;
	pval * enabled_triggers;
	pval * escalation_limit;
	pval * escalation_minutes;
	pval * notify_plan;
	
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*UpdateWorker)(struct worker *,char *);
	
	struct worker svc;
	
	if (ZEND_NUM_ARGS() != 13 || getParameters(ht, 13, &bartlby_config,&worker_id, &mail, &icq, &services, &notify_levels, &active, &name, &password, &enabled_triggers, &escalation_limit, &escalation_minutes, &notify_plan)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(enabled_triggers);
	convert_to_string(bartlby_config);
	convert_to_string(mail);
	convert_to_string(password);
	convert_to_string(icq);
	convert_to_string(services);
	convert_to_string(notify_levels);
	convert_to_string(name);
	convert_to_long(active);
	convert_to_string(notify_plan);
	
	convert_to_long(worker_id);
	convert_to_long(escalation_limit);
	convert_to_long(escalation_minutes);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(UpdateWorker,SOHandle, "UpdateWorker");
	
	strcpy(svc.notify_plan, Z_STRVAL_P(notify_plan));
	strcpy(svc.password, Z_STRVAL_P(password));
	strcpy(svc.name, Z_STRVAL_P(name));
	strcpy(svc.mail, Z_STRVAL_P(mail));
	strcpy(svc.icq, Z_STRVAL_P(icq));
	strcpy(svc.services, Z_STRVAL_P(services));
	strcpy(svc.notify_levels, Z_STRVAL_P(notify_levels));
	strcpy(svc.enabled_triggers, Z_STRVAL_P(enabled_triggers));
	svc.active=Z_LVAL_P(active);
	svc.worker_id=Z_LVAL_P(worker_id);
	
	svc.escalation_limit=Z_LVAL_P(escalation_limit);
	svc.escalation_minutes=Z_LVAL_P(escalation_minutes);
	
	ret=UpdateWorker(&svc, Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	RETURN_STRING(Z_STRVAL_P(icq),1);
	//Mark as gone
	bartlby_mark_object_gone(Z_STRVAL_P(bartlby_config),Z_LVAL_P(worker_id), BARTLBY_WORKER_GONE, BARTLBY_OBJECT_CHANGED);
	
	RETURN_LONG(ret);		
}

PHP_FUNCTION(bartlby_get_worker_by_id) {
		pval * bartlby_config;
	
	pval * worker_id;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*GetWorkerById)(int,struct worker *, char *);
	
	struct worker  svc;
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config,&worker_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	
	convert_to_long(worker_id);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(GetWorkerById,SOHandle, "GetWorkerById");
	
	ret=GetWorkerById(Z_LVAL_P(worker_id),&svc, Z_STRVAL_P(bartlby_config));
	
	if(ret < 0) {
		RETURN_FALSE;	
	} else {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_string(return_value, "mail", svc.mail, 1);
		add_assoc_string(return_value, "icq", svc.icq, 1);
		add_assoc_string(return_value, "notify_plan", svc.notify_plan, 1);
		add_assoc_string(return_value, "services", svc.services, 1);
		
		
		
		add_assoc_string(return_value, "notify_levels", svc.notify_levels,1);
		add_assoc_string(return_value, "name", svc.name,1);
		add_assoc_string(return_value, "password", svc.password,1);
		add_assoc_string(return_value, "enabled_triggers", svc.enabled_triggers,1);
		 
		add_assoc_long(return_value, "worker_id", svc.worker_id);
		add_assoc_long(return_value, "active", svc.active);
		
		add_assoc_long(return_value, "escalation_limit", svc.escalation_limit);
		add_assoc_long(return_value, "escalation_minutes", svc.escalation_minutes);
	}
	dlclose(SOHandle);
		
}

PHP_FUNCTION(bartlby_config) {
	pval * bartlby_config;
	pval * bartlby_key;
	char * k;
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config, &bartlby_key)==FAILURE) {
		WRONG_PARAM_COUNT;
	}	
	
	convert_to_string(bartlby_config);	
	convert_to_string(bartlby_key);	
	
	k=getConfigValue(Z_STRVAL_P(bartlby_key), Z_STRVAL_P(bartlby_config));
	if(k == NULL) {
		
		RETURN_FALSE;
	} else {
		RETURN_STRING(k,1);	
	}
	
	
}

PHP_FUNCTION(bartlby_get_info) {
	
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	
	
	
	
	pval * bartlby_config;
	pval * bartlby_service_id;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &bartlby_config)==FAILURE) {
		WRONG_PARAM_COUNT;
	}	
	
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config));
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		
		add_assoc_long(return_value, "services", shm_hdr->svccount);
		
		add_assoc_long(return_value, "workers", shm_hdr->wrkcount);
		add_assoc_long(return_value, "current_running", shm_hdr->current_running);
		add_assoc_long(return_value, "do_reload", shm_hdr->do_reload);
		//add_assoc_long(return_value, "version", shm_hdr->version);
		add_assoc_string(return_value, "version", shm_hdr->version, 1);
		add_assoc_long(return_value, "last_replication", shm_hdr->last_replication);
		add_assoc_long(return_value, "startup_time", shm_hdr->startup_time);
		add_assoc_long(return_value, "downtimes", shm_hdr->dtcount);
		add_assoc_long(return_value, "sirene_mode", shm_hdr->sirene_mode);
		add_assoc_long(return_value, "servers", shm_hdr->srvcount);
		add_assoc_long(return_value, "round_time_sum", shm_hdr->pstat.sum);
		add_assoc_long(return_value, "round_time_count", shm_hdr->pstat.counter);
		
		shmdt(bartlby_address);
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		
		RETURN_FALSE;
	}
	
	
}

PHP_FUNCTION(bartlby_delete_server) {
	
	pval * bartlby_config;
	pval * server_id;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*DeleteServer)(int, char*);
	
	struct service svc;
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config,&server_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	
	convert_to_long(server_id);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(DeleteServer,SOHandle, "DeleteServer");
	
	
	
	ret=DeleteServer(Z_LVAL_P(server_id),Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	bartlby_mark_object_gone(Z_STRVAL_P(bartlby_config),Z_LVAL_P(server_id), BARTLBY_SERVER_GONE, BARTLBY_OBJECT_DELETED);

	RETURN_LONG(ret);
}
PHP_FUNCTION(bartlby_get_service_by_id) {
	pval * bartlby_config;
	
	pval * service_id;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*GetServiceById)(int,struct service *, char *);
	
	struct service  svc;
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config,&service_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	
	convert_to_long(service_id);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(GetServiceById,SOHandle, "GetServiceById");
	
	ret=GetServiceById(Z_LVAL_P(service_id),&svc, Z_STRVAL_P(bartlby_config));
	if(ret < 0) {
		RETURN_FALSE;	
	} else {
		
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_long(return_value, "service_id", svc.service_id);
		add_assoc_long(return_value, "server_id", svc.server_id);
		add_assoc_long(return_value, "last_state", svc.last_state);
		add_assoc_long(return_value, "current_state", svc.current_state);

				
		add_assoc_string(return_value, "new_server_text", svc.new_server_text, 1);
		add_assoc_string(return_value, "service_name", svc.service_name, 1);

		add_assoc_string(return_value, "plugin", svc.plugin, 1);
		add_assoc_string(return_value, "plugin_arguments", svc.plugin_arguments, 1);
		
		add_assoc_long(return_value, "check_interval", svc.check_interval);
		add_assoc_long(return_value, "check_interval_original", svc.check_interval_original);
		add_assoc_long(return_value, "last_check", svc.last_check);
		
		add_assoc_string(return_value, "server_name", svc.srv->server_name, 1);
		add_assoc_string(return_value, "client_ip", svc.srv->client_ip, 1);
		add_assoc_string(return_value, "server_icon", svc.srv->server_icon, 1);
		add_assoc_long(return_value, "client_port", svc.srv->client_port);
		
		add_assoc_string(return_value, "exec_plan", svc.service_exec_plan, 1);
		
		add_assoc_long(return_value, "notify_enabled", svc.notify_enabled);
		add_assoc_long(return_value, "last_notify_send", svc.last_notify_send);
		add_assoc_long(return_value, "flap_count", svc.flap_count);
		
		add_assoc_long(return_value, "service_active", svc.service_active);
		add_assoc_long(return_value, "service_type", svc.service_type);
		add_assoc_long(return_value, "service_passive_timeout", svc.service_passive_timeout);
		add_assoc_long(return_value, "service_check_timeout", svc.service_check_timeout);
		add_assoc_long(return_value, "service_ack", svc.service_ack);
		add_assoc_long(return_value, "service_retain", svc.service_retain);
		
		
		add_assoc_string(return_value, "service_var", svc.service_var, 1);

		
		add_assoc_string(return_value, "service_snmp_community", svc.snmp_info.community, 1);
		add_assoc_string(return_value, "service_snmp_objid", svc.snmp_info.objid, 1);
		
		add_assoc_long(return_value, "service_snmp_warning",svc.snmp_info.warn);
		add_assoc_long(return_value, "service_snmp_critical",svc.snmp_info.crit);
		add_assoc_long(return_value, "service_snmp_version",svc.snmp_info.version);
		add_assoc_long(return_value, "service_snmp_type",svc.snmp_info.type);
		
		add_assoc_long(return_value, "flap_seconds",svc.flap_seconds);
		
		
		//svc.renotify_interval, svc.escalate_divisor
		add_assoc_long(return_value, "renotify_interval",svc.renotify_interval);
		add_assoc_long(return_value, "escalate_divisor",svc.escalate_divisor);
		
		
		
			
	}
	free(svc.srv);
	dlclose(SOHandle);
}

PHP_FUNCTION(bartlby_delete_service) {
		pval * bartlby_config;
	pval * service_id;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*DeleteService)(int, char*);
	
	struct service svc;
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config, &service_id) ==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	
	convert_to_long(service_id);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(DeleteService,SOHandle, "DeleteService");
	
	
	
	ret=DeleteService(Z_LVAL_P(service_id),Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	//Mark as delted
	bartlby_mark_object_gone(Z_STRVAL_P(bartlby_config),Z_LVAL_P(service_id), BARTLBY_SERVICE_GONE, BARTLBY_OBJECT_DELETED);
	
	RETURN_LONG(ret);
	
}

PHP_FUNCTION(bartlby_modify_service) {
	struct service svc;
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	int rtc;
	
	int (*UpdateService)(struct service *, char *);
	
	pval *service_id, * server_id , * bartlby_config, * plugin, * service_name , * plugin_arguments, * notify_enabled, *exec_plan, *check_interval, *service_type, *service_passive_timeout, *service_var, *service_check_timeout, * service_ack, * service_retain;
	pval * snmp_community, * snmp_objid, *snmp_version, *snmp_warning, *snmp_critical, *snmp_type;
	pval * service_active;
	pval * flap_seconds;
	pval * renotify_interval;
	pval * escalate_divisor;
	
	
	/*
	long renotify_interval; // interval to renotify
	long escalate_divisor; 
	svc->server_id, 
	svc->plugin, 
	svc->service_name,
	svc->plugin_arguments,
	svc->notify_enabled,
	1,
	SVC_TIME_FROM,
	SVC_TIME_TO,
	svc->check_interval,
	svc->service_type,
	svc->service_var,
	svc->service_passive_timeout
	*/
	
	if(ZEND_NUM_ARGS() != 25 || getParameters(ht, 25, &bartlby_config,&service_id,  &server_id, &plugin,&service_name,&plugin_arguments,&notify_enabled,&exec_plan,&check_interval, &service_type,&service_var,&service_passive_timeout,&service_check_timeout, &service_ack, &service_retain, &snmp_community, &snmp_objid, &snmp_version, &snmp_warning, &snmp_critical, &snmp_type, &service_active, &flap_seconds, &renotify_interval, &escalate_divisor) == FAILURE) {
		WRONG_PARAM_COUNT;	
	}
	convert_to_string(bartlby_config);
	convert_to_string(plugin);
	convert_to_string(service_name);
	convert_to_string(plugin_arguments);
	convert_to_long(notify_enabled);
	convert_to_long(service_id);
	convert_to_long(server_id);
	convert_to_long(service_check_timeout);
	
	
	convert_to_long(check_interval);
	convert_to_long(service_type);
	convert_to_long(service_passive_timeout);
	convert_to_string(service_var);
	convert_to_long(service_ack);
	convert_to_long(service_retain);
	
	
	convert_to_string(snmp_community);
	convert_to_string(snmp_objid);
	convert_to_long(snmp_version);
	convert_to_long(snmp_warning);
	convert_to_long(snmp_critical);
	convert_to_long(snmp_type);
	convert_to_long(service_active);
	
	convert_to_long(flap_seconds);
	
	convert_to_long(renotify_interval);
	convert_to_long(escalate_divisor);
	
	convert_to_string(exec_plan);
	
	svc.service_id=Z_LVAL_P(service_id);
	sprintf(svc.plugin, "%s", Z_STRVAL_P(plugin));
	sprintf(svc.service_name, "%s", Z_STRVAL_P(service_name));
	sprintf(svc.plugin_arguments, "%s", Z_STRVAL_P(plugin_arguments));
	
	sprintf(svc.service_exec_plan, "%s", Z_STRVAL_P(exec_plan));
	
	svc.notify_enabled=Z_LVAL_P(notify_enabled);
	
	
	svc.server_id=Z_LVAL_P(server_id);
	svc.service_ack=Z_LVAL_P(service_ack);
	svc.service_retain=Z_LVAL_P(service_retain);
	
	svc.service_check_timeout=Z_LVAL_P(service_check_timeout);
	
	svc.check_interval=Z_LVAL_P(check_interval);
	svc.service_type=Z_LVAL_P(service_type);
	svc.service_passive_timeout=Z_LVAL_P(service_passive_timeout);
	sprintf(svc.service_var, "%s", Z_STRVAL_P(service_var));
	
	sprintf(svc.snmp_info.community, "%s", Z_STRVAL_P(snmp_community));
	sprintf(svc.snmp_info.objid, "%s", Z_STRVAL_P(snmp_objid));
	svc.snmp_info.version=Z_LVAL_P(snmp_version);
	svc.snmp_info.warn=Z_LVAL_P(snmp_warning);
	svc.snmp_info.crit=Z_LVAL_P(snmp_critical);
	svc.snmp_info.type=Z_LVAL_P(snmp_type);
	
	svc.service_active=Z_LVAL_P(service_active);
	svc.flap_seconds=Z_LVAL_P(flap_seconds);
	
	svc.renotify_interval=Z_LVAL_P(renotify_interval);
	svc.escalate_divisor=Z_LVAL_P(escalate_divisor);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(UpdateService,SOHandle, "UpdateService");
	
	rtc=UpdateService(&svc, Z_STRVAL_P(bartlby_config));
	dlclose(SOHandle);
	//Mark as changed
	bartlby_mark_object_gone(Z_STRVAL_P(bartlby_config),Z_LVAL_P(service_id), BARTLBY_SERVICE_GONE, BARTLBY_OBJECT_CHANGED);

	RETURN_LONG(rtc);
	
	
	
	
		
	
}

PHP_FUNCTION(bartlby_add_service) {
	struct service svc;
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	int rtc;
	
	int (*AddService)(struct service *, char *);
	
	pval * server_id , * bartlby_config, * plugin, * service_name , * plugin_arguments, * notify_enabled, *exec_plan, *check_interval, *service_type, *service_passive_timeout, *service_var, * service_check_timeout, * service_ack, * service_retain;
	pval * snmp_community, * snmp_objid, *snmp_version, *snmp_warning, *snmp_critical, *snmp_type;
	pval * service_active;
	pval * flap_seconds;
	pval * renotify_interval;
	pval * escalate_divisor;
	
	
	
	/*
	svc->server_id, 
	svc->plugin, 
	svc->service_name,
	svc->plugin_arguments,
	svc->notify_enabled,
	1,
	SVC_TIME_FROM,
	SVC_TIME_TO,
	svc->check_interval,
	svc->service_type,
	svc->service_var,
	svc->service_passive_timeout
	svc->service_ack
	*/
	
	if(ZEND_NUM_ARGS() != 24 || getParameters(ht, 24, &bartlby_config, &server_id, &plugin,&service_name,&plugin_arguments,&notify_enabled,&exec_plan,&check_interval, &service_type,&service_var,&service_passive_timeout, &service_check_timeout, &service_ack, &service_retain, &snmp_community, &snmp_objid, &snmp_version, &snmp_warning, &snmp_critical, &snmp_type, &service_active, &flap_seconds, &renotify_interval, &escalate_divisor) == FAILURE) {
		WRONG_PARAM_COUNT;	
	}
	convert_to_string(bartlby_config);
	convert_to_string(plugin);
	convert_to_string(service_name);
	convert_to_string(plugin_arguments);
	convert_to_long(notify_enabled);
	
	convert_to_long(check_interval);
	convert_to_long(service_type);
	convert_to_long(service_passive_timeout);
	convert_to_long(server_id);
	convert_to_long(service_check_timeout);
	convert_to_string(service_var);
	convert_to_string(exec_plan);
	convert_to_long(service_ack);
	convert_to_long(service_retain);
	convert_to_long(service_active);
	convert_to_long(flap_seconds);
	
	
	convert_to_long(escalate_divisor);
	convert_to_long(renotify_interval);
	
	convert_to_string(snmp_community);
	convert_to_string(snmp_objid);
	convert_to_long(snmp_version);
	convert_to_long(snmp_warning);
	convert_to_long(snmp_critical);
	convert_to_long(snmp_type);
	
	
	sprintf(svc.plugin, "%s", Z_STRVAL_P(plugin));
	sprintf(svc.service_exec_plan, "%s", Z_STRVAL_P(exec_plan));
	
	sprintf(svc.service_name, "%s", Z_STRVAL_P(service_name));
	sprintf(svc.plugin_arguments, "%s", Z_STRVAL_P(plugin_arguments));
	svc.notify_enabled=Z_LVAL_P(notify_enabled);
	
	svc.service_ack = Z_LVAL_P(service_ack);
	svc.service_retain=Z_LVAL_P(service_retain);
	
	svc.server_id=Z_LVAL_P(server_id);
	svc.flap_seconds=Z_LVAL_P(flap_seconds);
	
	svc.escalate_divisor=Z_LVAL_P(escalate_divisor);
	svc.renotify_interval=Z_LVAL_P(renotify_interval);
	
	
	svc.service_check_timeout=Z_LVAL_P(service_check_timeout);
	
	svc.check_interval=Z_LVAL_P(check_interval);
	svc.service_type=Z_LVAL_P(service_type);
	svc.service_passive_timeout=Z_LVAL_P(service_passive_timeout);
	sprintf(svc.service_var, "%s", Z_STRVAL_P(service_var));
	
	sprintf(svc.snmp_info.community, "%s", Z_STRVAL_P(snmp_community));
	sprintf(svc.snmp_info.objid, "%s", Z_STRVAL_P(snmp_objid));
	svc.snmp_info.version=Z_LVAL_P(snmp_version);
	svc.snmp_info.warn=Z_LVAL_P(snmp_warning);
	svc.snmp_info.crit=Z_LVAL_P(snmp_critical);
	svc.snmp_info.type=Z_LVAL_P(snmp_type);
	svc.service_active=Z_LVAL_P(service_active);
	
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(AddService,SOHandle, "AddService");
	
	rtc=AddService(&svc, Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	RETURN_LONG(rtc);
	
	
	
		
	
}
PHP_FUNCTION(bartlby_add_server) {
	
	pval * bartlby_config;
	pval * server_name;
	pval * server_ip;
	pval * server_port;
	pval * server_icon;
	pval * server_enabled;
	pval * server_flap_seconds;
	pval * server_notify;
	pval * server_dead;
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*AddServer)(struct server *,char *);
	
	struct server srv;
	
	if (ZEND_NUM_ARGS() != 9 || getParameters(ht, 9, &bartlby_config,&server_name, &server_ip, &server_port, &server_icon, &server_enabled, &server_notify, &server_flap_seconds, &server_dead)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	convert_to_string(server_name);
	convert_to_string(server_ip);
	convert_to_long(server_port);
	convert_to_string(server_icon);
	convert_to_long(server_enabled);
	convert_to_long(server_flap_seconds);
	convert_to_long(server_notify);
	convert_to_long(server_dead);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(AddServer,SOHandle, "AddServer");
	
	strcpy(srv.server_name, Z_STRVAL_P(server_name));
	srv.client_port=Z_LVAL_P(server_port);
	strcpy(srv.client_ip, Z_STRVAL_P(server_ip));
	strcpy(srv.server_icon, Z_STRVAL_P(server_icon));
	srv.server_enabled=Z_LVAL_P(server_enabled);
	srv.server_flap_seconds=Z_LVAL_P(server_flap_seconds);
	srv.server_notify=Z_LVAL_P(server_notify);
	srv.server_dead=Z_LVAL_P(server_dead);
	
	ret=AddServer(&srv, Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	RETURN_LONG(ret);
}

PHP_FUNCTION(bartlby_modify_server) {
	
	pval * bartlby_config;
	pval * server_name;
	pval * server_ip;
	pval * server_port;
	pval * server_id;
	pval * server_icon;
	pval * server_enabled;
	pval * server_flap_seconds;
	pval * server_notify;
	pval * server_dead;
	
	
	void * SOHandle;
	char * dlmsg;
	
	int ret;
	
	int (*ModifyServer)(struct server *,char *);
	
	struct server srv;
	
	if (ZEND_NUM_ARGS() != 10 || getParameters(ht, 10, &bartlby_config,&server_id, &server_name, &server_ip, &server_port, &server_icon, &server_enabled, &server_notify, &server_flap_seconds, &server_dead)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	convert_to_string(server_name);
	convert_to_string(server_ip);
	convert_to_long(server_port);
	convert_to_long(server_id);
	convert_to_string(server_icon);
	convert_to_long(server_enabled);
	convert_to_long(server_flap_seconds);
	convert_to_long(server_notify);
	convert_to_long(server_dead);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(ModifyServer,SOHandle, "ModifyServer");
	
	strcpy(srv.server_name, Z_STRVAL_P(server_name));
	srv.client_port=Z_LVAL_P(server_port);
	srv.server_id=Z_LVAL_P(server_id);
	
	strcpy(srv.client_ip, Z_STRVAL_P(server_ip));
	strcpy(srv.server_icon, Z_STRVAL_P(server_icon));
	srv.server_enabled=Z_LVAL_P(server_enabled);
	srv.server_flap_seconds=Z_LVAL_P(server_flap_seconds);
	srv.server_notify=Z_LVAL_P(server_notify);
	
	srv.server_dead=Z_LVAL_P(server_dead);
	
	
	ret=ModifyServer(&srv, Z_STRVAL_P(bartlby_config));
	
	dlclose(SOHandle);
	bartlby_mark_object_gone(Z_STRVAL_P(bartlby_config),Z_LVAL_P(server_id), BARTLBY_SERVER_GONE, BARTLBY_OBJECT_CHANGED);

	RETURN_LONG(ret);
}

PHP_FUNCTION(bartlby_get_server_by_id) {
	pval * bartlby_config;
	
	pval * server_id;
	
	char * shmtok;
	void * SOHandle;
	char * dlmsg;
	struct shm_header * shm_hdr;
	struct worker * wrkmap;
	struct downtime * dtmap;
	struct service * svcmap;
	struct server * srvmap;
	void * bartlby_address;
	int x;
	
	
	int ret;
	
	int (*GetServerById)(int,struct server *, char *);
	
	struct server  svc;
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config,&server_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(bartlby_config);
	
	convert_to_long(server_id);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	
	
	LOAD_SYMBOL(GetServerById,SOHandle, "GetServerById");
	
	ret=GetServerById(Z_LVAL_P(server_id),&svc, Z_STRVAL_P(bartlby_config));
	
	if(ret < 0) {
		RETURN_FALSE;	
	} else {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_string(return_value, "server_name", svc.server_name, 1);
		add_assoc_string(return_value, "server_ip", svc.client_ip, 1);
		add_assoc_string(return_value, "server_icon", svc.server_icon, 1);
		add_assoc_long(return_value, "server_port",svc.client_port);
		add_assoc_long(return_value, "server_id",Z_LVAL_P(server_id));
		
		
		bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config));
		if(bartlby_address != NULL) {
			shm_hdr=(struct shm_header *)(void *)bartlby_address;
			svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
			wrkmap=(struct worker *)(void*)&svcmap[shm_hdr->svccount]+20;
			dtmap=(struct downtime *)(void *)&wrkmap[shm_hdr->wrkcount]+20;
			srvmap=(struct server *)(void *)&dtmap[shm_hdr->dtcount]+20;
			
			for(x=0; x<shm_hdr->srvcount; x++) {
				if(srvmap[x].server_id == Z_LVAL_P(server_id)) {
					add_assoc_long(return_value, "server_enabled",srvmap[x].server_enabled);
					add_assoc_long(return_value, "server_notify",srvmap[x].server_notify);
					add_assoc_long(return_value, "server_flap_seconds",srvmap[x].server_flap_seconds);
					add_assoc_long(return_value, "last_notify_send",srvmap[x].last_notify_send);
					add_assoc_long(return_value, "server_dead",srvmap[x].server_dead);
					add_assoc_long(return_value, "server_shm_place",x);
					break;
				}			
			}
			
			
			
			shmdt(bartlby_address);
			
       	
		
		} else {
			php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
			free(shmtok);
			RETURN_FALSE;
		}
		
			
	}
	dlclose(SOHandle);
		
}

PHP_FUNCTION(bartlby_lib_info) {
	char * dlmsg;
	void * SOHandle;
	char * data_lib;
	pval * bartlby_config;
	
	char * (*GetAutor)();
	char * (*GetVersion)();
	char * (*GetName)();
	char * GetAutorStr;
	char * GetVersionStr;
	char * GetNameStr;
	
	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &bartlby_config)==FAILURE) {
		WRONG_PARAM_COUNT;
	}	
	convert_to_string(bartlby_config);
	
	SOHandle=bartlby_get_sohandle(Z_STRVAL_P(bartlby_config));
	if(SOHandle == NULL) {
		php_error(E_WARNING, "bartlby SO error");	
		RETURN_FALSE;	
	}
	LOAD_SYMBOL(GetAutor,SOHandle, "GetAutor");
    	LOAD_SYMBOL(GetVersion,SOHandle, "GetVersion");
    	LOAD_SYMBOL(GetName,SOHandle, "GetName");
    	
	GetAutorStr=GetAutor();
    	GetVersionStr=GetVersion();
    	GetNameStr=GetName();
    	
    	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_assoc_string(return_value, "Autor", GetAutorStr, 1);
	add_assoc_string(return_value, "Version", GetVersionStr, 1);
	add_assoc_string(return_value, "Name", GetNameStr, 1);
	
	free(GetAutorStr);
	free(GetVersionStr);
	free(GetNameStr);
	dlclose(SOHandle);
    	
}

PHP_FUNCTION(bartlby_get_service) {
	
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	int current_time;
	int is_down;
	int y;
	
	struct service * svcmap;
	struct worker * wrkmap;
	struct downtime * dtmap;
	struct server * srvmap;
	
	pval * bartlby_config;
	pval * bartlby_service_id;
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config, &bartlby_service_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}	
	convert_to_long(bartlby_service_id);
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
		
	
	
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config)); 
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		wrkmap=(struct worker *)(void*)&svcmap[shm_hdr->svccount]+20;
		dtmap=(struct downtime *)(void*)&wrkmap[shm_hdr->wrkcount]+20;
		srvmap=(struct server *)(void*)&dtmap[shm_hdr->dtcount]+20;
		
		if(Z_LVAL_P(bartlby_service_id) > shm_hdr->svccount-1) {
			php_error(E_WARNING, "Service id out of bounds");	
			RETURN_FALSE;	
		}
		add_assoc_long(return_value, "service_id", svcmap[Z_LVAL_P(bartlby_service_id)].service_id);
		add_assoc_long(return_value, "server_id", svcmap[Z_LVAL_P(bartlby_service_id)].server_id);
		add_assoc_long(return_value, "last_state", svcmap[Z_LVAL_P(bartlby_service_id)].last_state);
		
		
		if(svcmap[Z_LVAL_P(bartlby_service_id)].is_server_dead < 0) {
			add_assoc_long(return_value, "current_state", -3);
			add_assoc_long(return_value, "server_is_dead", 1);
		} else {
			add_assoc_long(return_value, "current_state", svcmap[Z_LVAL_P(bartlby_service_id)].current_state);
		}
		
		
		
		add_assoc_long(return_value, "client_port", srvmap[svcmap[Z_LVAL_P(bartlby_service_id)].srv_place].client_port);
				
		add_assoc_string(return_value, "new_server_text", svcmap[Z_LVAL_P(bartlby_service_id)].new_server_text, 1);
		add_assoc_string(return_value, "service_name", svcmap[Z_LVAL_P(bartlby_service_id)].service_name, 1);
		add_assoc_string(return_value, "server_name", srvmap[svcmap[Z_LVAL_P(bartlby_service_id)].srv_place].server_name, 1);
		add_assoc_string(return_value, "client_ip", srvmap[svcmap[Z_LVAL_P(bartlby_service_id)].srv_place].client_ip, 1);
		add_assoc_string(return_value, "plugin", svcmap[Z_LVAL_P(bartlby_service_id)].plugin, 1);
		add_assoc_string(return_value, "plugin_arguments", svcmap[Z_LVAL_P(bartlby_service_id)].plugin_arguments, 1);
		
		add_assoc_long(return_value, "check_interval", svcmap[Z_LVAL_P(bartlby_service_id)].check_interval);
		add_assoc_long(return_value, "check_interval_original", svcmap[Z_LVAL_P(bartlby_service_id)].check_interval_original);
		add_assoc_long(return_value, "last_check", svcmap[Z_LVAL_P(bartlby_service_id)].last_check);
		
		
		add_assoc_string(return_value, "exec_plan", svcmap[Z_LVAL_P(bartlby_service_id)].service_exec_plan, 1);
		
		add_assoc_long(return_value, "notify_enabled", svcmap[Z_LVAL_P(bartlby_service_id)].notify_enabled);
		add_assoc_long(return_value, "last_notify_send", svcmap[Z_LVAL_P(bartlby_service_id)].last_notify_send);
		add_assoc_long(return_value, "flap_count", svcmap[Z_LVAL_P(bartlby_service_id)].flap_count);
		
		
		add_assoc_long(return_value, "service_active", svcmap[Z_LVAL_P(bartlby_service_id)].service_active);
		add_assoc_long(return_value, "service_type", svcmap[Z_LVAL_P(bartlby_service_id)].service_type);
		add_assoc_long(return_value, "service_passive_timeout", svcmap[Z_LVAL_P(bartlby_service_id)].service_passive_timeout);
		
		
		add_assoc_string(return_value, "service_var", svcmap[Z_LVAL_P(bartlby_service_id)].service_var, 1);
		
		add_assoc_long(return_value, "service_check_timeout", svcmap[Z_LVAL_P(bartlby_service_id)].service_check_timeout);
		add_assoc_long(return_value, "service_ack", svcmap[Z_LVAL_P(bartlby_service_id)].service_ack);
		
		add_assoc_long(return_value, "service_retain", svcmap[Z_LVAL_P(bartlby_service_id)].service_retain);
		add_assoc_long(return_value, "service_retain_current", svcmap[Z_LVAL_P(bartlby_service_id)].service_retain_current);
		add_assoc_long(return_value, "check_is_running", svcmap[Z_LVAL_P(bartlby_service_id)].process.pid);
		add_assoc_long(return_value, "check_starttime", svcmap[Z_LVAL_P(bartlby_service_id)].process.start_time);
		
		
		add_assoc_long(return_value, "service_time_sum", svcmap[Z_LVAL_P(bartlby_service_id)].pstat.sum);
		add_assoc_long(return_value, "service_time_count",svcmap[Z_LVAL_P(bartlby_service_id)].pstat.counter);
		
		add_assoc_long(return_value, "service_delay_sum", svcmap[Z_LVAL_P(bartlby_service_id)].delay_time.sum);
		add_assoc_long(return_value, "service_delay_count",svcmap[Z_LVAL_P(bartlby_service_id)].delay_time.counter);
		
		
		add_assoc_string(return_value, "service_var",  svcmap[Z_LVAL_P(bartlby_service_id)].service_var, 1);
		add_assoc_string(return_value, "server_icon",  srvmap[svcmap[Z_LVAL_P(bartlby_service_id)].srv_place].server_icon, 1);
		
		add_assoc_string(return_value, "service_snmp_community",  svcmap[Z_LVAL_P(bartlby_service_id)].snmp_info.community, 1);
		add_assoc_string(return_value, "service_snmp_objid",  svcmap[Z_LVAL_P(bartlby_service_id)].snmp_info.objid, 1);
		
		add_assoc_long(return_value, "service_snmp_warning", svcmap[Z_LVAL_P(bartlby_service_id)].snmp_info.warn);
		add_assoc_long(return_value, "service_snmp_critical", svcmap[Z_LVAL_P(bartlby_service_id)].snmp_info.crit);
		add_assoc_long(return_value, "service_snmp_version", svcmap[Z_LVAL_P(bartlby_service_id)].snmp_info.version);
		add_assoc_long(return_value, "service_snmp_type", svcmap[Z_LVAL_P(bartlby_service_id)].snmp_info.type);
		
		add_assoc_long(return_value, "flap_seconds", svcmap[Z_LVAL_P(bartlby_service_id)].flap_seconds);
		
		add_assoc_long(return_value, "server_flap_seconds",srvmap[svcmap[Z_LVAL_P(bartlby_service_id)].srv_place].server_flap_seconds);
		add_assoc_long(return_value, "server_last_notify_send",srvmap[svcmap[Z_LVAL_P(bartlby_service_id)].srv_place].last_notify_send);
		add_assoc_long(return_value, "server_notify",srvmap[svcmap[Z_LVAL_P(bartlby_service_id)].srv_place].server_notify);
		add_assoc_long(return_value, "server_enabled",srvmap[svcmap[Z_LVAL_P(bartlby_service_id)].srv_place].server_enabled);
		add_assoc_long(return_value, "server_dead",srvmap[svcmap[Z_LVAL_P(bartlby_service_id)].srv_place].server_dead);
			
		add_assoc_long(return_value, "renotify_interval",svcmap[Z_LVAL_P(bartlby_service_id)].renotify_interval);
		add_assoc_long(return_value, "escalate_divisor",svcmap[Z_LVAL_P(bartlby_service_id)].escalate_divisor);
		add_assoc_long(return_value, "is_gone",svcmap[Z_LVAL_P(bartlby_service_id)].is_gone);
			
		
			
		
		//Downtime 060120
		is_down=0;
		current_time=time(NULL);
		for(y=0; y<shm_hdr->dtcount; y++) {
				is_down=0;
				if(current_time >= dtmap[y].downtime_from && current_time <= dtmap[y].downtime_to) {
					switch(dtmap[y].downtime_type) {
						case DT_SERVICE:
							if(dtmap[y].service_id == svcmap[Z_LVAL_P(bartlby_service_id)].service_id) {
								is_down=1;	
								
							}
						break;
						case DT_SERVER:
							if(dtmap[y].service_id == svcmap[Z_LVAL_P(bartlby_service_id)].server_id) {
								is_down=2;	
							}
						break;				
					}
					if(is_down > 0) {
						add_assoc_long(return_value, "is_downtime", 1);
						add_assoc_long(return_value, "downtime_from", dtmap[y].downtime_from);
						add_assoc_long(return_value, "downtime_to", dtmap[y].downtime_to);
						add_assoc_string(return_value, "downtime_notice", dtmap[y].downtime_notice, 1);
						add_assoc_long(return_value, "downtime_service", dtmap[y].service_id);
						add_assoc_long(return_value, "downtime_type", dtmap[y].downtime_type);
						break;
					}
				}	
		}
		if(is_down==0) {
			add_assoc_long(return_value, "is_downtime", 0);	
		}
		
		
		
		shmdt(bartlby_address);
	/*	
		int service_id;
	int server_id;
	int last_state;
	int current_state;
	int client_port;
	char  new_server_text[2048];
	char  service_name[2048];
	char  server_name[2048];
	char  client_ip[2048];
	char  plugin[2048];
	char  plugin_arguments[2048];
	int check_interval;
	int last_check;
	
	
	
	int hour_from;
	int min_from;
	int hour_to;
	int min_to;
	
	
	int notify_enabled;
	int last_notify_send;
	int flap_count;
	
	char  service_var[2048];
	int service_type;
	int service_passive_timeout;
	*/
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}
	
	
}

PHP_FUNCTION(bartlby_get_worker) {
	char * shmtok;
	int shm_id;
	void * bartlby_address;
	struct shm_header * shm_hdr;
	
	struct worker * wrkmap;
	struct service * svcmap;
	
	pval * bartlby_config;
	pval * bartlby_worker_id;
	
	if (ZEND_NUM_ARGS() != 2 || getParameters(ht, 2, &bartlby_config, &bartlby_worker_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}	
	convert_to_long(bartlby_worker_id);
	convert_to_string(bartlby_config);
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	bartlby_address=bartlby_get_shm(Z_STRVAL_P(bartlby_config));
	if(bartlby_address != NULL) {
		shm_hdr=(struct shm_header *)(void *)bartlby_address;
		svcmap=(struct service *)(void *)bartlby_address+sizeof(struct shm_header);
		wrkmap=(struct worker *)(void*)&svcmap[shm_hdr->svccount]+20;
		
		if(Z_LVAL_P(bartlby_worker_id) > shm_hdr->wrkcount-1) {
			php_error(E_WARNING, "Worker id out of bounds");	
			RETURN_FALSE;	
		}
		
		
		add_assoc_string(return_value, "mail", wrkmap[Z_LVAL_P(bartlby_worker_id)].mail, 1);
		add_assoc_string(return_value, "icq", wrkmap[Z_LVAL_P(bartlby_worker_id)].icq, 1);
		add_assoc_string(return_value, "notify_plan", wrkmap[Z_LVAL_P(bartlby_worker_id)].notify_plan, 1);
		add_assoc_string(return_value, "services", wrkmap[Z_LVAL_P(bartlby_worker_id)].services, 1);
		
		add_assoc_string(return_value, "notify_levels", wrkmap[Z_LVAL_P(bartlby_worker_id)].notify_levels,1);
		add_assoc_string(return_value, "name", wrkmap[Z_LVAL_P(bartlby_worker_id)].name,1);
		add_assoc_string(return_value, "password", wrkmap[Z_LVAL_P(bartlby_worker_id)].password,1);
		add_assoc_long(return_value, "worker_id", wrkmap[Z_LVAL_P(bartlby_worker_id)].worker_id);
		
		add_assoc_long(return_value, "escalation_limit", wrkmap[Z_LVAL_P(bartlby_worker_id)].escalation_limit);
		add_assoc_long(return_value, "escalation_minutes", wrkmap[Z_LVAL_P(bartlby_worker_id)].escalation_minutes);
		
		add_assoc_string(return_value, "enabled_triggers", wrkmap[Z_LVAL_P(bartlby_worker_id)].enabled_triggers,1);
		add_assoc_long(return_value, "active", wrkmap[Z_LVAL_P(bartlby_worker_id)].active);
		add_assoc_long(return_value, "is_gone", wrkmap[Z_LVAL_P(bartlby_worker_id)].is_gone);		

		shmdt(bartlby_address);
		
	/*	
	char  mail[2048];
	char  icq[2048];
	char  services[2048];
	
	
	int icq_notify;
	int mail_notify;
	int escalation_count;
	int escalation_time;
	char t[500];
	*/
	
	} else {
		php_error(E_WARNING, "SHM segment is not existing (bartlby running?)");	
		free(shmtok);
		RETURN_FALSE;
	}
	
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
