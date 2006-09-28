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
	def_respawn.cpp

	MODIFIED: 22 Sep 2006 Hammag
	REASON: - Creation
	
*/

#include "main.h"

PDefRespawn::PDefRespawn()
{
	mIndex = 0;
}

PDefRespawn::~PDefRespawn()
{

}

bool PDefRespawn::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0:	// setentry
					continue;

			case 1:	// index
				mIndex = std::atoi(i->c_str()); break;

			case 2: // value
				mWorldID = std::atoi(i->c_str()); break;

			case 3: // value
				mEntityID = std::atoi(i->c_str()); break;

			case 4: // value
				mHazardLevel = std::atoi(i->c_str()); break;
				  				  				  				  
			case 5: // name
				mName = *i; break;

			case 6: // world name
				mFlag = *i; break;	      
		}
	}

	return true;
}
