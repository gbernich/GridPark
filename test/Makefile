all: hello_world


hello_world: hello_world.o
	g++ `pkg-config --cflags opencv` `pkg-config --libs opencv` hello_world.o -o hello_world


hello_world.o: hello_world.cpp
	g++ `pkg-config --cflags opencv` `pkg-config --libs opencv` -c hello_world.cpp


clean:
	rm *.o hello_world

