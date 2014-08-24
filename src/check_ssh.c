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




#ifndef SSH_KEYTYPE_RSA
#define SSH_KEYTYPE_RSA 2
#endif

#ifndef SSH_ADDON
void bartlby_check_ssh(struct service * svc, char * cfgfile) {
	sprintf(svc->new_server_text, "%s", "SSH Support ist not compiled in (--enable-ssh=yes) bartlby-core");
	svc->current_state = STATE_CRITICAL;
}

#else

void bartlby_check_ssh(struct service * svc, char * cfgfile) {
	
	ssh_session my_ssh_session;
  ssh_channel channel;
  ssh_private_key pkey;
	ssh_public_key pubkey;
	ssh_string pubstring;
	
	
	int rc;
	int state;	
	int nbytes;
	int auth;
	char buffer[2048];
	char buffer2[2048];
	char rmessage[2048];
	int bytes_read;
	
	int log=9;
	char * cmd_line;
	
	
	
	my_ssh_session = ssh_new();
	if (my_ssh_session == NULL) {
		sprintf(svc->new_server_text, "connect failed");
    goto safe_free;
		return;
  }
 
  ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, svc->srv->client_ip);
  ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &svc->srv->client_port);
  
 	//ssh_options_set(my_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &log);
  ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, svc->srv->server_ssh_username);
  
  
 	rc = ssh_connect(my_ssh_session);
  if (rc != SSH_OK)   {
   	sprintf(svc->new_server_text, "Connect to server failed username: '%s'", svc->srv->server_ssh_username);
		svc->current_state = STATE_CRITICAL;
    goto safe_free;
		return;
  }
   ssh_set_blocking(my_ssh_session, 1);
   pkey=privatekey_from_file(my_ssh_session, svc->srv->server_ssh_keyfile,SSH_KEYTYPE_RSA, svc->srv->server_ssh_passphrase);  //ONLY RSA KEYS WILL WORK
   if(pkey == NULL) {
   	 	sprintf(svc->new_server_text, "failed to load private key '%s'", svc->srv->server_ssh_keyfile);
			svc->current_state = STATE_CRITICAL;
      goto safe_free;
			return;
	 }
   pubkey=publickey_from_privatekey(pkey);
   if(pubkey == NULL) {
   		sprintf(svc->new_server_text, "could not get public key from private key '%s'", svc->srv->server_ssh_keyfile);
			svc->current_state = STATE_CRITICAL;
      goto safe_free;
			return;
	 }
   pubstring = publickey_to_string(pubkey);
   if(pubstring == NULL) {
   		sprintf(svc->new_server_text, "could not convert public key to string from private key '%s'", svc->srv->server_ssh_keyfile);
			svc->current_state = STATE_CRITICAL;
      goto safe_free;
			return;
	 	
   }
   rc=ssh_userauth_pubkey(my_ssh_session, NULL, pubstring, pkey);
   //rc=ssh_userauth_try_publickey(my_ssh_session, NULL, pubkey);
   privatekey_free(pkey);
	 string_free(pubstring);
   
   
   if(rc != SSH_AUTH_SUCCESS) {
   		sprintf(svc->new_server_text, "authentication failed using private key '%s' user: '%s' return code: %d error-msg: %s", svc->srv->server_ssh_keyfile, svc->srv->server_ssh_username, rc, ssh_get_error(my_ssh_session));
			svc->current_state = STATE_CRITICAL;
      goto safe_free;
			return;
	 	
   }
   
   channel = channel_new(my_ssh_session);;
   
    if (channel == NULL) {
        
        sprintf(svc->new_server_text, "SSH Channel open failed");
				svc->current_state = STATE_CRITICAL;
        goto safe_free;
				return;
	 }

    rc = channel_open_session(channel);
    if (rc < 0) {
    		   		
        goto failed;
    }
		asprintf(&cmd_line, "~/bartlby-plugins/%s %s", svc->plugin, svc->plugin_arguments);
    rc = channel_request_exec(channel, cmd_line);
    free(cmd_line);
    if (rc < 0) {
        goto failed;
    }
		sprintf(buffer2, "");
	memset(buffer, '\0', sizeof(char)*(1024*2));
	memset(buffer2, '\0', sizeof(char)*(1024*2));
    bytes_read=0;
    nbytes = channel_read(channel, buffer, sizeof(buffer), 0);
    
    while (nbytes > 0) {
      
      bytes_read += nbytes;
      if(strlen(buffer2)+strlen(buffer) <= 2044) { //0-127|
        strcat(buffer2, buffer);
        nbytes = channel_read(channel, buffer, sizeof(buffer), 0);
      } else {
        //_log("BUFFER BUFFER");
        break;        
      }
    }

    if (nbytes < 0) {
        goto failed;
    }
    
  	

		
    channel_send_eof(channel);
    rc=channel_get_exit_status(channel);
    
    
    channel_close(channel);
   
    
    
    channel_free(channel);
    ssh_free(my_ssh_session);
		
		
  	bartlby_check_grep_perf_line(buffer2, svc, cfgfile);

	memset(rmessage, '\0', sizeof(char)*(1024*2));
  	sprintf(rmessage, "%d|%s", rc, buffer2); 
   
    bartlby_action_handle_reply(svc, rmessage, cfgfile);
  	
		
		
  
  	return;
 

safe_free:
  if(channel != NULL) channel_close(channel);
  if(channel != NULL) channel_free(channel);
  if(my_ssh_session != NULL) ssh_disconnect(my_ssh_session);
  if(my_ssh_session != NULL) ssh_free(my_ssh_session);
  return;

failed:
		channel_close(channel);
    channel_free(channel);
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    
    sprintf(svc->new_server_text, "SSH Communication failed");
		svc->current_state = STATE_CRITICAL;
		return;
}

#endif













