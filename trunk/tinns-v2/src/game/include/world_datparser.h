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
	world_datparser.h
	Class to parse .dat world files

	MODIFIED: 29 Sep 2006 Hammag
	REASON: - Creation
*/

#ifndef WORLD_DATPARSER_H
#define WORLD_DATPARSER_H

class PFile;

class PWorldDatParser
{
	private :
	  PFile* f;
	  
    bool ProcessSec2ElemType3(u32 nSize);
    
	public :
		PWorldDatParser();
		~PWorldDatParser();

		bool LoadDatFile(char* nFilename);
		
};

#endif

