/* $Id: perf.c,v 1.11 2007/07/27 22:54:04 hjanuschka Exp $ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005 Helmut Januschka - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */
/*
$Revision: 1.11 $
$Source: /cvsroot/bartlby/bartlby-core/src/perf.c,v $


$Log: perf.c,v $
Revision 1.11  2007/07/27 22:54:04  hjanuschka
int to long changing

Revision 1.10  2006/12/05 03:47:12  hjanuschka
auto commit

Revision 1.9  2006/12/02 21:34:56  hjanuschka
auto commit

Revision 1.8  2006/09/09 19:38:34  hjanuschka
auto commit

Revision 1.7  2006/08/03 20:52:57  hjanuschka
*** empty log message ***

Revision 1.6  2006/06/04 23:55:28  hjanuschka
core: SSL_connect (timeout issue's solved , at least i hope :))
core: when perfhandlers_enabled == false, you now can enable single services
core: plugin_arguments supports $MACROS
core: config variables try now to cache themselfe to minimize I/O activity
core: .so extensions support added

Revision 1.5  2006/05/06 23:32:02  hjanuschka
*** empty log message ***

Revision 1.4  2006/04/23 18:07:43  hjanuschka
core/ui/php: checks can now be forced
ui: remote xml special_addon support
core: svc perf MS
core: round perf MS
php: svcmap, get_service perf MS
ui: perf MS

Revision 1.3  2006/02/09 00:14:50  hjanuschka
datalib: mysql/ catch failed logins
core: fixed some setuid problems with datalib
core: zero worker detected and logged
core: network code re-worked, much faster and cleaner now
core: encode/decode removed
php: encode/decode removed
ui: topology map manager added
ui: nicer menu (flap)
ui: server_detail (added)
startup sh: pre-start check if logfile is writeable

Revision 1.2  2006/01/16 20:51:41  hjanuschka
performance stuff moved to perf.c
timeing information on perf handler

Revision 1.1  2005/12/29 20:05:55  hjanuschka
core statistic (should be used in debug mode only produces a biiiig file)

*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>


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
		perf_trigger = malloc(sizeof(char) * (strlen(cfg_perf_dir)+50+strlen(svc->plugin)+return_bytes+50));
		sprintf(perf_trigger, "%s/%s", cfg_perf_dir, svc->plugin);
		if(stat(perf_trigger, &perf_s) < 0) {
			_log("Performance Trigger: %s not found", perf_trigger);	
		} else {
			
			sprintf(perf_trigger, "%s/%s %ld %s 2>&1 > /dev/null", cfg_perf_dir, svc->plugin, svc->service_id, return_buffer);
			signal(SIGPIPE,SIG_DFL);
			signal(SIGCHLD,SIG_DFL);
			gettimeofday(&stat_start,NULL);
			
			
			phandler=popen(perf_trigger, "r");
			if(phandler != NULL) {
				fgets(dummy_buffer, 1024, phandler);
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

