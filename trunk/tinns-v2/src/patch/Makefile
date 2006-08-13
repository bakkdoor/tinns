inc=/usr/include/mysql
lib=/usr/lib/mysql
CC=gcc
CXX=g++
CFLAGS=-pedantic -W -Wall -O2 -D_GNU_SOURCE -I$(inc) -D_THREAD_SAFE -D_REENTRANT -static -L$(lib) -Wno-long-long
CXX_FLAGS=$(CFLAGS)

SOURCES=$(wildcard *.cpp) $(wildcard ../netcode/*.cpp)
OBJECTS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

all: patchserver

patchserver: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) -lrt -lpthread -lz -lm -lcrypt -L"/usr/lib/mysql" -lmysqlclient
	mv patchserver ../../

clean:
	-rm $(OBJECTS) patchserver
