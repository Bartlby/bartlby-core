AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
DIST_SUBDIRS  = src

bin_PROGRAMS = bartlby bartlby_shmt bartlby_portier
bartlby_SOURCES = src/bartlby.c src/config.c src/global.c src/sched.c src/shm.c src/check.c src/trigger.c src/daemon.c src/replication.c src/perf.c src/downtime.c src/ack.c src/event.c src/snmp.c src/ src/ext.c src/check_active_v2.c src/check_active.c src/check_local.c src/check_group.c src/check_nrpe.c
bartlby_LDADD = -ldl $(OPENSSL_FLAG) $(SNMP_FLAG)
bartlby_CFLAGS = $(AM_CFLAGS)  $(NRPE_ADDON) $(SSL_ADDON)  $(SNMP_ADDON)

bartlby_shmt_SOURCES = src/shm.c src/config.c src/global.c src/tools/shmt.c
bartlby_shmt_LDADD = -ldl $(OPENSSL_FLAG) $(SNMP_FLAG)
bartlby_shmt_CFLAGS = $(AM_CFLAGS) $(NRPE_ADDON) $(SSL_ADDON)  $(SNMP_ADDON)

bartlby_portier_SOURCES =  src/config.c src/global.c src/shm.c src/perf.c src/ext.c src/tools/portier.c
bartlby_portier_LDADD = -ldl $(OPENSSL_FLAG) $(SNMP_FLAG)
bartlby_portier_CFLAGS = $(AM_CFLAGS) $(NRPE_ADDON) $(SSL_ADDON) $(SNMP_ADDON)

lib_LTLIBRARIES = libmysql.la
libmysql_la_SOURCES = src/global.c src/config.c src/libs/mysql.c 
libmysql_la_LIBADD = $(LIB_MYSQL)  

#perfhandler
perfhandlerdir=$(prefix)/perf
perfhandler_DATA=perf/bartlby_files_in_dir perf/bartlby_http perf/bartlby_if perf/bartlby_load perf/bartlby_snmp.sh perf/bartlby_swap.sh

#perfhandler-defaults
perfhandlerdefaultsdir=$(prefix)/perf/defaults
perfhandlerdefaults_DATA=perf/defaults/bartlby_files_in_dir.rrd.xml  perf/defaults/bartlby_http.rrd.xml perf/defaults/bartlby_if.rrd.xml perf/defaults/bartlby_load.rrd.xml perf/defaults/bartlby_snmp.sh.rrd.xml perf/defaults/bartlby_swap.sh.rrd.xml 


#trigger
triggerdir=$(prefix)/trigger
trigger_DATA=trigger/icq.sh trigger/jabber.sh trigger/mail.sh trigger/push.sh trigger/sms.sh

sysconf_DATA=bartlby.cfg bartlby.startup

mysqldir=$(prefix)
mysql_DATA=mysql.shema




install-data-hook:
	 for i in $(perfhandler_DATA) ; do chmod +x $(DESTDIR)$(prefix)/$$i ; done ;
	 for i in $(perfhandlerdefaults_DATA) ; do chmod +x $(DESTDIR)$(prefix)/$$i ; done ;
	 for i in $(trigger_DATA) ; do chmod +x $(DESTDIR)$(prefix)/$$i ; done ;
	 $(MKDIR_P) $(DESTDIR)$(prefix)/contrib
	 $(MKDIR_P) $(DESTDIR)$(prefix)/var/log
	 chmod +x $(DESTDIR)$(prefix)/etc/bartlby.startup
	 


dist_noinst_SCRIPTS = autogen.sh