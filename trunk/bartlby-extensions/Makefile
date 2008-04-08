BARTLBY_BASE=../bartlby-core/
include ${BARTLBY_BASE}/Makefile.conf

HELLOWORLD = ${BARTLBY_BASE}/src/global.o  ${BARTLBY_BASE}/src/config.o helloworld.o
WATCHDOG = ${BARTLBY_BASE}/src/global.o  ${BARTLBY_BASE}/src/config.o watchdog.o

NAGIOS_NSC = ${BARTLBY_BASE}/src/shm.o ${BARTLBY_BASE}/src/global.o  ${BARTLBY_BASE}/src/config.o nsc.o


EXTLOGGER = ${BARTLBY_BASE}/src/shm.o  ${BARTLBY_BASE}/src/global.o  ${BARTLBY_BASE}/src/config.o extlogger.o

DISTRIBUTIVE = ${BARTLBY_BASE}/src/shm.o ${BARTLBY_BASE}/src/global.o  ${BARTLBY_BASE}/src/config.o distributive.o

.SUFFIXES: .c .S .o

.c.o:

	$(CC) $(INCLUDE_MYSQL) $(EXTRAOPTIONS) $(INCLUDE_LL) -I${BARTLBY_BASE}/include -c $<


all:   helloworld nagios_nsc extlogger distributive watchdog

helloworld: ${HELLOWORLD}
	$(LNK)   $(DYNLINK)  $(EXTRAOPTIONS) $(INCLUDE_LL) -o helloworld.so ${HELLOWORLD}
	
nagios_nsc: ${NAGIOS_NSC}
	$(LNK)   $(DYNLINK)  $(EXTRAOPTIONS) $(INCLUDE_LL) -o nsc.so ${NAGIOS_NSC}

distributive: ${DISTRIBUTIVE}
	$(LNK)   $(DYNLINK)  $(EXTRAOPTIONS) $(INCLUDE_LL) -o distributive.so ${DISTRIBUTIVE}

extlogger: ${EXTLOGGER}
	$(LNK)  $(INCLUDE_MYSQL) $(LMYSQL) $(DYNLINK)  $(EXTRAOPTIONS) $(INCLUDE_LL) -o extlogger.so ${EXTLOGGER}
	
	
watchdog: ${WATCHDOG}
	$(LNK)  $(INCLUDE_MYSQL) $(LMYSQL) $(DYNLINK)  $(EXTRAOPTIONS) $(INCLUDE_LL) -o watchdog.so ${WATCHDOG}

install: all
	$(MKDIRP)  ${BARTLBY_HOME}/ext/
	$(CPPVA) -m 777 nsc.so ${BARTLBY_HOME}/ext/nsc.so
	$(CPPVA) -m 777 extlogger.so ${BARTLBY_HOME}/ext/extlogger.so
	$(CPPVA) -m 777 README_EXTLOGGER.txt ${BARTLBY_HOME}/ext/
	
	
	$(CPPVA) -m 777 watchdog.so ${BARTLBY_HOME}/ext/watchdog.so
	$(CPPVA) -m 777 README_WATCHDOG.txt ${BARTLBY_HOME}/ext/


	



clean:
	$(RMVFR) *.o 

