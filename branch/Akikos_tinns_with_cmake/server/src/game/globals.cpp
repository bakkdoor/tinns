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
	REASON: - exchanged Pretender strings by TinNS
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
	MODIFIED: 22 Jul 2006 Hammag
	REASON: - Added Server NOT NULL check to avoid segfault when shuting down during startup
	MODIFIED: 27 Aug 2006 Hammag
	REASON: - Implemented shared Config class use and config template to load conf.
		- Added gameserver configtemplate.h include,
		- Added new required parameters to Config->LoadOptions()
	MODIFIED: 02 Oct 2006 Hammag
	REASON: - Added gDevDebug global flag to control development debug outputs (flagged messaged,
		  dump-flagged messes, dev console->print)
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for buildsystem
		- clean ups

	TODO:   - Get logfile name from config file
*/


#include "main.h"

#include "configtemplate.h"
#include "include/isc.h"

#include "include/msgbuilder.h"
#include "include/worlds.h"
#include "include/appartements.h"
#include "include/npc.h"
#include "include/subway.h"
#include "include/terminal.h"

#include "include/version.h"


const char ServerVersion[] = TINNS_GAME_VERSION;
const char SVNRevision[] = TINNS_SVN_REVISION;

PVehicles *Vehicles = 0;
PMySQL *MySQL = 0;
PConsole *Console = 0;
PServer *Server = 0;
PConfig *Config = 0;
PConfig *CmdAccess = 0;
PGameDefs *GameDefs = 0;
PChars *Chars = 0;
PFileSystem *Filesystem = 0;
PGameServer *GameServer = 0;
ServerSocket *ServerSock = 0;
PMsgBuilder *MsgBuilder = 0;
PWorlds *Worlds = 0;
PAppartements* Appartements = 0;
PWorldActors* WorldActors = 0;
PNPCManager* NPCManager = 0;
PSubway* Subway = 0;
PTerminal* Terminal = 0;
PLuaEngine* LuaEngine = 0;
POutpost* Outposts = 0;
PMultiPart* MultiPartHandler = 0;

//multi-user chat implementation
PClientManager *ClientManager = 0;
PChat *Chat = 0;
PCommands *GameCommands = 0;

PISC *ISC = 0;

const std::string EmptyString;

// Development debug output control (set by config option dev_debug)
bool gDevDebug = false;

bool InitTinNS()
{
    Console = new PConsole("log/gameserver.log"); // Make that from config file !!!
    Console->Print("Starting TinNS Gameserver");
    Console->Print(WHITE, BLUE, "/-------------------------------------------------------------------\\");
    Console->Print(WHITE, BLUE, "|               TinNS (TinNS is not a Neocron Server)               |");
    Console->Print(WHITE, BLUE, "|            Copyright (C) 2005 Linux Addicted Community            |");
    Console->Print(WHITE, BLUE, "|                  maintainer Akiko <akiko@gmx.org>                 |");
    Console->Print(WHITE, BLUE, "|             ==========================================            |");
    Console->Print(WHITE, BLUE, "|      Head coders:                   The packet analyzing team:    |");
    Console->Print(WHITE, BLUE, "|      - Akiko                         - MaxxJag                    |");
    Console->Print(WHITE, BLUE, "|      - bakkdoor                      - Sting                      |");
    Console->Print(WHITE, BLUE, "|      - Namikon                       - Balm                       |");
    Console->Print(WHITE, BLUE, "|      - Hammag                                                     |");
    Console->Print(WHITE, BLUE, "|-------------------------------------------------------------------|");
    Console->Print(WHITE, BLUE, "|  This project would'nt be at its current stage without the help   |");
    Console->Print(WHITE, BLUE, "|        from the NeoPolis team, special thanks to you guys!        |");
    Console->Print(WHITE, BLUE, "|-------------------------------------------------------------------|");
    Console->Print(WHITE, BLUE, "|  This project is under GPL, see any source file for more details  |");
    Console->Print(WHITE, BLUE, "\\-------------------------------------------------------------------/");

    //char svnrev[10];
    //GetSVNRev(svnrev);
    Console->LPrint("You are running TinNS Gameserver version");
    Console->LPrint(GREEN, BLACK, " %s", ServerVersion);
    Console->LPrint(WHITE, BLACK, " - SVN Rev");
    Console->LPrint(GREEN, BLACK, " %s", SVNRevision);
    Console->LClose();

    Config = new PConfig();
    if(!Config->LoadOptions(GameConfigTemplate, "./conf/gameserver.conf"))
        return false; //Shutdown();

    CmdAccess = new PConfig();
    if(!CmdAccess->LoadOptions(CommandsTemplate, "./conf/commands.conf"))
        return false; //Shutdown();

    gDevDebug = Config->GetOptionInt("dev_debug");
    std::string MyName = Config->GetOption("server_name");
    std::string IP = Config->GetOption("server_ip");
    char myCname[100], myCip[100];
    strncpy (myCname, Console->ColorText(CYAN, BLACK, MyName.c_str()), 100);
    strncpy (myCip, Console->ColorText(CYAN, BLACK, IP.c_str()), 100);
    Console->Print("My name is '%s', and my address is %s", myCname, myCip);

    MySQL = new PMySQL();
    if(MySQL->Connect() == false)
        return false; //Shutdown();

    Filesystem = new PFileSystem();

    GameDefs = new PGameDefs();
    GameDefs->Init();

    Worlds = new PWorlds();
    Worlds->LoadWorlds();

    WorldActors = new PWorldActors();
	LuaEngine = new PLuaEngine();

    NPCManager = new PNPCManager();
    Appartements = new PAppartements;
    Subway = new PSubway;

    if (!PAccount::SetUsernameRegexFilter(Config->GetOption("username_filter").c_str()))
    {
      Console->Print("%s Could not creat username_filter PCRE '%s'", Console->ColorText(RED, BLACK, "[Error]"), Config->GetOption("username_filter").c_str());
      return false;
    }
    if(!PAccount::SetPasswordRegexFilter(Config->GetOption("password_filter").c_str()))
    {
      Console->Print("%s Could not creat password_filter PCRE '%s'", Console->ColorText(RED, BLACK, "[Error]"), Config->GetOption("password_filter").c_str());
      return false;
    }
    
    if (!PChar::SetCharnameRegexFilter(Config->GetOption("charname_filter").c_str()))
    {
      Console->Print("%s Could not creat charname_filter PCRE '%s'", Console->ColorText(RED, BLACK, "[Error]"), Config->GetOption("charname_filter").c_str());
      return false;
    }
    Chars = new PChars();
       
    ServerSock = new ServerSocket();
    Server = new PServer();
    GameServer = new PGameServer();
    MsgBuilder = new PMsgBuilder();

    Vehicles = new PVehicles();

    ClientManager = new PClientManager();
    Chat = new PChat();
    GameCommands = new PCommands();

    ISC = new PISC();
    Terminal = new PTerminal();

	Outposts = new POutpost();

	MultiPartHandler = new PMultiPart();

    return true;
}

void Shutdown()
{
    if(MultiPartHandler)
        delete MultiPartHandler;
    if(Outposts)
        delete Outposts;
    if(LuaEngine)
        delete LuaEngine;
    if(Terminal)
        delete Terminal;
    if(WorldActors)
        delete WorldActors;
    if(NPCManager)
        delete NPCManager;
    if(ISC)
    {
        ISC->Shutdown();
        delete ISC;
    }
    if(Server)
        Server->Shutdown();
    if(GameServer)
        delete GameServer;
    if(Chat)
        delete Chat;
    if(Server)
        delete Server;
    if(Filesystem)
        delete Filesystem;
    if(GameDefs)
        delete GameDefs;
    if(Worlds)
    {
        Worlds->Shutdown();
        delete Worlds;
    }
    if(Subway)
        delete Subway;
    if(Chars)
        delete Chars;
    if(MySQL)
        delete MySQL;
    if(Config)
        delete Config;
    if(Console)
        delete Console;
    if(ServerSock)
        delete ServerSock;

    exit(0);
}

