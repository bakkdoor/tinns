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

#include "main.h"

ServerSocket* ServerSock = 0;
PConsole *Console = 0;
PServer *Server = 0;
PConfig *Config = 0;
PFileSystem *Filesystem = 0;
PPatchServer *PatchServer = 0;

bool InitTinNS()
{
	Console = new PConsole();
	Console->Print("Starting TinNS Patchserver...");
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

	char svnrev[10];
	GetSVNRev(svnrev);
	Console->LPrint("You are running TinNS Patchserver version");
	Console->LPrint(GREEN, BLACK, " %s", svnrev);
	Console->LClose();
	ServerSock = new ServerSocket();
	Config = new PConfig();
	Server = new PServer();
	if(!Config->LoadOptions())
	    Shutdown();

	PatchServer = new PPatchServer();

	return true;
}

void Shutdown()
{
	if(PatchServer) delete PatchServer;
	if(Config) delete Config;
	if(Console) delete Console;
    if(ServerSock) delete ServerSock;
    exit(0);
}

