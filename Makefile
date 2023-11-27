CXX=g++
CXXFLAGS=-Wall -Wextra -g3
IFLAGS=-Iinclude -I.

GBIT=gbit
GBIT_LDFLAGS=-L$(GBIT) -lgbit

MODULES=CPU \
	Instruction \
	InstructionSet \
	Cartridge
TOOLS=TestCPU \
	ROMExplorer \
	LoadBlob

OBJECTS:=$(addsuffix .o, $(MODULES))
OBJECTS:=$(addprefix build/, $(OBJECTS))

TARGETS=$(addprefix bin/, $(TOOLS))

all: prepare $(TARGETS)

prepare:
	@mkdir -p build
	@mkdir -p bin

test:
	LD_LIBRARY_PATH=$(GBIT) ./TestCPU

bin/TestCPU: build/TestCPU.o $(OBJECTS)
	$(CXX) -o $@ $^ $(GBIT_LDFLAGS)

bin/ROMExplorer: build/ROMExplorer.o $(OBJECTS)
	$(CXX) -o $@ $^

bin/LoadBlob: build/LoadBlob.o $(OBJECTS)
	$(CXX) -o $@ $^

build/%.o: src/tools/%.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o $@ $^

build/%.o: src/cpu/%.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o $@ $^

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -o $@ $^

clean:
	rm -rf bin/ build/


