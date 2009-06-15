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
	udp_quickaccessbelt.cpp - decoder classes for UDP Quickaccessbelt related messages

	CREATION: 30 Dec 2006 Namikon

	MODIFIED: 01 Sep 2007 Hammag
	REASON: - Put analysis code in Analyse() and change to use new item management methods
	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "udp_quickaccessbelt.h"

#include "include/container.h"
#include "include/inventory.h"


/**** PUdpItemSlotUse ****/

PUdpItemSlotUse::PUdpItemSlotUse( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x1f";
}

PUdpMsgAnalyser* PUdpItemSlotUse::Analyse()
{
  mDecodeData->mName << "=Active QuickBelt Slot";

  mTargetSlot = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 8 );
  // TODO : Check on mTargetSlot value + put set to INV_WORN_QB_HAND for hand
  if ( mTargetSlot == 255 ) // H "slot 0" Hand
    mTargetSlot = INV_WORN_QB_HAND;

  //if(mTargetSlot == 11) //  ALT-H ?

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpItemSlotUse::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();
  PItem* targetItem = NULL;
  bool tUsable = false;
  bool tUsableInHand = false;
  u16 nWeaponId = 0;

  if( gDevDebug )
    Console->Print( "%s Client trying to activate item in slot %d.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTargetSlot );

  u8 currentActiveSlot = tChar->GetQuickBeltActiveSlot();
  if ( mTargetSlot == INV_WORN_QB_HAND )
  {
    if( gDevDebug )
      Console->Print( "%s Want to use Hand", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    tUsable = true;
    tUsableInHand = true;
  }
  else
  {
    targetItem = tChar->GetInventory()->GetContainer( INV_LOC_WORN )->GetItem( INV_WORN_QB_START + mTargetSlot );
    if ( targetItem )
    {
      // TODO : do the real check;
      if( gDevDebug )
        Console->Print( "%s Want to use existing item", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      tUsable = true;
      tUsableInHand = true;
      nWeaponId = targetItem->GetValue1();
    }
  }

//TODO:
// if tUsableInHand
//  must check if weapon and allowed in zone
//  and change tUsableInHand to false if needed

  if ( tUsable && !tUsableInHand )
  {
    // active item in belt, but don't take in hand
    if ( gDevDebug )
      Console->Print( "%s activation of QB item slot %d (%s)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTargetSlot, targetItem->GetName().c_str() );
  }
  else
  {
    if ( !tUsable )
    {
      if ( gDevDebug )
        Console->Print( "%s Want to use non-usable item", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      mTargetSlot = INV_WORN_QB_NONE; // if not usable, unequip active one
    }

    if ( mTargetSlot == currentActiveSlot ) // if same as active, unequip active one
    {
      if ( gDevDebug )
        Console->Print( "%s Want to use active slot", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      mTargetSlot = INV_WORN_QB_NONE;
    }

    if ( mTargetSlot != currentActiveSlot ) // now do somthing only if not same as active
    {
      if ( tChar->SetQuickBeltActiveSlot( mTargetSlot ) )
      {
        PMessage* tmpMsg;
        tmpMsg = MsgBuilder->BuildCharHelloMsg( nClient );
        ClientManager->UDPBroadcast( tmpMsg, nClient );

        tmpMsg = MsgBuilder->BuildUndefineduseMsg( nClient, 59 );
        nClient->SendUDPMessage( tmpMsg );

        if ( nWeaponId > 0 )
        {
          tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg2( nClient );
          nClient->SendUDPMessage( tmpMsg );
        }
        tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg3( nClient, mTargetSlot );
        nClient->SendUDPMessage( tmpMsg );

        tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg4( nClient, nWeaponId );
        nClient->SendUDPMessage( tmpMsg );

        if ( gDevDebug )
          Console->Print("%s activation of QB item slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTargetSlot );
      }
      else Console->Print( "%s activation of QB slot %d refused by PChar", Console->ColorText( YELLOW, BLACK, "[WARNING]" ), mTargetSlot );
    }
    else
    {
      Console->Print( "%s Same slot %d as active - should not happen", Console->ColorText( YELLOW, BLACK, "[WARNING]" ), mTargetSlot );
    }
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
