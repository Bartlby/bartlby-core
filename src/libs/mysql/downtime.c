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


 #define UPDATE_DOWNTIME "update \
                            downtime \
                          set \
                            downtime_notice='%s', \
                            downtime_from=%d, \
                            downtime_to=%d, \
                            service_id=%d, \
                            downtime_type=%d, \
                            orch_id=%d \
                          where  \
                            downtime_id=%ld"

#define DEL_DOWNTIME "delete from downtime where downtime_id=%ld"
#define ADD_DOWNTIME "INSERT INTO \
                              downtime \
                                ( \
                                  downtime_type, \
                                  downtime_from, \
                                  downtime_to, \
                                  service_id, \
                                  downtime_notice, \
                                  orch_id \
                                ) \
                              VALUES( \
                                  %d, \
                                  %d, \
                                  %d, \
                                  %d, \
                                  '%s', \
                                  '%d' \
                              )"

#define DOWNTIME_SEL "select \
                        downtime_id, \
                        downtime_type, \
                        downtime_from, \
                        downtime_to, \
                        downtime_notice, \
                        service_id, \
                        orch_id \
                      from \
                        downtime %s"


#define DOWNTIME_SELECTOR "select \
                        downtime_id, \
                        downtime_type, \
                        downtime_from, \
                        downtime_to, \
                        downtime_notice, \
                        service_id, \
                        orch_id \
                      from \
                        downtime where downtime_id=%ld"


#define DOWNTIME_CHANGE_ID "update downtime set downtime_id=%ld where downtime_id=%ld"


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
BARTLBY_SQL_PROTECTION_INIT;
		CHK_ERR(mysql,NULL);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
		CHK_ERR(mysql,NULL);
	mysql_select_db(mysql, mysql_db);
      		CHK_ERR(mysql,NULL);
	
	
	
	CHECKED_ASPRINTF(&sqlupd, UPDATE_DOWNTIME, 
                    BARTLBY_SQL_PROTECTION(svc->downtime_notice),
                    svc->downtime_from,
                    svc->downtime_to,
                    svc->service_id,
                    svc->downtime_type,
                    svc->orch_id,
                    svc->downtime_id);
	
	
	
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

int DeleteDowntime(long downtime_id, char * config) {
	
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
	
	
	
	CHECKED_ASPRINTF(&sqlupd, DEL_DOWNTIME, downtime_id);
	
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

long DowntimeChangeId(long from, long to, char * config) {
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
	
	
	
	CHECKED_ASPRINTF(&sqlupd, DOWNTIME_CHANGE_ID, to, from);
	
	
	
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






long AddDowntime(struct downtime * svc, char *config) {
	
  
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
	
	
	
	CHECKED_ASPRINTF(&sqlupd, ADD_DOWNTIME, 
                            svc->downtime_type,
                            svc->downtime_from,
                            svc->downtime_to,
                            svc->service_id,
                            BARTLBY_SQL_PROTECTION(svc->downtime_notice),
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
int GetDowntimeById(long downtime_id, struct downtime * svcs, char * config) {
 
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
          

        CHECKED_ASPRINTF(&sqlupd, DOWNTIME_SELECTOR, downtime_id);
  
  
      mysql_query(mysql, sqlupd);

    res = mysql_store_result(mysql);
    CHK_ERR(mysql,NULL);
        
        tmprc=-1;
        if(mysql_num_rows(res) == 1) {
          row=mysql_fetch_row(res);
          
        
            if(row[0] != NULL) {
              
              svcs->downtime_id = atol(row[0]);
            } else {
              svcs->downtime_id = -1;           
            }
            
            if(row[1] != NULL) {
              
              svcs->downtime_type = atoi(row[1]);
            } else {
              svcs->downtime_type = -1;           
            }
            if(row[2] != NULL) {
              
              svcs->downtime_from = atoi(row[2]);
            } else {
              svcs->downtime_from = -1;           
            }
            if(row[3] != NULL) {
              
              svcs->downtime_to = atoi(row[3]);
            } else {
              svcs->downtime_to = -1;           
            }
            
            if(row[4] != NULL) {
              
              sprintf(svcs->downtime_notice, "%s", row[4]);
            } else {
              sprintf(svcs->downtime_notice, "(null)");             
            }
            if(row[5] != NULL) {
              
              svcs->service_id = atol(row[5]);
            } else {
              svcs->service_id = -1;            
            }
            svcs->orch_id=atoi(row[6]);
            
            svcs->is_gone=0;    
            
            tmprc=0;  
          }
          
    
        BARTLBY_MYSQL_CLOSE(mysql);
        mysql_free_result(res);
      
      free(mysql_host);
      free(mysql_user);
      free(mysql_pw);
      free(mysql_db);
      

          
          
return tmprc;
       
  
  
  
  
}

long GetDowntimeMap(struct downtime * svcs, char * config, int orch_id) {
	
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
      	CHECKED_ASPRINTF(&sql, DOWNTIME_SEL, where);

      	mysql_query(mysql, sql);
		CHK_ERR(mysql,NULL);

		
		free(where);
		free(sql);

      	res = mysql_store_result(mysql);
      		CHK_ERR(mysql,NULL);
      	
      	
      	if(mysql_num_rows(res) > 0) {
      		
      		while ( (row=mysql_fetch_row(res)) != NULL) {

  			if(row[0] != NULL) {
      				
      				svcs[i].downtime_id = atol(row[0]);
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
      				sprintf(svcs[i].downtime_notice, "%s", row[4]);
      			} else {
      				sprintf(svcs[i].downtime_notice, "(null)");     				
      			}
      			if(row[5] != NULL) {
      				
      				svcs[i].service_id = atol(row[5]);
      			} else {
      				svcs[i].service_id = -1;    				
      			}
      			svcs[i].orch_id=atoi(row[6]);
      			
			svcs[i].is_gone=0;		
      			
      			i++;
      		}
      		
    
		    BARTLBY_MYSQL_CLOSE(mysql);
			mysql_free_result(res);
			
			free(mysql_host);
			free(mysql_user);
			free(mysql_pw);
			free(mysql_db);
			

      		
      		
      		return i;
      	} else { 
      		_log(LH_LIB, B_LOG_INFO, "no Downtimes found!");	
      	}
	
	
	BARTLBY_MYSQL_CLOSE(mysql);
	mysql_free_result(res);
	
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return 0;
	
	
}