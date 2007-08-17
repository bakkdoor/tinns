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
  friend class PContainerEntry;
  friend class PMsgBuilder;

  private:
    u32 mItemID;
    const PDefItems* mDefItem;
    
    bool mStackable;
    u8 mStackSize;
    
    u32 mLoadedAmmoId;
    u8 mLoadedAmmoNb;

    u8 mPropertiesFlags;
    
    u8 mCurDuration;
    u8 mMaxDuration;
    u8 mDamages;
    u8 mFrequency;
    u8 mHandling;
    u8 mRange;
    
    u8 mUsedSlots;
    u8 mMaxSlots;
    u8 mSlot[5];
    u8 mModificators;
    
    u32 mConstructorId;
    
  public:
    PItem(u32 ItemID, u8 nStackSize = 1, u8 CurDur = 0, u8 MaxDur = 0, u8 Dmg = 0, u8 Freq = 0, u8 Hand = 0, u8 Rng = 0);
    //~PItem();
    
    inline u32 GetItemID() {return mItemID; }
    
    inline int const GetType() { return mDefItem->GetType(); }
    inline u8 const GetItemflags() { return mDefItem->GetItemflags(); }
    
    inline u8 const GetSizeX() { return mDefItem->GetSizeX(); }
    inline u8 const GetSizeY() { return mDefItem->GetSizeY(); }
    inline float const GetWeight() { return mStackSize * mDefItem->GetWeight(); }
    inline float const GetSingleUnitWeight() { return mDefItem->GetWeight(); }
    inline float const GetFillWeight() { return mDefItem->GetFillWeight(); }
    inline u32 const GetBasePrice() { return mDefItem->GetBasePrice(); }
    inline u16 const GetTechlevel() { return mDefItem->GetTechlevel(); }
    inline int const GetValue1() { return mDefItem->GetValue1(); }
    inline int const GetValue2() { return mDefItem->GetValue2(); }
    inline int const GetValue3() { return mDefItem->GetValue3(); }
    inline int const GetQualifier() { return mDefItem->GetQualifier(); }        
    
    inline bool const IsStackable() { return mDefItem->IsStackable(); }
    inline u8 GetStackSize() { return mStackSize; }
    u8 AddToStack(u8 ItemNb); // return the nb of added items
    u8 TakeFromStack(u8 ItemNb); // return the nb of retreived items        

    //mItemGroupID = def->GetItemGroupID();
    
    //mBasePrice = def->GetBasePrice();
    //mTechlevel = def->GetTechlevel();
    //mItemflags = def->GetItemflags();

};

/*
mType:
ITEMTYPE_WEAPON  1
ITEMTYPE_AMMO  2
ITEMTYPE_HEALTH  3
ITEMTYPE_IMPLANT 4
ITEMTYPE_DRUG  5
ITEMTYPE_MOD  6
ITEMTYPE_GFXMOD  7
ITEMTYPE_BLUEPRINT 8
ITEMTYPE_ARMOR  9
ITEMTYPE_PSIMOD  10
ITEMTYPE_PSIMODREADY 11
ITEMTYPE_REPAIR  12
ITEMTYPE_RECYCLER 13
ITEMTYPE_DATACUBE 14																		

// gfxmodflags: moegliche weapon enhancements werte addieren 																						
1 : flashlight weapon enhancement	
2 : scope weapon enhancement
4 : silencer weapon enhancement
8 : laserpointer weapon enhancement

// itemflags:
1 : researchable
2 : no drop
4 : no max repair decay
8 : for ammos  / can reload weapons ?

*/
#endif
