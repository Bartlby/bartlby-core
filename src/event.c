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






void * bartlby_event_queue_set_hardcopy(void * bartlby_address, void * hardcopy, int event_queue_last) {
	long event_queue_size;
	struct btl_event * ev_temp;
	struct shm_header * hdr_temp;

	ev_temp=bartlby_SHM_EventMap(bartlby_address);
	hdr_temp=bartlby_SHM_GetHDR(bartlby_address);

	event_queue_size=sizeof(struct btl_event)*EVENT_QUEUE_MAX;
	_log(LH_EVNT, B_LOG_DEBUG,"EVENT QUEUE: SET HARDCOPY: %d", event_queue_size);
	memcpy(ev_temp, hardcopy, event_queue_size);
	free(hardcopy);
	hdr_temp->cur_event_index=event_queue_last;
	
	return NULL;
}


void * bartlby_event_queue_get_hardcopy(void * bartlby_address) {

	void * return_hardcopy;
	
	long event_queue_size;
	struct btl_event * ev_temp;
	

	ev_temp=bartlby_SHM_EventMap(bartlby_address);
	
	event_queue_size=sizeof(struct btl_event)*NOTIFICATION_LOG_MAX;
	_log(LH_EVNT, B_LOG_DEBUG,"EVENT QUEUE: GET HARDCOPY: %d", event_queue_size);
	return_hardcopy  = malloc(event_queue_size);
	memcpy(return_hardcopy, ev_temp, event_queue_size);

	return return_hardcopy;


}


void bartlby_event_init(void * bartlby_address) {
	int x;
		
	struct btl_event * evs;
	struct shm_header * hdr;


	hdr=bartlby_SHM_GetHDR(bartlby_address);
	evs=bartlby_SHM_EventMap(bartlby_address);
	hdr->cur_event_index=0;
	
	for(x=0; x<EVENT_QUEUE_MAX; x++) {
		evs[x].evnt_id=0;
		evs[x].evnt_time=time(NULL);
		sprintf(evs[x].evnt_message, "(null)");
	}
	_log(LH_EVNT, B_LOG_INFO,"Init event queue done %d messages available", x);
}

int bartlby_push_event(char * cfgfile,  void * bartlby_address, int event_id, const char * str) {
//	printf("LOG: %s\n", str);

	struct btl_event * evs;
	struct shm_header * hdr;


	hdr=bartlby_SHM_GetHDR(bartlby_address);
	evs=bartlby_SHM_EventMap(bartlby_address);

	
	
	char * sem_name;
	sem_t * sem_id;
	int x;

	sem_name=getConfigValue("event_queue_sem_name", cfgfile);
	if(sem_name == NULL) {
		sem_name=strdup("event_queue_sem_name_generic");
		
	}
	sem_id=sem_open(sem_name, O_CREAT, 0755, 1);
	sem_wait(sem_id);
		
 	x=hdr->cur_event_index;
 	evs[x].evnt_id=event_id;
 	evs[x].evnt_time=time(NULL);
 	bartlby_callback(EXTENSION_CALLBACK_EVENT_PUSHED, &evs[x]);
   	snprintf(evs[x].evnt_message, 4095,"%s", str);
   	

	if((hdr->cur_event_index+1) == EVENT_QUEUE_MAX) {
		//_log("Event: %d will reach maximum", (hdr->cur_event_index+1));
		hdr->cur_event_index=0;	
	} else {
		hdr->cur_event_index++;	
		//_log("NEW CID: %d", hdr->cur_event_index);
	}

	sem_post(sem_id);
	free(sem_name);
	
   	
   	
   	return 1;   
}

