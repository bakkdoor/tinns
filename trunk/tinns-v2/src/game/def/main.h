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
  MODIFIED: 22 Sep 2006 Hammag
	REASON: - added def_appartements related stuff
	        - added def_appplaces related stuff
	        - added def_respawn related stuff
	MODIFIED: 28 Sep 2006 Hammag
	REASON: - added def_worldfile (worlds.ini) related stuff	 	  
 	  
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

#define DEF_CHARACTERS "defs/characters.def"
#define DEF_SKILLS "defs/skills.def"
#define DEF_SUBSKILLS "defs/subskill.def"
#define DEF_CHARKINDS "defs/charkinds.def"
#define DEF_WORLDINFO "defs/worldinfo.def"
#define DEF_FRACTIONS "defs/fractions.def"
#define DEF_HACK "defs/hack.def"
#define DEF_ITEMS "defs/items.def"
#define DEF_WORLDMODEL "defs/worldmodel.def"
#define DEF_APPPLACES "defs/appplaces.def"
#define DEF_APPARTEMENTS "defs/appartements.def"
#define DEF_RESPAWN "defs/respawn.def"

#define WRLD_WORLDFILE "worlds/worlds.ini"

using namespace std;

#endif

