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
        def_skills.cpp

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

// skill.def includes skill names and skill/subskill mapping

PDefSkill::PDefSkill()
{
	mSubSkills = 0;
	mNumSubSkills = 0;
}

PDefSkill::~PDefSkill()
{
	delete [] mSubSkills;
}

bool PDefSkill::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0 :	// setentry
				continue;

			case 1 :
				mIndex = atol(i->c_str()); break;

			case 2 :
				mName = *i; break;

			case 3 :
				mShortName = *i; break;

			case 4 :
			{
				mNumSubSkills = atol(i->c_str());
				mSubSkills = new int[mNumSubSkills];
				std::memset(mSubSkills, 0, sizeof(int)*mNumSubSkills);
				break;
			}
		}

		if(Idx >= 5)
		{
			mSubSkills[Idx-5] = atoi(i->c_str());
			if(!GameDefs->GetSubSkillDef(mSubSkills[Idx-5]))
			{
				Console->Print("Skill def error: skill %s refers to nonexistant subskill %i", mShortName.c_str(), mSubSkills[Idx-5]);
				return false;
			}
		}
	}

	if(Idx-5 != mNumSubSkills)
	{
		Console->Print("Warning: skill %s has incorrect number of subskills", mShortName.c_str());
		Console->Print("  Expected %i, available %i", mNumSubSkills, Idx-5);
	}
	return true;
}
