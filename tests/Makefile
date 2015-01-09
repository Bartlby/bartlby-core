all: bartlby_test

.c.o:
	gcc -I../include/ -I/usr/include/curl -I/usr/include/json $(CFLAGS)   -g -O2 -c $<

tinytest.o: tinytest.h

OBJS=tinytest.o bartlby_test.o global.o worker.o core.o

bartlby_test: $(OBJS)
	gcc -ldl -ljson -lcurl  -g -O2 $(OBJS) ../libbartlby.a  -o bartlby_test



clean:
	rm -f *.o *~ bartlby_test


