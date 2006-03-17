all: patchserver infoserver gameserver

patchserver: src/patch/Makefile

infoserver: src/info/Makefile

gameserver: src/game/Makefile

clean:
	rm src/netcode/*.o
	rm src/patch/*.o
	rm src/info/*.o
	rm src/game/*.o
	rm patchserver
	rm infoserver
	rm gameserver
