/*
	globals.h

	MODIFIED: 12 September 2005 Akiko
	REASON: - exchanged Pretender Strings by TinNS
		- removed some Windows specific code
*/

#ifndef GLOBALS_H
#define GLOBALS_H

extern class PConsole *Console;
extern class PRConsole *RemoteConsole;
extern class PConfig *Config;
extern class PGameDefs *GameDefs;
extern class PDatabase *Database;
extern class PFileSystem *Filesystem;
extern class PServer *Server;
extern class PPatchServer *PatchServer;
extern class PInfoServer *InfoServer;
extern class PGameServer *GameServer;

bool InitTinNS();
void ShutdownTinNS();

#endif

