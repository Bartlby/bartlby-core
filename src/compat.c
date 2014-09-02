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

/*
	
*/


#include <bartlby.h>



#ifdef NEEDS_JSON_GET_EX
int json_object_object_get_ex(struct json_object* jso, const char *key, struct json_object **value) {
 *value = json_object_object_get(jso, key);
    if (*value != NULL)
        return 1;
    else
        return 0;
}

#endif


#ifdef NEEDS_JSON_INT64
struct json_object* json_object_new_int64(int64_t i) {
	return json_object_new_int(i);
}


int32_t json_object_get_int64(struct json_object *obj) {
	return json_object_get_int(obj);
}
#endif