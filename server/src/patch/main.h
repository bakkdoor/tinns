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
    MODIFIED: 6 Jul 2006 Hammag
	  REASON: - moved include "types.h" before include "../netcode/main.h" to permit compile
*/

#ifndef MAIN_H
#define MAIN_H

//#include "version.h"

#define PATCH_PORT 7000
#define MAX_PATCH_CLIENTS 50

//basic includes
#include "external.h"

//tinns includes
#include "types.h"
#include "netcode.h"

#include "console.h"
#include "config.h"
#include "filesystem.h"
#include "client.h"
#include "server.h"
#include "patchserver.h"
#include "misc.h"
#include "globals.h"

using namespace std;

#endif

