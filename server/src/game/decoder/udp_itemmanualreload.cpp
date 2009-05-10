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

 udp_itemmanualreload.cpp - decoder classes for UDP item manual reload messages

 CREATION: 9 May 2009 Hammag

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_itemmanualreload.h"
#include "container.h"

/**** PUdpItemManualReload ****/

PUdpItemManualReload::PUdpItemManualReload( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x18";
}

PUdpMsgAnalyser* PUdpItemManualReload::Analyse()
{
  mDecodeData->mName << "=Reloading item (manual)";

  //PMessage* nMsg = mDecodeData->mMessage;
  mUnknown = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 9 );
  // 0e: reload
  // 01 / slotid 0xNNNN : use item from backpack
  if ( gDevDebug )
    mDecodeData->mMessage->Dump();
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpItemManualReload::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();
  
  if ( gDevDebug )
    Console->Print( "%s Manual reload start", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    
  u8 activeSlot = tChar->GetQuickBeltActiveSlot();
  if (( activeSlot != INV_WORN_QB_NONE ) && ( activeSlot != INV_WORN_QB_HAND ) )
  {
    u8 newAmmoCount = GetMaxLoadableAmmos( activeSlot );
    if ( newAmmoCount )
    {
      PMessage* tmpMsg = MsgBuilder->BuildStartWeaponReloadMsg( nClient->GetLocalID() );
      ClientManager->UDPBroadcast( tmpMsg, nClient );

      tmpMsg = new PMessage( 32 );
      nClient->IncreaseUDP_ID();

      *tmpMsg << ( u8 )0x13;
      *tmpMsg << ( u16 )nClient->GetUDP_ID();
      *tmpMsg << ( u16 )nClient->GetSessionID();

      *tmpMsg << ( u8 )0x00; // Message length
      *tmpMsg << ( u8 )0x03;
      *tmpMsg << ( u16 )nClient->GetUDP_ID();
      *tmpMsg << ( u8 )0x1f;
      *tmpMsg << ( u16 )nClient->GetLocalID();
      *tmpMsg << ( u8 )0x25;
      *tmpMsg << ( u8 )0x13;

      nClient->IncreaseTransactionID();
      *tmpMsg << ( u16 )nClient->GetTransactionID();
      *tmpMsg << ( u8 )0x05; // cmd=Ammo count update
      *tmpMsg << ( u8 )INV_LOC_WORN;
      *tmpMsg << ( u8 )(INV_WORN_QB_START + activeSlot); // X
      *tmpMsg << ( u8 )0x00; //(Y)
      *tmpMsg << ( u8 )newAmmoCount; // Ammo count

      ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

      nClient->SendUDPMessage( tmpMsg );
    }
  }
  //else
    //Console->Print( "%s no reloadable weapon on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), activeSlot );


  /*  PClient* nClient = mDecodeData->mClient;
    //PChar* tChar = nClient->GetChar();
    PMessage* tmpMsg = new PMessage(32);

    nClient->IncreaseUDP_ID();
    nClient->IncreaseTransactionID();

    *tmpMsg << (u8)0x13;
    *tmpMsg << (u16)nClient->GetUDP_ID();
    *tmpMsg << (u16)nClient->GetSessionID();

    *tmpMsg << (u8)0x00; // Message length
    *tmpMsg << (u8)0x03;
    *tmpMsg << (u16)nClient->GetUDP_ID();
    *tmpMsg << (u8)0x1f;
    *tmpMsg << (u16)nClient->GetLocalID();
    *tmpMsg << (u8)0x25;
    *tmpMsg << (u8)0x13;
    *tmpMsg << (u16)nClient->GetTransactionID();
    *tmpMsg << (u8)0x0a; // cmd=Ammo type update (?)
    *tmpMsg << (u8)0x03;
    *tmpMsg << (u8)0x02;
    *tmpMsg << (u8)0x00;
    *tmpMsg << (u8)0x00;
    *tmpMsg << (u8)0x00;
    nClient->IncreaseTransactionID();
    *tmpMsg << (u16)nClient->GetTransactionID();
    *tmpMsg << (u8)0x05; // cmd=Ammo count update
    *tmpMsg << (u8)INV_LOC_BACKPACK;
    *tmpMsg << (u8)0x0e; // X
    *tmpMsg << (u8)0x00; //(Y)
    *tmpMsg << (u8)0x03; // Ammo count
    nClient->IncreaseTransactionID();
    *tmpMsg << (u16)nClient->GetTransactionID();
    *tmpMsg << (u8)0x05; // cmd=Ammo count update
    *tmpMsg << (u8)INV_LOC_WORN;
    *tmpMsg << (u8)0x03; // X
    *tmpMsg << (u8)0x00; //(Y)
    *tmpMsg << (u8)0x02; // Ammo count

    (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
  tmpMsg->Dump();
    nClient->SendUDPMessage(tmpMsg);
  */
  /* Resp:
  03:33:00:1f:01:00:25:13
  c2:01:0a:00:02:00:00:00 ?? set ammo type ?
  c3:01:05:03:00:00:12 Update ammo left
  c4:01:05:02:00:00:0c Update ammo left
   
  tt:tt:02:loc:x:y delete item
  */

  /* manual reload
    srv resp : 13:58:00:d6:d4:08:03:58:00:1f:01:00:25:16 (do start reload anim - even if no relaod needed or no more ammo avail)
    cli resp: 13:3f:00:bd:d4:08:03:3f:00:1f:01:00:25:25 (reload anim done , also when reaload not possible because mission ammo)
    srv resp: 13:59:00:d7:d4:16:03:59:00:1f:01:00:25:13:8d:22:05:03:11:00:16:8e:22:05:02:00:00:0c : update inv if needed

    if(mUnknown != 0x0e)
   Console->Print(YELLOW, BLACK, "PUdpManualReloadItem: additionnal byte differs from 0x0e : %2x", mUnknown);
  */
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

u8 PUdpItemManualReload::GetMaxLoadableAmmos( u8 nBeltSlotId )
{
  PItem* activeItem = mDecodeData->mClient->GetChar()->GetInventory()->GetContainer( INV_DB_LOC_WORN )->GetItem( INV_WORN_QB_START + nBeltSlotId );
  if ( activeItem && ( activeItem->GetType() == ITEM_TYPE_WEAPON ) )
  {
    const PDefWeapon* activeWeapon = GameDefs->Weapons()->GetDef( activeItem->GetValue1() );
    //Console->Print( "%s Using weapon %d, type %d on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), activeItem->GetValue1(), activeItem->GetType(), nBeltSlotId );
    if ( activeWeapon )
    {
      int ammoId = activeWeapon->GetAmmoType(0); // not really the loaded ammo for now
      if ( ammoId > 0 ) // weapon really uses ammos
      {
        const PDefAmmo* activeAmmo = GameDefs->Ammos()->GetDef( ammoId );
        if ( activeAmmo )
        {
          return activeAmmo->GetMagSize();
        }
        //else
          //Console->Print( "%s no valid ammo (%d) on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), ammoId, nBeltSlotId );
      }
      //else
        //Console->Print( "%s no usable ammo (%d) on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), ammoId, nBeltSlotId );
    }
    //else
      //Console->Print( "%s no valid weapon (%d, type %d) on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), activeItem->GetValue1(), activeItem->GetType(), nBeltSlotId );
  }
  //else
    //Console->Print( "%s no valid item on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nBeltSlotId );
  return 0;
}

/**** PUdpReloadAnimDone ****/

PUdpReloadAnimDone::PUdpReloadAnimDone( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x25";
}

PUdpMsgAnalyser* PUdpReloadAnimDone::Analyse()
{
  mDecodeData->mName << "=Reload animation done";
  /*
    PMessage* nMsg = mDecodeData->mMessage;
    nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 8 );

    ( *nMsg ) >> mWeaponId;
    ( *nMsg ) >> mTargetRawItemID; // !!! 0x000003fe when shooting with no target
    ( *nMsg ) >> mUnknown2; // aiming ??? 0 to 52 +?
    ( *nMsg ) >> mTargetedHeight; // range 0 (bottom) to 26 (?) (top)
    ( *nMsg ) >> mScore; // range 0x00 to 0xff  Score ???
  */

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpReloadAnimDone::DoAction()
{
  //PClient* nClient = mDecodeData->mClient;
  //PChar* tChar = nClient->GetChar();

  //PMessage* tmpMsg = MsgBuilder->BuildHeldItemUsedMsg( nClient->GetLocalID(), mWeaponId, mTargetRawItemID, mUnknown2, mTargetedHeight, 0 ); // 'score' is not broadcasted, but set to 0
  //ClientManager->UDPBroadcast( tmpMsg, nClient );

  //if ( gDevDebug )
  Console->Print( "%s Reload animation done", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
  //mDecodeData->mMessage->Dump();

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
