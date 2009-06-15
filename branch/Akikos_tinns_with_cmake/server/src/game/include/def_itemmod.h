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
 def_itemmod.h

    CREATED: 31 Mar 2009 Hammag
*/

#ifndef DEF_ITEMMOD_H
#define DEF_ITEMMOD_H

#include "def.h"

class PDefItemMod : public PDef
{
  private :
    //int mIndex;
	int mType; // if mType > 0, it refers to a weapon type from weapons.def +1
	//int mDuration; // "implant base build time" = ???? always 0 anyway...
	int mChangeNum;
    int mChangeTarget[4]; // qualifier id	0 - 7	item qualifier id	4=range	
    float mChangeValue[4];
	float mChangeScale[4];
	std::string mName;
	//int mIconId;

  public :
    PDefItemMod();
    //~PDefItemMod();

    bool LoadFromDef( PTokenList *Tokens );

    inline const std::string &GetName() const { return mName; }
    inline int GetType() const { return mType; }
	//inline int GetDuration() const { return mDuration; }
    inline int GetChangeNum() const { return mChangeNum; }
    inline int GetChangeTarget(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mChangeNum)) ? mChangeTarget[nIdx] : 0) ; }
	inline float GetChangeValue(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mChangeNum)) ? mChangeValue[nIdx] : 0) ; }
	inline float GetChangeScale(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mChangeNum)) ? mChangeScale[nIdx] : 0) ; }
};

#endif
