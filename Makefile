MGBDIS=mgbdis/

all: gbcpu test.gb

test: TestCPU

gbcpu: main.cpp GameBoyCPU.o
	g++ -Wall -Wextra -c -o main.o main.cpp
	g++ -o gbcpu main.o GameBoyCPU.o

GameBoyCPU.o: GameBoyCPU.cpp GameBoyCPU.hpp GameBoyMMap.hpp GameBoyRegisters.hpp GameBoyMemoryInterface.hpp GameBoyMockMMap.hpp
	g++ -Wall -Wextra -c -o GameBoyCPU.o GameBoyCPU.cpp

TestCPU: TestCPU.o GameBoyCPU.o
	g++ -o TestCPU TestCPU.o GameBoyCPU.o -Lgbit/ -lgbit

TestCPU.o:
	g++ -Wall -Wextra -c -o TestCPU.o TestCPU.cpp

disasm: test.gb
	$(MGBDIS)/mgbdis.py --print-hex --overwrite --uppercase-hex test.gb

test.gb: test.ogb 
	rgblink -o test.gb test.ogb
	rgbfix -v -p 0xFF test.gb

test.ogb: test.asm
	rgbasm -L -o test.ogb test.asm

clean:
	rm *.o *.ogb *.gb gbcpu

