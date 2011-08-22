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
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#ifndef MAIN_H
#define MAIN_H

//#include "version.h"

//basic includes
#include "include/external.h"

//tinns includes
#include "include/types.h"
#include "include/config.h"

#include "include/console.h"
#include "include/misc.h"

#include "include/netcode.h"

#include "include/globals.h"

#include "include/msgdecoder.h"
#include "udpanalyser.h"


/*
#include "../gamemonkey/gmMachine.h"
#include "../gamemonkey/gmCall.h"
*/

#include "include/filesystem.h"

// MySQL Support // shouldn't be needed as DB-objects access class should do that
#ifdef MYSQL_INC_DIR
#include <mysql/mysql.h>
#else
#include <mysql.h>
#endif

#include "include/sql.h"

#include "include/skill.h"

#include "include/chars.h"
#include "include/accounts.h"
#include "include/defs.h"
#include "include/client.h"
#include "include/server.h"
#include "include/misc.h"
#include "include/gameserver.h"
#include "include/globals.h"
#include "include/zoning.h"
#include "include/item.h"
#include "include/inventory.h"

#include "include/chat.h"
#include "include/commands.h"
#include "include/clientmanager.h"
#include "include/msgbuilder.h"
#include "include/worldactors.h"
#include "include/npc.h"
#include "include/outpost.h"
#include "include/multipart.h"
#include "include/terminal.h"

#endif

