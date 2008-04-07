/* $Id: shmt.c,v 1.7 2008/03/03 12:01:27 hjanuschka Exp $ */
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
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>



#include <bartlby.h>

void bartlby_exit(void) {
	//bartlby_log_usage();	
}


void bartlby_log_usage(void) {
	struct rusage ru;
	
	if(getrusage(RUSAGE_CHILDREN, &ru) == -1 ) {
		_log("getrusage() error...(%s)", strerror(errno));	
		return;
	}
	_log("*********** dumping usage of bartlby process...*****************");
	_log("user time used (secs/usecs): %ld/%ld", ru.ru_utime.tv_sec, ru.ru_utime.tv_usec);
	_log("system time used (secs/usecs): %ld/%ld", ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);
	_log("%-10ld\t%s", ru.ru_maxrss, " maximum resident size"); 
	_log("%-10ld\t%s", ru.ru_ixrss, " integral shared memory size"); 
	_log("%-10ld\t%s", ru.ru_idrss, " integral unshared memory size"); 
	_log("%-10ld\t%s", ru.ru_isrss, " integral unshared data stack size"); 
	_log("%-10ld\t%s", ru.ru_minflt, " page reclaims"); 
	_log("%-10ld\t%s", ru.ru_majflt, " page faults"); 
	_log("%-10ld\t%s", ru.ru_nswap, " swaps"); 
	
	_log("%-10ld\t%s", ru.ru_inblock, " block input operations"); 
	_log("%-10ld\t%s", ru.ru_oublock, " block output operations"); 
	_log("%-10ld\t%s", ru.ru_msgsnd, " messages sent"); 
	_log("%-10ld\t%s", ru.ru_msgrcv, " messages received"); 
	_log("%-10ld\t%s", ru.ru_nsignals, " signals received"); 
	_log("%-10ld\t%s", ru.ru_nvcsw, " voluntary context switches"); 
	_log("%-10ld\t%s", ru.ru_nivcsw, " involuntary context switches"); 
	_log("******************* DONE ***************************************");
	
	
}

void bartlby_read_limit(int resource, char *name)
{

	struct rlimit limit;

   	
	if(getrlimit(resource, &limit) == -1)	{
		_log("getrlimit error...\n");
		return;
	}
	limit.rlim_cur=RLIM_INFINITY;
	if(setrlimit(resource, &limit) == -1) {
		_log("setrlimit error...%s (%s)",name, strerror(errno));
	}
	
	if(getrlimit(resource, &limit) == -1)	{
		_log("getrlimit error...\n");
		return;
	}

	if(limit.rlim_cur == RLIM_INFINITY) {
		_log("%15s | Soft-Limit : unlimited",name);
	} else {
		_log("%15s | Soft-Limit : %12ld",name, limit.rlim_cur);
		
	}
	
	if(limit.rlim_max == RLIM_INFINITY) {
		_log("%15s | Hard-Limit : unlimited",name);
	} else {
		_log("%15s | Hard-Limit : %12ld",name, limit.rlim_cur);
		
	}

	
}

void bartlby_set_limits(void) {
	int limit_value[] = {RLIMIT_CORE, RLIMIT_CPU, RLIMIT_DATA, RLIMIT_FSIZE,
                    RLIMIT_MEMLOCK, RLIMIT_NPROC,
                    RLIMIT_RSS, RLIMIT_STACK, /*RLIMIT_VMEM*/};

	char *limit_name[] = {"RLIMIT_CORE","RLIMIT_CPU","RLIMIT_DATA","RLIMIT_FSIZE",
                      "RLIMIT_MEMLOCK","RLIMIT_NPROC",
                      "RLIMIT_RSS","RLIMIT_STACK",/*"RLIMIT_VMEM",*/NULL};
                      
	int i;

	for(i=0; limit_name[i]!= NULL; i++) {
		bartlby_read_limit(limit_value[i], limit_name[i]);
		
	}
     
     
}
void bartlby_log_banner(void) {
	_log("*****************Welcome to Bartlby*********************");	
	_log("*        Next generation of system monitoring          *");
	_log("*                                                      *");
	_log("*       License: GPLv2                                 *");
	_log("*                                                      *");
	_log("********************************************************");
}


void bartlby_pre_init(char * cfgfile) {
	FILE * pidfile;
	char * base_dir;
	char pidfname[1024];
	char pidstr[1024];
	char * pid_def_name;
	
	
	//atexit(bartlby_exit);
	bartlby_log_banner();
	
	base_dir = getConfigValue("basedir", cfgfile);
	pid_def_name = getConfigValue("pidfile_dir", cfgfile);
	
	
	
	if(base_dir == NULL) {
		
		base_dir=strdup("/");
	}
	if(pid_def_name == NULL) {
		pid_def_name=strdup(base_dir);
	}
	
	bartlby_set_limits();
  	
  	
	if(chdir(base_dir) < 0) {		
		_log("basedir setting failed :%s (%s)", base_dir,strerror(errno));
		exit(1);	
	} else {
		_log("basedir set to:%s", base_dir);	
	}
	
	/* out of manpage: This system call always succeeds and the previous value of the mask is returned.*/
	umask(0);
	
	
	sprintf(pidfname, "%s/bartlby.pid", pid_def_name);
	pidfile=fopen(pidfname, "w");
	if(pidfile == NULL) {
		_log("Pid file  failed '%s'", pidfname);
		
	} else {
		sprintf(pidstr, "%d", getpid());
		if(fwrite(pidstr, sizeof(char), strlen(pidstr), pidfile) <= 0) {
			_log("pidfile creation failed");
		} else {
			if(fclose(pidfile) == EOF) {
				_log("fclose() failed for pidfile!!");
				exit(1);
			}
			_log("pidfile is at: '%s'", pidfname);
		}
		
	}
	
	if(setenv("BARTLBY_HOME", base_dir,1) == 0) {
		_log("$BARTLBY_HOME='%s'", base_dir);
	} else {
		_log("setenv $BARTLBY_HOME='%s' failed", base_dir);	
	}
	//freopen("/dev/null", "a", stderr);
	//freopen("/dev/null", "a", stdout);
	
	
	free(base_dir);
	free(pid_def_name);
	
	
}

void bartlby_end_clean(char *cfgfile) {
	char * base_dir;
	char pidfname[1024];
	
	char * pid_def_name;
	base_dir = getConfigValue("basedir", cfgfile);
	pid_def_name = getConfigValue("pidfile_dir", cfgfile);
	
	bartlby_log_usage();
	
	if(base_dir == NULL) {
		
		base_dir=strdup("/");
	}
	if(pid_def_name == NULL) {
		pid_def_name=strdup(base_dir);
	}
	sprintf(pidfname, "%s/bartlby.pid", pid_def_name);
	
	if(unlink(pidfname) == 0) {	
		_log("%s Pid file removed", pidfname);
	} else {
		_log("%s Pid file remove failed", pidfname);
	}
	free(base_dir);
	free(pid_def_name);
	
	
}

void bartlby_get_daemon(char * cfgfile) {
	pid_t pid;
	
		
	if ((pid = fork ()) != 0) {
		//_log("Fork failed");
      		exit(1);
      	}
      	
	if(setsid() < 0 ) {
		_log("Cannot setsid()");	
	}
	
	
	
	signal(SIGHUP, SIG_IGN);
	
	
	
	return;
	
}
