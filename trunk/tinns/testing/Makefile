CC=gcc
CXX=g++
CFLAGS=-pedantic -Wall -D_GNU_SOURCE
CXX_FLAGS=$(CFLAGS)

SOURCES=$(wildcard TinyXML/*.cpp) $(wildcard *.cpp) $(wildcard HawkNL/*.c)
OBJECTS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@
        
all: tinns

tinns: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) -lpthread -lz

clean:
	-rm $(OBJECTS) tinns
