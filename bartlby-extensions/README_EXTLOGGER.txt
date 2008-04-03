the 'extlogger' extensions logs the state and its duration to a mysql 
for quicker report generation this could be very usefull

you have to do the following

create a mysql db (or use the same from bartlby)
create a table:


	CREATE TABLE `log` (
  		`service_id` int(11) NOT NULL default '0',
  		`starttime` int(11) NOT NULL default '0',
  		`endtime` int(11) NOT NULL default '0',
  		`startstate` int(11) NOT NULL default '0',
  		`endstate` int(11) NOT NULL default '0',
  		`startmsg` text NOT NULL,
  		`endmsg` text NOT NULL
	)




after that load the extension in the bartlby core by:

	extension_count=1
	extension[1]=PATH/TO/EXTENSIONS/extlogger.so




set a few config variables for 'extlogger':
	
	extlogger_type=mysql
	extlogger_host=localhost
	extlogger_user=bartlby
	extlogger_password=mysql2pw2bart4
	extlogger_db=bartlby_log
