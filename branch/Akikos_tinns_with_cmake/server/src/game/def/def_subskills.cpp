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
        def_subskills.cpp

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

// subskill.def, names and parameters of all subskills

PDefSubSkill::PDefSubSkill()
{
	mActionModifiers = 0;
	mNumActionModifiers = 0;
}

PDefSubSkill::~PDefSubSkill()
{
	delete [] mActionModifiers;
}

bool PDefSubSkill::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0 :	// setentry
				continue;

			case 1 :
				mIndex = atoi(i->c_str()); break;

			case 2 :
				mName = *i; break;

			case 3 :
				mShortName = *i; break;

			case 4 :
				mStrengthenFactor = static_cast<float>(atof(i->c_str())); break;

			case 5 :
			{
				mNumActionModifiers = atoi(i->c_str());
				mActionModifiers = new int[mNumActionModifiers];
				std::memset(mActionModifiers, 0, sizeof(int)*mNumActionModifiers);
				break;
			}
		}

		if(Idx>=6)
			mActionModifiers[Idx-6] = atoi(i->c_str());
	}

	return true;
}

