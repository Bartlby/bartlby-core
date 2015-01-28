/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2013 Helmut Januschka - All Rights Reserved
 *   Contact: <helmut@januschka.com>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
  *   support: via http://github.com/Bartlby
 * ----------------------------------------------------------------------- 
*/


#include <bartlby.h>

#include "bartlby_mysql.h"



 #define SERVICE_MAP_SELECTOR "select \
                                service_id, \
                                service_name, \
                                service_state, \
                                service_plugin, \
                                service_args, \
                                UNIX_TIMESTAMP(service_last_check), \
                                service_interval, \
                                service_text, \
                                service_notify, \
                                service_type, \
                                service_var, \
                                service_passive_timeout, \
                                service_active, \
                                service_check_timeout, \
                                service_ack_enabled, \
                                service_retain, \
                                service_snmp_community, \
                                service_snmp_objid, \
                                service_snmp_version, \
                                service_snmp_warning, \
                                service_snmp_critical, \
                                service_snmp_type, \
                                flap_seconds, \
                                service_exec_plan, \
                                renotify_interval, \
                                escalate_divisor, \
                                fires_events, \
                                enabled_triggers, \
                                service_snmp_textmatch, \
                                UNIX_TIMESTAMP(service_last_notify_send), \
                                UNIX_TIMESTAMP(service_last_state_change), \
                                service_retain_current, \
                                service_ack_current, \
                                server_id, \
                                service_handled, \
                                orch_id, \
                                usid, \
                                prio, \
                                notify_super_users \
                            from services svc %s  \
                            ORDER BY RAND()"





#define SERVICE_UPDATE_TEXT "update services \
                            set \
                              service_last_check=FROM_UNIXTIME(%d), \
                              service_text='%s', \
                              service_state=%d, \
                              service_last_notify_send=FROM_UNIXTIME(%d), \
                              service_last_state_change=FROM_UNIXTIME(%d), \
                              service_ack_current=%d, \
                              service_retain_current=%ld, \
                              service_handled=%d \
                            where \
                              service_id=%ld"






#define ADD_SERVICE "insert into services \
                              ( \
                                service_last_check, \
                                server_id, \
                                service_plugin, \
                                service_name, \
                                service_state, \
                                service_text, \
                                service_args, \
                                service_notify, \
                                service_active, \
                                service_interval, \
                                service_type, \
                                service_var, \
                                service_passive_timeout, \
                                service_check_timeout, \
                                service_ack_enabled, \
                                service_retain, \
                                service_snmp_community, \
                                service_snmp_objid, \
                                service_snmp_version, \
                                service_snmp_warning, \
                                service_snmp_critical, \
                                service_snmp_type, \
                                flap_seconds, \
                                service_exec_plan, \
                                renotify_interval, \
                                escalate_divisor, \
                                fires_events, \
                                enabled_triggers, \
                                service_snmp_textmatch, \
                                orch_id, \
                                usid, \
                                prio, \
                                notify_super_users \
                              )  \
                        values( \
                          NOW(), \
                          %ld, \
                          '%s', \
                          '%s', \
                          4, \
                          'Newly created', \
                          '%s', \
                          %d, \
                          %d, \
                          %ld, \
                          %d, \
                          '%s', \
                          %ld, \
                          %ld, \
                          %d, \
                          %ld, \
                          '%s', \
                          '%s', \
                          %d, \
                          %ld, \
                          %ld, \
                          %ld, \
                          %ld, \
                          '%s', \
                          %ld, \
                          %ld, \
                          %ld, \
                          '%s', \
                          '%s', \
                          '%d', \
                          '%s', \
                          %d, \
                          %d \
                    )"


#define DELETE_SERVICE "delete from services where service_id=%ld"
#define SERVICE_CHANGE_ID "update services set service_id=%ld where service_id=%ld"

#define UPDATE_SERVICE "update \
                          services  \
                        set  \
                          service_type=%d, \
                          service_name='%s', \
                          server_id=%ld, \
                          service_interval = %ld, \
                          service_plugin='%s', \
                          service_args='%s', \
                          service_passive_timeout=%ld, \
                          service_var='%s', \
                          service_check_timeout=%ld, \
                          service_ack_enabled='%d', \
                          service_retain='%ld', \
                          service_snmp_community='%s', \
                          service_snmp_objid='%s', \
                          service_snmp_version='%d', \
                          service_snmp_warning='%ld', \
                          service_snmp_critical='%ld', \
                          service_snmp_type='%ld', \
                          service_notify='%d', \
                          service_active='%d', \
                          flap_seconds='%ld', \
                          service_exec_plan='%s ', \
                          renotify_interval=%ld, \
                          escalate_divisor=%ld, \
                          fires_events=%ld, \
                          enabled_triggers='%s', \
                          service_snmp_textmatch='%s', \
                          service_handled=%d, \
                          orch_id=%d,   \
                          usid='%s', \
                          prio=%d, \
                          notify_super_users=%d \
                      where \
                        service_id=%ld"

#define SERVICE_SELECTOR "select \
                            service_id, \
                            service_name, \
                            service_state, \
                            service_plugin, \
                            service_args, \
                            UNIX_TIMESTAMP(service_last_check), \
                            service_interval, \
                            service_text, \
                            service_notify, \
                            service_type, \
                            service_var, \
                            service_passive_timeout, \
                            service_active, \
                            service_check_timeout, \
                            service_ack_enabled, \
                            service_retain, \
                            service_snmp_community, \
                            service_snmp_objid, \
                            service_snmp_version, \
                            service_snmp_warning, \
                            service_snmp_critical, \
                            service_snmp_type, \
                            flap_seconds, \
                            service_exec_plan, \
                            renotify_interval, \
                            escalate_divisor, \
                            fires_events, \
                            enabled_triggers, \
                            service_snmp_textmatch, \
                            UNIX_TIMESTAMP(service_last_notify_send), \
                            UNIX_TIMESTAMP(service_last_state_change), \
                            service_retain_current, \
                            service_ack_current, \
                            server_id, \
                            service_handled, \
                            orch_id,   \
                            usid, \
                            prio, \
                            notify_super_users \
                        from  \
                            services svc  \
                        where service_id=%ld"


#define UPDATE_SERVICE_INTERVAL "update services set service_interval=%ld where service_id=%ld"


long ServiceChangeId(long from, long to, char * config) {
	MYSQL *mysql;
	
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	
	CHECKED_ASPRINTF(&sqlupd, SERVICE_CHANGE_ID, to, from);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
			
	free(sqlupd);
		
	
	BARTLBY_MYSQL_CLOSE(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return to;	
}

int GetServiceById(long service_id, struct service * svc, char * config) {
	
	
	
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	char * sqlupd;
	struct server * n_srv;
	
	int tmprc;
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	
	
	
	
	
	
	mysql=mysql_init(NULL);
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	CHECKED_ASPRINTF(&sqlupd, SERVICE_SELECTOR, service_id);
	

	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql,NULL);
      	
      	
      	if(mysql_num_rows(res) == 1 ) {
      		row=mysql_fetch_row(res);
      		svc->service_id=atol(row[0]);
      		svc->server_id=atol(row[33]);
      		svc->handled=atoi(row[34]);
      		svc->orch_id=atoi(row[35]);
          
          if(row[36] != NULL) {
            sprintf(svc->usid, "%s", row[36]);
          } else {
            sprintf(svc->usid, "%ld", svc->service_id);
          }
          
          svc->prio=atoi(row[37]);
          svc->notify_super_users=atoi(row[38]);


      		svc->last_state=atoi(row[2]);
      		svc->current_state=atoi(row[2]);
      		
      		
      		n_srv=malloc(sizeof(struct server));
      		GetServerById(svc->server_id, n_srv, config);
      		svc->srv=n_srv;
      		
      		if(row[1] != NULL) {
      			//svc->service_name=malloc(strlen(row[1])*sizeof(char)+2);
      			sprintf(svc->service_name, "%s", row[1]);
      			
      		} else {
      			//svc->service_name=NULL;     				
      			sprintf(svc->service_name, "(null)");
      		}
      		
      		
      		
      		if(row[27] != NULL) {
      			snprintf(svc->enabled_triggers, 512, "%s", row[27]);
      		} else {
      			sprintf(svc->enabled_triggers, "%s", "");	
      		}
      		
      		
      		if(row[28] != NULL) {
      			snprintf(svc->snmp_info.textmatch, 1024, "%s", row[28]);
      		} else {
      			sprintf(svc->snmp_info.textmatch, "%s", "");	
      		}
					
      		svc->last_notify_send=atoi(row[29]);
      		svc->last_state_change=atoi(row[30]);
      		
      		svc->service_retain_current=atol(row[31]);
      		svc->service_ack_current=atoi(row[32]);
      		
      		if(svc->last_notify_send == 0) svc->last_notify_send = time(NULL);
      		if(svc->last_state_change == 0) svc->last_state_change = time(NULL);
      		
      		
      		
      		
      		sprintf(svc->current_output, "%s", row[7]);
      		
      		
      		
      		
      		if(row[3] != NULL) {
      			
      			sprintf(svc->plugin, "%s", row[3]);
      			
      		} else {
      			
      			sprintf(svc->plugin, "(null)");       				
      		}
      		
      		if(row[4] != NULL) {
      			//svc->plugin_arguments=malloc(strlen(row[8])*sizeof(char)+2);
      			sprintf(svc->plugin_arguments, "%s", row[4]);
      			
      		} else {
      			//svc->plugin_arguments=NULL; 
      			sprintf(svc->plugin_arguments, "(null)");      				
      		}
      		
      		svc->lcheck.tv_usec=0;
      		svc->lcheck.tv_sec=atoi(row[5]);
      		svc->last_check=atoi(row[5]);
      		
      		svc->check_interval=atol(row[6]);
      		
      		svc->check_interval_original=atol(row[6])*1000;
      		      		
      		
      	
      		
      		svc->notify_enabled=atoi(row[8]);
      	
      		svc->service_type = atoi(row[9]);
      		
      		if(row[10] != NULL) {
      			//svc->service_var=malloc(strlen(row[18])*sizeof(char)+2);
      			sprintf(svc->service_var, "%s", row[10]);
      			
      		} else {
      			//svc->service_var=NULL;
      			sprintf(svc->service_var, "(null)");
      		}
      		
      		svc->service_passive_timeout=atol(row[11]);
      		
      		svc->service_active=atoi(row[12]);
      		svc->service_check_timeout=atol(row[13]);
      		if(row[14] != NULL) {
      			svc->service_ack_enabled = atoi(row[14]);	
      		} 
      		svc->service_retain=atol(row[15]);
      		svc->flap_count=0;
      		
      		if(row[16] != NULL) {
      			snprintf(svc->snmp_info.community, 500, "%s", row[16]);
      		} else {
      			sprintf(svc->snmp_info.community, "(null)");
      		}
      		
      		if(row[17] != NULL) {
      			snprintf(svc->snmp_info.objid, 1000, "%s", row[17]);
      		} else {
      			sprintf(svc->snmp_info.objid, "(null)");	
      		}
      		
      		svc->snmp_info.version = atoi(row[18]);
      		svc->snmp_info.warn = atol(row[19]);
      		svc->snmp_info.crit = atol(row[20]);
      		svc->snmp_info.type = atol(row[21]);
      		
      		svc->flap_seconds = atol(row[22]);
      		
      		if(row[23] != NULL) {
      			snprintf(svc->service_exec_plan, 2047, "%s", row[23]);
      		} else {
      			sprintf(svc->service_exec_plan, " ");	
      		}
      		//svc.renotify_interval, svc.escalate_divisor
      		
      		svc->renotify_interval=atol(row[24]);
      		svc->escalate_divisor=atol(row[25]);
      		svc->fires_events=atol(row[26]);
				
					
					if(svc->service_ack_enabled == 0) svc->service_ack_current=0;
	
    		svc->is_gone = 0;  		
      		tmprc=0;
      	} else {
		tmprc=-1;
	}
	
	
	mysql_free_result(res);
      	BARTLBY_MYSQL_CLOSE(mysql);
      	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	free(sqlupd);
	
	
	return tmprc;	
}
int UpdateServiceInterval(struct service * svc, char * config) {
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	
		
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
      		
  
  CHECKED_ASPRINTF(&sqlupd, UPDATE_SERVICE_INTERVAL, 	svc->check_interval, 	svc->service_id);
	
	//Log("dbg", sqlupd);

	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
	
	
	free(sqlupd);
	rtc=1;
  BARTLBY_MYSQL_CLOSE(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	
	return rtc;	
	
  
}
int UpdateService(struct service * svc, char *config) {
	/*
		We get a struct service
		fully filled :-)
		update it it ;-)
		and return svc->service_id
	*/
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	
		
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	/*
	server_id %d
	service_plugin %s
	service_name %s
	service_state 4
	service_text 'Newly"
	service_args %s
	service_notify %d
	service_active %d
	service_time_from %s
	service_time_to  %s
	service_intervall %d
	service_type %d
	service_var %s
	service_passive_timeout %d
	
	 */
	 
	CHECKED_ASPRINTF(&sqlupd, UPDATE_SERVICE, 
                                	svc->service_type, 
                                	BARTLBY_SQL_PROTECTION(svc->service_name), 
                                	svc->server_id,
                                	svc->check_interval,
                                	BARTLBY_SQL_PROTECTION(svc->plugin),
                                	BARTLBY_SQL_PROTECTION(svc->plugin_arguments),
                                	svc->service_passive_timeout,
                                	BARTLBY_SQL_PROTECTION(svc->service_var),
                                	svc->service_check_timeout,
                                	svc->service_ack_enabled,
                                	svc->service_retain,
                                	BARTLBY_SQL_PROTECTION(svc->snmp_info.community),
                                	BARTLBY_SQL_PROTECTION(svc->snmp_info.objid),
                                	svc->snmp_info.version,
                                	svc->snmp_info.warn,
                                	svc->snmp_info.crit,
                                	svc->snmp_info.type,
                                	svc->notify_enabled,
                                	svc->service_active,	
                                	svc->flap_seconds,
                                	BARTLBY_SQL_PROTECTION(svc->service_exec_plan),
                                	svc->renotify_interval,
                                  svc->escalate_divisor,
                                  svc->fires_events,
                                  BARTLBY_SQL_PROTECTION(svc->enabled_triggers),
                                  BARTLBY_SQL_PROTECTION(svc->snmp_info.textmatch),
                                  svc->handled,
                                  svc->orch_id,
                                  BARTLBY_SQL_PROTECTION(svc->usid),
                                  svc->prio,
                                  svc->notify_super_users,
                                	svc->service_id
                                	
                                	
                                	);
	
	//Log("dbg", sqlupd);

	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
	
	
	free(sqlupd);
	rtc=1;
	BARTLBY_MYSQL_CLOSE(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	
	return rtc;	
}

int DeleteService(long service_id, char * config) {
	/*
		we get a svc->server_id
		KICK it (not like beckham)
		
	*/
	MYSQL *mysql;

	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	
	CHECKED_ASPRINTF(&sqlupd, DELETE_SERVICE, service_id);
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
	
	
	free(sqlupd);
	
	BARTLBY_MYSQL_CLOSE(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return 1;		
	
	
}


long AddService(struct service * svc, char *config) {
	/*
		We get a struct service
		fully filled :-)
		store it ;-)
		and return svc->service_id
	*/
	MYSQL *mysql;
	long rtc;
	
	char * sqlupd;
	
	
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	
	
	
	mysql=mysql_init(NULL);
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	/*
	server_id %d
	service_plugin %s
	service_name %s
	service_state 4
	service_text 'Newly"
	service_args %s
	service_notify %d
	service_active %d
	service_time_from %s
	service_time_to  %s
	service_intervall %d
	service_type %d
	service_var %s
	service_passive_timeout %d
	snmp_info 
		char community[512];
		int version;
		char objid[1024];
		int warn;
		int crit;
		int type;
		
	 */
	 
	
	
	
	CHECKED_ASPRINTF(&sqlupd, ADD_SERVICE, 
                            	svc->server_id, 
                            	BARTLBY_SQL_PROTECTION(svc->plugin), 
                            	BARTLBY_SQL_PROTECTION(svc->service_name),
                            	BARTLBY_SQL_PROTECTION(svc->plugin_arguments),
                            	svc->notify_enabled,
                            	svc->service_active,
                            	svc->check_interval,
                            	svc->service_type,
                            	BARTLBY_SQL_PROTECTION(svc->service_var),
                            	svc->service_passive_timeout,
                            	svc->service_check_timeout,
                            	svc->service_ack_enabled,
                            	svc->service_retain,
                            	BARTLBY_SQL_PROTECTION(svc->snmp_info.community),
                            	BARTLBY_SQL_PROTECTION(svc->snmp_info.objid),
                            	svc->snmp_info.version,
                            	svc->snmp_info.warn,
                            	svc->snmp_info.crit,
                            	svc->snmp_info.type,
                            	svc->flap_seconds,
                            	BARTLBY_SQL_PROTECTION(svc->service_exec_plan),
                            	svc->renotify_interval,
                              svc->escalate_divisor,
                              svc->fires_events,
                              BARTLBY_SQL_PROTECTION(svc->enabled_triggers),
                              BARTLBY_SQL_PROTECTION(svc->snmp_info.textmatch),
                              svc->orch_id,
                              BARTLBY_SQL_PROTECTION(svc->usid),
                              svc->prio,
                              svc->notify_super_users
                            	);
                            	
	//Log("dbg", sqlupd);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
	
	
	free(sqlupd);
	rtc=mysql_insert_id(mysql);
	
	BARTLBY_MYSQL_CLOSE(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	
	return rtc;	
}


int doUpdate(struct service * svc, char * config) {

	MYSQL *mysql;
	
	char * sqlupd;
	
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	
	
	
	
	CHECKED_ASPRINTF(&sqlupd, SERVICE_UPDATE_TEXT,
                                        svc->last_check,
                                        BARTLBY_SQL_PROTECTION(svc->current_output),
                                        svc->current_state,
                                        svc->last_notify_send,
                                        svc->last_state_change,
                                        svc->service_ack_current,
                                        svc->service_retain_current,
                                        svc->handled,
                                        svc->service_id
                                        );
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
	
	
	free(sqlupd);
		
	BARTLBY_MYSQL_CLOSE(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return 1;
}


int GetServiceMap(struct service * svcs, char * config, int orch_id) {
	
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	int i=0;

	char * sql, *where;


	set_cfg(config);
	mysql=mysql_init(NULL);
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
      		

      	if(orch_id > 0) {
      		CHECKED_ASPRINTF(&where, " where orch_id=%d", orch_id);
      	} else {
      		CHECKED_ASPRINTF(&where, " ");
      	}
      	CHECKED_ASPRINTF(&sql, SERVICE_MAP_SELECTOR, where);

      	mysql_query(mysql, sql);
		CHK_ERR(mysql,NULL);

		
		free(where);
		free(sql);

      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql,NULL);
      		
      		
      		
	if(mysql_num_rows(res) > 0) {
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {
      			
      			svcs[i].service_id=atol(row[0]);
      			svcs[i].server_id=atol(row[33]);
      			svcs[i].handled=atoi(row[34]);
      			svcs[i].orch_id=atoi(row[35]);
            
            if(row[36] != NULL) {
              sprintf(svcs[i].usid, "%s", row[36]);
            } else {
              sprintf(svcs[i].usid, "%ld", svcs[i].service_id);              
            }


            svcs[i].prio=atoi(row[37]);
            svcs[i].notify_super_users=atoi(row[38]);
            
      			svcs[i].last_state=atoi(row[2]);
      			svcs[i].current_state=atoi(row[2]);
      			svcs[i].servicegroup_counter=0;
      			
      			if(row[1] != NULL) {
      				sprintf(svcs[i].service_name, "%s", row[1]);
      				
      			} else {
      				sprintf(svcs[i].service_name, "(null)");
      			}
      			sprintf(svcs[i].current_output, "%s", row[7]);
      			if(row[3] != NULL) {
      				sprintf(svcs[i].plugin, "%s", row[3]);
      				
      			} else {
      				sprintf(svcs[i].plugin, "(null)");       				
      			}
      			if(row[4] != NULL) {
      				sprintf(svcs[i].plugin_arguments, "%s", row[4]);
      			} else {
      				sprintf(svcs[i].plugin_arguments, "(null)");      				
      			}
      			svcs[i].lcheck.tv_usec=0;
      			svcs[i].lcheck.tv_sec=atoi(row[5]);                      
						svcs[i].last_check=atoi(row[5]);    				                            
      			svcs[i].check_interval=atol(row[6]);
      			svcs[i].check_interval_original=atol(row[6])*1000;
      			svcs[i].notify_enabled=atoi(row[8]);
      			svcs[i].last_notify_send=atoi(row[29]);
      			svcs[i].last_state_change=atoi(row[30]);
      			if(svcs[i].last_notify_send == 0) svcs[i].last_notify_send=time(NULL);
      			if(svcs[i].last_state_change == 0) svcs[i].last_state_change=time(NULL);
      			svcs[i].service_type = atoi(row[9]);
      			if(row[10] != NULL) {
      				sprintf(svcs[i].service_var, "%s", row[10]);
      			} else {
      				sprintf(svcs[i].service_var, "(null)");
      			}
      			svcs[i].service_passive_timeout=atol(row[11]);
      			svcs[i].service_active=atoi(row[12]);
      			svcs[i].service_check_timeout=atol(row[13]);
      			if(row[14] != NULL) {
      				svcs[i].service_ack_enabled = atoi(row[14]);
      			}
      			svcs[i].service_retain=atol(row[15]);
      			svcs[i].service_retain_current=atol(row[31]);
      			svcs[i].service_ack_current=atoi(row[32]);
      			svcs[i].flap_count=0;
      			svcs[i].process.pid = 0;
      			svcs[i].process.start_time = 0;
      			svcs[i].notify_last_state=svcs[i].current_state;
      			svcs[i].pstat.sum=0;
      			svcs[i].pstat.counter=0;
      			svcs[i].delay_time.sum=0;
      			svcs[i].delay_time.counter=0;
      			svcs[i].do_force=0;
      			svcs[i].recovery_outstanding=RECOVERY_DONE;
      			if(svcs[i].current_state == STATE_CRITICAL) {
      				svcs[i].recovery_outstanding=RECOVERY_OUTSTANDING;
      			}
      			if(row[16] != NULL) {
      				snprintf(svcs[i].snmp_info.community, 500, "%s", row[16]);
      			} else {
      				sprintf(svcs[i].snmp_info.community, "(null)");
      			}
      			if(row[17] != NULL) {
      				snprintf(svcs[i].snmp_info.objid, 1000, "%s", row[17]);
      			} else {
      				sprintf(svcs[i].snmp_info.objid, "(null)");	
      			}
      			svcs[i].snmp_info.version = atoi(row[18]);
      			svcs[i].snmp_info.warn = atol(row[19]);
      			svcs[i].snmp_info.crit = atol(row[20]);
      			svcs[i].snmp_info.type = atol(row[21]);
      			svcs[i].flap_seconds=atol(row[22]);
      			if(row[23] != NULL) {
      				snprintf(svcs[i].service_exec_plan, 2047, "%s", row[23]);
      			} else {
      				sprintf(svcs[i].service_exec_plan, "%s", "");	
      			}
      			svcs[i].renotify_interval=atol(row[24]);
      			svcs[i].escalate_divisor=atol(row[25]);
      			svcs[i].fires_events=atol(row[26]);
						svcs[i].is_gone = 0;	
						if(row[27] != NULL) {
      				snprintf(svcs[i].enabled_triggers, 512, "%s", row[27]);
      			} else {
      				sprintf(svcs[i].enabled_triggers, "%s", "");	
      			}
      			if(row[28] != NULL) {
      				snprintf(svcs[i].snmp_info.textmatch, 1024, "%s", row[28]);
      			} else {
      				sprintf(svcs[i].snmp_info.textmatch, "%s", "");	
      			}
      			
      			if(svcs[i].service_ack_enabled == 0) svcs[i].service_ack_current=0;
      			i++;
      		}
      		
      		
      		mysql_free_result(res);
      		BARTLBY_MYSQL_CLOSE(mysql);
      		free(mysql_host);
		free(mysql_user);
		free(mysql_pw);
		free(mysql_db);
      		return i;
      	} else { 
      		_log(LH_LIB, B_LOG_INFO, "no services found!");	
      	}
    	

  mysql_free_result(res);
  BARTLBY_MYSQL_CLOSE(mysql);
  free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return 0;	
}


                        
