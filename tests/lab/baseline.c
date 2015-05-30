/*

smoothed = 0;
        while(*(last_values + i) != -1 && i < max_entries) {

            aux = *(last_values + i);

            if (i > 0) {
                smoothed = smoothed + EXPONENTIALALPHA * (aux - smoothed);
            } else {
                smoothed = aux;
            }

            i++;
        }

        dv->top = smoothed * tolerance;
        dv->bottom = smoothed * ( 2 - tolerance);
#define EXPONENTIALALPHA 0.3
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <json.h>
#include <string.h>
#include <math.h>
#include <regex.h>
#include <time.h>
#include <stdlib.h>



//SOURCE TYPE 

#define BARTLBY_BASELINE_DS_HISTORY 1
#define BARTLBY_BASELINE_DS_DB 2


// statistic algos
#define BARTLBY_STATISTIC_STD_DEVIATION 1
#define BARTLBY_STATISTIC_EXP_SMOOTH 2        



//seconds for the time window to look at
#define TIME_TOLERANCE 30

//tolerance for the value in percentage of the standard deviation
#define VALUE_TOLERANCE 400

//Days to lookback for baseline calculation
#define DAYS_BACK 7

#define BASE_HISTORY_PATH "/tmp/bartlby_test/"


#define CHECKED_ASPRINTF(...)                                       \
    if (asprintf( __VA_ARGS__ ) == -1) {                             \
       fprintf(stderr, "ASPRINTF FAILED");\
       exit(1); \
    }

typedef struct {
    json_object * json_result;
    int baseline_broken;
} BASELINE;


typedef struct {
    float top;
    float bottom;
    float val;
    float avg;
    float sum;
    char alg_name[80];

} deviation_alg;


json_object * parse_perf_data(char * string);
char *file2str(    const char *path,    unsigned long *file_len_out);
float standard_deviation(float data[],  int n);

void baseline_destroy(BASELINE * bsl) {

    json_object_put(bsl->json_result);


}

void baseline_create_test_data(long svc_id,
                               char * cfg,
                               float data_points[],
                               int data_points_len,
                               char * output_format,
                               int days_back) {
    time_t current_midnight;
    time_t work_on;
    int i;
    int x;

    char work_on_file[1024];
    struct tm * tm_info;
    char * record_buffer;
    char time_buffer[80];

    json_object * jso;

    FILE * fp;

    int records_created = 0;

    current_midnight = time(NULL);


    char * base_history_path=BASE_HISTORY_PATH; //FIXME getconfig


    for (i = days_back; i > 0; i--) {
        work_on = current_midnight - (86400 * i);
        tm_info = localtime ( &work_on );
        strftime( time_buffer, 80, "%Y.%m.%d", tm_info );

        //FIXME HISTORY FILE PATH!!!! - should be variable
        sprintf(work_on_file, "%s%d-%s.history", base_history_path, svc_id, time_buffer);
        fp = fopen(work_on_file, "w");




        for (x = 0; x < data_points_len; x++) {
            //fprintf(stderr, "ADD day: %d - point %d\n", i, x);


            CHECKED_ASPRINTF(&record_buffer, output_format, data_points[x])

            jso = json_object_new_object();
            json_object_object_add(jso, "current_state", json_object_new_int(1));
            json_object_object_add(jso, "last_write", json_object_new_int(work_on + x));
            json_object_object_add(jso, "output", json_object_new_string(record_buffer));

            fprintf(fp, "%s\n#############REC##############\n", json_object_to_json_string(jso));

            json_object_put(jso);
            free(record_buffer);
            records_created++;
        }


        fclose(fp);


    }
    fprintf(stderr, "TEST Records created: %d\n", records_created);
}




void bartlby_baseline_append_day_data_from_stathistory(long svc_id, 
                                 time_t work_on,
                                 int tolerance_window_start,
                                 int tolerance_window_end,
                                 json_object * last_records,
                                 char * cfg
                                 ) {



    struct tm * tm_info;
    char time_buffer[80];
    char work_on_file[1024];

    char * file_contents;
    char * file_contents_dup;
    char * file_contents_record;
    json_object * file_contents_record_jso;
    json_object * current_record_jso;
    char * record_token;

    json_object * data_point;

    char * base_history_path=BASE_HISTORY_PATH; //fixme getconfig





    tm_info = localtime ( &work_on );
    strftime( time_buffer, 80, "%Y.%m.%d", tm_info );

    //FIXME HISTORY FILE PATH!!!! - should be variable
    sprintf(work_on_file, "%s%d-%s.history", base_history_path, svc_id, time_buffer);
    //fprintf(stderr, "%s\n", work_on_file);



    file_contents = file2str(work_on_file, NULL);
    file_contents_dup = file_contents;



    while ((record_token = strsep (&file_contents_dup, "#")) != NULL) {
        if (strlen(record_token) > 4) {
            file_contents_record_jso = json_tokener_parse(record_token);


            json_object * record_time;
            json_object * record_output;
            json_object * has_seen_key;

            if (json_object_object_get_ex(file_contents_record_jso, "last_write", &record_time)) {
                json_object_object_get_ex(file_contents_record_jso, "output", &record_output);



                if (json_object_get_int64(record_time) >= tolerance_window_start && json_object_get_int64(record_time)  <= tolerance_window_end) {
                    current_record_jso = parse_perf_data((char*)json_object_get_string(record_output));


                    json_object_object_foreach(current_record_jso, key0, val0)
                    {

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




            json_object_put(file_contents_record_jso);
        }
    }


    free(file_contents);









}

void bartlby_baseline_append_day_data(long svc_id, 
                                 time_t work_on,
                                 int tolerance_window_start,
                                 int tolerance_window_end,
                                 json_object * last_records,
                                 char * cfg,
                                 int source_type) {



    switch(source_type) {
        case BARTLBY_BASELINE_DS_HISTORY:
            bartlby_baseline_append_day_data_from_stathistory(svc_id,
                                                              work_on,
                                                              tolerance_window_start,
                                                              tolerance_window_end,
                                                              last_records,
                                                              cfg
                                                              );
        break;

        default:
            //FIXME unable to find data source
        break;

    }

}

void bartlby_baseline_deviation_exp_smooth(float  baseline_values[], int baseline_value_count, float tolerance, json_object * data_point, deviation_alg * alg_out) {

            double sum;
            double avg;
            double top;
            double bottom;
            float std_deviation;
            int x;



            float smoothed=0;
            float aux;
            sum=0;
            for (x = 0; x < baseline_value_count; x++) {
                aux = json_object_get_double(json_object_object_get(json_object_array_get_idx(data_point, x), "value"));
                if(x>0) {
                    smoothed=smoothed+0.3*(aux - smoothed); //0.3 EXPONENTIALALPHA
                } else {
                    smoothed=aux;
                }
                sum += aux;
            }
            avg = sum / baseline_value_count;

            //fprintf(stderr, "SMOOTHED: %10f\n", smoothed);
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

void bartlby_baseline_deviation_std(float  baseline_values[], int baseline_value_count, float tolerance, json_object * data_point,deviation_alg * alg_out) {

            double sum;
            double avg;
            double top;
            double bottom;
            float std_deviation;
            int x;

            sum = 0;
            for (x = 0; x < baseline_value_count; x++) {
                baseline_values[x] = json_object_get_double(json_object_object_get(json_object_array_get_idx(data_point, x), "value"));

                //fprintf(stderr, "KEY: %s VAL:%10f\n", key0, baseline_values[x]);
                sum += baseline_values[x];
            }
            avg = sum / baseline_value_count;

            std_deviation = standard_deviation(baseline_values, baseline_value_count);
            
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

BASELINE * calculate_baseline(long svc_id,
                              BASELINE * result_baseline,
                              int days_back,
                              int time_tolerance,
                              int value_tolerance,
                              char * service_output,
                              char * cfg,
                              int source_type,
                              int statistic_algo
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



    baseline_broken_keys = json_object_new_array();
    return_object = json_object_new_object();
    //FIXME GET SERVICE CURRENT OUTPUT
    perf_data = parse_perf_data(service_output);

    if (perf_data != NULL) {


        //GET RECORDS

        current_midnight = time(NULL);
        last_records = json_object_new_object();

        for (i = days_back; i > 0; i--) {
            //Walk threw days
            work_on = current_midnight - (86400 * i);
            tolerance_window_start = work_on - time_tolerance;
            tolerance_window_end = work_on + time_tolerance;

            bartlby_baseline_append_day_data(
                svc_id,
                work_on,
                tolerance_window_start,
                tolerance_window_end,
                last_records,
                cfg,
                source_type
            );




        }



        json_object_object_foreach(last_records, key0, val0)
        {
            //Check Baseline of value

            float * baseline_values;
            int baseline_value_count;
            int x;
            
            float tolerance;

            deviation_alg deviation;

            json_object * curr_live_val;

            baseline_value_count =  json_object_array_length(val0);
            baseline_values = malloc(sizeof(float) * baseline_value_count);
            tolerance = value_tolerance / 100 + 1;

            bartlby_baseline_deviation(baseline_values, baseline_value_count,tolerance, val0, &deviation, statistic_algo);


            
            json_object_object_get_ex(perf_data, key0, &curr_live_val);



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
                //fprintf(stderr, "BROKEN for key: %s\n", key0);
                json_object_array_add(baseline_broken_keys, json_object_new_string(key0));
            }




            json_object_object_add(return_object, key0, baseline_new);

            free(baseline_values);



        }
        json_object_object_add(return_object, "baseline_broken", json_object_new_boolean(baseline_broken));
        json_object_object_add(return_object, "baseline_broken_keys", baseline_broken_keys);


        result_baseline->json_result = return_object;
        result_baseline->baseline_broken = baseline_broken;


        //Cleanup return object - FIXME remove this later
        json_object_put(last_records);
        //json_object_put(return_object);

        //initial perf data
        json_object_put(perf_data);

        return result_baseline;


    }

}

json_object * parse_perf_data(char * string) {

    char * work_str;
    char * repos_work_str;
    char * token;
    char * key;
    regex_t regex;
    int ret;
    int length;

    char * value_string;
    char * key_string;


    regmatch_t pmatch[5];


    work_str = strdup(string);
    repos_work_str = strstr(work_str, "|");

    if (repos_work_str == NULL) {
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
                return NULL;
            }
            if (regexec(&regex, token, 5, pmatch, 0) == 0) {

                //FIXME CHECK IF we have atlease 2 groups found - else return null, or skip var

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

    //fprintf(stderr, "JSON %s", json_object_to_json_string(jso_ret));
    return jso_ret;

}

float standard_deviation(float data[], int n)
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



int main(int argc, char ** argv) {
    BASELINE bsl;

    float  data_points[] = {10, 20, 30, 40, 40, 30, 20, 10};

    baseline_create_test_data(8316,
                              "/opt/bartlby/etc/bartlby.cfg",
                              data_points,
                              8,
                              "some plugin output | mem_usage=44.99%;; mem_overhead=41.57MB;; mem_active=1843.20MB;; mem_swap=0.00MB;; mem_swapin=0.00MB;; mem_swapout=0.00MB;; mem_memctl=0.00MB;; test_value=%fMB;;",
                              7);

    calculate_baseline(8316,
                       &bsl,
                       DAYS_BACK,
                       TIME_TOLERANCE,
                       VALUE_TOLERANCE,
                       "some plugin output | mem_usage=44.99%;; mem_overhead=41.57MB;; mem_active=1843.20MB;; mem_swap=0.00MB;; mem_swapin=0.00MB;; mem_swapout=0.00MB;; mem_memctl=0.00MB;; test_value=10.0MB;;",
                       "/opt/bartlby/etc/bartlby.cfg",
                       BARTLBY_BASELINE_DS_HISTORY,
                       BARTLBY_STATISTIC_EXP_SMOOTH

                       );



    printf("\t%s\n", json_object_to_json_string_ext(bsl.json_result, JSON_C_TO_STRING_PRETTY));
    if (bsl.baseline_broken == 0) {
        fprintf(stderr, "BASELINE not broken\n");
    } else {
        fprintf(stderr, "BASELINE broken\n");
    }
    baseline_destroy(&bsl);

}



//file2str clib
char *file2strl(
    const char *path,
    unsigned int *file_len_out
)
{
    FILE *file;

    if (!(file = fopen(path, "rb")))
    {
        fprintf(stderr, "Unable to open file %s\n", path);
        return NULL;
    }

    if (-1 == fseek(file, 0, SEEK_END))
    {
        fprintf(stderr, "Unable to seek file %s\n", path);
        return NULL;
    }

    unsigned long file_len;
    if (-1 == (file_len = ftell(file)))
    {
        fprintf(stderr, "Unable to ftell() file %s\n", path);
        return NULL;
    }

    if (-1 == fseek(file, 0, SEEK_SET))
    {
        fprintf(stderr, "Unable to seek file %s\n", path);
        return NULL;
    }

    char *contents;
    if (!(contents = malloc(file_len + 1)))
    {
        fprintf(stderr, "Memory error!\n");
        fclose(file);
        return NULL;
    }

    fread(contents, file_len, 1, file);
    fclose(file);

    contents[file_len] = '\0';

    if (file_len_out)
        *file_len_out = file_len + 1;

    return contents;
}

char *file2str(
    const char *path,
    unsigned long *file_len_out
)
{
    return file2strl(path, NULL);
}
