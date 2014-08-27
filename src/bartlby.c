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

char * gCfgfile;
int gDebug=0;
char * gSOName; //Shared library name
void * gSOHandle;
const char * dlmsg;

char * gGetAutorStr;
char * gGetVersionStr;
char * gGetNameStr;
char * (*gGetAutor)();
char * (*gGetVersion)();
char * (*gGetName)();
long   (*gExpectVersion)();
int (*gGetServiceMap)(struct service *, char *, int);
int (*gGetServerMap)(struct server *, char *, int);
int (*gGetWorkerMap)(struct worker *,char *,int);
int (*gGetDowntimeMap)(struct downtime *, char *,int);
int (*gGetServerGroupMap)(struct servergroup *, char *,int);
int (*gGetServiceGroupMap)(struct servicegroup *, char *,int);

void (*gDataLibInit)(char *, int);


struct  shm_counter  * (*gGetCounter)(char *);
char * gShmtok;
struct service * gsvcmap;
struct worker * gwrkmap;
struct downtime * gdtmap;
struct server * gsrvmap;

struct servergroup * gsrvgrpmap;
struct servicegroup * gsvcgrpmap;

int global_startup_time;


int gshm_id;
void * gBartlby_address;
int gshm_svc_cnt;
int gshm_wrk_cnt;
int gshm_dt_cnt;
int gshm_srv_cnt;
int gshm_srvgrp_cnt;
int gshm_svcgrp_cnt;

long gSHMSize;

struct shmid_ds gshm_desc;

struct shm_header * gshm_hdr;


int gReuseSHM=0;
int gOnlySHMPop=0;
int gWBSHMToDB=0;



void dispHelp(void) {
	
	
	printf("\n");
	printf("	Bartlby\n");
	printf("          %s Version %s (%s) started. compiled %s/%s\n", PROGNAME, VERSION,REL_NAME, __DATE__, __TIME__);
	printf("	Next generation sys monitoring\n");
	printf("	bartlby.org\n");
	printf("\n");
	printf("	Usage: bartlby [OPTIONS] [CONFIGFILE]\n");
	printf("	\n");
	printf("	   -d, --debug          Runs bartlby in foreground with stdout logging\n");
	printf("	   -r, --reuse          if SHM is already here reuse it (could be dangorous, handle with care)\n");
	printf("	   -s                   popuplate shm\n");
	printf("	   -w                   WB SHM to DB\n");
	
	printf("	\n");
	printf("	   -h, --help           show this help\n");
	printf("	   -v, --version        version\n");
	printf("	--------------------------------------------------------------------------\n");
	printf("               \n");
	exit(1);
}


void bartlby_parse_argv(int argc, char ** argv){
	static struct option longopts[] = {
		{ "help",	0, NULL, 'h'},
		{ "debug",	0, NULL, 'd'},
		{ "debug",	0, NULL, 'd'},
		{ "reuse",	0, NULL, 'r'},
		{ "version",	0, NULL, 'v'},
		
		{ NULL,		0, NULL, 0}
	};
	int c;

	if(argc == 1) {
		dispHelp();		
	}
	
	for (;;) {
		c = getopt_long(argc, argv, "hdrsvw", longopts, (int *) 0);
		if (c == -1)
			break;
		switch (c) {
		case 'h':  /* --help */
			dispHelp();
		break;
		case 'd':
			gDebug=1;
			
		break;
		case 'r':
			gReuseSHM=1;
		break;
		case 's':
			gOnlySHMPop=1;	
		break;
		case 'w':
			gWBSHMToDB=1;
		break;
		case 'v':
			printf("%s\n", VERSION);
			exit(0);
		break;
		
		default:
			dispHelp();
		}
	}
	
	gCfgfile=argv[optind];
	
	
	
		
	
}

void bartlby_load_shm_stuff(char * cfgfile) {
	
	gSOName = getConfigValue("data_library", cfgfile);
	if(gSOName == NULL) {
		_log(LH_MAIN, B_LOG_CRIT,"No data_library specified in `%s' config file", cfgfile);
		exit(1);
	}
	_log(LH_MAIN, B_LOG_INFO,"using data lib: `%s'", gSOName);
	gSOHandle=dlopen(gSOName, RTLD_LAZY);
		
    	if((dlmsg=dlerror()) != NULL) {
        	_log(LH_MAIN, B_LOG_CRIT,"Error: %s", dlmsg);
        	exit(1);
    	}
    	LOAD_SYMBOL(gDataLibInit, gSOHandle, "DataLibInit");
    	LOAD_SYMBOL(gGetAutor,gSOHandle, "GetAutor");
    	LOAD_SYMBOL(gGetVersion,gSOHandle, "GetVersion");
    	LOAD_SYMBOL(gGetServiceMap,gSOHandle, "GetServiceMap");
    	LOAD_SYMBOL(gGetServerMap,gSOHandle, "GetServerMap");
    	LOAD_SYMBOL(gGetWorkerMap,gSOHandle, "GetWorkerMap");
    	LOAD_SYMBOL(gGetDowntimeMap,gSOHandle, "GetDowntimeMap");
    	
    	LOAD_SYMBOL(gGetServerGroupMap,gSOHandle, "GetServerGroupMap");
    	LOAD_SYMBOL(gGetServiceGroupMap,gSOHandle, "GetServiceGroupMap");
    	
    	
    	
    	LOAD_SYMBOL(gGetName,gSOHandle, "GetName");
    	LOAD_SYMBOL(gExpectVersion,gSOHandle, "ExpectVersion");
    	LOAD_SYMBOL(gGetCounter,gSOHandle, "GetCounter");
    	    	
    	
    	gDataLibInit(cfgfile,gDebug);
    	    	
    	gGetAutorStr=gGetAutor();
    	gGetVersionStr=gGetVersion();
    	gGetNameStr=gGetName();
    	
    	if(gExpectVersion() > EXPECTCORE || EXPECTCORE < gExpectVersion() || EXPECTCORE != gExpectVersion()) {
    		_log(LH_MAIN, B_LOG_CRIT,"*****Version check failed Module is compiled for version '%ld' of %s requiring '%ld'", gExpectVersion(), PROGNAME, EXPECTCORE);	
    		
    		exit(1);
    	} 
    	
    	_log(LH_MAIN, B_LOG_INFO,"Data Lib (%s) by: '%s' Version: %s", gGetNameStr, gGetAutorStr, gGetVersionStr);
    		
	free(gGetAutorStr);
	free(gGetVersionStr);
	free(gGetNameStr);
	
	gShmtok = getConfigValue("shm_key", cfgfile);
	if(gShmtok == NULL) {
		_log(LH_MAIN, B_LOG_CRIT,"Unset variable `shm_key'");
		exit(1);
	}
	
	
    	
	
	
	
		
}
void bartlby_init(void) {
	
	_log(LH_MAIN, B_LOG_INFO,"%s Version %s (%s) started. compiled %s/%s", PROGNAME, VERSION,REL_NAME, __DATE__, __TIME__);
	#ifdef SNMP_ADDON
	_log(LH_MAIN, B_LOG_INFO,"SNMP support compiled in");
	#endif
	#ifdef SSH_ADDON
	_log(LH_MAIN, B_LOG_INFO,"SSH support compiled in");
	#endif
	#ifdef HAVE_SSL
	_log(LH_MAIN, B_LOG_INFO,"SSL support compiled in");
	SSL_library_init();
	SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();
	_log(LH_MAIN, B_LOG_INFO,"ssl lib init, ssl_algo, error_strings");	
	#endif
	#ifdef WITH_NRPE
	_log(LH_MAIN, B_LOG_INFO,"NRPE Support compiled in");
	#endif
	#ifdef HAVE_DEBUG
	_log(LH_MAIN, B_LOG_INFO,"DEBUGING ENABLED");
	_debug("TEST DEBUG MESSAGE");
	#endif
	bartlby_orchestra_init(gshm_hdr);


	
}
void bartlby_setuid(void) {
	char * cfg_user;
	struct passwd * ui;
	
	
	cfg_user = getConfigValue("user", gCfgfile);
	if(cfg_user == NULL) {
		_log(LH_MAIN, B_LOG_CRIT,"user not set in config file");
		exit(2);			
	}
	ui=getpwnam(cfg_user);
	if(ui == NULL) {
		_log(LH_MAIN, B_LOG_CRIT,"User: %s not found cannot setuid running as %d", cfg_user, getuid());	
	} else {
		if(setuid(ui->pw_uid) < 0) {
			_log(LH_MAIN, B_LOG_CRIT,"setuid() failed: %s", ui->pw_name);	
			exit(2);
		}
		if(setgid(ui->pw_gid) < 0) {
			_log(LH_MAIN, B_LOG_CRIT,"setgid() failed: %d '%s`", ui->pw_gid, strerror(errno));	
		//	exit(2);
			
		}
		_log(LH_MAIN, B_LOG_INFO,"User: %s/%d", ui->pw_name, ui->pw_gid);	
	}
	
	free(cfg_user);
}

void bartlby_shm_fits(char * cfgfile) {

		struct shm_counter * shmc;
		int suggested_minimum;
		
		long cfg_shm_size_bytes;
		char *  cfg_shm_size;
		
		
		
		cfg_shm_size = getConfigValue("shm_size", cfgfile);
		
		if(cfg_shm_size==NULL) {
			cfg_shm_size_bytes=10;		
		} else {
			cfg_shm_size_bytes=atol(cfg_shm_size);
		}
		
		free(cfg_shm_size);
		
		shmc = gGetCounter(cfgfile);
		if(shmc == NULL) {
			exit(0);	
		}
		gSHMSize=cfg_shm_size_bytes*1024*1024;	
		
		suggested_minimum = (sizeof(struct shm_header) + (sizeof(struct server) * shmc->servers) + (sizeof(struct worker) * shmc->worker) + (sizeof(struct service) * shmc->services) + (sizeof(struct downtime) * shmc->downtimes) + 2000 + (sizeof(struct btl_event)*EVENT_QUEUE_MAX))  + (sizeof(struct servergroup) * shmc->servergroups) + (sizeof(struct servicegroup) * shmc->servicegroups) * 2;
		if(gSHMSize <= suggested_minimum) {
			_log(LH_MAIN, B_LOG_CRIT,"SHM is to small minimum: %d KB ", suggested_minimum/1024);
			exit(1);	
		}
		_log(LH_MAIN, B_LOG_INFO,"SHM requires: %d KB ", suggested_minimum/1024);
		_log(LH_MAIN, B_LOG_DEBUG,"Size: S=%ld, W=%ld, D=%ld, H=%ld, E=%ld, SRG=%ld,SVG=%ld", sizeof(struct service), sizeof(struct worker), sizeof(struct downtime), sizeof(struct shm_header), sizeof(struct btl_event), sizeof(struct servergroup), sizeof(struct servicegroup));
		free(shmc);
		

	
}

int bartlby_populate_shm(char * cfgfile) {
		int shm_got_reused=0;

		int orch_id;
		
		gshm_id = shmget(ftok(gShmtok, 32), gSHMSize,IPC_CREAT | IPC_EXCL | 0777);
		if(gshm_id < 0 && gReuseSHM == 1) {
			_log(LH_MAIN, B_LOG_INFO,"trying to reuse SHM");
			gshm_id = shmget(ftok(gShmtok, 32), gSHMSize,IPC_CREAT | 0777);
			shm_got_reused=1;
		}
		
		
		if(gshm_id != -1) {

			orch_id=bartlby_orchestra_get_id(cfgfile);

			gBartlby_address=shmat(gshm_id,NULL,0);
			
			gshm_hdr=bartlby_SHM_GetHDR(gBartlby_address);
			
			
			gsvcmap=bartlby_SHM_ServiceMap(gBartlby_address);
			gshm_svc_cnt=gGetServiceMap(gsvcmap, cfgfile, orch_id);
			gshm_hdr->svccount=gshm_svc_cnt;
			
			
			gwrkmap = bartlby_SHM_WorkerMap(gBartlby_address);
			gshm_wrk_cnt=gGetWorkerMap(gwrkmap, cfgfile,orch_id);
			gshm_hdr->wrkcount=gshm_wrk_cnt;
			
			gdtmap=bartlby_SHM_DowntimeMap(gBartlby_address);
			gshm_dt_cnt=gGetDowntimeMap(gdtmap, cfgfile,orch_id);
			gshm_hdr->dtcount=gshm_dt_cnt;
				
			gsrvmap=bartlby_SHM_ServerMap(gBartlby_address);
			gshm_srv_cnt=gGetServerMap(gsrvmap, cfgfile, orch_id);
			
			gshm_hdr->srvcount=gshm_srv_cnt;
				
			//06.04.24 Init EVENT QUEUE
			bartlby_event_init(gBartlby_address);
			bartlby_ext_init(gBartlby_address, gSOHandle, cfgfile);
				
			
			//AddServerGroups
			gsrvgrpmap = bartlby_SHM_ServerGroupMap(gBartlby_address);
			gshm_srvgrp_cnt = gGetServerGroupMap(gsrvgrpmap, cfgfile, orch_id);
			gshm_hdr->srvgroupcount=gshm_srvgrp_cnt;
			
						
			//AddServicegroups
			gsvcgrpmap = bartlby_SHM_ServiceGroupMap(gBartlby_address);
			gshm_svcgrp_cnt = gGetServiceGroupMap(gsvcgrpmap, cfgfile, orch_id);
			gshm_hdr->svcgroupcount=gshm_svcgrp_cnt;
			
			
			_log(LH_MAIN, B_LOG_INFO,"Workers: %ld", gshm_hdr->wrkcount);
			_log(LH_MAIN, B_LOG_INFO,"Downtimes: %ld", gshm_hdr->dtcount);
			_log(LH_MAIN, B_LOG_INFO,"Servers: %ld", gshm_hdr->srvcount);
			_log(LH_MAIN, B_LOG_INFO,"ServerGroups: %ld", gshm_hdr->srvgroupcount);
			_log(LH_MAIN, B_LOG_INFO,"ServiceGroups: %ld", gshm_hdr->svcgroupcount);

			gshm_hdr->current_running=0;
			sprintf(gshm_hdr->version, "%s-%s (%s)", PROGNAME, VERSION, REL_NAME);
						
			gshm_hdr->do_reload=0;
			gshm_hdr->last_replication=-1;
			//shm_hdr->startup_time=time(NULL);
			if(shm_got_reused != 1) {
				gshm_hdr->startup_time=global_startup_time;
			}
			
			gshm_hdr->sirene_mode=0; //Default disable	
			gshm_hdr->size_of_structs=sizeof(struct shm_header)+sizeof(struct worker)+sizeof(struct service)+sizeof(struct downtime)+sizeof(struct server) + +sizeof(struct servergroup)+sizeof(struct servicegroup);
			gshm_hdr->pstat.sum=0;
			gshm_hdr->pstat.counter=0;
			
			gshm_hdr->checks_performed=0;
			gshm_hdr->checks_performed_time=time(NULL);
			
			
			
			if(gshm_hdr->wrkcount <= 0 && orch_id == 0) {
				_log(LH_MAIN, B_LOG_CRIT,"Found workers are below zero (%ld) maybe your datalib config isnt OK or you havent completed the setup", gshm_hdr->wrkcount);
				
				if(shmdt(gBartlby_address) < 0) {
					_log(LH_MAIN, B_LOG_CRIT,"shmdt() failed '%s`", strerror(errno));	
				}
				
				gshm_id = shmget(ftok(gShmtok, 32), 0, 0600);
				
				if(shmctl(gshm_id, IPC_RMID, &gshm_desc) < 0) {
					_log(LH_MAIN, B_LOG_CRIT,"shmctl() failed '%s`", strerror(errno));	
				}
				
				return -1;
				
			}
			
			bartlby_SHM_link_services_servers(gBartlby_address, cfgfile, gSOHandle);
			
			
		} else {
			
			
			_log(LH_MAIN, B_LOG_CRIT,"SHM is already exsisting do a `ipcrm shm SHMID' or something like that %s", strerror(errno));
			exit(1);
		}

	
	return 0;
}

int bartlby_go(char * cfgfile) {
	int exi_code=0;
	/*
	do_reload = 0 NO
	do_reload = 1 YES
	do_reload = 2 SCHEDULE DB IMPORT
	do_reload = 3 DB IMPORT PREPARED

	*/
	void * notification_log_hardcopy;
	int notification_log_last_max;
	time_t notification_log_last_run;

	bartlby_notification_log_init(gshm_hdr);
	
	
	while(exi_code != 1) {
		
		
		
		exi_code=schedule_loop(cfgfile, gBartlby_address, gSOHandle);
		
		_log(LH_MAIN, B_LOG_INFO,"Scheduler ended with: %d", exi_code);
		
		
		//Destroy SHM
		bartlby_ext_shutdown(exi_code);
		//write back all services 
		sched_write_back_all(cfgfile, gBartlby_address, gSOHandle);
		

		if(gshm_hdr->do_reload == 2) {
			_log(LH_MAIN, B_LOG_INFO,"Waiting to receive DB config");
			//sleep(3);
			gshm_hdr->do_reload=3; 
			while(gshm_hdr->do_reload != 0) {
				//_log("STILL WAITING TO CONTINUE");
				sleep(1);
			}	
		}
		//Make Notify Log Survivable
		notification_log_last_run=gshm_hdr->notification_log_aggregate_last_run;
		notification_log_last_max = gshm_hdr->notification_log_current_top;
		notification_log_hardcopy =  bartlby_notification_log_get_hardcopy(gshm_hdr);


		if(shmdt(gBartlby_address) < 0) {
			_log(LH_MAIN, B_LOG_CRIT,"shmdt() failed '%s`", strerror(errno));	
		}
		
		gshm_id = shmget(ftok(gShmtok, 32), 0, 0600);
		if(shmctl(gshm_id, IPC_RMID, &gshm_desc) < 0) {
			_log(LH_MAIN, B_LOG_CRIT,"shmctl() failed '%s`", strerror(errno));		
		}
		
		
		if(exi_code != 1) {
			//re populate SHM

			bartlby_shm_fits(gCfgfile);
			if(bartlby_populate_shm(gCfgfile) < 0) {
				//in case of zero workers
				exit(1);
			}
			bartlby_notification_log_set_hardcopy(gshm_hdr, notification_log_hardcopy, notification_log_last_max, notification_log_last_run);
		}
		

		
		
	}
	if(notification_log_hardcopy != NULL) free(notification_log_hardcopy);
	return 1;
}


int main(int argc, char ** argv) {
	char * daemon_mode;
	

	global_startup_time=time(NULL);
	
	
	
	//parse argv
	bartlby_parse_argv(argc, argv);
	
	
	set_cfg(gCfgfile);
	cfg_init_cache();
	cfg_fill_with_file(gCfgfile);
	

	
	//handle -d flag ;) 
	if(gDebug == 1) {
		cfg_update_cache("daemon", "false");
		cfg_update_cache("logfile", "/dev/stdout");
	}
	bartlby_init();

	if(gWBSHMToDB == 1) {
		bartlby_load_shm_stuff(gCfgfile);
		
		gshm_id = shmget(ftok(gShmtok, 32), gSHMSize, IPC_EXCL | 0777);
		
		gBartlby_address=shmat(gshm_id,NULL,0);
		gshm_hdr=bartlby_SHM_GetHDR(gBartlby_address);
		_log(LH_MAIN, B_LOG_INFO,"Writing Back SHM STATE to DB");
		sched_write_back_all(gCfgfile, gBartlby_address, gSOHandle);


		exit(1);
	}
	

	
	
	if(gOnlySHMPop == 1) {
		bartlby_load_shm_stuff(gCfgfile);
		bartlby_shm_fits(gCfgfile);

	
		if(bartlby_populate_shm(gCfgfile) < 0) {

			//in case of zero workers
			exit(1);
		}
		_log(LH_MAIN, B_LOG_INFO,"SHM Populated");
		exit(0);
	}
	
	bartlby_setuid();
	
	//set cfg
	
	
	
		
	
	
	
		
	
	//get deamon if needed
	daemon_mode=getConfigValue("daemon", gCfgfile);
	if(daemon_mode == NULL) {
		daemon_mode=strdup("false");	
	}

		
	
	
	bartlby_load_shm_stuff(gCfgfile);
	bartlby_shm_fits(gCfgfile);
	if(bartlby_populate_shm(gCfgfile) < 0) {
		//in case of zero workers
		exit(1);
	}
	
	if(strcmp(daemon_mode,"true") == 0) {	
		
		bartlby_get_daemon(gCfgfile);
	} 	

	bartlby_pre_init(gCfgfile);	


	//start it
	if(bartlby_go(gCfgfile) < 0) {
			_log(LH_MAIN, B_LOG_CRIT,"bartlby_go() failed");
	}
		
	
	free(gShmtok);
/*
	if(dlclose(gSOHandle) < 0) {
		_log(LH_MAIN, B_LOG_CRIT,"dlclose() failed '%s`", strerror(errno));	
	}	*/

	
	_log(LH_MAIN, B_LOG_INFO,"%s Ended(Daemon: %s)", PROGNAME, daemon_mode);	
	bartlby_end_clean(gCfgfile);
	
	free(daemon_mode);

	return 0;	
}




