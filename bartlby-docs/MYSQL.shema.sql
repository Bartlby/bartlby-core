--$Id: MYSQL.shema.sql,v 1.3 2005/09/25 16:45:04 hjanuschka Exp $
--$Revision: 1.3 $                                           
--$Source: /cvsroot/bartlby/bartlby-docs/MYSQL.shema.sql,v $      


--$Log: MYSQL.shema.sql,v $
--Revision 1.3  2005/09/25 16:45:04  hjanuschka
--database layout changes ;-)
--
--Revision 1.2  2005/09/15 00:30:31  hjanuschka
--added a simple copy&paste installtion documentation :-)
--
--Revision 1.1  2005/09/14 22:41:26  hjanuschka
--install docu try out
--

drop database bartlby;
--create the database
create database bartlby;
use bartlby;

--create services table 
CREATE TABLE services (
  service_id int(12) NOT NULL auto_increment,
  server_id int(12) NOT NULL default '0',
  service_plugin varchar(255) NOT NULL default '',
  service_name varchar(255) NOT NULL default '',
  service_state int(12) NOT NULL default '0',
  service_text varchar(255) NOT NULL default '',
  service_args varchar(255) NOT NULL default '',
  service_last_check datetime NOT NULL default '0000-00-00 00:00:00',
  service_notify int(2) NOT NULL default '0',
  service_active int(2) NOT NULL default '1',
  service_current int(2) NOT NULL default '0',
  service_flapping datetime NOT NULL default '0000-00-00 00:00:00',
  service_time_from time NOT NULL default '00:00:00',
  service_time_to time NOT NULL default '00:00:00',
  service_interval int(255) NOT NULL default '1',
  service_type int(11) NOT NULL default '1',
  service_var varchar(255) default NULL,
  service_passive_timeout int(11) NOT NULL default '100',
  PRIMARY KEY  (service_id),
  KEY service_id (service_id),
  KEY service_id_2 (service_id,server_id)
);


--create servers table

CREATE TABLE servers (
  server_id int(12) NOT NULL auto_increment,
  server_ip varchar(255) NOT NULL default '',
  server_name varchar(255) NOT NULL default '',
  server_ico varchar(255) NOT NULL default '',
  server_enabled int(2) NOT NULL default '1',
  server_port int(255) NOT NULL default '9030',
  PRIMARY KEY  (server_id),
  UNIQUE KEY server_id (server_id)
);

--create workers table

CREATE TABLE workers (
  worker_id int(12) NOT NULL auto_increment,
  worker_name varchar(255) NOT NULL default '',
  worker_mail varchar(255) NOT NULL default '',
  worker_icq int(20) NOT NULL default '0',
  worker_active int(2) NOT NULL default '0',
  password varchar(255) NOT NULL default '',
  status varchar(255) NOT NULL default '',
  enabled_services varchar(255) NOT NULL default '',
  notify_levels varchar(50) NOT NULL default '',
  enabled_triggers varchar(50) NOT NULL default '|mail.sh|',
  PRIMARY KEY  (worker_id)
);

insert into workers(worker_name, worker_mail,password) VALUES('admin', 'bartlby-default@januschka.com', 'password');