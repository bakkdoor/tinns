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
	def_appartements.cpp

	MODIFIED: 22 Sep 2006 Hammag
	REASON: - Creation
	
*/

#include "main.h"

PDefAppartement::PDefAppartement()
{
	mIndex = 0;
}

PDefAppartement::~PDefAppartement()
{

}

bool PDefAppartement::LoadFromDef(PTokenList *Tokens)
{
  mFaction = 0;
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0:	// setentry
					continue;

			case 1:	// index
				mIndex = std::atoi(i->c_str()); break;

			case 2: // name
				mName = *i; break;

			case 3: // world name
				mWorldName = *i; break;
				
			case 4: // value
				mValue = std::atoi(i->c_str()); break;

			case 5: // number of places
				mPlaceCount = std::atoi(i->c_str()); break;
				  
		  case 6:
		  case 7:
		  case 8:
		  case 9:
		  case 10:
		  case 11:
		  case 12:
		  case 13:
		    mPlace[Idx-6] = std::atoi(i->c_str()); break;
		      
			case 14: // faction if base appartement
				mFaction = std::atoi(i->c_str()); break;		      
		}
	}
//Console->Print("%04d:%s file:%s val:%d places:%d pl1:%d pl2:%d pl8:%d faction:%d",
//      mIndex, mName.c_str(), mWorldName.c_str(), mValue, mPlaceCount, mPlace[0], mPlace[1], mPlace[7], mFaction);
	return true;
}
