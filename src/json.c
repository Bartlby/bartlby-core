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

json_object * bartlby_trigger_to_json(struct trigger * trig) {
	//Caller has to free
	json_object * jso_out;
	jso_out = json_object_new_object();

	json_object_object_add(jso_out, "trigger_id", json_object_new_int64(trig->trigger_id));
	json_object_object_add(jso_out, "trigger_name", json_object_new_string(trig->trigger_name));
	

	return jso_out;


}

json_object * bartlby_service_to_json(struct service * svc) {
	//Caller has to free
	json_object * jso_out;
	jso_out = json_object_new_object();

	json_object_object_add(jso_out, "service_id", json_object_new_int64(svc->service_id));
	json_object_object_add(jso_out, "service_name", json_object_new_string(svc->service_name));
	json_object_object_add(jso_out, "server_name", json_object_new_string(svc->srv->server_name));
	json_object_object_add(jso_out, "server_id", json_object_new_int64(svc->srv->server_id));
	json_object_object_add(jso_out, "plugin", json_object_new_string(svc->plugin));
	json_object_object_add(jso_out, "plugin_arguments", json_object_new_string(svc->plugin_arguments));
	json_object_object_add(jso_out, "current_output", json_object_new_string(svc->current_output));
	json_object_object_add(jso_out, "current_state", json_object_new_int64(svc->current_state));
	

	return jso_out;


}



