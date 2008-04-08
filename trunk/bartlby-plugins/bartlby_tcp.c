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



static long _port;
static char * _ip;
static int _timeout=10;
static char * _sendstr;
static char * _expectstr;
static int _skipwelcome=0;


static int connection_timed_out=0;

#define CRITICAL 2
#define WARN 1
#define OK 0


static void conn_timeout(int signo) {
	connection_timed_out=1;	
	
}

void help() {
	printf("bartlby check port\n");
	printf("Check if a given tcp port is open\n");
	printf("* optional send a string and expect a regex\n");
	printf("---------------------Usage-----------------------------------------------\n");
	printf("./hj_check_http [OPTIONS] HOST\n");
	printf("\n");
	printf("     -p,--port         Port (Default: 80)\n");
	printf("     -t,--timeout      Timeout in seconds on connect/send/recv\n");
	printf("     -i,--ip           Ip of the host\n");
	printf("     -s,--sendstr      String to send a \\n is append \n");
	printf("     -e,--expectstr    POSIX Regex\n");
	printf("     -w,--welcomeskip  Skip a welcome msg (e.g.: smtp announce)\n");
	printf("if the port cannot be opened within the timeout a critical is generated\n");
	printf("if sendstr is given and the result  doesnt match the regex in -expectstr also a warning is generated\n");
	printf(" Mail server example:\n");
	printf("        check_port -i localhost -p 25 -s 'HELO localhost\\n' -e '250.*' -w\n");
	printf("-------------------------------------------------------------------------\n");
	
	
	exit(1);	
}
void parse_options(int argc, char ** argv) {
	static struct option longopts[] = {
		{ "help",	0, NULL, 'h'},
		{ "ip",	0, NULL, 'i'},
		{ "timeout",	0, NULL, 't'},
		{ "port",	0, NULL, 'p'},
		{ "sendstr",	0, NULL, 's'},
		{ "expectstr", 	0, NULL, 'e'},
		
				
		{ NULL,		0, NULL, 0}
	};
	int c;
	
	
	if(argc < 2) {
		help();
			
	}

	for (;;) {
		c = getopt_long(argc, argv, "i:t:p:ws:e:h", longopts, (int *) 0);
		if (c == -1)
			break;
		switch (c) {
		case 'h':  /* --help */
			help();
		break;
		
		case 'i':
			_ip=optarg;
		break;
		case 't':
			_timeout=atoi(optarg);
		break;
		case 'p':
			_port=atol(optarg);
		break;
		case 's':
			_sendstr=optarg;
		break;
		case 'w':
			_skipwelcome=1;
		break;
		case 'e':
			_expectstr=optarg;
		break;
		
		
		
		
		
		
		
		
		}
	}
	
}

void my_exit(int code, char * msg) {
	printf("[ IP: '%s' Port: %ld expect: '%s' Resulted: %s ]\\\\dbr\n", _ip, _port,_expectstr,msg);
	exit(code);
}
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

int main(int argc, char ** argv) {
	int c_sock, c_rtc, b_rtc;
	struct sockaddr_in remote;
	struct sigaction act1, oact1;
	struct hostent * host;
	
	char server_out[2048];
	
	int is_esc;
	int oidx;
	int x;
	
	char server_return[2048];
	
	
	parse_options(argc, argv);
	
	connection_timed_out=0;
	alarm(_timeout);
	if((host = gethostbyname(_ip)) == 0) {
		my_exit(CRITICAL, "DNS Errror");	
	}
	if(connection_timed_out != 0) {
		my_exit(CRITICAL, "DNS timeout");	
	}
	connection_timed_out=0;
	alarm(0);
	
	memset(&remote, '\0', sizeof(remote));
	remote.sin_family=AF_INET;
	remote.sin_addr.s_addr=htonl(INADDR_ANY);
	remote.sin_addr.s_addr=((struct in_addr*) (host->h_addr))->s_addr;
	remote.sin_port=htons(_port);
	
	if((c_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		
		
		my_exit(CRITICAL, "Cant create socket");	
	}
	
	act1.sa_handler = conn_timeout;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags=0;
	#ifdef SA_INTERRUPT
	act1.sa_flags |= SA_INTERRUPT;
	#endif
	
	if(sigaction(SIGALRM, &act1, &oact1) < 0) {
		my_exit(CRITICAL, "Time out handler cant be installed");		
	}
	
	alarm(_timeout);
	c_rtc=connect(c_sock, (void *) &remote, sizeof(remote));
	alarm(0);
	
	if(connection_timed_out == 1 || c_rtc == -1) {
		
		my_exit(CRITICAL, "Timed out or other conn error");	
	}
	if(_sendstr != NULL) {
		if(_skipwelcome == 1) {
			connection_timed_out=0;
			alarm(_timeout);		
			b_rtc = read(c_sock, server_return, 1024);
			if(connection_timed_out != 0 || b_rtc < 0) {
				my_exit(CRITICAL, "welcome cannot be read");	
			}
			server_return[b_rtc-1]='\0';
			
		}
		alarm(_timeout);
		
		
		oidx=0;
		is_esc=0;
		strcpy(server_out, "");
		for(x=0; x<strlen(_sendstr); x++) {
			//Damn eval of \n\r
			if(_sendstr[x] == '\\') {
				is_esc=1;
				continue;
			}
			if(is_esc == 1) {
				
				if(_sendstr[x] == 'n') {
					
					server_out[oidx]='\n';
					oidx++;
				}	
				if(_sendstr[x] == 'r') {
					server_out[oidx]='\r';
					oidx++;
				}	
				is_esc=0;
			} else {
				
					server_out[oidx]=_sendstr[x];
					oidx++;
					is_esc=0;
						
			}
			
		}
		
		
		send(c_sock, server_out, strlen(server_out), 0);
		
		alarm(0);
		
		if(connection_timed_out != 0) {
			my_exit(CRITICAL, "timeout sending");	
		} 
		connection_timed_out=0;
		alarm(_timeout);		
		b_rtc = read(c_sock, server_return, 1024);
		if(connection_timed_out != 0 || b_rtc < 0) {
			my_exit(CRITICAL, "timeout reciev");	
		} 
		server_return[b_rtc-1]='\0';
		for(x=0; x<b_rtc-1; x++)  {
			if(server_return[x] == '\r')  {
				server_return[x] = ' ';
			}
		}
		if(_expectstr == NULL) {
			my_exit(OK, "No expect STR");
		} else {
			if(my_regMatch(server_return, _expectstr) != 0) {
				my_exit(CRITICAL, server_return);
			}	
		}
		
	}
	
	
	my_exit(OK, "OK");
	return 1;		
}
