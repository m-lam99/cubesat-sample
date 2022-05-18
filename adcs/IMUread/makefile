CXXFLAGS = -std=c++11 -Wall -O2 -D__VERBOSE__

all: run

run: objs/RPi_BNO055.o objs/Print_Data.o
	g++ $(CXXFLAGS) -o run objs/RPi_BNO055.o objs/Print_Data.o -lpigpio

objs/RPi_BNO055.o:
	g++ $(CXXFLAGS) -c -I ./include -o objs/RPi_BNO055.o  src/RPi_BNO055.cpp

objs/Print_Data.o:
	g++ $(CXXFLAGS) -c -I ./include -o objs/Print_Data.o src/Print_Data.cpp

clean:
	rm -f run objs/RPi_BNO055.o objs/Print_Data.o
