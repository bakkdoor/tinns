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

	udp_packet0.h - decoder classes for UDP "Packet0" messages
  
	CREATION: 31 Aug 2006 Hammag

	MODIFIED:
	REASON: - 

*/

#include "main.h"
#include "udp_packet0.h"

/**** PUdpPacket0 ****/

PUdpPacket0::PUdpPacket0(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x2a";
//nDecodeData->mTraceDump=true;
//mDecodeData->mTraceKnownMsg = true;
} 

PUdpMsgAnalyser* PUdpPacket0::Analyse()
{
  mDecodeData->mName << "=Packet0";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  
  return this;
}

bool PUdpPacket0::DoAction()
{
  if (mDecodeData->mState & DECODE_ACTION_READY)
  {
    PMessage* tmpMsg = MsgBuilder->BuildPacket0Msg(mDecodeData->mClient);
    mDecodeData->mClient->SendUDPMessage(tmpMsg);

    //mDecodeData->mMessage->SetNextByteOffset(mDecodeData->Sub0x13StartNext);    
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
  }
  else
    return false; 
}
