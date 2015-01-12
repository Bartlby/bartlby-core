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


#define UPDATE_SERVICEGROUP "update \
                              servicegroups  \
                            set \
                              servicegroup_name='%s', \
                              servicegroup_notify=%d, \
                              servicegroup_active=%d, \
                              servicegroup_members='%s', \
                              servicegroup_dead=%d, \
                              enabled_triggers='%s', \
                              orch_id=%d \
                            where \
                              servicegroup_id=%ld"


#define DEL_SERVICEGROUP "delete from servicegroups where servicegroup_id=%ld"
#define ADD_SERVICEGROUP "INSERT INTO \
                                servicegroups( \
                                    servicegroup_name, \
                                    servicegroup_notify, \
                                    servicegroup_active, \
                                    servicegroup_members, \
                                    servicegroup_dead, \
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

#define SERVICEGROUP_SEL "select  \
                            servicegroup_id, \
                            servicegroup_name, \
                            servicegroup_notify, \
                            servicegroup_active, \
                            servicegroup_members, \
                            servicegroup_dead, \
                            enabled_triggers, \
                            orch_id \
                        from  \
                          servicegroups %s"


#define SERVICEGROUP_SELECTOR "select \
                            servicegroup_id, \
                            servicegroup_name, \
                            servicegroup_notify, \
                            servicegroup_active, \
                            servicegroup_members, \
                            servicegroup_dead, \
                            enabled_triggers, \
                            orch_id  \
                          from \
                            servicegroups where servicegroup_id=%ld"


#define SERVICEGROUP_CHANGE_ID "update servicegroups set servicegroup_id=%ld where servicegroup_id=%ld"






long ServiceGroupChangeId(long from, long to, char * config) {
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
	
	
	CHECKED_ASPRINTF(&sqlupd, SERVICEGROUP_CHANGE_ID, to, from);
	
	
	
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


int GetsServicegroupById(long servicegroup_id, struct servicegroup * svc, char * config) {
  
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
  
  CHECKED_ASPRINTF(&sqlupd, SERVICEGROUP_SELECTOR, servicegroup_id);
  
  
  mysql_query(mysql, sqlupd);
    CHK_ERR(mysql,NULL);
        res = mysql_store_result(mysql);
          CHK_ERR(mysql,NULL);
        
        
        if(mysql_num_rows(res) == 1 ) {
          row=mysql_fetch_row(res);
            if(row[0] != NULL) {
              svc->servicegroup_id = atol(row[0]);
            } else {
              svc->servicegroup_id = -1;            
            }
            
            if(row[1] != NULL) {
              
              sprintf(svc->servicegroup_name, "%s", row[1]);
            } else {
              sprintf(svc->servicegroup_name, "(null)");            
            }
            if(row[2] != NULL) {
              
              svc->servicegroup_notify = atoi(row[2]);
            } else {
              svc->servicegroup_notify = 1;           
            }
            if(row[3] != NULL) {
              
              svc->servicegroup_active = atoi(row[3]);
            } else {
              svc->servicegroup_active = 1;           
            }
            if(row[4] != NULL) {
              
              sprintf(svc->servicegroup_members, "%s", row[4]);
              if(strcmp(svc->servicegroup_name, "DEFAULT") == 0) {
                sprintf(svc->servicegroup_members, "%s", "");
              }             
            } else {
              sprintf(svc->servicegroup_members, "(null)");             
            }
            if(row[5] != NULL) {
              svc->servicegroup_dead = atoi(row[5]);
            } else{
              svc->servicegroup_dead = 0;
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

int GetServiceGroupMap(struct servicegroup * svcs, char * config, int orch_id) {
	
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
      	CHECKED_ASPRINTF(&sql, SERVICEGROUP_SEL, where);

      	mysql_query(mysql, sql);
		CHK_ERR(mysql,NULL);

    
		
		free(where);
		free(sql);




	res = mysql_store_result(mysql);
      		CHK_ERR(mysql,NULL);
      	
      	
      	if(mysql_num_rows(res) > 0) {
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {

  			if(row[0] != NULL) {
      				
      				svcs[i].servicegroup_id = atol(row[0]);
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
      		_log(LH_LIB, B_LOG_INFO, "no Servicegroups found!");	
      	}
	
	
	
	mysql_free_result(res);
  BARTLBY_MYSQL_CLOSE(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return 0;
	
	
}

long AddServiceGroup(struct servicegroup * svc, char *config) {
	
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
	
	
	CHECKED_ASPRINTF(&sqlupd, ADD_SERVICEGROUP, 
                              BARTLBY_SQL_PROTECTION(svc->servicegroup_name),
                              svc->servicegroup_notify,
                              svc->servicegroup_active,
                              BARTLBY_SQL_PROTECTION(svc->servicegroup_members),
                              svc->servicegroup_dead,
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


int DeleteServiceGroup(long servicegroup_id, char * config) {
	
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
	
	
	CHECKED_ASPRINTF(&sqlupd, DEL_SERVICEGROUP, servicegroup_id);
	
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
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	
	CHECKED_ASPRINTF(&sqlupd, UPDATE_SERVICEGROUP,
                                  BARTLBY_SQL_PROTECTION(svc->servicegroup_name),
                                  svc->servicegroup_notify,
                                  svc->servicegroup_active,
                                  BARTLBY_SQL_PROTECTION(svc->servicegroup_members),
                                  svc->servicegroup_dead,
                                  BARTLBY_SQL_PROTECTION(svc->enabled_triggers),
                                  svc->orch_id,
                                  svc->servicegroup_id
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

