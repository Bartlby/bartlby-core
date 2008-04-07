/* $Id$ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2008 Helmut Januschka - All Rights Reserved
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
#include <malloc.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>	
#include <unistd.h>	
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>

#include <dlfcn.h>


#include <bartlby.h>

#define MAX_EXTENSIONS 100
#define MAX_EXTENSIONS_CALLBACK 200

struct dl_handles {
	char dlname[1024];
	char * soHandle;
	int (*dispatcher)(int, void *);	
	int is_ok;
} dlh;




void * gSHMAddr;
void * gDataLoader;
char * gCFG;
int module_count;

char * dlmsg;


struct dl_handles dl_objects[MAX_EXTENSIONS];


int bartlby_callback(int type, void *data) {
	
	int x;
	
	for(x=0; x<module_count; x++) {
		if(dl_objects[x].is_ok == EXTENSION_OK) {
			
        		if(dl_objects[x].dispatcher(type, data) != EXTENSION_OK) {
        			return EXTENSION_NOK;	
        		}
        		
        		
		}	
	}

	return EXTENSION_OK;
}



void bartlby_ext_shutdown(int sched_exit_code) {
	int x;
	int (*mod_shutdown)(int);
	
	
	for(x=0; x<module_count; x++) {
		if(dl_objects[x].is_ok == EXTENSION_OK) {
			mod_shutdown=dlsym(dl_objects[x].soHandle, "bartlby_extension_shutdown");
    			if((dlmsg=dlerror()) != NULL) {
        			_log("%s skipping extension shutdown error: %s", dl_objects[x].dlname, dlmsg);
        			continue;
        		}
        		mod_shutdown(sched_exit_code);
        		dl_objects[x].is_ok=EXTENSION_NOK;
        		dlclose(dl_objects[x].soHandle);
		}	
	}
	
}


void bartlby_ext_load(char * mod) {
	int dlLoadOK;
	int (*mod_startup)(void *, void *, char *);
	
	
	_log("Module: %s", mod);	
	
	dl_objects[module_count].soHandle=dlopen(mod, RTLD_LAZY);
		
    	if((dlmsg=dlerror()) != NULL) {
        	_log("%s skipping extension Error: %s", mod, dlmsg);
        	return;
    	} else {
    		//CALL init function
    		//module_count++
    		snprintf(dl_objects[module_count].dlname,1000, "%s", mod);
    		mod_startup=dlsym(dl_objects[module_count].soHandle, "bartlby_extension_startup");
    		if((dlmsg=dlerror()) != NULL) {
        		_log("%s skipping extension Error: %s", mod, dlmsg);
        		return;
        	}
        	dlLoadOK=mod_startup(gSHMAddr, gDataLoader, gCFG);
        	if(dlLoadOK != EXTENSION_OK) {
        		_log("%s skipping extension because of init error", mod);
        		dlclose(dl_objects[module_count].soHandle);
        		return;	
        	}
        	dl_objects[module_count].dispatcher=dlsym(dl_objects[module_count].soHandle, "bartlby_extension_dispatcher");
    		if((dlmsg=dlerror()) != NULL) {
        		_log("%s skipping extension Error: %s", mod, dlmsg);
        		return;
        	}
        	
        	dl_objects[module_count].is_ok=EXTENSION_OK;
    	}
    	module_count++;	
    	
	
}
void bartlby_ext_loadall(void) {
	char * sext_count;
	int ext_count;
	int x;
	char * loadstr;
	char find_str[1024];
	
	for(x=0; x<MAX_EXTENSIONS; x++) {
		dl_objects[x].is_ok=EXTENSION_NOK;
	}

	
	sext_count = getConfigValue("extension_count", gCFG);
	if(sext_count != NULL) {
		ext_count=atoi(sext_count);
		_log("Loading extensions: %d", ext_count);
		for(x=1; x<=ext_count; x++) {
			snprintf(find_str, 1023, "extension[%d]", x);
			loadstr = getConfigValue(find_str, gCFG);
			if(loadstr != NULL) {
				bartlby_ext_load(loadstr);	
				free(loadstr);	
			} else {
				_log("extension[%d] not defined...skipping", x);	
			}
			
		}
		
		
		free(sext_count);
	} else {
		_log("no EXTENSION will be loaded");	
	}
}


void bartlby_ext_init(void * shm_addr, void * data_loader, char * cfg) {

	gCFG=cfg;
	gDataLoader=data_loader;
	gSHMAddr=shm_addr;
	module_count=0;
	bartlby_ext_loadall();

		
}


