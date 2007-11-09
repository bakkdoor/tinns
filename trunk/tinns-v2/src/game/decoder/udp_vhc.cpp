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
#include "subway.h"

/**** PUdpVhcMove ****/

PUdpVhcMove::PUdpVhcMove(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x32";
} 

PUdpMsgAnalyser* PUdpVhcMove::Analyse()
{
  mDecodeData->mName << "=Vhc move";
  
  PMessage* nMsg = mDecodeData->mMessage;
  //nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 4);
  
  Console->Print(YELLOW, BLACK, "[DEBUG] VHC move %d", nMsg->U8Data(mDecodeData->Sub0x13Start + 4));
  nMsg->Dump();
  
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
    
    
		//			else if (Data[offset+1] == 0x32)
		//			{
						//Position of vehicle and what direction it's going
						//Byte offset+19
						//0 = not moving
						//&1 = Left
						//&2 = Right
						//&4 = Forward
						//&8 = Back
						/*
						13 d7 01 b4 9b 
						16 
						03 d7 01 32 f9 03 03 9b b3 81 84 07 a2 7e 03 bf 84 7f 01 00 ff 04 
						| ID  |     Coords      |         ??               | Forward
						*/
						//32 fb 03 00 00 00 f0 41 00
						//32 f7 03 03 0c 7a 51 86 6a 98 7e c0 ce 1e 27 01 00 00 00

		/*				if (Data[offset+4] == 0x00) //Subway updates
						{
							Network_SubwayUpdate (ClientNum, Data[offset+2], Data[offset+7], Data[offset+9]);							
						}
						else if (Data[offset+4] == 0x03) //Vehicle position updates
						{
							Client_Sockets[ClientNum].CharInfo.Coords[0] = (*(unsigned short*)&Data[offset+5])-768;
							Client_Sockets[ClientNum].CharInfo.Coords[1] = (*(unsigned short*)&Data[offset+7])-768;
							Client_Sockets[ClientNum].CharInfo.Coords[2] = (*(unsigned short*)&Data[offset+9])-768;
							// *(unsigned short*)&Client_Sockets[ClientNum].CharInfo.Coords[3] = *(unsigned short*)&Data[offset+12];
							AddToLog (LOG_DEBUG, "User %i, Packet 0x32 VHC ID %.2x %.2x: X(%.2x %.2x) Y(%.2x %.2x) Z(%.2x %.2x) U/D(%i) L/R(%i)", ClientNum, Data[offset+2], Data[offset+3], Data[offset+5], Data[offset+6], Data[offset+7], Data[offset+8], Data[offset+9], Data[offset+10], Data[offset+11], Data[offset+12], Data[offset+13]);
							Client_SendNPCUpdateData ( ClientNum );
							//Client sends
							//13 69 00 ab ae 13          32 f8 03 03 ee 76 fd 85 0c 90 7c 11 89 5a 28 01 00 ff 06
							//Server Sends
							//13 d5 03 17 b2 16 03 d5 03 32 f8 03 03 ee 76 fd 85 0c 90 7c 11 89 5a 28 01 00 ff 06
							//Test
							//13 33 00 e0 de 19 32 fc 03 03 0d 82 7f 85 1a 95 80 80 96 06 7e 02 00 00 00 01 00 00 80 00 80
							SendBuffer[0] = 0x13;
							SendBuffer[5] = 0x19;
							SendBuffer[6] = 0x32;
							*(unsigned short*)&SendBuffer[7] = *(unsigned short*)&Data[offset+2]+1024;
							SendBuffer[9] = 0x03; //??
							SendBuffer[10] = 0x0d; //??
							*(short*)&SendBuffer[11] = *(short*)&Data[offset+5];
							*(short*)&SendBuffer[13] = *(short*)&Data[offset+7];
							*(short*)&SendBuffer[15] = *(short*)&Data[offset+9];
							SendBuffer[17] = 0x80; 
							SendBuffer[18] = 0x96;
							SendBuffer[19] = 0x06;
							SendBuffer[20] = 0x7e;
							SendBuffer[21] = 0x02;
							SendBuffer[22] = 0x00;
							SendBuffer[23] = 0x00;
							SendBuffer[24] = 0x00;
							SendBuffer[25] = 0x01;
							SendBuffer[26] = 0x00;
							SendBuffer[27] = 0x00;
							SendBuffer[28] = 0x80;
							SendBuffer[29] = 0x00;
							SendBuffer[30] = 0x80;

							Network_SendPacketToZone (SendBuffer, 31, Client_Sockets[ClientNum].CharInfo.Location);
						}    
    
    */
    
    
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

Console->Print(YELLOW, BLACK, "[DEBUG] Trying to enter vhc %d on seat %d", mVehicleID, mVehicleSeat);
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

/**** PUdpSubwayUpdate ****/

PUdpSubwayUpdate::PUdpSubwayUpdate(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x00";
} 

PUdpMsgAnalyser* PUdpSubwayUpdate::Analyse()
{
  mDecodeData->mName << "=Subway update";
  
  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 2);
  
  *nMsg >> mVehicleID;
  nMsg->IncreaseNextByteOffset(3);
  *nMsg >> mPosition;
  *nMsg >> mDoorOpened;
  
  if(gDevDebug)  
    Console->Print(YELLOW, BLACK, "[DEBUG] Subway update 0x%4x : pos 0x%4x, status 0x%2x", mVehicleID, mPosition, mDoorOpened);

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpSubwayUpdate::DoAction()
{
  Subway->UpdateInfo(mVehicleID, mPosition, mDoorOpened);

  PMessage* tmpMsg = MsgBuilder->BuildSubwaySingleUpdateMsg(mDecodeData->mClient, mVehicleID, mPosition, mDoorOpened);
  mDecodeData->mClient->SendUDPMessage(tmpMsg);

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
