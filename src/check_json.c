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


input is like 
				{
					"plugin": "bartlby_load",
					"plugin_arguments": " -w 10 -c 20 -p"
				}

return data:
				{
					"output": "SOME OUTPUT",
					"exit_code": 2‚
				}


*/


#include <bartlby.h>



void bartlby_check_json(struct service * svc, char * cfgfile) {
	json_object * jso_out, *jso_output, *jso_exitcode, *jso_in;
	const char * json_payload;
	struct http_output * http_out;
	int temp_status;

	jso_out = bartlby_service_to_json(svc);
	json_payload = json_object_to_json_string(jso_out);

	
	
	http_out=bartlby_http_post_request(svc->srv->json_endpoint, (char *)json_payload, svc->service_check_timeout); 

	json_object_put(jso_out);

	
	if(http_out->curl_code == BARTLBY_HTTP_FINE) {
		//_log(LH_MAIN, B_LOG_INFO,"HTTP RETURN DATA: %s", http_out->ptr);
		jso_in=json_tokener_parse(http_out->ptr);
		if(json_object_object_get_ex(jso_in, "output", &jso_output)) { 
			snprintf(svc->new_server_text, 2047, "%s", json_object_get_string(jso_output));
		} else {
			snprintf(svc->new_server_text, 2047, "No Output specified");
		}
		
		if(json_object_object_get_ex(jso_in, "exit_code", &jso_exitcode)) { 
			temp_status=json_object_get_int64(jso_exitcode);
			switch(temp_status)  {
				case STATE_OK:
				case STATE_WARNING:
				case STATE_CRITICAL:
					svc->current_state=temp_status;
				break;
				default:
					svc->current_state=STATE_UNKOWN;
				break;
			}
		} else {
			svc->current_state=STATE_CRITICAL;
		}
		json_object_put(jso_in);

	} else {
		//_log(LH_MAIN, B_LOG_INFO,"HTTP RETURN CODE: %d error: %s", http_out->curl_code, curl_easy_strerror(http_out->curl_code));
		snprintf(svc->new_server_text, 2048, "http error %d error: %s", http_out->curl_code, curl_easy_strerror(http_out->curl_code));
		svc->current_state = STATE_CRITICAL;
	}
	bartlby_free_http_output(http_out);


	
	bartlby_check_grep_perf_line(svc->new_server_text, svc, cfgfile);
    return;
		
}


