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

#include "def.h"

class PDefItems : public PDef
{
	private :

    //int mIndex;//1
    std::string mName;//2
    int mModel; // used for IG display
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
		//~PDefItems();

		bool LoadFromDef(PTokenList *Tokens);

		inline const std::string &GetName() const { return mName; }
        inline int GetModel() const { return mModel; }
		inline int GetType() const { return mType; }
		inline int GetValue1() const { return mValue1; }
		inline int GetValue2() const { return mValue2; }
		inline int GetValue3() const { return mValue3; }
		inline int GetSizeX() const { return mSizeX; }
		inline int GetSizeY() const { return mSizeY; }
		inline float GetWeight() const { return mWeight; }
		inline bool IsStackable() const { return (mStackable == 1); }
		inline float GetFillWeight() const { return mFillWeight; }
		inline int GetQualifier() const { return mQualifier; }
		inline int GetGfxMods() const { return mGfxMods; }
		inline int GetItemGroupID() const { return mItemGroupID; }
		inline int GetTextDescID() const { return mTextDescID; }
		inline int GetBasePrice() const { return mBasePrice; }
		inline int GetTechlevel() const { return mTechlevel; }
		inline int GetItemflags() const { return mItemflags; }
};


class PDefItemsMap : public PDefMap<PDefItems>
{
  private:
    std::map<int, PDefItems*>::const_iterator* mMapItCache;
    int mMapItCacheCount;
    std::map<int, std::vector<int> > mItemGroups;
    int mMaxItemGroupId;
    void BuildMapItCache();
    void BuildItemGroups();

  public:
    PDefItemsMap();
    ~PDefItemsMap();
    bool Load(const char* nName, const char* nFilename);
    const PDefItems* GetDefBySeqIndex( int nSeqIndex ) const;
    int GetRandomItemIdFromGroup( int nGroupId ) const;

    inline std::map<int, PDefItems*>::const_iterator ConstIteratorBegin() const { return mDefs.begin(); }
    inline std::map<int, PDefItems*>::const_iterator ConstIteratorEnd() const { return mDefs.end(); }
};

#endif
