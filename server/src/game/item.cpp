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
  item.cpp - item class

	MODIFIED: 11 Jul 2006 Hammag
	REASON: - creation
	
*/

#include "main.h"

#include "item.h"
#include "gamedefs.h"
#include "def_items.h"

PItem::PItem(u32 ItemID, u8 nStackSize, u8 CurDur, u8 MaxDur, u8 Dmg, u8 Freq, u8 Hand, u8 Rng)
{
  mDefItem = GameDefs->GetItemsDef(ItemID);
  if (mDefItem) {
    mItemID = ItemID;
    mStackable = mDefItem->IsStackable();
    mStackSize = mStackable ? nStackSize : 1;

    mLoadedAmmoId = 0;
    mLoadedAmmoNb = 0;

    mPropertiesFlags = 0;

    mUsedSlots = mMaxSlots = 0;
    mModificators = 0;
    
    mConstructorId = 0;
    //mType = mDefItem->GetType();
    //mValue1 = mDefItem->GetValue1();
    //mValue2 = mDefItem->GetValue2();
    //mValue3 = mDefItem->GetValue3();
    //mSizeX = mDefItem->GetSizeX();
    //mSizeY = mDefItem->GetSizeY();	
    //mWeight = mDefItem->GetWeight();
    //mFillWeight = mDefItem->GetFillWeight();
    //mQualifier = mDefItem->GetQualifier();
    //mItemGroupID = mDefItem->GetItemGroupID();	
    //mBasePrice = mDefItem->GetBasePrice();
    //mTechlevel = mDefItem->GetTechlevel();
    //mItemflags = mDefItem->GetItemflags();

    //if (!mStackable) // only non-stackable items can have quality stats (?)
    //{
    //  mStackSize = 1;
      mCurDuration = CurDur;
      mMaxDuration = MaxDur;
      mDamages = Dmg;
      mFrequency = Freq;
      mHandling = Hand;
      mRange = Rng;
    //}
  }
  else
  {
    mItemID = 0;
    Console->Print(YELLOW, BLACK, "PItem::PItem: Invalid item ID %d", ItemID);
  }
}

void PItem::MakeItemStandard(u8 GlobalQualityMin, u8 GlobalQualityMax)
{
  if(GlobalQualityMin > GlobalQualityMax) GlobalQualityMin = GlobalQualityMax;

  mCurDuration = 255;
  mMaxDuration = 255;
  if(GlobalQualityMin == GlobalQualityMax)
  {
	mDamages = GlobalQualityMin;
	mFrequency = GlobalQualityMin;
	mHandling = GlobalQualityMin;
	mRange = GlobalQualityMin;
  }
  else
  {
	mDamages = (u8) GameServer->GetRandom(GlobalQualityMax, GlobalQualityMin);
	mFrequency = (u8) GameServer->GetRandom(GlobalQualityMax, GlobalQualityMin);
	mHandling = (u8) GameServer->GetRandom(GlobalQualityMax, GlobalQualityMin);
	mRange = (u8) GameServer->GetRandom(GlobalQualityMax, GlobalQualityMin);
  }
  
}

u8 PItem::AddToStack(u8 ItemNb) // returns nb of items not added
{
  u8 addedItems = 0;
  if (mStackable)
  {
    addedItems = (ItemNb <= (MAX_ITEMSTACK_SIZE - mStackSize)) ? ItemNb : (MAX_ITEMSTACK_SIZE - mStackSize);
    mStackSize += addedItems;
  }
  return (ItemNb - addedItems);
}

u8 PItem::TakeFromStack(u8 ItemNb)
{
  u8 retreivedItems = 0;
  if (mStackable)
  {
	retreivedItems = min(mStackSize, ItemNb);
	mStackSize -= retreivedItems;
  }
  return retreivedItems; 
}
