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

 udp_outfitter.cpp - decoder classes for UDP outfitter related messages

 CREATION: 20 Mar 2009 Hammag

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_helditemaction.h"


/**** PUdpOutfitter ****/

PUdpHeldItemAction::PUdpHeldItemAction(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x01";
}

PUdpMsgAnalyser* PUdpHeldItemAction::Analyse()
{
    mDecodeData->mName << "=Held item Action";

	PMessage* nMsg = mDecodeData->mMessage;
	nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 8);

	(*nMsg) >> mUnknown1;
	(*nMsg) >> mTargetRawItemID;
	(*nMsg) >> mUnknown2;
	(*nMsg) >> mTargetedHeight; // range 0x00 (bottom) to 0x0f (top)
	(*nMsg) >> mScore; // range 0x00 to 0xff  Score ???

	//10:03:16:00:1f:01:00:01
	//84:03: (900) ??? 13:00 with knife
	//fe:03:00:00: targeted mRawItemID or none(?) for 0x3fe
	//00: ???
	//0e:71 targetted height on target. Range 0x000 - 0xfff
	
	//10:03:17:00:1f:01:00:01
	//84:03:fe:03:00:00:00:0e:4b
	
	//10:03:07:01:1f:01:00:01
	//22:01: (290)
	//fe:03:00:00:00:
	//0d:80

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpHeldItemAction::DoAction()
{
	//PClient* nClient = mDecodeData->mClient;
    //PChar* tChar = nClient->GetChar();
	
    //PMessage* tmpMsg = MsgBuilder->BuildCharHelloMsg(nClient);
    //ClientManager->UDPBroadcast(tmpMsg, nClient);
	
	Console->Print(YELLOW, BLACK, "Handled item action toward target %d (0x%08x) height %d/15 score %d/255", mTargetRawItemID, mTargetRawItemID, mTargetedHeight, mScore);
	//mDecodeData->mMessage->Dump();

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}