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

#ifdef HAVE_SSL
#include <bartlby_v2_dh.h>
#endif


static sig_atomic_t conn_timedout = 0;
static unsigned long crc32_table[256];



void agent_v2_generate_crc32_table(void);
void agent_v2_alarm_handler(int sig);
unsigned long agent_v2_calculate_crc32(char *buffer, int buffer_size);
void agent_v2_randomize_buffer(char *buffer,int buffer_size);



void bartlby_check_v2(struct service * svc, char * cfgfile, int use_ssl) {
	
	int sd;
	
#ifdef HAVE_SSL 
	SSL_CTX *ctx;
	SSL *ssl;
#endif     
	u_int32_t packet_crc32;
	u_int32_t calculated_crc32;
	int16_t result;
	int rc;
	agent_v2_packet send_packet;
	agent_v2_packet receive_packet;
	int bytes_to_send;
	int bytes_to_recv;
     
	signal(SIGALRM,agent_v2_alarm_handler);
      /* generate the CRC 32 table */
	agent_v2_generate_crc32_table();
	
#ifdef HAVE_SSL
	if(use_ssl == 1) {
		
		
    if((ctx=SSL_CTX_new(SSLv23_client_method()))==NULL){
			sprintf(svc->new_server_text, "%s", "AgentV2: Error - could not create SSL context.");
       		svc->current_state=STATE_CRITICAL;
       		_log("%s", ERR_error_string(ERR_get_error(), NULL));
		}
		/* use only TLSv1 protocol */
		SSL_CTX_set_options(ctx,SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
	}
#endif

	conn_timedout=0;
	alarm(svc->service_check_timeout);
	result=bartlby_agent_tcp_connect(svc->srv->client_ip,svc->srv->client_port,&sd, svc);
	
	if(conn_timedout == 1) {
		sprintf(svc->new_server_text, "%s", "timed out");
		svc->current_state=STATE_CRITICAL;	
		return;
	}
	if(result != STATE_OK) {
		sprintf(svc->new_server_text, "%s", "connect failed");	
		svc->current_state=STATE_CRITICAL;
		return;
	}
	
#ifdef HAVE_SSL	
	if(use_ssl == 1) {
		/* do SSL handshake */
		if((ssl=SSL_new(ctx))!=NULL){
			SSL_CTX_set_cipher_list(ctx,"ADH");
			SSL_set_fd(ssl,sd);
			
			rc=SSL_connect(ssl);
			
			conn_timedout=0;
			alarm(svc->service_check_timeout);
				
			if(rc !=1){
				sprintf(svc->new_server_text, "%s", "AgentV2: Error - Could not complete SSL handshake.");
					_log("SSL_error: %s", ERR_error_string(ERR_get_error(), NULL));
     		         		svc->current_state=STATE_CRITICAL;
     		         		SSL_CTX_free(ctx);
     		         		return;
			}
		} else {
			sprintf(svc->new_server_text,"AgentV2: Error - Could not create SSL connection structure."); 
			svc->current_state=STATE_CRITICAL;
			_log("%s", ERR_error_string(ERR_get_error(), NULL));
			SSL_CTX_free(ctx);
			close(sd);
			return;
		}
	} 
#endif
	
	
	
	bzero(&send_packet,sizeof(send_packet));
     	
     	/* fill the packet with semi-random data */
     	agent_v2_randomize_buffer((char *)&send_packet,sizeof(send_packet));

     	/* initialize packet data */
	sprintf(send_packet.plugin, "%s", svc->plugin);
	sprintf(send_packet.cmdline, "%s", svc->plugin_arguments);
	sprintf(send_packet.perf_handler, " ");
	
	send_packet.packet_type=(int16_t)htons(AGENT_V2_SENT_PACKET);
	
     	send_packet.crc32_value=(u_int32_t)0L;
     	
     	
	calculated_crc32=agent_v2_calculate_crc32((char *)&send_packet,sizeof(send_packet));
	
	send_packet.crc32_value=(u_int32_t)htonl(calculated_crc32);
     	
	bytes_to_send=sizeof(send_packet);


	conn_timedout=0;
	alarm(svc->service_check_timeout);
#ifdef HAVE_SSL
	if(use_ssl == 1) {
		
		rc=SSL_write(ssl,&send_packet,bytes_to_send);
		
	} else {
#endif
		rc=bartlby_tcp_sendall(sd,(char *)&send_packet,&bytes_to_send);
		
			
#ifdef HAVE_SSL
	}
#endif
	if(conn_timedout == 1) {
		_log("V2: timeout ok");
		sprintf(svc->new_server_text, "%s", "V2 timed out2");
		svc->current_state=STATE_CRITICAL;	
		return;
	}
       if(rc<0)
       	rc=-1;

	if(rc==-1){
		sprintf(svc->new_server_text, "%s", "AgentV2: Error sending to host");
		close(sd);
		svc->current_state=STATE_CRITICAL;
		return;
	}
     	bytes_to_recv=sizeof(receive_packet);

	conn_timedout=0;
	alarm(svc->service_check_timeout);
	
#ifdef HAVE_SSL
	if(use_ssl == 1) {
		
		rc=SSL_read(ssl,(char *)&receive_packet,bytes_to_recv);
		
	} else {
#endif       
       
       rc=bartlby_tcp_recvall(sd,(char *)&receive_packet,&bytes_to_recv,svc->service_check_timeout);
       
#ifdef HAVE_SSL       
	}
#endif

       if(conn_timedout == 1) {
		_log("timeout ok");
		sprintf(svc->new_server_text, "%s", "timed out4");
		svc->current_state=STATE_CRITICAL;	
		return;
	}
	/* reset timeout */
	alarm(0);

#ifdef HAVE_SSL 		
	if(use_ssl == 1) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
	}
#endif

	close(sd);

	if(rc<0){
		sprintf(svc->new_server_text, "%s", "AgentV2: Error receiving data from agent");
		svc->current_state=STATE_CRITICAL;
		return;
	}else if(rc==0){
		sprintf(svc->new_server_text,"%s", "AgentV2: Received 0 bytes from agent");
		svc->current_state=STATE_CRITICAL;
		return;
	}else if(bytes_to_recv<sizeof(receive_packet)){
		sprintf(svc->new_server_text, "AgentV2: Receive underflow - only %d bytes received (%ld expected).\n",bytes_to_recv,(unsigned long)sizeof(receive_packet));
		svc->current_state=STATE_CRITICAL;
		return;
	}
           
	packet_crc32=ntohl(receive_packet.crc32_value);
	receive_packet.crc32_value=0L;
	calculated_crc32=agent_v2_calculate_crc32((char *)&receive_packet,sizeof(receive_packet));
	if(packet_crc32!=calculated_crc32){
		sprintf(svc->new_server_text,"%s", "AgentV2: Response packet had invalid CRC32.");
		svc->current_state=STATE_CRITICAL;
		close(sd);
		return;
	}
	
	

	
	/* check packet type */
	if(ntohs(receive_packet.packet_type)!=AGENT_V2_RETURN_PACKET){
		sprintf(svc->new_server_text,"%s","AgentV2: Invalid packet type received from server.");
		close(sd);
		svc->current_state=STATE_CRITICAL;
		return;
	}
	
	/* get the return code from the remote plugin */
	result=(int16_t)receive_packet.exit_code;
	
	/* print the output returned by the daemon */
	receive_packet.output[2048-1]='\x0';
	if(!strcmp(receive_packet.output,"")) {
		sprintf(svc->new_server_text,"%s","AgentV2: No output returned from agent");
	} else {
		sprintf(svc->new_server_text,"%s",receive_packet.output);
	}

	switch ((int16_t)receive_packet.exit_code) {
		case STATE_OK:
		case STATE_WARNING:
		case STATE_CRITICAL:
		case STATE_INFO:
			svc->current_state=(int16_t)receive_packet.exit_code;
		break;
		
		default:
		
		svc->current_state=STATE_CRITICAL;
				
	}
	
	if(strlen(receive_packet.perf_handler) > 6) {
		bartlby_perf_track(svc,receive_packet.perf_handler, strlen(receive_packet.perf_handler), cfgfile);
		//_log("perf: %s", receive_packet.perf_handler);
	}
                    
     
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

void agent_v2_alarm_handler(int sig){

        conn_timedout = 1;
        //_log("FIXME: nrpe timeout SSL_connect");
        
       
       
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
        



