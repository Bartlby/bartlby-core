/* $Id: bartlby.c,v 1.46 2008/03/17 19:04:43 hjanuschka Exp $ */
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
$Revision: 1.46 $
$Source: /cvsroot/bartlby/bartlby-core/src/bartlby.c,v $


$Log: bartlby.c,v $
Revision 1.46  2008/03/17 19:04:43  hjanuschka
SF Feature issue #1916495 changed static key,value length of config cache to pre defined constant variables (config.c, bartlby.h) reported by Markus Elfring (elfring)

Revision 1.43  2007/07/27 22:54:04  hjanuschka
int to long changing

Revision 1.42  2007/02/15 20:46:38  hjanuschka
auto commit

Revision 1.41  2007/02/15 16:25:32  hjanuschka
auto commit

Revision 1.40  2007/01/05 01:49:00  hjanuschka
auto commit

Revision 1.38  2006/11/28 03:30:42  hjanuschka
auto commit

Revision 1.37  2006/10/05 23:19:37  hjanuschka
auto commit

Revision 1.36  2006/09/09 19:38:34  hjanuschka
auto commit

Revision 1.35  2006/09/03 22:19:47  hjanuschka
auto commit

Revision 1.34  2006/08/03 20:29:13  hjanuschka
auto commit

Revision 1.33  2006/07/25 21:42:03  hjanuschka
auto commit

Revision 1.32  2006/06/04 23:55:28  hjanuschka
core: SSL_connect (timeout issue's solved , at least i hope :))
core: when perfhandlers_enabled == false, you now can enable single services
core: plugin_arguments supports $MACROS
core: config variables try now to cache themselfe to minimize I/O activity
core: .so extensions support added

Revision 1.31  2006/05/28 16:18:27  hjanuschka
commit before release

Revision 1.30  2006/05/24 19:18:35  hjanuschka
version bump

Revision 1.29  2006/05/21 21:18:10  hjanuschka
commit before workweek

Revision 1.28  2006/05/20 20:52:18  hjanuschka
set core dump limit in deamon mode
snmp minimal fixes
announce if SNMP is compiled in on startup

Revision 1.27  2006/04/24 22:20:00  hjanuschka
core: event queue

Revision 1.26  2006/04/23 18:07:43  hjanuschka
core/ui/php: checks can now be forced
ui: remote xml special_addon support
core: svc perf MS
core: round perf MS
php: svcmap, get_service perf MS
ui: perf MS

Revision 1.25  2006/02/25 18:53:22  hjanuschka
setuid too late, pid file was created by root
makefile: copy mysql.shema to $BASEDIR

Revision 1.24  2006/02/25 02:02:46  hjanuschka
core: configure/ --with-user=
core: configure/ install all files and directories with chown $BARTLBY_USER
core: lib/mysql [worker|service|server]_by_id returns negative value if not found

Revision 1.23  2006/02/10 23:54:46  hjanuschka
SIRENE mode added

Revision 1.22  2006/02/05 21:49:47  hjanuschka
*** empty log message ***

Revision 1.21  2006/01/19 23:30:22  hjanuschka
introducing downtime's

Revision 1.20  2006/01/09 23:53:10  hjanuschka
minor changes

Revision 1.19  2006/01/08 16:17:24  hjanuschka
mysql shema^

Revision 1.18  2005/12/13 23:17:53  hjanuschka
setuid before creating shm segment

Revision 1.17  2005/11/16 23:51:29  hjanuschka
version bump 0.9.9a (Exusiai)
replication tests minor fixes

Revision 1.16  2005/10/25 20:36:32  hjanuschka
startup time is'nt reset on cfg reload now

Revision 1.15  2005/09/30 23:58:02  hjanuschka
*** empty log message ***

Revision 1.14  2005/09/28 21:46:30  hjanuschka
converted files to unix
jabber.sh -> disabled core dumps -> jabblibs segfaults
                                    will try to patch it later

Revision 1.13  2005/09/18 11:28:12  hjanuschka
replication now works :-)
core: can run as slave and load data from a file instead of data_lib
ui: displays a warning if in slave mode to not add/modify servers/services
portier: recieves and writes shm dump to disk
so hot stand by should be possible ;-)
slave also does service checking

Revision 1.12  2005/09/18 05:05:43  hjanuschka
compile warnings

Revision 1.11  2005/09/18 04:04:52  hjanuschka
replication interface (currently just a try out)
one instance can now replicate itself to another using portier as a transport way
FIXME: need to sort out a binary write() problem

Revision 1.10  2005/09/13 19:43:31  hjanuschka
human readable release code name REL_NAME
fixed printf() in shutdown daemon *fg*

Revision 1.9  2005/09/13 19:29:18  hjanuschka
daemon: pidfile, remove pidfile at end
mysql.c: fixed 2 segfaults under _MALLOC_CHECK=2

Revision 1.8  2005/09/06 20:59:12  hjanuschka
performance tests fixes addition's

Revision 1.7  2005/09/05 19:53:12  hjanuschka
2 day uptime without a single sigsegv ;-)
added daemon function ;-)
	new cfg vars daemon=[true|false], basedir, logfile

Revision 1.6  2005/09/03 23:01:13  hjanuschka
datalib api refined
moved to version 0.9.7
reload via SHM

Revision 1.5  2005/09/03 20:11:22  hjanuschka
fixups

added addworker, deleteworker, modifyworker, getworkerbyid

Revision 1.4  2005/08/28 18:00:22  hjanuschka
data_lib api extended, service/add/delete/update

Revision 1.3  2005/08/28 15:59:47  hjanuschka
CVS header ;-)

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
#include <getopt.h>

#include <dlfcn.h>


#ifdef HAVE_SSL 
	#include <openssl/dh.h>
	#include <openssl/ssl.h>
	#include <openssl/err.h>
	
#endif

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
int (*gGetServiceMap)(struct service *, char *);
int (*gGetServerMap)(struct server *, char *);
int (*gGetWorkerMap)(struct worker *,char *);
int (*gGetDowntimeMap)(struct downtime *, char *);
struct  shm_counter  * (*gGetCounter)(char *);
char * gShmtok;
struct service * gsvcmap;
struct worker * gwrkmap;
struct downtime * gdtmap;
struct server * gsrvmap;
int global_startup_time;

int gshm_id;
void * gBartlby_address;
int gshm_svc_cnt;
int gshm_wrk_cnt;
int gshm_dt_cnt;
int gshm_srv_cnt;
long gSHMSize;

struct shmid_ds gshm_desc;

struct shm_header * gshm_hdr;


int gReuseSHM=0;





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
		{ "reuse",	0, NULL, 'r'},
		{ "version",	0, NULL, 'v'},
		
		{ NULL,		0, NULL, 0}
	};
	int c;

	if(argc == 1) {
		dispHelp();		
	}
	
	for (;;) {
		c = getopt_long(argc, argv, "hdrv", longopts, (int *) 0);
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
		_log("No data_library specified in `%s' config file", cfgfile);
		exit(1);
	}
	_log("using data lib: `%s'", gSOName);
	gSOHandle=dlopen(gSOName, RTLD_LAZY);
		
    	if((dlmsg=dlerror()) != NULL) {
        	_log("Error: %s", dlmsg);
        	exit(1);
    	}
    	LOAD_SYMBOL(gGetAutor,gSOHandle, "GetAutor");
    	LOAD_SYMBOL(gGetVersion,gSOHandle, "GetVersion");
    	LOAD_SYMBOL(gGetServiceMap,gSOHandle, "GetServiceMap");
    	LOAD_SYMBOL(gGetServerMap,gSOHandle, "GetServerMap");
    	LOAD_SYMBOL(gGetWorkerMap,gSOHandle, "GetWorkerMap");
    	LOAD_SYMBOL(gGetDowntimeMap,gSOHandle, "GetDowntimeMap");
    	LOAD_SYMBOL(gGetName,gSOHandle, "GetName");
    	LOAD_SYMBOL(gExpectVersion,gSOHandle, "ExpectVersion");
    	LOAD_SYMBOL(gGetCounter,gSOHandle, "GetCounter");
    	    	
    	gGetAutorStr=gGetAutor();
    	gGetVersionStr=gGetVersion();
    	gGetNameStr=gGetName();
    	
    	if(gExpectVersion() > EXPECTCORE || EXPECTCORE < gExpectVersion() || EXPECTCORE != gExpectVersion()) {
    		_log("*****Version check failed Module is compiled for version '%ld' of %s", gExpectVersion(), PROGNAME);	
    		_log("*****The Module is compiled under '%d' Version of %s", EXPECTCORE, PROGNAME);
    		exit(1);
    	} 
    	
    	_log("Data Lib (%s) by: '%s' Version: %s", gGetNameStr, gGetAutorStr, gGetVersionStr);
    		
	free(gGetAutorStr);
	free(gGetVersionStr);
	free(gGetNameStr);
	
	gShmtok = getConfigValue("shm_key", cfgfile);
	if(gShmtok == NULL) {
		_log("Unset variable `shm_key'");
		exit(1);
	}
	
	
    	
	
	
	
		
}
void bartlby_init(void) {
	
	_log("%s Version %s (%s) started. compiled %s/%s", PROGNAME, VERSION,REL_NAME, __DATE__, __TIME__);
	#ifdef SNMP_ADDON
	_log("SNMP support compiled in");
	#endif
	#ifdef HAVE_SSL
	_log("SSL support compiled in");
	SSL_library_init();
	SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();
	_log("ssl lib init, ssl_algo, error_strings");	
	#endif
	#ifdef WITH_NRPE
	_log("NRPE Support compiled in");
	#endif
	
}
void bartlby_setuid(void) {
	char * cfg_user;
	struct passwd * ui;
	
	
	cfg_user = getConfigValue("user", gCfgfile);
	if(cfg_user == NULL) {
		_log("user not set in config file");
		exit(2);			
	}
	ui=getpwnam(cfg_user);
	if(ui == NULL) {
		_log("User: %s not found cannot setuid running as %d", cfg_user, getuid());	
	} else {
		if(setuid(ui->pw_uid) < 0) {
			_log("setuid() failed: %s", ui->pw_name);	
			exit(2);
		}
		if(setgid(ui->pw_gid) < 0) {
			_log("setgid() failed: %d '%s`", ui->pw_gid, strerror(errno));	
		//	exit(2);
			
		}
		_log("User: %s/%d", ui->pw_name, ui->pw_gid);	
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
		
		suggested_minimum = (sizeof(struct shm_header) + (sizeof(struct server) * shmc->servers) + (sizeof(struct worker) * shmc->worker) + (sizeof(struct service) * shmc->services) + (sizeof(struct downtime) * shmc->downtimes) + 2000 + (sizeof(struct btl_event)*EVENT_QUEUE_MAX)) * 2;
		if(gSHMSize <= suggested_minimum) {
			_log("SHM is to small minimum: %d KB ", suggested_minimum/1024);
			exit(1);	
		}
		_log("SHM requires: %d KB ", suggested_minimum/1024);
		_log("Size: S=%ld, W=%ld, D=%ld, H=%ld, E=%ld", sizeof(struct service), sizeof(struct worker), sizeof(struct downtime), sizeof(struct shm_header), sizeof(struct btl_event));
		free(shmc);
		

	
}

int bartlby_populate_shm(char * cfgfile) {
		
		gshm_id = shmget(ftok(gShmtok, 32), gSHMSize,IPC_CREAT | IPC_EXCL | 0777);
		
		if(gshm_id < 0 && gReuseSHM == 1) {
			_log("trying to reuse SHM");
			gshm_id = shmget(ftok(gShmtok, 32), gSHMSize,IPC_CREAT | 0777);
		}
		
		if(gshm_id != -1) {
			gBartlby_address=shmat(gshm_id,NULL,0);
			
			gshm_hdr=(struct shm_header *)(void *)gBartlby_address;
			gsvcmap=(struct service *)(void *)gBartlby_address+sizeof(struct shm_header);
			
							
				
			gshm_svc_cnt=gGetServiceMap(gsvcmap, cfgfile);
			
			gshm_hdr->svccount=gshm_svc_cnt;
			
			gsvcmap=bartlby_SHM_ServiceMap(gBartlby_address);
			
			gwrkmap=(struct worker *)(void*)&gsvcmap[gshm_svc_cnt]+20;
			gshm_wrk_cnt=gGetWorkerMap(gwrkmap, cfgfile);
			gshm_hdr->wrkcount=gshm_wrk_cnt;
			
			gdtmap=(struct downtime *)(void *)&gwrkmap[gshm_wrk_cnt]+20;
			gshm_dt_cnt=gGetDowntimeMap(gdtmap, cfgfile);
			gshm_hdr->dtcount=gshm_dt_cnt;
				
			gsrvmap=(struct server *)(void *)&gdtmap[gshm_dt_cnt]+20;
			gshm_srv_cnt=gGetServerMap(gsrvmap, cfgfile);
			
			gshm_hdr->srvcount=gshm_srv_cnt;
				
			//06.04.24 Init EVENT QUEUE
			bartlby_event_init(gBartlby_address);
			bartlby_ext_init(gBartlby_address, gSOHandle, cfgfile);
				
			
			_log("Workers: %ld", gshm_hdr->wrkcount);
			_log("Downtimes: %ld", gshm_hdr->dtcount);
			_log("Servers: %ld", gshm_hdr->srvcount);
			gshm_hdr->current_running=0;
			sprintf(gshm_hdr->version, "%s-%s (%s)", PROGNAME, VERSION, REL_NAME);
						
			gshm_hdr->do_reload=0;
			gshm_hdr->last_replication=-1;
			//shm_hdr->startup_time=time(NULL);
			if(gReuseSHM != 1) {
				gshm_hdr->startup_time=global_startup_time;
			}
			
			gshm_hdr->sirene_mode=0; //Default disable	
			gshm_hdr->size_of_structs=sizeof(struct shm_header)+sizeof(struct worker)+sizeof(struct service)+sizeof(struct downtime)+sizeof(struct server);
			gshm_hdr->pstat.sum=0;
			gshm_hdr->pstat.counter=0;
			
			
			
			
			
			if(gshm_hdr->wrkcount <= 0) {
				_log("Found workers are below zero (%ld) maybe your datalib config isnt OK or you havent completed the setup", gshm_hdr->wrkcount);
				
				if(shmdt(gBartlby_address) < 0) {
					_log("shmdt() failed '%s`", strerror(errno));	
				}
				
				gshm_id = shmget(ftok(gShmtok, 32), 0, 0600);
				
				if(shmctl(gshm_id, IPC_RMID, &gshm_desc) < 0) {
					_log("shmctl() failed '%s`", strerror(errno));	
				}
				
				return -1;
				
			}
			
			bartlby_SHM_link_services_servers(gBartlby_address, cfgfile);
			
			
		} else {
			
			
			_log("SHM is already exsisting do a `ipcrm shm SHMID' or something like that");
			exit(1);
		}

	return 0;
}

int bartlby_go(char * cfgfile) {
	int exi_code=0;
	
	while(exi_code != 1) {
		
		
		exi_code=schedule_loop(cfgfile, gBartlby_address, gSOHandle);
		_log("Scheduler ended with: %d", exi_code);
		
		
		
		
		
		//Destroy SHM
		bartlby_ext_shutdown(exi_code);
		//write back all services 
		sched_write_back_all(cfgfile, gBartlby_address, gSOHandle);
		
				
		if(shmdt(gBartlby_address) < 0) {
			_log("shmdt() failed '%s`", strerror(errno));	
		}
		
		gshm_id = shmget(ftok(gShmtok, 32), 0, 0600);
		if(shmctl(gshm_id, IPC_RMID, &gshm_desc) < 0) {
			_log("shmctl() failed '%s`", strerror(errno));		
		}
		
		
		if(exi_code != 1) {
			//re populate SHM called reload *fg*
			bartlby_shm_fits(gCfgfile);
			if(bartlby_populate_shm(gCfgfile) < 0) {
				//in case of zero workers
				exit(1);
			}
		}
		
		
	}

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
	
	bartlby_setuid();
	
	//set cfg
	
	
	
		
	
	
	
		
	
	//get deamon if needed
	daemon_mode=getConfigValue("daemon", gCfgfile);
	if(daemon_mode == NULL) {
		daemon_mode=strdup("false");	
	}
	if(strcmp(daemon_mode,"true") == 0) {	
		
		bartlby_get_daemon(gCfgfile);
	} 	
		
	bartlby_pre_init(gCfgfile);	
	//populate shm
	
	bartlby_load_shm_stuff(gCfgfile);
	
	bartlby_shm_fits(gCfgfile);
	
	if(bartlby_populate_shm(gCfgfile) < 0) {
		//in case of zero workers
		exit(1);
	}
	
	//start it
	if(bartlby_go(gCfgfile) < 0) {
			_log("bartlby_go() failed");
	}
		
	
	free(gShmtok);

	if(dlclose(gSOHandle) < 0) {
		_log("dlclose() failed '%s`", strerror(errno));	
	}	

	_log("%s Ended(Daemon: %s)", PROGNAME, daemon_mode);	
	bartlby_end_clean(gCfgfile);
	
	free(daemon_mode);

return 1;	
}




