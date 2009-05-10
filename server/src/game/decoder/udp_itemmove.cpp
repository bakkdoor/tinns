/*
 TinNS (TinNS is not a Neocron Server)
 Copyright (C) 2005 Linux Addicted Community
 maintainer Akiko <akiko@gmx.org>

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

 udp_itemmove.cpp - decoder classes for UDP item move related messages

 CREATION: 30 Dec 2006 Namikon

 MODIFIED: 28 Aug 2007 Hammag
 REASON: Moved decoding part from DoAction() to Analyse(), use natural decode methods

*/

#include "main.h"
#include "udp_itemmove.h"
#include "chars.h"
#include "inventory.h"
#include "container.h"

/**** PUdpItemMove ****/

#define PUdpItemMove_NO_BOX_TAKE_ALL

PUdpItemMove::PUdpItemMove( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x1e";
}

PUdpMsgAnalyser* PUdpItemMove::Analyse()
{
  mDecodeData->mName << "=Moving item";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 8 );

  *nMsg >> mSrcLoc;
  *nMsg >> mSrcX;
  *nMsg >> mSrcY;
  *nMsg >> mDstLoc;
  *nMsg >> mDstX;
  *nMsg >> mDstY;
  *nMsg >> mItemCnt;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpItemMove::DoAction()
{
//mDecodeData->mTraceDump = true;
  PMessage* tmpMsg;
  bool MoveSucceded = false;
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();
  u8 origDstX = mDstX;
  u8 origDstY = mDstY;
  PContainerEntry* tEntry;

  PItem* tItem = NULL;

  PContainer* SrcContainer = GetContainerByLoc( tChar, mSrcLoc );
  if ( SrcContainer )
  {
    tEntry = SrcContainer->GetEntry( mSrcX );
    tItem = SrcContainer->GetItem( mSrcX );
    if ( mSrcLoc != mDstLoc )
    {
      PContainer* DstContainer = GetContainerByLoc( tChar, mDstLoc );
      if ( mSrcY )
        Console->Print( "%s PUdpItemMove: intramove: src Y != 0", Console->ColorText( YELLOW, BLACK, "[WARNING]" ) );
#ifdef PUdpItemMove_NO_BOX_TAKE_ALL
      if (( mSrcLoc != INV_LOC_BOX ) || ( mDstX != 255 ) || ( mDstY != 255 ) )
      {
#endif
        if ( DstContainer )
        {
          //PContainerEntry* tEntry = SrcContainer->GetEntry(mSrcX);
          MoveSucceded = SrcContainer->MoveItem( mSrcX, mItemCnt, DstContainer, mDstX, mDstX, mDstY );
          /*if(tEntry)
            tEntry->Get2DPos(&mDstX, &mDstY);*/
        }
#ifdef PUdpItemMove_NO_BOX_TAKE_ALL
      }
#endif
    }
    else
    {
      if ( mSrcY || mDstY )
        Console->Print( "%s PUdpItemMove: intramove: src Y or dst Y != 0", Console->ColorText( YELLOW, BLACK, "[WARNING]" ) );
      MoveSucceded = SrcContainer->MoveItem( mSrcX, mItemCnt, mDstX );
    }
  }

  if ( MoveSucceded )
  {
    if (( mDstX == 255 ) && ( mDstY == 255 ) && ( mSrcLoc == INV_LOC_BOX ) )
    {
      tmpMsg = MsgBuilder->BuildUndefineduseMsg( nClient, 0x2c );
      nClient->SendUDPMessage( tmpMsg );
      tmpMsg = MsgBuilder->BuildBoxItemMoveMsg( nClient, tEntry, mSrcX, mSrcY, mDstLoc, mDstX, mDstY, mItemCnt );
    }
    else
    {
      tmpMsg = MsgBuilder->BuildItemMoveMsg( nClient, mSrcLoc, mSrcX, mSrcY, mDstLoc, mDstX, mDstY, mItemCnt );
    }
    nClient->SendUDPMessage( tmpMsg );

    if ( gDevDebug )
      Console->Print( "%s Success : Item move %u:%u-%u => %u:%u-%u : %u %s", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mSrcLoc, mSrcX, mSrcY, mDstLoc, origDstX, origDstY, mItemCnt, tItem ? tItem->GetName().c_str() : "" );
    /*Console->Print(YELLOW, BLACK, "--- Worn Inventory ---");
    tChar->GetInventory()->GetContainer(INV_LOC_WORN)->Dump();
    Console->Print(YELLOW, BLACK, "--- Backpack Inventory ---");
    tChar->GetInventory()->GetContainer(INV_LOC_BACKPACK)->Dump();
    Console->Print(YELLOW, BLACK, "--- Gogo Inventory ---");
    tChar->GetInventory()->GetContainer(INV_LOC_GOGO)->Dump();
    if (tChar->GetInventory()->IsDirty())
      Console->Print(YELLOW, BLACK, "Inventory DIRTY"); */

// => TO CHECK: deactivate item in-hand if active slot now emtpy ???
  }
  else
  {
#ifdef PUdpItemMove_NO_BOX_TAKE_ALL
    if (( mSrcLoc != INV_LOC_BOX ) || ( mDstX != 255 ) || ( mDstY != 255 ) )
    {
#endif
      if ( mDstLoc == INV_LOC_GROUND )
        Chat->send( nClient, CHAT_DIRECT, "System", "Can't throw items to the ground yet." );
      else
        Console->Print( "%s Item move failed %u:%u-%u => %u:%u-%u : %u %s", Console->ColorText( YELLOW, BLACK, "[WARNING]" ), mSrcLoc, mSrcX, mSrcY, mDstLoc, origDstX, origDstY, mItemCnt, tItem ? tItem->GetName().c_str() : "" );
      // Send 'null move'
      //tmpMsg = MsgBuilder->BuildItemMoveMsg(nClient, mSrcLoc, mSrcX, mSrcY, mSrcLoc, mSrcX, mSrcY, mItemCnt);
      //nClient->SendUDPMessage(tmpMsg);
#ifdef PUdpItemMove_NO_BOX_TAKE_ALL
    }
    else
    {
      Chat->send( nClient, CHAT_DIRECT, "System", "'TAKE ALL' can't be used yet." );
    }
#endif
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

PContainer* PUdpItemMove::GetContainerByLoc( PChar* nChar, u8 nLoc )
{
  PContainer* tContainer = NULL;

  switch ( nLoc )
  {
    case INV_LOC_WORN:
    case INV_LOC_BACKPACK:
    case INV_LOC_GOGO:
      {
        tContainer = nChar->GetInventory()->GetContainer( nLoc );
        break;
      }

    case INV_LOC_BOX:
      {
        tContainer = nChar->GetContainerInExclusiveUse();
        break;
      }
    case INV_LOC_GROUND:
    case INV_LOC_NPCTRADE:
    default:
      break;
  }

  return tContainer;
}


/**** PUdpItemMoveBP ****/

PUdpItemMoveBP::PUdpItemMoveBP( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x14";
}

PUdpMsgAnalyser* PUdpItemMoveBP::Analyse()
{
  mDecodeData->mName << "=Moving item";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 9 );

  *nMsg >> mSrcSlotId;
  *nMsg >> mDumb;
  *nMsg >> mDstX;
  *nMsg >> mDstY;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpItemMoveBP::DoAction()
{
//mDecodeData->mTraceDump = true;
//  PMessage* tmpMsg;
  bool MoveSucceded = false;
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();

  PContainer2DWorkaround* SrcContainer = tChar->GetInventory()->GetBackpackContainer();
  if ( SrcContainer )
  {
    if ( mDumb )
      Console->Print( "%s PUdpItemMoveBP: Dumb != 0 (%d)", Console->ColorText( YELLOW, BLACK, "[WARNING]" ), mDumb );
    if ( gDevDebug )
      Console->Print( "%s PUdpItemMoveBP: Tyring Item move: slot %u => %u-%u", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mSrcSlotId, mDstX, mDstY );
    PContainerEntry* tEntry = SrcContainer->GetEntry( mSrcSlotId );
    if ( tEntry )
    {
      SrcContainer->SetUsed( tEntry, false );
      SrcContainer->SetEntryPosXY( tEntry, mSrcSlotId, mDstX, mDstY );
      SrcContainer->SetUsed( tEntry );
      SrcContainer->SetDirty();
      MoveSucceded = true;
    }
    else
      Console->Print( "%s PUdpItemMoveBP: trying to use invalid slot %d", Console->ColorText( RED, BLACK, "[WARNING]" ), mSrcSlotId );
  }
  else
  {
    Console->Print( "%s trying to use invalid Backpack container", Console->ColorText( RED, BLACK, "[WARNING]" ) );
  }
//Console->Print(YELLOW, BLACK, "--- Backpack Inventory ---");
//SrcContainer->Dump();

// No answer to confirm ?
//tmpMsg = NULL;

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpItemDropOnItem ****/

PUdpItemDropOnItem::PUdpItemDropOnItem( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x17";
}

PUdpMsgAnalyser* PUdpItemDropOnItem::Analyse()
{
  mDecodeData->mName << "=Dropping item on item";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 9 );

  *nMsg >> mSrcLoc;
  *nMsg >> mSrcX; // Linear Idx
  *nMsg >> mSrcY; // 0
  *nMsg >> mDstLoc;
  *nMsg >> mDstX; // Linear Idx
  *nMsg >> mDstY; // 0

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpItemDropOnItem::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();
  if ( gDevDebug )
    Console->Print( "%s PUdpItemDropOnItem: %u:%u-%u => %u:%u-%u", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mSrcLoc, mSrcX, mSrcY, mDstLoc, mDstX, mDstY );
  PContainer* SrcContainer = PUdpItemMove::GetContainerByLoc( tChar, mSrcLoc );
  PContainer* DstContainer = PUdpItemMove::GetContainerByLoc( tChar, mDstLoc );
  if ( SrcContainer && DstContainer )
  {
    PItem* sItem = SrcContainer->GetItem( mSrcX );
    PItem* dItem = DstContainer->GetItem( mDstX );

    if ( gDevDebug )
      Console->Print( "%s Drop item %s on %s", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), sItem ? sItem->GetName().c_str() : "???", dItem ? dItem->GetName().c_str() : "???" );
  }
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
