all: send recv



send: send.o db_utils.o
	gcc send.o db_utils.o -o send -L/usr/lib/mysql -lmysqlclient

recv: recv.o db_utils.o
	gcc recv.o db_utils.o -o recv -L/usr/lib/mysql -lmysqlclient



send.o: send.c
	gcc -c send.c

recv.o: recv.c
	gcc -c recv.c



db_utils.o: db_utils.c
	gcc -c db_utils.c -std=c99 `mysql_config --cflags --libs`



clean:
	rm *.o send recv
