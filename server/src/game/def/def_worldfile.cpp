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
	def_worldfile.cpp (infos from worlds/worlds.ini)

	MODIFIED: 28 Sep 2007 Hammag
	REASON: - Creation
	
	NOTA: Some entries in worlds.ini share the same Id ...
	      e.g: 505: techtown_enter 1/1a/1b/2
	      As we don't know what it means & how it is supposed to be handled,
	      we keep the first one for the moment. (this is managed in gamedef.cpp)
	TODO: Add a config entry to select the Nth entry for each such case,
	      as well as a config entry to select the default entry to be kept (first/last)
*/

#include "main.h"

PDefWorldFile::PDefWorldFile()
{
	mIndex = 0;
	mFileInWorldsDir = false;
}

PDefWorldFile::~PDefWorldFile()
{

}

bool PDefWorldFile::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	int StartPos = 0;
	int TailLen = 0;
	int Len;
	
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0:	// setentry
				continue;

			case 1:	// index
				mIndex = atoi(i->c_str()); break;

			case 2: // name
			{
			  Len = (*i).length();

        while((StartPos = (*i).find("\\",StartPos))>-1)
          (*i)[StartPos]='/';
        StartPos = 0;
        
        if ((*i)[0] == '\"')
          StartPos = 1;
        
			  if ( (*i).substr(StartPos,9) == "./worlds/" )
			  {
			    mFileInWorldsDir = true;
			    StartPos += 9;
			  }
			  else if ( (*i).substr(StartPos,2) == "./" )
          StartPos += 2;
        
        if ((*i)[Len-1] == '\"')
          TailLen = 1;           
			  if ( (Len > (StartPos+TailLen+4)) && ((*i)[Len-TailLen-4] == '.') )
			    TailLen += 4;
			    
			  mName = (*i).substr(StartPos,Len-StartPos-TailLen); // remove prefix, extension and doublequotes				
				
				break;
			}
		}
	}

	return (Idx == 3);
}
