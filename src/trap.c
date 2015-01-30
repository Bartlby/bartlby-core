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




#include <bartlby.h>


int bartlby_submit_trap(const char * trap_data, void * bartlby_address, char * cfgfile) {
	
	int x;
	regex_t catcher_compiled, status_compiled;
	regex_t stati_compiled[3];
	
	int max_matches=3;
	regmatch_t matches[max_matches];
	char * match_copy;
	char * status_match;
	char * status_to_use;

	int is_ok=0;
	int is_warning=0;
	int is_critical=0;
	int status=4;
	long svc_id=-1;
	struct service * svc;

	char * safe_log;

	int rules_matched=0;


	struct trap * trapmap;
	struct shm_header * shm_hdr;
	struct service * svcmap;


	shm_hdr=bartlby_SHM_GetHDR(bartlby_address);
	trapmap=bartlby_SHM_TrapMap(bartlby_address);
	svcmap=bartlby_SHM_ServiceMap(bartlby_address);
	
	for(x=0; x<shm_hdr->trapcount; x++) {
		status_to_use=NULL;
		svc_id=-1;
		if (regcomp(&catcher_compiled, trapmap[x].trap_catcher, REG_EXTENDED)) {
      		_log(LH_PORTIER, B_LOG_CRIT,"Catcher rule compile failed for rule: %ld", trapmap[x].trap_id);
      		continue;
    	}
    	if(regexec(&catcher_compiled, trap_data,0,0,0) == 0) {
				//Catcher matches
				//printf("Catcher matches Rule %ld\n", trapmap[x].trap_id);    	
    			rules_matched++;
    			trapmap[x].matched++;
    			trapmap[x].trap_last_match=time(NULL);
    			snprintf(trapmap[x].trap_last_data,2047, "%s", trap_data);
				//Get status text
				if(regcomp(&status_compiled, trapmap[x].trap_status_text, REG_EXTENDED)) {
					_log(LH_PORTIER, B_LOG_CRIT,"Status rule compile failed for rule: %ld", trapmap[x].trap_id);
				} else {
					if (regexec(&status_compiled, trap_data, max_matches, matches, 0) == 0) {
						 if (matches[1].rm_so != (size_t)-1) {
							match_copy = strdup(trap_data);
							match_copy[matches[1].rm_eo] = 0;
							status_match = match_copy   + matches[1].rm_so;
							
							
							status_to_use=strndup(status_match, 1020);
							free(match_copy);
						} else {
							
							//Status rule matched but no status text returned
							status_to_use=strndup(trap_data, 1020);
						}
					} else {
							//Status rule not matched
							status_to_use=strndup(trap_data, 1020);
							
					}
					regfree(&status_compiled);
				}

				is_ok=0;
				is_critical=0;
				is_warning=0;
				status=4;

				if(regcomp(&stati_compiled[0], trapmap[x].trap_status_ok, REG_EXTENDED) == 0) {
						if(regexec(&stati_compiled[0], trap_data, 0,0,0) == 0) {
							is_ok=1;
						}
						regfree(&stati_compiled[0]);
				}

				if(regcomp(&stati_compiled[1], trapmap[x].trap_status_warning, REG_EXTENDED) == 0) {
						if(regexec(&stati_compiled[1], trap_data, 0,0,0) == 0) {
							is_warning=1;
						}
						regfree(&stati_compiled[1]);
				}				
				if(regcomp(&stati_compiled[2], trapmap[x].trap_status_critical, REG_EXTENDED) == 0) {
						if(regexec(&stati_compiled[2], trap_data, 0,0,0) == 0) {
							is_critical=1;
						}
						regfree(&stati_compiled[2]);
				}		

				if(trapmap[x].trap_fixed_status != -2) {
					status=trapmap[x].trap_fixed_status;
				} else {
					if(is_ok == 1 && strlen(trapmap[x].trap_status_ok) > 1) status=0;
					if(is_warning == 1 && strlen(trapmap[x].trap_status_warning) > 1) status=1;
					if(is_critical == 1 && strlen(trapmap[x].trap_status_critical) > 1) status=2;

				}
				
				//CHECK IF SVC IS ASSIGNED - set the status 
				if(trapmap[x].service_shm_place >= 0) {

					svc=&svcmap[trapmap[x].service_shm_place];
					svc_id=svc->service_id;

					svc->last_state=svc->current_state;
					svc->current_state=status;
					snprintf(svc->current_output,1020, "%s", status_to_use);
					svc->last_check=time(NULL);



				}
				safe_log=remove_nl_copy(status_to_use);
				_log(LH_PORTIER, B_LOG_HASTO, "@TRAP@|%ld|%s|%d|%d|%s",trapmap[x].trap_id,trapmap[x].trap_name, status,svc_id, safe_log);
				free(safe_log);
				if(status_to_use != NULL)  {
					free(status_to_use);
				}
				if(trapmap[x].trap_is_final == 1) {
					//printf("IS FINAL\n");
					regfree(&catcher_compiled);
					break;
				}
    	} 
    	regfree(&catcher_compiled);
	}

	return rules_matched;
}