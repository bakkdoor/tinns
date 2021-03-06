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

//Type
#define ITEM_TYPE_VARIOUS 0
#define ITEM_TYPE_WEAPON 1
#define ITEM_TYPE_AMMO 2
#define ITEM_TYPE_HEALTH 3
#define ITEM_TYPE_IMPLANT 4
#define ITEM_TYPE_DRUG 5
#define ITEM_TYPE_MOD 6
#define ITEM_TYPE_GFXMOD 7
#define ITEM_TYPE_BLUEPRINT 8
#define ITEM_TYPE_ARMOR 9
#define ITEM_TYPE_PSIMOD 10
#define ITEM_TYPE_PSIMODREADY 11
#define ITEM_TYPE_REPAIR 12
#define ITEM_TYPE_RECYCLER 13
#define ITEM_TYPE_DATACUBE 14
#define ITEM_TYPE_CONSTRUCTOR 15
#define ITEM_TYPE_RESEARCHER 16
#define ITEM_TYPE_IMPLANTER 17
#define ITEM_TYPE_APARTMENTKEY 18
#define ITEM_TYPE_CLANKEY 19
#define ITEM_TYPE_CASHCUBE 20
#define ITEM_TYPE_AUTOWEAPON 21
#define ITEM_TYPE_VHCKEY 22
#define ITEM_TYPE_UNIDENTPART 24
#define ITEM_TYPE_WRECKEDPART 25
#define ITEM_TYPE_SALVAGE 26
#define ITEM_TYPE_VHCCOMPONENT 27
#define ITEM_TYPE_RECORDABLE 28

																		

// gfxmodflags																						
#define ITEM_MOD_FLASHLIGHT 1	
#define ITEM_MOD_SCOP 2
#define ITEM_MOD_SILENCER 4
#define ITEM_MOD_LASERPOINTER 8

// itemflags:
#define ITEM_FLAG_RESEARCHABLE 1
#define ITEM_FLAG_NO_DROP 2
#define ITEM_FLAG_NO_MAX_REPAIRE_DECAY 4
#define ITEM_FLAG_AMMO 8 // for loadable ammo
// not sure for ITEM_FLAG_AMMO

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
    void MakeItemStandard(u8 GlobalQualityMin = 120, u8 GlobalQualityMax = 180);
        
    inline u32 GetItemID() {return mItemID; }
    
    inline int GetType() { return mDefItem->GetType(); }
    inline u8 GetItemflags() { return mDefItem->GetItemflags(); }
    inline const std::string &GetName() const { return mDefItem->GetName(); }
    inline u8 GetSizeX() { return mDefItem->GetSizeX(); }
    inline u8 GetSizeY() { return mDefItem->GetSizeY(); }
    inline float GetWeight() { return mStackSize * mDefItem->GetWeight(); }
    inline float GetSingleUnitWeight() { return mDefItem->GetWeight(); }
    inline float GetFillWeight() { return mDefItem->GetFillWeight(); }
    inline u32 GetBasePrice() { return mDefItem->GetBasePrice(); }
    inline u16 GetTechlevel() { return mDefItem->GetTechlevel(); }
    inline int GetValue1() { return mDefItem->GetValue1(); }
    inline int GetValue2() { return mDefItem->GetValue2(); }
    inline int GetValue3() { return mDefItem->GetValue3(); }
    inline int GetQualifier() { return mDefItem->GetQualifier(); }        
    
    inline bool IsStackable() { return mDefItem->IsStackable(); }
    inline u8 GetStackSize() { return mStackSize; }
    u8 AddToStack(u8 ItemNb); // return the nb of items NOT added
    u8 TakeFromStack(u8 ItemNb); // return the nb of retreived items        

    //mItemGroupID = def->GetItemGroupID();
    
};

#endif
