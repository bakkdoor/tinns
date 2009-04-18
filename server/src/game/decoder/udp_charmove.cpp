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

 udp_charmove.cpp - decoder classes for UDP char movement messages

 CREATION: 5 Sep 2006 Hammag

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_charmove.h"

#include "worlds.h"
#include "vehicle.h"
#include "subway.h"

#define JUMPHEIGHT 80

/**** PUdpCharPosUpdate ****/

PUdpCharPosUpdate::PUdpCharPosUpdate( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x20";
}

PUdpMsgAnalyser* PUdpCharPosUpdate::Analyse()
{
  mDecodeData->mName << "=Char position update";

//mDecodeData->mTraceUnknownMsg = true; // temp stop being bugged with unknown move msg
//mDecodeData->mTraceKnownMsg = true;

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 4 );

  *nMsg >> mInfoBitfield;
//bitfield [C?AL UXZY] packet read from LSB
//          8.21 8421
//Console->Print(YELLOW, BLACK, "[DEBUG] PUdpCharPosUpdate: Bitfield value %02x", mInfoBitfield);
  if ( mInfoBitfield & 0x80 ) // Sitting on chair
  {
    if ( mInfoBitfield & 0x7f )
    {
      mDecodeData->mName << " + mixed bitfield (" << mInfoBitfield << ")";
      mDecodeData->mState = DECODE_UNKNOWN | DECODE_FINISHED;
      Console->Print( YELLOW, BLACK, "[DEBUG] PUdpCharPosUpdate: Client %d sent Mixed field value %x", mDecodeData->mClient->GetID(), mInfoBitfield );
    }
    else
    {
      mDecodeData->mName << " (Char sitting)";

      *nMsg >> mChairItemID;
      *nMsg >> mChairItemSeat;
      mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
//mDecodeData->mTraceKnownMsg = true;
//mDecodeData->mTraceDump = true;
      if ( gDevDebug ) Console->Print( YELLOW, BLACK, "[DEBUG] Localid %d sitting on object %d (0x%08x) seat %d", mDecodeData->mClient->GetLocalID(), mChairItemID, mChairItemID, mChairItemSeat );
    }
  }
  else
  {
    if ( mInfoBitfield & 0x01 )
    {
      *nMsg >> mNewY;
    }
    if ( mInfoBitfield & 0x02 )
    {
      *nMsg >> mNewZ;
    }
    if ( mInfoBitfield & 0x04 )
    {
      *nMsg >> mNewX;
    }
    if ( mInfoBitfield & 0x08 )
    {
      *nMsg >> mNewUD;
    }
    if ( mInfoBitfield & 0x10 )
    {
      *nMsg >> mNewLR;
    }
    if ( mInfoBitfield & 0x20 )
    {
      *nMsg >> mNewAct;
    }
    if ( mInfoBitfield & 0x40 ) // purpose unknown
    {
      *nMsg >> mNewUnknown;
      if ( gDevDebug && mNewUnknown )
      {
        Console->Print( YELLOW, BLACK, "[DEBUG] PUdpCharPosUpdate: Client %d sent field 0x40 (in %x) value %x", mDecodeData->mClient->GetID(), mInfoBitfield, mNewUnknown );
      }
    }

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  }

  return this;
}

bool PUdpCharPosUpdate::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();
  PMessage* tmpMsg;

  if ( mInfoBitfield & 0x80 ) // Sitting on chair
  {
    //tmpMsg = MsgBuilder->BuildCharSittingMsg(nClient, mChairItemID);
    //tmpMsg = MsgBuilder->BuildCharSittingMsg(nClient);
    //ClientManager->UDPBroadcast(tmpMsg, nClient, 5000); // TODO: Get the range from config
    tmpMsg = MsgBuilder->BuildCharPosUpdateMsg( nClient );
    ClientManager->UDPBroadcast( tmpMsg, nClient, 5000 ); // TODO: Get the range from config
  }
  else
  {
    bool IsRealMove = false;

    if (( mInfoBitfield & 0x01 ) && ( nChar->Coords.mY != mNewY ) )
    {
      nChar->Coords.mY = mNewY;
      IsRealMove = true;
    }
    if (( mInfoBitfield & 0x02 ) && ( nChar->Coords.mZ != mNewZ ) )
    {
      nChar->Coords.mZ = mNewZ;
      IsRealMove = true;
    }
    if (( mInfoBitfield & 0x04 ) && ( nChar->Coords.mX != mNewX ) )
    {
      nChar->Coords.mX = mNewX;
      IsRealMove = true;
    }
    if ( mInfoBitfield & 0x08 )
    {
      nChar->Coords.mUD = mNewUD;
      //IsRealMove = true;
    }
    if (( mInfoBitfield & 0x10 ) && ( nChar->Coords.mLR != mNewLR ) )
    {
      nChar->Coords.mLR = mNewLR;
      IsRealMove = true;
    }
    if ( mInfoBitfield & 0x20 )
    {
      nChar->Coords.mAct = mNewAct;
    }
    if ( mInfoBitfield & 0x20 )
    {
      nChar->Coords.mUnknown = mNewUnknown;
    }
    // movement action byte mask:
    // 0x00 NC has no focus (player alt+tab'ed out)
    // 0x02 kneeing
    // 0x20 Walk mode (= not run mode)
    // 0x08 left step
    // 0x10 right step
    // 0x40 forward
    // 0x80 backward
    // bits:  00000000
    //        BFWRL.K.

    if ( IsRealMove )
    {
      nChar->SetDirtyFlag();
    }
    /*if(IsRealMove)
    if(mInfoBitfield == 0x7f)
    {
      tmpMsg = MsgBuilder->BuildCharPosUpdateMsg(nClient);
      ClientManager->UDPBroadcast(tmpMsg, nClient, 5000); // TODO: Get the range from config
    }*/

    u16 nSaveZ = nChar->Coords.mZ;
    if(nChar->Coords.mJumpingState)
    {
      mInfoBitfield |= 0x02; // Update Z and Act only
      if(nChar->Coords.mJumpingState == 1)
      {
        if ( gDevDebug ) Console->Print( YELLOW, BLACK, "[DEBUG] Send moving jump");
        (nChar->Coords.mZ) += JUMPHEIGHT;
        nChar->Coords.mJumpingState = 2; // Jump done
      }
      else
        nChar->Coords.mJumpingState = 0; // No pending jump
    }

    tmpMsg = MsgBuilder->BuildCharPosUpdate2Msg( nClient, mInfoBitfield );

    if(nChar->Coords.mJumpingState)
      nChar->Coords.mZ = nSaveZ;

    ClientManager->UDPBroadcast( tmpMsg, nClient, 5000, true );

    if ( IsRealMove && nClient->GetDebugMode( DBG_LOCATION ) )
    {
      char DbgMessage[128];
      f32 f[3];
//      u32 h[3];
      f[0] = nChar->Coords.mY - 32000;
      f[1] = nChar->Coords.mZ - 32000;
      f[2] = nChar->Coords.mX - 32000;
//      memcpy(h, f, 3 * sizeof(u32));
//      snprintf(DbgMessage, 128, "position y:%0.1f (0x%08x) z:%0.1f (0x%08x) x:%0.1f (0x%08x)", f[0], h[0], f[1], h[1], f[2], h[2]);
      snprintf( DbgMessage, 128, "position y:%0.1f z:%0.1f x:%0.1f lr:%d (Act=%x BF=%x)", f[0], f[1], f[2], nChar->Coords.mLR, nChar->Coords.mAct , mInfoBitfield );
      Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
    }
  }
//if(IsRealMove)
//Console->Print("Char %d position : X(%d) Y(%d) Z(%d) U/D(%d) L/R(%d) Action(%02x)", mDecodeData->mClient->GetID(), nChar->Coords.mX, nChar->Coords.mY, nChar->Coords.mZ, nChar->Coords.mUD, nChar->Coords.mLR, nChar->Coords.mAct);

  if ( mInfoBitfield >= 0x7f )
  {
    tmpMsg = MsgBuilder->BuildCharHealthUpdateMsg( nClient );
    ClientManager->UDPBroadcast( tmpMsg, nClient );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}


/**** PUdpCharExitChair ****/

PUdpCharExitChair::PUdpCharExitChair( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x22";
}

PUdpMsgAnalyser* PUdpCharExitChair::Analyse()
{
  mDecodeData->mName << "=Char exiting chair";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharExitChair::DoAction()
{
  DoLeaveChair( mDecodeData->mClient->GetChar(), mDecodeData->mClient );

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

bool PUdpCharExitChair::DoLeaveChair( PChar* nChar, PClient* nClient, PSpawnedVehicle* nVhc, bool nForce )
{
  if ( ! nClient )
    nClient = ClientManager->getClientByChar( nChar->GetID() );
  if ( ! nClient )
  {
    Console->Print(RED, BLACK, "[Error] PUdpCharExitChair::DoLeaveChair called without PClient* and client not available when searching by char." );
    return false;
  }

  PMessage* tmpMsg;
  u32 cSeatObjectId;
  u8 cSeatId;
  PSeatType cSeatType = nChar->GetSeatInUse( &cSeatObjectId, &cSeatId );
  u32 tNowTime = GameServer->GetGameTime();
  bool ReadyToExit = false;

  // This is only for investigation on subway timing
  if ( nClient->GetDebugMode( DBG_SUBWAY ) && ( cSeatType == seat_subway ) )
  {
    char DbgMessage[80];
    u8 tCabId;
    Subway->GetInfoIndex( cSeatObjectId, &tCabId );
    u8 tStationId = Subway->GetStation( cSeatObjectId, tNowTime );
    std::string* StationName = Subway->GetStationName( tStationId );
    std::string OpenState = ( Subway->IsDoorOpen( cSeatObjectId, tNowTime ) ? "open" : "closed" );

    snprintf( DbgMessage, 80, "Cab %d (%d) is in station %s (%d). Door is %s (time: %d)", tCabId, cSeatObjectId, StationName->c_str(), tStationId, OpenState.c_str(), tNowTime );
    DbgMessage[79] = 0;
    Chat->send( nClient, CHAT_DIRECT, "System", DbgMessage );
    delete StationName;
  }
  // end investigation

  if ( cSeatType )
  {
    PWorld* tWorld = Worlds->GetWorld( nChar->GetLocation() );

    if ( cSeatType == seat_chair )
    {
      tWorld->CharLeaveChair( nClient->GetLocalID(), cSeatObjectId );
      nChar->SetSeatInUse( seat_none );
      ReadyToExit = true;
    }
    else if ( cSeatType == seat_subway )
    {
      if ( Subway->IsDoorOpen( cSeatObjectId, tNowTime ) || nForce )
      {
        Subway->GetStationExitPosition( &( nChar->Coords ), Subway->GetStation( cSeatObjectId, tNowTime ), GetRandomFloat() );
        Subway->UnsetSeatUser( cSeatObjectId, cSeatId, nChar->GetID() );
        nChar->SetSeatInUse( seat_none );
        ReadyToExit = true;
      }
      else
      {
        tmpMsg = MsgBuilder->BuildText100Msg( nClient, 6, cSeatObjectId ); // "Damn, locked"
        nClient->SendUDPMessage( tmpMsg );
      }
    }
    else if ( cSeatType == seat_vhc )
    {
      PSpawnedVehicle* tVhc = tWorld->GetSpawnedVehicules()->GetVehicle( cSeatObjectId );
      if ( tVhc && (!nVhc || (tVhc == nVhc)) )
      {
        PVhcCoordinates tCoords = tVhc->GetPosition();
        tVhc->UnsetSeatUser( cSeatId, nChar->GetID() );
        nChar->SetSeatInUse( seat_none );
        nChar->Coords.SetPosition( tCoords.GetY(), tCoords.GetZ(), tCoords.GetX() ); // to complete with LR
        ReadyToExit = true;
      }
    }
    else
    {
      Console->Print( RED, BLACK, "Error: PUdpCharExitChair::DoLeaveChair : Invalid seat type %d", cSeatType );
    }

    if ( ReadyToExit )
    {
      nChar->SetSeatInUse( seat_none );

      tmpMsg = MsgBuilder->BuildCharExitSeatMsg( nClient );
      ClientManager->UDPBroadcast( tmpMsg, nClient );

      if ( gDevDebug ) Console->Print( "Localchar %d get up from chair %d.", nClient->GetLocalID(), cSeatObjectId );
      return true;
    }
  }

  return false;
}

/**** PUdpCharJump ****/

PUdpCharJump::PUdpCharJump( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x02";
}

PUdpMsgAnalyser* PUdpCharJump::Analyse()
{
  mDecodeData->mName << "=Char jumping";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharJump::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();

  if( nChar->Coords.mAct & 0xd8 ) // if moving, don't jump now and wait next update
  {
    nChar->Coords.mJumpingState = 1; // Jump pending. Do at next pos update
    if ( gDevDebug ) Console->Print( YELLOW, BLACK, "[DEBUG] Prepare moving jump");
  }
  else // If not moving, jump now
  {
    if ( gDevDebug ) Console->Print( YELLOW, BLACK, "[DEBUG] Send static jump");
    u8 mInfoBitfield = 0x22; // Update Z and Act only
    u16 nSaveZ = nChar->Coords.mZ;
    (nChar->Coords.mZ) += JUMPHEIGHT;
    PMessage* tmpMsg = MsgBuilder->BuildCharPosUpdate2Msg( nClient, mInfoBitfield );
    nChar->Coords.mZ = nSaveZ;
    nChar->Coords.mJumpingState = 2; // Jump done
    ClientManager->UDPBroadcast( tmpMsg, nClient, 5000, true );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;

  return true;
}


/**** PUdpCharTargeting ****/

PUdpCharTargeting::PUdpCharTargeting( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x2d";
}

PUdpMsgAnalyser* PUdpCharTargeting::Analyse()
{
  mDecodeData->mName << "=Targeting char";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharTargeting::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();
  u16 CharID = mDecodeData->mMessage->U16Data( mDecodeData->Sub0x13Start + 2 );
  u8 strangeval1 = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 4 );
  u8 strangeval2 = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 5 );
  u8 strangeval3 = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 6 );

  tChar->SetLookingAt( CharID );

  if ( gDevDebug ) Console->Print( "Char %d targeting char %d. // %d %d %d", mDecodeData->mClient->GetID(), CharID, strangeval1, strangeval2, strangeval3 );
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
