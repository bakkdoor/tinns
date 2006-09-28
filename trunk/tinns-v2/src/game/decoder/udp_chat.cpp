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

	udp_chat.h - decoder classes for UDP chat  messages
  
	CREATION: 15 Sep 2006 Hammag

	MODIFIED:
	REASON: - 

*/

#include "main.h"
#include "udp_chat.h"

/**** PUdpChatLocal ****/

PUdpChatLocal::PUdpChatLocal(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x1b";
} 

PUdpMsgAnalyser* PUdpChatLocal::Analyse()
{
  mDecodeData->mName << "=Local chat";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  
  return this;
}

bool PUdpChatLocal::DoAction()
{
  // temp
  Chat->HandleGameChat(mDecodeData->mClient, (u8*)mDecodeData->mMessage->GetMessageData() + mDecodeData->Sub0x13Start);
    /*PMessage* cMsg = mDecodeData->mMessage;
    u32 ClientTime = cMsg->U32Data(mDecodeData->Sub0x13Start+2);
    
    PMessage* tmpMsg = MsgBuilder->BuildPingMsg(mDecodeData->mClient, ClientTime);
    mDecodeData->mClient->getUDPConn()->SendMessage(tmpMsg);*/
    
    //cMsg->SetNextByteOffset(mDecodeData->Sub0x13StartNext);
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

/**** PUdpChatGlobal ****/

PUdpChatGlobal::PUdpChatGlobal(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x3b";
} 

PUdpMsgAnalyser* PUdpChatGlobal::Analyse()
{
  //u16 dumb;
  mDecodeData->mName << "=Global chat";

/*  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 12);
  *nMsg >> mVehicleID; // ? not u32 ???
  *nMsg >> dumb;
  *nMsg >> mVehicleSeat;*/

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpChatGlobal::DoAction()
{
  // Temp
  Chat->HandleGameChat(mDecodeData->mClient, (u8*)mDecodeData->mMessage->GetMessageData() + mDecodeData->Sub0x13Start);
/*  PMessage* tmpMsg = MsgBuilder->BuildCharEnteringVhcMsg (mDecodeData->mClient, mVehicleID, mVehicleSeat);
  ClientManager->UDPBroadcast(tmpMsg, mDecodeData->mClient);
*/
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpChatListAdd ****/

PUdpChatListAdd::PUdpChatListAdd(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x33";
} 

PUdpMsgAnalyser* PUdpChatListAdd::Analyse()
{
  mDecodeData->mName << "=Add char to chat list";

  PMessage* nMsg = mDecodeData->mMessage;
  u8 PSize = nMsg->U8Data(mDecodeData->Sub0x13Start);
  mChatList = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 8);
  
  if ((mChatList == 1) || (mChatList == 2))
  {
    if ((PSize > 8) && (nMsg->U8Data(mDecodeData->Sub0x13StartNext -1) == 0))
    {
      mAddedCharname = (char*)mDecodeData->mMessage->GetMessageData() + mDecodeData->Sub0x13Start + 9;
      mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    }
    else
    {
      mDecodeData->mState = DECODE_ERROR;
      mDecodeData->mErrorDetail = "Invalid charname position";
    }
  
    return this;
  }
  else
  {
    mDecodeData->mUnknownType = mChatList;
    return new PUdpMsgUnknown(mDecodeData);
  }
}

bool PUdpChatListAdd::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  std::string AddedChar = mAddedCharname;
  PChar* tChar = Database->GetChar(AddedChar);
  u32 AddedCharID = (tChar ? tChar->GetID() : 0);
  bool AddResult = false;
  
  if (AddedCharID)
  {
    switch(mChatList)
    {
      case 1:
      {
        AddResult = nClient->GetChar()->SetDirectChat(AddedCharID);
        break; 
      }
      case 2:
      {
        AddResult = nClient->GetChar()->AddBuddy(AddedCharID);
        break; 
      }
    }
  }
  
  if (AddResult)
  {
    PMessage* tmpMsg = MsgBuilder->BuildChatAddMsg (nClient, AddedCharID, mChatList);
    nClient->getUDPConn()->SendMessage(tmpMsg);
  }
  
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpChatListRemove ****/

PUdpChatListRemove::PUdpChatListRemove(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x39";
} 

PUdpMsgAnalyser* PUdpChatListRemove::Analyse()
{
  mDecodeData->mName << "=remove char from chat list";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 8);
  (*nMsg) >> mChatList;
  
  if ((mChatList == 1) || (mChatList == 2))
  {
    (*nMsg) >> mRemovedCharID;
    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
  }
  else
  {
    mDecodeData->mUnknownType = mChatList;
    return new PUdpMsgUnknown(mDecodeData);
  }
}

bool PUdpChatListRemove::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  
  bool RemoveResult = false;
  
  if (mRemovedCharID)
  {
    switch(mChatList)
    {
      case 1:
      {
        RemoveResult = nClient->GetChar()->SetDirectChat(0);
        break; 
      }
      case 2:
      {
        RemoveResult = nClient->GetChar()->RemoveBuddy(mRemovedCharID);
        break; 
      }
    }
  }
  
  // No known response yet ...
  /*if (AddResult)
  {
    PMessage* tmpMsg = MsgBuilder->BuildChatAddMsg (nClient, mRemovedCharID, mChatList);
    nClient->getUDPConn()->SendMessage(tmpMsg);
  }*/
  
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
