CXX_FLAGS = -std=c++11 -Wall -O3

all: dot

dot: run
	dot a.dot -Tpng -o a.png

run: program
	program mississippi.txt ss

program:
	g++ $(CXX_FLAGS) -c main.cpp -o main.o
	g++ main.o -o program

clean:
	rm -rf *o program
