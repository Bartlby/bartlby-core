/* $Id$ *//* $Id$ */
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




#include <bartlby.h>

long bartlby_milli_timediff(struct timeval end, struct timeval start) {
	long r;
	
	r=((end.tv_sec - start.tv_sec) * 1000) +  (((1000000 + end.tv_usec - start.tv_usec) / 1000) - 1000);	
	
	return r;
}

int bartlby_core_perf_track(struct shm_header * hdr, struct service * svc, int type, int time) {
	
	// be nice to CFG access use env variable :-)
	switch(type) {
		case PERF_TYPE_ROUND_TIME:
			
			hdr->pstat.counter++;
			hdr->pstat.sum += time;
			bartlby_callback(EXTENSION_CALLBACK_ROUND_TIME, hdr);
		break;
		case PERF_TYPE_SVC_TIME:
			svc->pstat.counter++;
			svc->pstat.sum += time;
			bartlby_callback(EXTENSION_CALLBACK_CHECK_TIME, svc);
			
		break;
		
		
		default: _log("unknown perf type: %d", type);	
	}
	return -1;	
}

void bartlby_perf_track(struct service * svc,char * return_buffer, int return_bytes, char * cfgfile) {
	
	struct stat perf_s;
	//char perf_out[2048];
	char * cfg_perf_dir;
	char * perf_trigger;
	char * perf_enabled;
	//int perf_child;
	struct timeval stat_end, stat_start;
	char my_own_handler[1024];
	char * found_my_own;
	
	FILE * phandler;
	char dummy_buffer[1024];
	
	
	//signal(SIGCHLD, SIG_IGN);
	sprintf(my_own_handler, "perfhandler_enabled_%ld", svc->service_id);
	
	
	perf_enabled=getConfigValue("perfhandler_enabled", cfgfile);
	found_my_own=getConfigValue(my_own_handler, cfgfile);
	
	if(perf_enabled == NULL) {
		perf_enabled=strdup("true");
		//sprintf(perf_enabled, "true");
	}
	if(found_my_own == NULL) {
		found_my_own=strdup("false");
		//sprintf(perf_enabled, "true");
	}
	
	if(strcmp(perf_enabled, "false") == 0 && strcmp(found_my_own, "false") == 0)  {
		free(perf_enabled);
		free(found_my_own);
		return;
			
	}
	free(found_my_own);
	free(perf_enabled);
	
	cfg_perf_dir=getConfigValue("performance_dir", cfgfile);
	if(cfg_perf_dir != NULL) {
		asprintf(&perf_trigger, "%s/%s", cfg_perf_dir, svc->plugin);
		if(stat(perf_trigger, &perf_s) < 0) {
			_log("Performance Trigger: %s not found", perf_trigger);	
		} else {
			free(perf_trigger);
			asprintf(&perf_trigger, "%s/%s %ld %s 2>&1 > /dev/null", cfg_perf_dir, svc->plugin, svc->service_id, return_buffer);
			signal(SIGPIPE,SIG_DFL);
			signal(SIGCHLD,SIG_DFL);
			gettimeofday(&stat_start,NULL);
			
			
			phandler=popen(perf_trigger, "r");
			if(phandler != NULL) {
				if(fgets(dummy_buffer, 1024, phandler) == NULL) { 
					_log("Performance Trigger: %s fgets failed", perf_trigger);	
				}
				pclose(phandler);	
				
			} else {
				_log("Performance Trigger: %s failed popen", perf_trigger);	
			}
			
			gettimeofday(&stat_end,NULL);
			//_log("@PERF@%d|%s:%d/%s", bartlby_milli_timediff(stat_end,stat_start),svc->server_name,svc->client_port, svc->service_name);
			/*
			switch(perf_child=fork()) {
				case -1:
					_log("fork error");
				break;
				
				case 0:
					gettimeofday(&stat_start,NULL);
					system(perf_trigger);
					gettimeofday(&stat_end,NULL);
					//_log("@PERF@%d|%s:%d/%s", bartlby_milli_timediff(stat_end,stat_start),svc->server_name,svc->client_port, svc->service_name);
					exit(1);
				break;	
				default:
					//_log("Forked perf trigger %s", perf_trigger);
					
				break;
			}
			
			*/
		}
		free(perf_trigger);
		
		free(cfg_perf_dir);	
	}	
	
}

