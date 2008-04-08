#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <regex.h>
#include <getopt.h>

static int connection_timed_out=0;

static char * hostname;
static char * request;
static int timeout;
static int port;
static int get_request=1;
static int do_regex=0;
static char * regex;
static int neo_reg=0;
static int do_perf=0;

static int reg_matched=0;
static int recieved_bytes;
struct timeval tv_done;
int timing;

#define CRITICAL 2
#define WARN 1
#define OK 0

#define HEAD_REQ_COMMAND "HEAD %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n"
#define GET_REQ_COMMAND "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n"



int my_regMatch(char * subj, char * pat) {
	int iResult;	
	regex_t reg;
	if(regcomp(&reg, pat, RE_SYNTAX_EGREP) != 0) {
		return -1;
	}
	iResult = regexec(&reg, subj,0 ,0,0);
	regfree(&reg);
	return iResult;	
	
	
	
	
}


void my_exit(int i, char * msg, int http_code) {
	printf("Check HTTP 1.0 h.januschka - Server: %s; Port: %d; URI: %s;HTTP: %d; [%s] Time: %03ld/%03ld Bytes: %d ", hostname, port, request, http_code,  msg, tv_done.tv_sec, tv_done.tv_usec, recieved_bytes);
	if(neo_reg) printf(" NEOGATED ");
	
	if(do_regex && http_code != 0) { 
		
		printf(" matched: %s/%d \n", regex, reg_matched);
		if(reg_matched > 0) {
			if(!neo_reg) {
				exit(OK);
			} else {
				exit(CRITICAL);
			}
		} else {
			if(!neo_reg) {
				exit(CRITICAL);
			} else {
				exit(OK);
			}
		}
	}
	printf("\n");
	
	exit(i);
	
}

static void conn_timeout(int signo) {
	connection_timed_out=1;	
	
}
void help() {
	printf("HTTP 1.1 Check 0.9\n");
	printf("Written by h.januschka (h@januschka.com)\n");
	printf("* Checks the HTTP status Code 200/404/500\n");
	printf("* Triggers timeout\n");
	printf("* Triggers DNS lookup failure's\n");
	printf("---------------------Usage-----------------------------------------------\n");
	printf("./hj_check_http [OPTIONS] HOST\n");
	printf("\n");
	printf("     -p,--port         Port (Default: 80)\n");
	printf("     -t,--timeout      Timeout in seconds on connect/send/recv\n");
	printf("     -r,--request      Request to Send (Default /)\n");
	printf("     -g,--head         HEAD request instead of get\n");
	printf("     -m,--match        RegMatch on Result of request if matches found exit(OK) else exit(CRITICAL)\n");
	printf("     -n,--neogate      if -m is used alert if match found\n");
	printf("-------------------------------------------------------------------------\n");
	printf("Creation Date: 2004-09-21 h.januschka\n");
	
	exit(1);	
}
void parse_options(int argc, char ** argv) {
	static struct option longopts[] = {
		{ "help",	0, NULL, 'h'},
		{ "port",	0, NULL, 'p'},
		{ "timeout",	0, NULL, 't'},
		{ "request",	0, NULL, 'r'},
		{ "head",	0, NULL, 'g'},
		{ "match", 	0, NULL, 'm'},
		{ "neogate",	0, NULL, 'n'},
		{ "xxa",	0, NULL, 'S'},
		{ "xxb",	0, NULL, 'U'},
		{ "xxc",	0, NULL, 'H'},
		{ "xxd",	0, NULL, 'T'},
		{ "xxe",	0, NULL, 'P'},
		{ "xxf",	0, NULL, 'N'},
				
		{ NULL,		0, NULL, 0}
	};
	int c;
	
	
	if(argc < 2) {
		help();
			
	}

	for (;;) {
		c = getopt_long(argc, argv, "S:U:H:N:p:t:m:nr:hgP", longopts, (int *) 0);
		if (c == -1)
			break;
		switch (c) {
		case 'h':  /* --help */
			help();
		break;
		
		case 'N':
		case 'n':
			neo_reg=1;
		break;
		
		case 'S':
		case 'm':
			do_regex=1;
			regex=optarg;
		break;
		
		case 'g':
			get_request=0;
		break;
		case 'P':
			do_perf=1;
		break;
		case 'p':
			port=atoi(optarg);
		break;
		
		case 't':
			timeout=atoi(optarg);
		break;
		
		case 'H':
			hostname=optarg;
		break;
		case 'U':
		case 'r':
			request=optarg;
		break;
		
		
		
		
		
		
		}
	}
	if(hostname == NULL)
		hostname=argv[optind];
	
}
		
int bartlby_milli_timediff(struct timeval end, struct timeval start) {
	        return ((end.tv_sec - start.tv_sec) * 1000) +  (((1000000 + end.tv_usec - start.tv_usec) / 1000) - 1000);
}



int main(int argc, char ** argv) {
	int c_rtc=-1;
	int c_sock;
	int b_rtc;
	
	int HTTP_code;
	
	char * client_request;
	char server_return[1024];
	char fin_server_return[1024000];
	
	struct sockaddr_in remote;
	struct sigaction act1, oact1;
	struct hostent * host;
	
	struct timeval tv_start, tv_end;
	
	//tv_done.tv_sec=0;
	//tv_done.tv_usec=0;
	gettimeofday(&tv_start, NULL);
	
	//Some defaults
	port=80;
	timeout=5;
	get_request=1;
	request=malloc(sizeof(char)*2);
	request[0]='/';
	request[1]='\0';
	
	hostname = NULL;
		
	parse_options(argc, argv);
	
	
	
	if(get_request) {
		client_request=malloc(sizeof(char)*(strlen(GET_REQ_COMMAND)+strlen(hostname)+strlen(request)));
		sprintf(client_request, GET_REQ_COMMAND, request, hostname);
	} else {
		client_request=malloc(sizeof(char)*(strlen(HEAD_REQ_COMMAND)+strlen(hostname)+strlen(request)));
		sprintf(client_request, HEAD_REQ_COMMAND, request, hostname);
	}
	//printf("sending: '%s'\n", client_request);
	
	connection_timed_out=0;
	alarm(timeout);
	if((host = gethostbyname(hostname)) == 0) {
		my_exit(CRITICAL, "DNS Errror", 0);	
	}
	if(connection_timed_out != 0) {
		my_exit(CRITICAL, "DNS timeout", 0);	
	}
	connection_timed_out=0;
	alarm(0);
	
	memset(&remote, '\0', sizeof(remote));
	remote.sin_family=AF_INET;
	remote.sin_addr.s_addr=htonl(INADDR_ANY);
	remote.sin_addr.s_addr=((struct in_addr*) (host->h_addr))->s_addr;
	remote.sin_port=htons(port);
	
	if((c_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		free(client_request);
		
		my_exit(CRITICAL, "Cant create socket", 0);	
	}
	
	act1.sa_handler = conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		free(client_request);
		my_exit(CRITICAL, "Time out handler cant be installed",0);		
	}
	
	alarm(timeout);
	c_rtc=connect(c_sock, (void *) &remote, sizeof(remote));
	alarm(0);
	
	if(connection_timed_out == 1 || c_rtc == -1) {
		free(client_request);
		my_exit(CRITICAL, "Timed out or other conn error",0);	
	}
	
	connection_timed_out=0;
	
	alarm(timeout);
	send(c_sock, client_request, strlen(client_request), 0);
	
	alarm(0);
	
	
	free(client_request);
	
	if(connection_timed_out != 0) {
		my_exit(CRITICAL, "timeout sending", 0);	
	} 
	
	connection_timed_out=0;
	
	alarm(timeout);	
	while ((b_rtc = read(c_sock, server_return, 1024))) {
		//printf("%d: %s\n", b_rtc, server_return);
		sscanf(server_return, "HTTP/1.1 %d\r\n", &HTTP_code);
		server_return[b_rtc]='\0';
		strcat(fin_server_return, server_return);
		
		if(b_rtc == -1) break;
		//printf("%d, %s", get_request, server_return);
		recieved_bytes += b_rtc;
	}
	
	
	if(connection_timed_out != 0) {
			my_exit(CRITICAL, "Timeout during recv",0);	
	}
	
	alarm(0);
	
	
	connection_timed_out=0;
	
	
	close(c_sock);
	if(do_regex) {
	      if(my_regMatch(fin_server_return, regex) == 0) reg_matched++;
	}
	 
	
	
	gettimeofday(&tv_end, NULL);
	//tv_done.tv_sec=tv_end.tv_sec-tv_start.tv_sec;
	//tv_done.tv_usec=tv_end.tv_usec/1000-tv_start.tv_usec/1000;
	//time_done=(tv_end.tv_usec - tv_start.tv_usec)/1000;
	//printf("Time: Seconds: %03ld/%03ld\n", tv_done.tv_sec, tv_done.tv_usec);
	
	timing=bartlby_milli_timediff(tv_end, tv_start);

	if(do_perf == 1 ) {
		printf("PERF: %d ms\n", timing);
	}
	
	
	switch(HTTP_code) {
		case 200:
			my_exit(OK, "DNS=OK;Timeout=false;HTTP=OK", HTTP_code);
		break;
		case 404:
			my_exit(WARN, "File not found", HTTP_code);
		break;
		
		case 401:
			my_exit(OK, "Access Denied - but alive", HTTP_code);
		break;
		case 403:
			my_exit(OK, "Access Denied - but alive", HTTP_code);
		case 500:
			my_exit(CRITICAL, "Internal Server Error", HTTP_code);
		break;	
		case 302:
			my_exit(OK, "Document moved", HTTP_code);
		break;
		default:
			my_exit(CRITICAL, "Unkown state", HTTP_code);
		
	}
	return 1;
}
