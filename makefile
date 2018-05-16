ggsx: GraphGrepSXConsole.o
	cd vflib2; make
	g++ -w -o ggsx GraphGrepSXConsole.o -L./vflib2/lib -lvf -lstdc++ -lm -O3 -pthread -std=c++0x
GraphGrepSXConsole.o: GraphGrepSXConsole.cpp 
	g++ -w -c -I./vflib2/include -I ./GGSXLib GraphGrepSXConsole.cpp

clean:
	rm ggsx
	cd vflib2; make clean
