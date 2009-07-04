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

 udp_helditemaction.cpp - decoder classes for held item related messages

 CREATION: 20 Mar 2009 Hammag

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_helditemaction.h"
#include "worlds.h"
#include "furnituretemplate.h"

/**** PUdpHeldItemBasicAction ****/

PUdpHeldItemBasicAction::PUdpHeldItemBasicAction( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x00";
}

PUdpMsgAnalyser* PUdpHeldItemBasicAction::Analyse()
{
  mDecodeData->mName << "=Basic action";

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpHeldItemBasicAction::DoAction()
{
  mDecodeData->mMessage->Dump();
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}


/**** PUdpHeldItemAimedAction ****/

PUdpHeldItemAimedAction::PUdpHeldItemAimedAction( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x01";
}

PUdpMsgAnalyser* PUdpHeldItemAimedAction::Analyse()
{
  mDecodeData->mName << "=Held item Action";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 8 );

  ( *nMsg ) >> mWeaponId;
  ( *nMsg ) >> mTargetRawItemID; // !!! 0x000003fe when shooting with no target
  ( *nMsg ) >> mAiming; // aiming ??? 0 to 52 +?
  ( *nMsg ) >> mTargetedHeight; // range 0 (bottom) to 26 (?) (top)
  ( *nMsg ) >> mScore; // range 0 to 255(?)  Score ???

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpHeldItemAimedAction::DoAction()
{
  u16 maxBroadcastDistance = 0; // parameter.
  PClient* nClient = mDecodeData->mClient;
  //PChar* tChar = nClient->GetChar();
//  PCharCoordinates& nCharCoords = nClient->GetChar()->Coords;
  PMessage* tmpMsg;

  // IF APU SPELL
  //10: 03:be:00:1f:[02:00]:25:1a:[68:11:96:43]:[78:de:12:00:] = ????
  //tmpMsg = MsgBuilder->BuildHeldItemUse3Msg( nClient, 0x1168, 0x4369, 0xde78, 0x0012 ); // unknwon use, unknown data (spells duration/delays ?)
  //nClient->SendUDPMessage(tmpMsg);

  //tmpMsg = MsgBuilder->BuildHeldItemUse2Msg( nClient->GetLocalID(), mTargetRawItemID ); // start spell invocation
  //ClientManager->UDPBroadcast( tmpMsg, nClient, maxBroadcastDistance );

  // END IF APU SPELL

  tmpMsg = MsgBuilder->BuildHeldItemUseMsg( nClient->GetLocalID(), mWeaponId, mTargetRawItemID, mAiming, mTargetedHeight, 0 ); // 'score' is not broadcasted, but set to 0
  ClientManager->UDPBroadcast( tmpMsg, nClient, maxBroadcastDistance, true );

  // IF APU SPELL
  // After spell invocation delay
  // 03:8b:00:1f:[02:00]:2c:01:?[ae:bf]?:?[f5:45]?:[00:20:01:00]:[0e]:
  tmpMsg = MsgBuilder->BuildHeldItemUse4Msg( nClient->GetLocalID(), mTargetRawItemID, 0xaebf, 0x45f5, mTargetedHeight );
  ClientManager->UDPBroadcast( tmpMsg, nClient, maxBroadcastDistance );
  // END IF APUI SPELL

  //if ( gDevDebug )
  Console->Print( "%s Handled item action toward target %d (0x%08x) weaponId=%d aiming=%d 'height'=%d 'score'=%d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTargetRawItemID, mTargetRawItemID, mWeaponId, mAiming, mTargetedHeight, mScore );
  //mDecodeData->mMessage->Dump();

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}


/**** PUdpHeldItemLaunchingAction ****/

PUdpHeldItemLaunchingAction::PUdpHeldItemLaunchingAction( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x20";
}

PUdpMsgAnalyser* PUdpHeldItemLaunchingAction::Analyse()
{
  mDecodeData->mName << "=Start Lauching action";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 8 );

  ( *nMsg ) >> mWeaponId;
  ( *nMsg ) >> mSourceY;
  ( *nMsg ) >> mSourceZ;
  ( *nMsg ) >> mSourceX;
  ( *nMsg ) >> mSourceUD;
  ( *nMsg ) >> mSourceLR;
  ( *nMsg ) >> mUnknown1;
  ( *nMsg ) >> mUnknown2; // client timestamp ? => TODO: compare with data from ping request
  ( *nMsg ) >> mTargetRawItemID;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
  /*
1b 03 73 00 1f 01 00 20
a6 00
81 7d
f0 7b
50 7e
6e
e2
7d 20
30 ad db 45
fe 03 00 00
  */
}

bool PUdpHeldItemLaunchingAction::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();
  PWorld* CurrentWorld = Worlds->GetWorld( tChar->GetLocation() );
  const PFurnitureItemTemplate* tFurnitureTemplate = NULL;
  const PDefWorldModel* tFurnitureModel = NULL;

  u32 mRawItemID = mDecodeData->mMessage->U32Data( mDecodeData->Sub0x13Start + 24 );
  //if(gDevDebug) Console->Print("Client %d wants to hack itemID %d ***not managed yet***", mDecodeData->mClient->GetID(), mRawItemID);

  // First try to find out if we're hacking an dynamic actor
  if ( WorldActors->IsDynamicActor( mRawItemID ) == true )
  {
    // Now get the get the function value: (What kind of hackable object)
    int tFunctionVal = WorldActors->GetWorldActorFunctionID( mRawItemID );

    // Then get the FUNCTION VALUE as furniture model so we can access its subvalues etc. Here: Hack difficult
    tFurnitureModel = GameDefs->WorldModels()->GetDef( tFunctionVal );
  }
  else
  {
    // Dat files have smaller IDs
    u32 ItemID = mRawItemID / 1024 - 1;

    // Now grab the template from .dat file
    tFurnitureTemplate = CurrentWorld->GetFurnitureItemTemplate( ItemID );

    // Then get the FUNCTION VALUE as furniture model so we can access its subvalues etc
    tFurnitureModel = CurrentWorld->GetFurnitureItemModel( ItemID );
  }

  if ( tFurnitureModel ) // We have an valid worldobject? Fine. Then start the hackgame
  {
    u8 tHackDifficult = tFurnitureModel->GetHackDifficulty();
    u8 tHackPenalty = tFurnitureModel->GetHackPenalty();

    // Print it!
    if(tHackDifficult)
    {
      //if (gDevDebug)
      Console->Print( "%s Client trying to hack itemID %d. Hack difficult: %d Hack penalty %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mRawItemID, tHackDifficult, tHackPenalty );

      PMessage* tmpMsg = MsgBuilder->BuildStartHackGameMsg( nClient, mRawItemID, tHackDifficult );
      nClient->SendUDPMessage( tmpMsg );
      tmpMsg = NULL;
    }
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpItemAddonActivation ****/

PUdpItemAddonActivation::PUdpItemAddonActivation( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x0c";
}

PUdpMsgAnalyser* PUdpItemAddonActivation::Analyse()
{
  mDecodeData->mName << "=Held item addon activation/deactivation";

  mAddonIdx = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 9 );

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpItemAddonActivation::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  //PChar* tChar = nClient->GetChar();
  nClient->testval8 ^= ( 1 << mAddonIdx ); // Toggle state // Using testval8 for testing only!!!

  PMessage* tmpMsg =  MsgBuilder->BuildHeldItemAddonActivationMsg( nClient, nClient->testval8 ); // For testing only!!!
  ClientManager->UDPBroadcast( tmpMsg, nClient );

  if ( gDevDebug )
    Console->Print( "%s Handled item (de)activation of addon %d (0x%02x)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mAddonIdx, nClient->testval8 );

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
