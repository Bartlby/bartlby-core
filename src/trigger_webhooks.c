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




void bartlby_trigger_webhooks(char * cfgfile, struct service *svc, struct worker *wrk, struct trigger *trig, char *msg)
{
		
	json_object *json_packet;
	
	//CALL WEBHOOK TRIGGERS -> post request containing the service
	// 5 second fixed timeout?

	json_packet = json_object_new_object();
	json_object_object_add(json_packet, "trigger_name", json_object_new_string(trig->trigger_name));
	json_object_object_add(json_packet, "message", json_object_new_string(msg));

	bartlby_call_webhooks(cfgfile, svc,5, trig->trigger_data, json_packet);
	
	



}