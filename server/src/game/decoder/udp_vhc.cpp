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

 udp_vhc.h - decoder classes for UDP vehicle related messages

 CREATION: 5 Sep 2006 Hammag

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "worlds.h"
#include "vehicle.h"
#include "udp_vhc.h"
#include "subway.h"

/**** PUdpVhcMove ****/

PUdpVhcMove::PUdpVhcMove( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x32";
}

PUdpMsgAnalyser* PUdpVhcMove::Analyse()
{
  mDecodeData->mName << "=Vhc move";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 2 );

  *nMsg >> mVhcLocalId;
  *nMsg >> mMoveType; // 0 for subway/chair, 3 for vhc, 7 in nc2.2
  *nMsg >> mNewY;
  *nMsg >> mNewZ;
  *nMsg >> mNewX;
  *nMsg >> mNewUD; // neutral :=0x7f, > := upward, < := downward
  *nMsg >> mNewLR;
  *nMsg >> mNewRoll; // neutral := 0x7fff > := bank left, < := bank right
  *nMsg >> mUnk1;
  *nMsg >> mFF;
  *nMsg >> mAction;
//0 = not moving
//&01 = Left
//&02 = Right
//&04 = Forward
//&08 = Back
//&10 = Shoot button
//&20 = Pushing down
//&40 = Pulling up
//
  /*
  --- rolling front+left
  13 b0 00 02 bf
  15
  32
  c9 03 =short VhcId
  07
  c3 66 37 8b 47 6e 80 2a 88 70 27 22
  81
  01 00
  ff = speed% ?
  05 = forward+left ?
  => srv resp
  13 6b 01 bd bf
  18
  03 6a 01 32 c9 03 07 8b 66 37 8b d9 6d 80 e0 8f
  ea 27 22 81 01 00 ff 05
  0c
  03 6b 01 1f 01 00 30 5e 79 36 87 87
  13
  32 c9 03 03 8b 66 38 8b d9 6d 7f 2a 88 92 7f 01 00 00 05
  0b
  20 ed 03 5d 5a c8 8d 9b 7b c2 00

  --- stopped
  13 88 01 da bf
  13
  32
  c9 03 =short VhcId
  03
  05
  66 36 8b 90 6d 7f 5b 9d 00
  80
  01 00
  00 = speed% ?
  00 = no move

  */
  if ( gDevDebug )
  {
    Console->Print( YELLOW, BLACK, "[DEBUG] VHC move type %d - objid %d", mMoveType, mVhcLocalId );
    //nMsg->Dump();
    Console->Print( "X=%d Y=%d Z=%d Act=%d", mNewX - 768, mNewY - 768, mNewZ - 768, mAction );
    Console->Print( "LR=%d UD=%d Ro=%d Unk=%d Act=%02x ff=%x", mNewLR, mNewUD, mNewRoll, mUnk1, mAction, mFF );
    Console->Print( "Msg len: %d", nMsg->U8Data( mDecodeData->Sub0x13Start ) );
  }

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpVhcMove::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  //PCharCoordinates* nCoords = &(nClient->GetChar()->Coords);
  PWorld* CurrentWorld = Worlds->GetWorld( nClient->GetChar()->GetLocation() );

  if ( CurrentWorld )
  {
    PSpawnedVehicle* tVhc = CurrentWorld->GetSpawnedVehicules()->GetVehicle( mVhcLocalId );
    if ( tVhc )
    {
      //Todo: calc & mem Speed & Accel vectors
      PVhcCoordinates nPos, destPos;
      nPos.SetPosition( mNewY - 768, mNewZ - 768, mNewX - 768, mNewUD, mNewLR, mNewRoll, mAction, mUnk1, mFF );
      tVhc->SetPosition( &nPos );
      PMessage* tmpMsg = MsgBuilder->BuildVhcPosUpdate2Msg( tVhc );
      ClientManager->UDPBroadcast( tmpMsg, mDecodeData->mClient );

      tmpMsg = MsgBuilder->BuildVhcPosUpdateMsg( tVhc );
      ClientManager->UDPBroadcast( tmpMsg, mDecodeData->mClient );

      u32 destWorldId;
      if (( destWorldId = CurrentWorld->GetVhcZoningDestination( tVhc, &destPos ) ) )
      {
        if ( nClient->GetDebugMode( DBG_LOCATION ) )
        {
          u8 pH = 0;
          u8 pV = 0;
          Worlds->GetWorldmapFromWorldId( destWorldId, pH, pV );
          char DbgMessage[128];
          snprintf( DbgMessage, 128, "Vhc zoning to zone %c%02d (id %d)", ( 'a' + pV ), pH, destWorldId );
          Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
        }

        DoVhcZoning( tVhc, nClient->GetChar()->GetLocation(), destWorldId, &destPos );
      }
    }
    else
      Console->Print( RED, BLACK, "[Error] PUdpVhcMove: Inexistant vhc Id %d", mVhcLocalId );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

// Failures are not managed yet
bool PUdpVhcMove::DoVhcZoning( PSpawnedVehicle* currVhc, u32 currWorldId, u32 destWorldId, PVhcCoordinates* destPos )
{
  u32 seatedCharsId[8];
  u32 vhcGlobalId = currVhc->GetVehicleId();
  u32 vhcLocalId = currVhc->GetLocalId();
  PVhcCoordinates currCoords = currVhc->GetPosition();
  u8 numSeats = currVhc->GetNumSeats();
  u32 sittingCharId;
  PClient* sittingClient;
  PClient* sittingClients[8];
  PMessage* tmpMsg;

  for ( u8 i = 0; i < numSeats; ++i )
  {
    // Save seated chars list
    seatedCharsId[i] = sittingCharId = currVhc->GetSeatUser( i );

    if ( sittingCharId )
    {
      sittingClients[i] = sittingClient = ClientManager->getClientByChar( sittingCharId );
      // Tag each client as zoning to avoid transient side effects
      sittingClient->SetZoning();
      // Trigger zoning
      //tmpMsg = MsgBuilder->BuildGenrepZoningMsg( sittingClient, destWorldId, 0 ); // unknown value // 0x62bc or 0x2d4e
      //sittingClient->SendUDPMessage( tmpMsg );
      // We send the unseat msg to the corresponding client only.
      sittingClient->GetChar()->Coords.SetPosition( currCoords.GetY(), currCoords.GetZ(), currCoords.GetX() );
      tmpMsg = MsgBuilder->BuildCharExitSeatMsg( sittingClient );
      sittingClient->FillInUDP_ID( tmpMsg );
      sittingClient->SendUDPMessage( tmpMsg );
    }
    else
      sittingClients[i] = 0;
  }

  // Unspawn vhc instance from local world
  Vehicles->UnspawnVehicle( vhcGlobalId );
  tmpMsg = MsgBuilder->BuildRemoveWorldObjectMsg( vhcLocalId );
  ClientManager->UDPBroadcast( tmpMsg, currWorldId );

  // Spawn vhc instance in destWorld
  PSpawnedVehicle* destVhc = Vehicles->SpawnVehicle( vhcGlobalId, destWorldId, destPos );
  if ( destVhc )
  {
    tmpMsg = MsgBuilder->BuildVhcPosUpdateMsg( destVhc );
    ClientManager->UDPBroadcast( tmpMsg, destWorldId );

    vhcLocalId = destVhc->GetLocalId();

    // Update chars seat in use and restore vhc used seats
    PChar* sittingChar;
    for ( u8 i = 0; i < numSeats; ++i )
    {
      if (( sittingClient = sittingClients[i] ) )
      {
        Console->Print( "%s PUdpVhcMove::DoVhcZoning : Char %d sitting on vhc %d, seat %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), seatedCharsId[i], vhcLocalId, i );
        sittingChar = sittingClient->GetChar();
        sittingChar->SetSeatInUse( seat_vhc, vhcLocalId, i );
        sittingChar->Coords.SetPosition( destPos->GetY(), destPos->GetZ(), destPos->GetX() );
        destVhc->SetSeatUser( i, seatedCharsId[i] );
      }
    }
  }
  else
  {
    for ( u8 i = 0; i < numSeats; ++i )
    {
      if ( sittingClients[i] )
      {
        sittingClients[i]->GetChar()->SetSeatInUse( seat_none );
      }
    }

    return false;
  }

  return true;

}

/**** PUdpVhcUse ****/

PUdpVhcUse::PUdpVhcUse( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x0f";
}

PUdpMsgAnalyser* PUdpVhcUse::Analyse()
{
  mDecodeData->mName << "=Try enter vhc";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 12 );
  *nMsg >> mVehicleID; // u32
  *nMsg >> mVehicleSeat;

  if ( gDevDebug )
    Console->Print( "%s Localid %d trying to enter vhc %d on seat %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mDecodeData->mClient->GetLocalID(), mVehicleID, mVehicleSeat );
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpVhcUse::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();
  PMessage* tmpMsg;

  if ( nChar->GetSeatInUse() == seat_none ) // Refuse if Char is already sitting somewhere
  {
    PWorld* CurrentWorld = Worlds->GetWorld( nChar->GetLocation() );
    if ( CurrentWorld )
    {
      PSpawnedVehicle* tVhc = CurrentWorld->GetSpawnedVehicules()->GetVehicle( mVehicleID );
      if ( tVhc )
      {
        if ( tVhc->SetSeatUser( mVehicleSeat, nChar->GetID() ) ) // Char was able to sit
        {
          nChar->SetSeatInUse( seat_vhc, mVehicleID, mVehicleSeat );
          tmpMsg = MsgBuilder->BuildCharUseSeatMsg( nClient, mVehicleID, mVehicleSeat );
          ClientManager->UDPBroadcast( tmpMsg, nClient );
        }
        else
        {
          tmpMsg = MsgBuilder->BuildText100Msg( nClient, 1, mVehicleID ); // Already in use
          nClient->SendUDPMessage( tmpMsg );
        }
      }
    }
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

void PUdpVhcUse::DoFreeSitting( PClient* nClient,  PSpawnedVehicle* nVhc, u32 nRawVhcLocalId, u8 nSeatId )
{
  PMessage* tmpMsg;

  if (( nVhc->GetNbFreeSeats() > 1 ) && ( nSeatId > nVhc->GetNumSeats() ) )
  {
    u8 freeSeats = nVhc->GetFreeSeatsFlags();
    tmpMsg = MsgBuilder->BuildCharUseVhcMsg( nClient, nRawVhcLocalId, nVhc->GetInformation().GetVehicleType(), freeSeats );
    nClient->SendUDPMessage( tmpMsg ); // Open seat selection window
  }
  else
  {
    if ( nSeatId <= nVhc->GetNumSeats() )
    {
      if ( nVhc->GetSeatUser( nSeatId ) )
      {
        tmpMsg = MsgBuilder->BuildText100Msg( nClient, 1, nRawVhcLocalId ); // Already in use
        nClient->SendUDPMessage( tmpMsg );
      }
    }
    else
    {
      nSeatId = nVhc->GetFirstFreeSeat();

      if ( nSeatId != 255 )
      {
        if ( nVhc->SetSeatUser( nSeatId, nClient->GetChar()->GetID() ) ) // Char was able to sit
        {
          nClient->GetChar()->SetSeatInUse( seat_vhc, nRawVhcLocalId, nSeatId );
          tmpMsg = MsgBuilder->BuildCharUseSeatMsg( nClient, nRawVhcLocalId, nSeatId );
          ClientManager->UDPBroadcast( tmpMsg, nClient );
        }
        else
        {
          tmpMsg = MsgBuilder->BuildText100Msg( nClient, 0, nRawVhcLocalId ); // Undefined failure
          nClient->SendUDPMessage( tmpMsg );
        }
      }
      else
      {
        tmpMsg = MsgBuilder->BuildText100Msg( nClient, 5, nRawVhcLocalId ); // "No free seat" msg
        nClient->SendUDPMessage( tmpMsg );
      }
    }
  }
}

/**** PUdpSubwayUpdate ****/

PUdpSubwayUpdate::PUdpSubwayUpdate( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x00";
}

PUdpMsgAnalyser* PUdpSubwayUpdate::Analyse()
{
  u8 Dumb;

  mDecodeData->mName << "=Subway update";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 2 );

  *nMsg >> mVehicleID;
  *nMsg >> Dumb;
  *nMsg >> mPosition;
  *nMsg >> mDoorOpened;

  if ( gDevDebug )
    Console->Print( "%s Subway update 0x%4x : pos 0x%4x, status 0x%2x", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mVehicleID, mPosition, mDoorOpened );

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpSubwayUpdate::DoAction()
{
  /* No real use for that ?
    Subway->UpdateInfo(mVehicleID, mPosition, mDoorOpened);

    PMessage* tmpMsg = MsgBuilder->BuildSubwaySingleUpdateMsg(mVehicleID, mPosition, mDoorOpened);
    ClientManager->UDPBroadcast(tmpMsg, mDecodeData->mClient);
  */
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpRequestVhcInfo ****/

PUdpRequestVhcInfo::PUdpRequestVhcInfo( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x27";
}

PUdpMsgAnalyser* PUdpRequestVhcInfo::Analyse()
{
  mDecodeData->mName << "=Request seatable object info";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 2 );

  *nMsg >> mVehicleID;

  if ( gDevDebug )
    Console->Print( "%s Request Seatable Info for 0x%04x :", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mVehicleID );

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpRequestVhcInfo::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  //PCharCoordinates* nCoords = &(nClient->GetChar()->Coords);
  PWorld* CurrentWorld = Worlds->GetWorld( nClient->GetChar()->GetLocation() );

  if ( CurrentWorld )
  {
    PSpawnedVehicle* tVhc = CurrentWorld->GetSpawnedVehicules()->GetVehicle( mVehicleID );
    if ( tVhc )
    {
      if ( gDevDebug )
        Console->Print("%s Sending Info for vhcId 0x%04x : type %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mVehicleID, tVhc->GetInformation().GetVehicleType() );
      PMessage* tmpMsg = MsgBuilder->BuildVhcInfoMsg( nClient, tVhc );
      nClient->SendUDPMessage( tmpMsg );
    }
    else
      Console->Print( RED, BLACK, "[Error] PUdpRequestVhcInfo: Inexistant vhc Id %d", mVehicleID );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
