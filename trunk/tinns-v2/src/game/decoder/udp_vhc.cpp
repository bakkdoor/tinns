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
#include "udp_vhc.h"

/**** PUdpVhcMove ****/

PUdpVhcMove::PUdpVhcMove(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x32";
} 

PUdpMsgAnalyser* PUdpVhcMove::Analyse()
{
  mDecodeData->mName << "=Vhc move";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  
  return this;
}

bool PUdpVhcMove::DoAction()
{
    /*PMessage* cMsg = mDecodeData->mMessage;
    u32 ClientTime = cMsg->U32Data(mDecodeData->Sub0x13Start+2);
    
    PMessage* tmpMsg = MsgBuilder->BuildPingMsg(mDecodeData->mClient, ClientTime);
    mDecodeData->mClient->getUDPConn()->SendMessage(tmpMsg);*/
    
    //cMsg->SetNextByteOffset(mDecodeData->Sub0x13StartNext);
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
/*
else if(type == 0x32) // Vehicle update
{
    // Vehicle updates
    if(Packet[19] == 0)
    {
        Console->Print("Vehicle going nowhere");
    }
    else
    {
        if(Packet[19] & 1)
            Console->Print("Vehicle going left");
        if(Packet[19] & 2)
            Console->Print("Vehicle going right");
        if(Packet[19] & 4)
            Console->Print("Vehicle going forward");
        if(Packet[19] & 8)
            Console->Print("Vehicle going back");
        Char->SetDirtyFlag();
    }
    //Playerposition update
    Char->Coords.mY = *(u16*)&Packet[5];
    Char->Coords.mZ = *(u16*)&Packet[7];
    Char->Coords.mX = *(u16*)&Packet[9];
    Char->Coords.mUD = Packet[11];
    Char->Coords.mLR = Packet[12];
    Char->Coords.mAct = Packet[13];
    // movement byte:
    // 0x00 NC has no focus (player alt+tab'ed out)
    // 0x20 Char does nothing
    // 0x22 kneeing
    // 0x28 left step
    // 0x30 right step
    // 0x40 walking (not running)
    // 0x60 forward
    // 0xA0 backward
    break;
}
*/


/**** PUdpVhcUse ****/

PUdpVhcUse::PUdpVhcUse(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x0f";
} 

PUdpMsgAnalyser* PUdpVhcUse::Analyse()
{
  u16 dumb;
  mDecodeData->mName << "=Try enter vhc";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 12);
  *nMsg >> mVehicleID; // ? not u32 ???
  *nMsg >> dumb;
  *nMsg >> mVehicleSeat;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpVhcUse::DoAction()
{  
  PMessage* tmpMsg = MsgBuilder->BuildCharEnteringVhcMsg (mDecodeData->mClient, mVehicleID, mVehicleSeat);
  ClientManager->UDPBroadcast(tmpMsg, mDecodeData->mClient);

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
