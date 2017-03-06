all: open_parking hello_world

open_parking: open_parking.o ip.o
	g++ `pkg-config --cflags opencv` `pkg-config --libs opencv` open_parking.o ip.o -o open_parking

	
open_parking.o: open_parking.cpp
	g++ `pkg-config --cflags opencv` `pkg-config --libs opencv` -c open_parking.cpp



hello_world: hello_world.o ip.o
	g++ `pkg-config --cflags opencv` `pkg-config --libs opencv` hello_world.o ip.o -o hello_world


hello_world.o: hello_world.cpp
	g++ `pkg-config --cflags opencv` `pkg-config --libs opencv` -c hello_world.cpp


ip.o: ip.cpp
	g++ `pkg-config --cflags opencv` `pkg-config --libs opencv` -c ip.cpp

clean:
	rm *.o open_parking hello_world

