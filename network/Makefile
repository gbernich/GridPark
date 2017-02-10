all: send recv

send: send.o network.o
	gcc send.o network.o -o send

recv: recv.o network.o
	gcc recv.o network.o -o recv

send.o: send.c
	gcc -c send.c

recv.o: recv.c
	gcc -c recv.c

network.o: network.c
	gcc -c network.c

clean:
	rm *.o send recv
