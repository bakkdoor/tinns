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

 udp_itemuse.cpp - decoder classes for UDP inventory item use messages

 CREATION: 9 May 2009 Hammag

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_itemuse.h"


/**** PUdpItemUse ****/

PUdpItemUse::PUdpItemUse( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x01";
}

PUdpMsgAnalyser* PUdpItemUse::Analyse()
{
  mDecodeData->mName << "=Inventory item use";
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

bool PUdpItemUse::DoAction()
{
  //PClient* nClient = mDecodeData->mClient;
  //PChar* tChar = nClient->GetChar();

  //PMessage* tmpMsg = MsgBuilder->BuildHeldItemUsedMsg( nClient->GetLocalID(), mWeaponId, mTargetRawItemID, mUnknown2, mTargetedHeight, 0 ); // 'score' is not broadcasted, but set to 0
  //ClientManager->UDPBroadcast( tmpMsg, nClient );

  //if ( gDevDebug )
  {
    Console->Print( "%s Using inventory item", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    mDecodeData->mMessage->Dump();
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
