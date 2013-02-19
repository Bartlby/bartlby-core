/* $Id$ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2009 Helmut Januschka - All Rights Reserved
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



#include <bartlby.h>

#define CHK_ERR(x) \
		if (x != NULL) {\
			if(mysql_errno(x) != 0) {\
		 		_log("mysql error: %s", mysql_error(x)); \
      		 		return -1; \
      			}\
      		} else {\
      			_log("Mysql Error %s:%d", __FILE__, __LINE__); \
      			return -1; \
      		}

#define CHK_ERR_NULL(x) \
		if (x != NULL) {\
			if(mysql_errno(x) != 0) {\
		 		_log("mysql error: %s", mysql_error(x)); \
      		 		return NULL; \
      			}\
      		} else {\
      			_log("Mysql Error %s:%d", __FILE__, __LINE__); \
      			return NULL; \
      		}      		

//2 nach exec_plan
//renotify_interval, escalate_divisor
#define AUTOR "Helmut Januschka \"helmut@januschka.com\" http://bartlby.org"
#define NAME "MYSQL Connector"
#define DLVERSION  "1.3.2"

#define SERVER_MAP_SELECTOR "select server_id, server_ip, server_name, server_ico, server_enabled, server_port, server_dead, server_flap_seconds, server_notify, server_ssh_keyfile, server_ssh_passphrase, server_ssh_username, enabled_triggers from servers"

#define SELECTOR "select svc.service_id, svc.service_name, svc.service_state, srv.server_name, srv.server_id, srv.server_port, srv.server_ip, svc.service_plugin, svc.service_args, UNIX_TIMESTAMP(svc.service_last_check), svc.service_interval, svc.service_text, 'a', 'a', 'a','a', svc.service_notify, svc.service_type, svc.service_var, svc.service_passive_timeout,service_active, svc.service_check_timeout, srv.server_ico, svc.service_ack, svc.service_retain, svc.service_snmp_community, svc.service_snmp_objid, svc.service_snmp_version, svc.service_snmp_warning, svc.service_snmp_critical, svc.service_snmp_type, svc.flap_seconds, svc.service_exec_plan, svc.renotify_interval, svc.escalate_divisor, svc.fires_events, svc.enabled_triggers, svc.service_snmp_textmatch, UNIX_TIMESTAMP(svc.service_last_notify_send), UNIX_TIMESTAMP(svc.service_last_state_change)   from services svc, servers srv where svc.server_id=srv.server_id ORDER BY RAND()"
#define WORKER_SELECTOR "select worker_mail, worker_icq, enabled_services ,notify_levels, worker_active, worker_name, worker_id, password, enabled_triggers, escalation_limit, escalation_minutes, notify_plan from workers"
#define SERVICE_UPDATE_TEXT "update services set service_last_check=FROM_UNIXTIME(%d), service_text='%s', service_state=%d, service_last_notify_send=FROM_UNIXTIME(%d), service_last_state_change=FROM_UNIXTIME(%d) where service_id=%ld"



#define ADD_SERVER "insert into servers (server_name,server_ip,server_port, server_ico, server_enabled, server_notify, server_flap_seconds, server_dead, server_ssh_keyfile, server_ssh_passphrase, server_ssh_username, enabled_triggers) VALUES('%s','%s', '%d', '%s', '%d', '%d', '%ld', '%d', '%s','%s', '%s', '%s')"
#define DELETE_SERVER "delete from servers where server_id=%d"
#define UPDATE_SERVER "update servers set server_name='%s',server_ip='%s',server_port=%d, server_ico='%s', server_enabled='%d', server_notify='%d', server_flap_seconds='%ld', server_dead='%d', server_ssh_keyfile='%s', server_ssh_passphrase='%s', server_ssh_username='%s', enabled_triggers='%s' where server_id=%ld"
#define SERVER_SELECTOR "select server_name, server_ip, server_port, server_ico, server_enabled, server_notify, server_flap_seconds, server_dead, server_ssh_keyfile, server_ssh_passphrase, server_ssh_username, enabled_triggers from servers where server_id=%d"
#define SERVER_CHANGE_ID "update servers set server_id=%d where server_id=%d"
#define SERVER_CHANGE_SERVICES "update services set server_id=%d where server_id=%d"

#define SERVER_UPDATE_TEXT "update servers set server_enabled='%d', server_notify='%d' where server_id=%ld"

#define DELETE_SERVICE_BY_SERVER "delete from services where server_id=%d"

#define ADD_SERVICE "insert into services(server_id, service_plugin, service_name, service_state,service_text, service_args,service_notify, service_active, service_interval, service_type,service_var,service_passive_timeout,service_check_timeout, service_ack, service_retain, service_snmp_community, service_snmp_objid, service_snmp_version, service_snmp_warning, service_snmp_critical, service_snmp_type, flap_seconds, service_exec_plan,renotify_interval, escalate_divisor, fires_events, enabled_triggers, service_snmp_textmatch) values(%ld,'%s','%s',4, 'Newly created', '%s',%d,%d,%ld,%d,'%s',%ld, %ld, %d, %ld, '%s', '%s', %d, %ld, %ld, %ld, %ld, '%s',%ld,%ld, %ld, '%s', '%s')"
#define DELETE_SERVICE "delete from services where service_id=%d"
#define SERVICE_CHANGE_ID "update services set service_id=%d where service_id=%d"

#define UPDATE_SERVICE "update services set service_type=%d,service_name='%s',server_id=%ld,service_interval = %ld, service_plugin='%s',service_args='%s',service_passive_timeout=%ld, service_var='%s',service_check_timeout=%ld, service_ack='%d', service_retain='%ld', service_snmp_community='%s', service_snmp_objid='%s', service_snmp_version='%d', service_snmp_warning='%ld', service_snmp_critical='%ld', service_snmp_type='%ld', service_notify='%d', service_active='%d', flap_seconds='%ld', service_exec_plan='%s',renotify_interval=%ld, escalate_divisor=%ld, fires_events=%ld, enabled_triggers='%s', service_snmp_textmatch='%s'  where service_id=%ld"
#define SERVICE_SELECTOR "select svc.service_id, svc.service_name, svc.service_state, srv.server_name, srv.server_id, srv.server_port, srv.server_ip, svc.service_plugin, svc.service_args, UNIX_TIMESTAMP(svc.service_last_check), svc.service_interval, svc.service_text, 'a', 'a','a', 'a', svc.service_notify, svc.service_type, svc.service_var, svc.service_passive_timeout, svc.service_active,svc.service_check_timeout, svc.service_ack, svc.service_retain, svc.service_snmp_community, svc.service_snmp_objid, svc.service_snmp_version, svc.service_snmp_warning, svc.service_snmp_critical, svc.service_snmp_type, srv.server_ico, svc.flap_seconds, svc.service_exec_plan, svc.renotify_interval, svc.escalate_divisor, svc.fires_events, srv.server_ssh_keyfile, srv.server_ssh_passphrase, srv.server_ssh_username, svc.enabled_triggers, srv.enabled_triggers, svc.service_snmp_textmatch, UNIX_TIMESTAMP(svc.service_last_notify_send), UNIX_TIMESTAMP(svc.service_last_state_change)  from services svc, servers srv where svc.server_id=srv.server_id and svc.service_id=%d"




#define ADD_WORKER    "INSERT INTO workers(worker_mail, worker_icq, notify_levels, worker_active, worker_name, password,enabled_triggers, escalation_limit, escalation_minutes, notify_plan) VALUES('%s', '%s', '%s', %d, '%s', '%s', '%s', '%ld', '%ld', '%s')"
#define DELETE_WORKER "delete from workers where worker_id=%d"
#define UPDATE_WORKER "update workers set worker_mail='%s', worker_icq='%s', notify_levels='%s', worker_active=%d, worker_name='%s', password='%s', enabled_triggers='%s', escalation_limit='%ld', escalation_minutes='%ld', notify_plan='%s', enabled_services='%s' WHERE worker_id=%ld"
#define WORKER_SEL "select worker_mail, worker_icq, enabled_services,notify_levels, worker_active, worker_name, worker_id, password, enabled_triggers, escalation_limit, escalation_minutes, notify_plan from workers where worker_id=%d"
#define WORKER_CHANGE_ID "update workers set worker_id=%d where worker_id=%d"


#define UPDATE_DOWNTIME "update downtime set downtime_notice='%s', downtime_from=%d,downtime_to=%d, service_id=%d, downtime_type=%d where downtime_id=%ld"
#define DEL_DOWNTIME "delete from downtime where downtime_id=%d"
#define ADD_DOWNTIME "INSERT INTO downtime(downtime_type, downtime_from,downtime_to,service_id, downtime_notice) VALUES(%d,%d,%d,%d, '%s')"
#define DOWNTIME_SEL "select downtime_id, downtime_type, downtime_from, downtime_to, downtime_notice, service_id from downtime"
#define DOWNTIME_CHANGE_ID "update downtime set downtime_id=%d where downtime_id=%d"



#define UPDATE_SERVERGROUP "update servergroups set servergroup_name='%s', servergroup_notify=%d,servergroup_active=%d, servergroup_members='%s', servergroup_dead=%d, enabled_triggers='%s' where servergroup_id=%ld"
#define DEL_SERVERGROUP "delete from servergroups where servergroup_id=%d"
#define ADD_SERVERGROUP "INSERT INTO servergroups(servergroup_name, servergroup_notify,servergroup_active,servergroup_members, servergroup_dead, enabled_triggers) VALUES('%s', %d,%d,'%s', %d, '%s')"
#define SERVERGROUP_SEL "select servergroup_id, servergroup_name, servergroup_notify, servergroup_active, servergroup_members, servergroup_dead, enabled_triggers from servergroups"
#define SERVERGROUP_CHANGE_ID "update servergroups set servergroup_id=%d where servergroup_id=%d"


#define UPDATE_SERVICEGROUP "update servicegroups set servicegroup_name='%s', servicegroup_notify=%d,servicegroup_active=%d, servicegroup_members='%s', servicegroup_dead=%d, enabled_triggers='%s' where servicegroup_id=%ld"
#define DEL_SERVICEGROUP "delete from servicegroups where servicegroup_id=%d"
#define ADD_SERVICEGROUP "INSERT INTO servicegroups(servicegroup_name, servicegroup_notify,servicegroup_active,servicegroup_members, servicegroup_dead, enabled_triggers) VALUES('%s', %d,%d,'%s', %d, '%s')"
#define SERVICEGROUP_SEL "select servicegroup_id, servicegroup_name, servicegroup_notify, servicegroup_active, servicegroup_members, servicegroup_dead, enabled_triggers from servicegroups"
#define SERVICEGROUP_CHANGE_ID "update servicegroups set servicegroup_id=%d where servicegroup_id=%d"





//Counters
#define COUNT_SERVICES "select count(1) from services"
#define COUNT_WORKERS "select count(1) from workers"
#define COUNT_DOWNTIMES "select count(1) from downtime"
#define COUNT_SERVERS "select count(1) from servers"
#define COUNT_SERVERGROUPS "select count(1) from servergroups"
#define COUNT_SERVICEGROUPS "select count(1) from servicegroups"

/*

struct shm_counter {
	int worker;
	int services;
	int downtimes;	
}

*/

struct shm_counter * GetCounter(char * config) {
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	struct shm_counter * shmc;
	
	
	shmc = malloc(sizeof(struct shm_counter) + 10);
	
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	
	
	


	mysql=mysql_init(NULL);
	CHK_ERR_NULL(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
	CHK_ERR_NULL(mysql);
     mysql_select_db(mysql, mysql_db);
     CHK_ERR_NULL(mysql);
	
	
	mysql_query(mysql, COUNT_SERVICES);
	CHK_ERR_NULL(mysql);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->services = atoi(row[0]);
     	}
     	
     	
     } else {
     	shmc->services = 0;	
     }
      		
     mysql_free_result(res);
     
     mysql_query(mysql, COUNT_WORKERS);
	CHK_ERR_NULL(mysql);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->worker = atoi(row[0]);
     	}
     	
     	
     } else {
     	shmc->worker = 0;	
     }
      		
     mysql_free_result(res);
     
     mysql_query(mysql, COUNT_DOWNTIMES);
	CHK_ERR_NULL(mysql);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->downtimes = atoi(row[0]);
     	}
     	
     	
     } else {
     	shmc->downtimes = 0;	
     }
      		
     mysql_free_result(res);
     //COUNT_SERVERS
     mysql_query(mysql, COUNT_SERVERS);
	CHK_ERR_NULL(mysql);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->servers = atoi(row[0]);
     	}
     	
     	
     } else {
     	shmc->downtimes = 0;	
     }
      		
     mysql_free_result(res);
     
     
     
     //COUNT_SERVICEGROUPS
     mysql_query(mysql, COUNT_SERVICEGROUPS);
	CHK_ERR_NULL(mysql);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->servicegroups = atoi(row[0]);
     	}
     	
     	
     } else {
     	shmc->servicegroups = 0;	
     }
      		
     mysql_free_result(res);
     
     
     
     //COUNT_SERVERGROUPS
     mysql_query(mysql, COUNT_SERVERGROUPS);
	CHK_ERR_NULL(mysql);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->servergroups = atoi(row[0]);
     	}
     	
     	
     } else {
     	shmc->servergroups = 0;	
     }
      		
     mysql_free_result(res);
     
     
	mysql_close(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return shmc;
	
	
}


int UpdateDowntime(struct downtime * svc, char *config) {
	/*
		modify worker
	*/
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, UPDATE_DOWNTIME, svc->downtime_notice, svc->downtime_from, svc->downtime_to, svc->service_id, svc->downtime_type, svc->downtime_id);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=1;
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return rtc;	
}

int DeleteDowntime(int downtime_id, char * config) {
	
	MYSQL *mysql;

	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, DEL_DOWNTIME, downtime_id);
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return 1;		
	
	
}

int DowntimeChangeId(int from, int to, char * config) {
	MYSQL *mysql;
	
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, DOWNTIME_CHANGE_ID, to, from);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
			
	free(sqlupd);
		
	
	mysql_close(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return to;	
}

int WorkerChangeId(int from, int to, char * config) {
	MYSQL *mysql;
	
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, WORKER_CHANGE_ID, to, from);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
			
	free(sqlupd);
		
	
	mysql_close(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return to;	
}

int ServiceChangeId(int from, int to, char * config) {
	MYSQL *mysql;
	
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, SERVICE_CHANGE_ID, to, from);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
			
	free(sqlupd);
		
	
	mysql_close(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return to;	
}

int ServerChangeId(int from, int to, int sr, char * config) {
	MYSQL *mysql;
	
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, SERVER_CHANGE_ID, to, from);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
			
	free(sqlupd);
	
	if(sr == 1) {
		
		
		asprintf(&sqlupd, SERVER_CHANGE_SERVICES, to,from);
		mysql_query(mysql, sqlupd);
			CHK_ERR(mysql);
		free(sqlupd);	
			
	}
	
	
	mysql_close(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return to;	
}
int AddDowntime(struct downtime * svc, char *config) {
	
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, ADD_DOWNTIME, svc->downtime_type, svc->downtime_from, svc->downtime_to, svc->service_id, svc->downtime_notice);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=mysql_insert_id(mysql);
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return rtc;	
}	

int GetDowntimeMap(struct downtime * svcs, char * config) {
	
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	int i=0;
	
	


	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	mysql_query(mysql, DOWNTIME_SEL);
		CHK_ERR(mysql);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql);
      	
      	
      	if(mysql_num_rows(res) > 0) {
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {

  			if(row[0] != NULL) {
      				
      				svcs[i].downtime_id = atoi(row[0]);
      			} else {
      				svcs[i].downtime_id = -1;    				
      			}
      			
      			if(row[1] != NULL) {
      				
      				svcs[i].downtime_type = atoi(row[1]);
      			} else {
      				svcs[i].downtime_type = -1;    				
      			}
      			if(row[2] != NULL) {
      				
      				svcs[i].downtime_from = atoi(row[2]);
      			} else {
      				svcs[i].downtime_from = -1;    				
      			}
      			if(row[3] != NULL) {
      				
      				svcs[i].downtime_to = atoi(row[3]);
      			} else {
      				svcs[i].downtime_to = -1;    				
      			}
      			
      			if(row[4] != NULL) {
      				//svcs[i].icq=malloc(strlen(row[1])*sizeof(char)+2);
      				sprintf(svcs[i].downtime_notice, "%s", row[4]);
      			} else {
      				sprintf(svcs[i].downtime_notice, "(null)");     				
      			}
      			if(row[5] != NULL) {
      				
      				svcs[i].service_id = atoi(row[5]);
      			} else {
      				svcs[i].service_id = -1;    				
      			}
      			
			svcs[i].is_gone=0;		
      			
      			i++;
      		}
      		
      		mysql_free_result(res);
      		mysql_close(mysql);
      		free(mysql_host);
		free(mysql_user);
		free(mysql_pw);
		free(mysql_db);
      		return i;
      	} else { 
      		_log( "no Downtimes found!");	
      	}
	
	
	
	
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return -1;
	
	
}






int GetWorkerById(int worker_id, struct worker * svc, char * config) {
	
	int tmprc;
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	char * sqlupd;
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	
	
	
	
	
	
	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, WORKER_SEL, worker_id);
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql);
      	
      	
      	if(mysql_num_rows(res) == 1 ) {
      		row=mysql_fetch_row(res);
      		
      		if(row[0] != NULL ) {
      			sprintf(svc->mail, "%s", row[0]);	
      		} else {
      			sprintf(svc->mail, "(null)");
      		}
      		if(row[1] != NULL) {
      			sprintf(svc->icq, "%s", row[1]);	
      		} else {
      			sprintf(svc->icq, "(null)");	
      		}
      		if(row[2] != NULL) {
      			sprintf(svc->services, "%s", row[2]);	
      		} else {
      			sprintf(svc->services, "(null)");	
      		}
      		if(row[3] != NULL) {
      			sprintf(svc->notify_levels, "%s", row[3]);	
      		} else {
      			sprintf(svc->notify_levels, "(null)");	
      		}
      		if(row[4] != NULL) {
      			svc->active=atoi(row[4]);	
      		} else {
      			svc->active=-1;	
      		}
      		if(row[5] != NULL) {
      			sprintf(svc->name, "%s", row[5]);	
      		} else {
      			sprintf(svc->name, "(null)");	
      		}
      		if(row[6] != NULL) {
      			svc->worker_id=atoi(row[6]);	
      		} else {
      			svc->worker_id=-1;	
      		}
      		if(row[7] != NULL) {
      			sprintf(svc->password, "%s", row[7]);
      					
      		} else {
      			sprintf(svc->password, "(null)");	
      		}
      		if(row[8] != NULL) {
      			sprintf(svc->enabled_triggers, "%s", row[8]);
      				
      		} else {
      			sprintf(svc->enabled_triggers, "(null)");	
      		}
      		if(row[9] != NULL) {
      			svc->escalation_limit=atoi(row[9]);	
      		} else {
      			svc->escalation_limit=50;	
      		}
      		if(row[10] != NULL) {
      			svc->escalation_minutes=atoi(row[10]);	
      		} else {
      			svc->escalation_minutes=2;	
      		}
      		if(row[11] != NULL) {
      			sprintf(svc->notify_plan, "%s", row[11]);	
      		} else {
      			sprintf(svc->notify_plan, " ");	
      		}
		svc->is_gone=0;
      		tmprc=0;
      		//printf("limit: %ld, minutes: %ld", svc->escalation_limit, svc->escalation_minutes);
      	} else {
			tmprc=-1;
	}
	
	
	mysql_free_result(res);
      	mysql_close(mysql);
      	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	free(sqlupd);
	return tmprc;
		
}

int UpdateWorker(struct worker * svc, char *config) {
	/*
		modify worker
	*/
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, UPDATE_WORKER, svc->mail, svc->icq, svc->notify_levels, svc->active, svc->name,svc->password,svc->enabled_triggers,svc->escalation_limit, svc->escalation_minutes, svc->notify_plan,svc->services, svc->worker_id);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=1;
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return rtc;	
}


int DeleteWorker(int worker_id, char * config) {
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
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, DELETE_WORKER, worker_id);
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return 1;		
	
	
}

int AddWorker(struct worker * svc, char *config) {
	/*
		We get a struct worker
		filled with worker_mail, worker_ic, enabled_services, notify_levels, active
		store it ;-)
		and return wrk->worker_id
	*/
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, ADD_WORKER, svc->mail, svc->icq, svc->notify_levels, svc->active, svc->name, svc->password, svc->enabled_triggers, svc->escalation_limit, svc->escalation_minutes, svc->notify_plan);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=mysql_insert_id(mysql);
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return rtc;	
}	


int GetServiceById(int service_id, struct service * svc, char * config) {
	
	
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
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, SERVICE_SELECTOR, service_id);
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql);
      	
      	
      	if(mysql_num_rows(res) == 1 ) {
      		row=mysql_fetch_row(res);
      		svc->service_id=atoi(row[0]);
      		svc->server_id=atoi(row[4]);
      		svc->last_state=atoi(row[2]);
      		svc->current_state=atoi(row[2]);
      		
      		n_srv=malloc(sizeof(struct server));
      		
      		if(row[1] != NULL) {
      			//svc->service_name=malloc(strlen(row[1])*sizeof(char)+2);
      			sprintf(svc->service_name, "%s", row[1]);
      			
      		} else {
      			//svc->service_name=NULL;     				
      			sprintf(svc->service_name, "(null)");
      		}
      		
      		
      		
      		if(row[3] != NULL) {
      			//svc->server_name=malloc(strlen(row[3])*sizeof(char)+2);
      			sprintf(n_srv->server_name, "%s", row[3]);
      			
      		} else {
//    			svc->server_name=NULL;   
      			sprintf(n_srv->server_name, "(null)");  				
      		}
      		
      		
      		if(row[6] != NULL) {
      			//svc->client_ip=malloc(strlen(row[6])*sizeof(char)+2);
      			sprintf(n_srv->client_ip, "%s", row[6]);
      			
      		} else {
      			  
      			sprintf(n_srv->client_ip, "(null)");     				
      		}
      		if(row[30] != NULL) {
      			snprintf(n_srv->server_icon, 1000, "%s", row[30]);
      		} else {
      			sprintf(n_srv->server_icon, "(null)");	
      		}		
      		
      		n_srv->client_port=atoi(row[5]);
      		
      		if(row[36] != NULL) {
      			snprintf(n_srv->server_ssh_keyfile, 512, "%s", row[36]);
      		} else {
      			sprintf(n_srv->server_ssh_keyfile, " ");	
      		}
					if(row[37] != NULL) {
      			snprintf(n_srv->server_ssh_passphrase, 512, "%s", row[37]);
      		} else {
      			sprintf(n_srv->server_ssh_passphrase, " ");	
      		}
					if(row[38] != NULL) {
      			snprintf(n_srv->server_ssh_username, 512, "%s", row[38]);
      		} else {
      			sprintf(n_srv->server_ssh_username, " ");	
      		}
      		
      		if(row[39] != NULL) {
      			snprintf(svc->enabled_triggers, 512, "%s", row[39]);
      		} else {
      			sprintf(svc->enabled_triggers, "%s", "");	
      		}
      		if(row[40] != NULL) {
      			snprintf(n_srv->enabled_triggers, 512, "%s", row[40]);
      		} else {
      			sprintf(n_srv->enabled_triggers, "%s", "");	
      		}
      		
      		if(row[41] != NULL) {
      			snprintf(svc->snmp_info.textmatch, 1024, "%s", row[41]);
      		} else {
      			sprintf(svc->snmp_info.textmatch, "%s", "");	
      		}
					
      		svc->last_notify_send=atoi(row[42]);
      		svc->last_state_change=atoi(row[43]);
      		
      		if(svc->last_notify_send == 0) svc->last_notify_send = time(NULL);
      		if(svc->last_state_change == 0) svc->last_state_change = time(NULL);
      		
      		svc->srv=n_srv;
      		
      		
//     		svc->new_server_text=malloc(strlen(row[11])*sizeof(char)+2);
      		
      		sprintf(svc->new_server_text, "%s", row[11]);
      		
      		///svc->new_server_text=row[11];
      		
      		
      		
      		
      		if(row[7] != NULL) {
      			//svc->plugin=malloc(strlen(row[7])*sizeof(char)+2);
      			sprintf(svc->plugin, "%s", row[7]);
      			
      		} else {
      			//svc->plugin=NULL; 
      			sprintf(svc->plugin, "(null)");       				
      		}
      		
      		if(row[8] != NULL) {
      			//svc->plugin_arguments=malloc(strlen(row[8])*sizeof(char)+2);
      			sprintf(svc->plugin_arguments, "%s", row[8]);
      			
      		} else {
      			//svc->plugin_arguments=NULL; 
      			sprintf(svc->plugin_arguments, "(null)");      				
      		}
      		
      		svc->lcheck.tv_usec=0;
      		svc->lcheck.tv_sec=atoi(row[9]);
      		svc->last_check=atoi(row[9]);
      		
      		svc->check_interval=atol(row[10]);
      		
      		svc->check_interval_original=atol(row[10])*1000;
      		      		
      		
      	
      		
      		svc->notify_enabled=atoi(row[16]);
      	
      		svc->service_type = atoi(row[17]);
      		
      		if(row[18] != NULL) {
      			//svc->service_var=malloc(strlen(row[18])*sizeof(char)+2);
      			sprintf(svc->service_var, "%s", row[18]);
      			
      		} else {
      			//svc->service_var=NULL;
      			sprintf(svc->service_var, "(null)");
      		}
      		
      		svc->service_passive_timeout=atoi(row[19]);
      		
      		svc->service_active=atoi(row[20]);
      		svc->service_check_timeout=atoi(row[21]);
      		if(row[22] != NULL) {
      			svc->service_ack = atoi(row[22]);	
      		} else {
      			svc->service_ack = ACK_NOT_NEEDED;
      		}
      		svc->service_retain=atoi(row[23]);
      		svc->flap_count=0;
      		
      		if(row[24] != NULL) {
      			snprintf(svc->snmp_info.community, 500, "%s", row[24]);
      		} else {
      			sprintf(svc->snmp_info.community, "(null)");
      		}
      		
      		if(row[25] != NULL) {
      			snprintf(svc->snmp_info.objid, 1000, "%s", row[25]);
      		} else {
      			sprintf(svc->snmp_info.objid, "(null)");	
      		}
      		
      		svc->snmp_info.version = atoi(row[26]);
      		svc->snmp_info.warn = atoi(row[27]);
      		svc->snmp_info.crit = atoi(row[28]);
      		svc->snmp_info.type = atoi(row[29]);
      		
      		svc->flap_seconds = atoi(row[31]);
      		
      		if(row[32] != NULL) {
      			snprintf(svc->service_exec_plan, 2047, "%s", row[32]);
      		} else {
      			sprintf(svc->service_exec_plan, " ");	
      		}
      		//svc.renotify_interval, svc.escalate_divisor
      		
      		svc->renotify_interval=atoi(row[33]);
      		svc->escalate_divisor=atoi(row[34]);
      		svc->fires_events=atoi(row[35]);
				
					
	
	
    		svc->is_gone = 0;  		
      		tmprc=0;
      	} else {
		tmprc=-1;
	}
	
	
	mysql_free_result(res);
      	mysql_close(mysql);
      	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	free(sqlupd);
	
	
	return tmprc;	
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
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
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
	 
	asprintf(&sqlupd, UPDATE_SERVICE, 
	svc->service_type, 
	svc->service_name, 
	svc->server_id,
	svc->check_interval,
	svc->plugin,
	svc->plugin_arguments,
	svc->service_passive_timeout,
	svc->service_var,
	svc->service_check_timeout,
	svc->service_ack,
	svc->service_retain,
	svc->snmp_info.community,
	svc->snmp_info.objid,
	svc->snmp_info.version,
	svc->snmp_info.warn,
	svc->snmp_info.crit,
	svc->snmp_info.type,
	svc->notify_enabled,
	svc->service_active,	
	svc->flap_seconds,
	svc->service_exec_plan,
	svc->renotify_interval,
  svc->escalate_divisor,
  svc->fires_events,
  svc->enabled_triggers,
  svc->snmp_info.textmatch,
	svc->service_id
	
	
	);
	
	//Log("dbg", sqlupd);

	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=mysql_insert_id(mysql);
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	
	return rtc;	
}

int DeleteService(int service_id, char * config) {
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
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, DELETE_SERVICE, service_id);
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return 1;		
	
	
}


int AddService(struct service * svc, char *config) {
	/*
		We get a struct service
		fully filled :-)
		store it ;-)
		and return svc->service_id
	*/
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	
	
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	service_mysql_safe(svc);
	
	
	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
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
	 
	
	
	
	asprintf(&sqlupd, ADD_SERVICE, 
	svc->server_id, 
	svc->plugin, 
	svc->service_name,
	svc->plugin_arguments,
	svc->notify_enabled,
	svc->service_active,
	svc->check_interval,
	svc->service_type,
	svc->service_var,
	svc->service_passive_timeout,
	svc->service_check_timeout,
	svc->service_ack,
	svc->service_retain,
	svc->snmp_info.community,
	svc->snmp_info.objid,
	svc->snmp_info.version,
	svc->snmp_info.warn,
	svc->snmp_info.crit,
	svc->snmp_info.type,
	svc->flap_seconds,
	svc->service_exec_plan,
	svc->renotify_interval,
  svc->escalate_divisor,
  svc->fires_events,
  svc->enabled_triggers,
  svc->snmp_info.textmatch
	);
	
	//Log("dbg", sqlupd);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=mysql_insert_id(mysql);
	
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	
	return rtc;	
}

int GetServerById(int server_id, struct server * svc, char * config) {
	
	int tmprc;
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	char * sqlupd;
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	
	
	
	
	
	
	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	asprintf(&sqlupd, SERVER_SELECTOR, server_id);
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql);
      	
      	
      	if(mysql_num_rows(res) == 1 ) {
      		row=mysql_fetch_row(res);
      		
      		if(row[0] != NULL ) {
      			sprintf(svc->server_name, "%s", row[0]);	
      		} else {
      			sprintf(svc->server_name, "(null)");
      		}
      		if(row[1] != NULL) {
      			sprintf(svc->client_ip, "%s", row[1]);	
      		} else {
      			sprintf(svc->client_ip, "(null)");	
      		}
      		if(row[2] != NULL) {
      			svc->client_port=atoi(row[2]);	
      		} else {
      			svc->client_port=-1;	
      		}
      		if(row[3] != NULL) {
      			sprintf(svc->server_icon, "%s", row[3]);
      		} else {
      			sprintf(svc->server_icon, "(null)");
      		}
      		
      		svc->server_enabled=atoi(row[4]);
      		svc->server_notify=atoi(row[5]);
      		svc->server_flap_seconds=atoi(row[6]);
      		svc->server_dead=atoi(row[7]);
      		if(row[8] != NULL) {
      			sprintf(svc->server_ssh_keyfile, "%s", row[8]);
      		} else {
      			sprintf(svc->server_ssh_keyfile, " ");
      		}
      		if(row[9] != NULL) {
      			sprintf(svc->server_ssh_passphrase, "%s", row[9]);
      		} else {
      			sprintf(svc->server_ssh_passphrase, " ");
      		}
      		if(row[10] != NULL) {
      			sprintf(svc->server_ssh_username, "%s", row[10]);
      		} else {
      			sprintf(svc->server_ssh_username, " ");
      		}
					if(row[11] != NULL) {
      			sprintf(svc->enabled_triggers, "%s", row[11]);
      		} else {
      			sprintf(svc->enabled_triggers, " ");
      		}




		svc->is_gone=0;  		
      		
		tmprc=0;
      	} else {
			tmprc=-1;
	}
	
	
	mysql_free_result(res);
      	mysql_close(mysql);
      	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	free(sqlupd);
	return tmprc;
		
}
		
int ModifyServer(struct server * svc, char *config) {
	/*
		We get a struct service
		filled with server_name, client_port, client_ip, server_id
		Modify_it it ;-)
		and return svc->server_id
	*/
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	//service_mysql_safe(svc);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, UPDATE_SERVER, svc->server_name, svc->client_ip, svc->client_port,svc->server_icon,svc->server_enabled, svc->server_notify, svc->server_flap_seconds,svc->server_dead,svc->server_ssh_keyfile, svc->server_ssh_passphrase, svc->server_ssh_username,svc->enabled_triggers, svc->server_id);
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=1;
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return rtc;	
}		
	
int DeleteServer(int server_id, char * config) {
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
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, DELETE_SERVER, server_id);
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	
	
	//DELETE_SERVICE_BY_SERVER
	asprintf(&sqlupd, DELETE_SERVICE_BY_SERVER, server_id);
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return 1;		
	
	
}
int AddServer(struct server * svc, char *config) {
	/*
		We get a struct service
		filled with server_name, client_port, client_ip
		store it ;-)
		and return svc->server_id
	*/
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, ADD_SERVER, svc->server_name, svc->client_ip, svc->client_port, svc->server_icon, svc->server_enabled, svc->server_notify, svc->server_flap_seconds, svc->server_dead, svc->server_ssh_keyfile, svc->server_ssh_passphrase, svc->server_ssh_username, svc->enabled_triggers);
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=mysql_insert_id(mysql);
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return rtc;	
}	
char * GetName() {
	
	return strdup(NAME);
}
long ExpectVersion() {
	return EXPECTCORE;	
}	
char * GetAutor() {
	
	return strdup(AUTOR);
}
char * GetVersion() {
	char * vers;
	asprintf(&vers, "%s on %s", DLVERSION, MYSQL_SERVER_VERSION);
	return vers;
}

//doUpdateServer
//

int doUpdateServer(struct server * svc, char * config) {

        MYSQL *mysql;

        char * sqlupd;



        char * mysql_host = getConfigValue("mysql_host", config);
        char * mysql_user = getConfigValue("mysql_user", config);
        char * mysql_pw = getConfigValue("mysql_pw", config);
        char * mysql_db = getConfigValue("mysql_db", config);

        mysql=mysql_init(NULL);
                CHK_ERR(mysql);
        mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
                CHK_ERR(mysql);
        mysql_select_db(mysql, mysql_db);
                CHK_ERR(mysql);


	

        asprintf(&sqlupd, SERVER_UPDATE_TEXT, svc->server_enabled, svc->server_notify, svc->server_id);


        mysql_query(mysql, sqlupd);
                CHK_ERR(mysql);


        free(sqlupd);

        mysql_close(mysql);

        free(mysql_host);
        free(mysql_user);
        free(mysql_pw);
        free(mysql_db);
        return 1;

}
int doUpdate(struct service * svc, char * config) {

	MYSQL *mysql;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	service_mysql_safe(svc);
	
	
	asprintf(&sqlupd, SERVICE_UPDATE_TEXT, svc->last_check, svc->new_server_text, svc->current_state, svc->last_notify_send, svc->last_state_change, svc->service_id);
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
		
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return 1;
}

int GetWorkerMap(struct worker * svcs, char * config) {
	
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	int i=0;
	
	


	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	mysql_query(mysql, WORKER_SELECTOR);
		CHK_ERR(mysql);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql);
      	
      	
      	if(mysql_num_rows(res) > 0) {
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {
      			
      			
      			
      			
      			if(row[0] != NULL) {
      				//svcs[i].mail=malloc(strlen(row[0])*sizeof(char)+2);
      				sprintf(svcs[i].mail, "%s", row[0]);
      			} else {
      				sprintf(svcs[i].mail, "(null)");     				
      			}

			if(row[2] != NULL) {
                                sprintf(svcs[i].services, "%s", row[2]);
                        } else {
                                sprintf(svcs[i].services, "(null)");
                        }	
      			
      			if(row[1] != NULL) {
      				//svcs[i].icq=malloc(strlen(row[1])*sizeof(char)+2);
      				sprintf(svcs[i].icq, "%s", row[1]);
      			} else {
      				sprintf(svcs[i].icq, "(null)");     				
      			}
      			
      			
      			if(row[3] != NULL) {
      				sprintf(svcs[i].notify_levels, "%s", row[3]);
      					
      			} else {
      				sprintf(svcs[i].notify_levels, "(null)");	
      			}
      			if(row[4] != NULL) {
      				svcs[i].active = atoi(row[4]);	
      			} else {
      				svcs[i].active = -1;
      			}
      			if(row[5] != NULL) {
      				sprintf(svcs[i].name, "%s", row[5]);
      					
      			} else {
      				sprintf(svcs[i].name, "(null)");	
      			}
      			if(row[6] != NULL) {
      				svcs[i].worker_id = atoi(row[6]);	
      			} else {
      				svcs[i].worker_id = -1;
      			}
      			if(row[7] != NULL) {
      				sprintf(svcs[i].password, "%s", row[7]);
      					
      			} else {
      				sprintf(svcs[i].password, "(null)");	
      			}
      			if(row[8] != NULL) {
      				sprintf(svcs[i].enabled_triggers, "%s", row[8]);
      					
      			} else {
      				sprintf(svcs[i].enabled_triggers, "(null)");	
      			}
      			if(row[9] != NULL) {
      				svcs[i].escalation_limit = atoi(row[9]);	
      			} else {
      				svcs[i].escalation_limit = 50;
      			}
      			if(row[10] != NULL) {
      				svcs[i].escalation_minutes = atoi(row[10]);	
      			} else {
      				svcs[i].escalation_minutes = 2;
      			}
      			if(row[11] != NULL) {
      				sprintf(svcs[i].notify_plan, "%s", row[11]);
      					
      			} else {
      				sprintf(svcs[i].notify_plan, " ");	
      			}
      			//_log("%d escal", svcs[i].escalation_limit);
      			svcs[i].escalation_count=0;
      			svcs[i].escalation_time=time(NULL);
			svcs[i].is_gone = 0;      	
			i++;
      		}
      		
      		mysql_free_result(res);
      		mysql_close(mysql);
      		free(mysql_host);
		free(mysql_user);
		free(mysql_pw);
		free(mysql_db);
      		return i;
      	} else { 
      		_log( "no worker found!");	
      	}
	
	
	
	
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return -1;
	
	
}
int GetServiceMap(struct service * svcs, char * config) {
	
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	int i=0;
	set_cfg(config);
	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
      		
      	mysql_query(mysql, SELECTOR);
		CHK_ERR(mysql);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql);
      		
      		
      		
	if(mysql_num_rows(res) > 0) {
      		
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {
      			
      			svcs[i].service_id=atoi(row[0]);
      			svcs[i].server_id=atoi(row[4]);
      			svcs[i].last_state=atoi(row[2]);
      			svcs[i].current_state=atoi(row[2]);
      			svcs[i].servicegroup_counter=0;
      			
      			if(row[1] != NULL) {
      				//svcs[i].service_name=malloc(strlen(row[1])*sizeof(char)+2);
      				sprintf(svcs[i].service_name, "%s", row[1]);
      				
      			} else {
      				//svcs[i].service_name=NULL;     				
      				sprintf(svcs[i].service_name, "(null)");
      			}
      			
      			
      			
//     			svcs[i].new_server_text=malloc(strlen(row[11])*sizeof(char)+2);
      			
      			sprintf(svcs[i].new_server_text, "%s", row[11]);
      			
      			///svcs[i].new_server_text=row[11];
      			
      			
      			
      			
      			if(row[7] != NULL) {
      				//svcs[i].plugin=malloc(strlen(row[7])*sizeof(char)+2);
      				sprintf(svcs[i].plugin, "%s", row[7]);
      				
      			} else {
      				//svcs[i].plugin=NULL; 
      				sprintf(svcs[i].plugin, "(null)");       				
      			}
      			
      			if(row[8] != NULL) {
      				//svcs[i].plugin_arguments=malloc(strlen(row[8])*sizeof(char)+2);
      				sprintf(svcs[i].plugin_arguments, "%s", row[8]);
      				
      			} else {
      				//svcs[i].plugin_arguments=NULL; 
      				sprintf(svcs[i].plugin_arguments, "(null)");      				
      			}
      			
      			
      			//svcs[i].last_check=time(NULL)+(i*2);
      				      
      			svcs[i].lcheck.tv_usec=0;
      			svcs[i].lcheck.tv_sec=atoi(row[9]);                      
						svcs[i].last_check=atoi(row[9]);    				                            
      				                                 
      			svcs[i].check_interval=atol(row[10]);
      			svcs[i].check_interval_original=atol(row[10])*1000;
      			
      			
      			
      			svcs[i].notify_enabled=atoi(row[16]);
      			svcs[i].last_notify_send=atoi(row[38]);
      			svcs[i].last_state_change=atoi(row[39]);
      			
      			
      			if(svcs[i].last_notify_send == 0) svcs[i].last_notify_send=time(NULL);
      			if(svcs[i].last_state_change == 0) svcs[i].last_state_change=time(NULL);
      			
      			//svc.service_type, svc.service_var, svc.service_passive_timeout
      			svcs[i].service_type = atoi(row[17]);
      			
      			if(row[18] != NULL) {
      				//svcs[i].service_var=malloc(strlen(row[18])*sizeof(char)+2);
      				sprintf(svcs[i].service_var, "%s", row[18]);
      				
      			} else {
      				//svcs[i].service_var=NULL;
      				sprintf(svcs[i].service_var, "(null)");
      			}
      			
      			svcs[i].service_passive_timeout=atoi(row[19]);
      			svcs[i].service_active=atoi(row[20]);
      			svcs[i].service_check_timeout=atoi(row[21]);
      			
      			
      			if(row[23] != NULL) {
      				svcs[i].service_ack = atoi(row[23]);
      			} else {
      				svcs[i].service_ack = ACK_NOT_NEEDED;
      			}
      			
      			svcs[i].service_retain=atoi(row[24]);
      			svcs[i].service_retain_current=atoi(row[24])+1;
      			
      			svcs[i].flap_count=0;
      			svcs[i].process.pid = 0;
      			svcs[i].process.start_time = 0;
      			
      			svcs[i].notify_last_state=svcs[i].current_state;
      			svcs[i].notify_last_time=time(NULL);
      			svcs[i].pstat.sum=0;
      			svcs[i].pstat.counter=0;
      			
      			svcs[i].delay_time.sum=0;
      			svcs[i].delay_time.counter=0;
      			
      			svcs[i].do_force=0;
      			svcs[i].recovery_outstanding=RECOVERY_DONE;
      			if(svcs[i].current_state == STATE_CRITICAL) {
      				svcs[i].recovery_outstanding=RECOVERY_OUTSTANDING;
      			}
      			
      			
      			
      				
      			if(row[25] != NULL) {
      				snprintf(svcs[i].snmp_info.community, 500, "%s", row[25]);
      			} else {
      				sprintf(svcs[i].snmp_info.community, "(null)");
      			}
      			
      			if(row[26] != NULL) {
      				snprintf(svcs[i].snmp_info.objid, 1000, "%s", row[26]);
      			} else {
      				sprintf(svcs[i].snmp_info.objid, "(null)");	
      			}
      			
      			svcs[i].snmp_info.version = atoi(row[27]);
      			svcs[i].snmp_info.warn = atoi(row[28]);
      			svcs[i].snmp_info.crit = atoi(row[29]);
      			svcs[i].snmp_info.type = atoi(row[30]);
      			
      			svcs[i].flap_seconds=atoi(row[31]);
      			
      			if(row[32] != NULL) {
      				snprintf(svcs[i].service_exec_plan, 2047, "%s", row[32]);
      			} else {
      				sprintf(svcs[i].service_exec_plan, "%s", "");	
      			}
      			
      			//renotify_interval, escalate_divisor
      			svcs[i].renotify_interval=atoi(row[33]);
      			svcs[i].escalate_divisor=atoi(row[34]);
      			
      			svcs[i].fires_events=atoi(row[35]);
						svcs[i].is_gone = 0;	
						
						
						if(row[36] != NULL) {
      				snprintf(svcs[i].enabled_triggers, 512, "%s", row[36]);
      			} else {
      				sprintf(svcs[i].enabled_triggers, "%s", "");	
      			}
      			if(row[37] != NULL) {
      				snprintf(svcs[i].snmp_info.textmatch, 1024, "%s", row[37]);
      			} else {
      				sprintf(svcs[i].snmp_info.textmatch, "%s", "");	
      			}
      				
      			//bartlby_replace_svc_in_str(svcs[i].plugin_arguments, &svcs[i], 2048);
      			i++;
      		}
      		
      		
      		mysql_free_result(res);
      		mysql_close(mysql);
      		free(mysql_host);
		free(mysql_user);
		free(mysql_pw);
		free(mysql_db);
      		return i;
      	} else { 
      		_log("no services found!");	
      	}
    	
    	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return 0;	
}



/*
//#define SERVER_MAP_SELECTOR "select 
0 = server_id
1 = server_ip
2 = server_name
3 = server_ico
4 =  server_enabled 
5 = server_port
6 = server_dead
7 = server_flap_seconds
8 =  server_notify

*/
int GetServerMap(struct server * srv, char * config) {
	
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	int i=0;
	set_cfg(config);
	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
      		
      	mysql_query(mysql, SERVER_MAP_SELECTOR);
		CHK_ERR(mysql);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql);
      		
      		
	if(mysql_num_rows(res) > 0) {
      		
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {
      			
      			srv[i].server_id=atoi(row[0]);
      			srv[i].server_enabled=atoi(row[4]);
      			srv[i].client_port=atoi(row[5]);
      			srv[i].server_dead=atoi(row[6]);
      			srv[i].server_flap_seconds=atoi(row[7]);
      			srv[i].server_notify=atoi(row[8]);
      			srv[i].last_notify_send=time(NULL);
      			srv[i].flap_count=0;
      			srv[i].servergroup_counter=0;
      			
      			if(row[1] != NULL) {
      				//svcs[i].service_name=malloc(strlen(row[1])*sizeof(char)+2);
      				sprintf(srv[i].client_ip, "%s", row[1]);
      				
      			} else {
      				//svcs[i].service_name=NULL;     				
      				sprintf(srv[i].client_ip, "(null)");
      			}
      			if(row[2] != NULL) {
      				//svcs[i].service_name=malloc(strlen(row[1])*sizeof(char)+2);
      				sprintf(srv[i].server_name, "%s", row[2]);
      				
      			} else {
      				//svcs[i].service_name=NULL;     				
      				sprintf(srv[i].server_name, "(null)");
      			}
      			if(row[3] != NULL) {
      				//svcs[i].service_name=malloc(strlen(row[1])*sizeof(char)+2);
      				sprintf(srv[i].server_icon, "%s", row[3]);
      				
      			} else {
      				//svcs[i].service_name=NULL;     				
      				sprintf(srv[i].server_icon, "(null)");
      			}
      			
      			if(row[9] != NULL) {
      				//svcs[i].service_name=malloc(strlen(row[1])*sizeof(char)+2);
      				sprintf(srv[i].server_ssh_keyfile, "%s", row[9]);
      				
      			} else {
      				//svcs[i].service_name=NULL;     				
      				sprintf(srv[i].server_ssh_keyfile, " ");
      			}
      			if(row[10] != NULL) {
      				//svcs[i].service_name=malloc(strlen(row[1])*sizeof(char)+2);
      				sprintf(srv[i].server_ssh_passphrase, "%s", row[10]);
      				
      			} else {
      				//svcs[i].service_name=NULL;     				
      				sprintf(srv[i].server_ssh_passphrase, " ");
      			}
      			if(row[11] != NULL) {
      				//svcs[i].service_name=malloc(strlen(row[1])*sizeof(char)+2);
      				sprintf(srv[i].server_ssh_username, "%s", row[11]);
      				
      			} else {
      				//svcs[i].service_name=NULL;     				
      				sprintf(srv[i].server_ssh_username, " ");
      			}
      			if(row[12] != NULL) {
      				//svcs[i].service_name=malloc(strlen(row[1])*sizeof(char)+2);
      				sprintf(srv[i].enabled_triggers, "%s", row[12]);
      				
      			} else {
      				//svcs[i].service_name=NULL;     				
      				sprintf(srv[i].enabled_triggers, " ");
      			}
      			
      			
 			srv[i].is_gone=0;     			
      			i++;
      		}
      		
      		
      		mysql_free_result(res);
      		mysql_close(mysql);
      		free(mysql_host);
		free(mysql_user);
		free(mysql_pw);
		free(mysql_db);
      		return i;
      	} else { 
      		_log("no servers found!");	
      	}
    	
    	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return 0;	
}



int ServerGroupChangeId(int from, int to, char * config) {
	MYSQL *mysql;
	
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, SERVERGROUP_CHANGE_ID, to, from);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
			
	free(sqlupd);
		
	
	mysql_close(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return to;	
}

int GetServerGroupMap(struct servergroup * svcs, char * config) {
	
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	int i=0;
	
	


	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	mysql_query(mysql, SERVERGROUP_SEL);
	
		CHK_ERR(mysql);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql);
      	
      	
      	if(mysql_num_rows(res) > 0) {
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {

  			if(row[0] != NULL) {
      				
      				svcs[i].servergroup_id = atoi(row[0]);
      			} else {
      				svcs[i].servergroup_id = -1;    				
      			}
      			
      			if(row[1] != NULL) {
      				
      				sprintf(svcs[i].servergroup_name, "%s", row[1]);
      			} else {
      				sprintf(svcs[i].servergroup_name, "(null)");     				
      			}
      			if(row[2] != NULL) {
      				
      				svcs[i].servergroup_notify = atoi(row[2]);
      			} else {
      				svcs[i].servergroup_notify = 1;    				
      			}
      			if(row[3] != NULL) {
      				
      				svcs[i].servergroup_active = atoi(row[3]);
      			} else {
      				svcs[i].servergroup_active = 1;    				
      			}
      			if(row[4] != NULL) {
      				
      				sprintf(svcs[i].servergroup_members, "%s", row[4]);
      				if(strcmp(svcs[i].servergroup_name, "DEFAULT") == 0) {
      					sprintf(svcs[i].servergroup_members, "%s", "");
      				}      				
      			} else {
      				sprintf(svcs[i].servergroup_members, "(null)");     				
      			}
						if(row[5] != NULL) {
      				svcs[i].servergroup_dead = atoi(row[5]);
      			} else{
      				svcs[i].servergroup_dead = 0;
      			}
      			if(row[6] != NULL) {
      				
      				sprintf(svcs[i].enabled_triggers, "%s", row[6]);
      			} else {
      				sprintf(svcs[i].enabled_triggers, "%s", "");     				
      			}
      			
		
      			i++;
      		}
      		
      		mysql_free_result(res);
      		mysql_close(mysql);
      		free(mysql_host);
		free(mysql_user);
		free(mysql_pw);
		free(mysql_db);
      		return i;
      	} else { 
      		_log( "no Servergroups found!");	
      	}
	
	
	
	
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return -1;
	
	
}

int AddServerGroup(struct servergroup * svc, char *config) {
	
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, ADD_SERVERGROUP, svc->servergroup_name, svc->servergroup_notify, svc->servergroup_active, svc->servergroup_members, svc->servergroup_dead, svc->enabled_triggers);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=mysql_insert_id(mysql);
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return rtc;	
}	


int DeleteServerGroup(int servergroup_id, char * config) {
	
	MYSQL *mysql;

	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, DEL_SERVERGROUP, servergroup_id);
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return 1;		
	
	
}

int UpdateServerGroup(struct servergroup * svc, char *config) {
	/*
		modify worker
	*/
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, UPDATE_SERVERGROUP, svc->servergroup_name, svc->servergroup_notify, svc->servergroup_active, svc->servergroup_members,svc->servergroup_dead,svc->enabled_triggers, svc->servergroup_id);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=1;
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return rtc;	
}



int ServiceGroupChangeId(int from, int to, char * config) {
	MYSQL *mysql;
	
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, SERVICEGROUP_CHANGE_ID, to, from);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
			
	free(sqlupd);
		
	
	mysql_close(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return to;	
}

int GetServiceGroupMap(struct servicegroup * svcs, char * config) {
	
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	int i=0;
	
	


	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	mysql_query(mysql, SERVICEGROUP_SEL);
	
		CHK_ERR(mysql);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql);
      	
      	
      	if(mysql_num_rows(res) > 0) {
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {

  			if(row[0] != NULL) {
      				
      				svcs[i].servicegroup_id = atoi(row[0]);
      			} else {
      				svcs[i].servicegroup_id = -1;    				
      			}
      			
      			if(row[1] != NULL) {
      				
      				sprintf(svcs[i].servicegroup_name, "%s", row[1]);
      			} else {
      				sprintf(svcs[i].servicegroup_name, "(null)");     				
      			} 
      			if(row[2] != NULL) {
      				
      				svcs[i].servicegroup_notify = atoi(row[2]);
      			} else {
      				svcs[i].servicegroup_notify = 1;    				
      			}
      			if(row[3] != NULL) {
      				
      				svcs[i].servicegroup_active = atoi(row[3]);
      			} else {
      				svcs[i].servicegroup_active = 1;    				
      			}
      			if(row[4] != NULL) {
      				
      				sprintf(svcs[i].servicegroup_members, "%s", row[4]);
      				
      				if(strcmp(svcs[i].servicegroup_name, "DEFAULT") == 0) {
      					sprintf(svcs[i].servicegroup_members, "%s", "");
      				}  
      				
      			} else {
      				sprintf(svcs[i].servicegroup_members, "(null)");     				
      			}
      			
      			if(row[5] != NULL) {
      				svcs[i].servicegroup_dead = atoi(row[5]);
      			} else{
      				svcs[i].servicegroup_dead = 0;
      			}
      			
      			if(row[6] != NULL) {
      				sprintf(svcs[i].enabled_triggers, "%s", row[6]);
      			} else{
      				sprintf(svcs[i].enabled_triggers, " ");
      			}
      			
		
      			i++;
      		}
      		
      		mysql_free_result(res);
      		mysql_close(mysql);
      		free(mysql_host);
		free(mysql_user);
		free(mysql_pw);
		free(mysql_db);
      		return i;
      	} else { 
      		_log( "no Servicegroups found!");	
      	}
	
	
	
	
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return -1;
	
	
}

int AddServiceGroup(struct servicegroup * svc, char *config) {
	
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, ADD_SERVICEGROUP, svc->servicegroup_name, svc->servicegroup_notify, svc->servicegroup_active, svc->servicegroup_members, svc->servicegroup_dead, svc->enabled_triggers);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=mysql_insert_id(mysql);
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return rtc;	
}	


int DeleteServiceGroup(int servicegroup_id, char * config) {
	
	MYSQL *mysql;

	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	asprintf(&sqlupd, DEL_SERVICEGROUP, servicegroup_id);
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return 1;		
	
	
}

int UpdateServiceGroup(struct servicegroup * svc, char *config) {
	/*
		modify worker
	*/
	MYSQL *mysql;
	int rtc;
	
	char * sqlupd;
	


	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);

	mysql=mysql_init(NULL);
		CHK_ERR(mysql);
	mysql=mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql);
	
	
	
	asprintf(&sqlupd, UPDATE_SERVICEGROUP, svc->servicegroup_name, svc->servicegroup_notify, svc->servicegroup_active, svc->servicegroup_members,svc->servicegroup_dead,svc->enabled_triggers, svc->servicegroup_id);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql);
	
	
	free(sqlupd);
	rtc=1;
	mysql_close(mysql);
		
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return rtc;	
}
