#include "config.h"

///*** asprintf on clang */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
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
#include <sys/prctl.h>
#include <mysql/mysql.h>
#include <sys/times.h> 
#include <semaphore.h>
#include <json.h>
#include <regex.h>
#include <curl/curl.h>


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

	
#define B_LOG_DEBUG 0
#define B_LOG_INFO 1
#define B_LOG_WARN 2
#define B_LOG_CRIT 3
#define B_LOG_HASTO 4



#define CHECKED_ASPRINTF(...)                                       \
    if (asprintf( __VA_ARGS__ ) == -1) {                             \
       _debug("ASPRINTF FAILED");\
    }

#define CHECKED_VASPRINTF(...)                                       \
    if (vasprintf( __VA_ARGS__ ) == -1) {                             \
       _debug("ASPRINTF FAILED");\
    }


static char * log_handles[] = {"DEBUG", "TRIGGER", "SCHED", "CHECK", "MAIN", "SHM", "DAEMON", "PERF", "ACK", "EVNT", "EXT", "NOTIFYLOG", "LIB", "PORTIER", "MOD", "ORCH"};
static char * log_levels[] = {"DEBUG", "INFO", "WARN", "CRIT", "HASTO"};

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


#ifdef HAVE_SSL 
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
#define EXPECTCORE 1600008

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


#define PORTIER_CONN_TIMEOUT 30

#define EVENT_QUEUE_MAX 128
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

#define MAX_GROUP_MEMBERS 512
#define GROUP_MEMBER_STR_LENGTH 2048



#define GROUP_CRITICAL "Group check critical"
#define GROUP_WITHOUT_PARMS "Group check without parameters"
#define GROUP_OK "Group check OK"

#define SCHED_MODE_WORKER 2
#define SCHED_MODE_FORK 1
#define SCHED_MODE_PROFILE 3


#define SERVICE_UNHANDLED 0;
#define SERVICE_HANDLED 1;


#define BARTLBY_OBJECT_CHANGED 1
#define BARTLBY_OBJECT_DELETED 2
#define BARTLBY_OBJECT_OUT_OF_SYNC 3




#define LOAD_SYMBOL(x,y,z) 	x=dlsym(y, z); \
    	if((dlmsg=dlerror()) != NULL) { \
        	_log(LH_MAIN, B_LOG_CRIT,"-Error: %s", dlmsg); \
        	exit(1); \
    	}




struct shm_counter {
	long worker;
	long services;
	long downtimes;
	long servers;	
	long servergroups;
	long servicegroups;
	long traps;
};

struct perf_statistic {
	long sum;
	long counter;	
};


struct snmpi {
	char community[512];
	int version;
	char objid[1024];
	long warn;
	long crit;
	long type;
	char textmatch[1024];
};
struct sprocess {
	int start_time;
	int pid;
		
};

struct sched_worker {
	int pid;
	struct service * svc;
	int start_time;
	int  idle;
	int shutdown;
	struct tms timing;
	int idx;


} astt;

struct notification_log_entry {
	int notification_valid; //-1 invalid == end of list
	long worker_id; //Worker id
	long service_id; //Service_id
	int state; //State
	int aggregated; //Default -1 > 0 - this notification has already been aggregated
	char trigger_name[512];
	int type; // 0 if it was a normal notification, 1 = it was a escalation notification to the standby's
	time_t time;
	int aggregation_interval;
	int received_via;
};
#define NOTIFICATION_LOG_MAX 512
#define NOTIFICATION_VIA_LOCAL 1
#define NOTIFICATION_VIA_UPSTREAM 2



struct shm_header {
	long size_of_structs;
	long thrdcount;
	long svccount;
	long wrkcount;
	long srvcount;
	long srvgroupcount;
	long svcgroupcount;
	long trapcount;
	long current_running;
	char  version[50];
	int do_reload;
	int last_replication;
	int startup_time;
	long dtcount;
	int sirene_mode;
	struct perf_statistic pstat;
	int cur_event_index;
	long checks_performed;
	int checks_performed_time;
	struct  sched_worker worker_threads[50];
	struct notification_log_entry notification_log[NOTIFICATION_LOG_MAX];
	long notification_log_current_top;	
	time_t notification_log_aggregate_last_run;
	
};

struct server {
	long server_id;
	char  client_ip[2048];
	char  server_name[2048];
	char server_icon[1024];
	int server_enabled;
	int client_port;
	int server_dead;
	int server_notify;
	long server_flap_seconds;
	int flap_count;
	int last_notify_send;
	
	struct service * dead_marker;
	int is_gone;
	
	struct servergroup * servergroups[MAX_GROUP_MEMBERS];
	long servergroup_counter;
	long servergroup_place[MAX_GROUP_MEMBERS];
	
	char server_ssh_keyfile[512];
	char server_ssh_passphrase[512];
	char server_ssh_username[512];
	
	char enabled_triggers[512];
	int default_service_type;
	int orch_id;
	char exec_plan[2048];
	char web_hooks[1024];
	char json_endpoint[256];
} xxyz;


struct trap {
    long trap_id;
    char trap_name[512];
    char trap_catcher[512];
    char trap_status_text[512];
    char trap_status_ok[512];
    char trap_status_warning[512];
    char trap_status_critical[512];
    long trap_service_id;
    long service_shm_place;
    int trap_fixed_status;
    int trap_prio;
    int trap_is_final;
    int orch_id;
    int is_gone;
    int matched;
    int trap_last_match;
    char trap_last_data[2048];
} zzk;

struct service {
	long service_id;
	long server_id;
	int last_state;
	int current_state;
	
	char  new_server_text[2048];
	char  service_name[2048];
	
	char  plugin[2048];
	char  plugin_arguments[2048];
	long check_interval;
	long check_interval_original;
	int last_check;
	struct timeval lcheck;
	
	/*Time stuff*/
	
	char service_exec_plan[2048];
	
	/*Notify things*/
	int notify_enabled;
	int last_notify_send;
	int last_state_change;
	long flap_count;
	
	int service_active;
	
	char  service_var[2048];
	int service_type;
	long service_passive_timeout;
	
	int notify_last_state;
	
	long service_check_timeout;
	
	
	
	int service_ack_enabled;
	int service_ack_current;
	
	
	long service_retain;
	long service_retain_current;
	
	
	
	struct perf_statistic pstat;
	struct perf_statistic delay_time;
	
	int do_force;
	
	struct snmpi snmp_info;
	
	int recovery_outstanding; //Flag to see if recover is waiting
	
	struct sprocess process;
	
	long flap_seconds;
	
	struct server * srv;
	long srv_place;
	
	int is_server_dead;
	
	/**/
	long renotify_interval; // interval to renotify
	long escalate_divisor; //

	int is_gone;
	
	
	struct servicegroup * servicegroups[MAX_GROUP_MEMBERS];
	long servicegroup_counter;
	long servicegroup_place[MAX_GROUP_MEMBERS];
	
	long fires_events;
	
	char enabled_triggers[512];

	int handled;
	int orch_id;
	int last_orch_sync;

	char usid[50];
	int prio;
	int notify_super_users;
};

struct servicegroup {
	long servicegroup_id;
	char servicegroup_name[1024];
	int servicegroup_notify;
	int servicegroup_active;
	char servicegroup_members[GROUP_MEMBER_STR_LENGTH];
	
	int servicegroup_dead;
	struct service * dead_marker;
	
	char enabled_triggers[512];
	int orch_id;
};

struct servergroup {
	long servergroup_id;
	char servergroup_name[1024];
	int servergroup_notify;
	int servergroup_active;
	char servergroup_members[GROUP_MEMBER_STR_LENGTH];
	int servergroup_dead;
	struct service * dead_marker;
	char enabled_triggers[512];
	int orch_id;
};


struct service_sort {
	struct service * svc;	
};
struct sched_worker_sort {
	struct sched_worker * th;
};


struct worker {
	char name[2048];
	char  mail[2048];
	char  icq[2048];
	
	
	long worker_id;
	int active;
	char notify_plan[2048];
	char password[2048];
	
	int escalation_count;
	int escalation_time;
	char notify_levels[20];
	char enabled_triggers[2048];
	char t[500];
	long escalation_limit;
	long escalation_minutes;
	int is_gone;

	char  visible_services[2048];
	char  visible_servers[2048];
	char  selected_services[2048];
	char  selected_servers[2048];
	int  notification_aggregation_interval;
	int is_super_user;

	int orch_id;

	char api_pubkey[255];
	char api_privkey[255];
	int api_enabled;
}sa;


struct downtime {
	long downtime_id;
	int downtime_type;
	int downtime_from;
	int downtime_to;
	char downtime_notice[2048];
	int service_id;
	
	int is_gone;

	int orch_id;
	
}sb;

struct btl_event {
	long evnt_id;
	char evnt_message[1024];
	int evnt_time;
		
}eb;
struct ext_notify {
	struct service * svc;	
	struct worker * wrk;
	char * trigger;
} ty;



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

#define BARTLBY_HTTP_FINE CURLE_OK

#define AGENT_V2_SENT_PACKET 1
#define AGENT_V2_RETURN_PACKET 2

#define PORTIER_SVCLIST_PACKET 1
#define PORTIER_RESULT_PACKET 2
#define PORTIER_REQUEST_PACKET 3


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


void bartlby_check_snmp(struct service * svc, char * cfgfile);
void bartlby_check_ssh(struct service * svc, char * cfgfile);
void bartlby_check_nrpe(struct service * svc, char * cfgfile, int use_ssl);

void str_mysql_safe(char * str);
void service_mysql_safe(struct service * svc);


//Replication
int replication_go(char *, void *, void *);

//SHM

int GetDowntimeMap(struct downtime * svcs, char * config, int orch_id);
struct service * bartlby_SHM_ServiceMap(void *);
struct sched_threads * bartlby_SHM_ThreadMap(void * shm_addr);
struct downtime * bartlby_SHM_DowntimeMap(void * shm_addr);
struct shm_header * bartlby_SHM_GetHDR(void *);
struct worker * bartlby_SHM_WorkerMap(void * shm_addr);
struct server * bartlby_SHM_ServerMap(void * shm_addr);
struct trap * bartlby_SHM_TrapMap(void * shm_addr);
struct servicegroup * bartlby_SHM_ServiceGroupMap(void * shm_addr);
struct servergroup * bartlby_SHM_ServerGroupMap(void * shm_addr);

void bartlby_SHM_link_services_servers(void * shm_addr, char * cfgfile, void * SOHandle);

void bartlby_perf_track(struct service * svc,char * return_buffer, int return_bytes, char * cfgfile);
int bartlby_core_perf_track(struct shm_header * hdr, struct service * svc, int type, int time);
long bartlby_milli_timediff(struct timeval end, struct timeval start);

void bartlby_trigger(struct service * svc, char * cfgfile, void * shm_addr, int do_check, int standby_workers_only);
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
int bartlby_push_event(int event_id, const char * str,  ...);

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
int bartlby_notification_log_last_notification_state(struct shm_header * shmhdr, char * cfgfile, long svc_id, long worker_id, char * trigger_name);
void * bartlby_notification_log_set_hardcopy(struct shm_header * shmhdr, void * hardcopy, long notification_log_current_top, time_t notification_log_last_run);
void * bartlby_notification_log_get_hardcopy(struct shm_header * shmhdr);
void bartlby_notification_log_finish(struct shm_header * shmhdr);
void bartlby_notification_log_init(struct shm_header * shmhdr);
void bartlby_notification_log_add(struct shm_header * shmhdr, char * cfgfile, long worker_id, long service_id, int state, int type, int aggregation_interval, char * trigger_name, int received_via);
void bartlby_notification_log_aggregate(struct shm_header *shmdr, char * cfgfile);
void bartlby_notification_log_debug(struct shm_header * shmhdr);

/*** HTTP REQUEST STUFF **/
struct http_output * bartlby_http_post_request(char *url, char *body, long timeout);
void bartlby_free_http_output(struct http_output * s);

/*** TRIGGER*/
int bartlby_servicegroup_has_trigger(struct service * svc, char * trigger);
int bartlby_servergroup_has_trigger(struct server * srv, char * trigger);
int bartlby_trigger_worker_level(struct worker * w,  struct service * svc, int node_id);
void bartlby_trigger_upstream(char * cfgfile, int has_local_users, int to_standbys, char * trigger_name, char * cmdl, struct service * svc);
int bartlby_worker_has_service(struct worker * w, struct service * svc, char * cfgfile, int node_id);
int bartlby_trigger_escalation(struct worker *w, struct service * svc, int standby_workers_only, int node_id);

int bartlby_trigger_per_worker(char * cfgfile, char * trigger_name, struct shm_header *hdr, struct worker * wrk, struct server * srvmap, int do_check, struct service * svc, char * find_trigger, int standby_workers_only, char * full_path, int upstream_enabled, int upstream_has_local_users, char * notify_msg, int received_via);

/*
PORTIER
*/
int bartlby_portier_connect(char *host_name,int port, int spool, char * cfgfile);
int bartlby_portier_send_trigger(char * passive_host, int passive_port, int to_standbys,char * trigger_name, char * execline, struct service * svc, int node_id, char * portier_passwd, char * cfgfile);
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

