#ifndef CONFIG
#define CONFIG "/opt/bartlby/etc/bartlby.cfg"
#endif

#define TEST_ORCH_ID 999

#define LOAD_SYMBOL_TEST(x,y,z) 	x=dlsym(y, z); \
    	if((dlmsg=dlerror()) != NULL) { \
        	tt_abort_printf(("dl error: %s\n", dlmsg));	\
			return;	\
    	}


void * bartlby_get_shm(char * cfgfile);
void * bartlby_get_sohandle(char * cfgfile);    	
