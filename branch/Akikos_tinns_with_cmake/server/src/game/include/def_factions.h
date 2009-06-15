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
	def_factions.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
*/

#ifndef DEF_FACTIONS_H
#define DEF_FACTIONS_H

#include "def.h"

static const int NUMFACTIONS = 19;	// for faction relations only

class PDefFaction : public PDef
{
	private :
		//int mIndex;
		std::string mName;
		int mStartValue;
		bool mAffected;
		int mRelations[NUMFACTIONS];
	public :
		PDefFaction();
		//~PDefFaction();

		bool LoadFromDef(PTokenList *Tokens);

		inline const std::string &GetName() const { return mName; }
		inline int GetStartValue() const { return mStartValue; }
		inline bool GetAffected() const { return mAffected; }
		int GetRelation(int Faction) const;
};

#endif

