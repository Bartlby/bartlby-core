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

static json_object   *payload_array;

struct upload_status {
	int lines_read;
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
	struct upload_status *upload_ctx = (struct upload_status *)userp;
	const char *data;

	if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
		return 0;
	}

	json_object *line = json_object_array_get_idx(payload_array, upload_ctx->lines_read);
	data = json_object_get_string(line);
	//data = payload_text[upload_ctx->lines_read];

	if (data) {
		size_t len = strlen(data);
		memcpy(ptr, data, len);
		upload_ctx->lines_read++;

		return len;
	}

	return 0;
}




void bartlby_trigger_smtp(char *cfgfile, struct service *svc, struct worker *wrk, struct trigger *trig, char *msg)
{
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *recipients = NULL;
	struct upload_status upload_ctx;
	json_object *smtp_options, *tmp_options[10];
	json_object *additional_headers;
	int additional_headers_length = 0;
	int x;

	char *tmpline;



	upload_ctx.lines_read = 0;

	smtp_options = json_tokener_parse(trig->trigger_data);
	if (!smtp_options) {
		//fprintf(stderr, "SMTP JSON Parse Error: %s", trig->trigger_data);
		//return;
	}
	fprintf(stderr,"SMTP OPTIONS: %s", json_object_to_json_string(smtp_options));

	if(!json_object_object_get_ex(smtp_options, "from", &tmp_options[3])) {
		_debug("SMTP FROM not set");
		return;
	}
	if(!json_object_object_get_ex(smtp_options, "url", &tmp_options[2])) {
		_debug("SMTP url not set error out");
		return;
	}



	payload_array = json_object_new_array();

	//FROM HEADER
	CHECKED_ASPRINTF(&tmpline, "From: <%s>\r\n", json_object_get_string(tmp_options[3]));
	json_object_array_add(payload_array, json_object_new_string(tmpline));
	free(tmpline);


	if(svc != NULL) {
		CHECKED_ASPRINTF(&tmpline, "Subject: [Bartlby]  %s/%s [%s->%s]\r\n", svc->srv->server_name, svc->service_name, getenv("BARTLBY_TRIGGER_SVC_CURRENT_STATE_READABLE"),getenv("BARTLBY_TRIGGER_SVC_CURRENT_LAST_STATE_READABLE"));
		
	} else {
		CHECKED_ASPRINTF(&tmpline, "Subject: [Bartlby] Aggregated\r\n");
	}
	json_object_array_add(payload_array, json_object_new_string(tmpline));
	free(tmpline);


	//User Supplied SMTP Headers
	if (json_object_object_get_ex(smtp_options, "headers", &additional_headers)) {
		additional_headers_length = json_object_array_length(additional_headers);
		for (x = 0; x < additional_headers_length; x++) {
			json_object_array_add(payload_array, json_object_array_get_idx(additional_headers, x));
		}
	}


	json_object_array_add(payload_array, json_object_new_string("\r\n"));


	//Data - Add Notification Message
	json_object_array_add(payload_array, json_object_new_string(msg));

	_debug("JSON TRIGGER_PAYLOAD: %s", json_object_to_json_string(payload_array));



	curl = curl_easy_init();
	if (curl) {

		if ( json_object_object_get_ex(smtp_options, "username", &tmp_options[0]) &&
				json_object_object_get_ex(smtp_options, "password", &tmp_options[1]) ) {

			//Authenticate
			curl_easy_setopt(curl, CURLOPT_USERNAME, json_object_get_string(tmp_options[0]));
			curl_easy_setopt(curl, CURLOPT_PASSWORD, json_object_get_string(tmp_options[1]));

		}

		if(json_object_object_get_ex(smtp_options, "timeout", &tmp_options[4])) {
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)json_object_get_int64(tmp_options[4]));
		} else {
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
		}


		curl_easy_setopt(curl, CURLOPT_URL, json_object_get_string(tmp_options[2]));
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, json_object_get_string(tmp_options[3]));


		recipients = curl_slist_append(recipients, wrk->mail);
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		//Hint for coverity
		/* coverity[bad_sizeof] */
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			_debug("SMTP Trigger failed: %s\n", curl_easy_strerror(res));
		}


		curl_slist_free_all(recipients);
		curl_easy_cleanup(curl);
	}
	json_object_put(payload_array);
	json_object_put(smtp_options);

}
