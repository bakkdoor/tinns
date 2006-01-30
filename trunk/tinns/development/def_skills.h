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
	def_skills.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
*/

#ifndef DEF_SKILLS_H
#define DEF_SKILLS_H

#ifdef _MSC_VER
	#pragma once
#endif

class PDefSkill
{
	private :
		int mIndex;
		std::string mName;
		std::string mShortName;
		int mNumSubSkills;
		int *mSubSkills;
	public :
		PDefSkill();
		~PDefSkill();

		bool LoadFromDef(PTokenList *Tokens);

		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline const std::string &GetShortName() const { return mShortName; }
		inline int GetNumSubSkills() const { return mNumSubSkills; }
		inline int GetSubSkill(int Index) const { return mSubSkills[Index]; }
};

#endif

