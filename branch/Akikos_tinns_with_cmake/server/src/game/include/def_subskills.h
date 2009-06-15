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
	def_subskill.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
*/

#ifndef DEF_SUBSKILLS_H
#define DEF_SUBSKILLS_H

#include "def.h"

class PDefSubSkill : public PDef
{
	private :
		//int mIndex;
		std::string mName;
		std::string mShortName;
		float mStrengthenFactor;
		int mNumActionModifiers;
		int *mActionModifiers;
	public :
		PDefSubSkill();
		~PDefSubSkill();

		bool LoadFromDef(PTokenList *Tokens);

		inline const std::string &GetName() const { return mName; }
		inline const std::string &GetShortName() const { return mShortName; }
		inline float GetStrengthenFactor() const { return mStrengthenFactor; }
		inline int GetNumActionModifiers() const { return mNumActionModifiers; }
		inline int GetActionModifier(int Index) const { return mActionModifiers[Index]; }
};

#endif

