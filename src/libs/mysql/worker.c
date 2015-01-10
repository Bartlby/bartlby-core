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


 #define WORKER_SELECTOR "select worker_mail, \
                               worker_icq, \
                               visible_services, \
                               notify_levels, \
                               worker_active, \
                               worker_name, \
                               worker_id, \
                               password, \
                               enabled_triggers, \
                               escalation_limit, \
                               escalation_minutes, \
                               notify_plan, \
                               visible_servers, \
                               selected_services, \
                               selected_servers, \
                               is_super_user, \
                               notification_aggregation_interval, \
                               orch_id, \
                               api_pubkey, \
                               api_privkey, \
                               api_enabled \
                          from workers  %s"


#define ADD_WORKER    "INSERT INTO workers \
                          ( \
                            worker_mail, \
                            worker_icq, \
                            notify_levels, \
                            worker_active, \
                            worker_name, \
                            password, \
                            enabled_triggers, \
                            escalation_limit, \
                            escalation_minutes, \
                            notify_plan, \
                            visible_services, \
                            visible_servers, \
                            selected_servers, \
                            selected_services, \
                            is_super_user, \
                            notification_aggregation_interval, \
                            orch_id, \
                            api_pubkey, \
                            api_privkey, \
                            api_enabled \
                          )  \
                        VALUES( \
                            '%s', \
                            '%s', \
                            '%s', \
                            %d, \
                            '%s', \
                            '%s', \
                            '%s', \
                            '%ld', \
                            '%ld', \
                            '%s', \
                            '%s', \
                            '%s', \
                            '%s', \
                            '%s', \
                            %d, \
                            %d, \
                            %d, \
                            '%s',\
                            '%s', \
                            '%d' \
                        )"

#define DELETE_WORKER "delete from workers where worker_id=%ld"

#define UPDATE_WORKER "update workers \
                          set \
                            worker_mail='%s', \
                            worker_icq='%s', \
                            notify_levels='%s', \
                            worker_active=%d, \
                            worker_name='%s', \
                            password='%s', \
                            enabled_triggers='%s', \
                            escalation_limit='%ld', \
                            escalation_minutes='%ld', \
                            notify_plan='%s', \
                            visible_services='%s', \
                            visible_servers='%s', \
                            selected_services='%s', \
                            selected_servers='%s', \
                            is_super_user=%d, \
                            notification_aggregation_interval=%d, \
                            orch_id=%d, \
                            api_pubkey = '%s', \
                            api_privkey = '%s', \
                            api_enabled = %d \
                        WHERE  \
                            worker_id=%ld"


#define WORKER_SEL "select  \
                          worker_mail, \
                          worker_icq, \
                          visible_services, \
                          notify_levels, \
                          worker_active, \
                          worker_name, \
                          worker_id, \
                          password, \
                          enabled_triggers, \
                          escalation_limit, \
                          escalation_minutes, \
                          notify_plan, \
                          visible_servers, \
                          selected_servers, \
                          selected_services, \
                          is_super_user, \
                          notification_aggregation_interval, \
                          orch_id,  \
                          api_pubkey, \
                          api_privkey, \
                          api_enabled \
                      from  \
                          workers where worker_id=%ld"


#define WORKER_CHANGE_ID "update workers set worker_id=%ld where worker_id=%ld"



long WorkerChangeId(long from, long to, char * config) {
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
	
	
	
	CHECKED_ASPRINTF(&sqlupd, WORKER_CHANGE_ID, to, from);
	
	
	
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

int GetWorkerById(long worker_id, struct worker * svc, char * config) {
	
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
	
	
	CHECKED_ASPRINTF(&sqlupd, WORKER_SEL, worker_id);
	
	
	mysql_query(mysql, sqlupd);
		CHK_ERR(mysql,NULL);
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql,NULL);
      	
      	
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
      			sprintf(svc->visible_services, "%s", row[2]);	
      		} else {
      			sprintf(svc->visible_services, " ");	
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
      			svc->worker_id=atol(row[6]);	
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
      			svc->escalation_limit=atol(row[9]);	
      		} else {
      			svc->escalation_limit=50;	
      		}
      		if(row[10] != NULL) {
      			svc->escalation_minutes=atol(row[10]);	
      		} else {
      			svc->escalation_minutes=2;	
      		}
      		if(row[11] != NULL) {
      			sprintf(svc->notify_plan, "%s", row[11]);	
      		} else {
      			sprintf(svc->notify_plan, " ");	
      		}

      		if(row[12] != NULL) {
      			sprintf(svc->visible_servers, "%s", row[12]);	
      		} else {
      			sprintf(svc->visible_servers, " ");	
      		}
      		if(row[13] != NULL) {
      			sprintf(svc->selected_servers, "%s", row[13]);	
      		} else {
      			sprintf(svc->selected_servers, " ");	
      		}
      		if(row[14] != NULL) {
      			sprintf(svc->selected_services, "%s", row[14]);	
      		} else {
      			sprintf(svc->selected_services, " ");	
      		}
      		svc->is_super_user=atoi(row[15]);
      		svc->notification_aggregation_interval=atoi(row[16]);
      		svc->orch_id=atoi(row[17]);
			     
          if(row[18] != NULL) {
            sprintf(svc->api_pubkey, "%s", row[18]); 
          } else {
            sprintf(svc->api_pubkey, " "); 
          }
          if(row[19] != NULL) {
            sprintf(svc->api_privkey, "%s", row[19]); 
          } else {
            sprintf(svc->api_privkey, " "); 
          }          

          svc->api_enabled=atoi(row[20]);

          svc->is_gone=0;
      		tmprc=0;
      		//printf("limit: %ld, minutes: %ld", svc->escalation_limit, svc->escalation_minutes);
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
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	
	CHECKED_ASPRINTF(&sqlupd, UPDATE_WORKER, 
                                  BARTLBY_SQL_PROTECTION(svc->mail),
                                  BARTLBY_SQL_PROTECTION(svc->icq),
                                  BARTLBY_SQL_PROTECTION(svc->notify_levels),
                                  svc->active,
                                  BARTLBY_SQL_PROTECTION(svc->name),
                                  BARTLBY_SQL_PROTECTION(svc->password),
                                  BARTLBY_SQL_PROTECTION(svc->enabled_triggers),
                                  svc->escalation_limit,
                                  svc->escalation_minutes,
                                  BARTLBY_SQL_PROTECTION(svc->notify_plan),
                                  BARTLBY_SQL_PROTECTION(svc->visible_services),
                                  BARTLBY_SQL_PROTECTION(svc->visible_servers),
                                  BARTLBY_SQL_PROTECTION(svc->selected_services),
                                  BARTLBY_SQL_PROTECTION(svc->selected_servers),
                                  svc->is_super_user,
                                  svc->notification_aggregation_interval,
                                  svc->orch_id,
                                  BARTLBY_SQL_PROTECTION(svc->api_pubkey),
                                  BARTLBY_SQL_PROTECTION(svc->api_privkey),
                                  svc->api_enabled,
                                  svc->worker_id
                                  );
	
	
	
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


int DeleteWorker(long worker_id, char * config) {
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
	
	
	
	CHECKED_ASPRINTF(&sqlupd, DELETE_WORKER, worker_id);
	
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

long AddWorker(struct worker * svc, char *config) {
	/*
		We get a struct worker
		filled with worker_mail, worker_ic, enabled_services, notify_levels, active
		store it ;-)
		and return wrk->worker_id
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
	
	
	
	CHECKED_ASPRINTF(&sqlupd, ADD_WORKER,
                                  BARTLBY_SQL_PROTECTION(svc->mail),
                                  BARTLBY_SQL_PROTECTION(svc->icq),
                                  BARTLBY_SQL_PROTECTION(svc->notify_levels),
                                  svc->active,
                                  BARTLBY_SQL_PROTECTION(svc->name),
                                  BARTLBY_SQL_PROTECTION(svc->password),
                                  BARTLBY_SQL_PROTECTION(svc->enabled_triggers),
                                  svc->escalation_limit,
                                  svc->escalation_minutes,
                                  BARTLBY_SQL_PROTECTION(svc->notify_plan),
                                  BARTLBY_SQL_PROTECTION(svc->visible_services),
                                  BARTLBY_SQL_PROTECTION(svc->visible_servers),
                                  BARTLBY_SQL_PROTECTION(svc->selected_servers),
                                  BARTLBY_SQL_PROTECTION(svc->selected_services),
                                  svc->is_super_user,
                                  svc->notification_aggregation_interval,
                                  svc->orch_id,
                                  BARTLBY_SQL_PROTECTION(svc->api_pubkey),
                                  BARTLBY_SQL_PROTECTION(svc->api_privkey),
                                  svc->api_enabled
                                  );
	
	
	
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

int GetWorkerMap(struct worker * svcs, char * config, int orch_id) {
	
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
      	CHECKED_ASPRINTF(&sql, WORKER_SELECTOR, where);

      	mysql_query(mysql, sql);
		CHK_ERR(mysql,NULL);

		
		free(where);
		free(sql);
	

      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql,NULL);
      	
      	
      	if(mysql_num_rows(res) > 0) {
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {
      			
      			
      			
      			
      			if(row[0] != NULL) {
      				//svcs[i].mail=malloc(strlen(row[0])*sizeof(char)+2);
      				sprintf(svcs[i].mail, "%s", row[0]);
      			} else {
      				sprintf(svcs[i].mail, "(null)");     				
      			}

			if(row[2] != NULL) {
                                sprintf(svcs[i].visible_services, "%s", row[2]);
                        } else {
                                sprintf(svcs[i].visible_services, "(null)");
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
      				svcs[i].worker_id = atol(row[6]);	
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
      				svcs[i].escalation_limit = atol(row[9]);	
      			} else {
      				svcs[i].escalation_limit = 50;
      			}
      			if(row[10] != NULL) {
      				svcs[i].escalation_minutes = atol(row[10]);	
      			} else {
      				svcs[i].escalation_minutes = 2;
      			}
      			if(row[11] != NULL) {
      				sprintf(svcs[i].notify_plan, "%s", row[11]);
      					
      			} else {
      				sprintf(svcs[i].notify_plan, " ");	
      			}
      			if(row[12] != NULL) {
      				sprintf(svcs[i].visible_servers, "%s", row[12]);
      					
      			} else {
      				sprintf(svcs[i].visible_servers, " ");	
      			}
      			if(row[13] != NULL) {
      				sprintf(svcs[i].selected_services, "%s", row[13]);
      					
      			} else {
      				sprintf(svcs[i].selected_services, " ");	
      			}
      			if(row[14] != NULL) {
      				sprintf(svcs[i].selected_servers, "%s", row[14]);
      					
      			} else {
      				sprintf(svcs[i].selected_servers, " ");	
      			}
      			
      			svcs[i].is_super_user=atoi(row[15]);
      			svcs[i].notification_aggregation_interval=atoi(row[16]);
				svcs[i].orch_id=atoi(row[17]);

            if(row[18] != NULL) {
              sprintf(svcs[i].api_pubkey, "%s", row[18]);
                
            } else {
              sprintf(svcs[i].api_pubkey, ""); 
            }
            if(row[19] != NULL) {
              sprintf(svcs[i].api_privkey, "%s", row[19]);
                
            } else {
              sprintf(svcs[i].api_privkey, ""); 
            }
            svcs[i].api_enabled=atoi(row[20]);

      			//_log("%d escal", svcs[i].escalation_limit);
      			svcs[i].escalation_count=0;
      			svcs[i].escalation_time=time(NULL);
			svcs[i].is_gone = 0;      	
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
      		_log(LH_LIB, B_LOG_INFO, "no worker found!");	
          free(mysql_host);
          free(mysql_user);
          free(mysql_pw);
          free(mysql_db);
      		return 0;
      	}
	
	
	
	
	
	
	return -1;
	
	
}                          