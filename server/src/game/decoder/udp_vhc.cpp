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

PUdpVhcMove::PUdpVhcMove(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x32";
} 

PUdpMsgAnalyser* PUdpVhcMove::Analyse()
{  
  mDecodeData->mName << "=Vhc move";
  
  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 2);
  
  *nMsg >> mVhcLocalId;
  *nMsg >> mMoveType; // 0 for subway, 3 for vhc
  *nMsg >> mNewY;
  *nMsg >> mNewZ;
  *nMsg >> mNewX;
  *nMsg >> mNewUD;
  *nMsg >> mNewLR;
  *nMsg >> mNewRoll;
  *nMsg >> mUnk1;
  *nMsg >> mFF;
  *nMsg >> mAction;
//0 = not moving
//&01 = Left
//&02 = Right
//&04 = Forward
//&08 = Back
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
  
  Console->Print(YELLOW, BLACK, "[DEBUG] VHC move type %d - objid %d", mMoveType, mVhcLocalId);
  //nMsg->Dump();
  Console->Print("X=%d Y=%d Z=%d Act=%d", mNewX, mNewY, mNewZ, mAction);
  Console->Print("LR=%d UD=%d Ro=%d Unk=%d Act=%02x ff=%x", mNewLR, mNewUD, mNewRoll, mUnk1, mAction, mFF);
 
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;  
  return this;
}

bool PUdpVhcMove::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  //PCharCoordinates* nCoords = &(nClient->GetChar()->Coords);
  PWorld* CurrentWorld = Worlds->GetWorld(nClient->GetChar()->GetLocation());

  if(CurrentWorld)
  {
    PSpawnedVehicle* tVhc = CurrentWorld->GetSpawnedVehicules()->GetVehicle(mVhcLocalId);
    if(tVhc)
    {
      //Todo: calc & mem Speed & Accel vectors
      PVhcCoordinates nPos;
      nPos.SetPosition(mNewY-768, mNewZ-768, mNewX-768, mNewLR, mNewUD, mNewRoll, mAction);
      tVhc->SetPosition(&nPos);
      PMessage* tmpMsg = MsgBuilder->BuildVhcPosUpdateMsg (tVhc);
      ClientManager->UDPBroadcast(tmpMsg, mDecodeData->mClient);
    }
    else
      Console->Print(RED, BLACK, "[Error] PUdpVhcMove: Inexistant vhc Id %d", mVhcLocalId);
  }
   
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}


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
  *nMsg >> mVehicleID;
  *nMsg >> dumb;
  *nMsg >> mVehicleSeat;

Console->Print(YELLOW, BLACK, "[DEBUG] Localid %d trying to enter vhc %d on seat %d (dumb=%d)", mDecodeData->mClient->GetLocalID(), mVehicleID, mVehicleSeat, dumb);
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpVhcUse::DoAction()
{  
  PMessage* tmpMsg = MsgBuilder->BuildCharUseSeatMsg (mDecodeData->mClient, mVehicleID, mVehicleSeat);
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
  u8 Dumb;
  
  mDecodeData->mName << "=Subway update";
  
  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 2);
  
  *nMsg >> mVehicleID;
  *nMsg >> Dumb;
  *nMsg >> mPosition;
  *nMsg >> mDoorOpened;
  
  if(gDevDebug)  
    Console->Print(YELLOW, BLACK, "[DEBUG] Subway update 0x%4x : pos 0x%4x, status 0x%2x", mVehicleID, mPosition, mDoorOpened);

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

/**** PUdpRequestSeatInfo ****/

PUdpRequestSeatInfo::PUdpRequestSeatInfo(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x27";
} 

PUdpMsgAnalyser* PUdpRequestSeatInfo::Analyse()
{  
  mDecodeData->mName << "=Request seat object info";
  
  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 2);
  
  *nMsg >> mVehicleID;
  
  //if(gDevDebug)  
    Console->Print(YELLOW, BLACK, "[DEBUG] Request Seat Info for 0x%04x :", mVehicleID);

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpRequestSeatInfo::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  //PCharCoordinates* nCoords = &(nClient->GetChar()->Coords);
  PWorld* CurrentWorld = Worlds->GetWorld(nClient->GetChar()->GetLocation());

  if(CurrentWorld)
  {
    PSpawnedVehicle* tVhc = CurrentWorld->GetSpawnedVehicules()->GetVehicle(mVehicleID);
    if(tVhc)
    {
Console->Print(YELLOW, BLACK, "[DEBUG] Sending Info for vhcId 0x%04x : type %d", mVehicleID, tVhc->GetInformation().GetVehicleType());  
      PMessage* tmpMsg = MsgBuilder->BuildVhcInfoMsg (nClient, tVhc);
      nClient->SendUDPMessage(tmpMsg);
    }
    else
      Console->Print(RED, BLACK, "[Error] PUdpRequestSeatInfo: Inexistant vhc Id %d", mVehicleID);        
  }
  
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
