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

#include "main.h"

PDefFaction::PDefFaction()
{
	memset(mRelations, 0, sizeof(mRelations)); // ... array members supposed to by auto-initialized by C++
}

bool PDefFaction::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
	    // setfracc	19	"Monster"	-1024	0	0	-1024	-1024	-1024	-1
		switch(Idx)
		{
			case 0 : // setfrac
				continue;

			case 1 :
				mIndex = atol(i->c_str()); break;

            case 2 :
                mName = *i;
                CleanUpString(&mName);
                break;

			case 3 :
				mStartValue = atol(i->c_str()); break;

			case 4 :
				mAffected = atol(i->c_str())!=0; break;

			case 5 :
				mSL = atol(i->c_str()); break;

			default :
			{
				if(Idx-6 < NUMFACTIONS)
					mRelations[Idx-6] = atol(i->c_str()); break;
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

