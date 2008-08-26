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
	def_items.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
	
  MODIFIED: 10 Jul Hammag
  REASON: - Full Item Def implementation
*/

#ifndef DEF_ITEMS_H
#define DEF_ITEMS_H

#ifdef _MSC_VER
	#pragma once
#endif

class PDefItems
{
	private :

    int mIndex;//1
    std::string mName;//2
    //int mModel; // used for IG display
    int mType;
    int mValue1;
    int mValue2;
    int mValue3;
    // int mBmNum; // used IG for inventory display
    // int mmBmNumIndex; // used IG for inventory display
    int mSizeX;
    int mSizeY;	
    // int mSmallbmnum; // used IG for inventory display
    float mWeight;
    int mStackable;
    float mFillWeight;
    int mQualifier;
    int mGfxMods;
    int mItemGroupID;	
    int mTextDescID;
    int mBasePrice;
    int mTechlevel;
    int mItemflags;
    // std::mShortname; // used IG for display on inventories Icons

	public :
		PDefItems();
		~PDefItems();

		bool LoadFromDef(PTokenList *Tokens);

		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
    inline const int GetType() const { return mType; }
    inline const int GetValue1() const { return mValue1; }
    inline const int GetValue2() const { return mValue2; }
    inline const int GetValue3() const { return mValue3; }
    inline const int GetSizeX() const { return mSizeX; }
    inline const int GetSizeY() const { return mSizeY; }
    inline const float GetWeight() const { return mWeight; }
    inline const bool IsStackable() const { return (mStackable == 1); }
    inline const float GetFillWeight() const { return mFillWeight; }
    inline const int GetQualifier() const { return mQualifier; }
    inline const int GetGfxMods() const { return mGfxMods; }
    inline const int GetItemGroupID() const { return mItemGroupID; }	
    inline const int GetTextDescID() const { return mTextDescID; }
    inline const int GetBasePrice() const { return mBasePrice; }
    inline const int GetTechlevel() const { return mTechlevel; }
    inline const int GetItemflags() const { return mItemflags; }
};

#endif

