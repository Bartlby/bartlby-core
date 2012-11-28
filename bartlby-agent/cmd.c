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



#include <time.h>
#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <getopt.h>
#include <netdb.h>



unsigned long crc32_table[256];

char * new_passive_text;
int new_passive_state;

char * passive_action=NULL;
char * passive_host=NULL;
int passive_service=-1;
int passive_port=-1;

static sig_atomic_t connection_timed_out=0;


static void bartlby_conn_timeout(int signo) {
 	connection_timed_out = 1;
}


int connect_to(char * host, int port) {
	int client_socket;
	int client_connect_retval=-1;
	struct sockaddr_in remote_side;
	struct hostent * remote_host;
	struct sigaction act1, oact1;
	
	
	connection_timed_out=0;
	
	if((remote_host = gethostbyname(host)) == 0) {
		
		return -1; //timeout
	}
	memset(&remote_side, '\0', sizeof(remote_side));
	remote_side.sin_family=AF_INET;
	remote_side.sin_addr.s_addr = htonl(INADDR_ANY);
	remote_side.sin_addr.s_addr = ((struct in_addr *) (remote_host->h_addr))->s_addr;
	remote_side.sin_port=htons(port);
	
	if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			
		
		return -2; //Socket
			
			
	}
	act1.sa_handler = bartlby_conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		
		return -3; //timeout handler
	
		
	}
	alarm(5);
	client_connect_retval = connect(client_socket, (void *) &remote_side, sizeof(remote_side));
	alarm(0);
	
	if(connection_timed_out == 1 || client_connect_retval == -1) {
		return -4; //connect
	} 
	connection_timed_out=0;
	
	return client_socket; 	
}

void cmd_get_passive() {
	int res;
	char verstr[2048];
	char cmdstr[2048];
	char result[2048];
	char * token, * token_t;
	int rc;
	
	res=connect_to(passive_host, passive_port);
	if(res > 0) {
		
		connection_timed_out=0;
		alarm(5);
		if(read(res, verstr, 1024) < 0) {
			printf("Read error\n");
			exit(1);
		}
		if(verstr[0] != '+') {
			printf("Server said a bad result: '%s'\n", verstr);
			close(res);
			exit(1);
		}
		alarm(0);
		//printf("Connected to: %s\n", verstr);		
		sprintf(cmdstr, "2|%d|", passive_service);
		connection_timed_out=0;
		alarm(5);
		if(write(res, cmdstr, 1024) < 0) {
			printf("BAD2!\n");
			exit(1);
		}
		alarm(0);
		connection_timed_out=0;
		alarm(5);
		if((rc=read(res, result, 1024)) < 0) {
			printf("BAD!\n");
			exit(1);
		}
		alarm(0);
		result[rc-1]='\0'; //cheap trim *fg*
		if(result[0] != '+') {
			printf("Server said a bad result: '%s'\n", result);
		}  else {
			token=strtok(result, "|");
			if(token != NULL) {
				token=strtok(NULL, " ");
				if(token != NULL) {
					token_t=strtok(NULL, "|");
					if(token_t == NULL) {
						token_t=strdup("");
					}
					printf("%s %s", token, token_t);	
					printf("\n");
					close(res);
					exit(0);
					
				} else {
					close(res);
					printf("hmmm3\n");	
					exit(3);
				}
			} else {
				close(res);
				printf("hmmm1\n");	
				exit(3);
			}
		}
		
		
			
	} else {
		
		printf("Connect failed\n");
		exit(2);	
	}	
}

void cmd_get_server_id() {
	int res;
	char verstr[2048];
	char cmdstr[2048];
	char result[2048];
	char myhostname[255];
	
	int rc;
	
	if(gethostname(myhostname, 255) != 0) {
		printf("gethostname() failed\n");
		exit(1);	
	}
	fprintf(stderr, "trying to get server id for: %s \n", myhostname);
	res=connect_to(passive_host, passive_port);
	if(res > 0) {
		
		connection_timed_out=0;
		alarm(5);
		if(read(res, verstr, 1024) < 0) {
			printf("BAD!\n");
			exit(1);
		}
		if(verstr[0] != '+') {
			printf("Server said a bad result: '%s'\n", verstr);
			close(res);
			exit(1);
		}
		alarm(0);
		//printf("Connected to: %s\n", verstr);		
		sprintf(cmdstr, "5|%s|\n", myhostname);
		
		connection_timed_out=0;
		alarm(5);
		if(write(res, cmdstr, 1024) < 0) {
			printf("BAD2!\n");
			exit(1);
		}
		alarm(0);
		connection_timed_out=0;
		alarm(5);
		while((rc=read(res, result, 1024)) > 0) {
			result[rc-1]='\0';
			printf("%s", result);
		}
		
			
	} else {
		
		printf("Connect failed\n");
		exit(2);	
	}	
}

void cmd_get_services() {
	int res;
	char verstr[2048];
	char cmdstr[2048];
	char result[2048];
	
	int rc;
	
	res=connect_to(passive_host, passive_port);
	if(res > 0) {
		
		connection_timed_out=0;
		alarm(5);
		if(read(res, verstr, 1024) < 0) {
			printf("BAD!\n");
			exit(1);
		}
		if(verstr[0] != '+') {
			printf("Server said a bad result: '%s'\n", verstr);
			close(res);
			exit(1);
		}
		alarm(0);
		//printf("Connected to: %s\n", verstr);		
		sprintf(cmdstr, "4|%d|\n", passive_service);
		
		connection_timed_out=0;
		alarm(5);
		if(write(res, cmdstr, 1024) < 0) {
			printf("BAD2!\n");
			exit(1);
		}
		alarm(0);
		connection_timed_out=0;
		alarm(5);
		while((rc=read(res, result, 1024)) > 0) {
			result[rc-1]='\0';
			printf("%s", result);
		}
		
			
	} else {
		
		printf("Connect failed\n");
		exit(2);	
	}	
}

void cmd_set_passive() {
	int res;
	char verstr[2048];
	char cmdstr[2048];
	char result[2048];
	
	int rc;
	
	res=connect_to(passive_host, passive_port);
	if(res > 0) {
		
		connection_timed_out=0;
		alarm(5);
		if(read(res, verstr, 1024) < 0) {
			printf("BAD!\n");
			exit(1);
		}
		if(verstr[0] != '+') {
			printf("Server said a bad result: '%s'\n", verstr);
			close(res);
			exit(1);
		}
		alarm(0);
		//printf("Connected to: %s\n", verstr);		
		sprintf(cmdstr, "1|%d|%d|%s", passive_service, new_passive_state, new_passive_text);
		connection_timed_out=0;
		alarm(5);
		if(write(res, cmdstr, 1024) < 0) {
			printf("BAD2!\n");
			exit(1);
		}
		alarm(0);
		connection_timed_out=0;
		alarm(5);
		if((rc=read(res, result, 1024)) < 0) {
			printf("BAD!\n");
			exit(1);
		}
		alarm(0);
		result[rc-1]='\0'; //cheap trim *fg*
		if(result[0] != '+') {
			printf("Server said a bad result: '%s'\n", result);
		}  else {
			printf("%s\n", result);
		}
		close(res);			
	} else {
		
		printf("Connect failed\n");
		exit(2);	
	}	
	exit(1);
}

void help() {
	printf("Bartlby cmd tool:\n");
	printf("-h             display help\n");
	printf("-i             host\n");
	printf("-p             port \n");
	printf("-s             service-id or server-id (if action == get_services)\n");
	printf("-m             new service text\n");
	printf("-e             new service state (0,1,2)\n");
	printf("-a             action\n");
	printf("               maybe: get_passive, set_passive, get_services, get_server_id\n");
	exit(1);	
}
void parse_options(int argc, char ** argv) {
	static struct option longopts[] = {
		{ "help",	0, NULL, 'h'},
		{ "host",	0, NULL, 'i'},
		{ "port", 0, NULL, 'p'},
		{ "service-id",	0, NULL, 's'},
		{ "action",	0, NULL, 'a'},
		
		{ NULL,		0, NULL, 0}
	};
	int c;
	
	
	if(argc < 2) {
		help();
			
	}

	for (;;) {
		c = getopt_long(argc, argv, "hi:e:m:s:p:a:S", longopts, (int *) 0);
		if (c == -1)
			break;
		switch (c) {
		case 'h':  /* --help */
			help();
		break;
		case 'e':
			new_passive_state=atoi(optarg);
		break;
		case 'm':
			new_passive_text=optarg;
		break;
		case 'i':
			
			passive_host=optarg;
		break;
		case 'p':
			passive_port=atoi(optarg);
		break;
		case 's':
			passive_service=atoi(optarg);
		break;
		case 'a':
			passive_action=optarg;
		break;
		
		
		default:
			help();
		}
	}
	
	
}

int main(int argc, char ** argv) {
	//printf("portier client\n");
	parse_options(argc, argv);
	//printf("Working on '%s:%d/%d' action(%s)\n", passive_host, passive_port, passive_service, passive_action);
	if(passive_action == NULL) {
		printf("Action unkown!\n");	
		exit(2);
	}
	if(passive_service <=0 || passive_port <= 0 || passive_host == NULL ){
		printf("Either port, service or host is missing\n");
		exit(3);	
	}
	
	
	if(strcmp(passive_action, "get_passive") == 0) {
		cmd_get_passive();		
	} else if (strcmp(passive_action, "set_passive") == 0) {
		cmd_set_passive();
	} else if(strcmp(passive_action, "get_services") == 0) {
		cmd_get_services();
	} else if(strcmp(passive_action, "get_server_id") == 0) {
		cmd_get_server_id();				
	} else {
		printf("Hmm action is pretty unusefull\n");
	}
	
	
	
	return 0;
}
