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

char config_file[255];



void set_cfg(char * cfg) {
	struct stat cStat;
	
	if(stat(cfg, &cStat) < 0) {
		fprintf(stderr, "Configfile '%s' does not exist\n", cfg);
		exit(1);	
	}
	sprintf(config_file, "%s", cfg);	
}


int service_is_in_time(const char * time_plan) {
	char * tmp;	
	char * tmp1;
	char * token, *token1;
	int rt;
	char idx[3];
	char * ttok;
	time_t tnow;
	struct tm *tmnow;
	struct tm fromcheck, tocheck;       
	int fromts, tots;
	char in_or_out;
	
	int min,hour, min1,hour1;
	int cur_ts;
	
	
	rt = -1;
	
	if(strlen(time_plan) < 5) {
		return 1; //if no plan?! always check ;)
	}
	
	
	
	cur_ts = time(&tnow);
	tmnow = localtime(&tnow);
	
	tmp = strdup(time_plan);
	token = strtok(tmp, "|");
	while(token != NULL) {
		sprintf(idx, "%c", *token);
		if(atoi(idx) == tmnow->tm_wday) {
			ttok = token+1;
			in_or_out = *ttok;
			if(in_or_out == '!') {
				rt=1;
			} else {
				rt=-1;	
			}
				
			ttok = token+2;
			//now check threw the timeranges
			tmp1 = strdup(ttok);
			token1=strtok(tmp1, ",");
			while(token1 != NULL) {
				if(sscanf(token1, "%d:%d-%d:%d", &hour,&min, &hour1, &min1) == 4) {
					
					fromcheck.tm_sec = 0;
					fromcheck.tm_min = min;
					fromcheck.tm_hour = hour;
					fromcheck.tm_mday = tmnow->tm_mday;
					fromcheck.tm_mon = tmnow->tm_mon;
					fromcheck.tm_year = tmnow->tm_year;
					fromcheck.tm_wday = tmnow->tm_wday;
					fromts = mktime(&fromcheck);
					
					tocheck.tm_sec = 0;
					tocheck.tm_min = min1;
					tocheck.tm_hour = hour1;
					tocheck.tm_mday = tmnow->tm_mday;
					tocheck.tm_mon = tmnow->tm_mon;
					tocheck.tm_year = tmnow->tm_year;
					tocheck.tm_wday = tmnow->tm_wday;
					tots = mktime(&tocheck);
					
					
					if(cur_ts >= fromts && cur_ts <= tots) {
						if(in_or_out == '=') {
							rt = 1;
						} else {
							
							rt = -1;
						}
	
					}
					
					
						
				
				
				}
				
				
				
				token1 = strtok(NULL, ",");	
			}
			
			free(tmp1);
			
			
		}
		/*
		if(rt > 0) {
			//Someone found;)
			break;	
		}
		*/
		
		token = strtok(NULL, "|");
		
	}
	
	free(tmp);	
	
	
	return rt;	
}


char * bartlby_beauty_state(int status) {
	char * ret;
	switch(status) {
		case STATE_OK:
			ret=strdup("OK");
		break;
		case STATE_WARNING:
			ret=strdup("WARNING");
			
		break;
		case STATE_CRITICAL:
			ret=strdup("CRITICAL");
		break;
		case STATE_SIRENE:
			ret=strdup("SIRENE");
		break;
		
		default:
			ret=strdup("UNKOWN");	
	}
	return ret;			
}

void bartlby_decode(char * msg, int length) {
	int x;
		
	for(x=0; x<length; x++) {
		//msg[x]=2^msg[x];	
		
	}
	
}
void str_mysql_safe(char * str) {
	int x;
	for(x=0; x<=strlen(str); x++) {
		if(str[x] == '\'')
			str[x]=' '; 
	}	
}
void service_mysql_safe(struct service * svc) {
	str_mysql_safe(svc->new_server_text);	
	str_mysql_safe(svc->service_name);
	//str_mysql_safe(svc->srv->server_name);
	//str_mysql_safe(svc->srv->client_ip);
	str_mysql_safe(svc->plugin);
	str_mysql_safe(svc->plugin_arguments);
	str_mysql_safe(svc->service_var);
	
	str_mysql_safe(svc->snmp_info.community);
	str_mysql_safe(svc->snmp_info.objid);
	
}

void bartlby_encode(char * msg, int length) {
	int x;
	for(x=0; x<length; x++) {
		//msg[x]=msg[x]^2;	
	}
	
}

int _log(const char * str,  ...) {
//	printf("LOG: %s\n", str);
	
	
	va_list argzeiger;
	time_t tnow;
	struct tm *tmnow;
	
	char * logfile;
	char * logfile_dd;
	FILE * fp;
	
	time(&tnow);
	tmnow = localtime(&tnow);
	
	if(strcmp(config_file, "") != 0) {
		logfile_dd=getConfigValue("logfile", config_file);
		if(logfile_dd == NULL) {
			printf("Logfile not set");
			exit(1);	
		}
		if(strcmp(logfile_dd, "/dev/stdout") != 0) {
			asprintf(&logfile, "%s.%02d.%02d.%02d", logfile_dd, tmnow->tm_year + 1900,tmnow->tm_mon + 1,tmnow->tm_mday); 	
		} else {
			logfile=strdup("/dev/stdout");	
		}
		
		
		
	} else {
		logfile_dd=strdup("-");	
		logfile=strdup("/dev/stdout");	
	}
	
	
	
 	va_start(argzeiger,str);
	if(strcmp(logfile, "/dev/stdout") == 0) {
		printf("%02d.%02d.%02d %02d:%02d:%02d;[%d];", tmnow->tm_mday,tmnow->tm_mon + 1,tmnow->tm_year + 1900, tmnow->tm_hour, tmnow->tm_min, tmnow->tm_sec, getpid());
		vprintf(str, argzeiger);
		printf(";\n");
	} else {
		fp=fopen(logfile, "a");   	
		if(fp == NULL) {
			perror(logfile);
			exit(1);	
		}
		fprintf(fp, "%02d.%02d.%02d %02d:%02d:%02d;[%d];", tmnow->tm_mday,tmnow->tm_mon + 1,tmnow->tm_year + 1900, tmnow->tm_hour, tmnow->tm_min, tmnow->tm_sec, getpid());
		vfprintf(fp, str, argzeiger);
		fprintf(fp, ";\n");
		//vprintf(string,argzeiger);
		//fflush(pos2_log_file);
		fclose(fp);
	}
   	
   	va_end(argzeiger);
   	free(logfile);
   	free(logfile_dd);
	return 1;   
}


void str_replace(char *str, const char *from, const char *to, int maxlen)
{
  char  *pstr   = str;
  int   fromlen = strlen(from);
  int   tolen   = strlen(to);
  
  while (*pstr != '\0' && pstr - str < maxlen) {
    if (strncmp(pstr, from, fromlen) != 0) {
      *pstr++;
      continue;
    }
    memmove(pstr + tolen, pstr + fromlen, maxlen - ((pstr + tolen) - str) - 1);
    memcpy(pstr, to, tolen);
    pstr += tolen;
  }
}

void bartlby_replace_svc_in_str(char * str, struct service * svc, int max) {
	char * human_state, * human_state_last;
	char * server_id, * service_id;
	
	
	
	
	
	asprintf(&server_id, "%ld", svc->srv->server_id);
	asprintf(&service_id, "%ld", svc->service_id);
	
	human_state=bartlby_beauty_state(svc->current_state);
	human_state_last=bartlby_beauty_state(svc->last_state);
	
	
	str_replace(str,"$READABLE_STATE$", human_state, max); 
	str_replace(str,"$READABLE_LAST$", human_state_last, max); 
	str_replace(str,"$PROGNAME$", PROGNAME, max); 
	str_replace(str,"$VERSION$", VERSION, max); 
	str_replace(str,"$SERVICE$", svc->service_name, max); 
	str_replace(str,"$SERVICE_ID$", service_id, max); 
	str_replace(str,"$SERVER_ID$", server_id, max); 
	str_replace(str,"$SERVICE_NAME$", svc->service_name, max); 
	str_replace(str,"$SERVICE_PLUGIN$", svc->plugin, max); 
  str_replace(str,"$SERVER$",  svc->srv->server_name, max);
  str_replace(str,"$SERVER_NAME$",  svc->srv->server_name, max);
  str_replace(str,"$MESSAGE$",  svc->new_server_text, max);
	
	setenv("SERVICE", svc->service_name, 1);
	setenv("PROGNAME", PROGNAME, 1);
	setenv("READABLE_STATE", human_state, 1);
	setenv("READABLE_LAST", human_state_last, 1);
	setenv("VERSION", VERSION, 1);
	setenv("MESSAGE",  svc->new_server_text, 1);
	setenv("SERVER_ID",  server_id, 1);
	setenv("SERVICE_ID", service_id, 1);
	setenv("SERVICE_NAME",  svc->service_name, 1);
	setenv("SERVICE_PLUGIN",  svc->plugin, 1);	
	setenv("SERVER",  svc->srv->server_name, 1);
	setenv("SERVER_NAME",  svc->srv->server_name, 1);
	
		
	free(server_id);
	free(service_id);
	free(human_state_last);
	free(human_state);
}


/* sends all data - thanks to Beej's Guide to Network Programming */
int bartlby_tcp_sendall(int s, char *buf, int *len){
	int total=0;
	int bytesleft=*len;
	int n=0;

	/* send all the data */
	while(total<*len){

		/* send some data */
		n=send(s,buf+total,bytesleft,0);

		/* break on error */
		if(n==-1)
			break;

		/* apply bytes we sent */
		total+=n;
		bytesleft-=n;
	        }

	/* return number of bytes actually send here */
	*len=total;

	/* return -1 on failure, 0 on success */
	return n==-1?-1:0;
        }


/* receives all data - modelled after sendall() */
int bartlby_tcp_recvall(int s, char *buf, int *len, int timeout){
	int total=0;
	int bytesleft=*len;
	int n=0;
	time_t start_time;
	time_t current_time;
	
	/* clear the receive buffer */
	bzero(buf,*len);

	time(&start_time);

	/* receive all data */
	while(total<*len){

		/* receive some data */
		n=recv(s,buf+total,bytesleft,0);

		/* no data has arrived yet (non-blocking socket) */
		if(n==-1 && errno==EAGAIN){
			time(&current_time);
			if(current_time-start_time>timeout)
				break;
			usleep(100);
			continue;
		        }

		/* receive error or client disconnect */
		else if(n<=0)
			break;

		/* apply bytes we received */
		total+=n;
		bytesleft-=n;
	        }

	/* return number of bytes actually received here */
	*len=total;

	/* return <=0 on failure, bytes received on success */
	return (n<=0)?n:total;
        }

int bartlby_agent_tcp_connect(char *host_name,int port,int *sd, struct service * svc){
	int result;

	result=bartlby_agent_tcp_my_connect(host_name,port,sd,"tcp", svc);

	return result;
}
        
int bartlby_agent_tcp_my_connect(char *host_name,int port,int *sd,char *proto, struct service * svc){
	int result;


	struct addrinfo hints, *res, *ressave;
	char ipvservice[20];
	int sockfd;
	
	sprintf(ipvservice, "%d",port);
	
	 memset(&hints, 0, sizeof(struct addrinfo));

   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
	
	 result = getaddrinfo(host_name, ipvservice, &hints, &res);
	 if(result < 0) {
	 		sprintf(svc->new_server_text, "getaddrinfo failed on '%s' - '%s'\n", host_name, gai_strerror(result));
			svc->current_state=STATE_CRITICAL;
			return STATE_CRITICAL;
	}
	ressave = res;
	 
	sockfd-1;
	while (res) {
        sockfd = socket(res->ai_family,
                        res->ai_socktype,
                        res->ai_protocol);

        if (!(sockfd < 0)) {
            if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
                break;

            close(sockfd);
            sockfd=-1;
        }
    res=res->ai_next;
  }
  freeaddrinfo(ressave);
 
	*sd=sockfd;
	
	
	
	switch(errno){  
		case ECONNREFUSED:
			sprintf(svc->new_server_text, "Connection refused by host\n");
			svc->current_state=STATE_CRITICAL;
			return STATE_CRITICAL;
			break;
		case ETIMEDOUT:
			sprintf(svc->new_server_text, "Timeout while attempting connection\n");
			svc->current_state=STATE_CRITICAL;
			return STATE_CRITICAL;
			break;
		case ENETUNREACH:
			sprintf(svc->new_server_text, "Network is unreachable\n");
			svc->current_state=STATE_CRITICAL;
			return STATE_CRITICAL;
			break;
		
  } 
	 if(sockfd==-1) {
  	return STATE_CRITICAL;
   }
	

	return STATE_OK;
}

