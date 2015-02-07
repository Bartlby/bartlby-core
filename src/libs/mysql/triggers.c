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
 



///TRIGGERS
#define UPDATE_TRIGGER "update triggers \
                                    set \
                                      trigger_name='%s', \
                                      trigger_enabled=%d, \
                                      trigger_type=%d, \
                                      trigger_data='%s', \
                                      trigger_execplan='%s', \
                                      orch_id=%d \
                                    where \
                                      trigger_id=%ld"

#define DEL_TRIGGER "delete from triggers where trigger_id=%ld"


#define ADD_TRIGGER "INSERT INTO \
                                      triggers( \
                                          trigger_name, \
                                          trigger_enabled, \
                                          trigger_type, \
                                          trigger_data, \
                                          trigger_execplan, \
                                          orch_id \
                                        ) \
                                      VALUES( \
                                          '%s', \
                                          '%d', \
                                          '%d', \
                                          '%s', \
                                          '%s', \
                                          '%d' \
                                      )"
#define TRIGGER_SEL "select \
                            trigger_id, \
                            trigger_name, \
                            trigger_enabled, \
                            trigger_type, \
                            trigger_data, \
                            trigger_execplan, \
                            orch_id \
                          from \
                            triggers %s "



#define TRIGGER_SELECTOR "select \
                            trigger_id, \
                            trigger_name, \
                            trigger_enabled, \
                            trigger_type, \
                            trigger_data, \
                            trigger_execplan, \
                            orch_id \
                        from \
                            triggers where trigger_id=%ld"

                            


#define TRIGGER_CHANGE_ID "update triggers set trigger_id=%ld where trigger_id=%ld"
///TRIGGERS;


long AddTrigger(struct trigger * svc, char *config) {
  
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
  
  




  CHECKED_ASPRINTF(&sqlupd, ADD_TRIGGER, 
                            BARTLBY_SQL_PROTECTION(svc->trigger_name),
                            svc->trigger_enabled,
                            svc->trigger_type, 
                            BARTLBY_SQL_PROTECTION(svc->trigger_data),
                            BARTLBY_SQL_PROTECTION(svc->trigger_execplan),
                            svc->orch_id                           
  );
  
  //debugStr(sqlupd);

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


int DeleteTrigger(long trigger_id, char * config) {
  
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
  
  
  CHECKED_ASPRINTF(&sqlupd, DEL_TRIGGER, trigger_id);
  
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

int UpdateTrigger(struct trigger * svc, char *config) {

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
  

  CHECKED_ASPRINTF(&sqlupd, UPDATE_TRIGGER,
                            BARTLBY_SQL_PROTECTION(svc->trigger_name),
                            svc->trigger_enabled,
                            svc->trigger_type,
                            BARTLBY_SQL_PROTECTION(svc->trigger_data),
                            BARTLBY_SQL_PROTECTION(svc->trigger_execplan),
                            svc->orch_id,
                            svc->trigger_id);

  
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

long TriggerChangeId(long from, long to, char * config) {
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
  
  
  CHECKED_ASPRINTF(&sqlupd, TRIGGER_CHANGE_ID, to, from);
  
  
  
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


int GetTriggerById(long trigger_id, struct trigger * svc, char * config) {
  
  int tmprc;
  MYSQL *mysql;
  MYSQL_ROW  row;
  MYSQL_RES  *res;
  char * sqlupd;
  
  char * mysql_host = getConfigValue("mysql_host", config);
  char * mysql_user = getConfigValue("mysql_user", config);
  char * mysql_pw = getConfigValue("mysql_pw", config);
  char * mysql_db = getConfigValue("mysql_db", config);
  
  char * trigger_dir;
  
  
  

  
  
  mysql=mysql_init(NULL);
BARTLBY_SQL_PROTECTION_INIT;
    CHK_ERR(mysql,NULL);
  mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
    CHK_ERR(mysql,NULL);
        mysql_select_db(mysql, mysql_db);
          CHK_ERR(mysql,NULL);
  
  CHECKED_ASPRINTF(&sqlupd, TRIGGER_SELECTOR, trigger_id);
  
  
  mysql_query(mysql, sqlupd);
    CHK_ERR(mysql,NULL);
        res = mysql_store_result(mysql);
          CHK_ERR(mysql,NULL);
        
        trigger_dir = getConfigValue("trigger_dir", config);
        
        if(mysql_num_rows(res) == 1 ) {
          row=mysql_fetch_row(res);
          svc->trigger_id = atol(row[0]);
          if(row[1] != NULL) {
            sprintf(svc->trigger_name, "%s", row[1]);
          } else {
            sprintf(svc->trigger_name, "(null)");          
          }
          svc->trigger_enabled = atoi(row[2]);
          svc->trigger_type = atoi(row[3]);
          if(row[4] != NULL) {
            sprintf(svc->trigger_data, "%s", row[4]);
          } else {
            sprintf(svc->trigger_data, "(null)");          
          }
          if(row[5] != NULL) {
            sprintf(svc->trigger_execplan, "%s", row[5]);
          } else {
            sprintf(svc->trigger_execplan, "(null)");          
          }
          svc->orch_id = atoi(row[6]);


          svc->trigger_count=0;
          
          if(svc->trigger_type == TRIGGER_TYPE_LOCAL) {
            sprintf(svc->trigger_full_path, "%s/%s", trigger_dir, svc->trigger_data);
          } else {
            sprintf(svc->trigger_full_path, " ");
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
  free(trigger_dir);
  return tmprc;
    
}

long GetTriggerMap(struct trigger * svcs, char * config, int orch_id) {
  
  MYSQL *mysql;
  MYSQL_ROW  row;
  MYSQL_RES  *res;
  
  
  char * mysql_host = getConfigValue("mysql_host", config);
  char * mysql_user = getConfigValue("mysql_user", config);
  char * mysql_pw = getConfigValue("mysql_pw", config);
  char * mysql_db = getConfigValue("mysql_db", config);

  char * trigger_dir;

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
        CHECKED_ASPRINTF(&sql, TRIGGER_SEL, where);

        mysql_query(mysql, sql);
    CHK_ERR(mysql,NULL);

    
    free(where);
    free(sql);


  res = mysql_store_result(mysql);
          CHK_ERR(mysql,NULL);
        
        trigger_dir = getConfigValue("trigger_dir", config);
        if(mysql_num_rows(res) > 0) {
          
          while ( (row=mysql_fetch_row(res)) != NULL) {
              
              svcs[i].trigger_id = atol(row[0]);
              if(row[1] != NULL) {
                sprintf(svcs[i].trigger_name, "%s", row[1]);
              } else {
                sprintf(svcs[i].trigger_name, "(null)");          
              }
              svcs[i].trigger_enabled = atoi(row[2]);
              svcs[i].trigger_type = atoi(row[3]);
              if(row[4] != NULL) {
                sprintf(svcs[i].trigger_data, "%s", row[4]);
              } else {
                sprintf(svcs[i].trigger_data, "(null)");          
              }
              if(row[5] != NULL) {
                sprintf(svcs[i].trigger_execplan, "%s", row[5]);
              } else {
                sprintf(svcs[i].trigger_execplan, "(null)");          
              }
              svcs[i].orch_id = atoi(row[6]);


              svcs[i].trigger_count=0;
              if(svcs[i].trigger_type == TRIGGER_TYPE_LOCAL) {
                sprintf(svcs[i].trigger_full_path, "%s/%s", trigger_dir, svcs[i].trigger_data);
              } else {
                sprintf(svcs[i].trigger_full_path, " ");
              }
              
              svcs[i].is_gone=0;


            i++;
          }
          
          mysql_free_result(res);
          BARTLBY_MYSQL_CLOSE(mysql);
          free(mysql_host);
          free(mysql_user);
          free(mysql_pw);
          free(mysql_db);
          free(trigger_dir);
          return i;
        } else { 
          _log(LH_LIB, B_LOG_INFO, "no Triggers found!");  
        }
  
  
  
  mysql_free_result(res);
  BARTLBY_MYSQL_CLOSE(mysql);
  free(mysql_host);
  free(mysql_user);
  free(mysql_pw);
  free(mysql_db);
  free(trigger_dir);
  return -1;
  
  
} 

                           