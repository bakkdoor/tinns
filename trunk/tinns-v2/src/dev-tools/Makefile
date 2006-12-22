all: cleandepfile svnrev

cleandepfile: cleandepfile.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

svnrev: getsvnrev.cpp
	g++ -pedantic -Wall -c getsvnrev.cpp -o getsvnrev.o
	g++ -o getsvnrev getsvnrev.o -lstdc++ -lz

