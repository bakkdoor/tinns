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

/* --- PInventoryEntry class --- */

PInventoryEntry::PInventoryEntry(PItem* nItem, u8 X, u8 Y, u32 nInvID, bool SetDirty)
{
  mItem = nItem;
  mPosX = X;
  mPosY = Y;
  mInvID = nInvID;
  mDirtyFlag = SetDirty;
}

bool PInventoryEntry::SQLSave(u32 CharID, u32 InvLoc)
{
  std::string query, queryv;

  if (mInvID)
  {
    query = "UPDATE inventory SET";

    query += Ssprintf(" inv_charid='%u',inv_loc='%u',inv_x='%u',inv_y='%u'", CharID, InvLoc, mPosX, mPosY);
    query += Ssprintf(",inv_itemid='%u',inv_qty='%u'", mItem->GetItemID(), mItem->GetStackSize());
    //query += Ssprintf(",inv_type='%u'", 0); // ???
    query += Ssprintf(",inv_curdur='%u'", (mItem->Quality).CurDuration);
    query += Ssprintf(",inv_dmg='%u'", (mItem->Quality).Damages);
    query += Ssprintf(",inv_freq='%u'", (mItem->Quality).Frequency);
    query += Ssprintf(",inv_hand='%u'", (mItem->Quality).Handling);
    query += Ssprintf(",inv_rng='%u'", (mItem->Quality).Range);
    query += Ssprintf(",inv_maxdur='%u'", (mItem->Quality).MaxDuration);

    query += Ssprintf(" WHERE inv_id='%u' LIMIT 1;", mInvID);
  }
  else
  {
    query = "INSERT INTO inventory (inv_id,inv_charid,inv_loc,inv_x,inv_y";
    queryv = ") VALUES (NULL"+Ssprintf(",'%u','%u','%u','%u'", CharID, InvLoc, mPosX, mPosY);

    query += ",inv_itemid,inv_qty,inv_type";
    queryv += Ssprintf(",'%u','%u','%u'", mItem->GetItemID(), mItem->GetStackSize(), 0);

    query += ",inv_curdur";
    queryv += Ssprintf(",'%u'", (mItem->Quality).CurDuration);
    query += ",inv_dmg";
    queryv += Ssprintf(",'%u'", (mItem->Quality).Damages);
    query += ",inv_freq";
    queryv += Ssprintf(",'%u'", (mItem->Quality).Frequency);
    query += ",inv_hand";
    queryv += Ssprintf(",'%u'", (mItem->Quality).Handling);
    query += ",inv_rng";
    queryv += Ssprintf(",'%u'", (mItem->Quality).Range);
    query += ",inv_maxdur";
    queryv += Ssprintf(",'%u'", (mItem->Quality).MaxDuration);

    query = query + queryv + ");";
  }

  if ( MySQL->GameQuery(query.c_str()) )
  {
      Console->Print(RED, BLACK, "PInventoryEntry::SQLSave could not add/update some inventory item in the database");
      Console->Print("Query was:");
      Console->Print("%s", query.c_str());
      MySQL->ShowGameSQLError();
      return false;
  }
  else
  {
    if (!mInvID)
      mInvID = MySQL->GetLastGameInsertId();
if (gDevDebug) Console->Print(GREEN, BLACK, "New item %d added to inventory DB", mInvID);
    mDirtyFlag = false;
    return true;
  }
}

bool PInventoryEntry::SQLDelete()
{
  std::string query;

  if (mInvID)
  {
    query = Ssprintf("DELETE FROM inventory WHERE inv_id='%u' LIMIT1", mInvID);

    if ( MySQL->GameQuery(query.c_str()) )
    {
      Console->Print(RED, BLACK, "PInventoryEntry::SQLDelete could not delete some inventory item from the database");
      Console->Print("Query was:");
      Console->Print("%s", query.c_str());
      MySQL->ShowGameSQLError();
      return false;
    }
    else
    {
if (gDevDebug) Console->Print(GREEN, BLACK, "Item %d deleted from inventory DB", mInvID);
      mInvID = 0;
      mDirtyFlag = false;
      return true;
    }
  }
  else
  {
    Console->Print(YELLOW, BLACK, "PInventoryEntry::SQLDelete: Item was not in inventory DB");
    return true;
  }
}

/* --- PInventory class --- */

PInventory::PInventory ()
{
  mRows=0;
  AddRow();
  for(int x = 0; x < 11; x++)
    mQuickAccessBelt[x] = 0;
}

PInventory::~PInventory ()
{
  int i;
  for( i = 0; i < mRows; i++)
    delete mInvSpace[i];

  std::multimap< u32, PInventoryEntry*>::iterator tmpIter;
  for(tmpIter = mInvContent.begin(); tmpIter != mInvContent.end(); tmpIter++)
  {
    delete tmpIter->second->mItem;
    delete tmpIter->second;
  }

}

void PInventory::AddRow()
{
  std::vector<bool>* NewRow = new std::vector<bool>(INV_WIDTH,0);
  mInvSpace.push_back(NewRow);
  ++mRows;
}

bool PInventory::IsInvSpaceAvailable(u8 PosX, u8 PosY, u8 SizeX, u8 SizeY)
{
  u8 h, v;

  for (v = 0; (v < SizeY) && (PosY+v < mRows) ; v++) // what is over existing rows is free
  {
    for (h = 0; h < SizeX; h++)
    {
      if(PosX+h >= INV_WIDTH) // what is over max col is not free
        return false;

      if((*mInvSpace[PosY+v])[PosX+h])
        return false;
    }
  }
  return true;
}

void PInventory::SetInvSpaceUsed(u8 PosX, u8 PosY, u8 SizeX, u8 SizeY, bool Value)
{
  u8 h, v;

  while (PosY+SizeY > mRows) // add new rows when needed
    AddRow();

  for (v = 0; (v < SizeY) && (PosY+v < mRows) ; v++)
  {
    for (h = 0; (h < SizeX) && (PosX+h < INV_WIDTH); h++)
      (*mInvSpace[PosY+v])[PosX+h] = Value;
  }
}

bool PInventory::SQLLoad(u32 CharID)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[1024];
    PItem* NewItem;
    u8 nPosX, nPosY, nStackSize;
    u32 nItemID, nInvID;
    u8 CurDur, Dmg, Freq, Hand, Rng, Dur;

    if (!mInvContent.empty())
    {
        Console->Print(RED, BLACK, "PInventory::SQLLoad: We don't want to load inventory on non-empty inventory for char %d", CharID);
        return false;
    }

    sprintf(query, "SELECT * FROM inventory WHERE (inv_charid='%d' and inv_loc='%d') LIMIT 1", CharID, INV_LOC_BACKPACK);
    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        //Console->Print(RED, BLACK, "Unable to load data from MySQL DB!");
        return false;
    }
    while((row = mysql_fetch_row(result)))
    {
        nPosX = std::atoi(row[i_x]);
        nPosY = std::atoi(row[i_y]);
        nItemID = std::atoi(row[i_itemid]);
        nStackSize = std::atoi(row[i_qty]);
        //     = std::atoi(row[i_type]);
        CurDur = std::atoi(row[i_curdur]);
        Dmg = std::atoi(row[i_dmg]);
        Freq = std::atoi(row[i_freq]);
        Hand = std::atoi(row[i_hand]);
        Rng = std::atoi(row[i_rng]);
        Dur = std::atoi(row[i_maxdur]);
        nInvID = std::atoi(row[i_invloc]);

        NewItem = new PItem(nItemID, nStackSize, CurDur, Dur, Dmg, Freq, Hand, Rng);
        if (!PutItem(NewItem, nInvID, nPosX, nPosY, false))
        {
            Console->Print(YELLOW, BLACK, "PInventory::SQLLoad: Can't load item %d in char %d inventory", nItemID, CharID);
            delete NewItem;
        }
    }
    MySQL->FreeGameSQLResult(result);
    return true;
}

bool PInventory::SQLSave(u32 CharID)
{
  std::multimap< u32, PInventoryEntry*>::iterator InvIter;
  PInventoryEntry* InvEntry;
  bool SavedOK = true;

  for(InvIter = mInvContent.begin(); InvIter != mInvContent.end(); InvIter++)
  {
    InvEntry = InvIter->second;
    if (InvEntry->mDirtyFlag)
      SavedOK = SavedOK && InvEntry->SQLSave(CharID, INV_LOC_BACKPACK);
  }

  return SavedOK;
}


bool PInventory::PutItem(PItem* NewItem, u32 nInvID, u8 nPosX, u8 nPosY, bool SetDirty)
{
  /* --- auto stacking not implemented yet
  1 - if stackable, check if same item(S) exists
  2 - - if exist
  3 - - - add to stack
  4 - - - if no new stack remains, update LastPutXY & return true
  5 - find an empty space
  6 -
  x - put in new space, update LastPutXY & return true

  */

  if (NewItem->GetItemID() == 0)
  {
   Console->Print(RED, BLACK, "PPInventory::PutItem: invalid item : ID 0");
   return false;
  }

  u8 x = 0, y = 0;
  bool Found = false;
  u8 SizeX = NewItem->GetSizeX();
  u8 SizeY = NewItem->GetSizeY();

  if ((nPosX < 255) && (nPosY < 255)) // if pos specified, try this pos then another one if not free
  {
    if (IsInvSpaceAvailable(nPosX, nPosY, SizeX, SizeY))
    {
      x = nPosX;
      y = nPosY;
      Found = true;
    }
    else
    {
        SetDirty = true;
    }
  }

    while (!Found)
    {
        for (x = 0; x <= (INV_WIDTH-SizeX); x++)
        {
        if (Found = IsInvSpaceAvailable(x, y, SizeX, SizeY))
            break;
        }
        if (Found)
        {
            break;
            SetDirty = true;
        }
        ++y;
    }

  SetInvSpaceUsed(x, y, SizeX, SizeY);
  PInventoryEntry* NewEntry = new PInventoryEntry(NewItem, x, y, nInvID, SetDirty);
  mLastInsertX = x;
  mLastInsertY = y;
  mInvContent.insert(std::make_pair(NewItem->GetItemID(), NewEntry));

  /* --- degug only ---
  Console->Print(GREEN, BLACK, "PPInventory::PutItem: added item %d at pos (%d,%d)", NewItem->GetItemID(), x, y);
  std::multimap< u32, PInventoryEntry*>::iterator tmpIter;
  PInventoryEntry* tmpEntry;
  PItem* tmpItem;
  for(tmpIter = mInvContent.begin(); tmpIter != mInvContent.end(); tmpIter++)
  {
    tmpEntry = tmpIter->second;
    tmpItem = tmpEntry->mItem;
    Console->Print(GREEN, BLACK, "(%d,%d) (%d x %d) Item %d (%s)", tmpEntry->mPosX, tmpEntry->mPosY, tmpItem->GetSizeX(), tmpItem->GetSizeY(), tmpItem->GetItemID(), (GameDefs->GetItemsDef(tmpItem->GetItemID())->GetName()).c_str());
  }

  std::string tmpS;
  for (y=0 ; y < mRows; y++)
  {
    tmpS = "";
    for (x=0 ; x < INV_WIDTH; x++)
    {
      tmpS += ((*mInvSpace[y])[x])?"X":".";
    }
    Console->Print("%s",tmpS.c_str());
  }
  Console->Print(GREEN, BLACK, "------------------------------------");
 ---            --- */

  return true;
}
/* --- not implemented yet
bool PInventory::CheckItem(u32 ItemID, u8 StackSize)
{

}

PItem *PInventory::GetItem(u32 ItemID, u8 StackSize)
{

}

PItem *PInventory::GetItemByPos(u8 nPosX, u8 nPosY, u8 StackSize)
{

}

bool PInventory::MoveItem(u8 oPosX, u8 oPosY, u8 dPosX, u8 dPosY)
{

}
*/
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
