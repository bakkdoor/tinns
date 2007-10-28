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
  container.h - base classe for containers
               

 MODIFIED: 28 Jul 2008 Hammag
 REASON: - creation

*/

#ifndef CONTAINER_H
#define CONTAINER_H

#define CONTAINER_MAX_SIZE 254

class PItem;

class PContainerEntry
{
  friend class PContainer;
  friend class PContainer2D;
  friend class PMsgBuilder;
  
  friend class PContainer2DWorkaround;
  
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
    
    PItem* mItem;
    u8  mPosX;
    u8  mPosY;
    u32 mInvID;
    bool mDirtyFlag;

    PContainerEntry(PItem* nItem, u8 X, u8 Y, u32 nInvID = 0, bool SetDirty = true);
    PContainerEntry(MYSQL_ROW row);
    
    bool SQLSave(u32 CharID, u32 InvLoc);
    bool SQLDelete();
    
    inline void Set2DPos(u8 nPosX, u8 nPosY) { mDirtyFlag = mDirtyFlag || (mPosX != nPosX) || (mPosY != nPosY) ; mPosX = nPosX; mPosY = nPosY; }
    
  public:
    ~PContainerEntry();
    
    inline void Get2DPos(u8* oPosX, u8* oPosY) { *oPosX = mPosX; *oPosY = mPosY; }
};



class PContainer // Holes allowed, no autofind free slots
{    
	protected:
    u8 mMaxSlots;
    std::vector< PContainerEntry* >* mContContent;
    u32 mCharID;
    u32 mInvLoc;
    u32 mExclusiveUseCharID;
    bool mDirtyFlag;
    
    inline bool IsSlotAllowed(u8 nSlotId) { return ((nSlotId < CONTAINER_MAX_SIZE) && (!mMaxSlots || (nSlotId < mMaxSlots))); }
    virtual bool AddEntry(PContainerEntry* NewEntry, u8 nSlotId = 0);
    virtual PContainerEntry* RemoveEntry(u8 nSlotId);
    virtual bool GetFreeSlot(u8* nSlotId);
		void Compact(u8 startSlotId = 0);
    
  public:
    PContainer(u8 nMaxSlots = 0);
    virtual ~PContainer();

    inline void SetInfo(u32 CharID, u32 InvLoc) { mCharID = CharID; mInvLoc = InvLoc; }
    inline u32 GetOwnerId() { return mCharID; }
    
    inline bool IsDirty() { return mDirtyFlag; }
    inline void SetDirty() { mDirtyFlag = true; }
    
    bool StartUse(u32 nExclusiveUseCharID = 0);
    virtual bool EndUse(u32 nExclusiveUseCharID = 0);

    bool SQLLoad();
    bool SQLSave();
    
    bool IsSlotFree(u8 nSlotId);
    virtual bool AddItem(PItem* NewItem, u32 nInvID = 0, u8 nPosX = 0, u8 nPosY = 0, bool SetDirty = true);
    
    virtual bool MoveItem(u8 srcSlotId, u8 nCount, u8 dstSlotId);
    bool MoveItem(u8 srcSlotId, u8 nCount, PContainer* dstContainer, u8 dstSlotId = 0, u8 nPosX = 0, u8 nPosY = 0);
	  virtual void SetEntryPosXY(PContainerEntry* nEntry, u8 nSlotId, u8 nPosX = 0, u8 nPosY = 0);

    virtual u8 RandomFill(u8 nItemCount = 0);
    
    PContainerEntry* GetEntry(u8 nSlotId);
    std::vector< PContainerEntry* >* GetEntries();
    PItem* GetItem(u8 nSlotId);
    
    virtual void Dump();
};


class PContainerWithHoles : public PContainer // Holes allowed, no autofind free slots
{   
  public:
    PContainerWithHoles(u8 nMaxSlots = 0) : PContainer(nMaxSlots){ nMaxSlots = nMaxSlots; }
    virtual ~PContainerWithHoles() {}
};


class PContainerAutoCompact : public PContainer // No holes allowed, automatic add to end slot (no control on insertion slot)
{
	protected:
    virtual PContainerEntry* RemoveEntry(u8 nSlotId);
    virtual bool GetFreeSlot(u8* nSlotId);
    
  public:
    PContainerAutoCompact(u8 nMaxSlots = 0) : PContainer(nMaxSlots){ nMaxSlots = nMaxSlots; }
    virtual ~PContainerAutoCompact() {}
		
    virtual bool MoveItem(u8 srcSlotId, u8 nCount, u8 dstSlotId); 
};


class PContainer2D : public PContainerAutoCompact // + slotId not used, non-significant XY used (no XY check yet)
{   
  public:
    PContainer2D(u8 nMaxSlots = 0) : PContainerAutoCompact(nMaxSlots){ nMaxSlots = nMaxSlots; }
    virtual ~PContainer2D() {}

		virtual void SetEntryPosXY(PContainerEntry* nEntry, u8 nSlotId, u8 nPosX = 0, u8 nPosY = 0);
};

class PContainer2DWorkaround : public PContainerWithHoles // Holes allowed, autofind free slot (always increasing id)
{
  private:
    u8 mNextFreeSlot;
    std::vector< std::vector<bool>* > mContSpace;
    u8 mMaxCols;
    u8 mMaxRows;
    u8 mRows;
    
    void AddRow();
      
    inline bool Is2DPosAllowed(u8 PosX, u8 PosY, u8 SizeX, u8 SizeY)
    {
      return ((PosX < mMaxCols-SizeX+1) && (PosY < mMaxRows-SizeY+1));
    }
    bool Is2DFree(u8 PosX, u8 PosY, u8 SizeX, u8 SizeY);
    bool FindValid2DPos(PContainerEntry* nEntry);
    

  protected:
    bool AddEntry(PContainerEntry* NewEntry, u8 nSlotId = 0);
    PContainerEntry* RemoveEntry(u8 nSlotId);
    bool GetFreeSlot(u8* nSlotId);
    
  public:
    PContainer2DWorkaround(u8 nMaxSlots = 0);
    ~PContainer2DWorkaround();
    
    bool MoveItem(u8 srcSlotId, u8 nCount, u8 dstSlotId);
    
    void Set2DPosMax(u8 MaxPosX, u8 MaxPosY = 254) { mMaxCols = MaxPosX; mMaxRows = MaxPosY; }
    void SetEntryPosXY(PContainerEntry* nEntry, u8 nSlotId, u8 nPosX = 0, u8 nPosY = 0);
    void SetUsed(PContainerEntry* nEntry, bool Value = true);
    void Dump();
};

class PContainerAutoFindFree : public PContainerWithHoles // No holes kept after EndUse, automatic find first free slots (no control on insertion slot)
{
  protected:
    virtual bool GetFreeSlot(u8* nSlotId);
    
  public:
    PContainerAutoFindFree(u8 nMaxSlots = 0) : PContainerWithHoles(nMaxSlots){ nMaxSlots = nMaxSlots; }
    virtual ~PContainerAutoFindFree() {}
};

class PContainerAutoCompactOnClose : public PContainerAutoFindFree // No holes kept after EndUse, automatic find first free slots (no control on insertion slot)
{    
  public:
    PContainerAutoCompactOnClose(u8 nMaxSlots = 0) : PContainerAutoFindFree(nMaxSlots){ nMaxSlots = nMaxSlots; }
    virtual ~PContainerAutoCompactOnClose() {}

    virtual bool EndUse(u32 nExclusiveUseCharID = 0);
}; 

#endif
