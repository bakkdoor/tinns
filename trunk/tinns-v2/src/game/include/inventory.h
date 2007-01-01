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
  inventory.h - classes for inventories
                  (inventory, belt, armor, implants, gogo, processor(?), maybe other containers(?) )

	MODIFIED: 10 Jul 2006 Hammag
	REASON: - creation


*/

#ifndef INVENTORY_H
#define INVENTORY_H

#define INV_WIDTH 10
#define INV_LOC_BACKPACK 3

/* Game DB inventory::inv_loc values used by MaxxJagg:
		1: GoGo
		2: QB
		3: Inventory
		... ?

// values from item movement messages:
	// 1 = Ground
	// 2 = QB, Armor, Implants //0-9 QB, 12-24 Processor, 26-38 Implants, 39-43 Armor
	// 3 = Inventory
	// 4 = loot? or Box?
	// 5 = NPC trading
	// 18 = GoGo

		What is inventory::inv_type ?
		No storage place in DB for constructor-character id + other (see item.h)
*/

class PInventoryEntry
{
  private:
    class PItem* mItem;
    u8  mPosX;
    u8  mPosY;
    u32 mInvID;
    bool mDirtyFlag;

    PInventoryEntry(PItem* nItem, u8 X, u8 Y, u32 nInvID = 0, bool SetDirty = true);

    bool SQLSave(u32 CharID, u32 InvLoc);
    bool SQLDelete();

  friend class PInventory;
};

class PInventory
{
   private:
        enum {
            i_invid = 0,
            i_charid,
            i_invloc,
            i_x,
            i_y,
            i_itemid,
            i_type,
            i_qty,
            i_curdur,
            i_dmg,
            i_freq,
            i_hand,
            i_rng,
            i_maxdur
        };

    // the following choice of containers can (should) be improved later
    std::multimap< u32, PInventoryEntry*> mInvContent;
    std::vector< std::vector<bool>* > mInvSpace; //mInvSpace(1, std::vector<bool>(INV_WIDTH,0))
    u16 mQuickAccessBelt[10]; // 10 Slots, dont think we need to do this dynamic.. do we?
    int mRows;
    u8 mLastInsertX;
    u8 mLastInsertY;
    void AddRow();
    bool IsInvSpaceAvailable(u8 PosX, u8 PosY, u8 SizeX = 1, u8 SizeY = 1);
    void SetInvSpaceUsed(u8 PosX, u8 PosY, u8 SizeX = 1, u8 SizeY = 1, bool Value = true);

  public:
    PInventory();
    ~PInventory();
    bool SQLLoad(u32 CharID);
    bool SQLSave(u32 CharID);
    bool PutItem(PItem *NewItem, u32 nInvID = 0, u8 nPosX = 255, u8 nPosY = 255, bool SetDirty = true);
    //bool CheckItem(u32 ItemID, u8 StackSize = 1);
    //PItem *GetItem(u32 ItemID, u8 StackSize = 1);
    //PItem *GetItemByPos(u8 nPosX, u8 nPosY, u8 StackSize = 1);
    //bool MoveItem(u8 oPosX, u8 oPosY, u8 dPosX, u8 dPosY);

    bool QB_IsFree(u8 nSlot);
    void QB_SetSlot(u8 nSlot, u16 nItemID);
    u16 QB_GetSlot(u8 nSlot);
    void QB_Move(u8 nSlotSRC, u8 nSlotDST);
};

#endif
