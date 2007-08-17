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
  inventory.cpp - classes for inventories
                  (inventory, belt, armor, implants, gogo, processor(?), maybe other containers(?) )

 MODIFIED: 10 Jul 2006 Hammag
 REASON: - creation : Quick and dirty main inventory (backpack) management


*/

#include "main.h"
#include "inventory.h"
#include "container.h"


/* --- PInventory class --- */

PInventory::PInventory ()
{
  mWorn = new PContainerLinearSlots(INV_WORN_COLS, INV_WORN_MAXSLOTS);
  mBackpack = new PContainer2DAreas(INV_BACKPACK_COLS);
  mGogo = new PContainerLinearSlots(INV_GOGO_COLS, INV_GOGO_MAXSLOTS);
}

PInventory::~PInventory ()
{
  delete mWorn;
  delete mBackpack;
  delete mGogo;
}

void PInventory::SetCharId(u32 CharID)
{
  mWorn->SetInfo(CharID, INV_DB_LOC_WORN);
  mBackpack->SetInfo(CharID, INV_DB_LOC_BACKPACK);
  mGogo->SetInfo(CharID, INV_DB_LOC_GOGO);
}

bool PInventory::SQLLoad()
{
  //return mWorn->SQLLoad() && mBackpack->SQLLoad() && mGogo->SQLLoad();
  
bool ret = mWorn->SQLLoad() && mBackpack->SQLLoad() && mGogo->SQLLoad();
Console->Print(YELLOW, BLACK, "--- Worn Inventory ---");
mWorn->Dump();
Console->Print(YELLOW, BLACK, "--- Backpack Inventory ---");
mBackpack->Dump();
Console->Print(YELLOW, BLACK, "--- Gogo Inventory ---");
mGogo->Dump();
Console->Print(YELLOW, BLACK, "--- End Inventory ---");
return ret;
}

bool PInventory::SQLSave()
{
  return mWorn->SQLSave() && mBackpack->SQLSave() && mGogo->SQLSave();
}

bool PInventory::AddItem(PItem *NewItem, u32 nInvLoc, u32 nInvID, u8 nPosX, u8 nPosY, bool SetDirty)
{
  PContainer* destContainer;
  switch(nInvLoc)
  {
    case INV_LOC_WORN:
      destContainer = mWorn;
      break;
    case INV_LOC_BACKPACK:
      destContainer = mBackpack;
      break;
    case INV_LOC_GOGO:
      destContainer = mGogo;
      break;
    default:
      return false;
  }
  return destContainer->AddItem(NewItem, nInvID, nPosX, nPosY, SetDirty);
}

/*
bool PInventory::QB_IsFree(u8 nSlot)
{
    if(nSlot > 9)
        return false;

    //    if (gDevDebug) Console->Print("Checking QBSlot %d. Content: %d", nSlot, mQuickAccessBelt[nSlot]);
    if(mQuickAccessBelt[nSlot] == 0)
        return true;
    else
        return false;
}

void PInventory::QB_SetSlot(u8 nSlot, u16 nItemID)
{
    if(nSlot > 9)
        return;
    //    if (gDevDebug) Console->Print("Setting QBSlot %d. Newcontent: %d Oldcontent: %d", nSlot, nItemID, mQuickAccessBelt[nSlot]);
    mQuickAccessBelt[nSlot] = nItemID;
}

u16 PInventory::QB_GetSlot(u8 nSlot)
{
    if(nSlot > 9)
        return 0;
    //    if (gDevDebug) Console->Print("Getting QBSlot %d. Content: %d", nSlot, mQuickAccessBelt[nSlot]);
    return mQuickAccessBelt[nSlot];
}

void PInventory::QB_Move(u8 nSlotSRC, u8 nSlotDST)
{
    if(nSlotSRC > 9 || nSlotDST > 9)
        return;

    //    if (gDevDebug) Console->Print("Moving QBSlot %d [%d] to %d [%d]", nSlotSRC, mQuickAccessBelt[nSlotSRC], nSlotDST, mQuickAccessBelt[nSlotDST]);
    mQuickAccessBelt[nSlotDST] = mQuickAccessBelt[nSlotSRC];
    mQuickAccessBelt[nSlotSRC] = 0;
    //    if (gDevDebug) Console->Print("Moving done. %d [%d] %d [%d]", nSlotSRC, mQuickAccessBelt[nSlotSRC], nSlotDST, mQuickAccessBelt[nSlotDST]);
}
*/
