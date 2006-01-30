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
        def_factions.cpp

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown

        MODIFIED: 25 Dec 2005 Namikon
        REASON: - Added GPL
*/

#include "tinns.h"

PDefFaction::PDefFaction()
{
	mIndex = 0;
	memset(mRelations, 0, sizeof(mRelations));
}

PDefFaction::~PDefFaction()
{
}

bool PDefFaction::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0 : // setfrac
				continue;

			case 1 :
				mIndex = std::atol(i->c_str()); break;

			case 2 :
				mStartValue = std::atol(i->c_str()); break;

			case 3 :
				mAffected = std::atol(i->c_str())!=0; break;

			default :
			{
				if(Idx-4 < NUMFACTIONS)
					mRelations[Idx-4]=std::atol(i->c_str()); break;
			}
		}
	}

	return true;
}

int PDefFaction::GetRelation(int Faction) const
{
	// faction 0 has no relations
	if(Faction <= 0 || Faction > NUMFACTIONS)
		return 0;

	return mRelations[Faction-1];
}

