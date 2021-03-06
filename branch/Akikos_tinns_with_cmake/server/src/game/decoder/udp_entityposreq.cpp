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
	udp_entityposreq.cpp - decoder classes for UDP entity position request messages

	CREATION: 8 jun 2007 Hammag

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "udp_entityposreq.h"

#include "include/worlds.h"


PUdpEntityPosRequest::PUdpEntityPosRequest(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x0b";
}

PUdpMsgAnalyser* PUdpEntityPosRequest::Analyse()
{
  mDecodeData->mName << "=Entity position request";

//mDecodeData->mTraceUnknownMsg = true; // temp stop being bugged with unknown move msg
//mDecodeData->mTraceKnownMsg = true;

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 7);

  *nMsg >> mEntityID; //(u16 ... or u32 ???)

  if(mEntityID >= WORLDDATATEMPLATE_MAXPOSITEMS)
  {
    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    mDecodeData->mErrorDetail = Ssprintf("Invalid position entity ID (%d)", mEntityID);
    Console->Print("%s Client[%d] sent invalid position entity Id[%d]", Console->ColorText(YELLOW, BLACK, "[Notice]"), mDecodeData->mClient->GetID(), mEntityID);
    mDecodeData->mTraceDump = true;
  }
  else
  {
    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  }

  return this;
}

bool PUdpEntityPosRequest::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();
  PWorld* currentWorld = Worlds->GetWorld(nChar->GetLocation());
  f32 fpX, fpY, fpZ;
  u16 pX, pY, pZ;

  if(currentWorld)
  {
    if((mEntityID < WORLDDATATEMPLATE_MAXPOSITEMS) && currentWorld->getPositionItemPosition(mEntityID, &fpX, &fpY, &fpZ))
    {
      pX = (u16) (fpX + 32000);
      pY = (u16) (fpY + 32000);
      pZ = (u16) (fpZ + 32000);

if(gDevDebug) Console->Print(GREEN, BLACK, "Client %d - Sending pos for entity %d : X=%d Y=%d Z=%d", mDecodeData->mClient->GetID(), mEntityID, pX, pY, pZ);
    }
    else if(!nClient->GetCharAwaitingWarpto(&pX, &pY, &pZ))
    {
      pX = pY = pZ = 0;
      Console->Print("%s Client[%d] requested invalid position entity %d. Position reset.", Console->ColorText(YELLOW, BLACK, "[Notice]"), mDecodeData->mClient->GetID(), mEntityID);
    }
    
    PMessage* tmpMsg;
    tmpMsg = MsgBuilder->BuildEntityPositionMsg(nClient, pX, pY, pZ);
    nClient->SendUDPMessage(tmpMsg);
    (nChar->Coords).mY=pY;
    (nChar->Coords).mZ=pZ;
    (nChar->Coords).mX=pX;
  }
  else
  {
    Console->Print(RED, BLACK, "[Warning] PUdpEntityPosRequest - Invalid world (%d)", nChar->GetLocation());
  }
//    if(IsRealMove && nClient->GetDebugMode(DBG_LOCATION))

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
