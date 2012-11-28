include Makefile.conf

AGENT = config.o  agent.o
AGENT_V = config.o  agent_v2.o global.o 

CMD = config.o cmd.o global.o

.c.o:

	$(CC) $(EXTRAOPTIONS)  $(INCLUDE_L)   -c $<


all: agent cmd agent_v

cmd: ${CMD}
	$(CC)  -I. $(OPENSSL_ADDON) $(EXTRAOPTIONS) $(INCLUDE_LL)   -o bartlby_cmd ${CMD}
	
agent_v: ${AGENT_V} 
	$(CC)  -I. $(OPENSSL_ADDON) $(EXTRAOPTIONS) $(INCLUDE_LL)   -o bartlby_agent_v2 ${AGENT_V}
	
agent: ${AGENT} 
	$(CC)   -I.  $(EXTRAOPTIONS) $(INCLUDE_LL)   -o bartlby_agent ${AGENT}


install:
	$(MKDIRP) $(BARTLBY_HOME);
	$(MKDIRP) $(PLUGIN_DIR);
	$(CPPVA) bartlby_agent_v2 $(BARTLBY_HOME)/
	$(CPPVA) bartlby_agent $(BARTLBY_HOME)/
	$(CPPVA) bartlby_cmd ${BARTLBY_HOME}/
	
	$(CPPVA) bartlby.cfg $(BARTLBY_HOME)/
	$(CPPVA) agent_sync.cfg $(BARTLBY_HOME)/
	$(CPPVA) agent_sync.sh $(BARTLBY_HOME)/
	
	$(CPPVA) passive_run.sample $(BARTLBY_HOME)/
	$(CPPVA) HOWTO.passive $(BARTLBY_HOME)/
	



clean:
	$(RMVFR) *.o 
