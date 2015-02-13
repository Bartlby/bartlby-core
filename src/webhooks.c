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

void bartlby_call_single_webhook(char * cfg,char * endpoint,  json_object * jso_out, int timeout) {
	json_object * jso_in, *jso_hook_status;
	const char * json_payload;
	struct http_output * http_out;


	
	json_payload = json_object_to_json_string(jso_out);
	http_out=bartlby_http_post_request(endpoint, (char *)json_payload, timeout); 
	
	if(http_out->curl_code == BARTLBY_HTTP_FINE) {
		
		jso_in=json_tokener_parse(http_out->ptr);
		if(json_object_object_get_ex(jso_in, "hook_status", &jso_hook_status)) { 
			_log(LH_MAIN, B_LOG_DEBUG, "Hook fired and status %ld", json_object_get_int64(jso_hook_status));
		} else {
			_log(LH_MAIN, B_LOG_DEBUG, "Hook fired without status code payload: %s - return payload", json_payload);
		}
		json_object_put(jso_in);
	} else {
		//_log(LH_MAIN, B_LOG_INFO,"HTTP RETURN CODE: %d error: %s", http_out->curl_code, curl_easy_strerror(http_out->curl_code));
		_log(LH_MAIN, B_LOG_CRIT, "webhook: http error %d error: %s", http_out->curl_code, curl_easy_strerror(http_out->curl_code));
		
	}
	bartlby_free_http_output(http_out);
	
}

void bartlby_call_webhooks(char * cfg, struct service * svc, int timeout, char * hook_in, json_object * additional_json) {
	/*
		hard:
			0 = BOTH
			1 = HARD
			2 = SOFT
	*/

	char * token;
	
	char * url;
	int x = 0;
	char * work_hooks;
	json_object * jso_out;
	

	if(strlen(hook_in) < 3) return;




  	work_hooks=strdup(hook_in);
	
  	//BUILD A JSON OBJECT -  With Service + additional data
  	jso_out = json_object_new_object();
	json_object_object_add(jso_out, "webhook",  json_object_new_string("webhook"));
	if(svc != NULL) {
		json_object_object_add(jso_out, "service",  bartlby_service_to_json(svc));
	} 
	if(additional_json != NULL) {
		json_object_object_add(jso_out, "additional", additional_json);
	}
	json_object_object_add(jso_out, "webhook",  json_object_new_int64(1));
	//BUILD A JSON OBJECT -  With Service + additional data



	 while ((token = strsep (&work_hooks, "\n")) != NULL) {
	 	url=strdup(token);
	 	trim(url);
	 	
	 	if(strlen(url) > 3) {
			bartlby_call_single_webhook(cfg, url, jso_out, timeout);
		}
      	x++;
      	free(url);
  	}
  	
	free(work_hooks);
	json_object_put(jso_out);
	

}



