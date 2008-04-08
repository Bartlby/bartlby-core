include Makefile.conf
 
TCPPLG = bartlby_tcp.o
LOADPLG = bartlby_load.o
HTTPPLG =  bartlby_check_http.o

TARGETS = bartlby_http bartlby_tcp

.SUFFIXES: .c .S .o

.c.o:

	$(CC) $(EXTRAOPTIONS)  $(INCLUDE_L)   -Wall -g -c $<

all: ${TARGETS}

bartlby_tcp: ${TCPPLG}

	$(CC) $(EXTRAOPTIONS)  $(INCLUDE_L)  -o bartlby_tcp ${TCPPLG}



bartlby_http: ${HTTPPLG}

	$(CC) $(EXTRAOPTIONS)  $(INCLUDE_L)  -o bartlby_http ${HTTPPLG}




	

install: all
	$(MKDIRP) ${BARTLBY_HOME}/;
	
	$(CPPVA) -m 777 bartlby.funcs $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_tcp $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_check_ilog $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_load $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_http $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_disk.sh $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_dns.sh $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_if $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_process.sh $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_snmp.sh $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_swap.sh $(BARTLBY_HOME)/;
	$(CPPVA) -m 777 bartlby_files_in_dir $(BARTLBY_HOME)/;
	

clean:
	rm -fv *.o 
