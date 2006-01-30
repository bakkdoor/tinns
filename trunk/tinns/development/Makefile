inc=/usr/include/mysql
lib=/usr/lib/mysql
CC=gcc
CXX=g++
CFLAGS=-pedantic -W -Wall -O2 -D_GNU_SOURCE -I$(inc) -D_THREAD_SAFE -D_REENTRANT -static -L$(lib) -Wno-long-long
CXX_FLAGS=$(CFLAGS)

SOURCES=$(wildcard *.cpp) $(wildcard HawkNL/*.c)
OBJECTS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

all: tinns

tinns: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) -lpthread -lz -lm -lcrypt -lmysqlclient

clean:
	-rm $(OBJECTS) tinns
