/*
baseline
*/


#include <bartlby.h>


//SOURCE TYPE 

#define BARTLBY_BARTLBY_BASELINE_DS_HISTORY 1
#define BARTLBY_BARTLBY_BASELINE_DS_DB 2


// statistic algos
#define BARTLBY_STATISTIC_STD_DEVIATION 1
#define BARTLBY_STATISTIC_EXP_SMOOTH 2        



//seconds for the time window to look at
#define BARTLBY_BASELINE_TIME_TOLERANCE 30

//tolerance for the value in percentage of the standard deviation
#define BARTLBY_BASELINE_VALUE_TOLERANCE 400

//Days to lookback for baseline calculation
#define BARTLBY_BASELINE_DAYS_BACK 7
  

#define BASE_HISTORY_PATH "/tmp/bartlby_test/"





void bartlby_baseline_destroy(BARTLBY_BASELINE * bsl) {

    json_object_put(bsl->json_result);


}


float bartlby_baseline_standard_deviation(float data[], int n)
{
    float mean = 0.0, sum_deviation = 0.0;
    int i;
    for (i = 0; i < n; ++i)
    {
        mean += data[i];
    }
    mean = mean / n;
    for (i = 0; i < n; ++i)
        sum_deviation += (data[i] - mean) * (data[i] - mean);
    return sqrt(sum_deviation / n);
}





BARTLBY_BASELINE * bartlby_check_baseline(struct service * svc, char * config_payload, char * config, void * bartlby_address) {
    //return NULL if failed
    json_object * json_config = json_tokener_parse(config_payload);


    /*
        {
            "alg": "standard_deviation",
            "time_tolerance": 120,
            "value_tolerance": 200
            "days_back": 7,
            "data_source": "history",
            "min_records": 20,
            "max_records": 100,
            "include_keys": ["load","test_value"]
        }
    */

    if(json_config ==  NULL) {
        return NULL;
    }

   

    int alg = BARTLBY_STATISTIC_STD_DEVIATION;
    int time_tolerance = BARTLBY_BASELINE_TIME_TOLERANCE;
    int value_tolerance = BARTLBY_BASELINE_VALUE_TOLERANCE;
    int days_back = BARTLBY_BASELINE_DAYS_BACK;
    int data_source= BARTLBY_BARTLBY_BASELINE_DS_HISTORY;
    int min_records=20;
    int max_records=100;
    int include_keys=-1;


    BARTLBY_BASELINE  * bsl;

    json_object * jso_alg, * jso_time_tolerance;
    json_object * jso_value_tolerance, * jso_days_back;
    json_object * jso_data_source, *jso_min_records, *jso_max_records;
    json_object * jso_include_keys  = NULL;


    //ALGO
    if (json_object_object_get_ex(json_config, "alg", &jso_alg)) {
            if(strcmp(json_object_get_string(jso_alg), "standard_deviation") == 0) {
                alg = BARTLBY_STATISTIC_STD_DEVIATION;
            }
            if(strcmp(json_object_get_string(jso_alg), "exponenatial_smooth") == 0) {
                alg = BARTLBY_STATISTIC_EXP_SMOOTH;
            }
    }


    if (json_object_object_get_ex(json_config, "time_tolerance", &jso_time_tolerance)) {
        if(json_object_get_int64(jso_time_tolerance) > 0) {
            time_tolerance=json_object_get_int64(jso_time_tolerance);
        }
    }

    if (json_object_object_get_ex(json_config, "value_tolerance", &jso_value_tolerance)) {
        if(json_object_get_int64(jso_value_tolerance) > 0) {
            value_tolerance=json_object_get_int64(jso_value_tolerance);
        }
    }

    if (json_object_object_get_ex(json_config, "days_back", &jso_days_back)) {
        if(json_object_get_int64(jso_days_back) > 0) {
            days_back=json_object_get_int64(jso_days_back);
        }
    }

   //DATA SOURCE
    if (json_object_object_get_ex(json_config, "data_source", &jso_data_source)) {
            if(strcmp(json_object_get_string(jso_data_source), "history") == 0) {
                data_source = BARTLBY_BARTLBY_BASELINE_DS_HISTORY;
            }
            
    }

    if (json_object_object_get_ex(json_config, "min_records", &jso_min_records)) {
        if(json_object_get_int64(jso_min_records) > 0) {
            min_records=json_object_get_int64(jso_min_records);
        }
    }

    if (json_object_object_get_ex(json_config, "max_records", &jso_max_records)) {
        if(json_object_get_int64(jso_max_records) > 0) {
            max_records=json_object_get_int64(jso_max_records);
        }
    }


    if (json_object_object_get_ex(json_config, "include_keys", &jso_include_keys)) {
       include_keys=1;
    }
    if(include_keys != 1) jso_include_keys=NULL;

    bsl = malloc(sizeof(BARTLBY_BASELINE)*1);
    bartlby_calculate_baseline(svc->service_id,
                       bsl,
                       days_back,
                       time_tolerance,
                       value_tolerance,
                       svc->current_output,
                       config,
                       data_source,
                       alg,
                       min_records,
                       max_records,
                       jso_include_keys
                       );




    return bsl;
}


json_object * bartlby_baseline_parse_perf_data(char * string) {

    char * work_str;
    char * repos_work_str;
    char * token;
    
    regex_t regex;
    int ret;
    int length;

    char * value_string;
    char * key_string;


    regmatch_t pmatch[5];


    work_str = strdup(string);
    repos_work_str = strstr(work_str, "|");

    if (repos_work_str == NULL) {
    	free(work_str);
        return NULL;
    }

    repos_work_str++; //Advance the |


    json_object * jso_ret;

    jso_ret = json_object_new_object();


    while ((token = strsep (&repos_work_str, " ")) != NULL) {
        if (strlen(token) > 3) {

            ret = regcomp(&regex, "([^=]+)=([0-9.]+)", REG_EXTENDED);
            if (ret != 0) {
                printf("Unable to compile regular expression\n");
                free(work_str);
                return NULL;
            }
            if (regexec(&regex, token, 3, pmatch, 0) == 0) {

                 if (pmatch[1].rm_so == (size_t)-1) continue;
            	 if (pmatch[2].rm_so == (size_t)-1) continue;


                length = pmatch[1].rm_eo - pmatch[1].rm_so;
                key_string = malloc((length + 1) * sizeof(char));
                memcpy(key_string, token + pmatch[1].rm_so, length);
                key_string[length] = 0;


                length = pmatch[2].rm_eo - pmatch[2].rm_so;
                value_string = malloc((length + 1) * sizeof(char));
                memcpy(value_string, token + pmatch[2].rm_so, length);
                value_string[length] = 0;



                json_object_object_add(jso_ret, key_string, json_object_new_double(atof(value_string)));





                free(key_string);
                free(value_string);

            }
            regfree(&regex);


        }

    }
    free(work_str);

    
    return jso_ret;

}

BARTLBY_BASELINE * bartlby_calculate_baseline(long svc_id,
                              BARTLBY_BASELINE * result_baseline,
                              int days_back,
                              int time_tolerance,
                              int value_tolerance,
                              char * service_output,
                              char * cfg,
                              int source_type,
                              int statistic_algo,
                              int min_records,
                              int max_records,
                              json_object * include_keys

                             ) {

    json_object * perf_data;
    json_object * last_records;

    time_t current_midnight;
    time_t work_on;
    long tolerance_window_end;
    long tolerance_window_start;

    
    json_object * return_object;

    int baseline_broken = 0;
    json_object * baseline_broken_keys;

    
    int i;

    long record_limiter=0;



    baseline_broken_keys = json_object_new_array();
    return_object = json_object_new_object();
    perf_data = bartlby_baseline_parse_perf_data(service_output);

    if (perf_data != NULL) {


        //GET RECORDS

        current_midnight = time(NULL);
        last_records = json_object_new_object();

        for (i = days_back; i > 0; i--) {

            if(record_limiter >= max_records) continue;

            //Walk threw days
            work_on = current_midnight - (86400 * i);
            tolerance_window_start = work_on - time_tolerance;
            tolerance_window_end = work_on + time_tolerance;

            

            record_limiter += bartlby_baseline_append_day_data(
                svc_id,
                work_on,
                tolerance_window_start,
                tolerance_window_end,
                last_records,
                cfg,
                source_type
            );





        }

        
        json_object_iter iter;
        json_object_object_foreachC(last_records, iter)
        {

            if(record_limiter <= min_records) continue;
            
            //Check Baseline of value

            float * baseline_values;
            int baseline_value_count;
            //int x;
            
            char * key0=iter.key;
            json_object * val0=iter.val;

            float tolerance;

            deviation_alg deviation;

            

            if(!bartlby_json_string_in_array(include_keys, key0)) {
                continue;
            } 
            
            json_object * curr_live_val;

            baseline_value_count =  json_object_array_length(val0);
            baseline_values = malloc(sizeof(float) * baseline_value_count);
            tolerance = value_tolerance / 100 + 1;


            
            
            
	        	

            bartlby_baseline_deviation(baseline_values, baseline_value_count,tolerance, val0, &deviation, statistic_algo);


            
            if(json_object_object_get_ex(perf_data, key0, &curr_live_val)) {

	            json_object * baseline_new;
	            baseline_new = json_object_new_object();
	            json_object_object_add(baseline_new, deviation.alg_name, json_object_new_double(deviation.val));
	            json_object_object_add(baseline_new, "avg", json_object_new_double(deviation.avg));
	            json_object_object_add(baseline_new, "top", json_object_new_double(deviation.top));
	            json_object_object_add(baseline_new, "bottom", json_object_new_double(deviation.bottom));
	            json_object_object_add(baseline_new, "current", json_object_new_double(json_object_get_double(curr_live_val)));
	            json_object_object_add(baseline_new, "raw_values", json_object_get(val0));


	            if (json_object_get_double(curr_live_val) < deviation.bottom || json_object_get_double(curr_live_val) > deviation.top ) {
	                baseline_broken = 1;
	                
	                json_object_array_add(baseline_broken_keys, json_object_new_string(key0));
	            }




	            json_object_object_add(return_object, key0, baseline_new);
	        } else {
	        	//Key not found
	        	json_object_array_add(baseline_broken_keys, json_object_new_string(key0));
	        	baseline_broken=1;
	        	
	        }
            free(baseline_values);

            //

        }
        json_object_object_add(return_object, "baseline_broken", json_object_new_boolean(baseline_broken));
        json_object_object_add(return_object, "baseline_broken_keys", baseline_broken_keys);


        


        result_baseline->json_result = return_object;
        result_baseline->baseline_broken = baseline_broken;

        
        //cleanup
        json_object_put(last_records);
        
        //initial perf data
        json_object_put(perf_data);

        return result_baseline;


    }
    return NULL;
}



void bartlby_baseline_deviation(float  baseline_values[], int baseline_value_count, float tolerance, json_object * data_point, deviation_alg * alg_out, int statistic_algo) {
    switch(statistic_algo) {
        case BARTLBY_STATISTIC_STD_DEVIATION:
            bartlby_baseline_deviation_std(baseline_values, baseline_value_count, tolerance, data_point, alg_out);
        break;
        case BARTLBY_STATISTIC_EXP_SMOOTH:
            bartlby_baseline_deviation_exp_smooth(baseline_values, baseline_value_count, tolerance, data_point,alg_out);
        break;

       

    }
}

void bartlby_baseline_deviation_std(float  baseline_values[], int baseline_value_count, float tolerance, json_object * data_point,deviation_alg * alg_out) {

            double sum;
            double avg;
            double top;
            double bottom;
            float std_deviation;
            int x;

            json_object * jso_temp;
            sum = 0;
            for (x = 0; x < baseline_value_count; x++) {
                if(json_object_object_get_ex(json_object_array_get_idx(data_point, x), "value", &jso_temp)) {
                	baseline_values[x] = json_object_get_double(jso_temp);

                	sum += baseline_values[x];

                	//json_object_put(jso_temp);
                }
            }
            avg = sum / baseline_value_count;

            std_deviation = bartlby_baseline_standard_deviation(baseline_values, baseline_value_count);
            
            top = avg + (std_deviation * tolerance);
            bottom = avg - (std_deviation * tolerance);
            if (bottom < 0) {
                bottom = 0;
            }


            alg_out->bottom=bottom;
            alg_out->top=top;
            alg_out->val=std_deviation;
            alg_out->sum=sum;
            alg_out->avg=avg;
            sprintf(alg_out->alg_name, "standard_deviation");

            //json_object_put(jso_temp);

            


}


long bartlby_baseline_append_day_data(long svc_id, 
                                 time_t work_on,
                                 int tolerance_window_start,
                                 int tolerance_window_end,
                                 json_object * last_records,
                                 char * cfg,
                                 int source_type) {



    switch(source_type) {
        case BARTLBY_BARTLBY_BASELINE_DS_HISTORY:
            return bartlby_baseline_append_day_data_from_stathistory(svc_id,
                                                              work_on,
                                                              tolerance_window_start,
                                                              tolerance_window_end,
                                                              last_records,
                                                              cfg
                                                              );
        break;

        default:
            _debug("UNABLE to find baseline datasource");
        break;

    }
    return 0;
}

long bartlby_baseline_append_day_data_from_stathistory(long svc_id, 
                                 time_t work_on,
                                 int tolerance_window_start,
                                 int tolerance_window_end,
                                 json_object * last_records,
                                 char * cfg
                                 ) {


    long records_found = 0;
    struct tm * tm_info;
    char work_on_file[1024];

    char * file_contents;
    char * file_contents_dup;
    //char * file_contents_record;
    json_object * file_contents_record_jso;
    json_object * current_record_jso;
    char * record_token;

    json_object * data_point;

    char * base_history_path= getConfigValue("statehistory_logdir", cfg); 

    if(base_history_path == NULL) {
    	base_history_path=strdup(BASE_HISTORY_PATH);
    }



    //asprintf(&file_path, "%s/%02d/%02d/%02d/%ld-%02d-%02d-%02d.history", cfg_statehistory_dir, tmnow->tm_year + 1900,tmnow->tm_mon + 1,tmnow->tm_mday, svc->service_id,tmnow->tm_year + 1900,tmnow->tm_mon + 1,tmnow->tm_mday);

    tm_info = localtime ( &work_on );


    sprintf(work_on_file, "%s/%02d/%02d/%02d/%ld-%02d-%02d-%02d.history", base_history_path, tm_info->tm_year + 1900,tm_info->tm_mon + 1,tm_info->tm_mday, svc_id,tm_info->tm_year + 1900,tm_info->tm_mon + 1,tm_info->tm_mday);

    
    
    



    file_contents = file2str(work_on_file);
    file_contents_dup = file_contents;

    if(file_contents == NULL) {
        free(base_history_path);
    	return 0;
    }

    while ((record_token = strsep (&file_contents_dup, "#")) != NULL) {
        if (strlen(record_token) > 4) {
            file_contents_record_jso = json_tokener_parse(record_token);


            json_object * record_time;
            json_object * record_output;
            json_object * has_seen_key;

            if (json_object_object_get_ex(file_contents_record_jso, "last_write", &record_time)) {
                if(json_object_object_get_ex(file_contents_record_jso, "output", &record_output)) {



	                if (json_object_get_int64(record_time) >= tolerance_window_start && json_object_get_int64(record_time)  <= tolerance_window_end) {
	                    records_found++;
	                    current_record_jso = bartlby_baseline_parse_perf_data((char*)json_object_get_string(record_output));

	                    json_object_iter iter;
	                    json_object_object_foreachC(current_record_jso, iter)
	                    {

	                        char * key0=iter.key;
	                        json_object * val0=iter.val;

	                        data_point = json_object_new_object();
	                        json_object_object_add(data_point, "time", json_object_new_int64(json_object_get_int64(record_time)));
	                        json_object_object_add(data_point, "value", json_object_new_double(json_object_get_double(val0)));


	                        if (json_object_object_get_ex(last_records, key0, &has_seen_key)) {
	                            //alread there
	                            json_object_array_add(has_seen_key, data_point);
	                        } else {
	                            json_object * new_array = json_object_new_array();
	                            json_object_array_add(new_array, data_point);
	                            json_object_object_add(last_records, key0, new_array);
	                        }

	                    }
	                    json_object_put(current_record_jso);




	                }
	            }

            }




            json_object_put(file_contents_record_jso);
        }
    }
    
    free(base_history_path);
    
    free(file_contents);


    return records_found;






}


void bartlby_baseline_deviation_exp_smooth(float  baseline_values[], int baseline_value_count, float tolerance, json_object * data_point, deviation_alg * alg_out) {

            double sum;
            double avg;
            double top;
            double bottom;
            //float std_deviation;
            int x;



            float smoothed=0;
            float aux;

            json_object * jso_temp;
            sum=0;
            for (x = 0; x < baseline_value_count; x++) {
                if(json_object_object_get_ex(json_object_array_get_idx(data_point, x), "value", &jso_temp)) {
                	aux = json_object_get_double(jso_temp);
                	if(x>0) {
                    	smoothed=smoothed+0.3*(aux - smoothed); //0.3 EXPONENTIALALPHA
	                } else {
	                    smoothed=aux;
	                }
	                sum += aux;
            	}
            }
            avg = sum / baseline_value_count;

            
            top = smoothed * 0.3;
            bottom = smoothed * ( 2 - 0.3);

            if(bottom < 0) bottom=0;

            alg_out->bottom=top;
            alg_out->top=bottom;
            alg_out->val=smoothed;
            alg_out->sum=sum;
            alg_out->avg=avg;
            sprintf(alg_out->alg_name, "exponenatial_smooth");

}
