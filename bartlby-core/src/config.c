/* $Id: config.c,v 1.13 2008/03/17 19:04:43 hjanuschka Exp $ */
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
$Revision: 1.13 $
$Source: /cvsroot/bartlby/bartlby-core/src/config.c,v $


$Log: config.c,v $
Revision 1.13  2008/03/17 19:04:43  hjanuschka
SF Feature issue #1916495 changed static key,value length of config cache to pre defined constant variables (config.c, bartlby.h) reported by Markus Elfring (elfring)

Revision 1.12  2008/03/16 21:06:11  hjanuschka
auto commit

Revision 1.11  2007/02/15 16:25:32  hjanuschka
auto commit

Revision 1.10  2007/01/05 01:49:00  hjanuschka
auto commit

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


#include <bartlby.h>


struct cfg_cache {
	char key[MAX_CCACHE_KEY];
	char value[MAX_CCACHE_VALUE];
		
} ccf;

static struct cfg_cache ccache[MAX_CCACHE];
static int cur_el=0;




void cfg_init_cache(void) {
	int x;
	for(x=0; x<MAX_CCACHE; x++) {
		snprintf(ccache[x].key,MAX_CCACHE_KEY,  "%s", "");
		snprintf(ccache[x].value,MAX_CCACHE_VALUE, "%s", "");
	}
	_log("Cache init ready for %d variables to hold", MAX_CCACHE);
			
			
}


void cfg_update_cache(char * k, char * v) {
	int x;
	for(x=0; x<MAX_CCACHE; x++) {
		if(strcmp(ccache[x].key, k) == 0) {
			snprintf(ccache[x].value,MAX_CCACHE_VALUE, "%s", v);	
			
			
		}	
	}	
	return;
}

char * cfg_cache_find(const char *k) {
	int x;
	for(x=0; x<MAX_CCACHE; x++) {
		if(strcmp(ccache[x].key, k) == 0) {
			
			return strdup(ccache[x].value);	
		}	
	}	
	return NULL;
}
char * cfg_add_to_cache(const char * k, char * v) {
	
	
	
	snprintf(ccache[cur_el].key,MAX_CCACHE_KEY,  "%s", k);
	snprintf(ccache[cur_el].value,MAX_CCACHE_VALUE, "%s", v);
	if(cur_el + 1 >= MAX_CCACHE) {
		cur_el=0;
	} else {
		cur_el++;
	}
		
	return NULL;
}


char * getConfigValue(char * key, char * fname) {
	return getConfigValue_ex(key, fname, 1);
}

char * getConfigValue_ex(const char * key, const char * fname, int cache) {
	FILE * fp;
	char  str[1024];
	char * val;

	char * tok;
	
	char * cache_value;
	
	if(cache == 1) {
		cache_value=cfg_cache_find(key);
		if(cache_value != NULL) {
			return cache_value;	
		}
	}
	
	fp=fopen(fname, "r");
	if(!fp)  {
		if(cache == 1) {
			//keep old known way
			printf("config fopen %s failed", fname);
			exit(0);
		} else {
			_log("file: '%s' cant be read", fname);
			return NULL;	
		}
		
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
						if(cache == 1) {
							cfg_add_to_cache(key, val);
						}
						return val;
						
				} else {
					continue;
				}
					
		}
			
	}
	
	
	fclose(fp);
	
	
	return NULL;
}

void cfg_fill_with_file(char * f) {
	FILE * fp;
	char  str[1024];
	char * val;
	char * key;
	char * tok;
	
	int c=0;
	
	fp=fopen(f, "r");
	if(!fp)  {
		printf("config fopen %s failed", f);
		exit(0);
		
		
	}
	
	
	
	while(fgets(str,1024,fp) != NULL) {
		str[strlen(str)-1]='\0';
		tok=strtok(str, "=");
		if(tok != NULL) {
			key=strdup(tok);
					
			tok=strtok(NULL, "=");
			if(tok == NULL) {
					return;
			}
			if(tok[strlen(tok)-1] == '\r') {
				tok[strlen(tok)-1]='\0';
			}
				
			
			val=strdup(tok);
			cfg_add_to_cache(key, val);
			
			c++;
			
			free(val);
			free(key);
				
					
		}
			
	}
	
	
	fclose(fp);
	_log("pre-cached %d kv-pairs", c);
		
}

