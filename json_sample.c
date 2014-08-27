#include <stdio.h>
#include <json.h>

int main(int argc, char ** argv) {

	json_object *my_string, *my_int, *my_object, *my_array, *my_object1, *my_object2;

	my_object = json_object_new_object();
	json_object_object_add(my_object, "key1", json_object_new_int(12));
	json_object_object_add(my_object, "key2", json_object_new_int(13));
	json_object_object_add(my_object, "key3", json_object_new_string("asdf"));

	printf("obj: %s\n", json_object_to_json_string(my_object));

	

	
	json_object_put(my_object);
	

	my_object1 = json_tokener_parse(argv[1]);
	if(json_object_object_get_ex(my_object1, "key1", &my_object2)) {
		printf("KEY1: %s \n", json_object_get_string(my_object2));
	}


	json_object_put(my_object1);
	json_object_put(my_object2);



	/*** loop threw object **/

	/*
		json_object_object_foreach(my_object, key, val)
	{
		printf("\t%s: %s\n", key, json_object_to_json_string(val));
	}
	*/

	/****** PARSE *****/

	


}
