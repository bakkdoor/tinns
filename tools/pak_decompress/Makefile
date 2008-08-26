CC=gcc
CXX=g++
CFLAGS=-pedantic -Wall
CXX_FLAGS=$(CFLAGS)

SOURCES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@
        
all: pak_decompress

pak_decompress: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) -lstdc++ -lz

clean:
	-rm $(OBJECTS) pak_decompress
