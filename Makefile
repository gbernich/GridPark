all: send recv



open_parking: open_parking.o ip.o
	g++ `pkg-config --cflags opencv` `pkg-config --libs opencv` open_parking.o ip.o -o open_parking

open_parking.o: open_parking.cpp
	g++ `pkg-config --cflags opencv` `pkg-config --libs opencv` -c open_parking.cpp



ip.o: ip.cpp
	g++ `pkg-config --cflags opencv` `pkg-config --libs opencv` -c ip.cpp



send: send.o db_utils.o
	gcc send.o db_utils.o common.c -o send -L/usr/lib/mysql -lmysqlclient

recv: recv.o db_utils.o common.o
	gcc recv.o db_utils.o common.o -o recv -L/usr/lib/mysql -lmysqlclient



send.o: send.c
	gcc -c send.c

recv.o: recv.c
	gcc -c recv.c



common.o: common.c
	gcc -c common.c

db_utils.o: db_utils.c
	gcc -c db_utils.c `mysql_config --cflags --libs`



clean:
	rm *.o send recv
