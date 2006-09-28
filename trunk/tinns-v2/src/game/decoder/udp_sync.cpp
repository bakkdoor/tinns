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

	udp_sync.cpp - decoder classes for UDP Sync messages
  
	CREATION: 30 Aug 2006 Hammag

	MODIFIED:
	REASON: -
	
	TODO : put chunking code from PUdpSync2::DoAction() in a SendChunkedMessqage(PClient* nClient, PMessage* nMessage, u8 nChunkSize = 220)
*/

#include "main.h"
#include "udp_sync.h"

/**** PUdpSync0 ****/

PUdpSync0::PUdpSync0(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x01";
} 

PUdpMsgAnalyser* PUdpSync0::Analyse()
{
  if (mDecodeData->mMessage->GetSize() == 10)
  {
    mDecodeData->mName << "=Sync0";
    if (mDecodeData->mClientState->UDP.mState != PGameState::UDP::GUS_SYNC1)
    {
      mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    }
    else
    {
      mDecodeData->mState = DECODE_FINISHED;
    }
    return this;
  }
  else
  {
    return new PUdpMsgUnknown(mDecodeData);
  }
}

bool PUdpSync0::DoAction()
{
  if ((mDecodeData->mState & DECODE_ACTION_READY) && (mDecodeData->mClientState->UDP.mState != PGameState::UDP::GUS_SYNC1))
  {
    GetToSync1(mDecodeData);
    return true;
  }
  else
  {
    mDecodeData->mErrorDetail = "No action ready or client already in Sync1";
    return false;
  }
}

void PUdpSync0::GetToSync1(PMsgDecodeData* nDecodeData)
{
	PClient* curClient = nDecodeData->mClient;

	u32 loc = curClient->GetChar()->GetLocation(); 
//Console->Print("inside HandleGame : Location: %d", loc);
	SendZone(curClient, loc);

	// "aliverep" ?
	PMessage* tmpMsg = new PMessage(7);
	// u8 up[] = {0x04, 0x01, 0x00, 0xe3, 0x6b, 0xe6, 0xee};
	*tmpMsg << (u8)0x04;
	*tmpMsg << (u16) curClient->GetLocalID();
	*tmpMsg << (u8)0xe3; // ?
	*tmpMsg << (u8)0x6b; // ?
	//*tmpMsg << (u16) (curClient->getUDPConn()->getAddr()).sin_port; // Local or remote port here ???
	*tmpMsg << (u16) (curClient->getUDPConn()->getPort());
	
	curClient->getUDPConn()->SendMessage(tmpMsg);
	
	nDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
	//State->UDP.mSynced=true;
	nDecodeData->mClientState->UDP.mState = PGameState::UDP::GUS_SYNC1;// To change later
//Console->Print("Initialize: UDP_ID");
	curClient->SetUDP_ID(0);  
}


/**** PUdpSync1 ****/

PUdpSync1::PUdpSync1(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x03";
} 

PUdpMsgAnalyser* PUdpSync1::Analyse()
{
  mDecodeData->mName << "=Sync1";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  
  return this;
}

bool PUdpSync1::DoAction()
{
  if ((mDecodeData->mState & DECODE_ACTION_READY) && (mDecodeData->mClientState->UDP.mState == PGameState::UDP::GUS_SYNC1))
  {
    mDecodeData->mClientState->UDP.mState = PGameState::UDP::GUS_SYNC2;
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
  }
  else
    return false; 
}

/**** PUdpSync2 ****/

PUdpSync2::PUdpSync2(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x0c";
} 

PUdpMsgAnalyser* PUdpSync2::Analyse()
{
  mDecodeData->mName << "=Sync2";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  
  return this;
}

bool PUdpSync2::DoAction()
{
  if ((mDecodeData->mState & DECODE_ACTION_READY) && (mDecodeData->mClientState->UDP.mState == PGameState::UDP::GUS_SYNC2))
  {
    PClient* nClient = mDecodeData->mClient;
    
    // Baseline message chunking & sending
    PMessage* BaselineMsg = MsgBuilder->BuildBaselineMsg(nClient);
    PMessage* ChunkBuffer;
    PMessage* ChunkMsg;
    const u16 ChunkSize = 220;
    u16 ChunksNum = (BaselineMsg->GetSize() + ChunkSize - 1)/ChunkSize;
    
    for (u16 ChunkID = 0; ChunkID < ChunksNum; ChunkID++)
    {
      ChunkBuffer = BaselineMsg->GetChunk(0, ChunkSize, ChunkID);
      if (ChunkBuffer == NULL)
      {
        Console->Print(RED, BLACK, "Baseline: Bad chunk number calculation: %d for size %d", ChunksNum, BaselineMsg->GetSize());
        break;
      }
      
      ChunkMsg = new PMessage(ChunkSize + 15);
      //Client->IncreaseUDP_ID();
      nClient->SetUDP_ID(nClient->GetUDP_ID()+1);
      
      *ChunkMsg << (u8)0x13;
      *ChunkMsg << (u16)nClient->GetUDP_ID();
      *ChunkMsg << (u16)nClient->GetSessionID();
      *ChunkMsg << (u8) (9 + ChunkBuffer->GetSize());
      *ChunkMsg << (u8)0x03;
      *ChunkMsg << (u16)nClient->GetUDP_ID();
      *ChunkMsg << (u8)0x07; // ???
      *ChunkMsg << (u16)ChunkID;
      *ChunkMsg << (u16)ChunksNum;
      *ChunkMsg << (u8)0x19; // ? Type ?
      *ChunkMsg << *ChunkBuffer;
      
//Console->Print(GREEN, BLACK, "Baseline: ----------- chunk %d/%d", ChunkID+1, ChunksNum);
//ChunkMsg->Dump();
      
      delete ChunkBuffer;
      nClient->getUDPConn()->SendMessage(ChunkMsg);
    }
    delete BaselineMsg;
    
    // Sending "CharInfo3" message
    PMessage* CharInfo3Msg = MsgBuilder->BuildCharInfo3Msg(nClient);
    nClient->getUDPConn()->SendMessage(CharInfo3Msg);
    
    mDecodeData->mClientState->UDP.mState = PGameState::UDP::GUS_SYNC3;
      
    // Dispatching info to & from other chars in zone
    int nbSent;
    nbSent = ClientManager->SendUDPZoneWelcomeToClient(nClient);
//Console->Print(GREEN, BLACK, " %d Welcome message were sent to client %d", nbSent, Client->GetIndex());
  
    PMessage* HelloMsg = MsgBuilder->BuildCharHelloMsg(nClient);   
    nbSent = ClientManager->UDPBroadcast(HelloMsg, nClient);
//Console->Print(GREEN, BLACK, "Client %d: Hello message sent to %d chars", Client->GetIndex(), nbSent);
    
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
  }
  else
    return false; 
}
