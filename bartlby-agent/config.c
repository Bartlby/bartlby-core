/* $Id: config.c,v 1.1.1.1 2006/07/22 23:27:48 hjanuschka Exp $ */
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
$Revision: 1.1.1.1 $
$Source: /cvsroot/bartlby/bartlby-agent/config.c,v $


$Log: config.c,v $
Revision 1.1.1.1  2006/07/22 23:27:48  hjanuschka
initial stand alone agent

Revision 1.9  2006/06/05 21:06:06  hjanuschka
*** empty log message ***

Revision 1.8  2006/06/04 23:55:28  hjanuschka
core: SSL_connect (timeout issue's solved , at least i hope :))
core: when perfhandlers_enabled == false, you now can enable single services
core: plugin_arguments supports $MACROS
core: config variables try now to cache themselfe to minimize I/O activity
core: .so extensions support added

Revision 1.7  2005/09/28 21:46:30  hjanuschka
converted files to unix
jabber.sh -> disabled core dumps -> jabblibs segfaults
                                    will try to patch it later

Revision 1.6  2005/09/25 13:30:18  hjanuschka
cfg: jabber variables
daemon: setenv BARTLBY_HOME (for triggers)
sched: wait_open timeout
mail.sh: sendmail trigger
trigger: $1 == email
$2 == icq
$3 == name
$4 == msg

Revision 1.5  2005/09/05 20:00:54  hjanuschka
stupid configfile issue fixed

Revision 1.4  2005/08/28 22:57:14  hjanuschka
config.c: fixed fclose BUG (too many open files ) missing fclose
service_active is now set by data_lib and acutally used by scheduler

Revision 1.3  2005/08/28 16:02:59  hjanuschka
CVS Header


*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>


#define MAX_CCACHE 1024

struct cfg_cache {
	char key[1024];
	char value[2048];
		
} ccf;

static struct cfg_cache ccache[MAX_CCACHE];
static int cur_el=0;

void cfg_init_cache(void) {
	int x;
	for(x=0; x<MAX_CCACHE; x++) {
		snprintf(ccache[x].key,1020,  "%s", "");
		snprintf(ccache[x].value,2000, "%s", "");
	}
	printf("Cache init ready for %d variables to hold", MAX_CCACHE);
			
			
}

char * cfg_add_to_cache(char * k, char * v) {
	snprintf(ccache[cur_el].key,1020,  "%s", k);
	snprintf(ccache[cur_el].value,2000, "%s", v);
	if(cur_el + 1 >= MAX_CCACHE) {
		cur_el=0;
	} else {
		cur_el++;
	}
		
	return NULL;
}

char * cfg_cache_find(char *k) {
	int x;
	for(x=0; x<MAX_CCACHE; x++) {
		if(strcmp(ccache[x].key, k) == 0) {
			//printf("Found cached key/value: %s/%s\n", ccache[x].key, ccache[x].value);
			return strdup(ccache[x].value);	
		}	
	}	
	return NULL;
}


char * getConfigValue(char * key, char * fname) {
	FILE * fp;
	char  str[1024];
	char * val;

	char * tok;
	
	char * cache_value;
	
	cache_value=cfg_cache_find(key);
	if(cache_value != NULL) {
		return cache_value;	
	}
	
	fp=fopen(fname, "r");
	if(!fp)  {
		printf("config fopen %s failed", fname);
		exit(0);
		
	}
	
	
	
	while(fgets(str,1024,fp) != NULL) {
		str[strlen(str)-1]='\0';
		tok=strtok(str, "=");
		if(tok != NULL) {
				if(strcmp(tok, key) == 0) {
						
						tok=strtok(NULL, "=");
						if(tok == NULL) {
								return NULL;
						}
						if(tok[strlen(tok)-1] == '\r') {
							tok[strlen(tok)-1]='\0';
						}
						
						fclose(fp);
						val=strdup(tok);
						cfg_add_to_cache(key, val);
						return val;
						
				} else {
					continue;
				}
					
		}
			
	}
	
	
	fclose(fp);
	
	
	return NULL;
}

