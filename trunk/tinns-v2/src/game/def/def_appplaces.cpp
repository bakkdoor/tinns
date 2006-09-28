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
	def_appplaces.cpp

	Created: 21 Sep 2006 Hammag
	REASON: -
*/

#include "main.h"

PDefAppPlace::PDefAppPlace()
{
	mIndex = 0;
}

PDefAppPlace::~PDefAppPlace()
{

}

bool PDefAppPlace::LoadFromDef(PTokenList *Tokens)
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

			case 2: // name
				mName = *i; break;

			case 3: // use flag
				mExitWorldID = std::atoi(i->c_str()); break;
				
			case 4: // function type
				mExitWorldEntity = std::atoi(i->c_str()); break;

			case 5: // function value
				mSewerLevel = std::atoi(i->c_str()); break;
		}
	}

	return true;
}
