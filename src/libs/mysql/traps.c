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
 

///TRAPS
#define UPDATE_TRAP "update traps \
                                    set \
                                      trap_name='%s', \
                                      trap_catcher='%s', \
                                      trap_status_text='%s', \
                                      trap_status_ok='%s', \
                                      trap_status_warning='%s', \
                                      trap_status_critical='%s', \
                                      trap_service_id=%ld, \
                                      trap_fixed_status=%d, \
                                      trap_prio=%d, \
                                      trap_is_final=%d, \
                                      orch_id=%d \
                                    where \
                                      trap_id=%ld"

#define DEL_TRAP "delete from traps where trap_id=%ld"





#define ADD_TRAP "INSERT INTO \
                                      traps( \
                                          trap_name, \
                                          trap_catcher, \
                                          trap_status_text, \
                                          trap_status_ok, \
                                          trap_status_warning, \
                                          trap_status_critical, \
                                          trap_service_id, \
                                          trap_fixed_status, \
                                          trap_prio, \
                                          trap_is_final, \
                                          orch_id \
                                        ) \
                                      VALUES( \
                                          '%s', \
                                          '%s', \
                                          '%s', \
                                          '%s', \
                                          '%s', \
                                          '%s', \
                                          '%ld', \
                                          '%d', \
                                          '%d', \
                                          '%d', \
                                          '%d' \
                                      )"
#define TRAP_SEL "select \
                            trap_id, \
                            trap_name, \
                            trap_catcher, \
                            trap_status_text, \
                            trap_status_ok, \
                            trap_status_warning, \
                            trap_status_critical, \
                            trap_service_id, \
                            trap_fixed_status, \
                            trap_prio, \
                            trap_is_final, \
                            orch_id  \
                          from \
                            traps %s order by trap_prio desc"



#define TRAP_SELECTOR "select \
                            trap_id, \
                            trap_name, \
                            trap_catcher, \
                            trap_status_text, \
                            trap_status_ok, \
                            trap_status_warning, \
                            trap_status_critical, \
                            trap_service_id, \
                            trap_fixed_status, \
                            trap_prio, \
                            trap_is_final, \
                            orch_id  \
                          from \
                            traps where trap_id=%ld"

                            


#define TRAP_CHANGE_ID "update traps set trap_id=%ld where trap_id=%ld"
///TRAPS;


long AddTrap(struct trap * svc, char *config) {
  
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
  
  
  CHECKED_ASPRINTF(&sqlupd, ADD_TRAP, 
                            BARTLBY_SQL_PROTECTION(svc->trap_name),
                            BARTLBY_SQL_PROTECTION(svc->trap_catcher),
                            BARTLBY_SQL_PROTECTION(svc->trap_status_text),
                            BARTLBY_SQL_PROTECTION(svc->trap_status_ok),
                            BARTLBY_SQL_PROTECTION(svc->trap_status_warning),
                            BARTLBY_SQL_PROTECTION(svc->trap_status_critical),
                            svc->trap_service_id,
                            svc->trap_fixed_status,
                            svc->trap_prio,
                            svc->trap_is_final,
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


int DeleteTrap(long trap_id, char * config) {
  
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
  
  
  CHECKED_ASPRINTF(&sqlupd, DEL_TRAP, trap_id);
  
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

int UpdateTrap(struct trap * svc, char *config) {

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
  
  CHECKED_ASPRINTF(&sqlupd, UPDATE_TRAP,
                            BARTLBY_SQL_PROTECTION(svc->trap_name),
                            BARTLBY_SQL_PROTECTION(svc->trap_catcher),
                            BARTLBY_SQL_PROTECTION(svc->trap_status_text),
                            BARTLBY_SQL_PROTECTION(svc->trap_status_ok),
                            BARTLBY_SQL_PROTECTION(svc->trap_status_warning),
                            BARTLBY_SQL_PROTECTION(svc->trap_status_critical),
                            svc->trap_service_id,
                            svc->trap_fixed_status,
                            svc->trap_prio,
                            svc->trap_is_final,
                            svc->orch_id,
                            svc->trap_id);
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

long TrapChangeId(long from, long to, char * config) {
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
  
  
  CHECKED_ASPRINTF(&sqlupd, TRAP_CHANGE_ID, to, from);
  
  
  
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


int GetTrapById(long trap_id, struct trap * svc, char * config) {
  
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
  
  CHECKED_ASPRINTF(&sqlupd, TRAP_SELECTOR, trap_id);
  
  
  mysql_query(mysql, sqlupd);
    CHK_ERR(mysql,NULL);
        res = mysql_store_result(mysql);
          CHK_ERR(mysql,NULL);
        
        
        if(mysql_num_rows(res) == 1 ) {
          row=mysql_fetch_row(res);
          svc->trap_id = atol(row[0]);

          if(row[1] != NULL) {
            sprintf(svc->trap_name, "%s", row[1]);
          } else {
            sprintf(svc->trap_name, "(null)");          
          }
          if(row[2] != NULL) {
            sprintf(svc->trap_catcher, "%s", row[2]);
          } else {
            sprintf(svc->trap_catcher, "(null)");          
          }
          if(row[3] != NULL) {
            sprintf(svc->trap_status_text, "%s", row[3]);
          } else {
            sprintf(svc->trap_status_text, "(null)");          
          }
          if(row[4] != NULL) {
            sprintf(svc->trap_status_ok, "%s", row[4]);
          } else {
            sprintf(svc->trap_status_ok, "(null)");          
          }
          if(row[5] != NULL) {
            sprintf(svc->trap_status_warning, "%s", row[5]);
          } else {
            sprintf(svc->trap_status_warning, "(null)");          
          }
          if(row[6] != NULL) {
            sprintf(svc->trap_status_critical, "%s", row[6]);
          } else {
            sprintf(svc->trap_status_critical, "(null)");          
          }

          sprintf(svc->trap_last_data, " ");          
          svc->trap_service_id = atol(row[7]);
          svc->trap_fixed_status = atol(row[8]);
          svc->trap_prio = atol(row[9]);
          svc->trap_is_final = atol(row[10]);
          svc->orch_id = atol(row[11]);
    
          svc->service_shm_place=-2;
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

long GetTrapMap(struct trap * svcs, char * config, int orch_id) {
  
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
        CHECKED_ASPRINTF(&sql, TRAP_SEL, where);

        mysql_query(mysql, sql);
    CHK_ERR(mysql,NULL);

    
    free(where);
    free(sql);


  res = mysql_store_result(mysql);
          CHK_ERR(mysql,NULL);
        
        
        if(mysql_num_rows(res) > 0) {
          
          while ( (row=mysql_fetch_row(res)) != NULL) {
              svcs[i].trap_id = atol(row[0]);
              if(row[1] != NULL) {
                sprintf(svcs[i].trap_name, "%s", row[1]);
              } else {
                sprintf(svcs[i].trap_name, "(null)");          
              }
              if(row[2] != NULL) {
                sprintf(svcs[i].trap_catcher, "%s", row[2]);
              } else {
                sprintf(svcs[i].trap_catcher, "(null)");          
              }
              if(row[3] != NULL) {
                sprintf(svcs[i].trap_status_text, "%s", row[3]);
              } else {
                sprintf(svcs[i].trap_status_text, "(null)");          
              }
              if(row[4] != NULL) {
                sprintf(svcs[i].trap_status_ok, "%s", row[4]);
              } else {
                sprintf(svcs[i].trap_status_ok, "(null)");          
              }
              if(row[5] != NULL) {
                sprintf(svcs[i].trap_status_warning, "%s", row[5]);
              } else {
                sprintf(svcs[i].trap_status_warning, "(null)");          
              }
              if(row[6] != NULL) {
                sprintf(svcs[i].trap_status_critical, "%s", row[6]);
              } else {
                sprintf(svcs[i].trap_status_critical, "(null)");          
              }
            
              sprintf(svcs[i].trap_last_data, " ");          
            
              svcs[i].trap_service_id = atol(row[7]);
              svcs[i].trap_fixed_status = atol(row[8]);
              svcs[i].trap_prio = atol(row[9]);
              svcs[i].trap_is_final = atol(row[10]);
              svcs[i].orch_id = atol(row[11]);
              svcs[i].trap_last_match = 0;
              svcs[i].service_shm_place=-2;

              svcs[i].matched=0;
              svcs[i].is_gone=0;

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
          _log(LH_LIB, B_LOG_INFO, "no Traps found!");  
        }
  
  
  
  mysql_free_result(res);
  BARTLBY_MYSQL_CLOSE(mysql);
  free(mysql_host);
  free(mysql_user);
  free(mysql_pw);
  free(mysql_db);
  
  return -1;
  
  
} 

                           