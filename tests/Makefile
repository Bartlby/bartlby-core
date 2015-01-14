all: bartlby_test

.c.o:
	gcc -I../include/ -I/usr/include/curl -I/usr/include/json $(CFLAGS)    -g -c $<

tinytest.o: tinytest.h

OBJS=tinytest.o bartlby_test.o global.o worker.o service.o server.o downtime.o traps.o servicegroup.o servergroup.o cleanup.o core.o

bartlby_test: $(OBJS)
	gcc -Wl,--no-as-needed -g -O2 $(OBJS) ../libbartlby.a -ldl -ljson -lcurl -lrt  -o bartlby_test



clean:
	rm -f *.o *~ bartlby_test


