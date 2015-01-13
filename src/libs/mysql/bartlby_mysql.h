struct mysql_buffers_list {
  char * sql;
  struct mysql_buffers_list * next;
};

char * bartlby_mysql_safe(MYSQL * mysql, struct mysql_buffers_list ** bartlby_protection_buff_list, char * value);
void bartlby_mysql_safe_free(struct mysql_buffers_list * head);
void bartlby_mysql_safe_init(struct mysql_buffers_list ** curr, struct mysql_buffers_list ** head);




#define BARTLBY_SQL_PROTECTION_FREE bartlby_mysql_safe_free(bartlby_protection_bartlby_protection_buff_list_head)

#define CHK_FREE_CRED if(mysql_user != NULL) { free(mysql_user); } \
					   if(mysql_pw != NULL) { free(mysql_pw); } \
					   if(mysql_host != NULL) { free(mysql_host); } \
					   if(mysql_db != NULL) { free(mysql_db); } \
             if(mysql != NULL) { mysql_close(mysql); mysql_library_end(); } \
             BARTLBY_SQL_PROTECTION_FREE;

#define CHK_ERR(x, y) \
		if (x != NULL) {\
			if(mysql_errno(x) != 0) {\
		 		_log(LH_LIB, B_LOG_CRIT,"mysql error: %s - %s:%d", mysql_error(x),  __FILE__, __LINE__); \
		 			CHK_FREE_CRED; \
          if(y != NULL) { free(y); } \
      		 		return -1; \
      			}\
      		} else {\
      			_log(LH_LIB, B_LOG_CRIT,"Mysql Error %s:%d", __FILE__, __LINE__); \
      			CHK_FREE_CRED; \
      			return -1; \
      		}

#define CHK_ERR_NULL(x,y) \
		if (x != NULL) {\
			if(mysql_errno(x) != 0) {\
		 		_log(LH_LIB, B_LOG_CRIT,"mysql error: %s - %s:%d", mysql_error(x), __FILE__, __LINE__); \
		 			CHK_FREE_CRED; \
          if(y != NULL) { free(y); } \
      		 		return NULL; \
      			}\
      		} else {\
      			_log(LH_LIB, B_LOG_CRIT,"Mysql Error %s:%d", __FILE__, __LINE__); \
      			CHK_FREE_CRED; \
            if(y != NULL) { free(y); } \
      			return NULL; \
      		}      		



#define AUTOR "btl-core"
#define NAME "MYSQL Connector"
#define DLVERSION  "1.5.1"




#define BARTLBY_SQL_PROTECTION_INIT struct mysql_buffers_list * bartlby_protection_buff_list; \
                                    struct mysql_buffers_list * bartlby_protection_bartlby_protection_buff_list_head; \
                                    bartlby_mysql_safe_init(&bartlby_protection_buff_list, &bartlby_protection_bartlby_protection_buff_list_head)

#define BARTLBY_SQL_PROTECTION(value) bartlby_mysql_safe(mysql, &bartlby_protection_buff_list, value)
                        

#define BARTLBY_MYSQL_CLOSE(mysql) mysql_close(mysql); \
                                   mysql_library_end(); \
                                   BARTLBY_SQL_PROTECTION_FREE;


int GetServerById(long server_id, struct server * svc, char * config);
