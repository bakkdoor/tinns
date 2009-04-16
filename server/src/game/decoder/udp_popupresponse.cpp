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

 udp_popupresponse.cpp - decoder classes for some UDP Popup response messages

 CREATION: 14 Apr 2009 Hammag
*/

#include "main.h"
#include "udp_popupresponse.h"
#include "vhcaccessrequest.h"
#include "worlds.h"


/**** PUdp0x1f ****/

PUdpPopupResponse::PUdpPopupResponse( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x09";
}

PUdpMsgAnalyser* PUdpPopupResponse::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser = NULL;
  mDecodeData->mState = DECODE_MORE;
  u8 MsgType = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 12 ); //u32 but only u8 used

  switch ( MsgType )
  {
    case 0x05:
    {
      nextAnalyser = new PUdpVentureWarpConfirm( mDecodeData );
      break;
    }
    case 0x06:
    {
      nextAnalyser = new PUdpVhcAccessResponse( mDecodeData );
      break;
    }
    default:
    {
      mDecodeData->mUnknownType = MsgType;
      mDecodeData->mTraceUnknownMsg = true;
      break;
    }
  }

  if ( ! nextAnalyser )
  {
    nextAnalyser = new PUdpMsgUnknown( mDecodeData );
  }

  return nextAnalyser;
}

/**** PUdpVentureWarpConfirm ****/

PUdpVentureWarpConfirm::PUdpVentureWarpConfirm( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x05";
}

PUdpMsgAnalyser* PUdpVentureWarpConfirm::Analyse()
{
  mDecodeData->mName << "=Venture Warp confirmation";

  PMessage* cMsg = mDecodeData->mMessage;
  cMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 16 );
  ( *cMsg ) >> mUnknown1;
  ( *cMsg ) >> mUnknown2;
  ( *cMsg ) >> mRawItemId;
  ( *cMsg ) >> mStatus;
  cMsg->Dump();
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpVentureWarpConfirm::DoAction()
{
  PClient* nClient = mDecodeData->mClient;

  if ( mStatus == 1 )
  {
    u32 newLocation;
    do
    {
      newLocation = 2000 + 20 * GetRandom( 10, 0 ) + GetRandom( 16, 1 );
    }
    while ( ! Worlds->IsValidWorld( newLocation ) );

    u16 nEntity = 10;
    u16 nEntityType = 1;

    PMessage* tmpMsg = MsgBuilder->BuildAptLiftUseMsg( nClient, newLocation, nEntity, nEntityType );
    nClient->getUDPConn()->SendMessage( tmpMsg );

    if ( gDevDebug )
      Console->Print( "Client[%d]: Venture Warping to zone %d (entity %d/%d)", nClient->GetID(), newLocation, nEntity, nEntityType );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}


/**** PUdpVhcAccessResponse ****/

PUdpVhcAccessResponse::PUdpVhcAccessResponse( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x06";
}

PUdpMsgAnalyser* PUdpVhcAccessResponse::Analyse()
{
  mDecodeData->mName << "=Response to vhc access request";

  PMessage* cMsg = mDecodeData->mMessage;
  cMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 16 );
  ( *cMsg ) >> mVhcAccessRequestId;
  ( *cMsg ) >> mUnknown;
  ( *cMsg ) >> mStatus;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpVhcAccessResponse::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();

  //if (gDevDebug)
    Console->Print( "Access response for Req n°%d : %d (unknown %d)", mVhcAccessRequestId, mStatus, mUnknown );

  if ( nChar->GetVhcAccessRequestList()->RegisterResponse( mVhcAccessRequestId, ( mStatus == 1 ) ) )
  {
    /*
    PWorld* CurrentWorld = Worlds->GetWorld( nChar->GetLocation() );
    if ( CurrentWorld )
    {
      PSpawnedVehicle* tVhc = CurrentWorld->GetSpawnedVehicules()->GetVehicle( mVehicleID );
      if ( tVhc )
      {
    */
    //PUdpVhcUse::DoFreeSitting( nClient,  tVhc, mRawItemID );
    /*
                  u8 freeSeats = tVhc->GetFreeSeatsFlags();
                  if ( freeSeats )
                  {
                    tmpMsg = MsgBuilder->BuildCharUseVhcMsg( nClient, mRawItemID, tVhc->GetInformation().GetVehicleType(), freeSeats );
                    nClient->SendUDPMessage( tmpMsg ); // Open seat selection window
                  }
    */
  }
  else
  {
////tmpMsg = MsgBuilder->BuildText100Msg( nClient, 19, mRawItemID ); // "req transmitted" msg
                ////  nClient->SendUDPMessage( tmpMsg );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
