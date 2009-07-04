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

 udp_ping.cpp - decoder classes for UDP Ping messages

 CREATION: 31 Aug 2006 Hammag

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_ping.h"

/**** PUdpPing ****/

PUdpPing::PUdpPing( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x0b";
}

PUdpMsgAnalyser* PUdpPing::Analyse()
{
  mDecodeData->mName << "=Ping";

  mClientTime = mDecodeData->mMessage->U32Data( mDecodeData->Sub0x13Start + 2 );

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpPing::DoAction()
{
  if ( mDecodeData->mState & DECODE_ACTION_READY )
  {
    if(gDevDebug)
      Console->Print( "%s PUdpPing: Client timestamp %d (0x%08x)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mClientTime, mClientTime );

    PMessage* tmpMsg = MsgBuilder->BuildPingMsg( mDecodeData->mClient, mClientTime );
    mDecodeData->mClient->SendUDPMessage( tmpMsg );

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
  }
  else
    return false;
}
