
all: gbcpu

gbcpu: main.cpp GameBoyCPU.o
	g++ -Wall -Wextra -c -o main.o main.cpp
	g++ -o gbcpu main.o GameBoyCPU.o

GameBoyCPU.o: GameBoyCPU.cpp GameBoyCPU.hpp GameBoyMMap.hpp
	g++ -Wall -Wextra -c -o GameBoyCPU.o GameBoyCPU.cpp

test.gb: test.asm
	rgbasm -L -o test.o test.asm
	rgblink -o test.gb test.o
	rgbfix -v -p 0xFF test.gb

#GameBoyMMap.o: GameBoyMMap.cpp
#	g++ -Wall -Wextra -c -o GameBoyMMap.o GameBoyMMap.cpp
