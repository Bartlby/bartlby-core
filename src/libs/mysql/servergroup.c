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

 #define UPDATE_SERVERGROUP "update servergroups \
                                    set \
                                      servergroup_name='%s', \
                                      servergroup_notify=%d, \
                                      servergroup_active=%d, \
                                      servergroup_members='%s', \
                                      servergroup_dead=%d, \
                                      enabled_triggers='%s', \
                                      orch_id=%d \
                                    where \
                                      servergroup_id=%ld"

#define DEL_SERVERGROUP "delete from servergroups where servergroup_id=%ld"
#define ADD_SERVERGROUP "INSERT INTO \
                                      servergroups( \
                                          servergroup_name, \
                                          servergroup_notify, \
                                          servergroup_active, \
                                          servergroup_members, \
                                          servergroup_dead, \
                                          enabled_triggers, \
                                          orch_id \
                                        ) \
                                      VALUES( \
                                          '%s', \
                                          %d, \
                                          %d, \
                                          '%s', \
                                          %d, \
                                          '%s', \
                                          '%d' \
                                      )"

#define SERVERGROUP_SEL "select \
                            servergroup_id, \
                            servergroup_name, \
                            servergroup_notify, \
                            servergroup_active, \
                            servergroup_members, \
                            servergroup_dead, \
                            enabled_triggers, \
                            orch_id  \
                          from \
                            servergroups %s"



#define SERVERGROUP_SELECTOR "select \
                            servergroup_id, \
                            servergroup_name, \
                            servergroup_notify, \
                            servergroup_active, \
                            servergroup_members, \
                            servergroup_dead, \
                            enabled_triggers, \
                            orch_id  \
                          from \
                            servergroups where servergroup_id=%ld"

                            


#define SERVERGROUP_CHANGE_ID "update servergroups set servergroup_id=%ld where servergroup_id=%ld"


long ServerGroupChangeId(long from, long to, char * config) {
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
	
	
	CHECKED_ASPRINTF(&sqlupd, SERVERGROUP_CHANGE_ID, to, from);
	
	
	
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


int GetServergroupById(long servergroup_id, struct servergroup * svc, char * config) {
  
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
  
  CHECKED_ASPRINTF(&sqlupd, SERVERGROUP_SELECTOR, servergroup_id);
  
  
  mysql_query(mysql, sqlupd);
    CHK_ERR(mysql,NULL);
        res = mysql_store_result(mysql);
          CHK_ERR(mysql,NULL);
        
        
        if(mysql_num_rows(res) == 1 ) {
          row=mysql_fetch_row(res);
            if(row[0] != NULL) {
              svc->servergroup_id = atol(row[0]);
            } else {
              svc->servergroup_id = -1;            
            }
            
            if(row[1] != NULL) {
              
              sprintf(svc->servergroup_name, "%s", row[1]);
            } else {
              sprintf(svc->servergroup_name, "(null)");            
            }
            if(row[2] != NULL) {
              
              svc->servergroup_notify = atoi(row[2]);
            } else {
              svc->servergroup_notify = 1;           
            }
            if(row[3] != NULL) {
              
              svc->servergroup_active = atoi(row[3]);
            } else {
              svc->servergroup_active = 1;           
            }
            if(row[4] != NULL) {
              
              sprintf(svc->servergroup_members, "%s", row[4]);
              if(strcmp(svc->servergroup_name, "DEFAULT") == 0) {
                sprintf(svc->servergroup_members, "%s", "");
              }             
            } else {
              sprintf(svc->servergroup_members, "(null)");             
            }
            if(row[5] != NULL) {
              svc->servergroup_dead = atoi(row[5]);
            } else{
              svc->servergroup_dead = 0;
            }
            if(row[6] != NULL) {
              
              sprintf(svc->enabled_triggers, "%s", row[6]);
            } else {
              sprintf(svc->enabled_triggers, "%s", "");            
            }
            
            svc->orch_id=atoi(row[7]);      



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

int GetServerGroupMap(struct servergroup * svcs, char * config, int orch_id) {
	
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
      	CHECKED_ASPRINTF(&sql, SERVERGROUP_SEL, where);

      	mysql_query(mysql, sql);
		CHK_ERR(mysql,NULL);

		
		free(where);
		free(sql);


	
      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql,NULL);
      	
      	
      	if(mysql_num_rows(res) > 0) {
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {

  			if(row[0] != NULL) {
      				
      				svcs[i].servergroup_id = atol(row[0]);
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
      			
				svcs[i].orch_id=atoi(row[7]);
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
      		_log(LH_LIB, B_LOG_INFO, "no Servergroups found!");	
      	}
	
	
	
	mysql_free_result(res);
  BARTLBY_MYSQL_CLOSE(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return -1;
	
	
}

long AddServerGroup(struct servergroup * svc, char *config) {
	
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
	
	
	CHECKED_ASPRINTF(&sqlupd, ADD_SERVERGROUP,
                                  BARTLBY_SQL_PROTECTION(svc->servergroup_name),
                                  svc->servergroup_notify,
                                  svc->servergroup_active,
                                  BARTLBY_SQL_PROTECTION(svc->servergroup_members),
                                  svc->servergroup_dead,
                                  BARTLBY_SQL_PROTECTION(svc->enabled_triggers),
                                  svc->orch_id
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


int DeleteServerGroup(long servergroup_id, char * config) {
	
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
	
	
	CHECKED_ASPRINTF(&sqlupd, DEL_SERVERGROUP, servergroup_id);
	
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
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	
	CHECKED_ASPRINTF(&sqlupd, UPDATE_SERVERGROUP, 
                                  BARTLBY_SQL_PROTECTION(svc->servergroup_name),
                                  svc->servergroup_notify,
                                  svc->servergroup_active,
                                  BARTLBY_SQL_PROTECTION(svc->servergroup_members),
                                  svc->servergroup_dead,
                                  BARTLBY_SQL_PROTECTION(svc->enabled_triggers),
                                  svc->orch_id,
                                  svc->servergroup_id
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

