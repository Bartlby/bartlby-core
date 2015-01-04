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




void bartlby_free_http_output(struct http_output * s) {
	free(s->ptr);
	free(s);
}
void bartlby_init_http_output(struct http_output *s)
{
	s->len = 0;
	s->ptr = malloc(s->len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
	s->curl_code=-1;
}

size_t bartlby_http_recv(void *ptr, size_t size, size_t nmemb, void *buffer)
{
	struct http_output * s = buffer;
	size_t new_len = s->len + size * nmemb;
	s->ptr = realloc(s->ptr, new_len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr + s->len, ptr, size * nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size * nmemb;
}

struct http_output * bartlby_http_post_request(char *url, char *body, long timeout)
{

	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		struct http_output * s; 
		s = malloc(sizeof(struct http_output));
		bartlby_init_http_output(s);

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, bartlby_http_recv);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, s);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		res = curl_easy_perform(curl);
		s->curl_code=res;
		curl_easy_cleanup(curl);
		return s;
		
	}
	return NULL;

}
