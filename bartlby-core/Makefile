include Makefile.conf


SUBDIRS = src/ src/libs/ src/tools/
TRIGGERS = icq.sh jabber.sh mail.sh
PERFHANDLERS = bartlby_http bartlby_if bartlby_load bartlby_snmp.sh bartlby_swap.sh bartlby_files_in_dir


DIRR = bartlby-core bartlby-plugins bartlby-php bartlby-ui bartlby-docs

all:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && make all); \
	done
	
	
	
install: all
	$(MKDIRP) ${BARTLBY_HOME}/bin/
	$(MKDIRP) ${BARTLBY_HOME}/etc/
	$(MKDIRP) ${BARTLBY_HOME}/lib/
	$(MKDIRP) ${BARTLBY_HOME}/var/
	$(MKDIRP) ${BARTLBY_HOME}/perf/
	$(MKDIRP) ${BARTLBY_HOME}/perf/defaults/
	$(MKDIRP) ${BARTLBY_HOME}/trigger/
	$(MKDIRP) ${BARTLBY_HOME}/ext/
	$(MKDIRP) ${BARTLBY_HOME}/contrib/
	$(MKDIRP) ${BARTLBY_HOME}/var/log/
	$(CPPVA) bartlby.cfg ${BARTLBY_HOME}/etc/
	$(CPPVA) config.log ${BARTLBY_HOME}/var/log/
	
	$(CPPVA) -m 777 bartlby.startup ${BARTLBY_HOME}/etc/
	
	list='$(TRIGGERS)'; for trigger in $$list; do \
	  $(CPPVA) -m 777 trigger/$$trigger ${BARTLBY_HOME}/trigger/; \
	done		
	
	list='$(PERFHANDLERS)'; for perfhandler in $$list; do \
	  $(CPPVA) -m 777 perf/$$perfhandler ${BARTLBY_HOME}/perf/; \
	  $(CPPVA) perf/defaults/$$perfhandler.rrd.xml ${BARTLBY_HOME}/perf/defaults/; \
	done		
		
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && make install); \
	done

	. ./post_compile.sh
clean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && make clean); \
	done
changelog:
	../make_changelog
	
	
	
cvs-clean:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && make clean); \
	done
	$(RMVFR) *.so
	$(RMVFR) bartlby
	$(RMVFR) shmt
	$(RMVFR) bartlby_shmt
	$(RMVFR) bartlby_cmd
	$(RMVFR) bartlby_portier
	$(RMVFR) *2006*
	$(RMVFR) config.status
	$(RMVFR) config.log
	$(RMVFR) autom4te.cache/
	$(RMVFR) bartlby.cperf
	$(RMVFR) bartlby.cfg
	$(RMVFR) Makefile.conf
	
	
	
	
	
	
cvs: cvs-clean
	cvs commit;
	
cvs-single: cvs-clean
		
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  for x in $$subdir*.c; do \
	  	cvs commit $$x; \
	  done \
	done
	
sf-release: 
	../makedist
	echo "goto admin page and add release"

website: changelog 
	
	scp ../CHANGELOG hjanuschka@shell.sourceforge.net:/home/users/h/hj/hjanuschka/bartlby/htdocs/ChangeLog
	list='$(DIRR)'; for subdir in $$list; do \
	  for x in $$subdir; do \
	  	scp /storage/SF.NET/BARTLBY/$$x/ChangeLog hjanuschka@shell.sourceforge.net:/home/users/h/hj/hjanuschka/bartlby/htdocs/ChangeLog-$$x; \
	  done \
	done
	date > /storage/SF.NET/BARTLBY/lastMod
	scp /storage/SF.NET/BARTLBY/lastMod hjanuschka@shell.sourceforge.net:/home/users/h/hj/hjanuschka/bartlby/htdocs/lastMod

