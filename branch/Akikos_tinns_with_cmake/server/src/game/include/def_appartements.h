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
	def_appartements.h

	MODIFIED: 22 Sep 2006 Hammag
	REASON: - Creation
	
*/

#ifndef DEF_APPARTEMENTS_H
#define DEF_APPARTEMENTS_H

#include "def.h"

class PDefAppartement : public PDef
{
	private :
		//int mIndex;
		std::string mName;
		std::string mWorldName;
		int mValue;
		int mPlaceCount;
		int mPlace[8];
		int mFaction;

	public :
		PDefAppartement();
		//~PDefAppartement();

		bool LoadFromDef(PTokenList *Tokens);

		inline int GetID() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline const std::string &GetWorldName() const { return mWorldName; }
		inline int GetValue() const { return mValue; }
		inline int GetPlaceCount() const { return mPlaceCount; }
		inline int GetPlace(int nIdx) const { return ( (nIdx < mPlaceCount) ? mPlace[nIdx] : 0 ); }
		inline int GetFaction() const { return mFaction; }
};


class PDefAppartementsMap : public PDefMap<PDefAppartement>
{
  public:
    inline std::map<int, PDefAppartement*>::const_iterator ConstIteratorBegin() const { return mDefs.begin(); }
    inline std::map<int, PDefAppartement*>::const_iterator ConstIteratorEnd() const { return mDefs.end(); }
};

#endif
