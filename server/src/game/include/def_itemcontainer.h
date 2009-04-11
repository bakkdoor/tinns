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
 def_itemcontainer.h

    CREATED: 02 Apr 2009 Hammag
*/

#ifndef DEF_ITEMCONTAINER_H
#define DEF_ITEMCONTAINER_H

#include "def.h"

class PDefItemContainer : public PDef
{
  private :
    //int mIndex;
	int mNumItemsAtOnce;
	int mRespawnTime;
	int mNumItems;
    int mItemId[6]; // <0: Group, >0: Item
    float mQuality[6]; // usually <1, <=> % ?
	int mChance[6]; // a weight, not a %

    int mCumulatedChance[6]; // Computed data in order to select random item entry

    void BuildCumulatedChance();

  public :
    PDefItemContainer();
    //~PDefItemContainer();

    bool LoadFromDef( PTokenList *Tokens );

	inline int GetNumItemsAtOnce() const { return mNumItemsAtOnce; }
	inline int GetRespawnTime() const { return mRespawnTime; }
    inline int GetNumItems() const { return mNumItems; }
    inline int GetItemId(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumItems)) ? mItemId[nIdx] : 0) ; }
	inline float GetQuality(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumItems)) ? mQuality[nIdx] : 0) ; }
	inline int GetChance(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumItems)) ? mChance[nIdx] : 0) ; }
    int GetRandomItemIdx() const; // Return Idx based on Entry chance
};

#endif
