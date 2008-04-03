/* $Id: replication.c,v 1.8 2006/06/05 21:06:07 hjanuschka Exp $ */
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
$Revision: 1.8 $
$Source: /cvsroot/bartlby/bartlby-core/src/replication.c,v $


$Log: replication.c,v $
Revision 1.8  2006/06/05 21:06:07  hjanuschka
*** empty log message ***

Revision 1.7  2006/04/23 18:07:43  hjanuschka
core/ui/php: checks can now be forced
ui: remote xml special_addon support
core: svc perf MS
core: round perf MS
php: svcmap, get_service perf MS
ui: perf MS

Revision 1.6  2005/10/25 20:36:32  hjanuschka
startup time is'nt reset on cfg reload now

Revision 1.5  2005/09/28 21:46:30  hjanuschka
converted files to unix
jabber.sh -> disabled core dumps -> jabblibs segfaults
                                    will try to patch it later

Revision 1.4  2005/09/18 11:28:12  hjanuschka
replication now works :-)
core: can run as slave and load data from a file instead of data_lib
ui: displays a warning if in slave mode to not add/modify servers/services
portier: recieves and writes shm dump to disk
so hot stand by should be possible ;-)
slave also does service checking

Revision 1.3  2005/09/18 05:03:52  hjanuschka
replication is false by default now
need to fix the damn write()/read() -> while() sh**

Revision 1.2  2005/09/18 04:04:52  hjanuschka
replication interface (currently just a try out)
one instance can now replicate itself to another using portier as a transport way
FIXME: need to sort out a binary write() problem

Revision 1.1  2005/09/18 01:33:54  hjanuschka
*** empty log message ***


*/

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>  
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <bartlby.h>
#include <errno.h>




int replication_go(char * cfgfile, void * shm_addr, void * SOHandle) {
	//_log("FIXME: replication with external program XML-RPC API");
	bartlby_callback(EXTENSION_CALLBACK_REPLICATION_GO, NULL);
	return 1;
}
