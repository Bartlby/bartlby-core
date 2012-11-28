include ../../Makefile.conf


MYSQL = ../global.o ../config.o mysql.o



.SUFFIXES: .c .S .o

.c.o:

	$(CC) $(EXTRAOPTIONS)  $(INCLUDE_MYSQL) $(INCLUDE_LL) -c $<


all:   mysql

mysql: ${MYSQL}
	@echo "[ Building binary 'mysql.so']";
	$(LNK)  $(INCLUDE_MYSQL) $(LMYSQL) $(DYNLINK)  $(EXTRAOPTIONS) $(INCLUDE_LL) -o mysql.so ${MYSQL}


install: all
	$(CPPVA) mysql.so ${BARTLBY_HOME}/lib/


	



clean:
	$(RMVFR) *.o 
