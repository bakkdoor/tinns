/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community

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
  item.h - item class

	MODIFIED: 11 Jul 2006 Hammag
	REASON: - creation
	
        TODO: Add CreatorID (for "named" item), CurrentMunitionID, CurrentMunitionNb (for weapons & charge-items: muns, medkit, etc.) to DB, equipped/free slots, & corresponding code
*/

#ifndef ITEM_H
#define ITEM_H

#define MAX_ITEMSTACK_SIZE 250

class PItem
{
      private:
        u32 mItemID;
        bool mStackable;
        u8 mStackSize;
        
        //int mType;
        //int mValue1;
        //int mValue2;
        //int mValue3;
        u8 mSizeX;
        u8 mSizeY;	
        float mWeight;
        //float mFillWeight;
        //int mQualifier;
        //int mItemGroupID;	
        //int mBasePrice;
        //int mTechlevel;
        //int mItemflags;

        struct PItemQuality {
          u8 CurDuration;
          u8 MaxDuration;
          u8 Damages;
          u8 Frequency;
          u8 Handling;
          u8 Range;
        };
        
      public:
        PItem(u32 ItemID, u8 nStackSize = 1, u8 CurDur = 0, u8 MaxDur = 0, u8 Dmg = 0, u8 Freq = 0, u8 Hand = 0, u8 Rng = 0);
        //~PItem();
        
        PItemQuality Quality;
        
        inline u32 GetItemID() {return mItemID; }
        inline bool const IsStackable() { return mStackable; }
        inline u8 GetStackSize() { return mStackSize; }
        u8 AddToStack(u8 ItemNb); // return the nb of added items
        u8 TakeFromStack(u8 ItemNb); // return the nb of retreived items
        inline u8 const GetSizeX() { return mSizeX; }
        inline u8 const GetSizeY() { return mSizeY; }
        inline float const GetWeight() { return mWeight; }
        inline float const GetStackWeight() { return mWeight * mStackSize; }

};

#endif
