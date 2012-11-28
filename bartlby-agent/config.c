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

#include <stdio.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <malloc.h>
#endif
#include <string.h>


#define MAX_CCACHE 1024

struct cfg_cache {
	char key[1024];
	char value[2048];
		
} ccf;

static struct cfg_cache ccache[MAX_CCACHE];
static int cur_el=0;


#define GOODCHARS " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890\"*:;'/_-."
int has_bad_chars( char * str) {
	int x;
	char  c;
	for (x = 0; x<strlen(str); x++) {
		c = str[x];
		if(strchr(GOODCHARS, c) == NULL) {
	    		//Character not found
	    		return -1;
		}
	}
	//check for ..
	if(strstr(str, "..") != NULL) {
		return -1;	
	}
	
	return 1;

}

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

