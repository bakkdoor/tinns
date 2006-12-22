CC=gcc
CXX=g++
CFLAGS=-pedantic -Wall
CXX_FLAGS=$(CFLAGS)

SOURCES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@
        
all: getsvnrev

getsvnrev: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) -lstdc++ -lz

clean:
	-rm $(OBJECTS) getsvnrev

