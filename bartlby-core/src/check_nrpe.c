/* $Id: shmt.c,v 1.7 2008/03/03 12:01:27 hjanuschka Exp $ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2008 Helmut Januschka - All Rights Reserved
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
/* ----------------------------------------------------------------------- *
 * BASED upon the original source of CHECK_NRPE package from nagios:
 *****************************************************************************************
 *
 * CHECK_NRPE.C - NRPE Plugin For Nagios
 * Copyright (c) 1999-2006 Ethan Galstad (nagios@nagios.org)
 * License: GPL
 *
 * Last Modified: 04-09-2006
 *
 * Command line: CHECK_NRPE -H <host_address> [-p port] [-c command] [-to to_sec]
 *
 * Description:
 *
 * This plugin will attempt to connect to the NRPE daemon on the specified server and port.
 * The daemon will attempt to run the command defined as [command].  Program output and
 * return code are sent back from the daemon and displayed as this plugin's own output and
 * return code.
 *
 ******************************************************************************************* 
 *   Modified and merged for bartlby on 24.05.2006 (REMEMBER This portion of code
 *   is one level below alpha!! :) you explicit have to enable NRP feature(s) 
 *   in the ./configure of bartlby-core (--enable-nrpe=yes)
 *
 * ----------------------------------------------------------------------- */



#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <netdb.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>

#include <bartlby.h>





#ifndef WITH_NRPE
void nrpe_display_license(void) {
	return;	
}
void bartlby_check_nrpe(struct service * svc, char * cfgfile, int use_ssl) {
	sprintf(svc->new_server_text, "%s", "NRPE  is expiremental Support ist not compiled in (--enable-nrpe=yes) bartlby-core");
	svc->current_state = STATE_CRITICAL;
}

#else

static sig_atomic_t conn_timedout = 0;
static unsigned long crc32_table[256];


#ifdef HAVE_SSL
#include <openssl/dh.h>
#include <openssl/ssl.h>



#endif



#define PROGRAM_VERSION "2.3-bartlby"
#define MODIFICATION_DATE "05-24-2006"
#define STATE_UNKNOWN   3
#define MAX_PACKETBUFFER_LENGTH 1024
#define OK		0
#define DEFAULT_SOCKET_TIMEOUT  10
#define TRUE            1
#define FALSE           0
#define MAX_INPUT_BUFFER        2048
#define DEFAULT_SERVER_PORT 5666
#define DEFAULT_NRPE_COMMAND    "_NRPE_CHECK"  /* check version of NRPE daemon */
#define QUERY_PACKET		1
#define NRPE_PACKET_VERSION_2   2
#define	RESPONSE_PACKET		2

#define OK		0
#define ERROR		-1

#define TRUE		1
#define FALSE		0


typedef struct packet_struct{
        int16_t   packet_version;
        int16_t   packet_type;
        u_int32_t crc32_value;
        int16_t   result_code;
        char      buffer[MAX_PACKETBUFFER_LENGTH];
        }packet;
        

void nrpe_generate_crc32_table(void);
int my_connect(char *host_name,int port,int *sd,char *proto, struct service * svc);
void alarm_handler(int sig);
int my_tcp_connect(char *host_name,int port,int *sd, struct service * svc);
int my_inet_aton_self(register const char *cp, struct in_addr *addr);
unsigned long calculate_crc32(char *buffer, int buffer_size);
void randomize_buffer(char *buffer,int buffer_size);
int nrperecvall(int s, char *buf, int *len, int timeout);        
int nrpesendall(int s, char *buf, int *len);
#ifdef HAVE_SSL
static int ssl_connect_timeout(SSL *ssl, int tmo);
static int unblock_socket(int soc);
static int block_socket(int soc);
#endif


void bartlby_check_nrpe(struct service * svc, char * cfgfile, int use_ssl) {
	
	int sd;
	
	char query[MAX_INPUT_BUFFER]="";
	
	#ifdef HAVE_SSL
	SSL_METHOD *meth;
	SSL_CTX *ctx;
	SSL *ssl;
	#endif
     
     u_int32_t packet_crc32;
     u_int32_t calculated_crc32;
     int16_t result;
     int rc;
     packet send_packet;
     packet receive_packet;
     int bytes_to_send;
     int bytes_to_recv;
     
     signal(SIGALRM,alarm_handler);
      /* generate the CRC 32 table */
      nrpe_generate_crc32_table();
        
#ifdef HAVE_SSL
        /* initialize SSL */
        if(use_ssl==TRUE){
                SSL_library_init();
                SSLeay_add_ssl_algorithms();
                meth=SSLv23_client_method();
                SSL_load_error_strings();
                if((ctx=SSL_CTX_new(meth))==NULL){
                        sprintf(svc->new_server_text, "%s", "CHECK_NRPE: Error - could not create SSL context.\n");
                        svc->current_state=STATE_CRITICAL;
                        }

                
                /* use only TLSv1 protocol */
                SSL_CTX_set_options(ctx,SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
       }
#endif	
	
	

	conn_timedout=0;
	alarm(svc->service_check_timeout);
	result=my_tcp_connect(svc->srv->client_ip,svc->srv->client_port,&sd, svc);
	
	if(conn_timedout == 1) {
		sprintf(svc->new_server_text, "%s", "timed out");
		svc->current_state=STATE_CRITICAL;	
		return;
	}
	if(result != STATE_OK) {
		svc->current_state=STATE_CRITICAL;
		return;
	}
	if(result == STATE_OK) {
#ifdef HAVE_SSL
        /* do SSL handshake */
		if(use_ssl==TRUE){
     		if((ssl=SSL_new(ctx))!=NULL){
     	     	SSL_CTX_set_cipher_list(ctx,"ADH");
				SSL_set_fd(ssl,sd);
				unblock_socket(sd);
				
				rc=ssl_connect_timeout(ssl, svc->service_check_timeout);
				
				
				if(rc <= 0) {
					
					sprintf(svc->new_server_text, "%s", "timed out!!");
					svc->current_state=STATE_CRITICAL;	
					return;
				}
				block_socket(sd);
				conn_timedout=0;
				alarm(svc->service_check_timeout);
				
     	          if(rc !=1){
     	          	sprintf(svc->new_server_text, "%s", "CHECK_NRPE: Error - Could not complete SSL handshake.\n");
     	          	svc->current_state=STATE_CRITICAL;
     	          	SSL_CTX_free(ctx);
                        	
     	          	return;
     	          }
			} else {
				sprintf(svc->new_server_text,"CHECK_NRPE: Error - Could not create SSL connection structure.\n"); 
				svc->current_state=STATE_CRITICAL;
				SSL_CTX_free(ctx);
                    close(sd);
                    return;
			}
		}
#endif		
		bzero(&send_packet,sizeof(send_packet));
     	
     	/* fill the packet with semi-random data */
     	randomize_buffer((char *)&send_packet,sizeof(send_packet));
     	
     	
     	snprintf(query,sizeof(query),"%s",svc->plugin);
        	query[sizeof(query)-1]='\x0';
        	strcat(query,"!!");
        	strncat(query,svc->plugin_arguments,strlen(svc->plugin_arguments));
          query[sizeof(query)-1]='\x0';
     	
     	
     	/* initialize packet data */
     	send_packet.packet_version=(int16_t)htons(NRPE_PACKET_VERSION_2);
     	send_packet.packet_type=(int16_t)htons(QUERY_PACKET);
     	strncpy(&send_packet.buffer[0],query,MAX_PACKETBUFFER_LENGTH);
     	send_packet.buffer[MAX_PACKETBUFFER_LENGTH-1]='\x0';	
     	
     	send_packet.crc32_value=(u_int32_t)0L;
          calculated_crc32=calculate_crc32((char *)&send_packet,sizeof(send_packet));
          send_packet.crc32_value=(u_int32_t)htonl(calculated_crc32);
     	
     	 bytes_to_send=sizeof(send_packet);
           if(use_ssl==FALSE)  {
           	conn_timedout=0;
			alarm(svc->service_check_timeout);
           	rc=nrpesendall(sd,(char *)&send_packet,&bytes_to_send);
           	if(conn_timedout == 1) {
					_log("timeout ok");
					sprintf(svc->new_server_text, "%s", "timed out1");
					svc->current_state=STATE_CRITICAL;	
					return;
			}
          }
#ifdef HAVE_SSL
		else{
			conn_timedout=0;
			alarm(svc->service_check_timeout);
			
          	rc=SSL_write(ssl,&send_packet,bytes_to_send);
          	if(conn_timedout == 1) {
					_log("timeout ok");
					sprintf(svc->new_server_text, "%s", "timed out2");
					svc->current_state=STATE_CRITICAL;	
					return;
			}
               if(rc<0)
               	rc=-1;
			}
			
#endif
		if(rc==-1){
			sprintf(svc->new_server_text, "%s", "CHECK_NRPE: Error sending query to host.\n");
			close(sd);
			svc->current_state=STATE_CRITICAL;
			return;
		}
     	bytes_to_recv=sizeof(receive_packet);
     	if(use_ssl==FALSE) {
     		conn_timedout=0;
			alarm(svc->service_check_timeout);
				
			rc=nrperecvall(sd,(char *)&receive_packet,&bytes_to_recv,svc->service_check_timeout);
			
			if(conn_timedout == 1) {
					_log("timeout ok");
					sprintf(svc->new_server_text, "%s", "timed out3");
					svc->current_state=STATE_CRITICAL;	
					return;
			}
		}
#ifdef HAVE_SSL
		else {
			conn_timedout=0;
			alarm(svc->service_check_timeout);
          	rc=SSL_read(ssl,&receive_packet,bytes_to_recv);
          	if(conn_timedout == 1) {
					_log("timeout ok");
					sprintf(svc->new_server_text, "%s", "timed out4");
					svc->current_state=STATE_CRITICAL;	
					return;
			}
          }
#endif

		/* reset timeout */
		alarm(0);		
#ifdef HAVE_SSL
		if(use_ssl==TRUE){
		        SSL_shutdown(ssl);
		        SSL_free(ssl);
		        SSL_CTX_free(ctx);
		}
#endif    
	close(sd);
	if(rc<0){
		sprintf(svc->new_server_text, "%s", "CHECK_NRPE: Error receiving data from daemon.\n");
		svc->current_state=STATE_CRITICAL;
		return;
	}else if(rc==0){
		sprintf(svc->new_server_text,"%s", "CHECK_NRPE: Received 0 bytes from daemon.  Check the remote server logs for error messages.\n");
		svc->current_state=STATE_CRITICAL;
		return;
	}else if(bytes_to_recv<sizeof(receive_packet)){
		sprintf(svc->new_server_text, "CHECK_NRPE: Receive underflow - only %d bytes received (%ld expected).\n",bytes_to_recv,(unsigned long)sizeof(receive_packet));
		svc->current_state=STATE_CRITICAL;
		return;
	}
           
	packet_crc32=ntohl(receive_packet.crc32_value);
	receive_packet.crc32_value=0L;
	calculated_crc32=calculate_crc32((char *)&receive_packet,sizeof(receive_packet));
	if(packet_crc32!=calculated_crc32){
	        sprintf(svc->new_server_text,"%s", "CHECK_NRPE: Response packet had invalid CRC32.\n");
	        svc->current_state=STATE_CRITICAL;
	        close(sd);
	        return;
	        }
	
	/* check packet version */
	if(ntohs(receive_packet.packet_version)!=NRPE_PACKET_VERSION_2){
	        sprintf(svc->new_server_text,"%s","CHECK_NRPE: Invalid packet version received from server.\n");
	        close(sd);
	        svc->current_state=STATE_CRITICAL;
	        return;
	        }
	
	/* check packet type */
	if(ntohs(receive_packet.packet_type)!=RESPONSE_PACKET){
	        sprintf(svc->new_server_text,"%s","CHECK_NRPE: Invalid packet type received from server.\n");
	        close(sd);
	        svc->current_state=STATE_CRITICAL;
	        return;
	        }
	
	/* get the return code from the remote plugin */
	result=(int16_t)ntohs(receive_packet.result_code);
	
	/* print the output returned by the daemon */
	receive_packet.buffer[MAX_PACKETBUFFER_LENGTH-1]='\x0';
	if(!strcmp(receive_packet.buffer,""))
	        sprintf(svc->new_server_text,"%s","CHECK_NRPE: No output returned from daemon.\n");
	else
	        sprintf(svc->new_server_text,"%s",receive_packet.buffer);


	
	}
	switch ((int16_t)ntohs(receive_packet.result_code)) {
		case STATE_OK:
		case STATE_WARNING:
		case STATE_CRITICAL:
			svc->current_state=(int16_t)ntohs(receive_packet.result_code);
		break;
		
		default:
		
		svc->current_state=STATE_CRITICAL;
				
	}
	
                    
     
	return;
	
	
	
		
}

void nrpe_generate_crc32_table(void){
	unsigned long crc, poly;
	int i, j;

	poly=0xEDB88320L;
	for(i=0;i<256;i++){
		crc=i;
		for(j=8;j>0;j--){
			if(crc & 1)
				crc=(crc>>1)^poly;
			else
				crc>>=1;
		        }
		crc32_table[i]=crc;
                }

	return;
}


/* opens a tcp or udp connection to a remote host */
int my_connect(char *host_name,int port,int *sd,char *proto, struct service * svc){
	struct sockaddr_in servaddr;
	struct hostent *hp;
	struct protoent *ptrp;
	int result;

	bzero((char *)&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);

	/* try to bypass using a DNS lookup if this is just an IP address */
	if(!my_inet_aton_self(host_name,&servaddr.sin_addr)){

		/* else do a DNS lookup */
		hp=gethostbyname((const char *)host_name);
		if(hp==NULL){
			sprintf(svc->new_server_text, "Invalid host name '%s'\n",host_name);
			svc->current_state=STATE_CRITICAL;
			return STATE_UNKNOWN;
		        }

		memcpy(&servaddr.sin_addr,hp->h_addr,hp->h_length);
	        }

	/* map transport protocol name to protocol number */
	if(((ptrp=getprotobyname(proto)))==NULL){
		sprintf(svc->new_server_text, "Cannot map \"%s\" to protocol number\n",proto);
		svc->current_state=STATE_CRITICAL;
		return STATE_UNKNOWN;
	        }

	/* create a socket */
	*sd=socket(PF_INET,(!strcmp(proto,"udp"))?SOCK_DGRAM:SOCK_STREAM,ptrp->p_proto);
	if(*sd<0){
		sprintf(svc->new_server_text, "Socket creation failed\n");
		svc->current_state=STATE_CRITICAL;
		return STATE_UNKNOWN;
	        }

	/* open a connection */
	result=connect(*sd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	if(result<0){
		switch(errno){  
		case ECONNREFUSED:
			sprintf(svc->new_server_text, "Connection refused by host\n");
			svc->current_state=STATE_CRITICAL;
			break;
		case ETIMEDOUT:
			sprintf(svc->new_server_text, "Timeout while attempting connection\n");
			svc->current_state=STATE_CRITICAL;
			break;
		case ENETUNREACH:
			sprintf(svc->new_server_text, "Network is unreachable\n");
			svc->current_state=STATE_CRITICAL;
			break;
		default:
			sprintf(svc->new_server_text, "Connection refused or timed out\n");
			svc->current_state=STATE_CRITICAL;
		}

			return STATE_CRITICAL;
	 }

	return STATE_OK;
}

void alarm_handler(int sig){

        conn_timedout = 1;
        //_log("FIXME: nrpe timeout SSL_connect");
        
       
       
}
int my_tcp_connect(char *host_name,int port,int *sd, struct service * svc){
	int result;

	result=my_connect(host_name,port,sd,"tcp", svc);

	return result;
}


/* This code was taken from Fyodor's nmap utility, which was originally taken from
   the GLIBC 2.0.6 libraries because Solaris doesn't contain the inet_aton() funtion. */
int my_inet_aton_self(register const char *cp, struct in_addr *addr){
	register unsigned int val;	/* changed from u_long --david */
	register int base, n;
	register char c;
	u_int parts[4];
	register u_int *pp = parts;

	c=*cp;

	for(;;){

		/*
		 * Collect number up to ``.''.
		 * Values are specified as for C:
		 * 0x=hex, 0=octal, isdigit=decimal.
		 */
		if (!isdigit((int)c))
			return (0);
		val=0;
		base=10;

		if(c=='0'){
			c=*++cp;
			if(c=='x'||c=='X')
				base=16,c=*++cp;
			else
				base=8;
		        }

		for(;;){
			if(isascii((int)c) && isdigit((int)c)){
				val=(val*base)+(c -'0');
				c=*++cp;
			        } 
			else if(base==16 && isascii((int)c) && isxdigit((int)c)){
				val=(val<<4) | (c+10-(islower((int)c)?'a':'A'));
				c = *++cp;
			        } 
			else
				break;
		        }

		if(c=='.'){

			/*
			 * Internet format:
			 *	a.b.c.d
			 *	a.b.c	(with c treated as 16 bits)
			 *	a.b	(with b treated as 24 bits)
			 */
			if(pp>=parts+3)
				return (0);
			*pp++=val;
			c=*++cp;
		        } 
		else
			break;
	        }

	/* Check for trailing characters */
	if(c!='\0' && (!isascii((int)c) || !isspace((int)c)))
		return (0);

	/* Concoct the address according to the number of parts specified */
	n=pp-parts+1;
	switch(n){

	case 0:
		return (0);		/* initial nondigit */

	case 1:				/* a -- 32 bits */
		break;

	case 2:				/* a.b -- 8.24 bits */
		if(val>0xffffff)
			return (0);
		val|=parts[0]<<24;
		break;

	case 3:				/* a.b.c -- 8.8.16 bits */
		if(val>0xffff)
			return (0);
		val|=(parts[0]<< 24) | (parts[1]<<16);
		break;

	case 4:				/* a.b.c.d -- 8.8.8.8 bits */
		if(val>0xff)
			return (0);
		val|=(parts[0]<<24) | (parts[1]<<16) | (parts[2]<<8);
		break;
	        }

	if(addr)
		addr->s_addr=htonl(val);

	return (1);
        }
        
void randomize_buffer(char *buffer,int buffer_size){
	FILE *fp;
	int x;
	int seed;

	/**** FILL BUFFER WITH RANDOM ALPHA-NUMERIC CHARACTERS ****/

	/***************************************************************
	   Only use alpha-numeric characters becase plugins usually
	   only generate numbers and letters in their output.  We
	   want the buffer to contain the same set of characters as
	   plugins, so its harder to distinguish where the real output
	   ends and the rest of the buffer (padded randomly) starts.
	***************************************************************/

	/* try to get seed value from /dev/urandom, as its a better source of entropy */
	fp=fopen("/dev/urandom","r");
	if(fp!=NULL){
		seed=fgetc(fp);
		fclose(fp);
	        }

	/* else fallback to using the current time as the seed */
	else
		seed=(int)time(NULL);

	srand(seed);
	for(x=0;x<buffer_size;x++)
		buffer[x]=(int)'0'+(int)(72.0*rand()/(RAND_MAX+1.0));

	return;
        }
      
        
/* calculates the CRC 32 value for a buffer */
unsigned long calculate_crc32(char *buffer, int buffer_size){
	register unsigned long crc;
	int this_char;
	int current_index;

	crc=0xFFFFFFFF;

	for(current_index=0;current_index<buffer_size;current_index++){
		this_char=(int)buffer[current_index];
		crc=((crc>>8) & 0x00FFFFFF) ^ crc32_table[(crc ^ this_char) & 0xFF];
	        }

	return (crc ^ 0xFFFFFFFF);
        }        
        

/* sends all data - thanks to Beej's Guide to Network Programming */
int nrpesendall(int s, char *buf, int *len){
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
int nrperecvall(int s, char *buf, int *len, int timeout){
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
			sleep(1);
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
        
/* show license */
void nrpe_display_license(void){

	_log("This program is released under the GPL (see below) with the additional");
	_log("exemption that compiling, linking, and/or using OpenSSL is allowed.");
	_log("This program is free software; you can redistribute it and/or modify");
	_log("it under the terms of the GNU General Public License as published by");
	_log("the Free Software Foundation; either version 2 of the License, or");
	_log("(at your option) any later version.");
	_log("This program is distributed in the hope that it will be useful,");
	_log("but WITHOUT ANY WARRANTY; without even the implied warranty of");
	_log("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
	_log("GNU General Public License for more details.");
	_log("You should have received a copy of the GNU General Public License");
	_log("along with this program; if not, write to the Free Software");
	_log("Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.");

	return;
        }     
        
/*-------------------------------------------------------------------------
 *
 * Taken from ssltunnel, (C) Alain Thivillon and Hervé Schauer Consultants
 *
 *------------------------------------------------------------------------*/
#ifdef HAVE_SSL
static int ssl_connect_timeout(SSL *ssl, int tmo)
{

  int r=0;
  int rfd, wfd;
  int n,maxfd;
  fd_set rfds, wfds;
  fd_set *prfds;
  struct timeval tv;
  long end;
  int t;
  int errcode;

  rfd = SSL_get_fd(ssl);
  wfd = SSL_get_fd(ssl);
  n = rfd + 1;
  maxfd = (rfd > wfd ? rfd : wfd) + 1;

  prfds = (fd_set *) NULL;
  end = tmo + time( NULL );

  tv.tv_sec = tmo;
  tv.tv_usec = 0;

  FD_ZERO(&wfds);
  FD_SET(wfd,&wfds);

  /* number of descriptors that changes status */
  while (0 < (n = select(n,prfds,&wfds,(fd_set *) 0,&tv)))
  {
    r = SSL_connect(ssl);
    SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);
    if (r > 0) {
      return r;
    }

    switch (errcode=SSL_get_error(ssl, r))
    {
    case SSL_ERROR_WANT_READ:
      prfds = &rfds;
      FD_ZERO(&rfds);
      FD_SET(rfd,&rfds);
      n = maxfd;
      break;
    case SSL_ERROR_WANT_WRITE:
      prfds = (fd_set *) 0;
      n = wfd + 1;
      break;
    default:
      /* some other error */
      switch (errcode) {
        case SSL_ERROR_SSL:
        case SSL_ERROR_SYSCALL:
          // fprintf(stderr,"ssl_connect : %d", SSL_get_error(ssl, r));
           break;
        default:
          // fprintf(stderr,"ssl_connect : %d", SSL_get_error(ssl, r));
           break;
      }
      return -2;
    }

    if ((t = end - time( NULL )) < 0) break;

    tv.tv_sec = t;
    tv.tv_usec = 0;

    FD_ZERO(&rfds);
    FD_SET(rfd,&rfds);
  }

  return -1;

}   

  
static int unblock_socket(int soc)
{
  int   flags =  fcntl(soc, F_GETFL, 0);
  if (flags < 0)
{
      perror("fcntl(F_GETFL)");
      return -1;
    }
  if (fcntl(soc, F_SETFL, O_NONBLOCK | flags) < 0)
    {
      perror("fcntl(F_SETFL,O_NONBLOCK)");
      return -1;
    }
  return 0;
}

static int block_socket(int soc)
{
  int   flags =  fcntl(soc, F_GETFL, 0);
  if (flags < 0)
    {
      perror("fcntl(F_GETFL)");
      return -1;
    }
  if (fcntl(soc, F_SETFL, (~O_NONBLOCK) & flags) < 0)
    {
      perror("fcntl(F_SETFL,~O_NONBLOCK)");
      return -1;
    }
  return 0;
}    
#endif  
#endif











