GBIT=gbit/

CXX=g++

CXXFLAGS=-Wall -Wextra -g3
IFLAGS=-Iinclude/ -I.
LDFLAGS=-L$(GBIT) -lgbit

all: prepare TestCPU

prepare:
	@mkdir -p build

test:
	LD_LIBRARY_PATH=$(GBIT) ./TestCPU

TestCPU: build/TestCPU.o build/CPU.o
	$(CXX) -o TestCPU build/TestCPU.o build/CPU.o $(LDFLAGS)

build/TestCPU.o: src/test/TestCPU.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/TestCPU.o src/test/TestCPU.cpp

build/CPU.o: src/cpu/CPU.cpp \
			include/cpu/CPU.hpp \
			include/cpu/MMap.hpp \
			include/cpu/Registers.hpp \
			include/cpu/MemoryInterface.hpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o build/CPU.o src/cpu/CPU.cpp

clean:
	rm -rf build/ TestCPU


