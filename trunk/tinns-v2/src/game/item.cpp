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

PItem::PItem(u32 ItemID, u8 nStackSize, u8 CurDur, u8 MaxDur, u8 Dmg, u8 Freq, u8 Hand, u8 Rng)
{
  const PDefItems *def = GameDefs->GetItemsDef(ItemID);
  if (def) {
    mItemID = ItemID;
    mStackable = def->IsStackable();
    mStackSize = nStackSize;
    
    //mType = def->GetType();
    //mValue1 = def->GetValue1();
    //mValue2 = def->GetValue2();
    //mValue3 = def->GetValue3();
    mSizeX = def->GetSizeX();
    mSizeY = def->GetSizeY();	
    mWeight = def->GetWeight();
    //mFillWeight = def->GetFillWeight();
    //mQualifier = def->GetQualifier();
    //mItemGroupID = def->GetItemGroupID();	
    //mBasePrice = def->GetBasePrice();
    //mTechlevel = def->GetTechlevel();
    //mItemflags = def->GetItemflags();

    if (!mStackable) // only non-stackable items can have quality stats
    {
      mStackSize = 1;
      Quality.CurDuration = CurDur;
      Quality.MaxDuration = MaxDur;
      Quality.Damages = Dmg;
      Quality.Frequency = Freq;
      Quality.Handling = Hand;
      Quality.Range = Rng;
    }
  }
  else
  {
    mItemID = 0;
    Console->Print(YELLOW, BLACK, "PItem::PItem: Invalid item ID %d", ItemID);
  }
}

u8 PItem::AddToStack(u8 ItemNb)
{
  u8 addedItems = 0;
  if (mStackable)
  {
    addedItems = min(mStackSize + ItemNb, MAX_ITEMSTACK_SIZE) - mStackSize;
    mStackSize += addedItems;
  }
  return addedItems;
}

u8 PItem::TakeFromStack(u8 ItemNb)
{
  u8 retreivedItems = min(mStackSize, ItemNb);
  mStackSize -= retreivedItems;
  return retreivedItems; 
}
