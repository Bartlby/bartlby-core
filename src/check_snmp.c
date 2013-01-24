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





#ifndef SNMP_ADDON
void bartlby_check_snmp(struct service * svc, char * cfgfile) {
	sprintf(svc->new_server_text, "%s", "SNMP Support ist not compiled in (--enable-snmp=yes) bartlby-core");
	svc->current_state = STATE_CRITICAL;
}

#else

void bartlby_check_snmp(struct service * svc, char * cfgfile) {
	
	struct snmp_session session, *ss;
	struct snmp_pdu *pdu;
	struct snmp_pdu *response;
	char *err;
	
	oid anOID[MAX_OID_LEN];
	size_t anOID_len = MAX_OID_LEN;
	
	struct variable_list *vars;
	int status;
	char buf[1024];
	
	
	
	init_snmp("snmpapp");
	snmp_sess_init( &session );                   
	session.peername = strdup(svc->srv->client_ip);	
	if(svc->snmp_info.version == 1) {
		session.version = SNMP_VERSION_1;
	} else if(svc->snmp_info.version == 2) {
		session.version = SNMP_VERSION_2c;	
	} else {
		sprintf(svc->new_server_text, "Unkown SNMP version");
		svc->current_state=STATE_CRITICAL;
	}
	
	session.community = (unsigned char *)strdup(svc->snmp_info.community);
	session.community_len = strlen((char *)session.community);
	SOCK_STARTUP;
	ss = snmp_open(&session);
	
	if (!ss) {
	    snmp_error(ss, NULL, NULL, &err);
	    snprintf(svc->new_server_text, 1000, "%s", err);
	    svc->current_state=STATE_CRITICAL;
	    return;
	}
	pdu = snmp_pdu_create(SNMP_MSG_GET);
	if(!snmp_parse_oid(svc->snmp_info.objid, anOID, &anOID_len)) {
			sprintf(svc->new_server_text, "invalid object identifier(%s)", svc->snmp_info.objid);
			svc->current_state=STATE_CRITICAL;
			
			
    			snmp_close(ss);
			SOCK_CLEANUP;
			return;
	}
	
	snmp_add_null_var(pdu, anOID, anOID_len);
	status = snmp_synch_response(ss, pdu, &response);
    
	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
    		/*
    			Logical part
    		
    		*/
    		vars = response->variables;
    		switch(vars->type) {
    			
    			case ASN_INTEGER:
    			case ASN_TIMETICKS:
    			case ASN_COUNTER:
    				snprint_value(buf, sizeof(buf), vars->name, vars->name_length, vars);
    				sprintf(svc->new_server_text, "Output %s - compare against: %ld", buf, *vars->val.integer);
						svc->current_state=STATE_OK;
						
				
						switch(svc->snmp_info.type) {
							case SNMP_CHECK_TYPE_LOWER: 
								if(*vars->val.integer < svc->snmp_info.crit) {
										svc->current_state=STATE_CRITICAL;
								}
								if(*vars->val.integer < svc->snmp_info.warn) {
										svc->current_state=STATE_WARNING;
								}
							break;
							case SNMP_CHECK_TYPE_GREATER:
								if(*vars->val.integer > svc->snmp_info.crit) {
										svc->current_state=STATE_CRITICAL;
								}
								if(*vars->val.integer > svc->snmp_info.warn) {
										svc->current_state=STATE_WARNING;
								}
							break;
							case SNMP_CHECK_TYPE_EQ:
								if(*vars->val.integer != svc->snmp_info.warn) {
									svc->current_state=STATE_WARNING;
								} 
								if(*vars->val.integer != svc->snmp_info.crit) {
									svc->current_state=STATE_CRITICAL;
								} 
								
									
							break;
							case SNMP_CHECK_TYPE_NOT_EQ:
								if(*vars->val.integer == svc->snmp_info.warn) {
									svc->current_state=STATE_WARNING;
								} 
								if(*vars->val.integer == svc->snmp_info.crit) {
									svc->current_state=STATE_CRITICAL;
								} 
								
									
							break;
							default:
									sprintf(svc->new_server_text, "Integer return %ld - does not support 'contain'", *vars->val.integer);
									svc->current_state=STATE_CRITICAL;
							break;
							
							
						}
				
	
    			break;
    			
    			default:	
    				snprint_value(buf, sizeof(buf), vars->name, vars->name_length, vars);
    				svc->current_state=STATE_OK;
    				switch(svc->snmp_info.type) {
    					case SNMP_CHECK_TYPE_EQ:
    						if(strcmp(buf, svc->snmp_info.textmatch) != 0) {
    							svc->current_state=STATE_CRITICAL;
    						}
    					break;
    					case SNMP_CHECK_TYPE_NOT_EQ:
    						if(strcmp(buf, svc->snmp_info.textmatch) == 0) {
    							svc->current_state=STATE_CRITICAL;
    						}
    					break;
    					case SNMP_CHECK_TYPE_CONTAINS:
    						
    						if(strstr(buf, svc->snmp_info.textmatch) == NULL) {
    							svc->current_state=STATE_CRITICAL;
    							
    						}
    					break;
    					default:
    						//INTEGER CHECK ON text Output
    						svc->current_state=STATE_CRITICAL;
    						sprintf(buf, "String return and integer check!");	
    					break;
    				}
    				
    				sprintf(svc->new_server_text, "%s - match against: '%s' - Type: %d", buf, svc->snmp_info.textmatch, svc->snmp_info.type);
						
    		}
    		
    		
    		
		
		if (response) {
      		snmp_free_pdu(response);
      	}
    		snmp_close(ss);
		SOCK_CLEANUP;
    		
		
		return;
	} else if (status == STAT_TIMEOUT) {
		sprintf(svc->new_server_text, "no response from: %s", session.peername);
		svc->current_state=STATE_CRITICAL;
		
		if (response) {
      		snmp_free_pdu(response);
      	}
    		snmp_close(ss);
		SOCK_CLEANUP;
		return;	
	} else {
		sprintf(svc->new_server_text, "%s", "an SNMP error Occured");
		svc->current_state=STATE_CRITICAL;
		if (response) {
      		snmp_free_pdu(response);
      	}
    		snmp_close(ss);
		SOCK_CLEANUP;	
	}
    
    

	
	
	
}

#endif













