#include "config.h"

///*** asprintf on clang */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#ifdef LUA_ADDON
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#endif


#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pwd.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ipc.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/wait.h>	
#include <sys/wait.h>  
#include <time.h>
#include <unistd.h>
#include <unistd.h>
#include <limits.h>

#if HAVE_PRCTL_H
#include <sys/prctl.h>
#endif

#include <mysql/mysql.h>
#include <sys/times.h> 
#include <semaphore.h>
#include <json.h>
#include <regex.h>
#include <curl/curl.h>
#include <math.h>

///CLIB EXTENSIONS
#include <buffer/clib_buffer.h>
#include <file2str/file2str.h>





/* Log Handles */
#define LH_DEBUG 0
#define LH_TRIGGER 1
#define LH_SCHED 2
#define LH_CHECK 3
#define LH_MAIN 4
#define LH_SHM 5
#define LH_DAEMON 6
#define LH_PERF 7
#define LH_ACK 8
#define LH_EVNT 9
#define LH_EXT 10
#define LH_NOTIFYLOG 11
#define LH_LIB 12
#define LH_PORTIER 13
#define LH_MOD 14
#define LH_ORCH 15
#define LH_LUA 16

	
#define B_LOG_DEBUG 0
#define B_LOG_INFO 1
#define B_LOG_WARN 2
#define B_LOG_CRIT 3
#define B_LOG_HASTO 4



#define BARTLBY_BASELINE_BROKEN_TRUE  1
#define BARTLBY_BASELINE_BROKEN_FALSE 0


#include "bartlby_shm.h"



#define CHECKED_ASPRINTF(...)                                       \
    if (asprintf( __VA_ARGS__ ) == -1) {                             \
       _debug("ASPRINTF FAILED");\
    }

#define CHECKED_VASPRINTF(...)                                       \
    if (vasprintf( __VA_ARGS__ ) == -1) {                             \
       _debug("ASPRINTF FAILED");\
    }



/* DEBUGGING */


#define HAVE_DEBUG 1
#define _debug(...) if(HAVE_DEBUG==1) { _log(LH_DEBUG, B_LOG_DEBUG, __VA_ARGS__); }




/* BIT MORE PORTABLE :)*/
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif


/* SIGCHLD fix for OSX*/
#ifndef SIGCLD
#define SIGCLD SIGCHLD
#endif


#ifdef SSL_ADDON 
	#include <openssl/dh.h>
	#include <openssl/ssl.h>
	#include <openssl/err.h>
	
#endif

#ifdef SSH_ADDON 
	#include <libssh/libssh.h>	
#endif


#ifdef SNMP_ADDON
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#endif






#define SNMP_CHECK_TYPE_LOWER 1
#define SNMP_CHECK_TYPE_GREATER 2
#define SNMP_CHECK_TYPE_EQ 3
#define SNMP_CHECK_TYPE_NOT_EQ 4
#define SNMP_CHECK_TYPE_CONTAINS 5


#define PROGNAME "bartlby"
#define REL_NAME_INT "Insomnia"
#define REL_NAME REL_NAME_INT " - rev:" __GIT_VERSION
//#define VERSION  "1.4.0"
#define EXPECTCORE 1600002

#define MAX_CCACHE 1024
#define MAX_CCACHE_KEY 1024
#define MAX_CCACHE_VALUE 1024


#define RECOVERY_OUTSTANDING 1
#define RECOVERY_DONE 0


#define EXTENSION_OK 0
#define EXTENSION_NOK 1


#define EXTENSION_CALLBACK_ALL -1
#define EXTENSION_CALLBACK_PRE_CHECK 1
#define EXTENSION_CALLBACK_POST_CHECK 2
#define EXTENSION_CALLBACK_STATE_CHANGED 3
#define EXTENSION_CALLBACK_TRIGGER_PRE 4
#define EXTENSION_CALLBACK_SCHED_WAIT 5
#define EXTENSION_CALLBACK_UNKOWN_CHECK_TYPE 6
#define EXTENSION_CALLBACK_ROUND_TIME 7
#define EXTENSION_CALLBACK_CHECK_TIME 8
#define EXTENSION_CALLBACK_EVENT_PUSHED 9
#define EXTENSION_CALLBACK_REPLICATION_GO 10
#define EXTENSION_CALLBACK_TRIGGER_FIRED 11
#define EXTENSION_CALLBACK_CHECK_WILL_RUN 12
#define EXTENSION_CALLBACK_SHM_POPULATED 13
#define EXTENSION_CALLBACK_SCRIPT_FIRED 14
#define EXTENSION_CALLBACK_WEBHOOKS_FIRED 15
    

#define PORTIER_CONN_TIMEOUT 30


#define EVENT_STATUS_CHANGED 2
#define EVENT_TRIGGER_PUSHED 3



#define STATE_OK 0
#define STATE_WARNING 1
#define STATE_CRITICAL 2
#define STATE_UNKOWN 3
#define STATE_INFO 4
#define STATE_SIRENE 7
#define STATE_DOWNTIME 8

//Service ACK

#define ACK_NOT_NEEDED 0
#define ACK_NEEDED 1
#define ACK_OUTSTANDING 2

#define DT_SERVICE 1
#define DT_SERVER 2
#define DT_SERVERGROUP 3
#define DT_SERVICEGROUP 4


#define SVC_TYPE_ACTIVE 1
#define SVC_TYPE_PASSIVE 2
#define SVC_TYPE_GROUP 3
#define SVC_TYPE_LOCAL 4
#define SVC_TYPE_SNMP 5
#define SVC_TYPE_NRPE 6
#define SVC_TYPE_NRPE_SSL 7
#define SVC_TYPE_V2 8
#define SVC_TYPE_V2_NO_SSL 9
#define SVC_TYPE_SSH 10
#define SVC_TYPE_TRAP 11
#define SVC_TYPE_JSON 12



#define SVC_THRESHOLD 10

#define PERF_TYPE_SVC_TIME 1
#define PERF_TYPE_ROUND_TIME 2 


#define PASSIVE_TIMEOUT "Passive Service has been timed out"
#define DNS_ERROR "DNS lookup error"
#define SOCKET_CREATE_ERROR "Socket create error"
#define ALARM_ERROR "Alarm setup error"
#define CONN_ERROR "Connection error"
#define RECV_ERROR "Recieve Error"
#define PROTOCOL_ERROR "Protocol Error"
#define TIMEOUT_ERROR "Recv() timedout"




#define GROUP_CRITICAL "Group check critical"
#define GROUP_WITHOUT_PARMS "Group check without parameters"
#define GROUP_OK "Group check OK"

#define SCHED_MODE_WORKER 2
#define SCHED_MODE_FORK 1
#define SCHED_MODE_PROFILE 3


#define SERVICE_UNHANDLED 0
#define SERVICE_HANDLED 1


#define BARTLBY_OBJECT_CHANGED 1
#define BARTLBY_OBJECT_DELETED 2
#define BARTLBY_OBJECT_OUT_OF_SYNC 3


#define NOTIFICATION_VIA_LOCAL 1
#define NOTIFICATION_VIA_UPSTREAM 2


#define BARTLBY_HTTP_FINE CURLE_OK

#define AGENT_V2_SENT_PACKET 1
#define AGENT_V2_RETURN_PACKET 2

#define PORTIER_SVCLIST_PACKET 1
#define PORTIER_RESULT_PACKET 2
#define PORTIER_REQUEST_PACKET 3

    

#define LOAD_SYMBOL(x,y,z) 	x=dlsym(y, z); \
    	if((dlmsg=dlerror()) != NULL) { \
        	_log(LH_MAIN, B_LOG_CRIT,"-Error: %s", dlmsg); \
        	exit(1); \
    	}






struct ext_notify {
	struct service * svc;	
	struct worker * wrk;
	char * trigger;
};
struct service_sort {
	struct service * svc;	
};
struct sched_worker_sort {
	struct sched_worker * th;
};
typedef struct v2_packet_struct{

	u_int32_t crc32_value;
	int16_t   exit_code;
	int16_t   packet_type;
	char      output[2048];
	char      cmdline[2048];
	char      plugin[2048];
	char 	   perf_handler[1024];
	
	 
} agent_v2_packet;

typedef struct port_packet_struct{

	u_int32_t crc32_value;
	int16_t   exit_code;
	int16_t   packet_type;
	char      output[2048];
	char      cmdline[2048];
	char      plugin[2048];
	char 	   perf_handler[1024];
	int32_t	   service_id;
	
	 
} portier_packet;


struct http_output {
	char *ptr;
	size_t len;
	int curl_code;
} iiiip;



typedef struct {
    json_object * json_result;
    int baseline_broken;
} BARTLBY_BASELINE;

typedef struct {
    float top;
    float bottom;
    float val;
    float avg;
    float sum;
    char alg_name[80];

} deviation_alg;


char * getConfigValue_ex(const char * key, const char * fname, int cache);
char * getConfigValue(char *, char *);
int clear_serviceMap(struct service **);
int clear_workerMap(struct worker ** m);

int schedule_loop(char *, void *, void *);
void sched_write_back_all(char * cfgfile, void * shm_addr, void * SOHandle);

void sched_reschedule(struct service * svc);
int service_is_in_time(const char * time_plan);
void bartlby_check_service(struct service * svc, void *, void *, char *);
void bartlby_check_active(struct service * svc, char * cfgfile);
void bartlby_check_local(struct service * svc, char * cfgfile);
void bartlby_check_group(struct service * svc, void * shm_addr);
void bartlby_check_json(struct service * svc, char * cfgfile);

void bartlby_check_snmp(struct service * svc, char * cfgfile);
void bartlby_check_ssh(struct service * svc, char * cfgfile);
void bartlby_check_nrpe(struct service * svc, char * cfgfile, int use_ssl);

void str_mysql_safe(char * str);
void service_mysql_safe(struct service * svc);


//Replication
int replication_go(char *, void *, void *);

//SHM

long GetDowntimeMap(struct downtime * svcs, char * config, int orch_id);
struct service * bartlby_SHM_ServiceMap(void *);
struct sched_threads * bartlby_SHM_ThreadMap(void * shm_addr);
struct downtime * bartlby_SHM_DowntimeMap(void * shm_addr);
struct shm_header * bartlby_SHM_GetHDR(void *);
struct worker * bartlby_SHM_WorkerMap(void * shm_addr);
struct server * bartlby_SHM_ServerMap(void * shm_addr);
struct trap * bartlby_SHM_TrapMap(void * shm_addr);
struct trigger * bartlby_SHM_TriggerMap(void * shm_addr);
struct servicegroup * bartlby_SHM_ServiceGroupMap(void * shm_addr);
struct servergroup * bartlby_SHM_ServerGroupMap(void * shm_addr);

void bartlby_SHM_link_services_servers(void * shm_addr, char * cfgfile, void * SOHandle);

void bartlby_perf_track(struct service * svc,char * return_buffer, int return_bytes, char * cfgfile);
int bartlby_core_perf_track(struct shm_header * hdr, struct service * svc, int type, int time);
long bartlby_milli_timediff(struct timeval end, struct timeval start);

void bartlby_trigger( struct service * svc,
						  char * cfgfile,
						  void * shm_addr,
						  int do_check,
						  int type_of_notification,
						  struct worker * specific_worker,
						  struct trigger * specific_trigger,
						  char * prebuilt_message
						  );

//Global :-)
int _log(int handle, int severity, const char * str,  ...);

void bartlby_decode(char * msg, int length);
void bartlby_encode(char * msg, int length);
char * bartlby_beauty_state(int status);

void bartlby_end_clean(char *cfgfile);
void bartlby_get_daemon(char * cfgfile);
void set_cfg(char * cfg);

void str_replace(char *str, const char *from, const char *to, int maxlen);
void bartlby_replace_svc_in_str(char * str, struct service * svc, int max);

void bartlby_action_handle_reply(struct service * svc, char * rmessage, char * cfgfile);
int bartlby_action_handle_reply_line(struct service * svc, char * line, char *cfgfile);
void bartlby_check_sirene(char * configfile, void * bartlby_address);
int bartlby_is_in_downtime(void * bartlby_address, struct service * svc);
ssize_t recvall(int _socket, char* buffer, int max_len,int flags);


//EVNT's
void bartlby_event_init(void * bartlby_address);
struct btl_event * bartlby_SHM_EventMap(void * shm_addr);
int bartlby_push_event(char * cfgfile,  void * bartlby_address, int event_id, const char * str);

void * bartlby_event_queue_get_hardcopy(void * bartlby_address);
void * bartlby_event_queue_set_hardcopy(void * bartlby_address, void * hardcopy, int event_queue_last);

void bartlby_pre_init(char * cfgfile);

void cfg_init_cache(void);
char * cfg_add_to_cache(const char * k, char * v);
void cfg_fill_with_file(char * f);
void cfg_update_cache(char * k, char * v);
void bartlby_ext_init(void * shm_addr, void * data_loader, char * cfg);
void bartlby_ext_shutdown(int sched_exit_code);
int bartlby_ext_register_callback(int type, void * fcn);
int bartlby_callback(int type, void *data);

void bartlby_fin_service(struct service * svc, void * SOHandle, void * shm_addr,char * cfgfile);

void bartlby_check_grep_perf_line(char * l, struct service * svc, char * cfgfile);

extern char config_file[255];

int bartlby_tcp_recvall(int s, char *buf, int *len, int timeout);
int bartlby_tcp_sendall(int s, char *buf, int *len);

void bartlby_log_usage(void);

void nrpe_display_license(void);
void bartlby_check_v2(struct service * svc, char * cfgfile, int use_ssl);


int sched_servergroup_notify(struct server * srv);
int sched_servergroup_active(struct server * srv);
int sched_servicegroup_notify(struct service * svc);
int sched_servicegroup_active(struct service * svc);

int bartlby_agent_tcp_my_connect(char *host_name,int port,int *sd,char *proto, struct service * svc);
int bartlby_agent_tcp_connect(char *host_name,int port,int *sd, struct service * svc);


char *remove_nl_copy(char *s);

//Orchestra
int bartlby_orchestra_get_id(char * cfgfile);
void bartlby_orchestra_init(struct shm_header * shmhdr);
int bartlby_orchestra_belongs_to_orch(struct service * svc, char * cfgfile);
int bartlby_orchestra_trap_belongs_to_orch(struct trap * trap, char * cfgfile);
void bartlby_orchestra_check_timeouts(struct service * svcmap,struct shm_header * hdr, char * cfgfile,void * shm_addr, void * SOHandle);
void bartlby_orchestra_send_svc(char * cfg, struct service * svc);
void bartlby_orchestra_upstream_log(char * cfg, char * log_line);
//Notification Log
int bartlby_notification_log_last_notification_state(struct shm_header * shmhdr, char * cfgfile, long svc_id, long worker_id, struct trigger * trig);
void * bartlby_notification_log_set_hardcopy(struct shm_header * shmhdr, void * hardcopy, long notification_log_current_top, time_t notification_log_last_run);
void * bartlby_notification_log_get_hardcopy(struct shm_header * shmhdr);
void bartlby_notification_log_finish(struct shm_header * shmhdr);
void bartlby_notification_log_init(struct shm_header * shmhdr);
void bartlby_notification_log_add(struct shm_header * shmhdr, char * cfgfile, long worker_id, long service_id, int state, int type, int aggregation_interval, struct trigger * trig, int received_via);
void bartlby_notification_log_aggregate(void * bartlby_address, struct shm_header *shmdr, char * cfgfile);
void bartlby_notification_log_debug(struct shm_header * shmhdr);

/*** HTTP REQUEST STUFF **/
struct http_output * bartlby_http_post_request(char *url, char *body, long timeout);
void bartlby_free_http_output(struct http_output * s);

/**WEBHOOKS **/
void bartlby_call_webhooks(char * cfg, struct service * svc, int timeout, char * hooks_in, json_object * additional_json);

/**** JSON **/
json_object * bartlby_service_to_json(struct service * svc);
json_object * bartlby_trigger_to_json(struct trigger * trig);
int bartlby_json_string_in_array(json_object * array, char * str);

/*** TRIGGER*/
int bartlby_servicegroup_has_trigger(struct service * svc, char * trigger);
int bartlby_servergroup_has_trigger(struct server * srv, char * trigger);
int bartlby_trigger_worker_level(struct worker * w,  struct service * svc, int node_id);
void bartlby_trigger_upstream(char * cfgfile, int has_local_users, int type_of_notification, struct trigger * trig, struct service * svc);
int bartlby_worker_has_service(struct worker * w, struct service * svc, char * cfgfile, int node_id);
int bartlby_trigger_escalation(struct worker *w, struct service * svc, int standby_workers_only, int node_id);

int bartlby_trigger_per_worker(char * cfgfile,
									void * shm_addr,
									struct worker * wrk,
									struct trigger * trig,
									struct service * svc,
									int type_of_notification,
									int do_check,
									int received_via,
									int upstream_enabled,
									int upstream_has_local_users,
									char * find_trigger,
									char * prebuilt_message);


struct service *  bartlby_notification_log_get_service(void * bartlby_address, long service_id);
struct trigger *  bartlby_notification_log_get_trigger(void * bartlby_address, long trigger_id);
struct worker *  bartlby_notification_log_get_worker(void * bartlby_address, long worker_id);

/*
 baseline
*/

void bartlby_baseline_destroy(BARTLBY_BASELINE * bsl);
json_object * bartlby_baseline_parse_perf_data(char * string);
float bartlby_baseline_standard_deviation(float data[],  int n);
long  bartlby_baseline_append_day_data_from_stathistory(long svc_id, 
                                 time_t work_on,
                                 int tolerance_window_start,
                                 int tolerance_window_end,
                                 json_object * last_records,
                                 char * cfg
                                 );

long bartlby_baseline_append_day_data(long svc_id, 
                                 time_t work_on,
                                 int tolerance_window_start,
                                 int tolerance_window_end,
                                 json_object * last_records,
                                 char * cfg,
                                 int source_type);

void bartlby_baseline_deviation_exp_smooth(float  baseline_values[], int baseline_value_count, float tolerance, json_object * data_point, deviation_alg * alg_out);
void bartlby_baseline_deviation_std(float  baseline_values[], int baseline_value_count, float tolerance, json_object * data_point,deviation_alg * alg_out);
void bartlby_baseline_deviation(float  baseline_values[], int baseline_value_count, float tolerance, json_object * data_point, deviation_alg * alg_out, int statistic_algo);

BARTLBY_BASELINE * bartlby_calculate_baseline(long svc_id,
                              BARTLBY_BASELINE * result_baseline,
                              int days_back,
                              int time_tolerance,
                              int value_tolerance,
                              char * service_output,
                              char * cfg,
                              int source_type,
                              int statistic_algo,
                              int min_records,
                              int max_records,
                              json_object * include_keys);
BARTLBY_BASELINE * bartlby_check_baseline(struct service * svc, char * config_payload, char * config, void * bartlby_address);


/*
traps
*/
int bartlby_submit_trap(const char * trap_data, void * bartlby_address, char * cfgfile);
/* lua */
int bartlby_finish_script(struct service * svc, char * script);
int bartlby_trigger_script(struct service * svc, char * script);


/*
trigger
*/
void bartlby_call_single_webhook(char * cfg,char * endpoint,  json_object * jso_out, int timeout);
void bartlby_trigger_local(char * cfgfile, struct service *svc, struct worker *wrk, struct trigger *trig, char *msg);
void bartlby_trigger_webhooks(char * cfgfile, struct service *svc, struct worker *wrk, struct trigger *trig, char *msg);
void bartlby_trigger_smtp(char * cfgfile, struct service *svc, struct worker *wrk, struct trigger *trig, char *msg);
void bartlby_trigger_lua(char * cfgfile, struct service *svc, struct worker *wrk, struct trigger *trig, char *msg);


int bartlby_lua_trigger_run(struct service * svc, struct worker * wrk, struct trigger * trig, char * script, char * msg);

/*
PORTIER
*/
int bartlby_portier_connect(char *host_name,int port, int spool, char * cfgfile);
int bartlby_portier_send_trigger(char * passive_host, int passive_port, int type_of_notification,struct trigger * trig, struct service * svc, int node_id, char * portier_passwd, char * cfgfile);
int bartlby_portier_send_svc_status(char * passive_host, int passive_port, char * passwd, struct service * svc, char * cfgfile);
int bartlby_portier_send(json_object * obj, int sock, int spool);
void bartlby_portier_disconnect(int sock, int spool);
char * bartlby_portier_fetch_reply(int sock, int spool);
int bartlby_portier_send_log(char * passive_host, int passive_port, char * passwd, char  * log_line, char * cfgfile);
//COMPAT
#ifdef NEEDS_JSON_GET_EX
int json_object_object_get_ex(struct json_object* jso, const char *key, struct json_object **value);
#endif

#ifdef NEEDS_JSON_INT64
struct json_object* json_object_new_int64(int64_t i);
int32_t json_object_get_int64(struct json_object *obj);

#endif



#ifdef NEEDS_JSON_OBJECT_ITER
struct json_object_iter
{
	char *key;
	struct json_object *val;
	struct lh_entry *entry;
};
#endif

#ifndef json_object_object_foreachC
#define json_object_object_foreachC(obj,iter)							\
	for(iter.entry = json_object_get_object(obj)->head; (iter.entry ? (iter.key = (char*)iter.entry->k, iter.val = (struct json_object*)iter.entry->v, iter.entry) : 0); iter.entry = iter.entry->next)
#endif

void trim(char * const a);

