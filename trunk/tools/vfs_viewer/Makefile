CC=cc
CXX=g++
CFLAGS=-W -Wall -O2
CXX_FLAGS=$(CFLAGS)

SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

%.o:%.c
	$(CXX) $(CXX_FLAGS) -c $< -o $@
        
all: vfs_viewer

vfs_viewer: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS)

clean:
	-rm $(OBJECTS) vfs_viewer
