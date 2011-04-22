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
	container.cpp - base classe for containers

	MODIFIED: 28 Jul 2008 Hammag
	REASON: - creation
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
		- clean ups
*/

#include "main.h"

#include "include/item.h"
#include "include/container.h"

/* --- PContainerEntry class --- */

PContainerEntry::PContainerEntry( PItem* nItem, u8 X, u8 Y, u32 nInvID, bool SetDirty )
{
  mItem = nItem;
  mPosX = X;
  mPosY = Y;
  mInvID = nInvID;
  mDirtyFlag = SetDirty;
}

PContainerEntry::PContainerEntry( MYSQL_ROW row )
{
  mItem = NULL;
  mInvID = atoi( row[i_invid] );
  mPosX = atoi( row[i_x] );
  mPosY = atoi( row[i_y] );
  mDirtyFlag = false;

  u32 nItemID = atoi( row[i_itemid] );
  u8 nStackSize = atoi( row[i_qty] );
  //     = std::atoi(row[i_type]);
  u8 CurDur = atoi( row[i_curdur] );
  u8 Dmg = atoi( row[i_dmg] );
  u8 Freq = atoi( row[i_freq] );
  u8 Hand = atoi( row[i_hand] );
  u8 Rng = atoi( row[i_rng] );
  u8 Dur = atoi( row[i_maxdur] );

  mItem = new PItem( nItemID, nStackSize, CurDur, Dur, Dmg, Freq, Hand, Rng );
}

PContainerEntry::~PContainerEntry()
{
  delete mItem;
}

bool PContainerEntry::SQLSave( u32 CharID, u32 InvLoc )
{
  std::string query, queryv;

  queryv += Ssprintf( " inv_charid='%u',inv_loc='%u',inv_x='%u',inv_y='%u'", CharID, InvLoc, mPosX, mPosY );
  queryv += Ssprintf( ",inv_itemid='%u',inv_qty='%u'", mItem->mItemID, mItem->mStackSize );
  //queryv += Ssprintf( ",inv_type='%u'", 0 );
  queryv += Ssprintf( ",inv_cdur='%u'", mItem->mCurDuration );
  queryv += Ssprintf( ",inv_dmg='%u'", mItem->mDamages );
  queryv += Ssprintf( ",inv_frq='%u'", mItem->mFrequency );
  queryv += Ssprintf( ",inv_hnd='%u'", mItem->mHandling );
  queryv += Ssprintf( ",inv_rng='%u'", mItem->mRange );
  queryv += Ssprintf( ",inv_mdur='%u'", mItem->mMaxDuration );

  if ( mInvID )
  {
    query = "UPDATE inventory SET " + queryv;
    query += Ssprintf( " WHERE inv_id='%u' LIMIT 1;", mInvID );
  }
  else
  {
    query = "INSERT INTO inventory SET " + queryv + ";";
  }

  if ( MySQL->GameQuery( query.c_str() ) )
  {
    Console->Print( RED, BLACK, "PContainerEntry::SQLSave could not add/update some inventory item in the database" );
    Console->Print( "Query was:" );
    Console->Print( "%s", query.c_str() );
    MySQL->ShowGameSQLError();
    return false;
  }
  else
  {
    if ( !mInvID )
    {
      mInvID = MySQL->GetLastGameInsertId();
      if ( gDevDebug )
        Console->Print( GREEN, BLACK, "New item %d added to inventory DB", mInvID );
    }
//Console->Print(YELLOW, BLACK, "PContainerEntry::SQLSave - Query was:");
//Console->Print(YELLOW, BLACK, "%s", query.c_str());
    mDirtyFlag = false;
    return true;
  }
}

bool PContainerEntry::SQLDelete()
{
  std::string query;

  if ( mInvID )
  {
    query = Ssprintf( "DELETE FROM inventory WHERE inv_id='%u' LIMIT 1;", mInvID );

    if ( MySQL->GameQuery( query.c_str() ) )
    {
      Console->Print( RED, BLACK, "PContainerEntry::SQLDelete could not delete some container item from the database" );
      Console->Print( "Query was:" );
      Console->Print( "%s", query.c_str() );
      MySQL->ShowGameSQLError();
      return false;
    }
    else
    {
      if ( gDevDebug )
        Console->Print( GREEN, BLACK, "Item %d deleted from container DB", mInvID );
      mInvID = 0;
      delete mItem;
      mItem = NULL;

      mDirtyFlag = false;
      return true;
    }
  }
  else
  {
    Console->Print( YELLOW, BLACK, "PContainerEntry::SQLDelete: Item was not in inventory DB" );
    return true;
  }
}

/* --- PContainer class --- */

PContainer::PContainer( u8 nMaxSlots )
{
  mMaxSlots = ( nMaxSlots > CONTAINER_MAX_SIZE ? CONTAINER_MAX_SIZE : nMaxSlots ) ;
  if ( mMaxSlots )
    mContContent = new std::vector< PContainerEntry* >( mMaxSlots, NULL );
  else
    mContContent = new std::vector< PContainerEntry* >;
  mCharID = 0;
  mInvLoc = 0;
  mExclusiveUseCharID = 0;
  mDirtyFlag = false;
}

PContainer::~PContainer()
{
  for ( u8 i = 0; i < mContContent->size(); ++i )
    delete mContContent->at( i );

  if ( mExclusiveUseCharID )
    Console->Print( RED, BLACK, "[ERROR] PContainer::~PContainer: Char %d still has exclusive access to container !!! Bad Pointer error will happen.", mExclusiveUseCharID );
}

bool PContainer::AddEntry( PContainerEntry* NewEntry, u8 nSlotId )
{
  if ( IsSlotAllowed( nSlotId ) )
  {
    for ( u8 i = mContContent->size(); i <= nSlotId; ++i ) // Extend as needed
      mContContent->push_back( NULL );
    if ( mContContent->at( nSlotId ) )
    {
      Console->Print( RED, BLACK, "[Warning] PContainer::AddEntry: Target entry already %d in use !!!", nSlotId );
      return false;
    }
    mContContent->at( nSlotId ) = NewEntry;
    mDirtyFlag = true;
    this->SetEntryPosXY( NewEntry, nSlotId, NewEntry->mPosX, NewEntry->mPosY );
    return true;
  }
  else
  {
    Console->Print( RED, BLACK, "[Warning] PContainer::AddEntry: Target entry %d not allowed (max = %d) !!!", nSlotId, mMaxSlots );
    return false;
  }
  /* --- degug only ---
  Console->Print(GREEN, BLACK, "PContainer::AddEntry: added item %d at pos (%d,%d)", NewEntry->mItem->GetItemID(), NewEntry->mPosX, NewEntry->mPosY);

  Dump();
  ---            --- */
}

void PContainer::SetEntryPosXY( PContainerEntry* nEntry, u8 nSlotId, u8 nPosX, u8 nPosY )
{
  nPosX = nPosY;
  nEntry->Set2DPos( nSlotId, 0 );
  mDirtyFlag = mDirtyFlag || nEntry->mDirtyFlag;
}

PContainerEntry* PContainer::RemoveEntry( u8 nSlotId )
{
  PContainerEntry* tEntry = NULL;

  if ( nSlotId < mContContent->size() )
  {
    tEntry = mContContent->at( nSlotId );
    mContContent->at( nSlotId ) = NULL;
  }
  return tEntry;
}

bool PContainer::GetFreeSlot( u8* nSlotId )
{
  return IsSlotFree( *nSlotId );
}

void PContainer::Compact( u8 startSlotId )
{
  u8 i, j;
  PContainerEntry* tEntry;

  for ( i = j = startSlotId; i < mContContent->size(); ++i )
  {
    if ( mContContent->at( i ) )
    {
      if ( j < i )
      {
        mContContent->at( j ) = tEntry = mContContent->at( i );
        this->SetEntryPosXY( tEntry, j, tEntry->mPosX, tEntry->mPosX );
        mContContent->at( i ) = NULL;
        mDirtyFlag = true;
      }
      ++j;
    }
  }

  if ( !mMaxSlots ) // reduce size only if not fixed-size
  {
    for ( ; j < i; ++j )
      mContContent->pop_back();
  }
}

bool PContainer::StartUse( u32 nExclusiveUseCharID )
{
  if ( mExclusiveUseCharID )
    return false;
  else
  {
    mExclusiveUseCharID = nExclusiveUseCharID;
    return true;
  }
}

bool PContainer::EndUse( u32 nExclusiveUseCharID )
{
  if ( nExclusiveUseCharID == mExclusiveUseCharID )
  {
    mExclusiveUseCharID = 0;
    return true;
  }
  else
  {
    Console->Print( RED, BLACK, "[Warning] PContainer::EndUse called with CharID %d when CharID %d had exclusive use", nExclusiveUseCharID, mExclusiveUseCharID );
    return false;
  }
}

bool PContainer::SQLLoad()
{
  if ( !mCharID )
  {
    return false;
  }

  MYSQL_RES *result;
  MYSQL_ROW row;
  char query[1024];
  PContainerEntry* NewEntry;
  bool SaveDirtyFlag;
  u8 nSlotId;
  //u8 nPosX, nPosY, nSizeX, nSizeY;

  /*if (!mContContent.empty())
  {
    Console->Print(RED, BLACK, "PContainer::SQLLoad: We don't want to load inventory on non-empty inventory for char %d", mCharID);
    return false;
  }*/

  snprintf( query, 1024, "SELECT * FROM inventory WHERE inv_charid='%d' AND inv_loc='%d' ORDER BY inv_x ASC", mCharID, mInvLoc );
  result = MySQL->GameResQuery( query );
  if ( result == NULL )
  {
    Console->Print( RED, BLACK, "[Warning] PContainer::SQLLoad could not load inventory from the database" );
    Console->Print( "Query was:" );
    Console->Print( "%s", query );
    MySQL->ShowGameSQLError();
    return false;
  }
  while (( row = mysql_fetch_row( result ) ) )
  {
    NewEntry = new PContainerEntry( row );
    if ( NewEntry->mItem->GetItemID() )
    {
      nSlotId = NewEntry->mPosX;
      SaveDirtyFlag = mDirtyFlag;

      if ( ! this->GetFreeSlot( &nSlotId ) || ! this->AddEntry( NewEntry, nSlotId ) )
      {
        Console->Print( YELLOW, BLACK, "[Warning] PContainer::SQLLoad: Can't add item %d in slot %d of char %d inventory", NewEntry->mItem->GetItemID(), nSlotId, mCharID );
        delete NewEntry;
        mDirtyFlag = SaveDirtyFlag;
      }
      else
      {
        mDirtyFlag  = SaveDirtyFlag || ( nSlotId != NewEntry->mPosX );
      }
    }
    else
    {
      Console->Print( YELLOW, BLACK, "[Notice] Invalid item in DB (inv_id = %d) - Ignored", NewEntry->mInvID );
      delete NewEntry;
    }
  }
  MySQL->FreeGameSQLResult( result );
  return true;
}

bool PContainer::SQLSave()
{
  if ( !mCharID )
    return false;

  PContainerEntry* ContEntry;
  bool SavedOK = true;

  for ( u8 i = 0; i < mContContent->size(); ++i )
  {
    if (( ContEntry = mContContent->at( i ) ) )
    {
      if (( ContEntry->mDirtyFlag ) )
      {
        SavedOK = SavedOK && ContEntry->SQLSave( mCharID, mInvLoc );
      }
    }
  }

  if ( SavedOK )
  {
    mDirtyFlag = false;
  }
  return SavedOK;
}

bool PContainer::IsSlotFree( u8 nSlotId )
{
  if ( !IsSlotAllowed( nSlotId ) )
    return false;

  if ( nSlotId >= mContContent->size() )
    return true;
  else
    return !( mContContent->at( nSlotId ) );
}

bool PContainer::AddItem( PItem* NewItem, u32 nInvID, u8 nPosX, u8 nPosY, bool SetDirty )
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

  if ( NewItem->GetItemID() == 0 )
  {
    Console->Print( RED, BLACK, "[Warning] PContainer::AddItem: invalid item : ID 0" );
    return false;
  }

  PContainerEntry* NewEntry = new PContainerEntry( NewItem, nPosX, nPosY, nInvID, SetDirty );

  if ( this->GetFreeSlot( &nPosX ) && this->AddEntry( NewEntry, nPosX ) ) // !!!! nPosX not good
    return true;
  else
  {
    delete NewEntry;
    return false;
  }
}

bool PContainer::MoveItem( u8 srcSlotId, u8 nCount, PContainer* dstContainer, u8 dstSlotId, u8 nPosX, u8 nPosY )
{
  if ( dstContainer == this )
    return this->MoveItem( srcSlotId, nCount, dstSlotId );
  else if ( dstContainer->GetFreeSlot( &dstSlotId ) )
  {
    u8 savePosX, savePosY;
    PContainerEntry* tEntry = this->GetEntry( srcSlotId );
    if ( tEntry )
    {
      tEntry->Get2DPos( &savePosX, &savePosY );
      tEntry->Set2DPos( nPosX, nPosY );
      if ( !dstContainer->AddEntry( tEntry, dstSlotId ) )
      {
        tEntry->Set2DPos( savePosX, savePosY );
        Console->Print( RED, BLACK, "[Warning] PContainer::MoveItem - Couldn't add entry !" );
        return false;
      }
      tEntry->Get2DPos( &nPosX, &nPosY );
      tEntry->Set2DPos( savePosX, savePosY );
      RemoveEntry( srcSlotId );
      tEntry->Set2DPos( nPosX, nPosY );

      if ( dstContainer->mCharID != this->mCharID ) // save at once if owner changes
      {
        if ( !tEntry->SQLSave( dstContainer->mCharID, dstContainer->mInvLoc ) ) // TODO: improve to manage non-persistent containers
        {
          Console->Print( RED, BLACK, "[Warning] PContainer::MoveItem - Moved entry not saved" );
        }
        //else
        //{
        //  Console->Print(GREEN, BLACK, "[Succes] PContainer::MoveItem - Moved entry saved");
        //}
      }
      return true;
    }
    else
    {
      Console->Print( RED, BLACK, "[Warning] trying to move invalid entry %d", srcSlotId );
      return false;
    }
  }
  return false;
}

// !!! Not partial move yet !!!
bool PContainer::MoveItem( u8 srcSlotId, u8 nCount, u8 dstSlotId )
{
  if ( !( IsSlotAllowed( srcSlotId ) && IsSlotAllowed( dstSlotId ) ) )
    return false;

  if ( srcSlotId == dstSlotId )
    return true;

  if (( mContContent->at( srcSlotId ) )
      && ( mContContent->at( srcSlotId )->mItem->GetStackSize() >= nCount )
      && IsSlotFree( dstSlotId ) )
  {
    PContainerEntry* tEntry = mContContent->at( dstSlotId ) = mContContent->at( srcSlotId );
    tEntry->mPosX = dstSlotId;
    tEntry->mPosY = 0;
    tEntry->mDirtyFlag = true;
    mContContent->at( srcSlotId ) = NULL;
    mDirtyFlag = true;
    return true;
  }
  else
    return false;
}

std::vector< PContainerEntry* >* PContainer::GetEntries()
{
  std::vector< PContainerEntry* >* Entries = new std::vector< PContainerEntry* >;

  for ( u8 i = 0; i < mContContent->size(); ++i )
  {
    if ( mContContent->at( i ) )
      Entries->push_back( mContContent->at( i ) );
  }

  return Entries;
}

PContainerEntry* PContainer::GetEntry( u8 nSlotId )
{
  if ( nSlotId >= mContContent->size() )
    return NULL;

  return mContContent->at( nSlotId );
}

PItem* PContainer::GetItem( u8 nSlotId )
{
  PContainerEntry* tEntry = this->GetEntry( nSlotId );
  return ( tEntry ? tEntry->mItem : NULL );
}

u8 PContainer::RandomFill( u8 nItemCount, int nItemContainerDefIndex )
{
  PItem* nItem = NULL;
  const PDefItems* nItemDef;
  u32 nItemSeqId;
  u8 CreatedCount = 0;

  if ( !nItemCount )
    nItemCount = mMaxSlots;
  if ( !nItemCount )
    nItemCount = CONTAINER_MAX_SIZE;

  if ( nItemContainerDefIndex >= 0 )
  {
    int newItemIdx, newItemIndex;
    u8 newItemQuality;

    const PDefItemContainer* containerDef = GameDefs->ItemContainers()->GetDef( nItemContainerDefIndex );
    if ( containerDef )
    {
      for ( u8 i = 0; i < nItemCount; ++i )
      {
        newItemIdx = containerDef->GetRandomItemIdx();
        if ( newItemIdx >= 0 )
        {
          newItemIndex = containerDef->GetItemId( newItemIdx );
          newItemQuality = ( u8 )( 255 & int( 255.0 * containerDef->GetQuality( newItemIdx ) ) );
          if ( newItemIndex <= 0 ) // Item group
          {
            newItemIndex = GameDefs->Items()->GetRandomItemIdFromGroup( -newItemIndex );
          }
          nItem = new PItem( newItemIndex, 1, 255, newItemQuality, newItemQuality, newItemQuality, newItemQuality, 255 );
          if ( nItem->GetItemID() )
          {
            if ( this->AddItem( nItem ) )
            {
              ++CreatedCount;
            }
            else
            {
              Console->Print( YELLOW, BLACK, "[Warning] Couldn't add item %d at pos %d", nItem->GetItemID(), i );
              delete nItem;
            }
          }
        }
      }
    }
    else
      Console->Print( YELLOW, BLACK, "[Warning] Container def not found: %d", nItemContainerDefIndex );
  }
  else
  {
    int NumItemsDefs = GameDefs->Items()->GetNumDefs();

    for ( u8 i = 0; i < nItemCount; ++i )
    {
      nItemSeqId = GetRandom( NumItemsDefs, 1 );

      if (( nItemDef = GameDefs->Items()->GetDefBySeqIndex( nItemSeqId ) ) )
      {
        nItem = new PItem( nItemDef->GetIndex(), 1, 255, 255, 255, 255, 255, 255 );
        if ( nItem->GetItemID() )
        {
          if ( this->AddItem( nItem ) )
          {
            ++CreatedCount;
          }
          else
          {
            //Console->Print(GREEN, BLACK, "Couldn't add item % d at pos % d", nItem->GetItemID(), i);
            delete nItem;
          }
        }
      }
      else
        Console->Print( RED, BLACK, "[Warning] PContainer::RandomFill Couldn't find item for SEQ %d", nItemSeqId );
    }
  }
  return CreatedCount;
}

void PContainer::Dump()
{
  PContainerEntry* tmpEntry;
  PItem* tmpItem;
  u8 i;
  //u8 y, x;

  for ( i = 0; i < mContContent->size(); ++i )
  {
    if (( tmpEntry = mContContent->at( i ) ) )
    {
      tmpItem = tmpEntry->mItem;
      Console->Print( GREEN, BLACK, "%d: (%d,%d) (%d x %d) Item %d (InvID %d) %s %s", i, tmpEntry->mPosX, tmpEntry->mPosY, tmpItem->GetSizeX(), tmpItem->GetSizeY(), tmpItem->GetItemID(), tmpEntry->mInvID, tmpItem->GetName().c_str(), tmpEntry->mDirtyFlag ? "[*]" : "" );
    }
  }

  if ( mDirtyFlag )
    Console->Print( GREEN, BLACK, "Dirty" );
  Console->Print( GREEN, BLACK, "------------------------------------" );
}

/* --- PContainerWithHoles class --- */


/* --- PContainerAutoCompact --- */
PContainerEntry* PContainerAutoCompact::RemoveEntry( u8 nSlotId )
{
  PContainerEntry* tEntry = NULL;

  if ( nSlotId < mContContent->size() )
  {
    tEntry = mContContent->at( nSlotId );
    mContContent->at( nSlotId ) = NULL;
    if ( nSlotId == ( mContContent->size() - 1 ) )
      mContContent->pop_back();
    else
      Compact( nSlotId );
  }
  return tEntry;
}

bool PContainerAutoCompact::GetFreeSlot( u8* nSlotId ) // not optimal. A "first free from end" would be better at PContainer level
{
  bool Found = false;

  for ( *nSlotId = 0; *nSlotId < mContContent->size(); ++*nSlotId )
    if ( ! mContContent->at( *nSlotId ) )
    {
      Found = true;
      break;
    }
  return ( Found || IsSlotAllowed( *nSlotId ) );
}

bool PContainerAutoCompact::MoveItem( u8 srcSlotId, u8 nCount, u8 dstSlotId )
{
  srcSlotId = nCount = dstSlotId;
  return false;
}

/* --- PContainer2D class --- */

void PContainer2D::SetEntryPosXY( PContainerEntry* nEntry, u8 nSlotId, u8 nPosX, u8 nPosY )
{
  nSlotId = nSlotId;
  if (( nPosX >= INV_BACKPACK_COLS ) || ( nPosY == 255 ) )
  {
    nPosX = nPosY = 0;
  }
  nEntry->Set2DPos( nPosX, nPosY );
  mDirtyFlag = mDirtyFlag || nEntry->mDirtyFlag;
}

/* --- PContainer2DWorkaround --- */
/////
PContainer2DWorkaround::PContainer2DWorkaround( u8 nMaxSlots ) : PContainerWithHoles( nMaxSlots )
{
  nMaxSlots = nMaxSlots;
  mNextFreeSlot = 0;
  mMaxCols = 254;
  mMaxRows = 254;
  mRows = 0;
  AddRow();
}

PContainer2DWorkaround::~PContainer2DWorkaround()
{
  for ( int i = 0; i < mRows; i++ )
    delete mContSpace[i];
}

bool PContainer2DWorkaround::AddEntry( PContainerEntry* tEntry, u8 nSlotId )
{
  if ( IsSlotAllowed( nSlotId ) )
  {
    if ( FindValid2DPos( tEntry ) )
    {
      for ( u8 i = mContContent->size(); i <= nSlotId; ++i ) // Extend as needed
        mContContent->push_back( NULL );
      if ( mContContent->at( nSlotId ) )
      {
        Console->Print( RED, BLACK, "[Warning] PContainer2DWorkaround::AddEntry: Target entry already %d in use !!!", nSlotId );
        return false;
      }
      mContContent->at( nSlotId ) = tEntry;
      mDirtyFlag = true;
      //this->SetEntryPosXY(tEntry, nSlotId, tEntry->mPosX, tEntry->mPosY);
      mDirtyFlag = mDirtyFlag || tEntry->mDirtyFlag;
      SetUsed( tEntry );
      return true;
    }
    else
    {
      Console->Print( RED, BLACK, "[Warning] PContainer2DWorkaround::AddEntry: No 2D space left !!!", nSlotId, mMaxSlots );
      return false;
    }
  }
  else
  {
    Console->Print( RED, BLACK, "[Warning] PContainer2DWorkaround::AddEntry: Target entry %d not allowed (max = %d) !!!", nSlotId, mMaxSlots );
    return false;
  }

}

PContainerEntry* PContainer2DWorkaround::RemoveEntry( u8 nSlotId )
{
  PContainerEntry* tEntry = NULL;

  if ( nSlotId < mContContent->size() )
  {
    tEntry = mContContent->at( nSlotId );
    mContContent->at( nSlotId ) = NULL;
    //Console->Print(YELLOW, BLACK, "Cleaning (%d,%d) (%d x %d)", tEntry->mPosX, tEntry->mPosY, tEntry->mItem->GetSizeX(), tEntry->mItem->GetSizeY());
    SetUsed( tEntry, false );
  }
  return tEntry;
}

bool PContainer2DWorkaround::GetFreeSlot( u8* nSlotId )
{
  if ( IsSlotAllowed( mNextFreeSlot ) )
  {
    *nSlotId = mNextFreeSlot++;
    return true;
  }
  else
    return false;
}

void PContainer2DWorkaround::SetEntryPosXY( PContainerEntry* nEntry, u8 nSlotId, u8 nPosX, u8 nPosY )
{
  nSlotId = nSlotId;
  if (( nPosX == 255 ) && ( nPosY == 255 ) )
  {
    nEntry->Set2DPos( 255, 255 );
  }
  else
  {
    if (( nPosX >= mMaxCols ) || ( nPosY >= mMaxRows ) )
    {
      nPosX = nPosY = 0;
    }
    nEntry->Set2DPos( nPosX, nPosY );

    if ( !FindValid2DPos( nEntry ) )
    {
      nEntry->Set2DPos( 255, 255 );
      Console->Print( RED, BLACK, "[Warning] PContainer2DWorkaround::SetEntryPosXY - Space position already used" );
    }
  }
  mDirtyFlag = mDirtyFlag || nEntry->mDirtyFlag;
}

bool PContainer2DWorkaround::MoveItem( u8 srcSlotId, u8 nCount, u8 dstSlotId )
{
  srcSlotId = nCount = dstSlotId;
  return false;
}

void PContainer2DWorkaround::AddRow()
{
  if ( mRows + 1 < mMaxRows )
  {
    std::vector<bool>* NewRow = new std::vector<bool>( mMaxCols, 0 );
    mContSpace.push_back( NewRow );
    ++mRows;
  }
}

bool PContainer2DWorkaround::Is2DFree( u8 PosX, u8 PosY, u8 SizeX, u8 SizeY )
{
  if (( PosX == 255 ) && ( PosY == 255 ) )
  {
    return true;
  }
  if ( !Is2DPosAllowed( PosX, PosY, SizeX, SizeY ) )
    return false;

  u8 h, v;
  for ( v = 0; ( v < SizeY ) && ( PosY + v < mRows ) ; v++ ) // what is over existing rows is free
  {
    for ( h = 0; h < SizeX; h++ )
    {
      if ( PosX + h >= mMaxCols ) // what is over max col is not free
        return false;

      if (( *mContSpace[PosY+v] )[PosX+h] )
        return false;
    }
  }
  return ( PosY + SizeY <= mMaxRows );
}

void PContainer2DWorkaround::SetUsed( PContainerEntry* nEntry, bool Value )
{
  u8 PosX = nEntry->mPosX;;
  u8 PosY = nEntry->mPosY;
  u8 SizeX = nEntry->mItem->GetSizeX();
  u8 SizeY = nEntry->mItem->GetSizeY();

  if ( !Is2DPosAllowed( PosX, PosY, SizeX, SizeY ) )
    return;

  while ( PosY + SizeY > mRows ) // add new rows when needed
    AddRow();

  u8 h, v;
  for ( v = 0; ( v < SizeY ) && ( PosY + v < mRows ) ; v++ )
  {
    for ( h = 0; ( h < SizeX ) && ( PosX + h < mMaxCols ); h++ )
      ( *mContSpace[PosY+v] )[PosX+h] = Value;
  }
}

bool PContainer2DWorkaround::FindValid2DPos( PContainerEntry* nEntry )
{
  bool Found = false;
  u8 SizeX = nEntry->mItem->GetSizeX();
  u8 SizeY = nEntry->mItem->GetSizeY();
  u8 dPosX, dPosY;

  if ( Is2DFree( nEntry->mPosX, nEntry->mPosY, SizeX, SizeY ) )
    Found = true;
  else
  {
    dPosX = nEntry->mPosX;
    dPosY = nEntry->mPosY;
    if ( !Is2DPosAllowed( dPosX, dPosY, SizeX, SizeY ) )
    {
      dPosX = dPosY = 0;
      //Console->Print(YELLOW, BLACK, "Init pos RESET");
    }

    while ( !Found && Is2DPosAllowed( dPosX, dPosY, SizeX, SizeY ) )
    {
      //Console->Print(YELLOW, BLACK, "Searching line %d", dPosY);
      for ( ; dPosX <= ( mMaxCols - SizeX ); ++dPosX )
      {
        if (( Found = Is2DFree( dPosX, dPosY, SizeX, SizeY ) ) )
          break;
      }
      if ( Found )
      {
        nEntry->mPosX = dPosX;
        nEntry->mPosY = dPosY;
        nEntry->mDirtyFlag = true;
        //Console->Print(YELLOW, BLACK, "Success: Found new space position : (%d,%d)", dPosX, dPosY);
        break;
      }
      ++dPosY;
      dPosX = 0;
    }
  }
  return Found;
}

void PContainer2DWorkaround::Dump()
{
  PContainer::Dump();

  std::string tmpS;
  u8 y, x;
  for ( y = 0 ; y < mRows; y++ )
  {
    tmpS = "";
    for ( x = 0 ; x < mMaxCols; x++ )
    {
      tmpS += (( *mContSpace[y] )[x] ) ? "X" : ".";
    }
    Console->Print( "%s", tmpS.c_str() );
  }
  Console->Print( GREEN, BLACK, "------------------------------------" );
}

/* --- PContainerAutoFindFree --- */
bool PContainerAutoFindFree::GetFreeSlot( u8* nSlotId )
{
  bool Found = false;

  for ( *nSlotId = 0; *nSlotId < mContContent->size(); ++*nSlotId )
    if ( ! mContContent->at( *nSlotId ) )
    {
      Found = true;
      break;
    }

  return ( Found || IsSlotAllowed( *nSlotId ) );
}

/* --- PContainerAutoCompactOnClose --- */
bool PContainerAutoCompactOnClose::EndUse( u32 nExclusiveUseCharID )
{
  if ( nExclusiveUseCharID == mExclusiveUseCharID )
  {
    mExclusiveUseCharID = 0;
    Compact();
    return true;
  }
  else
  {
    Console->Print( RED, BLACK, "[ERROR] PContainerAutoCompactOnClose::EndUse called with CharID %d when CharID %d had exclusive use", nExclusiveUseCharID, mExclusiveUseCharID );
    return false;
  }
}
