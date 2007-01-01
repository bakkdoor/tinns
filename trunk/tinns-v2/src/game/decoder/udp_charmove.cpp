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
  nDecodeData->mName << "/0x7f";
}

PUdpMsgAnalyser* PUdpCharPosUpdate::Analyse()
{
  mDecodeData->mName << "=Char position update";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 5);
  *nMsg >> mNewY;
  *nMsg >> mNewZ;
  *nMsg >> mNewX;
  *nMsg >> mNewUD;
  *nMsg >> mNewLR;
  *nMsg >> mNewAct;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharPosUpdate::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();
  bool IsRealMove = false;

  if ((nChar->Coords.mY != mNewY) || (nChar->Coords.mZ != mNewZ) || (nChar->Coords.mX != mNewX) || (nChar->Coords.mLR != mNewLR))
  {
    nChar->Coords.mY = mNewY;
    nChar->Coords.mZ = mNewZ;
    nChar->Coords.mX = mNewX;
    nChar->Coords.mLR = mNewLR;
    nChar->SetDirtyFlag();
    IsRealMove = true;
  }
  nChar->Coords.mUD = mNewUD;
  nChar->Coords.mAct = mNewAct;
        // movement byte mask:
        // 0x00 NC has no focus (player alt+tab'ed out)
        // 0x02 kneeing
        // 0x20 Walk mode (= not run mode)
        // 0x08 left step
        // 0x10 right step
        // 0x40 forward
        // 0x80 backward
        // bits:  00000000
        //        BFWRL.K.
//Console->Print("Char %d position update: X(%d) Y(%d) Z(%d) U/D(%d) L/R(%d) Action(%#.2x)", mDecodeData->mClient->GetID(), nChar->Coords.mX, nChar->Coords.mY, nChar->Coords.mZ, nChar->Coords.mUD, nChar->Coords.mLR, nChar->Coords.mAct);

  PMessage* tmpMsg = MsgBuilder->BuildCharHealthUpdateMsg(nClient);
  ClientManager->UDPBroadcast(tmpMsg, nClient);

  tmpMsg = MsgBuilder->BuildCharPosUpdateMsg(nClient);
  ClientManager->UDPBroadcast(tmpMsg, nClient, 5000); // TODO: Get the range from config

  if(IsRealMove && nClient->GetDebugMode(DBG_LOCATION))
  {
    char DbgMessage[128];
    f32 f[3];
    u32 h[3];
    f[0] = nChar->Coords.mY - 32000;
    f[1] = nChar->Coords.mZ - 32000;
    f[2] = nChar->Coords.mX - 32000;
    memcpy(h, f, 3 * sizeof(u32));
    snprintf(DbgMessage, 128, "position y:%0.1f (0x%08x) z:%0.1f (0x%08x) x:%0.1f (0x%08x)", f[0], h[0], f[1], h[1], f[2], h[2]);
    Chat->send(nClient, CHAT_GM, "Debug", DbgMessage);
  }

//if(IsRealMove)
//Console->Print("Char %d position : X(%d) Y(%d) Z(%d) U/D(%d) L/R(%d) Action(%02x)", mDecodeData->mClient->GetID(), nChar->Coords.mX, nChar->Coords.mY, nChar->Coords.mZ, nChar->Coords.mUD, nChar->Coords.mLR, nChar->Coords.mAct);

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpCharAttitudeUpdate ****/

PUdpCharAttitudeUpdate::PUdpCharAttitudeUpdate(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x20";
}

PUdpMsgAnalyser* PUdpCharAttitudeUpdate::Analyse()
{
  mDecodeData->mName << "=Char attitude update";

  mNewAct = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start+5);
//Console->Print"Char %d Action update: 0x%02x", mDecodeData->mClient->GetID(), mNewAct);

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharAttitudeUpdate::DoAction()
{
    mDecodeData->mClient->GetChar()->Coords.mAct = mNewAct;

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}


/**** PUdpCharSitting ****/

PUdpCharSitting::PUdpCharSitting(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x80";
}

PUdpMsgAnalyser* PUdpCharSitting::Analyse()
{
  mDecodeData->mName << "=Char sitting";

  PMessage* nMsg = mDecodeData->mMessage;
  //u16 LocalID = mDecodeData->mMessage->U16Data(mDecodeData->Sub0x13Start+2);
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 5);
  *nMsg >> mChairItemID;
  *nMsg >> mChairItemType;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharSitting::DoAction() // this message is repeated x times/sec ... is answer usefull ? each time ?
{
//Console->Print("Sitting on chair %d (%x)", mChairItemID, mChairItemID);
//PChar* nChar = mDecodeData->mClient->GetChar();
//Console->Print("Char %d position : X(0x%x) Y(0x%x) Z(0x%x) U/D(0x%x) L/R(0x%x) Action(0x%#.2x)", mDecodeData->mClient->GetID(), nChar->Coords.mX, nChar->Coords.mY, nChar->Coords.mZ, nChar->Coords.mUD, nChar->Coords.mLR, nChar->Coords.mAct);
//Console->Print("Char %d position : X(%d) Y(%d) Z(%d) U/D(%d) L/R(%d) Action(%02x)", mDecodeData->mClient->GetID(), nChar->Coords.mX, nChar->Coords.mY, nChar->Coords.mZ, nChar->Coords.mUD, nChar->Coords.mLR, nChar->Coords.mAct);
//mDecodeData->mTraceDump = true;
    PMessage* tmpMsg = MsgBuilder->BuildCharSittingMsg(mDecodeData->mClient, mChairItemID);
    ClientManager->UDPBroadcast(tmpMsg, mDecodeData->mClient);
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
