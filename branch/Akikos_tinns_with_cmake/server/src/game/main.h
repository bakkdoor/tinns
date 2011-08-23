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

	MODIFIED: 31 Aug 2005 Akiko
	REASON: - updating def file definitions to match the changes in gamedefs.cpp
		- added define for charfiles path
		- added define for database path
	MODIFIED: 26 Sep 2005 Akiko
	REASON: - reformated code
		- added GPL
		- added infoserver default port as define
		- added my threading model (mutex.h, semaphore.h, thread.h)
		- added basic includes pthread.h, semaphore.h and errno.h
	MODIFIED: 28 Sep 2005 Akiko
	REASON:	- added define for game server port
	MODIFIED: 30 Nov 2005 Akiko
	REASON: - added chat.h for the chat part from Namikon
	MODIFIED: 02 Dec 2005 Akiko
	REASON: - commented out mutex.h, thread.h, semaphore.h
	MODIFIED: 22 Dec 2005 Namikon/bakkdoor
	REASON: - Added commands.h, skill.h, clientmanager.h
	MODIFIED: 23 Dec 2005 bakkdoor
	REASON: - Added <csignal> for main.cpp -> catch strg-c and shutdown nicely
	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added mysql.h, sql.h for MySQL support
	MODIFIED: 01 Jan 2006 Namikon
	REASON: - Moved skill.h before chars.h (char.h needs skill.h now)
	MODIFIED: 30 May 2006 Namikon
	REASON: - Removed all useless includes to complete the server splitup; Also renamed tinns.h to main.h
	MODIFIED: 6 Jul 2006 Hammag
	REASON: - moved include "types.h" before include "../netcode/main.h" to permit compile
	MODIFIED: 10 Jul 2006 Hammag
	REASON: - added inventory.h
	REASON: - added item.h
	MODIFIED: 26 Jul 2006 Hammag
	REASON: - removed	#define GAME_PORT which is not used anymore (now in config file)
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
		- cleaned up a bit
*/

#ifndef MAIN_H
#define MAIN_H

//#include "version.h"

//basic includes
#include "include/external.h"

//tinns includes
#include "include/types.h"
#include "common/netcode.h"
/*
#include "../gamemonkey/gmMachine.h"
#include "../gamemonkey/gmCall.h"
*/
#include "common/console.h"
#include "common/misc.h"

// MySQL Support
#ifdef MYSQL_INC_DIR
#include <mysql/mysql.h>
#else
#include <mysql.h>
#endif

#include "include/sql.h"

#include "common/config.h"
#include "common/filesystem.h"
#include "include/defparser.h"

#include "include/skill.h"
#include "include/chars.h"
#include "include/accounts.h"
#include "include/client.h"
#include "include/server.h"
#include "include/gameserver.h"
#include "include/globals.h"
#include "include/defs.h"
#include "include/zoning.h"
#include "include/item.h"
#include "include/inventory.h"
#include "include/worldactors.h"
#include "include/npc.h"
#include "include/outpost.h"
#include "include/multipart.h"
#include "include/terminal.h"

#include "include/lua_engine.h"
#include "include/chat.h"
#include "include/commands.h"
#include "include/clientmanager.h"

#endif

