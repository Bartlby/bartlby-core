#include "bartlby.h"
#include "tinytest.h"
#include "tinytest_macros.h"

#include "bartlby_test.h"

#define BASE_HISTORY_PATH "./tmp/bartlby_history/"


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
    

    json_object * jso;
    char * log_path_copy;
    char * log_path_dirname;

    FILE * fp;

    int records_created = 0;

    current_midnight = time(NULL);


    char * base_history_path=BASE_HISTORY_PATH; 


    for (i = days_back; i > 0; i--) {
        work_on = current_midnight - (86400 * i);
        tm_info = localtime ( &work_on );
        

        
        sprintf(work_on_file, "%s/%02d/%02d/%02d/%ld-%02d-%02d-%02d.history", base_history_path, tm_info->tm_year + 1900,tm_info->tm_mon + 1,tm_info->tm_mday, svc_id,tm_info->tm_year + 1900,tm_info->tm_mon + 1,tm_info->tm_mday);
        
        log_path_copy=strdup(work_on_file);
		log_path_dirname=dirname(log_path_copy);

		mkdir_recursive(log_path_dirname, 0777);

		fprintf(stderr, "%s - %s", strerror(errno), work_on_file);

		free(log_path_copy);




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

void test_baseline(void *data) {
	(void)data;
	int rtc;


	BARTLBY_BASELINE * bsl;

    float  data_points[] = {10, 20, 30, 30, 40, 30, 20, 10};


    
    bartlby_baseline_create_test_data(8316,
                              CONFIG,
                              data_points,
                              8,
                              "some plugin output | mem_usage=44.99;; mem_overhead=41.57MB;; mem_active=1843.20MB;; mem_swap=0.00MB;; mem_swapin=0.00MB;; mem_swapout=0.00MB;; mem_memctl=0.00MB;; test_value=%fMB;;",
                              7);


    struct service svc;
    svc.service_id=8316;
    sprintf(svc.current_output, "some plugin output | mem_usage=44.99;; mem_overhead=41.57MB;; mem_active=1843.20MB;; mem_swap=0.00MB;; mem_swapin=0.00MB;; mem_swapout=0.00MB;; mem_memctl=0.00MB;; test_value=20.0MB;;");
    
    
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
    TT_DECLARE("INFO", ("Setting BASE_HISTORY_PATH env var "));
   	setenv("BARTLBY_statehistory_logdir", BASE_HISTORY_PATH, 1); 


   	TT_DECLARE("INFO", ("Checking good baseline "));
    bsl=bartlby_check_baseline(&svc, jso_config_package, CONFIG, NULL);
    tt_int_op(bsl->baseline_broken, ==, 0);


    sprintf(svc.current_output, "some plugin output | mem_usage=44.99;; mem_overhead=41.57MB;; mem_active=1843.20MB;; mem_swap=0.00MB;; mem_swapin=0.00MB;; mem_swapout=0.00MB;; mem_memctl=0.00MB;; test_value1=20.0MB;;");
    TT_DECLARE("INFO", ("Checking broken baseline "));
    bsl=bartlby_check_baseline(&svc, jso_config_package, CONFIG, NULL);
    tt_int_op(bsl->baseline_broken, ==, 1);


	end:

	;
}

struct testcase_t baseline_tests[] = {
	{ "test_baseline", test_baseline, },
	END_OF_TESTCASES
};