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
#include "bartlby.h"



//SOURCE TYPE 

#define BARTLBY_BARTLBY_BASELINE_DS_HISTORY 1
#define BARTLBY_BARTLBY_BASELINE_DS_DB 2


// statistic algos
#define BARTLBY_STATISTIC_STD_DEVIATION 1
#define BARTLBY_STATISTIC_EXP_SMOOTH 2        



//seconds for the time window to look at
#define TIME_TOLERANCE 30

//tolerance for the value in percentage of the standard deviation
#define VALUE_TOLERANCE 400

//Days to lookback for baseline calculation
#define DAYS_BACK 7


//Min Records before a baseline calculation is done
#define MIN_RECORDS 10

//MAX records to consider in baseline calculation
#define MAX_RECORDS 100        

#define BASE_HISTORY_PATH "/opt/bartlby/var/log/history/"










void bartlby_baseline_create_test_data(long svc_id,
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


    char * base_history_path=BASE_HISTORY_PATH; 


    for (i = days_back; i > 0; i--) {
        work_on = current_midnight - (86400 * i);
        tm_info = localtime ( &work_on );
        strftime( time_buffer, 80, "%Y.%m.%d", tm_info );

        
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



int main(int argc, char ** argv) {
    BARTLBY_BASELINE * bsl;

    float  data_points[] = {10, 20, 30, 30, 40, 30, 20, 10};


    
    bartlby_baseline_create_test_data(8316,
                              "/opt/bartlby/etc/bartlby.cfg",
                              data_points,
                              8,
                              "some plugin output | mem_usage=44.99%;; mem_overhead=41.57MB;; mem_active=1843.20MB;; mem_swap=0.00MB;; mem_swapin=0.00MB;; mem_swapout=0.00MB;; mem_memctl=0.00MB;; test_value=%fMB;;",
                              7);


    struct service svc;
    svc.service_id=8316;
    sprintf(svc.current_output, "some plugin output | mem_usage=44.99%;; mem_overhead=41.57MB;; mem_active=1843.20MB;; mem_swap=0.00MB;; mem_swapin=0.00MB;; mem_swapout=0.00MB;; mem_memctl=0.00MB;; test_value1=20.0MB;;");
    
    
    json_object * cfg_obj;




    char * jso_config_package = " \
        { \
            \"alg\": \"standard_deviation\", \
            \"time_tolerance\": 120, \
            \"value_tolerance\": 200, \
            \"days_back\": 7, \
            \"data_source\": \"history\", \
            \"min_records\": 20, \
            \"max_records\": 100, \
            \"include_keys\": [\"test_value\"] \
        } \
        ";

    
    bsl=bartlby_check_baseline(&svc, jso_config_package, "/opt/bartlby/etc/bartlby.cfg", NULL);
    

    
    printf("\t%s\n", json_object_to_json_string_ext(bsl->json_result, JSON_C_TO_STRING_PRETTY));
    if (bsl->baseline_broken == 0) {
        fprintf(stderr, "BARTLBY_BASELINE not broken\n");
    } else {
        fprintf(stderr, "BARTLBY_BASELINE broken\n");
    }
    bartlby_baseline_destroy(bsl);

}


