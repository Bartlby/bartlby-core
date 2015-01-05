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

static int db_is_connected=0;
static MYSQL * mysql_conn;








//Counters
#define COUNT_SERVICES "select count(1) from services"
#define COUNT_WORKERS "select count(1) from workers"
#define COUNT_DOWNTIMES "select count(1) from downtime"
#define COUNT_SERVERS "select count(1) from servers"
#define COUNT_SERVERGROUPS "select count(1) from servergroups"
#define COUNT_SERVICEGROUPS "select count(1) from servicegroups"
#define COUNT_TRAPS "select count(1) from traps"


#define TEST_SQL "select a='%s' b='%s' from test"



void bartlby_mysql_safe_init(struct mysql_buffers_list ** curr, struct mysql_buffers_list ** head) {

  *curr = malloc(sizeof(struct mysql_buffers_list)); \
  (*curr)->next=NULL; \
  (*curr)->sql=NULL; \
  *head=(*curr);

}
void bartlby_mysql_safe_free(struct mysql_buffers_list * head) {
  struct mysql_buffers_list * curr, *t;

  curr=head->next;
  while(curr != NULL) {
    t=curr;
    free(curr->sql);
    curr=t->next;
    free(t);


  }
  free(head);
}
char * bartlby_mysql_safe(MYSQL * mysql, struct mysql_buffers_list ** bartlby_protection_buff_list, char * value) {
    struct mysql_buffers_list * new;
    new=malloc(sizeof(struct mysql_buffers_list)); 

    
    new->next=NULL;
    new->sql = malloc((strlen(value)*2)+1); 
    mysql_real_escape_string(mysql, new->sql, value, strlen(value)); 

    (*bartlby_protection_buff_list)->next=new;
    *bartlby_protection_buff_list=new; 
    
    
    return new->sql;

}

int TestSQL(char * config, char * placeholder) {


  MYSQL *mysql;
  MYSQL_ROW  row;
  MYSQL_RES  *res;
  char * mysql_host = getConfigValue("mysql_host", config);
  char * mysql_user = getConfigValue("mysql_user", config);
  char * mysql_pw = getConfigValue("mysql_pw", config);
  char * mysql_db = getConfigValue("mysql_db", config);
  
  char * sql;
 


  mysql=mysql_init(NULL);
  
  BARTLBY_SQL_PROTECTION_INIT;
  mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
  CHK_ERR(mysql,NULL);
  mysql_select_db(mysql, mysql_db);
  CHK_ERR(mysql,NULL);
  

  


  CHECKED_ASPRINTF(&sql, TEST_SQL, 
    BARTLBY_SQL_PROTECTION(placeholder),
    BARTLBY_SQL_PROTECTION(placeholder)
  );



  printf("SQL: @%s@\n", sql);

  free(sql);


  BARTLBY_MYSQL_CLOSE(mysql);
  mysql_library_end();

  free(mysql_host);
  free(mysql_user);
  free(mysql_pw);
  free(mysql_db);
  

  return 0;

}                            




MYSQL * getDBConn(char * config) {
	//ping mysql_conn;
	//if gone
	if(db_is_connected == 0) {
		//CONNECT
		/*

			char * mysql_host = getConfigValue("mysql_host", config);
			char * mysql_user = getConfigValue("mysql_user", config);
			char * mysql_pw = getConfigValue("mysql_pw", config);
			char * mysql_db = getConfigValue("mysql_db", config);
		
		
		


			mysql=mysql_init(NULL);
BARTLBY_SQL_PROTECTION_INIT;
			CHK_ERR_NULL(mysql,NULL);
			mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
			CHK_ERR_NULL(mysql,NULL);
	     	mysql_select_db(mysql, mysql_db);
	     	CHK_ERR_NULL(mysql,NULL);

     */

		//mysql_options(&mysql, MYSQL_OPT_RECONNECT, 1); -> set auto reconnect
		//MYSQL * mysql_conn
	} else {
		//orig_thraad_id mysql_thread_id();
		//mysql_ping()  < 0 - reconnect		
		//new_thread_id  mysql_thread_id();

		//if(thread_id's changed reconnect happend)

		//MYSQL * mysql_conn
	}
	//return mysql_conn
  return NULL;
}

void DataLibInit(char * cfgfile,int do_debug) {
	db_is_connected=0;
}

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
BARTLBY_SQL_PROTECTION_INIT;
	CHK_ERR_NULL(mysql,shmc);
	mysql_real_connect(mysql, mysql_host, mysql_user, mysql_pw, NULL, 0, NULL, 0);
	CHK_ERR_NULL(mysql,shmc);
     mysql_select_db(mysql, mysql_db);
     CHK_ERR_NULL(mysql,shmc);
	
	
	mysql_query(mysql, COUNT_SERVICES);
	CHK_ERR_NULL(mysql,shmc);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql,shmc);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->services = atol(row[0]);
     	}
     	
     	
     } else {
     	shmc->services = 0;	
     }
      		
     mysql_free_result(res);
     
     mysql_query(mysql, COUNT_WORKERS);
	CHK_ERR_NULL(mysql,shmc);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql,shmc);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->worker = atol(row[0]);
     	}
     	
     	
     } else {
     	shmc->worker = 0;	
     }
      		
     mysql_free_result(res);
     
     mysql_query(mysql, COUNT_DOWNTIMES);
	CHK_ERR_NULL(mysql,shmc);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql,shmc);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->downtimes = atol(row[0]);
     	}
     	
     	
     } else {
     	shmc->downtimes = 0;	
     }
      		
     mysql_free_result(res);
     //COUNT_SERVERS
     mysql_query(mysql, COUNT_SERVERS);
	CHK_ERR_NULL(mysql,shmc);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql,shmc);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->servers = atol(row[0]);
     	}
     	
     	
     } else {
     		shmc->servers = 0;	
     }
      		
     mysql_free_result(res);
     
     
     
     //COUNT_SERVICEGROUPS
     mysql_query(mysql, COUNT_SERVICEGROUPS);
	CHK_ERR_NULL(mysql,shmc);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql,shmc);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->servicegroups = atol(row[0]);
     	}
     	
     	
     } else {
     	shmc->servicegroups = 0;	
     }
      		
     mysql_free_result(res);
     
     
     
     //COUNT_SERVERGROUPS
     mysql_query(mysql, COUNT_SERVERGROUPS);
	   CHK_ERR_NULL(mysql,shmc);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql,shmc);
     
     
     if(mysql_num_rows(res) > 0) {
     	row=mysql_fetch_row(res);

  		if(row[0] != NULL) {
     	 	shmc->servergroups = atol(row[0]);
     	}
     	
     	
     } else {
     	shmc->servergroups = 0;	
     }
      		
     mysql_free_result(res);

    //COUNT TRAPS
     mysql_query(mysql, COUNT_TRAPS);
     CHK_ERR_NULL(mysql,shmc);
     res = mysql_store_result(mysql);
     CHK_ERR_NULL(mysql,shmc);
     
     
     if(mysql_num_rows(res) > 0) {
      row=mysql_fetch_row(res);

      if(row[0] != NULL) {
        shmc->traps = atol(row[0]);
      }
      
      
     } else {
      shmc->traps = 0; 
     }
          
     mysql_free_result(res);     
     
     
	BARTLBY_MYSQL_CLOSE(mysql);
	free(mysql_host);
	free(mysql_user);
	free(mysql_pw);
	free(mysql_db);
	
	return shmc;
	
	
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
	CHECKED_ASPRINTF(&vers, "%s on %s", DLVERSION, MYSQL_SERVER_VERSION);
	return vers;
}












void debugStr(char * s) {
  FILE * fp;
  fp = fopen("/tmp/mysql_lib", "a");
  fwrite(s , sizeof(char) , strlen(s) , fp );
  fclose(fp);


}



