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
	globals.h

	MODIFIED: 12 September 2005 Akiko
	REASON: - exchanged Pretender Strings by TinNS
                - removed some Windows specific code
	MODIFIED: 16 Dec 2005 bakkdoor
	REASON: - Added global ClientManager and Chat Interface
	MODIFIED: 22 Dec 2005 Namikon
	REASON: - Added GPL

*/

#ifndef GLOBALS_H
#define GLOBALS_H

extern class ServerSocket *ServerSock;
extern class PConsole *Console;
//extern class PRConsole *RemoteConsole;
extern class PConfig *Config;
extern class PConfig *CmdAccess;

extern class PGameDefs *GameDefs;
extern class PDatabase *Database;
extern class PFileSystem *Filesystem;
extern class PServer *Server;
extern class PGameServer *GameServer;
extern class PMsgBuilder *MsgBuilder;
extern class PWorlds *Worlds;
extern class PAppartements* Appartements;
extern class PWorldActors* WorldActors;
//extern class gmMachine machine;   // Virtual Machine instance

//multiuser chat
extern class PClientManager *ClientManager;
extern class PNPCManager* NPCManager;
extern class PChat *Chat;
extern class PCommands *GameCommands;

// Development debug output control
extern bool gDevDebug;

//MySQL-Support
extern class PMySQL* MySQL;

//Vehicles
extern class PVehicles *Vehicles;

//Infoserver update
extern class PISC *ISC;

//Empty string
extern const std::string EmptyString;

extern const char ServerVersion[];
extern const char SVNRevision[];

bool InitTinNS();
void Shutdown();

#endif

