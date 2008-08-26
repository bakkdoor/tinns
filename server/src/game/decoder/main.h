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
	main.h - main include file, contains all needed includes and important definitions

	MODIFIED: 30 Aug 2006 Hammag
	REASON: - created

*/

#ifndef MAIN_H
#define MAIN_H

//#include "version.h"

//basic includes
#include "external.h"

//tinns includes
#include "types.h"
#include "config.h"

#include "console.h"
#include "misc.h"

#include "netcode.h"

#include "globals.h"

#include "msgdecoder.h"
#include "udpanalyser.h"

/*
#include "../gamemonkey/gmMachine.h"
#include "../gamemonkey/gmCall.h"
*/

// MySQL Support // shouldn't be needed as DB-objects access class should do that
#include "mysql.h"
#include "sql.h"

#include "skill.h"

#include "chars.h"
#include "accounts.h"
#include "defs.h"
#include "client.h"
#include "server.h"
#include "misc.h"
#include "gameserver.h"
#include "globals.h"
#include "zoning.h"
#include "item.h"
#include "inventory.h"

#include "chat.h"
#include "commands.h"
#include "clientmanager.h"
#include "msgbuilder.h"
#include "worldactors.h"
#include "npc.h"
#include "terminal.h"

using namespace std;

#endif

