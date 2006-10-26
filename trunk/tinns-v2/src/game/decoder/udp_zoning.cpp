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

	MODIFIED:
	REASON: - 

*/

#include "main.h"
#include "udp_zoning.h"

#include "udp_sync.h"

/**** PUdpZoning1 ****/

PUdpZoning1::PUdpZoning1(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x0d";
} 

PUdpMsgAnalyser* PUdpZoning1::Analyse()
{
  mDecodeData->mName << "=Zoning phase 1";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  
  return this;
}

bool PUdpZoning1::DoAction()
{
  PMessage* cMsg = mDecodeData->mMessage;
  
  u32 newLocation = cMsg->U32Data(mDecodeData->Sub0x13Start+11);
  mDecodeData->mClient->ChangeCharLocation(newLocation);
  
//Console->Print("Zoning Stage 1: New location: %d", newLocation);
  
  u16 nData = cMsg->U32Data(mDecodeData->Sub0x13Start+15);    

  PMessage* tmpMsg = MsgBuilder->BuildZoning1Msg(mDecodeData->mClient, nData);
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
    PMessage* tmpMsg = MsgBuilder->BuildZoningTCPReadyMsg();
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
  
  u16 newLocation = cMsg->U16Data(mDecodeData->Sub0x13Start+12); // not 32 ???
  u16 nData = cMsg->U16Data(mDecodeData->Sub0x13Start+16);
  
  PMessage* tmpMsg = MsgBuilder->BuildGenrepZoningMsg(nClient, newLocation, nData);
  nClient->getUDPConn()->SendMessage(tmpMsg);
	
	//Client_Sockets[ClientNum].CharInfo.Flags = PFLAG_ZONING; //Player started zoning
  nClient->ChangeCharLocation(newLocation);

	tmpMsg = MsgBuilder->BuildZoning1Msg(nClient, nData);
  nClient->getUDPConn()->SendMessage(tmpMsg);

Console->Print("Client[%d]: Genrep Zoning to zone %d (data %d)", nClient->GetID(), newLocation, nData);	
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
  PMessage* cMsg = mDecodeData->mMessage;
  PClient* nClient = mDecodeData->mClient;
  
  u16 newLocation = cMsg->U16Data(mDecodeData->Sub0x13Start+12); // not u32 ???
  u16 nData = cMsg->U16Data(mDecodeData->Sub0x13Start+16);
  
  PMessage* tmpMsg = MsgBuilder->BuildGenrepZoningMsg(nClient, newLocation, nData);
  nClient->getUDPConn()->SendMessage(tmpMsg);
	
	//Client_Sockets[ClientNum].CharInfo.Flags = PFLAG_ZONING; //Player started zoning
  nClient->ChangeCharLocation(100000 + nClient->GetChar()->GetBaseApartment());

	tmpMsg = MsgBuilder->BuildZoning1Msg(nClient, nData);
  nClient->getUDPConn()->SendMessage(tmpMsg);

Console->Print("Client[%d]: Genrep Zoning to Base Apartment (location %d - data %d)", nClient->GetID(), newLocation, nData);	
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

  nClient->GetChar()->AddGenrep(mLocation, mEntity);
  PMessage* tmpMsg = MsgBuilder->BuildGenrepAddToListMsg(nClient, mLocation, mEntity);
  nClient->getUDPConn()->SendMessage(tmpMsg);

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
  mPassword = nMsg->GetMessageData() + mDecodeData->Sub0x13Start + 14;
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
Console->Print("Client[%d]: Apt Access I/F (place %d - password %s)", nClient->GetID(), mAppartmentPlace, mPassword);  
  if ((Location > 100000) && (!strcmp ("Exit", mPassword)))
  {
    /*Client_GetAptExit (Client_Sockets[ClientNum].CharInfo.Location, &Location, &Entity)*/;
    AppLoc = MySQL->GetAptLocation(Location);
    const PDefAppPlace* nAppPlace = (AppLoc ? GameDefs->GetAppPlaceDef(AppLoc) : 0);
    if(nAppPlace)
    {
      Location = nAppPlace->GetExitWorldID();
      Entity = nAppPlace->GetExitWorldEntity();     
    }
    else
    {
      Console->Print("%s Client[%d] Char[%s] invalid location %d before App exit", Console->ColorText(RED, BLACK, "[Warning]"), nClient->GetID(), nChar->GetName().c_str(),Location);
      Location = 1; //PLAZA 1
      Entity = 100; //TYPHERRA MEMORIAL GR 1
    }
    tmpMsg = MsgBuilder->BuildAptLiftExitMsg(nClient, Location, Entity);
  }
  else
  {
    u32 Location = MySQL->GetAptID(mAppartmentPlace, (u8*)mPassword);
    if (Location > 0)
    {
      tmpMsg = MsgBuilder->BuildAptLiftUseMsg(nClient, Location, 1);
      nClient->ChangeCharLocation(Location);
    }
    else
    {
      tmpMsg = MsgBuilder->BuildAptLiftFailedMsg(nClient);
    }
    mDecodeData->mTraceDump = true;
  }
  
  nClient->getUDPConn()->SendMessage(tmpMsg);

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true; 
}
