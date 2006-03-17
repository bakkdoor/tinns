CC=gcc
CXX=g++
CFLAGS=-pedantic -W -Wall -O2 -D_GNU_SOURCE -static
CXX_FLAGS=$(CFLAGS)

SOURCES=$(wildcard *.cpp) $(wildcard ../netcode/*.cpp)
OBJECTS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

all: patchserver

patchserver: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) -lpthread -lz -lm -lcrypt
	mv patchserver ../../

clean:
	-rm $(OBJECTS) patchserver
