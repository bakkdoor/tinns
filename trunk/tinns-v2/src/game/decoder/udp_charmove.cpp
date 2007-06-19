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

	udp_charmove.cpp - decoder classes for UDP char movement messages

	CREATION: 5 Sep 2006 Hammag

	MODIFIED:
	REASON: -

*/

#include "main.h"
#include "udp_charmove.h"

#include "worlds.h"

/**** PUdpCharPosUpdate ****/

PUdpCharPosUpdate::PUdpCharPosUpdate(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x20";
}

PUdpMsgAnalyser* PUdpCharPosUpdate::Analyse()
{
  mDecodeData->mName << "=Char position update";

//mDecodeData->mTraceUnknownMsg = true; // temp stop being bugged with unknown move msg
//mDecodeData->mTraceKnownMsg = true;

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 4);
  
  *nMsg >> mInfoBitfield;
//bitfield [C?AL UXZY] packet read from LSB
//          8.21 8421
//Console->Print(YELLOW, BLACK, "[DEBUG] PUdpCharPosUpdate: Bitfield value %02x", mInfoBitfield);  
  if(mInfoBitfield & 0x80) // Sitting on chair
  {
    if(mInfoBitfield & 0x7f)
    {
      mDecodeData->mName << " + mixed bitfield (" << mInfoBitfield << ")";
      mDecodeData->mState = DECODE_UNKNOWN | DECODE_FINISHED;
Console->Print(YELLOW, BLACK, "[DEBUG] PUdpCharPosUpdate: Client %d sent Mixed field value %x", mDecodeData->mClient->GetID(), mInfoBitfield);
    }
    else
    {
      mDecodeData->mName << " (Char sitting)";

      *nMsg >> mChairItemID;
      *nMsg >> mChairItemType;      
      mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
//mDecodeData->mTraceKnownMsg = true;
//mDecodeData->mTraceDump = true;
//Console->Print(YELLOW, BLACK, "[DEBUG] Sitting on chair %d (%x) type %d", mChairItemID, mChairItemID, mChairItemType);
    }
  }
  else
  {  
    if(mInfoBitfield & 0x01) 
    {
      *nMsg >> mNewY;
    }
    if(mInfoBitfield & 0x02)
    {
      *nMsg >> mNewZ;
    }
    if(mInfoBitfield & 0x04)
    {
      *nMsg >> mNewX;
    }
    if(mInfoBitfield & 0x08)
    {
      *nMsg >> mNewUD;
    }
    if(mInfoBitfield & 0x10)
    {
      *nMsg >> mNewLR;
    }
    if(mInfoBitfield & 0x20)
    {
      *nMsg >> mNewAct;
    }
    if(mInfoBitfield & 0x40) // purpose unknown
    {
      *nMsg >> mNewUnknown;
      if(gDevDebug && mNewUnknown) {
        Console->Print(YELLOW, BLACK, "[DEBUG] PUdpCharPosUpdate: Client %d sent field 0x40 (in %x) value %x", mDecodeData->mClient->GetID(), mInfoBitfield, mNewUnknown);
      }
    }             
    
    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  }

  return this;
}

bool PUdpCharPosUpdate::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();

  if(mInfoBitfield & 0x80) // Sitting on chair
  {
    PMessage* tmpMsg;
    tmpMsg = MsgBuilder->BuildCharSittingMsg(nClient, mChairItemID);
    ClientManager->UDPBroadcast(tmpMsg, nClient, 5000); // TODO: Get the range from config
    tmpMsg = MsgBuilder->BuildCharPosUpdateMsg(nClient);
    ClientManager->UDPBroadcast(tmpMsg, nClient, 5000); // TODO: Get the range from config
  }
  else
  {
    bool IsRealMove = false;

    if((mInfoBitfield & 0x01) && (nChar->Coords.mY != mNewY))
    {
      nChar->Coords.mY = mNewY;
      IsRealMove = true;
    }
    if((mInfoBitfield & 0x02) && (nChar->Coords.mZ != mNewZ))
    {
      nChar->Coords.mZ = mNewZ;
      IsRealMove = true;
    }
    if((mInfoBitfield & 0x04) && (nChar->Coords.mX != mNewX))
    {
      nChar->Coords.mX = mNewX;
      IsRealMove = true;
    }
    if(mInfoBitfield & 0x08)
    {
      nChar->Coords.mUD = mNewUD;
      //IsRealMove = true;
    }
    if((mInfoBitfield & 0x10) && (nChar->Coords.mLR != mNewLR))
    {
      nChar->Coords.mLR = mNewLR;
      IsRealMove = true;
    }
    if(mInfoBitfield & 0x20)
    {
      nChar->Coords.mAct = mNewAct;
    }
    if(mInfoBitfield & 0x20)
    {
      nChar->Coords.mUnknown = mNewUnknown;
    }
    // movement action byte mask:
    // 0x00 NC has no focus (player alt+tab'ed out)
    // 0x02 kneeing
    // 0x20 Walk mode (= not run mode)
    // 0x08 left step
    // 0x10 right step
    // 0x40 forward
    // 0x80 backward
    // bits:  00000000
    //        BFWRL.K.

    PMessage* tmpMsg;
    if(mInfoBitfield == 0x7f)
    {  
      tmpMsg = MsgBuilder->BuildCharHealthUpdateMsg(nClient);
      ClientManager->UDPBroadcast(tmpMsg, nClient);
    }
    if(IsRealMove)
    {
      nChar->SetDirtyFlag();
    }
    /*if(IsRealMove)
    if(mInfoBitfield == 0x7f)
    {
      tmpMsg = MsgBuilder->BuildCharPosUpdateMsg(nClient);
      ClientManager->UDPBroadcast(tmpMsg, nClient, 5000); // TODO: Get the range from config
    }*/
    
    tmpMsg = MsgBuilder->BuildCharPosUpdate2Msg(nClient, mInfoBitfield);
    ClientManager->UDPBroadcast(tmpMsg, nClient, 5000, true);
    
    if(IsRealMove && nClient->GetDebugMode(DBG_LOCATION))
    {
      char DbgMessage[128];
      f32 f[3];
//      u32 h[3];
      f[0] = nChar->Coords.mY - 32000;
      f[1] = nChar->Coords.mZ - 32000;
      f[2] = nChar->Coords.mX - 32000;
//      memcpy(h, f, 3 * sizeof(u32));
//      snprintf(DbgMessage, 128, "position y:%0.1f (0x%08x) z:%0.1f (0x%08x) x:%0.1f (0x%08x)", f[0], h[0], f[1], h[1], f[2], h[2]);
      snprintf(DbgMessage, 128, "position y:%0.1f z:%0.1f x:%0.1f lr:%d (Act=%x BF=%x)", f[0], f[1], f[2], nChar->Coords.mLR, nChar->Coords.mAct ,mInfoBitfield);
      Chat->send(nClient, CHAT_GM, "Debug", DbgMessage);
    }
  }
//if(IsRealMove)
//Console->Print("Char %d position : X(%d) Y(%d) Z(%d) U/D(%d) L/R(%d) Action(%02x)", mDecodeData->mClient->GetID(), nChar->Coords.mX, nChar->Coords.mY, nChar->Coords.mZ, nChar->Coords.mUD, nChar->Coords.mLR, nChar->Coords.mAct);

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}


/**** PUdpCharExitChair ****/

PUdpCharExitChair::PUdpCharExitChair(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x22";
}

PUdpMsgAnalyser* PUdpCharExitChair::Analyse()
{
  mDecodeData->mName << "=Char exiting chair";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharExitChair::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();
  u32 ChairInUse = tChar->GetChairInUse();
  if(ChairInUse)
  {
    Worlds->GetWorld(tChar->GetLocation())->CharLeaveChair(nClient->GetLocalID(), ChairInUse);
    tChar->SetChairInUse(0);

    PMessage* tmpMsg = MsgBuilder->BuildCharExitChairMsg(nClient);
    ClientManager->UDPBroadcast(tmpMsg, nClient);

if (gDevDebug) Console->Print("Localchar %d get up from chair %d.", nClient->GetLocalID(), ChairInUse);
  }
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpCharJump ****/

PUdpCharJump::PUdpCharJump(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x02";
}

PUdpMsgAnalyser* PUdpCharJump::Analyse()
{
  mDecodeData->mName << "=Char jumping";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharJump::DoAction()
{
    /*PMessage* tmpMsg = MsgBuilder->BuildCharJumpingMsg(mDecodeData->mClient);
    ClientManager->UDPBroadcast(tmpMsg, mDecodeData->mClient);*/

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
if (gDevDebug) Console->Print("Char %d juming. Need server's broadcast answer format !!!", mDecodeData->mClient->GetID());
    return true;
}


/**** PUdpCharTargeting ****/

PUdpCharTargeting::PUdpCharTargeting(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x2d";
}

PUdpMsgAnalyser* PUdpCharTargeting::Analyse()
{
  mDecodeData->mName << "=Targeting char";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharTargeting::DoAction()
{
    PClient* nClient = mDecodeData->mClient;
    PChar* tChar = nClient->GetChar();
    u16 CharID = mDecodeData->mMessage->U16Data(mDecodeData->Sub0x13Start + 2);
    u8 strangeval1 = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 4);
    u8 strangeval2 = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 5);
    u8 strangeval3 = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 6);

    tChar->SetLookingAt(CharID);

    if (gDevDebug) Console->Print("Char %d targeting char %d. // %d %d %d", mDecodeData->mClient->GetID(), strangeval1, strangeval2, strangeval3);
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
