CXX=g++
CXXFLAGS=-Wall -Wextra -g3
IFLAGS=-Iinclude -I.

GBIT=gbit
GBIT_LDFLAGS=-L$(GBIT) -lgbit

all: prepare bin/TestCPU bin/ROMExplorer

prepare:
	@mkdir -p build
	@mkdir -p bin

test:
	LD_LIBRARY_PATH=$(GBIT) ./TestCPU

bin/TestCPU: build/TestCPU.o build/CPU.o
	$(CXX) -o bin/TestCPU build/TestCPU.o build/CPU.o $(GBIT_LDFLAGS)

bin/ROMExplorer: build/ROMExplorer.o build/Cartridge.o
	$(CXX) -o bin/ROMExplorer build/ROMExplorer.o build/Cartridge.o

build/TestCPU.o: src/test/TestCPU.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/TestCPU.o src/test/TestCPU.cpp

build/ROMExplorer.o: src/test/ROMExplorer.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/ROMExplorer.o src/test/ROMExplorer.cpp

build/Cartridge.o: src/Cartridge.cpp \
			include/Cartridge.hpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/Cartridge.o src/Cartridge.cpp

build/CPU.o: src/cpu/CPU.cpp \
			include/cpu/CPU.hpp \
			include/cpu/MMap.hpp \
			include/cpu/Registers.hpp \
			include/cpu/MemoryInterface.hpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/CPU.o src/cpu/CPU.cpp

clean:
	rm -rf bin/ build/


