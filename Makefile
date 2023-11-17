
all: gbcpu test.gb

gbcpu: main.cpp GameBoyCPU.o
	g++ -Wall -Wextra -c -o main.o main.cpp
	g++ -o gbcpu main.o GameBoyCPU.o

GameBoyCPU.o: GameBoyCPU.cpp GameBoyCPU.hpp GameBoyMMap.hpp
	g++ -Wall -Wextra -c -o GameBoyCPU.o GameBoyCPU.cpp

test.gb: test.ogb 
	rgblink -o test.gb test.ogb
	rgbfix -v -p 0xFF test.gb

test.ogb: test.asm
	rgbasm -L -o test.ogb test.asm

clean:
	rm *.o *.ogb *.gb gbcpu

