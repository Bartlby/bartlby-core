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
#ifndef __APPLE__
#include <malloc.h>
#endif
#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>


#ifdef HAVE_SSL
#include <openssl/dh.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#endif

extern unsigned long crc32_table[256];

/* calculates the CRC 32 value for a buffer */
unsigned long agent_v2_calculate_crc32(char *buffer, int buffer_size){
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
        
void agent_v2_randomize_buffer(char *buffer,int buffer_size){
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
      

void agent_v2_generate_crc32_table(void){
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
int portier_my_connect(char *host_name,int port,int *sd,char *proto){
	struct sockaddr_in servaddr;
	struct hostent *hp;
	struct protoent *ptrp;
	int result;

	bzero((char *)&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);

	hp=gethostbyname((const char *)host_name);
	if(hp==NULL){
		printf("Invalid host name '%s'\n",host_name);
		exit(2);
	}

	memcpy(&servaddr.sin_addr,hp->h_addr,hp->h_length);
	

	/* map transport protocol name to protocol number */
	if(((ptrp=getprotobyname(proto)))==NULL){
		printf("Cannot map \"%s\" to protocol number\n",proto);
		exit(2);
	}

	/* create a socket */
	*sd=socket(PF_INET,(!strcmp(proto,"udp"))?SOCK_DGRAM:SOCK_STREAM,ptrp->p_proto);
	if(*sd<0){
		printf("Socket creation failed\n");
		exit(2);
	}

	/* open a connection */
	result=connect(*sd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	if(result<0){
		switch(errno){  
		case ECONNREFUSED:
			printf("Connection refused by host\n");
		break;
		case ETIMEDOUT:
			printf("Timeout while attempting connection\n");
			
		break;
		case ENETUNREACH:
			printf("Network is unreachable\n");
			
		break;
		default:
			printf("Connection refused or timed out\n");
			
		}

		exit(2);
	 }

	return 0;
}

int portier_my_tcp_connect(char *host_name,int port,int *sd){
	int result;

	result=portier_my_connect(host_name,port,sd,"tcp");

	return result;
}



        
        

