/* $Id$ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2009 Helmut Januschka - All Rights Reserved
 *   Contact: <helmut@januschka.com>, <contact@bartlby.org>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 *   visit: www.bartlby.org for support
 * ----------------------------------------------------------------------- */
/*
$Revision$
$HeadURL$
$Date$
$Author$ 
*/

/*
configs:
	orch_master_pw=<PORTIER_PASSWD>
	orch_id=<ID OF THIS ORCH>
	orch_master_ip=<IP OF MASTER ORCH>
	orch_master_port=<PORT>
	
	bartlby_orchestra_init
*/


#include <bartlby.h>

static time_t orch_last_reload=0;

void bartlby_orchestra_upstream_log(char * cfg, char * log_line) {
	int rtc;
	char * orch_master_ip_cfg;
	char * orch_master_port_cfg;
	int orch_master_port;
	char * orch_master_pw_cfg;
	int orch_id;

	orch_id=bartlby_orchestra_get_id(cfg);
	if(orch_id != 0) {
		orch_master_ip_cfg=getConfigValue("orch_master_ip", cfg);		
		orch_master_port_cfg=getConfigValue("orch_master_port", cfg);
		orch_master_pw_cfg=getConfigValue("orch_master_pw", cfg);

		if(orch_master_ip_cfg != NULL && orch_master_pw_cfg != NULL && orch_master_port_cfg != NULL) {
			orch_master_port=atoi(orch_master_port_cfg);
			
			rtc=bartlby_portier_send_log(orch_master_ip_cfg, orch_master_port, orch_master_pw_cfg, log_line, cfg);	

			

		}  else {
			_log(LH_ORCH, B_LOG_CRIT, "you have to set 'orch_master_ip' && orch_master_pw && orch_master_port - skipping orch sync");
		}

		if(orch_master_port_cfg != NULL) free(orch_master_port_cfg);
		if(orch_master_ip_cfg != NULL) free(orch_master_ip_cfg);
		if(orch_master_pw_cfg != NULL) free(orch_master_pw_cfg);
		
	}


}
void bartlby_orchestra_send_svc(char * cfg, struct service * svc) {
	int rtc;
	char * orch_master_ip_cfg;
	char * orch_master_port_cfg;
	int orch_master_port;
	char * orch_master_pw_cfg;
	
	int orch_id;

	orch_id=bartlby_orchestra_get_id(cfg);
	if(orch_id != 0) {
		orch_master_ip_cfg=getConfigValue("orch_master_ip", cfg);		
		orch_master_port_cfg=getConfigValue("orch_master_port", cfg);
		orch_master_pw_cfg=getConfigValue("orch_master_pw", cfg);

		if(orch_master_ip_cfg != NULL && orch_master_pw_cfg != NULL && orch_master_port_cfg != NULL) {
			orch_master_port=atoi(orch_master_port_cfg);
			
			rtc=bartlby_portier_send_svc_status(orch_master_ip_cfg, orch_master_port, orch_master_pw_cfg, svc, cfg);	

			

		}  else {
			_log(LH_ORCH, B_LOG_CRIT, "you have to set 'orch_master_ip' && orch_master_pw && orch_master_port - skipping orch sync");
		}

		if(orch_master_port_cfg != NULL) free(orch_master_port_cfg);
		if(orch_master_ip_cfg != NULL) free(orch_master_ip_cfg);
		if(orch_master_pw_cfg != NULL) free(orch_master_pw_cfg);
		
	}
	

}

void bartlby_orchestra_check_timeouts(struct service * svcmap,struct shm_header * hdr, char * cfgfile,void * shm_addr, void * SOHandle) {
	int x;
	time_t cur_time;
	long my_diff;
	
	char * orch_auto_reload;
	int orch_auto_reload_seconds;

	char * orch_node_timeout;
	int orch_node_timeout_sec;


	orch_auto_reload=getConfigValue("orch_auto_reload", cfgfile);
	if(orch_auto_reload != NULL) {
		orch_auto_reload_seconds=atoi(orch_auto_reload);
		free(orch_auto_reload);
	} else {
		orch_auto_reload_seconds=600;
	}
	cur_time=time(NULL);
	
	///* check if reload is required ****////	
	if(cur_time-orch_last_reload > orch_auto_reload_seconds && bartlby_orchestra_get_id(cfgfile) != 0) {
		_log(LH_ORCH, B_LOG_INFO, "Auto Reloading because of %d Seconds since last reload", orch_auto_reload_seconds);
		hdr->do_reload=1;
		orch_last_reload=cur_time;
	}

	///** Check if we have a service that hasn't submitted a state-update within a timeframe
	orch_node_timeout=getConfigValue("orch_node_timeout", cfgfile);
	if(orch_node_timeout != NULL) {
		orch_node_timeout_sec=atoi(orch_node_timeout);
		free(orch_node_timeout);
	} else {
		orch_node_timeout_sec=300;
	}
		
	for(x=0; x<hdr->svccount; x++) {
		if(bartlby_orchestra_belongs_to_orch(&svcmap[x], cfgfile) == 0) {
			continue;
		}
		//Services dos not belong to this node - so check for timeout - runaaway
		my_diff=cur_time-(svcmap[x].last_check+(svcmap[x].check_interval*2));

		

		
		

		if(my_diff > orch_node_timeout_sec) {
			
			//_debug("TIMEOUT (%ld) FOR: %s %d - is the ORCH node dead?",my_diff, svcmap[x].service_name, svcmap[x].orch_id);
			//svcmap[x].is_gone=4;
			
			
			//PULL TRIGGERS? - set state to unkown? - critical? - retain count? - outputmsg? = "Timed out - no response from orch_id:%d"?
			//set is_gone=4 - if already - mark as REALLY DEAD - means 2 times timeout!
			svcmap[x].current_state=STATE_CRITICAL;
			snprintf(svcmap[x].new_server_text,200, "ORCH Sync Timeout");
			bartlby_fin_service(&svcmap[x],SOHandle,shm_addr,cfgfile);
			
			//FIXME - fix also in sched.c
			svcmap[x].last_check=time(NULL);
			sched_reschedule(&svcmap[x]);
			
		}	
	}
	

}
int bartlby_orchestra_belongs_to_orch(struct service * svc, char * cfgfile) {
	//check if svc->orch_id == current_orch_id - return 0 else return -1;
	int orch_id;

	orch_id=bartlby_orchestra_get_id(cfgfile);

	if(svc->orch_id != orch_id) {
		//_log(LH_ORCH, B_LOG_DEBUG, "Skipping service: %s because it is for ORCH: %d and i'am: %d", svc->service_name, svc->orch_id, orch_id);
		return -1;
	}
	//_log(LH_ORCH, B_LOG_DEBUG, "Running service: %s because it is for ORCH: %d and i'am: %d", svc->service_name, svc->orch_id, orch_id);
	return 0;


}
void bartlby_orchestra_init(struct shm_header * shmhdr) {
	//INIT - happens after shm population
	_log(LH_ORCH, B_LOG_DEBUG, "**********ORCHESTRA MODE INIT");
	orch_last_reload=time(NULL);
	
}
int bartlby_orchestra_get_id(char * cfgfile) {
	//Get the ORCH_ID of this instance
	//DEFAULT to 0	
	int orch_id=0;


	char * orch_id_cfg;

	orch_id_cfg = getConfigValue("orch_id", cfgfile);
	if(orch_id_cfg == NULL) {
		//_log(LH_ORCH, B_LOG_INFO, "'orch_id' not set - disabling orchestra mode");
		return 0;
	}

	orch_id=atoi(orch_id_cfg);
	free(orch_id_cfg);

	//_log(LH_ORCH, B_LOG_DEBUG, "**********ORCHESTRA NODE ID: %d", orch_id);
	return orch_id;
}