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

//NC containers message locations
#define INV_LOC_GROUND 1
#define INV_LOC_WORN 2
#define INV_LOC_BACKPACK 3
#define INV_LOC_BOX 4
#define INV_LOC_NPCTRADE 5
#define INV_LOC_GOGO 18
#define INV_LOC_BOX2 255
//PC-Trade window = ?

//Inventory containers info
#define INV_WORN_QB_START 0
#define INV_WORN_QB_END 9
#define INV_WORN_QB_HAND 255
#define INV_WORN_QB_NONE 99

#define INV_WORN_PROC_START 12
#define INV_WORN_PROC_END 24

#define INV_WORN_IMP_START 26
#define INV_WORN_IMP_END 38

#define INV_WORN_ARMOR_START 39
#define INV_WORN_ARMOR_END 43

#define INV_WORN_COLS 44
#define INV_WORN_MAXSLOTS 44


#define INV_BACKPACK_COLS 10

#define INV_GOGO_COLS 5
#define INV_GOGO_MAXSLOTS 50

#define INV_CABINET_COLS 5
#define INV_CABINET_MAXSLOTS 33

// inv_loc values in database
#define INV_DB_LOC_GOGO 1
#define INV_DB_LOC_WORN 2
#define INV_DB_LOC_BACKPACK 3

class PItem;
class PContainer;
class PContainerWithHoles;
class PContainer2DWorkaround;
class PContainerAutoFindFree;

class PInventory
{
  private:
    PContainerWithHoles* mWorn; // PContainerLinearSlots
    PContainer2DWorkaround* mBackpack; // PContainer2DAreas
    PContainerAutoFindFree* mGogo; // PContainerLinearSlots
    
  public:

    PInventory();
    ~PInventory();
    
    void SetCharId(u32 CharID);
    bool SQLLoad();
    bool SQLSave();
    PContainer* GetContainer(u8 nInvLoc);
    inline PContainer2DWorkaround* GetBackpackContainer() { return mBackpack; }

    bool IsDirty() const;
      
    bool AddItem(PItem* NewItem, u8 nInvLoc = INV_LOC_BACKPACK, u32 nInvID = 0, u8 nPosX = 0, u8 nPosY = 0, bool SetDirty = true);
    //bool CheckItem(u32 ItemID, u8 StackSize = 1);
    //PItem *GetItem(u32 ItemID, u8 StackSize = 1);
    //PItem *GetItemByPos(u8 nPosX, u8 nPosY, u8 StackSize = 1);
    //bool MoveItem(u8 oPosX, u8 oPosY, u8 dPosX, u8 dPosY);

    //bool QB_IsFree(u8 nSlot);
    //void QB_SetSlot(u8 nSlot, u16 nItemID);
    //u16 QB_GetSlot(u8 nSlot);
    //void QB_Move(u8 nSlotSRC, u8 nSlotDST);
};

#endif
