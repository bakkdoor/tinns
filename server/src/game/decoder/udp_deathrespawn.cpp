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

  udp_deathrespawn.cpp - decoder classes for UDP respawn selection after death message

  CREATION: 10 May 2009 Hammag

  MODIFIED:
  REASON: -

*/

#include "main.h"
#include "udp_deathrespawn.h"
#include "udp_zoning.h"

/**** PUdpDeathRespawn ****/

PUdpDeathRespawn::PUdpDeathRespawn( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x2f";
}

PUdpMsgAnalyser* PUdpDeathRespawn::Analyse()
{
  PMessage* tmpMsg = mDecodeData->mMessage;

  mDecodeData->mName << "=Death respawn selection";

  tmpMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 8 );
  ( *tmpMsg ) >> mEntity;
  ( *tmpMsg ) >> mWorldId;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpDeathRespawn::DoAction()
{
  //PClient* nClient = mDecodeData->mClient;
  //PChar* nChar = nClient->GetChar();

  //PMessage* tmpMsg = MsgBuilder->BuildReqInfoAnswerMsg(mDecodeData->mClient, mRequestType, mInfoId, Answer, len);
  //mDecodeData->mClient->SendUDPMessage(tmpMsg);

  //if ( gDevDebug )
    Console->Print( "%s Request to respawn in zone %d, entity %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mWorldId, mEntity );

  if( mWorldId == 0xffffffff )
  {
    PUdpAptGRZoning::DoEffectiveZoning( mDecodeData->mClient );
  }
  else
  {
    PUdpGenrepZoning::DoEffectiveZoning( mDecodeData->mClient, mWorldId, mEntity );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return ( true );
}
