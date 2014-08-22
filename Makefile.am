GIT_DESCR = $(shell sh -c 'git describe --abbrev=4 --always | sed s/^v//')
GIT_VERSION = -D__GIT_VERSION=\"$(GIT_DESCR)\"
AUTOMAKE_OPTIONS = subdir-objects 
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

noinst_HEADERS = include/bartlby.h

CORE_SOURCES=src/config.c src/global.c src/sched.c src/shm.c src/check.c src/trigger.c src/daemon.c src/replication.c src/perf.c src/downtime.c src/ack.c src/event.c src/check_snmp.c src/ src/ext.c src/check_active_v2.c src/check_active.c src/check_local.c src/check_group.c src/check_nrpe.c src/check_ssh.c src/notification_log.c




EXTRA_DIST = src


noinst_LIBRARIES = libbartlby.a
libbartlby_a_SOURCES =  $(CORE_SOURCES)
libbartlby_a_CFLAGS = $(AM_CFLAGS)  $(NRPE_ADDON) $(OPENSSL_ADDON)  $(SNMP_ADDON) $(SSH_ADDON) $(GIT_VERSION) -g -fPIC


bin_PROGRAMS = bartlby bartlby_shmt bartlby_portier
nodist_bartlby_SOURCES = src/bartlby.c


bartlby_LDADD = -ldl  libbartlby.a  $(OPENSSL_FLAG) $(SNMP_FLAG) $(SSH_FLAG) 
bartlby_LDFLAGS = -pthread
bartlby_CFLAGS = $(AM_CFLAGS)  $(NRPE_ADDON) $(OPENSSL_ADDON)  $(SNMP_ADDON) $(SSH_ADDON) $(GIT_VERSION) -g




nodist_bartlby_shmt_SOURCES = src/tools/shmt.c
bartlby_shmt_LDADD = -ldl libbartlby.a $(OPENSSL_FLAG) $(SNMP_FLAG) $(SSH_FLAG) 
bartlby_shmt_CFLAGS = $(AM_CFLAGS) $(NRPE_ADDON) $(SSL_ADDON)  $(SNMP_ADDON) $(SSH_ADDON) $(GIT_VERSION)
bartlby_shmt_LDFLAGS =  -Wl,--no-as-needed

nodist_bartlby_portier_SOURCES =  src/tools/portier.c
bartlby_portier_LDADD = -ldl libbartlby.a $(OPENSSL_FLAG) $(SNMP_FLAG) $(SSH_FLAG) 
bartlby_portier_LDFLAGS = -pthread -Wl,--no-as-needed
bartlby_portier_CFLAGS = $(AM_CFLAGS) $(NRPE_ADDON) $(SSL_ADDON) $(SNMP_ADDON) $(SSH_ADDON) $(GIT_VERSION)

lib_LTLIBRARIES = mysql.la
nodist_mysql_la_SOURCES =  src/libs/mysql.c 
mysql_la_LIBADD = $(LIB_MYSQL)  
mysql_la_LDFLAGS = -avoid-version -module -Wl,--export-dynamic  -Wl,--whole-archive,libbartlby.a,--no-whole-archive


#perfhandler
perfhandlerdir=$(prefix)/perf
dist_perfhandler_DATA=perf/bartlby_generic.sh  perf/bartlby_http perf/bartlby_if perf/bartlby_load perf/bartlby_snmp.sh perf/bartlby_swap.sh
#dist_perfhandler_DATA=perf/bartlby_http perf/bartlby_if perf/bartlby_load perf/bartlby_snmp.sh perf/bartlby_swap.sh

#perfhandler-defaults
perfhandlerdefaultsdir=$(prefix)/perf/defaults
dist_perfhandlerdefaults_DATA=perf/defaults/bartlby_http.rrd.xml perf/defaults/bartlby_if.rrd.xml perf/defaults/bartlby_load.rrd.xml perf/defaults/bartlby_snmp.sh.rrd.xml perf/defaults/bartlby_swap.sh.rrd.xml 


#trigger
triggerdir=$(prefix)/trigger
dist_trigger_DATA=trigger/icq.sh trigger/jabber.sh trigger/mail.sh trigger/push.sh trigger/sms.sh



mysqldir=$(prefix)
dist_mysql_DATA=mysql.shema

inst_startupdir=$(prefix)/etc
nodist_inst_startup_DATA=bartlby.startup

cfgdir=$(prefix)/etc
dist_noinst_cfg=bartlby.cfg


MAINTAINERCLEANFILES = Makefile.in aclocal.m4 compile \
            config.guess config.sub depcomp \
            install-sh ltmain.sh missing \
            include/config.h.in include/config.h.in~ configure m4/libtool.m4 m4/ltoptions.m4 \
            m4/ltsugar.m4 m4/ltversion.m4 m4/lt~obsolete.m4


install-data-hook:
	@list='$(dist_noinst_cfg)'; for p in $$list; do \
	    dest=`echo $$p`; \
	    if test -f $(DESTDIR)$(sysconfdir)/$$dest; then \
		echo "$@ will not overwrite existing $(DESTDIR)$(sysconfdir)/$$dest"; \
	    else \
		echo " $(INSTALL_DATA) $$p $(DESTDIR)$(sysconfdir)/$$dest"; \
		$(INSTALL_DATA) $$p $(DESTDIR)$(sysconfdir)/$$dest; \
	    fi; \
	done
	 for i in $(dist_perfhandler_DATA) ; do chmod +x $(DESTDIR)$(prefix)/$$i ; done ;
	 for i in $(dist_perfhandlerdefaults_DATA) ; do chmod +x $(DESTDIR)$(prefix)/$$i ; done ;
	 for i in $(dist_trigger_DATA) ; do chmod +x $(DESTDIR)$(prefix)/$$i ; done ;
	 $(MKDIR_P) $(DESTDIR)$(prefix)/contrib
	 $(MKDIR_P) $(DESTDIR)$(prefix)/var/log
	 chmod +x $(DESTDIR)$(prefix)/etc/bartlby.startup
	 
dist-hook:
	make clean
	rm -rf `find $(distdir) -type f -name \*.o -print` 
	rm -rf `find $(distdir) -type f -name \*.dirstamp -print` 
	rm -rf `find $(distdir) -type d -name .deps -print` 
	rm -rf `find $(distdir) -type d -name .libs -print` 

dist_noinst_SCRIPTS = autogen.sh

