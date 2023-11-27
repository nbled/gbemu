CXX=g++
CXXFLAGS=-Wall -Wextra -g3
IFLAGS=-Iinclude -I.

GBIT=gbit
GBIT_LDFLAGS=-L$(GBIT) -lgbit

all: prepare bin/TestCPU bin/ROMExplorer bin/LoadBlob

prepare:
	@mkdir -p build
	@mkdir -p bin

test:
	LD_LIBRARY_PATH=$(GBIT) ./TestCPU

bin/TestCPU: build/TestCPU.o build/CPU.o build/Instruction.o build/InstructionSet.o
	$(CXX) -o bin/TestCPU build/TestCPU.o build/CPU.o build/Instruction.o build/InstructionSet.o $(GBIT_LDFLAGS)

bin/ROMExplorer: build/ROMExplorer.o build/Cartridge.o build/Instruction.o build/InstructionSet.o
	$(CXX) -o bin/ROMExplorer build/ROMExplorer.o build/Cartridge.o build/Instruction.o build/InstructionSet.o

bin/LoadBlob: build/LoadBlob.o build/CPU.o build/Instruction.o  build/Instruction.o build/InstructionSet.o
	$(CXX) -o bin/LoadBlob build/LoadBlob.o build/CPU.o build/Instruction.o build/InstructionSet.o

build/TestCPU.o: src/test/TestCPU.cpp 
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/TestCPU.o src/test/TestCPU.cpp

build/LoadBlob.o: src/test/LoadBlob.cpp include/cpu/CPU.hpp include/memory/MemoryMap.hpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/LoadBlob.o src/test/LoadBlob.cpp

build/ROMExplorer.o: src/test/ROMExplorer.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/ROMExplorer.o src/test/ROMExplorer.cpp

build/Cartridge.o: src/Cartridge.cpp \
			include/Cartridge.hpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/Cartridge.o src/Cartridge.cpp

build/CPU.o: src/cpu/CPU.cpp \
			include/cpu/CPU.hpp \
			include/cpu/Registers.hpp \
			include/memory/MemoryMap.hpp \
			include/memory/MemorySegment.hpp \
			include/memory/MemoryInterface.hpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/CPU.o src/cpu/CPU.cpp

build/Instruction.o: build/InstructionSet.o \
				src/cpu/Instruction.cpp \
				include/cpu/Instruction.hpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/Instruction.o src/cpu/Instruction.cpp

build/InstructionSet.o: src/cpu/InstructionSet.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/InstructionSet.o src/cpu/InstructionSet.cpp

clean:
	rm -rf bin/ build/


