#define NOTIFICATION_LOG_MAX 512
#define MAX_GROUP_MEMBERS 512
#define GROUP_MEMBER_STR_LENGTH 2048
#define EVENT_QUEUE_MAX 128


#define BARTLBY_FIELD_LONG(name) long name;
#define BARTLBY_FIELD_LONG_SIZE(name, size) long name[size];
#define BARTLBY_FIELD_INT(name) int name;
#define BARTLBY_FIELD_CHAR_SIZE(name, size) char name[size];
#define BARTLBY_FIELD_TIMEVAL(name) struct timeval name;
#define BARTLBY_FIELD_TMS(name) struct tms name;
#define BARTLBY_FIELD_PERFSTAT(name) struct perf_statistic name;
#define BARTLBY_FIELD_SNMPI(name) struct snmpi name;
#define BARTLBY_FIELD_SPROCESS(name) struct sprocess name;
#define BARTLBY_FIELD_SERVER_P(name) struct server * name;
#define BARTLBY_FIELD_SERVICE_P(name) struct service * name;
#define BARTLBY_FIELD_SERVICEGROUP_SIZE(name,size) struct servicegroup * name[size];
#define BARTLBY_FIELD_SERVERGROUP_SIZE(name,size) struct servergroup * name[size];
#define BARTLBY_FIELD_TIMET(name) time_t name;  
#define BARTLBY_FIELD_SCHED_WORKER_SIZE(name, size) struct sched_worker name[size];    
#define BARTLBY_FIELD_NOTIFICATION_LOG_ENTRY_SIZE(name, size) struct notification_log_entry name[size];


#define BARTLBY_SERVICE_FIELDS \
        BARTLBY_FIELD_LONG(service_id) \
        BARTLBY_FIELD_LONG(server_id) \
        BARTLBY_FIELD_INT(last_state) \
        BARTLBY_FIELD_INT(current_state) \
        BARTLBY_FIELD_CHAR_SIZE(current_output,2048) \
        BARTLBY_FIELD_CHAR_SIZE(service_name,2048) \
        BARTLBY_FIELD_CHAR_SIZE(plugin,2048) \
        BARTLBY_FIELD_CHAR_SIZE(plugin_arguments, 2048) \
        BARTLBY_FIELD_LONG(check_interval) \
        BARTLBY_FIELD_LONG(check_interval_original) \
        BARTLBY_FIELD_INT(last_check) \
        BARTLBY_FIELD_TIMEVAL(lcheck) \
        BARTLBY_FIELD_CHAR_SIZE(service_exec_plan,2048) \
        BARTLBY_FIELD_INT(notify_enabled) \
        BARTLBY_FIELD_INT(last_notify_send) \
        BARTLBY_FIELD_INT(last_state_change) \
        BARTLBY_FIELD_LONG(flap_count) \
        BARTLBY_FIELD_INT(service_active) \
        BARTLBY_FIELD_CHAR_SIZE(service_var,2048) \
        BARTLBY_FIELD_INT(service_type) \
        BARTLBY_FIELD_LONG(service_passive_timeout) \
        BARTLBY_FIELD_INT(notify_last_state) \
        BARTLBY_FIELD_LONG(service_check_timeout) \
        BARTLBY_FIELD_INT(service_ack_enabled) \
        BARTLBY_FIELD_INT(service_ack_current) \
        BARTLBY_FIELD_LONG(service_retain) \
        BARTLBY_FIELD_LONG(service_retain_current) \
        BARTLBY_FIELD_PERFSTAT(pstat) \
        BARTLBY_FIELD_PERFSTAT(delay_time) \
        BARTLBY_FIELD_INT(do_force) \
        BARTLBY_FIELD_SNMPI(snmp_info) \
        BARTLBY_FIELD_INT(recovery_outstanding) \
        BARTLBY_FIELD_SPROCESS(process) \
        BARTLBY_FIELD_LONG(flap_seconds) \
        BARTLBY_FIELD_SERVER_P(srv) \
        BARTLBY_FIELD_LONG(srv_place) \
        BARTLBY_FIELD_INT(is_server_dead) \
        BARTLBY_FIELD_LONG(renotify_interval) \
        BARTLBY_FIELD_LONG(escalate_divisor) \
        BARTLBY_FIELD_INT( is_gone) \
        BARTLBY_FIELD_SERVICEGROUP_SIZE(servicegroups, MAX_GROUP_MEMBERS) \
        BARTLBY_FIELD_LONG(servicegroup_counter) \
        BARTLBY_FIELD_LONG_SIZE(servicegroup_place, MAX_GROUP_MEMBERS) \
        BARTLBY_FIELD_LONG(fires_events) \
        BARTLBY_FIELD_CHAR_SIZE(enabled_triggers,512) \
        BARTLBY_FIELD_INT(handled) \
        BARTLBY_FIELD_INT(orch_id) \
        BARTLBY_FIELD_INT(last_orch_sync) \
        BARTLBY_FIELD_CHAR_SIZE(usid, 50) \
        BARTLBY_FIELD_INT(prio) \
        BARTLBY_FIELD_INT(notify_super_users) \
        BARTLBY_FIELD_CHAR_SIZE(script, 2048) \
        BARTLBY_FIELD_INT(script_enabled) \

    

#define BARTLBY_SHM_COUNTER_FIELDS \
        BARTLBY_FIELD_LONG(worker) \
        BARTLBY_FIELD_LONG(services) \
        BARTLBY_FIELD_LONG(downtimes) \
        BARTLBY_FIELD_LONG(servers) \
        BARTLBY_FIELD_LONG(servergroups) \
        BARTLBY_FIELD_LONG(servicegroups) \
        BARTLBY_FIELD_LONG(traps) \
        BARTLBY_FIELD_LONG(triggers) \



#define BARTLBY_PERFSTATISTICS_FIELDS \
        BARTLBY_FIELD_LONG(sum) \
        BARTLBY_FIELD_LONG(counter) \

#define BARTLBY_SNMPI_FIELDS \
        BARTLBY_FIELD_CHAR_SIZE(community, 512) \
        BARTLBY_FIELD_INT(version) \
        BARTLBY_FIELD_CHAR_SIZE(objid, 1024) \
        BARTLBY_FIELD_LONG(warn) \
        BARTLBY_FIELD_LONG(crit) \
        BARTLBY_FIELD_LONG(type) \
        BARTLBY_FIELD_CHAR_SIZE(textmatch, 1024) \

#define BARTLBY_SPROCESS_FIELDS \
        BARTLBY_FIELD_INT(start_time) \
        BARTLBY_FIELD_INT(pid) \


#define BARTLBY_SCHEDWORKER_FIELDS \
        BARTLBY_FIELD_INT(pid) \
        BARTLBY_FIELD_SERVICE_P(svc) \
        BARTLBY_FIELD_INT(start_time) \
        BARTLBY_FIELD_INT(idle) \
        BARTLBY_FIELD_INT(shutdown) \
        BARTLBY_FIELD_TMS(timing) \
        BARTLBY_FIELD_INT(idx) \
        BARTLBY_FIELD_LONG(svc_id) \
        BARTLBY_FIELD_LONG(memory_used) \
        

//FIXME REMOVE trigger_name
#define BARTLBY_NOTIFICATION_LOG_ENTRY_FIELDS \
	BARTLBY_FIELD_INT(notification_valid) \
	BARTLBY_FIELD_LONG(worker_id) \
	BARTLBY_FIELD_LONG(service_id) \
	BARTLBY_FIELD_INT(state) \
	BARTLBY_FIELD_INT(aggregated) \
	BARTLBY_FIELD_LONG(trigger_id) \
	BARTLBY_FIELD_INT(type) \
	BARTLBY_FIELD_TIMET(time) \
	BARTLBY_FIELD_INT(aggregation_interval) \
	BARTLBY_FIELD_INT(received_via) \


#define BARTLBY_SHM_HEADER_FIELDS \
	BARTLBY_FIELD_LONG(size_of_structs) \
	BARTLBY_FIELD_LONG(thrdcount) \
	BARTLBY_FIELD_LONG(svccount) \
	BARTLBY_FIELD_LONG(wrkcount) \
	BARTLBY_FIELD_LONG(srvcount) \
	BARTLBY_FIELD_LONG(srvgroupcount) \
	BARTLBY_FIELD_LONG(svcgroupcount) \
	BARTLBY_FIELD_LONG(trapcount) \
	BARTLBY_FIELD_LONG(triggercount) \
	BARTLBY_FIELD_LONG(current_running) \
	BARTLBY_FIELD_CHAR_SIZE(version, 50) \
	BARTLBY_FIELD_INT(do_reload) \
	BARTLBY_FIELD_INT(last_replication) \
	BARTLBY_FIELD_INT(startup_time) \
	BARTLBY_FIELD_LONG(dtcount) \
	BARTLBY_FIELD_INT(sirene_mode) \
	BARTLBY_FIELD_PERFSTAT(pstat) \
	BARTLBY_FIELD_INT(cur_event_index) \
	BARTLBY_FIELD_LONG(checks_performed) \
	BARTLBY_FIELD_INT(checks_performed_time) \
	BARTLBY_FIELD_SCHED_WORKER_SIZE(worker_threads, 50) \
	BARTLBY_FIELD_NOTIFICATION_LOG_ENTRY_SIZE(notification_log, NOTIFICATION_LOG_MAX) \
	BARTLBY_FIELD_LONG(notification_log_current_top) \
	BARTLBY_FIELD_TIMET(notification_log_aggregate_last_run) \
	BARTLBY_FIELD_INT(sched_workers_count) \


#define BARTLBY_SERVER_FIELDS \
	BARTLBY_FIELD_LONG(server_id) \
	BARTLBY_FIELD_CHAR_SIZE(client_ip, 2048) \
	BARTLBY_FIELD_CHAR_SIZE(server_name, 2048) \
	BARTLBY_FIELD_CHAR_SIZE(server_icon, 1024) \
	BARTLBY_FIELD_INT(server_enabled) \
	BARTLBY_FIELD_INT(client_port) \
	BARTLBY_FIELD_INT(server_dead) \
	BARTLBY_FIELD_INT(server_notify) \
	BARTLBY_FIELD_LONG(server_flap_seconds) \
	BARTLBY_FIELD_INT(flap_count) \
	BARTLBY_FIELD_INT(last_notify_send) \
	BARTLBY_FIELD_SERVICE_P(dead_marker) \
	BARTLBY_FIELD_INT(is_gone) \
	BARTLBY_FIELD_SERVERGROUP_SIZE(servergroups, MAX_GROUP_MEMBERS) \
	BARTLBY_FIELD_LONG(servergroup_counter) \
	BARTLBY_FIELD_LONG_SIZE(servergroup_place, MAX_GROUP_MEMBERS) \
	BARTLBY_FIELD_CHAR_SIZE(server_ssh_keyfile, 512) \
	BARTLBY_FIELD_CHAR_SIZE(server_ssh_passphrase, 512) \
	BARTLBY_FIELD_CHAR_SIZE(server_ssh_username, 512) \
	BARTLBY_FIELD_CHAR_SIZE(enabled_triggers, 512) \
	BARTLBY_FIELD_INT(default_service_type) \
	BARTLBY_FIELD_INT(orch_id) \
	BARTLBY_FIELD_CHAR_SIZE(exec_plan, 2048) \
	BARTLBY_FIELD_CHAR_SIZE(web_hooks, 1024) \
	BARTLBY_FIELD_CHAR_SIZE(json_endpoint, 256) \
	BARTLBY_FIELD_INT(web_hooks_level) \

#define BARTLBY_TRAP_FIELDS \
	BARTLBY_FIELD_LONG(trap_id) \
	BARTLBY_FIELD_CHAR_SIZE(trap_name, 512) \
	BARTLBY_FIELD_CHAR_SIZE(trap_catcher, 512) \
	BARTLBY_FIELD_CHAR_SIZE(trap_status_text, 512) \
	BARTLBY_FIELD_CHAR_SIZE(trap_status_ok, 512) \
	BARTLBY_FIELD_CHAR_SIZE(trap_status_warning, 512) \
	BARTLBY_FIELD_CHAR_SIZE(trap_status_critical, 512) \
	BARTLBY_FIELD_LONG(trap_service_id) \
	BARTLBY_FIELD_LONG(service_shm_place) \
	BARTLBY_FIELD_INT(trap_fixed_status) \
	BARTLBY_FIELD_INT(trap_prio) \
	BARTLBY_FIELD_INT(trap_is_final) \
	BARTLBY_FIELD_INT(orch_id) \
	BARTLBY_FIELD_INT(is_gone) \
	BARTLBY_FIELD_INT(matched) \
	BARTLBY_FIELD_INT(trap_last_match) \
	BARTLBY_FIELD_CHAR_SIZE(trap_last_data, 2048) \

#define BARTLBY_SERVICEGROUP_FIELDS \
	BARTLBY_FIELD_LONG(servicegroup_id) \
	BARTLBY_FIELD_CHAR_SIZE(servicegroup_name, 1024) \
	BARTLBY_FIELD_INT(servicegroup_notify) \
	BARTLBY_FIELD_INT(servicegroup_active) \
	BARTLBY_FIELD_CHAR_SIZE(servicegroup_members, GROUP_MEMBER_STR_LENGTH) \
	BARTLBY_FIELD_INT(servicegroup_dead) \
	BARTLBY_FIELD_SERVICE_P(dead_marker) \
	BARTLBY_FIELD_CHAR_SIZE(enabled_triggers, 512) \
	BARTLBY_FIELD_INT(orch_id) \


#define BARTLBY_SERVERGROUP_FIELDS \
	BARTLBY_FIELD_LONG(servergroup_id) \
	BARTLBY_FIELD_CHAR_SIZE(servergroup_name, 1024) \
	BARTLBY_FIELD_INT(servergroup_notify) \
	BARTLBY_FIELD_INT(servergroup_active) \
	BARTLBY_FIELD_CHAR_SIZE(servergroup_members, GROUP_MEMBER_STR_LENGTH) \
	BARTLBY_FIELD_INT(servergroup_dead) \
	BARTLBY_FIELD_SERVICE_P(dead_marker) \
	BARTLBY_FIELD_CHAR_SIZE(enabled_triggers, 512) \
	BARTLBY_FIELD_INT(orch_id) \


#define BARTLBY_WORKER_FIELDS \
	BARTLBY_FIELD_CHAR_SIZE(name, 2048) \
	BARTLBY_FIELD_CHAR_SIZE(mail, 2048) \
	BARTLBY_FIELD_CHAR_SIZE(icq, 2048) \
	BARTLBY_FIELD_LONG(worker_id) \
	BARTLBY_FIELD_INT(active) \
	BARTLBY_FIELD_CHAR_SIZE(notify_plan, 2048) \
	BARTLBY_FIELD_CHAR_SIZE(password, 2048) \
	BARTLBY_FIELD_INT(escalation_count) \
	BARTLBY_FIELD_INT(escalation_time) \
	BARTLBY_FIELD_CHAR_SIZE(notify_levels, 20) \
	BARTLBY_FIELD_CHAR_SIZE(enabled_triggers, 2048) \
	BARTLBY_FIELD_CHAR_SIZE(t, 500) \
	BARTLBY_FIELD_LONG(escalation_limit) \
	BARTLBY_FIELD_LONG(escalation_minutes) \
	BARTLBY_FIELD_INT(is_gone) \
	BARTLBY_FIELD_CHAR_SIZE(visible_services, 2048) \
	BARTLBY_FIELD_CHAR_SIZE(visible_servers, 2048) \
	BARTLBY_FIELD_CHAR_SIZE(selected_services, 2048) \
	BARTLBY_FIELD_CHAR_SIZE(selected_servers, 2048) \
	BARTLBY_FIELD_INT(notification_aggregation_interval) \
	BARTLBY_FIELD_INT(is_super_user) \
	BARTLBY_FIELD_INT(orch_id) \
	BARTLBY_FIELD_CHAR_SIZE(api_pubkey, 255) \
	BARTLBY_FIELD_CHAR_SIZE(api_privkey, 255) \
	BARTLBY_FIELD_INT(api_enabled) \
	

#define BARTLBY_DOWNTIME_FIELDS \
	BARTLBY_FIELD_LONG(downtime_id) \
	BARTLBY_FIELD_INT(downtime_type) \
	BARTLBY_FIELD_INT(downtime_from) \
	BARTLBY_FIELD_INT(downtime_to) \
	BARTLBY_FIELD_CHAR_SIZE(downtime_notice, 2048) \
	BARTLBY_FIELD_INT(service_id) \
	BARTLBY_FIELD_INT(is_gone) \
	BARTLBY_FIELD_INT(orch_id) \

#define BARTLBY_BTL_EVENT_FIELDS \
	BARTLBY_FIELD_LONG(evnt_id) \
	BARTLBY_FIELD_CHAR_SIZE(evnt_message, 4096) \
	BARTLBY_FIELD_INT(evnt_time) \

#define BARTLBY_TRIGGER_FIELDS \
		BARTLBY_FIELD_LONG(trigger_id) \
		BARTLBY_FIELD_LONG(trigger_count) \
		BARTLBY_FIELD_CHAR_SIZE(trigger_name, 1024) \
		BARTLBY_FIELD_INT(trigger_enabled) \
		BARTLBY_FIELD_INT(trigger_type) \
		BARTLBY_FIELD_CHAR_SIZE(trigger_data, 1024) \
		BARTLBY_FIELD_CHAR_SIZE(trigger_execplan, 1024) \
		BARTLBY_FIELD_CHAR_SIZE(trigger_full_path, 1024) \
		BARTLBY_FIELD_INT(is_gone) \
		BARTLBY_FIELD_INT(orch_id) \
		
	
struct trigger {
	BARTLBY_TRIGGER_FIELDS
};	

struct shm_counter {
	BARTLBY_SHM_COUNTER_FIELDS
};
struct perf_statistic {
	BARTLBY_PERFSTATISTICS_FIELDS
};
struct snmpi {
	BARTLBY_SNMPI_FIELDS
};
struct sprocess {
	BARTLBY_SPROCESS_FIELDS
		
};
struct sched_worker {
	BARTLBY_SCHEDWORKER_FIELDS
};
struct notification_log_entry {
	BARTLBY_NOTIFICATION_LOG_ENTRY_FIELDS
};
struct shm_header {
	BARTLBY_SHM_HEADER_FIELDS	
};
struct server {
	BARTLBY_SERVER_FIELDS
};
struct trap {
	BARTLBY_TRAP_FIELDS
};
struct service{	
        BARTLBY_SERVICE_FIELDS
};
struct servicegroup {
	BARTLBY_SERVICEGROUP_FIELDS
};
struct servergroup {
	BARTLBY_SERVERGROUP_FIELDS
};
struct worker {
	BARTLBY_WORKER_FIELDS
};
struct downtime {
	BARTLBY_DOWNTIME_FIELDS	
};
struct btl_event {
	BARTLBY_BTL_EVENT_FIELDS		
};

