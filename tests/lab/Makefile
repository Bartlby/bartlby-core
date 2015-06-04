all:
	gcc -g -O0 -I/usr/include/json -l json  -o json_sample json_sample.c
	gcc -g -O0 -I/usr/include/json -l json -lm  -I../../include/ -I../../build/include/ -I../ -I../../src/deps/ -o baseline  baseline.c ../../build/libbartlby.a 
	gcc -g -O0 -I/usr/include/lua5.2/ -llua5.2 -o lua_sample lua_sample.c
	#gcc -O0 -g  -I/usr/local/include/luajit-2.0/ -lluajit-5.1 -o luajit_sample lua_sample.c
