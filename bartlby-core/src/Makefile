include ../Makefile.conf

OBJS = bartlby.o config.o global.o sched.o shm.o check.o trigger.o daemon.o replication.o perf.o downtime.o ack.o event.o snmp.o  ext.o check_active_v2.o check_active.o check_local.o check_group.o check_nrpe.o

.SUFFIXES: .c .S .o

.c.o:

	$(CC)  $(EXTRAOPTIONS)  $(INCLUDE_L)   -c $<

all:    bartlby


install: all
	$(CPPVA) bartlby ${BARTLBY_HOME}/bin/
	$(CPPVA) ../mysql.shema ${BARTLBY_HOME}/
	
	

bartlby: ${OBJS}
	@echo "[ Building binary 'bartlby']";
	$(LNK)   $(EXTRAOPTIONS) $(OPENSSL_ADDON) $(SNMP_ADDON) $(INCLUDE_L) $(LDL)  -o bartlby ${OBJS}







clean:
	$(RMVFR) *.o 
