/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community
	maintainer Akiko <akiko@gmx.org>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
	02110-1301, USA.
*/

/*
	globals.cpp

	MODIFIED: 12 September 2005 Akiko
	REASON:	- exchanged Pretender strings by TinNS
    MODIFIED: 16 Dec 2005 bakkdoor
	REASON: - Added global ClientManager and Chat Interface
	MODIFIED: 22 Dec 2005 Namikon
	REASON: - Added GPL
	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added MySQL Support
            - Fixed ShutdownTinns (Wont cause segmentation fault anymore)
    MODIFIED: 06 Jan 2006 Namikon
    REASON: - Added color to console outputs
            - Added shiny and colored copyright box :D
*/

#include "tinns.h"
PVehicles *Vehicles = 0;
PMySQL *MySQL = 0;
PConsole *Console = 0;
//PRConsole *RemoteConsole = 0;
PServer *Server = 0;
PConfig *Config = 0;
PGameDefs *GameDefs = 0;
PDatabase *Database = 0;
PFileSystem *Filesystem = 0;
PGameServer *GameServer = 0;
ServerSocket *ServerSock = 0;

//multi-user chat implementation
PClientManager *ClientManager = 0;
PChat *Chat = 0;

bool InitTinNS()
{
	Console = new PConsole();
	Console->Print("Starting TinNS...");
	Console->Print(WHITE, BLUE, "/-------------------------------------------------------------------\\");
	Console->Print(WHITE, BLUE, "|               TinNS (TinNS is not a Neocron Server)               |");
    Console->Print(WHITE, BLUE, "|            Copyright (C) 2005 Linux Addicted Community            |");
	Console->Print(WHITE, BLUE, "|                  maintainer Akiko <akiko@gmx.org>                 |");
	Console->Print(WHITE, BLUE, "|             ==========================================            |");
	Console->Print(WHITE, BLUE, "|      Head coders:                   The packet analyzing team:    |");
	Console->Print(WHITE, BLUE, "|      - Akiko                         - MaxxJag                    |");
	Console->Print(WHITE, BLUE, "|      - bakkdoor                      - Sting                      |");
	Console->Print(WHITE, BLUE, "|      - Namikon                       - Balm                       |");
	Console->Print(WHITE, BLUE, "|-------------------------------------------------------------------|");
	Console->Print(WHITE, BLUE, "|  This project would'nt be at its current stage without the help   |");
    Console->Print(WHITE, BLUE, "|        from the NeoPolis team, special thanks to you guys!        |");
	Console->Print(WHITE, BLUE, "|-------------------------------------------------------------------|");
	Console->Print(WHITE, BLUE, "|  This project is under GPL, see any source file for more details  |");
	Console->Print(WHITE, BLUE, "\\-------------------------------------------------------------------/");
	Console->Print("You are running TinNS version %s", Console->ColorText(GREEN, BLACK, "SVN 41"));
	Config = new PConfig();
	if(!Config->LoadOptions())
	{
		Console->Print("Config error, init aborted");
		return false;
	}
	std::string MyName = Config->GetOption("server_name");
	std::string IP = Config->GetOption("server_ip");
	char myCname[100], myCip[100];
    strcpy (myCname, Console->ColorText(CYAN, BLACK, MyName.c_str()));
	strcpy (myCip, Console->ColorText(CYAN, BLACK, IP.c_str()));
	Console->Print("My name is '%s', and my address is %s", myCname, myCip);

	MySQL = new PMySQL();
    if(MySQL->Connect() == false) {
        delete MySQL;
        delete Config;
        delete Console;
        delete ServerSock;
        exit(0);
    }

	//RemoteConsole = new PRConsole();

	Filesystem = new PFileSystem();

	GameDefs = new PGameDefs();
	GameDefs->Init();

	Database = new PDatabase();
	Database->Init();

	ServerSock = new ServerSocket();
	Server = new PServer();
	GameServer = new PGameServer();

	Vehicles = new PVehicles();

	ClientManager = new PClientManager();
	Chat = new PChat();

	return true;
}

void ShutdownTinNS()
{
	Server->Shutdown();
	if(Chat) delete Chat;
//	if(ClientManager) delete ClientManager;

	if(GameServer) delete GameServer;
	if(Server) delete Server;
	if(Filesystem) delete Filesystem;
//	if(RemoteConsole) delete RemoteConsole;
	if(GameDefs) delete GameDefs;
	if(Database) delete Database;
	if(MySQL) delete MySQL;
	if(Config) delete Config;
	if(Console) delete Console;
    if(ServerSock) delete ServerSock;
    exit(0);
}

