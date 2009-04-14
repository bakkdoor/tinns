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

	udp_zoning.cpp - decoder classes for UDP Zoning messages

	CREATION: 6 Sep 2006 Hammag

	MODIFIED: 15 Dec 2006 Hammag
	REASON: - added PUdpEndOfZoning management class

*/

#include "main.h"
#include "udp_zoning.h"

#include "udp_sync.h"
#include "worlds.h"
#include "appartements.h"

/**** PUdpZoning1 ****/

PUdpZoning1::PUdpZoning1(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x0d";
}

PUdpMsgAnalyser* PUdpZoning1::Analyse()
{
  mDecodeData->mName << "=Zoning phase 1";
  
  PMessage* cMsg = mDecodeData->mMessage;
  u8 dumb8;
  u16 dumb16;
  //mUnknown = cMsg->U16Data(mDecodeData->Sub0x13Start+7);
  //mNewLocation = cMsg->U32Data(mDecodeData->Sub0x13Start+11);
  //mNewEntity = cMsg->U16Data(mDecodeData->Sub0x13Start+15);
      
  cMsg->SetNextByteOffset(mDecodeData->Sub0x13Start+7);
  *cMsg >> dumb8; // u8 = 0x01 in NC1, other in NC2.2 ?
  *cMsg >> mUnknown; //u8
  *cMsg >> dumb16; //u16 unkown use
  *cMsg >> mNewLocation; //u32
  *cMsg >> mNewEntity; //u16
  //*cMsg >> dumb16; //u16 0x0000
//Console->Print("Zoning Stage 1: New location: %d, Entity %d, Unknown %d", mNewLocation, mNewEntity, (u16)mUnknown);
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpZoning1::DoAction()
{
  mDecodeData->mClient->ChangeCharLocation(mNewLocation);

//Console->Print("Zoning Stage 1: New location: %d", mNewLocation);

  PMessage* tmpMsg = MsgBuilder->BuildZoning1Msg(mDecodeData->mClient, mNewEntity, mUnknown);
  mDecodeData->mClient->getUDPConn()->SendMessage(tmpMsg);

//Console->Print("Zoning Stage 1: packet sent");
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpZoning2 ****/

PUdpZoning2::PUdpZoning2(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x03";
}

PUdpMsgAnalyser* PUdpZoning2::Analyse()
{
  mDecodeData->mName << "=Zoning phase 2";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpZoning2::DoAction()
{
//Console->Print("Zoning Stage 2: Sending Ready packet");
    PMessage* tmpMsg = MsgBuilder->BuildZoningTCPReadyMsg(); // Not always sent (dongeon door 20)
    mDecodeData->mClient->getTCPConn()->SendMessage(tmpMsg);

//Console->Print("Zoning Stage 2: Sending Zone information");
    PUdpSync0::GetToSync1(mDecodeData);

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

/**** PUdpGenrepZoning ****/

PUdpGenrepZoning::PUdpGenrepZoning(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x04";
}

PUdpMsgAnalyser* PUdpGenrepZoning::Analyse()
{
  mDecodeData->mName << "=Genrep Zoning";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpGenrepZoning::DoAction()
{
  PMessage* cMsg = mDecodeData->mMessage;
  PClient* nClient = mDecodeData->mClient;

  u32 newLocation = cMsg->U16Data(mDecodeData->Sub0x13Start+12);
  u16 nData = cMsg->U16Data(mDecodeData->Sub0x13Start+16);

  PMessage* tmpMsg = MsgBuilder->BuildGenrepZoningMsg(nClient, newLocation, nData);
  nClient->getUDPConn()->SendMessage(tmpMsg);

	//Client_Sockets[ClientNum].CharInfo.Flags = PFLAG_ZONING; //Player started zoning
  nClient->ChangeCharLocation(newLocation);

	tmpMsg = MsgBuilder->BuildZoning1Msg(nClient, nData);
  nClient->getUDPConn()->SendMessage(tmpMsg);

if (gDevDebug) Console->Print("Client[%d]: Genrep Zoning to zone %d (data %d)", nClient->GetID(), newLocation, nData);
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpAptGRZoning ****/

PUdpAptGRZoning::PUdpAptGRZoning(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x03";
}

PUdpMsgAnalyser* PUdpAptGRZoning::Analyse()
{
  mDecodeData->mName << "=Genrep Apt Zoning";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpAptGRZoning::DoAction()
{
  //PMessage* cMsg = mDecodeData->mMessage;
  PClient* nClient = mDecodeData->mClient;

  //u16 nData = cMsg->U16Data(mDecodeData->Sub0x13Start+12); // always 0x0047 ? Not a location/entity anyway...

  u32 newLocation = PWorlds::mAptBaseWorldId + nClient->GetChar()->GetBaseApartment();
  u16 nData = 0;
  
  PMessage* tmpMsg = MsgBuilder->BuildGenrepZoningMsg(nClient, newLocation, nData);
  nClient->getUDPConn()->SendMessage(tmpMsg);

  if (! nClient->ChangeCharLocation(newLocation))
    Console->Print("Client[%d]: Bad Apartment location %d", nClient->GetID(), newLocation);

  //Client_Sockets[ClientNum].CharInfo.Flags = PFLAG_ZONING; //Player started zoning
  /*if (! nClient->ChangeCharLocation(newLocation))
    Console->Print("Client[%d]: Bad Apartment location %d (client value %d)", nClient->GetID(), PWorlds::mAptBaseWorldId + nClient->GetChar()->GetBaseApartment(), newLocation);*/

	tmpMsg = MsgBuilder->BuildZoning1Msg(nClient, nData);
  nClient->getUDPConn()->SendMessage(tmpMsg);

if (gDevDebug) Console->Print("Client[%d]: Genrep Zoning to Base Apartment (location %d - data %d)", nClient->GetID(), newLocation, nData);
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpVentureWarpConfirm ****/

PUdpVentureWarpConfirm::PUdpVentureWarpConfirm(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x09";
}

PUdpMsgAnalyser* PUdpVentureWarpConfirm::Analyse()
{
  mDecodeData->mName << "=Venture Warp confirmation";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  PMessage* cMsg = mDecodeData->mMessage;
  cMsg->SetNextByteOffset(mDecodeData->Sub0x13Start+12);
  (*cMsg) >> mUnknown1;
  (*cMsg) >> mUnknown2;
  (*cMsg) >> mUnknown3;
  (*cMsg) >> mRawItemId;
  (*cMsg) >> mStatus;
cMsg->Dump();
  return this;
}

bool PUdpVentureWarpConfirm::DoAction()
{
  PClient* nClient = mDecodeData->mClient;

  if(mStatus == 1)
  {
	u32 newLocation;
	do
	{
	  newLocation = 2000 + 20 * GetRandom(10, 0) + GetRandom(16, 1);
	} while (! Worlds->IsValidWorld(newLocation));
	
	u16 nEntity = 10;
	u16 nEntityType = 1;
	
	PMessage* tmpMsg = MsgBuilder->BuildAptLiftUseMsg (nClient, newLocation, nEntity, nEntityType);
	nClient->getUDPConn()->SendMessage(tmpMsg);
	
	if (gDevDebug) Console->Print("Client[%d]: Venture Warping to zone %d (entity %d/%d)", nClient->GetID(), newLocation, nEntity, nEntityType);
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpAddGenrepToList ****/

PUdpAddGenrepToList::PUdpAddGenrepToList(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x02";
}

PUdpMsgAnalyser* PUdpAddGenrepToList::Analyse()
{
  mDecodeData->mName << "=Add Genrep to list";

  PMessage* nMsg = mDecodeData->mMessage;
  // LocalID @offset 10
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 12);
  *nMsg >> mLocation;
  *nMsg >> mEntity;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpAddGenrepToList::DoAction()
{
  PClient* nClient = mDecodeData->mClient;

//Console->Print("Client[%d]: Adding Genrep (location %d - entity %d)", nClient->GetID(), mLocation, mEntity);

  if(mLocation == 1086 && mEntity == 1111)
  {
      PMessage* tmpMsg = MsgBuilder->BuildTextIniMsg (nClient, 6, 160);
      nClient->getUDPConn()->SendMessage(tmpMsg);
  }
  else
  {
      nClient->GetChar()->AddGenrep(mLocation, mEntity);
      PMessage* tmpMsg = MsgBuilder->BuildGenrepAddToListMsg(nClient, mLocation, mEntity);
      nClient->getUDPConn()->SendMessage(tmpMsg);
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpAppartmentAccess ****/

PUdpAppartmentAccess::PUdpAppartmentAccess(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x38";
}

PUdpMsgAnalyser* PUdpAppartmentAccess::Analyse()
{
  mDecodeData->mName << "=Try acces appartment";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 9);
  mAppartmentPlace = nMsg->U16Data(mDecodeData->Sub0x13Start + 9);
  mPassword = (char*)nMsg->GetMessageData() + mDecodeData->Sub0x13Start + 14;
  // NO SIZE ????? DO SIZE CHECK !

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpAppartmentAccess::DoAction()
{
  PMessage* tmpMsg;
  u32 AppLoc, Location, Entity;
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();

  Location = nChar->GetLocation();
if (gDevDebug) Console->Print("Client[%d]: Apt Access I/F (place %d - password %s)", nClient->GetID(), mAppartmentPlace, mPassword);
  if ((Location > PWorlds::mAptBaseWorldId) && (!strcmp ("Exit", mPassword)))
  {
    AppLoc = Appartements->GetAptLocation(Location);
    const PDefAppPlace* nAppPlace = (AppLoc ? GameDefs->AppPlaces()->GetDef(AppLoc) : 0);
    if(nAppPlace)
    {
      Location = nAppPlace->GetExitWorldID();
      Entity = nAppPlace->GetExitWorldEntity();
    }
    else
    {
      Location = 1; //PLAZA 1
      Entity = 100; //TYPHERRA MEMORIAL GR 1
    }
    tmpMsg = MsgBuilder->BuildChangeLocationMsg(nClient, Location, Entity);
  }
  else
  {
    u32 Location = Appartements->GetAptID(mAppartmentPlace, (u8*)mPassword);
    if (Location > 1)
    {
      tmpMsg = MsgBuilder->BuildAptLiftUseMsg(nClient, Location, 1);
      nClient->ChangeCharLocation(Location);
    }
    else if (Location < 1)
    {
      tmpMsg = MsgBuilder->BuildAptLiftFailedMsg(nClient);
    }
    else
      tmpMsg = NULL;

    mDecodeData->mTraceDump = true;
  }

  if (tmpMsg)
    nClient->getUDPConn()->SendMessage(tmpMsg);

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpEndOfZoning ****/

PUdpEndOfZoning::PUdpEndOfZoning(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x08";
}

PUdpMsgAnalyser* PUdpEndOfZoning::Analyse()
{
  mDecodeData->mName << "=End of zoning";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpEndOfZoning::DoAction()
{
// Nothing implemented yet
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
