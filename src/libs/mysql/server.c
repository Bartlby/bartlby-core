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

#define SERVER_CHANGE_SERVICES "update services set server_id=%ld where server_id=%ld"
#define SERVER_CHANGE_SERVICES_ORCH_ID "update services set orch_id=%d where server_id=%ld"
#define SERVER_UPDATE_TEXT "update servers set server_enabled='%d', server_notify='%d' where server_id=%ld"
#define DELETE_SERVICE_BY_SERVER "delete from services where server_id=%ld"



 #define SERVER_MAP_SELECTOR "select server_id, \
                                    server_ip, \
                                    server_name, \
                                    server_ico, \
                                    server_enabled, \
                                    server_port, \
                                    server_dead, \
                                    server_flap_seconds, \
                                    server_notify, \
                                    server_ssh_keyfile, \
                                    server_ssh_passphrase, \
                                    server_ssh_username, \
                                    enabled_triggers, \
                                    default_service_type, \
                                    orch_id,  \
                                    exec_plan, \
                                    web_hooks, \
                                    json_endpoint, \
                                    web_hooks_level \
                              from servers  %s"

#define ADD_SERVER "insert into \
                                servers ( \
                                    server_name, \
                                    server_ip, \
                                    server_port,  \
                                    server_ico, \
                                    server_enabled, \
                                    server_notify, \
                                    server_flap_seconds, \
                                    server_dead, \
                                    server_ssh_keyfile, \
                                    server_ssh_passphrase, \
                                    server_ssh_username, \
                                    enabled_triggers, \
                                    default_service_type, \
                                    orch_id, \
                                    exec_plan, \
                                    web_hooks, \
                                    json_endpoint, \
                                    web_hooks_level \
                                    ) \
                                VALUES( \
                                    '%s', \
                                    '%s', \
                                    '%d', \
                                    '%s', \
                                    '%d', \
                                    '%d', \
                                    '%ld', \
                                    '%d', \
                                    '%s', \
                                    '%s', \
                                    '%s', \
                                    '%s', \
                                    '%d', \
                                    '%d', \
                                    '%s', \
                                    '%s', \
                                    '%s', \
                                    '%d' \
                                )"



#define DELETE_SERVER "delete from servers where server_id=%ld"



#define UPDATE_SERVER "update servers  \
                            set \
                              server_name='%s', \
                              server_ip='%s', \
                              server_port=%d, \
                              server_ico='%s', \
                              server_enabled='%d', \
                              server_notify='%d', \
                              server_flap_seconds='%ld', \
                              server_dead='%d', \
                              server_ssh_keyfile='%s', \
                              server_ssh_passphrase='%s', \
                              server_ssh_username='%s', \
                              enabled_triggers='%s', \
                              default_service_type='%d', \
                              orch_id='%d', \
                              exec_plan='%s', \
                              web_hooks='%s', \
                              json_endpoint='%s', \
                              web_hooks_level=%d \
                            where \
                              server_id=%ld"


#define SERVER_SELECTOR "select  \
                              server_name, \
                              server_ip, \
                              server_port, \
                              server_ico, \
                              server_enabled, \
                              server_notify, \
                              server_flap_seconds, \
                              server_dead, \
                              server_ssh_keyfile, \
                              server_ssh_passphrase, \
                              server_ssh_username, \
                              enabled_triggers, \
                              default_service_type, \
                              orch_id, \
                              exec_plan, \
                              web_hooks, \
                              json_endpoint, \
                              web_hooks_level \
                          from  \
                              servers  \
                          where \
                              server_id=%ld"

#define SERVER_CHANGE_ID "update servers set server_id=%ld where server_id=%ld"



long ServerChangeId(long from, long to, int sr, char * config) {
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
	
	
	
	CHECKED_ASPRINTF(&sqlupd, SERVER_CHANGE_ID, to, from);
	
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
			
	free(sqlupd);
	
	if(sr == 1) {
		
		
		CHECKED_ASPRINTF(&sqlupd, SERVER_CHANGE_SERVICES, to,from);
		mysql_query(mysql, sqlupd);
			CHK_ERR(mysql,NULL);
		free(sqlupd);	
			
	}
	
	
	BARTLBY_MYSQL_CLOSE(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return to;	
}

int GetServerById(long server_id, struct server * svc, char * config) {
	
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
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
      	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	CHECKED_ASPRINTF(&sqlupd, SERVER_SELECTOR, server_id);
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql,NULL);
      	
      	
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
      		svc->server_flap_seconds=atol(row[6]);
      		svc->server_dead=atoi(row[7]);
      		svc->default_service_type=atoi(row[12]);
      		svc->orch_id=atoi(row[13]);

          svc->web_hooks_level=atoi(row[17]);

          if(row[14] != NULL) {
            snprintf(svc->exec_plan, 2047, "%s", row[14]);
          } else {
             sprintf(svc->exec_plan, " "); 
          }
          if(row[15] != NULL) {
            snprintf(svc->web_hooks, 1023, "%s", row[15]);
          } else {
             sprintf(svc->web_hooks, " "); 
          }
          if(row[16] != NULL) {
            snprintf(svc->json_endpoint, 254, "%s", row[16]);
          } else {
             sprintf(svc->json_endpoint, " "); 
          }
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
      	BARTLBY_MYSQL_CLOSE(mysql);
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
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	CHECKED_ASPRINTF(&sqlupd, UPDATE_SERVER,
                                  BARTLBY_SQL_PROTECTION(svc->server_name),
                                  BARTLBY_SQL_PROTECTION(svc->client_ip),
                                  svc->client_port,
                                  BARTLBY_SQL_PROTECTION(svc->server_icon),
                                  svc->server_enabled,
                                  svc->server_notify,
                                  svc->server_flap_seconds,
                                  svc->server_dead,
                                  BARTLBY_SQL_PROTECTION(svc->server_ssh_keyfile),
                                  BARTLBY_SQL_PROTECTION(svc->server_ssh_passphrase),
                                  BARTLBY_SQL_PROTECTION(svc->server_ssh_username),
                                  BARTLBY_SQL_PROTECTION(svc->enabled_triggers),
                                  svc->default_service_type,
                                  svc->orch_id,
                                  BARTLBY_SQL_PROTECTION(svc->exec_plan),
                                  BARTLBY_SQL_PROTECTION(svc->web_hooks),
                                  BARTLBY_SQL_PROTECTION(svc->json_endpoint),
                                  svc->web_hooks_level,
                                  svc->server_id
                                  );
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
	
	
	free(sqlupd);


	//update services - if orch_id changed

	CHECKED_ASPRINTF(&sqlupd, SERVER_CHANGE_SERVICES_ORCH_ID,svc->orch_id, svc->server_id);	

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
	
int DeleteServer(long server_id, char * config) {
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
	
	
	CHECKED_ASPRINTF(&sqlupd, DELETE_SERVER, server_id);
	
	//Log("dbg", sqlupd);
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
	
	
	free(sqlupd);
	
	
	//DELETE_SERVICE_BY_SERVER
	CHECKED_ASPRINTF(&sqlupd, DELETE_SERVICE_BY_SERVER, server_id);
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
long AddServer(struct server * svc, char *config) {
	/*
		We get a struct service
		filled with server_name, client_port, client_ip
		store it ;-)
		and return svc->server_id
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
	
	
	CHECKED_ASPRINTF(&sqlupd, ADD_SERVER,
                            BARTLBY_SQL_PROTECTION(svc->server_name),
                            BARTLBY_SQL_PROTECTION(svc->client_ip),
                            svc->client_port,
                            BARTLBY_SQL_PROTECTION(svc->server_icon),
                            svc->server_enabled,
                            svc->server_notify,
                            svc->server_flap_seconds,
                            svc->server_dead,
                            BARTLBY_SQL_PROTECTION(svc->server_ssh_keyfile),
                            BARTLBY_SQL_PROTECTION(svc->server_ssh_passphrase),
                            BARTLBY_SQL_PROTECTION(svc->server_ssh_username),
                            BARTLBY_SQL_PROTECTION(svc->enabled_triggers),
                            svc->default_service_type,
                            svc->orch_id,
                            BARTLBY_SQL_PROTECTION(svc->exec_plan),
                            BARTLBY_SQL_PROTECTION(svc->web_hooks),
                            BARTLBY_SQL_PROTECTION(svc->json_endpoint),
                            svc->web_hooks_level
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

int doUpdateServer(struct server * svc, char * config) {

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

        

        CHECKED_ASPRINTF(&sqlupd, SERVER_UPDATE_TEXT, svc->server_enabled, svc->server_notify, svc->server_id);


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
long GetServerMap(struct server * srv, char * config, int orch_id) {
	
	MYSQL *mysql;
	MYSQL_ROW  row;
	MYSQL_RES  *res;
	
	char * mysql_host = getConfigValue("mysql_host", config);
	char * mysql_user = getConfigValue("mysql_user", config);
	char * mysql_pw = getConfigValue("mysql_pw", config);
	char * mysql_db = getConfigValue("mysql_db", config);
	long i=0;
	
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
      	CHECKED_ASPRINTF(&sql, SERVER_MAP_SELECTOR, where);

      	mysql_query(mysql, sql);
		CHK_ERR(mysql,NULL);

		
		free(where);
		free(sql);

	res = mysql_store_result(mysql);
      		CHK_ERR(mysql,NULL);
      		
      		
	if(mysql_num_rows(res) > 0) {
      		
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {
      			
      			srv[i].server_id=atol(row[0]);
      			srv[i].server_enabled=atoi(row[4]);
      			srv[i].client_port=atoi(row[5]);
      			srv[i].server_dead=atoi(row[6]);
      			srv[i].server_flap_seconds=atol(row[7]);
      			srv[i].server_notify=atoi(row[8]);
            srv[i].web_hooks_level=atoi(row[18]);
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
      			if(row[13] != NULL) {
      				//svcs[i].service_name=malloc(strlen(row[1])*sizeof(char)+2);
      				srv[i].default_service_type=atoi(row[13]);
      				
      			} else {
      				//svcs[i].service_name=NULL;     				
      				srv[i].default_service_type=1;
      			}

      			srv[i].orch_id=atoi(row[14]);

            if(row[15] != NULL) {
              snprintf(srv[i].exec_plan, 2047, "%s", row[15]);
            } else {
              sprintf(srv[i].exec_plan, " "); 
            }
            if(row[16] != NULL) {
              snprintf(srv[i].web_hooks, 1023, "%s", row[16]);
            } else {
              sprintf(srv[i].web_hooks, " "); 
            }
            if(row[17] != NULL) {
              snprintf(srv[i].json_endpoint, 254, "%s", row[17]);
            } else {
              sprintf(srv[i].json_endpoint, " "); 
            }


      			srv[i].is_gone=0;     			
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
      		_log(LH_LIB, B_LOG_INFO,"no servers found!");	
      	}
    	
  mysql_free_result(res);
  BARTLBY_MYSQL_CLOSE(mysql);
  free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	return 0;	
}
                              