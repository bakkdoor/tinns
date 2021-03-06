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
  MODIFIED: 27 Aug 2006 Hammag
  REASON: - Added AdditionnalConfigChecks() local function

*/

#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef _MSC_VER
	#pragma once
#endif

extern class ServerSocket* ServerSock;
extern class PConsole *Console;
extern class PServer *Server;
extern class PConfig *Config;
extern class PInfoServer *InfoServer;

extern class PMySQL* MySQL;
//extern class PAccounts* Accounts;  // To be removed

extern const char ServerVersion[];
extern const char SVNRevision[];

bool Init();
void Shutdown();
bool AdditionnalConfigChecks();

#endif

