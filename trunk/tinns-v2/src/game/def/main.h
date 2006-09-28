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
	MODIFIED: 21 Sep 2006 Hammag
	REASON: - added def_worldmodels related stuff   	  
*/

#ifndef MAIN_H
#define MAIN_H

#include "version.h"

//basic includes
#include "external.h"

//tinns includes
#include "types.h"
#include "config.h"

#include "console.h"
#include "misc.h"

#include "globals.h"

#include "filesystem.h"
#include "defparser.h"
#include "defs.h"

#define DEF_CHARACTERS "characters.def"
#define DEF_SKILLS "skills.def"
#define DEF_SUBSKILLS "subskill.def"
#define DEF_CHARKINDS "charkinds.def"
#define DEF_WORLDINFO "worldinfo.def"
#define DEF_FRACTIONS "fractions.def"
#define DEF_HACK "hack.def"
#define DEF_ITEMS "items.def"
#define DEF_WORLDMODEL "worldmodel.def"
#define DEF_APPPLACES "appplaces.def"
#define DEF_APPARTEMENTS "appartements.def"
#define DEF_RESPAWN "respawn.def"

using namespace std;

#endif

