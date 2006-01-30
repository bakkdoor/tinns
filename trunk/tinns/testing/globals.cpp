/*
	globals.cpp

	MODIFIED: 12 September 2005 Akiko
	REASON:	- exchanged Pretender strings by TinNS
*/

#include "tinns.h"

PConsole *Console = 0;
PRConsole *RemoteConsole = 0;
PServer *Server = 0;
PConfig *Config = 0;
PGameDefs *GameDefs = 0;
PDatabase *Database = 0;
PFileSystem *Filesystem = 0;
PPatchServer *PatchServer = 0;
PInfoServer *InfoServer = 0;
PGameServer *GameServer = 0;

bool InitTinNS()
{
	nlInit();
	nlSelectNetwork(NL_IP);
	Console = new PConsole();	
	Console->Print("Starting TinNS...");
	
	Config = new PConfig();
	if(!Config->LoadOptions())
	{
		Console->Print("Config error, init aborted");
		return false;
	}
	
	std::string MyName = Config->GetOption("server name");
	std::string IP = Config->GetOption("server ip");
	Console->Print("My name is '%s', and my address is %s", MyName.c_str(), IP.c_str());
	
	RemoteConsole = new PRConsole();
	
	Filesystem = new PFileSystem();

	GameDefs = new PGameDefs();
	GameDefs->Init();
	
	Database = new PDatabase();
	Database->Init();
	
	Server = new PServer();
	PatchServer = new PPatchServer();
	InfoServer = new PInfoServer();
	GameServer = new PGameServer();
	
	return true;
}

void ShutdownTinNS()
{
	Server->Shutdown();

	delete GameServer;
	delete InfoServer;
	delete PatchServer;
	delete Server;
	delete Filesystem;
	delete RemoteConsole;
	delete GameDefs;
	delete Database;
	delete Config;
	delete Console;
	
	nlShutdown();
}

